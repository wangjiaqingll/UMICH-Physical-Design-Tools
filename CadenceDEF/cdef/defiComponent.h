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

#ifndef defiComponent_h
#define defiComponent_h
#include <stdio.h>
#include "defiKRDefs.h"

/*
 * Placement status for the component.
 */

/*
 * Default is UNPLACED
 */
#define DEFI_COMPONENT_UNPLACED 0
#define DEFI_COMPONENT_PLACED 1
#define DEFI_COMPONENT_FIXED 2
#define DEFI_COMPONENT_COVER 3

/*
 * Struct holds the data for one component.
 */

/*
 * Returns arrays for the ll and ur of the rectangles in the region.
 */

/*
 * The number of items in the arrays is given in size.
 */

/*
 * Debug printing
 */

/*
 * instance id
 */

/*
 * name.
 */

/*
 * allocated size of name.
 */

/*
 * allocated size of id.
 */

/*
 * allocate size of foreign ori
 */

/*
 * placement status
 */

/*
 * the file supplied a region name for this comp
 */

/*
 * the file supplied an eeq
 */

/*
 * the file supplied an generate name and macro name
 */

/*
 * the file supplied a weight
 */

/*
 * the file supplied a foreign orig name
 */

/*
 * orientation
 */

/*
 * placement loc
 */

/*
 * region points
 */

/*
 * name.
 */

/*
 * allocated size of region name
 */

/*
 * allocated size of eeq
 */

/*
 * number of net connections
 */

/*
 * allocated size of nets array
 */

/*
 * net connections
 */

/*
 * the file supplied a foreign name
 */

/*
 * name
 */

/*
 * allocate size of foreign name
 */

/*
 * foreign loc
 */

/*
 * foreign ori
 */

typedef struct defiComponent_s {
  char           *id_;
  char           *name_;
  int             nameSize_;
  int             idSize_;
  int             ForiSize_;
  char            status_;
  char            hasRegionName_;
  char            hasEEQ_;
  char            hasGenerate_;
  char            hasWeight_;
  char            hasFori_;
  char            orient_;
  int             x_, y_;
  int             numRects_;
  int             rectsAllocated_;
  int            *rectXl_;
  int            *rectYl_;
  int            *rectXh_;
  int            *rectYh_;
  char           *regionName_;
  int             regionNameSize_;
  char           *EEQ_;
  int             EEQSize_;
  int             numNets_;
  int             netsAllocated_;
  char          **nets_;
  int             weight_;
  char           *source_;
  char            hasForeignName_;
  char           *foreignName_;
  int             foreignNameSize_;
  int             Fx_, Fy_;
  char           *Fori_;
  int             generateNameSize_;
  char           *generateName_;
  int             macroNameSize_;
  char           *macroName_;
  int             numProps_;
  int             propsAllocated_;
  char          **names_;
  char          **values_;
} defiComponent;

EXTERN defiComponent *
defiComponent_Create
  PROTO_PARAMS((  ));

EXTERN void
defiComponent_Init
  PROTO_PARAMS(( defiComponent * this ));

EXTERN void
defiComponent_Destroy
  PROTO_PARAMS(( defiComponent * this ));

EXTERN void
defiComponent_Delete
  PROTO_PARAMS(( defiComponent * this ));

EXTERN void
defiComponent_IdAndName
  PROTO_PARAMS(( defiComponent * this,
                 const char *id,
                 const char *name ));

EXTERN void
defiComponent_setGenerate
  PROTO_PARAMS(( defiComponent * this,
                 const char *genName,
                 const char *macroName ));

EXTERN void
defiComponent_setPlacementStatus
  PROTO_PARAMS(( defiComponent * this,
                 int n ));

EXTERN void
defiComponent_setPlacementLocation
  PROTO_PARAMS(( defiComponent * this,
                 int x,
                 int y,
                 int orient ));

