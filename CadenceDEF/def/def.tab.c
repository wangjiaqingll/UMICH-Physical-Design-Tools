/* A Bison parser, made by GNU Bison 3.0.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         defyyparse
#define yylex           defyylex
#define yyerror         defyyerror
#define yydebug         defyydebug
#define yynerrs         defyynerrs

#define yylval          defyylval
#define yychar          defyychar

/* Copy the first part of user declarations.  */
#line 15 "def.y" /* yacc.c:339  */

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "defrReader.hpp"
#include "lex.h"
#include "defiUser.hpp"
#include "defrCallBacks.hpp"

// TX_DIR:TRANSLATION ON

extern int dumb_mode;     // Read next N tokens in dumb mode
extern int no_num;        // No numbers for the next N tokens
extern int new_is_keyword;
extern int nondef_is_keyword;
extern char* History_text;

double save_x;
double save_y;

int names_case_sensitive = FALSE;
int defRetVal;

int shield = FALSE;       // To identify if the path is shield for 5.3

/* From def_keywords.cpp */
extern char* ringCopy(const char* string);


/* Macro to describe how we handle a callback.
 * If the function was set then call it.
 * If the function returns non zero then there was an error
 * so call the error routine and exit.
 */
#define CALLBACK(func, typ, data) \
    if (func) { \
      if ((defRetVal = (*func)(typ, data, defrUserData)) == PARSE_OK) { \
      } else if (defRetVal == STOP_PARSE) { \
	return defRetVal; \
      } else { \
        yyerror("Error in callback"); \
	return defRetVal; \
      } \
    }




#define FIXED 1
#define COVER 2
#define PLACED 3
#define UNPLACED 4

#line 128 "def.tab.c" /* yacc.c:339  */

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
   by #include "def.tab.h".  */
#ifndef YY_DEFYY_DEF_TAB_H_INCLUDED
# define YY_DEFYY_DEF_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int defyydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    QSTRING = 258,
    T_STRING = 259,
    SITE_PATTERN = 260,
    NUMBER = 261,
    K_HISTORY = 262,
    K_NAMESCASESENSITIVE = 263,
    K_DESIGN = 264,
    K_VIAS = 265,
    K_TECH = 266,
    K_UNITS = 267,
    K_ARRAY = 268,
    K_FLOORPLAN = 269,
    K_SITE = 270,
    K_CANPLACE = 271,
    K_CANNOTOCCUPY = 272,
    K_DIE_AREA = 273,
    K_PINS = 274,
    K_DEFAULTCAP = 275,
    K_MINPINS = 276,
    K_WIRECAP = 277,
    K_TRACKS = 278,
    K_GCELLGRID = 279,
    K_DO = 280,
    K_BY = 281,
    K_STEP = 282,
    K_LAYER = 283,
    K_ROW = 284,
    K_RECT = 285,
    K_COMPS = 286,
    K_COMP_GEN = 287,
    K_SOURCE = 288,
    K_WEIGHT = 289,
    K_EEQMASTER = 290,
    K_FIXED = 291,
    K_COVER = 292,
    K_UNPLACED = 293,
    K_PLACED = 294,
    K_FOREIGN = 295,
    K_REGION = 296,
    K_REGIONS = 297,
    K_NETS = 298,
    K_START_NET = 299,
    K_MUSTJOIN = 300,
    K_ORIGINAL = 301,
    K_USE = 302,
    K_STYLE = 303,
    K_PATTERN = 304,
    K_PATTERNNAME = 305,
    K_ESTCAP = 306,
    K_ROUTED = 307,
    K_NEW = 308,
    K_SNETS = 309,
    K_SHAPE = 310,
    K_WIDTH = 311,
    K_VOLTAGE = 312,
    K_SPACING = 313,
    K_NONDEFAULTRULE = 314,
    K_N = 315,
    K_S = 316,
    K_E = 317,
    K_W = 318,
    K_FN = 319,
    K_FE = 320,
    K_FS = 321,
    K_FW = 322,
    K_GROUPS = 323,
    K_GROUP = 324,
    K_SOFT = 325,
    K_MAXX = 326,
    K_MAXY = 327,
    K_MAXHALFPERIMETER = 328,
    K_CONSTRAINTS = 329,
    K_NET = 330,
    K_PATH = 331,
    K_SUM = 332,
    K_DIFF = 333,
    K_SCANCHAINS = 334,
    K_START = 335,
    K_FLOATING = 336,
    K_ORDERED = 337,
    K_STOP = 338,
    K_IN = 339,
    K_OUT = 340,
    K_RISEMIN = 341,
    K_RISEMAX = 342,
    K_FALLMIN = 343,
    K_FALLMAX = 344,
    K_WIREDLOGIC = 345,
    K_MAXDIST = 346,
    K_ASSERTIONS = 347,
    K_DISTANCE = 348,
    K_MICRONS = 349,
    K_END = 350,
    K_IOTIMINGS = 351,
    K_RISE = 352,
    K_FALL = 353,
    K_VARIABLE = 354,
    K_SLEWRATE = 355,
    K_CAPACITANCE = 356,
    K_DRIVECELL = 357,
    K_FROMPIN = 358,
    K_TOPIN = 359,
    K_PARALLEL = 360,
    K_TIMINGDISABLES = 361,
    K_THRUPIN = 362,
    K_MACRO = 363,
    K_PARTITIONS = 364,
    K_TURNOFF = 365,
    K_FROMCLOCKPIN = 366,
    K_FROMCOMPPIN = 367,
    K_FROMIOPIN = 368,
    K_TOCLOCKPIN = 369,
    K_TOCOMPPIN = 370,
    K_TOIOPIN = 371,
    K_SETUPRISE = 372,
    K_SETUPFALL = 373,
    K_HOLDRISE = 374,
    K_HOLDFALL = 375,
    K_VPIN = 376,
    K_SUBNET = 377,
    K_XTALK = 378,
    K_PIN = 379,
    K_SYNTHESIZED = 380,
    K_DEFINE = 381,
    K_DEFINES = 382,
    K_DEFINEB = 383,
    K_IF = 384,
    K_THEN = 385,
    K_ELSE = 386,
    K_FALSE = 387,
    K_TRUE = 388,
    K_EQ = 389,
    K_NE = 390,
    K_LE = 391,
    K_LT = 392,
    K_GE = 393,
    K_GT = 394,
    K_OR = 395,
    K_AND = 396,
    K_NOT = 397,
    K_SPECIAL = 398,
    K_DIRECTION = 399,
    K_RANGE = 400,
    K_FPC = 401,
    K_HORIZONTAL = 402,
    K_VERTICAL = 403,
    K_ALIGN = 404,
    K_MIN = 405,
    K_MAX = 406,
    K_EQUAL = 407,
    K_BOTTOMLEFT = 408,
    K_TOPRIGHT = 409,
    K_ROWS = 410,
    K_TAPER = 411,
    K_TAPERRULE = 412,
    K_VERSION = 413,
    K_DIVIDERCHAR = 414,
    K_BUSBITCHARS = 415,
    K_PROPERTYDEFINITIONS = 416,
    K_STRING = 417,
    K_REAL = 418,
    K_INTEGER = 419,
    K_PROPERTY = 420,
    K_BEGINEXT = 421,
    K_ENDEXT = 422,
    K_NAMEMAPSTRING = 423,
    K_COMPONENT = 424,
    K_PINPROPERTIES = 425,
    K_COMMONSCANPINS = 426,
    K_SNET = 427,
    K_COMPONENTPIN = 428,
    K_REENTRANTPATHS = 429,
    K_SHIELD = 430,
    K_SHIELDNET = 431,
    K_NOSHIELD = 432
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 69 "def.y" /* yacc.c:355  */

        double dval ;
        int    integer ;
        char * string ;
        int    keyword ;  /* really just a nop */
        struct defpoint pt;
        defTOKEN *tk;

#line 355 "def.tab.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE defyylval;

int defyyparse (void);

#endif /* !YY_DEFYY_DEF_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 370 "def.tab.c" /* yacc.c:358  */

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
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   942

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  185
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  365
/* YYNRULES -- Number of rules.  */
#define YYNRULES  622
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1091

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   432

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
     181,   182,   183,   180,   184,   179,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   178,
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
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   136,   136,   137,   140,   141,   145,   146,   146,   157,
     158,   159,   162,   162,   162,   162,   163,   163,   163,   163,
     164,   164,   164,   165,   165,   165,   166,   166,   169,   169,
     169,   169,   170,   170,   171,   171,   171,   171,   172,   172,
     172,   172,   172,   173,   173,   173,   176,   176,   180,   184,
     184,   188,   188,   192,   192,   196,   201,   200,   206,   207,
     211,   211,   219,   219,   227,   227,   235,   235,   243,   243,
     251,   251,   259,   259,   267,   267,   276,   275,   286,   288,
     290,   292,   294,   298,   299,   303,   307,   311,   315,   315,
     329,   329,   342,   342,   356,   357,   358,   359,   360,   361,
     362,   363,   366,   376,   380,   384,   385,   388,   398,   402,
     406,   410,   411,   414,   415,   416,   414,   425,   426,   429,
     435,   438,   444,   444,   449,   449,   455,   463,   467,   469,
     467,   483,   484,   487,   487,   491,   492,   495,   500,   499,
     514,   514,   520,   521,   521,   525,   526,   529,   536,   536,
     546,   550,   554,   557,   561,   562,   565,   566,   565,   571,
     572,   575,   575,   580,   580,   585,   590,   597,   603,   609,
     616,   620,   624,   628,   629,   633,   634,   633,   640,   643,
     648,   649,   652,   652,   656,   657,   660,   664,   667,   671,
     672,   675,   679,   690,   690,   699,   700,   705,   712,   713,
     716,   716,   716,   716,   717,   717,   717,   718,   718,   721,
     725,   725,   732,   732,   742,   743,   747,   747,   755,   757,
     764,   769,   776,   776,   781,   782,   785,   792,   796,   797,
     796,   808,   810,   814,   824,   826,   828,   830,   834,   841,
     845,   848,   852,   853,   856,   866,   871,   871,   878,   879,
     882,   882,   892,   892,   899,   899,   906,   906,   916,   917,
     922,   927,   928,   931,   931,   937,   937,   940,   940,   943,
     943,   946,   949,   952,   955,   955,   958,   958,   961,   961,
     964,   966,   966,   969,   970,   969,   980,   980,   996,   996,
     999,  1003,  1004,  1007,  1014,  1019,  1019,  1023,  1024,  1024,
    1028,  1037,  1041,  1042,  1046,  1048,  1050,  1054,  1056,  1058,
    1063,  1065,  1069,  1069,  1074,  1076,  1073,  1081,  1082,  1086,
    1088,  1094,  1101,  1107,  1113,  1118,  1125,  1132,  1139,  1149,
    1150,  1152,  1152,  1158,  1159,  1165,  1166,  1171,  1175,  1178,
    1179,  1182,  1186,  1187,  1190,  1190,  1191,  1191,  1194,  1194,
    1200,  1201,  1200,  1211,  1211,  1214,  1214,  1217,  1217,  1220,
    1223,  1226,  1226,  1229,  1229,  1232,  1232,  1235,  1239,  1239,
    1245,  1251,  1252,  1251,  1260,  1261,  1267,  1268,  1272,  1276,
    1280,  1283,  1287,  1288,  1291,  1295,  1295,  1306,  1307,  1311,
    1318,  1319,  1322,  1324,  1324,  1326,  1326,  1328,  1334,  1337,
    1343,  1344,  1347,  1351,  1352,  1356,  1360,  1364,  1370,  1374,
    1377,  1380,  1389,  1398,  1399,  1402,  1403,  1415,  1429,  1429,
    1434,  1434,  1440,  1445,  1452,  1453,  1457,  1457,  1467,  1468,
    1472,  1473,  1476,  1481,  1486,  1491,  1498,  1502,  1506,  1509,
    1514,  1515,  1519,  1523,  1523,  1530,  1531,  1536,  1537,  1541,
    1541,  1545,  1545,  1549,  1548,  1558,  1558,  1562,  1562,  1566,
    1574,  1575,  1584,  1600,  1601,  1605,  1604,  1616,  1617,  1626,
    1641,  1642,  1646,  1645,  1655,  1656,  1665,  1680,  1684,  1687,
    1692,  1693,  1697,  1702,  1702,  1709,  1710,  1714,  1719,  1724,
    1729,  1729,  1735,  1735,  1741,  1741,  1747,  1752,  1757,  1757,
    1760,  1764,  1768,  1773,  1774,  1778,  1779,  1778,  1784,  1786,
    1790,  1792,  1794,  1796,  1800,  1801,  1805,  1804,  1808,  1807,
    1812,  1813,  1816,  1816,  1818,  1818,  1822,  1825,  1830,  1831,
    1835,  1836,  1835,  1843,  1843,  1850,  1850,  1857,  1864,  1865,
    1864,  1870,  1870,  1877,  1882,  1885,  1890,  1891,  1895,  1900,
    1900,  1907,  1908,  1916,  1917,  1919,  1924,  1925,  1927,  1931,
    1932,  1935,  1935,  1941,  1941,  1947,  1947,  1953,  1953,  1959,
    1959,  1965,  1965,  1970,  1976,  1975,  1980,  1981,  1985,  1990,
    1997,  1998,  2002,  2003,  2006,  2008,  2010,  2012,  2017,  2018,
    2021,  2024,  2027,  2030,  2035,  2040,  2041,  2045,  2045,  2056,
    2057,  2061,  2062,  2065,  2070,  2070,  2074,  2077,  2083,  2084,
    2088,  2092,  2093,  2096,  2097,  2096,  2105,  2106,  2109,  2109,
    2114,  2115,  2118
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "QSTRING", "T_STRING", "SITE_PATTERN",
  "NUMBER", "K_HISTORY", "K_NAMESCASESENSITIVE", "K_DESIGN", "K_VIAS",
  "K_TECH", "K_UNITS", "K_ARRAY", "K_FLOORPLAN", "K_SITE", "K_CANPLACE",
  "K_CANNOTOCCUPY", "K_DIE_AREA", "K_PINS", "K_DEFAULTCAP", "K_MINPINS",
  "K_WIRECAP", "K_TRACKS", "K_GCELLGRID", "K_DO", "K_BY", "K_STEP",
  "K_LAYER", "K_ROW", "K_RECT", "K_COMPS", "K_COMP_GEN", "K_SOURCE",
  "K_WEIGHT", "K_EEQMASTER", "K_FIXED", "K_COVER", "K_UNPLACED",
  "K_PLACED", "K_FOREIGN", "K_REGION", "K_REGIONS", "K_NETS",
  "K_START_NET", "K_MUSTJOIN", "K_ORIGINAL", "K_USE", "K_STYLE",
  "K_PATTERN", "K_PATTERNNAME", "K_ESTCAP", "K_ROUTED", "K_NEW", "K_SNETS",
  "K_SHAPE", "K_WIDTH", "K_VOLTAGE", "K_SPACING", "K_NONDEFAULTRULE",
  "K_N", "K_S", "K_E", "K_W", "K_FN", "K_FE", "K_FS", "K_FW", "K_GROUPS",
  "K_GROUP", "K_SOFT", "K_MAXX", "K_MAXY", "K_MAXHALFPERIMETER",
  "K_CONSTRAINTS", "K_NET", "K_PATH", "K_SUM", "K_DIFF", "K_SCANCHAINS",
  "K_START", "K_FLOATING", "K_ORDERED", "K_STOP", "K_IN", "K_OUT",
  "K_RISEMIN", "K_RISEMAX", "K_FALLMIN", "K_FALLMAX", "K_WIREDLOGIC",
  "K_MAXDIST", "K_ASSERTIONS", "K_DISTANCE", "K_MICRONS", "K_END",
  "K_IOTIMINGS", "K_RISE", "K_FALL", "K_VARIABLE", "K_SLEWRATE",
  "K_CAPACITANCE", "K_DRIVECELL", "K_FROMPIN", "K_TOPIN", "K_PARALLEL",
  "K_TIMINGDISABLES", "K_THRUPIN", "K_MACRO", "K_PARTITIONS", "K_TURNOFF",
  "K_FROMCLOCKPIN", "K_FROMCOMPPIN", "K_FROMIOPIN", "K_TOCLOCKPIN",
  "K_TOCOMPPIN", "K_TOIOPIN", "K_SETUPRISE", "K_SETUPFALL", "K_HOLDRISE",
  "K_HOLDFALL", "K_VPIN", "K_SUBNET", "K_XTALK", "K_PIN", "K_SYNTHESIZED",
  "K_DEFINE", "K_DEFINES", "K_DEFINEB", "K_IF", "K_THEN", "K_ELSE",
  "K_FALSE", "K_TRUE", "K_EQ", "K_NE", "K_LE", "K_LT", "K_GE", "K_GT",
  "K_OR", "K_AND", "K_NOT", "K_SPECIAL", "K_DIRECTION", "K_RANGE", "K_FPC",
  "K_HORIZONTAL", "K_VERTICAL", "K_ALIGN", "K_MIN", "K_MAX", "K_EQUAL",
  "K_BOTTOMLEFT", "K_TOPRIGHT", "K_ROWS", "K_TAPER", "K_TAPERRULE",
  "K_VERSION", "K_DIVIDERCHAR", "K_BUSBITCHARS", "K_PROPERTYDEFINITIONS",
  "K_STRING", "K_REAL", "K_INTEGER", "K_PROPERTY", "K_BEGINEXT",
  "K_ENDEXT", "K_NAMEMAPSTRING", "K_COMPONENT", "K_PINPROPERTIES",
  "K_COMMONSCANPINS", "K_SNET", "K_COMPONENTPIN", "K_REENTRANTPATHS",
  "K_SHIELD", "K_SHIELDNET", "K_NOSHIELD", "';'", "'-'", "'+'", "'('",
  "')'", "'*'", "','", "$accept", "def_file", "version_stmt",
  "case_sens_stmt", "$@1", "rules", "rule", "design_section",
  "design_name", "$@2", "end_design", "tech_name", "$@3", "array_name",
  "$@4", "floorplan_name", "$@5", "history", "prop_def_section", "$@6",
  "property_defs", "property_def", "$@7", "$@8", "$@9", "$@10", "$@11",
  "$@12", "$@13", "$@14", "$@15", "property_type_and_val", "opt_num_val",
  "units", "divider_char", "bus_bit_chars", "site", "$@16", "canplace",
  "$@17", "cannotoccupy", "$@18", "orient", "die_area", "pin_cap_rule",
  "start_def_cap", "pin_caps", "pin_cap", "end_def_cap", "pin_rule",
  "start_pins", "pins", "pin", "$@19", "$@20", "$@21", "pin_options",
  "pin_option", "$@22", "$@23", "end_pins", "row_rule", "$@24", "$@25",
  "row_options", "row_option", "$@26", "row_prop_list", "row_prop",
  "tracks_rule", "$@27", "track_start", "$@28", "track_layer_statement",
  "$@29", "track_layers", "track_layer", "gcellgrid", "$@30",
  "extension_section", "extension_stmt", "via_section", "via",
  "via_declarations", "via_declaration", "$@31", "$@32", "layer_stmts",
  "layer_stmt", "$@33", "$@34", "pt", "via_end", "regions_section",
  "regions_start", "regions_stmts", "regions_stmt", "$@35", "$@36",
  "rect_list", "region_options", "region_option", "$@37",
  "region_prop_list", "region_prop", "comps_section", "start_comps",
  "comps_rule", "comp", "comp_start", "comp_id_and_name", "$@38",
  "comp_net_list", "comp_options", "comp_option", "comp_extension_stmt",
  "comp_eeq", "$@39", "comp_generate", "$@40", "opt_pattern",
  "comp_source", "$@41", "comp_region", "comp_pnt_list", "comp_property",
  "$@42", "comp_prop_list", "comp_prop", "comp_region_start",
  "comp_foreign", "$@43", "$@44", "opt_paren", "comp_type",
  "placement_status", "weight", "end_comps", "nets_section", "start_nets",
  "net_rules", "one_net", "net_and_connections", "net_start", "$@45",
  "net_connections", "net_connection", "$@46", "$@47", "$@48", "$@49",
  "conn_opt", "net_options", "net_option", "$@50", "$@51", "$@52", "$@53",
  "$@54", "$@55", "$@56", "$@57", "$@58", "$@59", "$@60", "$@61",
  "net_prop_list", "net_prop", "vpin_stmt", "vpin_begin", "$@62",
  "vpin_layer_opt", "$@63", "property_val", "vpin_options", "vpin_status",
  "net_type", "paths", "new_path", "$@64", "path", "$@65", "$@66",
  "path_item_list", "path_item", "path_pt", "opt_taper", "$@67",
  "opt_shape", "opt_width", "end_nets", "snets_section", "snet_rules",
  "snet_rule", "snet_options", "snet_option", "snet_other_option", "$@68",
  "$@69", "$@70", "$@71", "$@72", "$@73", "$@74", "$@75", "$@76",
  "snet_width", "$@77", "snet_voltage", "snet_spacing", "$@78", "$@79",
  "opt_snet_range", "opt_range", "start_snets", "end_snets",
  "groups_section", "groups_start", "group_rules", "group_rule",
  "start_group", "$@80", "group_members", "group_member", "group_options",
  "group_option", "$@81", "$@82", "group_region", "group_prop_list",
  "group_prop", "group_soft_options", "group_soft_option", "groups_end",
  "assertions_section", "constraint_section", "assertions_start",
  "constraints_start", "constraint_rules", "constraint_rule",
  "operand_rule", "operand", "$@83", "$@84", "operand_list",
  "wiredlogic_rule", "$@85", "opt_plus", "delay_specs", "delay_spec",
  "constraints_end", "assertions_end", "scanchains_section",
  "scanchain_start", "scanchain_rules", "scan_rule", "start_scan", "$@86",
  "scan_members", "opt_pin", "scan_member", "$@87", "$@88", "$@89", "$@90",
  "$@91", "opt_common_pins", "floating_inst_list", "one_floating_inst",
  "$@92", "floating_pins", "ordered_inst_list", "one_ordered_inst", "$@93",
  "ordered_pins", "scanchain_end", "iotiming_section", "iotiming_start",
  "iotiming_rules", "iotiming_rule", "start_iotiming", "$@94",
  "iotiming_members", "iotiming_member", "$@95", "$@96", "$@97",
  "risefall", "iotiming_end", "floorplan_contraints_section", "fp_start",
  "fp_stmts", "fp_stmt", "$@98", "$@99", "h_or_v", "constraint_type",
  "constrain_what_list", "constrain_what", "$@100", "$@101",
  "row_or_comp_list", "row_or_comp", "$@102", "$@103",
  "timingdisables_section", "timingdisables_start", "timingdisables_rules",
  "timingdisables_rule", "$@104", "$@105", "$@106", "$@107",
  "td_macro_option", "$@108", "$@109", "$@110", "timingdisables_end",
  "partitions_section", "partitions_start", "partition_rules",
  "partition_rule", "start_partition", "$@111", "turnoff", "turnoff_setup",
  "turnoff_hold", "partition_members", "partition_member", "$@112",
  "$@113", "$@114", "$@115", "$@116", "$@117", "minmaxpins", "$@118",
  "min_or_max_list", "min_or_max_member", "pin_list",
  "risefallminmax1_list", "risefallminmax1", "risefallminmax2_list",
  "risefallminmax2", "partitions_end", "comp_names", "comp_name", "$@119",
  "subnet_opt_syn", "subnet_options", "subnet_option", "$@120",
  "pin_props_section", "begin_pin_props", "opt_semi", "end_pin_props",
  "pin_prop_list", "pin_prop_terminal", "$@121", "$@122",
  "pin_prop_options", "pin_prop", "$@123", "pin_prop_name_value_list",
  "pin_prop_name_value", YY_NULLPTR
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
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,    59,    45,
      43,    40,    41,    42,    44
};
# endif

