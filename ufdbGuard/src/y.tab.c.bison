/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "sg.y" /* yacc.c:339  */


/*
 * ufdbGuard is copyrighted (C) 2005-2017 by URLfilterDB B.V. with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * squidGuard is copyrighted (C) 1998 by
 * ElTele Øst AS, Oslo, Norway, with all rights reserved.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (version 2) as
 * published by the Free Software Foundation.  It is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License (GPL) for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * (GPL2) along with this program.
 */

#define YYMALLOC  ufdbMalloc
#define YYFREE    ufdbFree

#define UFDB_LOG_USER_QUOTA   0   /* TODO remove */

#undef UFDB_DEBUG_ACL_ACCESS

#include "ufdb.h"
#include "sg.h"
#include "ufdblib.h"
#include "ufdbdb.h"
#include "ufdbchkport.h"

#define UFDB_DEBUG 0

#ifdef UFDB_DEBUG
#undef YYDEBUG
#define YYDEBUG 1
#endif

#define UFDB_TIME_DEBUG 0

#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <grp.h>
#include <syslog.h>
#include <signal.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>


FILE * yyin;
FILE * yyout;
int    lineno;

extern struct ufdbSetting * lastSetting;
extern struct ufdbSetting * Setting;

extern struct Source * lastSource;

extern struct Category * lastCat;
extern struct Category * Cat;

extern struct sgRewrite * lastRewrite;
extern struct sgRewrite * Rewrite;
extern struct ufdbRegExp *  lastRewriteRegExec;

extern struct ufdbTime * lastTime;
extern struct ufdbTime * TimeList;

extern struct LogFile * globalLogFile;

extern struct LogFileStat * lastLogFileStat;
extern struct LogFileStat * LogFileStat;

extern struct TimeElement * lastTimeElement;
extern struct TimeElement * TimeElement;

extern struct Acl * lastAcl;
extern struct Acl * defaultAcl;
extern struct Aclcategory * lastAclcategory;
 
extern struct ufdbRegExp * lastRegExpDest;

extern int    httpsConnectionCacheSize;

extern int    globalDebugTimeDelta;

static int   numTimeElements = 0;
static int * TimeElementsEvents = NULL;

static int   time_switch = 0;
static int   date_switch = 0;

static void setRefreshUserlist( int nmin );
static void setRefreshDomainlist( int nmin );
static void ufdbSourceGroup( int groupType, char * groupName );
static void ufdbSourceUserQuota( const char * seconds, const char * sporadic, const char * renew );
void sgAclSetValue( const char * what, char * value, int allowed );
void sgIpv4( const char * name );
void ufdbFreeDomainDb( struct sgDb * dbp );


#line 177 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ADMINISTRATOR = 258,
    QSTRING = 259,
    CHECK_PROXY_TUNNELS = 260,
    QUEUE_CHECKS = 261,
    AGGRESSIVE = 262,
    LOG_ONLY = 263,
    ON = 264,
    OFF = 265,
    CHAR_MINUS = 266,
    CHAR_I = 267,
    CHAR_EXCLAMATION = 268,
    IGNORECASE = 269,
    COMMA = 270,
    EQUAL = 271,
    PORT = 272,
    HTTP_SERVER = 273,
    INTERFACE = 274,
    IMAGES = 275,
    HTTPS_PROHIBIT_INSECURE_SSLV2 = 276,
    HTTPS_PROHIBIT_INSECURE_SSLV3 = 277,
    HTTPS_CONNECTION_CACHE_SIZE = 278,
    IDENTIFIER = 279,
    WORD = 280,
    END = 281,
    START_BRACKET = 282,
    STOP_BRACKET = 283,
    WEEKDAY = 284,
    CATEGORY = 285,
    REWRITE = 286,
    ACL = 287,
    CPUS = 288,
    TIME = 289,
    TVAL = 290,
    DVAL = 291,
    DVALCRON = 292,
    BLOCK_BUMPED_CONNECT = 293,
    EVALUATE_AND = 294,
    EVALUATE_OR = 295,
    SOURCE = 296,
    CIDR = 297,
    IPV4CLASS = 298,
    CONTINUE = 299,
    IPV4ADDR = 300,
    IPV6ADDR = 301,
    IPV6NET = 302,
    DBHOME = 303,
    DOMAINLIST = 304,
    EXECDOMAINLIST = 305,
    REFRESHDOMAINLIST = 306,
    URLLIST = 307,
    EXPRESSIONLIST = 308,
    CACERTS = 309,
    CACERTSDIR = 310,
    IPV4 = 311,
    IPV4LIST = 312,
    IPV6 = 313,
    IPV6LIST = 314,
    DOMAIN = 315,
    UNIX = 316,
    LDAP = 317,
    USER = 318,
    USERLIST = 319,
    EXECUSERLIST = 320,
    REFRESHUSERLIST = 321,
    USERQUOTA = 322,
    GROUP = 323,
    NL = 324,
    NUMBER = 325,
    NUMBERS = 326,
    PASS = 327,
    REDIRECT = 328,
    SUBST = 329,
    CHAR = 330,
    MINUTELY = 331,
    HOURLY = 332,
    DAILY = 333,
    WEEKLY = 334,
    DATE = 335,
    REDIRECT_FATAL_ERROR = 336,
    REDIRECT_LOADING_DATABASE = 337,
    REDIRECT_HTTPS = 338,
    REDIRECT_BUMPED_HTTPS = 339,
    WITHIN = 340,
    OUTSIDE = 341,
    ELSE = 342,
    ANONYMOUS = 343,
    SPORADIC = 344,
    PIDFILE = 345,
    LOGFILE = 346,
    LOGDIR = 347,
    LOGPASS = 348,
    LOGBLOCK = 349,
    LOGALL = 350,
    LOGALL_HTTPD = 351,
    MAIL_SERVER = 352,
    MY_HOSTNAME = 353,
    ADMIN_EMAIL = 354,
    SENDER_EMAIL = 355,
    EXTERNAL_STATUS_COMMAND = 356,
    TOKEN_ALLOW = 357,
    TOKEN_DENY = 358,
    YOUTUBE_EDUFILTER = 359,
    YOUTUBE_EDUFILTER_ID = 360,
    ALLOW_GOOGLE_HTTPS_USING_IP = 361,
    URL_LOOKUP_RESULT_DB_RELOAD = 362,
    URL_LOOKUP_RESULT_FATAL_ERROR = 363,
    URL_LOOKUP_DELAY_DB_RELOAD = 364,
    OPTION = 365,
    UFDB_SHOW_URL_DETAILS = 366,
    UFDB_LOG_URL_DETAILS = 367,
    SQUID_VERSION = 368,
    SQUID_USES_SSL_BUMP = 369,
    UPLOAD_CRASH_REPORTS = 370,
    LOOKUP_REVERSE_IP = 371,
    USE_IPV6_ON_WAN = 372,
    STRIP_DOMAIN_FROM_USERNAME = 373,
    UFDB_DEBUG_FILTER = 374,
    UFDB_DEBUG_SKYPE_PROBES = 375,
    UFDB_DEBUG_GTALK_PROBES = 376,
    UFDB_DEBUG_YAHOOMSG_PROBES = 377,
    UFDB_DEBUG_AIM_PROBES = 378,
    UFDB_DEBUG_FBCHAT_PROBES = 379,
    UFDB_DEBUG_CITRIXONLINE_PROBES = 380,
    UFDB_EXPRESSION_OPTIMISATION = 381,
    UFDB_EXPRESSION_DEBUG = 382,
    UFDB_DEBUG_EXTERNAL_SCRIPTS = 383,
    UFDB_NUM_WORKER_THREADS = 384,
    ENFORCE_HTTPS_WITH_HOSTNAME = 385,
    ENFORCE_HTTPS_OFFICAL_CERTIFICATE = 386,
    ALLOW_SKYPE_OVER_HTTPS = 387,
    ALLOW_UNKNOWN_PROTOCOL_OVER_HTTPS = 388,
    ALLOW_GTALK_OVER_HTTPS = 389,
    ALLOW_YAHOOMSG_OVER_HTTPS = 390,
    ALLOW_AIM_OVER_HTTPS = 391,
    ALLOW_FBCHAT_OVER_HTTPS = 392,
    ALLOW_CITRIXONLINE_OVER_HTTPS = 393,
    ALLOW_ANYDESK_OVER_HTTPS = 394,
    ALLOW_TEAMVIEWER_OVER_HTTPS = 395,
    ANALYSE_UNCATEGORISED = 396,
    LOG_UNCATEGORISED_URLS = 397,
    UPLOAD_STATS = 398,
    SAFE_SEARCH = 399,
    MAX_LOGFILE_SIZE = 400,
    DENY_MODE = 401,
    REUSE_ACL_NAMES = 402
  };
#endif
/* Tokens.  */
#define ADMINISTRATOR 258
#define QSTRING 259
#define CHECK_PROXY_TUNNELS 260
#define QUEUE_CHECKS 261
#define AGGRESSIVE 262
#define LOG_ONLY 263
#define ON 264
#define OFF 265
#define CHAR_MINUS 266
#define CHAR_I 267
#define CHAR_EXCLAMATION 268
#define IGNORECASE 269
#define COMMA 270
#define EQUAL 271
#define PORT 272
#define HTTP_SERVER 273
#define INTERFACE 274
#define IMAGES 275
#define HTTPS_PROHIBIT_INSECURE_SSLV2 276
#define HTTPS_PROHIBIT_INSECURE_SSLV3 277
#define HTTPS_CONNECTION_CACHE_SIZE 278
#define IDENTIFIER 279
#define WORD 280
#define END 281
#define START_BRACKET 282
#define STOP_BRACKET 283
#define WEEKDAY 284
#define CATEGORY 285
#define REWRITE 286
#define ACL 287
#define CPUS 288
#define TIME 289
#define TVAL 290
#define DVAL 291
#define DVALCRON 292
#define BLOCK_BUMPED_CONNECT 293
#define EVALUATE_AND 294
#define EVALUATE_OR 295
#define SOURCE 296
#define CIDR 297
#define IPV4CLASS 298
#define CONTINUE 299
#define IPV4ADDR 300
#define IPV6ADDR 301
#define IPV6NET 302
#define DBHOME 303
#define DOMAINLIST 304
#define EXECDOMAINLIST 305
#define REFRESHDOMAINLIST 306
#define URLLIST 307
#define EXPRESSIONLIST 308
#define CACERTS 309
#define CACERTSDIR 310
#define IPV4 311
#define IPV4LIST 312
#define IPV6 313
#define IPV6LIST 314
#define DOMAIN 315
#define UNIX 316
#define LDAP 317
#define USER 318
#define USERLIST 319
#define EXECUSERLIST 320
#define REFRESHUSERLIST 321
#define USERQUOTA 322
#define GROUP 323
#define NL 324
#define NUMBER 325
#define NUMBERS 326
#define PASS 327
#define REDIRECT 328
#define SUBST 329
#define CHAR 330
#define MINUTELY 331
#define HOURLY 332
#define DAILY 333
#define WEEKLY 334
#define DATE 335
#define REDIRECT_FATAL_ERROR 336
#define REDIRECT_LOADING_DATABASE 337
#define REDIRECT_HTTPS 338
#define REDIRECT_BUMPED_HTTPS 339
#define WITHIN 340
#define OUTSIDE 341
#define ELSE 342
#define ANONYMOUS 343
#define SPORADIC 344
#define PIDFILE 345
#define LOGFILE 346
#define LOGDIR 347
#define LOGPASS 348
#define LOGBLOCK 349
#define LOGALL 350
#define LOGALL_HTTPD 351
#define MAIL_SERVER 352
#define MY_HOSTNAME 353
#define ADMIN_EMAIL 354
#define SENDER_EMAIL 355
#define EXTERNAL_STATUS_COMMAND 356
#define TOKEN_ALLOW 357
#define TOKEN_DENY 358
#define YOUTUBE_EDUFILTER 359
#define YOUTUBE_EDUFILTER_ID 360
#define ALLOW_GOOGLE_HTTPS_USING_IP 361
#define URL_LOOKUP_RESULT_DB_RELOAD 362
#define URL_LOOKUP_RESULT_FATAL_ERROR 363
#define URL_LOOKUP_DELAY_DB_RELOAD 364
#define OPTION 365
#define UFDB_SHOW_URL_DETAILS 366
#define UFDB_LOG_URL_DETAILS 367
#define SQUID_VERSION 368
#define SQUID_USES_SSL_BUMP 369
#define UPLOAD_CRASH_REPORTS 370
#define LOOKUP_REVERSE_IP 371
#define USE_IPV6_ON_WAN 372
#define STRIP_DOMAIN_FROM_USERNAME 373
#define UFDB_DEBUG_FILTER 374
#define UFDB_DEBUG_SKYPE_PROBES 375
#define UFDB_DEBUG_GTALK_PROBES 376
#define UFDB_DEBUG_YAHOOMSG_PROBES 377
#define UFDB_DEBUG_AIM_PROBES 378
#define UFDB_DEBUG_FBCHAT_PROBES 379
#define UFDB_DEBUG_CITRIXONLINE_PROBES 380
#define UFDB_EXPRESSION_OPTIMISATION 381
#define UFDB_EXPRESSION_DEBUG 382
#define UFDB_DEBUG_EXTERNAL_SCRIPTS 383
#define UFDB_NUM_WORKER_THREADS 384
#define ENFORCE_HTTPS_WITH_HOSTNAME 385
#define ENFORCE_HTTPS_OFFICAL_CERTIFICATE 386
#define ALLOW_SKYPE_OVER_HTTPS 387
#define ALLOW_UNKNOWN_PROTOCOL_OVER_HTTPS 388
#define ALLOW_GTALK_OVER_HTTPS 389
#define ALLOW_YAHOOMSG_OVER_HTTPS 390
#define ALLOW_AIM_OVER_HTTPS 391
#define ALLOW_FBCHAT_OVER_HTTPS 392
#define ALLOW_CITRIXONLINE_OVER_HTTPS 393
#define ALLOW_ANYDESK_OVER_HTTPS 394
#define ALLOW_TEAMVIEWER_OVER_HTTPS 395
#define ANALYSE_UNCATEGORISED 396
#define LOG_UNCATEGORISED_URLS 397
#define UPLOAD_STATS 398
#define SAFE_SEARCH 399
#define MAX_LOGFILE_SIZE 400
#define DENY_MODE 401
#define REUSE_ACL_NAMES 402

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 112 "sg.y" /* yacc.c:355  */

  char * string;
  char * tval;
  char * dval;
  char * dvalcron;
  int    integer;

#line 519 "y.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 536 "y.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   550

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  148
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  117
/* YYNRULES -- Number of rules.  */
#define YYNRULES  338
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  496

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   402

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   200,   200,   204,   205,   209,   213,   214,   218,   219,
     220,   224,   228,   232,   236,   240,   244,   248,   252,   256,
     260,   261,   265,   270,   275,   280,   286,   287,   288,   289,
     293,   297,   301,   302,   306,   310,   314,   318,   322,   326,
     331,   351,   356,   361,   366,   371,   375,   379,   383,   387,
     391,   395,   399,   403,   407,   411,   415,   419,   423,   427,
     431,   435,   436,   440,   441,   445,   449,   460,   464,   465,
     469,   474,   479,   484,   489,   494,   501,   506,   514,   521,
     526,   538,   539,   545,   551,   552,   553,   557,   558,   562,
     566,   567,   569,   571,   573,   578,   582,   583,   587,   588,
     589,   590,   591,   592,   593,   594,   595,   596,   597,   598,
     599,   600,   601,   602,   603,   604,   605,   606,   607,   608,
     609,   610,   611,   612,   613,   614,   615,   616,   617,   618,
     619,   620,   621,   622,   623,   624,   625,   626,   627,   628,
     629,   631,   633,   635,   640,   644,   647,   649,   653,   654,
     655,   656,   657,   658,   659,   660,   661,   662,   663,   664,
     665,   667,   669,   671,   673,   674,   675,   676,   677,   678,
     679,   680,   681,   682,   683,   685,   687,   689,   691,   695,
     697,   698,   699,   702,   704,   705,   706,   710,   713,   714,
     718,   719,   720,   724,   726,   725,   730,   732,   736,   742,
     743,   744,   745,   746,   748,   750,   752,   756,   758,   759,
     760,   761,   762,   766,   770,   773,   775,   776,   777,   778,
     779,   783,   786,   788,   789,   793,   795,   800,   801,   805,
     808,   810,   815,   816,   817,   818,   819,   820,   821,   826,
     830,   833,   835,   840,   840,   840,   841,   841,   841,   842,
     842,   842,   843,   843,   844,   848,   848,   849,   853,   854,
     855,   856,   857,   858,   861,   864,   867,   871,   874,   875,
     879,   880,   881,   882,   883,   884,   885,   886,   887,   888,
     889,   890,   891,   892,   893,   894,   895,   896,   897,   898,
     899,   900,   901,   902,   903,   904,   905,   906,   907,   908,
     909,   910,   911,   912,   913,   914,   915,   916,   917,   918,
     919,   920,   921,   922,   923,   924,   925,   926,   927,   928,
     929,   930,   931,   932,   933,   934,   935,   936,   937,   938,
     939,   940,   941,   942,   943,   944,   945,   946,   947
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ADMINISTRATOR", "QSTRING",
  "CHECK_PROXY_TUNNELS", "QUEUE_CHECKS", "AGGRESSIVE", "LOG_ONLY", "ON",
  "OFF", "CHAR_MINUS", "CHAR_I", "CHAR_EXCLAMATION", "IGNORECASE", "COMMA",
  "EQUAL", "PORT", "HTTP_SERVER", "INTERFACE", "IMAGES",
  "HTTPS_PROHIBIT_INSECURE_SSLV2", "HTTPS_PROHIBIT_INSECURE_SSLV3",
  "HTTPS_CONNECTION_CACHE_SIZE", "IDENTIFIER", "WORD", "END",
  "START_BRACKET", "STOP_BRACKET", "WEEKDAY", "CATEGORY", "REWRITE", "ACL",
  "CPUS", "TIME", "TVAL", "DVAL", "DVALCRON", "BLOCK_BUMPED_CONNECT",
  "EVALUATE_AND", "EVALUATE_OR", "SOURCE", "CIDR", "IPV4CLASS", "CONTINUE",
  "IPV4ADDR", "IPV6ADDR", "IPV6NET", "DBHOME", "DOMAINLIST",
  "EXECDOMAINLIST", "REFRESHDOMAINLIST", "URLLIST", "EXPRESSIONLIST",
  "CACERTS", "CACERTSDIR", "IPV4", "IPV4LIST", "IPV6", "IPV6LIST",
  "DOMAIN", "UNIX", "LDAP", "USER", "USERLIST", "EXECUSERLIST",
  "REFRESHUSERLIST", "USERQUOTA", "GROUP", "NL", "NUMBER", "NUMBERS",
  "PASS", "REDIRECT", "SUBST", "CHAR", "MINUTELY", "HOURLY", "DAILY",
  "WEEKLY", "DATE", "REDIRECT_FATAL_ERROR", "REDIRECT_LOADING_DATABASE",
  "REDIRECT_HTTPS", "REDIRECT_BUMPED_HTTPS", "WITHIN", "OUTSIDE", "ELSE",
  "ANONYMOUS", "SPORADIC", "PIDFILE", "LOGFILE", "LOGDIR", "LOGPASS",
  "LOGBLOCK", "LOGALL", "LOGALL_HTTPD", "MAIL_SERVER", "MY_HOSTNAME",
  "ADMIN_EMAIL", "SENDER_EMAIL", "EXTERNAL_STATUS_COMMAND", "TOKEN_ALLOW",
  "TOKEN_DENY", "YOUTUBE_EDUFILTER", "YOUTUBE_EDUFILTER_ID",
  "ALLOW_GOOGLE_HTTPS_USING_IP", "URL_LOOKUP_RESULT_DB_RELOAD",
  "URL_LOOKUP_RESULT_FATAL_ERROR", "URL_LOOKUP_DELAY_DB_RELOAD", "OPTION",
  "UFDB_SHOW_URL_DETAILS", "UFDB_LOG_URL_DETAILS", "SQUID_VERSION",
  "SQUID_USES_SSL_BUMP", "UPLOAD_CRASH_REPORTS", "LOOKUP_REVERSE_IP",
  "USE_IPV6_ON_WAN", "STRIP_DOMAIN_FROM_USERNAME", "UFDB_DEBUG_FILTER",
  "UFDB_DEBUG_SKYPE_PROBES", "UFDB_DEBUG_GTALK_PROBES",
  "UFDB_DEBUG_YAHOOMSG_PROBES", "UFDB_DEBUG_AIM_PROBES",
  "UFDB_DEBUG_FBCHAT_PROBES", "UFDB_DEBUG_CITRIXONLINE_PROBES",
  "UFDB_EXPRESSION_OPTIMISATION", "UFDB_EXPRESSION_DEBUG",
  "UFDB_DEBUG_EXTERNAL_SCRIPTS", "UFDB_NUM_WORKER_THREADS",
  "ENFORCE_HTTPS_WITH_HOSTNAME", "ENFORCE_HTTPS_OFFICAL_CERTIFICATE",
  "ALLOW_SKYPE_OVER_HTTPS", "ALLOW_UNKNOWN_PROTOCOL_OVER_HTTPS",
  "ALLOW_GTALK_OVER_HTTPS", "ALLOW_YAHOOMSG_OVER_HTTPS",
  "ALLOW_AIM_OVER_HTTPS", "ALLOW_FBCHAT_OVER_HTTPS",
  "ALLOW_CITRIXONLINE_OVER_HTTPS", "ALLOW_ANYDESK_OVER_HTTPS",
  "ALLOW_TEAMVIEWER_OVER_HTTPS", "ANALYSE_UNCATEGORISED",
  "LOG_UNCATEGORISED_URLS", "UPLOAD_STATS", "SAFE_SEARCH",
  "MAX_LOGFILE_SIZE", "DENY_MODE", "REUSE_ACL_NAMES", "$accept", "start",
  "qidentifier", "https_cache_size", "allow_or_deny", "on_or_off",
  "log_pass", "log_block", "log_all", "logall_httpd", "youtube_edufilter",
  "youtube_edufilter_id", "allow_google_https_using_ip", "deny_mode",
  "reuse_acl_names", "debug_filter", "enforce_https_with_hostname",
  "enforce_https_offical_certificate", "https_prohibit_insecure_sslv2",
  "https_prohibit_insecure_sslv3", "check_proxy_tunnel_option",
  "check_proxy_tunnels", "admin_spec", "dbhome", "refreshuserlist",
  "refreshdomainlist", "url_lookup_result_db_reload",
  "url_lookup_result_fatal_error", "url_lookup_delay_db_reload",
  "squid_version", "num_worker_threads", "upload_crash_reports",
  "lookup_reverse_ip", "use_ipv6_on_wan", "squid_uses_ssl_bump",
  "ufdb_log_url_details", "ufdb_show_url_details",
  "ufdb_debug_skype_probes", "ufdb_debug_gtalk_probes",
  "ufdb_debug_yahoomsg_probes", "ufdb_debug_aim_probes",
  "ufdb_debug_fbchat_probes", "ufdb_debug_citrixonline_probes",
  "ufdb_expression_optimisation", "ufdb_expression_debug",
  "ufdb_debug_external_scripts", "mail_server", "my_hostname",
  "admin_email", "sender_email", "external_status_command", "logdir",
  "pidfile", "port", "interface", "cpus", "upload_stats", "redirect_https",
  "redirect_bumped_https", "redirect_loading_database",
  "redirect_fatal_error", "log_uncategorised_urls",
  "analyse_uncategorised", "strip_domain_from_username", "safe_search",
  "max_logfile_size", "httpd_option", "httpd_options", "http_server_def",
  "category", "category_block", "category_contents", "category_content",
  "source", "source_block", "source_contents", "source_content", "domain",
  "user", "acl_block", "acl_contents", "acl_header", "acl_content", "$@1",
  "access_contents", "access_content", "access_pass", "cidr", "ipv4class",
  "ipv4s", "ipv4", "ipv6s", "ipv6", "rew", "rew_block", "rew_contents",
  "rew_content", "time", "time_block", "time_contents", "time_content",
  "$@2", "$@3", "$@4", "$@5", "$@6", "$@7", "$@8", "ttime", "$@9", "date",
  "dval", "tval", "dvalcron", "an_error", "statements", "statement", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402
};
# endif

