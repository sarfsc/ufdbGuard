/* 
 * ufdbguardd.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2018 by URLfilterDB B.V. with all rights reserved.
 * 
 * Parts of the ufdbGuard daemon are based on squidGuard.
 *
 * Multithreaded ufdbGuard daemon
 *
 * $Id: ufdbguardd.c,v 1.175 2018/07/25 13:47:56 root Exp root $
 */

#undef UFDB_FREE_MEMORY

#include "ufdb.h"
#include "sg.h"
#include "ufdbdb.h"
#include "ufdbchkport.h"
#include "httpsQueue.h"
#include "httpserver.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>
#include <libgen.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#ifdef HAVE_SYS_SYSCALL_H
#include <sys/syscall.h>
#endif
#ifdef __linux__
#include <linux/unistd.h>
#endif
#include <sys/socket.h>
#if HAVE_UNIX_SOCKETS
#include <sys/un.h>
#endif
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#ifdef __linux__
#include <execinfo.h>
#endif

#ifdef _POSIX_PRIORITY_SCHEDULING
#include <sched.h>
#endif

#include <openssl/ssl.h>

#define FULL 1
#define EMPTY 0

#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
extern pthread_rwlock_t  TheDynamicSourcesLock;
extern pthread_rwlock_t  TheDynamicCategoriesLock;
#else
extern pthread_mutex_t   TheDynamicSourcesLock;
extern pthread_mutex_t   TheDynamicCategoriesLock;
#endif

#define _132THREADS 132

/* enforce one single cache line per mutex/rwlock */
UFDB_GCC_ALIGN64 
static pthread_mutex_t sighup_mutex = UFDB_STATIC_MUTEX_INIT;

#if defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4  &&  __SIZEOF_INT__ == 4
   /* no mutex required */
#elif defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8  &&  __SIZEOF_INT__ == 8
   /* no mutex required */
#else
UFDB_GCC_ALIGN64 
static pthread_mutex_t incrMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
UFDB_GCC_ALIGN64 
static pthread_rwlock_t TheDatabaseLock;
#endif

UFDB_GCC_ALIGN64 
static volatile int n_active_workers = 0;
static          int prev_n_workers = 0;
#define UFDB_MAX_WORKERS	UFDB_MAX_THREADS

static int     my_argc;
static char ** my_argv;
static char ** my_envp;
static char *  configFile = NULL;
static struct timeval  start_time;

static int       badSignal = 0;
static pthread_t badSignalThread;

extern int globalPid;

extern int globalDebugTimeDelta;
extern int sig_other;
extern char ** globalArgv;
extern char ** globalEnvp;

extern int httpsConnectionCacheSize;

static int        portNumCmdLine = -1;

static pthread_t  threadedSignalHandler = (pthread_t) 837;
static pthread_t  housekeeper = (pthread_t) 837;
static pthread_t  sockethandler = (pthread_t) NULL;
static pthread_t  dyn_userlist_handler = (pthread_t) 837;
static pthread_t  dyn_domainlist_handler = (pthread_t) 837;
static pthread_t  workers[UFDB_MAX_WORKERS+1] = { (pthread_t) 837, (pthread_t) 837 };
static pthread_t  httpsthr[UFDB_NUM_HTTPS_VERIFIERS] = { (pthread_t) 837, (pthread_t) 837 };
static char *     workerURLbuffers[UFDB_MAX_WORKERS+1];



static time_t          lastUpload = 0;
static int             testMode = 0;
static int             testConfig = 0;
static int             allowedToRemovePidFile = 0;
UFDB_GCC_ALIGN64
volatile unsigned long UFDBlookupCounter = 0;			/* hot variable */
volatile unsigned long UFDBuploadSeqNo = 0;
volatile unsigned long UFDBedufilterCounter = 0;
volatile unsigned long UFDBtestBlockCounter = 0;
static int             parseOnly = 0;
static int             runAsDaemon = 1;
static pid_t           httpDaemonPid = 0;
UFDB_GCC_ALIGN64
volatile unsigned long UFDBsafesearchCounter = 0;		/* warm variable */
volatile unsigned long UFDBuncategorisedCounter = 0;		/* warm variable */
volatile unsigned long UFDBblockCounter = 0;			/* warm variable */
volatile int           badSignalHandlerBusy = 0;

void BadSignalCaught( int signum );
static void uploadStatistics( const char * reason );
static void * worker_main( void * ptr );
static void * housekeeper_main( void * ptr );
static void * dynamic_userlist_updater_main( void * ptr );
static void * dynamic_domainlist_updater_main( void * ptr );

#define SECSOF8DAYS  (60*60*24*8)
#define SECSOF2DAYS  (60*60*24*2)

#define UFDB_DEBUG_YY 0

#if UFDB_DEBUG_YY
extern int yydebug;
#endif


static void usage( char option )
{
   if (option != '\0')
      fprintf( stderr, "unknown option '-%c'\n", option );

   fprintf( stderr, "usage: ufdbguardd [-A] [-d] [-v] [-r] [-R] [-T] [-h] [-w 32-129] [-p port] [-c <configfile>]\n" );
   fprintf( stderr, "-v      print version\n" );
   fprintf( stderr, "-c FN   use configuration file FN\n" );
   fprintf( stderr, "-T      test mode; do not block, only log which URLs might be blocked\n" );
   fprintf( stderr, "-w N    use N worker threads (default: %d)\n", UFDBglobalNworkers );
   fprintf( stderr, "-p N    use TCP port N to listen on\n" );
   fprintf( stderr, "-U user run with privileges of \"user\"\n" );
   fprintf( stderr, "-d      extra debug information in log file\n" );
   fprintf( stderr, "-R      debug RE matching\n" );
   fprintf( stderr, "-r      log all redirections\n" );
   fprintf( stderr, "-N      do not analyse uncategorised URLs (not recommended)\n" );
   fprintf( stderr, "-h      print this help text\n" );
   fprintf( stderr, "-C verify  verify a configuration file; must be the last option\n" );

   if (option != '\0')
      exit( 1 );
   else
      exit( 0 );
}


static int writePidFile( void )
{
   FILE * fp;
   char * fname;
   mode_t oldumask;
   struct stat dir;

   UFDBraisePrivileges( UFDBglobalUserName, "write pid file" );

   if (UFDBglobalPidFile == NULL)		/* pidfile is not set in the config file, so we use the default pid file */
   {
      fname = (char *) UFDBGUARDD_PID_FILE;
      if (stat( DEFAULT_PIDDIR, &dir ) != 0)
      {
	 ufdbLogMessage( "The directory '%s' where the pid file is stored, does not exist.  creating it.", DEFAULT_PIDDIR );
	 if (mkdir( DEFAULT_PIDDIR, S_IRUSR|S_IWUSR|S_IXUSR| S_IRGRP|S_IWGRP|S_IXGRP ) < 0)
	 {
	    ufdbLogError( "Cannot create directory %s: %s", DEFAULT_PIDDIR, strerror(errno) );
	    UFDBdropPrivileges( UFDBglobalUserName );
	    return 0;
	 }

	 if (UFDBglobalUserName[0] != '\0')
	 {
	    struct passwd * user;
	    user = getpwnam( UFDBglobalUserName );
	    if (user == NULL)
	    {
	       ufdbLogError( "user \"%s\" does not exist.  cannot change ownership of %s  *****",
			     UFDBglobalUserName, DEFAULT_PIDDIR );
	       UFDBdropPrivileges( UFDBglobalUserName );
	       return 0;
	    }
	    else
	    {
	       if (chown( DEFAULT_PIDDIR, user->pw_uid, user->pw_gid ) != 0)
	       {
		  ufdbLogError( "cannot change ownership of %s: %s", DEFAULT_PIDDIR, strerror(errno) );
		  UFDBdropPrivileges( UFDBglobalUserName );
		  return 0;
	       }
	       if (chmod( DEFAULT_PIDDIR, S_IRUSR|S_IWUSR|S_IXUSR |S_IRGRP|S_IWGRP|S_IXGRP |S_IROTH|S_IXOTH ) != 0)
	       {
		  ufdbLogError( "cannot change directory permissions (chmod) of %s: %s", 
                                DEFAULT_PIDDIR, strerror(errno) );
		  UFDBdropPrivileges( UFDBglobalUserName );
		  return 0;
	       }
	    }
	 }
      }
   }
   else
      fname = UFDBglobalPidFile;

   (void) unlink( fname );

   UFDBdropPrivileges( UFDBglobalUserName );

   /* make the pid file world readable */
   oldumask = umask( S_IXUSR | S_IWGRP|S_IXGRP | S_IWOTH|S_IXOTH );
   fp = fopen( fname, "w" );
   umask( oldumask );
   if (fp == NULL)
   {
      ufdbLogFatalError( "cannot write to PID file %s: %s\n"
                         "Check file and directory permission and ownership.", 
                         fname, strerror(errno) );
      return 0;
   }
   else
   {
      fprintf( fp, "%d\n", globalPid );
      fclose( fp );
      ufdbLogMessage( "PID %d written to %s", globalPid, fname );
      allowedToRemovePidFile = 1;
   }

   return 1;
}


static void removePidFile( void )
{
   FILE * fp;
#if HAVE_UNIX_SOCKETS
   char   unixsocket[64];
#endif

   if (UFDBglobalDebug)
      ufdbLogMessage( "removing pid file  allowed=%d", allowedToRemovePidFile );

   /* When the initialisation of the socket failed there is most likely
    * already an ufdbguardd daemon running and we may not remove
    * the pid file nor kill any ufdbhttpd daemon.
    */
   if (!allowedToRemovePidFile)
      return;

#if HAVE_UNIX_SOCKETS
   sprintf( unixsocket, "/tmp/ufdbguardd-%05d", UFDBglobalPortNum );
   if (unlink( unixsocket ) < 0  &&  errno != ENOENT)
      ufdbLogError( "cannot remove socket %s: %s", unixsocket, strerror(errno) );
#endif

   if (UFDBglobalPidFile == NULL)
      (void) unlink( (char *) UFDBGUARDD_PID_FILE );
   else
      (void) unlink( UFDBglobalPidFile );

   allowedToRemovePidFile = 0;

   if (UFDBglobalHttpdPort > 0)
   {
      UFDBraisePrivileges( UFDBglobalUserName, "kill ufdbhttpd" );
      fp = fopen( UFDBHTTPD_PID_FILE, "r" );
      if (fp != NULL)
      {
         if (fscanf( fp, "%d", &httpDaemonPid ) == 1)
	 {
	    ufdbLogMessage( "sending TERM signal to the HTTP daemon (pid=%d)", httpDaemonPid );
	    if (kill( httpDaemonPid, SIGTERM ) < 0)
	       ufdbLogError( "cannot send TERM signal to ufdbhttpd with pid %d: %s", 
                             httpDaemonPid, strerror(errno) );
	    /* TODO: find out why ufdbhttpd does not act on the TERM signal... */
	    usleep( 210000 );
	    (void) kill( httpDaemonPid, SIGKILL );  /* TODO: we might be killing an other process after a usleep() ... */
	 }
         fclose( fp );
      }
      UFDBdropPrivileges( UFDBglobalUserName );
   }
}


static void logStatistics( const char * reason )
{
   struct Acl *         acl;
   struct Category *    cat;
   struct AclCategory * aclcat;
   struct Source *      src;
   char *               logmsg;
   char *               lbuf;
   char *               implicitMsg;

   ufdbLogMessage( "statistics: %s", reason );
   ufdbLogMessage( "statistics: %lu URL lookups (%lu https). %lu URLs blocked. "
		   "%lu tunnels detected. %lu safe searches. %lu Youtube edufilter. "
                   "%lu uncategorised URLs. %lu clients. %lu users.",
		   UFDBlookupCounter, 
		   UFDB_API_num_https,
		   UFDBblockCounter + UFDBtestBlockCounter,
		   UFDBglobalTunnelCounter,
		   UFDBsafesearchCounter,
		   UFDBedufilterCounter,
		   UFDBuncategorisedCounter,
		   UFDBgetNumberOfRegisteredIPs(),
		   UFDBgetNumberOfRegisteredUsers()  );

   if (Cat == NULL  ||  UFDBgetSourceList() == NULL  ||  AclList == NULL)
      return;

   logmsg = (char *) ufdbMalloc( 256 * 1024 );
   implicitMsg = (char *) ufdbMalloc( 256 * 1024 );

   for (cat = Cat;  cat != NULL;  cat = cat->next)
   {
      ufdbLogMessage( "statistics: category %s was matched %lu times and blocked %lu times",
                      cat->name, cat->nmatches, cat->nblocks );
   }

   for (src = UFDBgetSourceList();  src != NULL;  src = src->next)
   {
      ufdbLogMessage( "statistics: source %s was matched %lu times and blocked %lu times",
                      src->name, src->nmatches, src->nblocks );
   }

   for (acl = AclList;  acl != NULL;  acl = acl->next)
   {
      if (acl->pass == NULL)
	 continue;

      lbuf = logmsg;
      if (acl->within == UFDB_ACL_ELSE)
         lbuf += sprintf( lbuf, "acl %s-else: ", acl->name );
      else if (acl->within == UFDB_ACL_WITHIN)
         lbuf += sprintf( lbuf, "acl %s-within-%s: ", acl->name, acl->time->name );
      else if (acl->within == UFDB_ACL_OUTSIDE)
         lbuf += sprintf( lbuf, "acl %s-outside-%s: ", acl->name, acl->time->name );
      else
         lbuf += sprintf( lbuf, "acl %s: ", acl->name );

      for (aclcat = acl->pass;  aclcat != NULL;  aclcat = aclcat->next)
      {
         cat = aclcat->cat;
	 if (cat != NULL  &&  !cat->active)
	    continue;

	 switch (aclcat->type)
	 {
	    case ACL_TYPE_TERMINATOR:
	       if (aclcat->access)
		  lbuf += sprintf( lbuf, " any" );
	       else
		  lbuf += sprintf( lbuf, " none" );
	       break;
	    case ACL_TYPE_DEFAULT:
               *lbuf++ = ' ';
	       if (!aclcat->access)
                  *lbuf++ = '!';
	       lbuf += sprintf( lbuf, "%s", aclcat->name );
	       break;
	    case ACL_TYPE_INADDR:
	       lbuf += sprintf( lbuf, " in-addr" );
	       break;
	 }
	 lbuf += sprintf( lbuf, ":%lu/%lu", aclcat->nblocks, aclcat->nmatches );
      }

      implicitMsg[0] = '\0';
      for (aclcat = acl->implicitPass;  aclcat != NULL;  aclcat = aclcat->next)
      {
         cat = aclcat->cat;
	 if (cat != NULL  &&  !cat->active)
	    continue;

	 lbuf += sprintf( lbuf, " %s:%lu/%lu", aclcat->name, aclcat->nblocks, aclcat->nmatches );
      }
      if (implicitMsg[0] != '\0')
      {
         lbuf += sprintf( logmsg, "  -- implicitly allowed: %s", implicitMsg );
      }

      ufdbLogMessage( "statistics: %s", logmsg );
   }

   ufdbFree( logmsg );
   ufdbFree( implicitMsg );
}


static void startHttpDaemon( void )
{
   int    i;
   const char * argv[24];
   struct stat statbuf;
   char   portStr[16];
   char   httpdbin[1024];

   if (UFDBglobalDebug || UFDBglobalDebugHttpd)
      ufdbLogMessage( "going to start ufdbhttpd" );

   sprintf( httpdbin, "%s/%s", DEFAULT_BINDIR, "ufdbhttpd" );
   if (stat( httpdbin, &statbuf ) < 0)
   {
      ufdbLogError( "cannot find the HTTP daemon executable (%s): %s  *****", httpdbin, strerror(errno) );
      return;
   }

   /* go back to user root to allow ufdbhttpd to use privileged ports */
   UFDBraisePrivileges( UFDBglobalUserName, "start ufdbhttpd" );

   httpDaemonPid = fork();
   if (httpDaemonPid == 0)	/* child */
   {
      globalPid = getpid();
      i = 0;
      argv[i++] = httpdbin;
      if (UFDBglobalDebug || UFDBglobalDebugHttpd)
         argv[i++] = "-d";
      argv[i++] = "-p";
      sprintf( portStr, "%d", UFDBglobalHttpdPort );
      argv[i++] = portStr;
      argv[i++] = "-l";
      if (UFDBglobalLogDir == NULL)
	 argv[i++] = DEFAULT_LOGDIR;
      else
	 argv[i++] = UFDBglobalLogDir;
      argv[i++] = "-I";
      if (UFDBglobalHttpdImagesDirectory[0] == '\0')
	 strcpy( UFDBglobalHttpdImagesDirectory, DEFAULT_IMAGESDIR );
      argv[i++] = UFDBglobalHttpdImagesDirectory;
      argv[i++] = "-i";
      argv[i++] = UFDBglobalHttpdInterface;
      if (UFDBglobalUserName[0] != '\0')
      {
	 argv[i++] = "-U";
	 argv[i++] = UFDBglobalUserName;
      }
      argv[i] = NULL;
      ufdbLogMessage( "starting HTTP daemon ..." );
      /* Note: the child has the logfile still open */
      execv( httpdbin, (char* const*) argv );
      ufdbLogError( "failed starting http daemon: execv failed: %s", strerror(errno) );
      _exit( 2 );
   }

   UFDBdropPrivileges( UFDBglobalUserName );

   if (httpDaemonPid < 0)
   {
      ufdbLogError( "cannot start HTTP daemon: fork failed: %s", strerror(errno) );
      httpDaemonPid = 0;
   }
   else
   {
#if 0
      int        status;
      pid_t      pid;
#endif

      ufdbLogMessage( "ufdbhttpd is started.  pid %ld  port=%d interface=%s images=%s",
                      (long) httpDaemonPid, UFDBglobalHttpdPort, UFDBglobalHttpdInterface, UFDBglobalHttpdImagesDirectory );
#if 0
      usleep( 300000 );
      while ((pid = waitpid( -1, &status, WNOHANG )) > 0)
	 ;
#endif
   }
}


static void adjustNumberOfWorkerThreads( void )
{
   int              i;
   pthread_attr_t   attr;

   /* IF the check_https_tunnel_option is aggressive we need more worker threads */
   /* Note that UFDB_MAX_THREADS currently is 1285 */
   if (UFDBgetTunnelCheckMethod() == UFDB_API_HTTPS_CHECK_AGGRESSIVE  &&  
       UFDBglobalNworkers < _132THREADS  &&  UFDBglobalNworkers < UFDB_MAX_THREADS)
   {
      if (UFDBglobalDebug)
         ufdbLogMessage( "adjusting #worker threads: starting %d threads", _132THREADS - UFDBglobalNworkers );

      pthread_attr_init( &attr );
      pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
#if HAVE_PTHREAD_ATTR_SETGUARDSIZE && UFDB_DEBUG
      pthread_attr_setstacksize( &attr, (400-8) * 1024 );
      pthread_attr_setguardsize( &attr, 8 * 1024 );
#else
      pthread_attr_setstacksize( &attr, 400 * 1024 );
#endif
      for (i = UFDBglobalNworkers; i < _132THREADS; i++)
      {
	 pthread_create( &workers[i], &attr, worker_main, (void *) ((long) i) );
	 if (i % 4 == 0)
	 {
#ifdef _POSIX_PRIORITY_SCHEDULING
	    sched_yield();
#else
	    usleep( 10000 );
#endif
         }
      }
      UFDBglobalNworkers = _132THREADS;
      prev_n_workers = UFDBglobalNworkers;
      ufdbLogMessage( "#threads is increased to %d because check-proxy-tunnels is set to \"aggressive\"", UFDBglobalNworkers );
   }

   if (UFDBglobalNworkers > prev_n_workers)
   {
      if (UFDBglobalDebug)
         ufdbLogMessage( "adjusting #worker threads: starting %d threads", UFDBglobalNworkers - prev_n_workers );

      pthread_attr_init( &attr );
      pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
#if HAVE_PTHREAD_ATTR_SETGUARDSIZE && UFDB_DEBUG
      pthread_attr_setstacksize( &attr, (400-8) * 1024 );
      pthread_attr_setguardsize( &attr, 8 * 1024 );
#else
      pthread_attr_setstacksize( &attr, 400 * 1024 );
#endif
      for (i = prev_n_workers; i < UFDBglobalNworkers; i++)
      {
	 pthread_create( &workers[i], &attr, worker_main, (void *) ((long) i) );
	 if (i % 4 == 0)
	 {
#ifdef _POSIX_PRIORITY_SCHEDULING
	    sched_yield();
#else
	    usleep( 10000 );
#endif
         }
      }
      prev_n_workers = UFDBglobalNworkers;
      ufdbLogMessage( "#threads is increased because num-worker-thread is %d", UFDBglobalNworkers );
   }
}


