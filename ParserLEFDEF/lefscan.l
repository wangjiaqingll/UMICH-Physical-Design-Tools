%{
#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

#include <string.h>
#include <ctype.h>
//#include <unistd.h>

#ifdef WIN32
#include <io.h>
#endif

#include "Geoms/plGeom.h"
#include "LEFDEFpoint.h"
typedef struct lexsstr
    {
    const char *str;
    int  DoDelete;
    } lexemeStruct;

extern int bCaseSens;

char *strCdup(char *s);


#include "lefparse.tab.h"

//static int end_of_file = 0;
int lefLinenum = 0;
static int errflag = 0;

%}

/* Definitions */

white          [\t ]
eoln        \n
letter      [a-zA-Z]
eletter     [a-zA-Z_/=|\[\]\(\)\<\>\.!\{\}\-\$\:\\]
digit        [0-9]
num_int      {digit}+
sign        \+|\-
pnnum_int    {sign}?{digit}+ 
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
star            \*
asgn        =
gletter   {eletter}|\*|#

%p 6000

%option    never-interactive
%%

#.*                           {  }

{white}+            {  }            
{eoln}              {  lefLinenum++ ;
                       //BEGIN(INITIAL);
                       }
{plus}          {  //printf("K_PLUS\n");
                     return(K_PLUS);}
{minus}         {  return(K_MINUS);}
{star}                  {  return(K_STAR);} 
{asgn}          { return(K_ASGN);}  

ABUTMENT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ABUTMENT";return(K_ABUTMENT);}
ACTIVE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ACTIVE";return(K_ACTIVE);}
ANALOG {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ANALOG";return(K_ANALOG);}
ANTENNAAREAFACTOR {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ANTENNAAREAFACTOR";return(K_ANTENNAAREAFACTOR);}
ANTENNALENGTHFACTOR {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ANTENNALENGTHFACTOR";return(K_ANTENNALENGTHFACTOR);}
ANTENNAMETALAREA {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ANTENNAMETALAREA";return(K_ANTENNAMETALAREA);}
ANTENNAMETALLENGTH {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ANTENNAMETALLENGTH";return(K_ANTENNAMETALLENGTH);}
ANTENNASIZE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ANTENNASIZE";return(K_ANTENNASIZE);}
ANYEDGE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ANYEDGE";return(K_ANYEDGE);}
ARRAY {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ARRAY";return(K_ARRAY);}
BLOCK {leflval.lexeme.DoDelete=0;leflval.lexeme.str="BLOCK";return(K_BLOCK);}
BOTTOMLEFT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="BOTTOMLEFT";return(K_BOTTOMLEFT);}
BOTTOMRIGHT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="BOTTOMRIGHT";return(K_BOTTOMRIGHT);}
BUSBITCHARS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="BUSBITCHARS";return(K_BUSBITCHARS);}
BY {leflval.lexeme.DoDelete=0;leflval.lexeme.str="BY";return(K_BY);}
CANNOTOCCUPY {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CANNOTOCCUPY";return(K_CANNOTOCCUPY);}
CANPLACE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CANPLACE";return(K_CANPLACE);}
CAPACITANCE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CAPACITANCE";return(K_CAP);}
CAPMULTIPLIER {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CAPMULTIPLIER";return(K_CAPMULTIPLIER);}
CLASS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CLASS";return(K_CLASS);}
CLOCK {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CLOCK";return(K_CLOCK);}
CORE {leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);return(K_CORE);}
COVER {leflval.lexeme.DoDelete=0;leflval.lexeme.str="COVER";return(K_COVER);}
CORRECTIONFACTOR {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CORRECTIONFACTOR";return(K_CORRECTIONFACTOR);}
CORRECTIONTABLE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CORRECTIONTABLE";return(K_CORRECTIONTABLE);}
CPERSQDIST {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CPERSQDIST";return(K_CPERSQ);}
CURRENTSOURCE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CURRENTSOURCE";return(K_CURRENTSOURCE);}
CURRENT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CURRENT";return(K_CURRENT);}
CUT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="CUT";return(K_CUT);}
DATA {leflval.lexeme.DoDelete=0;leflval.lexeme.str="DATA";return(K_DATA);}
DATABASE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="DATABASE";return(K_DATABASE);}
DEFAULT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="DEFAULT";return(K_DEFAULT);}
DEFAULTCAP {leflval.lexeme.DoDelete=0;leflval.lexeme.str="DEFAULTCAP";return(K_DEFAULTCAP);}
DELAY {leflval.lexeme.DoDelete=0;leflval.lexeme.str="DELAY";return(K_DELAY);}
DESIGN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="DESIGN";return(K_DESIGN);}
DIELECTRIC {leflval.lexeme.DoDelete=0;leflval.lexeme.str="DIELECTRIC";return(K_DIELECTRIC);}
DIVIDERCHAR {leflval.lexeme.DoDelete=0;leflval.lexeme.str="DIVIDERCHAR";return(K_DIVIDERCHAR);}
DIRECTION {leflval.lexeme.DoDelete=0;leflval.lexeme.str="DIRECTION";return(K_DIRECTION);}
DO {leflval.lexeme.DoDelete=0;leflval.lexeme.str="DO";return(K_DO);}
E {leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);return(K_E);} //KLUDGE
EDGECAPACITANCE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="EDGECAPACITANCE";return(K_EDGECAP);}
EDGERATE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="EDGERATE";return(K_EDGERATE);}
EDGERATETHRESHOLD1 {leflval.lexeme.DoDelete=0;leflval.lexeme.str="EDGERATETHRESHOLD1";return(K_EDGERATETHRESHOLD1);}
EDGERATETHRESHOLD2 {leflval.lexeme.DoDelete=0;leflval.lexeme.str="EDGERATETHRESHOLD2";return(K_EDGERATETHRESHOLD2);}
EDGERATESCALEFACTOR {leflval.lexeme.DoDelete=0;leflval.lexeme.str="EDGERATESCALEFACTOR";return(K_EDGERATESCALEFACTOR);}
EEQ {leflval.lexeme.DoDelete=0;leflval.lexeme.str="EEQ";return(K_EEQ);}
END {leflval.lexeme.DoDelete=0;leflval.lexeme.str="END";//BEGIN(LITERAL);
          return(K_END);}
