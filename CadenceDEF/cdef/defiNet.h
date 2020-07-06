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

#ifndef defiNet_h
#define defiNet_h
#include <stdio.h>
#include "defiKRDefs.h"
#include "defiPath.h"

/*
 * Either FIXED COVER ROUTED
 */

/*
 * Debug printing
 */

/*
 * name.
 */

/*
 * allocated size of name.
 */

/*
 * number of pins used in array.
 */

/*
 * number of pins allocated in array.
 */

/*
 * instance names for connections
 */

/*
 * pin names for connections
 */

/*
 * synthesized flags for pins
 */

/*
 * must-join flags
 */

/*
 * net type
 */

/*
 * paths for this subnet
 */

/*
 * number of paths used
 */

/*
 * allocated size of paths array
 */

typedef struct defiSubnet_s {
  char           *name_;
  int             nameSize_;
  int             numPins_;
  int             pinsAllocated_;
  char          **instances_;
  char          **pins_;
  char           *synthesized_;
  char           *musts_;
  char            isFixed_;
  char            isRouted_;
  char            isCover_;
  defiPath      **paths_;
  int             numPaths_;
  int             pathsAllocated_;
  char           *nonDefaultRule_;
} defiSubnet;

EXTERN defiSubnet *
defiSubnet_Create
  PROTO_PARAMS((  ));

EXTERN void
defiSubnet_Init
  PROTO_PARAMS(( defiSubnet * this ));

EXTERN void
defiSubnet_Destroy
  PROTO_PARAMS(( defiSubnet * this ));

EXTERN void
defiSubnet_Delete
  PROTO_PARAMS(( defiSubnet * this ));

EXTERN void
defiSubnet_setName
  PROTO_PARAMS(( defiSubnet * this,
                 const char *name ));

EXTERN void
defiSubnet_setNonDefault
  PROTO_PARAMS(( defiSubnet * this,
                 const char *name ));

EXTERN void
defiSubnet_addPin
  PROTO_PARAMS(( defiSubnet * this,
                 const char *instance,
                 const char *pin,
                 int syn ));

EXTERN void
defiSubnet_addMustPin
  PROTO_PARAMS(( defiSubnet * this,
                 const char *instance,
                 const char *pin,
                 int syn ));

EXTERN void
defiSubnet_setType
  PROTO_PARAMS(( defiSubnet * this,
                 const char *typ ));

EXTERN void
defiSubnet_addPath
  PROTO_PARAMS(( defiSubnet * this,
                 defiPath * p ));

EXTERN void
defiSubnet_print
  PROTO_PARAMS(( defiSubnet * this,
                 FILE * f ));

EXTERN const char *
defiSubnet_name
  PROTO_PARAMS(( const defiSubnet * this ));

EXTERN int
defiSubnet_numConnections
  PROTO_PARAMS(( defiSubnet * this ));

EXTERN const char *
defiSubnet_instance
  PROTO_PARAMS(( defiSubnet * this,
                 int index ));

EXTERN const char *
defiSubnet_pin
  PROTO_PARAMS(( defiSubnet * this,
                 int index ));

EXTERN int
defiSubnet_pinIsSynthesized
  PROTO_PARAMS(( defiSubnet * this,
                 int index ));

EXTERN int
defiSubnet_pinIsMustJoin
  PROTO_PARAMS(( defiSubnet * this,
                 int index ));

EXTERN int
defiSubnet_isFixed
  PROTO_PARAMS(( const defiSubnet * this ));

EXTERN int
defiSubnet_isRouted
  PROTO_PARAMS(( const defiSubnet * this ));

EXTERN int
defiSubnet_isCover
  PROTO_PARAMS(( const defiSubnet * this ));

EXTERN int
defiSubnet_hasNonDefaultRule
  PROTO_PARAMS(( const defiSubnet * this ));

EXTERN int
defiSubnet_hasShield
  PROTO_PARAMS(( const defiSubnet * this ));

EXTERN int
defiSubnet_hasShieldNet
  PROTO_PARAMS(( const defiSubnet * this ));

EXTERN int
defiSubnet_hasNoShieldNet
  PROTO_PARAMS(( const defiSubnet * this ));