#define YYPACT_NINF -792

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-792)))

#define YYTABLE_NINF -10

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -103,    62,    86,   123,  -105,  -792,  -792,    65,  -792,   144,
    -792,   229,   -23,  -792,  -792,   157,  -792,    80,  -792,  -792,
    -792,  -792,  -792,    42,   171,   175,  -792,  -792,  -792,   189,
     222,   231,   253,   269,   288,   327,   346,   194,   347,   358,
     367,   368,   383,   384,  -792,  -792,   386,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,   388,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,   392,   219,   404,   318,   410,   411,   412,   413,   414,
       6,    42,   242,  -792,   417,   418,   419,   246,   247,   248,
     249,   250,   251,   252,   254,  -792,   255,   256,   257,   258,
     259,   260,  -792,   261,    32,   -74,  -792,   -39,   -38,   -36,
     -28,   -26,     0,     3,    28,    29,    30,    31,    39,    40,
      43,   262,  -792,   263,   425,   264,   265,   440,   441,   442,
       8,     9,   271,  -792,  -792,   444,   447,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,    61,  -792,  -792,   446,   433,  -792,  -792,   435,  -792,
    -792,  -792,   430,   450,  -792,  -792,  -792,   415,  -792,  -792,
     431,  -792,  -792,  -792,  -792,  -792,   420,  -792,  -792,  -792,
    -792,  -792,   402,  -792,  -792,  -792,   393,  -792,  -792,  -792,
    -792,   372,    38,  -792,  -792,  -792,  -792,   391,  -792,   387,
    -792,  -792,  -792,  -792,   373,   287,  -792,  -792,  -792,   324,
    -792,  -792,   365,   -16,  -792,  -792,   363,  -792,  -792,  -792,
    -792,   303,  -792,  -792,  -792,  -792,  -792,   296,  -792,  -792,
     469,   470,   471,   297,   298,   299,   301,  -792,   453,   478,
    -792,  -792,  -792,  -792,  -792,   325,  -792,  -792,  -792,  -792,
    -792,   465,  -792,  -792,   484,   483,  -792,   486,  -792,   487,
    -792,   488,   -81,     7,  -792,   489,    49,   -32,  -792,    96,
    -792,   490,   491,  -792,  -792,  -792,   315,   316,  -792,  -792,
    -792,  -792,   494,   104,  -792,  -792,   109,  -792,   495,  -792,
    -792,  -792,  -792,   322,  -792,   497,   108,  -792,   498,  -792,
     280,   280,   280,  -792,  -792,  -792,  -792,   499,   500,   503,
     504,   505,   506,   507,  -792,   508,   509,   510,   511,   512,
     323,   477,  -792,  -792,   513,  -792,   279,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,    12,  -792,  -792,    42,  -792,
    -792,  -792,  -792,  -792,    -1,  -792,  -792,  -792,   492,   335,
       5,  -792,  -792,   244,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,   154,   515,   517,   273,   273,   518,   204,
    -792,  -792,   -19,  -792,  -792,   519,  -792,  -792,   520,  -792,
     119,  -792,  -792,   -30,   521,   523,   524,  -792,   421,  -792,
     215,  -792,  -792,   525,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,   514,   516,   522,   526,   280,    99,    99,    99,
      99,    99,    99,    99,    99,    99,   352,   449,   527,  -792,
      42,  -792,  -792,  -792,   528,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,    42,    42,   280,  -792,   529,
    -792,  -792,  -792,  -792,  -792,  -792,   531,  -792,  -792,  -792,
    -792,   532,  -792,  -792,  -792,  -792,  -792,    42,   536,  -792,
    -792,  -792,  -792,   537,  -792,  -792,  -792,  -792,   538,  -792,
     539,  -792,  -792,  -792,  -792,  -792,     1,  -792,  -792,  -792,
     542,  -792,    -4,   201,   362,  -792,   218,  -792,  -792,  -792,
    -792,  -792,  -792,   544,   545,   546,  -792,  -792,  -792,   548,
    -792,    87,  -792,  -792,  -792,   547,   551,    47,   138,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,   550,   552,   553,
     554,   540,   549,   416,   416,   558,   354,   379,   385,   389,
     390,   394,   395,   396,   397,  -792,  -792,   541,   213,    42,
      42,   560,   562,  -792,   566,   573,  -792,  -792,    42,  -792,
     574,  -792,   575,   576,   577,   578,  -792,   579,   580,   581,
    -792,  -792,   582,  -792,   583,   584,    42,  -792,   585,   586,
     587,   588,  -792,   589,   590,   591,   592,  -792,   593,  -792,
     594,  -792,   595,   583,  -792,  -792,  -792,   596,  -792,   273,
    -792,  -792,   530,   597,   598,   599,   600,   603,  -792,  -792,
     604,   429,   432,  -792,  -792,  -792,   607,   606,   609,   216,
     533,   423,  -792,  -792,   424,  -792,  -792,   145,   612,   613,
     614,   616,   618,   619,  -792,   601,   602,   605,   448,   623,
    -792,   624,   626,   626,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,   620,  -792,   455,  -792,     4,  -792,
    -792,  -792,    42,   220,   621,  -792,  -792,    25,   630,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,   631,  -792,
     583,  -792,   610,  -792,  -792,    73,   632,   458,   458,   458,
    -792,  -792,  -792,  -792,  -792,   633,   634,   631,  -792,   610,
      13,   197,  -792,   637,  -792,   636,  -792,  -792,  -792,  -792,
     639,   640,   641,   639,   642,  -792,  -792,  -792,   643,   644,
    -792,   645,   646,   647,  -792,  -792,  -792,   650,   651,  -792,
    -792,  -792,  -792,   652,   653,  -792,   654,   655,  -792,   223,
     656,   658,   659,  -792,   622,   660,  -792,  -792,  -792,  -792,
     663,  -792,  -792,  -792,  -792,  -792,   482,  -792,  -792,  -792,
     662,  -792,  -792,   156,  -792,   479,   156,  -792,   610,   664,
    -792,  -792,  -792,  -792,  -792,  -792,    42,   458,   -65,  -792,
     493,   496,   501,  -792,  -792,   583,  -792,    42,  -792,   665,
     666,   667,  -792,   670,  -792,   502,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,   672,  -792,  -792,  -792,  -792,  -792,   226,
     673,   565,  -792,   225,   283,   291,   225,   283,   291,  -792,
     534,  -792,   657,   661,   668,   675,  -792,  -792,  -792,  -792,
     663,   678,   682,  -792,  -792,   683,  -792,  -792,  -792,  -792,
    -792,   214,  -792,  -792,   535,   583,   280,   543,  -792,  -792,
    -792,  -792,   468,   610,  -792,  -792,  -792,  -792,   156,  -792,
    -792,   555,   556,   557,  -792,   183,  -792,   685,  -792,  -792,
     684,   686,   687,   688,   283,  -792,   691,   692,   694,   695,
    -792,  -792,   283,  -792,   696,   697,   698,   679,   227,  -792,
      42,  -792,  -792,  -792,   701,  -792,   583,  -792,   669,   142,
    -792,  -792,  -792,   702,  -792,  -792,   703,  -792,   705,  -792,
     559,  -792,  -792,   563,   706,  -792,    33,   707,   708,   710,
     711,  -792,  -792,  -792,  -792,  -792,  -792,  -792,   712,   713,
     714,   715,  -792,     2,  -792,  -792,    42,    42,   718,   719,
     722,   610,   723,  -792,  -792,   561,   724,   725,   727,   728,
    -792,  -792,  -792,  -792,  -792,   729,   732,  -792,  -792,  -792,
    -792,  -792,   730,   567,   568,   569,   737,  -792,  -792,  -792,
     740,   280,  -792,   156,  -792,   570,  -792,  -792,   744,    14,
    -792,  -792,   571,   572,   745,   608,   608,  -792,   746,   749,
     156,  -792,  -792,  -792,  -792,  -792,   747,   752,  -792,  -792,
    -792,   635,   611,  -792,    16,    17,  -792,   615,   617,   625,
     -13,  -792,   753,  -792,  -792,  -792,  -792,    42,  -792,  -792,
    -792,    19,    20,    22,    23,    15,   754,   755,  -792,  -792,
    -792,  -792,   233,    42,   627,  -792,   628,  -792,   629,  -792,
     638,  -792,  -792,  -792,  -792,   757,   758,   759,   760,  -792,
     648,  -792,  -792,  -792,  -792,  -792,  -792,   649,   671,   674,
     676,  -792,  -792,  -792,  -792,  -792,  -792,   761,   156,  -792,
    -792
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       4,     0,     0,     6,     0,     1,     7,     0,     5,     0,
      11,     2,     0,    55,    46,     0,    49,     0,    51,    53,
      88,    90,    92,     0,     0,     0,   140,   148,   128,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    56,   150,     0,    10,    12,    28,
       3,    29,    42,    43,    30,    45,    31,    32,    33,    34,
      35,    36,    37,    38,   105,    39,   111,    44,    40,     0,
      41,    14,    13,   154,    23,   173,    15,   189,    16,   242,
      17,   339,    18,   382,    21,    20,   413,   413,    19,   440,
      22,   480,    24,   503,    25,   528,    26,   546,    27,   611,
       8,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   104,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,     0,     0,     0,     0,
       0,     0,    58,   608,     0,     0,   138,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   153,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   110,   141,     0,     0,   188,   172,   241,
     378,   381,   412,   439,   411,   479,   527,   545,   502,    86,
      87,     0,   609,   607,     0,     0,   106,   103,     0,   113,
     112,   109,     0,     0,   156,   155,   152,     0,   175,   174,
       0,   193,   190,   198,   195,   187,     0,   246,   243,   261,
     248,   240,     0,   342,   340,   338,     0,   385,   383,   387,
     380,     0,     0,   414,   415,   416,   409,     0,   410,     0,
     443,   441,   445,   438,     0,     0,   481,   485,   478,     0,
     505,   504,     0,     0,   529,   526,     0,   549,   547,   559,
     544,     0,   613,   606,   612,    47,    50,     0,    52,    54,
       0,     0,     0,     0,     0,     0,     0,   102,     0,     0,
      60,    72,    66,    68,    62,     0,    74,    70,    64,    76,
      59,     0,   108,   127,     0,     0,   170,     0,   171,     0,
     239,     0,     0,   192,   337,     0,     0,   245,   379,     0,
     408,     0,   390,   437,   418,   420,     0,     0,   426,   430,
     436,   477,     0,     0,   500,   483,     0,   501,     0,   543,
     530,   533,   535,     0,   594,     0,     0,   610,     0,    85,
       0,     0,     0,   166,   168,   167,   169,     0,     0,     0,
       0,     0,     0,     0,    57,     0,     0,     0,     0,     0,
       0,     0,   157,   176,     0,   191,     0,   209,   199,   208,
     206,   200,   201,   205,   207,     0,   204,   202,     0,   203,
     197,   196,   247,   244,     0,   290,   262,   280,   297,     0,
       0,   249,   341,     0,   367,   343,   347,   344,   345,   346,
     386,   389,   388,     0,     0,     0,     0,     0,     0,     0,
     444,   442,     0,   459,   446,     0,   492,   494,     0,   482,
       0,   497,   486,     0,     0,     0,     0,   537,   551,   548,
       0,   573,   560,     0,    94,    96,    95,    97,    98,    99,
     100,   101,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   159,
       0,   194,   212,   216,     0,   210,   234,   235,   237,   236,
     228,   227,   222,   151,   219,     0,   218,     0,   265,     0,
     307,   308,   267,   274,   276,   269,     0,   309,   278,   295,
     286,     0,   288,   281,   283,   263,   298,     0,     0,   250,
     254,   252,   353,     0,   355,   361,   363,   357,     0,   368,
       0,   371,   365,   350,   348,   384,     0,   397,   391,   419,
       0,   424,     0,     0,   428,   417,     0,   431,   449,   451,
     453,   455,   457,     0,     0,     0,   496,   498,   499,     0,
     490,     0,   508,   509,   506,     0,     0,     0,   553,   550,
     561,   563,   565,   567,   569,   571,   614,     0,     0,     0,
       0,     0,    80,   376,   376,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,   114,   142,     0,     0,
     180,     0,     0,   238,     0,     0,   224,   220,     0,   233,
       0,   271,     0,     0,     0,     0,   273,     0,     0,     0,
     272,   291,     0,   284,     0,     0,     0,   256,     0,     0,
       0,     0,   359,     0,     0,     0,     0,   360,     0,   370,
       0,   291,     0,     0,   395,   403,   393,     0,   422,     0,
     423,   429,     0,     0,     0,     0,     0,     0,   463,   470,
       0,   460,     0,   493,   495,   489,     0,     0,     0,     0,
       0,     0,   538,   541,     0,   554,   555,   556,     0,     0,
       0,     0,     0,     0,   616,     0,     0,     0,     0,     0,
      81,     0,    83,    83,    82,    61,    73,    67,    69,    63,
      75,    71,    65,    77,     0,   143,     0,   158,     0,   165,
     160,   178,     0,     0,   214,   217,   211,     0,   223,   221,
     266,   268,   275,   277,   270,   279,   296,   595,   289,   282,
       0,   314,   264,   310,   299,   302,     0,   258,   258,   258,
     354,   356,   362,   364,   358,     0,     0,   366,   351,   349,
       0,   392,   400,     0,   425,     0,   432,   433,   434,   435,
     447,   452,   454,   447,     0,   458,   484,   491,     0,     0,
     510,     0,     0,     0,   514,   531,   534,     0,     0,   536,
     557,   558,   552,     0,     0,   582,     0,     0,   582,     0,
       0,     0,     0,   149,     0,     0,    84,    79,    78,   115,
     145,   139,   161,   163,   179,   177,     0,   181,   215,   213,
       0,   231,   229,     0,   225,   601,     0,   292,   285,   335,
     312,   311,   305,   306,   304,   294,     0,   258,     0,   259,
       0,     0,     0,   369,   372,     0,   399,     0,   396,     0,
       0,     0,   404,   394,   421,     0,   448,   450,   465,   464,
     472,   471,   456,     0,   487,   488,   512,   511,   513,     0,
       0,     0,   542,     0,     0,   566,     0,     0,   572,   615,
       0,   617,     0,     0,     0,     0,   377,   117,   147,   144,
     145,     0,     0,   182,   232,     0,   301,   300,   226,   597,
     596,   287,   293,   336,   333,     0,     0,     0,   260,   251,
     255,   253,   374,   352,   398,   405,   406,   407,     0,   401,
     427,   467,   474,     0,   507,     0,   515,     0,   539,   576,
       0,     0,     0,     0,   564,   588,     0,     0,     0,     0,
     583,   576,   570,   618,     0,     0,     0,     0,     0,   146,
       0,   164,   184,   230,     0,   604,     0,   602,     0,   329,
     313,   303,   257,     0,   373,   402,     0,   466,     0,   473,
     461,   516,   518,     0,     0,   562,     0,     0,     0,     0,
       0,   589,   584,   586,   585,   587,   568,   620,     0,     0,
       0,     0,   116,     0,   118,   120,     0,     0,   183,     0,
       0,   603,     0,   330,   331,     0,     0,     0,     0,     0,
     520,   520,   532,   540,   574,     0,     0,   577,   590,   592,
     591,   593,   619,     0,     0,     0,     0,   124,   122,   119,
       0,     0,   162,     0,   185,   599,   605,   334,     0,     0,
     315,   375,     0,     0,     0,   517,   519,   580,     0,     0,
       0,   621,    89,    91,    93,   129,     0,     0,   121,   126,
     186,     0,     0,   332,     0,     0,   317,   468,   475,     0,
       0,   521,   575,   578,   579,   622,   131,     0,   123,   600,
     598,     0,     0,     0,     0,   316,     0,     0,   462,   524,
     522,   581,     0,     0,     0,   321,     0,   323,     0,   322,
       0,   324,   319,   318,   320,     0,     0,     0,     0,   130,
       0,   132,   125,   325,   327,   326,   328,     0,     0,     0,
       0,   133,   469,   476,   525,   523,   135,   134,     0,   136,
     137
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,   -82,    48,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -330,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -171,  -792,  -792,  -792,  -792,
    -289,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -111,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -196,  -792,  -792,  -792,  -792,
    -792,  -792,   677,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -665,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,   117,  -792,  -792,  -792,
    -792,  -792,  -792,  -778,  -792,  -792,  -379,  -607,  -792,  -792,
    -174,  -792,  -792,  -792,  -792,  -312,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,   212,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,   680,  -792,
    -792,  -217,  -792,  -792,   371,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,    36,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -664,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -201,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -130,  -792,  -792,  -792,  -792,
      18,  -792,   -64,  -791,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,  -792,
    -792,  -792,  -792,  -792,  -792
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     7,     9,    11,    47,    48,    49,   101,
      50,    51,   103,    52,   105,    53,   106,    54,    55,   132,
     181,   280,   339,   343,   347,   341,   342,   346,   340,   345,
     348,   556,   767,    56,    57,    58,    59,   107,    60,   108,
      61,   109,   432,    62,    63,    64,   134,   186,   187,    65,
      66,   135,   190,   284,   674,   847,   908,   954,  1017,  1016,
     191,    67,   116,  1036,  1052,  1071,  1086,  1087,  1089,    68,
     192,    69,   114,   676,   770,   849,   850,    70,   115,    71,
     799,    72,    73,   137,   195,   287,   449,   568,   680,   851,
     852,   111,   196,    74,    75,   138,   199,   289,   450,   570,
     683,   777,   912,   958,   994,    76,    77,   139,   202,   203,
     204,   291,   293,   292,   358,   359,   360,   574,   361,   571,
     779,   362,   572,   363,   466,   364,   576,   688,   784,   365,
     366,   575,   855,   782,   367,   368,   369,   205,    78,    79,
     140,   208,   209,   210,   295,   297,   381,   598,   600,   599,
     706,   800,   296,   376,   594,   580,   582,   585,   583,   584,
     587,   592,   593,   700,   589,   591,   698,   787,   377,   378,
     588,   487,   595,   858,   795,   796,   485,   702,   791,   865,
     703,   789,  1026,  1045,  1063,  1000,   965,   998,   919,   864,
     211,    80,   141,   214,   299,   385,   386,   613,   612,   805,
     601,   603,   606,   604,   605,   611,   387,   608,   388,   389,
     610,   872,   924,   662,    81,   215,    82,    83,   142,   218,
     219,   301,   302,   392,   393,   508,   722,   720,   808,   813,
     879,   721,   812,   220,    84,    85,    86,    87,   143,   223,
     224,   511,   394,   395,   512,   225,   398,   622,   399,   517,
     228,   226,    88,    89,   145,   231,   232,   312,   313,   817,
     404,   627,   628,   629,   630,   631,   735,   731,   819,   881,
     927,   732,   821,   882,   929,   233,    90,    91,   146,   236,
     237,   405,   316,   412,   636,   524,   525,   531,   238,    92,
      93,   147,   241,   318,   639,   534,   744,   829,   886,   970,
     971,  1005,  1031,  1068,  1067,    94,    95,   148,   244,   414,
     830,   415,   416,   644,   747,   934,   748,   245,    96,    97,
     149,   248,   249,   325,   539,   647,   752,   326,   422,   648,
     649,   650,   651,   652,   653,   935,  1007,   936,   977,  1032,
     835,   900,   894,   895,   250,   785,   860,   914,  1022,   861,
     917,   960,    98,    99,   183,   253,   150,   254,   328,   654,
     759,   841,   947,   982,  1011
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     162,   433,   434,   357,   504,   309,   719,   375,   862,   489,
     384,   370,   160,   379,   263,   265,   464,   806,  1049,  1062,
    1024,   188,  1041,  1043,   403,  1054,  1056,   411,  1058,  1060,
     987,   780,   468,   469,   772,   470,   471,   421,   456,   457,
     458,   459,   614,   801,   802,   472,   473,   474,   475,   988,
     476,   477,   974,   184,   773,     1,   193,   197,   478,   200,
     868,   518,   519,   520,   521,    -9,    10,   206,     4,   212,
     270,   615,    -9,     8,    -9,    -9,    -9,    -9,    -9,    -9,
      -9,    -9,    -9,    -9,    -9,    -9,     5,   320,    -9,    -9,
     271,   321,   322,   788,    -9,   216,    -9,   355,   221,   356,
     925,   463,   272,   941,   507,   189,   551,    -9,    -9,   792,
     793,   941,   794,   304,   305,   306,   307,   532,   533,    -9,
     479,   480,   481,   227,   229,   234,   239,   185,   308,   490,
     273,     6,   867,    -9,   242,   246,   274,   579,   251,    -9,
     194,   198,  1050,   201,    -9,   989,   990,   463,    12,   380,
     642,   207,   522,   207,   643,   100,   275,    -9,   323,   856,
      -9,    -9,   857,   102,   482,   463,   616,   463,   463,   889,
     463,    -9,   901,   104,    -9,   483,   484,   112,   618,   217,
     619,   113,   222,   975,   976,   276,   637,   638,   491,   161,
     371,   264,   266,   110,   110,   117,   999,  1025,   873,  1042,
    1044,  1055,  1057,   125,  1059,  1061,   110,   222,   230,   235,
     240,    -9,   406,   407,   408,  1020,   527,   528,   243,   247,
     529,   530,   252,   110,    -9,    -9,    -9,   373,   118,   374,
     277,    -9,  1035,   278,   279,    -9,    13,   119,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
     470,   471,    26,    27,   465,   645,   646,   467,    28,   120,
      29,   552,   553,   554,   750,   751,   477,   555,   809,   810,
     811,    30,    31,   915,   382,   121,   383,   492,   493,   679,
     470,   471,   401,    32,   402,   463,   419,   409,   420,   410,
     494,   495,   496,   497,   122,   498,   477,    33,   963,   964,
     499,   500,   501,    34,   623,   624,   625,   626,    35,   961,
    1090,   452,   453,   454,   455,   456,   457,   458,   459,   460,
     461,    36,   527,   528,    37,    38,   540,   541,   542,   543,
     544,   545,   505,   123,   506,    39,   931,   932,    40,   569,
     424,   425,   426,   427,   428,   429,   430,   431,   304,   305,
     306,   307,   124,   126,   577,   578,   557,   558,   559,   560,
     561,   562,   563,   564,   127,   740,   741,   742,   743,   890,
     891,   892,   893,   128,   129,    41,   596,   896,   897,   898,
     899,   463,   515,   620,   516,   619,   130,   131,    42,    43,
      44,   677,   133,   678,   136,    45,   151,   152,   775,    46,
     776,   839,   724,   840,   884,   952,   885,   953,   153,   502,
     463,  1069,   154,  1070,   155,   156,   157,   158,   159,   503,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   257,   174,   175,   176,   177,   178,   179,   180,   182,
     255,   256,   258,   259,   462,   463,   260,   261,   262,   267,
     268,   269,   281,   282,   283,   285,   298,   288,   681,   682,
     286,   300,   290,   294,   303,   310,   311,   689,   315,   314,
     317,   319,   324,   327,   329,   330,   331,   332,   337,   333,
     334,   335,   916,   336,   338,   705,   344,   349,   350,   351,
     352,   353,   354,   372,   390,   391,   396,   397,   400,   413,
     417,   418,   423,   447,   448,   435,   436,   437,   438,   439,
     440,   441,   442,   443,   444,   445,   488,   451,   446,   509,
     486,   510,   514,   523,   566,   535,   526,   536,   537,   546,
     565,   538,   665,   567,   573,   581,   921,   586,   590,   547,
     597,   548,   621,   602,   607,   609,   617,   549,   632,   633,
     634,   640,   660,   550,   635,   641,   655,   666,   656,   657,
     658,   661,   664,   667,   684,   659,   685,   668,   669,   675,
     686,   774,   670,   671,   672,   673,   781,   687,   690,   691,
     692,   693,   694,   695,   696,   697,   699,   701,   704,   707,
     708,   709,   710,   711,   712,   713,   714,   715,   716,   718,
     723,   746,   749,   726,   727,   728,   729,   730,   733,   807,
     734,   737,   738,   923,   736,   739,   753,   754,   755,   955,
     756,   725,   757,   758,   769,   778,   763,   760,   761,   764,
     765,   762,   766,   771,   783,   786,   797,   745,   798,   803,
     804,   814,   815,   816,   818,   820,   823,   853,   845,   824,
     825,   826,   827,   828,   831,   832,   833,   834,   836,   837,
     859,  1019,   842,   790,   843,   844,   846,   848,   854,   888,
     863,   875,   876,   877,   878,   869,   883,   887,   870,   909,
     880,   907,   910,   871,   904,   866,   911,   913,   905,   933,
     937,   920,   938,   939,   940,   906,   874,   942,   943,   903,
     944,   945,   948,   949,   950,   959,   951,   967,   966,   968,
     973,   768,   956,   978,   979,   918,   980,   981,   983,   984,
     985,   986,   993,   995,   962,   922,   996,   997,   717,  1002,
    1001,  1003,  1004,  1064,  1010,  1008,   926,   928,  1009,   930,
     969,   972,   999,  1015,  1018,  1012,  1013,  1014,  1023,  1029,
    1021,  1037,  1033,  1027,  1028,  1034,  1038,  1051,  1065,  1066,
    1039,  1077,  1078,  1079,  1080,  1088,   663,   144,   513,   822,
    1006,   946,     0,   902,     0,     0,   838,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1030,
       0,     0,     0,  1040,     0,     0,  1046,     0,  1047,   957,
       0,     0,     0,     0,     0,     0,     0,  1048,     0,  1073,
    1074,  1075,     0,  1081,     0,     0,     0,     0,   213,     0,
    1076,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1082,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   991,   992,     0,     0,     0,
       0,     0,     0,  1083,     0,     0,  1084,     0,  1085,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1053,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1072
};