static void Usr1SignalCaught( int sig )
{
   FILE * fp;
   int    oldStatus;

   if (UFDBglobalTerminating)
      return;

   ufdbLogMessage( "USR1 signal received for logfile rotation" );
   oldStatus = UFDBchangeStatus( UFDB_API_STATUS_ROLLING_LOGFILE );
   UFDBrotateLogfile();
   if (UFDBglobalHttpdPort > 0)
   {
      fp = fopen( UFDBHTTPD_PID_FILE, "r" );
      if (fp != NULL)
      {
	 if (1 != fscanf( fp, "%d", &httpDaemonPid ))
	    httpDaemonPid = -1;
	 fclose( fp );
      }
      else
	 httpDaemonPid = -1;

      if (httpDaemonPid > 0)
      {
	 int retval;

	 ufdbLogMessage( "sending USR1 signal to ufdbhttpd (pid=%d)", httpDaemonPid );
	 /* TODO: use setreuid() */
	 retval = kill( httpDaemonPid, SIGUSR1 );
	 if (retval != 0)
	    ufdbLogError( "cannot send USR1 signal to httpd daemon: %s", strerror( errno ) );
      }
   }
   ufdbLogMessage( "USR1 signal received for logfile rotation" );
   UFDBchangeStatus( oldStatus );
}


static void Usr2SignalCaught( int sig )
{
   int    oldStatus;

   if (UFDBglobalTerminating)
      return;

   ufdbLogMessage( "USR2 signal received to trigger monitoring commands" );
   oldStatus = UFDBchangeStatus( UFDB_API_STATUS_UPDATE );
   UFDBchangeStatus( oldStatus );
   ufdbLogMessage( "status update done" );
}


/*
 * OpenBSD is very picky with its implementation of signals and
 * does not allow violation of the POSIX standard that says that
 * a signal handler may NOT raise a signal, so calling pthread_kill
 * is not allowed.  
 * The signal handlers below do nothing.  They are installed to
 * tell the OS to send the signals to the process and the special
 * signal interception threads will handle the signals asynchroneously.
 *
 * These signal handlers can do nothing, not even call 
 * ufdbLogMessage because it uses a mutex.
 */
static void catchHUPSignal( int signal )
{
}


static void catchUSR1Signal( int signal )
{
}


static void catchUSR2Signal( int signal )
{
}


static void catchChildSignal( int signal )
{
   /* For some reason, on OpenBSD the SIGCHLD does not get through
    * to the signal_handler_thread() so we must do a waitpid() here.
    */
#if defined(__OpenBSD__) || defined(__NetBSD__)  || defined(__FreeBSD__)
   int        status;
   pid_t      pid;

   while ((pid = waitpid( -1, &status, WNOHANG )) > 0)
      ;
#endif
}


#if !UFDB_PRODUCE_CORE_DUMPS
static void catchAbortSignal( int signum )
{
   badSignal = SIGABRT;
   badSignalHandlerBusy = 1;
   UFDBglobalReconfig = UFDB_RECONFIGR_ABORT;
   UFDBglobalTerminating = 1;
   (void) alarm( 0 );

   /* how can it be? we already ignored SIGPIPE but saw the signal raised after a SIGABRT.
    * So ignore it again...
    */
   ufdbSetSignalHandler( SIGPIPE, SIG_IGN );

   ufdbLogMessage( "catchAbortSignal: ABORT signal was sent to ufdbguardd " UFDB_VERSION );
   BadSignalCaught( SIGABRT );

   removePidFile();

   UFDBchangeStatus( UFDB_API_STATUS_FATAL_ERROR );
   sleep( 1 );
   sleep( 1 );
   UFDBchangeStatus( UFDB_API_STATUS_TERMINATED );

   exit( 3 );
}
#endif


void catchTermSignal( int signum )
{
   /* do nothing. sigwait() in a thread will deal with the signal */
}


static void TermSignalCaught( int signum )
{
   if (UFDBglobalTerminating)
   {
      (void) alarm( 0 );
      ufdbLogMessage( "received %s signal but I was already terminating (reconfig=%d)  *****",
                      signum == SIGINT ? "INT" : "TERM", UFDBglobalReconfig );
      UFDBglobalReconfig = UFDB_RECONFIGR_TERM;
      return;
   }

   UFDBglobalTerminating = 1;
   UFDBglobalReconfig = UFDB_RECONFIGR_TERM;
   (void) alarm( 0 );

   ufdbLogMessage( "received %s signal", signum == SIGINT ? "INT" : "TERM" );
   uploadStatistics( signum == SIGINT ? "INT" : "TERM" );

   UFDBchangeStatus( UFDB_API_STATUS_TERMINATED );

   usleep( 100000 );
   if (sockethandler != (pthread_t) NULL)
      pthread_cancel( sockethandler );

   /* we are inside the signal_handler_thread and prefer that the main thread does all the cleanup */
   /* do 8 short sleeps instead of one large sleep since a few of them will be interrupted by signals */
   sleep( 1 );		
   sleep( 1 );	
   sleep( 1 );	
   sleep( 1 );	
   sleep( 1 );	
   sleep( 1 );	
   sleep( 1 );	
   sleep( 1 );	

   /* close all connections with clients */
   UFDBcloseFilesNoLog();

   removePidFile();
   ufdbLogMessage( "TermSignalCaught: bye bye" );
   _exit( 0 );
}


static int do_upload_crashreport( char * filename )
{
   int               fd;
   int               s;
   int               port;
   SSL *	     ssl;
   int               msglen;
   int               nwritten;
   char *            message;
   char *            content;
   struct stat       stbuf;
   struct utsname    sysinfo;

   fd = open( filename, O_RDONLY );
   if (fd < 0)
   {
      ufdbLogError( "cannot open crash report %s", filename );
      return 0;
   }

   if (fstat( fd, &stbuf ) != 0)
   {
      ufdbLogError( "cannot fstat crash report %s: %s", filename, strerror(errno) );
      close( fd );
      return 0;
   }

   ssl = NULL;
   port = 443;
   s = UFDBopenSocket( UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, port );
   if (s >= 0)
   {
      if (UFDBopenssl_connect( UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, port, s, &ssl ) != UFDB_API_OK)
      {
         ufdbLogError( "SSL/TLS handshake with %s:%d failed", UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, port );
	 close( s );
	 s = -1;
      }
   }
   else if (UFDBglobalDebug)
      ufdbLogMessage( "do_upload_crashreport: cannot connect to %s:443  trying %s:80", 
                      UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE );

   if (s < 0)
   {
      port = 80;
      s = UFDBopenSocket( UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, port );
      if (s < 0)
      {
	 ufdbLogError( "cannot upload crash report %s; connect to server %s on port 443 and port 80 failed with error: %s", 
		       filename, UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, strerror(errno) );
	 close( fd );
	 return 0;
      }
   }

   message = (char *) ufdbMalloc( 4096 + stbuf.st_size );

   content = (char *) ufdbMalloc( stbuf.st_size + 1 );
   if (read( fd, content, stbuf.st_size ) != stbuf.st_size)
   {
      ufdbLogError( "cannot read crash report %s : %s", filename, strerror(errno) );
      ufdbLogMessage( "It is recommended to send the crash report by email to support@urlfilterdb.com  *****" );
      close( s );
      close( fd );
      ufdbFree( message );
      ufdbFree( content );
      return 0;
   }
   content[stbuf.st_size] = '\0';

   ufdbGetSysInfo( &sysinfo );
   msglen = sprintf( message, 
	    "POST /cgi-bin/crashreport.pl HTTP/1.1\r\n"
	    "Host: " UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE "\r\n"
	    "User-Agent: ufdbGuardd-" UFDB_VERSION "\r\n"
	    "Content-Type: text/plain\r\n"
	    "Content-Length: %ld\r\n"
	    "Connection: close\r\n"
	    "X-NodeName: %s\r\n"
	    "X-NumThreads: %d\r\n"
	    "X-FileName: %s\r\n"
	    "\r\n"
	    "%s\r\n",
	    stbuf.st_size,
	    sysinfo.nodename,
	    UFDBglobalNworkers,
	    filename,
	    content  );

   errno = 0;
   if (port == 443)
      nwritten = UFDBopenssl_write( message, msglen, ssl );
   else
      nwritten = (int) write( s, message, msglen );
   if (nwritten != msglen)
      ufdbLogError( "cannot upload crash report %s : nwritten %d, %s", filename, nwritten, strerror(errno) );

   /* TODO: check HTTP reply status */

   ufdbFree( message );
   ufdbFree( content );
   if (port == 443)
      UFDBopenssl_close( ssl );
   close( s );
   close( fd );

   return nwritten == msglen;
}


static void upload_crash_reports( void )
{
   int             reports_seen;
   int             previous_status;
   DIR *           dirh;
   struct dirent * entry;

   reports_seen = 0;
   previous_status = 0;
   dirh = opendir( "/tmp" );
   if (dirh != NULL)
   {
      while ((entry = readdir(dirh)) != NULL)
      {
         if (strncmp( entry->d_name, "urlfilterdb.crashreport.", 24 ) == 0)
	 {
	    int  retval;
	    char fname[1024];
	    char newfname[1024];

	    reports_seen = 1;
	    sprintf( fname, "/tmp/%s", entry->d_name );
	    if (UFDBglobalUploadCrashReports)
	    {
	       if (do_upload_crashreport( fname ))
	       {
		  sprintf( newfname, "/tmp/uploaded.%s", entry->d_name );
		  UFDBraisePrivileges( UFDBglobalUserName, "rename uploaded crash report" );
		  retval = rename( fname, newfname );
		  UFDBdropPrivileges( UFDBglobalUserName );
		  if (retval == 0)
		     ufdbLogMessage( "uploaded and renamed crash report %s to %s", fname, newfname );
		  else
		     ufdbLogError( "could not rename crash report from %s to %s: %s", 
                                   fname, newfname, strerror(errno) );
	       }
	       else
	       {
		  if (!reports_seen)
		     previous_status = UFDBchangeStatus( UFDB_API_STATUS_CRASH_REPORT_UPLOADED );
		  else
		     (void) UFDBchangeStatus( UFDB_API_STATUS_CRASH_REPORT_UPLOADED );
	       }
	    }
	    else
	    {
	       ufdbLogError( "Not allowed to upload crash report '%s'  *****", fname );
	       ufdbLogError( "It is recommended to send the crash report to the support desk of urlfilterdb.com  *****" );
	       ufdbLogError( "Set upload-crash-reports to 'on' to upload crash reports.  *****" );
	       if (!reports_seen)
		  previous_status = UFDBchangeStatus( UFDB_API_STATUS_CRASH_REPORT_NOT_UPLOADED );
	       else
		  (void) UFDBchangeStatus( UFDB_API_STATUS_CRASH_REPORT_NOT_UPLOADED );
	    }
	 }
      }
      closedir( dirh );
   }

   if (reports_seen)
      (void) UFDBchangeStatus( previous_status );
}


void BadSignalCaught( int signum )	
{
   int       i;
   int       num;
   char      me[48];
   
   /* We desperately want a crash report but repeating signals prevent this so ignore the bad signal */
   ufdbSetSignalHandler( signum, SIG_IGN );
   /* pclose() generates a SIGCHLD which we also want to ignore here */
   ufdbSetSignalHandler( SIGCHLD, SIG_IGN );

   /* find out which thread has a signal */
   sprintf( me, "pid %d", (int) getpid() );
   if (badSignalThread == threadedSignalHandler)
   {
      strcpy( me, "thread signal-handler" );
   }
   else if (badSignalThread == housekeeper)
   {
      strcpy( me, "thread housekeeper" );
   }
   else if (badSignalThread == sockethandler)
   {
      strcpy( me, "thread socket-handler" );
   }
   else if (badSignalThread == dyn_userlist_handler)
   {
      strcpy( me, "thread dynamic-userlist-handler" );
   }
   else if (badSignalThread == dyn_domainlist_handler)
   {
      strcpy( me, "thread dynamic-domainlist-handler" );
   }
   else 
   {
      num = UFDB_NUM_HTTPS_VERIFIERS;
      for (i = 0; i < num; i++)
         if (badSignalThread == httpsthr[i])
	 {
	    sprintf( me, "thread https-verifier-%02d", i );
	    break;
	 }
      num = UFDBglobalNworkers;
      for (i = 0; i < num; i++)
         if (badSignalThread == workers[i])
	 {
	    sprintf( me, "thread worker-%03d", i );
	    ufdbLogMessage( "worker %03d received signal %d while processing URL '%s'", 
                            i, signum, workerURLbuffers[i] );
	    break;
	 }
   }
   ufdbLogMessage( "%s caught bad signal %d  *****", me, signum );

   if (strcmp( UFDB_GDB_PATH, "no" ) == 0)
   {
      ufdbLogMessage( "ufdb-pstack is not called since the gdb package was not installed." );
      ufdbLogMessage( "It is highly recommended to install gdb and reinstall ufdbguard to be able to produce a crash report.  *****" );
      ufdbLogMessage( "Crash reports are essential to be able to resolve bugs quickly." );
      for (i = 0; i < UFDBglobalNworkers; i++)
         ufdbLogMessage( "worker %03d is processing URL '%s'", i, workerURLbuffers[i] );
   }
   else
      ufdbExecutePstack( me );

   uploadStatistics( "BADSIG" );

   UFDBchangeStatus( UFDB_API_STATUS_FATAL_ERROR );
   badSignalHandlerBusy = 0;
}


static void crasher( void )
{
   if (badSignal == 0)
      badSignal = SIGILL;
   badSignalHandlerBusy = 1;
   ufdbLogMessage( "crasher: fatal error occurred and this program is about to crash  *****" );
   BadSignalCaught( SIGABRT );
   sleep( 1 );
   exit( 9 );
}


void __wrap___stack_chk_fail( void )
{
   badSignalHandlerBusy = 1;
#if 0
   UFDBglobalReconfig = UFDB_RECONFIGR_STACK;
   UFDBglobalTerminating = 1;
#endif
   (void) alarm( 0 );

   ufdbLogFatalError( "stack allocation failed.  simulating bad signal to call ufdb-pstack." );

   badSignalThread = pthread_self();
   BadSignalCaught( SIGQUIT );
   _exit( 4 );
}


void __GI___fortify_fail( const char * msg )
{
   badSignalHandlerBusy = 1;
#if 0
   UFDBglobalReconfig = UFDB_RECONFIGR_FORTIFY;
   UFDBglobalTerminating = 1;
#endif
   (void) alarm( 0 );

   ufdbLogFatalError( "fortified code failed: %s.  simulating bad signal to call ufdb-pstack.", msg );

   badSignalThread = pthread_self();
   BadSignalCaught( SIGQUIT );
   _exit( 4 );
}


void __wrap___fortify_fail( const char * msg )
{
   badSignalHandlerBusy = 1;
#if 0
   UFDBglobalReconfig = UFDB_RECONFIGR_FORTIFY;
   UFDBglobalTerminating = 1;
#endif
   (void) alarm( 0 );

   ufdbLogFatalError( "fortified code failed: %s.  simulating bad signal to call ufdb-pstack.", msg );

   badSignalThread = pthread_self();
   BadSignalCaught( SIGQUIT );
   _exit( 4 );
}


void __wrap___assert_fail( const char * assertion, const char * file, unsigned int line, const char * function )
{
   badSignalHandlerBusy = 1;
#if 0
   UFDBglobalReconfig = UFDB_RECONFIGR_FORTIFY;
   UFDBglobalTerminating = 1;
#endif
   (void) alarm( 0 );

   ufdbLogFatalError( "libc assertion failure: %s line %d: in function %s: %s", file, line, function, assertion );

   badSignalThread = pthread_self();
   BadSignalCaught( SIGQUIT );
   _exit( 4 );
}


#if !UFDB_PRODUCE_CORE_DUMPS
static void ufdbCatchBadSignal( int signum )	
{
   /* Note that we are inside a signal handler and can do almost nothing. */

   if (badSignal != signum)
   {
      badSignalHandlerBusy = 1;
      badSignal = signum;
      UFDBglobalReconfig = UFDB_RECONFIGR_BADSIG;
      UFDBglobalTerminating = 1;
      (void) alarm( 0 );

      badSignalThread = pthread_self();

      ufdbLogMessage( "ufdbCatchBadSignal: received signal %d", signum );

      /* Cancel the sockethandler thread so that the main thread will wake up and 
       * wind up.
       * But sleep first so that ufdb-pstack can do its job in an other thread.
       */
      sleep( 1 );
      if (sockethandler != (pthread_t) NULL)
         pthread_cancel( sockethandler );
#if 0
      sleep( 1 );
      pthread_kill( sockethandler, SIGTERM );
#endif

      removePidFile();

      /* If for any reason the termination by main fails, we have to call exit() ourselves. */
      sleep( 2 );
      sleep( 2 );
      sleep( 2 );
      sleep( 2 );
      sleep( 2 );
      ufdbLogError( "ufdbCatchBadSignal: calling exit() after waiting for 10 seconds for main to terminate ufdbguardd" );
      exit( 10 );
   }
}
#endif


#define MAX_RWLOCK_ATTEMPTS 60