EXTERN void
defiComponent_setRegionName
  PROTO_PARAMS(( defiComponent * this,
                 const char *name ));

EXTERN void
defiComponent_setRegionBounds
  PROTO_PARAMS(( defiComponent * this,
                 int xl,
                 int yl,
                 int xh,
                 int yh ));

EXTERN void
defiComponent_setEEQ
  PROTO_PARAMS(( defiComponent * this,
                 const char *name ));

EXTERN void
defiComponent_addNet
  PROTO_PARAMS(( defiComponent * this,
                 const char *netName ));

EXTERN void
defiComponent_addProperty
  PROTO_PARAMS(( defiComponent * this,
                 const char *name,
                 const char *value ));

EXTERN void
defiComponent_reverseNetOrder
  PROTO_PARAMS(( defiComponent * this ));

EXTERN void
defiComponent_setWeight
  PROTO_PARAMS(( defiComponent * this,
                 int w ));

EXTERN void
defiComponent_setSource
  PROTO_PARAMS(( defiComponent * this,
                 const char *name ));

EXTERN void
defiComponent_setForeignName
  PROTO_PARAMS(( defiComponent * this,
                 const char *name ));

EXTERN void
defiComponent_setFori
  PROTO_PARAMS(( defiComponent * this,
                 const char *name ));

EXTERN void
defiComponent_setForeignLocation
  PROTO_PARAMS(( defiComponent * this,
                 int x,
                 int y,
                 char *ori ));

EXTERN void
defiComponent_clear
  PROTO_PARAMS(( defiComponent * this ));

EXTERN const char *
defiComponent_id
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN const char *
defiComponent_name
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_placementStatus
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_isPlaced
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_isFixed
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_isCover
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_placementX
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_placementY
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_placementOrient
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_hasRegionName
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_hasRegionBounds
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_hasEEQ
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_hasGenerate
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_hasSource
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_hasWeight
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_weight
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_hasNets
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_numNets
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN const char *
defiComponent_net
  PROTO_PARAMS(( const defiComponent * this,
                 int index ));

EXTERN const char *
defiComponent_regionName
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN const char *
defiComponent_source
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN const char *
defiComponent_EEQ
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN const char *
defiComponent_generateName
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN const char *
defiComponent_macroName
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN void
defiComponent_regionBounds
  PROTO_PARAMS(( defiComponent * this,
                 int *size,
                 int **xl,
                 int **yl,
                 int **xh,
                 int **yh ));

EXTERN int
defiComponent_hasForeignName
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN const char *
defiComponent_foreignName
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_foreignX
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_foreignY
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN const char *
defiComponent_foreignOri
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_hasFori
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN int
defiComponent_numProps
  PROTO_PARAMS(( const defiComponent * this ));

EXTERN char *
defiComponent_propName
  PROTO_PARAMS(( const defiComponent * this,
                 int index ));

EXTERN char *
defiComponent_propValue
  PROTO_PARAMS(( const defiComponent * this,
                 int index ));

EXTERN void
defiComponent_print
  PROTO_PARAMS(( defiComponent * this,
                 FILE * fout ));

EXTERN void
defiComponent_bumpId
  PROTO_PARAMS(( defiComponent * this,
                 int size ));

EXTERN void
defiComponent_bumpName
  PROTO_PARAMS(( defiComponent * this,
                 int size ));

EXTERN void
defiComponent_bumpRegionName
  PROTO_PARAMS(( defiComponent * this,
                 int size ));

EXTERN void
defiComponent_bumpEEQ
  PROTO_PARAMS(( defiComponent * this,
                 int size ));

EXTERN void
defiComponent_bumpNets
  PROTO_PARAMS(( defiComponent * this,
                 int size ));

EXTERN void
defiComponent_bumpForeignName
  PROTO_PARAMS(( defiComponent * this,
                 int size ));

EXTERN void
defiComponent_bumpFori
  PROTO_PARAMS(( defiComponent * this,
                 int size ));

#endif
