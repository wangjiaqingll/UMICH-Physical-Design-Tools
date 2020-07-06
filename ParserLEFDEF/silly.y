%{
#include <limits.h>
//#include <malloc.h>
#include <stdlib.h>
//#include "DB.h"
#include "Geoms/plGeom.h"
#include "Placement/placement.h"
#include "LEFDEFpoint.h"
#include "parserLEFDEF.h"
//#include "hash.h"
#include "localhash.h"
#include <ABKCommon/sgi_hash_map.h>

using std::pair;
using std::vector;

typedef struct lexsstr
    {
    char *str;
    int  DoDelete;
    } lexemeStruct;

void leferror(char*);
int leflex(void);
int bCaseSens=0;

%}

%union {
    int keyword;
    float fval;
    int ival;
    lexemeStruct lexeme;
    unsigned uval;
    LEFDEFpoint pt;
    void *ignore;
    LEFDEFrectangle rectVal;
}


%token  <lexeme> K_ID

%token  K_ORDBR1
%token  K_ORDBR2
%token  <lexeme> K_RISERS  
%token  <lexeme> K_RISECS         
%token  <lexeme> K_RISESATT1 
%token  <lexeme> K_RISET0         
%token  <lexeme> K_FALLRS   
%token  <lexeme> K_FALLCS   
%token  <lexeme> K_FALLSATT1
%token  <lexeme> K_FALLT0 
%token  <lexeme> K_VARIABLE
%token  <lexeme> K_INTRINSIC
%token  <lexeme> K_SOURCE
%token  <lexeme> K_CASE
%token  <lexeme> K_ON
%token  <lexeme> K_OFF
%token  <lexeme> K_LAYER
%token  <lexeme> K_TYPE
%token  <lexeme> K_ROUTING
%token  <lexeme> K_MSLICE
%token  <lexeme> K_CUT
%token  <lexeme> K_OVERLAP
%token  <lexeme> K_END
%token  <lexeme> K_PITCH
%token  <lexeme> K_WIDTH
%token  <lexeme> K_SPACING
%token  <lexeme> K_DIRECTION
%token  <lexeme> K_RES
%token  <lexeme> K_CAP
%token  <lexeme> K_RPERSQ
%token  <lexeme> K_CPERSQ
%token  <lexeme> K_HEIGHT
%token  <lexeme> K_THICKNESS
%token  <lexeme> K_SHRINKAGE
%token  <lexeme> K_CAPMULTIPLIER
%token  <lexeme> K_EDGECAP
%token  <lexeme> K_HORIZ
%token  <lexeme> K_VERT
%token  <lexeme> K_VIA
%token  <lexeme> K_DEFAULT
%token  <lexeme> K_SITE
%token  <lexeme> K_PAD
%token  <lexeme> K_CORE
%token  <lexeme> K_ORIGIN
%token  <lexeme> K_SYMMETRY
%token  <lexeme> K_X
%token  <lexeme> K_Y
%token  <lexeme> K_R90
%token  <lexeme> K_SIZE
%token  <lexeme> K_BY
%token  <lexeme> K_ARRAY
%token  <lexeme> K_N
%token  <lexeme> K_S
%token  <lexeme> K_W
%token  <lexeme> K_E
%token  <lexeme> K_FN
%token  <lexeme> K_FE
%token  <lexeme> K_FW
%token  <lexeme> K_FS
%token  <lexeme> K_DO
%token  <lexeme> K_STEP
%token  <lexeme> K_CANPLACE
%token  <lexeme> K_CANNOTOCCUPY
%token  <lexeme> K_TRACKS
%token  <lexeme> K_GCELL
%token  <lexeme> K_FLOORPLAN
%token  <lexeme> K_DEFAULTCAP
%token  <lexeme> K_MINPINS
%token  <lexeme> K_WIRECAP
%token  <lexeme> K_MACRO
%token  <lexeme> K_CLASS
%token  <lexeme> K_LEQ
%token  <lexeme> K_EEQ
%token  <lexeme> K_COVER
%token  <lexeme> K_RING
%token  <lexeme> K_BLOCK
%token  <lexeme> K_ENDCAP
%token  <lexeme> K_FEEDTHRU
%token  <lexeme> K_TIEHIGH
%token  <lexeme> K_TIELOW
%token  <lexeme> K_INPUT
%token  <lexeme> K_OUTPUT
%token  <lexeme> K_INOUT
%token  <lexeme> K_POWER
%token  <lexeme> K_SPACER
%token  <lexeme> K_PRE
%token  <lexeme> K_POST
%token  <lexeme> K_TO
%token  <lexeme> K_TOPLEFT
%token  <lexeme> K_TOPRIGHT
%token  <lexeme> K_BOTTOMLEFT
%token  <lexeme> K_BOTTOMRIGHT
%token  <lexeme> K_PIN
%token  <lexeme> K_USE
%token  <lexeme> K_SHAPE
%token  <lexeme> K_SETUPTIME
%token  <lexeme> K_SKIP
%token  <lexeme> K_ABUTMENT
%token  <lexeme> K_SIGNAL
%token  <lexeme> K_GROUND
%token  <lexeme> K_CLOCK
%token  <lexeme> K_TIEOFF
%token  <lexeme> K_ANALOG
%token  <lexeme> K_DATA
%token  <lexeme> K_PORT
%token  <lexeme> K_OBS
%token  <lexeme> K_RECT
%token  <lexeme> K_PATH
%token  <lexeme> K_POLYGON
%token  <lexeme> K_ITERATE
%token  <lexeme> K_TIMING
%token  <lexeme> K_UNATE
%token  <lexeme> K_INVERT
%token  <lexeme> K_NONINVERT
%token  <lexeme> K_NONUNATE
%token  <lexeme> K_RISE
%token  <lexeme> K_FALL
%token  <lexeme> K_TRANSITION
%token  <lexeme> K_DELAY
%token  <lexeme> K_FROMPIN
%token  <lexeme> K_TOPIN
%token  <lexeme> K_LIBRARY
%token  <lexeme> K_DESIGN
%token K_PLUS
%token K_MINUS
%token K_STAR
%token K_ASGN
%token  <lexeme> K_ACTIVE
%token  <lexeme> K_ANTENNAAREAFACTOR
%token  <lexeme> K_ANTENNALENGTHFACTOR
%token  <lexeme> K_ANTENNAMETALAREA
%token  <lexeme> K_ANTENNAMETALLENGTH
%token  <lexeme> K_ANTENNASIZE
%token  <lexeme> K_ANYEDGE
%token  <lexeme> K_CORRECTIONFACTOR
%token  <lexeme> K_CORRECTIONTABLE
%token  <lexeme> K_CURRENT
%token  <lexeme> K_CURRENTSOURCE
%token  <lexeme> K_DATABASE
%token  <lexeme> K_DIELECTRIC
%token  <lexeme> K_EDGERATE
%token  <lexeme> K_EDGERATETHRESHOLD1
%token  <lexeme> K_EDGERATETHRESHOLD2
%token  <lexeme> K_EDGERATESCALEFACTOR
%token  <lexeme> K_FALLTHRESH
%token  <lexeme> K_FALLSATCUR
%token <lexeme>  K_FALLSLEWLIMIT
%token  <lexeme> K_FALLVOLTAGETHRESHOLD
%token  <lexeme> K_FOREIGN
%token  <lexeme> K_FUNCTION
%token  <lexeme> K_GENERATE
%token  <lexeme> K_HOLD
%token  <lexeme> K_INOUTPINANTENNASIZE
%token  <lexeme> K_INPUTPINANTENNASIZE
%token  <lexeme> K_INPUTNOISEMARGIN
%token  <lexeme> K_INTEGER
%token  <lexeme> K_IRDROP
%token  <lexeme> K_IV_TABLES
%token  <lexeme> K_LEAKAGE
%token  <lexeme> K_MAXDELAY
%token <lexeme>  K_MAXLOAD 
%token  <lexeme> K_METALOVERHANG
%token  <lexeme> K_MICRONS
%token  <lexeme> K_MILLIAMPS
%token  <lexeme> K_MILLIWATTS
%token  <lexeme> K_MINFEATURE
%token  <lexeme> K_MUSTJOIN
%token  <lexeme> K_NANOSECONDS
%token  <lexeme> K_NEGEDGE
%token  <lexeme> K_NOISETABLE
%token  <lexeme> K_NONDEFAULTRULE
%token  <lexeme> K_OHMS
%token  <lexeme> K_OFFSET
%token  <lexeme> K_OUTPUTRESISTANCE
%token  <lexeme> K_OUTPUTPINANTENNASIZE
%token  <lexeme> K_OUTPUTNOISEMARGIN
%token  <lexeme> K_OVERHANG
%token  <lexeme> K_PICOFARADS
%token  <lexeme> K_POSEDGE
%token  <lexeme> K_PROPERTY
%token  <lexeme> K_PROPERTYDEFINITIONS
%token  <lexeme> K_PULLDOWNRES
%token  <lexeme> K_RANGE
%token  <lexeme> K_REAL
%token  <lexeme> K_RESISTIVE
%token  <lexeme> K_RISETHRESH
%token  <lexeme> K_RISESATCUR
%token <lexeme>  K_RISESLEWLIMIT 
%token  <lexeme> K_RISEVOLTAGETHRESHOLD
%token  <lexeme> K_SAMENET
%token  <lexeme> K_SETUP
%token  <lexeme> K_STACK
%token  <lexeme> K_STRING
%token  <lexeme> K_STRUCTURE
%token  <lexeme> K_TABLE
%token  <lexeme> K_TABLEAXIS
%token  <lexeme> K_TABLEENTRIES
%token  <lexeme> K_TABLEDIMENSION
%token  <lexeme> K_TIEOFFR
%token  <lexeme> K_TIME
%token  <lexeme> K_TRANSITIONTIME
%token  <lexeme> K_TRISTATE
%token  <lexeme> K_TOPOFSTACKONLY
%token  <lexeme> K_UNIVERSALNOISEMARGIN
%token  <lexeme> K_USER
%token  <lexeme> K_UNITS
%token  <lexeme> K_VERSION
%token  <lexeme> K_VHI
%token  <lexeme> K_VIARULE
%token  <lexeme> K_VICTIMLENGTH
%token  <lexeme> K_VICTIMNOISE
%token  <lexeme> K_VLO
%token  <lexeme> K_VOLTAGE
%token  <lexeme> K_VOLTS
%token  <lexeme> K_WIREEXTENSION
%token  <lexeme> K_DIVIDERCHAR
%token  <lexeme> K_BUSBITCHARS
%token  K_COMMA
%token  K_SEMCOL
%token  K_DOT_DOT
%token  K_DOT
%token  K_COLON
%token  <fval> NUMBER
%token <lexeme> QUOTED_STR



%%

lef_file
    : K_ID 
{
ParserLEFDEF::regionHashMapType r;

ParserLEFDEF::masterCellHashMapType h;
                    ParserLEFDEF::regionHashMapType::iterator iR =
                                r.find((char const*)(NULL));

    pair<char const* const,dbMasterCell*> const &pref =
        pair<char const* const,dbMasterCell*>(
                                              (const char*)NULL,
                                              (dbMasterCell*)NULL);



    pair<ParserLEFDEF::masterCellHashMapType::iterator,bool> p =
        h.insert(pref);

}

%%
/*
   main () 
   {
   yyparse();
   }
   
   */



void leferror ( char *s) 
{ 
  //printf("%s\n\tIn line %d of LEF\n",s,lefLinenum+1);
  //fprintf(stderr,"%s\n\tIn line %d of LEF\n",s,lefLinenum+1);
  //fflush(stdout);
  //fflush(stderr);
  //exit(-1);
}




