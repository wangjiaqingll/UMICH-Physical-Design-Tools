%{
 /* 
* This file is part of the Cadence LEF/DEF Open Source
 * Distribution, Product Version 5.3, and is subject to the Cadence
 * LEF/DEF Open Source License Agreement. Your continued use of
 * this file constitutes your acceptance of the terms of the LEF/DEF
 * Open Source License and an agreement to abide by its terms. If
 * you don't agree with this, you must remove this and any other
 * files which are part of the distribution and destroy any copies
 * made.
 *
 * For updates, support, or to become part of the LEF/DEF Community,
 * check www.openeda.org for details.
 */

 
 #include <string.h>
 #include <strings.h>
 #include <stdlib.h>
 #include <math.h>
 /* # include <sys/types.h> */

 /* # include <sys/stat.h> */

 /* # include <fcntl.h> */

 /* # include <unistd.h> */

 /* # ifndef	WIN32 */

 /* # include <sys/param.h> */

 /* # endif */

 #include "FEF_stringhash.h"
 #include "lex.h"
 #include "lefiDefs.h"
 #include "lefiUser.h"
 #include "lefrCallBacks.h"
 #ifdef WIN32
 #include <malloc.h>
 #endif
  
 #define LYPROP_ECAP "EDGE_CAPACITANCE"
 
 int lefRetVal;
 
 /* Macro to describe how we handle a callback.
 * If the function was set then call it.
 * If the function returns non zero then there was an error
 * so call the error routine and exit.
 */

 #define CALLBACK(func, typ, data) \
 if (func) { \
 if ((lefRetVal = (*func)(typ, data, lefrUserData)) == 0) { \
 } else if (lefRetVal == STOP_PARSE) { \
 	return lefRetVal; \
 } else { \
 	yyerror("Error in callback"); \
 	return lefRetVal; \
 } \
 }
 
 extern int lefNlines, lefNtokens, lefErrors;
 extern FILE* lefFile;
 
 /* *********************************************************************/
/* *********************************************************************/

 #define C_EQ 0
 #define C_NE 1
 #define C_LT 2
 #define C_LE 3
 #define C_GT 4
 #define C_GE 5
 
 extern int lefDumbMode; /* for communicating with parser
 */
extern int lefNoNum; /* likewise, says no numbers for the next N tokens
 */
extern int lefNlToken; /* likewise
  */
extern int lefDefIf; /* likewise
  */

 int lefNamesCaseSensitive = FALSE; /* are names case sensitive?
 */

 /* XXXXXXXXXXXXX check out these variables */

 double lef_save_x, lef_save_y; /* for interpreting (*) notation of LEF/DEF
 */

 /* #define STRING_LIST_SIZE 1024 */
 /* char string_list[STRING_LIST_SIZE]; */

 
 static char temp_name[256];
 
 static int siteDef, symDef, sizeDef, pinDef, obsDef, origDef;
 static int useLenThr, lenThr;
 
 int comp_str(char *s1, int op, char *s2)
 {
 int k = strcmp(s1, s2);
 switch (op) {
 	case C_EQ: return k == 0;
 	case C_NE: return k != 0;
 	case C_GT: return k > 0;
 	case C_GE: return k >= 0;
 	case C_LT: return k < 0;
 	case C_LE: return k <= 0;
 	}
 return 0;
 }
 int comp_num(double s1, int op, double s2)
 {
 double k = s1 - s2;
 switch (op) {
 	case C_EQ: return k == 0;
 	case C_NE: return k != 0;
 	case C_GT: return k > 0;
 	case C_GE: return k >= 0;
 	case C_LT: return k < 0;
 	case C_LE: return k <= 0;
 	}
 return 0;
 }
 %}
 
 %union {
 	double dval ;
 	int integer ;
 	char * string ;
 	POINT pt;
 }
 
 %token <string> K_HISTORY
 %token K_ABUT K_ABUTMENT K_ACTIVE K_ANALOG K_ANTENNAAREAFACTOR K_ARRAY
 %token K_BLOCK K_BOTTOMLEFT K_BOTTOMRIGHT
 %token K_BY K_CAPACITANCE K_CAPMULTIPLIER K_CLASS K_CLOCK K_CLOCKTYPE
 %token K_COLUMNMAJOR
 %token K_COMPONENTS K_CORE K_CORNER K_COVER K_CPERSQDIST K_CURRENT
 %token K_CURRENTSOURCE K_CUT K_DEFAULT K_DATABASE K_DATA
 %token K_DIELECTRIC K_DIRECTION K_DO K_EDGECAPACITANCE
 %token K_EEQ K_END K_ENDCAP K_FALL K_FALLCS K_FALLT0 K_FALLSATT1
 %token K_FALLRS K_FALLSATCUR K_FALLTHRESH K_FEEDTHRU K_FIXED K_FOREIGN K_FROMPIN
 %token K_GENERATE K_GENERATOR K_GROUND K_HEIGHT K_HORIZONTAL K_INOUT K_INPUT
 %token K_INPUTNOISEMARGIN K_COMPONENTPIN
 %token K_INTRINSIC K_INVERT K_IRDROP K_ITERATE K_IV_TABLES K_LAYER K_LEAKAGE
 %token K_LEQ K_LIBRARY K_MACRO K_MATCH K_MAXDELAY K_MAXLOAD K_METALOVERHANG K_MILLIAMPS
 %token K_MILLIWATTS K_MINFEATURE K_MAXLOAD K_MUSTJOIN K_NAMESCASESENSITIVE K_NANOSECONDS
 %token K_NETS K_NEW K_NONDEFAULTRULE
 %token K_NONINVERT K_NONUNATE K_OBS K_OHMS K_OFFSET K_ORIENTATION K_ORIGIN K_OUTPUT
 %token K_OUTPUTNOISEMARGIN K_OUTPUTRESISTANCE
 %token K_OVERHANG K_OVERLAP K_OFF K_ON K_OVERLAPS K_PAD K_PATH K_PATTERN K_PICOFARADS
 %token K_PIN K_PITCH
 %token K_PLACED K_POLYGON K_PORT K_POST K_POWER K_PRE K_PULLDOWNRES K_RECT
 %token K_RESISTANCE K_RESISTIVE K_RING K_RISE K_RISECS K_RISERS K_RISESATCUR K_RISETHRESH
 %token K_RISESATT1 K_RISET0 K_RISEVOLTAGETHRESHOLD K_FALLVOLTAGETHRESHOLD
 %token K_ROUTING K_ROWMAJOR K_RPERSQ K_SAMENET K_SCANUSE K_SHAPE K_SHRINKAGE
 %token K_SIGNAL K_SITE K_SIZE K_SOURCE K_SPACER K_SPACING K_SPECIALNETS K_STACK
 %token K_START K_STEP K_STOP K_STRUCTURE K_SYMMETRY K_TABLE K_THICKNESS K_TIEHIGH
 %token K_TIELOW K_TIEOFFR K_TIME K_TIMING K_TO K_TOPIN K_TOPLEFT K_TOPRIGHT
 %token K_TOPOFSTACKONLY
 %token K_TRISTATE K_TYPE K_UNATENESS K_UNITS K_USE K_VARIABLE K_VERTICAL K_VHI
 %token K_VIA K_VIARULE K_VLO K_VOLTAGE K_VOLTS K_WIDTH K_X K_Y K_R90
 %token <string> T_STRING QSTRING
 %token <dval> NUMBER
 %token K_N K_S K_E K_W K_FN K_FS K_FE K_FW
 %token K_R0 K_R90 K_R180 K_R270 K_MX K_MY K_MXR90 K_MYR90
 %token K_USER K_MASTERSLICE
 %token K_ENDMACRO K_ENDMACROPIN K_ENDVIARULE K_ENDVIA K_ENDLAYER K_ENDSITE
 %token K_CANPLACE K_CANNOTOCCUPY K_TRACKS K_FLOORPLAN K_GCELLGRID K_DEFAULTCAP
 %token K_MINPINS K_WIRECAP
 %token K_STABLE K_SETUP K_HOLD
 %token K_DEFINE K_DEFINES K_DEFINEB K_IF K_THEN K_ELSE K_FALSE K_TRUE
 %token K_EQ K_NE K_LE K_LT K_GE K_GT K_OR K_AND K_NOT
 %token K_DELAY K_TABLEDIMENSION K_TABLEAXIS K_TABLEENTRIES K_TRANSITIONTIME
 %token K_EXTENSION
 %token K_PROPDEF K_STRING K_INTEGER K_REAL K_RANGE K_PROPERTY
 %token K_VIRTUAL K_BUSBITCHARS K_VERSION
 %token K_BEGINEXT K_ENDEXT
 %token K_UNIVERSALNOISEMARGIN K_EDGERATETHRESHOLD1 K_CORRECTIONTABLE
 %token K_EDGERATESCALEFACTOR K_EDGERATETHRESHOLD2 K_VICTIMNOISE
 %token K_NOISETABLE K_EDGERATE K_OUTPUTRESISTANCE K_VICTIMLENGTH
 %token K_CORRECTIONFACTOR K_OUTPUTPINANTENNASIZE
 %token K_INPUTPINANTENNASIZE K_INOUTPINANTENNASIZE
 %token K_CURRENTDEN K_PWL K_ANTENNALENGTHFACTOR K_TAPERRULE
 %token K_DIVIDERCHAR K_ANTENNASIZE K_ANTENNAMETALLENGTH K_ANTENNAMETALAREA
 %token K_RISESLEWLIMIT K_FALLSLEWLIMIT K_FUNCTION K_BUFFER K_INVERTER
 %token K_NAMEMAPSTRING K_NOWIREEXTENSIONATPIN K_WIREEXTENSION
 %token K_MESSAGE K_CREATEFILE K_OPENFILE K_CLOSEFILE K_WARNING
 %token K_ERROR K_FATALERROR
 %token K_RECOVERY K_SKEW K_ANYEDGE K_POSEDGE K_NEGEDGE
 %token K_SDFCONDSTART K_SDFCONDEND K_SDFCOND
 %token K_MPWH K_MPWL K_PERIOD
 %token K_ACCURRENTDENSITY K_DCCURRENTDENSITY K_AVERAGE K_PEAK K_RMS K_FREQUENCY
 %token K_CUTAREA K_MEGAHERTZ K_USELENGTHTHRESHOLD K_LENGTHTHRESHOLD
 
 %type <pt> opt_range
 %type <string> start_macro end_macro
 %type <string> start_layer
 %type <string> macro_pin_use
 %type <string> macro_scan_use
 %type <string> pin_shape
 %type <string> pad_type core_type endcap_type class_type site_class
 %type <string> start_foreign
 %type <pt> pt
 %type <pt> macro_origin
 %type <string> layer_option layer_options layer_type layer_direction
 %type <string> electrical_direction
 %type <integer> orientation
 %type <dval> expression
 %type <integer> b_expr
 %type <string> s_expr
 %type <integer> relop
 %type <string> opt_layer_name risefall unateness delay_or_transition
 %type <string> two_pin_trigger from_pin_trigger to_pin_trigger
 %type <string> one_pin_trigger
 %type <string> layer_table_type
 
 %nonassoc IF
 %left K_AND
 %left K_OR
 %left K_LE K_EQ K_LT K_NE K_GE K_GT
 %nonassoc LNOT
 %left '-' '+'
 %left '*' '/'
 %nonassoc UMINUS
 
 %%
 
 lef_file: rules
 	| rules end_library
 	;
 
 version: K_VERSION NUMBER ';'
 { CALLBACK(lefrVersionCbk, lefrVersionCbkType, $2); }
;
 
 dividerchar: K_DIVIDERCHAR QSTRING ';'
 { CALLBACK(lefrDividerCharCbk, lefrDividerCharCbkType, $2); }
;
 
 busbitchars: K_BUSBITCHARS QSTRING ';'
 { CALLBACK(lefrBusBitCharsCbk, lefrBusBitCharsCbkType, $2); }
