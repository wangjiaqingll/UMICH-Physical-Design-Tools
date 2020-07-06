/*
 * This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 * Distribution,  Product Version 5.3, and is subject to the Cadence LEF/DEF
 * Open Source License Agreement.   Your  continued  use  of this file
 * constitutes your acceptance of the terms of the LEF/DEF Open Source
 * License and an agreement to abide by its  terms.   If you  don't  agree
 * with  this, you must remove this and any other files which are part of the
 * distribution and  destroy any  copies made.
 * 
 * For updates, support, or to become part of the LEF/DEF Community, check
 * www.openeda.org for details.
 */

#ifndef LEFW_WRITERCALLS_H
#define LEFW_WRITERCALLS_H
#include <stdio.h>
#include "lefiKRDefs.h"
#include "lefiDefs.h"
#include "lefiUser.h"
/* Return codes for writing functions: */
#define LEFW_OK              0
#define LEFW_UNINITIALIZED   1
#define LEFW_BAD_ORDER       2
#define LEFW_BAD_DATA        3
#define LEFW_ALREADY_DEFINED 4

/*
 * orient 0 = N 1 = W 2 = S 3 = E 4 = FN 5 = FE 6 = FS 7 = FW
 */

/*
 * The LEF writer initialization.  Must be called first. Either this routine
 * or lefwInitCbk should be call only, Can't call both routines. This routine
 * must be called only once. Returns 0 if successful.
 */
EXTERN int lefwInit(FILE * f);

/*
 * The LEF writer initialization.  Must be called first. Either this routine
 * or lefwInit should be call only, Can't call both routines. This routine
 * must be called only once. Returns 0 if successful.
 */
EXTERN int lefwInitCbk(FILE * f);

/*
 * This routine will set the writer to write out an encrypted lef file. This
 * routine must be called only once and has to be called after lefwInit or
 * lefwInitCbk Need to call lefwCloseEncrypt to do some cleaning if this
 * routine has called
 */
EXTERN int lefwEncrypt();

/*
 * This routine needs to be called if lefwEncrypt has called. It should be
 * called before the fclose. It does some house cleaning.
 */
EXTERN int lefwCloseEncrypt();

/*
 * This routine will write a blank line
 */
EXTERN int lefwNewLine();

/*
 * This routine is called after lefwInit. This routine can be called only
 * once. Returns 0 if successfull.
 */
EXTERN int lefwVersion(int vers1, int vers2);

/*
 * This routine is called afer lefwInit. This routine can be called only
 * once. Returns 0 if successfull. The caseSensitive can be ON or OFF.
 */
EXTERN int lefwCaseSensitive(const char *caseSensitive);

/*
 * This routine is called afer lefwInit. This routine can be called only
 * once. Returns 0 if successfull. The noWireExtensionAtPin can be ON or OFF.
 */
EXTERN int lefwNoWireExtensionAtPin(const char *noWireExt);

/*
 * This routine is called afer lefwInit. This routine can be called only
 * once. Returns 0 if successfull. already have one
 */
EXTERN int lefwMinfeature(double minFeatureX, double minFeatureY);

/*
 * This routine is called afer lefwInit. This routine can be called only
 * once. Returns 0 if successfull.
 */
EXTERN int lefwDielectric(double dielectric);

/*
 * This routine is called afer lefwInit. This routine can be called only
 * once. Returns 0 if successfull.
 */
EXTERN int lefwBusBitChars(const char *busBitChars);

/*
 * This routine is called afer lefwInit. This routine can be called only
 * once. Returns 0 if successfull.
 */
EXTERN int lefwDividerChar(const char *dividerChar);

/*
 * This routine is called after lefwInit. This routine can be called only
 * once. Returns 0 if successfull. This section of routines is optional. The
 * routine starts the units section. All of the units must follow.
 */
EXTERN int lefwStartUnits();

/*
 * This routine is called once for each unit.  The call must be preceeded by
 * a call to lefwStartUnits and must be terminated by a call to lefwEndUnits.
 * Returns 0 if successfull.
 */
EXTERN int 
lefwUnits(double time,    /* optional(0) - TIME NANOSECONDS */
double capacitance,  /* optional(0) - CAPACITANCE PICOFARADS */
double resistance,  /* optional(0) - RESISTANCE OHMS */
double power,    /* optional(0) - POWER MILLIWATTS */
double current,  /* optional(0) - CURRENT MILLIAMPS */
double voltage,  /* optional(0) - VOLTAGE VOLTS */
double database);  /* optional(0) - DATABASE MICRONS */

/*
 * This routine is called once for each unit.  It is separated from lefwUnits
 * due to backwards compatible for pre 5.3. Returns 0 if successfull.
 */
EXTERN int lefwUnitsFrequency(double frequency);

/*
 * This routine must be called after the lefwUnits call (if any). The routine
 * can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEndUnits();

/*
 * This routine is called after lefwInit. Either this routine or
 * lefwStartLayerRouting must be called. Multiple sections of lefwStartLayer
 * can be called. This routine can be called only once per section. Returns 0
 * if successfull. The routine starts the layer section. All of the layers
 * must follow. The type can be either CUT for Cur Layer,or  MASTERSLICE or
 * OVERLAP for Masterslice or Overlay Layer.
 */