#define YYPACT_NINF -345

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-345)))

#define YYTABLE_NINF -256

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -345,    12,    64,  -345,  -345,    -2,   446,   -53,    39,    80,
     140,   140,   -45,    99,   128,    62,    96,     7,     7,   195,
     -41,    40,  -345,   147,   170,   210,   222,   299,   357,   140,
     140,   140,   140,   224,   227,   228,   243,   255,   140,   256,
     140,   -25,   -25,   140,   140,   140,   257,   140,   140,   140,
     140,   140,     6,   140,   140,   140,   140,   140,   140,   140,
     140,   140,    74,   140,   140,   127,   140,   140,   140,   199,
     140,   140,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,   238,  -345,   244,  -345,  -345,   247,  -345,
     260,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,   285,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,   254,   276,   277,
     279,   270,    89,   372,   411,    33,     0,   231,    84,   395,
     285,  -345,  -345,   -13,  -345,  -345,  -345,   408,     9,     7,
     192,   211,   410,   412,   424,     7,     7,     2,   258,  -345,
    -345,  -345,  -345,  -345,  -345,   456,  -345,   460,  -345,    63,
    -345,   469,     7,   237,   479,     7,     7,    50,  -345,  -345,
    -345,     7,     7,    67,  -345,  -345,  -345,   175,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,     7,     7,
     190,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,   482,   484,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,   486,  -345,   140,   140,   140,   140,
     236,   356,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,    23,  -345,  -345,    55,  -345,  -345,   291,  -345,
     488,   491,   293,  -345,  -345,  -345,   240,  -345,  -345,  -345,
    -345,  -345,   494,  -345,  -345,  -345,  -345,   496,  -345,   290,
       7,   295,   176,   317,   318,   239,   501,  -345,   510,   110,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,   100,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,   293,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,   164,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,   105,   312,  -345,  -345,  -345,  -345,  -345,   526,
    -345,  -345,  -345,   513,  -345,   303,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,   312,   312,   312,   314,  -345,   319,
     342,   319,   331,  -345,   220,  -345,  -345,  -345,  -345,  -345,
     319,   319,   319,   312,   312,   312,  -345,  -345,  -345,   319,
     348,  -345,   283,   312,  -345,  -345
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
     268,     0,     0,     1,   267,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   337,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   275,   277,   278,   279,   280,   328,   330,   329,
     331,   332,   281,   282,   283,   284,   285,   276,   274,   286,
     313,   314,   295,   296,   294,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   315,   289,   290,   291,   292,   293,   287,   288,   317,
     316,   318,   319,   320,   321,   322,   323,   325,   324,   327,
     326,   333,   273,   270,   272,     0,   271,   334,     0,   335,
       0,   336,   338,   269,    31,    27,    28,    29,    26,    30,
      65,     0,     4,     3,    67,    66,    10,     8,     9,    24,
      25,     5,    94,    91,    92,    93,    90,   228,   227,   188,
      68,    69,   239,   144,    33,    32,    35,    34,    74,    73,
      71,    72,    64,    63,    62,    61,    11,    12,    13,    14,
      56,    57,    58,    59,    60,    15,    16,    17,     6,     7,
      36,    37,    38,    46,    45,    39,    44,    41,    42,    43,
      78,    21,    20,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    40,    22,    23,    77,    76,    75,    70,    79,
      80,    18,    19,    96,   146,   230,   241,     0,     0,     0,
      88,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    89,   187,     0,   196,   189,    95,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    97,
     145,   170,   171,   178,   215,     0,   222,     0,   179,     0,
     183,     0,     0,     0,     0,     0,     0,     0,   147,   229,
     232,     0,     0,     0,   231,   254,   240,   246,   252,   242,
      81,    83,    84,    82,    86,    85,    87,   190,     0,     0,
       0,   119,   118,   100,    99,    98,   101,   104,   103,   102,
     107,     0,     0,   106,   105,   113,   112,   115,   114,   117,
     116,   120,   121,   143,     0,   142,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   164,   166,   165,   167,   169,   168,   148,   183,
       0,     0,   149,   152,   151,   155,     0,   157,   156,   172,
     173,   177,     0,   176,   233,   234,   238,     0,   237,     0,
       0,     0,     0,     0,     0,   193,     0,   207,     0,     0,
     197,   109,   108,   111,   110,   141,   140,   129,   130,   122,
     124,   126,   128,   131,   139,   132,   133,   134,   135,   136,
     137,   138,   123,   220,   221,   216,   224,   225,   226,   223,
     182,   181,   180,   150,   154,   153,   159,   158,   186,   185,
     184,     0,   175,   174,   236,   235,   244,   247,   250,   264,
     266,   253,   259,   263,   191,   192,   194,   200,   199,   198,
     202,   201,   206,     0,   205,     0,   213,   214,   217,   218,
     163,   160,   161,   162,     0,     0,     0,     0,   265,   258,
       0,   262,     0,   208,     0,   212,   209,   204,   203,   219,
     245,   248,   251,   261,     0,     0,   196,   210,   211,   260,
       0,   257,     0,     0,   195,   256
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -345,  -345,    -9,  -345,   322,    -8,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,   117,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,     1,  -345,
    -345,  -345,  -345,  -345,  -117,  -345,  -345,  -345,  -345,  -345,
     -85,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,  -344,  -345,
    -345,   -95,  -255,  -345,  -345,  -345,  -345
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,   430,    72,   200,   159,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
     149,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   240,   241,   132,   133,
     134,   243,   269,   135,   136,   244,   288,   358,   362,   137,
     242,   254,   255,   472,   310,   390,   449,   458,   459,   352,
     415,   355,   419,   138,   139,   245,   294,   140,   141,   246,
     299,   379,   464,   380,   465,   381,   466,   382,   469,   484,
     441,   442,   470,   443,   142,     2,   143
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     155,   295,   144,   160,   166,   168,   152,   156,   172,   173,
     175,   152,     3,   152,   307,   157,   158,   150,   183,   185,
     313,   186,   187,   188,   189,   161,   153,   333,   296,   176,
     195,   153,   197,   153,   314,   202,   203,   204,   413,   206,
     207,   208,   209,   210,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   152,   223,   224,   226,   227,   228,
     229,   289,   231,   232,    -2,     4,   151,     5,   414,     6,
     416,   152,   308,   309,   153,   371,   211,   198,   199,   297,
     298,     7,     8,     9,   152,    10,    11,    12,   152,   169,
     334,   153,   376,   152,    13,    14,    15,    16,    17,   471,
     162,   417,   418,   152,   153,    18,   163,   290,   153,   301,
     177,   455,    19,   153,   152,    20,   467,   252,   291,   292,
     480,   481,   482,   153,   293,   154,   359,   360,   156,   302,
      21,   361,   152,    22,   153,   452,   157,   158,   372,   489,
     468,   156,   456,   457,   222,    23,    24,    25,    26,   157,
     158,   178,   153,   167,    27,   377,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,   170,   171,    38,    39,
      40,    41,    42,    43,   179,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,   152,   225,   453,   152,
    -249,   164,   165,   317,  -243,    65,    66,    67,    68,    69,
      70,    71,   439,   440,   180,   152,   153,   318,   385,   153,
     174,   386,   320,   321,   152,   322,   181,   487,   190,   490,
     491,   191,   192,   253,   460,   153,   323,   156,   495,   303,
     305,   461,   462,   463,   153,   157,   158,   193,   312,   315,
     316,   319,   324,   326,   328,   330,   331,   332,   335,   194,
     196,   205,   387,   388,  -125,   233,   354,  -125,   357,   230,
     247,   234,   364,   365,   235,   368,   369,   370,   373,   336,
     337,   389,   374,   375,   378,  -125,  -125,   236,  -125,  -125,
    -125,  -125,   248,   249,   250,   152,   338,   152,   251,   383,
     384,   300,   237,   152,   238,   239,   420,   366,   428,  -125,
     431,   494,   392,   394,   386,   153,   421,   153,   429,   436,
     438,  -125,  -125,   153,   182,   396,   446,  -125,   397,   398,
     399,   400,   401,   402,   403,   404,   405,   406,   407,   408,
     409,   410,   411,   412,   444,   445,  -125,   468,   414,   422,
     439,   425,   427,   485,  -255,   387,   388,   156,   486,   493,
     423,   152,   339,   433,   201,   157,   158,   306,   435,   492,
     479,   437,   483,     0,   389,     0,     0,   448,     0,   451,
     454,   153,   184,     0,  -127,     0,     0,  -127,   340,   341,
     342,   343,   344,   345,   346,   347,   348,   349,   350,   152,
     256,     0,   351,   257,     0,  -127,  -127,     0,  -127,  -127,
    -127,  -127,   152,     0,   152,     0,   152,     0,     0,   153,
     304,   258,   259,     0,   260,   261,   262,   263,   152,  -127,
       0,     0,   153,   311,   153,   325,   153,   327,     0,   270,
     476,  -127,  -127,     0,   478,   264,     0,  -127,   153,   329,
     271,   272,   145,   146,   147,   273,   148,   265,   266,     0,
     152,     0,     0,   267,   152,   488,  -127,   274,   275,   276,
     277,   278,   279,   152,   280,   281,   282,     0,   283,   284,
     153,   353,   268,   152,   153,   356,   152,     0,   152,     0,
     152,     0,   152,   153,   363,   152,   285,   286,   152,     0,
     152,     0,   287,   153,   367,   152,   153,   391,   153,   393,
     153,   395,   153,   424,   152,   153,   426,   152,   153,   432,
     153,   434,     0,     0,     0,   153,   447,     0,     0,     0,
     152,     0,     0,   473,   153,   450,     0,   153,   477,   474,
       0,   475,     0,     0,     0,     0,     0,     0,     0,     0,
     153
};

