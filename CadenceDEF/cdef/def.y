/* 
 *     This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
  *  Distribution,  Product Version 5.3, and is subject to the Cadence
  *  LEF/DEF Open Source License Agreement.   Your  continued  use  of
  *  this file constitutes your acceptance of the terms of the LEF/DEF
  *  Open Source License and an agreement to abide by its  terms.   If
  *  you  don't  agree  with  this, you must remove this and any other
  *  files which are part of the distribution and  destroy any  copies
  *  made.
  *
  *     For updates, support, or to become part of the LEF/DEF Community,
  *  check www.openeda.org for details.
  */

 
 %{
 #include <stdlib.h>
 #include <string.h>
 #include <malloc.h>
 #include "defrReader.h"
 #include "lex.h"
 #include "defiUser.h"
 #include "defrCallBacks.h"
 
 
 extern int dumb_mode; /* Read next N tokens in dumb mode
 */
extern int no_num; /* No numbers for the next N tokens
 */
extern int new_is_keyword;
 extern int nondef_is_keyword;
 extern char* History_text;
 
 double save_x;
 double save_y;
 
 int names_case_sensitive = FALSE;
 int defRetVal;
 
 int shield = FALSE; /* To identify if the path is shield for 5.3
 */

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
 %}
 
 %union {
 double dval ;
 int integer ;
 char * string ;
 int keyword ; /* really just a nop */

 struct defpoint pt;
 defTOKEN *tk;
 }
 
 %token <string> QSTRING
 %token <string> T_STRING SITE_PATTERN
 %token <dval> NUMBER
 %token <keyword> K_HISTORY K_NAMESCASESENSITIVE
 %token <keyword> K_DESIGN K_VIAS K_TECH K_UNITS K_ARRAY K_FLOORPLAN
 %token <keyword> K_SITE K_CANPLACE K_CANNOTOCCUPY K_DIE_AREA
 %token <keyword> K_PINS
 %token <keyword> K_DEFAULTCAP K_MINPINS K_WIRECAP
 %token <keyword> K_TRACKS K_GCELLGRID
 %token <keyword> K_DO K_BY K_STEP K_LAYER K_ROW K_RECT
 %token <keyword> K_COMPS K_COMP_GEN K_SOURCE K_WEIGHT K_EEQMASTER
 %token <keyword> K_FIXED K_COVER K_UNPLACED K_PLACED K_FOREIGN K_REGION
 %token <keyword> K_REGIONS
 %token <keyword> K_NETS K_START_NET K_MUSTJOIN K_ORIGINAL K_USE K_STYLE
 %token <keyword> K_PATTERN K_PATTERNNAME K_ESTCAP K_ROUTED K_NEW
 %token <keyword> K_SNETS K_SHAPE K_WIDTH K_VOLTAGE K_SPACING K_NONDEFAULTRULE
 %token <keyword> K_N K_S K_E K_W K_FN K_FE K_FS K_FW
 %token <keyword> K_GROUPS K_GROUP K_SOFT K_MAXX K_MAXY K_MAXHALFPERIMETER
 %token <keyword> K_CONSTRAINTS K_NET K_PATH K_SUM K_DIFF
 %token <keyword> K_SCANCHAINS K_START K_FLOATING K_ORDERED K_STOP K_IN K_OUT
 %token <keyword> K_RISEMIN K_RISEMAX K_FALLMIN K_FALLMAX K_WIREDLOGIC
 %token <keyword> K_MAXDIST
 %token <keyword> K_ASSERTIONS
 %token <keyword> K_DISTANCE K_MICRONS
 %token <keyword> K_END
 %token <keyword> K_IOTIMINGS K_RISE K_FALL K_VARIABLE K_SLEWRATE K_CAPACITANCE
 %token <keyword> K_DRIVECELL K_FROMPIN K_TOPIN K_PARALLEL
 %token <keyword> K_TIMINGDISABLES K_THRUPIN K_MACRO
 %token <keyword> K_PARTITIONS K_TURNOFF
 %token <keyword> K_FROMCLOCKPIN K_FROMCOMPPIN K_FROMIOPIN
 %token <keyword> K_TOCLOCKPIN K_TOCOMPPIN K_TOIOPIN
 %token <keyword> K_SETUPRISE K_SETUPFALL K_HOLDRISE K_HOLDFALL
 %token <keyword> K_VPIN K_SUBNET K_XTALK K_PIN K_SYNTHESIZED
 %token K_DEFINE K_DEFINES K_DEFINEB K_IF K_THEN K_ELSE K_FALSE K_TRUE
 %token K_EQ K_NE K_LE K_LT K_GE K_GT K_OR K_AND K_NOT
 %token <keyword> K_SPECIAL K_DIRECTION K_RANGE
 %token <keyword> K_FPC K_HORIZONTAL K_VERTICAL K_ALIGN K_MIN K_MAX K_EQUAL
 %token <keyword> K_BOTTOMLEFT K_TOPRIGHT K_ROWS K_TAPER K_TAPERRULE
 %token <keyword> K_VERSION K_DIVIDERCHAR K_BUSBITCHARS
 %token <keyword> K_PROPERTYDEFINITIONS K_STRING K_REAL K_INTEGER K_PROPERTY
 %token <keyword> K_BEGINEXT K_ENDEXT K_NAMEMAPSTRING
 %token <keyword> K_COMPONENT
 %token <keyword> K_PINPROPERTIES
 %token <keyword> K_COMMONSCANPINS K_SNET K_COMPONENTPIN K_REENTRANTPATHS
 %token <keyword> K_SHIELD K_SHIELDNET K_NOSHIELD
 %type <pt> pt opt_paren
 %type <integer> comp_net_list subnet_opt_syn
 %type <integer> orient
 %type <integer> placement_status
 %type <string> net_type track_start
 %type <tk> path paths new_path
 %type <string> risefall opt_pin opt_pattern
 %type <string> property_val vpin_status opt_plus
 %type <string> h_or_v turnoff_setup turnoff_hold
 %type <integer> conn_opt
 
 %%
 
 def_file: version_stmt case_sens_stmt rules
 | version_stmt case_sens_stmt rules end_design
 ;
 
 version_stmt: /* empty */

 | K_VERSION NUMBER ';'
 { CALLBACK(defrVersionCbk, defrVersionCbkType, $2); }
;
 
 case_sens_stmt: /* empty */

 | K_NAMESCASESENSITIVE { dumb_mode = 1; } T_STRING ';'
 {
 if (strcmp($3, "OFF") == 0)
 names_case_sensitive = 0;
 else if (strcmp($3, "off") == 0)
 names_case_sensitive = 0;
 CALLBACK(defrCaseSensitiveCbk, defrCaseSensitiveCbkType,
 names_case_sensitive);
 }
;
 
 rules: /* empty */

 | rules rule
 | error
 ;
 
 rule: design_section | via_section | extension_section| comps_section
 | nets_section | snets_section | groups_section | scanchains_section
 | constraint_section | assertions_section | iotiming_section
 | regions_section | floorplan_contraints_section | timingdisables_section
 | partitions_section | pin_props_section
 ;
 
 design_section: design_name | tech_name | history | units |
 divider_char | bus_bit_chars |
 site | canplace | cannotoccupy | die_area |
 pin_cap_rule | pin_rule | tracks_rule | gcellgrid | array_name |
 floorplan_name | row_rule | prop_def_section
 ;
 
 design_name: K_DESIGN {dumb_mode = 1; no_num = 1; } T_STRING ';'
 	 { CALLBACK(defrDesignCbk, defrDesignStartCbkType, $3); }
;
 
 end_design: K_END K_DESIGN
 	 { CALLBACK(defrDesignEndCbk, defrDesignEndCbkType, 0); }
;
 
 tech_name: K_TECH { dumb_mode = 1; no_num = 1; } T_STRING ';'
 { CALLBACK(defrTechnologyCbk, defrTechNameCbkType, $3); }
;
 
 array_name: K_ARRAY {dumb_mode = 1; no_num = 1;} T_STRING ';'
 	 { CALLBACK(defrArrayNameCbk, defrArrayNameCbkType, $3); }
;
 
 floorplan_name: K_FLOORPLAN { dumb_mode = 1; no_num = 1; } T_STRING ';'
 	 { CALLBACK(defrFloorPlanNameCbk, defrFloorPlanNameCbkType, $3); }
;
 
 history: K_HISTORY
 { CALLBACK(defrHistoryCbk, defrHistoryCbkType, History_text); }
;
 
 prop_def_section: K_PROPERTYDEFINITIONS
 	 { CALLBACK(defrPropDefStartCbk, defrPropDefStartCbkType, 0); }
 property_defs K_END K_PROPERTYDEFINITIONS
 	 { CALLBACK(defrPropDefEndCbk, defrPropDefEndCbkType, 0); }
;
 
 property_defs: /* empty */

 | property_defs property_def
 { }