static void HupSignalCaught( int sig )
{
   time_t          t;
   struct tm       thetime;
   int             status;
   pid_t           pid;
   FILE *          fp;
#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
   int             ret;
   struct timespec DatabaseLockTimeout;
#else
   unsigned int    sleep_period;
#endif

   if (UFDBglobalTerminating)
      return;

   if (pthread_mutex_trylock( &sighup_mutex ) != 0)
   {
      ufdbLogMessage( "HUP signal is already being processed" );
      return;
   }

   /* going to reload the configuration but first wait for others to finish */
   while (UFDBglobalReconfig)
      usleep( 100000 );
   UFDBglobalReconfig = UFDB_RECONFIGR_HUP;

   alarm( 0 );

   ufdbLogMessage( "HUP signal received to reload the configuration and database" );
   UFDBchangeStatus( UFDB_API_STATUS_RELOADING );

   uploadStatistics( "HUP" );

   /* To prevent counting 'old' clients: 
    * if today is Wednesday, we reset the IP counters.
    * If reset was more than 8 days ago, we also do a reset now.
    */
   t = time( NULL );
   if (lastUpload != 0  &&  t - lastUpload > SECSOF8DAYS)
   {
      UFDBinitializeIPcounters();
      UFDBinitializeUserCounters();
   }
   else
   {
      localtime_r( &t, &thetime );
      if (thetime.tm_wday == 3)
      {
	 UFDBinitializeIPcounters();
         UFDBinitializeUserCounters();
      }
   }

   /* Get the pid of the HTTP daemon. It is used later to kill it. */
   httpDaemonPid = -1;
   if (UFDBglobalHttpdPort > 0)
   {
      fp = fopen( UFDBHTTPD_PID_FILE, "r" );
      if (fp != NULL)
      {
	 if (1 != fscanf( fp, "%d", &httpDaemonPid ))
	 {
	    ufdbLogError( "file %s does not have a process id", UFDBHTTPD_PID_FILE );
	    httpDaemonPid = -1;
	 }
	 fclose( fp );
      }
      else
      {
	 ufdbLogError( "cannot open file %s: %s", UFDBHTTPD_PID_FILE, strerror(errno) );
      }
   }


   sleep( 1 );
   if (UFDBgetTunnelCheckMethod() == UFDB_API_HTTPS_CHECK_OFF)
   {
      usleep( 500 * 1000 );		/* 0.5 seconds */
   }
   else
   {
      sleep( 1 );
      sleep( 1 );
      sleep( 1 );
   }

   if (UFDBgetTunnelCheckMethod() == UFDB_API_HTTPS_CHECK_AGGRESSIVE)
   {
      /* with aggressive HTTPS port probing we must wait patiently for workers to release the read lock */
      sleep( 1 );
      sleep( 1 );
      sleep( 1 );
      sleep( 1 );
      sleep( 1 );

      sleep( 1 );
      sleep( 1 );
      sleep( 1 );
      sleep( 1 );
      sleep( 1 );
   }

#if UFDB_HAVE_NATIVE_RWLOCK_MUTEX

#if 0
   /* Inside a virtual machine, sometimes pthread_rwlock_timedwrlock fails.
    * The root cause is unknown, but we try to resolve this by first using
    * pthread_rwlock_trywrlock.
    */
   ret = pthread_rwlock_trywrlock( &TheDatabaseLock );
   if (ret == 0)
   {
      ufdbLogMessage( "database write lock acquired with first attempt" );
      goto lock_was_acquired;
   }
#endif

   /* 
    * Synchronise with all worker threads.
    * UFDBglobalReconfig is set for the last 1.5-11 seconds so the worker threads have 
    * most likely no read-locks any more.  Try to get a write lock within 0.3 seconds
    * and if this fails, we will sleep 5 seconds more and acquire the rwlock with
    * pthread_rwlock_wrlock().
    *
    * We use a 0.3-second timeout since we do not want to block readers (workers)
    * too long because the flow of messages back to Squid may not be interrupted too long.
    * Note: 1,000,000,000 nanoseconds make 1 second.
    */
   ufdbLogMessage( "requesting a write lock for the in-memory database (fast mode) ..." );

   DatabaseLockTimeout.tv_sec = 0;
   DatabaseLockTimeout.tv_nsec = 300000000;             /* 0.3 seconds */
   ret = pthread_rwlock_timedwrlock( &TheDatabaseLock, &DatabaseLockTimeout );
   if (ret == 0)
      ufdbLogMessage( "lock acquired to reload the URL database" );
   else
   {
      if (ret == ETIMEDOUT  ||  ret == EBUSY)
      {
	 ufdbLogMessage( "Could not obtain a lock quickly to refresh the URL database\n"
	                 "waiting 5 more seconds before attempting to acquire the lock again  *****" );
	 /* sleep(5) and account for interrupts */
	 sleep( 1 );
	 sleep( 1 );
	 sleep( 1 );
	 sleep( 1 );
	 sleep( 1 );
      }
      else 
	 ufdbLogError( "pthread_rwlock_timedwrlock failed with code %d  *****", ret );

      sleep( 1 );
      ufdbLogMessage( "requesting a write lock for the in-memory database (slow mode)..." );

      /* 
       * Wait many seconds for a write lock ...
       * This most likely DOES NOT DISTURB USERS since most workers will be in "reload mode" 
       * replying requests from Squid.
       * This state is not desirable since "reload mode" either does not filter or blocks all access
       * so the time to reload a configuration should always be as short as possible.
       */
      DatabaseLockTimeout.tv_sec  = 60;
      DatabaseLockTimeout.tv_nsec = 0;
      ret = pthread_rwlock_timedwrlock( &TheDatabaseLock, &DatabaseLockTimeout );
      if (ret == 0)
	 ufdbLogMessage( "lock acquired to reload the configuration and URL database" );
      else
      {
         if (ret == EDEADLK)
            ufdbLogFatalError( "HupSignalCaught: deadlock detected trying to acquire database write lock" );
         else
	 if (ret == ETIMEDOUT  ||  ret == EBUSY)
	 {
	    ufdbLogError( "HUP signal received but could not acquire a lock on the configuration  *****\n"
	                  "Perhaps a dynamic refresh is taking too long or hangs "
                          "(check all commands used by execuserlist and execdomainlist) *****\n"
			  "Verify that the system load is normal.\n"
			  "On virtual machines, verify that the virtual machine has sufficient resources.\n"
			  "Request support from the Support Desk of www.urlfilterdb.com "
                          "in case there are any doubts." );
            ufdbExecutePstack( "debug-cannot-get-rwlock-in-60-seconds" );
	 }
	 else
	    ufdbLogError( "pthread_rwlock_wrlock failed with code %d  *****", ret );

         /*
          * We have two choices here:
          * 1) raise SIGABRT since we cannot reload the configuration,
          * 2) forget SIGHUP and not reload the configuration.
          * Both options are not very desirable, but option 2 is a bit more friendly for the end users.
          */
         if (ret != ETIMEDOUT  &&  ret != EBUSY)
            ufdbExecutePstack( "cannot-get-rwlock-for-database-refresh-after-many-attempts" );
         ufdbHandleAlarmForTimeEvents( UFDB_PARAM_INIT );
         UFDBchangeStatus( UFDB_API_STATUS_FATAL_ERROR );
         if (ret == EDEADLK)
         {
            ufdbLogFatalError( "must terminate because of a fatal deadlock" );
            exit( 9 );
         }
         UFDBglobalReconfig = UFDB_RECONFIGR_NONE;
         return;
      }
   }

// lock_was_acquired:
   /* 
    * It was believed that the rwlock must be released ASAP but it not the case.
    * the lock will be relased after all processing has been terminated.
    */

#else
   /* sleep() is a very crude way to allow worker threads to finish what
    * they were doing before we remove the database from memory,
    * but it allows us not to use mutexes for a many-reader-one-writer problem.
    */
   sleep_period = UFDBglobalNworkers / 3;
   if (sleep_period < 30)
      sleep_period = 30;
   if (sleep_period > 80)
      sleep_period = 80;
   if (UFDBgetTunnelCheckMethod() == UFDB_API_HTTPS_CHECK_AGGRESSIVE)
      sleep_period += 15;
   ufdbLogMessage( "sleeping %d seconds to allow worker threads to finish current work", sleep_period );
   sleep( sleep_period );		
   ret = -1;
#endif

   UFDBglobalReconfig = UFDB_RECONFIGR_RELOAD;

   /* TODO: reload CA certificates */
   /* TODO: clear security cache */

   ufdbLogMessage( "releasing URL database from memory ..." );
   ufdbFreeAllMemory();
   ufdbLogMessage( "reading configuration file and database ..." );
   UFDBglobalFatalError = 0;
   UFDBglobalHttpdPort = 0;
   if (sgReadConfig( configFile ) == 0)
   {
      UFDBglobalReconfig = UFDB_RECONFIGR_FATAL;
      ufdbLogFatalError( "exiting due to missing configuration file" );
      exit( 3 );
   }
   if (UFDBglobalTerminating)
   {
      ufdbLogMessage( "HupSignalCaught: a fatal error occurred and ufdbguardd is terminating  *****" );
#if UFDB_HAVE_NATIVE_RWLOCK_MUTEX
      if (ret == 0)
      {
         ufdbLogMessage( "releasing the rwlock" );
         (void) pthread_rwlock_unlock( &TheDatabaseLock );
      }
#endif
      return;
   }
   if (UFDBglobalFatalError)
   {
      ufdbLogMessage( "A FATAL ERROR OCCURRED: all requests are %s since url-lookup-result-when-fatal-error is %s "
                      "(see previous lines with \"FATAL ERROR\" for more details)  *****",
                      UFDBglobalURLlookupResultFatalError == UFDB_ALLOW ? "ALLOWED" : "BLOCKED",
                      UFDBglobalURLlookupResultFatalError == UFDB_ALLOW ? "allow" : "deny"  );
   }

   /* TODO: only stop/start ufdbhttpd if the parameters have changed */
   if (httpDaemonPid > 0)
   {
      int retval;

      UFDBraisePrivileges( UFDBglobalUserName, "kill old ufdbhttpd" );

      ufdbLogMessage( "killing old ufdbhttpd (pid=%d)", httpDaemonPid );
      retval = kill( httpDaemonPid, SIGTERM );
      if (retval != 0)
	 ufdbLogError( "cannot send TERM signal to the HTTP daemon (pid=%d): %s", 
		       httpDaemonPid, strerror( errno ) );

      usleep( 400000 );
      /* be sure that the daemon is killed. */
      if (kill( httpDaemonPid, SIGKILL ) == 0)
	 usleep( 100000 );

      while ((pid = waitpid( -1, &status, WNOHANG )) > 0)
	 ;

      if (kill( httpDaemonPid, 0 ) < 0)
	 ufdbLogMessage( "HTTP daemon (pid=%d) terminated to be restarted (OK)", httpDaemonPid );
      else
	 ufdbLogError( "HTTP daemon did not terminate: %s", strerror(errno) );

      UFDBdropPrivileges( UFDBglobalUserName );
   }

   if (UFDBglobalHttpdPort > 0)
      startHttpDaemon();

   UFDBlogConfig();

   adjustNumberOfWorkerThreads();

   UFDBlookupCounter = 0;
   UFDB_API_num_https = 0;
   UFDB_API_num_bumps = 0;
   UFDB_API_num_url_localnet = 0;
   UFDBblockCounter = 0;
   UFDBtestBlockCounter = 0;
   UFDBuncategorisedCounter = 0;
   UFDBglobalTunnelCounter = 0;
   UFDBsafesearchCounter = 0;
   UFDBedufilterCounter = 0;
   UFDBuploadSeqNo++;

   if (UFDBglobalFatalError)
   {
      UFDBchangeStatus( UFDB_API_STATUS_FATAL_ERROR );
   }
   else
   {
      UFDBchangeStatus( UFDB_API_STATUS_RELOAD_OK );
   }

#if 0
   /* delay resetting UFDBglobalReconfig to let the workers finish their loops */
   usleep( 100000 );
#endif 

   ufdbHandleAlarmForTimeEvents( UFDB_PARAM_INIT );
   if (UFDBglobalFatalError)
      ufdbLogMessage( "A FATAL ERROR OCCURRED: all requests are answered with \"OK\" "
                      "(see previous lines with \"FATAL ERROR\" for more details)  *****" );
   else
      ufdbLogMessage( "the new configuration and database are loaded for ufdbguardd " UFDB_VERSION );

   UFDBglobalReconfig = UFDB_RECONFIGR_NONE;
#if UFDB_HAVE_NATIVE_RWLOCK_MUTEX
   ufdbLogMessage( "releasing the rwlock" );
   if (ret == 0)
   {
      (void) pthread_rwlock_unlock( &TheDatabaseLock );
   }
#endif

   pthread_mutex_unlock( &sighup_mutex );

#if 0
   pthread_kill( dyn_userlist_handler, SIGHUP );
   pthread_kill( dyn_domainlist_handler, SIGHUP );
#endif
}


static void incr_active_workers( void )
{
#if defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4  &&  __SIZEOF_INT__ == 4
   (void) __sync_add_and_fetch( &n_active_workers, 1 );
#elif defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8  &&  __SIZEOF_INT__ == 8
   (void) __sync_add_and_fetch( &n_active_workers, 1 );
#else
   pthread_mutex_lock( &incrMutex );
   n_active_workers++;
   pthread_mutex_unlock( &incrMutex );
#endif
}


static void decr_active_workers( void )
{
#if defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4  &&  __SIZEOF_LONG__ == 4
   (void) __sync_sub_and_fetch( &n_active_workers, 1 );
#elif defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8  &&  __SIZEOF_LONG__ == 8
   (void) __sync_sub_and_fetch( &n_active_workers, 1 );
#else
   pthread_mutex_lock( &incrMutex );
   n_active_workers--;
   pthread_mutex_unlock( &incrMutex );
#endif
}


/* 
 * Communication between the worker and other threads is done via
 * RW-locks and data is passed via a queue.
 *
 * Since this queue is a FIFO we could use a tail queue. But we don't
 * like the malloc() overhead, so therefore we use a static array.
 */

#define UFDB_MAX_FD_QUEUE   UFDB_MAX_THREADS


static struct {
   int    fd;
} q[UFDB_MAX_FD_QUEUE];

UFDB_GCC_ALIGN64 
static pthread_mutex_t fdq_lock = UFDB_STATIC_MUTEX_INIT;

UFDB_GCC_ALIGN64 
static pthread_cond_t  empty = PTHREAD_COND_INITIALIZER;

UFDB_GCC_ALIGN64 
volatile static int    n_queued = 0;
volatile static int    ihead = 0;                    /* array index for the head */
volatile static int    itail = 0;                    /* array index for the tail */


/*
 * enqueue a new fd.
 */
static void putFdQueue( int fd )
{
   int ret;

   ret = pthread_mutex_lock( &fdq_lock );		/* ======================================= */
   if (ret != 0)
#ifdef UFDB_DEBUG
      ufdbLogError( "putFdQueue: mutex_lock failed with code %d *****", ret );
#else
      ;
#endif

   if (n_queued < UFDB_MAX_FD_QUEUE)
   {
      /*
       * insert it at the tail
       */
      q[itail].fd = fd;

      itail = (itail + 1) % UFDB_MAX_FD_QUEUE;
      n_queued++;

      /*
       * leave the critical section
       */
      ret = pthread_mutex_unlock( &fdq_lock );		/* ======================================= */
      if (ret != 0)
#ifdef UFDB_DEBUG
	 ufdbLogError( "putFdQueue: mutex_unlock failed with code %d *****", ret );
#else
         ;
#endif

      /*
       * signal anyone who is waiting
       */
      if (n_queued == 1)
	 pthread_cond_signal( &empty );
      else
	 pthread_cond_broadcast( &empty );
   } 
   else    
   {
      ret = pthread_mutex_unlock( &fdq_lock );		/* ======================================= */
      ufdbLogFatalError( "connection queue is full.  *****\n"
                         "There are too many ufdbgclient processes !!\n"
                         "the maximum is %d when the -w option is used\n"
      			 "closing fd %d", 
			 UFDB_MAX_FD_QUEUE, fd );
      /* there is no other option than to close the connection */
      /* TO-DO: maybe: close all queued fd's ? */
      close( fd );
   }

   if (n_queued + n_active_workers > UFDBglobalNworkers)
   {
      usleep( 100000 );
      if (n_queued + n_active_workers > UFDBglobalNworkers)
      {
	 ufdbLogError( "NOT ENOUGH WORKER THREADS  *****\n"
		       "There are curently %d active worker threads and\n"
		       "the configured maximum number of worker threads is %d and\n"
		       "there are %d connections pending to be serviced.\n"
		       "Count the number of ufdbgclient processes which should be less than %d\n"
		       "You may also request help at support@urlfilterdb.com",
		       n_active_workers,
		       UFDBglobalNworkers,
		       n_queued,
		       UFDBglobalNworkers );
      }
   }
}


/*
 * get a fd where there is new content.
 */
static void getFdQueue( int * fd )
{
   int ret;

allover:
   ret = pthread_mutex_lock( &fdq_lock );
   if (ret != 0)
#ifdef UFDB_DEBUG
      ufdbLogError( "getFdQueue: mutex_lock failed with code %d *****", ret );
#else
      ;
#endif

   while (1)
   {
      /*
       * if there are jobs in the queue
       */
      if (n_queued > 0)
      {
         n_queued--;
         /*
          * get the one at the head
          */
         *fd = q[ihead].fd;
         ihead = (ihead + 1) % UFDB_MAX_FD_QUEUE;

         ret = pthread_mutex_unlock( &fdq_lock );
	 if (ret != 0)
#ifdef UFDB_DEBUG
	    ufdbLogError( "getFdQueue: mutex_unlock failed with code %d *****", ret );
#else
            ;
#endif

	 return;
      } 
      else   /* otherwise wait until there are fds available */ 
      {
         pthread_cond_wait( &empty, &fdq_lock );
         /*
          * when I'm here, I've been signaled because there
          * are jobs in the queue.  Go try and get one.
          */
	 ret = pthread_mutex_unlock( &fdq_lock );
	 if (ret != 0)
#ifdef UFDB_DEBUG
	    ufdbLogError( "getFdQueue: mutex_unlock failed with code %d *****", ret );
#else
            ;
#endif
	 usleep( ((unsigned long) pthread_self()) % 821 );
	 goto allover;
      }
   }
}


static void daemon_accept_connections( 
   int            s, 
   int            protocol )
{
   int            n;
   int            newfd;
   fd_set         fds;
   struct timeval tv;

   /*
    * Allow that this thread can be cancelled without delays at any time.
    */
   pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
   pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );

#if 0
   if (fcntl( s, F_SETFD, FD_CLOEXEC ))
      ;