static const yytype_int16 yycheck[] =
{
     111,   331,   332,   292,   383,   222,   613,   296,   786,     4,
     299,     4,     6,    45,     6,     6,     4,     4,    31,     4,
       6,    95,     6,     6,   313,     6,     6,   316,     6,     6,
      28,     6,    33,    34,    30,    36,    37,   326,    36,    37,
      38,    39,    41,   708,   709,    46,    47,    48,    49,    47,
      51,    52,    19,    21,    50,   158,    95,    95,    59,    95,
     125,    80,    81,    82,    83,     0,     1,    95,     6,    95,
       9,    70,     7,   178,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,     0,   103,    23,    24,
      29,   107,   108,   700,    29,    95,    31,   178,    95,   180,
     878,   166,    41,   894,   393,   179,   436,    42,    43,    36,
      37,   902,    39,    75,    76,    77,    78,   147,   148,    54,
     121,   122,   123,    95,    95,    95,    95,    95,    90,   124,
      69,     8,   797,    68,    95,    95,    75,   467,    95,    74,
     179,   179,   155,   179,    79,   143,   144,   166,     4,   181,
     103,   179,   171,   179,   107,   178,    95,    92,   174,     3,
      95,    96,     6,     6,   165,   166,   165,   166,   166,   833,
     166,   106,   836,    93,   109,   176,   177,     6,   182,   179,
     184,     6,   179,   150,   151,   124,    99,   100,   183,   183,
     183,   183,   183,   181,   181,     6,   181,   183,   805,   183,
     183,   182,   182,     9,   182,   182,   181,   179,   179,   179,
     179,   146,   103,   104,   105,   993,    97,    98,   179,   179,
     101,   102,   179,   181,   159,   160,   161,   178,     6,   180,
     169,   166,  1010,   172,   173,   170,     7,     6,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      36,    37,    23,    24,   365,   117,   118,   368,    29,     6,
      31,   162,   163,   164,   119,   120,    52,   168,    71,    72,
      73,    42,    43,    59,   178,     6,   180,    33,    34,   568,
      36,    37,   178,    54,   180,   166,   178,   178,   180,   180,
      46,    47,    48,    49,     6,    51,    52,    68,   156,   157,
      56,    57,    58,    74,    86,    87,    88,    89,    79,   916,
    1088,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    92,    97,    98,    95,    96,   111,   112,   113,   114,
     115,   116,   178,     6,   180,   106,   153,   154,   109,   450,
      60,    61,    62,    63,    64,    65,    66,    67,    75,    76,
      77,    78,     6,     6,   465,   466,   438,   439,   440,   441,
     442,   443,   444,   445,     6,   149,   150,   151,   152,    86,
      87,    88,    89,     6,     6,   146,   487,    86,    87,    88,
      89,   166,   178,   182,   180,   184,     3,     3,   159,   160,
     161,   178,     6,   180,     6,   166,     4,   178,   178,   170,
     180,   178,   619,   180,   178,   178,   180,   180,     4,   165,
     166,   178,    94,   180,     4,     4,     4,     4,     4,   175,
     178,     4,     4,     4,   178,   178,   178,   178,   178,   178,
     178,     6,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   165,   166,     6,     6,     6,   178,
       6,     4,     6,    20,    19,    25,    54,    42,   569,   570,
      10,    68,    31,    43,    92,    74,    79,   578,   181,    96,
     146,   106,   109,   170,   178,     6,     6,     6,    25,   182,
     182,   182,   861,   182,     6,   596,   161,    22,     4,     6,
       4,     4,     4,     4,     4,     4,   181,   181,     4,     4,
     178,     4,     4,   180,    27,     6,     6,     4,     4,     4,
       4,     4,     4,     4,     4,     4,   181,     4,     6,     4,
      28,     4,     4,     4,    75,     4,     6,     4,     4,     4,
     178,   110,   178,     6,     6,     6,   866,     6,     6,    25,
       4,    25,   180,     6,     6,     6,     4,    25,     4,     4,
       4,     4,     3,    27,     6,     4,     6,   178,     6,     6,
       6,   145,     4,   178,     4,    25,     4,   178,   178,    28,
       4,   682,   178,   178,   178,   178,   687,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,   178,   178,     6,     6,     6,     6,     4,     4,   720,
     181,     4,     6,   145,   182,     6,     4,     4,     4,   908,
       4,    91,     4,     4,     4,     4,   178,    26,    26,     6,
       6,    26,     6,   178,     4,     4,     4,   104,   180,     6,
       6,     4,     6,     4,     4,     4,     4,   165,    26,     6,
       6,     6,     6,     6,     4,     4,     4,     4,     4,     4,
     181,   991,     6,    53,     6,     6,     6,     4,     6,   104,
       6,     6,     6,     6,     4,   182,     4,     4,   182,   850,
     178,     6,     4,   182,    27,   796,     4,     4,    27,     4,
       6,   865,     6,     6,     6,    27,   807,     6,     6,   165,
       6,     6,     6,     6,     6,     4,    27,     4,     6,     4,
       4,   663,   908,     6,     6,   180,     6,     6,     6,     6,
       6,     6,     4,     4,    55,   182,     4,     4,   611,     4,
       6,     4,     4,  1045,     4,     6,   181,   181,     6,   182,
     181,   178,   181,     6,     4,   178,   178,   178,     4,     4,
     180,     4,     6,   182,   182,     6,     4,     4,     4,     4,
     125,     4,     4,     4,     4,     4,   554,    87,   397,   733,
     971,   901,    -1,   837,    -1,    -1,   758,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   181,
      -1,    -1,    -1,   182,    -1,    -1,   181,    -1,   181,   910,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   182,    -1,   182,
     182,   182,    -1,   165,    -1,    -1,    -1,    -1,   141,    -1,
     182,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   182,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   956,   957,    -1,    -1,    -1,
      -1,    -1,    -1,   182,    -1,    -1,   182,    -1,   182,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1037,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1053
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   158,   186,   187,     6,     0,     8,   188,   178,   189,
       1,   190,     4,     7,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    23,    24,    29,    31,
      42,    43,    54,    68,    74,    79,    92,    95,    96,   106,
     109,   146,   159,   160,   161,   166,   170,   191,   192,   193,
     195,   196,   198,   200,   202,   203,   218,   219,   220,   221,
     223,   225,   228,   229,   230,   234,   235,   246,   254,   256,
     262,   264,   266,   267,   278,   279,   290,   291,   323,   324,
     376,   399,   401,   402,   419,   420,   421,   422,   437,   438,
     461,   462,   474,   475,   490,   491,   503,   504,   537,   538,
     178,   194,     6,   197,    93,   199,   201,   222,   224,   226,
     181,   276,     6,     6,   257,   263,   247,     6,     6,     6,
       6,     6,     6,     6,     6,     9,     6,     6,     6,     6,
       3,     3,   204,     6,   231,   236,     6,   268,   280,   292,
     325,   377,   403,   423,   423,   439,   463,   476,   492,   505,
     541,     4,   178,     4,    94,     4,     4,     4,     4,     4,
       6,   183,   276,   178,     4,     4,     4,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   205,   178,   539,    21,    95,   232,   233,    95,   179,
     237,   245,   255,    95,   179,   269,   277,    95,   179,   281,
      95,   179,   293,   294,   295,   322,    95,   179,   326,   327,
     328,   375,    95,   327,   378,   400,    95,   179,   404,   405,
     418,    95,   179,   424,   425,   430,   436,    95,   435,    95,
     179,   440,   441,   460,    95,   179,   464,   465,   473,    95,
     179,   477,    95,   179,   493,   502,    95,   179,   506,   507,
     529,    95,   179,   540,   542,   178,   178,     6,   178,   178,
       6,     6,     6,     6,   183,     6,   183,   178,     6,     4,
       9,    29,    41,    69,    75,    95,   124,   169,   172,   173,
     206,     6,    20,    19,   238,    25,    10,   270,    42,   282,
      31,   296,   298,   297,    43,   329,   337,   330,    54,   379,
      68,   406,   407,    92,    75,    76,    77,    78,    90,   426,
      74,    79,   442,   443,    96,   181,   467,   146,   478,   106,
     103,   107,   108,   174,   109,   508,   512,   170,   543,   178,
       6,     6,     6,   182,   182,   182,   182,    25,     6,   207,
     213,   210,   211,   208,   161,   214,   212,   209,   215,    22,
       4,     6,     4,     4,     4,   178,   180,   265,   299,   300,
     301,   303,   306,   308,   310,   314,   315,   319,   320,   321,
       4,   183,     4,   178,   180,   265,   338,   353,   354,    45,
     181,   331,   178,   180,   265,   380,   381,   391,   393,   394,
       4,     4,   408,   409,   427,   428,   181,   181,   431,   433,
       4,   178,   180,   265,   445,   466,   103,   104,   105,   178,
     180,   265,   468,     4,   494,   496,   497,   178,     4,   178,
     180,   265,   513,     4,    60,    61,    62,    63,    64,    65,
      66,    67,   227,   227,   227,     6,     6,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     6,   180,    27,   271,
     283,     4,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,   165,   166,     4,   276,   309,   276,    33,    34,
      36,    37,    46,    47,    48,    49,    51,    52,    59,   121,
     122,   123,   165,   176,   177,   361,    28,   356,   181,     4,
     124,   183,    33,    34,    46,    47,    48,    49,    51,    56,
      57,    58,   165,   175,   361,   178,   180,   265,   410,     4,
       4,   426,   429,   429,     4,   178,   180,   434,    80,    81,
      82,    83,   171,     4,   470,   471,     6,    97,    98,   101,
     102,   472,   147,   148,   480,     4,     4,     4,   110,   509,
     111,   112,   113,   114,   115,   116,     4,    25,    25,    25,
      27,   227,   162,   163,   164,   168,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   178,    75,     6,   272,   276,
     284,   304,   307,     6,   302,   316,   311,   276,   276,   227,
     340,     6,   341,   343,   344,   342,     6,   345,   355,   349,
       6,   350,   346,   347,   339,   357,   276,     4,   332,   334,
     333,   385,     6,   386,   388,   389,   387,     6,   392,     6,
     395,   390,   383,   382,    41,    70,   165,     4,   182,   184,
     182,   180,   432,    86,    87,    88,    89,   446,   447,   448,
     449,   450,     4,     4,     4,     6,   469,    99,   100,   479,
       4,     4,   103,   107,   498,   117,   118,   510,   514,   515,
     516,   517,   518,   519,   544,     6,     6,     6,     6,    25,
       3,   145,   398,   398,     4,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   239,    28,   258,   178,   180,   265,
     273,   276,   276,   285,     4,     4,     4,     4,   312,   276,
       4,     4,     4,     4,     4,     4,     4,     4,   351,     4,
     348,     4,   362,   365,     4,   276,   335,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,   351,     4,   362,
     412,   416,   411,     4,   426,    91,     6,     6,     6,     6,
       4,   452,   456,     4,   181,   451,   182,     4,     6,     6,
     149,   150,   151,   152,   481,   104,   178,   499,   501,   178,
     119,   120,   511,     4,     4,     4,     4,     4,     4,   545,
      26,    26,    26,   178,     6,     6,     6,   217,   217,     4,
     259,   178,    30,    50,   276,   178,   180,   286,     4,   305,
       6,   276,   318,     4,   313,   530,     4,   352,   362,   366,
      53,   363,    36,    37,    39,   359,   360,     4,   180,   265,
     336,   336,   336,     6,     6,   384,     4,   276,   413,    71,
      72,    73,   417,   414,     4,     6,     4,   444,     4,   453,
       4,   457,   444,     4,     6,     6,     6,     6,     6,   482,
     495,     4,     4,     4,     4,   525,     4,     4,   525,   178,
     180,   546,     6,     6,     6,    26,     6,   240,     4,   260,
     261,   274,   275,   165,     6,   317,     3,     6,   358,   181,
     531,   534,   358,     6,   374,   364,   276,   336,   125,   182,
     182,   182,   396,   362,   276,     6,     6,     6,     4,   415,
     178,   454,   458,     4,   178,   180,   483,     4,   104,   472,
      86,    87,    88,    89,   527,   528,    86,    87,    88,    89,
     526,   472,   527,   165,    27,    27,    27,     6,   241,   260,
       4,     4,   287,     4,   532,    59,   361,   535,   180,   373,
     365,   227,   182,   145,   397,   358,   181,   455,   181,   459,
     182,   153,   154,     4,   500,   520,   522,     6,     6,     6,
       6,   528,     6,     6,     6,     6,   520,   547,     6,     6,
       6,    27,   178,   180,   242,   265,   320,   276,   288,     4,
     536,   362,    55,   156,   157,   371,     6,     4,     4,   181,
     484,   485,   178,     4,    19,   150,   151,   523,     6,     6,
       6,     6,   548,     6,     6,     6,     6,    28,    47,   143,
     144,   276,   276,     4,   289,     4,     4,     4,   372,   181,
     370,     6,     4,     4,     4,   486,   486,   521,     6,     6,
       4,   549,   178,   178,   178,     6,   244,   243,     4,   227,
     358,   180,   533,     4,     6,   183,   367,   182,   182,     4,
     181,   487,   524,     6,     6,   358,   248,     4,     4,   125,
     182,     6,   183,     6,   183,   368,   181,   181,   182,    31,
     155,     4,   249,   276,     6,   182,     6,   182,     6,   182,
       6,   182,     4,   369,   370,     4,     4,   489,   488,   178,
     180,   250,   276,   182,   182,   182,   182,     4,     4,     4,
       4,   165,   182,   182,   182,   182,   251,   252,     4,   253,
     358
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   185,   186,   186,   187,   187,   188,   189,   188,   190,
     190,   190,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   194,   193,   195,   197,
     196,   199,   198,   201,   200,   202,   204,   203,   205,   205,
     207,   206,   208,   206,   209,   206,   210,   206,   211,   206,
     212,   206,   213,   206,   214,   206,   215,   206,   216,   216,
     216,   216,   216,   217,   217,   218,   219,   220,   222,   221,
     224,   223,   226,   225,   227,   227,   227,   227,   227,   227,
     227,   227,   228,   229,   230,   231,   231,   232,   233,   234,
     235,   236,   236,   238,   239,   240,   237,   241,   241,   242,
     242,   242,   243,   242,   244,   242,   242,   245,   247,   248,
     246,   249,   249,   251,   250,   252,   252,   253,   255,   254,
     257,   256,   258,   259,   258,   260,   260,   261,   263,   262,
     264,   265,   266,   267,   268,   268,   270,   271,   269,   272,
     272,   274,   273,   275,   273,   273,   276,   276,   276,   276,
     277,   278,   279,   280,   280,   282,   283,   281,   284,   284,
     285,   285,   287,   286,   288,   288,   289,   290,   291,   292,
     292,   293,   294,   296,   295,   297,   297,   297,   298,   298,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   300,
     302,   301,   304,   303,   305,   305,   307,   306,   308,   308,
     309,   309,   311,   310,   312,   312,   313,   314,   316,   317,
     315,   318,   318,   319,   320,   320,   320,   320,   321,   322,
     323,   324,   325,   325,   326,   327,   329,   328,   330,   330,
     332,   331,   333,   331,   334,   331,   335,   331,   336,   336,
     336,   337,   337,   339,   338,   340,   338,   341,   338,   342,
     338,   338,   338,   338,   343,   338,   344,   338,   345,   338,
     338,   346,   338,   347,   348,   338,   349,   338,   350,   338,
     338,   351,   351,   352,   353,   355,   354,   356,   357,   356,
     358,   358,   359,   359,   360,   360,   360,   361,   361,   361,
     362,   362,   364,   363,   366,   367,   365,   368,   368,   369,
     369,   370,   370,   370,   370,   370,   370,   370,   370,   371,
     371,   372,   371,   373,   373,   374,   374,   375,   376,   377,
     377,   378,   379,   379,   380,   380,   380,   380,   382,   381,
     383,   384,   381,   385,   381,   386,   381,   387,   381,   381,
     381,   388,   381,   389,   381,   390,   381,   381,   392,   391,
     393,   395,   396,   394,   397,   397,   398,   398,   399,   400,
     401,   402,   403,   403,   404,   406,   405,   407,   407,   408,
     409,   409,   410,   411,   410,   412,   410,   410,   413,   413,
     414,   414,   415,   416,   416,   417,   417,   417,   418,   419,
     420,   421,   422,   423,   423,   424,   424,   425,   427,   426,
     428,   426,   426,   426,   429,   429,   431,   430,   432,   432,
     433,   433,   434,   434,   434,   434,   435,   436,   437,   438,
     439,   439,   440,   442,   441,   443,   443,   444,   444,   446,
     445,   447,   445,   448,   445,   449,   445,   450,   445,   445,
     451,   451,   451,   452,   452,   454,   453,   455,   455,   455,
     456,   456,   458,   457,   459,   459,   459,   460,   461,   462,
     463,   463,   464,   466,   465,   467,   467,   468,   468,   468,
     469,   468,   470,   468,   471,   468,   468,   468,   472,   472,
     473,   474,   475,   476,   476,   478,   479,   477,   480,   480,
     481,   481,   481,   481,   482,   482,   484,   483,   485,   483,
     486,   486,   488,   487,   489,   487,   490,   491,   492,   492,
     494,   495,   493,   496,   493,   497,   493,   493,   499,   500,
     498,   501,   498,   502,   503,   504,   505,   505,   506,   508,
     507,   509,   509,   510,   510,   510,   511,   511,   511,   512,
     512,   514,   513,   515,   513,   516,   513,   517,   513,   518,
     513,   519,   513,   513,   521,   520,   522,   522,   523,   523,
     524,   524,   525,   525,   526,   526,   526,   526,   527,   527,
     528,   528,   528,   528,   529,   530,   530,   532,   531,   533,
     533,   534,   534,   535,   536,   535,   537,   538,   539,   539,
     540,   541,   541,   543,   544,   542,   545,   545,   547,   546,
     548,   548,   549
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     4,     0,     3,     0,     0,     4,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     4,     2,     0,
       4,     0,     4,     0,     4,     1,     0,     5,     0,     2,
       0,     5,     0,     5,     0,     5,     0,     5,     0,     5,
       0,     5,     0,     5,     0,     5,     0,     5,     3,     3,
       1,     2,     2,     0,     1,     5,     3,     3,     0,    14,
       0,    14,     0,    14,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     3,     2,     0,     2,     5,     2,     3,
       3,     0,     2,     0,     0,     0,    10,     0,     2,     2,
       1,     3,     0,     4,     0,     6,     3,     2,     0,     0,
      17,     0,     2,     0,     4,     0,     2,     2,     0,     9,
       0,     3,     0,     0,     3,     0,     2,     1,     0,     9,
       1,     2,     3,     3,     0,     2,     0,     0,     6,     0,
       2,     0,     6,     0,     4,     1,     4,     4,     4,     4,
       2,     4,     3,     0,     2,     0,     0,     7,     2,     3,
       0,     2,     0,     4,     0,     2,     2,     3,     3,     0,
       2,     3,     2,     0,     4,     0,     2,     2,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     4,     0,     5,     0,     1,     0,     4,     2,     2,
       2,     3,     0,     4,     0,     2,     2,     2,     0,     0,
       7,     1,     2,     3,     2,     2,     2,     2,     3,     2,
       3,     3,     0,     2,     3,     2,     0,     3,     0,     2,
       0,     6,     0,     6,     0,     6,     0,     7,     0,     1,
       2,     0,     2,     0,     4,     0,     4,     0,     4,     0,
       4,     3,     3,     3,     0,     4,     0,     4,     0,     4,
       1,     0,     4,     0,     0,     5,     0,     6,     0,     4,
       1,     0,     2,     2,     5,     0,     4,     0,     0,     3,
       1,     1,     0,     3,     1,     1,     1,     1,     1,     1,
       1,     2,     0,     3,     0,     0,     8,     0,     2,     1,
       1,     4,     4,     4,     4,     5,     5,     5,     5,     0,
       1,     0,     3,     0,     3,     0,     1,     2,     3,     0,
       2,     3,     0,     2,     1,     1,     1,     1,     0,     4,
       0,     0,     6,     0,     4,     0,     4,     0,     4,     3,
       3,     0,     4,     0,     4,     0,     4,     1,     0,     5,
       3,     0,     0,     7,     0,     3,     0,     3,     3,     2,
       3,     3,     0,     2,     4,     0,     3,     0,     2,     1,
       0,     2,     3,     0,     4,     0,     4,     1,     2,     1,
       0,     2,     2,     0,     2,     2,     2,     2,     2,     3,
       3,     3,     3,     0,     2,     1,     1,     4,     0,     3,
       0,     6,     4,     4,     1,     3,     0,     8,     0,     1,
       0,     2,     3,     3,     3,     3,     2,     2,     3,     3,
       0,     2,     3,     0,     3,     0,     2,     0,     1,     0,
       5,     0,     4,     0,     4,     0,     5,     0,     4,     1,
       0,     4,     8,     0,     2,     0,     3,     0,     4,     8,
       0,     2,     0,     3,     0,     4,     8,     2,     3,     3,
       0,     2,     3,     0,     6,     0,     2,     5,     5,     3,
       0,     4,     0,     3,     0,     3,     2,     1,     1,     1,
       2,     4,     3,     0,     2,     0,     0,     8,     1,     1,
       1,     2,     2,     2,     0,     2,     0,     4,     0,     4,
       0,     2,     0,     5,     0,     5,     3,     3,     0,     2,
       0,     0,    10,     0,     6,     0,     6,     3,     0,     0,
       6,     0,     3,     2,     3,     3,     0,     2,     3,     0,
       4,     0,     3,     0,     1,     1,     0,     1,     1,     0,
       2,     0,     7,     0,     6,     0,     5,     0,     7,     0,
       6,     0,     5,     1,     0,     4,     0,     2,     3,     3,
       0,     2,     0,     2,     2,     2,     2,     2,     1,     2,
       3,     3,     3,     3,     2,     0,     2,     0,     6,     0,
       2,     0,     2,     2,     0,     3,     3,     3,     0,     1,
       2,     0,     2,     0,     0,     7,     0,     2,     0,     4,
       0,     2,     2
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
        case 5:
#line 142 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrVersionCbk, defrVersionCbkType, (yyvsp[-1].dval)); }
#line 2350 "def.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 146 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; }
#line 2356 "def.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 147 "def.y" /* yacc.c:1646  */
    {
        if (strcmp((yyvsp[-1].string), "OFF") == 0)
          names_case_sensitive = 0;
        else if (strcmp((yyvsp[-1].string), "off") == 0)
          names_case_sensitive = 0;
        CALLBACK(defrCaseSensitiveCbk, defrCaseSensitiveCbkType,
                 names_case_sensitive); 
      }
#line 2369 "def.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 176 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1; }
#line 2375 "def.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 177 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrDesignCbk, defrDesignStartCbkType, (yyvsp[-1].string)); }
#line 2381 "def.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 181 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrDesignEndCbk, defrDesignEndCbkType, 0); }
#line 2387 "def.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 184 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 2393 "def.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 185 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrTechnologyCbk, defrTechNameCbkType, (yyvsp[-1].string)); }
#line 2399 "def.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 188 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1;}
#line 2405 "def.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 189 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrArrayNameCbk, defrArrayNameCbkType, (yyvsp[-1].string)); }
#line 2411 "def.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 192 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 2417 "def.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 193 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrFloorPlanNameCbk, defrFloorPlanNameCbkType, (yyvsp[-1].string)); }
#line 2423 "def.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 197 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrHistoryCbk, defrHistoryCbkType, History_text); }
#line 2429 "def.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 201 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPropDefStartCbk, defrPropDefStartCbkType, 0); }
#line 2435 "def.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 203 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPropDefEndCbk, defrPropDefEndCbkType, 0); }
#line 2441 "def.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 208 "def.y" /* yacc.c:1646  */
    { }
#line 2447 "def.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 211 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1; defrProp.defiProp::clear(); }
#line 2453 "def.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 213 "def.y" /* yacc.c:1646  */
    {
              if (defrPropCbk) {
                defrProp.defiProp::setPropType("design", (yyvsp[-2].string));
		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
              }
            }
#line 2464 "def.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 219 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1 ; no_num = 1; defrProp.defiProp::clear(); }
#line 2470 "def.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 221 "def.y" /* yacc.c:1646  */
    {
              if (defrPropCbk) {
                defrProp.defiProp::setPropType("net", (yyvsp[-2].string));
		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
              }
            }
#line 2481 "def.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 227 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1 ; no_num = 1; defrProp.defiProp::clear(); }
#line 2487 "def.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 229 "def.y" /* yacc.c:1646  */
    {
              if (defrPropCbk) {
                defrProp.defiProp::setPropType("specialnet", (yyvsp[-2].string));
		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
              }
            }
#line 2498 "def.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 235 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1 ; no_num = 1; defrProp.defiProp::clear(); }
#line 2504 "def.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 237 "def.y" /* yacc.c:1646  */
    {
              if (defrPropCbk) {
                defrProp.defiProp::setPropType("region", (yyvsp[-2].string));
		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
              }
            }
#line 2515 "def.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 243 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1 ; no_num = 1; defrProp.defiProp::clear(); }
#line 2521 "def.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 245 "def.y" /* yacc.c:1646  */
    {
              if (defrPropCbk) {
                defrProp.defiProp::setPropType("group", (yyvsp[-2].string));
		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
              }
            }
#line 2532 "def.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 251 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1 ; no_num = 1; defrProp.defiProp::clear(); }
#line 2538 "def.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 253 "def.y" /* yacc.c:1646  */
    {
              if (defrPropCbk) {
                defrProp.defiProp::setPropType("component", (yyvsp[-2].string));
		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
              }
            }
#line 2549 "def.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 259 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1 ; no_num = 1; defrProp.defiProp::clear(); }
#line 2555 "def.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 261 "def.y" /* yacc.c:1646  */
    {
              if (defrPropCbk) {
                defrProp.defiProp::setPropType("row", (yyvsp[-2].string));
		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
              }
            }
#line 2566 "def.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 267 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1 ; no_num = 1; defrProp.defiProp::clear(); }
#line 2572 "def.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 269 "def.y" /* yacc.c:1646  */
    {
              if (defrPropCbk) {
                defrProp.defiProp::setPropType("pin", (yyvsp[-2].string));
		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
              }
            }
#line 2583 "def.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 276 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1 ; no_num = 1; defrProp.defiProp::clear(); }
#line 2589 "def.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 278 "def.y" /* yacc.c:1646  */
    {
              if (defrPropCbk) {
                defrProp.defiProp::setPropType("componentpin", (yyvsp[-2].string));
		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
              }
            }
#line 2600 "def.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 287 "def.y" /* yacc.c:1646  */
    { if (defrPropCbk) defrProp.defiProp::setPropInteger(); }
#line 2606 "def.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 289 "def.y" /* yacc.c:1646  */
    { if (defrPropCbk) defrProp.defiProp::setPropReal(); }
#line 2612 "def.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 291 "def.y" /* yacc.c:1646  */
    { if (defrPropCbk) defrProp.defiProp::setPropString(); }
#line 2618 "def.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 293 "def.y" /* yacc.c:1646  */
    { if (defrPropCbk) defrProp.defiProp::setPropQString((yyvsp[0].string)); }
#line 2624 "def.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 295 "def.y" /* yacc.c:1646  */
    { if (defrPropCbk) defrProp.defiProp::setPropNameMapString((yyvsp[0].string)); }
#line 2630 "def.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 300 "def.y" /* yacc.c:1646  */
    { if (defrPropCbk) defrProp.defiProp::setNumber((yyvsp[0].dval)); }
#line 2636 "def.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 304 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrUnitsCbk,  defrUnitsCbkType, (yyvsp[-1].dval)); }
#line 2642 "def.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 308 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrDividerCbk, defrDividerCbkType, (yyvsp[-1].string)); }
#line 2648 "def.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 312 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrBusBitCbk, defrBusBitCbkType, (yyvsp[-1].string)); }
#line 2654 "def.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 315 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; defrSite.defiSite::clear(); }
#line 2660 "def.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 318 "def.y" /* yacc.c:1646  */
    {
            if (defrSiteCbk) {
              defrSite.defiSite::setName((yyvsp[-11].string));
              defrSite.defiSite::setLocation((yyvsp[-10].dval),(yyvsp[-9].dval));
              defrSite.defiSite::setOrient((yyvsp[-8].integer));
              defrSite.defiSite::setDo((yyvsp[-6].dval),(yyvsp[-4].dval),(yyvsp[-2].dval),(yyvsp[-1].dval));
	      CALLBACK(defrSiteCbk, defrSiteCbkType, &(defrSite));
            }
	  }
#line 2674 "def.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 329 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1;no_num = 1; }
#line 2680 "def.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 331 "def.y" /* yacc.c:1646  */
    {
              if (defrCanplaceCbk) {
                defrCanplace.defiSite::setName((yyvsp[-11].string));
                defrCanplace.defiSite::setLocation((yyvsp[-10].dval),(yyvsp[-9].dval));
                defrCanplace.defiSite::setOrient((yyvsp[-8].integer));
                defrCanplace.defiSite::setDo((yyvsp[-6].dval),(yyvsp[-4].dval),(yyvsp[-2].dval),(yyvsp[-1].dval));
		CALLBACK(defrCanplaceCbk, defrCanplaceCbkType,
		&(defrCanplace));
              }
            }
#line 2695 "def.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 342 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1;no_num = 1; }
#line 2701 "def.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 344 "def.y" /* yacc.c:1646  */
    {
              if (defrCannotOccupyCbk) {
                defrCannotOccupy.defiSite::setName((yyvsp[-11].string));
                defrCannotOccupy.defiSite::setLocation((yyvsp[-10].dval),(yyvsp[-9].dval));
                defrCannotOccupy.defiSite::setOrient((yyvsp[-8].integer));
                defrCannotOccupy.defiSite::setDo((yyvsp[-6].dval),(yyvsp[-4].dval),(yyvsp[-2].dval),(yyvsp[-1].dval));
		CALLBACK(defrCannotOccupyCbk, defrCannotOccupyCbkType,
                        &(defrCannotOccupy));
              }
            }
#line 2716 "def.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 356 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 0;}
#line 2722 "def.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 357 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 3;}
#line 2728 "def.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 358 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 2;}
#line 2734 "def.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 359 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 1;}
#line 2740 "def.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 360 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 4;}
#line 2746 "def.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 361 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 5;}
#line 2752 "def.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 362 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 6;}
#line 2758 "def.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 363 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 7;}
#line 2764 "def.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 367 "def.y" /* yacc.c:1646  */
    {
	    if (defrDieAreaCbk) {
	      defrDieArea.defiBox::setLowerLeft((yyvsp[-2].pt).x,(yyvsp[-2].pt).y);
	      defrDieArea.defiBox::setUpperRight((yyvsp[-1].pt).x,(yyvsp[-1].pt).y);
	      CALLBACK(defrDieAreaCbk, defrDieAreaCbkType, &(defrDieArea));
	    }
	  }
#line 2776 "def.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 377 "def.y" /* yacc.c:1646  */
    { }
#line 2782 "def.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 381 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrDefaultCapCbk, defrDefaultCapCbkType, ROUND((yyvsp[0].dval))); }
#line 2788 "def.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 389 "def.y" /* yacc.c:1646  */
    {
	    if (defrPinCapCbk) {
	      defrPinCap.defiPinCap::setPin(ROUND((yyvsp[-3].dval)));
	      defrPinCap.defiPinCap::setCap((yyvsp[-1].dval));
	      CALLBACK(defrPinCapCbk, defrPinCapCbkType, &(defrPinCap));
	    }
	  }
#line 2800 "def.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 399 "def.y" /* yacc.c:1646  */
    { }
#line 2806 "def.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 403 "def.y" /* yacc.c:1646  */
    { }
#line 2812 "def.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 407 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrStartPinsCbk, defrStartPinsCbkType, ROUND((yyvsp[-1].dval))); }
#line 2818 "def.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 414 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1;no_num = 1; }
#line 2824 "def.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 415 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1; }
#line 2830 "def.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 416 "def.y" /* yacc.c:1646  */
    {
            if (defrPinCbk || defrPinExtCbk) {
              defrPin.defiPin::Setup((yyvsp[-4].string), (yyvsp[0].string));
            }
          }
#line 2840 "def.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 422 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPinCbk, defrPinCbkType, &defrPin); }
#line 2846 "def.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 430 "def.y" /* yacc.c:1646  */
    {
            if (defrPinCbk)
              defrPin.defiPin::setSpecial();
          }
#line 2855 "def.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 436 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPinExtCbk, defrPinExtCbkType, History_text); }
#line 2861 "def.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 439 "def.y" /* yacc.c:1646  */
    {
              if (defrPinCbk || defrPinExtCbk)
                defrPin.defiPin::setDirection((yyvsp[0].string));
            }
#line 2870 "def.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 444 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1;  no_num = 1; }
#line 2876 "def.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 445 "def.y" /* yacc.c:1646  */
    {
              if (defrPinCbk || defrPinExtCbk) defrPin.defiPin::setUse((yyvsp[0].string));
            }
#line 2884 "def.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 449 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; }
#line 2890 "def.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 450 "def.y" /* yacc.c:1646  */
    {
              if (defrPinCbk || defrPinExtCbk)
                defrPin.defiPin::setLayer((yyvsp[-2].string),(yyvsp[-1].pt).x,(yyvsp[-1].pt).y,(yyvsp[0].pt).x,(yyvsp[0].pt).y);
            }
#line 2899 "def.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 456 "def.y" /* yacc.c:1646  */
    {
              if (defrPinCbk || defrPinExtCbk)
                defrPin.defiPin::setPlacement((yyvsp[-2].integer), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
            }
#line 2908 "def.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 464 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPinEndCbk, defrPinEndCbkType, 0); }
#line 2914 "def.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 467 "def.y" /* yacc.c:1646  */
    {dumb_mode = 2; no_num = 2; }
#line 2920 "def.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 469 "def.y" /* yacc.c:1646  */
    {
          if (defrRowCbk) {
            defrRow.defiRow::setup((yyvsp[-11].string), (yyvsp[-10].string), (yyvsp[-9].dval), (yyvsp[-8].dval), (yyvsp[-7].integer));
            defrRow.defiRow::setDo(ROUND((yyvsp[-5].dval)), ROUND((yyvsp[-3].dval)), (yyvsp[-1].dval), (yyvsp[0].dval));
          }
        }
#line 2931 "def.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 476 "def.y" /* yacc.c:1646  */
    {
          if (defrRowCbk) {
	    CALLBACK(defrRowCbk, defrRowCbkType, &defrRow);
          }
        }
#line 2941 "def.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 487 "def.y" /* yacc.c:1646  */
    { dumb_mode = 10000000; }
#line 2947 "def.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 488 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; }
#line 2953 "def.tab.c" /* yacc.c:1646  */
    break;

  case 137:
#line 496 "def.y" /* yacc.c:1646  */
    { if (defrRegionCbk) defrRow.defiRow::addProperty((yyvsp[-1].string), (yyvsp[0].string)); }
#line 2959 "def.tab.c" /* yacc.c:1646  */
    break;

  case 138:
#line 500 "def.y" /* yacc.c:1646  */
    {
          if (defrTrackCbk) {
            defrTrack.defiTrack::setup((yyvsp[-1].string));
          }
        }
#line 2969 "def.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 506 "def.y" /* yacc.c:1646  */
    {
          if (defrTrackCbk) {
            defrTrack.defiTrack::setDo(ROUND((yyvsp[-7].dval)), ROUND((yyvsp[-4].dval)), (yyvsp[-2].dval));
	    CALLBACK(defrTrackCbk, defrTrackCbkType, &defrTrack);
          }
        }
#line 2980 "def.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 514 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1;  no_num = 1; }
#line 2986 "def.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 515 "def.y" /* yacc.c:1646  */
    {
          (yyval.string) = (yyvsp[0].string);
        }
#line 2994 "def.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 521 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1000; }
#line 3000 "def.tab.c" /* yacc.c:1646  */
    break;

  case 144:
#line 522 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; }
#line 3006 "def.tab.c" /* yacc.c:1646  */
    break;

  case 147:
#line 530 "def.y" /* yacc.c:1646  */
    {
          if (defrTrackCbk)
            defrTrack.defiTrack::addLayer((yyvsp[0].string));
        }
#line 3015 "def.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 536 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3021 "def.tab.c" /* yacc.c:1646  */
    break;

  case 149:
#line 538 "def.y" /* yacc.c:1646  */
    {
	  if (defrGcellGridCbk) {
	    defrGcellGrid.defiGcellGrid::setup((yyvsp[-6].string), ROUND((yyvsp[-5].dval)), ROUND((yyvsp[-3].dval)), (yyvsp[-1].dval));
	    CALLBACK(defrGcellGridCbk, defrGcellGridCbkType, &defrGcellGrid);
	  }
	}
#line 3032 "def.tab.c" /* yacc.c:1646  */
    break;

  case 150:
#line 547 "def.y" /* yacc.c:1646  */
    { }
#line 3038 "def.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 551 "def.y" /* yacc.c:1646  */
    { }
#line 3044 "def.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 558 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrViaStartCbk, defrViaStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 3050 "def.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 565 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1;no_num = 1; }
#line 3056 "def.tab.c" /* yacc.c:1646  */
    break;

  case 157:
#line 566 "def.y" /* yacc.c:1646  */
    { if (defrViaCbk) defrVia.defiVia::setup((yyvsp[0].string)); }
#line 3062 "def.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 568 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrViaCbk, defrViaCbkType, &defrVia); }
#line 3068 "def.tab.c" /* yacc.c:1646  */
    break;

  case 161:
#line 575 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1;no_num = 1; }
#line 3074 "def.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 576 "def.y" /* yacc.c:1646  */
    {
              if (defrViaCbk)
                defrVia.defiVia::addLayer((yyvsp[-2].string), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
            }
#line 3083 "def.tab.c" /* yacc.c:1646  */
    break;

  case 163:
#line 580 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1;no_num = 1; }
#line 3089 "def.tab.c" /* yacc.c:1646  */
    break;

  case 164:
#line 581 "def.y" /* yacc.c:1646  */
    {
              if (defrViaCbk)
                defrVia.defiVia::addPattern((yyvsp[0].string));
            }
#line 3098 "def.tab.c" /* yacc.c:1646  */
    break;

  case 165:
#line 586 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrViaExtCbk, defrViaExtCbkType, History_text); }
#line 3104 "def.tab.c" /* yacc.c:1646  */
    break;

  case 166:
#line 591 "def.y" /* yacc.c:1646  */
    {
            save_x = (yyvsp[-2].dval);
            save_y = (yyvsp[-1].dval);
            (yyval.pt).x = ROUND((yyvsp[-2].dval));
            (yyval.pt).y = ROUND((yyvsp[-1].dval));
            }
#line 3115 "def.tab.c" /* yacc.c:1646  */
    break;

  case 167:
#line 598 "def.y" /* yacc.c:1646  */
    {
            save_y = (yyvsp[-1].dval);
            (yyval.pt).x = ROUND(save_x);
            (yyval.pt).y = ROUND((yyvsp[-1].dval));
            }
#line 3125 "def.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 604 "def.y" /* yacc.c:1646  */
    {
            save_x = (yyvsp[-2].dval);
            (yyval.pt).x = ROUND((yyvsp[-2].dval));
            (yyval.pt).y = ROUND(save_y);
            }
#line 3135 "def.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 610 "def.y" /* yacc.c:1646  */
    {
            (yyval.pt).x = ROUND(save_x);
            (yyval.pt).y = ROUND(save_y);
            }
#line 3144 "def.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 617 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrViaEndCbk, defrViaEndCbkType, 0); }
#line 3150 "def.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 621 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrRegionEndCbk, defrRegionEndCbkType, 0); }
#line 3156 "def.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 625 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrRegionStartCbk, defrRegionStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 3162 "def.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 630 "def.y" /* yacc.c:1646  */
    {}
#line 3168 "def.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 633 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3174 "def.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 634 "def.y" /* yacc.c:1646  */
    { if (defrRegionCbk) defrRegion.defiRegion::setup((yyvsp[0].string)); }
#line 3180 "def.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 636 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrRegionCbk, defrRegionCbkType, &defrRegion); }
#line 3186 "def.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 641 "def.y" /* yacc.c:1646  */
    { if (defrRegionCbk)
	  defrRegion.defiRegion::addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 3193 "def.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 644 "def.y" /* yacc.c:1646  */
    { if (defrRegionCbk)
	  defrRegion.defiRegion::addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 3200 "def.tab.c" /* yacc.c:1646  */
    break;

  case 182:
#line 652 "def.y" /* yacc.c:1646  */
    { dumb_mode = 10000000; }
#line 3206 "def.tab.c" /* yacc.c:1646  */
    break;

  case 183:
#line 653 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; }
#line 3212 "def.tab.c" /* yacc.c:1646  */
    break;

  case 186:
#line 661 "def.y" /* yacc.c:1646  */
    { if (defrRegionCbk) defrRegion.defiRegion::addProperty((yyvsp[-1].string), (yyvsp[0].string)); }
#line 3218 "def.tab.c" /* yacc.c:1646  */
    break;

  case 188:
#line 668 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrComponentStartCbk, defrComponentStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 3224 "def.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 676 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrComponentCbk, defrComponentCbkType, &defrComponent); }
#line 3230 "def.tab.c" /* yacc.c:1646  */
    break;

  case 192:
#line 680 "def.y" /* yacc.c:1646  */
    {
              dumb_mode = 0;
	      no_num = 0;
              /* The net connections were added to the array in
               * reverse order so fix the order now */
              if (defrComponentCbk)
                defrComponent.defiComponent::reverseNetOrder();
            }
#line 3243 "def.tab.c" /* yacc.c:1646  */
    break;

  case 193:
#line 690 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1000000000; no_num = 10000000; }
#line 3249 "def.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 692 "def.y" /* yacc.c:1646  */
    {
            if (defrComponentCbk)
              defrComponent.defiComponent::IdAndName((yyvsp[-1].string), (yyvsp[0].string));
            }
#line 3258 "def.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 699 "def.y" /* yacc.c:1646  */
    { }
#line 3264 "def.tab.c" /* yacc.c:1646  */
    break;

  case 196:
#line 701 "def.y" /* yacc.c:1646  */
    {
              if (defrComponentCbk)
                defrComponent.defiComponent::addNet("*");
            }
#line 3273 "def.tab.c" /* yacc.c:1646  */
    break;

  case 197:
#line 706 "def.y" /* yacc.c:1646  */
    {
              if (defrComponentCbk)
                defrComponent.defiComponent::addNet((yyvsp[0].string));
            }
#line 3282 "def.tab.c" /* yacc.c:1646  */
    break;

  case 209:
#line 722 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrComponentExtCbk, defrComponentExtCbkType, History_text); }
#line 3288 "def.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 725 "def.y" /* yacc.c:1646  */
    {dumb_mode=1; no_num = 1; }
#line 3294 "def.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 726 "def.y" /* yacc.c:1646  */
    {
              if (defrComponentCbk)
                defrComponent.defiComponent::setEEQ((yyvsp[0].string));
            }
#line 3303 "def.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 732 "def.y" /* yacc.c:1646  */
    { dumb_mode = 2;  no_num = 2; }
#line 3309 "def.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 734 "def.y" /* yacc.c:1646  */
    {
              if (defrComponentCbk)
                defrComponent.defiComponent::setGenerate((yyvsp[-1].string), (yyvsp[0].string));
            }
#line 3318 "def.tab.c" /* yacc.c:1646  */
    break;

  case 214:
#line 742 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)""; }
#line 3324 "def.tab.c" /* yacc.c:1646  */
    break;

  case 215:
#line 744 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string); }
#line 3330 "def.tab.c" /* yacc.c:1646  */
    break;

  case 216:
#line 747 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3336 "def.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 748 "def.y" /* yacc.c:1646  */
    {
              if (defrComponentCbk)
                defrComponent.defiComponent::setSource((yyvsp[0].string));
            }
#line 3345 "def.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 756 "def.y" /* yacc.c:1646  */
    { }
#line 3351 "def.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 758 "def.y" /* yacc.c:1646  */
    {
	    if (defrComponentCbk)
	      defrComponent.defiComponent::setRegionName((yyvsp[0].string));
	  }
#line 3360 "def.tab.c" /* yacc.c:1646  */
    break;

  case 220:
#line 765 "def.y" /* yacc.c:1646  */
    { 
	  if (defrComponentCbk)
	    defrComponent.defiComponent::setRegionBounds((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
	}
#line 3369 "def.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 770 "def.y" /* yacc.c:1646  */
    { 
	  if (defrComponentCbk)
	    defrComponent.defiComponent::setRegionBounds((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
	}
#line 3378 "def.tab.c" /* yacc.c:1646  */
    break;

  case 222:
#line 776 "def.y" /* yacc.c:1646  */
    { dumb_mode = 10000000; }
#line 3384 "def.tab.c" /* yacc.c:1646  */
    break;

  case 223:
#line 778 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; }
#line 3390 "def.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 786 "def.y" /* yacc.c:1646  */
    {
              if (defrComponentCbk)
                defrComponent.defiComponent::addProperty((yyvsp[-1].string), (yyvsp[0].string));
            }
#line 3399 "def.tab.c" /* yacc.c:1646  */
    break;

  case 227:
#line 793 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3405 "def.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 796 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3411 "def.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 797 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3417 "def.tab.c" /* yacc.c:1646  */
    break;

  case 230:
#line 798 "def.y" /* yacc.c:1646  */
    { 
              if (defrComponentCbk) {
                defrComponent.defiComponent::setForeignName((yyvsp[-3].string));
                defrComponent.defiComponent::setForeignLocation(
                (yyvsp[-2].pt).x, (yyvsp[-2].pt).y, (yyvsp[0].string));
              }
             }
#line 3429 "def.tab.c" /* yacc.c:1646  */
    break;

  case 231:
#line 809 "def.y" /* yacc.c:1646  */
    { (yyval.pt) = (yyvsp[0].pt); }
#line 3435 "def.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 811 "def.y" /* yacc.c:1646  */
    { (yyval.pt).x = static_cast<int>((yyvsp[-1].dval)); (yyval.pt).y = static_cast<int>((yyvsp[0].dval)); }
#line 3441 "def.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 815 "def.y" /* yacc.c:1646  */
    {
              if (defrComponentCbk) {
                defrComponent.defiComponent::setPlacementStatus((yyvsp[-2].integer));
                defrComponent.defiComponent::setPlacementLocation(
                (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
              }
            }
#line 3453 "def.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 825 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = DEFI_COMPONENT_FIXED; }
#line 3459 "def.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 827 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = DEFI_COMPONENT_COVER; }
#line 3465 "def.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 829 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = DEFI_COMPONENT_PLACED; }
#line 3471 "def.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 831 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = DEFI_COMPONENT_UNPLACED; }
#line 3477 "def.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 835 "def.y" /* yacc.c:1646  */
    {
              if (defrComponentCbk)
                defrComponent.defiComponent::setWeight(ROUND((yyvsp[0].dval)));
            }
#line 3486 "def.tab.c" /* yacc.c:1646  */
    break;

  case 239:
#line 842 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrComponentEndCbk, defrComponentEndCbkType, 0); }
#line 3492 "def.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 849 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrNetStartCbk, defrNetStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 3498 "def.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 857 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrNetCbk, defrNetCbkType, &defrNet); }
#line 3504 "def.tab.c" /* yacc.c:1646  */
    break;

  case 245:
#line 867 "def.y" /* yacc.c:1646  */
    {dumb_mode = 0; no_num = 0; }
#line 3510 "def.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 871 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1000000000; no_num = 10000000; nondef_is_keyword = TRUE;}
#line 3516 "def.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 872 "def.y" /* yacc.c:1646  */
    {
              if (defrNetCbk)
                defrNet.defiNet::setName((yyvsp[0].string));
            }
#line 3525 "def.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 882 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1000000000; no_num = 10000000;}
#line 3531 "def.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 883 "def.y" /* yacc.c:1646  */
    {
            if (defrNetCbk)
              defrNet.defiNet::addPin((yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].integer));
            /* 1/14/2000 -- Wanda da Rosa, pcr 289156 */
            /* reset dumb_mode & no_num to 3 , just in case */
            /* the next statement is another net_connection */
            dumb_mode = 3;
            no_num = 3;
            }
#line 3545 "def.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 892 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1;}
#line 3551 "def.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 893 "def.y" /* yacc.c:1646  */
    {
            if (defrNetCbk)
              defrNet.defiNet::addPin("*", (yyvsp[-2].string), (yyvsp[-1].integer));
            dumb_mode = 3;
            no_num = 3;
            }
#line 3562 "def.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 899 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1;}
#line 3568 "def.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 900 "def.y" /* yacc.c:1646  */
    {
            if (defrNetCbk)
              defrNet.defiNet::addPin("PIN", (yyvsp[-2].string), (yyvsp[-1].integer));
            dumb_mode = 3;
            no_num = 3;
            }
#line 3579 "def.tab.c" /* yacc.c:1646  */
    break;

  case 256:
#line 906 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1;}
#line 3585 "def.tab.c" /* yacc.c:1646  */
    break;

  case 257:
#line 907 "def.y" /* yacc.c:1646  */
    {   
            if (defrNetCbk)
              defrNet.defiNet::addMustPin((yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].integer));
            dumb_mode = 3;
            no_num = 3;
            }
#line 3596 "def.tab.c" /* yacc.c:1646  */
    break;

  case 258:
#line 916 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 0; }
#line 3602 "def.tab.c" /* yacc.c:1646  */
    break;

  case 259:
#line 918 "def.y" /* yacc.c:1646  */
    {
	    CALLBACK(defrNetConnectionExtCbk, defrNetConnectionExtCbkType,
              History_text);
	    (yyval.integer) = 0; }
#line 3611 "def.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 923 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 1; }
#line 3617 "def.tab.c" /* yacc.c:1646  */
    break;

  case 263:
#line 931 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; }
#line 3623 "def.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 932 "def.y" /* yacc.c:1646  */
    {  
            if (defrNetCbk) defrNet.defiNet::setType((yyvsp[-2].string));
            new_is_keyword = FALSE;
            nondef_is_keyword = FALSE;
            }
#line 3633 "def.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 937 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3639 "def.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 938 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setSource((yyvsp[0].string)); }
#line 3645 "def.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 940 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1;}
#line 3651 "def.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 941 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setOriginal((yyvsp[0].string)); }
#line 3657 "def.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 943 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3663 "def.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 944 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setPattern((yyvsp[0].string)); }
#line 3669 "def.tab.c" /* yacc.c:1646  */
    break;

  case 271:
#line 947 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setWeight(ROUND((yyvsp[0].dval))); }
#line 3675 "def.tab.c" /* yacc.c:1646  */
    break;

  case 272:
#line 950 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setXTalk(ROUND((yyvsp[0].dval))); }
#line 3681 "def.tab.c" /* yacc.c:1646  */
    break;

  case 273:
#line 953 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setCap((yyvsp[0].dval)); }
#line 3687 "def.tab.c" /* yacc.c:1646  */
    break;

  case 274:
#line 955 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3693 "def.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 956 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setUse((yyvsp[0].string)); }
#line 3699 "def.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 958 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3705 "def.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 959 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setStyle((yyvsp[0].string)); }
#line 3711 "def.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 961 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3717 "def.tab.c" /* yacc.c:1646  */
    break;

  case 279:
#line 962 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setNonDefaultRule((yyvsp[0].string)); }
#line 3723 "def.tab.c" /* yacc.c:1646  */
    break;

  case 281:
#line 966 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3729 "def.tab.c" /* yacc.c:1646  */
    break;

  case 282:
#line 967 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::addShieldNet((yyvsp[0].string)); }
#line 3735 "def.tab.c" /* yacc.c:1646  */
    break;

  case 283:
#line 969 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 3741 "def.tab.c" /* yacc.c:1646  */
    break;

  case 284:
#line 970 "def.y" /* yacc.c:1646  */
    {
            if (defrNetCbk) defrNet.defiNet::addShield("");
            new_is_keyword = FALSE;
            shield = TRUE;    /* save the path info in the shield paths */
            }
#line 3751 "def.tab.c" /* yacc.c:1646  */
    break;

  case 285:
#line 976 "def.y" /* yacc.c:1646  */
    {
            shield = FALSE;
            }
#line 3759 "def.tab.c" /* yacc.c:1646  */
    break;

  case 286:
#line 980 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1;
                         if (defrNetCbk) {
                           defrSubnet =
                           (defiSubnet*)malloc(sizeof(defiSubnet));
                           defrSubnet->defiSubnet::Init();
                         }
                       }
#line 3771 "def.tab.c" /* yacc.c:1646  */
    break;

  case 287:
#line 988 "def.y" /* yacc.c:1646  */
    {
            if (defrNetCbk) {
              defrSubnet->defiSubnet::setName((yyvsp[-2].string));
              defrNet.defiNet::addSubnet(defrSubnet);
              defrSubnet = 0;
            }
            }
#line 3783 "def.tab.c" /* yacc.c:1646  */
    break;

  case 288:
#line 996 "def.y" /* yacc.c:1646  */
    {dumb_mode = 10000000;}
#line 3789 "def.tab.c" /* yacc.c:1646  */
    break;

  case 289:
#line 997 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; }
#line 3795 "def.tab.c" /* yacc.c:1646  */
    break;

  case 290:
#line 1000 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrNetExtCbk, defrNetExtCbkType, History_text); }
#line 3801 "def.tab.c" /* yacc.c:1646  */
    break;

  case 293:
#line 1008 "def.y" /* yacc.c:1646  */
    {
              if (defrNetCbk)
                defrNet.defiNet::addProp((yyvsp[-1].string), (yyvsp[0].string));
            }
#line 3810 "def.tab.c" /* yacc.c:1646  */
    break;

  case 294:
#line 1015 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk)
	defrNet.defiNet::addVpinBounds((yyvsp[-2].pt).x, (yyvsp[-2].pt).y, (yyvsp[-1].pt).x, (yyvsp[-1].pt).y); }
#line 3817 "def.tab.c" /* yacc.c:1646  */
    break;

  case 295:
#line 1019 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1;}
#line 3823 "def.tab.c" /* yacc.c:1646  */
    break;

  case 296:
#line 1020 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::addVpin((yyvsp[0].string)); }
#line 3829 "def.tab.c" /* yacc.c:1646  */
    break;

  case 298:
#line 1024 "def.y" /* yacc.c:1646  */
    {dumb_mode=1;}
#line 3835 "def.tab.c" /* yacc.c:1646  */
    break;

  case 299:
#line 1025 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::addVpinLayer((yyvsp[0].string)); }
#line 3841 "def.tab.c" /* yacc.c:1646  */
    break;

  case 300:
#line 1029 "def.y" /* yacc.c:1646  */
    {
	      /* We will use a temporary string to store the number.
	       * The string space is borrowed from the ring buffer
	       * in the lexer. */
              char* str = ringCopy("                       ");
              sprintf(str, "%g", (yyvsp[0].dval));
              (yyval.string) = str;
            }
#line 3854 "def.tab.c" /* yacc.c:1646  */
    break;

  case 301:
#line 1038 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string);}
#line 3860 "def.tab.c" /* yacc.c:1646  */
    break;

  case 303:
#line 1043 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::addVpinLoc((yyvsp[-2].string), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer)); }
#line 3866 "def.tab.c" /* yacc.c:1646  */
    break;

  case 304:
#line 1047 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"PLACED"; }
#line 3872 "def.tab.c" /* yacc.c:1646  */
    break;

  case 305:
#line 1049 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"FIXED"; }
#line 3878 "def.tab.c" /* yacc.c:1646  */
    break;

  case 306:
#line 1051 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"COVER"; }
#line 3884 "def.tab.c" /* yacc.c:1646  */
    break;

  case 307:
#line 1055 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"FIXED"; }
#line 3890 "def.tab.c" /* yacc.c:1646  */
    break;

  case 308:
#line 1057 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"COVER"; }
#line 3896 "def.tab.c" /* yacc.c:1646  */
    break;

  case 309:
#line 1059 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"ROUTED"; }
#line 3902 "def.tab.c" /* yacc.c:1646  */
    break;

  case 310:
#line 1064 "def.y" /* yacc.c:1646  */
    { if (defrNeedPathData) pathIsDone(shield); }
#line 3908 "def.tab.c" /* yacc.c:1646  */
    break;

  case 311:
#line 1066 "def.y" /* yacc.c:1646  */
    { }
#line 3914 "def.tab.c" /* yacc.c:1646  */
    break;

  case 312:
#line 1069 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; }
#line 3920 "def.tab.c" /* yacc.c:1646  */
    break;

  case 313:
#line 1070 "def.y" /* yacc.c:1646  */
    { if (defrNeedPathData) pathIsDone(shield); }
#line 3926 "def.tab.c" /* yacc.c:1646  */
    break;

  case 314:
#line 1074 "def.y" /* yacc.c:1646  */
    { if (defrNeedPathData) defrPath->defiPath::addLayer((yyvsp[0].string)); }
#line 3932 "def.tab.c" /* yacc.c:1646  */
    break;

  case 315:
#line 1076 "def.y" /* yacc.c:1646  */
    { dumb_mode = 10000000; new_is_keyword = TRUE; }
#line 3938 "def.tab.c" /* yacc.c:1646  */
    break;

  case 316:
#line 1078 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; }
#line 3944 "def.tab.c" /* yacc.c:1646  */
    break;

  case 319:
#line 1087 "def.y" /* yacc.c:1646  */
    { if (defrNeedPathData) defrPath->defiPath::addVia((yyvsp[0].string)); }
#line 3950 "def.tab.c" /* yacc.c:1646  */
    break;

  case 320:
#line 1089 "def.y" /* yacc.c:1646  */
    { }
#line 3956 "def.tab.c" /* yacc.c:1646  */
    break;

  case 321:
#line 1095 "def.y" /* yacc.c:1646  */
    {
	if (defrNeedPathData)
	  defrPath->defiPath::addPoint(ROUND((yyvsp[-2].dval)), ROUND((yyvsp[-1].dval))); 
	save_x = (yyvsp[-2].dval);
	save_y = (yyvsp[-1].dval);
      }
#line 3967 "def.tab.c" /* yacc.c:1646  */
    break;

  case 322:
#line 1102 "def.y" /* yacc.c:1646  */
    {
	if (defrNeedPathData)
	  defrPath->defiPath::addPoint(ROUND(save_x), ROUND((yyvsp[-1].dval))); 
	save_y = (yyvsp[-1].dval);
      }
#line 3977 "def.tab.c" /* yacc.c:1646  */
    break;

  case 323:
#line 1108 "def.y" /* yacc.c:1646  */
    {
	if (defrNeedPathData)
	  defrPath->defiPath::addPoint(ROUND((yyvsp[-2].dval)), ROUND(save_y)); 
	save_x = (yyvsp[-2].dval);
      }
#line 3987 "def.tab.c" /* yacc.c:1646  */
    break;

  case 324:
#line 1114 "def.y" /* yacc.c:1646  */
    {
	if (defrNeedPathData)
	  defrPath->defiPath::addPoint(ROUND(save_x), ROUND(save_y)); 
      }
#line 3996 "def.tab.c" /* yacc.c:1646  */
    break;

  case 325:
#line 1119 "def.y" /* yacc.c:1646  */
    {
	if (defrNeedPathData)
	  defrPath->defiPath::addFlushPoint(ROUND((yyvsp[-3].dval)), ROUND((yyvsp[-2].dval)), ROUND((yyvsp[-1].dval))); 
	save_x = (yyvsp[-3].dval);
	save_y = (yyvsp[-2].dval);
      }
#line 4007 "def.tab.c" /* yacc.c:1646  */
    break;

  case 326:
#line 1126 "def.y" /* yacc.c:1646  */
    {
	if (defrNeedPathData)
	  defrPath->defiPath::addFlushPoint(ROUND(save_x), ROUND((yyvsp[-2].dval)),
	  ROUND((yyvsp[-1].dval))); 
	save_y = (yyvsp[-2].dval);
      }
#line 4018 "def.tab.c" /* yacc.c:1646  */
    break;

  case 327:
#line 1133 "def.y" /* yacc.c:1646  */
    {
	if (defrNeedPathData)
	  defrPath->defiPath::addFlushPoint(ROUND((yyvsp[-3].dval)), ROUND(save_y),
	  ROUND((yyvsp[-1].dval))); 
	save_x = (yyvsp[-3].dval);
      }
#line 4029 "def.tab.c" /* yacc.c:1646  */
    break;

  case 328:
#line 1140 "def.y" /* yacc.c:1646  */
    {
	if (defrNeedPathData)
	  defrPath->defiPath::addFlushPoint(ROUND(save_x), ROUND(save_y),
	  ROUND((yyvsp[-1].dval))); 
      }
#line 4039 "def.tab.c" /* yacc.c:1646  */
    break;

  case 329:
#line 1149 "def.y" /* yacc.c:1646  */
    { }
#line 4045 "def.tab.c" /* yacc.c:1646  */
    break;

  case 330:
#line 1151 "def.y" /* yacc.c:1646  */
    { if (defrNeedPathData) defrPath->defiPath::setTaper(); }
#line 4051 "def.tab.c" /* yacc.c:1646  */
    break;

  case 331:
#line 1152 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; }
#line 4057 "def.tab.c" /* yacc.c:1646  */
    break;

  case 332:
#line 1153 "def.y" /* yacc.c:1646  */
    { if (defrNeedPathData) defrPath->defiPath::addTaperRule((yyvsp[0].string)); }
#line 4063 "def.tab.c" /* yacc.c:1646  */
    break;

  case 333:
#line 1158 "def.y" /* yacc.c:1646  */
    { }
#line 4069 "def.tab.c" /* yacc.c:1646  */
    break;

  case 334:
#line 1160 "def.y" /* yacc.c:1646  */
    { if (defrNeedPathData) defrPath->defiPath::addShape((yyvsp[0].string)); }
#line 4075 "def.tab.c" /* yacc.c:1646  */
    break;

  case 335:
#line 1165 "def.y" /* yacc.c:1646  */
    { }
#line 4081 "def.tab.c" /* yacc.c:1646  */
    break;

  case 336:
#line 1167 "def.y" /* yacc.c:1646  */
    { if (defrNeedPathData) defrPath->defiPath::addWidth(ROUND((yyvsp[0].dval))); }
#line 4087 "def.tab.c" /* yacc.c:1646  */
    break;

  case 337:
#line 1172 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrNetEndCbk, defrNetEndCbkType, 0); }
#line 4093 "def.tab.c" /* yacc.c:1646  */
    break;

  case 341:
#line 1183 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrSNetCbk, defrSNetCbkType, &defrNet); }
#line 4099 "def.tab.c" /* yacc.c:1646  */
    break;

  case 348:
#line 1194 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; }
#line 4105 "def.tab.c" /* yacc.c:1646  */
    break;

  case 349:
#line 1195 "def.y" /* yacc.c:1646  */
    {
            if (defrNetCbk) defrNet.defiNet::setType((yyvsp[-2].string));
            new_is_keyword = FALSE;
            }
#line 4114 "def.tab.c" /* yacc.c:1646  */
    break;

  case 350:
#line 1200 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4120 "def.tab.c" /* yacc.c:1646  */
    break;

  case 351:
#line 1201 "def.y" /* yacc.c:1646  */
    {
            if (defrNetCbk) defrNet.defiNet::addShield((yyvsp[0].string));
            new_is_keyword = FALSE;
            shield = TRUE;   /* save the path info in the shield paths */
            }
#line 4130 "def.tab.c" /* yacc.c:1646  */
    break;

  case 352:
#line 1207 "def.y" /* yacc.c:1646  */
    {
            shield = FALSE;
            }
#line 4138 "def.tab.c" /* yacc.c:1646  */
    break;

  case 353:
#line 1211 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4144 "def.tab.c" /* yacc.c:1646  */
    break;

  case 354:
#line 1212 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setSource((yyvsp[0].string)); }
#line 4150 "def.tab.c" /* yacc.c:1646  */
    break;

  case 355:
#line 1214 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1;}
#line 4156 "def.tab.c" /* yacc.c:1646  */
    break;

  case 356:
#line 1215 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setOriginal((yyvsp[0].string)); }
#line 4162 "def.tab.c" /* yacc.c:1646  */
    break;

  case 357:
#line 1217 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4168 "def.tab.c" /* yacc.c:1646  */
    break;

  case 358:
#line 1218 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setPattern((yyvsp[0].string)); }
#line 4174 "def.tab.c" /* yacc.c:1646  */
    break;

  case 359:
#line 1221 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setWeight(ROUND((yyvsp[0].dval))); }
#line 4180 "def.tab.c" /* yacc.c:1646  */
    break;

  case 360:
#line 1224 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setCap((yyvsp[0].dval)); }
#line 4186 "def.tab.c" /* yacc.c:1646  */
    break;

  case 361:
#line 1226 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4192 "def.tab.c" /* yacc.c:1646  */
    break;

  case 362:
#line 1227 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setUse((yyvsp[0].string)); }
#line 4198 "def.tab.c" /* yacc.c:1646  */
    break;

  case 363:
#line 1229 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4204 "def.tab.c" /* yacc.c:1646  */
    break;

  case 364:
#line 1230 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrNet.defiNet::setStyle((yyvsp[0].string)); }
#line 4210 "def.tab.c" /* yacc.c:1646  */
    break;

  case 365:
#line 1232 "def.y" /* yacc.c:1646  */
    {dumb_mode = 10000000;}
#line 4216 "def.tab.c" /* yacc.c:1646  */
    break;

  case 366:
#line 1233 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; }
#line 4222 "def.tab.c" /* yacc.c:1646  */
    break;

  case 367:
#line 1236 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrNetExtCbk, defrNetExtCbkType, History_text); }
#line 4228 "def.tab.c" /* yacc.c:1646  */
    break;

  case 368:
#line 1239 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; }
#line 4234 "def.tab.c" /* yacc.c:1646  */
    break;

  case 369:
#line 1240 "def.y" /* yacc.c:1646  */
    {
              if (defrSNetCbk) defrNet.defiNet::setWidth((yyvsp[-1].string), (yyvsp[0].dval));
            }
#line 4242 "def.tab.c" /* yacc.c:1646  */
    break;

  case 370:
#line 1246 "def.y" /* yacc.c:1646  */
    {
              if (defrSNetCbk) defrNet.defiNet::setVoltage((yyvsp[0].dval));
            }
#line 4250 "def.tab.c" /* yacc.c:1646  */
    break;

  case 371:
#line 1251 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; }
#line 4256 "def.tab.c" /* yacc.c:1646  */
    break;

  case 372:
#line 1252 "def.y" /* yacc.c:1646  */
    {
              if (defrSNetCbk) defrNet.defiNet::setSpacing((yyvsp[-1].string),(yyvsp[0].dval));
            }
#line 4264 "def.tab.c" /* yacc.c:1646  */
    break;

  case 373:
#line 1256 "def.y" /* yacc.c:1646  */
    {
            }
#line 4271 "def.tab.c" /* yacc.c:1646  */
    break;

  case 375:
#line 1262 "def.y" /* yacc.c:1646  */
    {
              if (defrSNetCbk) defrNet.defiNet::setRange((yyvsp[-1].dval),(yyvsp[0].dval));
            }
#line 4279 "def.tab.c" /* yacc.c:1646  */
    break;

  case 377:
#line 1269 "def.y" /* yacc.c:1646  */
    { defrProp.defiProp::setRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 4285 "def.tab.c" /* yacc.c:1646  */
    break;

  case 378:
#line 1273 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrSNetStartCbk, defrSNetStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 4291 "def.tab.c" /* yacc.c:1646  */
    break;

  case 379:
#line 1277 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrSNetEndCbk, defrSNetEndCbkType, 0); }
#line 4297 "def.tab.c" /* yacc.c:1646  */
    break;

  case 381:
#line 1284 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrGroupsStartCbk, defrGroupsStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 4303 "def.tab.c" /* yacc.c:1646  */
    break;

  case 384:
#line 1292 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrGroupCbk, defrGroupCbkType, &defrGroup); }
#line 4309 "def.tab.c" /* yacc.c:1646  */
    break;

  case 385:
#line 1295 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4315 "def.tab.c" /* yacc.c:1646  */
    break;

  case 386:
#line 1296 "def.y" /* yacc.c:1646  */
    {
              dumb_mode = 1000000000;
              no_num = 1000000000;
              /* dumb_mode is automatically turned off at the first
               * + in the options or at the ; at the end of the group */
              if (defrGroupCbk) defrGroup.defiGroup::setup((yyvsp[0].string));
              CALLBACK(defrGroupNameCbk, defrGroupNameCbkType, (yyvsp[0].string));
            }
#line 4328 "def.tab.c" /* yacc.c:1646  */
    break;

  case 388:
#line 1308 "def.y" /* yacc.c:1646  */
    {  }
#line 4334 "def.tab.c" /* yacc.c:1646  */
    break;

  case 389:
#line 1312 "def.y" /* yacc.c:1646  */
    {
              /* if (defrGroupCbk) defrGroup.defiGroup::addMember($1); */
              CALLBACK(defrGroupMemberCbk, defrGroupMemberCbkType, (yyvsp[0].string));
            }
#line 4343 "def.tab.c" /* yacc.c:1646  */
    break;

  case 392:
#line 1323 "def.y" /* yacc.c:1646  */
    { }
#line 4349 "def.tab.c" /* yacc.c:1646  */
    break;

  case 393:
#line 1324 "def.y" /* yacc.c:1646  */
    { dumb_mode = 10000000; }
#line 4355 "def.tab.c" /* yacc.c:1646  */
    break;

  case 394:
#line 1325 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; }
#line 4361 "def.tab.c" /* yacc.c:1646  */
    break;

  case 395:
#line 1326 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1;  no_num = 1; }
#line 4367 "def.tab.c" /* yacc.c:1646  */
    break;

  case 396:
#line 1327 "def.y" /* yacc.c:1646  */
    { }
#line 4373 "def.tab.c" /* yacc.c:1646  */
    break;

  case 397:
#line 1329 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrGroupExtCbk, defrGroupExtCbkType,
	      History_text); }
#line 4380 "def.tab.c" /* yacc.c:1646  */
    break;

  case 398:
#line 1335 "def.y" /* yacc.c:1646  */
    { if (defrGroupCbk)
	    defrGroup.defiGroup::addRegionRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 4387 "def.tab.c" /* yacc.c:1646  */
    break;

  case 399:
#line 1338 "def.y" /* yacc.c:1646  */
    { if (defrGroupCbk)
	    defrGroup.defiGroup::setRegionName((yyvsp[0].string));
	  }
#line 4395 "def.tab.c" /* yacc.c:1646  */
    break;

  case 402:
#line 1348 "def.y" /* yacc.c:1646  */
    { if (defrGroupCbk) defrGroup.defiGroup::addProperty((yyvsp[-1].string), (yyvsp[0].string)); }
#line 4401 "def.tab.c" /* yacc.c:1646  */
    break;

  case 404:
#line 1353 "def.y" /* yacc.c:1646  */
    { }
#line 4407 "def.tab.c" /* yacc.c:1646  */
    break;

  case 405:
#line 1357 "def.y" /* yacc.c:1646  */
    {
               if (defrGroupCbk) defrGroup.defiGroup::setMaxX(ROUND((yyvsp[0].dval)));
             }
#line 4415 "def.tab.c" /* yacc.c:1646  */
    break;

  case 406:
#line 1361 "def.y" /* yacc.c:1646  */
    { 
               if (defrGroupCbk) defrGroup.defiGroup::setMaxY(ROUND((yyvsp[0].dval)));
             }
#line 4423 "def.tab.c" /* yacc.c:1646  */
    break;

  case 407:
#line 1365 "def.y" /* yacc.c:1646  */
    { 
               if (defrGroupCbk) defrGroup.defiGroup::setPerim(ROUND((yyvsp[0].dval)));
             }
#line 4431 "def.tab.c" /* yacc.c:1646  */
    break;

  case 408:
#line 1371 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrGroupsEndCbk, defrGroupsEndCbkType, 0); }
#line 4437 "def.tab.c" /* yacc.c:1646  */
    break;

  case 411:
#line 1381 "def.y" /* yacc.c:1646  */
    {
	  CALLBACK(defrAssertionsStartCbk, defrAssertionsStartCbkType,
	      ROUND((yyvsp[-1].dval)));
	  if (defrAssertionCbk)
              defrAssertion.defiAssertion::setAssertionMode();
	}
#line 4448 "def.tab.c" /* yacc.c:1646  */
    break;

  case 412:
#line 1390 "def.y" /* yacc.c:1646  */
    {
            CALLBACK(defrConstraintsStartCbk, defrConstraintsStartCbkType,
              ROUND((yyvsp[-1].dval)));
	    if (defrConstraintCbk)
	      defrAssertion.defiAssertion::setConstraintMode();
          }
#line 4459 "def.tab.c" /* yacc.c:1646  */
    break;

  case 416:
#line 1404 "def.y" /* yacc.c:1646  */
    {
              if (defrAssertion.defiAssertion::isConstraint()) 
		CALLBACK(defrConstraintCbk, defrConstraintCbkType,
		&defrAssertion);
              if (defrAssertion.defiAssertion::isAssertion()) 
		CALLBACK(defrAssertionCbk, defrAssertionCbkType,
		&defrAssertion);
            }
#line 4472 "def.tab.c" /* yacc.c:1646  */
    break;

  case 417:
#line 1416 "def.y" /* yacc.c:1646  */
    { 
              if (defrAssertion.defiAssertion::isConstraint()) 
		CALLBACK(defrConstraintCbk, defrConstraintCbkType,
		&defrAssertion);
              if (defrAssertion.defiAssertion::isAssertion()) 
                CALLBACK(defrAssertionCbk, defrAssertionCbkType,
		&defrAssertion);
   
              // reset all the flags and everything
              defrAssertion.defiAssertion::clear();
            }
#line 4488 "def.tab.c" /* yacc.c:1646  */
    break;

  case 418:
#line 1429 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4494 "def.tab.c" /* yacc.c:1646  */
    break;

  case 419:
#line 1430 "def.y" /* yacc.c:1646  */
    {
              if (defrConstraintCbk || defrAssertionCbk)
                defrAssertion.defiAssertion::addNet((yyvsp[0].string));
            }
#line 4503 "def.tab.c" /* yacc.c:1646  */
    break;

  case 420:
#line 1434 "def.y" /* yacc.c:1646  */
    {dumb_mode = 4; no_num = 4;}
#line 4509 "def.tab.c" /* yacc.c:1646  */
    break;

  case 421:
#line 1436 "def.y" /* yacc.c:1646  */
    {
              if (defrConstraintCbk || defrAssertionCbk)
                defrAssertion.defiAssertion::addPath((yyvsp[-3].string), (yyvsp[-2].string), (yyvsp[-1].string), (yyvsp[0].string));
            }
#line 4518 "def.tab.c" /* yacc.c:1646  */
    break;

  case 422:
#line 1441 "def.y" /* yacc.c:1646  */
    {
              if (defrConstraintCbk || defrAssertionCbk)
                defrAssertion.defiAssertion::setSum();
            }
#line 4527 "def.tab.c" /* yacc.c:1646  */
    break;

  case 423:
#line 1446 "def.y" /* yacc.c:1646  */
    {
              if (defrConstraintCbk || defrAssertionCbk)
                defrAssertion.defiAssertion::setDiff();
            }
#line 4536 "def.tab.c" /* yacc.c:1646  */
    break;

  case 425:
#line 1454 "def.y" /* yacc.c:1646  */
    { }
#line 4542 "def.tab.c" /* yacc.c:1646  */
    break;

  case 426:
#line 1457 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4548 "def.tab.c" /* yacc.c:1646  */
    break;

  case 427:
#line 1459 "def.y" /* yacc.c:1646  */
    {
              if (defrConstraintCbk || defrAssertionCbk)
                defrAssertion.defiAssertion::setWiredlogic((yyvsp[-4].string), (yyvsp[-1].dval));
            }
#line 4557 "def.tab.c" /* yacc.c:1646  */
    break;

  case 428:
#line 1467 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)""; }
#line 4563 "def.tab.c" /* yacc.c:1646  */
    break;

  case 429:
#line 1469 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"+"; }
#line 4569 "def.tab.c" /* yacc.c:1646  */
    break;

  case 432:
#line 1477 "def.y" /* yacc.c:1646  */
    {
              if (defrConstraintCbk || defrAssertionCbk)
                defrAssertion.defiAssertion::setRiseMin((yyvsp[0].dval));
            }
#line 4578 "def.tab.c" /* yacc.c:1646  */
    break;

  case 433:
#line 1482 "def.y" /* yacc.c:1646  */
    {
              if (defrConstraintCbk || defrAssertionCbk)
                defrAssertion.defiAssertion::setRiseMax((yyvsp[0].dval));
            }
#line 4587 "def.tab.c" /* yacc.c:1646  */
    break;

  case 434:
#line 1487 "def.y" /* yacc.c:1646  */
    {
              if (defrConstraintCbk || defrAssertionCbk)
                defrAssertion.defiAssertion::setFallMin((yyvsp[0].dval));
            }
#line 4596 "def.tab.c" /* yacc.c:1646  */
    break;

  case 435:
#line 1492 "def.y" /* yacc.c:1646  */
    {
              if (defrConstraintCbk || defrAssertionCbk)
                defrAssertion.defiAssertion::setFallMax((yyvsp[0].dval));
            }
#line 4605 "def.tab.c" /* yacc.c:1646  */
    break;

  case 436:
#line 1499 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrConstraintsEndCbk, defrConstraintsEndCbkType, 0); }
#line 4611 "def.tab.c" /* yacc.c:1646  */
    break;

  case 437:
#line 1503 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrAssertionsEndCbk, defrAssertionsEndCbkType, 0); }
#line 4617 "def.tab.c" /* yacc.c:1646  */
    break;

  case 439:
#line 1510 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrScanchainsStartCbk, defrScanchainsStartCbkType,
              ROUND((yyvsp[-1].dval))); }
#line 4624 "def.tab.c" /* yacc.c:1646  */
    break;

  case 441:
#line 1516 "def.y" /* yacc.c:1646  */
    {}
#line 4630 "def.tab.c" /* yacc.c:1646  */
    break;

  case 442:
#line 1520 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrScanchainCbk, defrScanchainCbkType, &defrScanchain); }
#line 4636 "def.tab.c" /* yacc.c:1646  */
    break;

  case 443:
#line 1523 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1;}
#line 4642 "def.tab.c" /* yacc.c:1646  */
    break;

  case 444:
#line 1524 "def.y" /* yacc.c:1646  */
    {
              if (defrScanchainCbk)
                defrScanchain.defiScanchain::setName((yyvsp[0].string));
            }
#line 4651 "def.tab.c" /* yacc.c:1646  */
    break;

  case 447:
#line 1536 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)""; }
#line 4657 "def.tab.c" /* yacc.c:1646  */
    break;

  case 448:
#line 1538 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string); }
#line 4663 "def.tab.c" /* yacc.c:1646  */
    break;

  case 449:
#line 1541 "def.y" /* yacc.c:1646  */
    {dumb_mode = 2; no_num = 2;}
#line 4669 "def.tab.c" /* yacc.c:1646  */
    break;

  case 450:
#line 1542 "def.y" /* yacc.c:1646  */
    { if (defrScanchainCbk)
	    defrScanchain.defiScanchain::setStart((yyvsp[-1].string), (yyvsp[0].string)); }
#line 4676 "def.tab.c" /* yacc.c:1646  */
    break;

  case 451:
#line 1545 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4682 "def.tab.c" /* yacc.c:1646  */
    break;

  case 452:
#line 1546 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; no_num = 0; }
#line 4688 "def.tab.c" /* yacc.c:1646  */
    break;

  case 453:
#line 1549 "def.y" /* yacc.c:1646  */
    {
            dumb_mode = 1;
	    no_num = 1;
            if (defrScanchainCbk)
              defrScanchain.defiScanchain::addOrderedList();
          }
#line 4699 "def.tab.c" /* yacc.c:1646  */
    break;

  case 454:
#line 1556 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; no_num = 0; }
#line 4705 "def.tab.c" /* yacc.c:1646  */
    break;

  case 455:
#line 1558 "def.y" /* yacc.c:1646  */
    {dumb_mode = 2; no_num = 2; }
#line 4711 "def.tab.c" /* yacc.c:1646  */
    break;

  case 456:
#line 1559 "def.y" /* yacc.c:1646  */
    { if (defrScanchainCbk)
	    defrScanchain.defiScanchain::setStop((yyvsp[-1].string), (yyvsp[0].string)); }
#line 4718 "def.tab.c" /* yacc.c:1646  */
    break;

  case 457:
#line 1562 "def.y" /* yacc.c:1646  */
    { dumb_mode = 10; no_num = 10; }
#line 4724 "def.tab.c" /* yacc.c:1646  */
    break;

  case 458:
#line 1564 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0;  no_num = 0; }
#line 4730 "def.tab.c" /* yacc.c:1646  */
    break;

  case 459:
#line 1567 "def.y" /* yacc.c:1646  */
    {
	    CALLBACK(defrScanChainExtCbk, defrScanChainExtCbkType,
              History_text);
	  }
#line 4739 "def.tab.c" /* yacc.c:1646  */
    break;

  case 460:
#line 1574 "def.y" /* yacc.c:1646  */
    { }
#line 4745 "def.tab.c" /* yacc.c:1646  */
    break;

  case 461:
#line 1576 "def.y" /* yacc.c:1646  */
    {
	if (defrScanchainCbk) {
	  if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
	    defrScanchain.defiScanchain::setCommonIn((yyvsp[-1].string));
	  else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
	    defrScanchain.defiScanchain::setCommonOut((yyvsp[-1].string));
	}
      }
#line 4758 "def.tab.c" /* yacc.c:1646  */
    break;

  case 462:
#line 1585 "def.y" /* yacc.c:1646  */
    {
	if (defrScanchainCbk) {
	  if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
	    defrScanchain.defiScanchain::setCommonIn((yyvsp[-5].string));
	  else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
	    defrScanchain.defiScanchain::setCommonOut((yyvsp[-5].string));
	  if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
	    defrScanchain.defiScanchain::setCommonIn((yyvsp[-1].string));
	  else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
	    defrScanchain.defiScanchain::setCommonOut((yyvsp[-1].string));
	}
      }
#line 4775 "def.tab.c" /* yacc.c:1646  */
    break;

  case 465:
#line 1605 "def.y" /* yacc.c:1646  */
    {
	dumb_mode = 1000;
	no_num = 1000;
	if (defrScanchainCbk)
	  defrScanchain.defiScanchain::addFloatingInst((yyvsp[0].string));
      }
#line 4786 "def.tab.c" /* yacc.c:1646  */
    break;

  case 466:
#line 1612 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4792 "def.tab.c" /* yacc.c:1646  */
    break;

  case 467:
#line 1616 "def.y" /* yacc.c:1646  */
    { }
#line 4798 "def.tab.c" /* yacc.c:1646  */
    break;

  case 468:
#line 1618 "def.y" /* yacc.c:1646  */
    {
	if (defrScanchainCbk) {
	  if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
	    defrScanchain.defiScanchain::addFloatingIn((yyvsp[-1].string));
	  else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
	    defrScanchain.defiScanchain::addFloatingOut((yyvsp[-1].string));
	}
      }
#line 4811 "def.tab.c" /* yacc.c:1646  */
    break;

  case 469:
#line 1627 "def.y" /* yacc.c:1646  */
    {
	if (defrScanchainCbk) {
	  if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
	    defrScanchain.defiScanchain::addFloatingIn((yyvsp[-5].string));
	  else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
	    defrScanchain.defiScanchain::addFloatingOut((yyvsp[-5].string));
	  if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
	    defrScanchain.defiScanchain::addFloatingIn((yyvsp[-1].string));
	  else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
	    defrScanchain.defiScanchain::addFloatingOut((yyvsp[-1].string));
	}
      }
#line 4828 "def.tab.c" /* yacc.c:1646  */
    break;

  case 472:
#line 1646 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1000; no_num = 1000; 
	if (defrScanchainCbk)
	  defrScanchain.defiScanchain::addOrderedInst((yyvsp[0].string));
      }
#line 4837 "def.tab.c" /* yacc.c:1646  */
    break;

  case 473:
#line 1651 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 4843 "def.tab.c" /* yacc.c:1646  */
    break;

  case 474:
#line 1655 "def.y" /* yacc.c:1646  */
    { }
#line 4849 "def.tab.c" /* yacc.c:1646  */
    break;

  case 475:
#line 1657 "def.y" /* yacc.c:1646  */
    {
	if (defrScanchainCbk) {
	  if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
	    defrScanchain.defiScanchain::addOrderedIn((yyvsp[-1].string));
	  else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
	    defrScanchain.defiScanchain::addOrderedOut((yyvsp[-1].string));
	}
      }
#line 4862 "def.tab.c" /* yacc.c:1646  */
    break;

  case 476:
#line 1666 "def.y" /* yacc.c:1646  */
    {
	if (defrScanchainCbk) {
	  if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
	    defrScanchain.defiScanchain::addOrderedIn((yyvsp[-5].string));
	  else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
	    defrScanchain.defiScanchain::addOrderedOut((yyvsp[-5].string));
	  if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
	    defrScanchain.defiScanchain::addOrderedIn((yyvsp[-1].string));
	  else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
	    defrScanchain.defiScanchain::addOrderedOut((yyvsp[-1].string));
	}
      }
#line 4879 "def.tab.c" /* yacc.c:1646  */
    break;

  case 477:
#line 1681 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrScanchainsEndCbk, defrScanchainsEndCbkType, 0); }
#line 4885 "def.tab.c" /* yacc.c:1646  */
    break;

  case 479:
#line 1688 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrIOTimingsStartCbk, defrIOTimingsStartCbkType,
              ROUND((yyvsp[-1].dval))); }
#line 4892 "def.tab.c" /* yacc.c:1646  */
    break;

  case 481:
#line 1694 "def.y" /* yacc.c:1646  */
    {}
#line 4898 "def.tab.c" /* yacc.c:1646  */
    break;

  case 482:
#line 1698 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrIOTimingCbk, defrIOTimingCbkType,
                  &defrIOTiming); }