;
 
 property_def: K_DESIGN {dumb_mode = 1; no_num = 1; defiProp_clear( &(defrProp) ) ;
 }
 T_STRING property_type_and_val ';'
 {
 if (defrPropCbk) {
 defiProp_setPropType( &(defrProp),
 "design",
$3) ;
 		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
 }
 }
 | K_NET { dumb_mode = 1 ; no_num = 1; defiProp_clear( &(defrProp) ) ;
 }
 T_STRING property_type_and_val ';'
 {
 if (defrPropCbk) {
 defiProp_setPropType( &(defrProp),
 "net",
$3) ;
 		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
 }
 }
 | K_SNET { dumb_mode = 1 ; no_num = 1; defiProp_clear( &(defrProp) ) ;
 }
 T_STRING property_type_and_val ';'
 {
 if (defrPropCbk) {
 defiProp_setPropType( &(defrProp),
 "specialnet",
$3) ;
 		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
 }
 }
 | K_REGION { dumb_mode = 1 ; no_num = 1; defiProp_clear( &(defrProp) ) ;
 }
 T_STRING property_type_and_val ';'
 {
 if (defrPropCbk) {
 defiProp_setPropType( &(defrProp),
 "region",
$3) ;
 		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
 }
 }
 | K_GROUP { dumb_mode = 1 ; no_num = 1; defiProp_clear( &(defrProp) ) ;
 }
 T_STRING property_type_and_val ';'
 {
 if (defrPropCbk) {
 defiProp_setPropType( &(defrProp),
 "group",
$3) ;
 		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
 }
 }
 | K_COMPONENT { dumb_mode = 1 ; no_num = 1; defiProp_clear( &(defrProp) ) ;
 }
 T_STRING property_type_and_val ';'
 {
 if (defrPropCbk) {
 defiProp_setPropType( &(defrProp),
 "component",
$3) ;
 		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
 }
 }
 | K_ROW { dumb_mode = 1 ; no_num = 1; defiProp_clear( &(defrProp) ) ;
 }
 T_STRING property_type_and_val ';'
 {
 if (defrPropCbk) {
 defiProp_setPropType( &(defrProp),
 "row",
$3) ;
 		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
 }
 }
 | K_PIN { dumb_mode = 1 ; no_num = 1; defiProp_clear( &(defrProp) ) ;
 }
 T_STRING property_type_and_val ';'
 {
 if (defrPropCbk) {
 defiProp_setPropType( &(defrProp),
 "pin",
$3) ;
 		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
 }
 }
 | K_COMPONENTPIN
 { dumb_mode = 1 ; no_num = 1; defiProp_clear( &(defrProp) ) ;
 }
 T_STRING property_type_and_val ';'
 {
 if (defrPropCbk) {
 defiProp_setPropType( &(defrProp),
 "componentpin",
$3) ;
 		CALLBACK(defrPropCbk, defrPropCbkType, &defrProp);
 }
 }
;
 
 property_type_and_val: K_INTEGER opt_range opt_num_val
 { if (defrPropCbk) defiProp_setPropInteger( &(defrProp) ) ;
 }
 | K_REAL opt_range opt_num_val
 { if (defrPropCbk) defiProp_setPropReal( &(defrProp) ) ;
 }
 | K_STRING
 { if (defrPropCbk) defiProp_setPropString( &(defrProp) ) ;
 }
 | K_STRING QSTRING
 { if (defrPropCbk) defiProp_setPropQString( &(defrProp),
 $2) ;
 }
 | K_NAMEMAPSTRING T_STRING
 { if (defrPropCbk) defiProp_setPropNameMapString( &(defrProp),
 $2) ;
 }
;
 
 opt_num_val: /* empty */

 | NUMBER
 { if (defrPropCbk) defiProp_setNumber( &(defrProp),
 $1) ;
 }
;
 
 units: K_UNITS K_DISTANCE K_MICRONS NUMBER ';'
 	 { CALLBACK(defrUnitsCbk, defrUnitsCbkType, $4); }
;
 
 divider_char: K_DIVIDERCHAR QSTRING ';'
 	 { CALLBACK(defrDividerCbk, defrDividerCbkType, $2); }
;
 
 bus_bit_chars: K_BUSBITCHARS QSTRING ';'
 	 { CALLBACK(defrBusBitCbk, defrBusBitCbkType, $2); }
;
 
 site: K_SITE { dumb_mode = 1; no_num = 1; defiSite_clear( &(defrSite) ) ;
 }
 T_STRING NUMBER NUMBER orient
 K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER ';'
 	 {
 if (defrSiteCbk) {
 defiSite_setName( &(defrSite),
 $3) ;
 defiSite_setLocation( &(defrSite),
 $4,
$5) ;
 defiSite_setOrient( &(defrSite),
 $6) ;
 defiSite_setDo( &(defrSite),
 $8,
$10,
$12,
$13) ;
 	 CALLBACK(defrSiteCbk, defrSiteCbkType, &(defrSite));
 }
 	 }
;
 
 canplace: K_CANPLACE {dumb_mode = 1;no_num = 1; } T_STRING NUMBER NUMBER
 orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER ';'
 {
 if (defrCanplaceCbk) {
 defiSite_setName( &(defrCanplace),
 $3) ;
 defiSite_setLocation( &(defrCanplace),
 $4,
$5) ;
 defiSite_setOrient( &(defrCanplace),
 $6) ;
 defiSite_setDo( &(defrCanplace),
 $8,
$10,
$12,
$13) ;
 		CALLBACK(defrCanplaceCbk, defrCanplaceCbkType,
 		&(defrCanplace));
 }
 }
;
 cannotoccupy: K_CANNOTOCCUPY {dumb_mode = 1;no_num = 1; } T_STRING NUMBER NUMBER
 orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER ';'
 {
 if (defrCannotOccupyCbk) {
 defiSite_setName( &(defrCannotOccupy),
 $3) ;
 defiSite_setLocation( &(defrCannotOccupy),
 $4,
$5) ;
 defiSite_setOrient( &(defrCannotOccupy),
 $6) ;
 defiSite_setDo( &(defrCannotOccupy),
 $8,
$10,
$12,
$13) ;
 		CALLBACK(defrCannotOccupyCbk, defrCannotOccupyCbkType,
 &(defrCannotOccupy));
 }
 }
;
 
 orient: K_N {$$ = 0;}
 | K_E {$$ = 3;}
 | K_S {$$ = 2;}
 | K_W {$$ = 1;}
 | K_FN {$$ = 4;}
 | K_FE {$$ = 5;}
 | K_FS {$$ = 6;}
 | K_FW {$$ = 7;}
;
 
 die_area: K_DIE_AREA pt pt ';'
 	 {
 	 if (defrDieAreaCbk) {
 	 defiBox_setLowerLeft( &(defrDieArea),
 $2.x,
$2.y) ;
 	 defiBox_setUpperRight( &(defrDieArea),
 $3.x,
$3.y) ;
 	 CALLBACK(defrDieAreaCbk, defrDieAreaCbkType, &(defrDieArea));
 	 }
 	 }
;
 
 pin_cap_rule: start_def_cap pin_caps end_def_cap
 { }
;
 
 start_def_cap: K_DEFAULTCAP NUMBER
 	{ CALLBACK(defrDefaultCapCbk, defrDefaultCapCbkType, ROUND($2)); }
;
 
 pin_caps: /* empty */

 | pin_caps pin_cap
 ;
 
 pin_cap: K_MINPINS NUMBER K_WIRECAP NUMBER ';'
 	 {
 	 if (defrPinCapCbk) {
 	 defiPinCap_setPin( &(defrPinCap),
 ROUND($2)) ;
 	 defiPinCap_setCap( &(defrPinCap),
 $4) ;
 	 CALLBACK(defrPinCapCbk, defrPinCapCbkType, &(defrPinCap));
 	 }
 	 }
;
 
 end_def_cap: K_END K_DEFAULTCAP
 { }
;
 
 pin_rule: start_pins pins end_pins
 { }
;
 
 start_pins: K_PINS NUMBER ';'
 	 { CALLBACK(defrStartPinsCbk, defrStartPinsCbkType, ROUND($2)); }
;
 
 pins: /* empty */
 | pins pin
 ;
 
 pin: '-' {dumb_mode = 1;no_num = 1; } T_STRING '+' K_NET
 	{dumb_mode = 1; no_num = 1; } T_STRING
 {
 if (defrPinCbk || defrPinExtCbk) {
 defiPin_Setup( &(defrPin),
 $3,
$7) ;
 }
 }
 pin_options ';'
 { CALLBACK(defrPinCbk, defrPinCbkType, &defrPin); }
;
 
 pin_options: /* empty */
 | pin_options pin_option
;
 
 pin_option: '+' K_SPECIAL
 {
 if (defrPinCbk)
 defiPin_setSpecial( &(defrPin) ) ;
 }
 
 | extension_stmt
 { CALLBACK(defrPinExtCbk, defrPinExtCbkType, History_text); }
 
 | '+' K_DIRECTION T_STRING
 {
 if (defrPinCbk || defrPinExtCbk)
 defiPin_setDirection( &(defrPin),
 $3) ;
 }
 
 | '+' K_USE { dumb_mode = 1; no_num = 1; } T_STRING
 {
 if (defrPinCbk || defrPinExtCbk) defiPin_setUse( &(defrPin),
 $4) ;
 }
 
 | '+' K_LAYER { dumb_mode = 1; } T_STRING pt pt
 {
 if (defrPinCbk || defrPinExtCbk)
 defiPin_setLayer( &(defrPin),
 $4,
$5.x,
$5.y,
$6.x,
$6.y) ;
 }
 
 | placement_status pt orient
 {
 if (defrPinCbk || defrPinExtCbk)
 defiPin_setPlacement( &(defrPin),
 $1,
$2.x,
$2.y,
$3) ;
 }
 
;
 
 end_pins: K_END K_PINS
 	{ CALLBACK(defrPinEndCbk, defrPinEndCbkType, 0); }