static const yytype_int16 yycheck[] =
{
       9,     1,     4,    11,    13,    14,     4,     1,    17,    18,
      19,     4,     0,     4,    27,     9,    10,    70,    27,    28,
      11,    29,    30,    31,    32,    70,    24,    25,    28,    70,
      38,    24,    40,    24,    25,    43,    44,    45,    15,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,     4,    63,    64,    65,    66,    67,
      68,    28,    70,    71,     0,     1,    27,     3,    45,     5,
      15,     4,    85,    86,    24,    25,    70,   102,   103,    79,
      80,    17,    18,    19,     4,    21,    22,    23,     4,    27,
      88,    24,    25,     4,    30,    31,    32,    33,    34,   443,
       1,    46,    47,     4,    24,    41,     7,    74,    24,    25,
      70,    11,    48,    24,     4,    51,    11,    28,    85,    86,
     464,   465,   466,    24,    91,    45,    63,    64,     1,    45,
      66,    68,     4,    69,    24,    25,     9,    10,    88,   483,
      35,     1,    42,    43,    70,    81,    82,    83,    84,     9,
      10,     4,    24,    25,    90,    88,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,    70,    71,   104,   105,
     106,   107,   108,   109,     4,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     4,    70,    88,     4,
      25,   102,   103,    11,    29,   141,   142,   143,   144,   145,
     146,   147,    36,    37,     4,     4,    24,    25,    28,    24,
      25,    31,    11,    12,     4,    14,     4,     7,     4,   484,
     485,     4,     4,   242,    70,    24,    25,     1,   493,   248,
     249,    77,    78,    79,    24,     9,    10,     4,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,     4,
       4,     4,    72,    73,    28,    27,   275,    31,   277,    70,
      16,    27,   281,   282,    27,   284,   285,   286,   287,    21,
      22,    91,   291,   292,   293,    49,    50,    27,    52,    53,
      54,    55,    16,    16,    15,     4,    38,     4,    28,   308,
     309,    70,    17,     4,    19,    20,    15,    70,    15,    73,
      70,    28,   321,   322,    31,    24,    25,    24,    25,    29,
      25,    85,    86,    24,    25,   334,    87,    91,   336,   337,
     338,   339,   340,   341,   342,   343,   344,   345,   346,   347,
     348,   349,   350,   351,    27,    27,   110,    35,    45,   358,
      36,   360,   361,    11,    35,    72,    73,     1,    27,    11,
     359,     4,   104,   372,    42,     9,    10,   250,   377,   486,
     455,   380,   467,    -1,    91,    -1,    -1,   386,    -1,   388,
     389,    24,    25,    -1,    28,    -1,    -1,    31,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,     4,
      28,    -1,   144,    31,    -1,    49,    50,    -1,    52,    53,
      54,    55,     4,    -1,     4,    -1,     4,    -1,    -1,    24,
      25,    49,    50,    -1,    52,    53,    54,    55,     4,    73,
      -1,    -1,    24,    25,    24,    25,    24,    25,    -1,    28,
     449,    85,    86,    -1,   453,    73,    -1,    91,    24,    25,
      39,    40,     6,     7,     8,    44,    10,    85,    86,    -1,
       4,    -1,    -1,    91,     4,   474,   110,    56,    57,    58,
      59,    60,    61,     4,    63,    64,    65,    -1,    67,    68,
      24,    25,   110,     4,    24,    25,     4,    -1,     4,    -1,
       4,    -1,     4,    24,    25,     4,    85,    86,     4,    -1,
       4,    -1,    91,    24,    25,     4,    24,    25,    24,    25,
      24,    25,    24,    25,     4,    24,    25,     4,    24,    25,
      24,    25,    -1,    -1,    -1,    24,    25,    -1,    -1,    -1,
       4,    -1,    -1,     7,    24,    25,    -1,    24,    25,    13,
      -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      24
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   149,   263,     0,     1,     3,     5,    17,    18,    19,
      21,    22,    23,    30,    31,    32,    33,    34,    41,    48,
      51,    66,    69,    81,    82,    83,    84,    90,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   104,   105,
     106,   107,   108,   109,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   141,   142,   143,   144,   145,
     146,   147,   151,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   216,   217,   218,   221,   222,   227,   241,   242,
     245,   246,   262,   264,     4,     6,     7,     8,    10,   168,
      70,    27,     4,    24,    45,   150,     1,     9,    10,   153,
     153,    70,     1,     7,   102,   103,   150,    25,   150,    27,
      70,    71,   150,   150,    25,   150,    70,    70,     4,     4,
       4,     4,    25,   150,    25,   150,   153,   153,   153,   153,
       4,     4,     4,     4,     4,   153,     4,   153,   102,   103,
     152,   152,   153,   153,   153,     4,   153,   153,   153,   153,
     153,    70,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,    70,   153,   153,    70,   153,   153,   153,   153,
      70,   153,   153,    27,    27,    27,    27,    17,    19,    20,
     214,   215,   228,   219,   223,   243,   247,    16,    16,    16,
      15,    28,    28,   150,   229,   230,    28,    31,    49,    50,
      52,    53,    54,    55,    73,    85,    86,    91,   110,   220,
      28,    39,    40,    44,    56,    57,    58,    59,    60,    61,
      63,    64,    65,    67,    68,    85,    86,    91,   224,    28,
      74,    85,    86,    91,   244,     1,    28,    79,    80,   248,
      70,    25,    45,   150,    25,   150,   215,    27,    85,    86,
     232,    25,   150,    11,    25,   150,   150,    11,    25,   150,
      11,    12,    14,    25,   150,    25,   150,    25,   150,    25,
     150,   150,   150,    25,    88,   150,    21,    22,    38,   104,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   144,   237,    25,   150,   239,    25,   150,   225,    63,
      64,    68,   226,    25,   150,   150,    70,    25,   150,   150,
     150,    25,    88,   150,   150,   150,    25,    88,   150,   249,
     251,   253,   255,   150,   150,    28,    31,    72,    73,    91,
     233,    25,   150,    25,   150,    25,   150,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,    15,    45,   238,    15,    46,    47,   240,
      15,    25,   150,   226,    25,   150,    25,   150,    15,    25,
     150,    70,    25,   150,    25,   150,    29,   150,    25,    36,
      37,   258,   259,   261,    27,    27,    87,    25,   150,   234,
      25,   150,    25,    88,   150,    11,    42,    43,   235,   236,
      70,    77,    78,    79,   250,   252,   254,    11,    35,   256,
     260,   256,   231,     7,    13,    15,   150,    25,   150,   238,
     256,   256,   256,   259,   257,    11,    27,     7,   150,   256,
     260,   260,   232,    11,    28,   260
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   148,   149,   150,   150,   151,   152,   152,   153,   153,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   163,   164,   165,   166,   167,   168,   168,   168,   168,
     169,   170,   171,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   199,   200,   200,   201,   202,   202,   203,   203,
     204,   205,   206,   207,   208,   209,   210,   210,   211,   212,
     213,   214,   214,   214,   214,   214,   214,   215,   215,   216,
     217,   217,   217,   217,   217,   218,   219,   219,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   221,   222,   223,   223,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   225,
     225,   225,   225,   226,   226,   226,   226,   227,   228,   228,
     229,   229,   229,   230,   231,   230,   232,   232,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   234,   234,   234,
     234,   234,   234,   235,   236,   237,   237,   237,   237,   237,
     237,   238,   239,   239,   239,   240,   240,   241,   241,   242,
     243,   243,   244,   244,   244,   244,   244,   244,   244,   245,
     246,   247,   247,   249,   250,   248,   251,   252,   248,   253,
     254,   248,   255,   248,   248,   257,   256,   256,   258,   258,
     258,   258,   258,   258,   259,   260,   261,   262,   263,   263,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     1,     4,
       2,     2,     2,     2,     2,     4,     0,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     3,     3,
       3,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     3,     3,     3,     2,     3,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     4,     0,     2,     2,     2,
       3,     2,     2,     3,     3,     2,     2,     2,     3,     3,
       4,     4,     4,     4,     2,     2,     2,     2,     2,     2,
       1,     1,     2,     2,     3,     3,     2,     2,     1,     0,
       2,     2,     2,     0,     2,     2,     2,     4,     0,     2,
       2,     4,     4,     3,     0,     8,     0,     2,     2,     2,
       2,     2,     2,     3,     3,     2,     2,     0,     2,     2,
       3,     3,     2,     1,     1,     0,     2,     3,     3,     4,
       2,     1,     0,     2,     2,     1,     1,     2,     2,     4,
       0,     2,     1,     2,     2,     3,     3,     2,     2,     2,
       4,     0,     2,     0,     0,     5,     0,     0,     5,     0,
       0,     5,     0,     3,     1,     0,     5,     3,     2,     1,
       4,     3,     2,     1,     1,     1,     1,     1,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 204 "sg.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string); }
#line 2076 "y.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 205 "sg.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string); }
#line 2082 "y.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 209 "sg.y" /* yacc.c:1646  */
    { httpsConnectionCacheSize = atoi( (yyvsp[0].string) ); ufdbFree( (yyvsp[0].string) ); }
#line 2088 "y.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 213 "sg.y" /* yacc.c:1646  */
    { (yyval.integer) = UFDB_ALLOW; }
#line 2094 "y.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 214 "sg.y" /* yacc.c:1646  */
    { (yyval.integer) = UFDB_DENY; }
#line 2100 "y.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 218 "sg.y" /* yacc.c:1646  */
    { (yyval.integer) = 1; }
#line 2106 "y.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 219 "sg.y" /* yacc.c:1646  */
    { (yyval.integer) = 0; }
#line 2112 "y.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 220 "sg.y" /* yacc.c:1646  */
    { ufdbLogFatalError( "syntax error at line %d: expected 'on' or 'off'", lineno );   }
#line 2118 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 224 "sg.y" /* yacc.c:1646  */
    { UFDBglobalLogPass = (yyvsp[0].integer); }
#line 2124 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 228 "sg.y" /* yacc.c:1646  */
    { UFDBglobalLogBlock = (yyvsp[0].integer); }
#line 2130 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 232 "sg.y" /* yacc.c:1646  */
    { UFDBglobalLogAllRequests = (yyvsp[0].integer); }
#line 2136 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 236 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebugHttpd = (yyvsp[0].integer); }
#line 2142 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 240 "sg.y" /* yacc.c:1646  */
    { UFDBglobalYoutubeEdufilter = (yyvsp[0].integer); }
#line 2148 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 244 "sg.y" /* yacc.c:1646  */
    { UFDBglobalYoutubeEdufilterID = (yyvsp[0].string); }
#line 2154 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 248 "sg.y" /* yacc.c:1646  */
    { UFDBglobalAllowGoogleHTTPSusingIP = (yyvsp[0].integer); }
#line 2160 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 252 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDenyMode = (yyvsp[0].integer); }
#line 2166 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 256 "sg.y" /* yacc.c:1646  */
    { UFDBglobalReuseAclNames = (yyvsp[0].integer); }
#line 2172 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 260 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebug = (yyvsp[0].integer); }
#line 2178 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 261 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebug = atoi( (yyvsp[0].string) ); }
#line 2184 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 266 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: option enforce-https-with-hostname must be part of the security category", lineno );  }
#line 2190 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 271 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: option enforce-https-official-certificate must be part of the security category", lineno );  }
#line 2196 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 276 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: option https-prohibit-insecure-sslv2 must be part of the security category", lineno );  }
#line 2202 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 281 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: option https-prohibit-insecure-sslv3 must be part of the security category", lineno );  }
#line 2208 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 286 "sg.y" /* yacc.c:1646  */
    { (yyval.integer) = UFDB_API_HTTPS_CHECK_OFF; }
#line 2214 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 287 "sg.y" /* yacc.c:1646  */
    { (yyval.integer) = UFDB_API_HTTPS_CHECK_QUEUE_CHECKS; }
#line 2220 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 288 "sg.y" /* yacc.c:1646  */
    { (yyval.integer) = UFDB_API_HTTPS_CHECK_AGGRESSIVE; }
#line 2226 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 289 "sg.y" /* yacc.c:1646  */
    { (yyval.integer) = UFDB_API_HTTPS_LOG_ONLY; }
#line 2232 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 293 "sg.y" /* yacc.c:1646  */
    { UFDBsetTunnelCheckMethod( (yyvsp[0].integer) ); }
#line 2238 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 297 "sg.y" /* yacc.c:1646  */
    { ufdbSetting("administrator",(yyvsp[0].string)); }
#line 2244 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 301 "sg.y" /* yacc.c:1646  */
    { ufdbSetting("dbhome",(yyvsp[0].string)); }
#line 2250 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 302 "sg.y" /* yacc.c:1646  */
    { ufdbSetting("dbhome",(yyvsp[0].string)); }
#line 2256 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 306 "sg.y" /* yacc.c:1646  */
    { setRefreshUserlist( atoi((yyvsp[0].string)) ); }
#line 2262 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 310 "sg.y" /* yacc.c:1646  */
    { setRefreshDomainlist( atoi((yyvsp[0].string)) ); }
#line 2268 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 315 "sg.y" /* yacc.c:1646  */
    { UFDBglobalURLlookupResultDBreload = (yyvsp[0].integer); }
#line 2274 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 319 "sg.y" /* yacc.c:1646  */
    { UFDBglobalURLlookupResultFatalError = (yyvsp[0].integer); }
#line 2280 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 323 "sg.y" /* yacc.c:1646  */
    { UFDBglobalURLlookupDelayDBreload = (yyvsp[0].integer); }
#line 2286 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 327 "sg.y" /* yacc.c:1646  */
    { ufdbFree( UFDBglobalSquidVersion );  UFDBglobalSquidVersion = (yyvsp[0].string);  }
#line 2292 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 332 "sg.y" /* yacc.c:1646  */
    { int new_n_workers;
					  /* ONLY increase #workers */
					  new_n_workers = atoi( (yyvsp[0].string) );  
					  if (new_n_workers < 32)
					  {
					     ufdbLogError( "num-worker-threads must be at least 32" );
					     new_n_workers = 32;
					  }
					  else if (new_n_workers > UFDB_MAX_THREADS)
					  {
					     ufdbLogError( "num-worker-threads can be at most %d", UFDB_MAX_THREADS );
					     new_n_workers = UFDB_MAX_THREADS;
					  }
					  if (new_n_workers > UFDBglobalNworkers)
					     UFDBglobalNworkers = new_n_workers;
					}
#line 2313 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 352 "sg.y" /* yacc.c:1646  */
    { UFDBglobalUploadCrashReports = (yyvsp[0].integer);  }
#line 2319 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 357 "sg.y" /* yacc.c:1646  */
    { UFDBglobalLookupReverseIP = (yyvsp[0].integer);  }
#line 2325 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 362 "sg.y" /* yacc.c:1646  */
    { UFDBglobalUseAlsoIPv6onWan = (yyvsp[0].integer);  }
#line 2331 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 367 "sg.y" /* yacc.c:1646  */
    { UFDBglobalSquidUsesSSLbump = (yyvsp[0].integer);  }
#line 2337 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 371 "sg.y" /* yacc.c:1646  */
    { UFDBglobalLogURLdetails = (yyvsp[0].integer); }
#line 2343 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 375 "sg.y" /* yacc.c:1646  */
    { UFDBglobalShowURLdetails = (yyvsp[0].integer); }
#line 2349 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 379 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebugSkype = (yyvsp[0].integer); }
#line 2355 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 383 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebugGtalk = (yyvsp[0].integer); }
#line 2361 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 387 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebugYahooMsg = (yyvsp[0].integer); }
#line 2367 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 391 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebugAim = (yyvsp[0].integer); }
#line 2373 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 395 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebugFBchat = (yyvsp[0].integer); }
#line 2379 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 399 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebugCitrixOnline = (yyvsp[0].integer); }
#line 2385 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 403 "sg.y" /* yacc.c:1646  */
    { UFDBglobalExpressionOptimisation = (yyvsp[0].integer); }
#line 2391 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 407 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebugRegexp = (yyvsp[0].integer); }
#line 2397 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 411 "sg.y" /* yacc.c:1646  */
    { UFDBglobalDebugExternalScripts = (yyvsp[0].integer); }
#line 2403 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 415 "sg.y" /* yacc.c:1646  */
    { ufdbFree( UFDBglobalEmailServer ); UFDBglobalEmailServer = (yyvsp[0].string); }
#line 2409 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 419 "sg.y" /* yacc.c:1646  */
    { ufdbFree( UFDBglobalMyHostname ); UFDBglobalMyHostname = (yyvsp[0].string); }
#line 2415 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 423 "sg.y" /* yacc.c:1646  */
    { ufdbFree( UFDBglobalAdminEmail ); UFDBglobalAdminEmail = (yyvsp[0].string); }
#line 2421 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 427 "sg.y" /* yacc.c:1646  */
    { ufdbFree( UFDBglobalSenderEmail ); UFDBglobalSenderEmail = (yyvsp[0].string); }
#line 2427 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 431 "sg.y" /* yacc.c:1646  */
    {  ufdbFree( UFDBglobalExternalStatusCommand );  UFDBglobalExternalStatusCommand = (yyvsp[0].string); }
#line 2433 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 435 "sg.y" /* yacc.c:1646  */
    { ufdbSetting("logdir",(yyvsp[0].string)); }
#line 2439 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 436 "sg.y" /* yacc.c:1646  */
    { ufdbSetting("logdir",(yyvsp[0].string)); }
#line 2445 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 440 "sg.y" /* yacc.c:1646  */
    { ufdbSetting("pidfile",(yyvsp[0].string)); }
#line 2451 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 441 "sg.y" /* yacc.c:1646  */
    { ufdbSetting("pidfile",(yyvsp[0].string)); }
#line 2457 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 445 "sg.y" /* yacc.c:1646  */
    { ufdbSetting( "port", (yyvsp[0].string) ); }
#line 2463 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 449 "sg.y" /* yacc.c:1646  */
    { 
#if HAVE_UNIX_SOCKETS
                                             ufdbLogError( "ufdbguardd is configured to use UNIX sockets.  \"interface\" is ignored." );
#else
		                             if (strcmp((yyvsp[0].string),"all")== 0)
						strcpy( UFDBglobalInterface, "all" );    
					     else
					        ufdbLogFatalError( "interface must be \"all\" or IP address" );
#endif
					     ufdbFree( (yyvsp[0].string) );
					   }
#line 2479 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 460 "sg.y" /* yacc.c:1646  */
    { strcpy( UFDBglobalInterface, (yyvsp[0].string) );  ufdbFree( (yyvsp[0].string) ); }
#line 2485 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 464 "sg.y" /* yacc.c:1646  */
    { ufdbSetCPU( (yyvsp[0].string) ); ufdbFree( (yyvsp[0].string) ); }
#line 2491 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 465 "sg.y" /* yacc.c:1646  */
    { ufdbSetCPU( (yyvsp[0].string) ); ufdbFree( (yyvsp[0].string) ); }
#line 2497 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 470 "sg.y" /* yacc.c:1646  */
    { UFDBglobalUploadStats = (yyvsp[0].integer);   }
#line 2503 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 475 "sg.y" /* yacc.c:1646  */
    { strcpy( UFDBglobalRedirectHttps, (yyvsp[0].string) );  ufdbFree( (yyvsp[0].string) );  }
#line 2509 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 480 "sg.y" /* yacc.c:1646  */
    { strcpy( UFDBglobalRedirectBumpedHttps, (yyvsp[0].string) );  ufdbFree( (yyvsp[0].string) );  }
#line 2515 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 485 "sg.y" /* yacc.c:1646  */
    { strcpy( UFDBglobalLoadingDatabaseRedirect, (yyvsp[0].string) );  ufdbFree( (yyvsp[0].string) );  }
#line 2521 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 490 "sg.y" /* yacc.c:1646  */
    { strcpy( UFDBglobalFatalErrorRedirect, (yyvsp[0].string) );  ufdbFree( (yyvsp[0].string) );  }
#line 2527 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 495 "sg.y" /* yacc.c:1646  */
    {
			UFDBglobalLogUncategorisedURLs = (yyvsp[0].integer);   
		     }
#line 2535 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 502 "sg.y" /* yacc.c:1646  */
    { 
			UFDBglobalAnalyseUncategorisedURLs = (yyvsp[0].integer);   
		     }
#line 2543 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 507 "sg.y" /* yacc.c:1646  */
    {
			UFDBglobalAnalyseUncategorisedURLs = atoi( (yyvsp[0].string) ); 
			ufdbFree( (yyvsp[0].string) ); 
		     }
#line 2552 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 515 "sg.y" /* yacc.c:1646  */
    {
		        UFDBglobalStripDomainFromUsername = (yyvsp[0].integer);
		     }
#line 2560 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 522 "sg.y" /* yacc.c:1646  */
    { UFDBglobalSafeSearch = (yyvsp[0].integer); }
#line 2566 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 527 "sg.y" /* yacc.c:1646  */
    { 
		       UFDBglobalMaxLogfileSize = strtoul( (yyvsp[0].string), NULL, 10 );
		       ufdbFree( (yyvsp[0].string) );
		       if (UFDBglobalMaxLogfileSize < 2 * 1024 * 1024)		/* minimum is 2 MB */
		          UFDBglobalMaxLogfileSize = 2 * 1024 * 1024;
		       if (UFDBglobalMaxLogfileSize > 2000000000)		/* maximum is 2 GB */
		          UFDBglobalMaxLogfileSize = 2000000000;
		     }
#line 2579 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 538 "sg.y" /* yacc.c:1646  */
    { UFDBglobalHttpdPort = atoi( (yyvsp[0].string) ); ufdbFree( (yyvsp[0].string) ); }
#line 2585 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 539 "sg.y" /* yacc.c:1646  */
    { if (strcmp((yyvsp[0].string),"all")== 0)
						strcpy( UFDBglobalHttpdInterface, "all" );    
					     else
					        ufdbLogFatalError( "http-server interface must be \"all\" or IP address" );
					     ufdbFree( (yyvsp[0].string) );
					   }
#line 2596 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 545 "sg.y" /* yacc.c:1646  */
    { if (strcmp((yyvsp[0].string),"all")== 0)
						strcpy( UFDBglobalHttpdInterface, "all" );    
					     else
					        ufdbLogFatalError( "http-server interface must be \"all\" or IP address" );
					     ufdbFree( (yyvsp[0].string) );
					   }
#line 2607 "y.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 551 "sg.y" /* yacc.c:1646  */
    { strcpy( UFDBglobalHttpdInterface, (yyvsp[0].string) );       ufdbFree( (yyvsp[0].string) ); }
#line 2613 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 552 "sg.y" /* yacc.c:1646  */
    { strcpy( UFDBglobalHttpdImagesDirectory, (yyvsp[0].string) ); ufdbFree( (yyvsp[0].string) ); }
#line 2619 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 553 "sg.y" /* yacc.c:1646  */
    { strcpy( UFDBglobalHttpdImagesDirectory, (yyvsp[0].string) ); ufdbFree( (yyvsp[0].string) ); }
#line 2625 "y.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 566 "sg.y" /* yacc.c:1646  */
    { ufdbCategory( (yyvsp[0].string) ); }
#line 2631 "y.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 567 "sg.y" /* yacc.c:1646  */
    { ufdbCategory( ufdbStrdup("aggressive") );
		                         yyerror( (char *) "\"aggressive\" is a keyword and must be surrounded by quotes" ); }
#line 2638 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 569 "sg.y" /* yacc.c:1646  */
    { ufdbCategory( ufdbStrdup("allow") );
		                         yyerror( (char *) "\"allow\" is a keyword and must be surrounded by quotes" ); }
#line 2645 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 571 "sg.y" /* yacc.c:1646  */
    { ufdbCategory( ufdbStrdup("deny") );
		                         yyerror( (char *) "\"deny\" is a keyword and must be surrounded by quotes" ); }
#line 2652 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 573 "sg.y" /* yacc.c:1646  */
    { ufdbCategory( ufdbStrdup("syntax-error") );
		                         yyerror( (char *) "erroneous category definition.  Perhaps the category ID is a reserved word?" ); }
#line 2659 "y.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 579 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryEnd(); }
#line 2665 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 587 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryDomainList( (yyvsp[0].string) ); }
#line 2671 "y.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 588 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryDomainList( (yyvsp[0].string) ); }
#line 2677 "y.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 589 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryDomainList( NULL ); }
#line 2683 "y.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 590 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryExecDomainList( (yyvsp[0].string) ); }
#line 2689 "y.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 591 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryUrlList( (yyvsp[0].string) ); }
#line 2695 "y.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 592 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryUrlList( (yyvsp[0].string) ); }
#line 2701 "y.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 593 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryUrlList( NULL ); }
#line 2707 "y.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 594 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryExpressionList( (yyvsp[0].string), "n" ); }
#line 2713 "y.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 595 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryExpressionList( (yyvsp[0].string), "n" ); }
#line 2719 "y.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 596 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryExpressionList( NULL, NULL ); }
#line 2725 "y.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 597 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryExpressionList( (yyvsp[0].string), "i" ); }
#line 2731 "y.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 598 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryExpressionList( (yyvsp[0].string), "i" ); }
#line 2737 "y.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 599 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryExpressionList( (yyvsp[0].string), "i" ); }
#line 2743 "y.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 600 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryExpressionList( (yyvsp[0].string), "i" ); }
#line 2749 "y.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 601 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryCACertsFile( (yyvsp[0].string) ); }
#line 2755 "y.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 602 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryCACertsFile( (yyvsp[0].string) ); }
#line 2761 "y.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 603 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryCACertsDir( (yyvsp[0].string) ); }
#line 2767 "y.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 604 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryCACertsDir( (yyvsp[0].string) ); }
#line 2773 "y.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 605 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryRedirect( (yyvsp[0].string) ); }
#line 2779 "y.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 606 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryRedirect( (yyvsp[0].string) ); }
#line 2785 "y.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 607 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryRewrite( (yyvsp[0].string) ); }
#line 2791 "y.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 608 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryRewrite( (yyvsp[0].string) ); }
#line 2797 "y.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 609 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryTime( (yyvsp[0].string), UFDB_ACL_WITHIN ); }
#line 2803 "y.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 610 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryTime( (yyvsp[0].string), UFDB_ACL_OUTSIDE ); }
#line 2809 "y.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 611 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryBlockConnect( (yyvsp[0].integer) ); }
#line 2815 "y.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 612 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_SAFE_SEARCH );  }
#line 2821 "y.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 613 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_YOUTUBE_EDUFILTER );  }
#line 2827 "y.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 614 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption(  1, UFDB_OPT_HTTPS_WITH_HOSTNAME );  }
#line 2833 "y.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 615 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_HTTPS_WITH_HOSTNAME );  }
#line 2839 "y.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 616 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption(  1, UFDB_OPT_HTTPS_OFFICAL_CERTIFICATE );  }
#line 2845 "y.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 617 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_HTTPS_OFFICAL_CERTIFICATE );  }
#line 2851 "y.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 618 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_PROHIBIT_INSECURE_SSLV2 );  }
#line 2857 "y.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 619 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_PROHIBIT_INSECURE_SSLV3 );  }
#line 2863 "y.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 620 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_SKYPE_OVER_HTTPS );  }
#line 2869 "y.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 621 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_GTALK_OVER_HTTPS );  }
#line 2875 "y.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 622 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_YAHOOMSG_OVER_HTTPS );  }
#line 2881 "y.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 623 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_AIM_OVER_HTTPS );  }
#line 2887 "y.tab.c" /* yacc.c:1646  */
    break;

  case 135:
#line 624 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_FBCHAT_OVER_HTTPS );  }
#line 2893 "y.tab.c" /* yacc.c:1646  */
    break;

  case 136:
#line 625 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_CITRIXONLINE_OVER_HTTPS );  }
#line 2899 "y.tab.c" /* yacc.c:1646  */
    break;

  case 137:
#line 626 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_ANYDESK_OVER_HTTPS );  }
#line 2905 "y.tab.c" /* yacc.c:1646  */
    break;

  case 138:
#line 627 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_TEAMVIEWER_OVER_HTTPS );  }
#line 2911 "y.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 628 "sg.y" /* yacc.c:1646  */
    { ufdbCategoryOption( (yyvsp[0].integer), UFDB_OPT_UNKNOWN_PROTOCOL_OVER_HTTPS );  }
#line 2917 "y.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 629 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
		                                  ufdbFree( (yyvsp[0].string) ); }
