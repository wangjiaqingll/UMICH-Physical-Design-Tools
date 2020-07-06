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

#ifndef lefiLayer_h
#define lefiLayer_h
#include <stdio.h>
#include "lefiKRDefs.h"

typedef struct lefiLayerDensity_s {
  char           *type_;
  double          oneEntry_;
  int             numFrequency_;
  double         *frequency_;
  int             numWidths_;
  double         *widths_;
  int             numTableEntries_;
  double         *tableEntries_;
  int             numCutareas_;
  double         *cutareas_;
} lefiLayerDensity;

EXTERN lefiLayerDensity *
lefiLayerDensity_Create
  PROTO_PARAMS((  ));

EXTERN void
lefiLayerDensity_Delete
  PROTO_PARAMS(( lefiLayerDensity * this ));

EXTERN void
lefiLayerDensity_Init
  PROTO_PARAMS(( lefiLayerDensity * this,
                 const char *type ));

EXTERN void
lefiLayerDensity_Destroy
  PROTO_PARAMS(( lefiLayerDensity * this ));

EXTERN void
lefiLayerDensity_clear
  PROTO_PARAMS(( lefiLayerDensity * this ));

EXTERN void
lefiLayerDensity_setOneEntry
  PROTO_PARAMS(( lefiLayerDensity * this,
                 double entry ));

EXTERN void
lefiLayerDensity_addFrequency
  PROTO_PARAMS(( lefiLayerDensity * this,
                 int num,
                 double *frequency ));

EXTERN void
lefiLayerDensity_addWidth
  PROTO_PARAMS(( lefiLayerDensity * this,
                 int num,
                 double *width ));

EXTERN void
lefiLayerDensity_addTableEntry
  PROTO_PARAMS(( lefiLayerDensity * this,
                 int num,
                 double *entry ));

EXTERN void
lefiLayerDensity_addCutarea
  PROTO_PARAMS(( lefiLayerDensity * this,
                 int num,
                 double *cutarea ));

EXTERN char *
lefiLayerDensity_type
  PROTO_PARAMS(( lefiLayerDensity * this ));

EXTERN int
lefiLayerDensity_hasOneEntry
  PROTO_PARAMS(( lefiLayerDensity * this ));

EXTERN double
lefiLayerDensity_oneEntry
  PROTO_PARAMS(( lefiLayerDensity * this ));

EXTERN int
lefiLayerDensity_numFrequency
  PROTO_PARAMS(( lefiLayerDensity * this ));

EXTERN double
lefiLayerDensity_frequency
  PROTO_PARAMS(( lefiLayerDensity * this,
                 int index ));

EXTERN int
lefiLayerDensity_numWidths
  PROTO_PARAMS(( lefiLayerDensity * this ));

EXTERN double
lefiLayerDensity_width
  PROTO_PARAMS(( lefiLayerDensity * this,
                 int index ));

EXTERN int
lefiLayerDensity_numTableEntries
  PROTO_PARAMS(( lefiLayerDensity * this ));

EXTERN double
lefiLayerDensity_tableEntry
  PROTO_PARAMS(( lefiLayerDensity * this,
                 int index ));

EXTERN int
lefiLayerDensity_numCutareas
  PROTO_PARAMS(( lefiLayerDensity * this ));

EXTERN double
lefiLayerDensity_cutArea
  PROTO_PARAMS(( lefiLayerDensity * this,
                 int index ));

/*
 * calls clear to init
 */

/*
 * Debug print
 */

/*
 * for Cut Layer, spacing is multiple
 */

/*
 * number of ACCURRENTDENSITY
 */

/*
 * number of DCCURRENTDENSITY
 */

typedef struct lefiLayer_s {
  char           *name_;
  int             nameSize_;
  char           *type_;
  int             typeSize_;
  int             hasPitch_;
  int             hasOffset_;
  int             hasWidth_;
  int             hasSpacing_;
  int             hasDirection_;
  int             hasResistance_;
  int             hasCapacitance_;
  int             hasHeight_;
  int             hasWireExtension_;
  int             hasThickness_;
  int             hasShrinkage_;
  int             hasCapMultiplier_;
  int             hasEdgeCap_;
  int             hasAntennaArea_;
  int             hasAntennaLength_;
  int             hasCurrentDensityPoint_;
  int             hasUseLengthThreshold_;
  double          currentDensity_;
  double          pitch_;
  double          offset_;
  double          width_;
  double          wireExtension_;
  int             numSpacings_;
  int             spacingsAllocated_;
  double         *spacing_;
  char          **spacingName_;
  double          left_;
  double          right_;
  char           *direction_;
  double          resistance_;
  double          capacitance_;
  double          height_;
  double          thickness_;
  double          shrinkage_;
  double          capMultiplier_;
  double          edgeCap_;
  double          antennaArea_;
  double          antennaLength_;
  int             numCurrentPoints_;
  int             currentPointsAllocated_;
  double         *currentWidths_;
  double         *current_;
  int             numCapacitancePoints_;
  int             capacitancePointsAllocated_;
  double         *capacitanceWidths_;
  double         *capacitances_;
  int             numResistancePoints_;
  int             resistancePointsAllocated_;
  double         *resistanceWidths_;
  double         *resistances_;
  int             numProps_;
  int             propsAllocated_;
  char          **names_;
  char          **values_;
  int             numAccurrents_;
  int             accurrentAllocated_;
  lefiLayerDensity **accurrents_;
  int             numDccurrents_;
  int             dccurrentAllocated_;
  lefiLayerDensity **dccurrents_;
  int             numNums_;
  int             numAllocated_;
  double         *nums_;
  double          lengthThreshold_;
} lefiLayer;