;
 
 row_rule: K_ROW {dumb_mode = 2; no_num = 2; } T_STRING T_STRING NUMBER NUMBER
 orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
 {
 if (defrRowCbk) {
 defiRow_setup( &(defrRow),
 $3,
$4,
$5,
$6,
$7) ;
 defiRow_setDo( &(defrRow),
 ROUND($9),ROUND($11),$13,$14) ;
 }
 }
 row_options ';'
 {
 if (defrRowCbk) {
 	 CALLBACK(defrRowCbk, defrRowCbkType, &defrRow);
 }
 }
;
 
 row_options: /* empty */

 | row_options row_option
 ;
 
 row_option : '+' K_PROPERTY { dumb_mode = 10000000; } row_prop_list
 { dumb_mode = 0; }
;
 
 row_prop_list : /* empty */

 | row_prop_list row_prop
 ;
 
 row_prop : T_STRING property_val
 { if (defrRegionCbk) defiRow_addProperty( &(defrRow),
 $1,
$2) ;
 }
;
 
 tracks_rule: track_start NUMBER
 {
 if (defrTrackCbk) {
 defiTrack_setup( &(defrTrack),
 $1) ;
 }
 }
 K_DO NUMBER K_STEP NUMBER track_layer_statement ';'
 {
 if (defrTrackCbk) {
 defiTrack_setDo( &(defrTrack),
 ROUND($2),ROUND($5),$7) ;
 	 CALLBACK(defrTrackCbk, defrTrackCbkType, &defrTrack);
 }
 }
;
 
 track_start: K_TRACKS { dumb_mode = 1; no_num = 1; } T_STRING
 {
 $$ = $3;
 }
;
 
 track_layer_statement: /* empty */

 | K_LAYER { dumb_mode = 1000; } track_layers
 { dumb_mode = 0; }
;
 
 track_layers: /* empty */

 | track_layer track_layers
 ;
 
 track_layer: T_STRING
 {
 if (defrTrackCbk)
 defiTrack_addLayer( &(defrTrack),
 $1) ;
 }
;
 
 gcellgrid: K_GCELLGRID { dumb_mode = 1; no_num = 1; } T_STRING NUMBER
 K_DO NUMBER K_STEP NUMBER ';'
 	{
 	 if (defrGcellGridCbk) {
 	 defiGcellGrid_setup( &(defrGcellGrid),
 $3,
ROUND($4),ROUND($6),$8) ;
 	 CALLBACK(defrGcellGridCbk, defrGcellGridCbkType, &defrGcellGrid);
 	 }
 	}
;
 
 extension_section: K_BEGINEXT
 { }
;
 
 extension_stmt: '+' K_BEGINEXT
 { }
;
 
 via_section: via via_declarations via_end
 ;
 
 via: K_VIAS NUMBER ';'
 	 { CALLBACK(defrViaStartCbk, defrViaStartCbkType, ROUND($2)); }
;
 
 via_declarations: /* empty */

 | via_declarations via_declaration
 ;
 
 via_declaration: '-' {dumb_mode = 1;no_num = 1; } T_STRING
 { if (defrViaCbk) defiVia_setup( &(defrVia),
 $3) ;
 }
 layer_stmts ';'
 { CALLBACK(defrViaCbk, defrViaCbkType, &defrVia); }
;
 
 layer_stmts: /* empty */

 | layer_stmts layer_stmt
 ;
 
 layer_stmt: '+' K_RECT {dumb_mode = 1;no_num = 1; } T_STRING pt pt
 {
 if (defrViaCbk)
 defiVia_addLayer( &(defrVia),
 $4,
$5.x,
$5.y,
$6.x,
$6.y) ;
 }
 | '+' K_PATTERNNAME {dumb_mode = 1;no_num = 1; } T_STRING
 {
 if (defrViaCbk)
 defiVia_addPattern( &(defrVia),
 $4) ;
 }
 | extension_stmt
 { CALLBACK(defrViaExtCbk, defrViaExtCbkType, History_text); }
;
 
 pt : '(' NUMBER NUMBER ')'
 {
 save_x = $2;
 save_y = $3;
 $$.x = ROUND($2);
 $$.y = ROUND($3);
 }
 | '(' '*' NUMBER ')'
 {
 save_y = $3;
 $$.x = ROUND(save_x);
 $$.y = ROUND($3);
 }
 | '(' NUMBER '*' ')'
 {
 save_x = $2;
 $$.x = ROUND($2);
 $$.y = ROUND(save_y);
 }
 | '(' '*' '*' ')'
 {
 $$.x = ROUND(save_x);
 $$.y = ROUND(save_y);
 }
;
 
 via_end: K_END K_VIAS
 	{ CALLBACK(defrViaEndCbk, defrViaEndCbkType, 0); }
;
 
 regions_section: regions_start regions_stmts K_END K_REGIONS
 	{ CALLBACK(defrRegionEndCbk, defrRegionEndCbkType, 0); }
;
 
 regions_start: K_REGIONS NUMBER ';'
 	{ CALLBACK(defrRegionStartCbk, defrRegionStartCbkType, ROUND($2)); }
;
 
 regions_stmts: /* empty */

 | regions_stmts regions_stmt
 {}
;
 
 regions_stmt: '-' { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrRegionCbk) defiRegion_setup( &(defrRegion),
 $3) ;
 }
 rect_list region_options ';'
 { CALLBACK(defrRegionCbk, defrRegionCbkType, &defrRegion); }
;
 
 rect_list :
 pt pt
 { if (defrRegionCbk)
 	 defiRegion_addRect( &(defrRegion),
 $1.x,
$1.y,
$2.x,
$2.y) ;
 }
 | rect_list pt pt
 { if (defrRegionCbk)
 	 defiRegion_addRect( &(defrRegion),
 $2.x,
$2.y,
$3.x,
$3.y) ;
 }
 ;
 
 region_options: /* empty */
 | region_options region_option
 ;
 
 region_option : '+' K_PROPERTY { dumb_mode = 10000000; } region_prop_list
 { dumb_mode = 0; }
;
 
 region_prop_list : /* empty */
 | region_prop_list region_prop
 ;
 
 region_prop : T_STRING property_val
 { if (defrRegionCbk) defiRegion_addProperty( &(defrRegion),
 $1,
$2) ;
 }
;
 
 comps_section: start_comps comps_rule end_comps
 ;
 
 start_comps: K_COMPS NUMBER ';'
 { CALLBACK(defrComponentStartCbk, defrComponentStartCbkType, ROUND($2)); }
;
 
 comps_rule: /* empty */
 | comps_rule comp
 ;
 
 comp: comp_start comp_options ';'
 	{ CALLBACK(defrComponentCbk, defrComponentCbkType, &defrComponent); }
;
 
 comp_start: comp_id_and_name comp_net_list
 {
 dumb_mode = 0;
 	 no_num = 0;
 /* The net connections were added to the array in
 * reverse order so fix the order now */

 if (defrComponentCbk)
 defiComponent_reverseNetOrder( &(defrComponent) ) ;
 }
;
 
 comp_id_and_name: '-' {dumb_mode = 1000000000; no_num = 10000000; }
 T_STRING T_STRING
 {
 if (defrComponentCbk)
 defiComponent_IdAndName( &(defrComponent),
 $3,
$4) ;
 }
;
 
 comp_net_list: /* empty */
 { }
 | comp_net_list '*'
 {
 if (defrComponentCbk)
 defiComponent_addNet( &(defrComponent),
 "*") ;
 }
 | comp_net_list T_STRING
 {
 if (defrComponentCbk)
 defiComponent_addNet( &(defrComponent),
 $2) ;
 }
;
 
 comp_options: /* empty */
 | comp_options comp_option
 ;
 
 comp_option: comp_generate | comp_source | comp_type | weight |
 comp_foreign | comp_region | comp_eeq |
 comp_property | comp_extension_stmt
 ;
 
 comp_extension_stmt: extension_stmt
 { CALLBACK(defrComponentExtCbk, defrComponentExtCbkType, History_text); }
;
 
 comp_eeq: '+' K_EEQMASTER {dumb_mode=1; no_num = 1; } T_STRING
 {
 if (defrComponentCbk)
 defiComponent_setEEQ( &(defrComponent),
 $4) ;
 }
;
 
 comp_generate: '+' K_COMP_GEN { dumb_mode = 2; no_num = 2; } T_STRING
 opt_pattern
 {
 if (defrComponentCbk)
 defiComponent_setGenerate( &(defrComponent),
 $4,
$5) ;
 }
;

 opt_pattern :
 /* empty */
 { $$ = (char*)""; }
 | T_STRING
 { $$ = $1; }
;
 
 comp_source: '+' K_SOURCE { dumb_mode = 1; no_num = 1; } T_STRING
 {
 if (defrComponentCbk)
 defiComponent_setSource( &(defrComponent),
 $4) ;
 }
;
 
 comp_region:
 	comp_region_start comp_pnt_list
 	 { }
 | comp_region_start T_STRING
 	 {
 	 if (defrComponentCbk)
 	 defiComponent_setRegionName( &(defrComponent),
 $2) ;
 	 }
;
 
 comp_pnt_list: pt pt
 	{
 	 if (defrComponentCbk)
 	 defiComponent_setRegionBounds( &(defrComponent),
 $1.x,
$1.y,
$2.x,
$2.y) ;
 	}
 | comp_pnt_list pt pt
 	{
 	 if (defrComponentCbk)
 	 defiComponent_setRegionBounds( &(defrComponent),
 $2.x,
$2.y,
$3.x,
$3.y) ;
 	}