EXTERN int
defiSubnet_numPaths
  PROTO_PARAMS(( const defiSubnet * this ));

EXTERN defiPath *
defiSubnet_path
  PROTO_PARAMS(( defiSubnet * this,
                 int index ));

EXTERN const char *
defiSubnet_nonDefaultRule
  PROTO_PARAMS(( const defiSubnet * this ));

EXTERN void
defiSubnet_bumpName
  PROTO_PARAMS(( defiSubnet * this,
                 int size ));

EXTERN void
defiSubnet_bumpPins
  PROTO_PARAMS(( defiSubnet * this,
                 int size ));

EXTERN void
defiSubnet_bumpPaths
  PROTO_PARAMS(( defiSubnet * this,
                 int size ));

EXTERN void
defiSubnet_clear
  PROTO_PARAMS(( defiSubnet * this ));


typedef struct defiVpin_s {
  int             xl_;
  int             yl_;
  int             xh_;
  int             yh_;
  int             orient_;

  /* 0-7  -1 is no orient   */
  char            status_;

  /* P-placed  F-fixed  C-cover  ' '- none   */
  int             xLoc_;
  int             yLoc_;
  char           *name_;
  char           *layer_;
} defiVpin;

EXTERN defiVpin *
defiVpin_Create
  PROTO_PARAMS((  ));

EXTERN void
defiVpin_Delete
  PROTO_PARAMS(( defiVpin * this ));

EXTERN void
defiVpin_Init
  PROTO_PARAMS(( defiVpin * this,
                 const char *name ));

EXTERN void
defiVpin_Destroy
  PROTO_PARAMS(( defiVpin * this ));

EXTERN void
defiVpin_setLayer
  PROTO_PARAMS(( defiVpin * this,
                 const char *name ));

EXTERN void
defiVpin_setBounds
  PROTO_PARAMS(( defiVpin * this,
                 int xl,
                 int yl,
                 int xh,
                 int yh ));

EXTERN void
defiVpin_setOrient
  PROTO_PARAMS(( defiVpin * this,
                 int orient ));

EXTERN void
defiVpin_setLoc
  PROTO_PARAMS(( defiVpin * this,
                 int x,
                 int y ));

EXTERN void
defiVpin_setStatus
  PROTO_PARAMS(( defiVpin * this,
                 char st ));

EXTERN int
defiVpin_xl
  PROTO_PARAMS(( const defiVpin * this ));

EXTERN int
defiVpin_yl
  PROTO_PARAMS(( const defiVpin * this ));

EXTERN int
defiVpin_xh
  PROTO_PARAMS(( const defiVpin * this ));

EXTERN int
defiVpin_yh
  PROTO_PARAMS(( const defiVpin * this ));

EXTERN char
defiVpin_status
  PROTO_PARAMS(( const defiVpin * this ));

EXTERN int
defiVpin_orient
  PROTO_PARAMS(( const defiVpin * this ));

EXTERN int
defiVpin_xLoc
  PROTO_PARAMS(( const defiVpin * this ));

EXTERN int
defiVpin_yLoc
  PROTO_PARAMS(( const defiVpin * this ));

EXTERN const char *
defiVpin_name
  PROTO_PARAMS(( const defiVpin * this ));

EXTERN const char *
defiVpin_layer
  PROTO_PARAMS(( const defiVpin * this ));


typedef struct defiShield_s {
  char           *name_;
  int             numPaths_;
  int             pathsAllocated_;
  defiPath      **paths_;
} defiShield;

EXTERN defiShield *
defiShield_Create
  PROTO_PARAMS((  ));

EXTERN void
defiShield_Delete
  PROTO_PARAMS(( defiShield * this ));

EXTERN void
defiShield_Init
  PROTO_PARAMS(( defiShield * this,
                 const char *name ));

EXTERN void
defiShield_Destroy
  PROTO_PARAMS(( defiShield * this ));

EXTERN void
defiShield_clear
  PROTO_PARAMS(( defiShield * this ));

EXTERN void
defiShield_addPath
  PROTO_PARAMS(( defiShield * this,
                 defiPath * p ));

