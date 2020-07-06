%{
#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

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
//extern MasterCellHashTable *pMasterTable;
int          NumMasterCells;
//dbMasterRec *currentCellPtr;
//dbMasterPinRec *currentPinPtr;
//dbLayerGeometryList *pLGeomList;
//dbGeometry *pGeom;
//dbLayDirType tempDirection;
float Dummy;
char  DummyString[511];
extern int lefLinenum;
static char errtxt[1023];
static char tempDir[1023];
static char tempUse[1023];
static char tempShape[1023];
static BBox tempBBox;
static char *tempstr;
//static ctainerLayerGeoms emptyLGs;
int bCaseSens=0;
dbLayer *pTempLayer;
dbSite  *pTempSite;

//extern dbUnits   *pUnitsPtr;
//extern dbTimeNetlistRec *pNetlist;
//extern dbWrapper *pWrapperPtr;

extern ParserLEFDEF *pp;
extern DB           *pdb;

///block of globals for dbLayer ctor

static unsigned runningLayerIdx=0;
static dbLayer::Type tempLayerType;
static dbLayer::Dir tempLayerDir;
static unsigned tempPitch;
static unsigned tempLayerWidth;
static unsigned tempLayerHeight;
static unsigned tempThickness;
static unsigned tempSpacing;
static unsigned tempShrinkage;
static double   tempCapMult;
static double   tempEdgeCap;
static double   tempUnitRes;
static double   tempUnitCap;
static char     tempLayerName[1023];
//end dbLayer globals

//block of globals for dbSite ctor
static char         tempSiteName[1023];
static unsigned     tempSiteHeight;
static unsigned     tempSiteWidth;
static dbSite::Type tempSiteType;
static Symmetry     tempSymmetry;
//end dbSite globals

//block of globals for dbMasterCell ctor
static dbMasterCellType::MClass      tempMacroType;
static dbMasterCellType::PadSubClass tempPadClass;
static LEFDEFpoint                   tempCellOrig;
static ctainerMasterPinsLocal        tempPins;
//static ctainerMasterPinsLocal        emptyPins;
static ctainerLayerGeoms             tempObs;
static dbMasterCell                 *pTempCell;
static dbMasterCell                 *pTempEquivCell;
equivClassesMasterCells             *pTempCellQSpace;
static char                          tempCellName[1023];
static unsigned                      tempCellWidth;
static unsigned                      tempCellHeight;
enum EqClassFoundType {EQ_CLASS_FOUND,EQ_CLASS_NOT_FOUND,AMBIG_EQ_FOUND} ;

static EqClassFoundType                                 leqFound,
                                                        eeqFound,
                                                        *pLocalFound;
eqClassMasterCells *pCurrentLEQclass,*pCurrentEEQclass,**ppLocalClass;
static dbMasterCell                  *pLEQCell,*pEEQCell,**ppLocalEqCell;
//end dbMasterCell globals

//block of globals for dbMasterPin ctor
static char tempPinName[1023];
static ctainerLayerGeoms tempPorts;
static dbMasterPin *pTempPin;
static EqClassFoundType pinClassFound;
eqClassMasterPins *pCurrentPinClass;
dbMasterPin       *pLEQpin;
static dbMasterPin                  *pMustJoinPin;
//end dbMasterPin globals

//globals for geometries
static BBox tempLGrectangle;
static BBox tempGeomBoundingBox;
static vector<Point> tempPolygon;
static bool inPolygon; //true means we're looking at a polygon geom;
                       //false means a path

static ctainerLayerGeoms *pLGeomList;
static unsigned tempLGwidth;
//end globals for geometries

//globals for vias
static char              tempViaName[1023];
static double            tempViaResis;
static ctainerLayerGeoms tempViaLGs;
static dbVia            *pTempVia;
//end globals for vias

static bool warnMultSpace=true;


#ifndef BISON_DEB
int lefdebug; //in this case this is useless; it's just so we
              //don't get an error trying to initialize it.
#endif
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
%token  <lexeme> K_LENGTHTHRESHOLD K_USELENGTHTHRESHOLD

%token  K_COMMA
%token  K_SEMCOL
%token  K_DOT_DOT
%token  K_DOT
%token  K_COLON
%token  <fval> NUMBER
%token <lexeme> QUOTED_STR

%type <ival> h_or_v
%type <ival> layer_type_entry
%type <ival> layer_type
%type <lexeme> string
%type <pt> point
%type <ival> distance
%type <rectVal> rect_point
%type <rectVal> rect


%%

lef_file
    : lef_rules
        | lef_rules end_lib_stmt {/*printf("parsed successfully\n")*/};

lef_rules
        :
    | lef_rules rule {/*printf("One rule parsed\n");*/}
;

rule    : layer_stmt {//printf("One layer parsed\n");
                     }
        | version_stmt
        | inant_stmt
        | outant_stmt
        | ioant_stmt
        | busbitchar_stmt
        | dividerchar_stmt
        | propdefs_stmt
        | spacing_stmt
        | macro_stmt
        | lef_via_stmt
        | lef_site_stmt
        | lef_array_stmt
        | viarule_stmt
        | nondefault_stmt
        | case_stmt
        | minfeature_stmt
        | dielectric_stmt
        | irdrop_stmt
        | crosstalk_stmt
        | units_stmt
        ;

end_lib_stmt
    : K_END K_LIBRARY {
                      }
    ;

version_stmt
    : K_VERSION NUMBER K_SEMCOL {}
;

inant_stmt
    : K_INPUTPINANTENNASIZE NUMBER K_SEMCOL {}
;

outant_stmt
    : K_OUTPUTPINANTENNASIZE NUMBER K_SEMCOL {}
;

ioant_stmt
    : K_INOUTPINANTENNASIZE NUMBER K_SEMCOL {}
;

busbitchar_stmt
    : K_BUSBITCHARS QUOTED_STR K_SEMCOL {}
;

dividerchar_stmt
    : K_DIVIDERCHAR QUOTED_STR K_SEMCOL {}
;

propdefs_stmt
    : K_PROPERTYDEFINITIONS propdefs_list K_END K_PROPERTYDEFINITIONS {}
;

propdefs_list
    :
    | propdefs_list propdef_item {}
;

propdef_item
    : object_type string prop_type poss_range poss_prop_val  K_SEMCOL
     {
     if ($2.DoDelete) free ($2.str);
     }
;

object_type
    : K_LIBRARY {}
    | K_VIA     {}
    | K_MACRO   {}
    | K_PIN     {}
;

prop_type
    : K_INTEGER {}
    | K_REAL    {}
    | K_STRING  {}
;

poss_range
    : {}
    | K_RANGE NUMBER NUMBER {}
;

poss_prop_val
    : {}
    | prop_val {}
;

prop_val
    : NUMBER {}
    | QUOTED_STR {}
;

case_stmt
    : begin_case K_ON K_SEMCOL {bCaseSens = 1;}
    | begin_case K_OFF K_SEMCOL {bCaseSens = 0;}
;

units_stmt
    : K_UNITS units_rules end_units {}
;

units_rules
    :
    | units_rules units_rule {}
;

units_rule
    : units_time  {}
    | units_cap   {}
    | units_res   {}
    | units_power {}
    | units_cur   {}
    | units_vlt   {}
    | units_db    {}
;

end_units: K_END K_UNITS {}
;

units_time: K_TIME K_NANOSECONDS NUMBER K_SEMCOL
    {
    //pUnitsPtr->timConv = (int)($3+1e-5);
    }
;

units_cap: K_CAP K_PICOFARADS NUMBER K_SEMCOL
    {
    //pUnitsPtr->capConv = (int)($3+1e-5);
    }
;

units_res: K_RES K_OHMS NUMBER K_SEMCOL
    {
    //pUnitsPtr->resConv = (int)($3+1e-5);
    }
;

units_power: K_POWER K_MILLIWATTS NUMBER K_SEMCOL
    {
    //pUnitsPtr->powConv = (int)($3+1e-5);
    }
;

units_cur: K_CURRENT K_MILLIAMPS NUMBER K_SEMCOL
    {
    //pUnitsPtr->curConv = (int)($3+1e-5);
    }
;

units_vlt: K_VOLTAGE K_VOLTS NUMBER K_SEMCOL
    {
    //pUnitsPtr->vltConv = (int)($3+1e-5);
    }
;

units_db: K_DATABASE K_MICRONS NUMBER K_SEMCOL
    {
    //pUnitsPtr->LEFDistConv = (int)($3+1e-5);
    //pUnitsPtr->DEFDistConv = pUnitsPtr->LEFDistConv;
    pdb->context._units = (int)($3+1e-5);
    }
;

begin_case
        : K_CASE {}
;

minfeature_stmt
        : K_MINFEATURE point K_SEMCOL { pdb->context._minFeatureX = $2.x;
                                        pdb->context._minFeatureY = $2.y; }
;

dielectric_stmt
        : K_DIELECTRIC NUMBER K_SEMCOL {} ;

irdrop_stmt
        : K_IRDROP irtable_attr_list K_END K_IRDROP {}
;

irtable_attr_list
        : 
        | irtable_attr_list irtable_attr
;

irtable_attr
        : K_TABLE string point_list K_SEMCOL {if ($2.DoDelete) free($2.str)}
;


/* MACRO STATEMENT BEGINS */

macro_stmt
    : macro_id macro_body_list end_macro_id
    {
    //if (currentCellPtr->getMacroType() == MacroTypeCORE)
    //    currentCellPtr->SetType(TypeCC); // May be changed to TypeSC
    pdb->design._cellLib.push_back(pTempCell =
                 new dbMasterCell(tempCellName,
                                  tempMacroType,
                                  tempPadClass, //meaningful only if pad
                                  tempCellWidth,
                                  tempCellHeight,
                                  tempCellOrig,
                                  tempPins,
                                  tempObs,
                                  tempSymmetry,
                                  *pTempSite) );

    ctainerMasterPinsLocal::iterator iP;

    itLayerGeom iLG;

    for (iP=tempPins.begin();iP!=tempPins.end();iP++)
        {
        (*iP)->_cell = pTempCell;

        //shift all port locations by the origin
        ctainerLayerGeoms &LGs = (*iP)->_ports;

        for (iLG=LGs.begin();iLG!=LGs.end();iLG++)
            {
            (*iLG)->_rect.TranslateBy(tempCellOrig);
            }
        }

    // now shift obstacles by origin
    for (iLG=tempObs.begin();iLG!=tempObs.end();iLG++)
        {
        (*iLG)->_rect.TranslateBy(tempCellOrig);
        }


    pair<char const* const,dbMasterCell*> const &pref =
        pair<char const* const,dbMasterCell*>(
                                 tempstr=strdup(tempCellName),
                                 pTempCell) ;
                                 
    pp->stringsToClean.push_back(tempstr);
                                         

    pair<ParserLEFDEF::masterCellHashMapType::iterator,bool> p =
        pp->masterCellHash.insert(pref);

    if (!p.second)
        {
        sprintf(errtxt,"duplicate declaration of master cell %s "
                  "in line %d of LEF; ignored",tempCellName,lefLinenum);
        abkwarn(0,errtxt);
        pdb->design._cellLib.pop_back();
        delete pTempCell;
        for (unsigned idx=0;idx<tempPins.size();idx++) delete tempPins[idx];
        tempPins.clear();
        break;
        }

    // now we can add the pins to the _pinLib; search for
    // "b3nrpqe" in comments to see what I'm talking about.

    for (unsigned idx=0;idx<tempPins.size();idx++) 
        pdb->design._pinLib.push_back(tempPins[idx]);

   switch (leqFound)
       {
       case EQ_CLASS_FOUND:
           pCurrentLEQclass->push_back(pTempCell);
           break;
       case EQ_CLASS_NOT_FOUND:
           {
           eqClassMasterCells newClass;
           newClass.push_back(pLEQCell);
           newClass.push_back(pTempCell);
           pp->tempCellLEQs.push_back(newClass);
           break;
           }
       //note: if no leq was specified, the value of leqFound will
       //      be AMBIG_EQ_FOUND, so neither branch taken.
       case AMBIG_EQ_FOUND:
           break; //satisfy compiler
       }

   switch (eeqFound)
       {
       case EQ_CLASS_FOUND:
           pCurrentEEQclass->push_back(pTempCell);
           break;
       case EQ_CLASS_NOT_FOUND:
           {
           eqClassMasterCells newClass;
           newClass.push_back(pEEQCell);
           newClass.push_back(pTempCell);
           pp->tempCellEEQs.push_back(newClass);
           break;
           }
       //note: if no eeq was specified, the value of eeqFound will
       //      be AMBIG_EQ_FOUND, so neither branch taken.
       case AMBIG_EQ_FOUND:
           break; //satisfy compiler
       }
   ;
}
;

macro_id
    : K_MACRO string
    {
    strcpy(tempCellName,$2.str);
    tempMacroType = dbMasterCellType::CORE;  //the default value
    tempPadClass = dbMasterCellType::PadUNKNOWN;
    tempCellWidth = tempCellHeight =  unsigned(-1);
    tempCellOrig.x = tempCellOrig.y = 0;
    tempPins.clear();
    tempObs.clear();
    pTempSite = NULL;
    pTempCell = NULL;
    leqFound = eeqFound = AMBIG_EQ_FOUND;
    tempSymmetry.x = tempSymmetry.y = tempSymmetry.rot90 = false;
    pCurrentLEQclass=pCurrentEEQclass = NULL;
    pLEQCell = pEEQCell = NULL;
    ppLocalClass  = NULL;
    ppLocalEqCell = NULL;
    pLocalFound   = NULL;
    if ($2.DoDelete) free($2.str);
    }
;


macro_body_list
    : 
    | macro_body_list macro_body
    ;

macro_body
    : K_CLASS macro_class K_SEMCOL {}
        | K_FOREIGN string point_orient_opt K_SEMCOL {if ($2.DoDelete) free($2.str)}
        | K_SOURCE source_class K_SEMCOL {}
    | leq_or_eeq
    | K_ORIGIN point K_SEMCOL {tempCellOrig=$2}
    | macro_size{}
    | symmetry
    | site_id K_SEMCOL
    | site_entry
    | pin_stmt
    | obs_stmt
    | timing_stmt
    | K_POWER NUMBER K_SEMCOL {}
    | property_assignment_list {}
    | K_FUNCTION string K_SEMCOL {} // the only value I know for "string"
                                    // is BUFFER -- what are the others?
                                    // 1-7-00 mro   
;

macro_size
    :K_SIZE distance K_BY distance K_SEMCOL
        {
        tempCellWidth = $2;
        tempCellHeight= $4;
        }
;

macro_class
    : K_COVER {tempMacroType = dbMasterCellType::COVER}
    | K_RING  {tempMacroType = dbMasterCellType::RING}
    | K_BLOCK {tempMacroType = dbMasterCellType::BLOCK}
    | K_CORE macro_core_subclass 
          {tempMacroType = dbMasterCellType::CORE;
           if ($1.DoDelete) free ($1.str);/*part of kludge*/}
    | K_PAD macro_pad_subclass 
          {tempMacroType = dbMasterCellType::PAD}
    | K_ENDCAP macro_endcap_subclass 
          {tempMacroType = dbMasterCellType::ENDCAP}
;

macro_core_subclass
    :
    | K_FEEDTHRU   {}
    | K_TIEHIGH    {}
    | K_TIELOW     {}
;

macro_pad_subclass
    : {} //empty
    | K_INPUT  {tempPadClass = dbMasterCellType::INPUT;
      if ($1.DoDelete) free ($1.str);/*part of kludge*/}
    | K_OUTPUT {tempPadClass = dbMasterCellType::OUTPUT;
      if ($1.DoDelete) free ($1.str);/*part of kludge*/}
    | K_INOUT  {tempPadClass = dbMasterCellType::INOUT}
    | K_POWER  {tempPadClass = dbMasterCellType::POWER}
    | K_SPACER {tempPadClass = dbMasterCellType::SPACER}
;

macro_endcap_subclass
    : K_PRE   {}
    | K_POST  {}
    | macro_endcap_subclass_side {}
;

macro_endcap_subclass_side
    : K_TOPLEFT  {}
    | K_TOPRIGHT {}
    | K_BOTTOMLEFT  {}
    | K_BOTTOMRIGHT {}
;

source_class
        : K_USER     {}
        | K_GENERATE {}
        | K_BLOCK    {}
;

loeeq_start
    : K_LEQ {
            pTempCellQSpace = &(pp->tempCellLEQs);
            ppLocalClass = &pCurrentLEQclass;
            pLocalFound  = &leqFound;
            ppLocalEqCell = &pLEQCell;
            }
    | K_EEQ {
            pTempCellQSpace = &(pp->tempCellEEQs);
            ppLocalClass = &pCurrentEEQclass;
            pLocalFound  = &eeqFound;
            ppLocalEqCell = &pEEQCell;
            }
;

leq_or_eeq
    : loeeq_start loeeq_string  K_SEMCOL   
    {
    bool found=false;
    equivClassesMasterCells::iterator iEC;
    eqClassMasterCells::iterator iMC;
    for (iEC=pTempCellQSpace->begin();
         iEC!=pTempCellQSpace->end();iEC++)
        {
        for (iMC = (*iEC).begin();iMC!=(*iEC).end();iMC++)
            {
            if ((*iMC) == pTempEquivCell )
                {
                found = true;
                break;
                }
            }
        if (found)
            break;
        }

    // At this point we know where the new cell will go:
    // if found is true, then pTempCell will be added to
    // (*iEC); otherwise we'll make a new vector containing
    // pTempEquivCell and pTempCell and push it back on pTempCellQSpace.

    // Unfortunately pTempCell does not yet exist.  So we've got
    // to save this info to be used after construction.

    if (found)
        {
        *ppLocalClass = &(*iEC);
        *pLocalFound = EQ_CLASS_FOUND;
        }
    else
        {
        *ppLocalEqCell = pTempEquivCell;
        *pLocalFound = EQ_CLASS_NOT_FOUND;
        }
    }
;

loeeq_string : string
    {
    ParserLEFDEF::masterCellHashMapType::iterator iC 
                 = pp->masterCellHash.find($1.str);
    if (iC == pp->masterCellHash.end())
        {
        sprintf(errtxt,"Master Cell %s not found in line %d "
                       "of LEF file",$1.str,lefLinenum);
        abkfatal(0,errtxt);
        }
     else
        pTempEquivCell = (*iC).second;

    if ($1.DoDelete) free ($1.str);
    }
;

pin_stmt
    : pin_id pin_attr_list end_pin_id  
        {
        if (tempPorts.size() == 0)
            {
            //sprintf(errtxt,"In line %d of LEF file, pin %s "
            //               "of cell %s has no port geometries; ignored",
            //               lefLinenum,tempPinName,tempCellName);
            //abkwarn(tempPorts.size(),errtxt);
            //break;
            tempGeomBoundingBox.add(0,0); //work-around
            }
        if (tempGeomBoundingBox.isEmpty())
            {
            sprintf(errtxt,"In line %d of LEF file, pin %s "
                           "of cell %s has empty bounding box; adding "
                           "a point to prevent crash",lefLinenum,tempPinName,
                           tempCellName);
            abkwarn(0,errtxt);
            tempGeomBoundingBox.add(0,0); //work-around
            }

        //pdb->design._pinLib.push_back(pTempPin =
        //line above had to be removed; we can't afford to add
        //this pin to the _pinLib until we know if the master cell
        //is going to stay. comment reference string "b3nrpqe"

        tempPins.push_back(pTempPin =
             new dbMasterPin(tempPinName,
                              tempDir,
                              tempUse,
                              tempShape,
                              tempGeomBoundingBox,
                              tempGeomBoundingBox.getGeomCenter()
                              )
                              );
        pTempPin->_riseDelay = pTempPin->_fallDelay = pTempPin->_riseResist
        = pTempPin->_fallResist = pTempPin->_inputCap = -1.0;

        pTempPin->_ports = tempPorts;

        if (pMustJoinPin)
            {
            if (pMustJoinPin->_mustJoin)
                {
                pMustJoinPin->_mustJoin->push_back(pTempPin);
                }
            else
                {
                ctainerMasterPinsLocal *pmj = new ctainerMasterPinsLocal;
                pmj->push_back(pMustJoinPin);
                pmj->push_back(pTempPin);
                pMustJoinPin->_mustJoin = pmj;
                pTempPin->_mustJoin = pmj;
                }
            }

        switch(pinClassFound)
            {
            case EQ_CLASS_FOUND:
                pCurrentPinClass->push_back(pTempPin);
                break;
            case EQ_CLASS_NOT_FOUND:
                {
                eqClassMasterPins newClass;
                newClass.push_back(pLEQpin);
                newClass.push_back(pTempPin);
                pdb->design._pinLEQs.push_back(newClass);
                break;
                }

            //note: if no LEQ was specified at all, we get
            //      the value AMBIG_EQ_FOUND for pinClassFound,
            //      so neither branch taken
            case AMBIG_EQ_FOUND:
                break; //satisfy compiler
            }
        }
;

pin_id
    : K_PIN string
    {
    strcpy(tempPinName,$2.str);
    tempDir[0] = tempUse[0] = tempShape[0] = '\0';
    tempBBox.clear();
    tempPorts.clear();
    tempGeomBoundingBox.clear();
    tempPolygon.clear();
    pinClassFound = AMBIG_EQ_FOUND;
    pCurrentPinClass=NULL;
    pLEQpin = NULL;
    pMustJoinPin = NULL;
    //currentPinPtr = new dbMasterPinRec;
    //if (!currentPinPtr) leferror("Failed to allocate pin");
    //currentPinPtr->setName($<lexeme>2.str);

    if ($2.DoDelete) free($<lexeme>2.str);

    //currentCellPtr->m_pPinTable->AddPin(currentPinPtr);
    //currentCellPtr->IncrementNumPins();
    }
;

end_pin_id
    : K_END string  {
                     //LOOK! check that name is the same
                     if ($2.DoDelete) free($2.str)
                     }
;

pin_attr_list
    :
    | pin_attr_list pin_attr {}
;

pin_attr
        : K_FOREIGN string K_SEMCOL    {if ($2.DoDelete) free($2.str)}
        | K_FOREIGN string K_STRUCTURE point_orient_opt K_SEMCOL {if ($2.DoDelete) free($2.str)}
    | K_LEQ string K_SEMCOL
     {
     //we want to search tempPins *only* for the equivalent pin
     ctainerMasterPinsLocal::const_iterator iPeq;
     bool nameFound = false;
     for (iPeq=tempPins.begin();iPeq!=tempPins.end();iPeq++)
         {
         if (strcmp((*iPeq)->getName(),$2.str)==0)
             {
             nameFound = true;
             break;
             }
         }

     if (!nameFound)
         {
         sprintf(errtxt,"Pin %s, allegedly equivalent to pin %s,"
                        " not found in cell %s in line %d of LEF",
                        $2.str,tempPinName,tempCellName,lefLinenum);
         abkfatal(0,errtxt);
         }

    // now we've found the equivalent pin; we need to see if
    // it's already got an equiv class
    typedef equivClassesMasterPins  &rQSpace;
    rQSpace LEQs(pdb->design._pinLEQs);

    bool classFound = false;
    equivClassesMasterPins::iterator iPC;
    eqClassMasterPins::const_iterator iP;
    
    for (iPC=LEQs.begin();iPC!=LEQs.end();iPC++)
        {
        for (iP=(*iPC).begin();iP!=(*iPC).end();iP++)
            {
            if ((*iP) == (*iPeq))
                {
                classFound = true;
                break;
                } 
            }
        if (classFound)
            break;
        }

    if (classFound)
        {
        pCurrentPinClass = &(*iPC);
        pinClassFound = EQ_CLASS_FOUND;
        }
    else
        {
        pLEQpin=(*iPeq);
        pinClassFound = EQ_CLASS_NOT_FOUND;
        }

     if ($2.DoDelete) free($2.str);
     }
    | direction pin_direction K_SEMCOL {}
    | K_USE net_pin_usage K_SEMCOL {}
    | K_SHAPE pin_shape K_SEMCOL  {}
        | K_MUSTJOIN string K_SEMCOL   
            {
             //we want to search tempPins *only* for the mustjoin pin
             ctainerMasterPinsLocal::const_iterator iPmj;
             bool nameFound = false;
             for (iPmj=tempPins.begin();iPmj!=tempPins.end();iPmj++)
                 {
                 if (strcmp((*iPmj)->getName(),$2.str)==0)
                     {
                     nameFound = true;
                     pMustJoinPin = *iPmj;
                     break;
                     }
                 }

             if (!nameFound)
                 {
                 sprintf(errtxt,"Pin %s, which allegedly must join pin %s,"
                                " not found in cell %s in line %d of LEF",
                                $2.str,tempPinName,tempCellName,lefLinenum);
                 abkfatal(0,errtxt);
                 }
//            tempMustJoin = true;
            if ($2.DoDelete) free($2.str);
            }
        | K_OUTPUTNOISEMARGIN string K_SEMCOL {if ($2.DoDelete) free($2.str)}
        | K_OUTPUTRESISTANCE string K_SEMCOL  {if ($2.DoDelete) free($2.str)}
        | K_INPUTNOISEMARGIN string K_SEMCOL {if ($2.DoDelete) free($2.str)}
    | K_POWER NUMBER K_SEMCOL    {}
        | K_LEAKAGE NUMBER K_SEMCOL  {}
    | K_CAP NUMBER K_SEMCOL   {}
    | K_RES NUMBER K_SEMCOL   {}
        | K_PULLDOWNRES NUMBER K_SEMCOL  {}
        | K_TIEOFFR NUMBER K_SEMCOL        {}
        | K_VHI NUMBER K_SEMCOL              {}
        | K_VLO NUMBER K_SEMCOL                {}
        | K_RISETHRESH NUMBER K_SEMCOL           {}
        | K_FALLTHRESH NUMBER K_SEMCOL             {}
        | K_RISESATCUR NUMBER K_SEMCOL               {}
        | K_FALLSATCUR NUMBER K_SEMCOL                 {}
        | K_RISEVOLTAGETHRESHOLD NUMBER K_SEMCOL         {}
        | K_FALLVOLTAGETHRESHOLD NUMBER K_SEMCOL           {}
        | K_MAXDELAY NUMBER K_SEMCOL                         {}
        | K_MAXLOAD NUMBER K_SEMCOL                         {}
        | K_RISESLEWLIMIT NUMBER K_SEMCOL                         {}
        | K_FALLSLEWLIMIT NUMBER K_SEMCOL                         {}
        | K_CURRENTSOURCE a_or_r K_SEMCOL                      {}
        | K_IV_TABLES NUMBER NUMBER K_SEMCOL {}                  {}
    | K_SETUPTIME NUMBER K_SEMCOL              {}
    | port                                       {}
    | property_assignment_list {}  
    | K_ANTENNASIZE distance poss_layer_id_existing K_SEMCOL
         {/*LOOK! "distance" is a guess*/}  
    | K_ANTENNAMETALAREA NUMBER poss_layer_id_existing K_SEMCOL
         {/*LOOK! units are sq microns, irresp of UNITS stmt*/}  
    | K_ANTENNAMETALLENGTH NUMBER poss_layer_id_existing K_SEMCOL
         {/*LOOK! units are microns, irresp of UNITS stmt*/}  
;

direction
        :
        | K_DIRECTION  {}
;

pin_direction
    : K_INPUT {strcpy(tempDir,"INPUT");
      if ($1.DoDelete) free ($1.str);/*part of kludge*/}
    | K_INOUT {strcpy(tempDir,"INOUT");}
    | K_OUTPUT {strcpy(tempDir,"OUTPUT");
      if ($1.DoDelete) free ($1.str);/*part of kludge*/}
    | K_OUTPUT K_TRISTATE {strcpy(tempDir,"OUTPUT TRISTATE");
      if ($1.DoDelete) free ($1.str);/*part of kludge*/}
;

pin_shape
    : K_ABUTMENT {strcpy(tempShape,"ABUTMENT");}
    | K_RING       {strcpy(tempShape,"RING");}
    | K_FEEDTHRU     {strcpy(tempShape,"FEEDTHRU");}
;


net_pin_usage
    : K_SIGNAL  {strcpy(tempUse,"SIGNAL");}
    | K_POWER     {strcpy(tempUse,"POWER");}
    | K_GROUND      {strcpy(tempUse,"GROUND");}
    | K_CLOCK         {strcpy(tempUse,"CLOCK");}
    | K_TIEOFF          {strcpy(tempUse,"TIEOFF");}
    | K_ANALOG            {strcpy(tempUse,"ANALOG");}
    | K_DATA                {strcpy(tempUse,"DATA");}
;

a_or_r  : K_ACTIVE    {}
        | K_RESISTIVE   {}
;

port
    : start_port layer_geom_list K_END  { }
;

start_port : K_PORT {
                    if ($1.DoDelete) free($1.str); //KLUDGE
                    pLGeomList = &tempPorts;
                    /*pLGeomList = &(currentPinPtr->ports);*/
                    pTempLayer=NULL;
                    }
;

obs_stmt
    : K_OBS {pLGeomList = &tempObs;tempGeomBoundingBox.clear();
             tempPolygon.clear();pTempLayer=NULL;} layer_geom_list
            K_END {}
;

layer_geom_list
    : 
    | layer_geom_list layer_geom    {}
;

layer_geom
    : layer
        {
        tempLGwidth = pTempLayer->getWidth();
        }

    | layer_width
        {
        }
     
     | geom 
        {
        //LOOK! the whole "width" thing is unnecessary for us for at the moment.
        //      it only refers to what happens in PATH stmts.
        //      if we decide to parse these at some point, we will need
        //      to use this.

        //OK, these rules have gone through a bunch of iterations.  The
        //current incarnation (ParserLEFDEF4.5.4, 22 Jan 98) is almost
        //absurdly liberal, but that shouldn't really hurt anything.

        //motivation:  Some examples specify a layer before a geometry
        //             given as a via, even though it's meaningless in
        //             that case (AMD1,NEC_HUGE).  Others don't (SANYO).
        //             Others give a layer and then several rectangles,
        //             even though that (like a layer before a via_geom)
        //             is apparently forbidden by LEF manual page 1-53.
        //
        //             I guess the only thing we really need to be
        //             sure of is that, if a geometry is given other
        //             than as a via, a layer has been specified.  Next
        //             check takes care of that, since we set pTempLayer
        //             to NULL before getting here (rules start_port
        //             and obs_stmt).


        if (!pTempLayer)
            {
            sprintf(errtxt,"In line %d of LEF: geometry with undefined layer",
                            lefLinenum);
            abkfatal(0,errtxt);
            }
        if (tempPolygon.empty())
            pLGeomList->push_back(new dbLayerGeom(tempLGrectangle,
                                                      *pTempLayer));
        else
            pLGeomList->push_back(new dbLayerGeom(tempPolygon,
                                                      *pTempLayer));
        }

    | via_geom
        {
        }
;

poss_layer_id_existing
    : /*empty*/ {pTempLayer=NULL;}
    | layer_id_existing
;

layer_id_existing
    : K_LAYER string {
                     strcpy(DummyString,$<lexeme>2.str);
                     ParserLEFDEF::layerHashMapType::iterator iL =
                                                   pp->layerHash.find($2.str);
                     if (iL == pp->layerHash.end())
                         {
                         sprintf(errtxt,"Layer %s not found in line %d "
                                        "of LEF file",$2.str,lefLinenum);
                         abkfatal(0,errtxt);
                         }
                      else
                         pTempLayer = (*iL).second;
                      if ($2.DoDelete) free($2.str);
                      }
;


layer   : layer_id_existing K_SEMCOL  
    {
    }
;

geom
    : rect       {
                 tempLGrectangle = $1;
                 tempGeomBoundingBox.add($1.llx,$1.lly);
                 tempGeomBoundingBox.add($1.urx,$1.ury);
                 }
    | path         {abkwarn(0,"path geometry declared; ignored");}
    | polygon        {}
    | rect_iter        {abkwarn(0,"iterated rectangle geometry declared; ignored");}
    | path_iter          {abkwarn(0,"iterated path geometry declared; ignored");}
    | polygon_iter         {abkwarn(0,"iterated polygon geometry declared; ignored");}
;


layer_width   : K_WIDTH distance K_SEMCOL  {tempLayerWidth=$2;}
;

rect    : K_RECT rect_point K_SEMCOL {$$=$2;}
;

rect_point
    : point point   
       {
       $$.llx = int(min($1.x,$2.x));
       $$.lly = int(min($1.y,$2.y));
       $$.urx = int(max($1.x,$2.x));
       $$.ury = int(max($1.y,$2.y));
       }
;

point   : distance distance  
          {$$.x = $1; $$.y = $2;}
;

path    : K_PATH {inPolygon=false;} point_list K_SEMCOL  {}
;

polygon : K_POLYGON {inPolygon=true;} point_list K_SEMCOL {}
;

rect_iter
    : K_RECT K_ITERATE rect_point lef_step_pattern K_SEMCOL  {}
;

path_iter
    : K_PATH K_ITERATE point_list lef_step_pattern K_SEMCOL {}
;

polygon_iter
    : K_POLYGON K_ITERATE point_list lef_step_pattern K_SEMCOL  {}
;

point_list
    : list_point
    | point_list list_point
;

list_point //LOOK! paths not yet supported 1-12-00 mro
    : point
     {
     if (inPolygon) tempPolygon.push_back($1);
     tempGeomBoundingBox += $1;
     }
;


lef_step_pattern
    : K_DO NUMBER K_BY NUMBER K_STEP distance distance {}
;

via_geom
    : K_VIA point via_id  K_SEMCOL //note: for this to work right,
                                   //it's critical that pLGeomList
                                   //have been set.  This is set in
                                   //the rules start_port and obs_stmt.
                                   //You get to this rule by the sequence
                                   // {port,obs_stmt}/layer_geom_list/
                                   // layer_geom/gen_geom/via_geom.
                                   //If any rules in this path change,
                                   //or are referenced by new rules,
                                   //revisit this.
        {
        if ($1.DoDelete) free ($1.str); //KLUDGE
        typedef ctainerLayerGeoms const &rlg;
        rlg LGref(pTempVia->getGeometries());

        ctainerLayerGeoms::const_iterator iLG;

        for (iLG=LGref.begin();iLG!=LGref.end();iLG++)
            {
            BBox b=*(*iLG);
            b.TranslateBy($2);
            pLGeomList->push_back(new dbLayerGeom(b,(*iLG)->getLayer()));
            tempGeomBoundingBox.add(b.xMin,b.yMin);
            tempGeomBoundingBox.add(b.xMax,b.yMax);
            }
        }
    | K_VIA K_ITERATE point via_id lef_step_pattern K_SEMCOL 
        {
        if ($1.DoDelete) free ($1.str); //KLUDGE
        abkwarn(0,"geometry specified as iterated via; ignored");
        }
;

via_id: string
    {
     ParserLEFDEF::viaHashMapType::iterator iV
                   = pp->viaHash.find($1.str);
     if (iV == pp->viaHash.end())
         {
         sprintf(errtxt,"Via %s not found in line %d "
                        "of LEF file",$1.str,lefLinenum);
         abkfatal(0,errtxt);
         }
     else
         pTempVia = (*iV).second;

     if ($1.DoDelete) free($1.str);

    }
;

timing_stmt
    : K_TIMING timing_attr_list K_END K_TIMING
        {
        }
;

 /*
timing_tbl_stmt
    : K_TIMING timing_tbl_attr_list K_END K_TIMING
    {
    }
*/

timing_attr_list
    :
    | timing_attr_list timing_attr   {}

 /*
timing_tbl_attr_list
    :
    | timing_tbl_attr_list timing_tbl_attr {}
*/
;

timing_attr
    : pin_spec gen_timing_entry_list {}
;

gen_timing_entry_list
    : gen_timing_entry {}
    | gen_timing_entry_list gen_timing_entry {}
;

gen_timing_entry
    : timing_entry {}
    | tbl_timing_entry {}
;


tbl_timing_entry
    :  tim_tbl_prefix 
          tbldim_spec tblax_spec123 tblent_spec {}
;

tblax_spec123
    : tblax_spec
    | tblax_spec tblax_spec
    | tblax_spec tblax_spec tblax_spec 
;

tbldim_spec : K_TABLEDIMENSION NUMBER NUMBER NUMBER K_SEMCOL {}
;

tblax_spec : K_TABLEAXIS number_list K_SEMCOL {}
;

tblent_spec : K_TABLEENTRIES min_typ_max_list K_SEMCOL {}
;

tim_tbl_prefix
    : 
    | r_or_f d_or_t poss_unate {}
    | h_or_s n_or_p_edge n_or_p_edge  {} // This is new and I
                                         // don't know what it means
                                         // mro 1-7-00
;

h_or_s
    : K_HOLD  {}
    | K_SETUP {}
;

n_or_p_edge
    : K_NEGEDGE {}
    | K_POSEDGE {}
    | K_ANYEDGE {}
;

d_or_t
    : K_DELAY {}
    | K_TRANSITIONTIME {}
;


poss_unate
    :
    | unate_spec {}
;


timing_entry
    : unate_spec  K_SEMCOL {}
    | r_or_f K_INTRINSIC min_max slew1_opt K_VARIABLE min_max K_SEMCOL {}
        | K_RISERS min_max K_SEMCOL {}
        | K_FALLRS min_max K_SEMCOL   {}
        | K_RISECS min_max K_SEMCOL     {}
        | K_FALLCS min_max K_SEMCOL       {}
        | K_RISESATT1 min_max K_SEMCOL      {}
        | K_FALLSATT1 min_max K_SEMCOL        {}
        | K_RISET0 min_max K_SEMCOL             {}
        | K_FALLT0 min_max K_SEMCOL               {}
;

unate_spec:K_UNATE unate {}
;

unate
    : K_INVERT {}
    | K_NONINVERT {}
    | K_NONUNATE    {}
    ;

r_or_f
    : K_RISE  {}
    | K_FALL    {}
    ;

min_max
    : NUMBER NUMBER {}
;

slew1_opt
    :
    | NUMBER min_max NUMBER slew2_opt   {}
;

slew2_opt
    :
    | min_max NUMBER {}
;

/*
table_type
    : K_TRANSITION
    | K_DELAY
    ;
*/

pin_spec
    : from_pin_spec to_pin_spec {}
    | from_pin_spec               {}
    | to_pin_spec                   {}
;

number_list
    : NUMBER   {}
    | number_list NUMBER    {}
;

min_typ_max_list
    : min_typ_max
    | min_typ_max_list min_typ_max
    ;

min_typ_max
    : K_ORDBR1 NUMBER NUMBER NUMBER K_ORDBR2
    ;


from_pin_spec
    : K_FROMPIN id_list K_SEMCOL  {}
;

to_pin_spec
    : K_TOPIN id_list K_SEMCOL   {}
;

/*
end_design_stmt
    : K_END K_DESIGN {}
*/

end_macro_id
    : K_END string
    {
    //currentCellPtr->BuildPinArray();
    //NumMasterCells++;
    if ($2.DoDelete) free($2.str)
    }
    ;


/* MACRO STATEMENT ENDS */

/* SPACING STATEMENT BEGINS */


spacing_stmt
    : K_SPACING same_net_attr_list K_END K_SPACING 
                 {
                 }
;


same_net_attr_list
    : 
    | same_net_attr_list same_net_attr    {}
;

same_net_attr
    : K_SAMENET string string  distance stack_opt K_SEMCOL   {if ($2.DoDelete) free($2.str); if ($3.DoDelete) free($3.str);}
;

stack_opt
    :
    | K_STACK{}
;


/* SPACING STATEMENT ENDS */


/* LAYER STATEMENT BEGINS */


layer_stmt
    : layer_id_new layer_body_opt end_layer_id   
       {
       //pTempLayer->direction = tempDirection;
       }
;

layer_body_opt
    :
    | layer_type layer_attr_list {}
;

layer_type
    : K_TYPE layer_type_entry K_SEMCOL {}
;

layer_type_entry
    : K_MSLICE  {tempLayerType = dbLayer::MASTERSLICE}
    | K_CUT     {tempLayerType = dbLayer::CUT}
    | K_OVERLAP {tempLayerType = dbLayer::OVERLAP}
    | K_ROUTING {tempLayerType = dbLayer::ROUTING}
;

layer_attr_list
    : 
    | layer_attr_list layer_attr  {}
;

layer_attr
    : K_PITCH distance K_SEMCOL {tempPitch = $2}
    | K_WIDTH distance K_SEMCOL {tempLayerWidth = $2}
    | K_SPACING distance poss_layer_space_subset K_SEMCOL  {tempSpacing = $2;}
    | K_DIRECTION h_or_v K_SEMCOL {//taken care of in h_or_v rule
                                  }
    | K_RES K_RPERSQ NUMBER K_SEMCOL {tempUnitRes = $3;}
    | K_CAP K_CPERSQ NUMBER K_SEMCOL {tempUnitCap = $3;}
    | K_EDGECAP NUMBER K_SEMCOL  {tempEdgeCap = $2; }
    | K_CAPMULTIPLIER NUMBER K_SEMCOL {tempCapMult = $2;}
    | K_HEIGHT distance K_SEMCOL          {tempLayerHeight = $2;}
    | K_THICKNESS distance K_SEMCOL         {tempThickness = $2;}
    | K_SHRINKAGE distance K_SEMCOL           {tempShrinkage = $2;}
    | K_OFFSET distance K_SEMCOL {} // "distance" is a guess
    | K_ANTENNAAREAFACTOR NUMBER K_SEMCOL {}
    | K_ANTENNALENGTHFACTOR NUMBER K_SEMCOL {}
;

poss_layer_space_subset //if this option is used, dbLayer object
                        //will not take the different spacings into account
                        // (no structure for it)
    : {}
    | layer_space_subset
      {
      if (warnMultSpace)
          {
          warnMultSpace=false;
          abkwarn(0,"Multiple spacing rules for regular wiring in a "
                  "single layer.\nThis is not supported by DB; the "
                  "spacing used will be the last specified.\nLength "
                  "thresholds are also unsupported.");
          }
      }
;

layer_space_subset
    : K_RANGE distance distance poss_ult {}
    | K_LENGTHTHRESHOLD distance {}
;

poss_ult
    : {}
    | K_USELENGTHTHRESHOLD {}
;

h_or_v
    : K_HORIZ {tempLayerDir = dbLayer::HORIZ}
    | K_VERT  {tempLayerDir = dbLayer::VERT}
;


layer_id_new
    : K_LAYER string {
                     strcpy(tempLayerName,$2.str); if ($2.DoDelete) free($2.str);
                     tempLayerDir = dbLayer::NOPREFERRED;
                     tempLayerType= dbLayer::Type(-1) ;
                     tempPitch=tempLayerWidth=tempThickness=tempSpacing
                     =tempShrinkage= tempLayerHeight = unsigned(-1);
                     tempCapMult=tempEdgeCap=tempUnitRes=tempUnitCap=DBL_MAX;
                     }
;

end_layer_id
    : K_END string {
                   //LOOK! need to check names
                   pdb->context._layers.push_back(pTempLayer =
                                                  new dbLayer(
                                                      tempLayerName,
                                                      runningLayerIdx++,
                                                      tempLayerType,
                                                      tempLayerDir,
                                                      tempPitch,
                                                      tempLayerHeight,
                                                      tempLayerWidth,
                                                      tempThickness,
                                                      tempSpacing,
                                                      tempShrinkage,
                                                      tempCapMult,
                                                      tempEdgeCap,
                                                      tempUnitRes,
                                                      tempUnitCap) );

                   pair<char const* const,dbLayer*> const &pref =
                         pair<char const* const,dbLayer*>(
                                                     tempstr=
                                                          strdup(tempLayerName),
                                                     pTempLayer) ;
                                                     
                   pp->stringsToClean.push_back(tempstr);
                                               
                   
                   pair<ParserLEFDEF::layerHashMapType::iterator,bool> p =
                        pp->layerHash.insert(pref);

                   if (!p.second)
                       {
                       sprintf(errtxt,"duplicate declaration of layer %s "
                                      "in line %d of LEF",tempLayerName,lefLinenum);
                       abkfatal(0,errtxt);
                       pdb->context._layers.pop_back();
                       delete pTempLayer;
                       }

                   if ($2.DoDelete) free($2.str);
                   }
;


/* LAYER STATEMENT ENDS */



/*  VIA STATEMENT BEGINS */
lef_via_start: K_VIA string
    {
    if ($1.DoDelete) free ($1.str); //KLUDGE
    tempViaResis = DBL_MAX;
    tempViaLGs.clear();
    pTempLayer   = NULL;
    strcpy(tempViaName,$2.str);
    if ($2.DoDelete) free ($2.str);
    }
;

lef_via_stmt
    : lef_via_start default via_attr_list end_via_id 
           {
           std::sort(tempViaLGs.begin(),tempViaLGs.end(),CompareLayerGeoms());
           pp->tempAllVias.push_back(pTempVia = new dbVia(tempViaName,
                                                          tempViaResis,
                                                          tempViaLGs));
            pair<char const* const,dbVia*> const &pref =
                pair<char const* const,dbVia*>(
                                         tempstr=strdup(tempViaName),
                                         pTempVia) ;
                                 
            pp->stringsToClean.push_back(tempstr);
                                         

            pair<ParserLEFDEF::viaHashMapType::iterator,bool> p =
                pp->viaHash.insert(pref);

            if (!p.second)
                {
                sprintf(errtxt,"duplicate declaration of via %s "
                          "in line %d of LEF.  First declaration"
                          " accepted; subsequent ignored.",tempViaName,lefLinenum);
                abkwarn(0,errtxt);
                pp->tempAllVias.pop_back();
                delete pTempVia;
                }
           }
;

default :
        | K_DEFAULT    {}
;

via_attr_list
    : 
    | via_attr_list via_attr   {}
;

via_attr
    : K_RES NUMBER K_SEMCOL  {tempViaResis=$2}
    | layer rect_list       {}
    | K_FOREIGN string point_orient_opt K_SEMCOL  {if ($2.DoDelete) free($2.str)}
    | property_assignment_list {}
    | K_TOPOFSTACKONLY {}
;

property_assignment_list
    : K_PROPERTY property_name_val_list K_SEMCOL {}
;

property_name_val_list
    : property_name_val                        {}
    | property_name_val_list property_name_val {}
;

property_name_val
    : string prop_val
     {
     if ($1.DoDelete) free ($1.str);
     } //should check that the K_ID is a declared prop name
;

rect_list
    : rect {tempViaLGs.push_back(new dbLayerGeom($1,*pTempLayer,true))}
    | rect_list rect  {tempViaLGs.push_back(new dbLayerGeom($2,*pTempLayer,true))}
;

point_orient_opt
        : 
        | point orient_opt  {}
;

orient_opt
        : 
        | orient  {}
;

end_via_id
    : K_END string{/*LOOK! check names*/
                  if ($2.DoDelete) free($2.str)}
;
    


/* VIA STATEMENT ENDS */

/* VIARULE STATEMENT BEGINS */

viarule_stmt 
        : K_VIARULE string nongenerate_stmt K_END string    {if ($2.DoDelete) free($2.str);if ($5.DoDelete) free ($5.str);}
        | K_VIARULE string K_GENERATE generate_stmt K_END string  {if ($2.DoDelete) free($2.str); if ($6.DoDelete) free($6.str);}
;

nongenerate_stmt
        : viarule_layer_stmt viarule_layer_stmt viarule_via_stmt_list {}
        | viarule_layer_stmt viarule_via_stmt_list{} //LOOK! not in manual
;

viarule_layer_stmt
        : K_LAYER string K_SEMCOL K_DIRECTION h_or_v K_SEMCOL viarule_layer_attr_list {if ($2.DoDelete) free($2.str)}
;

viarule_layer_attr_list
        : 
        | viarule_layer_attr_list viarule_layer_attr {}
;

viarule_layer_attr
        : K_WIDTH distance K_TO distance K_SEMCOL {}
        | K_OVERHANG distance K_SEMCOL            {}
        | K_METALOVERHANG distance K_SEMCOL         {}
;

viarule_via_stmt_list
        :
        | viarule_via_stmt_list viarule_via_stmt  {}
;

viarule_via_stmt
        : K_VIA string K_SEMCOL {
                                if ($1.DoDelete) free($1.str); //KLUDGE
                                if ($2.DoDelete) free($2.str)
                                }
;

generate_stmt
        : viarule_layer_stmt viarule_layer_stmt viarulegen_layer_stmt {}
;

viarulegen_layer_stmt
        :
        | K_LAYER string K_SEMCOL viarulegen_layer_attr_list {if ($2.DoDelete) free($2.str)}
;

viarulegen_layer_attr_list
        : 
        | viarulegen_layer_attr_list viarulegen_layer_attr {}
;

viarulegen_layer_attr
        : rect  {}
        | K_SPACING distance K_BY distance K_SEMCOL {}
        | K_RES NUMBER K_SEMCOL                   {}
;
        
/* VIARULE STATEMENT ENDS */

/* NONDEFAULT STATEMENT BEGINS */

nondefault_stmt
        : K_NONDEFAULTRULE string nondefault_list K_END string    {if ($2.DoDelete) free($2.str); if ($5.DoDelete) free($5.str);}
;

nondefault_list
        :
        | nondefault_list nondefault_element {}
;



nondefault_element
        : nondefault_layer  {}
        | nondefault_via  {}
        | nondefault_spacing  {}
;

nondefault_layer
        : K_LAYER string nondefault_layer_attr_list K_END string  {if ($2.DoDelete) free($2.str); if ($5.DoDelete) free($5.str)}
;

nondefault_layer_attr_list
        :
        |nondefault_layer_attr_list nondefault_layer_attr {}
;

nondefault_layer_attr
        : K_WIDTH distance K_SEMCOL {} 
        | K_SPACING distance K_SEMCOL {}
        | K_WIREEXTENSION distance K_SEMCOL {}
;

nondefault_via
        : K_VIA string via_attr_list K_END string 
             {
             if ($1.DoDelete) free($1.str); //KLUDGE
             if ($2.DoDelete) free($2.str);
             if ($5.DoDelete) free($5.str);
             }
;

nondefault_spacing
        : spacing_stmt  {}
;

/*NONDEFAULT RULE STATEMENT ENDS */

/* CORSSTALK STATEMENT BEGINS */

crosstalk_stmt 
        : noise_thresholds noisetable correctiontable {}
;

noise_thresholds
        : noise threshold_list {}
;

noise   : K_UNIVERSALNOISEMARGIN NUMBER NUMBER K_SEMCOL     {}
;

threshold_list
        : 
        | threshold_list threshold   {}
;

threshold
        : K_EDGERATETHRESHOLD1 NUMBER K_SEMCOL {}
        | K_EDGERATETHRESHOLD2 NUMBER K_SEMCOL {}
        | K_EDGERATESCALEFACTOR NUMBER K_SEMCOL  {}
;

noisetable
        : noisetable_id nt_edgerate nt_outputres nt_viclnthnois_list end_nt {}
;

noisetable_id
        : K_NOISETABLE string K_SEMCOL {if ($2.DoDelete) free($2.str)}
;

end_nt  : K_END string {/*LOOK! check names*/
                        if ($2.DoDelete) free($2.str)}
;

nt_edgerate
        :  K_EDGERATE NUMBER K_SEMCOL  {}
;

nt_outputres
        : K_OUTPUTRESISTANCE number_list  K_SEMCOL  {}
;

nt_viclnthnois_list 
        : nt_viclnthnois   {}
        | nt_viclnthnois_list nt_viclnthnois   {}
;

nt_viclnthnois
        : nt_viclnth nt_vicnois   {}
;

nt_viclnth 
        : K_VICTIMLENGTH NUMBER K_SEMCOL   {}
;

nt_vicnois
        : K_VICTIMNOISE number_list K_SEMCOL  {}
;

correctiontable
        : crcntable_id nt_edgerate nt_outputres ct_viclnthcorrfact_list end_ct {}
;

crcntable_id
        : K_CORRECTIONTABLE string K_SEMCOL   {if ($2.DoDelete) free($2.str)}
;

end_ct  : K_END string   {if ($2.DoDelete) free($2.str)}
;

ct_viclnthcorrfact_list 
        : ct_viclnthcorrfact    {}
        | ct_viclnthcorrfact_list  ct_viclnthcorrfact   {}
;

ct_viclnthcorrfact
        : nt_viclnth ct_corrfact  {}
;

ct_corrfact
        : K_CORRECTIONFACTOR number_list K_SEMCOL   {}
;

/* SITE STATEMENT BEGINS */

lef_site_new
    : K_SITE string
        {
        strcpy(tempSiteName,$2.str);
        tempSiteHeight = tempSiteWidth = unsigned(-1);
        tempSymmetry.x = tempSymmetry.y = tempSymmetry.rot90 = false;
        tempSiteType = dbSite::Type(-1);
        if ($2.DoDelete) free($2.str);
        }
;
          

lef_site_stmt
    : lef_site_new site_attr_list end_site_id 
               {
               pdb->context._sites.push_back(pTempSite =
                 new dbSite(tempSiteName,
                            tempSiteType,
                            tempSymmetry,
                            tempSiteHeight,
                            tempSiteWidth));

               pair<char const* const,dbSite*> const &pref =
                         pair<char const* const,dbSite*>(
                                                     tempstr=
                                                          strdup(tempSiteName),
                                                     pTempSite) ;
                                                     
               pp->stringsToClean.push_back(tempstr);
                                                              
                   
               pair<ParserLEFDEF::siteHashMapType::iterator,bool> p =
                        pp->siteHash.insert(pref);

               if (!p.second)
                   {
                   sprintf(errtxt,"duplicate declaration of site %s "
                                  "in line %d of LEF",tempSiteName,lefLinenum);
                   abkfatal(0,errtxt);
                   pdb->context._sites.pop_back();
                   delete pTempSite;
                   }
               }
;

site_attr_list
    : site_attr   {}
    | site_attr_list site_attr   {}
;

site_attr
    : K_CLASS site_class K_SEMCOL   {}
    | symmetry                        {}
    | K_SIZE distance K_BY distance K_SEMCOL {tempSiteWidth  = $2;
                                              tempSiteHeight = $4;}
;

site_class
    : K_PAD    {tempSiteType = dbSite::PAD}
    | K_CORE   {tempSiteType = dbSite::CORE;
                if ($1.DoDelete) free ($1.str);/*part of kludge*/
               }
;


symmetry
    : K_SYMMETRY symmetry_attr_list K_SEMCOL  {}
;

symmetry_attr_list
    : symmetry_attr {}
    | symmetry_attr_list symmetry_attr {}
;

symmetry_attr
    : K_X {tempSymmetry.x=true;if ($1.DoDelete) free ($1.str) /*KLUDGE*/}
    | K_Y {tempSymmetry.y=true;if ($1.DoDelete) free ($1.str) /*KLUDGE*/}
    | K_R90 {tempSymmetry.rot90=true;}
;

end_site_id
    : K_END string {//LOOK! need to check name is the same
                   if ($2.DoDelete) free($2.str)}
;


/* SITE STATEMENT ENDS */


/* ARRAY STATEMENT BEGINS */

lef_array_stmt
    : array_id array_attr_list defcap_table_opt end_array_id {}
;

array_id
    : K_ARRAY string  {if ($2.DoDelete) free($2.str)}
;

end_array_id
    : K_END string   {if ($2.DoDelete) free($2.str)}
;

array_attr_list
    : array_attr {}
    | array_attr_list array_attr  {}
;

array_attr
    : site_entry  {}
    | canplace_entry     {}
    | cannotoccupy_entry   {}
    | K_TRACKS x_or_y NUMBER K_DO NUMBER {}
        K_STEP distance layer_list_opt K_SEMCOL {}
    | K_GCELL lef_ggrid_list K_SEMCOL           {}
    | floorplan_id floorplan_attr_list end_floorplan_id {}
;


site_entry
    : site_id lef_site_pattern K_SEMCOL   {}
;

site_id : K_SITE string 
                     {
                     strcpy(DummyString,$<lexeme>2.str);
                     ParserLEFDEF::siteHashMapType::iterator iL
                                   = pp->siteHash.find($2.str);
                     if (iL == pp->siteHash.end())
                         {
                         sprintf(errtxt,"Site %s not found in line %d "
                                        "of LEF file",$2.str,lefLinenum);
                         abkfatal(0,errtxt);
                         }
                     else
                         pTempSite = (*iL).second;

                     if ($2.DoDelete) free($2.str);
                     }
;

lef_site_pattern
    : point orient lef_step_pattern  {}
;

/* Orient will be same as in DEF  Keep it for the timebeing */
orient
    : K_N {if ($1.DoDelete) free ($1.str);}  //KLUDGE
    | K_S {if ($1.DoDelete) free ($1.str);}  //KLUDGE
    | K_W   {if ($1.DoDelete) free ($1.str);}//KLUDGE
    | K_E {if ($1.DoDelete) free ($1.str);}  //KLUDGE
    | K_FN    {}
    | K_FS      {}
    | K_FW        {}
    | K_FE          {}
;

/* Orient will be same as in DEF  Keep it for the timebeing */


canplace_entry
    : K_CANPLACE string lef_site_pattern K_SEMCOL   {if ($2.DoDelete) free($2.str)}
;

cannotoccupy_entry
    : K_CANNOTOCCUPY string lef_site_pattern K_SEMCOL  {if ($2.DoDelete) free($2.str)}
;

x_or_y  : K_X {if ($1.DoDelete) free ($1.str) /*KLUDGE*/}
    | K_Y     {if ($1.DoDelete) free ($1.str) /*KLUDGE*/}
;

layer_list_opt
    :
    | K_LAYER id_list     {}
;

id_list : string {if ($1.DoDelete) free($1.str)}
    | id_list string { //LOOK! this is a problem.  How are we
                       //going to free everything?
                     if ($2.DoDelete) free($2.str)}
;

lef_ggrid_list
    : lef_ggrid    {}
    | lef_ggrid_list lef_ggrid  {}
;

lef_ggrid
    : x_or_y NUMBER K_DO NUMBER K_STEP distance {}
;

floorplan_id
    : K_FLOORPLAN string   {if ($2.DoDelete) free($2.str)}
;

end_floorplan_id
    : K_END string  {/*LOOK! check names*/
                    if ($2.DoDelete) free($2.str)}
;

floorplan_attr_list
    : floorplan_attr  {}
    | floorplan_attr_list floorplan_attr    {}
;

floorplan_attr
    : canplace_entry  {}
    | cannotoccupy_entry  {}
    ;

/*
alias_stmt
    : K_ALIAS string K_EQ {if ($2.DoDelete) free($2.str)}

define_stmt
    : K_DEFINE string K_EQ  expr K_SEMCOL{if ($2.DoDelete) free($2.str)}
*/


/* For 'expr' go to the end of the file */

defcap_table_opt
    :
    | defcap_table {}
    ;

distance : NUMBER
    {
    if ($1 >= 0.0)
        $$ = (int)($1*(pdb->context._units)+0.5);
    else
        $$ = (int)($1*(pdb->context._units)-0.5);
    }
;

defcap_table
    : K_DEFAULTCAP NUMBER defcap_attr_list K_END K_DEFAULTCAP {}
;

defcap_attr_list
    : defcap_attr   {}
    | defcap_attr_list defcap_attr  {}
;

defcap_attr
    : K_MINPINS NUMBER K_WIRECAP NUMBER K_SEMCOL  {}
;

string
    : K_ID  {$$=$1;}

| K_ABUTMENT {$$=$1;}
| K_ACTIVE {$$=$1;}
| K_ANTENNAAREAFACTOR {$$=$1;}
| K_ANTENNALENGTHFACTOR {$$=$1;}
| K_ANTENNAMETALAREA {$$=$1;}
| K_ANTENNAMETALLENGTH {$$=$1;}
| K_ANTENNASIZE {$$=$1;}
| K_ANYEDGE {$$=$1;}
| K_ANALOG {$$=$1;}
| K_ARRAY {$$=$1;}
| K_BLOCK {$$=$1;}
| K_BOTTOMLEFT {$$=$1;}
| K_BOTTOMRIGHT {$$=$1;}
| K_BY {$$=$1;}
| K_CANNOTOCCUPY {$$=$1;}
| K_CANPLACE {$$=$1;}
| K_CAP {$$=$1;}
| K_CAPMULTIPLIER {$$=$1;}
| K_CLASS {$$=$1;}
| K_CLOCK {$$=$1;}
| K_CORE {$$=$1;}
| K_COVER {$$=$1;}
| K_CORRECTIONFACTOR {$$=$1;}
| K_CORRECTIONTABLE {$$=$1;}
| K_CPERSQ {$$=$1;}
| K_CURRENT {$$=$1;}
| K_CURRENTSOURCE {$$=$1;}
| K_CUT {$$=$1;}
| K_DATA {$$=$1;}
| K_DATABASE {$$=$1;}
| K_DEFAULT {$$=$1;}
| K_DEFAULTCAP {$$=$1;}
| K_DELAY {$$=$1;}
| K_DESIGN {$$=$1;}
| K_DIELECTRIC {$$=$1;}
| K_DIRECTION {$$=$1;}
| K_DO {$$=$1;}
| K_E {$$=$1;}
| K_EDGECAP {$$=$1;}
| K_EDGERATE {$$=$1;}
| K_EDGERATETHRESHOLD1 {$$=$1;}
| K_EDGERATETHRESHOLD2 {$$=$1;}
| K_EDGERATESCALEFACTOR {$$=$1;}
| K_EEQ {$$=$1;}
| K_END {$$=$1;}
| K_ENDCAP {$$=$1;}
| K_FALL {$$=$1;}
| K_FALLRS {$$=$1;}
| K_FALLCS {$$=$1;}
| K_FALLSATT1 {$$=$1;}
| K_FALLT0 {$$=$1;}
| K_FALLTHRESH {$$=$1;}
| K_FALLSATCUR {$$=$1;}
| K_FALLVOLTAGETHRESHOLD {$$=$1;}
| K_FE {$$=$1;}
| K_FEEDTHRU {$$=$1;}
| K_FLOORPLAN {$$=$1;}
| K_FN {$$=$1;}
| K_FOREIGN {$$=$1;}
| K_FROMPIN {$$=$1;}
| K_FS {$$=$1;}
| K_FW {$$=$1;}
| K_GCELL {$$=$1;}
| K_GENERATE {$$=$1;}
| K_GROUND {$$=$1;}
| K_HEIGHT {$$=$1;}
| K_HORIZ {$$=$1;}
| K_INOUT {$$=$1;}
| K_INPUT {$$=$1;}
| K_INPUTNOISEMARGIN {$$=$1;}
| K_INOUTPINANTENNASIZE {$$=$1;}
| K_INPUTPINANTENNASIZE {$$=$1;}
| K_INTRINSIC {$$=$1;}
| K_INVERT {$$=$1;}
| K_IRDROP {$$=$1;}
| K_ITERATE {$$=$1;}
| K_IV_TABLES {$$=$1;}
| K_LAYER {$$=$1;}
| K_LEAKAGE {$$=$1;}
| K_LEQ {$$=$1;}
| K_LIBRARY {$$=$1;}
| K_MACRO {$$=$1;}
| K_MILLIAMPS {$$=$1;}
| K_MSLICE {$$=$1;}
| K_MAXDELAY {$$=$1;}
| K_METALOVERHANG {$$=$1;}
| K_MICRONS {$$=$1;}
| K_MILLIWATTS {$$=$1;}
| K_MINFEATURE {$$=$1;}
| K_MINPINS {$$=$1;}
| K_MUSTJOIN {$$=$1;}
| K_N {$$=$1;}
| K_NANOSECONDS {$$=$1;}
| K_CASE {$$=$1;}
| K_NOISETABLE {$$=$1;}
| K_NONDEFAULTRULE {$$=$1;}
| K_NONINVERT {$$=$1;}
| K_NONUNATE {$$=$1;}
| K_OBS {$$=$1;}
| K_OFF {$$=$1;}
| K_OHMS {$$=$1;}
| K_ON {$$=$1;}
| K_ORIGIN {$$=$1;}
| K_OUTPUT {$$=$1;}
| K_OUTPUTRESISTANCE {$$=$1;}
| K_OUTPUTNOISEMARGIN {$$=$1;}
| K_OUTPUTPINANTENNASIZE {$$=$1;}
| K_OVERHANG {$$=$1;}
| K_OVERLAP {$$=$1;}
| K_PAD {$$=$1;}
| K_PATH {$$=$1;}
| K_PICOFARADS {$$=$1;}
| K_PIN {$$=$1;}
| K_PITCH {$$=$1;}
| K_POLYGON {$$=$1;}
| K_PORT {$$=$1;}
| K_POST {$$=$1;}
| K_POWER {$$=$1;}
| K_PRE {$$=$1;}
| K_PULLDOWNRES {$$=$1;}
| K_R90 {$$=$1;}
| K_RECT {$$=$1;}
| K_RES {$$=$1;}
| K_RESISTIVE {$$=$1;}
| K_RING {$$=$1;}
| K_RISE {$$=$1;}
| K_RISERS {$$=$1;}
| K_RISECS {$$=$1;}
| K_RISESATT1 {$$=$1;}
| K_RISET0 {$$=$1;}
| K_RISETHRESH {$$=$1;}
| K_RISESATCUR {$$=$1;}
| K_RISEVOLTAGETHRESHOLD {$$=$1;}
| K_ROUTING {$$=$1;}
| K_RPERSQ {$$=$1;}
| K_S {$$=$1;}
| K_SAMENET {$$=$1;}
| K_SETUPTIME {$$=$1;}
| K_SHAPE {$$=$1;}
| K_SHRINKAGE {$$=$1;}
| K_SIGNAL {$$=$1;}
| K_SITE {$$=$1;}
| K_SIZE {$$=$1;}
| K_SKIP {$$=$1;}
| K_SPACER {$$=$1;}
| K_SPACING {$$=$1;}
| K_STACK {$$=$1;}
| K_STEP {$$=$1;}
| K_STRUCTURE {$$=$1;}
| K_SYMMETRY {$$=$1;}
| K_TABLE {$$=$1;}
| K_THICKNESS {$$=$1;}
| K_TIEHIGH {$$=$1;}
| K_TIELOW {$$=$1;}
| K_TIEOFF {$$=$1;}
| K_TIEOFFR {$$=$1;}
| K_TIMING {$$=$1;}
| K_TIME {$$=$1;}
| K_TO {$$=$1;}
| K_TOPIN {$$=$1;}
| K_TOPLEFT {$$=$1;}
| K_TOPRIGHT {$$=$1;}
| K_TRACKS {$$=$1;}
| K_TRANSITION {$$=$1;}
| K_TRANSITIONTIME {$$=$1;}
| K_TRISTATE {$$=$1;}
| K_TYPE {$$=$1;}
| K_UNATE {$$=$1;}
| K_UNIVERSALNOISEMARGIN {$$=$1;}
| K_UNITS {$$=$1;}
| K_USE {$$=$1;}
| K_USER {$$=$1;}
| K_TOPOFSTACKONLY {$$=$1;}
| K_VARIABLE {$$=$1;}
| K_VERT {$$=$1;}
| K_VHI {$$=$1;}
| K_VIA {$$=$1;}
| K_VIARULE {$$=$1;}
| K_VICTIMLENGTH {$$=$1;}
| K_VICTIMNOISE {$$=$1;}
| K_VLO {$$=$1;}
| K_VOLTAGE {$$=$1;}
| K_VOLTS {$$=$1;}
| K_W {$$=$1;}
| K_WIDTH {$$=$1;}
| K_WIRECAP {$$=$1;}
| K_WIREEXTENSION {$$=$1;}
| K_X {$$=$1;}
| K_Y {$$=$1;}

;

    
/* ARRAY STATEMENT ENDS */

%%
/*
   main () 
   {
   yyparse();
   }
   
   */


void leferror ( char *s) 
{ 
  printf("%s\n\tIn line %d of LEF\n",s,lefLinenum+1);
  fprintf(stderr,"%s\n\tIn line %d of LEF\n",s,lefLinenum+1);
  fflush(stdout);
  fflush(stderr);
  exit(-1);
}