ENDCAP {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ENDCAP";return(K_ENDCAP);}
FALL {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FALL";return(K_FALL);}
FALLRS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FALLRS";return(K_FALLRS);}
FALLCS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FALLCS";return(K_FALLCS);}
FALLSATT1 {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FALLSATT1";return(K_FALLSATT1);}
FALLSLEWLIMIT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FALLSLEWLIMIT";return(K_FALLSLEWLIMIT);}
FALLT0 {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FALLT0";return(K_FALLT0);}
FALLTHRESH {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FALLTHRESH";return(K_FALLTHRESH);}
FALLSATCUR {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FALLSATCUR";return(K_FALLSATCUR);}
FALLVOLTAGETHRESHOLD {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FALLVOLTAGETHRESHOLD";return(K_FALLVOLTAGETHRESHOLD);}
FE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FE";return(K_FE);}
FEEDTHRU {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FEEDTHRU";return(K_FEEDTHRU);}
FLOORPLAN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FLOORPLAN";return(K_FLOORPLAN);}
FN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FN";return(K_FN);}
FOREIGN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FOREIGN";return(K_FOREIGN);}
FROMPIN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FROMPIN";return(K_FROMPIN);}
FUNCTION {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FUNCTION";return(K_FUNCTION);}
FS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FS";return(K_FS);}
FW {leflval.lexeme.DoDelete=0;leflval.lexeme.str="FW";return(K_FW);}
GCELLGRID {leflval.lexeme.DoDelete=0;leflval.lexeme.str="GCELLGRID";return(K_GCELL);}
GENERATE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="GENERATE";return(K_GENERATE);}
GROUND {leflval.lexeme.DoDelete=0;leflval.lexeme.str="GROUND";return(K_GROUND);}
HEIGHT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="HEIGHT";return(K_HEIGHT);}
HOLD {leflval.lexeme.DoDelete=0;leflval.lexeme.str="HOLD";return(K_HOLD);}
HORIZONTAL {leflval.lexeme.DoDelete=0;leflval.lexeme.str="HORIZONTAL";return(K_HORIZ);}
INOUT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="INOUT";return(K_INOUT);}

 
INPUT {/*this line is a kludge to make one file parse (CSHELL)*/leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);return(K_INPUT);}
INOUTPINANTENNASIZE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="INOUTPINANTENNASIZE";return(K_INOUTPINANTENNASIZE);}
INPUTPINANTENNASIZE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="INPUTPINANTENNASIZE";return(K_INPUTPINANTENNASIZE);}
INPUTNOISEMARGIN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="INPUTNOISEMARGIN";return(K_INPUTNOISEMARGIN);}
INTEGER {leflval.lexeme.DoDelete=0;leflval.lexeme.str="INTEGER";return(K_INTEGER);}
INTRINSIC {leflval.lexeme.DoDelete=0;leflval.lexeme.str="INTRINSIC";return(K_INTRINSIC);}
INVERT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="INVERT";return(K_INVERT);}
IRDROP {leflval.lexeme.DoDelete=0;leflval.lexeme.str="IRDROP";return(K_IRDROP);}
ITERATE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ITERATE";return(K_ITERATE);}
IV_TABLES {leflval.lexeme.DoDelete=0;leflval.lexeme.str="IV_TABLES";return(K_IV_TABLES);}
LAYER {leflval.lexeme.DoDelete=0;leflval.lexeme.str="LAYER";//BEGIN(LITERAL);
              return(K_LAYER);}
LEAKAGE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="LEAKAGE";return(K_LEAKAGE);}
LEQ {leflval.lexeme.DoDelete=0;leflval.lexeme.str="LEQ";return(K_LEQ);}
LIBRARY {leflval.lexeme.DoDelete=0;leflval.lexeme.str="LIBRARY";return(K_LIBRARY);}
LENGTHTHRESHOLD {leflval.lexeme.DoDelete=0;leflval.lexeme.str="LENGTHTHRESHOLD";return(K_LENGTHTHRESHOLD);}
MACRO {leflval.lexeme.DoDelete=0;leflval.lexeme.str="MACRO";return(K_MACRO);}
MASTERSLICE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="MASTERSLICE";return(K_MSLICE);}
MAXDELAY {leflval.lexeme.DoDelete=0;leflval.lexeme.str="MAXDELAY";return(K_MAXDELAY);}
MAXLOAD {leflval.lexeme.DoDelete=0;leflval.lexeme.str="MAXLOAD";return(K_MAXLOAD);}
METALOVERHANG {leflval.lexeme.DoDelete=0;leflval.lexeme.str="METALOVERHANG";return(K_METALOVERHANG);}
MICRONS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="MICRONS";return(K_MICRONS);}
MILLIAMPS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="MILLIAMPS";return(K_MILLIAMPS);}
MILLIWATTS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="MILLIWATTS";return(K_MILLIWATTS);}
MINFEATURE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="MINFEATURE";return(K_MINFEATURE);}
MINPINS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="MINPINS";return(K_MINPINS);}
MUSTJOIN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="MUSTJOIN";return(K_MUSTJOIN);}
N {leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);return(K_N);} //KLUDGE
NAMESCASESENSITIVE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="NAMESCASESENSITIVE";return(K_CASE);}
NANOSECONDS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="NANOSECONDS";return(K_NANOSECONDS);}
NEGEDGE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="NEGEDGE";return(K_NEGEDGE);}
NOISETABLE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="NOISETABLE";return(K_NOISETABLE);}
NONDEFAULTRULE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="NONDEFAULTRULE";return(K_NONDEFAULTRULE);}
NONINVERT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="NONINVERT";return(K_NONINVERT);}
NONUNATE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="NONUNATE";return(K_NONUNATE);}
OBS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="OBS";return(K_OBS);}
OHMS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="OHMS";return(K_OHMS);}
OFF {leflval.lexeme.DoDelete=0;leflval.lexeme.str="OFF";return(K_OFF);}
OFFSET {leflval.lexeme.DoDelete=0;leflval.lexeme.str="OFFSET";return(K_OFFSET);}
ON {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ON";return(K_ON);}
ORIGIN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ORIGIN";return(K_ORIGIN);}

OUTPUT {/*this line is a kludge to make one file parse (CSHELL)*/leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);return(K_OUTPUT);}