;
 
 comp_property: '+' K_PROPERTY { dumb_mode = 10000000; }
 comp_prop_list
 { dumb_mode = 0; }
;
 
 comp_prop_list: /* empty */

 | comp_prop_list comp_prop
 ;
 
 comp_prop: T_STRING property_val
 {
 if (defrComponentCbk)
 defiComponent_addProperty( &(defrComponent),
 $1,
$2) ;
 }
;
 
 comp_region_start: '+' K_REGION
 { dumb_mode = 1; no_num = 1; }
;
 
 comp_foreign: '+' K_FOREIGN { dumb_mode = 1; no_num = 1; } T_STRING
 opt_paren { dumb_mode = 1; no_num = 1; } T_STRING
 {
 if (defrComponentCbk) {
 defiComponent_setForeignName( &(defrComponent),
 $4) ;
 defiComponent_setForeignLocation( &(defrComponent),
 $5.x,
$5.y,
$7) ;
 }
 }
;
 
 opt_paren:
 pt
 	 { $$ = $1; }
 | NUMBER NUMBER
 	 { $$.x = $1; $$.y = $2; }
;
 
 comp_type: placement_status pt orient
 {
 if (defrComponentCbk) {
 defiComponent_setPlacementStatus( &(defrComponent),
 $1) ;
 defiComponent_setPlacementLocation( &(defrComponent),
 $2.x,
$2.y,
$3) ;
 }
 }
;
 
 placement_status: '+' K_FIXED
 { $$ = DEFI_COMPONENT_FIXED; }
 | '+' K_COVER
 { $$ = DEFI_COMPONENT_COVER; }
 | '+' K_PLACED
 { $$ = DEFI_COMPONENT_PLACED; }
 | '+' K_UNPLACED
 { $$ = DEFI_COMPONENT_UNPLACED; }
;
 
 weight: '+' K_WEIGHT NUMBER
 {
 if (defrComponentCbk)
 defiComponent_setWeight( &(defrComponent),
 ROUND($3)) ;
 }
;
 
 end_comps: K_END K_COMPS
 	 { CALLBACK(defrComponentEndCbk, defrComponentEndCbkType, 0); }
;
 
 nets_section: start_nets net_rules end_nets
 ;
 
 start_nets: K_NETS NUMBER ';'
 	{ CALLBACK(defrNetStartCbk, defrNetStartCbkType, ROUND($2)); }
;
 
 net_rules: /* empty */

 | net_rules one_net
 ;
 
 one_net: net_and_connections net_options ';'
 	{ CALLBACK(defrNetCbk, defrNetCbkType, &defrNet); }
 /* 
** net_and_connections: net_start {dumb_mode = 1000000000; no_num = 10000000;}
 ** net_connections
 ** wmd -- this can be used to replace
 ** | '(' K_PIN {dumb_mode = 1; no_num = 1;} T_STRING conn_opt ')' (???)
 */
;

 net_and_connections: net_start net_connections
 {dumb_mode = 0; no_num = 0; }
 
 /* pcr 235555 & 236210 */

;
 net_start: '-' {dumb_mode = 1000000000; no_num = 10000000; nondef_is_keyword = TRUE;} T_STRING
 {
 if (defrNetCbk)
 defiNet_setName( &(defrNet),
 $3) ;
 }
;
 
 net_connections: /* empty */

 | net_connections net_connection
 ;
 
 net_connection: '(' T_STRING {dumb_mode = 1000000000; no_num = 10000000;} T_STRING conn_opt ')'
 {
 if (defrNetCbk)
 defiNet_addPin( &(defrNet),
 $2,
$4,
$5) ;
 /* 1/14/2000 -- Wanda da Rosa, pcr 289156 */

 /* reset dumb_mode & no_num to 3 , just in case */

 /* the next statement is another net_connection */

 dumb_mode = 3;
 no_num = 3;
 }
 | '(' '*' {dumb_mode = 1; no_num = 1;} T_STRING conn_opt ')'
 {
 if (defrNetCbk)
 defiNet_addPin( &(defrNet),
 "*",
$4,
$5) ;
 dumb_mode = 3;
 no_num = 3;
 }
 | '(' K_PIN {dumb_mode = 1; no_num = 1;} T_STRING conn_opt ')'
 {
 if (defrNetCbk)
 defiNet_addPin( &(defrNet),
 "PIN",
$4,
$5) ;
 dumb_mode = 3;
 no_num = 3;
 }
 | K_MUSTJOIN '(' T_STRING {dumb_mode = 1; no_num = 1;} T_STRING conn_opt ')'
 {
 if (defrNetCbk)
 defiNet_addMustPin( &(defrNet),
 $3,
$5,
$6) ;
 dumb_mode = 3;
 no_num = 3;
 }
;
 
 conn_opt: /* empty */

 { $$ = 0; }
 | extension_stmt
 {
 	 CALLBACK(defrNetConnectionExtCbk, defrNetConnectionExtCbkType,
 History_text);
 	 $$ = 0; }
 | '+' K_SYNTHESIZED
 	 { $$ = 1; }
 
 /* These are all the optional fields for a net that go after the '+' */
;

 net_options: /* empty */

 | net_options net_option
 ;
 
 net_option: '+' net_type { dumb_mode = 1; } paths
 {
 if (defrNetCbk) defiNet_setType( &(defrNet),
 $2) ;
 new_is_keyword = FALSE;
 nondef_is_keyword = FALSE;
 }
 | '+' K_SOURCE { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiNet_setSource( &(defrNet),
 $4) ;
 }
 
 | '+' K_ORIGINAL {dumb_mode = 1; no_num = 1;} T_STRING
 { if (defrNetCbk) defiNet_setOriginal( &(defrNet),
 $4) ;
 }
 
 | '+' K_PATTERN { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiNet_setPattern( &(defrNet),
 $4) ;
 }
 
 | '+' K_WEIGHT NUMBER
 { if (defrNetCbk) defiNet_setWeight( &(defrNet),
 ROUND($3)) ;
 }
 
 | '+' K_XTALK NUMBER
 { if (defrNetCbk) defiNet_setXTalk( &(defrNet),
 ROUND($3)) ;
 }
 
 | '+' K_ESTCAP NUMBER
 { if (defrNetCbk) defiNet_setCap( &(defrNet),
 $3) ;
 }
 
 | '+' K_USE { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiNet_setUse( &(defrNet),
 $4) ;
 }
 
 | '+' K_STYLE { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiNet_setStyle( &(defrNet),
 $4) ;
 }
 
 | '+' K_NONDEFAULTRULE { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiNet_setNonDefaultRule( &(defrNet),
 $4) ;
 }
 
 | vpin_stmt
 
 | '+' K_SHIELDNET { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiNet_addShieldNet( &(defrNet),
 $4) ;
 }
 
 | '+' K_NOSHIELD { dumb_mode = 1; no_num = 1; }
 {
 if (defrNetCbk) defiNet_addShield( &(defrNet),
 "") ;
 new_is_keyword = FALSE;
 shield = TRUE; /* save the path info in the shield paths */

 }
 paths
 {
 shield = FALSE;
 }
 
 | '+' K_SUBNET { dumb_mode = 1; no_num = 1;
 if (defrNetCbk) {
 defrSubnet =
 (defiSubnet*)malloc(sizeof(defiSubnet));
 defiSubnet_Init( defrSubnet ) ;
 }
 }
 T_STRING comp_names subnet_options
 {
 if (defrNetCbk) {
 defiSubnet_setName( defrSubnet,
 $4) ;
 defiNet_addSubnet( &(defrNet),
 defrSubnet) ;
 defrSubnet = 0;
 }
 }
 
 | '+' K_PROPERTY {dumb_mode = 10000000;} net_prop_list
 { dumb_mode = 0; }
 
 | extension_stmt
 { CALLBACK(defrNetExtCbk, defrNetExtCbkType, History_text); }
;
 
 net_prop_list: /* empty */

 | net_prop_list net_prop
 ;
 
 net_prop: T_STRING property_val
 {
 if (defrNetCbk)
 defiNet_addProp( &(defrNet),
 $1,
$2) ;
 }
;
 
 vpin_stmt: vpin_begin vpin_layer_opt pt pt vpin_options
 { if (defrNetCbk)
 defiNet_addVpinBounds( &(	defrNet),
 $3.x,
$3.y,
$4.x,
$4.y) ;
 }
;
 
 vpin_begin: '+' K_VPIN {dumb_mode = 1; no_num = 1;} T_STRING
 	 { if (defrNetCbk) defiNet_addVpin( &(defrNet),
 $4) ;
 }
;
 
 vpin_layer_opt: /* empty */

 | K_LAYER {dumb_mode=1;} T_STRING
 	 { if (defrNetCbk) defiNet_addVpinLayer( &(defrNet),
 $3) ;
 }
;
 
 property_val: NUMBER
 {
 	 /* We will use a temporary string to store the number.
 	 * The string space is borrowed from the ring buffer
 	 * in the lexer. */

 char* str = ringCopy(" ");
 sprintf(str, "%g", $1);
 $$ = str;
 }
 | QSTRING
 { $$ = $1;}
;
 
 vpin_options: /* empty */

 | vpin_status pt orient
 { if (defrNetCbk) defiNet_addVpinLoc( &(defrNet),
 $1,
$2.x,
$2.y,
$3) ;
 }
