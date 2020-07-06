%{
#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
//#include <malloc.h>
#include "Geoms/plGeom.h"
#include "LEFDEFpoint.h"
typedef struct lexsstr
    {
    char *str;
    int  DoDelete;
    } lexemeStruct;

#include <DB/DB.h>
#include "parserLEFDEF.h"
//#include "hash.h"
#include "localhash.h"
#include <ABKCommon/sgi_hash_map.h>

using std::pair;
using std::vector;

void deferror (const char *s) ;
int lefparse(void);
int deflex(void);
//CellHashTable *pCellTable;
//NetHashTable  *pNetTable, *pSNetTable;
//MasterCellHashTable *pMasterTable;
//dbPinRec            *firstPinGlobalList=NULL;
//dbPinRec            *lastPinGlobalList=NULL;
int nNets;
int nNetsSoFar=0;
int nSNets;
int nSNetsSoFar=0;
int bInSNets;  //are we in the "special" nets section?
static char *tempstr;
//dbTimeNetRec *netPtr;
//dbTimeCellRec *tempCellPtr;
//dbTimeNetlistRec *pNetlist;
//dbWrapper *pWrapperPtr;
//dbUnits   *pUnitsPtr;
//dbPinRec  *oldPinPtr;
int runningPinIdx;
int nPinsThisNet;
int nCellsThisNet;
double lastX,lastY;
//enum {CHANGE_IN_X,CHANGE_IN_Y,DEFAULT_CHANGE} lastChange=DEFAULT_CHANGE;
dbLayer::Dir pathDirection,oldPathDirection;
int tempArea;
extern int defLinenum;
static char errtxt[1023];
int mikesMain(int argc,char **argv);
//section for variables from parseArgs
extern int lefdebug;
extern FILE *lefin, *defin;
int nIOsThisNet;
int nOUTThisNet;
bool bSpecial;

extern DB           *pdb;
extern ParserLEFDEF *pp;

//globals for dbCell ctor and other comps section info
static char               tempCellName[1023];
static dbMasterCell      *pTempMaster;
static ctainerPinsLocal   tempPins;
//static ctainerPinsLocal   emptyPins;
static dbCell            *pTempCell;
static LEFDEFpoint        tempLoc;
static bool               tempLocValid;
static bool               tempFixed;
static Orient             tempOrient;
//end dbCell globals

//globals for regions
static parserRegion* pTempRegion;
static unsigned runParRegIdx=0;
//end globals for regions

//globals for dbPin ctor
static dbPin             *pTempPin;
//end dbPin globals

//globals for dbNet ctor
static char              tempNetName[1023];
static dbNetType::NetUse tempNetUse;
static int               runningNetIdx=0;
static int               runningSNetIdx=0;
static dbNet            *pTempNet;
static char              tempPinName[1023];
static ctainerLayerGeoms tempWiring; //for special nets only
static bool              logicalPinReferenced;
//end dbNet globals

//globals for external pins
static dbMasterPin *pTempMasterPin;
static char tempMasterPinUse[127];
static char tempMasterPinDir[127];
static BBox tempMpinBBox;
static dbLayerGeom *pTempPort;
static dbLayer     *pTempLayer;
static dbMasterCell *pTempMasterCell;
static dbMasterCellType::PadSubClass tempPadClass;
//end extpin globals

//globals for groups
static vector<dbCell*> tempCompCache;
//static vector<dbCell*> emptyCompCache;
static bool            bSoft;
static int             nNumGroupRegions; // to check my understanding only
static int             tempMaxX,tempMaxY,tempMaxHp;
//end groups globals

//globals for dbRow ctor
static char       tempRowName[1023];
static dbSite    *pTempSite;
static Point      tempRowOrig;
static dbRow::Dir tempRowDir;
//static Orient     tempRowOrient; //already have tempOrient
static unsigned   tempNumSites;
static unsigned   tempRowSpacing;
//end dbRow globals

//globals for tracks
static bool tempHoriz;
static vector<dbLayer*> trackLayers;
//end tracks globals

//globals for vias
static dbVia            *pTempVia;
static char              tempViaName[1023];
static ctainerLayerGeoms tempViaLGs;
//end vias globals

//globals for special wiring
static double        tempWidth;
static Point         lastPoint;
        //following two vectors are parallel 
static vector<BBoxExtended>    wireGeoms;
static vector<bool> wrongWay;

double        wireMinX,wireMinY,wireMaxX,wireMaxY;
//end special wiring globals

int bCaseSensDEF=0;

static char cellname[2048];
static char pinname[2048];
static char tempGroupName[2048];
%}

%union {
    double       dval ;
    int          integer ;
    lexemeStruct string ;
    int          keyword ;  /* really just a nop */
    struct       LEFDEFpoint pt;
    BBoxSimple   bbox;
    /* TOKEN *tk; */
}

%token <string>  QSTRING
%token <string>  STRING_not_keyword
%token <dval>    NUMBER
%token <string> K_HISTORY K_DESIGN K_VIAS K_TECH K_UNITS K_ARRAY K_FLOORPLAN
%token <string> K_LRECT K_SITE K_DIEAREA K_PINS
%token <string> K_DCAP K_MINPINS K_WIRECAP
%token <string> K_TRACKS K_GCELLGRID K_SYNTH
%token <string> K_DO K_BY K_STEP K_LAYER K_XTALK
%token <string> K_COMPS K_COMP_GEN K_SOURCE K_WEIGHT
%token <string> K_FIXED K_COVER K_PLACED K_UNPLACED K_FOREIGN K_REGION
%token <string> K_NETS K_MUSTJOIN K_ORIGINAL K_USE K_ORDER
%token <string> K_FLOAT K_SCAN K_COMM_SCAN K_START K_STOP
%token <string> K_IN K_OUT
%token <string> K_PATTERN K_SCANFIX K_ESTCAP K_ROUTED K_NEW
%token <string> K_SNETS K_WIDTH K_VOLTAGE K_SPACING
%token <string> K_N K_S K_E K_W K_FN K_FE K_FS K_FW
%token <string> K_GROUPS K_START_GROUP K_SOFT K_MAXX K_MAXY K_MAXHALFPERIMETER
%token <string> K_CONSTRAINTS K_NET K_PATH K_RISEMIN K_RISEMAX
%token <string> K_FALLMIN K_FALLMAX  K_WIREDLOGIC
%token <string> K_MAXDIST K_ASSERTIONS K_SUM
%token <string> K_DISTANCE K_MICRONS
%token <string> K_LE K_GE K_NE K_END K_DCAPS
%token <string> K_PATTERNNAME K_ROW 
%token <string> K_SIGNAL K_POWER K_GROUND K_CLOCK K_TIEOFF K_ANALOG 
%token <string> K_INPUT K_OUTPUT K_INOUT K_TRISTATE K_DIRECTION K_SPECIAL
%token <string> K_SHAPE K_ABUTMENT K_RING K_FEEDTHRU K_TAPER
%token <string> K_REGIONS K_PROPERTY
%token <string> K_VERSION
%token <string> K_BUSBITCHARS K_DIVIDERCHAR
%token <string> QUOTED_STR
%token <string> K_X K_Y
%token <string> K_ON K_OFF K_CASE
%token <string> K_PROPERTYDEFINITIONS K_SPECIALNET K_GROUP K_COMPONENTPIN
%token <string> K_INTEGER K_REAL K_STRING K_RANGE K_COMPONENT
%token <string> K_RISE K_FALL K_VARIABLE K_SLEWRATE K_CAPACITANCE
%token <string> K_DRIVECELL K_FROMPIN K_TOPIN K_PARALLEL K_PIN K_IOTIMINGS
%token <string> K_PINPROPERTIES
%token <string> K_STRIPE
%token K_PLUS K_MINUS K_STAR  K_ASGN  SITE_PATTERN K_COMMA K_SEMCOL   
%token K_DOT_DOT K_DOT K_COLON K_ORDBR1 K_ORDBR2 K_CHAR_CONST K_INT_CONST
%token K_REAL_CONST K_IDENTIFIER


%type  <pt>   pt
%type <bbox>  bbox
%type <integer> pin
%type <integer> orient
%type <integer> comp_types
%type <integer> comp_netlist
%type <string> net_type
%type <string> array_name
%type <string> floorplan_name
%type <string> STRING_not_NEW
%type <string> STRING


/*
   %type <tk> paths new_paths
   */

%%


def_file: rules 
    | rules end_design
    ;

rules: /* empty */
    | rules rule
    | error
        ;

rule: version_section | busbitchar_section|case_section| dividerchar_section
    | propdefs_section
    | design_section | via_section | regions_section 
    | comps_section 
    | nets_section | snets_section | groups_section 
    | scan_section | constraint_section | iotimings_section
    | pinproperties_section
        ;

version_section
    : K_VERSION NUMBER K_SEMCOL {}
;

