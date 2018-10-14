/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
    REDIRECT_DEFAULT_URL = 340,
    WITHIN = 341,
    OUTSIDE = 342,
    ELSE = 343,
    ANONYMOUS = 344,
    SPORADIC = 345,
    PIDFILE = 346,
    LOGFILE = 347,
    LOGDIR = 348,
    LOGPASS = 349,
    LOGBLOCK = 350,
    LOGALL = 351,
    LOGALL_HTTPD = 352,
    MAIL_SERVER = 353,
    MY_HOSTNAME = 354,
    ADMIN_EMAIL = 355,
    SENDER_EMAIL = 356,
    EXTERNAL_STATUS_COMMAND = 357,
    TOKEN_ALLOW = 358,
    TOKEN_DENY = 359,
    YOUTUBE_EDUFILTER = 360,
    YOUTUBE_EDUFILTER_ID = 361,
    ALLOW_GOOGLE_HTTPS_USING_IP = 362,
    URL_LOOKUP_RESULT_DB_RELOAD = 363,
    URL_LOOKUP_RESULT_FATAL_ERROR = 364,
    URL_LOOKUP_DELAY_DB_RELOAD = 365,
    OPTION = 366,
    UFDB_SHOW_URL_DETAILS = 367,
    UFDB_LOG_URL_DETAILS = 368,
    SQUID_VERSION = 369,
    SQUID_USES_SSL_BUMP = 370,
    UPLOAD_CRASH_REPORTS = 371,
    LOOKUP_REVERSE_IP = 372,
    USE_IPV6_ON_WAN = 373,
    STRIP_DOMAIN_FROM_USERNAME = 374,
    UFDB_DEBUG_FILTER = 375,
    UFDB_DEBUG_SKYPE_PROBES = 376,
    UFDB_DEBUG_GTALK_PROBES = 377,
    UFDB_DEBUG_YAHOOMSG_PROBES = 378,
    UFDB_DEBUG_AIM_PROBES = 379,
    UFDB_DEBUG_FBCHAT_PROBES = 380,
    UFDB_DEBUG_CITRIXONLINE_PROBES = 381,
    UFDB_EXPRESSION_OPTIMISATION = 382,
    UFDB_EXPRESSION_DEBUG = 383,
    UFDB_DEBUG_EXTERNAL_SCRIPTS = 384,
    UFDB_NUM_WORKER_THREADS = 385,
    ENFORCE_HTTPS_WITH_HOSTNAME = 386,
    ENFORCE_HTTPS_OFFICAL_CERTIFICATE = 387,
    ALLOW_SKYPE_OVER_HTTPS = 388,
    ALLOW_UNKNOWN_PROTOCOL_OVER_HTTPS = 389,
    ALLOW_GTALK_OVER_HTTPS = 390,
    ALLOW_YAHOOMSG_OVER_HTTPS = 391,
    ALLOW_AIM_OVER_HTTPS = 392,
    ALLOW_FBCHAT_OVER_HTTPS = 393,
    ALLOW_CITRIXONLINE_OVER_HTTPS = 394,
    ALLOW_ANYDESK_OVER_HTTPS = 395,
    ALLOW_TEAMVIEWER_OVER_HTTPS = 396,
    ANALYSE_UNCATEGORISED = 397,
    LOG_UNCATEGORISED_URLS = 398,
    UPLOAD_STATS = 399,
    SAFE_SEARCH = 400,
    MAX_LOGFILE_SIZE = 401,
    DENY_MODE = 402,
    REUSE_ACL_NAMES = 403
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
#define REDIRECT_DEFAULT_URL 340
#define WITHIN 341
#define OUTSIDE 342
#define ELSE 343
#define ANONYMOUS 344
#define SPORADIC 345
#define PIDFILE 346
#define LOGFILE 347
#define LOGDIR 348
#define LOGPASS 349
#define LOGBLOCK 350
#define LOGALL 351
#define LOGALL_HTTPD 352
#define MAIL_SERVER 353
#define MY_HOSTNAME 354
#define ADMIN_EMAIL 355
#define SENDER_EMAIL 356
#define EXTERNAL_STATUS_COMMAND 357
#define TOKEN_ALLOW 358
#define TOKEN_DENY 359
#define YOUTUBE_EDUFILTER 360
#define YOUTUBE_EDUFILTER_ID 361
#define ALLOW_GOOGLE_HTTPS_USING_IP 362
#define URL_LOOKUP_RESULT_DB_RELOAD 363
#define URL_LOOKUP_RESULT_FATAL_ERROR 364
#define URL_LOOKUP_DELAY_DB_RELOAD 365
#define OPTION 366
#define UFDB_SHOW_URL_DETAILS 367
#define UFDB_LOG_URL_DETAILS 368
#define SQUID_VERSION 369
#define SQUID_USES_SSL_BUMP 370
#define UPLOAD_CRASH_REPORTS 371
#define LOOKUP_REVERSE_IP 372
#define USE_IPV6_ON_WAN 373
#define STRIP_DOMAIN_FROM_USERNAME 374
#define UFDB_DEBUG_FILTER 375
#define UFDB_DEBUG_SKYPE_PROBES 376
#define UFDB_DEBUG_GTALK_PROBES 377
#define UFDB_DEBUG_YAHOOMSG_PROBES 378
#define UFDB_DEBUG_AIM_PROBES 379
#define UFDB_DEBUG_FBCHAT_PROBES 380
#define UFDB_DEBUG_CITRIXONLINE_PROBES 381
#define UFDB_EXPRESSION_OPTIMISATION 382
#define UFDB_EXPRESSION_DEBUG 383
#define UFDB_DEBUG_EXTERNAL_SCRIPTS 384
#define UFDB_NUM_WORKER_THREADS 385
#define ENFORCE_HTTPS_WITH_HOSTNAME 386
#define ENFORCE_HTTPS_OFFICAL_CERTIFICATE 387
#define ALLOW_SKYPE_OVER_HTTPS 388
#define ALLOW_UNKNOWN_PROTOCOL_OVER_HTTPS 389
#define ALLOW_GTALK_OVER_HTTPS 390
#define ALLOW_YAHOOMSG_OVER_HTTPS 391
#define ALLOW_AIM_OVER_HTTPS 392
#define ALLOW_FBCHAT_OVER_HTTPS 393
#define ALLOW_CITRIXONLINE_OVER_HTTPS 394
#define ALLOW_ANYDESK_OVER_HTTPS 395
#define ALLOW_TEAMVIEWER_OVER_HTTPS 396
#define ANALYSE_UNCATEGORISED 397
#define LOG_UNCATEGORISED_URLS 398
#define UPLOAD_STATS 399
#define SAFE_SEARCH 400
#define MAX_LOGFILE_SIZE 401
#define DENY_MODE 402
#define REUSE_ACL_NAMES 403

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 112 "sg.y" /* yacc.c:1909  */

  char * string;
  char * tval;
  char * dval;
  char * dvalcron;
  int    integer;

#line 358 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