;
 
 rules: /* empty */

 	| rules rule
 	| error
 	 { }
;
 
 end_library: K_END K_LIBRARY
 { CALLBACK(lefrLibraryEndCbk, lefrLibraryEndCbkType, 0); }
;
 
 rule: version | busbitchars | case_sensitivity | units_section
 | layer_rule | via | viarule | viarule_generate | dividerchar
 | wireextension | msg_statement
 | spacing_rule | dielectric | minfeature | irdrop | site | macro | array
 | def_statement | nondefault_rule | prop_def_section
 | universalnoisemargin | edgeratethreshold1
 | edgeratescalefactor | edgeratethreshold2
 | noisetable | correctiontable | input_antenna
 | output_antenna | inout_antenna | extension
 | create_file_statement
 	;
 
 case_sensitivity: K_NAMESCASESENSITIVE K_ON ';'
 	 {
 	 lefNamesCaseSensitive = TRUE;
 	 CALLBACK(lefrCaseSensitiveCbk, lefrCaseSensitiveCbkType,
 			 lefNamesCaseSensitive);
 	 }
 | K_NAMESCASESENSITIVE K_OFF ';'
 	 {
 	 lefNamesCaseSensitive = FALSE;
 	 CALLBACK(lefrCaseSensitiveCbk, lefrCaseSensitiveCbkType,
 			 lefNamesCaseSensitive);
 	 }
;
 
 wireextension: K_NOWIREEXTENSIONATPIN K_ON ';'
 { CALLBACK(lefrNoWireExtensionCbk, lefrNoWireExtensionCbkType, "ON"); }
 | K_NOWIREEXTENSIONATPIN K_OFF ';'
 { CALLBACK(lefrNoWireExtensionCbk, lefrNoWireExtensionCbkType, "OFF"); }
;
 
 units_section: start_units units_rules K_END K_UNITS
 { CALLBACK(lefrUnitsCbk, lefrUnitsCbkType, &lefrUnits); }
;
 
 start_units: K_UNITS
 { lefiUnits_clear( &(lefrUnits) ) ;
 }
;
 
 units_rules: /* empty */

 | units_rules units_rule
 ;
 
 units_rule: K_TIME K_NANOSECONDS NUMBER ';'
 { if (lefrUnitsCbk) lefiUnits_setTime( &(lefrUnits),
 $3) ;
 }
 | K_CAPACITANCE K_PICOFARADS NUMBER ';'
 { if (lefrUnitsCbk) lefiUnits_setCapacitance( &(lefrUnits),
 $3) ;
 }
 | K_RESISTANCE K_OHMS NUMBER ';'
 { if (lefrUnitsCbk) lefiUnits_setResistance( &(lefrUnits),
 $3) ;
 }
 | K_POWER K_MILLIWATTS NUMBER ';'
 { if (lefrUnitsCbk) lefiUnits_setPower( &(lefrUnits),
 $3) ;
 }
 | K_CURRENT K_MILLIAMPS NUMBER ';'
 { if (lefrUnitsCbk) lefiUnits_setCurrent( &(lefrUnits),
 $3) ;
 }
 | K_VOLTAGE K_VOLTS NUMBER ';'
 { if (lefrUnitsCbk) lefiUnits_setVoltage( &(lefrUnits),
 $3) ;
 }
 | K_DATABASE { lefDumbMode = 1; } T_STRING NUMBER ';'
 { if (lefrUnitsCbk) lefiUnits_setDatabase( &(lefrUnits),
 $3,
$4) ;
 }
 | K_FREQUENCY K_MEGAHERTZ NUMBER ';'
 { if (lefrUnitsCbk) lefiUnits_setFrequency( &(lefrUnits),
 $3) ;
 }
;

 layer_rule: start_layer layer_options end_layer
 { CALLBACK(lefrLayerCbk, lefrLayerCbkType, &lefrLayer); }
;
 
 start_layer: K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING
 {
 if (lefrLayerCbk)
 lefiLayer_setName( &(lefrLayer),
 $3) ;
 useLenThr = 0;
 lenThr = 0;
 }
; 
 
 end_layer: K_END {lefDumbMode = 1; lefNoNum = 1; } T_STRING
 { }
;
 
 layer_options:
 /* empty */
 { }
 | layer_options layer_option
 { }
;
 
 layer_option:
 K_TYPE layer_type ';'
 { if (lefrLayerCbk) lefiLayer_setType( &(lefrLayer),
 $2) ;
 }
 | K_PITCH NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setPitch( &(lefrLayer),
 $2) ;
 }
 | K_OFFSET NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setOffset( &(lefrLayer),
 $2) ;
 }
 | K_WIDTH NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setWidth( &(lefrLayer),
 $2) ;
 }
 | K_SPACING NUMBER opt_range opt_layer_name ';'
 {
 if (lefrLayerCbk) {
 if (lenThr) {
 	 lefiLayer_setSpacing( &(lefrLayer),
 $2,
$3.x,
$3.y) ;
 lefiLayer_setSpacingLength( &(lefrLayer),
 $3.x) ;
 lenThr = 0;
 } else {
 	 lefiLayer_setSpacing( &(lefrLayer),
 $2,
$3.x,
$3.y) ;
 if (useLenThr) {
 lefiLayer_setSpacingUseLength( &(lefrLayer) ) ;
 useLenThr = 0;
 }
 }
 	if ($4) lefiLayer_setSpacingName( &(lefrLayer),
 $4) ;
 }
 }
 | K_DIRECTION layer_direction ';'
 { if (lefrLayerCbk) lefiLayer_setDirection( &(lefrLayer),
 $2) ;
 }
 | K_RESISTANCE K_RPERSQ NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setResistance( &(lefrLayer),
 $3) ;
 }
 | K_RESISTANCE K_RPERSQ K_PWL '(' res_points ')' ';'
 { }
 | K_CAPACITANCE K_CPERSQDIST NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setCapacitance( &(lefrLayer),
 $3) ;
 }
 | K_CAPACITANCE K_CPERSQDIST K_PWL '(' cap_points ')' ';'
 { }
 | K_HEIGHT NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setHeight( &(lefrLayer),
 $2) ;
 }
 | K_WIREEXTENSION NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setWireExtension( &(lefrLayer),
 $2) ;
 }
 | K_THICKNESS NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setThickness( &(lefrLayer),
 $2) ;
 }
 | K_SHRINKAGE NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setShrinkage( &(lefrLayer),
 $2) ;
 }
 | K_CAPMULTIPLIER NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setCapMultiplier( &(lefrLayer),
 $2) ;
 }
 | K_EDGECAPACITANCE NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setEdgeCap( &(lefrLayer),
 $2) ;
 }
 | K_ANTENNAAREAFACTOR NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setAntennaArea( &(lefrLayer),
 $2) ;
 }
 | K_ANTENNALENGTHFACTOR NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setAntennaLength( &(lefrLayer),
 $2) ;
 }
 | K_CURRENTDEN NUMBER ';'
 { if (lefrLayerCbk) lefiLayer_setCurrentDensity( &(lefrLayer),
 $2) ;
 }
 | K_CURRENTDEN K_PWL '(' current_density_pwl_list ')' ';'
 { }
 | K_CURRENTDEN '(' NUMBER NUMBER ')' ';'
 { if (lefrLayerCbk) lefiLayer_setCurrentPoint( &(lefrLayer),
 $3,
$4) ;
 }
 | K_PROPERTY { lefDumbMode = 10000000; } layer_prop_list ';'
 { lefDumbMode = 0; }
 | K_ACCURRENTDENSITY layer_table_type
 { if (lefrLayerCbk) lefiLayer_addAccurrentDensity( &(lefrLayer),
 $2) ;
 }
 ac_layer_table_opt_list ';'
 | K_ACCURRENTDENSITY layer_table_type NUMBER ';'
 { if (lefrLayerCbk) {
 lefiLayer_addAccurrentDensity( &(lefrLayer),
 $2) ;
 lefiLayer_setAcOneEntry( &(lefrLayer),
 $3) ;
 }
 }
 | K_DCCURRENTDENSITY K_AVERAGE
 { if (lefrLayerCbk) lefiLayer_addDccurrentDensity( &(lefrLayer),
 "AVERAGE") ;
 }
 dc_layer_table_opt_list ';'
 | K_DCCURRENTDENSITY K_AVERAGE NUMBER ';'
 { if (lefrLayerCbk) {
 lefiLayer_addDccurrentDensity( &(lefrLayer),
 "AVERAGE") ;
 lefiLayer_setDcOneEntry( &(lefrLayer),
 $3) ;
 }
 }
;
 
 layer_table_type:
 K_PEAK {$$ = (char*)"PEAK";}
 | K_AVERAGE {$$ = (char*)"AVERAGE";}
 | K_RMS {$$ = (char*)"RMS";}
;
 
 ac_layer_table_opt_list:
 | ac_layer_table_opt_list ac_layer_table_opt
;
 
 ac_layer_table_opt:
 K_FREQUENCY NUMBER
 { if (lefrLayerCbk) lefiLayer_addNumber( &(lefrLayer),
 $2) ;
 }
 number_list ';'
 { if (lefrLayerCbk) lefiLayer_addAcFrequency( &(lefrLayer) ) ;
 }
 | K_TABLEENTRIES NUMBER
 { if (lefrLayerCbk) lefiLayer_addNumber( &(lefrLayer),
 $2) ;
 }
 number_list
 { if (lefrLayerCbk) lefiLayer_addAcTableEntry( &(lefrLayer) ) ;
 }
 | K_WIDTH NUMBER
 { if (lefrLayerCbk) lefiLayer_addNumber( &(lefrLayer),
 $2) ;
 }
 number_list ';'
 { if (lefrLayerCbk) lefiLayer_addAcWidth( &(lefrLayer) ) ;
 }
;
 
 dc_layer_table_opt_list:
 | dc_layer_table_opt_list dc_layer_table_opt
;
 
 dc_layer_table_opt:
 K_CUTAREA NUMBER
 { if (lefrLayerCbk) lefiLayer_addNumber( &(lefrLayer),
 $2) ;
 }
 number_list ';'
 { if (lefrLayerCbk) lefiLayer_addDcCutarea( &(lefrLayer) ) ;
 }
 | K_TABLEENTRIES NUMBER
 { if (lefrLayerCbk) lefiLayer_addNumber( &(lefrLayer),
 $2) ;
 }
 number_list
 { if (lefrLayerCbk) lefiLayer_addDcTableEntry( &(lefrLayer) ) ;
 }
 | K_WIDTH NUMBER
 { if (lefrLayerCbk) lefiLayer_addNumber( &(lefrLayer),
 $2) ;
 }
 number_list ';'
 { if (lefrLayerCbk) lefiLayer_addDcWidth( &(lefrLayer) ) ;
 }
;
 
 number_list:
 | number_list NUMBER
 { if (lefrLayerCbk) lefiLayer_addNumber( &(lefrLayer),
 $2) ;
 }
;
 
 layer_prop_list:
 layer_prop
 | layer_prop_list layer_prop
 ;
 
 layer_prop:
 T_STRING T_STRING
 { if (lefrLayerCbk) lefiLayer_addProp( &(lefrLayer),
 $1,
$2) ;
 }
 | T_STRING QSTRING
 { if (lefrLayerCbk) lefiLayer_addProp( &(lefrLayer),
 $1,
$2) ;
 }
 | T_STRING NUMBER
 {
 char temp[32];
 sprintf(temp, "%g", $2);
 if (lefrLayerCbk) lefiLayer_addProp( &(lefrLayer),
 $1,
temp) ;
 }
;
 
 current_density_pwl_list :
 current_density_pwl
 { }
 | current_density_pwl_list current_density_pwl
 { }