EXTERN const char *
defiShield_shieldName
  PROTO_PARAMS(( const defiShield * this ));

EXTERN int
defiShield_numPaths
  PROTO_PARAMS(( const defiShield * this ));

EXTERN defiPath *
defiShield_path
  PROTO_PARAMS(( defiShield * this,
                 int index ));

EXTERN void
defiShield_bumpPaths
  PROTO_PARAMS(( defiShield * this,
                 int size ));

/*
 * Struct holds the data for one component.
 */

/*
 * Routines used by YACC to set the fields in the net.
 */

/*
 * Either FIXED COVER ROUTED
 */

/*
 * Routines to return the value of net data.
 */

/*
 * Debug printing
 */

/*
 * name.
 */

/*
 * allocated size of name.
 */

/*
 * number of pins used in array.
 */

/*
 * number of pins allocated in array.
 */

/*
 * instance names for connections
 */

/*
 * pin names for connections
 */

/*
 * must-join flags for pins
 */

/*
 * synthesized flags for pins
 */

/*
 * net weight
 */

/*
 * flag for optional weight
 */

/*
 * net type
 */

/*
 * file supplied a capacitance value
 */

/*
 * num of props in array
 */

/*
 * Prop names
 */

/*
 * Prop values All in strings!
 */

/*
 * allocated size of props array
 */

/*
 * num of subnets in array
 */

/*
 * Prop names
 */

/*
 * allocated size of props array
 */

/*
 * cap value
 */

/*
 * paths for this subnet
 */

/*
 * number of paths used
 */

/*
 * allocated size of paths array
 */

/*
 * number of SHIELD paths used
 */

/*
 * allocated size of SHIELD paths array
 */

/*
 * SHIELD data
 */

/*
 * number of SHIELDNETS used in array.
 */

/*
 * number of SHIELDNETS allocated in array.
 */

/*
 * name of the SHIELDNET
 */

typedef struct defiNet_s {
  char           *name_;
  int             nameSize_;
  int             numPins_;
  int             pinsAllocated_;
  char          **instances_;
  char          **pins_;
  char           *musts_;
  char           *synthesized_;
  int             weight_;
  char            hasWeight_;
  char            isFixed_;
  char            isRouted_;
  char            isCover_;
  char            hasCap_;
  char            hasVoltage_;
  int             numProps_;
  char          **propNames_;
  char          **propValues_;
  int             propsAllocated_;
  int             numSubnets_;
  defiSubnet    **subnets_;
  int             subnetsAllocated_;
  double          cap_;
  char           *source_;
  char           *pattern_;
  char           *original_;
  char           *use_;
  char           *nonDefaultRule_;
  char           *style_;
  defiPath      **paths_;
  int             numPaths_;
  int             pathsAllocated_;
  double          voltage_;
  int             widthsAllocated_;
  int             numWidths_;
  char          **wlayers_;
  double         *wdist_;
  int             spacingAllocated_;
  int             numSpacing_;
  char          **slayers_;
  double         *sdist_;
  double         *sleft_;
  double         *sright_;
  int             xTalk_;
  int             numVpins_;
  int             vpinsAllocated_;
  defiVpin      **vpins_;
  int             numShields_;
  int             shieldsAllocated_;
  defiShield    **shields_;
  int             numShieldNet_;
  int             shieldNetsAllocated_;
  char          **shieldNet_;
} defiNet;

EXTERN defiNet *
defiNet_Create
  PROTO_PARAMS((  ));

EXTERN void
defiNet_Init
  PROTO_PARAMS(( defiNet * this ));

EXTERN void
defiNet_Destroy
  PROTO_PARAMS(( defiNet * this ));

EXTERN void
defiNet_Delete
  PROTO_PARAMS(( defiNet * this ));

EXTERN void
defiNet_setName
  PROTO_PARAMS(( defiNet * this,
                 const char *name ));

EXTERN void
defiNet_addPin
  PROTO_PARAMS(( defiNet * this,
                 const char *instance,
                 const char *pin,
                 int syn ));

EXTERN void
defiNet_addMustPin
  PROTO_PARAMS(( defiNet * this,
                 const char *instance,
                 const char *pin,
                 int syn ));