#endif

   while (!UFDBglobalTerminating)
   {
      FD_ZERO( &fds );
      FD_SET( s, &fds );
      errno = 0;
      n = select( s+1, &fds, (fd_set *) NULL, (fd_set *) NULL, (struct timeval *) NULL );

      if (UFDBglobalTerminating)
	 break;

      if (n > 0) 
      {
	 newfd = accept( s, NULL, NULL );
	 if (newfd < 0)
	 {
	    if (errno == EINTR)
	    {
	       continue;
	    }
	    ufdbLogError( "SRV: accept on socket failed: %s", strerror(errno) );
	    return;
	 }

#if 0
	 if (fcntl( newfd, F_SETFD, FD_CLOEXEC ))
	    ;
#endif

	 if (protocol == AF_INET)
	 {
	    int sock_parm;

	    sock_parm = 1;
	    setsockopt( newfd, IPPROTO_TCP, TCP_NODELAY, (void *) &sock_parm, sizeof(sock_parm) );
	    sock_parm = 1;
	    setsockopt( newfd, SOL_SOCKET, SO_KEEPALIVE, (void *) &sock_parm, sizeof(sock_parm) );
	    sock_parm = 64 * 1024;
	    setsockopt( newfd, SOL_SOCKET, SO_SNDBUF, (void *) &sock_parm, sizeof(sock_parm) );
	    sock_parm = 64 * 1024;
	    setsockopt( newfd, SOL_SOCKET, SO_RCVBUF, (void *) &sock_parm, sizeof(sock_parm) );
	 }

	 /*
	  *  In case of troubles, the timeout prevents that a thread will block for ever.
	  */
#if 0
	 tv.tv_sec = 16 * 60;		/* we need unlimited read timeout since we simply wait for input */
	 tv.tv_usec = 0;
	 setsockopt( newfd, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
#endif
	 tv.tv_sec = 5;			/* 5 second send timeout */
	 tv.tv_usec = 0;
	 setsockopt( newfd, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

	 if (UFDBglobalDebug > 2)
	    ufdbLogMessage( "SRV: accept new fd %2d", newfd );

	 putFdQueue( newfd );
      }
   }

   ufdbLogMessage( "connection handler: terminating=%d badsig=%d", UFDBglobalTerminating, badSignal );
   if (badSignal)
   {
      ufdbLogMessage( "connection handler: sleeping to let ufdb-pstack make a crash report" );
      sleep( 1 );
      sleep( 1 );
      sleep( 1 );
   }
}


static int allowed_to_remove_unix_socket( void )
{
   int    retval;
   pid_t  pid;
   char * pidfile;
   FILE * fp;

   if (UFDBglobalPidFile == NULL)
      pidfile = UFDBGUARDD_PID_FILE;
   else
      pidfile = UFDBglobalPidFile;

   /* read the pid file, if no file return 1 */
   pid = -1;
   fp = fopen( pidfile, "r" );
   if (fp != NULL)
   {
      if (1 != fscanf( fp, "%d", &pid ))
      {
	 ufdbLogError( "allowed_to_remove_unix_socket: pid file \"%s\" does not have a process id\n", pidfile );
	 fclose( fp );
	 return 1;
      }
      else if (UFDBglobalDebug)
         ufdbLogMessage( "allowed_to_remove_unix_socket: read pid %d from file \"%s\"", pid, pidfile );
      fclose( fp );
      if (pid < 0)
	 return 1;
   }
   else
   {
      if (UFDBglobalDebug)
	 ufdbLogMessage( "allowed_to_remove_unix_socket: cannot open file \"%s\": %s", pidfile, strerror(errno) );
      return 1;
   }

   /* check if the pid exists, if not return 1 */
   UFDBraisePrivileges( UFDBglobalUserName, "check if an other ufdbguardd process exists" );
   retval = kill( pid, 0 );
   UFDBdropPrivileges( UFDBglobalUserName );
   if (retval != 0)
   {
      if (UFDBglobalDebug)
	 ufdbLogMessage( "allowed_to_remove_unix_socket: sending signal 0 to pid %d failed: %s", pid, strerror(errno) );
      return 1;
   }

   ufdbLogFatalError( "another instance of ufdbguardd with pid %d is running.", pid );
   return 0;
}


void * socket_handler_thread( void * ptr )
{
   int                 protocol;
   sigset_t            sigset;
   int                 i;
   int                 s;
   int                 sock_parm;
   int                 retval;
   char *              ev;
   long                num_cpus;
   struct utsname      sysinfo;
   pthread_attr_t      attr;
   struct sockaddr_in  addr_in;
#if HAVE_UNIX_SOCKETS
   struct sockaddr_un  addr_un;
#endif

   UFDBglobalReconfig = UFDB_RECONFIGR_INIT;

   /* Most signals must be blocked.
    * This is a requirement to use sigwait() in a thread.
    */
   sigemptyset( &sigset );
   sigaddset( &sigset, SIGHUP );
   sigaddset( &sigset, SIGUSR1 );
   sigaddset( &sigset, SIGUSR2 );
   sigaddset( &sigset, SIGCHLD );
   sigaddset( &sigset, SIGTERM );
   sigaddset( &sigset, SIGINT );
   sigaddset( &sigset, SIGALRM );
#if 0
   sigaddset( &sigset, SIGSEGV );	/* blocking SEGV with pthread_sigmask has undefined results */
   sigaddset( &sigset, SIGILL );
   sigaddset( &sigset, SIGBUS );
#endif
   pthread_sigmask( SIG_BLOCK, &sigset, NULL );

   gettimeofday( &start_time, NULL );

   globalArgv = my_argv;
   globalEnvp = my_envp;

   /* ufdbSetGlobalErrorLogFile( 0 ); */
   ufdbResetUnknownURLs();
   UFDBglobalFatalError = 0;
   prev_n_workers = UFDBglobalNworkers;

   if (sgReadConfig( configFile ) == 0)
   {
      ufdbLogFatalError( "exiting due to missing configuration file" );
      UFDBchangeStatus( UFDB_API_STATUS_FATAL_ERROR );
      exit( 3 );
   }

   /* Now is the right moment to write out main PID to the pid file.
    * Note that systemd monitors the creation of the PID file.
    */
   if (writePidFile() == 0)
   {
      ufdbLogError( "make sure that %s exists and has the correct permissions", DEFAULT_PIDDIR );
      ufdbLogFatalError( "cannot write my PID to the pidfile (see previous lines)" );
   }
   atexit( removePidFile );

   /* ufdbSetGlobalErrorLogFile( 0 ); */
   if (UFDBglobalDebug)
      ufdbLogMessage( "sgReadConfig() finished. fatal=%d", UFDBglobalFatalError );
   UFDBlogConfig();

   /* do this as early as possible since other threads may be waiting for the initialisation to be completed */
   UFDBinitHTTPSchecker();

   ev = getenv( "LC_CTYPE" );
   if (ev == NULL)
      ufdbLogMessage( "LC_CTYPE is not set" );
   else
      ufdbLogMessage( "LC_CTYPE is '%s'", ev );

   ev = getenv( "LANG" );
   if (ev == NULL)
      ufdbLogMessage( "LANG is not set" );
   else
      ufdbLogMessage( "LANG is '%s'", ev );

   ufdbRegisterFatalErrorCrashfun( crasher );

   adjustNumberOfWorkerThreads();	/* create more worker threads if necessary */

#ifdef _POSIX_PRIORITY_SCHEDULING
   sched_yield();
#else
   usleep( 50000 );
#endif

   pthread_attr_init( &attr );
   pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
#if HAVE_PTHREAD_ATTR_SETGUARDSIZE && UFDB_DEBUG
   pthread_attr_setstacksize( &attr, (400-8) * 1024 );
   pthread_attr_setguardsize( &attr, 8 * 1024 );
#else
   pthread_attr_setstacksize( &attr, 400 * 1024 );
#endif

   for (i = 0; i < UFDB_NUM_HTTPS_VERIFIERS; i++)
   {
      pthread_create( &httpsthr[i], &attr, UFDBhttpsTunnelVerifier, (void *) ((long) i) );
      if (i % 4 == 0)
      {
#ifdef _POSIX_PRIORITY_SCHEDULING
	 sched_yield();
#else
	 usleep( 10000 );
#endif
      }
   }
   ufdbLogMessage( "%d HTTPS verification threads created.", UFDB_NUM_HTTPS_VERIFIERS );
   usleep( 50000 );

   if (testMode)
      ufdbLogMessage( "-T option is used.  In test mode no URLs are ever blocked." );

   if (UFDBglobalFatalError)
   {
      ufdbLogError( "A FATAL ERROR OCCURRED: ALL REQUESTS ARE ANSWERED WITH \"OK\" "
                    "(see previous lines with \"FATAL ERROR\" for more information)  *****" );
      UFDBchangeStatus( UFDB_API_STATUS_FATAL_ERROR );
   }
   else
   {
      ufdbHandleAlarmForTimeEvents( UFDB_PARAM_INIT );
      UFDBchangeStatus( UFDB_API_STATUS_STARTED_OK );
   }

   /*
    * Create the daemon socket that the it accepts connections on.
    * if available, first try a UNIX socket and then an IP socket.
    */
#if HAVE_UNIX_SOCKETS
   protocol = AF_UNIX;
   s = socket( protocol, SOCK_STREAM, 0 );
   if (s < 0)
   {
      protocol = AF_INET;
      s = socket( protocol, SOCK_STREAM, 0 );
   }
#else
   protocol = AF_INET;
   s = socket( protocol, SOCK_STREAM, 0 );
#endif

   if (s < 0)
   {
      ufdbLogError( "cannot create daemon socket: %s", strerror(errno) );
      UFDBchangeStatus( UFDB_API_STATUS_FATAL_ERROR );
      pthread_exit( (void *) 0 ); 
   }

   /* 
    * The port number can be specified in the config file (UFDBglobalPortNum is set by the parser)
    * or a command line parameter (portNumCmdLine is set in main).
    * The command line setting has preference.
    */
   if (portNumCmdLine >= 0)
      UFDBglobalPortNum = portNumCmdLine;

   errno = 0;
#if HAVE_UNIX_SOCKETS
   if (protocol == AF_UNIX)
   {
      sprintf( addr_un.sun_path, "/tmp/ufdbguardd-%05d", UFDBglobalPortNum );
#if 0
      if (unlink( addr_un.sun_path ) < 0  &&  errno != ENOENT)
         ufdbLogFatalError( "cannot remove socket %s: %s", addr_un.sun_path, strerror(errno) );
#endif
      addr_un.sun_family = AF_UNIX;
      /* with anti-aliasing warnings ON, connect/bind cause compiler warnings which we may ignore */
      retval = bind( s, (struct sockaddr *) &addr_un, sizeof(addr_un) );
      /* allow anybody to connect to the socket */
      if (retval == 0)
      {
         (void) chmod( addr_un.sun_path, S_IRUSR|S_IWUSR| S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH );
	 ufdbLogMessage( "UNIX socket \"%s\" successfully created", addr_un.sun_path );
      }
      else
      {
	 if (allowed_to_remove_unix_socket())
	 {
	    ufdbLogMessage( "socket_handler: first attempt: cannot bind daemon socket: %s (protocol=UNIX)",
                            strerror(errno) );
	    errno = 0;
	    retval = unlink( addr_un.sun_path );
	    if (retval != 0  &&  errno != ENOENT)
	       ufdbLogFatalError( "socket_handler: could not remove '%s': %s",
                                  addr_un.sun_path, strerror(errno) );
	    else
	    {
	       retval = bind( s, (struct sockaddr *) &addr_un, sizeof(addr_un) );
	       if (retval != 0)
		  ufdbLogFatalError( "socket_handler: second attempt: failed to bind socket '%s': %s",
                                     addr_un.sun_path, strerror(errno) );
	       else
	       {
		  (void) chmod( addr_un.sun_path, S_IRUSR|S_IWUSR| S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH );
		  ufdbLogMessage( "UNIX socket \"%s\" successfully created with the second attempt",
                                  addr_un.sun_path );
	       }
	    }
	 }
      }
   }
   else
#endif
   {
      /*
       * Allow server-side addresses to be reused (don't have to wait for timeout).
       */
      sock_parm = 1;
      setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_parm, sizeof(sock_parm) );

      addr_in.sin_family = AF_INET;
      addr_in.sin_port = htons( UFDBglobalPortNum );
      if (strcmp( UFDBglobalInterface, "all" ) == 0)
	 addr_in.sin_addr.s_addr = htonl( INADDR_ANY );
      else
      {
	 struct in_addr iaddr;
	 if (inet_pton( AF_INET, UFDBglobalInterface, &iaddr ) == 0)
	 {
	    addr_in.sin_addr.s_addr = htonl( INADDR_ANY );
	    ufdbLogError( "interface parameter '%s' is invalid. I will listen on port %d on ALL interfaces.",
			  UFDBglobalInterface, UFDBglobalPortNum );
	 }
	 else
	    addr_in.sin_addr.s_addr = iaddr.s_addr;
      }
      /* with anti-aliasing warnings ON, connect/bind cause compiler warnings which we may ignore */
      retval = bind( s, (struct sockaddr *) &addr_in, sizeof(addr_in) );
      if (retval >= 0)
	 ufdbLogMessage( "IP socket port %d on interface \"%s\" successfully created", 
	                 UFDBglobalPortNum, UFDBglobalInterface );
   }

   if (retval < 0)
   {
      ufdbLogFatalError( "cannot bind daemon socket: %s (protocol=%s)", strerror(errno), 
                         protocol==AF_INET ? "IP" : "UNIX" );
      fprintf( stderr, "cannot bind daemon socket: %s (protocol=%s)  *****\n", strerror(errno), 
               protocol==AF_INET ? "IP" : "UNIX" );
      ufdbLogMessage( "check for and kill old daemon processes" );
      fprintf( stderr, "check for and kill old daemon processes\n" );
#if HAVE_UNIX_SOCKETS
      ufdbLogMessage( "and remove UNIX socket file \"%s\"", addr_un.sun_path );
      fprintf( stderr, "and remove UNIX socket file \"%s\"\n", addr_un.sun_path );
#endif
      close( s );
      UFDBchangeStatus( UFDB_API_STATUS_FATAL_ERROR );
      pthread_exit( (void *) 0 );
   }
   
   /*
    * According to a comment in the Apache httpd source code, these socket
    * options should only be set after a successful bind....
    */
   sock_parm = 1;
   setsockopt( s, SOL_SOCKET, SO_KEEPALIVE, (void *) &sock_parm, sizeof(sock_parm) );

   if (protocol == AF_INET)
   {
      sock_parm = 1;
      setsockopt( s, IPPROTO_TCP, TCP_NODELAY, (void *) &sock_parm, sizeof(sock_parm) );
   }

   if (listen( s, UFDB_MAX_THREADS ) < 0)
   {
      ufdbLogFatalError( "cannot listen on daemon socket: %s", strerror(errno) );
      close( s );
      UFDBchangeStatus( UFDB_API_STATUS_FATAL_ERROR );
      pthread_exit( (void *) 0 );
   }

   if (protocol == AF_INET)
      ufdbLogMessage( "listening on interface %s port %d", UFDBglobalInterface, UFDBglobalPortNum );
#if HAVE_UNIX_SOCKETS
   else
      ufdbLogMessage( "listening on UNIX socket \"%s\"", addr_un.sun_path );
#endif

#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
#if HAVE_PTHREAD_RWLOCK_PREFER_WRITER_NP
   ufdbLogMessage( "using rwlock for database locking with preference for \"writer\"" );
#else
   ufdbLogMessage( "using rwlock for database locking but could not set preferred locking for \"writer\"" );
#endif
#else
   ufdbLogMessage( "using mutex for database locking" );
#endif

#ifdef _POSIX_PRIORITY_SCHEDULING
   ufdbLogMessage( "processor yielding is enabled" );
#endif

   ufdbGetSysInfo( &sysinfo );
   num_cpus = ufdbGetNumCPUs();
   ufdbLogMessage( "system: %s %s %s %s on %ld CPU%s", sysinfo.machine, sysinfo.sysname, sysinfo.release, 
                   sysinfo.nodename, num_cpus, num_cpus>1?"s":"" );
   ufdbLogMessage( "ufdbguardd " UFDB_VERSION " started with %d URL verification threads and "
                   "%d TLS/SSL verification threads", 
                   UFDBglobalNworkers, UFDB_NUM_HTTPS_VERIFIERS );
   if (UFDBglobalHttpdPort > 0)
      startHttpDaemon();

   /* let the other threads finish their initialisation before we accept connections */
   usleep( 200000 );
   UFDBglobalReconfig = UFDB_RECONFIGR_NONE;

   daemon_accept_connections( s, protocol );

#if 0
   removePidFile();
#endif
   pthread_exit( NULL );

   return NULL;
}


/*
 * This thread waits and handles the following signals:
 *    HUP, CHLD, USR1, USR2, TERM, INT and ALRM.
 */
UFDB_GCC_HOT
void * signal_handler_thread( void * ptr )
{
   sigset_t        sigset;
   int             sig;

   ufdbSetThreadCPUaffinity( 0 );

   /* The HUP and other signals must be blocked.
    * This is a requirement to use sigwait() in a thread.
    */
   sigemptyset( &sigset );
   sigaddset( &sigset, SIGHUP );
   sigaddset( &sigset, SIGUSR1 );
   sigaddset( &sigset, SIGUSR2 );
   sigaddset( &sigset, SIGCHLD );
   sigaddset( &sigset, SIGTERM );
   sigaddset( &sigset, SIGINT );
   sigaddset( &sigset, SIGALRM );
#if 0
   sigaddset( &sigset, SIGSEGV );	/* blocking SEGV with pthread_sigmask has undefined results */
   sigaddset( &sigset, SIGILL );
   sigaddset( &sigset, SIGBUS );
#endif
   pthread_sigmask( SIG_BLOCK, &sigset, NULL );

   sig = 0;
   while (1)
   {
      int        status;
      pid_t      pid;

      /* do not disturb pclose() and let it do its waitpid() */
      if (sig == SIGCHLD)
         usleep( 100000 );

      while ((pid = waitpid( -1, &status, WNOHANG )) > 0)
      {
	 if (UFDBglobalDebug > 1  &&  WIFEXITED(status))
	    ufdbLogMessage( "signal handler: child process %d terminated with status %d",
                            pid, WEXITSTATUS(status) );
      }

      sig = 0;
      if (0 != sigwait( &sigset, &sig ))
      {
         ufdbLogError( "signal_handler_thread: sigwait() failed with %s", strerror(errno) );
	 continue;
      }

      switch (sig)
      {
      case SIGCHLD:   
         break;
      case SIGTERM:
      case SIGINT:
         TermSignalCaught( sig );
	 break;
      case SIGHUP:
	 HupSignalCaught( sig );
	 break;
      case SIGUSR1:
	 Usr1SignalCaught( sig );
         break;
      case SIGUSR2:
	 Usr2SignalCaught( sig );
         break;
      case SIGALRM:
	 (void) pthread_rwlock_rdlock( &TheDatabaseLock );
	 ufdbHandleAlarmForTimeEvents( UFDB_PARAM_ALARM );
	 (void) pthread_rwlock_unlock( &TheDatabaseLock );
         break;
      default:
         ufdbLogError( "signal handler: unexpected signal %d received *****", sig );
      }
   }

   return NULL;
}


/*
 * A pseudo-random sample of URLs is checked against all known categories
 * and if it appears to be uncategorised, it is buffered to be sent
 * to URLfilterDB at a later date (when SIGHUP is received).
 *
 * return 0 iff the URL is uncategorised, 1 otherwise.
 */
int ufdbVerifyURLallCats( 
   UFDBrevURL *          revURL,
   char *                URL )
{
   struct Category *     cat;
   struct UFDBmemTable * mt;

   if (UFDBglobalReconfig)
      return 1;

   if (strncmp( URL, "function%", 9 ) == 0)
      return 1;

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "ufdbVerifyURLallCats: %s", URL );

   if (UFDBglobalCheckedDB.table.nNextLevels > 0)
   {
      if (UFDBlookupRevUrl( &(UFDBglobalCheckedDB.table.nextLevel[0]), revURL ))
         return 1;
   }
   if (UFDBglobalCheckedExpressions != NULL)
   {
      if (ufdbRegExpMatch( UFDBglobalCheckedExpressions, URL ) == UFDB_API_MATCH)
         return 1;
   }

   for (cat = Cat;  cat != NULL;  cat = cat->next)
   {
      if (UFDBglobalReconfig)
         return 1;
      if (cat->domainlistDb != NULL)
      {
	 mt = (struct UFDBmemTable *) cat->domainlistDb->dbcp;
	 if (UFDBlookupRevUrl( &(mt->table.nextLevel[0]), revURL ))
	    return 1;
      }
      /* we do not use RE matching in ufdbVerifyURLallCats() since we want those URLs to be marked as uncategorised to get in the database tables */
   }

   return 0;
}


static const char * _blockReason(
   int reason )
{
   switch (reason)
   { 
   case UFDB_API_BLOCKR_PASS:              return "PASS";
   case UFDB_API_BLOCKR_ACL:               return "BLOCK";
   case UFDB_API_BLOCKR_ACL_NONE:          return "BLOCK";
   case UFDB_API_BLOCKR_HTTPS_OPTION:      return "BLOCK";
   case UFDB_API_BLOCKR_SKYPE:             return "BLOCK";
   case UFDB_API_BLOCKR_SAFESEARCH:        return "REDIR";
   case UFDB_API_BLOCKR_LOADING_DB:        return "BLOCK-LD";
   case UFDB_API_BLOCKR_FATAL_ERROR:       return "BLOCK-FATAL";
   case UFDB_API_BLOCKR_CHECKED:           return "PASS";
   case UFDB_API_BLOCKR_YOUTUBE_EDUFILTER: return "REDIR";
   }
   return "BLOCK";
}