;
 
 current_density_pwl: '(' NUMBER NUMBER ')'
 { if (lefrLayerCbk) lefiLayer_setCurrentPoint( &(lefrLayer),
 $2,
$3) ;
 }
;
 
 cap_points :
 cap_point
 | cap_points cap_point
 ;
 
 cap_point: '(' NUMBER NUMBER ')'
 { if (lefrLayerCbk) lefiLayer_setCapacitancePoint( &(lefrLayer),
 $2,
$3) ;
 }
;
 
 res_points :
 res_point
 | res_points res_point
 { }
;
 
 res_point: '(' NUMBER NUMBER ')'
 { if (lefrLayerCbk) lefiLayer_setResistancePoint( &(lefrLayer),
 $2,
$3) ;
 }
;
 
 layer_type:
 K_ROUTING {$$ = (char*)"ROUTING";}
 | K_CUT {$$ = (char*)"CUT";}
 | K_OVERLAP {$$ = (char*)"OVERLAP";}
 | K_MASTERSLICE {$$ = (char*)"MASTERSLICE";}
 | K_VIRTUAL {$$ = (char*)"VIRTUAL";}
;
 
 layer_direction:
 K_HORIZONTAL {$$ = (char*)"HORIZONTAL";}
 | K_VERTICAL {$$ = (char*)"VERTICAL";}
;
 
 via: start_via via_options end_via
 { CALLBACK(lefrViaCbk, lefrViaCbkType, &lefrVia); }
;
 
 via_keyword : K_VIA /* neededto have a VIA named via*/
 { lefDumbMode = 1; lefNoNum = 1;}
;
 
 start_via: via_keyword T_STRING
 {
 /* 0 is nodefault */

 if (lefrViaCbk) lefiVia_setName( &(lefrVia),
 $2,
0) ;
 }
 | via_keyword T_STRING K_DEFAULT
 {
 /* 1 is default */

 if (lefrViaCbk) lefiVia_setName( &(lefrVia),
 $2,
1) ;
 }
;
 
 via_options:
 /* empty */

 | via_options via_option
 ;
 
 via_option:
 via_foreign
 { }
 | via_layer_rule
 { }
 | K_RESISTANCE NUMBER ';'
 { if (lefrViaCbk) lefiVia_setResistance( &(lefrVia),
 $2) ;
 }
 | K_PROPERTY { lefDumbMode = 1000000; } via_prop_list ';'
 { lefDumbMode = 0; }
 | K_TOPOFSTACKONLY
 { if (lefrViaCbk) lefiVia_setTopOfStack( &(lefrVia) ) ;
 }
;
 
 via_prop_list:
 via_name_value_pair
 | via_prop_list via_name_value_pair
 ;
 
 via_name_value_pair:
 T_STRING NUMBER
 { if (lefrViaCbk) lefiVia_addNumProp( &(lefrVia),
 $1,
$2) ;
 }
 | T_STRING QSTRING
 { if (lefrViaCbk) lefiVia_addProp( &(lefrVia),
 $1,
$2) ;
 }
 | T_STRING T_STRING
 { if (lefrViaCbk) lefiVia_addProp( &(lefrVia),
 $1,
$2) ;
 }
;
 
 via_foreign:
 start_foreign ';'
 { if (lefrViaCbk) lefiVia_setForeign( &(lefrVia),
 $1,
0,
0.0,
0.0,
-1) ;
 }
 | start_foreign pt ';'
 { if (lefrViaCbk) lefiVia_setForeign( &(lefrVia),
 $1,
1,
$2.x,
$2.y,
-1) ;
 }
 | start_foreign pt orientation ';'
 { if (lefrViaCbk) lefiVia_setForeign( &(lefrVia),
 $1,
1,
$2.x,
$2.y,
$3) ;
 }
 | start_foreign orientation ';'
 { if (lefrViaCbk) lefiVia_setForeign( &(lefrVia),
 $1,
0,
0.0,
0.0,
$2) ;
 }
;
 
 start_foreign:	K_FOREIGN {lefDumbMode = 1; lefNoNum= 1;} T_STRING
 { $$ = $3; }
;
 
 orientation:
 K_N {$$ = 0;}
 | K_E {$$ = 3;}
 | K_S {$$ = 2;}
 | K_W {$$ = 1;}
 | K_FN {$$ = 4;}
 | K_FE {$$ = 5;}
 | K_FS {$$ = 6;}
 | K_FW {$$ = 7;}
 | K_R0 {$$ = 0;}
 | K_R90 {$$ = 1;}
 | K_R180 {$$ = 2;}
 | K_R270 {$$ = 3;}
 | K_MX {$$ = 4;}
 | K_MY {$$ = 5;}
 | K_MXR90 {$$ = 6;}
 | K_MYR90 {$$ = 7;}
;
 
 via_layer_rule: via_layer via_rects
 { }
;
 
 via_layer: K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
 { if (lefrViaCbk) lefiVia_addLayer( &(lefrVia),
 $3) ;
 }
;
 
 via_rects:
 /* empty */

 | via_rects via_rect
 ;
 
 via_rect: K_RECT pt pt ';'
 { if (lefrViaCbk)
 lefiVia_addRectToLayer( &(lefrVia),
 $2.x,
$2.y,
$3.x,
$3.y) ;
 }
;
 
 end_via: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
 { }
;
 
 viarule_keyword : K_VIARULE { lefDumbMode = 1; lefNoNum = 1;} T_STRING
 { if (lefrViaRuleCbk) lefiViaRule_setName( &(lefrViaRule),
 $3) ;
 }
;
 
 viarule:
 viarule_keyword viarule_layer_list via_names opt_viarule_props end_viarule
 { CALLBACK(lefrViaRuleCbk, lefrViaRuleCbkType, &lefrViaRule); }
;
 
 viarule_generate:
 viarule_keyword K_GENERATE
 viarule_layer_list opt_viarule_props end_viarule
 {
 if (lefrViaRuleCbk) lefiViaRule_setGenerate( &(lefrViaRule) ) ;
 CALLBACK(lefrViaRuleCbk, lefrViaRuleCbkType, &lefrViaRule);
 }
;
 
 viarule_layer_list :
 viarule_layer
 | viarule_layer_list viarule_layer
 ;
 
 opt_viarule_props:
 /* empty */

 | viarule_props
 ;
 
 viarule_props:
 viarule_prop
 | viarule_props viarule_prop
 ;
 
 viarule_prop: K_PROPERTY { lefDumbMode = 10000000; } viarule_prop_list ';'
 { lefDumbMode = 0; }
;
 
 viarule_prop_list:
 viarule_prop
 | viarule_prop_list viarule_prop
 ;
 
 viarule_prop:
 T_STRING T_STRING
 { if (lefrViaRuleCbk) lefiViaRule_addProp( &(lefrViaRule),
 $1,
$2) ;
 }
 | T_STRING QSTRING
 { if (lefrViaRuleCbk) lefiViaRule_addProp( &(lefrViaRule),
 $1,
$2) ;
 }
 | T_STRING NUMBER
 {
 char temp[32];
 sprintf(temp, "%g", $2);
 if (lefrViaRuleCbk) lefiViaRule_addProp( &(lefrViaRule),
 $1,
temp) ;
 }
;
 
 viarule_layer: viarule_layer_name viarule_layer_options
 ;
 
 via_names:
 /* empty */

 | via_names via_name
 ;
 
 via_name: via_keyword T_STRING ';'
 { if (lefrViaRuleCbk) lefiViaRule_addViaName( &(lefrViaRule),
 $2) ;
 }
;
 
 viarule_layer_name: K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
 { if (lefrViaRuleCbk) lefiViaRule_setLayer( &(lefrViaRule),
 $3) ;
 }
;
 
 viarule_layer_options:
 /* empty */

 | viarule_layer_options viarule_layer_option
 ;
 
 viarule_layer_option:
 K_DIRECTION K_HORIZONTAL ';'
 { if (lefrViaRuleCbk) lefiViaRule_setHorizontal( &(lefrViaRule) ) ;
 }
 | K_DIRECTION K_VERTICAL ';'
 { if (lefrViaRuleCbk) lefiViaRule_setVertical( &(lefrViaRule) ) ;
 }
 | K_WIDTH NUMBER K_TO NUMBER ';'
 { if (lefrViaRuleCbk) lefiViaRule_setWidth( &(lefrViaRule),
 $2,
$4) ;
 }
 | K_OVERHANG NUMBER ';'
 { if (lefrViaRuleCbk) lefiViaRule_setOverhang( &(lefrViaRule),
 $2) ;
 }
 | K_METALOVERHANG NUMBER ';'
 { if (lefrViaRuleCbk) lefiViaRule_setMetalOverhang( &(lefrViaRule),
 $2) ;
 }
 | K_RECT pt pt ';'
 { if (lefrViaRuleCbk)
 lefiViaRule_setRect( &(	lefrViaRule),
 $2.x,
$2.y,
$3.x,
$3.y) ;
 }
 | K_SPACING NUMBER K_BY NUMBER ';'
 { if (lefrViaRuleCbk) lefiViaRule_setSpacing( &(lefrViaRule),
 $2,
$4) ;
 }
 | K_RESISTANCE NUMBER ';'
 { if (lefrViaRuleCbk) lefiViaRule_setResistance( &(lefrViaRule),
 $2) ;
 }
;
 
 
 end_viarule: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
 { }
;
 
 spacing_rule: start_spacing spacings end_spacing
 { }
;
 
 start_spacing: K_SPACING
 { if (lefrSpacingBeginCbk)
 	CALLBACK(lefrSpacingBeginCbk, lefrSpacingBeginCbkType, 0); }
;
 
 end_spacing: K_END K_SPACING
 { if (lefrSpacingEndCbk)
 CALLBACK(lefrSpacingEndCbk, lefrSpacingEndCbkType, 0); }
;
 
 spacings:
 /* empty*/

 | spacings spacing
 ;
 
 spacing: samenet_keyword T_STRING T_STRING NUMBER ';'
 {
 if (lefrSpacingCbk) {
 lefiSpacing_set( &(	lefrSpacing),
 $2,
$3,
$4,
0) ;
 	CALLBACK(lefrSpacingCbk, lefrSpacingCbkType, &lefrSpacing);
 }
 }
 | samenet_keyword T_STRING T_STRING NUMBER K_STACK ';'
 {
 if (lefrSpacingCbk) {
 lefiSpacing_set( &(	lefrSpacing),
 $2,
$3,
$4,
1) ;
 	CALLBACK(lefrSpacingCbk, lefrSpacingCbkType, &lefrSpacing);
 }
 }
;
 
 samenet_keyword: K_SAMENET
 { lefDumbMode = 2; lefNoNum = 2; } /* must be followed by two names */
;
 
 irdrop: start_irdrop ir_tables end_irdrop
 { }
;
 
 start_irdrop: K_IRDROP
 { if (lefrIRDropBeginCbk)
 	CALLBACK(lefrIRDropBeginCbk, lefrIRDropBeginCbkType, 0); }
;
 
 end_irdrop: K_END K_IRDROP
 { if (lefrIRDropEndCbk)
 	 CALLBACK(lefrIRDropEndCbk, lefrIRDropEndCbkType, 0); }
;
 
 ir_tables:
 /* empty */

 | ir_tables ir_table
 ;
 
 ir_table: ir_tablename ir_table_values ';'
 { CALLBACK(lefrIRDropCbk, lefrIRDropCbkType, &lefrIRDrop); }
;
 
 ir_table_values:
 /* empty */

 | ir_table_values ir_table_value
 ;
 
 ir_table_value: NUMBER NUMBER
 { if (lefrIRDropCbk) lefiIRDrop_setValues( &(lefrIRDrop),
 $1,
$2) ;
 }
;
 
 ir_tablename: K_TABLE T_STRING
 { if (lefrIRDropCbk) lefiIRDrop_setTableName( &(lefrIRDrop),
 $2) ;
 }