EXTERN int 
lefwStartLayer(const char *layerName,
const char *type);  /* CUT | MASTERSLICE | OVERLAP */

/*
 * This routine must be called after lefwStartLayer. This routine can be
 * called multiple times. This section of routines is optional. Returns 0 if
 * successfull. This routine is called if the layer type is CUT when
 * lefwStartLayer is called. The spacing is the minSpacing. The name2 is
 * optional, it is for 2ndCutLayerName.
 */
EXTERN int 
lefwLayer(double spacing,  /* minSpacing */
const char *name2);  /* optional(NULL) - 2ndCutLayername */

/*
 * This routine must be called after the lefwStartLayer call (if any). The
 * routine can be called only once per section. Returns 0 if successfull.
 */
EXTERN int lefwEndLayer(const char *layerName);

/*
 * This routine is called after lefwInit. Either this routine or
 * lefwStartLayer must be called. Multiple section of lefwStartLayer can be
 * called. The routine can be called only once per section. Returns 0 if
 * successfull. The routine starts the layer routing section. All of the
 * layers must follow.
 */
EXTERN int lefwStartLayerRouting(const char *layerName);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is required for LayerRouting. Returns 0 if successfull.
 */
EXTERN int 
lefwLayerRouting(const char *direction,  /* HORIZONTAL | VERTICAL */
double width);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is required for SE only Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingPitch(double pitch);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingOffset(double offset);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int 
lefwLayerRoutingSpacing(double spacing, double leftRange,
double rightRange);

/*
 * Either this routine or lefwLayerRoutingSpacingLengthThreshold can be
 * called once after lefwLayerRoutingSpacing. This routine is valid only if
 * either or both leftRange and rightRange in lefwLayerRoutingSpacing are non
 * zero Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingSpacingUseLengthThreshold();

/*
 * Either this routine or lefwLayerRoutingSpacingUseLengthThreshold can be
 * called once after lefwLayerRoutingSpacing. This routine is valid only if
 * both lefRange and right Range in lefwLayerRoutingSpacing are zero Returns
 * 0 if successfull.
 */
EXTERN int lefwLayerRoutingSpacingLengthThreshold(double lengthValue);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingWireExtension(double wireExtension);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingResistance(const char *resistance);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingCapacitance(const char *capacitance);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingHeight(double height);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingThickness(double thickness);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingShrinkage(double shrinkage);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingCapMultiplier(double capMultiplier);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingEdgeCap(double edgeCap);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingAntennaArea(double antennaArea);

/*
 * This routine must be called only once after lefwStartLayerRouting. This
 * routine is optional Returns 0 if successfull.
 */
EXTERN int lefwLayerRoutingAntennaLength(double antennaLength);

/*
 * This routine must be called after the lefwStartLayer call (if any). The
 * routine can be called only once per section. Returns 0 if successfull.
 */
EXTERN int lefwEndLayerRouting(const char *layerName);

/*
 * This routine must be called after lefwStartLayerRouting or lefwStartLayer.
 * This routine is optional and can be called multiple times. if the value
 * for the variable value is not 0, one of the following routines have to be
 * called: lefwLayerACFrequency, lefwLayerACWidth, lefwLayerACCutarea, or
 * lefwLayerACTableEntries Returns 0 if successfull.
 */
EXTERN int lefwLayerACCurrentDensity(const char *type, double value);

/*
 * This routine must be called after lefwLayerACCurrentDensity. This routine
 * is required and can be called only once after each
 * lefwLayerACCurrentDensity. Returns 0 if successfull.
 */
EXTERN int lefwLayerACFrequency(int numFrequency, double *frequency);

/*
 * This routine must be called after lefwLayerACCurrentDensity. This routine
 * is optional and can be called only once after each
 * lefwLayerACCurrentDensity. This routine can only be called in Layer
 * Routing Returns 0 if successfull.
 */
EXTERN int lefwLayerACWidth(int numWidths, double *widths);

/*
 * This routine must be called after lefwLayerACCurrentDensity. This routine
 * is optional and can be called only once after each
 * lefwLayerACCurrentDensity. This routine can only be called in Layer
 * Returns 0 if successfull.
 */
EXTERN int lefwLayerACCutarea(int numCutareas, double *cutareas);

/*
 * This routine must be called after lefwLayerACCurrentDensity. This routine
 * is required and can be called only once after each
 * lefwLayerACCurrentDensity. Returns 0 if successfull.
 */
EXTERN int lefwLayerACTableEntries(int numEntries, double *entries);

/*
 * This routine must be called after lefwStartLayerRouting or lefwStartLayer.
 * This routine is optional and can be called multiple times. if the value
 * for the variable value is not 0, one of the following routines have to be
 * called: lefwLayerDCWidth, lefwLayerDCCutarea, or lefwLayerDCTableEntries
 * Returns 0 if successfull.
 */
EXTERN int lefwLayerDCCurrentDensity(const char *type, double value);

/*
 * This routine must be called after lefwLayerDCCurrentDensity. This routine
 * is optional and can be called only once after each
 * lefwLayerDCCurrentDensity. This routine can only be called in Layer
 * Routing Returns 0 if successfull.
 */
EXTERN int lefwLayerDCWidth(int numWidths, double *widths);