#line 4905 "def.tab.c" /* yacc.c:1646  */
    break;

  case 483:
#line 1702 "def.y" /* yacc.c:1646  */
    {dumb_mode = 2; no_num = 2; }
#line 4911 "def.tab.c" /* yacc.c:1646  */
    break;

  case 484:
#line 1703 "def.y" /* yacc.c:1646  */
    {
              if (defrIOTimingCbk)
                defrIOTiming.defiIOTiming::setName((yyvsp[-2].string), (yyvsp[-1].string));
            }
#line 4920 "def.tab.c" /* yacc.c:1646  */
    break;

  case 487:
#line 1715 "def.y" /* yacc.c:1646  */
    {
              if (defrIOTimingCbk) 
                defrIOTiming.defiIOTiming::setVariable((yyvsp[-3].string), (yyvsp[-1].dval), (yyvsp[0].dval));
            }
#line 4929 "def.tab.c" /* yacc.c:1646  */
    break;

  case 488:
#line 1720 "def.y" /* yacc.c:1646  */
    {
              if (defrIOTimingCbk) 
                defrIOTiming.defiIOTiming::setSlewRate((yyvsp[-3].string), (yyvsp[-1].dval), (yyvsp[0].dval));
            }
#line 4938 "def.tab.c" /* yacc.c:1646  */
    break;

  case 489:
#line 1725 "def.y" /* yacc.c:1646  */
    {
              if (defrIOTimingCbk) 
                defrIOTiming.defiIOTiming::setCapacitance((yyvsp[0].dval));
            }
#line 4947 "def.tab.c" /* yacc.c:1646  */
    break;

  case 490:
#line 1729 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1; }
#line 4953 "def.tab.c" /* yacc.c:1646  */
    break;

  case 491:
#line 1730 "def.y" /* yacc.c:1646  */
    {
              if (defrIOTimingCbk) 
                defrIOTiming.defiIOTiming::setDriveCell((yyvsp[0].string));
            }
#line 4962 "def.tab.c" /* yacc.c:1646  */
    break;

  case 492:
#line 1735 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1; }
#line 4968 "def.tab.c" /* yacc.c:1646  */
    break;

  case 493:
#line 1736 "def.y" /* yacc.c:1646  */
    {
              if (defrIOTimingCbk) 
                defrIOTiming.defiIOTiming::setFrom((yyvsp[0].string));
            }
#line 4977 "def.tab.c" /* yacc.c:1646  */
    break;

  case 494:
#line 1741 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1; }
#line 4983 "def.tab.c" /* yacc.c:1646  */
    break;

  case 495:
#line 1742 "def.y" /* yacc.c:1646  */
    {
              if (defrIOTimingCbk) 
                defrIOTiming.defiIOTiming::setTo((yyvsp[0].string));
            }