;
 
 minfeature: K_MINFEATURE NUMBER NUMBER ';'
 {
 if (lefrMinFeatureCbk)
 lefiMinFeature_set( &(lefrMinFeature),
 $2,
$3) ;
 CALLBACK(lefrMinFeatureCbk, lefrMinFeatureCbkType, &lefrMinFeature);
 }
;
 
 dielectric: K_DIELECTRIC NUMBER ';'
 { CALLBACK(lefrDielectricCbk, lefrDielectricCbkType, $2); }
;
 
 nondefault_rule: K_NONDEFAULTRULE T_STRING
 { if (lefrNonDefaultCbk) lefiNonDefault_setName( &(lefrNonDefault),
 $2) ;
 }
 nd_rules K_END T_STRING
 {
 if (lefrNonDefaultCbk) lefiNonDefault_end( &(lefrNonDefault) ) ;
 CALLBACK(lefrNonDefaultCbk, lefrNonDefaultCbkType, &lefrNonDefault);
 }
;
 
 nd_rules:
 /* empty */

 | nd_rules nd_rule
 ;
 
 nd_rule:
 nd_layer
 | via
 | spacing_rule
 | nd_prop
 ;
 
 nd_prop: K_PROPERTY { lefDumbMode = 10000000; } nd_prop_list ';'
 { lefDumbMode = 0; }
;
 
 nd_prop_list:
 nd_prop
 | nd_prop_list nd_prop
 ;
 
 nd_prop:
 T_STRING T_STRING
 { if (lefrNonDefaultCbk) lefiNonDefault_addProp( &(lefrNonDefault),
 $1,
$2) ;
 }
 | T_STRING QSTRING
 { if (lefrNonDefaultCbk) lefiNonDefault_addProp( &(lefrNonDefault),
 $1,
$2) ;
 }
 | T_STRING NUMBER
 {
 char temp[32];
 sprintf(temp, "%g", $2);
 if (lefrNonDefaultCbk)
 lefiNonDefault_addProp( &(	lefrNonDefault),
 $1,
temp) ;
 }
;
 
 nd_layer: K_LAYER T_STRING
 { if (lefrNonDefaultCbk) lefiNonDefault_addLayer( &(lefrNonDefault),
 $2) ;
 }
 nd_layer_stmts K_END T_STRING
 ;
 
 nd_layer_stmts:
 /* empty */

 | nd_layer_stmts nd_layer_stmt
 ;
 
 nd_layer_stmt:
 K_WIDTH NUMBER ';'
 { if (lefrNonDefaultCbk) lefiNonDefault_addWidth( &(lefrNonDefault),
 $2) ;
 }
 | K_SPACING NUMBER ';'
 { if (lefrNonDefaultCbk) lefiNonDefault_addSpacing( &(lefrNonDefault),
 $2) ;
 }
 | K_WIREEXTENSION NUMBER ';'
 { if (lefrNonDefaultCbk)
 lefiNonDefault_addWireExtension( &(lefrNonDefault),
 $2) ;
 }
;
 
 site: start_site site_options end_site
 { CALLBACK(lefrSiteCbk, lefrSiteCbkType, &lefrSite); }
;
 
 start_site: K_SITE {lefDumbMode = 1; lefNoNum = 1;} T_STRING
 { if (lefrSiteCbk) lefiSite_setName( &(lefrSite),
 $3) ;
 }
;
 
 end_site: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
 { }
;
 
 site_options:
 /* empty */

 | site_options site_option
 ;
 
 site_option:
 K_SIZE NUMBER K_BY NUMBER ';'
 { if (lefrSiteCbk) lefiSite_setSize( &(lefrSite),
 $2,
$4) ;
 }
 | site_symmetry_statement
 { }
 | site_class
 { if (lefrSiteCbk) lefiSite_setClass( &(lefrSite),
 $1) ;
 }
;
 
 site_class:
 K_CLASS K_PAD ';' {$$ = (char*)"PAD"; }
 | K_CLASS K_CORE ';' {$$ = (char*)"CORE"; }
 | K_CLASS K_VIRTUAL ';' {$$ = (char*)"VIRTUAL"; }
;
 
 site_symmetry_statement: K_SYMMETRY site_symmetries ';'
 { }
;
 
 site_symmetries:
 /* empty */

 | site_symmetries site_symmetry
 ;
 
 site_symmetry:
 K_X
 { if (lefrSiteCbk) lefiSite_setXSymmetry( &(lefrSite) ) ;
 }
 | K_Y
 { if (lefrSiteCbk) lefiSite_setYSymmetry( &(lefrSite) ) ;
 }
 | K_R90
 { if (lefrSiteCbk) lefiSite_set90Symmetry( &(lefrSite) ) ;
 }
;
 
 pt:
 NUMBER NUMBER
 { $$.x = $1; $$.y = $2; }
 | '(' NUMBER NUMBER ')'
 { $$.x = $2; $$.y = $3; }
;
 
 macro: start_macro macro_options end_macro
 { CALLBACK(lefrMacroCbk, lefrMacroCbkType, &lefrMacro); lefrDoSite = 0; }
;
 
 start_macro: K_MACRO {lefDumbMode = 1; lefNoNum = 1;} T_STRING
 {
 siteDef = 0;
 symDef = 0;
 sizeDef = 0;
 pinDef = 0;
 obsDef = 0;
 origDef = 0;
 lefiMacro_clear( &(lefrMacro) ) ;
 if (lefrMacroCbk) lefiMacro_setName( &(lefrMacro),
 $3) ;
 CALLBACK(lefrMacroBeginCbk, lefrMacroBeginCbkType, $3);
 }
;
 
 end_macro: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
 { }
;
 
 macro_options:
 /* empty */

 | macro_options macro_option
 ;
 
 macro_option:
 macro_class
 | macro_generator
 | macro_generate
 | macro_source
 | macro_symmetry_statement
 | macro_origin
 { }
 | macro_power
 { }
 | macro_foreign
 { }
 | macro_eeq
 | macro_leq
 | macro_size
 { }
 | macro_site
 { }
 | macro_pin
 { }
 | K_FUNCTION K_BUFFER ';'
 { if (lefrMacroCbk) lefiMacro_setBuffer( &(lefrMacro) ) ;
 }
 | K_FUNCTION K_INVERTER ';'
 { if (lefrMacroCbk) lefiMacro_setInverter( &(lefrMacro) ) ;
 }
 | macro_obs
 { }
 | macro_clocktype
 { }
 | timing
 { }
 | K_PROPERTY {lefDumbMode = 1000000;} macro_prop_list ';'
 { lefDumbMode = 0; }
;
 
 macro_prop_list:
 macro_name_value_pair
 | macro_prop_list macro_name_value_pair
 ;
 
 macro_symmetry_statement: K_SYMMETRY macro_symmetries ';'
 { symDef = 1; }
;
 
 macro_symmetries:
 /* empty */

 | macro_symmetries macro_symmetry
 ;
 
 macro_symmetry:
 K_X
 { if (lefrMacroCbk) lefiMacro_setXSymmetry( &(lefrMacro) ) ;
 }
 | K_Y
 { if (lefrMacroCbk) lefiMacro_setYSymmetry( &(lefrMacro) ) ;
 }
 | K_R90
 { if (lefrMacroCbk) lefiMacro_set90Symmetry( &(lefrMacro) ) ;
 }
;
 
 macro_name_value_pair:
 T_STRING NUMBER
 { if (lefrMacroCbk) lefiMacro_setNumProperty( &(lefrMacro),
 $1,
$2) ;
 }
 | T_STRING QSTRING
 { if (lefrMacroCbk) lefiMacro_setProperty( &(lefrMacro),
 $1,
$2) ;
 }
 | T_STRING T_STRING
 { if (lefrMacroCbk) lefiMacro_setProperty( &(lefrMacro),
 $1,
$2) ;
 }
;
 
 macro_class: K_CLASS class_type ';'
 { if (lefrMacroCbk) lefiMacro_setClass( &(lefrMacro),
 $2) ;
 }
;
 
 class_type:
 K_COVER {$$ = (char*)"COVER"; }
 | K_RING {$$ = (char*)"RING"; }
 | K_BLOCK {$$ = (char*)"BLOCK"; }
 | K_PAD {$$ = (char*)"PAD"; }
 | K_VIRTUAL {$$ = (char*)"VIRTUAL"; }
 | K_PAD pad_type
 {sprintf(temp_name, "PAD %s", $2);
 $$ = temp_name;}
 | K_CORE {$$ = (char*)"CORE"; }
 | K_CORNER
 {$$ = (char*)"CORNER";
 /* This token is NOT in the spec but has shown up in
 * some lef files. This exception came from LEFOUT
 * in 'frameworks'
 */

 }
 | K_CORE core_type
 {sprintf(temp_name, "CORE %s", $2);
 $$ = temp_name;}
 | K_ENDCAP endcap_type
 {sprintf(temp_name, "ENDCAP %s", $2);
 $$ = temp_name;}
;
 
 pad_type:
 K_INPUT 	{$$ = (char*)"INPUT";}
 | K_OUTPUT	{$$ = (char*)"OUTPUT";}
 | K_INOUT 	{$$ = (char*)"INOUT";}
 | K_POWER 	{$$ = (char*)"POWER";}
 | K_SPACER	{$$ = (char*)"SPACER";}
;
 
 core_type:
 K_FEEDTHRU 	{$$ = (char*)"FEEDTHRU";}
 | K_TIEHIGH 	{$$ = (char*)"TIEHIGH";}
 | K_TIELOW 	{$$ = (char*)"TIELOW";}
;
 
 endcap_type:
 K_PRE 	{$$ = (char*)"PRE";}
 | K_POST 	{$$ = (char*)"POST";}
 | K_TOPLEFT 	{$$ = (char*)"TOPLEFT";}
 | K_TOPRIGHT 	{$$ = (char*)"TOPRIGHT";}
 | K_BOTTOMLEFT 	{$$ = (char*)"BOTTOMLEFT";}
 | K_BOTTOMRIGHT	{$$ = (char*)"BOTTOMRIGHT";}
;
 
 macro_generator: K_GENERATOR T_STRING ';'
 { if (lefrMacroCbk) lefiMacro_setGenerator( &(lefrMacro),
 $2) ;
 }
;
 
 macro_generate: K_GENERATE T_STRING T_STRING ';'
 { if (lefrMacroCbk) lefiMacro_setGenerate( &(lefrMacro),
 $2,
$3) ;
 }
;
 
 macro_source:
 K_SOURCE K_USER ';'
 { if (lefrMacroCbk) lefiMacro_setSource( &(lefrMacro),
 "USER") ;
 }
 | K_SOURCE K_GENERATE ';'
 { if (lefrMacroCbk) lefiMacro_setSource( &(lefrMacro),
 "GENERATE") ;
 }
 | K_SOURCE K_BLOCK ';'
 { if (lefrMacroCbk) lefiMacro_setSource( &(lefrMacro),
 "BLOCK") ;
 }
;
 
 macro_power: K_POWER NUMBER ';'
 { if (lefrMacroCbk) lefiMacro_setPower( &(lefrMacro),
 $2) ;
 }
;
 
 macro_origin: K_ORIGIN pt ';'
 {
 if (origDef) { /* Has multiple ORIGIN defined in a macro, stop parsing*/

 yyerror("ORIGIN statement is defined more than once. Parser stops executions.");
 return 1;
 }
 origDef = 1;
 if (siteDef) { /* SITE is defined before ORIGIN, error, stop parsing */

 yyerror("SITE is defined before ORIGIN. Parser stops execution.");
 return 1;
 } else if (pinDef) { /* PIN is defined before ORIGIN */

 yyerror("PIN is defined before ORIGIN. Parser stops execution");
 return 1;
 } else if (obsDef) { /* OBS is defined before ORIGIN */

 yyerror("OBS is defined before ORIGIN. Parser stops execution");
 return 1;
 }
 if (lefrMacroCbk) lefiMacro_setOrigin( &(lefrMacro),
 $2.x,
$2.y) ;
 }