;
 
 vpin_status: K_PLACED
 { $$ = (char*)"PLACED"; }
 | K_FIXED
 { $$ = (char*)"FIXED"; }
 | K_COVER
 { $$ = (char*)"COVER"; }
;
 
 net_type: K_FIXED
 { $$ = (char*)"FIXED"; }
 | K_COVER
 { $$ = (char*)"COVER"; }
 | K_ROUTED
 { $$ = (char*)"ROUTED"; }
;
 
 paths:
 path
 { if (defrNeedPathData) pathIsDone(shield); }
 | paths new_path
 { }
;
 
 new_path: K_NEW { dumb_mode = 1; } path
 { if (defrNeedPathData) pathIsDone(shield); }
;
 
 path: T_STRING
 { if (defrNeedPathData) defiPath_addLayer( defrPath,
 $1) ;
 }
 opt_width opt_shape opt_taper path_pt
 { dumb_mode = 10000000; new_is_keyword = TRUE; }
 path_item_list
 { dumb_mode = 0; }
;
 
 path_item_list: /* empty */

 | path_item_list path_item
 ;
 
 path_item:
 T_STRING
 { if (defrNeedPathData) defiPath_addVia( defrPath,
 $1) ;
 }
 | path_pt
 { }
;
 
 
 path_pt :
 '(' NUMBER NUMBER ')'
 {
 	if (defrNeedPathData)
 	 defiPath_addPoint( defrPath,
 ROUND($2),ROUND($3)) ;
 	save_x = $2;
 	save_y = $3;
 }
 | '(' '*' NUMBER ')'
 {
 	if (defrNeedPathData)
 	 defiPath_addPoint( defrPath,
 ROUND(save_x),ROUND($3)) ;
 	save_y = $3;
 }
 | '(' NUMBER '*' ')'
 {
 	if (defrNeedPathData)
 	 defiPath_addPoint( defrPath,
 ROUND($2),ROUND(save_y)) ;
 	save_x = $2;
 }
 | '(' '*' '*' ')'
 {
 	if (defrNeedPathData)
 	 defiPath_addPoint( defrPath,
 ROUND(save_x),ROUND(save_y)) ;
 }
 | '(' NUMBER NUMBER NUMBER ')'
 {
 	if (defrNeedPathData)
 	 defiPath_addFlushPoint( defrPath,
 ROUND($2),ROUND($3),ROUND($4)) ;
 	save_x = $2;
 	save_y = $3;
 }
 | '(' '*' NUMBER NUMBER ')'
 {
 	if (defrNeedPathData)
 	 defiPath_addFlushPoint( defrPath,
 ROUND(save_x),ROUND($3),ROUND($4)) ;
 	save_y = $3;
 }
 | '(' NUMBER '*' NUMBER ')'
 {
 	if (defrNeedPathData)
 	 defiPath_addFlushPoint( defrPath,
 ROUND($2),ROUND(save_y),ROUND($4)) ;
 	save_x = $2;
 }
 | '(' '*' '*' NUMBER ')'
 {
 	if (defrNeedPathData)
 	 defiPath_addFlushPoint( defrPath,
 ROUND(save_x),ROUND(save_y),ROUND($4)) ;
 }
;
 
 opt_taper:
 /* empty */
 { }
 | K_TAPER
 { if (defrNeedPathData) defiPath_setTaper( defrPath ) ;
 }
 | K_TAPERRULE { dumb_mode = 1; } T_STRING
 { if (defrNeedPathData) defiPath_addTaperRule( defrPath,
 $3) ;
 }
;
 
 opt_shape:
 /* empty */
 { }
 | '+' K_SHAPE T_STRING
 { if (defrNeedPathData) defiPath_addShape( defrPath,
 $3) ;
 }
;
 
 opt_width:
 /* empty */
 { }
 | NUMBER
 { if (defrNeedPathData) defiPath_addWidth( defrPath,
 ROUND($1)) ;
 }
;
 
 end_nets: K_END K_NETS
 	 { CALLBACK(defrNetEndCbk, defrNetEndCbkType, 0); }
;
 
 snets_section : start_snets snet_rules end_snets
 ;
 
 snet_rules: /* empty */

 | snet_rules snet_rule
 ;
 
 snet_rule: net_and_connections snet_options ';'
 	{ CALLBACK(defrSNetCbk, defrSNetCbkType, &defrNet); }
;
 
 snet_options: /* empty */

 | snet_options snet_option
 ;
 
 snet_option: snet_width | snet_voltage |
 snet_spacing | snet_other_option
 ;
 
 snet_other_option: '+' net_type { dumb_mode = 1; } paths
 {
 if (defrNetCbk) defiNet_setType( &(defrNet),
 $2) ;
 new_is_keyword = FALSE;
 }
 
 | '+' K_SHIELD { dumb_mode = 1; no_num = 1; } T_STRING
 {
 if (defrNetCbk) defiNet_addShield( &(defrNet),
 $4) ;
 new_is_keyword = FALSE;
 shield = TRUE; /* save the path info in the shield paths */

 }
 paths
 {
 shield = FALSE;
 }
 
 | '+' K_SOURCE { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiNet_setSource( &(defrNet),
 $4) ;
 }
 
 | '+' K_ORIGINAL {dumb_mode = 1; no_num = 1;} T_STRING
 { if (defrNetCbk) defiNet_setOriginal( &(defrNet),
 $4) ;
 }
 
 | '+' K_PATTERN { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiNet_setPattern( &(defrNet),
 $4) ;
 }
 
 | '+' K_WEIGHT NUMBER
 { if (defrNetCbk) defiNet_setWeight( &(defrNet),
 ROUND($3)) ;
 }
 
 | '+' K_ESTCAP NUMBER
 { if (defrNetCbk) defiNet_setCap( &(defrNet),
 $3) ;
 }
 
 | '+' K_USE { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiNet_setUse( &(defrNet),
 $4) ;
 }
 
 | '+' K_STYLE { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiNet_setStyle( &(defrNet),
 $4) ;
 }
 
 | '+' K_PROPERTY {dumb_mode = 10000000;} net_prop_list
 { dumb_mode = 0; }
 
 | extension_stmt
 { CALLBACK(defrNetExtCbk, defrNetExtCbkType, History_text); }
;
 
 snet_width: '+' K_WIDTH { dumb_mode = 1; } T_STRING NUMBER
 {
 if (defrSNetCbk) defiNet_setWidth( &(defrNet),
 $4,
$5) ;
 }
;
 
 snet_voltage: '+' K_VOLTAGE NUMBER
 {
 if (defrSNetCbk) defiNet_setVoltage( &(defrNet),
 $3) ;
 }
;
 
 snet_spacing: '+' K_SPACING { dumb_mode = 1; } T_STRING NUMBER
 {
 if (defrSNetCbk) defiNet_setSpacing( &(defrNet),
 $4,
$5) ;
 }
 opt_snet_range
 {
 }
;
 
 opt_snet_range: /* nothing */

 | K_RANGE NUMBER NUMBER
 {
 if (defrSNetCbk) defiNet_setRange( &(defrNet),
 $2,
$3) ;
 }
;
 
 opt_range: /* nothing */

 | K_RANGE NUMBER NUMBER
 { defiProp_setRange( &(defrProp),
 $2,
$3) ;
 }
;
 
 start_snets: K_SNETS NUMBER ';'
 { CALLBACK(defrSNetStartCbk, defrSNetStartCbkType, ROUND($2)); }
;
 
 end_snets: K_END K_SNETS
 	 { CALLBACK(defrSNetEndCbk, defrSNetEndCbkType, 0); }
;
 
 groups_section: groups_start group_rules groups_end
 ;
 
 groups_start: K_GROUPS NUMBER ';'
 	{ CALLBACK(defrGroupsStartCbk, defrGroupsStartCbkType, ROUND($2)); }
;
 
 group_rules: /* empty */

 | group_rules group_rule
 ;
 
 group_rule: start_group group_members group_options ';'
 	{ CALLBACK(defrGroupCbk, defrGroupCbkType, &defrGroup); }
;
 
 start_group: '-' { dumb_mode = 1; no_num = 1; } T_STRING
 {
 dumb_mode = 1000000000;
 no_num = 1000000000;
 /* dumb_mode is automatically turned off at the first
 * + in the options or at the ; at the end of the group */

 if (defrGroupCbk) defiGroup_setup( &(defrGroup),
 $3) ;
 CALLBACK(defrGroupNameCbk, defrGroupNameCbkType, $3);
 }
;
 
 group_members:
 | group_members group_member
 { }
;
 
 group_member: T_STRING
 {
 /* if (defrGroupCbk) defiGroup_addMember( &(defrGroup),
 $1) ;
 */

 CALLBACK(defrGroupMemberCbk, defrGroupMemberCbkType, $1);
 }
;
 
 group_options: /* empty */

 | group_options group_option
 ;
 
 group_option: '+' K_SOFT group_soft_options
 	 { }
 | '+' K_PROPERTY { dumb_mode = 10000000; } group_prop_list
 	 { dumb_mode = 0; }
 | '+' K_REGION { dumb_mode = 1; no_num = 1; } group_region
 	 { }
 | extension_stmt
 	 { CALLBACK(defrGroupExtCbk, defrGroupExtCbkType,
 	 History_text); }
;
 
 group_region:
 pt pt
 	 { if (defrGroupCbk)
 	 defiGroup_addRegionRect( &(defrGroup),
 $1.x,
$1.y,
$2.x,
$2.y) ;
 }
 | T_STRING
 	 { if (defrGroupCbk)
 	 defiGroup_setRegionName( &(defrGroup),
 $1) ;
 	 }