#line 2924 "y.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 631 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
		                                  ufdbFree( (yyvsp[0].string) ); }
#line 2931 "y.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 633 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
		                                  ufdbFree( (yyvsp[0].string) ); }
#line 2938 "y.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 635 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
		                                  ufdbFree( (yyvsp[0].string) ); }
#line 2945 "y.tab.c" /* yacc.c:1646  */
    break;

  case 144:
#line 640 "sg.y" /* yacc.c:1646  */
    { defSource( (yyvsp[0].string) ); }
#line 2951 "y.tab.c" /* yacc.c:1646  */
    break;

  case 145:
#line 644 "sg.y" /* yacc.c:1646  */
    { defSourceEnd(); }
#line 2957 "y.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 656 "sg.y" /* yacc.c:1646  */
    { ufdbSourceUserList( (yyvsp[0].string) ); }
#line 2963 "y.tab.c" /* yacc.c:1646  */
    break;

  case 152:
#line 657 "sg.y" /* yacc.c:1646  */
    { ufdbSourceUserList( (yyvsp[0].string) ); }
#line 2969 "y.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 658 "sg.y" /* yacc.c:1646  */
    { ufdbSourceUserList( (yyvsp[0].string) ); }
#line 2975 "y.tab.c" /* yacc.c:1646  */
    break;

  case 154:
#line 659 "sg.y" /* yacc.c:1646  */
    { ufdbSourceUserList( (yyvsp[0].string) ); }
#line 2981 "y.tab.c" /* yacc.c:1646  */
    break;

  case 155:
#line 660 "sg.y" /* yacc.c:1646  */
    { ufdbSourceExecUserList( (yyvsp[0].string) ); }
#line 2987 "y.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 661 "sg.y" /* yacc.c:1646  */
    { ufdbSourceGroup( UFDB_GROUPTYPE_UNIX, (yyvsp[0].string) ); }
#line 2993 "y.tab.c" /* yacc.c:1646  */
    break;

  case 157:
#line 662 "sg.y" /* yacc.c:1646  */
    { ufdbSourceGroup( UFDB_GROUPTYPE_UNIX, (yyvsp[0].string) ); }
#line 2999 "y.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 663 "sg.y" /* yacc.c:1646  */
    { ufdbSourceGroup( UFDB_GROUPTYPE_UNIX, (yyvsp[0].string) ); }
#line 3005 "y.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 664 "sg.y" /* yacc.c:1646  */
    { ufdbSourceGroup( UFDB_GROUPTYPE_UNIX, (yyvsp[0].string) ); }
#line 3011 "y.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 665 "sg.y" /* yacc.c:1646  */
    { ufdbSourceUserQuota( (yyvsp[-2].string), (yyvsp[-1].string), "3600" );  
		                                   ufdbFree( (yyvsp[-2].string) ); ufdbFree( (yyvsp[-1].string) ); }
#line 3018 "y.tab.c" /* yacc.c:1646  */
    break;

  case 161:
#line 667 "sg.y" /* yacc.c:1646  */
    { ufdbSourceUserQuota( (yyvsp[-2].string), (yyvsp[-1].string), "86400" );  
		                                   ufdbFree( (yyvsp[-2].string) ); ufdbFree( (yyvsp[-1].string) ); }
#line 3025 "y.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 669 "sg.y" /* yacc.c:1646  */
    { ufdbSourceUserQuota( (yyvsp[-2].string), (yyvsp[-1].string), "604800" );  
		                                   ufdbFree( (yyvsp[-2].string) ); ufdbFree( (yyvsp[-1].string) ); }
#line 3032 "y.tab.c" /* yacc.c:1646  */
    break;

  case 163:
#line 671 "sg.y" /* yacc.c:1646  */
    { ufdbSourceUserQuota( (yyvsp[-2].string), (yyvsp[-1].string), (yyvsp[0].string) );  
		                                   ufdbFree( (yyvsp[-2].string) ); ufdbFree( (yyvsp[-1].string) ); ufdbFree( (yyvsp[0].string) ); }
#line 3039 "y.tab.c" /* yacc.c:1646  */
    break;

  case 165:
#line 674 "sg.y" /* yacc.c:1646  */
    { defSourceIPV4List( (yyvsp[0].string) ); }
#line 3045 "y.tab.c" /* yacc.c:1646  */
    break;

  case 166:
#line 675 "sg.y" /* yacc.c:1646  */
    { defSourceIPV4List( (yyvsp[0].string) ); }
#line 3051 "y.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 677 "sg.y" /* yacc.c:1646  */
    { defSourceIPV6List( (yyvsp[0].string) ); }
#line 3057 "y.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 678 "sg.y" /* yacc.c:1646  */
    { defSourceIPV6List( (yyvsp[0].string) ); }
#line 3063 "y.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 679 "sg.y" /* yacc.c:1646  */
    { ufdbSourceEval( UFDB_EVAL_AND ); }
#line 3069 "y.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 680 "sg.y" /* yacc.c:1646  */
    { ufdbSourceEval( UFDB_EVAL_OR ); }
#line 3075 "y.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 681 "sg.y" /* yacc.c:1646  */
    { defSourceTime( (yyvsp[0].string), UFDB_ACL_WITHIN ); }
#line 3081 "y.tab.c" /* yacc.c:1646  */
    break;

  case 173:
#line 682 "sg.y" /* yacc.c:1646  */
    { defSourceTime( (yyvsp[0].string), UFDB_ACL_OUTSIDE ); }
#line 3087 "y.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 683 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
						   ufdbFree( (yyvsp[0].string) ); }
#line 3094 "y.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 685 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
						   ufdbFree( (yyvsp[0].string) ); }
#line 3101 "y.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 687 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
						   ufdbFree( (yyvsp[0].string) ); }
#line 3108 "y.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 689 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
						   ufdbFree( (yyvsp[0].string) ); }
#line 3115 "y.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 691 "sg.y" /* yacc.c:1646  */
    { lastSource->cont_search = 1; }
#line 3121 "y.tab.c" /* yacc.c:1646  */
    break;

  case 180:
#line 697 "sg.y" /* yacc.c:1646  */
    { defSourceDomain( (yyvsp[0].string) ); }
#line 3127 "y.tab.c" /* yacc.c:1646  */
    break;

  case 181:
#line 698 "sg.y" /* yacc.c:1646  */
    { defSourceDomain( (yyvsp[0].string) ); }
#line 3133 "y.tab.c" /* yacc.c:1646  */
    break;

  case 184:
#line 704 "sg.y" /* yacc.c:1646  */
    { ufdbSourceUser( (yyvsp[0].string) ); }
#line 3139 "y.tab.c" /* yacc.c:1646  */
    break;

  case 185:
#line 705 "sg.y" /* yacc.c:1646  */
    { ufdbSourceUser( (yyvsp[0].string) ); }
#line 3145 "y.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 718 "sg.y" /* yacc.c:1646  */
    { sgAcl( (yyvsp[-1].string), NULL, UFDB_ACL_NONE );  }
#line 3151 "y.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 719 "sg.y" /* yacc.c:1646  */
    { sgAcl( (yyvsp[-3].string), (yyvsp[-1].string), UFDB_ACL_WITHIN );  }
#line 3157 "y.tab.c" /* yacc.c:1646  */
    break;

  case 192:
#line 720 "sg.y" /* yacc.c:1646  */
    { sgAcl( (yyvsp[-3].string), (yyvsp[-1].string), UFDB_ACL_OUTSIDE ); }
#line 3163 "y.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 726 "sg.y" /* yacc.c:1646  */
    { sgAcl( NULL, NULL, UFDB_ACL_ELSE );    }
#line 3169 "y.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 736 "sg.y" /* yacc.c:1646  */
    { if (lastAcl != NULL  &&  lastAcl->pass == NULL) 
                                                  {
                                                     ufdbLogMessage( "line %d: acl has an empty pass statement.  Adding 'any'.", lineno );
                                                     sgAclSetValue( "pass", ufdbStrdup("any"), 1 ); 
                                                  }
                                                }
#line 3180 "y.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 742 "sg.y" /* yacc.c:1646  */
    { sgAclSetValue( "rewrite", (yyvsp[0].string), 0 ); }
#line 3186 "y.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 743 "sg.y" /* yacc.c:1646  */
    { sgAclSetValue( "rewrite", (yyvsp[0].string), 0 ); }
#line 3192 "y.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 744 "sg.y" /* yacc.c:1646  */
    { sgAclSetValue( "redirect", (yyvsp[0].string), 0 ); }
#line 3198 "y.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 745 "sg.y" /* yacc.c:1646  */
    { sgAclSetValue( "redirect", (yyvsp[0].string), 0 ); }
#line 3204 "y.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 746 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
						  ufdbFree( (yyvsp[0].string) ); }
#line 3211 "y.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 748 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
						  ufdbFree( (yyvsp[0].string) ); }
#line 3218 "y.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 750 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
						  ufdbFree( (yyvsp[0].string) ); }
#line 3225 "y.tab.c" /* yacc.c:1646  */
    break;

  case 206:
#line 752 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 
						  ufdbFree( (yyvsp[0].string) ); }
#line 3232 "y.tab.c" /* yacc.c:1646  */
    break;

  case 208:
#line 758 "sg.y" /* yacc.c:1646  */
    { yyerror( (char *) "\"aggressive\" is a keyword and must be surrounded by quotes" );  }
#line 3238 "y.tab.c" /* yacc.c:1646  */
    break;

  case 209:
#line 759 "sg.y" /* yacc.c:1646  */
    { sgAclSetValue( "pass", (yyvsp[0].string), 1 ); }
#line 3244 "y.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 760 "sg.y" /* yacc.c:1646  */
    { yyerror( (char *) "\"aggressive\" is a keyword and must be surrounded by quotes" );  }
#line 3250 "y.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 761 "sg.y" /* yacc.c:1646  */
    { sgAclSetValue( "pass", (yyvsp[0].string), 0 ); }
#line 3256 "y.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 766 "sg.y" /* yacc.c:1646  */
    { sgIpv4( (yyvsp[0].string) ); ufdbFree( (yyvsp[0].string) ); }
#line 3262 "y.tab.c" /* yacc.c:1646  */
    break;

  case 214:
#line 770 "sg.y" /* yacc.c:1646  */
    { sgIpv4( (yyvsp[0].string) ); ufdbFree( (yyvsp[0].string) ); }
#line 3268 "y.tab.c" /* yacc.c:1646  */
    break;

  case 216:
#line 775 "sg.y" /* yacc.c:1646  */
    { sgIpv4( "255.255.255.255" );  sgSetIpType( SG_IPTYPE_HOST, NULL, 0 ); }
#line 3274 "y.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 776 "sg.y" /* yacc.c:1646  */
    { sgSetIpType( SG_IPTYPE_CIDR, NULL, 0 ); }
#line 3280 "y.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 777 "sg.y" /* yacc.c:1646  */
    { sgSetIpType( SG_IPTYPE_CLASS, NULL, 0 ); }
#line 3286 "y.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 778 "sg.y" /* yacc.c:1646  */
    { sgSetIpType( SG_IPTYPE_RANGE, NULL, 0 ); }
#line 3292 "y.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 783 "sg.y" /* yacc.c:1646  */
    { sgIpv4( (yyvsp[0].string) );  ufdbFree( (yyvsp[0].string) );  }
#line 3298 "y.tab.c" /* yacc.c:1646  */
    break;

  case 225:
#line 793 "sg.y" /* yacc.c:1646  */
    { sgIpv6( (yyvsp[0].string), SG_IPV6TYPE_HOST, UFDBglobalConfigFile, lineno );
                                                  ufdbFree( (yyvsp[0].string) );  }
#line 3305 "y.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 795 "sg.y" /* yacc.c:1646  */
    { sgIpv6( (yyvsp[0].string), SG_IPV6TYPE_CIDR, UFDBglobalConfigFile, lineno );
                                                  ufdbFree( (yyvsp[0].string) );  }
#line 3312 "y.tab.c" /* yacc.c:1646  */
    break;

  case 227:
#line 800 "sg.y" /* yacc.c:1646  */
    { sgRewrite( (yyvsp[0].string) ); }
#line 3318 "y.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 801 "sg.y" /* yacc.c:1646  */
    { sgRewrite( (yyvsp[0].string) ); }
#line 3324 "y.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 815 "sg.y" /* yacc.c:1646  */
    { sgRewriteSubstitute( (yyvsp[0].string) ); ufdbFree( (yyvsp[0].string) ); }
#line 3330 "y.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 816 "sg.y" /* yacc.c:1646  */
    { sgRewriteTime( (yyvsp[0].string), UFDB_ACL_WITHIN ); }
#line 3336 "y.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 817 "sg.y" /* yacc.c:1646  */
    { sgRewriteTime( (yyvsp[0].string), UFDB_ACL_OUTSIDE ); }
#line 3342 "y.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 818 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 							      ufdbFree( (yyvsp[0].string) ); }
#line 3348 "y.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 819 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 							      ufdbFree( (yyvsp[0].string) ); }
#line 3354 "y.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 820 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 							      ufdbFree( (yyvsp[0].string) ); }
#line 3360 "y.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 821 "sg.y" /* yacc.c:1646  */
    { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, (yyvsp[0].string) ); 							      ufdbFree( (yyvsp[0].string) ); }
#line 3366 "y.tab.c" /* yacc.c:1646  */
    break;

  case 239:
#line 826 "sg.y" /* yacc.c:1646  */
    { sgTime( (yyvsp[0].string) ); }
#line 3372 "y.tab.c" /* yacc.c:1646  */
    break;

  case 243:
#line 840 "sg.y" /* yacc.c:1646  */
    { sgTimeElementInit(); }
#line 3378 "y.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 840 "sg.y" /* yacc.c:1646  */
    { sgTimeElementAdd((yyvsp[0].string),T_WEEKDAY); }
#line 3384 "y.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 841 "sg.y" /* yacc.c:1646  */
    { sgTimeElementInit(); }
#line 3390 "y.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 841 "sg.y" /* yacc.c:1646  */
    { sgTimeElementAdd((yyvsp[0].string),T_WEEKLY); }
#line 3396 "y.tab.c" /* yacc.c:1646  */
    break;

  case 249:
#line 842 "sg.y" /* yacc.c:1646  */
    { sgTimeElementInit(); }
#line 3402 "y.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 842 "sg.y" /* yacc.c:1646  */
    { sgTimeElementAdd((yyvsp[0].string),T_WEEKLY); }
#line 3408 "y.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 843 "sg.y" /* yacc.c:1646  */
    { sgTimeElementInit(); }
#line 3414 "y.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 843 "sg.y" /* yacc.c:1646  */
    { sgTimeElementEnd(); }
#line 3420 "y.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 844 "sg.y" /* yacc.c:1646  */
    { ufdbLogFatalError( "invalid time specification at line %d", lineno );   }
#line 3426 "y.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 848 "sg.y" /* yacc.c:1646  */
    { sgTimeElementClone(); }
#line 3432 "y.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 861 "sg.y" /* yacc.c:1646  */
    { sgTimeElementAdd( (yyvsp[0].string), T_DVAL ); }
#line 3438 "y.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 864 "sg.y" /* yacc.c:1646  */
    { sgTimeElementAdd( (yyvsp[0].tval), T_TVAL ); }
#line 3444 "y.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 867 "sg.y" /* yacc.c:1646  */
    { sgTimeElementAdd( (yyvsp[0].string), T_DVALCRON ); }
#line 3450 "y.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 871 "sg.y" /* yacc.c:1646  */
    {  yyerror( (char *) "syntax error" );  }
#line 3456 "y.tab.c" /* yacc.c:1646  */
    break;


#line 3460 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 950 "sg.y" /* yacc.c:1906  */