;
 
 macro_foreign:
 start_foreign ';'
 { if (lefrMacroCbk)
 lefiMacro_setForeign( &(lefrMacro),
 $1,
0,
0.0,
0.0,
-1) ;
 }
 | start_foreign pt ';'
 { if (lefrMacroCbk)
 lefiMacro_setForeign( &(lefrMacro),
 $1,
1,
$2.x,
$2.y,
-1) ;
 }
 | start_foreign pt orientation ';'
 { if (lefrMacroCbk)
 lefiMacro_setForeign( &(lefrMacro),
 $1,
1,
$2.x,
$2.y,
$3) ;
 }
 | start_foreign orientation ';'
 { if (lefrMacroCbk)
 lefiMacro_setForeign( &(lefrMacro),
 $1,
0,
0.0,
0.0,
$2) ;
 }
;
 
 macro_eeq: K_EEQ { lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
 { if (lefrMacroCbk) lefiMacro_setEEQ( &(lefrMacro),
 $3) ;
 }
;
 
 macro_leq: K_LEQ { lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
 { if (lefrMacroCbk) lefiMacro_setLEQ( &(lefrMacro),
 $3) ;
 }
;
 
 macro_site:
 macro_site_word T_STRING ';'
 { if (lefrMacroCbk) lefiMacro_setSiteName( &(lefrMacro),
 $2) ;
 }
 | macro_site_word sitePattern ';'
 {
 if (lefrMacroCbk) {
 lefiMacro_setSitePattern( &(	lefrMacro),
 lefrSitePatternPtr) ;
 	lefrSitePatternPtr = 0;
 }
 }
;
 
 macro_site_word: K_SITE
 { lefDumbMode = 1; lefNoNum = 1; siteDef = 1;
 if (lefrMacroCbk) lefrDoSite = 1; }
;
 
 site_word: K_SITE
 { lefDumbMode = 1; lefNoNum = 1; }
;
 
 macro_size: K_SIZE NUMBER K_BY NUMBER ';'
 {
 sizeDef = 1;
 if (lefrMacroCbk) lefiMacro_setSize( &(lefrMacro),
 $2,
$4) ;
 }
 
 /* This is confusing, since FEF and LEF have opposite definitions of
 ports and pins */

;
 
 macro_pin: start_macro_pin macro_pin_options end_macro_pin
 { CALLBACK(lefrPinCbk, lefrPinCbkType, &lefrPin); }
;
 
 start_macro_pin: K_PIN {lefDumbMode = 1; lefNoNum = 1; pinDef = 1;} T_STRING
 { if (lefrPinCbk) lefiPin_setName( &(lefrPin),
 $3) ;
 }
;
 
 end_macro_pin: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
 { }
;
 
 macro_pin_options:
 /* empty */

 { }
 | macro_pin_options macro_pin_option
 { }
;
 
 macro_pin_option:
 start_foreign ';'
 { if (lefrPinCbk) lefiPin_setForeign( &(lefrPin),
 $1,
0,
0.0,
0.0,
-1) ;
 }
 | start_foreign pt ';'
 { if (lefrPinCbk) lefiPin_setForeign( &(lefrPin),
 $1,
1,
$2.x,
$2.y,
-1) ;
 }
 | start_foreign pt orientation ';'
 { if (lefrPinCbk) lefiPin_setForeign( &(lefrPin),
 $1,
1,
$2.x,
$2.y,
$3) ;
 }
 | start_foreign K_STRUCTURE ';'
 { if (lefrPinCbk) lefiPin_setForeign( &(lefrPin),
 $1,
0,
0.0,
0.0,
-1) ;
 }
 | start_foreign K_STRUCTURE pt ';'
 { if (lefrPinCbk) lefiPin_setForeign( &(lefrPin),
 $1,
1,
$3.x,
$3.y,
-1) ;
 }
 | start_foreign K_STRUCTURE pt orientation ';'
 { if (lefrPinCbk) lefiPin_setForeign( &(lefrPin),
 $1,
1,
$3.x,
$3.y,
$4) ;
 }
 | K_LEQ { lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
 { if (lefrPinCbk) lefiPin_setLEQ( &(lefrPin),
 $3) ;
 }
 | K_POWER NUMBER ';'
 { if (lefrPinCbk) lefiPin_setPower( &(lefrPin),
 $2) ;
 }
 | electrical_direction
 { if (lefrPinCbk) lefiPin_setDirection( &(lefrPin),
 $1) ;
 }
 | K_USE macro_pin_use ';'
 { if (lefrPinCbk) lefiPin_setUse( &(lefrPin),
 $2) ;
 }
 | K_SCANUSE macro_scan_use ';'
 { }
 | K_LEAKAGE NUMBER ';'
 { if (lefrPinCbk) lefiPin_setLeakage( &(lefrPin),
 $2) ;
 }
 | K_RISETHRESH NUMBER ';'
 { if (lefrPinCbk) lefiPin_setRiseThresh( &(lefrPin),
 $2) ;
 }
 | K_FALLTHRESH NUMBER ';'
 { if (lefrPinCbk) lefiPin_setFallThresh( &(lefrPin),
 $2) ;
 }
 | K_RISESATCUR NUMBER ';'
 { if (lefrPinCbk) lefiPin_setRiseSatcur( &(lefrPin),
 $2) ;
 }
 | K_FALLSATCUR NUMBER ';'
 { if (lefrPinCbk) lefiPin_setFallSatcur( &(lefrPin),
 $2) ;
 }
 | K_VLO NUMBER ';'
 { if (lefrPinCbk) lefiPin_setVLO( &(lefrPin),
 $2) ;
 }
 | K_VHI NUMBER ';'
 { if (lefrPinCbk) lefiPin_setVHI( &(lefrPin),
 $2) ;
 }
 | K_TIEOFFR NUMBER ';'
 { if (lefrPinCbk) lefiPin_setTieoffr( &(lefrPin),
 $2) ;
 }
 | K_SHAPE pin_shape ';'
 { if (lefrPinCbk) lefiPin_setShape( &(lefrPin),
 $2) ;
 }
 | K_MUSTJOIN {lefDumbMode = 1; lefNoNum = 1;} T_STRING ';'
 { if (lefrPinCbk) lefiPin_setMustjoin( &(lefrPin),
 $3) ;
 }
 | K_OUTPUTNOISEMARGIN {lefDumbMode = 1;} NUMBER NUMBER ';'
 { if (lefrPinCbk) lefiPin_setOutMargin( &(lefrPin),
 $3,
$4) ;
 }
 | K_OUTPUTRESISTANCE {lefDumbMode = 1;} NUMBER NUMBER ';'
 { if (lefrPinCbk) lefiPin_setOutResistance( &(lefrPin),
 $3,
$4) ;
 }
 | K_INPUTNOISEMARGIN {lefDumbMode = 1;} NUMBER NUMBER ';'
 { if (lefrPinCbk) lefiPin_setInMargin( &(lefrPin),
 $3,
$4) ;
 }
 | K_CAPACITANCE NUMBER ';'
 { if (lefrPinCbk) lefiPin_setCapacitance( &(lefrPin),
 $2) ;
 }
 | K_MAXDELAY NUMBER ';'
 { if (lefrPinCbk) lefiPin_setMaxdelay( &(lefrPin),
 $2) ;
 }
 | K_MAXLOAD NUMBER ';'
 { if (lefrPinCbk) lefiPin_setMaxload( &(lefrPin),
 $2) ;
 }
 | K_RESISTANCE NUMBER ';'
 { if (lefrPinCbk) lefiPin_setResistance( &(lefrPin),
 $2) ;
 }
 | K_PULLDOWNRES NUMBER ';'
 { if (lefrPinCbk) lefiPin_setPulldownres( &(lefrPin),
 $2) ;
 }
 | K_CURRENTSOURCE K_ACTIVE ';'
 { if (lefrPinCbk) lefiPin_setCurrentSource( &(lefrPin),
 "ACTIVE") ;
 }
 | K_CURRENTSOURCE K_RESISTIVE ';'
 { if (lefrPinCbk) lefiPin_setCurrentSource( &(lefrPin),
 "RESISTIVE") ;
 }
 | K_RISEVOLTAGETHRESHOLD NUMBER ';'
 { if (lefrPinCbk) lefiPin_setRiseVoltage( &(lefrPin),
 $2) ;
 }
 | K_FALLVOLTAGETHRESHOLD NUMBER ';'
 { if (lefrPinCbk) lefiPin_setFallVoltage( &(lefrPin),
 $2) ;
 }
 | K_IV_TABLES T_STRING T_STRING ';'
 { if (lefrPinCbk) lefiPin_setTables( &(lefrPin),
 $2,
$3) ;
 }
 | K_TAPERRULE T_STRING ';'
 { if (lefrPinCbk) lefiPin_setTaperRule( &(lefrPin),
 $2) ;
 }
 | K_PROPERTY {lefDumbMode = 1000000;} pin_prop_list ';'
 { lefDumbMode = 0; }
 | start_macro_port geometries K_END
 {
 lefDumbMode = 0;
 if (lefrPinCbk) {
 lefiPin_addPort( &(	lefrPin),
 lefrGeometriesPtr) ;
 	lefrGeometriesPtr = 0;
 	lefrDoGeometries = 0;
 }
 }
 | K_ANTENNASIZE NUMBER opt_layer_name ';'
 { if (lefrPinCbk) lefiPin_addAntennaSize( &(lefrPin),
 $2,
$3) ;
 }
 | K_ANTENNAMETALAREA NUMBER opt_layer_name ';'
 { if (lefrPinCbk) lefiPin_addAntennaMetalArea( &(lefrPin),
 $2,
$3) ;
 }
 | K_ANTENNAMETALLENGTH NUMBER opt_layer_name ';'
 { if (lefrPinCbk) lefiPin_addAntennaMetalLength( &(lefrPin),
 $2,
$3) ;
 }
 | K_RISESLEWLIMIT NUMBER ';'
 { if (lefrPinCbk) lefiPin_setRiseSlewLimit( &(lefrPin),
 $2) ;
 }
 | K_FALLSLEWLIMIT NUMBER ';'
 { if (lefrPinCbk) lefiPin_setFallSlewLimit( &(lefrPin),
 $2) ;
 }
;
 
 
 pin_prop_list:
 pin_name_value_pair
 | pin_prop_list pin_name_value_pair
 ;
 
 pin_name_value_pair:
 T_STRING NUMBER
 { if (lefrPinCbk) lefiPin_setNumProperty( &(lefrPin),
 $1,
$2) ;
 }
 | T_STRING QSTRING
 { if (lefrPinCbk) lefiPin_setProperty( &(lefrPin),
 $1,
$2) ;
 }
 | T_STRING T_STRING
 { if (lefrPinCbk) lefiPin_setProperty( &(lefrPin),
 $1,
$2) ;
 }
;
 
 electrical_direction:
 K_DIRECTION K_INPUT ';' {$$ = (char*)"INPUT";}
 | K_DIRECTION K_OUTPUT ';' {$$ = (char*)"OUTPUT";}
 | K_DIRECTION K_OUTPUT K_TRISTATE ';' {$$ = (char*)"OUTPUT TRISTATE";}
 | K_DIRECTION K_INOUT ';' {$$ = (char*)"INOUT";}
 | K_DIRECTION K_FEEDTHRU ';' {$$ = (char*)"FEEDTHRU";}
;
 
 start_macro_port: K_PORT
 {
 if (lefrPinCbk) {
 	lefrDoGeometries = 1;
 	lefrGeometriesPtr = (lefiGeometries*)malloc( sizeof(lefiGeometries));
 lefiGeometries_Init( 	lefrGeometriesPtr ) ;
 }
 }
;
 
 macro_pin_use:
 K_SIGNAL	{$$ = (char*)"SIGNAL";}
 | K_ANALOG 	{$$ = (char*)"ANALOG";}
 | K_POWER 	{$$ = (char*)"POWER";}
 | K_GROUND	{$$ = (char*)"GROUND";}
 | K_CLOCK	{$$ = (char*)"CLOCK";}
 | K_DATA	{$$ = (char*)"DATA";}
;
 
 macro_scan_use:
 K_INPUT {$$ = (char*)"INPUT";}
 | K_OUTPUT	{$$ = (char*)"OUTPUT";}
 | K_START	{$$ = (char*)"START";}
 | K_STOP	{$$ = (char*)"STOP";}
;
 
 pin_shape:
 /* empty */
{$$ = (char*)""; } /* non-ring shape */

 | K_ABUTMENT	{$$ = (char*)"ABUTMENT";}
 | K_RING 	{$$ = (char*)"RING";}
 | K_FEEDTHRU	{$$ = (char*)"FEEDTHRU";}
;
 
 geometries:
 /* empty */
{ }
 | geometries geometry { }
;
 
 geometry:
 K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
 { if (lefrDoGeometries)
 lefiGeometries_addLayer( lefrGeometriesPtr,
 $3) ;
 }
 | K_WIDTH NUMBER ';'
 { if (lefrDoGeometries)
 lefiGeometries_addWidth( lefrGeometriesPtr,
 $2) ;
 }
 | K_PATH firstPt otherPts ';'
 { if (lefrDoGeometries) lefiGeometries_addPath( lefrGeometriesPtr ) ;
 }
 | K_PATH K_ITERATE firstPt otherPts stepPattern ';'
 { if (lefrDoGeometries)
 lefiGeometries_addPathIter( lefrGeometriesPtr ) ;
 }
 | K_RECT pt pt ';'
 { if (lefrDoGeometries) lefiGeometries_addRect( lefrGeometriesPtr,
 $2.x,
$2.y,
$3.x,
$3.y) ;
 }
 | K_RECT K_ITERATE pt pt stepPattern ';'
 { if (lefrDoGeometries) lefiGeometries_addRectIter( lefrGeometriesPtr,
 $3.x,
$3.y,
$4.x,
$4.y) ;
 }
 | K_POLYGON firstPt nextPt nextPt nextPt otherPts ';'
 { if (lefrDoGeometries)
 lefiGeometries_addPolygon( lefrGeometriesPtr ) ;
 }
 | K_POLYGON K_ITERATE firstPt nextPt nextPt nextPt otherPts stepPattern ';'
 { if (lefrDoGeometries)
 lefiGeometries_addPolygonIter( lefrGeometriesPtr ) ;
 }
 | via_placement
 { }
 | K_CLASS class_type ';'
 { if (lefrDoGeometries)
 lefiGeometries_addClass( lefrGeometriesPtr,
 $2) ;
 }
;
 
 firstPt: pt
 { if (lefrDoGeometries)
 lefiGeometries_startList( lefrGeometriesPtr,
 $1.x,
$1.y) ;
 }
;
 
 nextPt: pt
 { if (lefrDoGeometries)
 lefiGeometries_addToList( lefrGeometriesPtr,
 $1.x,
$1.y) ;
 }
;
 
 otherPts:
 /* empty */

 | nextPt otherPts
 ;
 
 via_placement:
 K_VIA pt {lefDumbMode = 1;} T_STRING ';'
 { if (lefrDoGeometries)
 lefiGeometries_addVia( lefrGeometriesPtr,
 $2.x,
$2.y,
$4) ;
 }
 | K_VIA K_ITERATE pt {lefDumbMode = 1; lefNoNum = 1;} T_STRING
 stepPattern ';'
 { if (lefrDoGeometries)
 lefiGeometries_addViaIter( lefrGeometriesPtr,
 $3.x,
$3.y,
$5) ;
 }
;
 
 
 stepPattern: K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
 { if (lefrDoGeometries)
 lefiGeometries_addStepPattern( lefrGeometriesPtr,
 $2,
$4,
$6,
$7) ;
 }
;
 
 sitePattern: T_STRING NUMBER NUMBER orientation
 K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
 {
 if (lefrDoSite) {
 	lefrSitePatternPtr = (lefiSitePattern*)malloc(
 				 sizeof(lefiSitePattern));
 lefiSitePattern_Init( 	lefrSitePatternPtr ) ;
 lefiSitePattern_set( 	lefrSitePatternPtr,
 $1,
$2,
$3,
$4,
$6,
$8,
$10,
$11) ;
 	}
 }
;
 
 trackPattern:
 K_X NUMBER K_DO NUMBER K_STEP NUMBER
 {
 if (lefrDoTrack) {
 	lefrTrackPatternPtr = (lefiTrackPattern*)malloc(
 				sizeof(lefiTrackPattern));
 lefiTrackPattern_Init( 	lefrTrackPatternPtr ) ;
 lefiTrackPattern_set( 	lefrTrackPatternPtr,
 "X",
$2,
$4,
$6) ;
 }
 }
 K_LAYER {lefDumbMode = 1000000000;} trackLayers
 { lefDumbMode = 0;}
 | K_Y NUMBER K_DO NUMBER K_STEP NUMBER
 {
 if (lefrDoTrack) {
 	lefrTrackPatternPtr = (lefiTrackPattern*)malloc(
 sizeof(lefiTrackPattern));
 lefiTrackPattern_Init( 	lefrTrackPatternPtr ) ;
 lefiTrackPattern_set( 	lefrTrackPatternPtr,
 "Y",
$2,
$4,
$6) ;
 }
 }
 K_LAYER {lefDumbMode = 1000000000;} trackLayers
 { lefDumbMode = 0;}
 | K_X NUMBER K_DO NUMBER K_STEP NUMBER
 {
 if (lefrDoTrack) {
 	lefrTrackPatternPtr = (lefiTrackPattern*)malloc(
 sizeof(lefiTrackPattern));
 lefiTrackPattern_Init( 	lefrTrackPatternPtr ) ;
 lefiTrackPattern_set( 	lefrTrackPatternPtr,
 "X",
$2,
$4,
$6) ;
 }
 }
 | K_Y NUMBER K_DO NUMBER K_STEP NUMBER
 {
 if (lefrDoTrack) {
 	lefrTrackPatternPtr = (lefiTrackPattern*)malloc(
 sizeof(lefiTrackPattern));
 lefiTrackPattern_Init( 	lefrTrackPatternPtr ) ;
 lefiTrackPattern_set( 	lefrTrackPatternPtr,
 "Y",
$2,
$4,
$6) ;
 }
 }
;
 
 trackLayers:
 /* empty */

 | trackLayers layer_name
 ;
 
 layer_name: T_STRING
 { if (lefrDoTrack) lefiTrackPattern_addLayer( lefrTrackPatternPtr,
 $1) ;
 }
;
 
 gcellPattern: K_X NUMBER K_DO NUMBER K_STEP NUMBER
 {
 if (lefrDoGcell) {
 	lefrGcellPatternPtr = (lefiGcellPattern*)malloc(
 sizeof(lefiGcellPattern));
 lefiGcellPattern_Init( 	lefrGcellPatternPtr ) ;
 lefiGcellPattern_set( 	lefrGcellPatternPtr,
 "X",
$2,
$4,
$6) ;
 }
 }
 | K_Y NUMBER K_DO NUMBER K_STEP NUMBER
 {
 if (lefrDoGcell) {
 	lefrGcellPatternPtr = (lefiGcellPattern*)malloc(
 sizeof(lefiGcellPattern));
 lefiGcellPattern_Init( 	lefrGcellPatternPtr ) ;
 lefiGcellPattern_set( 	lefrGcellPatternPtr,
 "Y",
$2,
$4,
$6) ;
 }
 }
;
 
 macro_obs: start_macro_obs geometries K_END
 {
 if (lefrObstructionCbk) {
 lefiObstruction_setGeometries( &(	lefrObstruction),
 lefrGeometriesPtr) ;
 	lefrGeometriesPtr = 0;
 	lefrDoGeometries = 0;
 }
 CALLBACK(lefrObstructionCbk, lefrObstructionCbkType, &lefrObstruction);
 lefDumbMode = 0;
 }
;
 
 start_macro_obs: K_OBS
 {
 obsDef = 1;
 if (lefrObstructionCbk) {
 	lefrDoGeometries = 1;
 	lefrGeometriesPtr = (lefiGeometries*)malloc(
 	 sizeof(lefiGeometries));
 lefiGeometries_Init( 	lefrGeometriesPtr ) ;
 	}
 }
;
 
 macro_clocktype: K_CLOCKTYPE { lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
 { if (lefrMacroCbk) lefiMacro_setClockType( &(lefrMacro),
 $3) ;
 }
;
 
 timing: start_timing timing_options end_timing
 { }
;
 
 start_timing: K_TIMING
 { /* XXXXX for macros */
}
;
 
 end_timing: K_END K_TIMING
 {
 if (lefrTimingCbk && lefiTiming_hasData( &(lefrTiming) ) )  CALLBACK(lefrTimingCbk, lefrTimingCbkType, &lefrTiming) ;
 lefiTiming_clear( &(lefrTiming) ) ;
 }
;
 
 timing_options:
 /* empty */

 | timing_options timing_option
 ;
 
 timing_option:
 K_FROMPIN
 {
 if (lefrTimingCbk && lefiTiming_hasData( &(lefrTiming) ) )  CALLBACK(lefrTimingCbk, lefrTimingCbkType, &lefrTiming) ;
 lefDumbMode = 1000000000;
 lefiTiming_clear( &(lefrTiming) ) ;
 }
 list_of_from_strings ';'
 { lefDumbMode = 0;}
 | K_TOPIN {lefDumbMode = 1000000000;} list_of_to_strings ';'
 { lefDumbMode = 0;}
 | risefall K_INTRINSIC NUMBER NUMBER
 { if (lefrTimingCbk) lefiTiming_addRiseFall( &(lefrTiming),
 $1,
$3,
$4) ;
 }
 slew_spec K_VARIABLE NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_addRiseFallVariable( &(lefrTiming),
 $8,
$9) ;
 }
 | risefall delay_or_transition K_UNATENESS unateness
 K_TABLEDIMENSION NUMBER NUMBER NUMBER ';'
 { if (lefrTimingCbk) {
 	if ($2[0] == 'D' || $2[0] == 'd') /* delay */

 	 lefiTiming_addDelay( &(lefrTiming),
 $1,
$4,
$6,
$7,
$8) ;
 	else
 	 lefiTiming_addTransition( &(lefrTiming),
 $1,
$4,
$6,
$7,
$8) ;
 	}
 }
 | K_TABLEAXIS list_of_table_axis_numbers ';'
 { }
 | K_TABLEENTRIES list_of_table_entries ';'
 { }
 | K_RISERS NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_setRiseRS( &(lefrTiming),
 $2,
$3) ;
 }
 | K_FALLRS NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_setFallRS( &(lefrTiming),
 $2,
$3) ;
 }
 | K_RISECS NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_setRiseCS( &(lefrTiming),
 $2,
$3) ;
 }
 | K_FALLCS NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_setFallCS( &(lefrTiming),
 $2,
$3) ;
 }
 | K_RISESATT1 NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_setRiseAtt1( &(lefrTiming),
 $2,
$3) ;
 }
 | K_FALLSATT1 NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_setFallAtt1( &(lefrTiming),
 $2,
$3) ;
 }
 | K_RISET0 NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_setRiseTo( &(lefrTiming),
 $2,
$3) ;
 }
 | K_FALLT0 NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_setFallTo( &(lefrTiming),
 $2,
$3) ;
 }
 | K_UNATENESS unateness ';'
 { if (lefrTimingCbk) lefiTiming_addUnateness( &(lefrTiming),
 $2) ;
 }
 | K_STABLE K_SETUP NUMBER K_HOLD NUMBER risefall ';'
 { if (lefrTimingCbk) lefiTiming_setStable( &(lefrTiming),
 $3,
$5,
$6) ;
 }
 | two_pin_trigger from_pin_trigger to_pin_trigger K_TABLEDIMENSION NUMBER NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_addSDF2Pins( &(lefrTiming),
 $1,
$2,
$3,
$5,
$6,
$7) ;
 }
 | one_pin_trigger K_TABLEDIMENSION NUMBER NUMBER NUMBER ';'
 { if (lefrTimingCbk) lefiTiming_addSDF1Pin( &(lefrTiming),
 $1,
$3,
$4,
$4) ;
 }
 | K_SDFCONDSTART QSTRING ';'
 { if (lefrTimingCbk) lefiTiming_setSDFcondStart( &(lefrTiming),
 $2) ;
 }
 | K_SDFCONDEND QSTRING ';'
 { if (lefrTimingCbk) lefiTiming_setSDFcondEnd( &(lefrTiming),
 $2) ;
 }
 | K_SDFCOND QSTRING ';'
 { if (lefrTimingCbk) lefiTiming_setSDFcond( &(lefrTiming),
 $2) ;
 }
 | K_EXTENSION ';'
 { /* XXXXX */
}
;
 
 one_pin_trigger:
 K_MPWH
 { $$ = (char*)"MPWH";}
 | K_MPWL
 { $$ = (char*)"MPWL";}
 | K_PERIOD
 { $$ = (char*)"PERIOD";}