;
 
 group_prop_list : /* empty */

 | group_prop_list group_prop
 ;
 
 group_prop : T_STRING property_val
 { if (defrGroupCbk) defiGroup_addProperty( &(defrGroup),
 $1,
$2) ;
 }
;
 
 group_soft_options: /* empty */

 | group_soft_options group_soft_option
 { }
;
 
 group_soft_option: K_MAXX NUMBER
 {
 if (defrGroupCbk) defiGroup_setMaxX( &(defrGroup),
 ROUND($2)) ;
 }
 | K_MAXY NUMBER
 {
 if (defrGroupCbk) defiGroup_setMaxY( &(defrGroup),
 ROUND($2)) ;
 }
 | K_MAXHALFPERIMETER NUMBER
 {
 if (defrGroupCbk) defiGroup_setPerim( &(defrGroup),
 ROUND($2)) ;
 }
;
 
 groups_end: K_END K_GROUPS
 	{ CALLBACK(defrGroupsEndCbk, defrGroupsEndCbkType, 0); }
;
 
 assertions_section: assertions_start constraint_rules assertions_end
 ;
 
 constraint_section: constraints_start constraint_rules constraints_end
 ;
 
 assertions_start: K_ASSERTIONS NUMBER ';'
 	{
 	 CALLBACK(defrAssertionsStartCbk, defrAssertionsStartCbkType,
 	 ROUND($2));
 	 if (defrAssertionCbk)
 defiAssertion_setAssertionMode( &(defrAssertion) ) ;
 	}
;
 
 constraints_start: K_CONSTRAINTS NUMBER ';'
 {
 CALLBACK(defrConstraintsStartCbk, defrConstraintsStartCbkType,
 ROUND($2));
 	 if (defrConstraintCbk)
 	 defiAssertion_setConstraintMode( &(defrAssertion) ) ;
 }
;
 
 constraint_rules: /* empty */

 | constraint_rules constraint_rule
 ;
 
 constraint_rule: operand_rule
 | wiredlogic_rule
 {
 if (defiAssertion_isConstraint( &(defrAssertion) )) 		CALLBACK(defrConstraintCbk, defrConstraintCbkType, 		&defrAssertion) ;
 if (defiAssertion_isAssertion( &(defrAssertion) )) 		CALLBACK(defrAssertionCbk, defrAssertionCbkType, 		&defrAssertion) ;
 }
 
;
 
 operand_rule: '-' operand delay_specs ';'
 {
 if (defiAssertion_isConstraint( &(defrAssertion) )) 		CALLBACK(defrConstraintCbk, defrConstraintCbkType, 		&defrAssertion) ;
 if (defiAssertion_isAssertion( &(defrAssertion) )) CALLBACK(defrAssertionCbk, defrAssertionCbkType, 		&defrAssertion) ;
 
 /* reset all the flags and everything
 */
defiAssertion_clear( &(defrAssertion) ) ;
 }
;
 
 operand: K_NET { dumb_mode = 1; no_num = 1; } T_STRING
 {
 if (defrConstraintCbk || defrAssertionCbk)
 defiAssertion_addNet( &(defrAssertion),
 $3) ;
 }
 | K_PATH {dumb_mode = 4; no_num = 4;}
 	 T_STRING T_STRING T_STRING T_STRING
 {
 if (defrConstraintCbk || defrAssertionCbk)
 defiAssertion_addPath( &(defrAssertion),
 $3,
$4,
$5,
$6) ;
 }
 | K_SUM '(' operand_list ')'
 {
 if (defrConstraintCbk || defrAssertionCbk)
 defiAssertion_setSum( &(defrAssertion) ) ;
 }
 | K_DIFF '(' operand_list ')'
 {
 if (defrConstraintCbk || defrAssertionCbk)
 defiAssertion_setDiff( &(defrAssertion) ) ;
 }
;
 
 operand_list: operand
 | operand_list ',' operand
 { }
;
 
 wiredlogic_rule: '-' K_WIREDLOGIC { dumb_mode = 1; no_num = 1; } T_STRING
 opt_plus K_MAXDIST NUMBER ';'
 {
 if (defrConstraintCbk || defrAssertionCbk)
 defiAssertion_setWiredlogic( &(defrAssertion),
 $4,
$7) ;
 }
;
 
 opt_plus:
 /* 	 empty */

 	 { $$ = (char*)""; }
 	| '+'
 	 { $$ = (char*)"+"; }
;
 
 delay_specs: /* empty */

 | delay_specs delay_spec
 ;
 
 delay_spec: '+' K_RISEMIN NUMBER
 {
 if (defrConstraintCbk || defrAssertionCbk)
 defiAssertion_setRiseMin( &(defrAssertion),
 $3) ;
 }
 | '+' K_RISEMAX NUMBER
 {
 if (defrConstraintCbk || defrAssertionCbk)
 defiAssertion_setRiseMax( &(defrAssertion),
 $3) ;
 }
 | '+' K_FALLMIN NUMBER
 {
 if (defrConstraintCbk || defrAssertionCbk)
 defiAssertion_setFallMin( &(defrAssertion),
 $3) ;
 }
 | '+' K_FALLMAX NUMBER
 {
 if (defrConstraintCbk || defrAssertionCbk)
 defiAssertion_setFallMax( &(defrAssertion),
 $3) ;
 }
;
 
 constraints_end: K_END K_CONSTRAINTS
 { CALLBACK(defrConstraintsEndCbk, defrConstraintsEndCbkType, 0); }
;
 
 assertions_end: K_END K_ASSERTIONS
 	{ CALLBACK(defrAssertionsEndCbk, defrAssertionsEndCbkType, 0); }
;
 
 scanchains_section: scanchain_start scanchain_rules scanchain_end
 ;
 
 scanchain_start: K_SCANCHAINS NUMBER ';'
 	{ CALLBACK(defrScanchainsStartCbk, defrScanchainsStartCbkType,
 ROUND($2)); }
;
 
 scanchain_rules: /* empty */

 | scanchain_rules scan_rule
 {}
;
 
 scan_rule: start_scan scan_members ';'
 { CALLBACK(defrScanchainCbk, defrScanchainCbkType, &defrScanchain); }
;
 
 start_scan: '-' {dumb_mode = 1; no_num = 1;} T_STRING
 {
 if (defrScanchainCbk)
 defiScanchain_setName( &(defrScanchain),
 $3) ;
 }
;
 
 scan_members:
 | scan_members scan_member
 ;
 
 opt_pin :
 /* empty */
 { $$ = (char*)""; }
 | T_STRING
 { $$ = $1; }
;
 
 scan_member: '+' K_START {dumb_mode = 2; no_num = 2;} T_STRING opt_pin
 	 { if (defrScanchainCbk)
 	 defiScanchain_setStart( &(defrScanchain),
 $4,
$5) ;
 }
 
 | '+' K_FLOATING { dumb_mode = 1; no_num = 1; } floating_inst_list
 	 { dumb_mode = 0; no_num = 0; }
 
 | '+' K_ORDERED
 {
 dumb_mode = 1;
 	 no_num = 1;
 if (defrScanchainCbk)
 defiScanchain_addOrderedList( &(defrScanchain) ) ;
 }
 ordered_inst_list
 	 { dumb_mode = 0; no_num = 0; }
 
 | '+' K_STOP {dumb_mode = 2; no_num = 2; } T_STRING opt_pin
 	 { if (defrScanchainCbk)
 	 defiScanchain_setStop( &(defrScanchain),
 $4,
$5) ;
 }
 
 	| '+' K_COMMONSCANPINS { dumb_mode = 10; no_num = 10; }
 	 opt_common_pins
 	 { dumb_mode = 0; no_num = 0; }
 
 | extension_stmt
 	 {
 	 CALLBACK(defrScanChainExtCbk, defrScanChainExtCbkType,
 History_text);
 	 }
;
 
 opt_common_pins: /* empty */

 { }
 | '(' T_STRING T_STRING ')'
 {
 	if (defrScanchainCbk) {
 	 if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
 	 defiScanchain_setCommonIn( &(defrScanchain),
 $3) ;
 	 else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
 	 defiScanchain_setCommonOut( &(defrScanchain),
 $3) ;
 	}
 }
 | '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')'
 {
 	if (defrScanchainCbk) {
 	 if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
 	 defiScanchain_setCommonIn( &(defrScanchain),
 $3) ;
 	 else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
 	 defiScanchain_setCommonOut( &(defrScanchain),
 $3) ;
 	 if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
 	 defiScanchain_setCommonIn( &(defrScanchain),
 $7) ;
 	 else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
 	 defiScanchain_setCommonOut( &(defrScanchain),
 $7) ;
 	}
 }
 
;
 
 floating_inst_list: /* empty */

 | floating_inst_list one_floating_inst
 ;
 
 one_floating_inst: T_STRING
 {
 	dumb_mode = 1000;
 	no_num = 1000;
 	if (defrScanchainCbk)
 	 defiScanchain_addFloatingInst( &(defrScanchain),
 $1) ;
 }
 floating_pins
 { dumb_mode = 1; no_num = 1; }