/*
 * housekeeper thread
 */
UFDB_GCC_HOT
static void * housekeeper_main( void * ptr )
{
   sigset_t                sigset;
   time_t                  t;

   /* The HUP signal must be blocked.
    * This is a requirement to use sigwait() in a thread.
    */
   sigemptyset( &sigset );
   sigaddset( &sigset, SIGHUP );
   sigaddset( &sigset, SIGUSR1 );
   sigaddset( &sigset, SIGUSR2 );
   sigaddset( &sigset, SIGCHLD );
   sigaddset( &sigset, SIGTERM );
   sigaddset( &sigset, SIGINT );
   sigaddset( &sigset, SIGALRM );
#if 0
   sigaddset( &sigset, SIGSEGV );	/* blocking SEGV with pthread_sigmask has undefined results */
   sigaddset( &sigset, SIGILL );
   sigaddset( &sigset, SIGBUS );
#endif
   pthread_sigmask( SIG_BLOCK, &sigset, NULL );

   ufdbSetThreadCPUaffinity( 0 );

   while (1)
   {
      if (UFDBglobalTerminating)
         break;

      pthread_testcancel();

      sleep( 120 );			/* wakeup every 2 minutes */
      t = time( NULL );
      if (lastUpload == 0)
         lastUpload = t;

      if (UFDBglobalTerminating)
         break;

      if (!UFDBglobalReconfig)
      {
	 if (t - lastUpload >= SECSOF2DAYS)
	 {
	    ufdbLogMessage( "No new URL database was loaded in the past 48 hours.  "
                            "It is highly recommended to refresh the URL database.  *****" );
	    if (UFDBglobalDebug)
	       ufdbLogMessage( "housekeeper: generating statistics since it was not done in the past 48 hours" );
	    uploadStatistics( "2DAYS" );
	    UFDBlookupCounter = 0;
	    UFDB_API_num_https = 0;
	    UFDB_API_num_bumps = 0;
	    UFDB_API_num_url_localnet = 0;
	    UFDBblockCounter = 0;
	    UFDBtestBlockCounter = 0;
	    UFDBuncategorisedCounter = 0;
	    UFDBglobalTunnelCounter = 0;
	    UFDBsafesearchCounter = 0;
	    UFDBedufilterCounter = 0;
	    UFDBuploadSeqNo++;
	    if (UFDBlastIPcounterResetDate != 0  &&  t - UFDBlastIPcounterResetDate >= SECSOF8DAYS)
	    {
	       if (UFDBglobalDebug)
		  ufdbLogMessage( "housekeeper: resetting user counters since there was no reset in 8 days" );
	       UFDBinitializeIPcounters();
	       UFDBinitializeUserCounters();
	    }
	 }
	 else
	 if (UFDBglobalCheckedDB.mem != NULL  &&  UFDBglobalCheckedDB.age <= 8  &&  t - lastUpload >= 8*60*60)
	 {
	    struct tm tm;
	    (void) localtime_r( &t, &tm );
	    if (tm.tm_hour == 22  &&  (tm.tm_min>=2 && tm.tm_min<=5))
	    {
	       uploadStatistics( "EOB" );		/* end of business - upload uncategorised URLs */
	       UFDBlookupCounter = 0;
	       UFDB_API_num_https = 0;
	       UFDB_API_num_bumps = 0;
	       UFDB_API_num_url_localnet = 0;
	       UFDBblockCounter = 0;
	       UFDBtestBlockCounter = 0;
	       UFDBuncategorisedCounter = 0;
	       UFDBglobalTunnelCounter = 0;
	       UFDBsafesearchCounter = 0;
	       UFDBedufilterCounter = 0;
	       UFDBuploadSeqNo++;
	    }
	 }
      }
   }

   pthread_exit( NULL );
}


UFDB_GCC_INLINE UFDB_GCC_HOT
static int write_answer_ok(
   int 	              fd, 
   char *             replybuf, 
   struct SquidInfo * si )
{
   char * r;
   char * channelid;

   /* NOTE: the optional channel ID is dealt with by us since 1.32.5 */
   r = replybuf;
   *r = '\0';
   channelid = si->channelid;
   if (*channelid != '\0')
   {
      while (*channelid != '\0')
         *r++ = *channelid++;
      *r++ = ' ';
   }

   if (UFDBglobalSquidHelperProtocol == UFDB_SQUID_HELPER_PROTOCOL3)
   {
      /* strcpy( r, "OK\n" ); */
      *r++ = 'O';
      *r++ = 'K';
      *r++ = '\n';
      *r = '\0';
      /* See also http://wiki.squid-cache.org/Features/AddonHelpers   and
       * http://www.squid-cache.org/Versions/v3/3.4/cfgman/url_rewrite_program.html
       */
   }
   else
   {
      *r++ = '\n';
      *r = '\0';
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "W%03d: sending OK with protocol %d: '%s'", si->worker, UFDBglobalSquidHelperProtocol, replybuf );

   return (int) write( fd, replybuf, strlen(replybuf) );	
}


UFDB_GCC_INLINE UFDB_GCC_HOT
static int write_answer_rewrite( 
   int 	              fd, 
   char *             replyBuf, 
   char *             URL, 
   struct SquidInfo * si )
{
   int                replyLen;

   /* NOTE: the optional channel ID is dealt with by ufdbgclient */

   if (UFDBglobalSquidHelperProtocol == UFDB_SQUID_HELPER_PROTOCOL3)
   {
      if (si->channelid[0] == '\0')
         replyLen = sprintf( replyBuf, "OK rewrite-url=\"%s\"\n", URL );
      else
         replyLen = sprintf( replyBuf, "%s OK rewrite-url=\"%s\"\n", si->channelid, URL );
   }
   else if (UFDBglobalSquidHelperProtocol == UFDB_SQUID_HELPER_PROTOCOL2)
   {
      if (si->channelid[0] == '\0')
         replyLen = sprintf( replyBuf, "%s\n", URL );
      else
         replyLen = sprintf( replyBuf, "%s %s\n", si->channelid, URL );
   }
   else
   {
      if (si->channelid[0] == '\0')
         replyLen = sprintf( replyBuf, "%s %s/%s %s %s\n", 
                             URL, 
                             si->srcIP[0] ? si->srcIP : "-", 
                             si->srcDomain[0] ? si->srcDomain : "-",
                             si->ident[0] ? si->ident : "-",
                             si->method );
      else
         replyLen = sprintf( replyBuf, "%s %s %s/%s %s %s\n", 
                             si->channelid,
                             URL, 
                             si->srcIP[0] ? si->srcIP : "-", 
                             si->srcDomain[0] ? si->srcDomain : "-",
                             si->ident[0] ? si->ident : "-",
                             si->method );
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "W%03d: sending rewrite with protocol %d: '%s'", 
                      si->worker, UFDBglobalSquidHelperProtocol, replyBuf );

   return (int) write( fd, replyBuf, replyLen );
}


UFDB_GCC_INLINE UFDB_GCC_HOT
static int write_answer_redir( 
   int                fd, 
   char *             replyBuf, 
   char *             URL, 
   struct SquidInfo * si )
{
   int                replyLen;

   /* NOTE: the optional channel ID is dealt with by ufdbgclient */

   if (UFDBglobalSquidHelperProtocol == UFDB_SQUID_HELPER_PROTOCOL3)    /* Squid 3.4+ */
   {
      if (si->blockReason == UFDB_API_BLOCKR_SAFESEARCH  ||
          si->blockReason == UFDB_API_BLOCKR_YOUTUBE_EDUFILTER)
      {
         return write_answer_rewrite( fd, replyBuf, URL, si );
      }

      /* the redirection URL may have a "30N:" prefix */
      if (*URL == '3' && *(URL+1) == '0' && (*(URL+2) >= '0' && *(URL+2) <= '9') && *(URL+3) == ':')
      {
         if (si->channelid[0] == '\0')
            replyLen = sprintf( replyBuf, "OK status=%3.3s url=\"%s\"\n", URL, URL+4 );
         else
            replyLen = sprintf( replyBuf, "%s OK status=%3.3s url=\"%s\"\n", si->channelid, URL, URL+4 );
      }
      else
      {
	 if (strcmp( si->method, "CONNECT" ) == 0)
         {
            /* must force rewrite for bumped HTTPS to make Squid reconnect */
            if (si->channelid[0] == '\0')
               replyLen = sprintf( replyBuf, "OK rewrite-url=\"%s\"\n", URL );             
            else
               replyLen = sprintf( replyBuf, "%s OK rewrite-url=\"%s\"\n", si->channelid, URL );             
         }
         else
         {
            if (si->channelid[0] == '\0')
               replyLen = sprintf( replyBuf, "OK status=" UFDB_DEFAULT_HTTP_RETURN_CODE " url=\"%s\"\n", 
                                   URL );
            else
               replyLen = sprintf( replyBuf, "%s OK status=" UFDB_DEFAULT_HTTP_RETURN_CODE " url=\"%s\"\n", 
                                   si->channelid, URL );
         }
      }
   }
   else if (UFDBglobalSquidHelperProtocol == UFDB_SQUID_HELPER_PROTOCOL2)       /* Squid 3.0 - 3.3 */
   {
      if (si->channelid[0] == '\0')
         replyLen = sprintf( replyBuf, "%s\n", URL );
      else
         replyLen = sprintf( replyBuf, "%s %s\n", si->channelid, URL );
   }
   else                                                                         /* Squid 2.x */
   {
      if (si->channelid[0] == '\0')
         replyLen = sprintf( replyBuf, "%s %s/%s %s %s\n", 
                             URL, 
                             si->srcIP[0] ? si->srcIP : "-", 
                             si->srcDomain[0] ? si->srcDomain : "-",
                             si->ident[0] ? si->ident : "-",
                             si->method );
      else
         replyLen = sprintf( replyBuf, "%s %s %s/%s %s %s\n", 
                             si->channelid,
                             URL, 
                             si->srcIP[0] ? si->srcIP : "-", 
                             si->srcDomain[0] ? si->srcDomain : "-",
                             si->ident[0] ? si->ident : "-",
                             si->method );
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "W%03d: sending redirect with protocol %d: '%s'", 
                      si->worker, UFDBglobalSquidHelperProtocol, replyBuf );

   return (int) write( fd, replyBuf, replyLen );
}


/*
 * main of the worker threads.
 */