;
 
 two_pin_trigger :
 K_SETUP
 { $$ = (char*)"SETUP";}
 | K_HOLD
 { $$ = (char*)"HOLD";}
 | K_RECOVERY
 { $$ = (char*)"RECOVERY";}
 | K_SKEW
 { $$ = (char*)"SKEW";}
;
 
 from_pin_trigger:
 K_ANYEDGE
 { $$ = (char*)"ANYEDGE";}
 | K_POSEDGE
 { $$ = (char*)"POSEDGE";}
 | K_NEGEDGE
 { $$ = (char*)"NEGEDGE";}
;
 
 to_pin_trigger:
 K_ANYEDGE
 { $$ = (char*)"ANYEDGE";}
 | K_POSEDGE
 { $$ = (char*)"POSEDGE";}
 | K_NEGEDGE
 { $$ = (char*)"NEGEDGE";}
;
 
 delay_or_transition :
 K_DELAY
 { $$ = (char*)"DELAY"; }
 | K_TRANSITIONTIME
 { $$ = (char*)"TRANSITION"; }
;
 
 list_of_table_entries:
 table_entry
 { }
 | list_of_table_entries table_entry
 { }
;
 
 table_entry: '(' NUMBER NUMBER NUMBER ')'
 { if (lefrTimingCbk) lefiTiming_addTableEntry( &(lefrTiming),
 $2,
$3,
$4) ;
 }