/*
 * This routine must be called after lefwLayerDCCurrentDensity. This routine
 * is optional and can be called only once after each
 * lefwLayerDCCurrentDensity. This routine can only be called in Layer
 * Returns 0 if successfull.
 */
EXTERN int lefwLayerDCCutarea(int numCutareas, double *cutareas);

/*
 * This routine must be called after lefwLayerDCCurrentDensity. This routine
 * is required and can be called only once after each
 * lefwLayerDCCurrentDensity. Returns 0 if successfull.
 */
EXTERN int lefwLayerDCTableEntries(int numEntries, double *entries);

/*
 * This routine is called after lefwInit. This section of routines is
 * optional. The routine can be called only once. Returns 0 if successfull.
 * The routine starts propertydefinitions section.
 */
EXTERN int lefwStartPropDef();

/*
 * This routine must be called after lefwStartPropDef. This routine can be
 * called multiple times. It adds integer property definition to the
 * statement. Returns 0 if successfull. The objType can be LIBRARY or VIA or
 * MACRO or PIN.
 */
EXTERN int 
lefwIntPropDef(
const char *objType,  /* LIBRARY | LAYER | VIA | VIARULE |
           * NONDEFAULTRULE | MACRO | PIN */
const char *propName,
double leftRange,/* optional(0) - RANGE */
double rightRange,  /* optional(0) */
int propValue);  /* optional(NULL) */

/*
 * This routine must be called after lefwStartPropDef. This routine can be
 * called multiple times. It adds real property definition to the statement.
 * Returns 0 if successfull. The objType can be LIBRARY or VIA or MACRO or
 * PIN.
 */
EXTERN int 
lefwRealPropDef(
const char *objType,  /* LIBRARY | LAYER | VIA | VIARULE |
           * NONDEFAULTRULE | MACRO | PIN */
const char *propName,
double leftRange,  /* optional(0) - RANGE */
double rightRange,  /* optional(0) */
double propValue);  /* optional(NULL) */

/*
 * This routine must be called after lefwStartPropDef. This routine can be
 * called multiple times. It adds string property definition to the
 * statement. Returns 0 if successfull. The objType can be LIBRARY or VIA or
 * MACRO or PIN.
 */
EXTERN int 
lefwStringPropDef(
const char *objType,  /* LIBRARY | LAYER | VIA | VIARULE |
           * NONDEFAULTRULE | MACRO | PIN */
const char *propName,
double leftRange,  /* optional(0) - RANGE */
double rightRange,  /* optional(0) */
const char *propValue);  /* optional(NULL) */

/*
 * This routine must be called after the lefwStartPropDef call (if any). The
 * routine can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEndPropDef();

/*
 * This routine is called after lefwInit. This routine must be called only
 * once. Returns 0 if successfull. The routine starts the via section. All of
 * the vias must follow.
 */
EXTERN int 
lefwStartVia(const char *viaName,
const char *isDefault);  /* optional(NULL) - DEFAULT */

/*
 * This routine is optional, it call only be called after lefwStartVia. It
 * can only be called once. Returns 0 if successfull.
 */
EXTERN int lefwViaTopofstackonly(); /* TOPOFSTACKONLY */

/*
 * This routine is optional, it call only be called after lefwStartVia. It
 * can only be called once. Returns 0 if successfull.
 */
EXTERN int 
lefwViaForeign(const char *foreignName,
double xl,  /* optional(0) - pt(x) */
double yl,  /* optional(0) - pt(y) */
int orient);  /* optional(-1) */

/*
 * This routine is optional, it call only be called after lefwStartVia. It
 * can only be called once. Returns 0 if successfull.
 */
EXTERN int lefwViaResistance(double resistance); /* RESISTANCE */

/*
 * This routine must be called after lefwStartVia. It can be called multiple
 * times. Returns 0 if successfull.
 */
EXTERN int lefwViaLayer(const char *layerName); /* LAYER */

/*
 * This routine is optional, it call only be called after lefwViaLayer. It
 * can be called multiple times. Returns 0 if successfull.
 */
EXTERN int 
lefwViaLayerRect(double x1l,  /* RECT pt1(x) */
double y1l,  /* RECT pt1(y) */
double x2l,  /* RECT pt2(x) */
double y2l);  /* RECT pt2(y) */

/*
 * This routine is called after lefwVia. This routine is optional, it adds
 * string property to the statement. It can be called multiple times. Returns
 * 0 if successfull.
 */
EXTERN int lefwStringProperty(const char *propName, const char *propValue);

/*
 * This routine is called after lefwVia. This routine is optional, it adds
 * real property to the statement. It can be called multiple times. Returns 0
 * if successfull.
 */
EXTERN int lefwRealProperty(const char *propName, double propValue);

/*
 * This routine is called after lefwVia. This routine is optional, it adds
 * int property to the statement. It can be called multiple times. Returns 0
 * if successfull.
 */
EXTERN int lefwIntProperty(const char *propName, int propValue);

/*
 * This routine must be called after the lefwStartVia call (if any). The
 * routine can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEndVia(const char *viaName);

/*
 * This routine is called after lefwInit. This routine must be called as a
 * set with lefwViaRuleLayer, lefwViaRuleVia and lefwEndViaRule multiple
 * times. Returns 0 if successfull. The routine starts the via rule section.
 * The via rule data and its property must follow
 */