busbitchar_section
    : K_BUSBITCHARS QUOTED_STR K_SEMCOL {}
;

dividerchar_section
    : K_DIVIDERCHAR QUOTED_STR K_SEMCOL {}
;

case_section
    : begin_case K_ON K_SEMCOL {bCaseSensDEF = 1;}
    | begin_case K_OFF K_SEMCOL {bCaseSensDEF = 0;}
;

begin_case
        : K_CASE {}
;

propdefs_section
    : K_PROPERTYDEFINITIONS propdefs_list K_END K_PROPERTYDEFINITIONS {}
;

propdefs_list
    :
    | propdefs_list propdef_item {}
;

propdef_item
    : object_type STRING prop_type poss_range poss_prop_val  K_SEMCOL
     {
     if ($2.DoDelete) free ($2.str);
     }
;

object_type
    : K_DESIGN        {}
    | K_COMPONENT     {}
    | K_NET           {}
    | K_SPECIALNET    {}
    | K_GROUP         {}
    | K_ROW           {}
    | K_COMPONENTPIN  {}
    | K_REGION    {}
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


design_section: design_name | tech_name | history | units | row | die_area |
          pin_cap_rule | pin_rule | tracks_rule | gcellgrid | array_name{} |
          floorplan_name{}
           ;

design_name: K_DESIGN STRING K_SEMCOL 
    {
    strcpy(pdb->designName,$2.str);
    if ($2.DoDelete) free($2.str);
    }
;


end_design: K_END K_DESIGN
    { 
    }
;

tech_name:   K_TECH STRING K_SEMCOL
    {
    strcpy(pdb->technoName,$2.str);
    if ($2.DoDelete)free($2.str)  ;
    }
;

array_name:   K_ARRAY STRING K_SEMCOL
        {$$=$2;if ($2.DoDelete) free ($2.str);}
;

floorplan_name:   K_FLOORPLAN STRING K_SEMCOL
        {$$=$2;if ($2.DoDelete) free ($2.str);}
;

history:  K_HISTORY  STRING_list K_SEMCOL
        { }
;

units:  K_UNITS K_DISTANCE K_MICRONS NUMBER K_SEMCOL
    {
    //pUnitsPtr->DEFDistConv = (int)($4+1e-5);
    }
;

row_start: K_ROW STRING {
                        strcpy(tempRowName,$2.str);
                        if ($2.DoDelete) free ($2.str);
                        }
;

site_start: K_SITE {
                   sprintf(tempRowName,"Anonymous Row %d",
                                       pp->tempAllRows.size());
                   }
;

row_init: row_start_or_site_start{
                                 pTempSite=NULL;
                                 tempNumSites = unsigned(-1);
                                 tempRowSpacing = unsigned(-1);
                                 tempRowOrig.x=tempRowOrig.y=DBL_MAX;
                                 tempRowDir = 
                                    static_cast<dbRow::Dir>(INT_MAX);
                                 // don't know how to give impossible value
                                 // to tempOrient
                                 }
;

row_start_or_site_start: row_start {}
                       | site_start{}
;

site_type_and_step_pattern: STRING site_step_pattern
                            {
                            ParserLEFDEF::siteHashMapType::iterator iS 
                                         = pp->siteHash.find($1.str);
                            if (iS == pp->siteHash.end())
                                {
                                sprintf(errtxt,"Site %s not found in line %d "
                                               "of DEF file",$1.str,
                                               defLinenum);
                                abkfatal(0,errtxt);
                                }
                             else
                                pTempSite = (*iS).second;
                            if ($1.DoDelete) free ($1.str);
                            }
;

row: row_init site_type_and_step_pattern
        {
        pp->tempAllRows.push_back(new dbRow(*pTempSite,
                                            tempRowOrig,
                                            tempRowDir,
                                            tempRowSpacing,
                                            tempNumSites,
                                            tempRowName,
                                            tempOrient));
        }
;

site_step_pattern: NUMBER NUMBER orient 
          K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER K_SEMCOL
             {
             tempRowOrig.x = $1;
             tempRowOrig.y = $2;
             if ($5 > 1)
                 {
                 tempNumSites=unsigned($5);
                 tempRowDir = dbRow::HORIZ;
                 tempRowSpacing = unsigned($9);
                 if ($7>1)
                     {
                     sprintf(errtxt,"ambiguous row direction in "
                                    "line %d of DEF",defLinenum);
                     abkfatal(0,errtxt);
                     }
                 }
             else if ($7 > 1)
                 {
                 tempNumSites=unsigned($7);
                 tempRowDir = dbRow::VERT;
                 tempRowSpacing = unsigned($10);
                 }
             else
                 { // 1x1 case
                 if ($5 !=1 || $7 != 1)
                     {
                     sprintf(errtxt,"illegal num sites in "
                                    "line %d of DEF",defLinenum);
                     abkfatal(0,errtxt);
                     }
                 tempNumSites=1;
                 tempRowDir = dbRow::HORIZ;  //rather strange, but
                                             //shouldn't matter
                 tempRowSpacing = 0;

/*                 if ($9>0)
                     {
                     if ($10!=0)
                         {
                         sprintf(errtxt,"ambiguous row direction in "
                                        "line %d of DEF",defLinenum);
                         abkfatal(0,errtxt);
                         }
                     tempRowDir = dbRow::HORIZ;
                     tempRowSpacing = $9;
                     }

                else if ($10>0)
                     {
                     tempRowDir = dbRow::VERT;
                     tempRowSpacing = $10;
                     }

                else
                     {
                     sprintf(errtxt,"ambiguous row direction (1x1) in "
                                    "line %d of DEF",defLinenum);
                     abkfatal(0,errtxt);
                     }
*/
                 } //end 1x1 case
             }
;

orient: K_N   {tempOrient = Orient(0,true);if($1.DoDelete)free($1.str)}
    | K_E     {tempOrient = Orient(90,true);if($1.DoDelete)free($1.str)}
    | K_S     {tempOrient = Orient(180,true);if($1.DoDelete)free($1.str)}
    | K_W     {tempOrient = Orient(270,true);if($1.DoDelete)free($1.str)}
    | K_FN  {tempOrient = Orient(0,false)}
    | K_FE  {tempOrient = Orient(90,false)}
    | K_FS  {tempOrient = Orient(180,false)}
    | K_FW  {tempOrient = Orient(270,false)}
        ;

die_area: K_DIEAREA bbox K_SEMCOL 
    {
    pdb->context._bbox = $2;
    }
;

pin_cap_rule: start_def_cap pin_caps end_def_cap
    { }
    ;

start_def_cap: K_DCAP NUMBER 
    {  }
    ;

pin_caps: /* empty */
    | pin_caps pin_cap
        ;

pin_cap: K_MINPINS NUMBER K_WIRECAP NUMBER K_SEMCOL
    { }
    ;

end_def_cap: K_END K_DCAP 
    { }
    ;

pin_rule: start_pins pins end_pins
    { }
;

start_pins: K_PINS NUMBER K_SEMCOL
    {
//    pNetlist->netList.reserve($2); 
    }
;

pins: /* empty */
    | pins pin
;

pin_start: K_MINUS STRING
    {
    bSpecial = false;
    tempPins.clear();
    strcpy(tempPinName,$2.str);
    if ($2.DoDelete) free($2.str);
    tempMasterPinUse[0] = '\0';
    tempMasterPinDir[0] = '\0';
    tempMpinBBox.clear();
    pTempPort = NULL;
    pTempLayer = NULL;
    tempLoc.x = tempLoc.y = DBL_MAX;
    tempLocValid = false;
    tempFixed    = false;
    tempOrient = Orient();
    tempNetUse = dbNetType::SIGNAL; //default added 9-11-97 mro
    tempPadClass = dbMasterCellType::PadUNKNOWN;
    }
;