EXTERN void
defiNet_setWeight
  PROTO_PARAMS(( defiNet * this,
                 int w ));

EXTERN void
defiNet_addProp
  PROTO_PARAMS(( defiNet * this,
                 const char *name,
                 const char *value ));

EXTERN void
defiNet_addSubnet
  PROTO_PARAMS(( defiNet * this,
                 defiSubnet * subnet ));

EXTERN void
defiNet_setType
  PROTO_PARAMS(( defiNet * this,
                 const char *typ ));

EXTERN void
defiNet_setSource
  PROTO_PARAMS(( defiNet * this,
                 const char *typ ));

EXTERN void
defiNet_setOriginal
  PROTO_PARAMS(( defiNet * this,
                 const char *typ ));

EXTERN void
defiNet_setPattern
  PROTO_PARAMS(( defiNet * this,
                 const char *typ ));

EXTERN void
defiNet_setCap
  PROTO_PARAMS(( defiNet * this,
                 double w ));

EXTERN void
defiNet_setUse
  PROTO_PARAMS(( defiNet * this,
                 const char *typ ));

EXTERN void
defiNet_setNonDefaultRule
  PROTO_PARAMS(( defiNet * this,
                 const char *typ ));

EXTERN void
defiNet_setStyle
  PROTO_PARAMS(( defiNet * this,
                 const char *typ ));

EXTERN void
defiNet_addShield
  PROTO_PARAMS(( defiNet * this,
                 const char *shieldNetName ));

EXTERN void
defiNet_addShieldNet
  PROTO_PARAMS(( defiNet * this,
                 const char *shieldNetName ));

EXTERN void
defiNet_addNoShieldNet
  PROTO_PARAMS(( defiNet * this,
                 const char *shieldNetName ));

EXTERN void
defiNet_addPath
  PROTO_PARAMS(( defiNet * this,
                 defiPath * p ));

EXTERN void
defiNet_addShieldPath
  PROTO_PARAMS(( defiNet * this,
                 defiPath * p ));

EXTERN void
defiNet_clear
  PROTO_PARAMS(( defiNet * this ));

EXTERN void
defiNet_setWidth
  PROTO_PARAMS(( defiNet * this,
                 const char *layer,
                 double dist ));

EXTERN void
defiNet_setSpacing
  PROTO_PARAMS(( defiNet * this,
                 const char *layer,
                 double dist ));

EXTERN void
defiNet_setVoltage
  PROTO_PARAMS(( defiNet * this,
                 double num ));

EXTERN void
defiNet_setRange
  PROTO_PARAMS(( defiNet * this,
                 double left,
                 double right ));

EXTERN void
defiNet_setXTalk
  PROTO_PARAMS(( defiNet * this,
                 int num ));

EXTERN void
defiNet_addVpin
  PROTO_PARAMS(( defiNet * this,
                 const char *name ));

EXTERN void
defiNet_addVpinLayer
  PROTO_PARAMS(( defiNet * this,
                 const char *name ));

EXTERN void
defiNet_addVpinLoc
  PROTO_PARAMS(( defiNet * this,
                 const char *status,
                 int x,
                 int y,
                 int orient ));

EXTERN void
defiNet_addVpinBounds
  PROTO_PARAMS(( defiNet * this,
                 int xl,
                 int yl,
                 int xh,
                 int yh ));

EXTERN const char *
defiNet_name
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_weight
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_numProps
  PROTO_PARAMS(( const defiNet * this ));

EXTERN const char *
defiNet_propName
  PROTO_PARAMS(( const defiNet * this,
                 int index ));

EXTERN const char *
defiNet_propValue
  PROTO_PARAMS(( const defiNet * this,
                 int index ));

EXTERN int
defiNet_numConnections
  PROTO_PARAMS(( const defiNet * this ));

EXTERN const char *
defiNet_instance
  PROTO_PARAMS(( const defiNet * this,
                 int index ));

EXTERN const char *
defiNet_pin
  PROTO_PARAMS(( const defiNet * this,
                 int index ));

EXTERN int
defiNet_pinIsMustJoin
  PROTO_PARAMS(( const defiNet * this,
                 int index ));