UFDB_GCC_HOT
static void * worker_main( void * ptr )
{
   sigset_t                sigset;
   UFDBthreadAdmin *       admin;
   int                     tnum;
   int                     fd;
   int                     nbytes;
   int                     isconnect;
   unsigned long           num_reqs;
   struct Source *         src;
   struct Acl *            acl;
   char * 	 	   redirect;
   char *                  line;
   char *                  answerBuf;
   char *                  tmp;
   int                     reconfiguring = -1;
#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
   int                     ret;
   int                     lock_used = 0;
#endif
   char                    cn[1024];
   struct SquidInfo        squidInfo;

   tnum = (int) ((long) ptr);

   /* The HUP signal must be blocked.
    * This is a requirement to use sigwait() in a thread.
    */
   sigemptyset( &sigset );
   sigaddset( &sigset, SIGHUP );
   sigaddset( &sigset, SIGUSR1 );
   sigaddset( &sigset, SIGUSR2 );
   sigaddset( &sigset, SIGCHLD );
   sigaddset( &sigset, SIGTERM );
   sigaddset( &sigset, SIGINT );
   sigaddset( &sigset, SIGALRM );
#if 0
   sigaddset( &sigset, SIGSEGV );	/* blocking SEGV with pthread_sigmask has undefined results */
   sigaddset( &sigset, SIGILL );
   sigaddset( &sigset, SIGBUS );
#endif
   pthread_sigmask( SIG_BLOCK, &sigset, NULL );

   ufdbSetThreadCPUaffinity( tnum );

   admin = UFDBallocThreadAdmin();
   line = (char *) ufdbMallocAligned( 64, UFDB_HTTP_1_1_MAX_URI_LENGTH + 512 );
   *line = '\0';
   workerURLbuffers[tnum] = line;
   tmp = (char *) ufdbMallocAligned( 64, UFDB_HTTP_1_1_MAX_URI_LENGTH + 8192 );
   answerBuf = (char *) ufdbMallocAligned( 64, UFDB_MAX_URL_LENGTH + 8192 );
   fd = -1;

   /* many threads are created, so give some CPU time to others now */
#ifdef _POSIX_PRIORITY_SCHEDULING
   sched_yield();
#else
   usleep( 100000 );
#endif

   while (1)
   {
      if (UFDBglobalTerminating)
         break;

      *line = '\0';
      *tmp = '\0';
      *answerBuf = '\0';

      squidInfo.channelid[0] = '\0';
      squidInfo.protocol[0] = '\0';
      squidInfo.method[0] = '\0';
      squidInfo.revUrl = NULL;
      squidInfo.aclpass = NULL;
      squidInfo.referer[0] = '\0';
      squidInfo.ident[0] = '\0';
      squidInfo.domain[0] = '\0';
      squidInfo.orig_domain[0] = '\0';
      squidInfo.worker = tnum;

      pthread_testcancel();
      getFdQueue( &fd );
      incr_active_workers();
      num_reqs = 0UL;
      if (UFDBglobalDebug > 2)
	 ufdbLogMessage( "W%03d: open fd %2d", tnum, fd );

      while ((nbytes = read( fd, line, UFDB_HTTP_1_1_MAX_URI_LENGTH-2 )) > 0)
      {
	 int decision;

         line[nbytes] = '\0';
	 if (nbytes > UFDB_HTTP_1_1_MAX_URI_LENGTH-2-2)
	 {
            ufdbLogMessage( "W%03d: input line has more than %d bytes and is truncated here", 
                            tnum, UFDB_HTTP_1_1_MAX_URI_LENGTH-2-2 );
	    line[UFDB_MAX_URL_LENGTH-2-2] = '\0';
	 }
	 else if (line[nbytes-1] == '\n')
	 {
	    nbytes--;
	    line[nbytes] = '\0';	/* remove the trailing \n */
	 }

	 pthread_testcancel();

         num_reqs++;
         /* no mutex nor atomic: faster and we do not care about the counter being off by a small number */
	 UFDBlookupCounter++;	

         if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "W%03d: request line=%s", tnum, line );

	 reconfiguring = UFDBglobalReconfig;
	 if (UFDBglobalTerminating)
	    break;

	 if (strncmp( line, "http://qdaemonstatus.urlfilterdb.com", 36 ) == 0)
	 {
	    sprintf( tmp, "http://cgibin.urlfilterdb.com/cgi-bin/URLblocked.cgi?reconfig=%d", reconfiguring );
	    if (write_answer_redir( fd, answerBuf, tmp, &squidInfo ) < 0)
	    {
	       ufdbLogError( "W%03d: write failed: fd=%d %s", tnum, fd, strerror(errno) );
	       goto write_error;
	    }
	    continue;
	 }

	 if (parseLine(admin, line, &squidInfo) != 1) 		/* break down input line into struct squidInfo */
	 {
#if 0
	    for (tmp = line; *tmp != '\0'; tmp++)
	       if (!isprint(*tmp))
	          *tmp = '_';
#endif
	    ufdbLogError( "W%03d: error parsing input line: %s", tnum, line );
	    if (write_answer_ok( fd, answerBuf, &squidInfo ) < 0)			/* TODO: use write_answer_error() */
	    {
	       ufdbLogError( "W%03d: write failed: fd=%d %s", tnum, fd, strerror(errno) );
	       UFDBfreeRevURL( admin, squidInfo.revUrl );
	       goto write_error;
	    }
	    continue;
	 }

	 /* The strstr() is to prevent loops with redirect 302:... ads and other URLs
	  * that are matched by regular expressions.
          * NEVER block these URLs:
	  */
	 if (strstr( squidInfo.orig, "/URLblocked.cgi" ) != NULL  ||
	     strncmp( squidInfo.orig, "blockedhttps.urlfilterdb.com", 28 ) == 0  ||
	     strncmp( squidInfo.orig, "https://blockedhttps.urlfilterdb.com", 36 ) == 0  ||
	     strncmp( squidInfo.orig, "http://cgibin.urlfilterdb.com/", 30 ) == 0)
	 {
	    if (write_answer_ok( fd, answerBuf, &squidInfo ) < 0)
	    {
	       ufdbLogError( "W%03d: write failed: fd=%d %s", tnum, fd, strerror(errno) );
	       UFDBfreeRevURL( admin, squidInfo.revUrl );
	       goto write_error;
	    }
	    continue;
	 }

	 if (squidInfo.srcDomain[0] == '\0') 
	 {
	    squidInfo.srcDomain[0] = '-';
	    squidInfo.srcDomain[1] = '\0';
	 }

	 if (squidInfo.ident[0] == '\0') 
	 {
	    squidInfo.ident[0] = '-';
	    squidInfo.ident[1] = '\0';
	 }

#if UFDB_TEST_CRASH
	 if (strcmp( squidInfo.url, "debug.urlfilterdb.com/debug-thread-crash" ) == 0  && 
	     strcmp( squidInfo.ident, "eviltest" ) == 0)
	 {
	    struct Source * s0;
	    /* test/debug crash handler and ufdb-pstack */
	    s0 = NULL;
	    ufdbLogMessage( "W%03d: before forced SEGV", tnum );
	    s0->name = (char *) "crash-here-with SEGV-signal";
	    ufdbLogError( "W%03d: after forced SEGV", tnum );
	 }
#endif

	 src = NULL;

	 if (reconfiguring == UFDB_RECONFIGR_NONE)
	 {
	    UFDBregisterCountedIP( squidInfo.srcIP );
	    UFDBregisterCountedUser( squidInfo.ident );
	 }

#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
	 if (reconfiguring)
	 {
	    lock_used = 0;
            src = NULL;
	 }
	 else
	 {
	    lock_used = 1;
	    ret = pthread_rwlock_rdlock( &TheDatabaseLock );		/* ================================== */
	    if (ret != 0)
            {
               char errstr[128];

	       ufdbLogFatalError( "W%03d: pthread_rwlock_rdlock failed with code %d", tnum, ret );
               UFDBglobalReconfig = UFDB_RECONFIGR_FATAL;
               UFDBglobalTerminating = 1;
               (void) alarm( 0 );
               badSignal = SIGABRT;
               badSignalThread = pthread_self();
               sprintf( errstr, "pthread_rwlock_rdlock-failed-thread-%d-with-retval-%d", tnum, ret );
               ufdbExecutePstack( errstr );
               exit( 9 );
            }
            if (UFDBglobalReconfig)
            {
               reconfiguring = UFDBglobalReconfig;
               (void) pthread_rwlock_unlock( &TheDatabaseLock );          /* ====== */
               lock_used = 0;
               src = NULL;
            }
            else
               src = UFDBgetSourceList();
	 }
#else
	 if (reconfiguring)
	 {
            src = NULL;
	 }
	 else
	 {
	    src = UFDBgetSourceList();
	 }
#endif

	 for ( ; ; )
	 {
	    const char *    categoryIdent;
	    const char *    ACLident;

	    src = UFDBfindSource( src, &squidInfo );
do_next_src:
	    if (src != NULL)
	       src->nmatches++;			/* no atomic increment: we may loose a few, no problem */
	    acl = UFDBfindACLbySource( src, &squidInfo );       /* returns acl of source or defaultAcl */

	    if (parseOnly)
	    {
	       if (write_answer_ok( fd, answerBuf, &squidInfo ) < 0)
	       {
		  ufdbLogError( "W%03d: write failed: fd=%d %s", tnum, fd, strerror(errno) );
		  UFDBfreeRevURL( admin, squidInfo.revUrl );
		  goto write_error;
	       }
	       break;
	    }

            tmp[0] = '\0';
	    isconnect = (strcmp( squidInfo.method, "CONNECT" ) == 0);
	    redirect = tmp;
            /* >95% is decided here */
	    decision = UFDBdecideAccessBasedOnURL( reconfiguring, acl, &squidInfo, redirect );
	    /* squidInfo.matchedAny may be set here */

	    if (isconnect  &&  UFDBglobalSquidUsesSSLbump)
	       UFDB_API_num_bumps++;

            if (squidInfo.islocalnet)
               UFDB_API_num_url_localnet++;

            /* url_has_ipvN ? */
	    if (decision == UFDB_ACL_ACCESS_DUNNO  &&  (squidInfo.url_has_ipv4 || squidInfo.url_has_ipv6))
	    {
	       if (squidInfo.islocalnet)
               {
                  decision = UFDB_ACL_ACCESS_ALLOW;
                  if (UFDBglobalDebug || UFDBglobalLogPass || UFDBglobalLogAllRequests)
                  {
                     ufdbLogMessage( "W%03d: IP %s is in localnet and is allowed",
                                     squidInfo.worker, squidInfo.domain );
                     strcpy( squidInfo.matchedBy, "local-ip" );
                  }
               }
	       else
	       {
	          char host[1024];
		  if (UFDBglobalLookupReverseIP  &&  
                      UFDBfindCorrectHostNameForIP( squidInfo.orig_domain, host ))
		  {
		     decision = UFDBdecideAccessBasedOnHostname( reconfiguring, acl, &squidInfo, admin, 
                                                                 host, redirect );
                     if (decision == UFDB_ACL_ACCESS_BLOCK)
                     {
                        /* reset matchedAny since most likely it was set in UFDBdecideAccessBasedOnURL() above */
                        squidInfo.matchedAny = 0;
                        if (UFDBglobalDebug || UFDBglobalLogBlock || UFDBglobalLogAllRequests)
                        {
                           ufdbLogMessage( "W%03d: reverse IP of '%s' is '%s' and will be blocked", 
                                           squidInfo.worker, squidInfo.domain, host );
                           strcpy( squidInfo.matchedBy, "reverse-ip" );
                        }
                     }
		  }
	       }
	    }

	    if (decision == UFDB_ACL_ACCESS_DUNNO  && 					/* HTTPS probing */
		!squidInfo.BlockedBumpedConnectAllowed  &&
	        (squidInfo.port == 443  ||  isconnect))
	    {
	       if (UFDBglobalHttpsWithHostname && (squidInfo.url_has_ipv4 || squidInfo.url_has_ipv6) && 
                   !UFDBglobalSquidUsesSSLbump)	      /* TODO: fix this when intercepted CONNECT has FQDN */
	       {
	          decision = UFDB_ACL_ACCESS_BLOCK;
		  squidInfo.blockReason = UFDB_API_BLOCKR_HTTPS_OPTION;
		  if (UFDBglobalDebug)
		     ufdbLogMessage( "%s:%d blocked because enforce-https-with-hostname is on", 
                                     squidInfo.domain, squidInfo.port );
	       }
	       else
	       {
                  if (reconfiguring == UFDB_RECONFIGR_NONE  &&  UFDBglobalReconfig == UFDB_RECONFIGR_NONE  &&
                      isconnect  &&  UFDBglobalTunnelCheckMethod != UFDB_API_HTTPS_CHECK_OFF)
                  {
                     char * content;
                     int    certErrors;
                     int    peekStatus;

                     cn[0] = '\0';
                     content = NULL;
                     certErrors = 0;

                     /* NOTE: we only peek at URLs with CONNECT, so other ports are not peeked (yet) */
                     peekStatus = UFDBsslPeekServer( squidInfo.orig_domain, squidInfo.port, cn, &certErrors,
                                                     &content, squidInfo.worker );
                     if (peekStatus == UFDB_API_OK)
                     {
                        if (certErrors  &&  UFDBglobalHttpsOfficialCertificate)
                        {
                           decision = UFDB_ACL_ACCESS_BLOCK;
                           squidInfo.blockReason = UFDB_API_BLOCKR_HTTPS_OPTION;
                           if (UFDBglobalDebug)
                              ufdbLogMessage( "%s:%d blocked because enforce-https-official-certificate is on", 
                                              squidInfo.orig_domain, squidInfo.port );
                        }
                        /* make decision based on certificate CN iff URL has IP address */
                        else if (!certErrors  &&  (squidInfo.url_has_ipv4 || squidInfo.url_has_ipv6)  &&  
                                 cn[0] != '\0')		
                        {
                           decision = UFDBdecideAccessBasedOnHostname( reconfiguring, acl, &squidInfo, admin, cn, 
                                                                       redirect );
                           if (decision != UFDB_ACL_ACCESS_DUNNO  &&  UFDBglobalDebug)
                              ufdbLogMessage( "%s:%d allowed/blocked because the CN of the TLS/SSL certificate "
                                              "is %s and matched category %s", 
                                              squidInfo.orig_domain, squidInfo.port, cn, squidInfo.aclpass->name );
                        }

#ifdef UFDB_DO_HTTPS_CONTENT_SCANNING
                        if (decision == UFDB_ACL_ACCESS_DUNNO  &&  content != NULL)
                        {
                           /* Note that the result of UFDBfindContentCategory() cannot be cached since a URL 
                            * can be in more than one category and depending on the ACL it may evaluate to 
                            * one or the other.
                            */
                           if (UFDBfindContentCategory( acl, content, &category ))			/* TODO */
                              decision = UFDBdecideAccessBasedOnCategory( acl, &squidInfo, category, redirect );
                        }
#endif

                        if (content != NULL)
                           ufdbFree( content );
                     }
                     else                               /* UFDBsslPeekServer() returned an error */
                     {
                        decision = UFDBdecideAccessByPeekStatus( peekStatus, acl, &squidInfo, redirect );
                        if (decision == UFDB_ACL_ACCESS_BLOCK  &&  UFDBglobalDebug)
                           ufdbLogMessage( "W%03d: %s:%d is blocked because it was probed and "
                                           "status is %s", 
                                           squidInfo.worker, squidInfo.orig_domain, squidInfo.port,
                                           ufdbAPIstatusString(peekStatus) );

                        if (decision == UFDB_ACL_ACCESS_DUNNO  &&  cn[0] != '\0')
                        {
                           /* the certificate has an error AND we retrieved a CN */
                           /* for some CN's we make a decision here */
                           if (strcmp( cn, "*.teamviewer.com" ) == 0)
                           {
                              decision = UFDBglobalTeamviewerOverHttps ? UFDB_ACL_ACCESS_ALLOW : UFDB_ACL_ACCESS_BLOCK;
                              if (decision == UFDB_ACL_ACCESS_BLOCK)
                              {
                                 squidInfo.blockReason = UFDB_API_BLOCKR_HTTPS_OPTION;
                                 if (UFDBglobalDebug)
                                    ufdbLogMessage( "W%03d: %s:%d blocked because the CN of the TLS/SSL certificate "
                                                    "is %s (teamviewer)", 
                                                    squidInfo.worker, squidInfo.orig_domain, 
                                                    squidInfo.port, cn );
                              }
                           }
                        }
                     }
                  }
               }
	    }

            if (decision == UFDB_ACL_ACCESS_DUNNO  &&  !squidInfo.matchedAny  &&  
                src != NULL  &&  src->cont_search  &&  src->next != NULL)
            {
               struct Source * nextsrc;
               nextsrc = UFDBfindSource( src->next, &squidInfo );
               if (nextsrc != NULL)
               { 
                  if (UFDBglobalDebug > 1)
                     ufdbLogMessage( "W%03d: source %s has no match and 'continue' so skipping to source %s", 
                                     tnum, src->name, nextsrc->name );
                  src = nextsrc;
                  goto do_next_src;
               }
            }

	    if (decision == UFDB_ACL_ACCESS_DUNNO  &&  squidInfo.port != 80  &&  
                !squidInfo.BlockedBumpedConnectAllowed)
	    {
	       /* Scanning of implicitpass was skipped to favor calling UFDBsslPeekServer().
	        * Evaluate implicitpass now.
		*/
	       decision = UFDBdecideImplicitAccessBasedOnURL( reconfiguring, acl, &squidInfo, redirect );
	       if (decision != UFDB_ACL_ACCESS_DUNNO  &&  UFDBglobalDebug > 1)
		  ufdbLogMessage( "W%03d: %s:%d is allowed because TLS peek was not decisive and the URL is "
                                  "in implicit category \"%s\"", 
				  tnum, squidInfo.orig_domain, squidInfo.port, squidInfo.aclpass->name );
	    }

	    if (UFDBglobalDebugRedirect)
	    {
	       ufdbLogMessage( "W%03d:   REDIRECT %s %s %s %s", tnum,
			       redirect, 
			       squidInfo.matchedBy[0] == 'h' && squidInfo.srcDomain[0] != '-' && 
                                  squidInfo.srcDomain[1] != '\0' ? 
                                     squidInfo.srcDomain : squidInfo.srcIP,
			       squidInfo.ident, 
			       squidInfo.method );
	    }

	    if (squidInfo.blockReason == UFDB_API_BLOCKR_FATAL_ERROR)
	    {
	       ACLident = "config";
	       categoryIdent = "fatal-error";
	    }
	    else if (squidInfo.blockReason == UFDB_API_BLOCKR_LOADING_DB)
	    {
	       ACLident = "config";
	       categoryIdent = "loading-database";
	    }
	    else if (squidInfo.blockReason == UFDB_API_BLOCKR_HTTPS_OPTION)
	    {
	       ACLident = "config";
	       categoryIdent = "https-option";
	    }
	    else if (squidInfo.blockReason == UFDB_API_BLOCKR_CHECKED)
	    {
	       if (acl == NULL)
		  ACLident = "default";
	       else
		  ACLident = acl->name;
	       categoryIdent = "checked";
	    }
	    else if (squidInfo.blockReason == UFDB_API_BLOCKR_PASS)
	    {
	       if (acl == NULL)
		  ACLident = "default";
	       else
		  ACLident = acl->name;
               if (squidInfo.aclpass != NULL)
               {
                  categoryIdent = squidInfo.aclpass->name;
               }
               else
                  categoryIdent = "Implicit";
	    }
	    else 
	    {
	       if (acl == NULL)
		  ACLident = "acl-null";
	       else
		  ACLident = acl->name;

	       if (UFDBglobalFatalError)
		  categoryIdent = "fatal-error";
	       else if (reconfiguring)
		  categoryIdent = "loading-database";
	       else if (squidInfo.matchedNone)
		  categoryIdent = "none";
	       else if (squidInfo.matchedAny  ||  squidInfo.aclpass == NULL)
	          categoryIdent = "any";
	       else if (squidInfo.aclpass->name == NULL)
		  categoryIdent = "any";
	       else 
		  categoryIdent = squidInfo.aclpass->name;
	    }

	    if (decision == UFDB_ACL_ACCESS_ALLOW  &&  squidInfo.blockReason == UFDB_API_BLOCKR_SAFESEARCH)
	    {
	       if (UFDBglobalLogAllRequests || UFDBglobalLogPass || UFDBglobalDebug > 1 || UFDBglobalDebugRedirect)
	       {
		  ufdbLogMessage( "REDIR %-16s %-15s %-10s %-13s %s %s", 
				  squidInfo.ident, 
				  squidInfo.matchedBy[0] == 'h' && squidInfo.srcDomain[0] != '-' && 
                                     squidInfo.srcDomain[1] != '\0' ? squidInfo.srcDomain : squidInfo.srcIP,
				  ACLident,
				  "SAFESEARCH", 
				  squidInfo.url2log, 
				  squidInfo.method );
	       }

	       if (write_answer_rewrite( fd, answerBuf, squidInfo.orig, &squidInfo ) < 0)
	       {
		  ufdbLogError( "W%03d: write failed for safesearch redirect: fd=%d %s", 
                                tnum, fd, strerror(errno) );
		  UFDBfreeRevURL( admin, squidInfo.revUrl );
		  goto write_error;
	       }
	       break;
	    }
	    else if (decision == UFDB_ACL_ACCESS_DUNNO  ||  decision == UFDB_ACL_ACCESS_ALLOW)
	    {
	       {
                  /* global SafeSearch */
		  if (UFDBglobalSafeSearch  &&
		      UFDBaddSafeSearch( squidInfo.domain, squidInfo.surl, squidInfo.orig ) 
			 == UFDB_API_MODIFIED_FOR_SAFESEARCH)
		  {
		     UFDBsafesearchCounter++;
		     if (UFDBglobalLogAllRequests || UFDBglobalLogPass || UFDBglobalDebugRedirect)
		     {
			ufdbLogMessage( "REDIR %-16s %-15s %-10s %-13s %s %s", 
					squidInfo.ident, 
					squidInfo.matchedBy[0] == 'h' && squidInfo.srcDomain[0] != '-' && 
                                           squidInfo.srcDomain[1] != '\0' ? 
                                              squidInfo.srcDomain : squidInfo.srcIP,
					ACLident,
					"SAFESEARCH", 
					squidInfo.url2log, 
					squidInfo.method );
		     }

		     /* send a REDIRECT to Squid */
		     if (write_answer_rewrite( fd, answerBuf, squidInfo.orig, &squidInfo ) < 0)
		     {
			ufdbLogError( "W%03d: write failed: fd=%d %s", tnum, fd, strerror(errno) );
			UFDBfreeRevURL( admin, squidInfo.revUrl );
			goto write_error;
		     }
		     break;
		  }

		  /* global edufilter; note that there is also a category-specific edufilter  */
		  if ((squidInfo.matchedAny || decision == UFDB_ACL_ACCESS_DUNNO)  &&
		      UFDBglobalYoutubeEdufilter  &&
		      !isconnect  &&
		      UFDBaddYoutubeEdufilter( squidInfo.domain, squidInfo.surl, squidInfo.orig )
		         == UFDB_API_MODIFIED_FOR_YOUTUBE_EDUFILTER)
		  {
		     UFDBedufilterCounter++;
		     if (UFDBglobalLogAllRequests || UFDBglobalLogPass || UFDBglobalDebugRedirect)
		     {
			ufdbLogMessage( "REDIR %-16s %-15s %-10s %-13s %s %s",
					squidInfo.ident, 
					squidInfo.matchedBy[0] == 'h' && squidInfo.srcDomain[0] != '-' && 
                                           squidInfo.srcDomain[1] != '\0' ? squidInfo.srcDomain : squidInfo.srcIP,
					ACLident,
					"EDUFILTER", 
					squidInfo.url2log, 
					squidInfo.method );
		     }

		     /* send a REDIRECT to Squid */
		     if (write_answer_rewrite( fd, answerBuf, squidInfo.orig, &squidInfo ) < 0)
		     {
			ufdbLogError( "W%03d: write failed: fd=%d %s", tnum, fd, strerror(errno) );
			UFDBfreeRevURL( admin, squidInfo.revUrl );
			goto write_error;
		     }
		     break;
		  }

		  if (write_answer_ok( fd, answerBuf, &squidInfo ) < 0)
		  {
		     ufdbLogError( "W%03d: write failed: fd=%d %s", tnum, fd, strerror(errno) );
		     UFDBfreeRevURL( admin, squidInfo.revUrl );
		     goto write_error;
		  }

		  if (UFDBglobalDebug || UFDBglobalLogAllRequests || UFDBglobalLogPass)
		  {
		     if (squidInfo.BlockedBumpedConnectAllowed)
		     {
			ufdbLogMessage( "BPASS %-16s %-15s %-10s %-13s %s %s %s",
					squidInfo.ident, 
					squidInfo.srcIP,
					ACLident, 
					squidInfo.islocalnet && squidInfo.matchedAny ? "localnet" : categoryIdent, 
					squidInfo.url2log, 
					squidInfo.method, 
					isconnect ? squidInfo.sni : "" );
		     }

		     ufdbLogMessage( "PASS  %-16s %-15s %-10s %-13s %s %s %s",
				     squidInfo.ident, 
				     squidInfo.matchedBy[0] == 'h' && squidInfo.srcDomain[0] != '-' && 
                                        squidInfo.srcDomain[1] != '\0' ? squidInfo.srcDomain : squidInfo.srcIP,
				     ACLident, 
				     squidInfo.islocalnet && squidInfo.matchedAny ? "localnet" : categoryIdent, 
				     squidInfo.url2log, 
				     squidInfo.method,
				     isconnect ? squidInfo.sni : "" );
		  }

		  if (!reconfiguring  &&  squidInfo.aclpass != NULL)
		  {
		     struct Category * cat;
		     squidInfo.aclpass->nmatches++;
		     cat = ufdbCategoryFindName( squidInfo.aclpass->name );
		     if (cat != NULL)
		        cat->nmatches++;
		  }

		  /* keep track of uncategorised URLs */
		  if (!reconfiguring  &&  !squidInfo.islocalnet  &&
		      (decision == UFDB_ACL_ACCESS_DUNNO  ||  squidInfo.matchedAny  ||  squidInfo.matchedNone))
		  {
		     if (!ufdbVerifyURLallCats( squidInfo.revUrl, squidInfo.url2log ))		
		     {
			ufdbRegisterUnknownURL( squidInfo.domain, squidInfo.port, squidInfo.referer );
			UFDBuncategorisedCounter++;
		     }
		  }
		  break;
	       }
               break;
	    }
	    else			/* decision == UFDB_ACL_ACCESS_BLOCK */
	    {
	       if (testMode)
	       {
		  ufdbLogMessage( "TEST-%-5s %s %s %s %s %s %s %s",
				  _blockReason(squidInfo.blockReason),
				  squidInfo.ident, 
				  squidInfo.matchedBy[0] == 'h' && squidInfo.srcDomain[0] != '-' && 
                                     squidInfo.srcDomain[1] != '\0' ? squidInfo.srcDomain : squidInfo.srcIP,
				  ACLident,
				  squidInfo.islocalnet && strcmp(categoryIdent,"any")==0 ? "localnet" : categoryIdent, 
				  squidInfo.url2log, 
				  squidInfo.method,
				  isconnect ? squidInfo.sni : "" );
		  if (write_answer_ok( fd, answerBuf, &squidInfo ) < 0)		/* send back an "OK" message */
		  {
		     ufdbLogError( "W%03d: write failed: fd=%d %s", tnum, fd, strerror(errno) );
		     UFDBfreeRevURL( admin, squidInfo.revUrl );
		     goto write_error;
		  }
		  UFDBtestBlockCounter++;
	       }
	       else
	       {
		  if (UFDBglobalLogBlock || UFDBglobalLogAllRequests)
		     ufdbLogMessage( "%-5s %-16s %-15s %-10s %-13s %s %s %s",
				     _blockReason(squidInfo.blockReason),
				     squidInfo.ident, 
				     squidInfo.matchedBy[0] == 'h' && squidInfo.srcDomain[0] != '-' && 
                                        squidInfo.srcDomain[1] != '\0' ? squidInfo.srcDomain : squidInfo.srcIP,
				     ACLident,
				     squidInfo.islocalnet && squidInfo.matchedAny ? "localnet" : categoryIdent, 
				     squidInfo.url2log, 
				     squidInfo.method,
				     isconnect ? squidInfo.sni : "" );
		  if (redirect[0] == '\0')
                  {
                     if (isconnect)
                        strcpy( redirect, UFDBglobalRedirectHttps );
                     else if (squidInfo.port == 443)
                        strcpy( redirect, UFDBglobalRedirectBumpedHttps );
                     else
                        strcpy( redirect, "http://cgibin.urlfilterdb.com/cgi-bin/URLblocked.cgi?"
                                          "category=%t&url=%u" );
                  }
		  if (write_answer_redir( fd, answerBuf, redirect, &squidInfo ) < 0)
		  {
		     ufdbLogError( "W%03d: write failed: fd=%d %s", tnum, fd, strerror(errno) );
		     UFDBfreeRevURL( admin, squidInfo.revUrl );
		     goto write_error;
		  }
		  if (UFDBglobalDebug > 2)
		     ufdbLogMessage( "   %s is redirected to %s", squidInfo.url2log, redirect );
		  UFDBblockCounter++;
		  if (src != NULL)
		     src->nblocks++;
		  if (!reconfiguring &&  squidInfo.aclpass != NULL)
		  {
		     struct Category * cat;
		     squidInfo.aclpass->nblocks++;
		     squidInfo.aclpass->nmatches++;
		     cat = ufdbCategoryFindName( squidInfo.aclpass->name );
		     if (cat != NULL)
		     {
		        cat->nblocks++;
		        cat->nmatches++;
		     }
		  }
	       }
	       break;
	    }
	 }

#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
	 if (lock_used)
	 {
	    ret = pthread_rwlock_unlock( &TheDatabaseLock );
	    if (ret != 0)
            {
               char errstr[128];

	       ufdbLogFatalError( "W%03d: pthread_rwlock_unlock failed with code %d", tnum, ret );
               UFDBglobalReconfig = UFDB_RECONFIGR_FATAL;
               UFDBglobalTerminating = 1;
               (void) alarm( 0 );
               badSignal = SIGABRT;
               badSignalThread = pthread_self();
               sprintf( errstr, "pthread_rwlock_unlock-failed-thread-%d-retval-%d", tnum, ret );
               ufdbExecutePstack( errstr );
               exit( 9 );
            }
	    lock_used = 0;
	 }
#endif

	 *line = '\0';
	 *tmp = '\0';
	 *answerBuf = '\0';
	 UFDBfreeRevURL( admin, squidInfo.revUrl );
	 squidInfo.protocol[0] = '\0';
	 squidInfo.method[0] = '\0';
	 squidInfo.revUrl = NULL;
	 squidInfo.aclpass = NULL;
	 squidInfo.referer[0] = '\0';
	 squidInfo.ident[0] = '\0';

	 /* If configured, all traffic is slowed down artificially by introducing a 0.1 second delay.
	  * The effect of this delay is that
	  * 1) the database reload thread gets the CPU more often.
	  * 2) during the database reload only a few URLs are verified (slowly) which means
	  *    that the users experience a slower internet connection and much less URLs
	  *    are passed through unfiltered.
	  */
         if (reconfiguring)
         {
            if (UFDBglobalURLlookupDelayDBreload)
               usleep( 100000 );
            else
            {
#ifdef _POSIX_PRIORITY_SCHEDULING
               /* the sighup thread must have high priority to obtain the database lock and to load quickly */
	       sched_yield();           
#endif
            }
         }

#ifdef _POSIX_PRIORITY_SCHEDULING
	 if (UFDBhttpsVerificationQueued() > 3)
	 {
	    static volatile int ny = 0;
	    if (++ny > 30)
	    {
	       ny = 0;
	       sched_yield();
	    }
	 }
#endif
	 if (sig_other)
	    break;
      }				/* end of while-read loop */

      if (nbytes < 0  &&  !UFDBglobalTerminating)
      {
         ufdbLogError( "W%03d: read failed: fd=%d %s", tnum, fd, strerror(errno) );
      }
write_error:
      *line = '\0';
      *tmp = '\0';
      *answerBuf = '\0';

      decr_active_workers();

#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
      if (lock_used)
      {
	 ret = pthread_rwlock_unlock( &TheDatabaseLock );
	 if (ret != 0)
         {
            char errstr[128];

            ufdbLogFatalError( "W%03d: pthread_rwlock_unlock after write error failed with code %d", tnum, ret );
            UFDBglobalReconfig = UFDB_RECONFIGR_FATAL;
            UFDBglobalTerminating = 1;
            (void) alarm( 0 );
            badSignal = SIGABRT;
            badSignalThread = pthread_self();
            sprintf( errstr, "pthread_rwlock_unlock-after-write-error-failed-thread-%d-retval-%d", tnum, ret );
            ufdbExecutePstack( errstr );
            exit( 9 );
         }
	 lock_used = 0;
      }
#endif

      if (num_reqs > 1UL)
	 ufdbLogMessage( "W%03d: %lu URL verifications", tnum, num_reqs );

      if (UFDBglobalDebug > 2)
	 ufdbLogMessage( "W%03d: close fd %2d", tnum, fd );
      close( fd );
      fd = -1;
   }

   ufdbFree( line );
   ufdbFree( tmp );
   ufdbFree( answerBuf );
   if (fd >= 0)
      close( fd );

   pthread_exit( NULL );
}