pin: pin_start  K_PLUS K_NET STRING ext_pin_net_opts K_SEMCOL
    {

    bool isLogicalPin=true; // will be changed if pin has geometry

    if (pTempPort)
        {
        isLogicalPin=false;
        char mpinname[1023];
        strcpy(mpinname,"FakedMasterPinFor");
        strcat(mpinname,tempPinName);

        switch(tempNetUse)
            {
            case dbNetType::SIGNAL:
                strcpy(tempMasterPinUse,"SIGNAL");
                break;
            case dbNetType::ANALOG:
                strcpy(tempMasterPinUse,"ANALOG");
                break;
            case dbNetType::POWER:
                strcpy(tempMasterPinUse,"POWER");
                break;
            case dbNetType::GROUND:
                strcpy(tempMasterPinUse,"GROUND");
                break;
            case dbNetType::CLOCK:
                strcpy(tempMasterPinUse,"CLOCK");
                break;
            case dbNetType::UNKNOWN:
                strcpy(tempMasterPinUse,"");
                break;
            case dbNetType::TIEOFF:
            default:
                abkfatal(0,"Illegal use spec for external pin");
            }

        if (tempMpinBBox.isEmpty())
            {
            sprintf(errtxt,"In line %d of DEF file, I/O pin %s has"
                    " empty bounding box; adding (0,0) "
                    "to avoid crash",defLinenum,mpinname);
            abkwarn(0,errtxt);
            tempMpinBBox += Point(0,0);
            }

        pdb->design._pinLib.push_back(
                  pTempMasterPin = new dbMasterPin(mpinname,
                                                   tempMasterPinDir,
                                                   tempMasterPinUse,
                                                   "",
                                                   tempMpinBBox,
                                                   tempMpinBBox.getGeomCenter()
                                                   )
                                                   );

        if (pTempPort) pTempMasterPin->_ports.push_back(pTempPort);

        ctainerMasterPinsLocal tempMasterPins;
        tempMasterPins.push_back(pTempMasterPin);
        ctainerLayerGeoms      emptyLGs;

        char mcellname[1023];
        strcpy(mcellname,"FakedMasterCellFor");
        strcat(mcellname,tempPinName);

        pdb->design._cellLib.push_back(
                  pTempMasterCell = new dbMasterCell(mcellname,
                                                     dbMasterCellType::EXTPIN,
                                                     tempPadClass,
                                                     0.0,
                                                     0.0,
                                                     Point(0.0,0.0),
                                                     tempMasterPins,
                                                     emptyLGs,
                                                     Symmetry(false,false,false),
                                                     *(pp->pExtPinSite))
                                                     );

        pTempMasterPin->_cell = pTempMasterCell;
        pair<char const* const,dbMasterCell*> const &pref =
            pair<char const* const,dbMasterCell*>(
                                     tempstr=strdup(mcellname),
                                     pTempMasterCell) ;
                                 
        pp->stringsToClean.push_back(tempstr);
                                         

        pair<ParserLEFDEF::masterCellHashMapType::iterator,bool> p =
            pp->masterCellHash.insert(pref);

        if (!p.second)
            {
            sprintf(errtxt,"duplicate declaration of IO pin master cell %s "
                      "in line %d of DEF; ignored",tempCellName,defLinenum);
            abkfatal(0,errtxt);
            pdb->design._cellLib.pop_back();
            delete pTempMasterCell;
            }

        int numPins = pp->tempAllPins.size();

        pp->tempAllPins.push_back(pTempPin = new dbPin(*pTempMasterPin,
                                                         numPins));

        tempPins.clear();
        tempPins.push_back(pTempPin);

        strcpy(cellname,"FakedInstantiatedCellFor");
        strcat(cellname,tempPinName);

        int numCells = pp->tempAllCells.size();

        pp->tempAllCells.push_back(pTempCell = new dbCell(cellname,
                                                            numCells,
                                                            *pTempMasterCell,
                                                            tempPins) );

        pTempPin->_cell = pTempCell;
        pair<char const* const,dbCell*> const &pref0 =
              pair<char const* const,dbCell*>(
                                          tempstr=strdup(cellname),
                                          pTempCell) ;
        pp->stringsToClean.push_back(tempstr);
        pair<ParserLEFDEF::cellHashMapType::iterator,bool> p0 =
             pp->cellHash.insert(pref0);

        if (!p0.second)
            {
            sprintf(errtxt,"duplicate io pin name %s "
                           "in line %d of DEF",cellname,defLinenum);
            abkfatal(0,errtxt);
            pp->tempAllCells.pop_back();
            delete pTempCell;
            }

        pp->tempAllLocsValid.push_back(tempLocValid);
        pp->tempAllOrients.push_back(tempOrient);
        pp->tempAllLocs.push_back(tempLoc);

        if (tempFixed)
           pp->fixedCompIndices.push_back(pTempCell->getIndex());

        } // end creation of faked stuff for non-logical pin
    else
        {
        tempPins.clear();
        }



    strcpy(tempNetName,$4.str);

    ParserLEFDEF::netHashMapType &condHash = (bSpecial)? pp->SnetHash
                                                       : pp->netHash;

    ParserLEFDEF::netHashMapType::iterator iN = condHash
                                                .find((char const*)tempNetName);

    if (iN == condHash.end())
        pTempNet=NULL;
     else
        pTempNet = (*iN).second;

    //LOOK! this code is almost identical to that in net_and_connections.
    //      how can duplication be avoided?

    if (!pTempNet)
        {
        pTempNet = new dbNet(tempNetName,
                             (bSpecial)?(runningSNetIdx++):(runningNetIdx++),
                             dbNetType(tempNetUse),
                             tempPins);

        if (bSpecial)
            pp->tempAllSNets.push_back(pTempNet);
        else
            pp->tempAllNets.push_back(pTempNet);

        pair<char const* const,dbNet*> const &pref1 =
              pair<char const* const,dbNet*>(
                                          tempstr=strdup(tempNetName),
                                          pTempNet) ;

        pp->stringsToClean.push_back(tempstr);
                                      
        pair<ParserLEFDEF::netHashMapType::iterator,bool> p1 =
             condHash.insert(pref1);

        if (!p1.second)
            {
            sprintf(errtxt,"internal error at net %s "
                           "in line %d of DEF",tempNetName,defLinenum);
            abkfatal(0,errtxt);
            }
        }
    else
        {
        // we have to add connections to an existing net
        ctainerPinsLocal::iterator iP;
        for (iP = tempPins.begin() ; iP != tempPins.end() ; iP++ )
            {
            pTempNet->_pins.push_back((*iP));
            }
        }

    if (isLogicalPin)
        pTempNet->_hasLogicalPin=true;

    ctainerPinsLocal::iterator iP;
    for (iP = tempPins.begin() ; iP != tempPins.end() ; iP++ )
        {
        dbPin &pin = *(*iP);
        if (pin._net)
            {
            sprintf(errtxt,"Pin %s of cell %s found on two nets, %s and %s",
                           pin.getMasterPinName(),
                           pin.getCell().getName(),
                           pin._net->getName(),
                           tempNetName);
            abkfatal(0,errtxt);
            }
        pin._net = pTempNet;
        }



    if ($4.DoDelete)free($4.str);
    }
;

end_pins: K_END K_PINS
    { }
;

ext_pin_net_opts:
                | ext_pin_net_opts ext_pin_net_opt {}
;

ext_pin_net_opt: direction_spec {} 
               | use_option {}
               | special_opt {}
               | ext_pin_layer_opt {}
               | comp_type {}
;

direction_spec : K_PLUS K_DIRECTION pin_direction {}
;

special_opt : K_PLUS K_SPECIAL {bSpecial=true;}
;

layer_name : STRING
      {              //note: this rule is used not only by ext_pin_layer_opt,
                     //but also by mult_layer_names (ultimately used for
                     //tracks)

      ParserLEFDEF::layerHashMapType::iterator iL =
                   pp->layerHash.find($1.str);

      if (iL == pp->layerHash.end())
         {
         sprintf(errtxt,"Layer %s not found in line %d "
                        "of DEF file",$1.str,defLinenum);
         abkfatal(0,errtxt);
         }
      else
         pTempLayer = (*iL).second;
      if ($1.DoDelete) free ($1.str);
      }
;

ext_pin_layer_opt : K_PLUS K_LAYER layer_name bbox
                  {
                  tempMpinBBox = $4;

                  pTempPort = new dbLayerGeom(tempMpinBBox,
                                             *pTempLayer);

                  }
;

pin_direction
    : K_INPUT {tempPadClass=dbMasterCellType::INPUT;
               strcpy(tempMasterPinDir,"OUTPUT")} //directions are reversed
    | K_INOUT {tempPadClass=dbMasterCellType::INOUT;
               strcpy(tempMasterPinDir,"INOUT")}  //because input to chip
    | K_OUTPUT {tempPadClass=dbMasterCellType::OUTPUT;
                strcpy(tempMasterPinDir,"INPUT")} //is output of fake cell
    | K_FEEDTHRU {tempPadClass=dbMasterCellType::PadUNKNOWN;//LOOK! is this right?
                  strcpy(tempMasterPinDir,"INOUT")} 
;


tracks_rule: track_start track_direction NUMBER K_DO NUMBER K_STEP NUMBER 
                        poss_mult_layers_spec K_SEMCOL
    {
    itLayer iL;
    for (iL=trackLayers.begin();iL!=trackLayers.end();iL++)
        {
        pp->tempAllTracks.push_back(new dbTrack(*(*iL),
                                                tempHoriz,
                                                $3,
                                                $5,
                                                $7));
        }

    }
;

track_start : K_TRACKS
    {
    pTempLayer=NULL;
    trackLayers.clear();
    }
;

track_direction
    :K_X {tempHoriz = true; if ($1.DoDelete) free($1.str);}
    |K_Y {tempHoriz = false; if ($1.DoDelete) free($1.str);}