#line 4992 "def.tab.c" /* yacc.c:1646  */
    break;

  case 496:
#line 1748 "def.y" /* yacc.c:1646  */
    {
              if (defrIOTimingCbk) 
                defrIOTiming.defiIOTiming::setParallel((yyvsp[0].dval));
            }
#line 5001 "def.tab.c" /* yacc.c:1646  */
    break;

  case 497:
#line 1753 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrIoTimingsExtCbk, defrIoTimingsExtCbkType,
              History_text); }
#line 5008 "def.tab.c" /* yacc.c:1646  */
    break;

  case 498:
#line 1757 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"RISE"; }
#line 5014 "def.tab.c" /* yacc.c:1646  */
    break;

  case 499:
#line 1757 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"FALL"; }
#line 5020 "def.tab.c" /* yacc.c:1646  */
    break;

  case 500:
#line 1761 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrIOTimingsEndCbk, defrIOTimingsEndCbkType, 0); }
#line 5026 "def.tab.c" /* yacc.c:1646  */
    break;

  case 501:
#line 1765 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrFPCEndCbk, defrFPCEndCbkType, 0); }
#line 5032 "def.tab.c" /* yacc.c:1646  */
    break;

  case 502:
#line 1769 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrFPCStartCbk, defrFPCStartCbkType,
              ROUND((yyvsp[-1].dval))); }