EXTERN int lefwStartViaRule(const char *viaRuleName);

/*
 * This routine is called after lefwStartViaRule. This routine has to be
 * called 2 times exact. Returns 0 if successfull. The direction1 or
 * direction2 can be HORIZONTAL or VERTICAL.
 */
EXTERN int 
lefwViaRuleLayer(
const char *layerName,
const char *direction,  /* HORIZONTAL | VERTICAL */
double minWidth,  /* optional(0) - WIDTH */
double maxWidth,  /* optional(0) - WIDTH */
double overhang,  /* optional(0) - OVERHANG */
double metalOverhang);  /* optional(0) - METALOVERHANG */

/*
 * This routine is called after lefwViaRuleLayer is called twice. This
 * routine is required in a viarule and can be called more than once. Returns
 * 0 if successfull. The direction1 or direction2 can be HORIZONTAL or
 * VERTICAL.
 */
EXTERN int 
lefwViaRuleVia(
const char *viaName);

/*
 * This routine must be called after the lefwStartViaRule call (if any). The
 * routine can be called only once per lefwStartViaRule. Returns 0 if
 * successfull.
 */
EXTERN int lefwEndViaRule(const char *viaRuleName);

/*
 * This routine is called after lefwInit. This routine must be called as a
 * set with lefwViaRuleGenLayer, lefwViaRuleGenLayer3, and lefwEndViaRuleGen
 * multiple times. Returns 0 if successfull. The routine starts the via rule
 * section. The via rule data and its property must follow
 */
EXTERN int lefwStartViaRuleGen(const char *viaRuleName);

/*
 * This routine is called after lefwStartViaRuleGen. This routine has to be
 * called 2 times exact. Returns 0 if successfull. The direction1 or
 * direction2 can be HORIZONTAL or VERTICAL.
 */
EXTERN int 
lefwViaRuleGenLayer(
const char *layerName,
const char *direction,  /* HORIZONTAL | VERTICAL */
double minWidth,  /* optional(0) - WIDTH */
double maxWidth,  /* optional(0) - WIDTH */
double overhang,  /* optional(0) - OVERHANG */
double metalOverhang);  /* optional(0) -
             * METALOVERHANG */

/*
 * This routine is called after lefwViaRuleLayerGen is called twice. This
 * routine is optional in a viarule generate and can be called once. Returns
 * 0 if successfull. The direction1 or direction2 can be HORIZONTAL or
 * VERTICAL.
 */
EXTERN int 
lefwViaRuleGenLayer3(
const char *layerName,
double xl, double yl,  /* RECT pt1(x), pt1(y) */
double xh, double yh,  /* RECT pt2(x), pt2(y) */
double xSpacing, double ySpacing,  /* SPACING x and y */
double resistance);  /* optional(0) - RESISTANCE */

/*
 * This routine must be called after the lefwStartViaRuleGen call (if any).
 * The routine can be called only once per lefwStartViaRuleGen. Returns 0 if
 * successfull.
 */
EXTERN int lefwEndViaRuleGen(const char *viaRuleName);

/*
 * This routine is called after lefwInit. This routine must be called only
 * once. Returns 0 if successfull. The routine starts the nonDefaultRule
 * section. The nonDefaultRule layers must follow
 */
EXTERN int lefwStartNonDefaultRule(const char *ruleName);

/*
 * This routine is called after lefwInit. This routine must be called after
 * lefwStartNonDefaultRule This routine can be called multiple times. Returns
 * 0 if successfull.
 */
EXTERN int 
lefwNonDefaultRuleLayer(const char *routingLayerName,
double width,  /* WIDTH */
double minSpacing,  /* MINSPACING */
double wireExtension);  /* optinal(0) - WIREEXTENSION */

/*
 * This routine must be called after the lefwStartNonDefaultRule call (if
 * any). The routine can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEndNonDefaultRule(const char *ruleName);

/*
 * This routine is called after lefwInit. This section of routines is
 * optional. This routine can be called only once. Returns 0 if successfull.
 * It starts the spacing section.
 */
EXTERN int lefwStartSpacing();

/*
 * This routine must be called after lefwStartSpacing. It can be called
 * multiple times. Returns 0 if successfull. The stack has to be StACK.
 */
EXTERN int 
lefwSpacing(const char *layerName1,
const char *layerName2,
double minSpace,
const char *stack);  /* optional(NULL) */

/*
 * This routine must be called after the lefwStartSpacing call (if any). The
 * routine can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEndSpacing();

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successfull.
 */
EXTERN int lefwUniversalNoiseMargin(double high, double low);

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEdgeRateThreshold1(double num);

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEdgeRateThreshold2(double num);

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEdgeRateScaleFactor(double num);

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successfull. This routine starts the
 * noisetable section.
 */
EXTERN int lefwStartNoiseTable(int num);

/*
 * This routine is called after lefwStartNoiseTable or lefwStartCorrectTable.
 * This routine is optional and it can be called multiple times Returns 0 if
 * successfull.
 */
EXTERN int 
lefwEdgeRate(int num);

/*
 * This routine is called after lefwEdgeRate. This routine is optional and it
 * can be called only once inside lefwEdgeRate. Returns 0 if successfull.
 */
EXTERN int lefwOutputResistance(int numResists, double *resistance);