EXTERN lefiLayer *
lefiLayer_Create
  PROTO_PARAMS((  ));

EXTERN void
lefiLayer_Init
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_Destroy
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_Delete
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_clear
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_setName
  PROTO_PARAMS(( lefiLayer * this,
                 const char *name ));

EXTERN void
lefiLayer_setType
  PROTO_PARAMS(( lefiLayer * this,
                 const char *typ ));

EXTERN void
lefiLayer_setPitch
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setOffset
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setWidth
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setSpacing
  PROTO_PARAMS(( lefiLayer * this,
                 double dist,
                 double left,
                 double right ));

EXTERN void
lefiLayer_setSpacingName
  PROTO_PARAMS(( lefiLayer * this,
                 const char *spacingName ));

EXTERN void
lefiLayer_setDirection
  PROTO_PARAMS(( lefiLayer * this,
                 const char *dir ));

EXTERN void
lefiLayer_setResistance
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setCapacitance
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setHeight
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setWireExtension
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setThickness
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setShrinkage
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setCapMultiplier
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setEdgeCap
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setAntennaArea
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setAntennaLength
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setCurrentDensity
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setCurrentPoint
  PROTO_PARAMS(( lefiLayer * this,
                 double width,
                 double current ));

EXTERN void
lefiLayer_setResistancePoint
  PROTO_PARAMS(( lefiLayer * this,
                 double width,
                 double res ));

EXTERN void
lefiLayer_setCapacitancePoint
  PROTO_PARAMS(( lefiLayer * this,
                 double width,
                 double cap ));

EXTERN void
lefiLayer_addProp
  PROTO_PARAMS(( lefiLayer * this,
                 const char *name,
                 const char *value ));

EXTERN void
lefiLayer_addAccurrentDensity
  PROTO_PARAMS(( lefiLayer * this,
                 const char *type ));

EXTERN void
lefiLayer_setAcOneEntry
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_addAcFrequency
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_addAcTableEntry
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_addAcWidth
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_addDccurrentDensity
  PROTO_PARAMS(( lefiLayer * this,
                 const char *type ));

EXTERN void
lefiLayer_setDcOneEntry
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_addDcTableEntry
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_addDcWidth
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_addDcCutarea
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_addNumber
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN void
lefiLayer_setSpacingUseLength
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN void
lefiLayer_setSpacingLength
  PROTO_PARAMS(( lefiLayer * this,
                 double num ));

EXTERN int
lefiLayer_hasType
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasPitch
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasOffset
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasWidth
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasSpacingNumber
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasSpacingRange
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasDirection
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasResistance
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasResistanceArray
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasCapacitance
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasCapacitanceArray
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasHeight
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasThickness
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasWireExtension
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasShrinkage
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasCapMultiplier
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasEdgeCap
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasAntennaLength
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasAntennaArea
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasCurrentDensityPoint
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasCurrentDensityArray
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasAccurrentDensity
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasDccurrentDensity
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN int
lefiLayer_hasSpacingUseLengthThreshold
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN int
lefiLayer_hasSpacingLengthThreshold
  PROTO_PARAMS(( lefiLayer * this ));

EXTERN int
lefiLayer_numProps
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN const char *
lefiLayer_propName
  PROTO_PARAMS(( const lefiLayer * this,
                 int index ));

EXTERN const char *
lefiLayer_propValue
  PROTO_PARAMS(( const lefiLayer * this,
                 int index ));

EXTERN int
lefiLayer_numSpacing
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN char *
lefiLayer_name
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN const char *
lefiLayer_type
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_pitch
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_offset
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_width
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_spacing
  PROTO_PARAMS(( const lefiLayer * this,
                 int index ));

EXTERN char *
lefiLayer_spacingName
  PROTO_PARAMS(( const lefiLayer * this,
                 int index ));

EXTERN double
lefiLayer_spacingRangeLeft
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_spacingRangeRight
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN const char *
lefiLayer_direction
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_resistance
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_capacitance
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_height
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_wireExtension
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_thickness
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_shrinkage
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_capMultiplier
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_edgeCap
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_antennaLength
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_antennaArea
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN double
lefiLayer_currentDensityPoint
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN void
lefiLayer_currentDensityArray
  PROTO_PARAMS(( const lefiLayer * this,
                 int *numPoints,
                 double **widths,
                 double **current ));

EXTERN void
lefiLayer_capacitanceArray
  PROTO_PARAMS(( const lefiLayer * this,
                 int *numPoints,
                 double **widths,
                 double **resValues ));

EXTERN void
lefiLayer_resistanceArray
  PROTO_PARAMS(( const lefiLayer * this,
                 int *numPoints,
                 double **widths,
                 double **capValues ));

EXTERN int
lefiLayer_numAccurrentDensity
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN lefiLayerDensity *
lefiLayer_accurrent
  PROTO_PARAMS(( const lefiLayer * this,
                 int index ));

EXTERN int
lefiLayer_numDccurrentDensity
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN lefiLayerDensity *
lefiLayer_dccurrent
  PROTO_PARAMS(( const lefiLayer * this,
                 int index ));

EXTERN double
lefiLayer_spacingLengthThreshold
  PROTO_PARAMS(( const lefiLayer * this ));

EXTERN void
lefiLayer_print
  PROTO_PARAMS(( const lefiLayer * this,
                 FILE * f ));

#endif