#line 5039 "def.tab.c" /* yacc.c:1646  */
    break;

  case 504:
#line 1775 "def.y" /* yacc.c:1646  */
    {}
#line 5045 "def.tab.c" /* yacc.c:1646  */
    break;

  case 505:
#line 1778 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1;  }
#line 5051 "def.tab.c" /* yacc.c:1646  */
    break;

  case 506:
#line 1779 "def.y" /* yacc.c:1646  */
    { if (defrFPCCbk) defrFPC.defiFPC::setName((yyvsp[-1].string), (yyvsp[0].string)); }
#line 5057 "def.tab.c" /* yacc.c:1646  */
    break;

  case 507:
#line 1781 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrFPCCbk, defrFPCCbkType, &defrFPC); }
#line 5063 "def.tab.c" /* yacc.c:1646  */
    break;

  case 508:
#line 1785 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"HORIZONTAL"; }
#line 5069 "def.tab.c" /* yacc.c:1646  */
    break;

  case 509:
#line 1787 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"VERTICAL"; }
#line 5075 "def.tab.c" /* yacc.c:1646  */
    break;

  case 510:
#line 1791 "def.y" /* yacc.c:1646  */
    { if (defrFPCCbk) defrFPC.defiFPC::setAlign(); }
#line 5081 "def.tab.c" /* yacc.c:1646  */
    break;

  case 511:
#line 1793 "def.y" /* yacc.c:1646  */
    { if (defrFPCCbk) defrFPC.defiFPC::setMax((yyvsp[0].dval)); }
#line 5087 "def.tab.c" /* yacc.c:1646  */
    break;

  case 512:
#line 1795 "def.y" /* yacc.c:1646  */
    { if (defrFPCCbk) defrFPC.defiFPC::setMin((yyvsp[0].dval)); }
#line 5093 "def.tab.c" /* yacc.c:1646  */
    break;

  case 513:
#line 1797 "def.y" /* yacc.c:1646  */
    { if (defrFPCCbk) defrFPC.defiFPC::setEqual((yyvsp[0].dval)); }
#line 5099 "def.tab.c" /* yacc.c:1646  */
    break;

  case 516:
#line 1805 "def.y" /* yacc.c:1646  */
    { if (defrFPCCbk) defrFPC.defiFPC::setDoingBottomLeft(); }
#line 5105 "def.tab.c" /* yacc.c:1646  */
    break;

  case 518:
#line 1808 "def.y" /* yacc.c:1646  */
    { if (defrFPCCbk) defrFPC.defiFPC::setDoingTopRight(); }
#line 5111 "def.tab.c" /* yacc.c:1646  */
    break;

  case 522:
#line 1816 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1; }
#line 5117 "def.tab.c" /* yacc.c:1646  */
    break;

  case 523:
#line 1817 "def.y" /* yacc.c:1646  */
    { if (defrFPCCbk) defrFPC.defiFPC::addRow((yyvsp[-1].string)); }
#line 5123 "def.tab.c" /* yacc.c:1646  */
    break;

  case 524:
#line 1818 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1; }
#line 5129 "def.tab.c" /* yacc.c:1646  */
    break;

  case 525:
#line 1819 "def.y" /* yacc.c:1646  */
    { if (defrFPCCbk) defrFPC.defiFPC::addComps((yyvsp[-1].string)); }
#line 5135 "def.tab.c" /* yacc.c:1646  */
    break;

  case 527:
#line 1826 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrTimingDisablesStartCbk, defrTimingDisablesStartCbkType,
              ROUND((yyvsp[-1].dval))); }
#line 5142 "def.tab.c" /* yacc.c:1646  */
    break;

  case 529:
#line 1832 "def.y" /* yacc.c:1646  */
    {}
#line 5148 "def.tab.c" /* yacc.c:1646  */
    break;

  case 530:
#line 1835 "def.y" /* yacc.c:1646  */
    { dumb_mode = 2; no_num = 2;  }
#line 5154 "def.tab.c" /* yacc.c:1646  */
    break;

  case 531:
#line 1836 "def.y" /* yacc.c:1646  */
    { dumb_mode = 2; no_num = 2;  }
#line 5160 "def.tab.c" /* yacc.c:1646  */
    break;

  case 532:
#line 1837 "def.y" /* yacc.c:1646  */
    {
              if (defrTimingDisableCbk)
                defrTimingDisable.defiTimingDisable::setFromTo((yyvsp[-6].string), (yyvsp[-5].string), (yyvsp[-2].string), (yyvsp[-1].string));
	      CALLBACK(defrTimingDisableCbk, defrTimingDisableCbkType,
                &defrTimingDisable);
	    }
#line 5171 "def.tab.c" /* yacc.c:1646  */
    break;

  case 533:
#line 1843 "def.y" /* yacc.c:1646  */
    {dumb_mode = 2; no_num = 2; }
#line 5177 "def.tab.c" /* yacc.c:1646  */
    break;

  case 534:
#line 1844 "def.y" /* yacc.c:1646  */
    {
              if (defrTimingDisableCbk)
                defrTimingDisable.defiTimingDisable::setThru((yyvsp[-2].string), (yyvsp[-1].string));
	      CALLBACK(defrTimingDisableCbk, defrTimingDisableCbkType,
                &defrTimingDisable);
            }
#line 5188 "def.tab.c" /* yacc.c:1646  */
    break;

  case 535:
#line 1850 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1;}
#line 5194 "def.tab.c" /* yacc.c:1646  */
    break;

  case 536:
#line 1851 "def.y" /* yacc.c:1646  */
    {
              if (defrTimingDisableCbk)
                defrTimingDisable.defiTimingDisable::setMacro((yyvsp[-2].string));
	      CALLBACK(defrTimingDisableCbk, defrTimingDisableCbkType,
                &defrTimingDisable);
            }
#line 5205 "def.tab.c" /* yacc.c:1646  */
    break;

  case 537:
#line 1858 "def.y" /* yacc.c:1646  */
    { if (defrTimingDisableCbk)
                defrTimingDisable.defiTimingDisable::setReentrantPathsFlag();
	    }
#line 5213 "def.tab.c" /* yacc.c:1646  */
    break;

  case 538:
#line 1864 "def.y" /* yacc.c:1646  */
    {dumb_mode = 1; no_num = 1;}
#line 5219 "def.tab.c" /* yacc.c:1646  */
    break;

  case 539:
#line 1865 "def.y" /* yacc.c:1646  */
    {dumb_mode=1; no_num = 1;}
#line 5225 "def.tab.c" /* yacc.c:1646  */
    break;

  case 540:
#line 1866 "def.y" /* yacc.c:1646  */
    {
              if (defrTimingDisableCbk)
                defrTimingDisable.defiTimingDisable::setMacroFromTo((yyvsp[-3].string),(yyvsp[0].string));
            }
#line 5234 "def.tab.c" /* yacc.c:1646  */
    break;

  case 541:
#line 1870 "def.y" /* yacc.c:1646  */
    {dumb_mode=1; no_num = 1;}
#line 5240 "def.tab.c" /* yacc.c:1646  */
    break;

  case 542:
#line 1871 "def.y" /* yacc.c:1646  */
    {
              if (defrTimingDisableCbk)
                defrTimingDisable.defiTimingDisable::setMacroThru((yyvsp[0].string));
            }
#line 5249 "def.tab.c" /* yacc.c:1646  */
    break;

  case 543:
#line 1878 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrTimingDisablesEndCbk, defrTimingDisablesEndCbkType, 0); }
#line 5255 "def.tab.c" /* yacc.c:1646  */
    break;

  case 545:
#line 1886 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPartitionsStartCbk, defrPartitionsStartCbkType,
              ROUND((yyvsp[-1].dval))); }
#line 5262 "def.tab.c" /* yacc.c:1646  */
    break;

  case 547:
#line 1892 "def.y" /* yacc.c:1646  */
    {}
#line 5268 "def.tab.c" /* yacc.c:1646  */
    break;

  case 548:
#line 1896 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPartitionCbk, defrPartitionCbkType,
              &defrPartition); }
#line 5275 "def.tab.c" /* yacc.c:1646  */
    break;

  case 549:
#line 1900 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 5281 "def.tab.c" /* yacc.c:1646  */
    break;

  case 550:
#line 1901 "def.y" /* yacc.c:1646  */
    {
            if (defrPartitionCbk)
              defrPartition.defiPartition::setName((yyvsp[-1].string));
            }
#line 5290 "def.tab.c" /* yacc.c:1646  */
    break;

  case 552:
#line 1909 "def.y" /* yacc.c:1646  */
    {
              if (defrPartitionCbk)
                defrPartition.defiPartition::addTurnOff((yyvsp[-1].string), (yyvsp[0].string));
            }
#line 5299 "def.tab.c" /* yacc.c:1646  */
    break;

  case 553:
#line 1916 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)" "; }
#line 5305 "def.tab.c" /* yacc.c:1646  */
    break;

  case 554:
#line 1918 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"R"; }
#line 5311 "def.tab.c" /* yacc.c:1646  */
    break;

  case 555:
#line 1920 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"F"; }
#line 5317 "def.tab.c" /* yacc.c:1646  */
    break;

  case 556:
#line 1924 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)" "; }
#line 5323 "def.tab.c" /* yacc.c:1646  */
    break;

  case 557:
#line 1926 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"R"; }
#line 5329 "def.tab.c" /* yacc.c:1646  */
    break;

  case 558:
#line 1928 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"F"; }
#line 5335 "def.tab.c" /* yacc.c:1646  */
    break;

  case 561:
#line 1935 "def.y" /* yacc.c:1646  */
    {dumb_mode=2; no_num = 2;}
#line 5341 "def.tab.c" /* yacc.c:1646  */
    break;

  case 562:
#line 1937 "def.y" /* yacc.c:1646  */
    {
              if (defrPartitionCbk)
                defrPartition.defiPartition::setFromClockPin((yyvsp[-3].string), (yyvsp[-2].string));
            }
#line 5350 "def.tab.c" /* yacc.c:1646  */
    break;

  case 563:
#line 1941 "def.y" /* yacc.c:1646  */
    {dumb_mode=2; no_num = 2; }
#line 5356 "def.tab.c" /* yacc.c:1646  */
    break;

  case 564:
#line 1943 "def.y" /* yacc.c:1646  */
    {
              if (defrPartitionCbk)
                defrPartition.defiPartition::setFromCompPin((yyvsp[-2].string), (yyvsp[-1].string));
            }
#line 5365 "def.tab.c" /* yacc.c:1646  */
    break;

  case 565:
#line 1947 "def.y" /* yacc.c:1646  */
    {dumb_mode=1; no_num = 1; }
#line 5371 "def.tab.c" /* yacc.c:1646  */
    break;

  case 566:
#line 1949 "def.y" /* yacc.c:1646  */
    {
              if (defrPartitionCbk)
                defrPartition.defiPartition::setFromIOPin((yyvsp[-1].string));
            }
#line 5380 "def.tab.c" /* yacc.c:1646  */
    break;

  case 567:
#line 1953 "def.y" /* yacc.c:1646  */
    {dumb_mode=2; no_num = 2; }
#line 5386 "def.tab.c" /* yacc.c:1646  */
    break;

  case 568:
#line 1955 "def.y" /* yacc.c:1646  */
    {
              if (defrPartitionCbk)
                defrPartition.defiPartition::setToClockPin((yyvsp[-3].string), (yyvsp[-2].string));
            }
#line 5395 "def.tab.c" /* yacc.c:1646  */
    break;

  case 569:
#line 1959 "def.y" /* yacc.c:1646  */
    {dumb_mode=2; no_num = 2; }
#line 5401 "def.tab.c" /* yacc.c:1646  */
    break;

  case 570:
#line 1961 "def.y" /* yacc.c:1646  */
    {
              if (defrPartitionCbk)
                defrPartition.defiPartition::setToCompPin((yyvsp[-2].string), (yyvsp[-1].string));
            }
#line 5410 "def.tab.c" /* yacc.c:1646  */
    break;

  case 571:
#line 1965 "def.y" /* yacc.c:1646  */
    {dumb_mode=1; no_num = 2; }
#line 5416 "def.tab.c" /* yacc.c:1646  */
    break;

  case 572:
#line 1966 "def.y" /* yacc.c:1646  */
    {
              if (defrPartitionCbk)
                defrPartition.defiPartition::setToIOPin((yyvsp[-1].string));
            }
#line 5425 "def.tab.c" /* yacc.c:1646  */
    break;

  case 573:
#line 1971 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPartitionsExtCbk, defrPartitionsExtCbkType,
              History_text); }
#line 5432 "def.tab.c" /* yacc.c:1646  */
    break;

  case 574:
#line 1976 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1000000000; no_num = 10000000; }
#line 5438 "def.tab.c" /* yacc.c:1646  */
    break;

  case 575:
#line 1977 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; no_num = 0; }
#line 5444 "def.tab.c" /* yacc.c:1646  */
    break;

  case 577:
#line 1982 "def.y" /* yacc.c:1646  */
    {}
#line 5450 "def.tab.c" /* yacc.c:1646  */
    break;

  case 578:
#line 1986 "def.y" /* yacc.c:1646  */
    {
              if (defrPartitionCbk)
                defrPartition.defiPartition::setMin((yyvsp[-1].dval), (yyvsp[0].dval));
            }
#line 5459 "def.tab.c" /* yacc.c:1646  */
    break;

  case 579:
#line 1991 "def.y" /* yacc.c:1646  */
    {
              if (defrPartitionCbk)
                defrPartition.defiPartition::setMax((yyvsp[-1].dval), (yyvsp[0].dval));
            }
#line 5468 "def.tab.c" /* yacc.c:1646  */
    break;

  case 581:
#line 1999 "def.y" /* yacc.c:1646  */
    { if (defrPartitionCbk) defrPartition.defiPartition::addPin((yyvsp[0].string)); }
#line 5474 "def.tab.c" /* yacc.c:1646  */
    break;

  case 584:
#line 2007 "def.y" /* yacc.c:1646  */
    { if (defrPartitionCbk) defrPartition.defiPartition::addRiseMin((yyvsp[0].dval)); }
#line 5480 "def.tab.c" /* yacc.c:1646  */
    break;

  case 585:
#line 2009 "def.y" /* yacc.c:1646  */
    { if (defrPartitionCbk) defrPartition.defiPartition::addFallMin((yyvsp[0].dval)); }
#line 5486 "def.tab.c" /* yacc.c:1646  */
    break;

  case 586:
#line 2011 "def.y" /* yacc.c:1646  */
    { if (defrPartitionCbk) defrPartition.defiPartition::addRiseMax((yyvsp[0].dval)); }
#line 5492 "def.tab.c" /* yacc.c:1646  */
    break;

  case 587:
#line 2013 "def.y" /* yacc.c:1646  */
    { if (defrPartitionCbk) defrPartition.defiPartition::addFallMax((yyvsp[0].dval)); }
#line 5498 "def.tab.c" /* yacc.c:1646  */
    break;

  case 590:
#line 2022 "def.y" /* yacc.c:1646  */
    { if (defrPartitionCbk)
	    defrPartition.defiPartition::addRiseMinRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 5505 "def.tab.c" /* yacc.c:1646  */
    break;

  case 591:
#line 2025 "def.y" /* yacc.c:1646  */
    { if (defrPartitionCbk)
	    defrPartition.defiPartition::addFallMinRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 5512 "def.tab.c" /* yacc.c:1646  */
    break;

  case 592:
#line 2028 "def.y" /* yacc.c:1646  */
    { if (defrPartitionCbk)
	    defrPartition.defiPartition::addRiseMaxRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 5519 "def.tab.c" /* yacc.c:1646  */
    break;

  case 593:
#line 2031 "def.y" /* yacc.c:1646  */
    { if (defrPartitionCbk)
	    defrPartition.defiPartition::addFallMaxRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 5526 "def.tab.c" /* yacc.c:1646  */
    break;

  case 594:
#line 2036 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPartitionsEndCbk, defrPartitionsEndCbkType, 0); }
#line 5532 "def.tab.c" /* yacc.c:1646  */
    break;

  case 596:
#line 2042 "def.y" /* yacc.c:1646  */
    {}
#line 5538 "def.tab.c" /* yacc.c:1646  */
    break;

  case 597:
#line 2045 "def.y" /* yacc.c:1646  */
    {dumb_mode=2; no_num = 2; }
#line 5544 "def.tab.c" /* yacc.c:1646  */
    break;

  case 598:
#line 2047 "def.y" /* yacc.c:1646  */
    {
            /* note that the first T_STRING could be the keyword VPIN */
	    if (defrNetCbk) {
	      defrSubnet->defiSubnet::addPin((yyvsp[-3].string), (yyvsp[-2].string), (yyvsp[-1].integer));
	    }
	  }
#line 5555 "def.tab.c" /* yacc.c:1646  */
    break;

  case 599:
#line 2056 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 0; }
#line 5561 "def.tab.c" /* yacc.c:1646  */
    break;

  case 600:
#line 2058 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 1; }
#line 5567 "def.tab.c" /* yacc.c:1646  */
    break;

  case 603:
#line 2066 "def.y" /* yacc.c:1646  */
    {  
            if (defrNetCbk) defrSubnet->defiSubnet::setType((yyvsp[-1].string));
            new_is_keyword = FALSE;
            }
#line 5576 "def.tab.c" /* yacc.c:1646  */
    break;

  case 604:
#line 2070 "def.y" /* yacc.c:1646  */
    { dumb_mode = 1; no_num = 1; }
#line 5582 "def.tab.c" /* yacc.c:1646  */
    break;

  case 605:
#line 2071 "def.y" /* yacc.c:1646  */
    { if (defrNetCbk) defrSubnet->defiSubnet::setNonDefault((yyvsp[0].string)); }
#line 5588 "def.tab.c" /* yacc.c:1646  */
    break;

  case 607:
#line 2078 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPinPropStartCbk, defrPinPropStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 5594 "def.tab.c" /* yacc.c:1646  */
    break;

  case 608:
#line 2083 "def.y" /* yacc.c:1646  */
    { }
#line 5600 "def.tab.c" /* yacc.c:1646  */
    break;

  case 609:
#line 2085 "def.y" /* yacc.c:1646  */
    { }
#line 5606 "def.tab.c" /* yacc.c:1646  */
    break;

  case 610:
#line 2089 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPinPropEndCbk, defrPinPropEndCbkType, 0); }
#line 5612 "def.tab.c" /* yacc.c:1646  */
    break;

  case 613:
#line 2096 "def.y" /* yacc.c:1646  */
    { dumb_mode = 2; no_num = 2; }
#line 5618 "def.tab.c" /* yacc.c:1646  */
    break;

  case 614:
#line 2097 "def.y" /* yacc.c:1646  */
    { if (defrPinPropCbk) defrPinProp.defiPinProp::setName((yyvsp[-1].string), (yyvsp[0].string)); }
#line 5624 "def.tab.c" /* yacc.c:1646  */
    break;

  case 615:
#line 2099 "def.y" /* yacc.c:1646  */
    { CALLBACK(defrPinPropCbk, defrPinPropCbkType, &defrPinProp);
      // reset the property number
      defrPinProp.defiPinProp::clear();
    }
#line 5633 "def.tab.c" /* yacc.c:1646  */
    break;

  case 618:
#line 2109 "def.y" /* yacc.c:1646  */
    { dumb_mode = 10000000; }
#line 5639 "def.tab.c" /* yacc.c:1646  */
    break;

  case 619:
#line 2111 "def.y" /* yacc.c:1646  */
    { dumb_mode = 0; }
#line 5645 "def.tab.c" /* yacc.c:1646  */
    break;

  case 622:
#line 2119 "def.y" /* yacc.c:1646  */
    { if (defrPinPropCbk) defrPinProp.defiPinProp::addProperty((yyvsp[-1].string), (yyvsp[0].string)); }
#line 5651 "def.tab.c" /* yacc.c:1646  */
    break;


#line 5655 "def.tab.c" /* yacc.c:1646  */
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
#line 2122 "def.y" /* yacc.c:1906  */