OUTPUTRESISTANCE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="OUTPUTRESISTANCE";return(K_OUTPUTRESISTANCE);}
OUTPUTNOISEMARGIN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="OUTPUTNOISEMARGIN";return(K_OUTPUTNOISEMARGIN);}
OUTPUTPINANTENNASIZE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="OUTPUTPINANTENNASIZE";return(K_OUTPUTPINANTENNASIZE);}
OVERHANG {leflval.lexeme.DoDelete=0;leflval.lexeme.str="OVERHANG";return(K_OVERHANG);}
OVERLAP {leflval.lexeme.DoDelete=0;leflval.lexeme.str="OVERLAP";return(K_OVERLAP);}
PAD {leflval.lexeme.DoDelete=0;leflval.lexeme.str="PAD";return(K_PAD);}
PATH {leflval.lexeme.DoDelete=0;leflval.lexeme.str="PATH";return(K_PATH);}
PIN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="PIN";//BEGIN(LITERAL);
          return(K_PIN);}
PICOFARADS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="PICOFARADS";return(K_PICOFARADS);}
PITCH {leflval.lexeme.DoDelete=0;leflval.lexeme.str="PITCH";return(K_PITCH);}
POLYGON {leflval.lexeme.DoDelete=0;leflval.lexeme.str="POLYGON";return(K_POLYGON);}
PORT {/*this line is a kludge to make one file parse (SUN_TIMING)*/leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);return(K_PORT);}
POSEDGE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="POSEDGE";return(K_POSEDGE);}
POST {leflval.lexeme.DoDelete=0;leflval.lexeme.str="POST";return(K_POST);}
POWER {leflval.lexeme.DoDelete=0;leflval.lexeme.str="POWER";return(K_POWER);}
PRE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="PRE";return(K_PRE);}
PROPERTY {leflval.lexeme.DoDelete=0;leflval.lexeme.str="PROPERTY";return(K_PROPERTY);}
PROPERTYDEFINITIONS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="PROPERTYDEFINITIONS";return(K_PROPERTYDEFINITIONS);}
PULLDOWNRES {leflval.lexeme.DoDelete=0;leflval.lexeme.str="PULLDOWNRES";return(K_PULLDOWNRES);}
R90 {leflval.lexeme.DoDelete=0;leflval.lexeme.str="R90";return(K_R90);}
RANGE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RANGE";return(K_RANGE);}
REAL {leflval.lexeme.DoDelete=0;leflval.lexeme.str="REAL";return(K_REAL);}
RECT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RECT";return(K_RECT);}
RESISTANCE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RESISTANCE";return(K_RES);}
RESISTIVE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RESISTIVE";return(K_RESISTIVE);}
RING {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RING";return(K_RING);}
RISE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RISE";return(K_RISE);}
RISERS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RISERS";return(K_RISERS);}
RISECS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RISECS";return(K_RISECS);}
RISESATT1 {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RISESATT1";return(K_RISESATT1);}
RISESLEWLIMIT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RISESLEWLIMIT";return(K_RISESLEWLIMIT);}
RISET0 {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RISET0";return(K_RISET0);}
RISETHRESH {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RISETHRESH";return(K_RISETHRESH);}
RISESATCUR {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RISESATCUR";return(K_RISESATCUR);}
RISEVOLTAGETHRESHOLD {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RISEVOLTAGETHRESHOLD";return(K_RISEVOLTAGETHRESHOLD);}
ROUTING {leflval.lexeme.DoDelete=0;leflval.lexeme.str="ROUTING";return(K_ROUTING);}
RPERSQ {leflval.lexeme.DoDelete=0;leflval.lexeme.str="RPERSQ";return(K_RPERSQ);}
S {leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);return(K_S);} //KLUDGE
SAMENET {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SAMENET";//BEGIN(LITERAL);
                  return(K_SAMENET);}
SETUP {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SETUP";return(K_SETUP);}
SETUPTIME {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SETUPTIME";return(K_SETUPTIME);}
SHAPE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SHAPE";return(K_SHAPE);}
SHRINKAGE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SHRINKAGE";return(K_SHRINKAGE);}
SIGNAL {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SIGNAL";return(K_SIGNAL);}
SITE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SITE";return(K_SITE);}
SIZE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SIZE";return(K_SIZE);}
SKIP {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SKIP";return(K_SKIP);}
SOURCE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SOURCE";return(K_SOURCE);}
SPACER {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SPACER";return(K_SPACER);}
SPACING {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SPACING";return(K_SPACING);}
STACK {leflval.lexeme.DoDelete=0;leflval.lexeme.str="STACK";return(K_STACK);}
STEP {leflval.lexeme.DoDelete=0;leflval.lexeme.str="STEP";return(K_STEP);}
STRING {leflval.lexeme.DoDelete=0;leflval.lexeme.str="STRING";return(K_STRING);}
STRUCTURE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="STRUCTURE";return(K_STRUCTURE);}
SYMMETRY {leflval.lexeme.DoDelete=0;leflval.lexeme.str="SYMMETRY";return(K_SYMMETRY);}
TABLE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TABLE";return(K_TABLE);}
TABLEAXIS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TABLEAXIS";return(K_TABLEAXIS);}
TABLEENTRIES {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TABLEENTRIES";return(K_TABLEENTRIES);}
TABLEDIMENSION {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TABLEDIMENSION";return(K_TABLEDIMENSION);}
THICKNESS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="THICKNESS";return(K_THICKNESS);}
TIEHIGH {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TIEHIGH";return(K_TIEHIGH);}
TIELOW {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TIELOW";return(K_TIELOW);}
TIEOFF {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TIEOFF";return(K_TIEOFF);}
TIEOFFR {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TIEOFFR";return(K_TIEOFFR);}
TIME {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TIME";return(K_TIME);}
TIMING {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TIMING";return(K_TIMING);}
TO {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TO";return(K_TO);}
TOPIN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TOPIN";return(K_TOPIN);}
TOPLEFT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TOPLEFT";return(K_TOPLEFT);}
TOPOFSTACKONLY {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TOPOFSTACKONLY";return(K_TOPOFSTACKONLY);}
TOPRIGHT {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TOPRIGHT";return(K_TOPRIGHT);}
TRACKS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TRACKS";return(K_TRACKS);}
TRANSITION {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TRANSITION";return(K_TRANSITION);}
TRANSITIONTIME {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TRANSITIONTIME";return(K_TRANSITIONTIME);}
TRISTATE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TRISTATE";return(K_TRISTATE);}
TYPE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="TYPE";return(K_TYPE);}
UNATENESS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="UNATENESS";return(K_UNATE);}
UNIVERSALNOISEMARGIN {leflval.lexeme.DoDelete=0;leflval.lexeme.str="UNIVERSALNOISEMARGIN";return(K_UNIVERSALNOISEMARGIN);}
UNITS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="UNITS";return(K_UNITS);}
USE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="USE";return(K_USE);}
USELENGTHTHRESHOLD {leflval.lexeme.DoDelete=0;leflval.lexeme.str="USELENGTHTHRESHOLD";return(K_USELENGTHTHRESHOLD);}
USER {leflval.lexeme.DoDelete=0;leflval.lexeme.str="USER";return(K_USER);}
VARIABLE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="VARIABLE";return(K_VARIABLE);}
VERSION {leflval.lexeme.DoDelete=0;leflval.lexeme.str="VERSION";return(K_VERSION);}
VERTICAL {leflval.lexeme.DoDelete=0;leflval.lexeme.str="VERTICAL";return(K_VERT);}
VHI {leflval.lexeme.DoDelete=0;leflval.lexeme.str="VHI";return(K_VHI);}
VIA {leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);//BEGIN(LITERAL);
                    return(K_VIA);}
VIARULE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="VIARULE";return(K_VIARULE);}
VICTIMLENGTH {leflval.lexeme.DoDelete=0;leflval.lexeme.str="VICTIMLENGTH";return(K_VICTIMLENGTH);}
VICTIMNOISE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="VICTIMNOISE";return(K_VICTIMNOISE);}
VLO {leflval.lexeme.DoDelete=0;leflval.lexeme.str="VLO";return(K_VLO);}
VOLTAGE {leflval.lexeme.DoDelete=0;leflval.lexeme.str="VOLTAGE";return(K_VOLTAGE);}
VOLTS {leflval.lexeme.DoDelete=0;leflval.lexeme.str="VOLTS";return(K_VOLTS);}
W {leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);return(K_W);} //KLUDGE
WIDTH {leflval.lexeme.DoDelete=0;leflval.lexeme.str="WIDTH";return(K_WIDTH);}
WIRECAP {leflval.lexeme.DoDelete=0;leflval.lexeme.str="WIRECAP";return(K_WIRECAP);}
WIREEXTENSION {leflval.lexeme.DoDelete=0;leflval.lexeme.str="WIREEXTENSION";return(K_WIREEXTENSION);}
X {/*this line is a kludge*/leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);return(K_X);}
Y {/*this line is a kludge*/leflval.lexeme.DoDelete=1;leflval.lexeme.str=strCdup(yytext);return(K_Y);}


{comma}         { return(K_COMMA);}   
{semcol}        {  //BEGIN(INITIAL);
                   return(K_SEMCOL);}
{dot}{dot}      {  return(K_DOT_DOT);}
{dot}           {  return(K_DOT);}
{colon}         {  return(K_COLON);}
{ord_br1}       {  return(K_ORDBR1);}
{ord_br2}       {  return(K_ORDBR2);}
{quoted_str}    {  return(QUOTED_STR);}


{pnnum_int} {  
     leflval.fval = atof(yytext);
     //printf("\n<>NUMBER %s<>, value %g\n", yytext, leflval.fval);
     return(NUMBER);
     } 
{pnnum_int}e{sign}?{num_int} {  
     leflval.fval = atof(yytext); 
     return(NUMBER);
     }
{sign}?{num_int}?{dot}{num_int}([e|E]{sign}?{num_int})? {  
     leflval.fval = atof(yytext); 
     return(NUMBER);
     }
({eletter}|{digit})({gletter}|{digit})* {   leflval.lexeme.DoDelete=1;
                        leflval.lexeme.str = strCdup(yytext) ;
                            // printf("\n<><>K_IDG %s<><>value = %s\n", yytext, leflval.lexeme.DoDelete=0;leflval.lexeme.str);
                            //BEGIN(INITIAL);
                            return(K_ID);}  
.           {  
            printf("\nLEF scan error on line number %d\n",lefLinenum);
            fflush(stdout);
            fflush(stderr);
            if (0) yyunput(0,NULL); //kill silly compiler warning
            exit(-1);
            errflag = 2;
            }
%%
/*
main()
{
 char p[100];
//BEGIN(INITIAL);
while ((*p = yylex()) != EOF){
}
}

*/

//This function forces everything to upper case in a case-insensitive
//LEFDEF.

char *strCdup(char *s)
    {
    char *pc = strdup(s);
    if (!bCaseSens)
        {
#ifdef _MSC_VER
        _strupr(pc);
#else
        char *pcRun = pc;
        while (*pcRun)
            {
            *pcRun = toupper(*pcRun);
            pcRun++;
            }
#endif
        }
    return pc;
    }