;
 
 list_of_table_axis_numbers:
 NUMBER
 { if (lefrTimingCbk) lefiTiming_addTableAxisNumber( &(lefrTiming),
 $1) ;
 }
 | list_of_table_axis_numbers NUMBER
 { if (lefrTimingCbk) lefiTiming_addTableAxisNumber( &(lefrTiming),
 $2) ;
 }
;
 
 slew_spec:
 /* empty */

 { }
 | NUMBER NUMBER NUMBER NUMBER
 { if (lefrTimingCbk) lefiTiming_addRiseFallSlew( &(lefrTiming),
 $1,
$2,
$3,
$4) ;
 }
 | NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER
 { if (lefrTimingCbk) lefiTiming_addRiseFallSlew( &(lefrTiming),
 $1,
$2,
$3,
$4) ;
 if (lefrTimingCbk) lefiTiming_addRiseFallSlew2( &(lefrTiming),
 $5,
$6,
$7) ;
 }
;
 
 risefall:
 K_RISE
 { $$ = (char*)"RISE"; }
 | K_FALL
 { $$ = (char*)"FALL"; }
;
 
 unateness:
 K_INVERT
 { $$ = (char*)"INVERT"; }
 | K_NONINVERT
 { $$ = (char*)"NONINVERT"; }
 | K_NONUNATE
 { $$ = (char*)"NONUNATE"; }
;
 
 list_of_from_strings:
 T_STRING
 { if (lefrTimingCbk) lefiTiming_addFromPin( &(lefrTiming),
 $1) ;
 }
 | list_of_from_strings T_STRING
 { if (lefrTimingCbk) lefiTiming_addFromPin( &(lefrTiming),
 $2) ;
 }
;
 
 list_of_to_strings:
 T_STRING
 { if (lefrTimingCbk) lefiTiming_addToPin( &(lefrTiming),
 $1) ;
 }
 | list_of_to_strings T_STRING
 { if (lefrTimingCbk) lefiTiming_addToPin( &(lefrTiming),
 $2) ;
 }
;
 
 array: start_array array_rules
 {
 	CALLBACK(lefrArrayCbk, lefrArrayCbkType, &lefrArray);
 lefiArray_clear( &(	lefrArray) ) ;
 	lefrSitePatternPtr = 0;
 	lefrDoSite = 0;
 }
 end_array
;
 
 start_array: K_ARRAY {lefDumbMode = 1; lefNoNum = 1;} T_STRING
 {
 if (lefrArrayCbk) {
 lefiArray_setName( &(	lefrArray),
 $3) ;
 	CALLBACK(lefrArrayBeginCbk, lefrArrayBeginCbkType, $3);
 }
 }
;
 
 end_array: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
 {
 if (lefrArrayCbk)
 	CALLBACK(lefrArrayEndCbk, lefrArrayEndCbkType, $3);
 }
;
 
 array_rules:
 /* empty */

 { }
 | array_rules array_rule
 { }
;
 
 array_rule:
 site_word { if (lefrArrayCbk) lefrDoSite = 1; lefDumbMode = 1; }
 sitePattern ';'
 {
 if (lefrArrayCbk) {
 lefiArray_addSitePattern( &(	lefrArray),
 lefrSitePatternPtr) ;
 }
 }
 | K_CANPLACE {lefDumbMode = 1; if (lefrArrayCbk) lefrDoSite = 1; }
 sitePattern ';'
 {
 if (lefrArrayCbk) {
 lefiArray_addCanPlace( &(	lefrArray),
 lefrSitePatternPtr) ;
 }
 }
 | K_CANNOTOCCUPY {lefDumbMode = 1; if (lefrArrayCbk) lefrDoSite = 1; }
 sitePattern ';'
 {
 if (lefrArrayCbk) {
 lefiArray_addCannotOccupy( &(	lefrArray),
 lefrSitePatternPtr) ;
 }
 }
 | K_TRACKS { if (lefrArrayCbk) lefrDoTrack = 1; } trackPattern ';'
 {
 if (lefrArrayCbk) {
 lefiArray_addTrack( &(	lefrArray),
 lefrTrackPatternPtr) ;
 }
 }
 | floorplan_start floorplan_list K_END T_STRING
 {
 }
 | K_GCELLGRID { if (lefrArrayCbk) lefrDoGcell = 1; } gcellPattern ';'
 {
 if (lefrArrayCbk) {
 lefiArray_addGcell( &(	lefrArray),
 lefrGcellPatternPtr) ;
 }
 }
 | K_DEFAULTCAP NUMBER cap_list K_END K_DEFAULTCAP
 {
 if (lefrArrayCbk) {
 lefiArray_setTableSize( &(	lefrArray),
 $2) ;
 }
 }
 | def_statement
 { }
;
 
 floorplan_start: K_FLOORPLAN T_STRING
 { if (lefrArrayCbk) lefiArray_addFloorPlan( &(lefrArray),
 $2) ;
 }
;
 
 floorplan_list:
 /* empty */

 { }
 | floorplan_list floorplan_element
 { }
;
 
 floorplan_element:
 K_CANPLACE { lefDumbMode = 1; if (lefrArrayCbk) lefrDoSite = 1; }
 sitePattern ';'
 {
 if (lefrArrayCbk)
 lefiArray_addSiteToFloorPlan( &(	lefrArray),
 "CANPLACE",
	lefrSitePatternPtr) ;
 }
 | K_CANNOTOCCUPY { if (lefrArrayCbk) lefrDoSite = 1; lefDumbMode = 1; }
 sitePattern ';'
 {
 if (lefrArrayCbk)
 lefiArray_addSiteToFloorPlan( &(	lefrArray),
 "CANNOTOCCUPY",
	lefrSitePatternPtr) ;
 }
;
 
 cap_list:
 /* empty */

 { }
 | cap_list one_cap
 { }
;
 
 one_cap: K_MINPINS NUMBER K_WIRECAP NUMBER ';'
 { if (lefrArrayCbk) lefiArray_addDefaultCap( &(lefrArray),
 $2,
$4) ;
 }
;
 
 msg_statement:
 K_MESSAGE {lefDumbMode=1;lefNlToken=TRUE;} T_STRING '=' s_expr dtrm
 { lefAddStringMessage($3, $5); }
;
 
 create_file_statement:
 K_CREATEFILE {lefDumbMode=1;lefNlToken=TRUE;} T_STRING '=' s_expr dtrm
 { }
;
 
 def_statement:
 K_DEFINE {lefDumbMode=1;lefNlToken=TRUE;} T_STRING '=' expression dtrm
 { lefAddNumDefine($3, $5); }
 | K_DEFINES {lefDumbMode=1;lefNlToken=TRUE;} T_STRING '=' s_expr dtrm
 { lefAddStringDefine($3, $5); }
 | K_DEFINEB {lefDumbMode=1;lefNlToken=TRUE;} T_STRING '=' b_expr dtrm
 { lefAddBooleanDefine($3, $5); }
 
 /* terminator for &defines. Can be semicolon or newline */
;

 dtrm:
 | ';' {lefNlToken = FALSE;}
 | '\n'	{lefNlToken = FALSE;}
;
 
 then:
 K_THEN
 | '\n' K_THEN
;
 
 else:
 K_ELSE
 | '\n' K_ELSE
 ;
 
 expression:
 expression '+' expression {$$ = $1 + $3; }
 | expression '-' expression {$$ = $1 - $3; }
 | expression '*' expression {$$ = $1 * $3; }
 | expression '/' expression {$$ = $1 / $3; }
 | '-' expression %prec UMINUS {$$ = -$2;}
 | '(' expression ')'		{$$ = $2;}
 | K_IF b_expr then expression else expression %prec IF
 		{$$ = ($2 != 0) ? $4 : $6;}
 | NUMBER			{$$ = $1;}
;
 
 b_expr:
 expression relop expression {$$ = comp_num($1,$2,$3);}
 | expression K_AND expression {$$ = $1 != 0 && $3 != 0;}
 | expression K_OR expression {$$ = $1 != 0 || $3 != 0;}
 | s_expr relop s_expr	 {$$ = comp_str($1,$2,$3);}
 | s_expr K_AND s_expr	 {$$ = $1[0] != 0 && $3[0] != 0;}
 | s_expr K_OR s_expr	 {$$ = $1[0] != 0 || $3[0] != 0;}
 | b_expr K_EQ b_expr	 {$$ = $1 == $3;}
 | b_expr K_NE b_expr	 {$$ = $1 != $3;}
 | b_expr K_AND b_expr	 {$$ = $1 && $3;}
 | b_expr K_OR b_expr	 {$$ = $1 || $3;}
 | K_NOT b_expr		 %prec LNOT {$$ = !$$;}
 | '(' b_expr ')'	 {$$ = $2;}
 | K_IF b_expr then b_expr else b_expr %prec IF
 	 {$$ = ($2 != 0) ? $4 : $6;}
 | K_TRUE		 {$$ = 1;}
 | K_FALSE		 {$$ = 0;}