/*
 * This routine is called after lefwOutputResistance. This routine is
 * optional and it can be called multiple times inside lefwOutputResistance
 * Returns 0 if successfull.
 */
EXTERN int lefwVictims(int length, int numNoises, double *noises);

/*
 * This routine must be called after the lefwStartNoiseTable call (if any).
 * The routine can be called only once per section. Returns 0 if successfull.
 */
EXTERN int lefwEndNoiseTable();

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successfull. This routine starts the
 * correctTable section.
 */
EXTERN int lefwStartCorrectTable(int num);

/*
 * This routine must be called after the lefwStartCorrectTable call (if any).
 * The routine can be called only once per section. Returns 0 if successfull.
 */
EXTERN int lefwEndCorrectTable();

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMinFeature(double x, double y);

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successfull.
 */
/* EXTERN int lefwDielectric (float dielectric); */

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successfull. This routine starts the
 * irdrop section.
 */
EXTERN int lefwStartIrdrop();

/*
 * This routine is must be called after lefwStartIrdrop. It can be called
 * multiple times. Returns 0 if successfull. The currentsNvolts is a list of
 * current and volts.
 */
EXTERN int lefwIrdropTable(const char *tableName, const char *currentsNvolts);

/*
 * This routine must be called after the lefwStartIrdrop call (if any). The
 * routine can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEndIrdrop();

/*
 * This routine is must be called after lefwInit. It can be called multiple
 * times. Returns 0 if successfull. The classType can be PAD or CORE. The
 * symmetry can be a list of X or Y or R90.
 */
EXTERN int 
lefwSite(const char *siteName, const char *classType,
const char *symmetry, double width, double height);

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called multiple times. Returns 0 if successfull. This routine starts
 * the array section.
 */
EXTERN int lefwStartArray(const char *arrayName);

/*
 * This routine is called after lefwStartArray. Either this routine,
 * lefwArrayCanplace, lefwArrayCannotoccupy, lefwArrayTracks, or
 * lefwStartArrayFloorplan must be called at least once. It can be call
 * multiple times. Returns 0 if successfull.
 */
EXTERN int 
lefwArraySite(const char *name, double origX, double origY,
int orient, double numX, double numY, double spaceX,
double spaceY);

/*
 * This routine is called after lefwStartArray. Either this routine,
 * lefwArraySite, lefwArrayCannotoccupy, lefwArrayTracks, or
 * lefwStartArrayFloorplan must be called at least once. It can be call
 * multiple times. Returns 0 if successfull.
 */
EXTERN int 
lefwArrayCanplace(const char *name, double origX, double origY,
int orient, double numX, double numY, double spaceX,
double spaceY);

/*
 * This routine is called after lefwStartArray. Either this routine,
 * lefwArraySite, lefwArrayCanplace, lefwArrayTracks, or
 * lefwStartArrayFloorplan must be called at least once. It can be call
 * multiple times. Returns 0 if successfull.
 */
EXTERN int 
lefwArrayCannotoccupy(const char *name, double origX, double origY,
int orient, double numX, double numY, double spaceX,
double spaceY);

/*
 * This routine is called after lefwStartArray. Either this routine,
 * lefwArraySite, lefwArrayCanplace, lefwArrayCannotoccupy, or
 * lefwStartArrayFloorplan must be called at least once. It can be call
 * multiple times. Returns 0 if successfull.
 */
EXTERN int 
lefwArrayTracks(const char *xy, double start, int numTracks,
double space, const char *layers);

/*
 * This routine is called after lefwStartArray. Either this routine,
 * lefwArraySite, lefwArrayCanplace, lefwArrayCannotoccupy, or
 * lefwArrayTracks must be called at least once. It can be call multiple
 * times. Returns 0 if successfull. The routine starts the array floorplan
 * section
 */
EXTERN int lefwStartArrayFloorplan(const char *name);

/*
 * This routine must be called after lefwStartArrayFloorplan. It can be
 * called multiple times. Returns 0 if successful. The site can be CANPLACE
 * or CANNOTOCCUPY
 */
EXTERN int 
lefwArrayFloorplan(const char *site, const char *name,
double origX, double origY, int orient,
double numX, double numY, double spaceX, double spaceY);

/*
 * This routine must be called after the lefwStartArrayFloorplan call (if
 * any). The routine can be called only once per section. Returns 0 if
 * successfull.
 */
EXTERN int lefwEndArrayFloorplan(const char *name);

/*
 * This routine is called after lefwStartArray. This routine is optional. It
 * can be called multiple times. Returns 0 if successfull. The xy can be X or
 * Y.
 */
EXTERN int 
lefwArrayGcellgrid(const char *xy, double startXY, int colRows,
double spaceXY);

/*
 * This routine is called after lefwStartArray. This section of routines is
 * optional and can be call only once. Returns 0 if successfull. The routine
 * starts the array defaultcap section
 */
EXTERN int lefwStartArrayDefaultCap(int size);

/*
 * This routine must be called after lefwStartArrayDefaultCap. It can be
 * called multiple times. Returns 0 if successful.
 */
EXTERN int lefwArrayDefaultCap(double numPins, double cap);

/*
 * This routine must be called after the lefwStartArrayDefaultCap call (if
 * any). The routine can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEndArrayDefaultCap();

/*
 * This routine must be called after the lefwStartArray call (if any). The
 * routine can be called only once per section. Returns 0 if successfull.
 */