;
 
 floating_pins: /* empty */

 { }
 | '(' T_STRING T_STRING ')'
 {
 	if (defrScanchainCbk) {
 	 if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
 	 defiScanchain_addFloatingIn( &(defrScanchain),
 $3) ;
 	 else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
 	 defiScanchain_addFloatingOut( &(defrScanchain),
 $3) ;
 	}
 }
 | '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')'
 {
 	if (defrScanchainCbk) {
 	 if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
 	 defiScanchain_addFloatingIn( &(defrScanchain),
 $3) ;
 	 else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
 	 defiScanchain_addFloatingOut( &(defrScanchain),
 $3) ;
 	 if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
 	 defiScanchain_addFloatingIn( &(defrScanchain),
 $7) ;
 	 else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
 	 defiScanchain_addFloatingOut( &(defrScanchain),
 $7) ;
 	}
 }
;
 
 ordered_inst_list: /* empty */

 | ordered_inst_list one_ordered_inst
 ;
 
 one_ordered_inst: T_STRING
 { dumb_mode = 1000; no_num = 1000;
 	if (defrScanchainCbk)
 	 defiScanchain_addOrderedInst( &(defrScanchain),
 $1) ;
 }
 ordered_pins
 { dumb_mode = 1; no_num = 1; }
;
 
 ordered_pins: /* empty */

 { }
 | '(' T_STRING T_STRING ')'
 {
 	if (defrScanchainCbk) {
 	 if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
 	 defiScanchain_addOrderedIn( &(defrScanchain),
 $3) ;
 	 else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
 	 defiScanchain_addOrderedOut( &(defrScanchain),
 $3) ;
 	}
 }
 | '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')'
 {
 	if (defrScanchainCbk) {
 	 if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
 	 defiScanchain_addOrderedIn( &(defrScanchain),
 $3) ;
 	 else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
 	 defiScanchain_addOrderedOut( &(defrScanchain),
 $3) ;
 	 if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
 	 defiScanchain_addOrderedIn( &(defrScanchain),
 $7) ;
 	 else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
 	 defiScanchain_addOrderedOut( &(defrScanchain),
 $7) ;
 	}
 }
;
 
 scanchain_end: K_END K_SCANCHAINS
 	{ CALLBACK(defrScanchainsEndCbk, defrScanchainsEndCbkType, 0); }
;
 
 iotiming_section: iotiming_start iotiming_rules iotiming_end
 ;
 
 iotiming_start: K_IOTIMINGS NUMBER ';'
 	{ CALLBACK(defrIOTimingsStartCbk, defrIOTimingsStartCbkType,
 ROUND($2)); }
;
 
 iotiming_rules: /* empty */

 | iotiming_rules iotiming_rule
 {}
;
 
 iotiming_rule: start_iotiming iotiming_members ';'
 	 { CALLBACK(defrIOTimingCbk, defrIOTimingCbkType,
 &defrIOTiming); }
;
 
 start_iotiming: '-' '(' {dumb_mode = 2; no_num = 2; } T_STRING T_STRING ')'
 {
 if (defrIOTimingCbk)
 defiIOTiming_setName( &(defrIOTiming),
 $4,
$5) ;
 }
;
 
 iotiming_members:
 | iotiming_members iotiming_member
 ;
 
 iotiming_member:
 '+' risefall K_VARIABLE NUMBER NUMBER
 {
 if (defrIOTimingCbk)
 defiIOTiming_setVariable( &(defrIOTiming),
 $2,
$4,
$5) ;
 }
 | '+' risefall K_SLEWRATE NUMBER NUMBER
 {
 if (defrIOTimingCbk)
 defiIOTiming_setSlewRate( &(defrIOTiming),
 $2,
$4,
$5) ;
 }
 | '+' K_CAPACITANCE NUMBER
 {
 if (defrIOTimingCbk)
 defiIOTiming_setCapacitance( &(defrIOTiming),
 $3) ;
 }
 | '+' K_DRIVECELL {dumb_mode = 1; no_num = 1; } T_STRING
 {
 if (defrIOTimingCbk)
 defiIOTiming_setDriveCell( &(defrIOTiming),
 $4) ;
 }
 /* |'+' K_FROMPIN {dumb_mode = 1; no_num = 1; } T_STRING*/

 | K_FROMPIN {dumb_mode = 1; no_num = 1; } T_STRING
 {
 if (defrIOTimingCbk)
 defiIOTiming_setFrom( &(defrIOTiming),
 $3) ;
 }
 /* |'+' K_TOPIN {dumb_mode = 1; no_num = 1; } T_STRING*/

 | K_TOPIN {dumb_mode = 1; no_num = 1; } T_STRING
 {
 if (defrIOTimingCbk)
 defiIOTiming_setTo( &(defrIOTiming),
 $3) ;
 }
 /* |'+' K_PARALLEL NUMBER*/

 | K_PARALLEL NUMBER
 {
 if (defrIOTimingCbk)
 defiIOTiming_setParallel( &(defrIOTiming),
 $2) ;
 }
 | extension_stmt
 { CALLBACK(defrIoTimingsExtCbk, defrIoTimingsExtCbkType,
 History_text); }
;
 
 risefall: K_RISE { $$ = (char*)"RISE"; } | K_FALL { $$ = (char*)"FALL"; }
;
 
 iotiming_end: K_END K_IOTIMINGS
 	{ CALLBACK(defrIOTimingsEndCbk, defrIOTimingsEndCbkType, 0); }
;
 
 floorplan_contraints_section: fp_start fp_stmts K_END K_FPC
 { CALLBACK(defrFPCEndCbk, defrFPCEndCbkType, 0); }
;
 
 fp_start: K_FPC NUMBER ';'
 { CALLBACK(defrFPCStartCbk, defrFPCStartCbkType,
 ROUND($2)); }
;
 
 fp_stmts: /* empty */

 | fp_stmts fp_stmt
 {}
;
 
 fp_stmt: '-' { dumb_mode = 1; no_num = 1; } T_STRING h_or_v
 { if (defrFPCCbk) defiFPC_setName( &(defrFPC),
 $3,
$4) ;
 }
 constraint_type constrain_what_list ';'
 { CALLBACK(defrFPCCbk, defrFPCCbkType, &defrFPC); }
;
 
 h_or_v: K_HORIZONTAL
 { $$ = (char*)"HORIZONTAL"; }
 | K_VERTICAL
 { $$ = (char*)"VERTICAL"; }
;
 
 constraint_type: K_ALIGN
 { if (defrFPCCbk) defiFPC_setAlign( &(defrFPC) ) ;
 }
 | K_MAX NUMBER
 { if (defrFPCCbk) defiFPC_setMax( &(defrFPC),
 $2) ;
 }
 | K_MIN NUMBER
 { if (defrFPCCbk) defiFPC_setMin( &(defrFPC),
 $2) ;
 }
 | K_EQUAL NUMBER
 { if (defrFPCCbk) defiFPC_setEqual( &(defrFPC),
 $2) ;
 }
;
 
 constrain_what_list: /* empty */

 | constrain_what_list constrain_what
 ;
 
 constrain_what: '+' K_BOTTOMLEFT
 { if (defrFPCCbk) defiFPC_setDoingBottomLeft( &(defrFPC) ) ;
 }
 row_or_comp_list
 | '+' K_TOPRIGHT
 { if (defrFPCCbk) defiFPC_setDoingTopRight( &(defrFPC) ) ;
 }
 row_or_comp_list
 ;
 
 row_or_comp_list: /* empty */

 | row_or_comp_list row_or_comp
;
 
 row_or_comp: '(' K_ROWS {dumb_mode = 1; no_num = 1; } T_STRING ')'
 { if (defrFPCCbk) defiFPC_addRow( &(defrFPC),
 $4) ;
 }
 | '(' K_COMPS {dumb_mode = 1; no_num = 1; } T_STRING ')'
 { if (defrFPCCbk) defiFPC_addComps( &(defrFPC),
 $4) ;
 }
;
 
 timingdisables_section: timingdisables_start timingdisables_rules timingdisables_end
 ;
 
 timingdisables_start: K_TIMINGDISABLES NUMBER ';'
 { CALLBACK(defrTimingDisablesStartCbk, defrTimingDisablesStartCbkType,
 ROUND($2)); }
;
 
 timingdisables_rules: /* empty */

 | timingdisables_rules timingdisables_rule
 {}
;
 
 timingdisables_rule: '-' K_FROMPIN { dumb_mode = 2; no_num = 2; } T_STRING T_STRING
 K_TOPIN { dumb_mode = 2; no_num = 2; } T_STRING T_STRING ';'
 {
 if (defrTimingDisableCbk)
 defiTimingDisable_setFromTo( &(defrTimingDisable),
 $4,
$5,
$8,
$9) ;
 	 CALLBACK(defrTimingDisableCbk, defrTimingDisableCbkType,
 &defrTimingDisable);
 	 }
 | '-' K_THRUPIN {dumb_mode = 2; no_num = 2; } T_STRING T_STRING ';'
 {
 if (defrTimingDisableCbk)
 defiTimingDisable_setThru( &(defrTimingDisable),
 $4,
$5) ;
 	 CALLBACK(defrTimingDisableCbk, defrTimingDisableCbkType,
 &defrTimingDisable);
 }
 | '-' K_MACRO {dumb_mode = 1; no_num = 1;} T_STRING td_macro_option ';'
 {
 if (defrTimingDisableCbk)
 defiTimingDisable_setMacro( &(defrTimingDisable),
 $4) ;
 	 CALLBACK(defrTimingDisableCbk, defrTimingDisableCbkType,
 &defrTimingDisable);
 }
 | '-' K_REENTRANTPATHS ';'
 { if (defrTimingDisableCbk)
 defiTimingDisable_setReentrantPathsFlag( &(defrTimingDisable) ) ;
 	 }
