%{
#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

#ifdef WIN32
#include <io.h>
#endif

#include <string.h>
#include "Geoms/plGeom.h"
#include "LEFDEFpoint.h"
typedef struct lexsstr
    {
    const char *str;
    int  DoDelete;
    } lexemeStruct;

extern char *strCdup(char *s);
extern int bCaseSens;

#include "parserLEFDEF.h"
#include "defparse.tab.h"


//static int end_of_file = 0;
int defLinenum = 0;
static int errflag = 0;

%}

/* Definitions */

white          [\t ]
eoln        \n
letter      [a-zA-Z]
eletter     [a-zA-Z_/=|\[\]\(\)\<\>\.!\{\}\-\$\:\\]
digit        [0-9]
num_int      {digit}+
pnnum_int    -?{digit}+ 
quote       \'\'
chr     [^']
/* chr_lit      \'(({chr})|({quote}))*\' */
nonquote    [^"]
quoted_str  \"({nonquote})*\"
comma       \,
semcol      \;
colon       \:
dot     \.
ord_br1     \(
ord_br2     \)
sq_br1      \[
sq_br2      \]
rel_op      \=|\>|\<|\>\=|\<\=|\<\>
plus            \+
minus           \-
sign        \+|\-
star            \*
asgn        =
notsem     [^\;]
gletter   {eletter}|\*|#

%p 6000

%x INSIDEBRACKETS
%x HISTORY
%option    never-interactive
%%

#.*                           {  }

<INITIAL,INSIDEBRACKETS>{white}+            {  }            
<INITIAL,INSIDEBRACKETS>{eoln}              {  defLinenum++ ;}
<INITIAL,INSIDEBRACKETS>{plus}          {  //printf("K_PLUS\n");
                  deflval.string.DoDelete=0;deflval.string.str="+";return(K_PLUS);}
{minus}         {  deflval.string.DoDelete=0;deflval.string.str="-";return(K_MINUS);}
<INITIAL,INSIDEBRACKETS>{star}                  {  
                                                deflval.string.DoDelete=0;deflval.string.str="*";
                                                return(K_STAR);} 
{asgn}          {deflval.string.DoDelete=0;deflval.string.str="=";return(K_ASGN);} 
HISTORY                         {  deflval.string.DoDelete=0;deflval.string.str="HISTORY";
                                   BEGIN(HISTORY);
                                   return(K_HISTORY);}
DESIGN                          {  deflval.string.DoDelete=0;deflval.string.str="DESIGN";
                                   return(K_DESIGN);}
VIAS                            {  deflval.string.DoDelete=0;deflval.string.str="VIAS";
                                   return(K_VIAS);}
TECHNOLOGY                            {deflval.string.DoDelete=0;deflval.string.str="TECHNOLOGY";
                                       return(K_TECH);}
DIEAREA                         {  deflval.string.DoDelete=0;deflval.string.str="DIEAREA";
                                   return(K_DIEAREA);}
UNITS                           {  deflval.string.DoDelete=0;deflval.string.str="UNITS";
                                   return(K_UNITS);}
ARRAY                           {  deflval.string.DoDelete=0;deflval.string.str="ARRAY";
                                   return(K_ARRAY);}
FLOORPLAN                       {  deflval.string.DoDelete=0;deflval.string.str="FLOORPLAN";
                                   return(K_FLOORPLAN);}
RECT                           {  deflval.string.DoDelete=0;deflval.string.str="RECT";
                                  return(K_LRECT);}
SITE                            { // printf("K_SITE\n");
                                   deflval.string.DoDelete=0;deflval.string.str="SITE";
                                   return(K_SITE);}
PINS                            {  deflval.string.DoDelete=0;deflval.string.str="PINS";
                                   return(K_PINS);}
DEFAULTCAP                      {  deflval.string.DoDelete=0;deflval.string.str="DEFAULTCAP";
                                   return(K_DCAPS);}
MINPINS                         {  deflval.string.DoDelete=0;deflval.string.str="MINPINS";
                                   return(K_MINPINS);}
WIRECAP                         {  deflval.string.DoDelete=0;deflval.string.str="WIRECAP";
                                   return(K_WIRECAP);}
TRACKS                          {  deflval.string.DoDelete=0;deflval.string.str="TRACKS";
                                   return(K_TRACKS);}
GCELLGRID                       {  deflval.string.DoDelete=0;deflval.string.str="GCELLGRID";
                                   return(K_GCELLGRID);}
<INITIAL,INSIDEBRACKETS>SYNTHESIZED   {  deflval.string.DoDelete=0;deflval.string.str="SYNTHESIZED";
                                   return(K_SYNTH);}
DO                              {  deflval.string.DoDelete=0;deflval.string.str="DO";
                                   return(K_DO); }
BY                              {  deflval.string.DoDelete=0;deflval.string.str="BY";
                                   return(K_BY);}
STEP                            {  deflval.string.DoDelete=0;deflval.string.str="STEP";
                                   return(K_STEP);}
LAYER                           {  deflval.string.DoDelete=0;deflval.string.str="LAYER";
                                   return(K_LAYER);}
XTALK                           {  deflval.string.DoDelete=0;deflval.string.str="XTALK";
                                   return(K_XTALK);}
COMPONENTS                      {  deflval.string.DoDelete=0;deflval.string.str="COMPONENTS";
                                   return(K_COMPS);}
GENERATE                        {  deflval.string.DoDelete=0;deflval.string.str="GENERATE";
                                   return(K_COMP_GEN);}
SOURCE                        {  deflval.string.DoDelete=0;deflval.string.str="SOURCE";
                                 return(K_SOURCE);}
WEIGHT                        {  deflval.string.DoDelete=0;deflval.string.str="WEIGHT";
                                 return(K_WEIGHT);}
FIXED                       {  deflval.string.DoDelete=0;deflval.string.str="FIXED";
                               return(K_FIXED);}
COVER                        {  deflval.string.DoDelete=0;deflval.string.str="COVER";
                                return(K_COVER);}
PLACED                       { // printf("K_PLACED\n");
                                deflval.string.DoDelete=0;deflval.string.str="PLACED";
                                return(K_PLACED);}
UNPLACED                       { 
                                deflval.string.DoDelete=0;deflval.string.str="UNPLACED";
                                return(K_UNPLACED);}
FOREIGN                        {  deflval.string.DoDelete=0;deflval.string.str="FOREIGN";
                                  return(K_FOREIGN);}
REGION                        {  deflval.string.DoDelete=0;deflval.string.str="REGION";
                                 return(K_REGION);}
SPECIALNETS                        {  deflval.string.DoDelete=0;deflval.string.str="SPECIALNETS";
                                      return(K_SNETS);}
NETS                        {  deflval.string.DoDelete=0;deflval.string.str="NETS";
                               return(K_NETS);}
MUSTJOIN                       {  deflval.string.DoDelete=0;deflval.string.str="MUSTJOIN";
                                  return(K_MUSTJOIN);}
ORIGINAL                        {  deflval.string.DoDelete=0;deflval.string.str="ORIGINAL";
                                   return(K_ORIGINAL);}
USE                        {  deflval.string.DoDelete=0;deflval.string.str="USE";
                              return(K_USE);}
ORDERED                        {  deflval.string.DoDelete=0;deflval.string.str="ORDERED";
                                  return(K_ORDER);}
FLOATING                        {  deflval.string.DoDelete=0;deflval.string.str="FLOATING";
                                   return(K_FLOAT);}
SCANCHAINS                     {  deflval.string.DoDelete=0;deflval.string.str="SCANCHAINS";
                                  return(K_SCAN);}
COMMONSCANPINS                 {  deflval.string.DoDelete=0;deflval.string.str="COMMONSCANPINS";
                                  return(K_COMM_SCAN);}
START                        {  deflval.string.DoDelete=0;deflval.string.str="START";
                                return(K_START);}
STOP                        {  deflval.string.DoDelete=0;deflval.string.str="STOP";
                               return(K_STOP);}
VERSION                        {  deflval.string.DoDelete=0;deflval.string.str="VERSION";
                               return(K_VERSION);}
PATTERNNAME                        {  deflval.string.DoDelete=0;deflval.string.str="PATTERNNAME";
                               return(K_PATTERNNAME);}
ROW                        {  deflval.string.DoDelete=0;deflval.string.str="ROW";
                               return(K_ROW);}
SIGNAL                        {  deflval.string.DoDelete=0;deflval.string.str="SIGNAL";
                               return(K_SIGNAL);}
POWER                        {  deflval.string.DoDelete=0;deflval.string.str="POWER";
                               return(K_POWER);}
GROUND                        {  deflval.string.DoDelete=0;deflval.string.str="GROUND";
                               return(K_GROUND);}
CLOCK                        {  deflval.string.DoDelete=0;deflval.string.str="CLOCK";
                               return(K_CLOCK);}
TIEOFF                        {  deflval.string.DoDelete=0;deflval.string.str="TIEOFF";
                               return(K_TIEOFF);}
ANALOG                        {  deflval.string.DoDelete=0;deflval.string.str="ANALOG";
                               return(K_ANALOG);}
REGIONS                        {  deflval.string.DoDelete=0;deflval.string.str="REGIONS";
                               return(K_REGIONS);}
PROPERTY                        {  deflval.string.DoDelete=0;deflval.string.str="PROPERTY";
                               return(K_PROPERTY);}
NAMESCASESENSITIVE {deflval.string.DoDelete=0;deflval.string.str="NAMESCASESENSITIVE";return(K_CASE);}
ON                        {  deflval.string.DoDelete=0;deflval.string.str="ON";
                               return(K_ON);}
OFF                        {  deflval.string.DoDelete=0;deflval.string.str="OFF";
                               return(K_OFF);}
PROPERTYDEFINITIONS {deflval.string.DoDelete=0;deflval.string.str="PROPERTYDEFINITIONS";return(K_PROPERTYDEFINITIONS);}
SPECIALNET {deflval.string.DoDelete=0;deflval.string.str="SPECIALNET";return(K_SPECIALNET);}
GROUP {deflval.string.DoDelete=0;deflval.string.str="GROUP";return(K_GROUP);}
COMPONENTPIN {deflval.string.DoDelete=0;deflval.string.str="COMPONENTPIN";return(K_COMPONENTPIN);}
INTEGER {deflval.string.DoDelete=0;deflval.string.str="INTEGER";return(K_INTEGER);}
REAL {deflval.string.DoDelete=0;deflval.string.str="REAL";return(K_REAL);}
STRING {deflval.string.DoDelete=0;deflval.string.str="STRING";return(K_STRING);}
RANGE {deflval.string.DoDelete=0;deflval.string.str="RANGE";return(K_RANGE);}
COMPONENT {deflval.string.DoDelete=0;deflval.string.str="COMPONENT";return(K_COMPONENT);}
X                        {  deflval.string.DoDelete=1;
                               deflval.string.str=strCdup(yytext);
                               return(K_X);}
Y                        {  deflval.string.DoDelete=1;
                               deflval.string.str=strCdup(yytext);
                               return(K_Y);}
INOUT {deflval.string.DoDelete=0;deflval.string.str="INOUT";return(K_INOUT);}
INPUT {deflval.string.DoDelete=0;deflval.string.str="INPUT";return(K_INPUT);}
OUTPUT {deflval.string.DoDelete=0;deflval.string.str="OUTPUT";return(K_OUTPUT);}
TRISTATE {deflval.string.DoDelete=0;deflval.string.str="TRISTATE";return(K_TRISTATE);}
DIRECTION {deflval.string.DoDelete=0;deflval.string.str="DIRECTION";return(K_DIRECTION);}
SPECIAL {deflval.string.DoDelete=0;deflval.string.str="SPECIAL";return(K_SPECIAL);}
SHAPE {deflval.string.DoDelete=0;deflval.string.str="SHAPE";return(K_SHAPE);}
ABUTMENT {deflval.string.DoDelete=0;deflval.string.str="ABUTMENT";return(K_ABUTMENT);}
RING {deflval.string.DoDelete=0;deflval.string.str="RING";return(K_RING);}
FEEDTHRU {deflval.string.DoDelete=0;deflval.string.str="FEEDTHRU";return(K_FEEDTHRU);}
TAPER {deflval.string.DoDelete=0;deflval.string.str="TAPER";return(K_TAPER);}

<INITIAL,INSIDEBRACKETS>IN    {  deflval.string.DoDelete=1;
                                 deflval.string.str=strCdup(yytext); //KLUDGE
                                 return(K_IN);}
<INITIAL,INSIDEBRACKETS>OUT    {  deflval.string.DoDelete=1;
                                  deflval.string.str=strCdup(yytext); //KLUDGE
                                  return(K_OUT);}
PATTERN                        {  deflval.string.DoDelete=0;deflval.string.str="PATTERN";
                                  return(K_PATTERN);}
SCANFIX                        {  deflval.string.DoDelete=0;deflval.string.str="SCANFIX";
                                  return(K_SCANFIX);}
ESTCAP                        {  deflval.string.DoDelete=0;deflval.string.str="ESTCAP";
                                 return(K_ESTCAP);}
ROUTED                        {  deflval.string.DoDelete=0;deflval.string.str="ROUTED";
                                 return(K_ROUTED);}
NEW                        {  deflval.string.DoDelete=0;deflval.string.str="NEW";
                              return(K_NEW);}
WIDTH                        {  deflval.string.DoDelete=0;deflval.string.str="WIDTH";
                                return(K_WIDTH);}
VOLTAGE                        {  deflval.string.DoDelete=0;deflval.string.str="VOLTAGE";
                                  return(K_VOLTAGE);}
SPACING                        {  deflval.string.DoDelete=0;deflval.string.str="SPACING";
                                  return(K_SPACING);}
N                        {  //printf("K_N\n");
                           deflval.string.DoDelete=1;
                           deflval.string.str=strCdup(yytext);
                           return(K_N);}
S                        {  deflval.string.DoDelete=1;
                            deflval.string.str=strCdup(yytext);
                            return(K_S);}
E                        {  deflval.string.DoDelete=1;
                            deflval.string.str=strCdup(yytext);
                            return(K_E);}
W                        {  deflval.string.DoDelete=1;
                            deflval.string.str=strCdup(yytext);
                            return(K_W);}
FN                       {  deflval.string.DoDelete=0;deflval.string.str="FN";
                            return(K_FN);}
FE                       {  deflval.string.DoDelete=0;deflval.string.str="FE";
                            return(K_FE);}
FS                       {  deflval.string.DoDelete=0;deflval.string.str="FS";
                            return(K_FS);}
FW                       {  deflval.string.DoDelete=0;deflval.string.str="FW";
                            return(K_FW);}
GROUPS                       {  deflval.string.DoDelete=0;deflval.string.str="GROUPS";
                                return(K_GROUPS);}
START_GROUP                       {  deflval.string.DoDelete=0;deflval.string.str="START_GROUP";
                                     return(K_START_GROUP);}
SOFT                        {  deflval.string.DoDelete=0;deflval.string.str="SOFT";
                               return(K_SOFT);}
MAXX                        {  deflval.string.DoDelete=0;deflval.string.str="MAXX";
                               return(K_MAXX);}
MAXY                        {  deflval.string.DoDelete=0;deflval.string.str="MAXY";
                               return(K_MAXY);}
MAXHALFPERIMETER                       {  deflval.string.DoDelete=0;deflval.string.str="MAXHALFPERIMETER";
                                          return(K_MAXHALFPERIMETER);}
CONSTRAINTS                        {  deflval.string.DoDelete=0;deflval.string.str="CONSTRAINTS";
                                      return(K_CONSTRAINTS);}
<INITIAL,INSIDEBRACKETS>NET                        {  deflval.string.DoDelete=0;deflval.string.str="NET";
                                                      return(K_NET);}
<INITIAL,INSIDEBRACKETS>PATH                       {  deflval.string.DoDelete=0;deflval.string.str="PATH";
                                                      return(K_PATH);}
RISEMIN                        {  deflval.string.DoDelete=0;deflval.string.str="RISEMIN";
                                  return(K_RISEMIN);}
RISEMAX                        {  deflval.string.DoDelete=0;deflval.string.str="RISEMAX";
                                  return(K_RISEMAX);}
FALLMIN                        {  deflval.string.DoDelete=0;deflval.string.str="FALLMIN";
                                  return(K_FALLMIN);}
FALLMAX                        {  deflval.string.DoDelete=0;deflval.string.str="FALLMAX";
                                  return(K_FALLMAX);}
WIREDLOGIC                       {  deflval.string.DoDelete=0;deflval.string.str="WIREDLOGIC";
                                    return(K_WIREDLOGIC);}
MAXDIST                        {  deflval.string.DoDelete=0;deflval.string.str="MAXDIST";
                                  return(K_MAXDIST);}
ASSERTIONS                       {  deflval.string.DoDelete=0;deflval.string.str="ASSERTIONS";
                                    return(K_ASSERTIONS);}
SUM                        {  deflval.string.DoDelete=0;deflval.string.str="SUM";
                              return(K_SUM);}
DISTANCE                        {  deflval.string.DoDelete=0;deflval.string.str="DISTANCE";
                                   return(K_DISTANCE);}
MICRONS                        {  deflval.string.DoDelete=0;deflval.string.str="MICRONS";
                                  return(K_MICRONS);}
LE                        {  deflval.string.DoDelete=0;deflval.string.str="LE";
                             return(K_LE);}
GE                        {  deflval.string.DoDelete=0;deflval.string.str="GE";
                             return(K_GE);}
NE                        {  deflval.string.DoDelete=0;deflval.string.str="NE";
                             return(K_NE);}
END                       {  deflval.string.DoDelete=0;deflval.string.str="END";
                             return(K_END);}
BUSBITCHARS                       {  deflval.string.DoDelete=0;deflval.string.str="BUSBITCHARS";
                             return(K_BUSBITCHARS);}
DIVIDERCHAR                       {  deflval.string.DoDelete=0;deflval.string.str="DIVIDERCHAR";
                             return(K_DIVIDERCHAR);}
VARIABLE                       {  deflval.string.DoDelete=0;deflval.string.str="VARIABLE";
                             return(K_VARIABLE);}
SLEWRATE                       {  deflval.string.DoDelete=0;deflval.string.str="SLEWRATE";
                             return(K_SLEWRATE);}
RISE                       {  deflval.string.DoDelete=0;deflval.string.str="RISE";
                             return(K_RISE);}
FALL                       {  deflval.string.DoDelete=0;deflval.string.str="FALL";
                             return(K_FALL);}
CAPACITANCE                       {  deflval.string.DoDelete=0;deflval.string.str="CAPACITANCE";
                             return(K_CAPACITANCE);}
DRIVECELL                       {  deflval.string.DoDelete=0;deflval.string.str="DRIVECELL";
                             return(K_DRIVECELL);}
FROMPIN                       {  deflval.string.DoDelete=0;deflval.string.str="FROMPIN";
                             return(K_FROMPIN);}
TOPIN                       {  deflval.string.DoDelete=0;deflval.string.str="TOPIN";
                             return(K_TOPIN);}
PARALLEL                       {  deflval.string.DoDelete=0;deflval.string.str="PARALLEL";
                             return(K_PARALLEL);}
PIN                       {  deflval.string.DoDelete=0;deflval.string.str="PIN";
                             return(K_PIN);}
IOTIMINGS                       {  deflval.string.DoDelete=0;deflval.string.str="IOTIMINGS";
                             return(K_IOTIMINGS);}
PINPROPERTIES               {  deflval.string.DoDelete=0;deflval.string.str="PINPROPERTIES";
                             return(K_PINPROPERTIES);} 
STRIPE               {  deflval.string.DoDelete=0;deflval.string.str="STRIPE";
                             return(K_STRIPE);} 

<INITIAL,INSIDEBRACKETS>{comma}         {return(K_COMMA);}   
<INITIAL,HISTORY,INSIDEBRACKETS>{semcol}        { BEGIN(INITIAL); return(K_SEMCOL);}
{dot}{dot}      {  return(K_DOT_DOT);}
{dot}           {  return(K_DOT);}
{colon}         {  return(K_COLON);}
{ord_br1}       {  BEGIN(INSIDEBRACKETS);return(K_ORDBR1);}
{quoted_str}    {  return(QUOTED_STR);}
<HISTORY>{notsem}* {
                   unsigned int idx;
                   for (idx=0;idx<strlen(yytext);idx++)
                       {
                       if (yytext[idx] == '\n') defLinenum++;
                       }
                   return(STRING_not_keyword);
                   }
<INSIDEBRACKETS>{ord_br2}       {  BEGIN(INITIAL);return(K_ORDBR2);}
<INITIAL,INSIDEBRACKETS>{pnnum_int}                     {   //printf("\n<>NUMBER %s<>\n", yytext);
                                    deflval.dval=atof(yytext);
                                    return(NUMBER);} 
<INITIAL,INSIDEBRACKETS>{pnnum_int}({dot}{num_int})?(e{sign}?{num_int})?       {deflval.dval=atof(yytext);return(NUMBER);}
<INITIAL,INSIDEBRACKETS>{pnnum_int}{dot}(e{sign}?{num_int})?       {deflval.dval=atof(yytext);return(NUMBER);}
<INITIAL,INSIDEBRACKETS>({eletter}|{digit})({gletter}|{digit})*     {   
                             //printf("\n<><>STRING %s<><>\n", yytext);
                             deflval.string.DoDelete=1;deflval.string.str = strCdup(yytext) ;
                             return(STRING_not_keyword);}  
.           {  
            printf("\nDEF scan error on line number %d\n",defLinenum);
            errflag = 2;
            fflush(stdout);
            fflush(stderr);
            if (0) yyunput(0,NULL); //kill silly compiler warning
            exit(-1);
            }

%%


/* QSTRING                         {  return(QSTRING);}*/
/* STRING                          {  return(STRING);} */
/* SITE_PATTERN                    {  return(SITE_PATTERN);} */
/* NUMBER                          {  return(NUMBER);} */
/* {chr_lit}        {  return(K_CHAR_CONST);} */