EXTERN int lefwEndArray(const char *arrayName);

/*
 * This routine is must be called after lefwInit. This routine can be called
 * multiple times. Returns 0 if successfull. This routine starts the macro
 * section.
 */
EXTERN int lefwStartMacro(const char *macroName);

/*
 * This routine is called after lefwStartMacro. This routine is optional and
 * can be called only once per macro section. Returns 0 if successfull. The
 * value1 can be COVER, RING, BLOCK, PAD, CORE, or ENCAP. The value2 can be
 * INPUT, OUTPUT, INOUT, POWER, or SPACER if value1 is PAD, or FEEDTHRU,
 * TIEHIGH, or TIELOW if value1 is CORE, or PRE, POST, TOPLEFT, TOPRIGHT,
 * BOOTOMLEFT, or BOTTOMRIGHT if value1 is ENCAP.
 */
EXTERN int 
lefwMacroClass(const char *value1,
const char *value2);  /* optional(NULL) */

/*
 * This routine is called after lefwStartMacro. This routine is optional and
 * can be called only once per macro section. Returns 0 if successfull. The
 * value1 can be USER, GENERATE, or BLOCK.
 */
EXTERN int lefwMacroSource(const char *value1);

/*
 * This routine is called after lefwStartMacro. This routine is optional and
 * can be called multiple times per macro section. Returns 0 if successfull.
 */
EXTERN int 
lefwMacroForeign(const char *name,
double xl,  /* optional(0) - pt(x) */
double yl,  /* optional(0) - pt(y) */
int orient);  /* optional(-1) - 0 to 7 */

/*
 * This routine is called after lefwStartMacro. This routine is optional and
 * can be called only once per macro section. Returns 0 if successfull.
 */
EXTERN int 
lefwMacroOrigin(double xl,  /* pt(x) */
double yl);  /* pt(y) */

/*
 * This routine is called after lefwStartMacro. This routine is optional and
 * can be called only once per macro section. Returns 0 if successfull.
 */
EXTERN int lefwMacroEEQ(const char *macroName);

/*
 * This routine is called after lefwStartMacro. This routine is optional and
 * can be called only once per macro section. Returns 0 if successfull.
 */
EXTERN int lefwMacroLEQ(const char *macroName);

/*
 * This routine is called after lefwStartMacro. This routine must be called
 * only once per macro section. Returns 0 if successfull.
 */
EXTERN int lefwMacroSize(double width, double height);

/*
 * This routine is called after lefwStartMacro. This routine is optional and
 * can be called only once per macro section. Returns 0 if successfull. The
 * symmetry can be a list of X, Y, or R90
 */
EXTERN int lefwMacroSymmetry(const char *symmetry);

/*
 * This routine is called after lefwStartMacro. This routine must be called
 * only once per macro section. Returns 0 if successfull.
 */
EXTERN int lefwMacroSite(const char *siteName);

/*
 * This routine is called after lefwStartMacro. This routine must be called
 * at least once per macro section. Returns 0 if successfull.
 */
EXTERN int 
lefwMacroSitePattern(const char *name, double origX, double origY,
int orient, int numX, int numY, double spaceX, double spaceY);

/*
 * This routine is called after lefwStartMacro. This routine is optional and
 * can be called only once per macro section. Returns 0 if successfull.
 */
EXTERN int lefwMacroPower(double power);

/*
 * This routine must be called after the lefwStartMacro call (if any). The
 * routine can be called only once per macro section. Returns 0 if
 * successfull.
 */
EXTERN int lefwEndMacro(const char *macroName);

/*
 * This routine must be called after the lefwStartMacro call (if any). The
 * routine can be called multiple time. Returns 0 if successfull. It starts
 * the macro pin section within macro.
 */
EXTERN int lefwStartMacroPin(const char *pinName);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinTaperRule(const char *ruleName);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int 
lefwMacroPinForeign(const char *name,
double xl,  /* optional(0) */
double yl,  /* optional(0) */
int orient);/* optional(-1) - 0 to 7 */

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinLEQ(const char *pinName);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull. The direction can
 * be INPUT, OUTPUT, OUTPUT TRISTATE, INOUT, or FEEDTHRU.
 */
EXTERN int lefwMacroPinDirection(const char *direction);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull. The use can be
 * SIGNAL, ANALOG, POWER, GROUND, or CLOCK.
 */
EXTERN int lefwMacroPinUse(const char *use);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull. The namce can be
 * ABUTMENT, RING, or FEEDTHRU.
 */
EXTERN int lefwMacroPinShape(const char *name);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinMustjoin(const char *name);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinOutputnoisemargin(int high, int low);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinOutputresistance(int high, int low);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinInputnoisemargin(int high, int low);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinPower(double power);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinLeakage(double leakage);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinCapacitance(double capacitance);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinResistance(double resistance);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinPulldownres(double resistance);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinTieoffr(double resistance);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinVHI(double voltage);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinVLO(double voltage);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinRisevoltagethreshold(double voltage);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinFallvoltagethreshold(double voltage);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinRisethresh(double capacitance);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinFallthresh(double capacitance);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinRisesatcur(double current);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinFallsatcur(double current);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull. The name can be
 * ACTIVE or RESISTIVE.
 */