int sgReadConfig( 
   char *            file )
{
   char *            defaultFile = (char *) DEFAULT_CONFIGFILE;
   struct Source *   source;
   struct Category * cat;
   struct Acl *      acl;

   lineno = 1;
   ufdbResetCPUs();

   UFDBglobalConfigFile = file;
   if (UFDBglobalConfigFile == NULL)
      UFDBglobalConfigFile = defaultFile;

   yyin = fopen( UFDBglobalConfigFile, "r" );
   if (yyin == NULL) 
   {
      syslog( LOG_ALERT, "%s: cannot open configuration file %s", UFDBprogname, UFDBglobalConfigFile );
      ufdbLogFatalError( "cannot open configuration file %s", UFDBglobalConfigFile );
      return 0;
   }

   ufdbLogMessage( "configuration file: %s", UFDBglobalConfigFile );

   /* keep the old HTTPS redirection URL.  It is most likely that we need it during the database reload */
   /* dot not reset UFDBglobalRedirectHttps and UFDBglobalRedirectBumpedHttps. */

   /* UFDBglobalDebug = 0; */
   strcpy( UFDBglobalInterface, "all" );
   /* UFDBglobalPortNum = UFDB_DAEMON_PORT;  the port can be set with a command line switch so do not reset it */
   /* UFDBglobalNworkers = 68;  do not reset this variable. */
   UFDBglobalDebugHttpd = 0;
   UFDBglobalLogPass = 0;
   UFDBglobalLogBlock = 0;
   UFDBglobalLogAllRequests = 0;
   UFDBglobalYoutubeEdufilter = 0;
   UFDBglobalYoutubeEdufilterID = (char *) "none";
   UFDBglobalAllowGoogleHTTPSusingIP = 0;
   UFDBglobalDenyMode = 1;
   UFDBglobalReuseAclNames = 1;

#if 0
   /* When a database is reloaded, these variables may NOT be reset since they are used during the reload !! */
   UFDBglobalURLlookupDelayDBreload = 0;
   UFDBglobalURLlookupResultDBreload = UFDB_ALLOW;
   UFDBglobalURLlookupResultFatalError = UFDB_ALLOW;
   strcpy( UFDBglobalLoadingDatabaseRedirect, "a-blocking-url" );
   ufdbFree( UFDBglobalSquidVersion );
#endif
   if (UFDBglobalSquidVersion == NULL)
      UFDBglobalSquidVersion = ufdbStrdup( UFDB_DEFAULT_SQUID_VERSION );

   UFDBglobalShowURLdetails = 0;
   UFDBglobalLogURLdetails = 0;
   UFDBglobalUploadCrashReports = 1;
   UFDBglobalLookupReverseIP = 0;
   UFDBglobalUseAlsoIPv6onWan = 1;
   UFDBglobalDebugSkype = 0;
   UFDBglobalDebugGtalk = 0;
   UFDBglobalDebugYahooMsg = 0;
   UFDBglobalDebugAim = 0;
   UFDBglobalDebugFBchat = 0;
   UFDBglobalDebugCitrixOnline = 0;
   UFDBglobalDebugRegexp = 0;
   UFDBglobalDebugExternalScripts = 0;
   UFDBglobalExpressionOptimisation = 1;
   UFDBglobalRefreshUserlistInterval = UFDB_DEFAULT_REFRESH_USERLIST;
   UFDBglobalRefreshDomainlistInterval = UFDB_DEFAULT_REFRESH_DOMAINLIST;
   UFDBglobalAnalyseUncategorisedURLs = 1;
   UFDBglobalLogUncategorisedURLs = 0;
   UFDBglobalUploadStats = 1;
   UFDBglobalSafeSearch = 1;
   UFDBglobalSkipSafeCategory = 0;
   UFDBglobalStripDomainFromUsername = 0;
   UFDBglobalTunnelCheckMethod = UFDB_API_HTTPS_CHECK_OFF;
   /* do not reset: UFDBglobalSquidUsesSSLbump = 0; */
   UFDBglobalHttpsWithHostname = 0;
   UFDBglobalHttpsOfficialCertificate = 0;
   UFDBglobalUnknownProtocolOverHttps = 1;
   UFDBglobalHttpsNoSSLv2 = 1;
   UFDBglobalHttpsNoSSLv3 = 0;
   UFDBglobalHttpdPort = 0;
   UFDBglobalDateOfCheckedDB[0] = '\0';
   strcpy( UFDBglobalHttpdInterface, "all" );
   strcpy( UFDBglobalHttpdImagesDirectory, "." );
 
   UFDBglobalDatabaseStatus = UFDB_API_STATUS_DATABASE_OK;

   (void) yyparse();		/* parse the configuration file */
   fclose( yyin );

   UFDBglobalDatabaseLoadTime = time( NULL );

   /*
    * For analysis of uncategorised URLs we also load a "checked" category
    * that contains URLs that are reviewed and checked not to be part of
    * any other category.
    */
   {
      char * dbhome;
      char   dbfname[1024];

      dbhome = ufdbSettingGetValue( (char *) "dbhome" );
      if (dbhome == NULL)
         dbhome = (char *) DEFAULT_DBHOME;

      /* TODO: check if "checked" was not already loaded... */
      if (UFDBglobalAnalyseUncategorisedURLs > 0)
      {
	 sprintf( dbfname, "%s/checked/domains.ufdb", dbhome );
	 if (UFDBloadDatabase( &UFDBglobalCheckedDB, dbfname ) != UFDB_API_OK)
	 {
	    UFDBglobalCheckedDB.mem = NULL;
	    UFDBglobalCheckedDB.index = NULL;
	    UFDBglobalCheckedDB.table.nNextLevels = 0;
	    if (UFDBglobalDebug)
	       ufdbLogMessage( "The URL database appears not be from URLfilterDB. No problem." );
	 }
	 else
	 {
	    sprintf( dbfname, "%s/checked/expressions", dbhome );
	    (void) UFDBloadExpressions( &UFDBglobalCheckedExpressions, dbfname );
	    ufdbLogMessage( "The implicitly allowed URL category 'checked' is loaded." );
	 }
      }
   }

   if (defaultAcl == NULL)
      ufdbLogFatalError( "\"default\" ACL is not defined" );
   else if (defaultAcl->pass == NULL)
      ufdbLogError( "\"default\" ACL is empty: by default all URLs are blocked." );

   /* A configuration file may have a source definition which is not used in the ACL list.
    * This raises the question "what to do with this ?".
    * If we do nothing, the source is matched but has no ACL definition and is silently skipped.
    * Most likely the administrator overlooked something so a warning is useful.
    */
   source = UFDBgetSourceList();  
   if (source == NULL  &&  defaultAcl == NULL)
      ufdbLogFatalError( "There are no sources and there is no default ACL" );

   for ( ; source != NULL;  source = source->next)
   {
      int found;

      found = 0;
      for (acl = AclList;  acl != NULL;  acl = acl->next)
      {
         if (strcmp( acl->name, source->name ) == 0)
	 {
	    found = 1;
	    break;
	 }
      }
      if (!found)
      {
         ufdbLogError( "source \"%s\" has no ACL.  *****\n"
	               "This most likely may lead to unexpected results when the source is matched.  *****\n"
		       "It is strongly suggested to remove the source definition OR add the source to the acl.  *****",
		       source->name );
	 source->active = 0;
      }
      else if (acl->pass == NULL)
      {
         ufdbLogMessage( "source \"%s\" has an empty \"pass\" in its ACL *****", source->name );
      }
   }

   for (cat = Cat;  cat != NULL;  cat = cat->next)
   {
      if (cat->options == 0  &&  cat->domainlist == NULL  &&  cat->execdomainlist == NULL  && 
          cat->expressionlist == NULL  &&  cat->rewrite == NULL  &&  cat->name != NULL)
      {
         ufdbLogError( "category \"%s\" has no content definition  *****", cat->name );
      }
   }

   if (UFDBglobalAdminEmail != NULL  &&  UFDBglobalEmailServer == NULL)
   {
      ufdbLogError( "No email server is defined; cannot send email to \"%s\"  *****", UFDBglobalAdminEmail );
   }
   if (UFDBglobalAdminEmail != NULL  &&  UFDBglobalEmailServer != NULL  &&  UFDBglobalSenderEmail == NULL)
   {
      ufdbLogMessage( "WARNING: No sender email address is defined; using \"%s\" as sender", UFDBglobalAdminEmail );
   }

   if (strncmp( UFDBglobalSquidVersion, "5.", 2 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "4.", 2 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.5", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.4", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.3", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.2", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.1", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.0", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "2.7", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "2.6", 3 ) != 0)
   {
      ufdbLogFatalError( "Unsupported Squid version \"%s\"  *****\n"
                         "The supported values for squid-version are: "
                         "5.x, 4.x, 3.5, 3.4, 3.3, 3.2, 3.1, 3.0, 2.7, 2.6  *****\n"
			 "If the version of Squid is higher, it is recommended to upgrade ufdbGuard.  *****\n"
			 "Alternatively set squid-version to the highest supported version",
			 UFDBglobalSquidVersion );
   }
   if (strncmp( UFDBglobalSquidVersion, "5.", 2 ) == 0  ||
       strncmp( UFDBglobalSquidVersion, "4.", 2 ) == 0  ||  
       strncmp( UFDBglobalSquidVersion, "3.5", 3 ) == 0  ||  
       strncmp( UFDBglobalSquidVersion, "3.4", 3 ) == 0)
      UFDBglobalSquidHelperProtocol = UFDB_SQUID_HELPER_PROTOCOL3;
   else if (strncmp( UFDBglobalSquidVersion, "2.6", 3 ) == 0  ||  
            strncmp( UFDBglobalSquidVersion, "2.7", 3 ) == 0)
      UFDBglobalSquidHelperProtocol = UFDB_SQUID_HELPER_PROTOCOL1;
   else
      UFDBglobalSquidHelperProtocol = UFDB_SQUID_HELPER_PROTOCOL2;	/* 3.0 - 3.3 */

   BuildImplicitPassLists();

   return 1;
}


static void setRefreshUserlist( int nmin )
{
   if (nmin < 5  ||  nmin > 24*60)
   {
      ufdbLogError( "refreshuserlist must have a value between 5 and %d, resetting to %d.", 
                    24*60, UFDB_DEFAULT_REFRESH_USERLIST );
      nmin = UFDB_DEFAULT_REFRESH_USERLIST;
   }
   UFDBglobalRefreshUserlistInterval = nmin;
}


static void setRefreshDomainlist( int nmin )
{
   if (nmin < 5  ||  nmin > 24*60)
   {
      ufdbLogError( "refreshdomainlist must have a value between 5 and %d, resetting to %d.", 24*60, UFDB_DEFAULT_REFRESH_DOMAINLIST );
      nmin = UFDB_DEFAULT_REFRESH_DOMAINLIST;
   }
   UFDBglobalRefreshDomainlistInterval = nmin;
}


/*
 * Source functions
 */

void defSource( 
   char *          source )
{
   struct Source * sp;

   if (UFDBglobalDebug)
      ufdbLogMessage( "defSource: defining source \"%s\"", source );

   if ((struct Source *) defSourceFindName(source) != NULL)
   {
      ufdbLogFatalError( "line %d: source %s is already defined in configuration file %s",
			 lineno, source, UFDBglobalConfigFile );
      ufdbFree( source );
      return;
   }

   sp = (struct Source *) ufdbMallocAligned( 64, sizeof(struct Source) );
   sp->name = source;
   sp->active = 1;
   sp->evaluationMethod = UFDBglobalDenyMode ? UFDB_EVAL_AND : UFDB_EVAL_OR;
   sp->ipv4 = NULL;
   sp->ipv6 = NULL;
   sp->lastipv4 = NULL;
   sp->lastipv6 = NULL;
   sp->domainDb = NULL;
   sp->userDb = NULL;
   sp->time = NULL;
   sp->within = UFDB_ACL_NONE;
   sp->cont_search = UFDBglobalDenyMode ? 1 : 0;
   sp->sarg0 = NULL;
   sp->userquota.seconds = 0;
   sp->userquota.renew = 0;
   sp->userquota.sporadic = 0;
   sp->next = NULL;
   sp->nblocks = 0;
   sp->nmatches = 0;

   if (UFDBgetSourceList() == NULL)
   {
      if (UFDBglobalDebug)
         ufdbLogMessage( "defSource: SourceList is set to point to \"%s\"", source );
      UFDBsetSourceList( sp );
      lastSource = sp;
   }
   else
   {
      lastSource->next = sp;
      lastSource = sp;
   }
}


void defSourceEnd( void )
{
   struct Source * s;

   s = lastSource;
   if (s->ipv4 == NULL  &&  s->ipv6 == NULL  &&  s->domainDb == NULL  &&  s->userDb == NULL)
   {
      ufdbLogError( "source \"%s\" missing active content, set inactive  *****", s->name );
      s->time = NULL;
      s->active = 0;
   }
}


void ufdbSourceUser( 
   char *               user )
{
   char *               lc;
   struct Source *      sp;

   sp = lastSource;
   if (sp->userDb != NULL  &&  sp->userDb->type != SGDBTYPE_USERLIST)
   {
      ufdbLogFatalError( "user \"%s\" in source \"%s\" on line %d: "
                         "found mix of exec and non-exec userlists", 
                         user, sp->name, lineno );
      return;
   }
   if (sp->userDb == NULL)
   {
      sp->userDb = (struct sgDb *) ufdbMalloc( sizeof(struct sgDb) );
      sp->userDb->dbhome = NULL;
      sp->userDb->dbcp = (void *) UFDBmemDBinit();
      sp->userDb->type = SGDBTYPE_USERLIST;
      sp->userDb->entries = 0;
   }

   for (lc = user; *lc != '\0'; lc++)    /* convert username to lowercase chars */
   {
      if (*lc <= 'Z'  &&  *lc >= 'A')
         *lc += 'a' - 'A';
   }

   if (UFDBglobalDebug)
      ufdbLogMessage( "ufdbSourceUser: adding user \"%s\"", user );

   sp->userDb->entries++;
   UFDBmemDBinsert( (struct UFDBmemDB *) sp->userDb->dbcp, user, NULL );
   ufdbFree( user );
}


static void ufdbSourceUnixGroup(
   char *          groupName  )
{
   struct Source * sp;
   struct group *  grp;
   int             n;
   char *          user;

   sp = lastSource;
   if (sp->userDb != NULL  &&  sp->userDb->type != SGDBTYPE_USERLIST)
   {
      ufdbLogFatalError( "unix group \"%s\" in source \"%s\" on line %d: "
                         "found mix of exec and non-exec userlists", 
                         groupName, sp->name, lineno );
      return;
   }
   if (sp->userDb == NULL) 
   {
      sp->userDb = (struct sgDb *) ufdbMalloc( sizeof(struct sgDb) );
      sp->userDb->dbhome = NULL;
      sp->userDb->dbcp = (void *) UFDBmemDBinit();
      sp->userDb->type = SGDBTYPE_USERLIST;
      sp->userDb->entries = 0;
   }
 
   n = 0;
   setgrent();
   errno = 0;
   grp = getgrnam( groupName );
   if (grp != NULL)
   {
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "found group \"%s\"", grp->gr_name );

      while ((user = grp->gr_mem[n]) != NULL)
      {
	 char * lc;

	 if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "group \"%s\" has user \"%s\"", grp->gr_name, user );
	 for (lc = user; *lc != '\0'; lc++)  	/* convert username to lowercase chars */
	 {
	    if (*lc <= 'Z'  &&  *lc >= 'A')
	       *lc += 'a' - 'A';
	 }
	 UFDBmemDBinsert( (struct UFDBmemDB *) sp->userDb->dbcp, user, NULL );
	 sp->userDb->entries++;
	 n++;
      }
      ufdbLogMessage( "source \"%s\": unix group \"%s\" has %d members", sp->name, groupName, n );
      if (n == 0)
	 ufdbLogMessage( "WARNING: source \"%s\": \"%s\" is an empty group  +++++", sp->name, groupName );
   }
   else
   {
      ufdbLogFatalError( "source \"%s\": \"%s\" is not a unix group", sp->name, groupName );
      if (errno)
	 ufdbLogError( "ufdbSourceUnixGroup: getgrnam returned error %d: %s", errno, strerror(errno) );
   }
   endgrent();
}


static void ufdbSourceGroup( 
   int    groupType,
   char * groupName  )
{
   switch (groupType)
   {
   case UFDB_GROUPTYPE_UNIX:
      ufdbSourceUnixGroup( groupName );
      break;
   default:     
      ufdbLogFatalError( "ufdbSourceGroup: unknown group type %d", groupType );
   }

   ufdbFree( groupName );
}


struct sgDb * UFDBretrieveExecDomainlist( struct Category * cat )
{
   char *        t;
   FILE *        fp;
   struct sgDb * newdb;
   int           exitcode;
   struct stat   Stat0;
   struct stat   Stat1;
   char          line[1024];
   char          basefilename[1024];
   char          fullfilename[1024];
   char          command[3000];

   if (cat == NULL)
   {
      ufdbLogFatalError( "UFDBretrieveExecDomainlist: category is NULL" );
      return NULL;
   }

   if (cat->domainlist == NULL)
   {
      ufdbLogError( "will not execute command for category %s since domainlist is undefined", cat->name );
      return NULL;
   }

   if (cat->execdomainlist == NULL)
   {
      ufdbLogError( "can't execute command for category %s since execdomainlist is undefined", cat->name );
      return NULL;
   }

   if (cat->domainlist[0] == '/')
   {
      strcpy( basefilename, cat->domainlist );
   }
   else
   {
      char * dbhome;
      dbhome = ufdbSettingGetValue( "dbhome" );
      if (dbhome == NULL)
	 dbhome = (char *) DEFAULT_DBHOME;
      sprintf( basefilename, "%s/%s", dbhome, cat->domainlist );
   }
   strcpy( fullfilename, basefilename );
   strcat( fullfilename, UFDBfileSuffix );

   if (stat( fullfilename, &Stat0 ) != 0)
   {
      Stat0.st_mtime = 0;
      ufdbLogMessage( "category %s: pre-execdomainlist: domainlist does not exist (%s)", cat->name, fullfilename );
   }

   /* The script that we will call needs the directory where the files reside,
    * so we set the environment variable UFDB_TABLEDIR for it to change its working directory.
    * (note that we are multithreaded and cannot use chdir() here)
    */
   strcpy( command, "cd " );
   strcat( command, basefilename );
   t = strrchr( command, '/' );
   if (t != NULL)
      *t = '\0';
   strcat( command, " ; " );
   strcat( command, cat->execdomainlist );

   errno = 0;
   if ((fp = popen(command,"r")) == NULL)
   {
      ufdbLogFatalError( "category %s: can't execute command of execdomainlist \"%s\": popen failed: %s  *****", 
                         cat->name, cat->execdomainlist, strerror(errno) );
      return NULL;
   }

   while (UFDBfgetsNoNL(line,sizeof(line),fp) != NULL)
   {
      ufdbLogMessage( "execdomainlist for %s produced: %s", cat->name, line );
   }
   errno = 0;
   exitcode = pclose( fp );

   if (exitcode == -1  &&  errno == ECHILD)
      exitcode = 0;
      /* most likely the child handler thread already did a wait() */

   if (exitcode == 0)
   {
      if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
         ufdbLogMessage( "   execdomainlist for %s: exit 0 (OK)", cat->name );
   }
   else if (exitcode == -1)
   {
      ufdbLogError( "execdomainlist for %s: pclose error: %d (%s)", cat->name, errno, strerror(errno) );
      return NULL;
   }
   else if (exitcode > 0)
   {
      ufdbLogError( "execdomainlist for %s: command (%s) terminated with an error exit code %d", 
                    cat->name, cat->execdomainlist, exitcode );
      return NULL;
   }

   /* The command defined by 'execdomainlist' has been executed and the exit code was 0 (OK), so
    * now it is verified if the file timestamp of domains.ufdb is newer.
    */

   if (stat( fullfilename, &Stat1 ) != 0)
   {
      ufdbLogError( "execdomainlist for %s: command failed to produce a .ufdb file (%s)", cat->name, fullfilename );
      return NULL;
   }

   newdb = (struct sgDb *) ufdbCalloc( sizeof(struct sgDb), 1 );
   newdb->dbhome = NULL;
   newdb->dbcp = NULL;
   newdb->type = SGDBTYPE_DOMAINLIST;

   sgDbInit( newdb, basefilename );
   if (newdb->dbcp == NULL)
   {
      ufdbLogError( "execdomainlist for %s: command produced a .ufdb file but I failed to load it", cat->name );
      ufdbFree( newdb );
      return NULL;
   }

   if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
      ufdbLogMessage( "execdomainlist for %s: new URL table is loaded", cat->name );

   return newdb;
}


void ufdbSourceExecUserList(
   char *          command )
{
   time_t          t0, te;
   struct Source * sp;

   /* execuserlist is used to dynamically retrieve a list of usernames and
    * is executed every 15 minutes to refresh the list of usernames.
    */

   sp = lastSource;
   if (sp->userDb != NULL  &&  sp->userDb->type != SGDBTYPE_EXECUSERLIST)
   {
      ufdbLogFatalError( "execuserlist \"%s\" in source \"%s\" on line %d: "
                         "found mix of exec and non-exec userlists", 
                         command, sp->name, lineno );
      return;
   }
   if (sp->userDb != NULL)
   {
      ufdbLogFatalError( "execuserlist \"%s\" in source \"%s\" on line %d: "
                         "more than one execuserlist is not supported.  Merge the userlists in the script.", 
                         command, sp->name, lineno );
      return;
   }
   ufdbLogMessage( "execuserlist \"%s\"", command );

   t0 = time( NULL );
   sp->userDb = UFDBretrieveExecUserlist( command );
   sp->sarg0 = command;				/* command is already malloc-ed */
   te = time( NULL );

   if (te - t0 > 6)
      ufdbLogMessage( "WARNING: it took %ld seconds to execute \"%s\"", te - t0, command );
 
   if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
   {
      UFDBmemDBprintUserDB( "user", (struct UFDBmemDB *) sp->userDb->dbcp );
   }
}


void ufdbSourceUserList( 
   char * file  )
{
   char * dbhome;
   char * f;
   FILE * fp;
   char * p;
   char * c;
   char * s;
   char * lc;
   char * lineptr;
   int    ullineno;
   struct sgDb * udb;
   struct Source * sp;
   char   line[10000];
 
   sp = lastSource;
   if (UFDBglobalDebug)
      ufdbLogMessage( "ufdbSourceUserList: source \"%s\"  file \"%s\"", 
       		      sp->name != NULL ? sp->name : "nosource", file );
   udb = sp->userDb;
   if (udb != NULL  &&  udb->type != SGDBTYPE_USERLIST)
   {
      ufdbLogFatalError( "userlist \"%s\" in source \"%s\" on line %d: "
                         "found mix of exec and non-exec userlists", 
                         file, sp->name, lineno );
      return;
   }
   if (udb == NULL)
   {
      udb = sp->userDb = (struct sgDb *) ufdbMalloc( sizeof(struct sgDb) );
      udb->dbhome = NULL;
      udb->dbcp = (void *) UFDBmemDBinit();
      udb->type = SGDBTYPE_USERLIST;
      udb->entries = 0;
   }
 
   dbhome = ufdbSettingGetValue( "dbhome" );
   if (dbhome == NULL)
      dbhome = (char *) DEFAULT_DBHOME;
 
   if (file[0] == '/')
      f = file;
   else
   {
      f = (char *) ufdbMalloc( strlen(dbhome) + strlen(file) + 2 );
      strcpy( f, dbhome );
      strcat( f, "/" );
      strcat( f, file );
      ufdbFree( file );
   }
 
   if ((fp = fopen(f,"r")) == NULL)
   {
      ufdbLogError( "line %d: can't open userlist %s: %s  *****", lineno, f, strerror(errno) );
      ufdbFree( f );
      return;
   }
   ullineno = 0;
 
   ufdbLogMessage( "userlist \"%s\"", f );
 
   while (fgets(line,sizeof(line),fp) != NULL)
   {
      ullineno++;
      if (line[0] == '#')			/* skip comments */
         continue;
 
      p = strchr( line, '\n' );
      if (p != NULL)
      {
         *p = '\0';
         if (p != line) 
         {
  	    if (*(p - 1) == '\r') 		/* remove ^M  */
  	       *(p-1) = '\0';
         }
      }
  
      if (line[0] == '\0')
      {
         ufdbLogError( "userlist %s: line %d: line is empty", f, ullineno );
         continue;
      }
  
      c = strchr( line, '#' );		/* remove comment */
      if (c != NULL)
         *c = '\0';
  
      p = strtok_r( line, " \t,", &lineptr );
      if (p == NULL  ||  *p == '\0')
         continue;
  
      /* we may have a passwd-style formatted file. Ignore ':' and everything else that follows. */
      if ((s = strchr(p,':')) != NULL) 
         *s = '\0';
  
      do
      {
         for (lc = p;  *lc != '\0';  lc++)   /* convert username to lowercase chars */
         {
  	    if (*lc <= 'Z'  &&  *lc >= 'A')
  	       *lc += 'a' - 'A';
         }
         if (*p != '\0')
 	 {
	    udb->entries++;
  	    UFDBmemDBinsert( (struct UFDBmemDB *) udb->dbcp, p, NULL );
         }
      } while ((p = strtok_r(NULL," \t,",&lineptr)) != NULL  &&  *p != '\0');
   }
   fclose( fp );

   if (UFDBglobalDebug)
   {
      ufdbLogMessage( "just read userlist \"%s\"", f );
      UFDBmemDBprintUserDB( "user", (struct UFDBmemDB *) udb->dbcp );
   }

   ufdbFree( f );
}