;

poss_mult_layers_spec : 
                      | mult_layers_spec {}
;

mult_layers_spec : K_LAYER mult_layer_names {}

;

mult_layer_names: /* empty */
    | mult_layer_names layer_name
    {trackLayers.push_back(pTempLayer)}
;

gcellgrid: K_GCELLGRID STRING NUMBER K_DO NUMBER K_STEP NUMBER K_SEMCOL
        {
        if ($2.DoDelete) free($2.str);
        }
;

via_section: via via_declarations via_end
;
    
via: K_VIAS NUMBER K_SEMCOL 
        {pp->tempAllVias.reserve(pp->tempAllVias.size() + (int)($2+.01))}
;

via_declarations: /* empty */
    | via_declarations via_declaration
;

via_start: K_MINUS
    {
    tempViaLGs.clear();
    pTempLayer=NULL;
    pTempVia=NULL;
    }
;

via_declaration: via_start via_name_tolerant layer_rects_or_patterns K_SEMCOL
        {
        std::sort(tempViaLGs.begin(),tempViaLGs.end(),CompareLayerGeoms());

        if (pTempVia)
            { //via was declared in LEF, or perhaps earlier in DEF
            if (pTempVia->getGeometries().size() != tempViaLGs.size())
                {
                sprintf(errtxt,"Size of previously-declared via "
                               "geometries does not equal size of current "
                               "declaration, via name %s, line %d "
                               "of DEF",pTempVia->getName(),defLinenum);

                abkwarn(0,errtxt);
                }
            ctainerLayerGeoms::const_iterator iPrevLGs;

            ctainerLayerGeoms::const_iterator iViaLGs;

            for (iPrevLGs=pTempVia->getGeometries().begin(),
                 iViaLGs = tempViaLGs.begin();

                 iPrevLGs != pTempVia->getGeometries().end();

                 iPrevLGs++,iViaLGs++)
                     {
                     Rectangle r1 = *(*iViaLGs);
                     Rectangle r2 = *(*iPrevLGs);
                     if (fabs(r1.xMin - r2.xMin) > 1e-4
                         || fabs(r1.xMax - r2.xMax) > 1e-4
                         || fabs(r1.yMin - r2.yMin) > 1e-4
                         || fabs(r1.yMax - r2.yMax) > 1e-4)
                         {
                         sprintf(errtxt,"Possible mismatch in redundant "
                                        "via declarations, via name %s, "
                                        "line %d of DEF",pTempVia->getName(),
                                        defLinenum);
                         abkwarn(0,errtxt);
                         break;
                         } 
                     }

            //Now we have to clean up as the LGs won't be getting
            //into tempAllVias

            itLayerGeom iLG;

            for (iLG=tempViaLGs.begin();iLG!=tempViaLGs.end();iLG++)
                delete (*iLG);


            }
        else
            { // new via
            pp->tempAllVias.push_back(pTempVia = new dbVia(tempViaName,
                                                          -1.0, //resistance
                                                                //not given
                                                                //in DEF
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
                abkfatal(0,"Internal error");
                }
            }
        }
;

regions_section: regions_start region_declarations regions_end {}
;

regions_start: K_REGIONS NUMBER K_SEMCOL {pp->regions.reserve($2)}
;

region_declarations: {} //empty
    |region_declarations region_declaration {}
;

region_declaration: region_boxes_start region_boxes region_prop K_SEMCOL {}
;

region_boxes_start: K_MINUS STRING 
                       {pTempRegion = new parserRegion(runParRegIdx++);
                        pp->regions.push_back(pTempRegion); //for cleanup
                        pair<char const* const,parserRegion*> const &pref =
                              pair<char const* const,parserRegion*>(
                                                          tempstr=strdup($2.str),
                                                          pTempRegion) ;

                        pp->stringsToClean.push_back(tempstr);
                                          
                        pair<ParserLEFDEF::regionHashMapType::iterator,bool> p =
                             pp->regionHash.insert(pref);

                        if (!p.second)
                            {
                            sprintf(errtxt,"internal error at region %s "
                                           "in line %d of DEF",$2.str,defLinenum);
                            abkfatal(0,errtxt);
                            }
                        if ($2.DoDelete) free($2.str);
                        }
;

region_boxes: region_box {}
            | region_boxes region_box {}
;

region_box: bbox {pTempRegion->boxes.push_back($1)}
;

region_prop: | K_PLUS K_PROPERTY STRING NUMBER 
                        {
			   abkwarn(0,"Properties not supported in regions");
			   if(!strcmp($3.str,"UTILIZATION"))
			     {
			       pTempRegion->utilConstraint = true;
			       pTempRegion->utilization = (double)$4;
			     }
			   if ($3.DoDelete) free ($3.str);
			 }
;

regions_end: K_END K_REGIONS {}
;

/* LOOK! the layer_pattern rule is not mentioned in the DEF manual */
/* but appears in CSHELL/rev2_8p.local.def                         */
/* NO, I WAS WRONG, it is mentioned, page 6-23.                    */
/* Perhaps the rules are not exactly right because according to    */
/* above reference, there should be one patternname for every rect */
/* but that doesn't seem to obtain in our examples                 */

layer_rects_or_patterns: /* empty */
    | layer_rects_or_patterns layer_rect_or_pattern {}
;

layer_rect_or_pattern : layer_rect {}
                      | layer_pattern {}
;

layer_rect: K_PLUS K_LRECT layer_name bbox
        {
        tempViaLGs.push_back(new dbLayerGeom($4,
                                            *pTempLayer,
                                            true));
        }
;

layer_pattern: K_PLUS K_PATTERNNAME STRING
        {
        if ($3.DoDelete) free ($3.str);
        }

;

pt : K_ORDBR1 NUMBER NUMBER K_ORDBR2
    {
    lastX = $$.x = (int)($2); //*(pUnitsPtr->DEFDistConv)+1e-5);
    lastY = $$.y = (int)($3); //*(pUnitsPtr->DEFDistConv)+1e-5);
    }
    | K_ORDBR1 K_STAR NUMBER K_ORDBR2
    {
    $$.x = lastX;
    lastY = $$.y = (int)($3); //*(pUnitsPtr->DEFDistConv)+1e-5);
    pathDirection = dbLayer::VERT;
    }
    | K_ORDBR1 NUMBER K_STAR K_ORDBR2
    {
    lastX = $$.x = (int)($2); //*(pUnitsPtr->DEFDistConv)+1e-5);
    $$.y = lastY;
    pathDirection = dbLayer::HORIZ;
   }
    | K_ORDBR1 K_STAR K_STAR K_ORDBR2
    {
    $$.x = lastX;
    $$.y = lastY;
    pathDirection = dbLayer::VERT;
   }
;

bbox : pt pt {$$.xMin=min($1.x,$2.x);
              $$.yMin=min($1.y,$2.y);
              $$.xMax=max($1.x,$2.x);
              $$.yMax=max($1.y,$2.y);}
;

via_end: K_END K_VIAS
        { }
        ;

comps_section: start_comps comps_rule end_comps
        ;

start_comps: K_COMPS NUMBER K_SEMCOL
    { //printf("parsed successfully \n");
    double dNumCells = $<dval>2;
    int    nNumCells = (int) dNumCells;

    if (dNumCells != (double) nNumCells)
        deferror("Nonintegral number of cells!\n");

    runningPinIdx = 0; 
    pp->tempAllCells.reserve((int)($2 * 1.2)); //20% leeway for extpins
    }
;

comps_rule: /* empty */
    | comps_rule comp     {//printf("comps_rule parsed \n");
                          }

;
comp: comp_start comp_options K_SEMCOL
    {//printf("comp parsed \n");
    unsigned cellIdx=pp->tempAllCells.size();
    pp->tempAllCells.push_back(pTempCell = new dbCell(tempCellName,
                                                      cellIdx,
                                                      *pTempMaster,
                                                      tempPins) );

    ctainerPinsLocal::iterator iP;

    for (iP=tempPins.begin();iP!=tempPins.end();iP++)
        {
        (*iP)->_cell = pTempCell;
        }

    pair<char const* const,dbCell*> const &pref =
          pair<char const* const,dbCell*>(
                                      tempstr=strdup(tempCellName),
                                      pTempCell) ;
                                      
    pp->stringsToClean.push_back(tempstr);
         
    
    pair<ParserLEFDEF::cellHashMapType::iterator,bool> p =
         pp->cellHash.insert(pref);

    if (!p.second)
        {
        sprintf(errtxt,"duplicate declaration of cell %s "
                       "in line %d of DEF; ignoring"
                       " (could be an IO pin of the same name)"
                       ,tempCellName,defLinenum);
        abkfatal(0,errtxt);
        delete pTempCell;
        pp->tempAllCells.pop_back();
        }

    pp->tempAllLocsValid.push_back(tempLocValid);
    pp->tempAllOrients.push_back(tempOrient);
    pp->tempAllLocs.push_back(tempLoc);
    if (pTempRegion)
        {
        compRegionPair thisPair(pTempCell,pTempRegion);
        pp->compRegions.push_back(thisPair);
        }

    if (tempFixed)
       pp->fixedCompIndices.push_back(pTempCell->getIndex());

    }
;

comp_start: comp_id_and_name comp_netlist
    { //printf("comp_start parsed\n");
    }
;

comp_id_and_name: K_MINUS STRING STRING
    {
    tempPins.clear();
    strcpy(tempCellName,$2.str);
    ParserLEFDEF::masterCellHashMapType::iterator iC 
                 = pp->masterCellHash.find($3.str);
    if (iC == pp->masterCellHash.end())
        {
        sprintf(errtxt,"Master Cell %s not found in line %d "
                       "of DEF file",$3.str,defLinenum);
        abkfatal(0,errtxt);
        }
     else
        pTempMaster = (*iC).second;


    ctainerMasterPinsLocal::const_iterator iP;

    for (iP = pTempMaster->pinsBegin();iP != pTempMaster->pinsEnd();iP++)
        {
        pp->tempAllPins.push_back(pTempPin = 
                                       new dbPin(*(*iP),runningPinIdx++));
        tempPins.push_back(pTempPin);

        }

    tempLoc.x = tempLoc.y = DBL_MAX;
    tempLocValid          = false;
    tempFixed             = false;
    tempOrient            = Orient();
    pTempRegion           = NULL;
    if ($2.DoDelete) free ($2.str);
    if ($3.DoDelete) free ($3.str);
    }

;
comp_netlist:   /* empty */
    { }
    | comp_netlist K_STAR
    {
    abkwarn(0,"Net connection specified in component\n");
    }
    | comp_netlist STRING
    {
    abkwarn(0,"Net connection specified in component\n");
    if ($2.DoDelete) free($2.str);
    }
;

comp_options: /* empty */
    |     comp_options comp_option { //printf("comp_options parsed\n");
                                   }
;

comp_option:  comp_generate {abkwarn(0,"component GENERATE option ignored")} 
          | comp_source     
          | comp_type { //printf("comp_option parsed\n");
                      }
          | weight 
          | comp_foreign 
          | comp_region 
;

comp_generate: K_PLUS K_COMP_GEN STRING STRING 
    {
    if ($3.DoDelete) free($3.str);
    if ($4.DoDelete) free($4.str);
    }
;

comp_source: K_PLUS K_SOURCE STRING 
    {
    if ($3.DoDelete) free($3.str);
    }
;

comp_region: K_PLUS K_REGION bbox   
                    {
                    pTempRegion = new parserRegion(runParRegIdx++);
                    pTempRegion->boxes.push_back($3);
                    pp->regions.push_back(pTempRegion); //for cleanup
                    }
            | K_PLUS K_REGION STRING
                    {
                    ParserLEFDEF::regionHashMapType::iterator iR = 
                                pp->regionHash.find((char const*)($3.str));

                    if (iR == pp->regionHash.end())
                        {
                        pTempRegion=NULL;
                        sprintf(errtxt,"Region %s not found for component %s "
                                       "in line %d of DEF file",$3.str,
                                       tempCellName,defLinenum);
                        abkfatal(0,errtxt);
                        }
                     else
                        pTempRegion = (*iR).second;

                    if ($3.DoDelete) free ($3.str);
                    }
;
                                    

comp_foreign: K_PLUS K_FOREIGN STRING pt STRING
    {
    if ($3.DoDelete) free($3.str);
    if ($5.DoDelete) free ($5.str);
    }
;

comp_type: comp_types pt orient
    {
    //tempCellPtr->setCoord($2.x,$2.y);
    //if (tempCellPtr->getPlacementStatus()==StatusFIXED)
    //    {
    //    dbRectangle &Region = tempCellPtr->Region;
    //    Region.llx = Region.urx = $2.x;
    //    Region.lly = Region.ury = $2.y;
    //    }

    tempLoc = $2;
    }
;
    
comp_types: K_PLUS K_FIXED 
        {
        //tempCellPtr->setPlacementStatus(StatusFIXED);
        //pNetlist->incrementNumFixed();
        tempLocValid = true;
        tempFixed    = true;
        }
    | K_PLUS K_COVER 
        {
        //tempCellPtr->setPlacementStatus(StatusCOVER);
        //pNetlist->incrementNumFixed();
        tempLocValid = true;
        tempFixed    = true;
        }
    | K_PLUS K_PLACED
        {
        //tempCellPtr->setPlacementStatus(StatusPLACED);
        tempLocValid = true;
        }
    | K_PLUS K_UNPLACED
        {
        tempLocValid = false;
        }
;

weight: K_PLUS K_WEIGHT NUMBER 
        { }
        ;

end_comps: K_END K_COMPS
        {
        //pNetlist->setNumCells(runningCellIdx);
        //pNetlist->sortCells();
        }
;

nets_section:  start_nets net_rules end_nets
           { //printf("nets_section parsed\n"); 
           }
;

start_nets: K_NETS NUMBER K_SEMCOL
    { //printf("start_nets parsed\n");
    bInSNets=0;
    double numNets = $<dval>2;
    nNets = (int)numNets;
    if (numNets !=(double)(nNets))
        deferror("nonintegral number of nets!\n");

    pp->tempAllNets.reserve(pp->tempAllNets.size()+nNets);

    //pNetlist->createNets(nNets,nNetsSoFar);
    }
;

net_rules: /* empty */
    | net_rules one_net { //printf("net_rules parsed\n"); 
                        }
;

one_net: net_and_connections net_options K_SEMCOL
    { //printf("one_net parsed\n");
    *(const_cast<dbNetType*>(&(pTempNet->_type))) = dbNetType(tempNetUse);

    }
;

net_and_connections: net_start net_connections
    { //printf("net_and_connection parsed\n");
    if (!pTempNet)
        {
        pTempNet = new dbNet(tempNetName,
                             (bInSNets)?(runningSNetIdx++):(runningNetIdx++),
                             dbNetType(dbNetType::UNKNOWN), //use is not set
                                                            //until net_options
                                                            //rule (or
                                                            // snet_options) 
                                                            // rule is parsed.
                             tempPins);

        if (bInSNets)
            {
            pp->tempAllSNets.push_back(pTempNet);
            }
        else
            pp->tempAllNets.push_back(pTempNet);

        pair<char const* const,dbNet*> const &pref =
              pair<char const* const,dbNet*>(
                                          tempstr=strdup(tempNetName),
                                          pTempNet) ;

        pp->stringsToClean.push_back(tempstr);

                                          
        ParserLEFDEF::netHashMapType &condHash = (bInSNets)? pp->SnetHash
                                                           : pp->netHash;
         
    
        pair<ParserLEFDEF::netHashMapType::iterator,bool> p =
             condHash.insert(pref);

        if (!p.second)
            {
            sprintf(errtxt,"internal error at net %s "
                           "in line %d of DEF",tempNetName,defLinenum);
            abkfatal(0,errtxt);
            }
        }
    else
        {
        // we have to add connections to an existing net from
        // an external pin
        ctainerPinsLocal::iterator iP;
        for (iP = tempPins.begin() ; iP != tempPins.end() ; iP++ )
            {
            dbPin &pin = *(*iP);
            if (pin._net)
                {
                if (pin._net!=pTempNet)
                   {
                   sprintf(errtxt,
                     "Pin %s of cell %s found on two nets, %s and %s"
                     " when adding connections to external pin",
                       pin.getMasterPinName(),
                       pin.getCell().getName(),
                       pin._net->getName(),
                       tempNetName);
                   abkfatal(0,errtxt);
                   }

                }
            else
                pTempNet->_pins.push_back((*iP));
            }
        }
    if (logicalPinReferenced)
        pTempNet->_hasLogicalPin=true;

    ctainerPinsLocal::iterator iP;
    for (iP = tempPins.begin() ; iP != tempPins.end() ; iP++ )
        {
        dbPin &pin = *(*iP);
        if (pin._net)
            {
            //const char * oldName=pin._net->getName();
            if (pin._net != pTempNet)
                {
                sprintf(errtxt,"Pin %s of cell %s found on two nets, %s and %s",
                               pin.getMasterPinName(),
                               pin.getCell().getName(),
                               pin._net->getName(),
                               tempNetName);
                abkfatal(0,errtxt);
                }
            }
        pin._net = pTempNet;
        }
    }
;

net_start: K_MINUS  STRING 
    {

    logicalPinReferenced=false;

    strcpy(tempNetName,$2.str);
    tempNetUse = dbNetType::SIGNAL; //changed from UNKNOWN mro 9-11-97
    tempPins.clear();
    tempWiring.clear();
    pTempCell = NULL;
    if ($2.DoDelete) free($2.str);

    ParserLEFDEF::netHashMapType &condHash = (bInSNets)? pp->SnetHash
                                                       : pp->netHash;

    ParserLEFDEF::netHashMapType::iterator iN = condHash
                                           .find((char const*)tempNetName);

    if (iN == condHash.end())
        pTempNet=NULL;
     else
        pTempNet = (*iN).second;

    }
;

net_connections: /* empty */
    | net_connections net_connection {}
;

net_connection: K_ORDBR1 STRING STRING  plus_synth K_ORDBR2
    { //printf("net_connection parsed\n");

    bool isIOpin=false;

    if (strcmp($2.str,"PIN")==0)
        {
        /*
        if (!pp->bIncludeLogicalPins ||
               (bInSNets && !pp->bIncludeSpecialPins) )
            {
            if ($2.DoDelete) free($2.str);
            if ($3.DoDelete) free($3.str);
            break;
            }
            */
        strcpy(cellname,"FakedInstantiatedCellFor");
        strcat(cellname,$3.str);
        strcpy(pinname,"FakedMasterPinFor");
        strcat(pinname,$3.str);
        isIOpin=true;
        }
    else
        {
        strcpy(cellname,$2.str);
        strcpy(pinname,$3.str);
        }


    ParserLEFDEF::cellHashMapType::iterator iC 
                 = pp->cellHash.find((const char*)cellname);

    if (iC == pp->cellHash.end())
        {
        if (!isIOpin)
            {
            sprintf(errtxt,"Cell %s not found in net %s in line %d "
                           "of DEF file",cellname,tempNetName,defLinenum);
            abkfatal(0,errtxt);
            }

        //assume this is a logical pin.  NOTE!!! if the name is
        //just spelled wrong, this code WILL NOT NOTICE!!! mro 001012

        logicalPinReferenced=true;

        }
     else
        {
        pTempCell = (*iC).second;

        ctainerPinsLocal::iterator iP;

        pTempPin = NULL;
        for (iP=pTempCell->_pins.begin();iP!=pTempCell->_pins.end();iP++)
            {
            if (strcmp((*iP)->_master._name,pinname)==0)
                {
                pTempPin = *iP;
                break;
                }
            }

        if (!pTempPin)
            {
            sprintf(errtxt,"Pin %s not found in cell %s in net %s in line %d "
                           "of DEF file",$3.str,$2.str,tempNetName,defLinenum);
            abkfatal(0,errtxt);
            }

        tempPins.push_back(pTempPin);
        }
    
    if ($2.DoDelete) free($2.str);
    if ($3.DoDelete) free($3.str);
    }
    | K_ORDBR1 K_STAR STRING K_ORDBR2
    {
    if ($3.DoDelete) free ($3.str);
    }
    | K_MUSTJOIN K_ORDBR1 STRING STRING K_ORDBR2
        {   
        if ($3.DoDelete) free ($3.str);
        if ($4.DoDelete) free ($4.str);
        }
;

plus_synth:  /* empty */
    | K_PLUS K_SYNTH 
    ;

/* These are all the optional fields for a net that go after the '+' */
net_options: /* empty */
    | net_options net_option
        ;

net_option: net_type paths
    {  }
    | K_PLUS K_SCANFIX
        { }
    | K_PLUS K_XTALK NUMBER
        { }
    | K_PLUS K_SOURCE STRING
        {
        if ($3.DoDelete) free ($3.str);
        }
    | K_PLUS K_ORIGINAL STRING
        {
        if ($3.DoDelete) free ($3.str);
        }
    | K_PLUS K_PATTERN STRING
        {
        if ($3.DoDelete) free ($3.str);
        }
    | K_PLUS K_WEIGHT NUMBER
        {
	  pTempNet->_weight = double($3);
	}
    | K_PLUS K_ESTCAP NUMBER
        { }
    | use_option
        {
        }
    | prop_spec
        {}
;

use_option: K_PLUS K_USE use_spec {}
;

use_spec: K_SIGNAL {tempNetUse = dbNetType::SIGNAL}
        | K_POWER  {tempNetUse = dbNetType::POWER}
        | K_GROUND {tempNetUse = dbNetType::GROUND}
        | K_CLOCK  {tempNetUse = dbNetType::CLOCK}
        | K_TIEOFF {tempNetUse = dbNetType::TIEOFF}
        | K_ANALOG {tempNetUse = dbNetType::ANALOG}
;

net_type: K_PLUS K_FIXED
        { }
    | K_PLUS K_COVER
        { }
    | K_PLUS K_ROUTED
        { }
        ;

paths:  
    layer_path  new_paths
    { 
    }
;

new_paths: /* empty */
    | new_paths new_keyword layer_path
    {  }
;

new_keyword: K_NEW
    {
    }
;
    

layer_path: layer_path_start pt_or_via_opts 
    {
    if (!bInSNets)
        break;     // We don't process regular wiring
    unsigned i;

    //Take care of final rectangle

    if (fabs(wireMinX-wireMaxX) > 1e-4 || fabs(wireMinY-wireMaxY) > 1e-4)
        {
        BBoxExtended b(wireMinX-tempWidth/2,wireMinY-tempWidth/2,
                        wireMaxX+tempWidth/2,wireMaxY+tempWidth/2);

        wireGeoms.push_back(b);
                                  
        wrongWay.push_back(pathDirection != pTempLayer->getDirection());
       }

    for (i=0;i<wireGeoms.size();i++)
        {
        //Next block says:  If we have overlapping rectangles, pare
        //redundant pieces off the wrong-way-routed wires.

        if (wrongWay[i])
           {
           if (i>0)
               wireGeoms[i].snip(wireGeoms[i-1]);
           if (i<wireGeoms.size()-1)
               wireGeoms[i].snip(wireGeoms[i+1]);
           }

        tempWiring.push_back(new dbLayerGeom(wireGeoms[i],*pTempLayer));
        }
    }
;

layer_path_start:  layer_name NUMBER poss_shape_opt path_start_pt 
                        {
                        tempWidth=$2;
                        wireGeoms.clear();
                        wrongWay.clear();
                        }
                | layer_name poss_taper path_start_pt
                    {
                    if (bInSNets)
                        {
                        sprintf(errtxt,"In line %d of DEF, special wiring "
                                       "given in regular wiring format",
                                        defLinenum);
                        abkfatal(0,errtxt);
                        }

                    }
;

poss_taper : /*empty*/
           | K_TAPER {}
;


poss_shape_opt : /*empty*/
               | shape_opt{}
;

shape_opt : K_PLUS K_SHAPE shape_spec {}
;

shape_spec: K_ABUTMENT {}
          | K_RING {}
          | K_FEEDTHRU {}
          | K_STRIPE {}
;

          

path_start_pt : pt {lastPoint=$1;
                    pathDirection = oldPathDirection = dbLayer::NOPREFERRED;
                    wireMinX=wireMaxX=$1.x;
                    wireMinY=wireMaxY=$1.y;}
;
   
pt_or_via_opts:  /* empty  */
    | pt_or_via_opts pt_or_via
;

pt_or_via: pt 
    {

    if (pathDirection != oldPathDirection 
         && oldPathDirection != dbLayer::NOPREFERRED)
         {
         BBoxExtended b(wireMinX-tempWidth/2,wireMinY-tempWidth/2,
                        wireMaxX+tempWidth/2,wireMaxY+tempWidth/2);

         wireGeoms.push_back(b);
                                  
         wrongWay.push_back(oldPathDirection != pTempLayer->getDirection());

         wireMinX = wireMaxX = lastPoint.x;
         wireMinY = wireMaxY = lastPoint.y;
         }

    lastPoint = $1;
    if ($1.x<wireMinX) wireMinX = $1.x;
    if ($1.x>wireMaxX) wireMaxX = $1.x;
    if ($1.y<wireMinY) wireMinY = $1.y;
    if ($1.y>wireMaxY) wireMaxY = $1.y;
    oldPathDirection = pathDirection;
    }
    | via_name //note that these geometries are not included in
               //wireGeoms, but are added directly to tempWiring
    {
    typedef ctainerLayerGeoms const &rlg;
    rlg LGref(pTempVia->getGeometries());

    ctainerLayerGeoms::const_iterator iLG;

    for (iLG=LGref.begin();iLG!=LGref.end();iLG++)
        {
        BBox b=*(*iLG);
        b.TranslateBy(lastPoint);
        tempWiring.push_back(new dbLayerGeom(b,(*iLG)->getLayer()));
        }

    }
;

end_nets: K_END K_NETS 
    { }
;

via_name: via_name_tolerant
    {
    if (!pTempVia)
        {
        sprintf(errtxt,"via %s not found in line %d of DEF",
                 tempViaName,defLinenum);
        abkfatal(pTempVia,errtxt);
        }
    }
;

//the next rule is "tolerant" in the sense that if the
//name is not found, it doesn't crash; it just sets
//pTempVia to NULL.  That means we can check whether
//a particular via was declared in LEF or not.

via_name_tolerant: STRING_not_NEW
    {
    strcpy(tempViaName,$1.str);
    if ($1.DoDelete) free($1.str);
    ParserLEFDEF::viaHashMapType::iterator iV
                   = pp->viaHash.find((char const*)tempViaName);
    if (iV == pp->viaHash.end())
         pTempVia = NULL;
    else
         pTempVia = (*iV).second;
    }
;

snets_section :  start_snets snet_rules end_snets
        ;

snet_rules: /* empty */
    | snet_rules snet_rule
        ;

snet_rule: net_and_connections snet_options K_SEMCOL
    {
    *(const_cast<dbNetType*>(&(pTempNet->_type))) = dbNetType(tempNetUse);
    pTempNet->_wiring = tempWiring;
    }
;


snet_options: /* empty */
    | snet_options snet_option
;

snet_option: snet_width | snet_voltage | 
    snet_spacing | net_option
;

snet_width: K_PLUS K_WIDTH STRING NUMBER
    {
    if ($3.DoDelete) free($3.str);
    }
;

snet_voltage: K_PLUS K_VOLTAGE NUMBER
        { }
;

snet_spacing: K_PLUS K_SPACING STRING NUMBER
    {
    if ($3.DoDelete) free($3.str);
    }
;

start_snets: K_SNETS NUMBER K_SEMCOL
    {
    double numSNets = $<dval>2;
    nSNets = (int)numSNets;
    if (nSNets !=(double)(nSNets))
        deferror("nonintegral number of special nets!\n");
//    pNetlist->createSNets(nSNets,nSNetsSoFar);
    bInSNets=1; 
    }
;

end_snets: K_END K_SNETS 
    {
    }
        ;

scan_section: scan_start scan_rules scan_end
;

scan_start: K_SCAN NUMBER K_SEMCOL
    { }
    ;

scan_rules: /* empty */
    | scan_rules one_scan
    ;

one_scan: scan_name scan_opts K_SEMCOL
;

scan_name: K_MINUS  STRING
    {
    if ($2.DoDelete) free ($2.str);
    }
;

scan_opts: scan_opt
    | scan_opts scan_opt {}
;

scan_opt: K_PLUS K_COMM_SCAN scan_in_pin scan_out_pin
    | K_PLUS K_START STRING STRING
    {
    if ($3.DoDelete) free($3.str);
    if ($4.DoDelete) free($4.str);
    }
    | K_PLUS K_FLOAT scan_elements
    { }
    | K_PLUS K_ORDER scan_elements
    { }
    | K_PLUS K_STOP  STRING STRING
    {
    if ($3.DoDelete) free($3.str);
    if ($4.DoDelete) free($4.str);
    }
;

scan_elements: /* empty */
    | scan_elements scan_element
    { }
;

scan_element:  STRING scan_pin_opt
    {
    if ($1.DoDelete) free ($1.str);
    }
;

scan_pin_opt: /* empty */
    | scan_in_pin scan_out_pin
    | scan_in_pin
    | scan_out_pin
;

scan_in_pin: K_ORDBR1 K_IN  STRING K_ORDBR2
    {
    if ($2.DoDelete) free ($2.str); //KLUDGE
    if ($3.DoDelete) free ($3.str);
    }
;

scan_out_pin: K_ORDBR1 K_OUT  STRING K_ORDBR2
    {
    if ($2.DoDelete) free ($2.str); //KLUDGE
    if ($3.DoDelete) free ($3.str);
    }
;

scan_end: K_END K_SCAN {}
;

groups_section: groups_start group_rules groups_end {}
;

        

groups_start: K_GROUPS NUMBER K_SEMCOL
    { }
;

group_rules: /* empty */
    | group_rules group_rule
;

group_rule: start_group group_members group_options K_SEMCOL 
    {
    if (nNumGroupRegions>1)
        {
        sprintf(errtxt,"More than one \"+ REGION\" keyword for a single"
                       " group before line %d in DEF.  This is contrary"
                       "\nto my reading of the DEF manual (but obviously"
                       " I wasn't really sure :-)  Please contact Mike. ",
                       defLinenum);
        abkfatal(0,errtxt);
        }

    if (bSoft)
        {
        //LOOK! this is somewhat wasteful in the case that the region
        //has been specified here by points (instead of by name); we're
        //creating essentially the same object twice.

        if (pTempRegion) //region specified)
            pTempRegion = new parserRegion(runParRegIdx++,
                                           *pTempRegion,
                                           tempMaxX,tempMaxY,tempMaxHp);
        else
            pTempRegion = new parserRegion(runParRegIdx++,false,DBL_MAX,true,
                                           tempMaxX,tempMaxY,tempMaxHp);

        pp->regions.push_back(pTempRegion);
        }

    std::vector<dbCell*>::iterator iC;
    for (iC=tempCompCache.begin();iC!=tempCompCache.end();iC++)
        {
        compRegionPair thisPair((*iC),pTempRegion);
//      compRegionPair thisPair(pTempCell,pTempRegion);
        pp->compRegions.push_back(thisPair);
        }

    }
;

start_group: K_MINUS STRING 
    {
    tempCompCache.clear();
    bSoft        = false;
    pTempRegion  = NULL;
    pTempCell    = NULL;
    nNumGroupRegions  = 0;
    tempMaxX = tempMaxY = tempMaxHp = int(pdb->context._bbox.getHalfPerim());
    strcpy(tempGroupName,$2.str);
    if ($2.DoDelete) free ($2.str);
    }
;

group_members: group_member      {}
    | group_members group_member {}
;

group_member: STRING
    {
    ParserLEFDEF::cellHashMapType::iterator iC 
                 = pp->cellHash.find($1.str);

    if (iC == pp->cellHash.end())
        {
        sprintf(errtxt,"Cell %s of group %s not found in line %d "
                       "of DEF file",$1.str,tempGroupName,defLinenum);
        strcat(errtxt,"\nPossible regular expression?  If so please"
                      " edit file; regexps not supported");
        abkfatal(0,errtxt);
        }
     else
        pTempCell = (*iC).second;
    tempCompCache.push_back(pTempCell);
    if ($1.DoDelete) free ($1.str);
    }
;

group_options: /* empty */
    | group_options group_option {}
;

group_option: K_PLUS K_SOFT group_soft_options 
                   {
                   bSoft = true;
                   }
            | K_PLUS K_REGION grp_region {nNumGroupRegions++;}
            | K_PLUS K_PROPERTY STRING NUMBER 
                  {
                  abkwarn(0,"Properties not supported in groups");
                  if ($3.DoDelete) free ($3.str);
                  }
;

grp_region: bbox  {
                  pTempRegion = new parserRegion(runParRegIdx++);
                  pTempRegion->boxes.push_back($1);
                  pp->regions.push_back(pTempRegion); //for cleanup
                  }
          | STRING 
                  {
                    ParserLEFDEF::regionHashMapType::iterator iR = 
                                pp->regionHash.find((char const*)($1.str));

                    if (iR == pp->regionHash.end())
                        {
                        pTempRegion=NULL;
                        sprintf(errtxt,"Region %s not found "
                                       "in line %d of DEF file",$1.str,
                                       defLinenum);
                        abkfatal(pTempRegion!=NULL,errtxt);
                        }
                     else
                        pTempRegion = (*iR).second;

                    if ($1.DoDelete) free ($1.str);
                  }
;
 
group_soft_options: /* empty */
    | group_soft_options group_soft_opt {}
;


group_soft_opt:  K_MAXX NUMBER {tempMaxX=int($2)}
                | K_MAXY NUMBER {tempMaxY=int($2)}
                | K_MAXHALFPERIMETER NUMBER {tempMaxHp=int($2)}
;

groups_end: K_END K_GROUPS {}
;

constraint_section: constraints_start constraint_rules constraints_end{}
;

constraints_start: K_CONSTRAINTS NUMBER K_SEMCOL {}
;

constraint_rules: /* empty */
    | constraint_rules constraint_rule {} 
;

constraint_rule:   operand_rule {}
         | wiredlogic_rule      {}
;

operand_rule: K_MINUS operand delay_specs K_SEMCOL
    | K_MINUS K_SUM K_ORDBR1 operand operands K_ORDBR2 delay_specs K_SEMCOL
    ;

operands:  /* empty */
    | K_COMMA  operand operands
    ;

operand: K_NET  STRING 
    {
    if ($2.DoDelete) free ($2.str);
    }
    | K_PATH STRING STRING STRING STRING
        { 
        if ($2.DoDelete) free ($2.str);
        if ($3.DoDelete) free ($3.str);
        if ($4.DoDelete) free ($4.str);
        if ($5.DoDelete) free ($5.str);
        }
    ;

wiredlogic_rule: K_WIREDLOGIC STRING K_MAXDIST NUMBER K_SEMCOL
    { 
    if ($2.DoDelete) free ($2.str);
    }
;

delay_specs: /* empty */
    | delay_specs delay_spec
    ;

delay_spec: K_PLUS K_RISEMIN NUMBER 
        { }
    | K_PLUS K_RISEMAX NUMBER 
        { }
    | K_PLUS K_FALLMIN NUMBER 
        { }
    | K_PLUS K_FALLMAX NUMBER 
        { }
    ;

constraints_end: K_END K_CONSTRAINTS
    { }
;

iotimings_section
    : K_IOTIMINGS  NUMBER K_SEMCOL iotimings_list K_END K_IOTIMINGS {}
;

iotimings_list
    :
    | iotimings_list iotiming_spec {}
;


iotiming_spec
    : K_MINUS iotiming_pin iotiming_trait_list K_SEMCOL {}
;

iotiming_pin
    : K_ORDBR1 STRING STRING K_ORDBR2 // could be either "comp pin" or "PIN name"
    {
    if ($2.DoDelete) free ($2.str);
    if ($3.DoDelete) free ($3.str);
    }
;

iotiming_trait_list
    :
    | iotiming_trait_list K_PLUS iotiming_trait
;

pinproperties_section
    : K_PINPROPERTIES NUMBER K_SEMCOL pinproperties_list K_END K_PINPROPERTIES
      {}
;

pinproperties_list
    :
    | pinproperties_list pinproperty_stmt
;

pinproperty_stmt
    : K_MINUS STRING STRING pinprop_list K_SEMCOL
      {
      if ($2.DoDelete) free ($2.str); //$2.str is either PIN or a comp name
      if ($3.DoDelete) free ($3.str); //$3.str is pin name
      }
;
pinprop_list
    :
    | pinprop_list prop_spec {}
;

prop_spec
    : K_PLUS K_PROPERTY prop_spec0_list {}
    {
    }
;

prop_spec0_list
    :
    |prop_spec0_list prop_spec0 {}
;

prop_spec0 
    : STRING prop_val
     {
     if ($1.DoDelete) free ($1.str); //$1.str is property name
     }
;

iotiming_trait
    : r_or_f K_VARIABLE NUMBER NUMBER {}
    | r_or_f K_SLEWRATE NUMBER NUMBER {}
    | K_CAPACITANCE NUMBER {}
    | K_DRIVECELL STRING  poss_drivecell_pins poss_drivecell_parallel
             {if ($2.DoDelete) free ($2.str);}
;

r_or_f
    : K_RISE {}
    | K_FALL {}
;

poss_drivecell_pins
    :
    | poss_drivecell_from K_TOPIN STRING {if ($3.DoDelete) free ($3.str);}
;

poss_drivecell_from
    :
    | K_FROMPIN STRING {if ($2.DoDelete) free ($2.str);}
;

poss_drivecell_parallel
    :
    | K_PARALLEL NUMBER {}
;



STRING_not_NEW: STRING_not_keyword{$$=$1;}
| K_HISTORY {$$=$1;}
| K_DESIGN {$$=$1;}
| K_VIAS {$$=$1;}
| K_TECH {$$=$1;}
| K_UNITS {$$=$1;}
| K_ARRAY {$$=$1;}
| K_FLOORPLAN {$$=$1;}
| K_LRECT {$$=$1;}
| K_SITE {$$=$1;}
| K_DIEAREA {$$=$1;}
| K_PINS {$$=$1;}
| K_DCAP {$$=$1;}
| K_MINPINS {$$=$1;}
| K_WIRECAP {$$=$1;}
| K_TRACKS {$$=$1;}
| K_GCELLGRID {$$=$1;}
| K_SYNTH {$$=$1;}
| K_DO {$$=$1;}
| K_BY {$$=$1;}
| K_STEP {$$=$1;}
| K_LAYER {$$=$1;}
| K_XTALK {$$=$1;}
| K_COMPS {$$=$1;}
| K_COMP_GEN {$$=$1;}
| K_SOURCE {$$=$1;}
| K_WEIGHT {$$=$1;}
| K_FIXED {$$=$1;}
| K_COVER {$$=$1;}
| K_PLACED {$$=$1;}
| K_FOREIGN {$$=$1;}
| K_REGION {$$=$1;}
| K_NETS {$$=$1;}
| K_MUSTJOIN {$$=$1;}
| K_ORIGINAL {$$=$1;}
| K_USE {$$=$1;}
| K_ORDER {$$=$1;}
| K_FLOAT {$$=$1;}
| K_SCAN {$$=$1;}
| K_COMM_SCAN {$$=$1;}
| K_START {$$=$1;}
| K_STOP {$$=$1;}
| K_IN {$$=$1;}
| K_OUT {$$=$1;}
| K_PATTERN {$$=$1;}
| K_SCANFIX {$$=$1;}
| K_ESTCAP {$$=$1;}
| K_ROUTED {$$=$1;}
| K_SNETS {$$=$1;}
| K_WIDTH {$$=$1;}
| K_VOLTAGE {$$=$1;}
| K_SPACING {$$=$1;}
| K_N {$$=$1;}
| K_S {$$=$1;}
| K_E {$$=$1;}
| K_W {$$=$1;}
| K_FN {$$=$1;}
| K_FE {$$=$1;}
| K_FS {$$=$1;}
| K_FW {$$=$1;}
| K_GROUPS {$$=$1;}
| K_START_GROUP {$$=$1;}
| K_SOFT  {$$=$1;}
| K_MAXX  {$$=$1;}
| K_MAXY  {$$=$1;}
| K_MAXHALFPERIMETER {$$=$1;}
| K_CONSTRAINTS  {$$=$1;}
| K_NET  {$$=$1;}
| K_PATH  {$$=$1;}
| K_RISEMIN  {$$=$1;}
| K_RISEMAX {$$=$1;}
| K_FALLMIN {$$=$1;}
| K_FALLMAX   {$$=$1;}
| K_WIREDLOGIC {$$=$1;}
| K_MAXDIST  {$$=$1;}
| K_ASSERTIONS  {$$=$1;}
| K_SUM {$$=$1;}
| K_DISTANCE  {$$=$1;}
| K_MICRONS {$$=$1;}
| K_LE  {$$=$1;}
| K_GE  {$$=$1;}
| K_NE  {$$=$1;}
| K_END  {$$=$1;}
| K_DCAPS  {$$=$1;}
| K_ROW  {$$=$1;}
| K_SIGNAL {$$=$1;}
| K_POWER  {$$=$1;}
| K_GROUND {$$=$1;}
| K_CLOCK  {$$=$1;}
| K_TIEOFF {$$=$1;}
| K_ANALOG {$$=$1;}
| K_INPUT   {$$=$1;}
| K_OUTPUT  {$$=$1;}
| K_INOUT   {$$=$1;}
| K_TRISTATE {$$=$1;}
| K_DIRECTION  {$$=$1;}
| K_SPECIAL  {$$=$1;}
| K_SHAPE   {$$=$1;}
| K_ABUTMENT     {$$=$1;}
| K_RING      {$$=$1;}
| K_FEEDTHRU  {$$=$1;}
| K_TAPER      {$$=$1;}
| K_X         {$$=$1;}
| K_Y         {$$=$1;}
| K_ON         {$$=$1;}
| K_OFF         {$$=$1;}
| K_CASE         {$$=$1;}
| K_PROPERTYDEFINITIONS         {$$=$1;}
| K_SPECIALNET         {$$=$1;}
| K_GROUP         {$$=$1;}
| K_COMPONENTPIN         {$$=$1;}
| K_INTEGER         {$$=$1;}
| K_REAL         {$$=$1;}
| K_STRING         {$$=$1;}
| K_RANGE         {$$=$1;}
| K_COMPONENT         {$$=$1;}
| K_RISE         {$$=$1;}
| K_FALL         {$$=$1;}
| K_VARIABLE         {$$=$1;}
| K_SLEWRATE         {$$=$1;}
| K_CAPACITANCE         {$$=$1;}
| K_DRIVECELL         {$$=$1;}
| K_FROMPIN         {$$=$1;}
| K_TOPIN         {$$=$1;}
| K_PARALLEL         {$$=$1;}
| K_PIN         {$$=$1;}
| K_IOTIMINGS         {$$=$1;}
;

STRING_list
    :
    | STRING_list STRING {if ($2.DoDelete) free ($2.str)}
;

STRING: STRING_not_NEW{$$=$1}
      | K_NEW{$$=$1;}
;
%%



void deferror (const char *s) 
{ 
  printf("%s\n At line number %d of DEF\n",s,defLinenum+1);
  fprintf(stderr,"%s\n At line number %d of DEF\n",s,defLinenum+1);
  fflush(stdout);
  fflush(stderr);
  exit(-1);
}