EXTERN int lefwMacroPinCurrentsource(const char *name);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinIV_Tables(const char *lowName, const char *highName);

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called multiple times. Returns 0 if successfull.
 */
EXTERN int 
lefwMacroPinAntennasize(double value,
const char *layerName);  /* optional(NULL) */

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called multiple times. Returns 0 if successfull.
 */
EXTERN int 
lefwMacroPinAntennaMetalArea(double value,
const char *layerName);  /* optional(NULL) */

/*
 * This routine is called after lefwStartMacroPin. This routine is optional
 * and can be called multiple times. Returns 0 if successfull.
 */
EXTERN int 
lefwMacroPinAntennaMetalLength(double value,
const char *layerName);  /* optional(NULL) */

/*
 * This routine must be called after the lefwStartMacroPin call (if any). The
 * routine can be called only once per macro section. Returns 0 if
 * successfull.
 */
EXTERN int lefwEndMacroPin(const char *pinName);

/*
 * This routine must be called after the lefwStartMacroPin call (if any). The
 * routine can be called multiple time. Returns 0 if successfull. It starts
 * the macro pin port section within macro pin. The classType can be NONE or
 * CORE.
 */
EXTERN int lefwStartMacroPinPort(const char *classType); /* optional(NULL) */

/*
 * This routine is called after lefwStartMacroPinPort. Either this routine or
 * lefwStartMacroPinPortVia must be called. Returns 0 if successfull.
 */
EXTERN int lefwMacroPinPortLayer(const char *layerName);

/*
 * This routine is called after lefwMacroPinPortLayer. Returns 0 if
 * successfull.
 */
EXTERN int lefwMacroPinPortLayerWidth(double width);

/*
 * This routine is called after lefwStartMacroPinPortLayer. Either this
 * routine, lefwStartMacroPinPortLayerRect, or
 * lefwStartMacroPinPortLayerPolygon must be called. Returns 0 if
 * successfull.
 */
EXTERN int 
lefwMacroPinPortLayerPath(int num_paths, double *xl, double *yl,
int numX,  /* optional(0) */
int numY,  /* optional(0) */
double spaceX,  /* optional(0) */
double spaceY);  /* optional(0) */

/*
 * This routine is called after lefwStartMacroPinPortLayer. Either this
 * routine, lefwStartMacroPinPortLayerPath, or
 * lefwStartMacroPinPortLayerPolygon must be called. Returns 0 if
 * successfull.
 */
EXTERN int 
lefwMacroPinPortLayerRect(double xl1, double yl1,
double xl2, double yl2,
int numX,  /* optional(0) */
int numY,  /* optional(0) */
double spaceX,  /* optional(0) */
double spaceY);  /* optional(0) */

/*
 * This routine is called after lefwStartMacroPinPortLayer. Either this
 * routine, lefwStartMacroPinPortLayerPath, or lefwStartMacroPinPortLayerRect
 * must be called. Returns 0 if successfull.
 */
EXTERN int 
lefwMacroPinPortLayerPolygon(int num_polys,
double *xl, double *yl,
int numX,  /* optional(0) */
int numY,  /* optional(0) */
double spaceX,  /* optional(0) */
double spaceY);  /* optional(0) */

/*
 * This routine is called after lefwStartMacroPinPort. Either this routine or
 * lefwStartMacroPinPortLayer must be called. Returns 0 if successfull.
 */
EXTERN int 
lefwMacroPinPortVia(double xl, double yl, const char *viaName,
int numX,  /* optional(0) */
int numY,  /* optional(0) */
double spaceX,  /* optional(0) */
double spaceY);  /* optional(0) */

/*
 * This routine must be called after the lefwStartMacroPinPort call (if any).
 * The routine can be called only once per macro section. Returns 0 if
 * successfull.
 */
EXTERN int lefwEndMacroPinPort();

/*
 * This routine is called after the lefwStartMacro call (if any). The routine
 * is optional and can be called multiple times. Returns 0 if successfull. It
 * starts the macro obs section within macro.
 */
EXTERN int lefwStartMacroObs();

/*
 * This routine is called after lefwStartMacroObs. Either this routine or
 * lefwMacroObsVia must be called. Returns 0 if successfull.
 */
EXTERN int lefwMacroObsLayer(const char *layerName);

/*
 * This routine is called after lefwStartMacroObs. Either this routine or
 * lefwMacroObsVia must be called. Returns 0 if successfull.
 */
EXTERN int lefwMacroObsLayerWidth(double width);

/*
 * This routine is called after lefwStartMacroObsLayer. Either this routine,
 * lefwMacroObsLayerRect, or lefwStartMacroObsLayerPolygon must be called.
 * Returns 0 if successfull.
 */
EXTERN int 
lefwMacroObsLayerPath(int num_paths, double *xl, double *yl,
int numX,  /* optional(0) */
int numY,  /* optional(0) */
double spaceX,  /* optional(0) */
double spaceY);  /* optional(0) */

/*
 * This routine is called after lefwStartMacroObsLayer. Either this routine,
 * lefwMacroObsLayerPath, or lefwStartMacroObsLayerPolygon must be called.
 * Returns 0 if successfull.
 */
EXTERN int 
lefwMacroObsLayerRect(double xl1, double yl1,
double xl2, double yl2,
int numX,  /* optional(0) */
int numY,  /* optional(0) */
double spaceX,  /* optional(0) */
double spaceY);  /* optional(0) */