static void ufdbSourceUserQuota( 
   const char * seconds, 
   const char * sporadic,  
   const char * renew )
{
   int    s;
   struct UserQuota * uq;
   struct Source * sp;
 
   sp = lastSource;
   uq = &sp->userquota;
   s = atoi( seconds );
   if (s <= 0)
      ufdbLogError( "line %d: userquota seconds sporadic hourly|daily|weekly", lineno );
   uq->seconds = s; 
   s = atoi( sporadic );
   if (s <= 0)
      ufdbLogError( "line %d: userquota seconds sporadic hourly|daily|weekly", lineno );
   uq->sporadic = s; 
   s = atoi( renew );
   if (s <= 0)
      ufdbLogError( "line %d: userquota seconds sporadic hourly|daily|weekly", lineno );
   uq->renew = s;
}


void defSourceDomain( 
   char * domain )
{
   struct Source * sp;

   sp = lastSource;
   if (sp->domainDb == NULL) 
      sp->domainDb = (struct sgDb *) UFDBmemDBinit();

   if (UFDBglobalDebug)
      ufdbLogMessage( "defSourceDomain \"%s\"", domain );

   UFDBmemDBinsert( (struct UFDBmemDB *) sp->domainDb, domain, NULL );

   ufdbFree( domain );
}


void ufdbSourceEval( 
   int method  )
{
   if (lastSource != NULL)
   {
      if (UFDBglobalDebug)
         ufdbLogMessage( "ufdbSourceEval %d: %s", method, 
	                 method == UFDB_EVAL_OR ? "evaluate-or" : "evaluate-and" );
      lastSource->evaluationMethod = method;
   }
}


void defSourceTime( 
   char *         name, 
   int            within )
{
   struct ufdbTime *  t;

   if ((t = sgTimeFindName(name)) == NULL)
   {
      ufdbLogFatalError( "line %d: time \"%s\" is not defined in configuration file %s",
		         lineno, name, UFDBglobalConfigFile );
      ufdbFree( name );
      return;
   }

   lastSource->within = within;
   lastSource->time = t;
   ufdbFree( name );
}


struct Source * defSourceFindName( 
   char *          name )
{
   struct Source * p;

   for (p = UFDBgetSourceList();  p != NULL;  p = p->next)
   {
      if (strcmp(name,p->name) == 0)
         return p;
   }

   return NULL;
}


void defSourceIPV4List( 
   char * file )
{
   char * dbhome = NULL;
   char * f;
   FILE * fp;
   char * p;
   char * c;
   char * cidr;
   int    i;
   int    l = 0;
   char * lineptr;
   char   line[UFDB_MAX_URL_LENGTH];

   dbhome = ufdbSettingGetValue( "dbhome" );
   if (dbhome == NULL)
      dbhome = (char *) DEFAULT_DBHOME;

   if (file[0] == '/') 
   {
      f = file;
   }
   else
   {
      f = (char *) ufdbMalloc( strlen(dbhome) + strlen(file) + 2 );
      strcpy( f, dbhome );
      strcat( f, "/" );
      strcat( f, file );
      ufdbFree( file );
   }

   if ((fp = fopen(f,"r")) == NULL) 
   {
      ufdbLogError( "line %d: can't open ipv4list %s: %s", lineno, f, strerror(errno) );
      ufdbFree( f );
     return;
   }

   ufdbLogMessage( "ipv4list \"%s\"", file );

   while (fgets(line,sizeof(line),fp) != NULL)
   {
      l++;
      if (*line == '#')
         continue;
      p = strchr( line, '\n' );
      if (p != NULL && p != line) 
      {
         if (*(p - 1) == '\r') 		/* removing ^M  */
            p--;
         *p = '\0';
      }
      c = strchr( line, '#' );
      p = strtok_r( line, " \t,", &lineptr );
      do {
         if (c != NULL && p >= c) 		/* find the comment */
            break;
         i = strspn( p, ".0123456789/-" );
         if (i == 0)
            break;
         *(p + i) = '\0';
         if ((cidr = strchr(p,'/')) != NULL) 
         {
            *cidr = '\0';
            cidr++;
            sgIpv4( p );
            sgIpv4( cidr );
            if (strchr(cidr,'.') == NULL)
               sgSetIpType( SG_IPTYPE_CIDR, f, l );
            else 
               sgSetIpType( SG_IPTYPE_CLASS, f, l );
          }
          else if ((cidr = strchr(p,'-')) != NULL) 
          {
             *cidr = '\0';
             cidr++;
             sgIpv4( p );
             sgIpv4( cidr );
             sgSetIpType( SG_IPTYPE_RANGE, f, l );
          }
          else
          {
             sgIpv4( p );
             sgIpv4( ufdbStrdup("255.255.255.255") );
             sgSetIpType( SG_IPTYPE_HOST, f, l );
          }
      } while ((p = strtok_r(NULL," \t,",&lineptr)) != NULL);
   }

   fclose( fp );
   ufdbFree( f );
}


void defSourceIPV6List( 
   char * file )
{
   /* TODO IPV6 */
   char * dbhome;
   char * f;
   FILE * fp;
   char * p;
   char * c;
   int    i;
   int    l = 0;
   char * lineptr;
   char   line[UFDB_MAX_URL_LENGTH];

   dbhome = ufdbSettingGetValue( "dbhome" );
   if (dbhome == NULL)
      dbhome = (char *) DEFAULT_DBHOME;

   if (file[0] == '/') 
   {
      f = file;
   }
   else
   {
      f = (char *) ufdbMalloc( strlen(dbhome) + strlen(file) + 2 );
      strcpy( f, dbhome );
      strcat( f, "/" );
      strcat( f, file );
      ufdbFree( file );
   }

   if ((fp = fopen(f,"r")) == NULL) 
   {
      ufdbLogError( "line %d: can't open ipv6list %s: %s", lineno, f, strerror(errno) );
      ufdbFree( f );
      return;
   }

   ufdbLogMessage( "ipv6list \"%s\"", f );

   while (fgets(line,sizeof(line),fp) != NULL)
   {
      l++;
      if (*line == '#')
         continue;
      p = strchr( line, '\n' );
      if (p != NULL && p != line) 
      {
         if (*(p - 1) == '\r') 		/* removing ^M  */
            p--;
         *p = '\0';
      }
      c = strchr( line, '#' );
      p = strtok_r( line, " \t,", &lineptr );
      do {
         if (c != NULL && p >= c) 		/* find the comment */
            break;
         i = strspn( p, ":.0123456789ABCDEFabcdef/" );
         if (i == 0)
            break;
         *(p + i) = '\0';
         if (strchr(p,'/') != NULL) 
         {
            sgIpv6( p, SG_IPV6TYPE_CIDR, f, l );
         }
         else
         {
            sgIpv6( p, SG_IPV6TYPE_HOST, f, l );
         }
      } while ((p = strtok_r(NULL," \t,",&lineptr)) != NULL);
   }

   fclose( fp );
   ufdbFree( f );
}


/* category block functions */

void ufdbCategory( 
  char *            cat )
{
  struct Category * sp;

#if UFDB_DEBUG
   ufdbLogMessage( "ufdbCategory %s", cat );
#endif

  if (Cat != NULL) 
  {
    if ((struct Category *) ufdbCategoryFindName(cat) != NULL)
    {
      ufdbLogFatalError( "line %d: category %s is already defined in configuration file %s",
		         lineno, cat, UFDBglobalConfigFile );
      ufdbFree( cat );
      return;
    }
  }

  sp = (struct Category *) ufdbMallocAligned( 64, sizeof(struct Category) );
  sp->active = 1;
  sp->options = 0;
  sp->within = UFDB_ACL_NONE;
  sp->blockBumpedConnect = 0;
  sp->name = cat;
  sp->domainlist = NULL;
  sp->domainlistDb = NULL;
  sp->execdomainlist = NULL;
  sp->expressionlist = NULL;
  sp->regExp = NULL;
  sp->redirect = NULL;
  sp->time = NULL;
  sp->rewrite = NULL;
  sp->cse = NULL;
  sp->cse2 = NULL;
  sp->next = NULL;
  sp->nblocks = 0;
  sp->nmatches = 0;

  if (Cat == NULL) {
    Cat = sp;
    lastCat = sp;
  } else {
    lastCat->next = sp;
    lastCat = sp;
  }

   /* category "security" is a special case: the options cannot be set default to 0
    * because the default value for allow-unknown-protocol-over-https is ON.
    */
   if (strcmp( cat, "security" ) == 0)
      sp->options = UFDB_OPT_UNKNOWN_PROTOCOL_OVER_HTTPS;
}


void ufdbCategoryEnd( void )
{
  struct Category * d;
 
  d = lastCat;
  if (d->domainlist == NULL  &&  d->expressionlist == NULL  && 
      d->redirect == NULL  &&  d->rewrite == NULL  &&
      d->options == 0)
  {
    ufdbLogError( "category \"%s\" is missing content, set inactive  *****", d->name );
    d->time = NULL;
    d->active = 0;
  }
}


void ufdbCategoryOption( int value, int option )
{
   struct Category * sp;

   sp = lastCat;
   if (value)
      sp->options = sp->options | option;
   else
      sp->options = sp->options & (~option);

   if (option == UFDB_OPT_HTTPS_WITH_HOSTNAME)
      UFDBglobalHttpsWithHostname = value;
   else if (option == UFDB_OPT_HTTPS_OFFICAL_CERTIFICATE)
      UFDBglobalHttpsOfficialCertificate = value;
   else if (option == UFDB_OPT_SKYPE_OVER_HTTPS)
      UFDBglobalSkypeOverHttps = value;
   else if (option == UFDB_OPT_GTALK_OVER_HTTPS)
      UFDBglobalGtalkOverHttps = value;
   else if (option == UFDB_OPT_YAHOOMSG_OVER_HTTPS)
      UFDBglobalYahooMsgOverHttps = value;
   else if (option == UFDB_OPT_AIM_OVER_HTTPS)
      UFDBglobalAimOverHttps = value;
   else if (option == UFDB_OPT_FBCHAT_OVER_HTTPS)
      UFDBglobalFBchatOverHttps = value;
   else if (option == UFDB_OPT_CITRIXONLINE_OVER_HTTPS)
      UFDBglobalCitrixOnlineOverHttps = value;
   else if (option == UFDB_OPT_ANYDESK_OVER_HTTPS)
      UFDBglobalAnydeskOverHttps = value;
   else if (option == UFDB_OPT_TEAMVIEWER_OVER_HTTPS)
      UFDBglobalTeamviewerOverHttps = value;
   else if (option == UFDB_OPT_PROHIBIT_INSECURE_SSLV2)
      UFDBglobalHttpsNoSSLv2 = value;
   else if (option == UFDB_OPT_PROHIBIT_INSECURE_SSLV3)
      UFDBglobalHttpsNoSSLv3 = value;
   else if (option == UFDB_OPT_UNKNOWN_PROTOCOL_OVER_HTTPS)
      UFDBglobalUnknownProtocolOverHttps = value;
   else if (option == UFDB_OPT_SAFE_SEARCH)
      ;
   else if (option == UFDB_OPT_YOUTUBE_EDUFILTER)
      ;
   else
      ufdbLogError( "ufdbCategoryOption: unrecognised option %d *****", option );

   if (UFDBglobalDebug)
      ufdbLogMessage( "ufdbCategoryOption: %s: option %d set to %d", sp->name, option, value );
}


void ufdbCategoryDomainList( 
  char * domainlist )
{
  struct Category * sp;
  char * dbhome;
  char * dl = domainlist;
  char * name;

  if (UFDBglobalDebug > 1)
     ufdbLogMessage( "ufdbCategoryDomainList %s", domainlist );

  if (UFDBglobalTerminating)
     return;

  sp = lastCat;

  if (sp->domainlistDb != NULL)
  {
     ufdbLogFatalError( "line %d: specify only one domainlist per category in configuration file %s",
                        lineno, UFDBglobalConfigFile );
     return;
  }

  dbhome = ufdbSettingGetValue("dbhome");
  if (dbhome == NULL)
    dbhome = (char *) DEFAULT_DBHOME;

  if (domainlist == NULL)
  {
    name = sp->name;
    dl = (char *) ufdbMalloc( sizeof("/dest/") + strlen(name) + sizeof("/domainlist") + 2 );
    strcpy(dl,"/dest/");
    strcat(dl,name);
    strcat(dl,"/domainlist");

    sp->domainlist = (char *) ufdbMalloc( strlen(dbhome) + strlen(dl) + 2 );
    strcpy(sp->domainlist,dbhome);
    strcat(sp->domainlist,"/");
    strcat(sp->domainlist,dl);
  }
  else
  {
    if (domainlist[0] == '/') 
      sp->domainlist = domainlist;
    else
    {
       sp->domainlist = (char *) ufdbMalloc( strlen(dbhome) + strlen(domainlist) + 2 );
       strcpy( sp->domainlist, dbhome );
       strcat( sp->domainlist, "/" );
       strcat( sp->domainlist, domainlist );
       ufdbFree( domainlist );
    }
  }

  sp->domainlistDb = (struct sgDb *) ufdbCalloc( 1, sizeof(struct sgDb) );
  sp->domainlistDb->type = SGDBTYPE_DOMAINLIST;
  sgDbInit( sp->domainlistDb, sp->domainlist );
  ufdbLogMessage( "loading URL table from \"%s\"", sp->domainlist );
  if (sp->domainlistDb->dbcp == NULL  ||
      ((struct UFDBmemTable *) sp->domainlistDb->dbcp)->table.nNextLevels == 0)
  {
    ufdbLogError( "URL database table \"%s\" is empty and is ignored   *****", sp->domainlist );
    ufdbFreeDomainDb( sp->domainlistDb );
    sp->domainlistDb = NULL;
  }
}


void ufdbCategoryExecDomainList( 
   char * command )
{
   struct Category * sp;

   if (UFDBglobalTerminating)
      return;

#if UFDB_DEBUG
   ufdbLogMessage( "ufdbCategoryExecDomainList %s", command );
#endif

   sp = lastCat;

   if (sp == NULL)
   {
      ufdbLogFatalError( "execdomainlist is not within a URL category" );
      return;
   }
   sp->execdomainlist = command;

   if (UFDBglobalDebug)
      ufdbLogMessage( "execdomainlist for category %s: \"%s\"", sp->name, command );
}


void ufdbFreeDomainDb( 
   struct sgDb * dbp )
{
   struct UFDBmemTable * mt;

   if (dbp == NULL)
      return;

   mt = (struct UFDBmemTable *) dbp->dbcp;
   if (mt != NULL)
   {
      if (mt->index != NULL)
	 ufdbFree( mt->index );				/* version 2.x */
      else
	 UFDBfreeTableIndex_1_2( &(mt->table) ); 	/* version 1.2 */
      ufdbFree( (void *) mt->mem );
      ufdbFree( mt );
   }
   ufdbFree( dbp );
}