;
 
 
 td_macro_option: K_FROMPIN {dumb_mode = 1; no_num = 1;} T_STRING K_TOPIN
 {dumb_mode=1; no_num = 1;} T_STRING
 {
 if (defrTimingDisableCbk)
 defiTimingDisable_setMacroFromTo( &(defrTimingDisable),
 $3,
$6) ;
 }
 | K_THRUPIN {dumb_mode=1; no_num = 1;} T_STRING
 {
 if (defrTimingDisableCbk)
 defiTimingDisable_setMacroThru( &(defrTimingDisable),
 $3) ;
 }
;
 
 timingdisables_end: K_END K_TIMINGDISABLES
 { CALLBACK(defrTimingDisablesEndCbk, defrTimingDisablesEndCbkType, 0); }
;
 
 
 partitions_section: partitions_start partition_rules partitions_end
 ;
 
 partitions_start: K_PARTITIONS NUMBER ';'
 	{ CALLBACK(defrPartitionsStartCbk, defrPartitionsStartCbkType,
 ROUND($2)); }
;
 
 partition_rules: /* empty */

 | partition_rules partition_rule
 {}
;
 
 partition_rule: start_partition partition_members ';'
 { CALLBACK(defrPartitionCbk, defrPartitionCbkType,
 &defrPartition); }
;
 
 start_partition: '-' { dumb_mode = 1; no_num = 1; } T_STRING turnoff
 {
 if (defrPartitionCbk)
 defiPartition_setName( &(defrPartition),
 $3) ;
 }
;
 
 turnoff: /* empty */

 | K_TURNOFF turnoff_setup turnoff_hold
 {
 if (defrPartitionCbk)
 defiPartition_addTurnOff( &(defrPartition),
 $2,
$3) ;
 }
;
 
 turnoff_setup: /* empty */

 { $$ = (char*)" "; }
 | K_SETUPRISE
 { $$ = (char*)"R"; }
 | K_SETUPFALL
 { $$ = (char*)"F"; }
;
 
 turnoff_hold: /* empty */

 { $$ = (char*)" "; }
 | K_HOLDRISE
 { $$ = (char*)"R"; }
 | K_HOLDFALL
 { $$ = (char*)"F"; }
;
 
 partition_members: /* empty */

 | partition_members partition_member
 ;
 
 partition_member: '+' K_FROMCLOCKPIN {dumb_mode=2; no_num = 2;}
 T_STRING T_STRING risefall minmaxpins
 {
 if (defrPartitionCbk)
 defiPartition_setFromClockPin( &(defrPartition),
 $4,
$5) ;
 }
 | '+' K_FROMCOMPPIN {dumb_mode=2; no_num = 2; }
 	 T_STRING T_STRING risefallminmax2_list
 {
 if (defrPartitionCbk)
 defiPartition_setFromCompPin( &(defrPartition),
 $4,
$5) ;
 }
 | '+' K_FROMIOPIN {dumb_mode=1; no_num = 1; } T_STRING
 	 risefallminmax1_list
 {
 if (defrPartitionCbk)
 defiPartition_setFromIOPin( &(defrPartition),
 $4) ;
 }
 | '+' K_TOCLOCKPIN {dumb_mode=2; no_num = 2; }
 	 T_STRING T_STRING risefall minmaxpins
 {
 if (defrPartitionCbk)
 defiPartition_setToClockPin( &(defrPartition),
 $4,
$5) ;
 }
 | '+' K_TOCOMPPIN {dumb_mode=2; no_num = 2; }
 	 T_STRING T_STRING risefallminmax2_list
 {
 if (defrPartitionCbk)
 defiPartition_setToCompPin( &(defrPartition),
 $4,
$5) ;
 }
 | '+' K_TOIOPIN {dumb_mode=1; no_num = 2; } T_STRING risefallminmax1_list
 {
 if (defrPartitionCbk)
 defiPartition_setToIOPin( &(defrPartition),
 $4) ;
 }
 | extension_stmt
 { CALLBACK(defrPartitionsExtCbk, defrPartitionsExtCbkType,
 History_text); }
;
 
 minmaxpins: min_or_max_list K_PINS
 { dumb_mode = 1000000000; no_num = 10000000; } pin_list
 { dumb_mode = 0; no_num = 0; }
;
 
 min_or_max_list: /* empty */

 | min_or_max_list min_or_max_member
 {}
;
 
 min_or_max_member: K_MIN NUMBER NUMBER
 {
 if (defrPartitionCbk)
 defiPartition_setMin( &(defrPartition),
 $2,
$3) ;
 }
 | K_MAX NUMBER NUMBER
 {
 if (defrPartitionCbk)
 defiPartition_setMax( &(defrPartition),
 $2,
$3) ;
}
;
 
 pin_list: /* empty */

 | pin_list T_STRING
 	{ if (defrPartitionCbk) defiPartition_addPin( &(defrPartition),
 $2) ;
}
;
 
 risefallminmax1_list: /* empty */

 | risefallminmax1_list risefallminmax1
;
 
 risefallminmax1: K_RISEMIN NUMBER
 { if (defrPartitionCbk) defiPartition_addRiseMin( &(defrPartition),
 $2) ;
 }
 | K_FALLMIN NUMBER
 { if (defrPartitionCbk) defiPartition_addFallMin( &(defrPartition),
 $2) ;
 }
 | K_RISEMAX NUMBER
 { if (defrPartitionCbk) defiPartition_addRiseMax( &(defrPartition),
 $2) ;
 }
 | K_FALLMAX NUMBER
 { if (defrPartitionCbk) defiPartition_addFallMax( &(defrPartition),
 $2) ;
 }
;
 
 risefallminmax2_list:
 risefallminmax2
 | risefallminmax2_list risefallminmax2
 ;
 
 risefallminmax2: K_RISEMIN NUMBER NUMBER
 	 { if (defrPartitionCbk)
 	 defiPartition_addRiseMinRange( &(defrPartition),
 $2,
$3) ;
 }
 | K_FALLMIN NUMBER NUMBER
 	 { if (defrPartitionCbk)
 	 defiPartition_addFallMinRange( &(defrPartition),
 $2,
$3) ;
 }
 | K_RISEMAX NUMBER NUMBER
 	 { if (defrPartitionCbk)
 	 defiPartition_addRiseMaxRange( &(defrPartition),
 $2,
$3) ;
 }
 | K_FALLMAX NUMBER NUMBER
 	 { if (defrPartitionCbk)
 	 defiPartition_addFallMaxRange( &(defrPartition),
 $2,
$3) ;
 }
;
 
 partitions_end: K_END K_PARTITIONS
 	{ CALLBACK(defrPartitionsEndCbk, defrPartitionsEndCbkType, 0); }
;
 
 
 comp_names: /* empty */

 | comp_names comp_name
 {}
;
 
 comp_name: '(' {dumb_mode=2; no_num = 2; } T_STRING
 T_STRING subnet_opt_syn ')'
 	 {
 /* note that the first T_STRING could be the keyword VPIN */

 	 if (defrNetCbk) {
 	 defiSubnet_addPin( defrSubnet,
 $3,
$4,
$5) ;
 	 }
 	 }
;
 
 subnet_opt_syn: /* empty */

 	 { $$ = 0; }
 | '+' K_SYNTHESIZED
 	 { $$ = 1; }
;
 
 subnet_options: /* empty */

 | subnet_options subnet_option
;
 
 subnet_option: net_type paths
 {
 if (defrNetCbk) defiSubnet_setType( defrSubnet,
 $1) ;
 new_is_keyword = FALSE;
 }
 | K_NONDEFAULTRULE { dumb_mode = 1; no_num = 1; } T_STRING
 { if (defrNetCbk) defiSubnet_setNonDefault( defrSubnet,
 $3) ;
 }
;
 
 pin_props_section: begin_pin_props pin_prop_list end_pin_props 
;
 
 begin_pin_props: K_PINPROPERTIES NUMBER opt_semi
 { CALLBACK(defrPinPropStartCbk, defrPinPropStartCbkType, ROUND($2)); }
;
 
 opt_semi:
 /* empty */

 { }
 | ';'
 { }
;
 
 end_pin_props: K_END K_PINPROPERTIES
 { CALLBACK(defrPinPropEndCbk, defrPinPropEndCbkType, 0); }
;
 
 pin_prop_list: /* empty */

 | pin_prop_list pin_prop_terminal
 ;
 
 pin_prop_terminal: '-' { dumb_mode = 2; no_num = 2; } T_STRING T_STRING
 { if (defrPinPropCbk) defiPinProp_setName( &(defrPinProp),
 $3,
$4) ;
 }
 pin_prop_options ';'
 { CALLBACK(defrPinPropCbk, defrPinPropCbkType, &defrPinProp);
 /* reset the property number
 */
defiPinProp_clear( &(defrPinProp) ) ;
 }
;
 
 pin_prop_options : /* empty */
 | pin_prop_options pin_prop 
;
 
 pin_prop: '+' K_PROPERTY { dumb_mode = 10000000; }
 pin_prop_name_value_list
 { dumb_mode = 0; }
;
 
 pin_prop_name_value_list : /* empty */

 | pin_prop_name_value_list pin_prop_name_value
 ;
 
 pin_prop_name_value : T_STRING property_val
 { if (defrPinPropCbk) defiPinProp_addProperty( &(defrPinProp),
 $1,
$2) ;
 }
;
 
 %%
 
 