/*
 * This routine is called after lefwStartMacroObsLayer. Either this routine,
 * lefwMacroObsLayerPath, or lefwStartMacroObsLayerPath must be called.
 * Returns 0 if successfull.
 */
EXTERN int 
lefwMacroObsLayerPolygon(int num_polys, double *xl, double *yl,
int numX,  /* optional(0) */
int numY,  /* optional(0) */
double spaceX,  /* optional(0) */
double spaceY);  /* optional(0) */

/*
 * This routine is called after lefwStartMacroObs. Either this routine or
 * lefwSacroObsLayer must be called. Returns 0 if successfull.
 */
EXTERN int 
lefwMacroObsVia(double xl, double yl, const char *viaName,
int numX,  /* optional(0) */
int numY,  /* optional(0) */
double spaceX,  /* optional(0) */
double spaceY);  /* optional(0) */

/*
 * This routine must be called after the lefwStartMacroObs call (if any). The
 * routine can be called only once per macro section. Returns 0 if
 * successfull.
 */
EXTERN int lefwEndMacroObs();

/*
 * This routine is called after the lefwStartMacro call (if any). The routine
 * is optional and can be called only once. Returns 0 if successfull. It
 * starts the macro timing section within macro.
 */
EXTERN int lefwStartMacroTiming();

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull. This routine is for
 * {FROMPIN pinName...; [TOPIN pinName...;] | FROMPIN pinName...;} This
 * routince can be called multiple times. The num_frPinNames contains the
 * number of object in the array fromPins.
 */
EXTERN int lefwMacroTimingPin(const char *fromPin, const char *toPin);

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull. The riseFall can be RISE
 * or FALL.
 */
EXTERN int 
lefwMacroTimingIntrinsic(const char *riseFall, double min,
double max,
double slewT1,  /* optional(0) */
double slewT1Min,  /* optional(0) */
double slewT1Max,  /* optional(0) */
double slewT2,  /* optional(0) */
double slewT2Min,  /* optional(0) */
double slewT2Max,  /* optional(0) */
double slewT3,  /* optional(0) */
double varMin, double varMax);

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull.
 */
EXTERN int lefwMacroTimingRisers(double min, double max);

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull.
 */
EXTERN int lefwMacroTimingFallrs(double min, double max);

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull.
 */
EXTERN int lefwMacroTimingRisecs(double min, double max);

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull.
 */
EXTERN int lefwMacroTimingFallcs(double min, double max);

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull.
 */
EXTERN int lefwMacroTimingRisesatt1(double min, double max);

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull.
 */
EXTERN int lefwMacroTimingFallsatt1(double min, double max);

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull.
 */
EXTERN int lefwMacroTimingRiset0(double min, double max);

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull.
 */
EXTERN int lefwMacroTimingFallt0(double min, double max);

/*
 * This routine must be called after the lefwStartMacroTiming. It can be
 * called multiple times. Returns 0 if successfull. The unateness can be
 * INVERT, NONINVERT or NONUNATE.
 */
EXTERN int lefwMacroTimingUnateness(const char *unateness);

/*
 * This routine must be called after the lefwStartMacroTiming call (if any).
 * The routine can be called only once. Returns 0 if successfull.
 */
EXTERN int lefwEndMacroTiming();

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successfull.
 */
EXTERN int 
lefwAntenna(const char *type,  /* INPUTPINANTENNASIZE | OUTPUTPINANTENNASIZE
         * | INOUTPINANTENNASIZE */
double value);

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successful.
 */
EXTERN int lefwStartBeginext(const char *name);

/*
 * This routine is called after lefwBeginext. This routine is optional, it
 * can be called only once. Returns 0 if successful.
 */
EXTERN int lefwBeginextCreator(const char *creatorName);

/*
 * This routine is called after lefwBeginext. This routine is optional, it
 * can be called only once. It gets the current system time and date. Returns
 * 0 if successful.
 */
EXTERN int lefwBeginextDate();

/*
 * This routine is called after lefwBeginext. This routine is optional, it
 * can be called only once. Returns 0 if successful.
 */
EXTERN int lefwBeginextRevision(int vers1, int vers2); /* vers1.vers2 */

/*
 * This routine is called after lefwBeginext. This routine is optional, it
 * can be called many times. It allows user to customize their own syntax.
 * Returns 0 if successful.
 */
EXTERN int lefwBeginextSyntax(const char *title, const char *string);

/*
 * This routine is called after lefwInit. This routine is optional and it can
 * be called only once. Returns 0 if successful.
 */
EXTERN int lefwEndBeginext();

/*
 * General routines that can be called anytime after the Init is called.
 */
EXTERN int lefwCurrentLineNumber();

/*
 * This routine must call last, it ends the Lef library. It must be called
 * only once. Returns 0 if successfull.
 */
EXTERN int lefwEnd();

/* This routine will print the error message. */
EXTERN void lefwPrintError(int status);

/*
 * This routine will allow user to write their own comemnt.  It will
 * automactically add a # infront of the line.
 */
EXTERN void lefwAddComment(const char *comment);

/* This routine will indent 3 blank spaces */
EXTERN void lefwAddIndent();

#endif