;
 
 s_expr:
 s_expr '+' s_expr
 {
 $$ = (char*)malloc(strlen($1)+strlen($3)+1);
 strcpy($$,$1);
 strcat($$,$3);
 }
 | '(' s_expr ')'
 { $$ = $2; }
 | K_IF b_expr then s_expr else s_expr %prec IF
 {
 lefDefIf = TRUE;
 if ($2 != 0) {
 	$$ = $4;
 } else {
 	$$ = $6;
 }
 }
 | QSTRING
 { $$ = $1; }
;
 
 relop:
 K_LE {$$ = C_LE;}
 | K_LT {$$ = C_LT;}
 | K_GE {$$ = C_GE;}
 | K_GT {$$ = C_GT;}
 | K_EQ {$$ = C_EQ;}
 | K_NE {$$ = C_NE;}
 | '=' {$$ = C_EQ;}
 | '<' {$$ = C_LT;}
 | '>' {$$ = C_GT;}
;
 
 
 prop_def_section: K_PROPDEF
 { CALLBACK(lefrPropBeginCbk, lefrPropBeginCbkType, 0); }
 prop_stmts K_END K_PROPDEF
 { CALLBACK(lefrPropEndCbk, lefrPropEndCbkType, 0); }
;
 
 prop_stmts:
 /* empty */

 { }
 | prop_stmts prop_stmt
 { }
;
 
 prop_stmt:
 K_LIBRARY {lefDumbMode = 1; lefiProp_clear( &(lefrProp) ) ;
 }
 T_STRING prop_define ';'
 {
 if (lefrPropCbk) lefiProp_setPropType( &(lefrProp),
 "library",
$3) ;
 CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
 }
 | K_COMPONENTPIN {lefDumbMode = 1; lefiProp_clear( &(lefrProp) ) ;
 }
 T_STRING prop_define ';'
 {
 if (lefrPropCbk) lefiProp_setPropType( &(lefrProp),
 "componentpin",
$3) ;
 CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
 }
 | K_PIN {lefDumbMode = 1; lefiProp_clear( &(lefrProp) ) ;
 }
 T_STRING prop_define ';'
 {
 if (lefrPropCbk) lefiProp_setPropType( &(lefrProp),
 "pin",
$3) ;
 CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
 }
 | K_MACRO {lefDumbMode = 1; lefiProp_clear( &(lefrProp) ) ;
 }
 T_STRING prop_define ';'
 {
 if (lefrPropCbk) lefiProp_setPropType( &(lefrProp),
 "macro",
$3) ;
 CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
 }
 | K_VIA {lefDumbMode = 1; lefiProp_clear( &(lefrProp) ) ;
 }
 T_STRING prop_define ';'
 {
 if (lefrPropCbk) lefiProp_setPropType( &(lefrProp),
 "via",
$3) ;
 CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
 }
 | K_VIARULE {lefDumbMode = 1; lefiProp_clear( &(lefrProp) ) ;
 }
 T_STRING prop_define ';'
 {
 if (lefrPropCbk) lefiProp_setPropType( &(lefrProp),
 "viarule",
$3) ;
 CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
 }
 | K_LAYER {lefDumbMode = 1; lefiProp_clear( &(lefrProp) ) ;
 }
 T_STRING prop_define ';'
 {
 if (lefrPropCbk) lefiProp_setPropType( &(lefrProp),
 "layer",
$3) ;
 CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
 }
 | K_NONDEFAULTRULE {lefDumbMode = 1; lefiProp_clear( &(lefrProp) ) ;
 }
 T_STRING prop_define ';'
 {
 if (lefrPropCbk) lefiProp_setPropType( &(lefrProp),
 "nondefaultrule",
$3) ;
 CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
 }
;
 
 prop_define:
 K_INTEGER opt_def_range opt_def_value
 { if (lefrPropCbk) lefiProp_setPropInteger( &(lefrProp) ) ;
 }
 | K_REAL opt_def_range opt_def_value
 { if (lefrPropCbk) lefiProp_setPropReal( &(lefrProp) ) ;
 }
 | K_STRING
 { if (lefrPropCbk) lefiProp_setPropString( &(lefrProp) ) ;
 }
 | K_STRING QSTRING
 { if (lefrPropCbk) lefiProp_setPropQString( &(lefrProp),
 $2) ;
 }
 | K_NAMEMAPSTRING T_STRING
 { if (lefrPropCbk) lefiProp_setPropNameMapString( &(lefrProp),
 $2) ;
 }
;
 
 opt_range:
 /* nothing */

 { $$.x = -1; $$.y = -1; }
 | K_RANGE NUMBER NUMBER K_USELENGTHTHRESHOLD
 { $$.x = $2; $$.y = $3; useLenThr = 1; }
 | K_RANGE NUMBER NUMBER
 { $$.x = $2; $$.y = $3; }
 | K_LENGTHTHRESHOLD NUMBER
 { $$.x = $2; $$.y = -1; lenThr = 1 }
;
 
 opt_def_range:
 /* nothing */

 { }
 | K_RANGE NUMBER NUMBER
 { lefiProp_setRange( &(lefrProp),
 $2,
$3) ;
 }
;
 
 opt_def_value:
 /* empty */

 { }
 | NUMBER
 { if (lefrPropCbk) lefiProp_setNumber( &(lefrProp),
 $1) ;
 }
;
 
 opt_layer_name:
 /* empty */

 { $$ = 0; }
 | K_LAYER T_STRING
 { $$ = $2; }
;
 
 universalnoisemargin: K_UNIVERSALNOISEMARGIN NUMBER NUMBER ';'
 {
 if (lefrNoiseMarginCbk) {
 lefrNoiseMargin.low = $2;
 lefrNoiseMargin.high = $3;
 }
 CALLBACK(lefrNoiseMarginCbk, lefrNoiseMarginCbkType, &lefrNoiseMargin);
 }
;
 
 edgeratethreshold1: K_EDGERATETHRESHOLD1 NUMBER ';'
 {
 if (lefrEdgeRateThreshold1Cbk) {
 	CALLBACK(lefrEdgeRateThreshold1Cbk,
 	lefrEdgeRateThreshold1CbkType, $2);
 }
 }
;
 
 edgeratethreshold2: K_EDGERATETHRESHOLD2 NUMBER ';'
 {
 if (lefrEdgeRateThreshold2Cbk) {
 	CALLBACK(lefrEdgeRateThreshold2Cbk,
 	lefrEdgeRateThreshold2CbkType, $2);
 }
 }
;
 
 edgeratescalefactor: K_EDGERATESCALEFACTOR NUMBER ';'
 {
 if (lefrEdgeRateScaleFactorCbk) {
 	CALLBACK(lefrEdgeRateScaleFactorCbk,
 	lefrEdgeRateScaleFactorCbkType, $2);
 }
 }
;
 
 noisetable: K_NOISETABLE NUMBER
 { if (lefrNoiseTableCbk) lefiNoiseTable_setup( &(lefrNoiseTable),
 $2) ;
 }
 ';' noise_table_list K_END K_NOISETABLE dtrm
 { CALLBACK(lefrNoiseTableCbk, lefrNoiseTableCbkType, &lefrNoiseTable); }
;
 
 noise_table_list :
 noise_table_entry
 | noise_table_list noise_table_entry
 ;
 
 noise_table_entry:
 K_EDGERATE NUMBER ';'
 { if (lefrNoiseTableCbk)
 {
 lefiNoiseTable_newEdge( &(lefrNoiseTable) ) ;
 lefiNoiseTable_addEdge( &(lefrNoiseTable),
 $2) ;
 }
 }
 | output_resistance_entry
 { }
;
 
 output_resistance_entry: K_OUTPUTRESISTANCE
 { if (lefrNoiseTableCbk) lefiNoiseTable_addResistance( &(lefrNoiseTable) ) ;
 }
 num_list ';' victim_list
 ;
 
 num_list:
 NUMBER
 { if (lefrNoiseTableCbk)
 lefiNoiseTable_addResistanceNumber( &(lefrNoiseTable),
 $1) ;
 }
 | num_list NUMBER
 { if (lefrNoiseTableCbk)
 lefiNoiseTable_addResistanceNumber( &(lefrNoiseTable),
 $2) ;
 }
;
 
 victim_list:
 victim
 | victim_list victim
 ;
 
 victim: K_VICTIMLENGTH NUMBER ';'
 	{ if (lefrNoiseTableCbk)
 lefiNoiseTable_addVictimLength( &(	lefrNoiseTable),
 $2) ;
 }
 K_VICTIMNOISE vnoiselist ';'
 	{ }
;
 
 vnoiselist:
 NUMBER
 { if (lefrNoiseTableCbk)
 lefiNoiseTable_addVictimNoise( &(lefrNoiseTable),
 $1) ;
 }
 | vnoiselist NUMBER
 { if (lefrNoiseTableCbk)
 lefiNoiseTable_addVictimNoise( &(lefrNoiseTable),
 $2) ;
 }
;
 
 correctiontable: K_CORRECTIONTABLE NUMBER ';'
 { if (lefrCorrectionTableCbk)
 lefiCorrectionTable_setup( &(lefrCorrectionTable),
 $2) ;
 }
 correction_table_list K_END K_CORRECTIONTABLE dtrm
 { CALLBACK(lefrCorrectionTableCbk,
 lefrCorrectionTableCbkType, &lefrCorrectionTable); }
;
 
 correction_table_list:
 correction_table_item
 | correction_table_list correction_table_item
 ;
 
 correction_table_item:
 K_EDGERATE NUMBER ';'
 { if (lefrCorrectionTableCbk)
 {
 lefiCorrectionTable_newEdge( &(lefrCorrectionTable) ) ;
 lefiCorrectionTable_addEdge( &(lefrCorrectionTable),
 $2) ;
 }
 }
 | output_list
 { }
;
 
 output_list: K_OUTPUTRESISTANCE
 { if (lefrCorrectionTableCbk)
 lefiCorrectionTable_addResistance( &(lefrCorrectionTable) ) ;
 }
 numo_list ';' corr_victim_list
 { }
;
 
 numo_list:
 NUMBER
 { if (lefrCorrectionTableCbk)
 lefiCorrectionTable_addResistanceNumber( &(lefrCorrectionTable),
 $1) ;
 }
 | numo_list NUMBER
 { if (lefrCorrectionTableCbk)
 lefiCorrectionTable_addResistanceNumber( &(lefrCorrectionTable),
 $2) ;
 }
;
 
 corr_victim_list:
 corr_victim
 | corr_victim_list corr_victim
 ;
 
 corr_victim:
 K_VICTIMLENGTH NUMBER ';'
 { if (lefrCorrectionTableCbk)
 lefiCorrectionTable_addVictimLength( &(lefrCorrectionTable),
 $2) ;
 }
 K_CORRECTIONFACTOR corr_list ';'
 { }
;
 
 corr_list:
 NUMBER
 { if (lefrCorrectionTableCbk)
 lefiCorrectionTable_addVictimCorrection( &(	lefrCorrectionTable),
 $1) ;
 }
 | corr_list NUMBER
 { if (lefrCorrectionTableCbk)
 lefiCorrectionTable_addVictimCorrection( &(	lefrCorrectionTable),
 $2) ;
 }
;
 
 input_antenna: K_INPUTPINANTENNASIZE NUMBER ';'
 { CALLBACK(lefrInputAntennaCbk, lefrInputAntennaCbkType, $2); }
;
 
 output_antenna: K_OUTPUTPINANTENNASIZE NUMBER ';'
 { CALLBACK(lefrOutputAntennaCbk, lefrOutputAntennaCbkType, $2); }
;
 
 inout_antenna: K_INOUTPINANTENNASIZE NUMBER ';'
 { CALLBACK(lefrInoutAntennaCbk, lefrInoutAntennaCbkType, $2); }
;
 
 extension: K_BEGINEXT
 { }
;
 
 %%
 
