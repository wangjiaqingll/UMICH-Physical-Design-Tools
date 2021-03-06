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

#ifndef lefiNonDefault_h
#define lefiNonDefault_h
#include <stdio.h>
#include "lefiKRDefs.h"
#include "lefiVia.h"
#include "lefiMisc.h"

/*
 * A non default rule can have one or more layers.
 */

/*
 * The layer information is kept in an array.
 */

/*
 * A non default rule can have one or more vias.
 */

/*
 * These routines return the via info.
 */

/*
 * A non default rule can have one or more spacing rules.
 */

/*
 * These routines return the that info.
 */

/*
 * Debug print
 */

/*
 * save the users' functions.
 */

/*
 * save the users' functions.
 */

/*
 * Layer information
 */

typedef struct lefiNonDefault_s {
  void           *oldViaCbk_;
  void           *oldSpacingCbk_;
  void           *oldSpacingBeginCbk_;
  void           *oldSpacingEndCbk_;
  void           *oldUserData_;
  int             nameSize_;
  char           *name_;
  int             numLayers_;
  int             layersAllocated_;
  char          **layerName_;
  double         *width_;
  double         *spacing_;
  double         *wireExtension_;
  char           *hasWidth_;
  char           *hasSpacing_;
  char           *hasWireExtension_;
  int             numVias_;
  int             allocatedVias_;
  lefiVia       **viaRules_;
  int             numSpacing_;
  int             allocatedSpacing_;
  lefiSpacing   **spacingRules_;
  int             numProps_;
  int             propsAllocated_;
  char          **names_;
  char          **values_;
} lefiNonDefault;

EXTERN lefiNonDefault *
lefiNonDefault_Create
  PROTO_PARAMS((  ));

EXTERN void
lefiNonDefault_Init
  PROTO_PARAMS(( lefiNonDefault * this ));

EXTERN void
lefiNonDefault_Destroy
  PROTO_PARAMS(( lefiNonDefault * this ));

EXTERN void
lefiNonDefault_Delete
  PROTO_PARAMS(( lefiNonDefault * this ));

EXTERN void
lefiNonDefault_setName
  PROTO_PARAMS(( lefiNonDefault * this,
                 const char *name ));

EXTERN void
lefiNonDefault_addLayer
  PROTO_PARAMS(( lefiNonDefault * this,
                 const char *name ));

EXTERN void
lefiNonDefault_addWidth
  PROTO_PARAMS(( lefiNonDefault * this,
                 double num ));

EXTERN void
lefiNonDefault_addWireExtension
  PROTO_PARAMS(( lefiNonDefault * this,
                 double num ));

EXTERN void
lefiNonDefault_addSpacing
  PROTO_PARAMS(( lefiNonDefault * this,
                 double num ));

EXTERN void
lefiNonDefault_addSpacingRule
  PROTO_PARAMS(( lefiNonDefault * this,
                 lefiSpacing * s ));

EXTERN void
lefiNonDefault_addViaRule
  PROTO_PARAMS(( lefiNonDefault * this,
                 lefiVia * v ));

EXTERN void
lefiNonDefault_end
  PROTO_PARAMS(( lefiNonDefault * this ));

EXTERN void
lefiNonDefault_clear
  PROTO_PARAMS(( lefiNonDefault * this ));

EXTERN void
lefiNonDefault_addProp
  PROTO_PARAMS(( lefiNonDefault * this,
                 const char *name,
                 const char *value ));

EXTERN const char *
lefiNonDefault_name
  PROTO_PARAMS(( const lefiNonDefault * this ));

EXTERN int
lefiNonDefault_numProps
  PROTO_PARAMS(( const lefiNonDefault * this ));

EXTERN const char *
lefiNonDefault_propName
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN const char *
lefiNonDefault_propValue
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN int
lefiNonDefault_numLayers
  PROTO_PARAMS(( const lefiNonDefault * this ));

EXTERN const char *
lefiNonDefault_layerName
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN int
lefiNonDefault_hasLayerWidth
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN double
lefiNonDefault_layerWidth
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN int
lefiNonDefault_hasLayerSpacing
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN double
lefiNonDefault_layerSpacing
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN int
lefiNonDefault_hasLayerWireExtension
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN double
lefiNonDefault_layerWireExtension
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN int
lefiNonDefault_numVias
  PROTO_PARAMS(( const lefiNonDefault * this ));

EXTERN lefiVia *
lefiNonDefault_viaRule
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN int
lefiNonDefault_numSpacingRules
  PROTO_PARAMS(( const lefiNonDefault * this ));

EXTERN lefiSpacing *
lefiNonDefault_spacingRule
  PROTO_PARAMS(( const lefiNonDefault * this,
                 int index ));

EXTERN void
lefiNonDefault_print
  PROTO_PARAMS(( lefiNonDefault * this,
                 FILE * f ));

#endif