/*
 *  main of thread that deals with dynamic sources 
 *  defined by execuserlist.
 */
static void * dynamic_userlist_updater_main( void * ptr )
{
   sigset_t                sigset;
   struct Source *         src;
   int                     i, n;
   int                     ret;
   char *                  commands[UFDB_MAX_USERLISTS+1];

   sigemptyset( &sigset );
   sigaddset( &sigset, SIGHUP );
   sigaddset( &sigset, SIGUSR1 );
   sigaddset( &sigset, SIGUSR2 );
   sigaddset( &sigset, SIGTERM );
   sigaddset( &sigset, SIGINT );
   sigaddset( &sigset, SIGALRM );
   sigaddset( &sigset, SIGCHLD );
#if 0
   sigaddset( &sigset, SIGSEGV );	/* blocking SEGV with pthread_sigmask has undefined results */
   sigaddset( &sigset, SIGILL );
   sigaddset( &sigset, SIGBUS );
#endif
   pthread_sigmask( SIG_BLOCK, &sigset, NULL );

   ufdbSetThreadCPUaffinity( 0 );

   sleep( 15 );
   goto first_refresh_no_delay;

   while (1)
   {
      struct timespec  tv;

      pthread_testcancel();

      if (UFDBglobalDebug)
	 ufdbLogMessage( "dynamic_userlist_updater_main: sleeping for %d minutes", 
                         UFDBglobalRefreshUserlistInterval );

      errno = 0;
      tv.tv_sec = UFDBglobalRefreshUserlistInterval * 60;
      tv.tv_nsec = 0;
      if (nanosleep( &tv, NULL ) < 0  &&  errno == EINTR)
      {
	 if (tv.tv_sec > UFDBglobalRefreshUserlistInterval * 30)
            (void) nanosleep( &tv, NULL );
      }

first_refresh_no_delay:
      if (UFDBglobalTerminating)
         break;
      if (UFDBglobalReconfig)
         continue;

      if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
	 ufdbLogMessage( "dynamic_userlist_updater_main: going to refresh execuserlists" );

      /* prepare a list of commands to call UFDBrefreshExecUserlist */
      ret = pthread_rwlock_rdlock( &TheDatabaseLock );
      if (ret != 0)
      {
	 ufdbLogError( "dynamic_userlist_updater_main: pthread_rwlock_rdlock rdlock failed with code %d  *****",
                       ret );
         continue;
      }

      if (UFDBglobalReconfig)
      {
         pthread_rwlock_unlock( &TheDatabaseLock );
         continue;
      }

      n = 0;
      src = UFDBgetSourceList();
      while (src != NULL)
      {
         if (src->active  &&  src->userDb != NULL  &&  src->userDb->type == SGDBTYPE_EXECUSERLIST)
	 {
	    commands[n] = ufdbStrdup( src->sarg0 );
	    ++n;
	 }
         src = src->next;
      }
      commands[n] = NULL;

      ret = pthread_rwlock_unlock( &TheDatabaseLock );
      if (ret != 0)
	 ufdbLogError( "dynamic_userlist_updater_main: error releasing database rdlock: %d  *****", ret );

      /* call UFDBrefreshExecUserlist for each command */
      for (i = 0;  i < n;  ++i)
      {
	 if (UFDBglobalTerminating)
	    break;
	 if (UFDBglobalReconfig)
	 {
	    ufdbLogMessage( "dynamic_userlist_updater_main: NOT refreshing execuserlist for \"%s\" since "
                            "the whole configuration is being reloaded",
                            commands[i] );
	    continue;			/* continue here to print warnings for all commands */
	 }
	 
	 if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
	    ufdbLogMessage( "dynamic_userlist_updater_main: refreshing execuserlist for \"%s\"", commands[i] );

	 UFDBrefreshExecUserlist( commands[i] );	/* NOTE: this may take a long time in some environments */
	 
	 if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
	    ufdbLogMessage( "dynamic_userlist_updater_main: refresh completed for \"%s\"", commands[i] );
	 ufdbFree( commands[i] );
      }

      if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
	 ufdbLogMessage( "dynamic_userlist_updater_main: refresh completed for all execuserlists "
                         "(%d userlists)", n );
   }

   /*NOTREACHED*/
   pthread_exit( NULL );
}


/*
 *  main of thread that deals with dynamic categories
 *  defined by execdomainlist.
 */
static void * dynamic_domainlist_updater_main( void * ptr )
{
   sigset_t                sigset;
   struct Category *       clist;
   int                     ret;
   int                     n;

   sigemptyset( &sigset );
   sigaddset( &sigset, SIGHUP );
   sigaddset( &sigset, SIGUSR1 );
   sigaddset( &sigset, SIGUSR2 );
   sigaddset( &sigset, SIGTERM );
   sigaddset( &sigset, SIGINT );
   sigaddset( &sigset, SIGALRM );
   sigaddset( &sigset, SIGCHLD );
#if 0
   sigaddset( &sigset, SIGSEGV );	/* blocking SEGV with pthread_sigmask has undefined results */
   sigaddset( &sigset, SIGILL );
   sigaddset( &sigset, SIGBUS );
#endif
   pthread_sigmask( SIG_BLOCK, &sigset, NULL );

   ufdbSetThreadCPUaffinity( 0 );

   while (1)
   {
      struct timespec  tv;

      pthread_testcancel();

      if (UFDBglobalDebug)
	 ufdbLogMessage( "dynamic_domainlist_updater_main: sleeping for %d minutes", 
                         UFDBglobalRefreshDomainlistInterval );

      tv.tv_sec = UFDBglobalRefreshDomainlistInterval * 60;
      tv.tv_nsec = 0;
      (void) nanosleep( &tv, NULL );

      if (UFDBglobalTerminating)
         break;
      if (UFDBglobalReconfig)
         continue;
      
      if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
	 ufdbLogMessage( "dynamic_domainlist_updater_main: going to refresh execdomainlists" );

      ret = pthread_rwlock_rdlock( &TheDatabaseLock );          /* >>====================================== */
      if (ret != 0)
      {
	 ufdbLogError( "dynamic_domainlist_updater_main: pthread_rwlock_rdlock database rdlock failed "
                       "with code %d  *****", ret );
         continue;
      }

      n = 0;
      for (clist = Cat;  clist != NULL;  clist = clist->next)
      {
         if (clist->active  &&  clist->execdomainlist != NULL)
	 {
	    struct sgDb * oldddb;
	    struct sgDb * newddb;

	    if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
	       ufdbLogMessage( "dynamic_domainlist_updater_main: refreshing execdomainlist for %s (%s)", 
	                       clist->name, clist->execdomainlist );

	    newddb = UFDBretrieveExecDomainlist( clist );
	    if (newddb == NULL)
	    {
	       if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
		  ufdbLogMessage( "refresh execdomainlist for %s presented no new domainlist", clist->name );
	       continue;
	    }
	    
	    ret = pthread_rwlock_wrlock( &TheDynamicCategoriesLock );
	    if (ret != 0)
	       ufdbLogError( "dynamic_domainlist_updater_main: pthread_rwlock_wrlock TheDynamicCategoriesLock "
                             "failed with code %d  *****", ret );

	    oldddb = clist->domainlistDb;
            clist->domainlistDb = newddb;
	    n++;

	    ret = pthread_rwlock_unlock( &TheDynamicCategoriesLock );
	    if (ret != 0)
	       ufdbLogError( "dynamic_domainlist_updater_main: pthread_rwlock_unlock TheDynamicCategoriesLock "
                             "failed with code %d  *****", ret );

	    ufdbFreeDomainDb( oldddb );

	    if (UFDBglobalDebug > 1 || UFDBglobalDebugExternalScripts)
	       ufdbLogMessage( "dynamic_domainlist_updater_main: refresh execdomainlist for \"%s\" completed",
	                       clist->name );
	 }
      }

      ret = pthread_rwlock_unlock( &TheDatabaseLock );          /* <<====================================== */
      if (ret != 0)
	 ufdbLogError( "dynamic_domainlist_updater_main: error releasing database rdlock: %d  *****", ret );

      if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
	 ufdbLogMessage( "dynamic_domainlist_updater_main: refresh completed for all execdomainlists "
                         "(%d domainlists)", n );
   }

   /*NOTREACHED*/
   pthread_exit( NULL );
}


void doCommand( char * command )
{
   /* There is a serious problem sending signals to ufdbguardd and ufdbhttpd.
    * 1) ufdbguardd cannot send SIGTERM, but can send SIGKILL to ufdbhttpd.
    * 2) ufdbUpdate cannot send SIGHUP to ufdbguardd.
    */

   if (strcmp( command, "verify" ) == 0)
   {
      testConfig = 1;
      runAsDaemon = 0;
      UFDBglobalUploadStats = 0;
      globalErrorLog = stderr;
   }
}


extern char ** environ;