void ufdbFreeIpv4List( struct Ipv4 * p )
{
   struct Ipv4 * tmp;

   while (p != NULL)
   {
      ufdbFree( p->str );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeIpv6List( struct Ipv6 * p )
{
   struct Ipv6 * tmp;

   while (p != NULL)
   {
      ufdbFree( p->str );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeSourceList( struct Source * p )
{
   struct Source * tmp;

   while (p != NULL)
   {
      ufdbFreeIpv4List( p->ipv4 );
      ufdbFreeIpv6List( p->ipv6 );
      ufdbFree( p->name );
      if (p->domainDb != NULL)
	 UFDBmemDBdeleteDB( (struct UFDBmemDB *) p->domainDb );

      /* execuserlist content is saved during a reload */
      if (p->userDb != NULL  &&  p->userDb->type != SGDBTYPE_EXECUSERLIST)
      {
	 UFDBmemDBdeleteDB( (struct UFDBmemDB *) p->userDb->dbcp );
	 ufdbFree( p->userDb );
      }
      ufdbFree( p->sarg0 );

      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeAclCategoryList( struct AclCategory * p )
{
   struct AclCategory * tmp;

   while (p != NULL)
   {
      /* ufdbLogMessage( "ufdbFreeAclCategoryList %08lx  %08lx  %s", (long) p, (long) p->name, p->name ); */
      ufdbFree( p->name );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeAclList( struct Acl * p )
{
   struct Acl * tmp;

   while (p != NULL)
   {
      /* ufdbLogMessage( "ufdbFreeAclList %08lx  %08lx  %s  %s", (long) p, (long) p->name, p->name, p->within == UFDB_ACL_ELSE ? "else" : "" ); */
      ufdbFree( p->name );
      /* ufdbLogMessage( "   freeing pass aclcategory list" ); */
      ufdbFreeAclCategoryList( p->pass );
      /* ufdbLogMessage( "   freeing implicit pass aclcategory list" ); */
      ufdbFreeAclCategoryList( p->implicitPass );
      ufdbFree( p->redirect );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeSettingList( struct ufdbSetting * p )
{
   struct ufdbSetting * tmp;
 
   while (p != NULL)
   {
      /* ufdbLogMessage( "ufdbFreeSettingList %08lx  %08lx", (long) p, (long) p->name ); */
      ufdbFree( p->name );
      ufdbFree( p->value );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


static void ufdbFreeCategoryList( struct Category * p )
{
   struct Category * tmp;

   while (p != NULL)
   {
      /* ufdbLogMessage( "ufdbFreeCategoryList %08lx  %08lx  %s", (long) p, (long) p->name, p->name ); */
      if (UFDBglobalDebug > 2  ||  UFDBglobalDebugRegexp)
         ufdbLogMessage( "ufdbFreeCategoryList: freeing regexp of category %s", p->name );
      ufdbFreeRegExprList( p->regExp );
      ufdbFree( p->name );
      ufdbFree( p->domainlist );
      ufdbFreeDomainDb( p->domainlistDb );
      ufdbFree( p->execdomainlist );
      ufdbFree( p->cse );
      ufdbFree( p->cse2 );
      ufdbFree( p->expressionlist );
      ufdbFree( p->redirect );
      /* ufdbFree( p->rewrite );  freed in ufdbFreeRewriteList() */
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeRewriteList( 
   struct sgRewrite * p )
{
   struct sgRewrite * tmp;

   while (p != NULL)
   {
      ufdbFreeRegExprList( p->rewrite );
      ufdbFree( p->name );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeTimeElement( 
   struct TimeElement * p )
{
   struct TimeElement * tmp;

   while (p != NULL)
   {
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeTime( 
   struct ufdbTime * p )
{
   struct ufdbTime * tmp;

   while (p != NULL)
   {
      ufdbFree( p->name );
      ufdbFreeTimeElement( p->element );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeAllMemory( void )
{
#if 0
   /* we cannot stop using the log file. we never close it */
   if (UFDBglobalLogDir != NULL)
      ufdbFree( UFDBglobalLogDir );
   UFDBglobalLogDir = NULL;
   globalLogFile = NULL;
#endif

#if 0
   /* we may need these variables during a reload */
   if (UFDBglobalPidFile != NULL)
   {
      ufdbFree( UFDBglobalPidFile );
      UFDBglobalPidFile = NULL;
   }
   if (UFDBglobalEmailServer != NULL)
   {
      ufdbFree( UFDBglobalEmailServer );
      UFDBglobalEmailServer = NULL;
   }
   if (UFDBglobalMyHostname != NULL)
   {
      ufdbFree( UFDBglobalMyHostname );
      UFDBglobalMyHostname = NULL;
   }
   if (UFDBglobalAdminEmail != NULL)
   {
      ufdbFree( UFDBglobalAdminEmail );
      UFDBglobalAdminEmail = NULL;
   }
   if (UFDBglobalSenderEmail != NULL)
   {
      ufdbFree( UFDBglobalSenderEmail );
      UFDBglobalSenderEmail = NULL;
   }
   if (UFDBglobalExternalStatusCommand != NULL)
   {
      ufdbFree( UFDBglobalExternalStatusCommand );
      UFDBglobalExternalStatusCommand = NULL;
   }
#endif

   ufdbFreeSettingList( Setting );
   Setting = NULL;
   ufdbFreeRewriteList( Rewrite );
   Rewrite = NULL;
   ufdbFreeCategoryList( Cat );
   Cat = NULL;
   ufdbFreeAclList( AclList );
   AclList = NULL;

   ufdbFreeSourceList( (struct Source *) SourceList );
   UFDBsetSourceList( NULL );
   lastSource = NULL;

   /* free UFDBglobalCheckedDB */
   if (UFDBglobalCheckedDB.table.nNextLevels > 0  &&  UFDBglobalCheckedDB.mem != NULL)
   {
      if (UFDBglobalCheckedDB.index != NULL)
         ufdbFree( (void *) UFDBglobalCheckedDB.index );
      else
	 UFDBfreeTableIndex_1_2( &(UFDBglobalCheckedDB.table) ); 		/* .nextLevel[0]) ); */
      ufdbFree( (void *) UFDBglobalCheckedDB.mem );
   }
   UFDBglobalCheckedDB.table.nNextLevels = 0;
   UFDBglobalCheckedDB.mem = NULL;
   UFDBglobalCheckedDB.index = NULL;

   if (UFDBglobalCheckedExpressions != NULL)
   {
      ufdbFreeRegExprList( UFDBglobalCheckedExpressions );
      UFDBglobalCheckedExpressions = NULL;
   }

   lastSetting = NULL;
   Setting = NULL;

   lastCat = NULL;
   Cat = NULL;

   lastRewrite = NULL;
   Rewrite = NULL;
   lastRewriteRegExec = NULL;

   ufdbFreeTime( TimeList );
   TimeList = NULL;
   lastTime = NULL;

   ufdbFree( TimeElementsEvents );
   TimeElementsEvents = NULL;
   numTimeElements = 0;

   lastTimeElement = NULL;
   TimeElement = NULL;

   lastAcl = NULL;
   defaultAcl = NULL;
   AclList = NULL;
   lastAclCategory = NULL;

   lastRegExpDest = NULL;

   ufdbFree( UFDBglobalSquidVersion );
   UFDBglobalSquidVersion = NULL;
}


void ufdbCategoryUrlList( 
   char * urllist )
{
   if (urllist == NULL)
      urllist = (char *) "-";
   ufdbLogError( "line %d: \"urllist %s\" is deprecated and ignored *****\n"
                 "ufdbGenTable should be called with the -u option to include URLs\n"
		 "ufdbGenTable combines URLs and domains in one table file so only the domainlist is required",
		 lineno, urllist );
   if (urllist[0] != '-')
      ufdbFree( urllist );
}


void ufdbCategoryExpressionList( 
  char * exprlist, 
  const char * chcase  )
{
  FILE * fp;
  char * dbhome;
  char * dl;
  char * name;
  char * p;
  int    flags;
  struct stat         statbuf;
  struct Category *   sp;
  struct ufdbRegExp * regexp;
  char   buf[UFDB_MAX_URL_LENGTH];
  char   errbuf[256];

#if UFDB_DEBUG
   ufdbLogMessage( "ufdbCategoryExpressionList %s", exprlist );
#endif

  if (UFDBglobalTerminating)
     return;

  flags = REG_EXTENDED | REG_NOSUB;
  dbhome = ufdbSettingGetValue( "dbhome" );
  sp = lastCat;
  if (dbhome == NULL)
    dbhome = (char *) DEFAULT_DBHOME;

  if (exprlist == NULL)
  {
    name = sp->name;
    dl = (char *) ufdbMalloc( sizeof("/dest/") + strlen(name) + sizeof("/expressionlist") + 2 );
    strcpy(dl,"/dest/");
    strcat(dl,name);
    strcat(dl,"/expressionlist");

    flags |= REG_ICASE; 	/* default case insensitive */

    sp->expressionlist = (char *) ufdbMalloc( strlen(dbhome) + strlen(dl) + 2 );
    strcpy(sp->expressionlist,dbhome);
    strcat(sp->expressionlist,"/");
    strcat(sp->expressionlist,dl);
  }
  else
  {
    if (exprlist[0] == '/') 
    {
      sp->expressionlist = exprlist;
    }
    else
    {
       sp->expressionlist = (char *) ufdbMalloc( strlen(dbhome) + strlen(exprlist) + 2 );
       strcpy( sp->expressionlist, dbhome );
       strcat( sp->expressionlist, "/" );
       strcat( sp->expressionlist, exprlist );
       ufdbFree( exprlist );
    }
    if (*chcase == 'i')
       flags |= REG_ICASE;     /* set case insensitive */
  }

  ufdbLogMessage( "loading regular expressions from \"%s\"", sp->expressionlist );

  if ((fp = fopen(sp->expressionlist, "r")) == NULL) 
  {
    ufdbLogFatalError( "cannot open expression list %s: %s", sp->expressionlist, strerror(errno) );
    return;
  }

  if (0 == fstat( fileno(fp), &statbuf ))
  {
     if (!S_ISREG(statbuf.st_mode))
     {
        ufdbLogFatalError( "expression list %s: not a regular file", sp->expressionlist );
	fclose( fp );
	return;
     }
  }

  while (!feof(fp)  &&  fgets(buf, sizeof(buf), fp) != NULL) 
  {
    if (buf[0] == '#')
       continue;

    p = (char *) strchr( buf, '\n' );
    if (p != NULL  &&  p != buf) 
    {
      if (*(p-1) == '\r') 	/* removing ^M  */
	p--;
      *p = '\0';
    }
    /* TO-DO: warn about leading and trailing spaces */
    regexp = ufdbNewPatternBuffer( buf, flags );
    if (regexp->error) 
    {
      regerror( regexp->error, regexp->compiled[0], errbuf, sizeof(errbuf) );
      ufdbLogError( "regular expression error in %s:\n%s: %s   *****", sp->expressionlist, errbuf, buf );
    }
    if (lastCat->regExp == NULL) 
    {
      lastCat->regExp = regexp;
      lastRegExpDest = regexp;
    }
    else
    {
      lastRegExpDest->next = regexp;
      lastRegExpDest = regexp;
    }
  }

  fclose( fp );

  if (UFDBglobalExpressionOptimisation)
     lastCat->regExp = UFDBoptimizeExprList( sp->expressionlist, lastCat->regExp );
}


void ufdbCategoryCACertsFile(
   char *              cacertsFile )
{
   struct Category *   cat;

   cat = lastCat;
   if (cat == NULL  ||  strcmp( cat->name, "security" ) != 0)
   {
      ufdbLogFatalError( "cacerts can only be defined inside the \"security\" category" );
      return;
   }

   cat->cse = cacertsFile;
   if (*cacertsFile == '/')
      strcpy( UFDBglobalCAcertsFile, cacertsFile );
   else
   {
      char * dbh;
      dbh = ufdbSettingGetValue( "dbhome" );
      if (dbh == NULL)
         dbh = (char *) DEFAULT_DBHOME;
      strcpy( UFDBglobalCAcertsFile, dbh );
      strcat( UFDBglobalCAcertsFile, "/" );
      strcat( UFDBglobalCAcertsFile, cacertsFile );
   }
}


void ufdbCategoryCACertsDir(
   char *              cacertsDir )
{
   struct Category *   cat;

   cat = lastCat;
   if (cat == NULL  ||  strcmp( cat->name, "security" ) != 0)
   {
      ufdbLogFatalError( "cacerts-dir can only be defined inside the \"security\" category" );
      return;
   }

   cat->cse2 = cacertsDir;
   if (*cacertsDir == '/')
      strcpy( UFDBglobalCAcertsDir, cacertsDir );
   else
   {
      char * dbh;
      dbh = ufdbSettingGetValue( "dbhome" );
      if (dbh == NULL)
        dbh = (char *) DEFAULT_DBHOME;
      strcpy( UFDBglobalCAcertsDir, dbh );
      strcat( UFDBglobalCAcertsDir, "/" );
      strcat( UFDBglobalCAcertsDir, cacertsDir );
   }
}


void ufdbCategoryRedirect( 
   char * value )
{
   struct Category * sp;

#if UFDB_DEBUG
   ufdbLogMessage( "ufdbCategoryRedirect %s", value );
#endif

   sp = lastCat;
   sp->redirect = value;
   /* TODO: check that "localhost" is not here if no 302 is used */
}


void ufdbCategoryRewrite( 
  char *              value )
{
  struct sgRewrite *  rewrite;

  if ((rewrite = sgRewriteFindName(value)) == NULL)
  {
    ufdbLogFatalError( "line %d: rewrite %s is not defined in configuration file %s",
		       lineno, value, UFDBglobalConfigFile );
    return;
  }

  lastCat->rewrite = rewrite;
}


void ufdbCategoryBlockConnect( 
   int flag  )
{
   lastCat->blockBumpedConnect = flag;

   if (UFDBglobalDebug)
      ufdbLogMessage( "ufdbCategoryBlockConnect: category \"%s\" : block-bumped-connect %s",
                      lastCat->name, flag ? "on" : "off" );
}


void ufdbCategoryTime(  
  char *        name, 
  int  		within )
{
  struct ufdbTime * t;

  if ((t = sgTimeFindName(name)) == NULL) 
  {
    ufdbLogFatalError( "line %d: time \"%s\" is not defined in configuration file %s",
		       lineno, name, UFDBglobalConfigFile );
    ufdbFree( name );
    return;
  }

  lastCat->within = within;
  lastCat->time = t;
  ufdbFree( name );
}


struct Category * ufdbCategoryFindName( 
  char *               name )
{
  struct Category * p;

  for (p = Cat; p != NULL; p = p->next)
  {
    if (strcmp(name,p->name) == 0)
      return p;
  }

  return NULL;
}


void sgRewrite( 
  char * rewrite )
{
  struct sgRewrite * rew;

#if UFDB_DEBUG
   ufdbLogMessage( "sgRewrite %s", rewrite );
#endif

  if (Rewrite != NULL)
  {
    if ((struct sgRewrite *) sgRewriteFindName(rewrite) != NULL)
    {
      ufdbLogFatalError( "line %d: rewrite \"%s\" is not defined in configuration file %s",
		       lineno, rewrite, UFDBglobalConfigFile );
      ufdbFree( rewrite );
      return;
    }
  }

  rew = (struct sgRewrite *) ufdbMalloc( sizeof(struct sgRewrite) );
  rew->name = rewrite;
  rew->active = 1;
  rew->rewrite = NULL;
  rew->time = NULL;
  rew->within = UFDB_ACL_NONE;
  rew->next = NULL;

  if (Rewrite == NULL) 
  {
    Rewrite = rew;
    lastRewrite = rew;
  }
  else
  {
    lastRewrite->next = rew;
    lastRewrite = rew;
  }
}


void sgRewriteTime( 
  char * name, 
  int    within )
{
  struct ufdbTime * t;

#if UFDB_DEBUG
   ufdbLogMessage( "sgRewriteTime %s %d", name, within );
#endif

  if ((t = sgTimeFindName(name)) == NULL)
  {
    ufdbLogFatalError( "line %d: time \"%s\" is not defined in configuration file %s",
		       lineno, name, UFDBglobalConfigFile );
    ufdbFree( name );
    return;
  }

  lastRewrite->within = within;
  lastRewrite->time = t;
  ufdbFree( name );
}


void sgRewriteSubstitute( 
  char * string )
{
  char * pattern; 
  char * subst = NULL;
  char * p;
  int    flags = REG_EXTENDED;
  int    global = 0;
  char * httpcode = NULL;
  struct ufdbRegExp * regexp;
  char   errbuf[256];

  pattern = string + 2; 	/* skipping s@ */
  p = pattern;
  while ((p = strchr(p,'@')) != NULL)
  {
    if (*(p - 1) != '\\') 
    {
      *p = '\0';
      subst = p + 1;
      break;
    }
    p++;
  }

  p = strrchr( subst, '@' );
  while (p != NULL  &&  *p != '\0')
  {
    if (*p == 'r' )
      httpcode =  (char *) REDIRECT_TEMPORARILY;
    if (*p == 'R' )
      httpcode =  (char *) REDIRECT_PERMANENT;
    if (*p == 'i' || *p == 'I')
      flags |= REG_ICASE;
    if (*p == 'g')
      global = 1;
    *p = '\0'; 		/* removes @i from string */
    p++;
  } 

  regexp = ufdbNewPatternBuffer( pattern, flags );
  if (regexp->error) 
  {
      regerror( regexp->error, regexp->compiled[0], errbuf, sizeof(errbuf) );
      ufdbLogError( "line %d: regular expression error in %s:\n%s   *****", lineno, pattern, errbuf );
  }
  else {
    regexp->substitute = ufdbStrdup( subst );
  }

  if (lastRewrite->rewrite == NULL)
    lastRewrite->rewrite = regexp;
  else 
    lastRewriteRegExec->next = regexp;
  regexp->httpcode = httpcode;
  regexp->global = global;
  lastRewriteRegExec = regexp;
}


struct sgRewrite * sgRewriteFindName( 
  char * name )
{
  struct sgRewrite * p;

  for (p = Rewrite; p != NULL; p = p->next)
  {
    if (strcmp(name,p->name) == 0)
      return p;
  }

  return NULL;
}


/*
 * Time functions
 */

/*
 * sgTime - parse configuration time statement.
 */
void sgTime( 
  char *        name )
{
  struct ufdbTime * t;

  if (UFDBglobalDebug)
     ufdbLogMessage( "sgTime %s", name );

  if (TimeList != NULL)
  {
    if ((struct ufdbTime *) sgTimeFindName(name) != NULL)
    {
      ufdbLogFatalError( "line %d: time \"%s\" is not defined in configuration file %s",
		         lineno, name, UFDBglobalConfigFile );
      ufdbFree( name );
      return;
    }
  }
  else 
    numTimeElements = 0;

  t = (struct ufdbTime *) ufdbMalloc( sizeof(struct ufdbTime) );
  t->name = name;
  t->active = 1;
  t->element = NULL;
  t->next = NULL;

  TimeElement = NULL;
  lastTimeElement = NULL;
  if (TimeList == NULL) 
  {
    TimeList = t;
    lastTime = t;
  }
  else
  {
    lastTime->next = t;
    lastTime = t;
  }
}


/*
 * sgTimeElementInit - initialise parsing of a configuration time element.
 */
void sgTimeElementInit( void )
{
   struct TimeElement * te;

   te = (struct TimeElement *) ufdbCalloc( 1, sizeof(struct TimeElement) );
   numTimeElements++;

   if (lastTime->element == NULL)
     lastTime->element = te;
   if (lastTimeElement != NULL)
     lastTimeElement->next = te;
   lastTimeElement = te;
}


/*
 * sgTimeElementEnd - finalise parsing of configuration time element.
 */
void sgTimeElementEnd( void )
{
  time_switch = 0;
  date_switch = 0;

  if (lastTimeElement->fromdate != 0)
  {
    if (lastTimeElement->todate == 0)
      lastTimeElement->todate = lastTimeElement->fromdate + 86399;
    else 
      lastTimeElement->todate = lastTimeElement->todate + 86399;
  }

  if (lastTimeElement->from == 0  &&  lastTimeElement->to == 0)
    lastTimeElement->to = 1439;  /* set time to 23:59 */
}


/*
 * sgTimeElementAdd - add configuration time element.
 */
void sgTimeElementAdd( 
  char * element, 
  char   type ) 
{
  struct TimeElement * te;
  char * p;
  char   wday;
  int    h, m, Y, M, D;
  time_t sec;
  char * lineptr;

  wday = 0;
  M = 0;
  D = -1;
  te = lastTimeElement;

  switch (type)
  {
  case T_WEEKDAY:
    p = strtok_r( element, " \t,", &lineptr );
    do {
      if (*p == '*') {
	wday = 0x7F;
      } else if (!strncmp(p,"sun",3)) {
	wday = wday | 0x01;
      } else if (!strncmp(p,"mon",3)) {
	wday = wday | 0x02;
      } else if (!strncmp(p,"tue",3)) {
	wday = wday | 0x04;
      } else if (!strncmp(p,"wed",3)) {
	wday = wday | 0x08;
      } else if (!strncmp(p,"thu",3)) {
	wday = wday | 0x10;
      } else if (!strncmp(p,"fri",3)) {
	wday = wday | 0x20;
      } else if (!strncmp(p,"sat",3)) {
	wday = wday | 0x40;
      }
      p = strtok_r( NULL, " \t,", &lineptr );
    } while (p != NULL);
    te->wday = wday;
    break;

  case T_TVAL:
    h = -1;
    m = -1;
    sscanf( element, "%d:%d", &h, &m );
    if ((h < 0 || h > 24) || (m < 0 || m > 59))
    {
      ufdbLogFatalError( "line %d: time format error in %s", lineno, UFDBglobalConfigFile );
      h = 0;
      m = 0;
    }
    if (time_switch == 0) 
    {
      time_switch++;
      te->from = (h * 60) + m ;
    }
    else
    {
      time_switch = 0;
      te->to = (h * 60) + m ;
    }
    break;

  case T_DVAL:
    sec = date2sec( element );
    if (sec == -1) 
    {
      ufdbLogFatalError( "line %d: date format error in %s", lineno, UFDBglobalConfigFile );
      sec = 1;
    }
    if (date_switch == 0) {
      date_switch++;
      te->fromdate = sec;
    } else {
      date_switch = 0;
      te->todate = sec;
    }
    break;

  case T_DVALCRON:
    p = strtok_r( element, "-./", &lineptr );
    Y = atoi(p);
    if (*p == '*')
      Y = -1;
    else
      Y = atoi(p);
    while ((p=strtok_r(NULL,"-./",&lineptr)) != NULL) 
    {
      if (*p == '*')
	if (M == 0)
	  M = -1;
	else 
	  D = -1;
      else
	if (M == 0)
	  M = atoi(p);
	else
	  D = atoi(p);
    }
    te->y = Y;
    te->m = M;
    te->d = D;
    break;

  case T_WEEKLY:
    p = element;
    while (*p != '\0') 
    {
      switch (*p) {
      case 'S':
      case 's':
	wday = wday | 0x01;
	break;
      case 'M':
      case 'm':
	wday = wday | 0x02;
	break;
      case 'T':
      case 't':
	wday = wday | 0x04;
	break;
      case 'W':
      case 'w':
	wday = wday | 0x08;
	break;
      case 'H':
      case 'h':
	wday = wday | 0x10;
	break;
      case 'F':
      case 'f':
	wday = wday | 0x20;
	break;
      case 'A':
      case 'a':
	wday = wday | 0x40;
	break;
      default:
	ufdbLogFatalError( "line %d: weekday format error in %s", lineno, UFDBglobalConfigFile );
	break;
      }
      p++;
    }
    te->wday = wday;
    break;
  }

  ufdbFree( element );
}


/* 
 * lookup a ufdbTime element by name.
 */
struct ufdbTime * sgTimeFindName( 
  char *        name )
{
  struct ufdbTime * p;

  for (p = TimeList; p != NULL; p = p->next)
  {
    if (strcmp(name,p->name) == 0)
      return p;
  }

  return NULL;
}


/*
 * sgTimeCmp - Time array sort function.
 */
static int sgTimeCmp( const void * a, const void * b )
{
   const int * aa = (const int *) a;
   const int * bb = (const int *) b;

   return *aa - *bb;
}


/*
 * getSortedTimeElementTimes - produce a sorted array of time element times
 */
static void getSortedTimeElementTimes( void )
{
   struct ufdbTime *    p;
   struct TimeElement * te;
   int                  i, j;
   int                  totalNumElems;
 
   if (UFDBglobalDebug)
      ufdbLogMessage( "getSortedTimeElementEvents" );

   if (TimeList == NULL)
      return;

   /* find total number of time elements */
   totalNumElems = 0;
   for (p = TimeList;  p != NULL;  p = p->next) 
      for (te = p->element;  te != NULL;  te = te->next) 
         totalNumElems++;

   TimeElementsEvents = (int *) ufdbCalloc( totalNumElems * 2 , sizeof(int) ); 

   i = 0;
   for (p = TimeList;  p != NULL;  p = p->next) 
   {
      for (te = p->element;  te != NULL;  te = te->next) 
      {
         TimeElementsEvents[i++] = te->from == 0 ? 1440 : te->from;
         TimeElementsEvents[i++] = te->to == 0   ? 1440 : te->to;
      }
   }
 
   qsort( TimeElementsEvents, totalNumElems * 2, sizeof(int), sgTimeCmp );

   if (UFDBglobalDebug)
      ufdbLogMessage( "   getSortedTimeElementEvents: after qsort" );

   /* remove identical time elements */
   for (i=1,j=1;  i < totalNumElems * 2;  i++) 
   {
      if (TimeElementsEvents[i] > TimeElementsEvents[i-1])
      {
	 TimeElementsEvents[j] = TimeElementsEvents[i];
         j++;
      }
   }

   numTimeElements = j;			/* #unique time elements */
}


/*
 * _TimeEvaluateElements - evaluate all elements if they match the current time/date and mark them active.
 */
static void _TimeEvaluateElements( 
  struct tm * tm_now, 
  time_t      now )
{
  struct ufdbTime *    tlist;
  struct TimeElement * te;
  int                  min;

  if (UFDBglobalDebug)
     ufdbLogMessage( "_TimeEvaluateElements" );

  for (tlist = TimeList;  tlist != NULL;  tlist = tlist->next)
  {
    tlist->active = 0;
    for (te = tlist->element;  te != NULL;  te = te->next)
    {
      if (te->wday != 0) 			/* check wday */
      {
	if (((1 << tm_now->tm_wday) & te->wday) != 0)  
	{
	  min = (tm_now->tm_hour * 60 ) + tm_now->tm_min;
	  if (min >= te->from  &&  min < te->to) 
	  {
	    tlist->active = 1;
	    break;
	  }
	}
      }
      else if (te->fromdate != 0)		/* check date */
      {
	if (now >= te->fromdate  &&  now <= te->todate) 
	{
	  min = (tm_now->tm_hour * 60 ) + tm_now->tm_min;
	  if (min >= te->from  &&  min < te->to) 
	  {
	    tlist->active = 1;
	    break;
	  }
	}
      }
      else					/* check crondate */
      {
	if (te->y == -1  ||  te->y == (tm_now->tm_year + 1900)) 
	{
	  if (te->m == -1  ||  te->m == (tm_now->tm_mon + 1)) 
	  {
	    if (te->d == -1  ||  te->d == (tm_now->tm_mday)) 
	    {
	      min = (tm_now->tm_hour * 60 ) + tm_now->tm_min;
	      if (min >= te->from  &&  min < te->to) 
	      {
		tlist->active = 1;
		break;
	      }
	    }
	  }
	}
      }
    }
    if (UFDBglobalDebug)
       ufdbLogMessage( "      _TimeEvaluateElements: time %s is %sactive", tlist->name, tlist->active?"":"not " );
  }
}


/*
 * _TimeSetAclSrcDestRew - mark all acl/source/dest/rew (in)active.
 */
static void _TimeSetAclSrcDestRew( void )
{
  struct Acl *         acl;
  struct Category *    cat;
  struct Source *      s;
  struct sgRewrite *   rew;
  int                  a;

  if (UFDBglobalDebug)
     ufdbLogMessage( "_TimeSetAclSrcDestRew" );

  for (acl = AclList;  acl != NULL;  acl = acl->next)
  {
    if (acl->time != NULL  &&  acl->within != UFDB_ACL_ELSE) 
    {
      /* Be careful here: we are multithreaded and other threads use the value 
       * of acl->active at the same time.
       */
      a = acl->time->active;
      if (acl->within == UFDB_ACL_OUTSIDE)
	a = !a;
      if (acl->next != NULL  &&  acl->next->within == UFDB_ACL_ELSE) 
	acl->next->active = !a;
      acl->active = a;
    }
#if 0
    if (acl->pass == NULL)
       acl->active = 0;			/* it can have a 'continue' so do not make it inactive */
#endif
    if (UFDBglobalDebug)
       ufdbLogMessage( "      _TimeSetAclSrcDestRew: acl %s %s is %sactive", acl->name, 
		       acl->within==UFDB_ACL_ELSE ? "ELSE" :
			  acl->within==UFDB_ACL_WITHIN ? "WITHIN" :
			     acl->within==UFDB_ACL_OUTSIDE ? "OUTSIDE" : "",
		       acl->active?"":"not " );
  }

  for (cat = Cat;  cat != NULL;  cat = cat->next)
  {
    if (cat->time != NULL) 
    {
      cat->active = cat->time->active;
      if (cat->within == UFDB_ACL_OUTSIDE)
	cat->active = !cat->active;
    }
    if (UFDBglobalDebug)
       ufdbLogMessage( "      _TimeSetAclSrcDestRew: category %s is %sactive", cat->name, cat->active?"":"not " );
  }

  for (s = UFDBgetSourceList();  s != NULL;  s = s->next)
  {
    if (s->time != NULL) 
    {
      s->active = s->time->active;
      if (s->within == UFDB_ACL_OUTSIDE)
	s->active = !s->active;
    }
    if (UFDBglobalDebug)
       ufdbLogMessage( "      _TimeSetAclSrcDestRew: source %s is %sactive", s->name, s->active?"":"not " );
  }

  for (rew = Rewrite; rew != NULL; rew = rew->next)
  {
    if (rew->time != NULL) 
    {
      rew->active = rew->time->active;
      if (rew->within == UFDB_ACL_OUTSIDE)
	 rew->active = !rew->active;
    }
    if (UFDBglobalDebug)
       ufdbLogMessage( "      _TimeSetAclSrcDestRew: rewite %s is %sactive", rew->name, rew->active?"":"not " );
  }
}


/*
 *  ufdbHandleAlarmForTimeEvents - the alarm for the next time event went off so 
 *                                 recalculate the time-dependent active state.
 */
void ufdbHandleAlarmForTimeEvents( 
   int        why )
{
   time_t     now;
   struct tm  tm_now;
   int        m;
   int        tindex;
   int        lastval;

   if (UFDBglobalDebug)
      ufdbLogMessage( "ufdbHandleAlarmForTimeEvents" );

   if (why == UFDB_PARAM_INIT)
      ufdbLogMessage( "time definitions are used; evaluating current ACLs" );
   else
   {
      ufdbLogMessage( "alarm went off to recalculate time ACLs" );
      if (UFDBglobalTerminating)
      {
	 ufdbLogMessage( "This alarm is ignored because ufdbguardd is exiting" );
	 return;
      }
      if (UFDBglobalReconfig)
      {
	 ufdbLogMessage( "This alarm is ignored because the configuration is being reloaded"
	                 " and a new alarm is set to go off in 15 seconds" );
	 alarm( 15 );
	 return;
      }
   }

   if (TimeList == NULL)
   {
      return;
   }

   getSortedTimeElementTimes();

   now = time(NULL) + 30 + globalDebugTimeDelta;
   localtime_r( &now, &tm_now ); 
   m = (tm_now.tm_hour * 60) + tm_now.tm_min;
  
   lastval = 0;
   for (tindex = 0;  tindex < numTimeElements;  tindex++)
   {
#if UFDB_TIME_DEBUG
      if (UFDBglobalDebug)
         ufdbLogMessage( "   TimeElementsEvents[%d] = %d", tindex, TimeElementsEvents[tindex] );
#endif
      lastval = TimeElementsEvents[tindex];
      if (TimeElementsEvents[tindex] >= m)
         break;
   }

   if (UFDBglobalDebug)
      ufdbLogMessage( "   ufdbHandleAlarmForTimeEvents: m = %d  tindex = %d, lastval = %d", m, tindex, lastval );

   if (lastval < m)
      m = (((1440 - m) + TimeElementsEvents[0]) * 60) - tm_now.tm_sec;
   else
      m = ((lastval - m) * 60) - tm_now.tm_sec;

   if (m <= 0)
      m = 30;

   _TimeEvaluateElements( &tm_now, now );
   _TimeSetAclSrcDestRew();

   ufdbFree( TimeElementsEvents );
   TimeElementsEvents = NULL;

   ufdbLogMessage( "next alarm is in %d seconds", (unsigned int) m );
   ufdbSetSignalHandler( SIGALRM, sgAlarm );
   (void) alarm( (unsigned int) m );
}


/*
 * sgTimeElementClone - copy a time specification.
 */
void sgTimeElementClone( void )
{
  struct TimeElement * te;
  struct TimeElement * tmp;

  te = lastTimeElement;
  if (te == NULL) 
  {
    ufdbLogFatalError( "No previous TimeElement in sgTimeElementClone !" );
    return;
  }
  else
  {
    sgTimeElementInit();
    lastTimeElement->wday = te->wday;
    lastTimeElement->from = te->from;
    lastTimeElement->to = te->to;
    lastTimeElement->y = te->y;
    lastTimeElement->m = te->m;
    lastTimeElement->d = te->d;
    lastTimeElement->fromdate = te->fromdate;
    lastTimeElement->todate = te->todate;
    tmp = lastTimeElement;
    lastTimeElement = te;
    sgTimeElementEnd();
    lastTimeElement = tmp;
  }
}


/*
 * IP functions
 */

void sgSetIpType( 
  int           type, 
  char *        file, 
  int           line )
{
  struct Ipv4 * ip;
  char *        p;
  char *        f;
  int           l;
  unsigned int   octet;
  unsigned int   op;

  ip = sgIpv4Last( lastSource );
  f = (file == NULL) ? UFDBglobalConfigFile : file;
  l = (line == 0) ? lineno : line;
  if (type == SG_IPTYPE_HOST)
    ip->mask = 0xffffffff;

  if (type == SG_IPTYPE_RANGE)
  {
    if (sgConvDot(ip->str,&op) == NULL)
    {
      ufdbLogFatalError( "IP address error in %s line %d", f, l );
      ip->mask = 0;
    }
    else 
      ip->mask = op;
    if ((unsigned int) ip->net > (unsigned int) ip->mask)
    {
      ufdbLogFatalError( "IP range error in %s line %d   %08x %08x", f, l, ip->net, ip->mask );
    }
  }

  if (type == SG_IPTYPE_CLASS)
  {
    p = ip->str;
    if (*p == '/')
      p++;
    if (sgConvDot(p,&op) == NULL)
    {
      ufdbLogFatalError( "IP address error in %s line %d", f, l );
      ip->mask = 0;
    }
    else 
      ip->mask = op;
  }

  if (type == SG_IPTYPE_CIDR)
  {
    p = ip->str;
    if (*p == '/')
      p++;
    octet = (unsigned long) atoi( p );
    if (octet > 32) 
    {
      ufdbLogFatalError( "IP address CIDR out of range \"%s\" in %s line %d", p, f, l );
      octet = 32;
    }
    if (octet == 32)
      ip->mask = 0xffffffff;
    else
      ip->mask = 0xffffffff ^ (0xffffffff >> octet);
    ip->net = ip->net & ip->mask;
  }

  ip->type = type;
  ip->next = (struct Ipv4 *) ufdbCalloc( 1, sizeof(struct Ipv4) );

#if 0
  ufdbLogMessage( "   sgSetIpType %2d  %08lx  %08lx", ip->type, ip->net, ip->mask );
#endif

  /* TO-DO: fix messy code where the last struct must contain zeros */
}


void sgIpv4( 
  const char * name )
{
  struct Ipv4 *  ipv4;
  unsigned int   op;

#if 0
  ufdbLogMessage( "   sgIpv4( %s )", name );
#endif

  if (lastSource->ipv4 == NULL) 
  {
    ipv4 = (struct Ipv4 *) ufdbCalloc( 1, sizeof(struct Ipv4) );
#if 0
    ip->str = NULL;
    ip->next = NULL;
#endif
    lastSource->ipv4 = ipv4;
    lastSource->lastipv4 = ipv4;
  }
  else
  {
    ipv4 = sgIpv4Last( lastSource );
  }

  if (ipv4->net_is_set == 0) 
  {
    ipv4->net_is_set = 1;
    if (sgConvDot(name,&op) == NULL) 
    {
      ufdbLogFatalError( "line %d: IP address error in %s", lineno, UFDBglobalConfigFile );
      ipv4->net = 0;
    }
    else 
      ipv4->net = op;
  }
  else
  {
    ipv4->str = ufdbStrdup( name );
  }

#if 0
  ufdbLogMessage( "   sgIpv4 %2d  %08lx  %08lx  %s", 
                  ipv4->type, ipv4->net, ipv4->mask, ipv4->str==NULL?"NULL":ipv4->str );
#endif

}


struct Ipv4 * sgIpv4Last( 
   struct Source * s )
{
   struct Ipv4 * ipv4;
   struct Ipv4 * last;

   last = NULL;
   for (ipv4 = s->ipv4;  ipv4 != NULL;  ipv4 = ipv4->next)
      last = ipv4;

   return last;
}


struct Ipv6 * sgIpv6Last( 
   struct Source * s )
{
   struct Ipv6 * ipv6;
   struct Ipv6 * last;

   last = NULL;
   for (ipv6 = s->ipv6;  ipv6 != NULL;  ipv6 = ipv6->next)
      last = ipv6;

   return last;
}



void sgIpv6( 
   const char * name,
   int          type,
   char *       file,
   int          line  )
{
   struct Ipv6 *  p;

#if 0
   ufdbLogMessage( "   sgIpv6( \"%s\", %d )", name, type );
#endif

   if (lastSource->ipv6 == NULL) 
   {
      p = (struct Ipv6 *) ufdbCalloc( 1, sizeof(struct Ipv6) );
      lastSource->ipv6 = p;
      lastSource->lastipv6 = p;
   }
   else
   {
      p = sgIpv6Last( lastSource );
   }

   p->str = ufdbStrdup( name );

   if (type == SG_IPV6TYPE_HOST)
   {
      if (!sgValidateIPv6( p->str, &p->ipv6 ))
      {
         ufdbLogFatalError( "incorrect IPv6 address \"%s\" in %s line %d", 
                            p->str, file, line );
      }
   }
   else if (type == SG_IPV6TYPE_CIDR)
   {
      char * s = strchr( p->str, '/' );
      if (s == NULL)
      {
         ufdbLogFatalError( "IPv6 net has no '/' in \"%s\" in %s line %d", 
                            p->str, file, line );
      }
      else
      {
         *s = '\0';
         p->cidr = (unsigned long) atoi( s+1 );
         if (p->cidr > 128) 
         {
            ufdbLogFatalError( "IPv6 address CIDR out of range \"%s\" in %s line %d", 
                               p->str, file, line );
            p->cidr = 128;
         }
         if (!sgValidateIPv6( p->str, &p->ipv6 ))
         {
            ufdbLogFatalError( "incorrect IPv6 address \"%s\" in %s line %d", 
                               p->str, file, line );
         }
         *s = '/';
      }
   }
   else
   {
      ufdbLogFatalError( "sgIpv6 called with unsupported type %d", type );
   }

   p->type = type;
   p->next = (struct Ipv6 *) ufdbCalloc( 1, sizeof(struct Ipv6) );

#if 1
   if (UFDBglobalDebug)
   {
      char newaddr[INET6_ADDRSTRLEN+1];

      newaddr[0] = '\0';
      inet_ntop( AF_INET6, (void *) &p->ipv6, newaddr, INET6_ADDRSTRLEN );
      ufdbLogMessage( "   sgIpv6 %2d  \"%s\"  %s %d", p->type, 
                      p->str==NULL ? "NULL" : p->str, newaddr, p->cidr );
   }
#endif
}


/*
 * ACL functions
 */

void sgAcl( 
   char *          name,
   char *          value, 
   int             within )
{
   struct Acl *    acl;
   struct Source * source;

#if UFDB_DEBUG
   ufdbLogMessage( "sgAcl name=\"%s\" value=\"%s\" within=%d line=%d", 
                   name==NULL?"NULL":name,  value==NULL?"NULL":value, within, lineno );
#endif

   if (AclList != NULL) 
   {
      if (sgAclFindName(name) != NULL && !UFDBglobalReuseAclNames)
      {
	 ufdbLogFatalError( "line %d: ACL \"%s\" is already defined in configuration file %s",
			    lineno, name, UFDBglobalConfigFile );
      }
   }

  if (within == UFDB_ACL_ELSE)
  {
     if (lastAcl == NULL)
     {
        ufdbLogFatalError( "line %d: ACL \"else\" has no parent ACL", lineno );
        return;
     }
     if (name == NULL)
        name = ufdbStrdup( lastAcl->name );
  }

  acl = (struct Acl *) ufdbMalloc( sizeof(struct Acl) );

  source = NULL;
  if (strcmp(name,"default") == 0)
  {
    defaultAcl = acl;
  }
  else
  {
    if ((source = defSourceFindName(name)) == NULL)
    {
      ufdbLogFatalError( "line %d: ACL source \"%s\" is not defined in configuration file %s",
		         lineno, name, UFDBglobalConfigFile );
      ufdbFree( name );
      return;
    }
  }

  acl->name = name;
  acl->active = within == UFDB_ACL_ELSE ? 0 : 1;
  acl->source = source;
  acl->pass = NULL;
  acl->implicitPass = NULL;
  acl->hasTerminatorNone = 0;
  acl->rewriteDefault = 1;
  acl->rewrite = NULL;
  acl->redirect = NULL;
  acl->time = NULL;
  acl->within = within;
  acl->next = NULL;

  if (value != NULL) 
  {
    struct ufdbTime * t;
    if ((t = sgTimeFindName(value)) == NULL) 
    {
      ufdbLogFatalError( "line %d: ACL time %s is not defined in configuration file %s",
		         lineno, value, UFDBglobalConfigFile );
      return;
    }
    acl->time = t;
  }

  if (AclList == NULL) 
  {
    AclList = acl;
    lastAcl = acl;
  }
  else
  {
    lastAcl->next = acl;
    lastAcl = acl;
  }
}


void sgAclSetValue( 
  const char *         what,
  char *               value, 
  int                  allowed ) 
{
  struct Category *    cat = NULL;
  struct AclCategory * aclcat;
  int                  type;
  
#if UFDB_DEBUG
   ufdbLogMessage( "sgAclSetValue %s %s%s", what, allowed?"":"!", value );
#endif

  if (lastAcl == NULL)
  {
     ufdbLogError( "error in configuration file on line %d: cannot set value for \"%s\" because there is no defined ACL", 
                   lineno, what );
     ufdbFree( value );
     return;
  }

  type = ACL_TYPE_TERMINATOR;

  if (strcmp(what,"pass") == 0)
  {
    if (strcmp(value,"any")==0 || strcmp(value,"all")==0)
    {
       if (!allowed)
          ufdbLogFatalError( "error in configuration file on line %d: do not use '!any' or '!all'.  Use 'none' instead.", lineno );
       allowed = 1;
    }
    else if (strcmp(value,"none") == 0)
    {
       if (!allowed)
          ufdbLogFatalError( "error in configuration file on line %d: do not use '!none'.  Use 'any' instead.", lineno );
       allowed = 0;
    }
    else if (strcmp(value,"in-addr") == 0) {
      type = ACL_TYPE_INADDR;
    }
    else
    {
      if ((cat = ufdbCategoryFindName(value)) == NULL) 
      {
	ufdbLogFatalError( "ACL category \"%s\" (line %d) is not defined in configuration file %s",
			   value, lineno, UFDBglobalConfigFile );
	ufdbFree( value );
        return;
      } 
      type = ACL_TYPE_DEFAULT;
    }

    aclcat = (struct AclCategory *) ufdbMallocAligned( 64, sizeof(struct AclCategory) );
    aclcat->name = value;
    aclcat->cat = cat;
    aclcat->access = allowed;
    aclcat->type = type;
    aclcat->next = NULL;
    aclcat->nblocks = 0;
    aclcat->nmatches = 0;

    if (lastAcl->pass == NULL) {
      lastAcl->pass = aclcat;
    } else {
      lastAclCategory->next = aclcat;
    }
    lastAclCategory = aclcat;
  }
  else if (strcmp(what,"redirect") == 0)
  {
    if (strcmp(value,"default") != 0)
    {
      lastAcl->redirect = value;
    }
    else 
    {
      lastAcl->redirect = NULL;
      ufdbFree( value );
    }
  }
  else if (strcmp(what,"rewrite") == 0)
  {
    if (strcmp(value,"none") == 0)
    {
      lastAcl->rewriteDefault = 0;
      lastAcl->rewrite = NULL;
    }
    else
    {
      struct sgRewrite * rewrite;

      if ((rewrite = sgRewriteFindName(value)) == NULL) 
      {
	ufdbLogFatalError( "rewrite %s is not defined in configuration file %s",
			   value, UFDBglobalConfigFile );
      }
      lastAcl->rewriteDefault = 0;
      lastAcl->rewrite = rewrite;
    }
    ufdbFree( value );
  }
}


struct Acl * sgAclFindName( 
  char *       name )
{
  struct Acl * p;

  if (name == NULL)
     return NULL;

  for (p = AclList;  p != NULL;  p = p->next)
  {
    if (strcmp(name,p->name) == 0)
      return p;
  }

  return NULL;
}


void yyerror( char * s )
{
   ufdbLogFatalError( "line %d: %s in configuration file %s", lineno, s, UFDBglobalConfigFile );
}


int yywrap()
{
  return 1;
}

