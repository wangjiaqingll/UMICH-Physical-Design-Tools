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

#ifndef defiPinCap_h
#define defiPinCap_h
#include "defiKRDefs.h"
#include <stdio.h>

/*
 * pin num
 */

/*
 * capacitance
 */

typedef struct defiPinCap_s {
  int             pin_;
  double          cap_;
} defiPinCap;

EXTERN void
defiPinCap_setPin
  PROTO_PARAMS(( defiPinCap * this,
                 int p ));

EXTERN void
defiPinCap_setCap
  PROTO_PARAMS(( defiPinCap * this,
                 double d ));

EXTERN int
defiPinCap_pin
  PROTO_PARAMS(( const defiPinCap * this ));

EXTERN double
defiPinCap_cap
  PROTO_PARAMS(( const defiPinCap * this ));

EXTERN void
defiPinCap_print
  PROTO_PARAMS(( const defiPinCap * this,
                 FILE * f ));

/*
 * optional parts
 */

/*
 * allocated size of pin name
 */

/*
 * allocated size of net name
 */

/*
 * orient 0-7
 */

/*
 * allocated size of length
 */

/*
 * allocated size of direction
 */

/*
 * allocated size of layer
 */

/*
 * placement
 */

typedef struct defiPin_s {
  int             pinNameLength_;
  char           *pinName_;
  int             netNameLength_;
  char           *netName_;
  char            hasDirection_;
  char            hasUse_;
  char            hasLayer_;
  char            placeType_;
  char            orient_;
  int             useLength_;
  char           *use_;
  int             directionLength_;
  char           *direction_;
  int             layerLength_;
  char           *layer_;
  int             xl_, yl_, xh_, yh_;
  int             x_, y_;
  int             hasSpecial_;
} defiPin;

EXTERN defiPin *
defiPin_Create
  PROTO_PARAMS((  ));

EXTERN void
defiPin_Init
  PROTO_PARAMS(( defiPin * this ));

EXTERN void
defiPin_Delete
  PROTO_PARAMS(( defiPin * this ));

EXTERN void
defiPin_Destroy
  PROTO_PARAMS(( defiPin * this ));

EXTERN void
defiPin_Setup
  PROTO_PARAMS(( defiPin * this,
                 const char *pinName,
                 const char *netName ));

EXTERN void
defiPin_setDirection
  PROTO_PARAMS(( defiPin * this,
                 const char *dir ));

EXTERN void
defiPin_setUse
  PROTO_PARAMS(( defiPin * this,
                 const char *use ));

EXTERN void
defiPin_setLayer
  PROTO_PARAMS(( defiPin * this,
                 const char *layer,
                 int xl,
                 int yl,
                 int xh,
                 int yh ));

EXTERN void
defiPin_setPlacement
  PROTO_PARAMS(( defiPin * this,
                 int typ,
                 int x,
                 int y,
                 int orient ));

EXTERN void
defiPin_setSpecial
  PROTO_PARAMS(( defiPin * this ));

EXTERN void
defiPin_clear
  PROTO_PARAMS(( defiPin * this ));

EXTERN const char *
defiPin_pinName
  PROTO_PARAMS(( const defiPin * this ));

EXTERN const char *
defiPin_netName
  PROTO_PARAMS(( const defiPin * this ));

EXTERN int
defiPin_hasDirection
  PROTO_PARAMS(( const defiPin * this ));

EXTERN int
defiPin_hasUse
  PROTO_PARAMS(( const defiPin * this ));

EXTERN int
defiPin_hasLayer
  PROTO_PARAMS(( const defiPin * this ));

EXTERN int
defiPin_hasPlacement
  PROTO_PARAMS(( const defiPin * this ));

EXTERN int
defiPin_isPlaced
  PROTO_PARAMS(( const defiPin * this ));

EXTERN int
defiPin_isCover
  PROTO_PARAMS(( const defiPin * this ));

EXTERN int
defiPin_isFixed
  PROTO_PARAMS(( const defiPin * this ));

EXTERN int
defiPin_placementX
  PROTO_PARAMS(( const defiPin * this ));

EXTERN int
defiPin_placementY
  PROTO_PARAMS(( const defiPin * this ));

EXTERN const char *
defiPin_direction
  PROTO_PARAMS(( const defiPin * this ));

EXTERN const char *
defiPin_use
  PROTO_PARAMS(( const defiPin * this ));

EXTERN const char *
defiPin_layer
  PROTO_PARAMS(( const defiPin * this ));

EXTERN void
defiPin_bounds
  PROTO_PARAMS(( const defiPin * this,
                 int *xl,
                 int *yl,
                 int *xh,
                 int *yh ));

EXTERN int
defiPin_orient
  PROTO_PARAMS(( const defiPin * this ));

EXTERN int
defiPin_hasSpecial
  PROTO_PARAMS(( const defiPin * this ));

EXTERN void
defiPin_print
  PROTO_PARAMS(( const defiPin * this,
                 FILE * f ));

#endif