EXTERN int
defiNet_pinIsSynthesized
  PROTO_PARAMS(( const defiNet * this,
                 int index ));

EXTERN int
defiNet_numSubnets
  PROTO_PARAMS(( const defiNet * this ));

EXTERN defiSubnet *
defiNet_subnet
  PROTO_PARAMS(( defiNet * this,
                 int index ));

EXTERN int
defiNet_isFixed
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_isRouted
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_isCover
  PROTO_PARAMS(( const defiNet * this ));


/* Routines to get the information about Virtual Pins.   */
EXTERN int
defiNet_numVpins
  PROTO_PARAMS(( const defiNet * this ));

EXTERN defiVpin *
defiNet_vpin
  PROTO_PARAMS(( const defiNet * this,
                 int index ));

EXTERN int
defiNet_hasProps
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasWeight
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasSubnets
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasSource
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasPattern
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasOriginal
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasCap
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasUse
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasStyle
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasNonDefaultRule
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasVoltage
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasSpacingRules
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasWidthRules
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_hasXTalk
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_numSpacingRules
  PROTO_PARAMS(( const defiNet * this ));

EXTERN void
defiNet_spacingRule
  PROTO_PARAMS(( defiNet * this,
                 int index,
                 char **layer,
                 double *dist,
                 double *left,
                 double *right ));

EXTERN int
defiNet_numWidthRules
  PROTO_PARAMS(( const defiNet * this ));

EXTERN void
defiNet_widthRule
  PROTO_PARAMS(( defiNet * this,
                 int index,
                 char **layer,
                 double *dist ));

EXTERN double
defiNet_voltage
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_XTalk
  PROTO_PARAMS(( const defiNet * this ));

EXTERN const char *
defiNet_source
  PROTO_PARAMS(( const defiNet * this ));

EXTERN const char *
defiNet_original
  PROTO_PARAMS(( const defiNet * this ));

EXTERN const char *
defiNet_pattern
  PROTO_PARAMS(( const defiNet * this ));

EXTERN double
defiNet_cap
  PROTO_PARAMS(( const defiNet * this ));

EXTERN const char *
defiNet_use
  PROTO_PARAMS(( const defiNet * this ));

EXTERN const char *
defiNet_style
  PROTO_PARAMS(( const defiNet * this ));

EXTERN const char *
defiNet_nonDefaultRule
  PROTO_PARAMS(( const defiNet * this ));

EXTERN int
defiNet_numPaths
  PROTO_PARAMS(( const defiNet * this ));

EXTERN defiPath *
defiNet_path
  PROTO_PARAMS(( defiNet * this,
                 int index ));

EXTERN int
defiNet_numShields
  PROTO_PARAMS(( const defiNet * this ));

EXTERN defiShield *
defiNet_shield
  PROTO_PARAMS(( defiNet * this,
                 int index ));

EXTERN int
defiNet_numShieldNets
  PROTO_PARAMS(( const defiNet * this ));

EXTERN const char *
defiNet_shieldNet
  PROTO_PARAMS(( const defiNet * this,
                 int index ));

EXTERN int
defiNet_numNoShields
  PROTO_PARAMS(( const defiNet * this ));

EXTERN defiShield *
defiNet_noShield
  PROTO_PARAMS(( defiNet * this,
                 int index ));

EXTERN void
defiNet_print
  PROTO_PARAMS(( defiNet * this,
                 FILE * f ));

EXTERN void
defiNet_bumpName
  PROTO_PARAMS(( defiNet * this,
                 int size ));

EXTERN void
defiNet_bumpPins
  PROTO_PARAMS(( defiNet * this,
                 int size ));

EXTERN void
defiNet_bumpProps
  PROTO_PARAMS(( defiNet * this,
                 int size ));

EXTERN void
defiNet_bumpSubnets
  PROTO_PARAMS(( defiNet * this,
                 int size ));

EXTERN void
defiNet_bumpPaths
  PROTO_PARAMS(( defiNet * this,
                 int size ));

EXTERN void
defiNet_bumpShieldNets
  PROTO_PARAMS(( defiNet * this,
                 int size ));

#endif