int main( 
   int            argc,
   char **        argv )
{
   int            i;
   pid_t          pid;
   sigset_t       sigset;
   pthread_attr_t attr;
   time_t 	  t;
   void *         dummy_ptr;
   struct rlimit  openfiles;
   char           niso_str[22];

   char ** envp = environ;

#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
   pthread_rwlockattr_t mylock_attr;
#endif

   strcpy( UFDBprogname, "ufdbguardd" );

   /* TO-DO: unset LC variables; regex cannot deal with multibyte characters */

   while ((i = getopt(argc, argv, "AC:TNdDRPSh?rt:c:L:p:U:vw:")) != EOF)
   {
      switch (i) {
      case 'A':
	 fprintf( stderr, "-A option is obsolete\n");
	 break;
      case 'C':
	 doCommand( optarg );
         break;
      case 'N':
	 fprintf( stderr, "-N option is obsolete.\n" );
	 break;
      case 'D':
	 runAsDaemon = 0;	/* undocumented -D option to prevent running as daemon */
	 break;
      case 'd':
	 UFDBglobalDebug++;
#if UFDB_DEBUG_YY
	 yydebug = 1;
#endif
	 break;
      case 'c':
	 configFile = optarg;
	 break;
      case 'p':
      	 portNumCmdLine = atoi( optarg );
	 if (portNumCmdLine <= 0) {
	    fprintf( stderr, "port number must be > 0\n" );
	    exit( 1 );
	 }
	 break;
      case 'P':			/* undocumented -P option for development purposes */
	 parseOnly = 1;
	 break;
      case 'R':
         UFDBglobalDebugRegexp = 1;
	 break;
      case 'S':
         UFDBglobalUploadStats = 0;
	 break;
      case 'r':
	 UFDBglobalDebugRedirect = 1;
	 break;
      case 't':			/* undocumented -t option for time-related testing */
	 if ((t = iso2sec(optarg)) == -1) {
	    fprintf( stderr, "-t dateformat error, should be yyyy-mm-ddTHH:MM:SS\n" );
	    exit( 1 );
	 }
	 if (t < 0) {
	    fprintf( stderr, "-t date have to after 1970-01-01T01:00:00\n" );
	    exit( 1 );
	 }
	 niso( t, niso_str );
	 ufdbLogMessage( "ufdbguardd emulating date %s", niso_str );
	 globalDebugTimeDelta = t - start_time.tv_sec;
	 start_time.tv_sec = start_time.tv_sec + globalDebugTimeDelta;
	 break;
      case 'T':
	 testMode = 1;		
	 break;
      case 'U':
	 if (strlen(optarg) <= 31)
	    strcpy( UFDBglobalUserName, optarg );
	 else
	    ufdbLogFatalError( "username supplied with -U option is too long" );
         break;
      case 'L':				/* TODO: remove undocumented -L option for alternate PID file */
	 UFDBglobalPidFile = ufdbStrdup( optarg );
         break;
      case 'v':
	 fprintf( stderr, "ufdbguardd %s\n", UFDB_VERSION );
	 fprintf( stderr, "The ufdbGuard software suite is Free and Open Source Software.\n" );
	 fprintf( stderr, "Copyright (C) 2005-2018 by URLfilterDB B.V. and others.\n" );
	 exit( 0 );
	 break;
      case 'w':
         UFDBglobalNworkers = atoi( optarg );
	 if (UFDBglobalNworkers < 65)
	    UFDBglobalNworkers = 65;
	 else if (UFDBglobalNworkers > UFDB_MAX_WORKERS)
	 {
	    fprintf( stderr, "-w option exceeds the maximum.  The number of threads is set to %d\n", 
                     UFDB_MAX_WORKERS );
	    UFDBglobalNworkers = UFDB_MAX_WORKERS;
	 }
	 break;
      case '?':
      case 'h':
         usage( '\0' );
	 break;
      default:
         usage( i );
      }
   }

   ufdbSetGlobalErrorLogFile( 0 );

   if (runAsDaemon)
   {
      if ((pid = fork()) != 0)		/* the parent exits */
      {
	 /* systemd likes to see the pid file when we exit but since the child process
	  * creates the pid file (rather late) we will busy-wait for the creation of the pid file
	  * with a maximum of 10 seconds.
          * Note that the pid file may have an alternative name and the busy-wait waits too long...
	  */
	 for (i = 0; i < 10; i++)
	 {
	    struct stat stbuf;
	    sleep( 1 );			
	    errno = 0;
	    if (stat( UFDBGUARDD_PID_FILE, &stbuf ) == 0)
	       break;
	 }
	 exit( 0 );        
      }
#ifndef UFDB_DEBUG
      close( 0 );
      close( 1 );
#endif
      setsid();		/* TODO: is setsid necessary when called from systemd ? */
   }
   globalPid = getpid();

   ufdbSetGlobalErrorLogFile( 0 );

   /* currently UFDB_MAX_THREADS is 1285 */
   openfiles.rlim_cur = UFDB_MAX_THREADS * 2 + 1;
   openfiles.rlim_max = UFDB_MAX_THREADS * 4 + 1;
   i = setrlimit( RLIMIT_NOFILE, &openfiles );
   if (i != 0)
      ufdbLogError( "cannot set maximum number of open files to %ld: %s  *****", 
                    (long) openfiles.rlim_cur, strerror(errno) );

   UFDBdropPrivileges( UFDBglobalUserName );

   /*
    * set reconfig immediately since during startup the worker threads must know that there is no database yet.
    */
   UFDBglobalReconfig = UFDB_RECONFIGR_INIT;

   UFDBinitializeIPcounters();
   UFDBinitializeUserCounters();

   /*
    * Initialise signal handlers.
    * The HUP signal is dealt with on a per thread basis.
    */
#if !UFDB_PRODUCE_CORE_DUMPS
   ufdbSetSignalHandler( SIGILL,  ufdbCatchBadSignal );
   ufdbSetSignalHandler( SIGBUS,  ufdbCatchBadSignal );
   ufdbSetSignalHandler( SIGSEGV, ufdbCatchBadSignal );
   ufdbSetSignalHandler( SIGABRT, catchAbortSignal );
#endif

   ufdbSetSignalHandler( SIGPIPE, SIG_IGN );

   ufdbSetSignalHandler( SIGTERM, catchTermSignal );
   ufdbSetSignalHandler( SIGINT,  catchTermSignal );

   ufdbSetSignalHandler( SIGHUP,  catchHUPSignal );
   ufdbSetSignalHandler( SIGUSR1, catchUSR1Signal );
   ufdbSetSignalHandler( SIGUSR2, catchUSR2Signal );

   ufdbSetSignalHandler( SIGCHLD, catchChildSignal );

   /* All signals must be blocked.
    * This is a requirement to use sigwait() in a thread.
    */
   sigemptyset( &sigset );
   sigaddset( &sigset, SIGHUP );
   sigaddset( &sigset, SIGUSR1 );
   sigaddset( &sigset, SIGUSR2 );
   sigaddset( &sigset, SIGCHLD );
   sigaddset( &sigset, SIGTERM );
   sigaddset( &sigset, SIGINT );
   sigaddset( &sigset, SIGALRM );
#if 0
   sigaddset( &sigset, SIGILL );
   sigaddset( &sigset, SIGBUS );
   sigaddset( &sigset, SIGSEGV );	/* blocking SEGV with pthread_sigmask has undefined results */
#endif
   pthread_sigmask( SIG_BLOCK, &sigset, NULL );

   my_argc = argc;
   my_argv = argv;
   my_envp = envp;

   if (testConfig)
   {
      UFDBglobalFatalError = 0;
      UFDBglobalHttpdPort = 0;
      if (sgReadConfig( configFile ) == 0)
      {
         ufdbLogFatalError( "missing configuration file" );
	 exit( 3 );
      }
      if (UFDBglobalFatalError)
      {
	 ufdbLogMessage( "A FATAL ERROR OCCURRED: (see *previous* lines with \"FATAL ERROR\" "
                         "for more details)  *****" );
	 exit( 1 );
      }
      exit( 0 );
   }

   pthread_rwlockattr_init( &mylock_attr );
#if HAVE_PTHREAD_RWLOCK_PREFER_WRITER_NP
   pthread_rwlockattr_setkind_np( &mylock_attr, PTHREAD_RWLOCK_PREFER_WRITER_NP );
#else
   ufdbLogMessage( "WARNING: could not set writer lock preference for the database lock.  *****\n"
                   "This may delay processing updates and loading a new configuration.  *****" );
#endif
   pthread_rwlock_init( &TheDatabaseLock, &mylock_attr );

   UFDBaclEngineInit();

#if 0
   /* initialise TLS/SSL */
   UFDBinitHTTPSchecker();
#endif

   /*
    * create the threads.
    */
   pthread_attr_init( &attr );
   pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
#if HAVE_PTHREAD_ATTR_SETGUARDSIZE && UFDB_DEBUG
   pthread_attr_setstacksize( &attr, (400-8) * 1024 );
   pthread_attr_setguardsize( &attr, 8 * 1024 );
#else
   pthread_attr_setstacksize( &attr, 400 * 1024 );
#endif

   pthread_create( &threadedSignalHandler, &attr, signal_handler_thread, (void *) 0 );
   /* sleep a bit since we really want that signal_handler_thread does its initialisation before going ahead */
   usleep( 100000 );

   for (i = 0; i < UFDBglobalNworkers; i++)
   {
      pthread_create( &workers[i], &attr, worker_main, (void *) ((long) i) );
      if (i % 4 == 0)
      {
#ifdef _POSIX_PRIORITY_SCHEDULING
	 sched_yield();
#else
	 usleep( 10000 );
#endif
      }
   }
   usleep( 100000 );
   ufdbLogMessage( "%d worker threads created.", UFDBglobalNworkers );

   pthread_create( &dyn_userlist_handler, &attr, dynamic_userlist_updater_main, (void *) 0 );
   pthread_create( &dyn_domainlist_handler, &attr, dynamic_domainlist_updater_main, (void *) 0 );
   pthread_create( &housekeeper, &attr, housekeeper_main, (void *) 0 );

   /* socket_handler_thread() :
    *  write the pid file
    *  create HTTPS verification threads
    *  read the config file
    *  increase number of worker threads if necessary
    *  listen on the socket to pass FDs to workers
    */
   pthread_create( &sockethandler, &attr, socket_handler_thread, (void *) 0 );

   /*
    * and finally we have some time to upload crash reports.
    */
   sleep( 1 );
   upload_crash_reports();

   /*
    * prevent an immediate exit.
    */
   sleep( 2 );

   /*
    * exit when the sockethandler thread terminates.
    */
   pthread_join( sockethandler, &dummy_ptr );

   if (UFDBglobalDebug)
      ufdbLogMessage( "Socket handler stopped. ufdbguardd is going to stop." );

   /*
    *  Note: sockethandler terminated because of a signal.
    */
   UFDBglobalReconfig = UFDB_RECONFIGR_FINISH;
   UFDBglobalTerminating = 1;
   alarm( 0 );

   if (badSignal)
   {
      ufdbLogMessage( "main: bad signal %d was caught", badSignal );
      removePidFile();
      badSignalHandlerBusy = 1;
      BadSignalCaught( badSignal );
      while (badSignalHandlerBusy)
         usleep( 150000 );
   }

   ufdbLogMessage( "terminating connections with clients" );
   UFDBcloseFilesNoLog();

   if (UFDBglobalDebug)
   {
      ufdbLogMessage( "cancelling threads ..." );
      pthread_cancel( housekeeper );
      pthread_cancel( dyn_userlist_handler );
      pthread_cancel( dyn_domainlist_handler );
      for (i = 0; i < UFDB_NUM_HTTPS_VERIFIERS; i++)
	 pthread_cancel( httpsthr[i] );
#ifdef _POSIX_PRIORITY_SCHEDULING
      sched_yield();
#endif
      for (i = 0; i < UFDBglobalNworkers; i++)
	 pthread_cancel( workers[i] );
#ifdef _POSIX_PRIORITY_SCHEDULING
      sched_yield();
#endif
   }

   removePidFile();
   UFDBchangeStatus( UFDB_API_STATUS_TERMINATED );
   ufdbLogMessage( "ufdbGuard daemon stopped" );

   if (!badSignal  &&  (UFDBglobalDebug > 1  ||  getenv( "UFDB_FREE_MEMORY_ON_EXIT" ) != NULL))
   {
      usleep( 250000 );
      ufdbLogMessage( "freeing all memory ..." );
      ufdbFreeAllMemory();
      ufdbLogMessage( "done freeing memory." );
   }

   _exit( 0 );

   return 0;
}


static char * makeBlockedCategoriesString( void )
{
   struct Category * cat;
   int               sz;
   static char       blocked[48001];	/* only one thread will call us so we may return a static array */

   sz = 0;
   blocked[0] = '\0';
   for (cat = Cat;  cat != NULL;  cat = cat->next)
   {
      sz += sprintf( &blocked[sz], "%s:%lu/%lu ", cat->name, cat->nmatches, cat->nblocks );
      if (sz > 47000)
      {
         strcat( &blocked[sz], " ..." );
	 break;
      }
   }

   return blocked;
}


static void uploadStatistics( const char * reason )
{
   char * URLs;
   char * message;
   int    length;
   int    s;
   int    port;
   SSL *  ssl;
   int    sslStatus;
   int    nbytes;
   int    written;
   int    dummy;
   int    status;
   int    onehour;
   long   num_cpus;
   unsigned long  num_clients;
   unsigned long  num_users;
   struct utsname sysinfo;
   time_t now;

   now = time( NULL );
   /* do not upload stats more than once per 19 minutes, except when debugging */
   if (UFDBglobalDebug == 0  &&  lastUpload != 0  &&  (now - lastUpload) < 19*60)
   {
      if (strcmp( reason, "HUP" ) == 0)
	 return;
   }

   onehour = (strcmp( reason, "1HR" ) == 0);
   if (onehour)
   {
      if (UFDBglobalCheckedDB.mem == NULL  ||  !UFDBglobalUploadStats)
	 return;
      if (UFDBglobalReconfig  ||  UFDBglobalTerminating)
         return;
   }

   if (!onehour)
      logStatistics( reason );

   lastUpload = now;

   num_clients = UFDBgetNumberOfRegisteredIPs();
   num_users = UFDBgetNumberOfRegisteredUsers();
   if (UFDBglobalCheckedDB.mem == NULL)		/* not a client of URLfilterDB */
   {
      if (UFDBlookupCounter < 200)
	 return;
      if (num_clients == 0)
         return;
   }

   if (UFDBglobalDebug)
      ufdbLogMessage( "uploadStatistics: upload uncategorised URLs: %s,  upload statistics: %s", 
                      UFDBglobalAnalyseUncategorisedURLs ? "yes" : "no",
		      UFDBglobalUploadStats ? "yes" : "no" );

   if (UFDBglobalAnalyseUncategorisedURLs)
   {
      URLs = ufdbGetUnknownURLs();
      if (URLs == NULL)
      {
         if (onehour)
	    return;
	 URLs = (char *) "example.com|";
      }
   }
   else
   {
      if (onehour)
         return;
      URLs = (char *) "example.com|";
   }

   if (UFDBglobalLogUncategorisedURLs)
   {
      if (!UFDBglobalAnalyseUncategorisedURLs)
	 ufdbLogError( "log-uncategorised-urls is ON but analyse-uncategorised-urls is OFF and therefore "
                       "there are no uncategorised URLs to be logged" );
      else if (UFDBuncategorisedCounter == 0)
      {
         ufdbLogMessage( "there are no uncategorised URLs" );
      }
      else
      {
	 char * u;
	 u = URLs;
         if (*u == '|'  &&  *(u+1) == 'N'  &&  *(u+2) <= '9'  && *(u+3) == '|')
            u += 4;
	 if (*u == '|')
	    u++;
         while (*u != '\0')
	 {
	    char * sep;
	    char * hash;
	    int    len;
	    sep = strchr( u, '|' );
	    if (sep == NULL)
	       break;
	    hash = strchr( u, '#' );
	    if (hash == NULL)
	       len = (int) (sep - u);
	    else
	       len = (int) (hash - u);
	    ufdbLogMessage( "uncategorised URL: %*.*s", len, len, u );
	    u = sep + 1;
	 }
      }
   }

   /* TO-DO: the length is probably 2 too less because of the trailing \r\n. investigate this */
   length = strlen( URLs );
   if (length <= 1)
   {
      if (onehour)
         return;
      URLs = (char *) "example.com|";
      length = strlen( URLs );
   }

   message = (char *) ufdbMalloc( 1500 + length + 48000 );
   if (message == NULL)
   {
      ufdbLogError( "could not malloc %d bytes for upload message. Aborting upload of uncategorised URLs",
                    1500 + length + 48000 );
      ufdbResetUnknownURLs();
      return;
   }

   ufdbGetSysInfo( &sysinfo );
   num_cpus = ufdbGetNumCPUs();

   if (UFDBglobalUploadStats)
   {
      if (onehour)
	 sprintf( message, 
		  "POST /cgi-bin/uncat.pl HTTP/1.1\r\n"
		  "Host: " UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE "\r\n"
		  "User-Agent: ufdbGuardd-" UFDB_VERSION "\r\n"
		  "Content-Type: text/plain\r\n"
		  "Content-Length: %d\r\n"
		  "Connection: close\r\n"
		  "X-HasURLfilterDB: %s\r\n"
		  "X-DatabaseDate: %s\r\n"
		  "X-SiteInfo: %s %ld %s %s\r\n"
		  "X-NodeName: %s\r\n"
		  "X-SquidVersion: %s\r\n"
		  "X-SquidUsesBumping: %s\r\n"
		  "X-NumClients: %lu\r\n"
		  "X-NumUsers: %lu\r\n"
		  "X-NumThreads: %d\r\n"
		  "X-UploadSeqNo: %lu\r\n"
		  "X-UploadReason: %s\r\n"
		  "\r\n"
		  "%s\r\n",
		  length, 
		  "yes",
		  UFDBglobalCheckedDB.date,
		  sysinfo.machine, num_cpus, sysinfo.sysname, sysinfo.release, 
		  sysinfo.nodename,
		  UFDBglobalSquidVersion,
		  UFDBglobalSquidUsesSSLbump ? "yes" : "no",
		  num_clients,
		  num_users,
		  UFDBglobalNworkers,
		  UFDBuploadSeqNo,
		  reason,
		  URLs );
      else
	 sprintf( message, 
		  "POST /cgi-bin/uncat.pl HTTP/1.1\r\n"
		  "Host: " UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE "\r\n"
		  "User-Agent: ufdbGuardd-" UFDB_VERSION "\r\n"
		  "Content-Type: text/plain\r\n"
		  "Content-Length: %d\r\n"
		  "Connection: close\r\n"
		  "X-HasURLfilterDB: %s\r\n"
		  "X-DatabaseDate: %s\r\n"
		  "X-SiteInfo: %s %ld %s %s\r\n"
		  "X-NodeName: %s\r\n"
		  "X-SquidVersion: %s\r\n"
		  "X-SquidUsesBumping: %s\r\n"
		  "X-NumClients: %lu\r\n"
		  "X-NumUsers: %lu\r\n"
		  "X-NumThreads: %d\r\n"
		  "X-NumLookups: %lu\r\n"
		  "X-NumHttpsLookups: %lu\r\n"
		  "X-NumBumps: %lu\r\n"
		  "X-Localnet: %lu\r\n"
		  "X-NumTunnelsDetected: %lu\r\n"
		  "X-NumTestBlock: %lu\r\n"
		  "X-NumBlock: %lu\r\n"
		  "X-NumSafeSearch: %lu\r\n"
		  "X-NumEduFilter: %lu\r\n"
		  "X-Uncategorised: %lu\r\n"
		  "X-BlockedCategories: %s\r\n"
		  "X-UploadSeqNo: %lu\r\n"
		  "X-UploadReason: %s\r\n"
		  "\r\n"
		  "%s\r\n",
		  length, 
		  (UFDBglobalCheckedDB.mem != NULL && UFDBglobalCheckedDB.age <= 10 ? "yes" : "no"),
		  (UFDBglobalCheckedDB.mem == NULL ? "void" : UFDBglobalCheckedDB.date),
		  sysinfo.machine, num_cpus, sysinfo.sysname, sysinfo.release, 
		  sysinfo.nodename,
		  UFDBglobalSquidVersion,
		  UFDBglobalSquidUsesSSLbump ? "yes" : "no",
		  num_clients,
		  num_users,
		  UFDBglobalNworkers,
		  UFDBlookupCounter,
		  UFDB_API_num_https,
		  UFDB_API_num_bumps,
		  UFDB_API_num_url_localnet,
		  UFDBglobalTunnelCounter,
		  UFDBtestBlockCounter,
		  UFDBblockCounter,
		  UFDBsafesearchCounter,
		  UFDBedufilterCounter,
		  UFDBuncategorisedCounter,
		  makeBlockedCategoriesString(),
		  UFDBuploadSeqNo,
		  reason,
		  URLs );
   }
   else
   {
      sprintf( message, 
	       "POST /cgi-bin/uncat.pl HTTP/1.1\r\n"
	       "Host: " UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE "\r\n"
	       "User-Agent: ufdbGuardd-" UFDB_VERSION "\r\n"
	       "Content-Type: text/plain\r\n"
	       "Content-Length: %d\r\n"
	       "X-HasURLfilterDB: %s\r\n"
	       "X-DatabaseDate: %s\r\n"
	       "X-SiteInfo: %s %ld %s %s\r\n"
	       "X-NodeName: %s\r\n"
	       "X-SquidVersion: %s\r\n"
	       "X-NumClients: %lu\r\n"
	       "X-NumUsers: %lu\r\n"
	       "X-NumThreads: %d\r\n"
	       "X-NumLookups: %lu\r\n"
	       "X-UploadSeqNo: %lu\r\n"
	       "X-UploadReason: %s\r\n"
	       "\r\n",
	       length,
	       (UFDBglobalCheckedDB.mem != NULL && UFDBglobalCheckedDB.age <= 10 ? "yes" : "no"),
	       (UFDBglobalCheckedDB.mem == NULL ? "void" : UFDBglobalCheckedDB.date),
	       sysinfo.machine, num_cpus, sysinfo.sysname, sysinfo.release, 
	       sysinfo.nodename,
	       UFDBglobalSquidVersion,
	       num_clients,
	       num_users,
	       UFDBglobalNworkers,
	       UFDBlookupCounter,
	       UFDBuploadSeqNo,
	       reason  );
   }
   length = strlen( message );

   /* This function migth be called N times when a bad signal is received.
    * Let's try to execute the upload only once by resetting the unknown URLs quickly.
    */
   ufdbResetUnknownURLs();

   ssl = NULL;
   port = 443;
   s = UFDBopenSocket( UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, port );
   if (s >= 0)
   {
      sslStatus = UFDBopenssl_connect( UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, port, s, &ssl );
      if (sslStatus != UFDB_API_OK)
      {
         ufdbLogError( "SSL/TLS handshake with %s:443 failed", UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE );
	 close( s );
	 s = -1;
      }
   }
   else if (UFDBglobalDebug)
      ufdbLogMessage( "cannot connect to %s:443  trying %s:80", 
                      UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE );

   if (s < 0)
   {
      port = 80;
      s = UFDBopenSocket( UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, port );
      if (s < 0)
      {
	 ufdbLogError( "cannot upload statistics and uncategorised URLs  *****\n"
		       "cannot open a communication socket to %s:443 nor %s:80 (%s)",
		       UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE,
                       strerror(errno) );
	 return;
      }
   }

   written = 0;
   while (length > 0)
   {
      if (port == 443)
	 nbytes = UFDBopenssl_write( message+written, length, ssl );
      else
	 nbytes = write( s, message+written, length );
      if (nbytes == 0)
         break;
      if (nbytes < 0)
      {
         if (errno != EINTR)
	    break;
      }
      else
      {
         written += nbytes;
	 length -= nbytes;
      }
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "upload statistics with HTTP POST to %s:%d : %s", 
                      UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, port, message );

   errno = 0;
   status = 0;
   if (port == 443)
      nbytes = UFDBopenssl_read( message, 700, ssl );
   else
      nbytes = read( s, message, 700 );
   if (nbytes < 0 || nbytes < 9)
   {
      ufdbLogError( "did not receive a HTTP response header. URLs are not uploaded: %s  *****", 
                    strerror(errno) );
      written = 0;
   }
   else if (sscanf( message, "HTTP/%d.%d %d ", &dummy, &dummy, &status ) != 3)
   {
      ufdbLogError( "received a malformed HTTP response header: URLs are not uploaded  *****" );
      written = 0;
   }
   else if (status != 200)
   {
      ufdbLogError( "received an unexpected HTTP response status: code %d. URLs are not uploaded  *****", 
                    status );
      written = 0;
   }

   if (UFDBglobalDebug)
   {
      if (nbytes > 0)
      {
         message[nbytes] = '\0';
	 ufdbLogMessage( "URL upload received this HTTP reply:\n%s", message );
	 ufdbLogMessage( "HTTP reply status is %d  (status message has %d bytes)", status, nbytes );
      }
   }

   if (port == 443)
      UFDBopenssl_close( ssl );
   close( s );
   ufdbFree( message );

   if (written > 0  &&  UFDBglobalAnalyseUncategorisedURLs)
      ufdbLogMessage( "uncategorised URLs have been uploaded" );
}

