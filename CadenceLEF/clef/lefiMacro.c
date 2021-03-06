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
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "lefiMacro.h"
#include "lefiMisc.h"
#include "lefiDebug.h"

/*
 * lefiGeometries
 */ lefiGeometries *
lefiGeometries_CreateUninitialized()
{
  lefiGeometries *this = (lefiGeometries *) malloc(sizeof(lefiGeometries));

  lefiGeometries_Init(this);
  return (this);
}

void 
lefiGeometries_Init(lefiGeometries * this)
{
  this->itemsAllocated_ = 2;
  this->numItems_ = 0;
  this->itemType_ = (enum lefiGeomEnum *) malloc(sizeof(enum lefiGeomEnum) * 2);
  this->items_ = (void **) malloc(sizeof(void *) * 2);
  this->pointsAllocated_ = 0;
  this->x_ = 0;
  this->y_ = 0;
}

void 
lefiGeometries_Destroy(lefiGeometries * this)
{
  lefiGeometries_clear(this);
  free((char *) (this->items_));
  free((char *) (this->itemType_));
  if (this->x_) {
    free((double *) (this->x_));
    free((double *) (this->y_));
  }
}

void
lefiGeometries_Delete(lefiGeometries * this)
{
  if (this == NULL)
    return;
  lefiGeometries_Destroy(this);
  free(this);
}

void 
lefiGeometries_clear(lefiGeometries * this)
{
  int     i;

  for (i = 0;
       i < this->numItems_;
       i++) {
    if (this->itemType_[i] == lefiGeomViaE)
      free(((struct lefiGeomVia *) (this->items_[i]))->name);
    if (this->itemType_[i] == lefiGeomViaIterE)
      free(((struct lefiGeomViaIter *) (this->items_[i]))->name);
    if (this->itemType_[i] == lefiGeomPathE) {
      ((struct lefiGeomPath *) (this->items_[i]))->numPoints = 0;
      free((double *) ((struct lefiGeomPath *) this->items_[i])->x);
      free((double *) ((struct lefiGeomPath *) this->items_[i])->y);
    }
    free((char *) (this->items_[i]));
  }
  this->numItems_ = 0;
}

void 
lefiGeometries_add(lefiGeometries * this,
		   void *v,
		   enum lefiGeomEnum e)
{
  if (this->numItems_ == this->itemsAllocated_) {
    int     i;

    void  **newi;

    enum lefiGeomEnum *newe;

    this->itemsAllocated_ *= 2;
    newe = (enum lefiGeomEnum *) malloc(sizeof(enum lefiGeomEnum) * this->itemsAllocated_);
    newi = (void **) malloc(sizeof(void *) * this->itemsAllocated_);
    for (i = 0;
	 i < this->numItems_;
	 i++) {
      newe[i] = this->itemType_[i];
      newi[i] = this->items_[i];
    }
    free((char *) (this->items_));
    free((char *) (this->itemType_));
    this->items_ = newi;
    this->itemType_ = newe;
  }
  this->items_[this->numItems_] = v;
  this->itemType_[this->numItems_] = e;
  this->numItems_ += 1;
}

void 
lefiGeometries_addLayer(lefiGeometries * this,
			const char *name)
{
  char   *c = (char *) malloc(strlen(name) + 1);

  strcpy(c, CASE(name));
  lefiGeometries_add(this,
		     (void *) c, lefiGeomLayerE);
}

void 
lefiGeometries_addClass(lefiGeometries * this,
			const char *name)
{
  char   *c = (char *) malloc(strlen(name) + 1);

  strcpy(c, CASE(name));
  lefiGeometries_add(this,
		     (void *) c, lefiGeomClassE);
}

void 
lefiGeometries_addWidth(lefiGeometries * this,
			double w)
{
  double *d = (double *) malloc(sizeof(double));

  *d = w;
  lefiGeometries_add(this,
		     (void *) d, lefiGeomWidthE);
}

void 
lefiGeometries_addPath(lefiGeometries * this)
{
  int     i;

  int     lim;

  struct lefiGeomPath *p = (struct lefiGeomPath *) malloc(sizeof(struct lefiGeomPath));

  lim = p->numPoints = this->numPoints_;
  p->x = (double *) malloc(sizeof(double) * lim);
  p->y = (double *) malloc(sizeof(double) * lim);
  for (i = 0;
       i < lim;
       i++) {
    p->x[i] = this->x_[i];
    p->y[i] = this->y_[i];
  }
  lefiGeometries_add(this,
		     (void *) p, lefiGeomPathE);
}

void 
lefiGeometries_addPathIter(lefiGeometries * this)
{
  int     i;

  int     lim;

  struct lefiGeomPathIter *p = (struct lefiGeomPathIter *) malloc(sizeof(struct lefiGeomPathIter));

  lim = p->numPoints = this->numPoints_;
  p->x = (double *) malloc(sizeof(double) * lim);
  p->y = (double *) malloc(sizeof(double) * lim);
  for (i = 0;
       i < lim;
       i++) {
    p->x[i] = this->x_[i];
    p->y[i] = this->y_[i];
  }
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  lefiGeometries_add(this,
		     (void *) p, lefiGeomPathIterE);
}

void 
lefiGeometries_addRect(lefiGeometries * this,
		       double xl,
		       double yl,
		       double xh,
		       double yh)
{
  struct lefiGeomRect *p = (struct lefiGeomRect *) malloc(sizeof(struct lefiGeomRect));

  p->xl = xl;
  p->yl = yl;
  p->xh = xh;
  p->yh = yh;
  lefiGeometries_add(this,
		     (void *) p, lefiGeomRectE);
}

void 
lefiGeometries_addRectIter(lefiGeometries * this,
			   double xl,
			   double yl,
			   double xh,
			   double yh)
{
  struct lefiGeomRectIter *p = (struct lefiGeomRectIter *) malloc(sizeof(struct lefiGeomRectIter));

  p->xl = xl;
  p->yl = yl;
  p->xh = xh;
  p->yh = yh;
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  lefiGeometries_add(this,
		     (void *) p, lefiGeomRectIterE);
}

void 
lefiGeometries_addPolygon(lefiGeometries * this)
{
  int     i;

  int     lim;

  struct lefiGeomPolygon *p = (struct lefiGeomPolygon *) malloc(sizeof(struct lefiGeomPolygon));

  lim = p->numPoints = this->numPoints_;
  p->x = (double *) malloc(sizeof(double) * lim);
  p->y = (double *) malloc(sizeof(double) * lim);
  for (i = 0;
       i < lim;
       i++) {
    p->x[i] = this->x_[i];
    p->y[i] = this->y_[i];
  }
  lefiGeometries_add(this,
		     (void *) p, lefiGeomPolygonE);
}

void 
lefiGeometries_addPolygonIter(lefiGeometries * this)
{
  int     i;

  int     lim;

  struct lefiGeomPolygonIter *p = (struct lefiGeomPolygonIter *) malloc(sizeof(struct lefiGeomPolygonIter));

  lim = p->numPoints = this->numPoints_;
  p->x = (double *) malloc(sizeof(double) * lim);
  p->y = (double *) malloc(sizeof(double) * lim);
  for (i = 0;
       i < lim;
       i++) {
    p->x[i] = this->x_[i];
    p->y[i] = this->y_[i];
  }
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  lefiGeometries_add(this,
		     (void *) p, lefiGeomPolygonIterE);
}

void 
lefiGeometries_addVia(lefiGeometries * this,
		      double x,
		      double y,
		      const char *name)
{
  struct lefiGeomVia *p = (struct lefiGeomVia *) malloc(sizeof(struct lefiGeomVia));

  char   *c = (char *) malloc(strlen(name) + 1);

  strcpy(c, CASE(name));
  p->x = x;
  p->y = y;
  p->name = c;
  lefiGeometries_add(this,
		     (void *) p, lefiGeomViaE);
}

void 
lefiGeometries_addViaIter(lefiGeometries * this,
			  double x,
			  double y,
			  const char *name)
{
  struct lefiGeomViaIter *p = (struct lefiGeomViaIter *) malloc(sizeof(struct lefiGeomViaIter));

  char   *c = (char *) malloc(strlen(name) + 1);

  strcpy(c, CASE(name));
  p->x = x;
  p->y = y;
  p->name = c;
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  lefiGeometries_add(this,
		     (void *) p, lefiGeomViaIterE);
}

void 
lefiGeometries_addStepPattern(lefiGeometries * this,
			      double xStart,
			      double yStart,
			      double xStep,
			      double yStep)
{
  this->xStart_ = xStart;
  this->yStart_ = yStart;
  this->xStep_ = xStep;
  this->yStep_ = yStep;
}

void 
lefiGeometries_startList(lefiGeometries * this,
			 double x,
			 double y)
{
  if (!this->x_) {
    this->numPoints_ = 0;
    this->pointsAllocated_ = 16;
    this->x_ = (double *) malloc(sizeof(double) * 16);
    this->y_ = (double *) malloc(sizeof(double) * 16);
  }
  else {
    this->numPoints_ = 0;
  }
  lefiGeometries_addToList(this,
			   x,
			   y);
}

void 
lefiGeometries_addToList(lefiGeometries * this,
			 double x,
			 double y)
{
  if (this->numPoints_ == this->pointsAllocated_) {
    int     i;

    double *nx;

    double *ny;

    this->pointsAllocated_ *= 2;
    nx = (double *) malloc(sizeof(double) * this->pointsAllocated_);
    ny = (double *) malloc(sizeof(double) * this->pointsAllocated_);
    for (i = 0;
	 i < this->numPoints_;
	 i++) {
      nx[i] = this->x_[i];
      ny[i] = this->y_[i];
    }
    free((char *) (this->x_));
    free((char *) (this->y_));
    this->x_ = nx;
    this->y_ = ny;
  }
  this->x_[this->numPoints_] = x;
  this->y_[this->numPoints_] = y;
  this->numPoints_ += 1;
}

int 
lefiGeometries_numItems(const lefiGeometries * this)
{
  return this->numItems_;
}

enum lefiGeomEnum 
lefiGeometries_itemType(const lefiGeometries * this,
			int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return lefiGeomUnknown;
  }
  return this->itemType_[index];
}

struct lefiGeomRect *
lefiGeometries_getRect(const lefiGeometries * this,
		       int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomRect *) (this->items_[index]);
}

struct lefiGeomRectIter *
lefiGeometries_getRectIter(const lefiGeometries * this,
			   int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomRectIter *) (this->items_[index]);
}

struct lefiGeomPath *
lefiGeometries_getPath(const lefiGeometries * this,
		       int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomPath *) (this->items_[index]);
}

struct lefiGeomPathIter *
lefiGeometries_getPathIter(const lefiGeometries * this,
			   int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomPathIter *) (this->items_[index]);
}

char   *
lefiGeometries_getLayer(const lefiGeometries * this,
			int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (char *) (this->items_[index]);
}

double 
lefiGeometries_getWidth(const lefiGeometries * this,
			int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return *((double *) (this->items_[index]));
}

struct lefiGeomPolygon *
lefiGeometries_getPolygon(const lefiGeometries * this,
			  int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomPolygon *) (this->items_[index]);
}

struct lefiGeomPolygonIter *
lefiGeometries_getPolygonIter(const lefiGeometries * this,
			      int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomPolygonIter *) (this->items_[index]);
}

char   *
lefiGeometries_getClass(const lefiGeometries * this,
			int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (char *) (this->items_[index]);
}

struct lefiGeomVia *
lefiGeometries_getVia(const lefiGeometries * this,
		      int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomVia *) (this->items_[index]);
}

struct lefiGeomViaIter *
lefiGeometries_getViaIter(const lefiGeometries * this,
			  int index)
{
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomViaIter *) (this->items_[index]);
}

void 
lefiGeometries_print(const lefiGeometries * this,
		     FILE * f)
{
  int     i;

  int     l;

  struct lefiGeomRect *rect;

  struct lefiGeomRectIter *rectiter;

  struct lefiGeomPath *path;

  struct lefiGeomPathIter *pathiter;

  struct lefiGeomPolygon *polygon;

  struct lefiGeomPolygonIter *polygoniter;

  for (i = 0;
       i < this->numItems_;
       i++) {
    switch (lefiGeometries_itemType(this,
				    i)) {
    case lefiGeomLayerE:
      fprintf(f, "Layer %s\n", lefiGeometries_getLayer(this,
						       i));
      break;
    case lefiGeomWidthE:
      fprintf(f, "Width %g\n", lefiGeometries_getWidth(this,
						       i));
      break;
    case lefiGeomPathE:
      path = lefiGeometries_getPath(this,
				    i);
      fprintf(f, "Path");
      for (l = 0;
	   l < path->numPoints;
	   l++)
	fprintf(f, " %g,%g", path->x[l], path->y[l]);
      fprintf(f, "\n");
      break;
    case lefiGeomPathIterE:
      pathiter = lefiGeometries_getPathIter(this,
					    i);
      fprintf(f, "Path iter start %g,%g step %g,%g\n", pathiter->xStart, pathiter->yStart, pathiter->xStep, pathiter->yStep);
      for (l = 0;
	   l < pathiter->numPoints;
	   l++)
	fprintf(f, " %g,%g", pathiter->x[l], pathiter->y[l]);
      fprintf(f, "\n");
      break;
    case lefiGeomRectE:
      rect = lefiGeometries_getRect(this,
				    i);
      fprintf(f, "Rect %g,%g %g,%g\n", rect->xl, rect->yl, rect->xh, rect->yh);
      break;
    case lefiGeomRectIterE:
      rectiter = lefiGeometries_getRectIter(this,
					    i);
      fprintf(f, "Rect iter start %g,%g step %g,%g\n", rectiter->xStart, rectiter->yStart, rectiter->xStep, rectiter->yStep);
      fprintf(f, " %g,%g %g,%g\n", rectiter->xl, rectiter->yl, rectiter->xh, rectiter->yh);
      break;
    case lefiGeomPolygonE:
      polygon = lefiGeometries_getPolygon(this,
					  i);
      fprintf(f, "Polygon");
      for (l = 0;
	   l < polygon->numPoints;
	   l++)
	fprintf(f, " %g,%g", polygon->x[l], polygon->y[l]);
      fprintf(f, "\n");
      break;
    case lefiGeomPolygonIterE:
      polygoniter = lefiGeometries_getPolygonIter(this,
						  i);
      fprintf(f, "Polygon iter start %g,%g step %g,%g\n", polygoniter->xStart, polygoniter->yStart, polygoniter->xStep, polygoniter->yStep);
      for (l = 0;
	   l < polygoniter->numPoints;
	   l++)
	fprintf(f, " %g,%g", polygoniter->x[l], polygoniter->y[l]);
      fprintf(f, "\n");
      break;
    case lefiGeomViaE:
      fprintf(f, "Via n");
      break;
    case lefiGeomViaIterE:
      fprintf(f, "Via iter n");
      break;
    case lefiGeomClassE:
      fprintf(f, "Classtype %s\n", this->items_[i]);
      break;
    default:
      lefiError("unknown geometry type");
      fprintf(f, "Unknown geometry type %d\n", (int) (lefiGeometries_itemType(this,
								       i)));
      break;
    }
  }
}

/*
 * lefiObstruction
 */ lefiObstruction *
lefiObstruction_CreateUninitialized()
{
  lefiObstruction *this = (lefiObstruction *) malloc(sizeof(lefiObstruction));

  lefiObstruction_Init(this);
  return (this);
}

void 
lefiObstruction_Init(lefiObstruction * this)
{
  this->geometries_ = 0;
}

void
lefiObstruction_Delete(lefiObstruction * this)
{
  if (this == NULL)
    return;
  lefiObstruction_Destroy(this);
  free(this);
}

void 
lefiObstruction_Destroy(lefiObstruction * this)
{
  lefiObstruction_clear(this);
}

void 
lefiObstruction_clear(lefiObstruction * this)
{
  if (this->geometries_) {
    lefiGeometries_Destroy(this->geometries_);
    free((char *) (this->geometries_));
  }
  this->geometries_ = 0;
}

void 
lefiObstruction_setGeometries(lefiObstruction * this,
			      lefiGeometries * g)
{
  lefiObstruction_clear(this);
  this->geometries_ = g;
}

lefiGeometries *
lefiObstruction_geometries(const lefiObstruction * this)
{
  return this->geometries_;
}

void 
lefiObstruction_print(const lefiObstruction * this,
		      FILE * f)
{
  lefiGeometries *g;

  fprintf(f, " Obstruction\n");
  g = this->geometries_;
  lefiGeometries_print(g,
		       f);
}

/*
 * lefiPin
 */ lefiPin *
lefiPin_CreateUninitialized()
{
  lefiPin *this = (lefiPin *) malloc(sizeof(lefiPin));

  lefiPin_Init(this);
  return (this);
}

void 
lefiPin_Init(lefiPin * this)
{
  this->nameSize_ = 16;
  this->name_ = (char *) malloc(16);
  this->portsAllocated_ = 2;
  this->ports_ = (lefiGeometries **) malloc(sizeof(lefiGeometries *) * 2);
  this->numPorts_ = 0;
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
  this->propNames_ = 0;
  this->propValues_ = 0;
  this->foreign_ = 0;
  this->LEQ_ = 0;
  this->mustjoin_ = 0;
  this->lowTable_ = 0;
  this->highTable_ = 0;
  this->taperRule_ = 0;
  lefiPin_bump(this,
	       &(this->foreign_), 16, &(this->foreignSize_));
  lefiPin_bump(this,
	       &(this->LEQ_), 16, &(this->LEQSize_));
  lefiPin_bump(this,
	       &(this->mustjoin_), 16, &(this->mustjoinSize_));
  lefiPin_bump(this,
	       &(this->lowTable_), 16, &(this->lowTableSize_));
  lefiPin_bump(this,
	       &(this->highTable_), 16, &(this->highTableSize_));
  this->numAntennaSize_ = 0;
  this->antennaSizeAllocated_ = 1;
  this->antennaSize_ = (double *) malloc(sizeof(double));
  this->antennaSizeLayer_ = (char **) malloc(sizeof(char *));
  this->numAntennaMetalArea_ = 0;
  this->antennaMetalAreaAllocated_ = 1;
  this->antennaMetalArea_ = (double *) malloc(sizeof(double));
  this->antennaMetalAreaLayer_ = (char **) malloc(sizeof(char *));
  this->numAntennaMetalLength_ = 0;
  this->antennaMetalLengthAllocated_ = 1;
  this->antennaMetalLength_ = (double *) malloc(sizeof(double));
  this->antennaMetalLengthLayer_ = (char **) malloc(sizeof(char *));
}

void
lefiPin_Delete(lefiPin * this)
{
  if (this == NULL)
    return;
  lefiPin_Destroy(this);
  free(this);
}

void 
lefiPin_Destroy(lefiPin * this)
{
  lefiPin_clear(this);
  free(this->name_);
  free((char *) (this->ports_));
  free(this->foreign_);
  free(this->LEQ_);
  free(this->mustjoin_);
  free(this->lowTable_);
  free(this->highTable_);
  if (this->propNames_)
    free((char *) (this->propNames_));
  if (this->propValues_)
    free((char *) (this->propValues_));
  if (this->propNums_)
    free((char *) (this->propNums_));
  free((char *) (this->antennaSize_));
  free((char *) (this->antennaSizeLayer_));
  free((char *) (this->antennaMetalArea_));
  free((char *) (this->antennaMetalAreaLayer_));
  free((char *) (this->antennaMetalLength_));
  free((char *) (this->antennaMetalLengthLayer_));
}

void 
lefiPin_clear(lefiPin * this)
{
  int     i;

  lefiGeometries *g;

  for (i = 0;
       i < this->numPorts_;
       i++) {
    g = this->ports_[i];
    lefiGeometries_Destroy(g);
    free((char *) g);
  }
  this->numPorts_ = 0;
  this->portsAllocated_ = 0;
  this->hasForeign_ = 0;
  this->hasForeignOrient_ = 0;
  this->hasForeignPoint_ = 0;
  this->hasLEQ_ = 0;
  this->hasDirection_ = 0;
  this->hasUse_ = 0;
  this->hasShape_ = 0;
  this->hasMustjoin_ = 0;
  this->hasOutMargin_ = 0;
  this->hasOutResistance_ = 0;
  this->hasInMargin_ = 0;
  this->hasPower_ = 0;
  this->hasLeakage_ = 0;
  this->hasMaxload_ = 0;
  this->hasMaxdelay_ = 0;
  this->hasCapacitance_ = 0;
  this->hasResistance_ = 0;
  this->hasPulldownres_ = 0;
  this->hasTieoffr_ = 0;
  this->hasVHI_ = 0;
  this->hasVLO_ = 0;
  this->hasRiseVoltage_ = 0;
  this->hasFallVoltage_ = 0;
  this->hasRiseThresh_ = 0;
  this->hasFallThresh_ = 0;
  this->hasRiseSatcur_ = 0;
  this->hasFallSatcur_ = 0;
  this->hasCurrentSource_ = 0;
  this->hasRiseSlewLimit_ = 0;
  this->hasFallSlewLimit_ = 0;
  this->hasTables_ = 0;
  for (i = 0;
       i < this->numAntennaSize_;
       i++)
    free(this->antennaSizeLayer_[i]);
  this->numAntennaSize_ = 0;
  for (i = 0;
       i < this->numAntennaMetalLength_;
       i++)
    free(this->antennaMetalLengthLayer_[i]);
  this->numAntennaMetalLength_ = 0;
  for (i = 0;
       i < this->numAntennaMetalArea_;
       i++)
    free(this->antennaMetalAreaLayer_[i]);
  this->numAntennaMetalArea_ = 0;
  for (i = 0;
       i < this->numProperties_;
       i++) {
    free(this->propNames_[i]);
    free(this->propValues_[i]);
  }
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
  if (this->taperRule_) {
    free(this->taperRule_);
    this->taperRule_ = 0;
  }
}

void 
lefiPin_bump(lefiPin * this,
	     char **array,
	     int len,
	     int *size)
{
  if (*array)
    free(*array);
  *array = (char *) malloc(len);
  *size = len;
}

void 
lefiPin_setName(lefiPin * this,
		const char *name)
{
  int     len = strlen(name) + 1;

  lefiPin_clear(this);
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char *) malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
}

void 
lefiPin_addPort(lefiPin * this,
		lefiGeometries * g)
{
  if (this->numPorts_ == this->portsAllocated_) {
    int     i;

    lefiGeometries **ng;

    if (this->portsAllocated_ == 0)
      this->portsAllocated_ = 2;
    else
      this->portsAllocated_ *= 2;
    ng = (lefiGeometries **) malloc(sizeof(lefiGeometries *) * this->portsAllocated_);
    for (i = 0;
	 i < this->numPorts_;
	 i++)
      ng[i] = this->ports_[i];
    free((char *) (this->ports_));
    this->ports_ = ng;
  }
  this->ports_[this->numPorts_++] = g;
}

void 
lefiPin_setForeign(lefiPin * this,
		   const char *name,
		   int hasPnt,
		   double x,
		   double y,
		   int orient)
{
  int     len = strlen(name) + 1;

  this->hasForeignPoint_ = hasPnt;
  this->foreignOrient_ = orient;
  this->foreignX_ = x;
  this->foreignY_ = y;
  this->hasForeign_ = 1;
  if (len > this->foreignSize_)
    lefiPin_bump(this,
		 &(this->foreign_), len, &(this->foreignSize_));
  strcpy(this->foreign_, CASE(name));
  if (orient != -1)
    this->hasForeignOrient_ = 1;
}

void 
lefiPin_setLEQ(lefiPin * this,
	       const char *name)
{
  int     len = strlen(name) + 1;

  if (len > this->LEQSize_)
    lefiPin_bump(this,
		 &(this->LEQ_), len, &(this->LEQSize_));
  strcpy(this->LEQ_, CASE(name));
  this->hasLEQ_ = 1;
}

void 
lefiPin_setDirection(lefiPin * this,
		     const char *name)
{
  strcpy(this->direction_, CASE(name));
  this->hasDirection_ = 1;
}

void 
lefiPin_setUse(lefiPin * this,
	       const char *name)
{
  strcpy(this->use_, CASE(name));
  this->hasUse_ = 1;
}

void 
lefiPin_setShape(lefiPin * this,
		 const char *name)
{
  strcpy(this->shape_, CASE(name));
  this->hasShape_ = 1;
}

void 
lefiPin_setMustjoin(lefiPin * this,
		    const char *name)
{
  int     len = strlen(name) + 1;

  if (len > this->mustjoinSize_)
    lefiPin_bump(this,
		 &(this->mustjoin_), len, &(this->mustjoinSize_));
  strcpy(this->mustjoin_, CASE(name));
  this->hasMustjoin_ = 1;
}

void 
lefiPin_setOutMargin(lefiPin * this,
		     double high,
		     double low)
{
  this->outMarginH_ = high;
  this->outMarginL_ = low;
  this->hasOutMargin_ = 1;
}

void 
lefiPin_setOutResistance(lefiPin * this,
			 double high,
			 double low)
{
  this->outResistanceH_ = high;
  this->outResistanceL_ = low;
  this->hasOutResistance_ = 1;
}

void 
lefiPin_setInMargin(lefiPin * this,
		    double high,
		    double low)
{
  this->inMarginH_ = high;
  this->inMarginL_ = low;
  this->hasInMargin_ = 1;
}

void 
lefiPin_setPower(lefiPin * this,
		 double power)
{
  this->power_ = power;
  this->hasPower_ = 1;
}

void 
lefiPin_setLeakage(lefiPin * this,
		   double current)
{
  this->leakage_ = current;
  this->hasLeakage_ = 1;
}

void 
lefiPin_setMaxload(lefiPin * this,
		   double capacitance)
{
  this->maxload_ = capacitance;
  this->hasMaxload_ = 1;
}

void 
lefiPin_setMaxdelay(lefiPin * this,
		    double dtime)
{
  this->maxdelay_ = dtime;
  this->hasMaxdelay_ = 1;
}

void 
lefiPin_setCapacitance(lefiPin * this,
		       double capacitance)
{
  this->capacitance_ = capacitance;
  this->hasCapacitance_ = 1;
}

void 
lefiPin_setResistance(lefiPin * this,
		      double resistance)
{
  this->resistance_ = resistance;
  this->hasResistance_ = 1;
}

void 
lefiPin_setPulldownres(lefiPin * this,
		       double resistance)
{
  this->pulldownres_ = resistance;
  this->hasPulldownres_ = 1;
}

void 
lefiPin_setTieoffr(lefiPin * this,
		   double resistance)
{
  this->tieoffr_ = resistance;
  this->hasTieoffr_ = 1;
}

void 
lefiPin_setVHI(lefiPin * this,
	       double voltage)
{
  this->VHI_ = voltage;
  this->hasVHI_ = 1;
}

void 
lefiPin_setVLO(lefiPin * this,
	       double voltage)
{
  this->VLO_ = voltage;
  this->hasVLO_ = 1;
}

void 
lefiPin_setRiseVoltage(lefiPin * this,
		       double voltage)
{
  this->riseVoltage_ = voltage;
  this->hasRiseVoltage_ = 1;
}

void 
lefiPin_setFallVoltage(lefiPin * this,
		       double voltage)
{
  this->fallVoltage_ = voltage;
  this->hasFallVoltage_ = 1;
}

void 
lefiPin_setFallSlewLimit(lefiPin * this,
			 double num)
{
  this->riseSlewLimit_ = num;
  this->hasFallSlewLimit_ = 1;
}

void 
lefiPin_setRiseSlewLimit(lefiPin * this,
			 double num)
{
  this->riseSlewLimit_ = num;
  this->hasRiseSlewLimit_ = 1;
}

void 
lefiPin_setRiseThresh(lefiPin * this,
		      double capacitance)
{
  this->riseThresh_ = capacitance;
  this->hasRiseThresh_ = 1;
}

void 
lefiPin_setTaperRule(lefiPin * this,
		     const char *name)
{
  int     len = strlen(name) + 1;

  this->taperRule_ = (char *) malloc(len);
  strcpy(this->taperRule_, name);
}

void 
lefiPin_setFallThresh(lefiPin * this,
		      double capacitance)
{
  this->fallThresh_ = capacitance;
  this->hasFallThresh_ = 1;
}

void 
lefiPin_setRiseSatcur(lefiPin * this,
		      double current)
{
  this->riseSatcur_ = current;
  this->hasRiseSatcur_ = 1;
}

void 
lefiPin_setFallSatcur(lefiPin * this,
		      double current)
{
  this->fallSatcur_ = current;
  this->hasFallSatcur_ = 1;
}

void 
lefiPin_setCurrentSource(lefiPin * this,
			 const char *name)
{
  strcpy(this->currentSource_, CASE(name));
  this->hasCurrentSource_ = 1;
}

void 
lefiPin_setTables(lefiPin * this,
		  const char *highName,
		  const char *lowName)
{
  int     len = strlen(highName) + 1;

  if (len > this->highTableSize_)
    lefiPin_bump(this,
		 &(this->highTable_), len, &(this->highTableSize_));
  strcpy(this->highTable_, CASE(highName));
  len = strlen(lowName) + 1;
  if (len > this->lowTableSize_)
    lefiPin_bump(this,
		 &(this->lowTable_), len, &(this->lowTableSize_));
  strcpy(this->lowTable_, CASE(lowName));
  this->hasTables_ = 1;
}

void 
lefiPin_setProperty(lefiPin * this,
		    const char *name,
		    const char *value)
{
  int     len;

  if (this->numProperties_ == this->propertiesAllocated_)
    lefiPin_bumpProps(this);
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char *) malloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProperties_] = (char *) malloc(len);
  strcpy(this->propValues_[this->numProperties_], CASE(value));
  this->numProperties_ += 1;
}

void 
lefiPin_setNumProperty(lefiPin * this,
		       const char *name,
		       double value)
{
  int     len;

  if (this->numProperties_ == this->propertiesAllocated_)
    lefiPin_bumpProps(this);
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char *) malloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  this->propValues_[this->numProperties_] = 0;
  this->propNums_[this->numProperties_] = value;
  this->numProperties_ += 1;
}

void 
lefiPin_bumpProps(lefiPin * this)
{
  int     lim = this->propertiesAllocated_;

  int     news;

  char  **newpn;

  char  **newpv;

  double *newd;

  news = lim ? lim + lim : 2;
  newpn = (char **) malloc(sizeof(char *) * news);
  newpv = (char **) malloc(sizeof(char *) * news);
  newd = (double *) malloc(sizeof(double) * news);
  lim = this->propertiesAllocated_ = news;
  if (lim > 2) {
    int     i;

    for (i = 0;
	 i < this->numProperties_;
	 i++) {
      newpn[i] = this->propNames_[i];
      newpv[i] = this->propValues_[i];
      newd[i] = this->propNums_[i];
    }
    free((char *) (this->propNames_));
    free((char *) (this->propValues_));
    free((char *) (this->propNums_));
  }
  this->propNames_ = newpn;
  this->propValues_ = newpv;
  this->propNums_ = newd;
}

int 
lefiPin_hasForeign(const lefiPin * this)
{
  return this->hasForeign_;
}

int 
lefiPin_hasForeignOrient(const lefiPin * this)
{
  return this->hasForeignOrient_;
}

int 
lefiPin_hasForeignPoint(const lefiPin * this)
{
  return this->hasForeignPoint_;
}

int 
lefiPin_hasLEQ(const lefiPin * this)
{
  return this->hasLEQ_;
}

int 
lefiPin_hasDirection(const lefiPin * this)
{
  return this->hasDirection_;
}

int 
lefiPin_hasUse(const lefiPin * this)
{
  return this->hasUse_;
}

int 
lefiPin_hasShape(const lefiPin * this)
{
  return this->hasShape_;
}

int 
lefiPin_hasMustjoin(const lefiPin * this)
{
  return this->hasMustjoin_;
}

int 
lefiPin_hasOutMargin(const lefiPin * this)
{
  return this->hasOutMargin_;
}

int 
lefiPin_hasOutResistance(const lefiPin * this)
{
  return this->hasOutResistance_;
}

int 
lefiPin_hasInMargin(const lefiPin * this)
{
  return this->hasInMargin_;
}

int 
lefiPin_hasPower(const lefiPin * this)
{
  return this->hasPower_;
}

int 
lefiPin_hasLeakage(const lefiPin * this)
{
  return this->hasLeakage_;
}

int 
lefiPin_hasMaxload(const lefiPin * this)
{
  return this->hasMaxload_;
}

int 
lefiPin_hasMaxdelay(const lefiPin * this)
{
  return this->hasMaxdelay_;
}

int 
lefiPin_hasCapacitance(const lefiPin * this)
{
  return this->hasCapacitance_;
}

int 
lefiPin_hasResistance(const lefiPin * this)
{
  return this->hasResistance_;
}

int 
lefiPin_hasPulldownres(const lefiPin * this)
{
  return this->hasPulldownres_;
}

int 
lefiPin_hasTieoffr(const lefiPin * this)
{
  return this->hasTieoffr_;
}

int 
lefiPin_hasVHI(const lefiPin * this)
{
  return this->hasVHI_;
}

int 
lefiPin_hasVLO(const lefiPin * this)
{
  return this->hasVLO_;
}

int 
lefiPin_hasFallSlewLimit(const lefiPin * this)
{
  return this->hasFallSlewLimit_;
}

int 
lefiPin_hasRiseSlewLimit(const lefiPin * this)
{
  return this->hasRiseSlewLimit_;
}

int 
lefiPin_hasRiseVoltage(const lefiPin * this)
{
  return this->hasRiseVoltage_;
}

int 
lefiPin_hasFallVoltage(const lefiPin * this)
{
  return this->hasFallVoltage_;
}

int 
lefiPin_hasRiseThresh(const lefiPin * this)
{
  return this->hasRiseThresh_;
}

int 
lefiPin_hasFallThresh(const lefiPin * this)
{
  return this->hasFallThresh_;
}

int 
lefiPin_hasRiseSatcur(const lefiPin * this)
{
  return this->hasRiseSatcur_;
}

int 
lefiPin_hasFallSatcur(const lefiPin * this)
{
  return this->hasFallSatcur_;
}

int 
lefiPin_hasCurrentSource(const lefiPin * this)
{
  return this->hasCurrentSource_;
}

int 
lefiPin_hasTables(const lefiPin * this)
{
  return this->hasTables_;
}

int 
lefiPin_hasAntennaSize(const lefiPin * this)
{
  return this->numAntennaSize_ ? 1 : 0;
}

int 
lefiPin_hasAntennaMetalLength(const lefiPin * this)
{
  return this->numAntennaMetalLength_ ? 1 : 0;
}

int 
lefiPin_hasAntennaMetalArea(const lefiPin * this)
{
  return this->numAntennaMetalArea_ ? 1 : 0;
}

int 
lefiPin_hasTaperRule(const lefiPin * this)
{
  return this->taperRule_ ? 1 : 0;
}

const char *
lefiPin_name(const lefiPin * this)
{
  return this->name_;
}

const char *
lefiPin_taperRule(const lefiPin * this)
{
  return this->taperRule_;
}

int 
lefiPin_numPorts(const lefiPin * this)
{
  return this->numPorts_;
}

lefiGeometries *
lefiPin_port(const lefiPin * this,
	     int index)
{
  if (index < 0 || index > this->numPorts_) {
    lefiError("bad pin number index");
    return 0;
  }
  return this->ports_[index];
}

const char *
lefiPin_foreignName(const lefiPin * this)
{
  return this->foreign_;
}

int 
lefiPin_foreignOrient(const lefiPin * this)
{
  return this->foreignOrient_;
}

double 
lefiPin_foreignX(const lefiPin * this)
{
  return this->foreignX_;
}

double 
lefiPin_foreignY(const lefiPin * this)
{
  return this->foreignY_;
}

const char *
lefiPin_LEQ(const lefiPin * this)
{
  return this->LEQ_;
}

const char *
lefiPin_direction(const lefiPin * this)
{
  return this->direction_;
}

const char *
lefiPin_use(const lefiPin * this)
{
  return this->use_;
}

const char *
lefiPin_shape(const lefiPin * this)
{
  return this->shape_;
}

const char *
lefiPin_mustjoin(const lefiPin * this)
{
  return this->mustjoin_;
}

double 
lefiPin_outMarginHigh(const lefiPin * this)
{
  return this->outMarginH_;
}

double 
lefiPin_outMarginLow(const lefiPin * this)
{
  return this->outMarginL_;
}

double 
lefiPin_outResistanceHigh(const lefiPin * this)
{
  return this->outResistanceH_;
}

double 
lefiPin_outResistanceLow(const lefiPin * this)
{
  return this->outResistanceL_;
}

double 
lefiPin_inMarginHigh(const lefiPin * this)
{
  return this->inMarginH_;
}

double 
lefiPin_inMarginLow(const lefiPin * this)
{
  return this->inMarginL_;
}

double 
lefiPin_power(const lefiPin * this)
{
  return this->power_;
}

double 
lefiPin_leakage(const lefiPin * this)
{
  return this->leakage_;
}

double 
lefiPin_maxload(const lefiPin * this)
{
  return this->maxload_;
}

double 
lefiPin_maxdelay(const lefiPin * this)
{
  return this->maxdelay_;
}

double 
lefiPin_capacitance(const lefiPin * this)
{
  return this->capacitance_;
}

double 
lefiPin_resistance(const lefiPin * this)
{
  return this->resistance_;
}

double 
lefiPin_pulldownres(const lefiPin * this)
{
  return this->pulldownres_;
}

double 
lefiPin_tieoffr(const lefiPin * this)
{
  return this->tieoffr_;
}

double 
lefiPin_VHI(const lefiPin * this)
{
  return this->VHI_;
}

double 
lefiPin_VLO(const lefiPin * this)
{
  return this->VLO_;
}

double 
lefiPin_fallSlewLimit(const lefiPin * this)
{
  return this->fallSlewLimit_;
}

double 
lefiPin_riseSlewLimit(const lefiPin * this)
{
  return this->riseSlewLimit_;
}

double 
lefiPin_riseVoltage(const lefiPin * this)
{
  return this->riseVoltage_;
}

double 
lefiPin_fallVoltage(const lefiPin * this)
{
  return this->fallVoltage_;
}

double 
lefiPin_riseThresh(const lefiPin * this)
{
  return this->riseThresh_;
}

double 
lefiPin_fallThresh(const lefiPin * this)
{
  return this->fallThresh_;
}

double 
lefiPin_riseSatcur(const lefiPin * this)
{
  return this->riseSatcur_;
}

double 
lefiPin_fallSatcur(const lefiPin * this)
{
  return this->fallSatcur_;
}

const char *
lefiPin_currentSource(const lefiPin * this)
{
  return this->currentSource_;
}

const char *
lefiPin_tableHighName(const lefiPin * this)
{
  return this->highTable_;
}

const char *
lefiPin_tableLowName(const lefiPin * this)
{
  return this->lowTable_;
}

const char *
lefiPin_antennaSizeLayer(const lefiPin * this,
			 int i)
{
  return this->antennaSizeLayer_[i];
}

const char *
lefiPin_antennaMetalAreaLayer(const lefiPin * this,
			      int i)
{
  return this->antennaMetalAreaLayer_[i];
}

const char *
lefiPin_antennaMetalLengthLayer(const lefiPin * this,
				int i)
{
  return this->antennaMetalLengthLayer_[i];
}

int 
lefiPin_numAntennaSize(const lefiPin * this)
{
  return this->numAntennaSize_;
}

int 
lefiPin_numAntennaMetalArea(const lefiPin * this)
{
  return this->numAntennaMetalArea_;
}

int 
lefiPin_numAntennaMetalLength(const lefiPin * this)
{
  return this->numAntennaMetalLength_;
}

double 
lefiPin_antennaSize(const lefiPin * this,
		    int i)
{
  return this->antennaSize_[i];
}

double 
lefiPin_antennaMetalArea(const lefiPin * this,
			 int i)
{
  return this->antennaMetalArea_[i];
}

double 
lefiPin_antennaMetalLength(const lefiPin * this,
			   int i)
{
  return this->antennaMetalLength_[i];
}

void 
lefiPin_addAntennaMetalLength(lefiPin * this,
			      double val,
			      const char *layer)
{
  int     len;

  if (this->numAntennaMetalLength_ == this->antennaMetalLengthAllocated_) {
    int     i;

    int     max;

    int     lim = this->numAntennaMetalLength_;

    double *nd;

    char  **nl;

    max = this->antennaMetalLengthAllocated_ *= 2;
    nd = (double *) malloc(sizeof(double) * max);
    nl = (char **) malloc(sizeof(double) * max);
    for (i = 0;
	 i < lim;
	 i++) {
      nl[i] = this->antennaMetalLengthLayer_[i];
      nd[i] = this->antennaMetalLength_[i];
    }
    free((char *) (this->antennaMetalLengthLayer_));
    free((char *) (this->antennaMetalLength_));
    this->antennaMetalLengthLayer_ = nl;
    this->antennaMetalLength_ = nd;
  }
  this->antennaMetalLength_[this->numAntennaMetalLength_] = val;
  if (layer) {
    len = strlen(layer) + 1;
    this->antennaMetalLengthLayer_[this->numAntennaMetalLength_] = (char *) malloc(len);
    strcpy(this->antennaMetalLengthLayer_[this->numAntennaMetalLength_], layer);
  }
  else
    this->antennaSizeLayer_[this->numAntennaSize_] = NULL;
  this->numAntennaMetalLength_ += 1;
}

void 
lefiPin_addAntennaSize(lefiPin * this,
		       double val,
		       const char *layer)
{
  int     len;

  if (this->numAntennaSize_ == this->antennaSizeAllocated_) {
    int     i;

    int     max;

    int     lim = this->numAntennaSize_;

    double *nd;

    char  **nl;

    max = this->antennaSizeAllocated_ *= 2;
    nd = (double *) malloc(sizeof(double) * max);
    nl = (char **) malloc(sizeof(double) * max);
    for (i = 0;
	 i < lim;
	 i++) {
      nl[i] = this->antennaSizeLayer_[i];
      nd[i] = this->antennaSize_[i];
    }
    free((char *) (this->antennaSizeLayer_));
    free((char *) (this->antennaSize_));
    this->antennaSizeLayer_ = nl;
    this->antennaSize_ = nd;
  }
  this->antennaSize_[this->numAntennaSize_] = val;
  if (layer) {
    len = strlen(layer) + 1;
    this->antennaSizeLayer_[this->numAntennaSize_] = (char *) malloc(len);
    strcpy(this->antennaSizeLayer_[this->numAntennaSize_], layer);
  }
  else
    this->antennaSizeLayer_[this->numAntennaSize_] = NULL;
  this->numAntennaSize_ += 1;
}

void 
lefiPin_addAntennaMetalArea(lefiPin * this,
			    double val,
			    const char *layer)
{
  int     len;

  if (this->numAntennaMetalArea_ == this->antennaMetalAreaAllocated_) {
    int     i;

    int     max;

    int     lim = this->numAntennaMetalArea_;

    double *nd;

    char  **nl;

    max = this->antennaMetalAreaAllocated_ *= 2;
    nd = (double *) malloc(sizeof(double) * max);
    nl = (char **) malloc(sizeof(double) * max);
    for (i = 0;
	 i < lim;
	 i++) {
      nl[i] = this->antennaMetalAreaLayer_[i];
      nd[i] = this->antennaMetalArea_[i];
    }
    free((char *) (this->antennaMetalAreaLayer_));
    free((char *) (this->antennaMetalArea_));
    this->antennaMetalAreaLayer_ = nl;
    this->antennaMetalArea_ = nd;
  }
  this->antennaMetalArea_[this->numAntennaMetalArea_] = val;
  if (layer) {
    len = strlen(layer) + 1;
    this->antennaMetalAreaLayer_[this->numAntennaMetalArea_] = (char *) malloc(len);
    strcpy(this->antennaMetalAreaLayer_[this->numAntennaMetalArea_], layer);
  }
  else
    this->antennaSizeLayer_[this->numAntennaSize_] = NULL;
  this->numAntennaMetalArea_ += 1;
}

int 
lefiPin_numProperties(const lefiPin * this)
{
  return this->numProperties_;
}

const char *
lefiPin_propName(const lefiPin * this,
		 int index)
{
  if (index < 0 || index >= this->numProperties_) {
    lefiError("ERROR: bad property number index");
    return 0;
  }
  return this->propNames_[index];
}

const char *
lefiPin_propValue(const lefiPin * this,
		  int index)
{
  if (index < 0 || index >= this->numProperties_) {
    lefiError("ERROR: bad property number index");
    return 0;
  }
  return this->propValues_[index];
}

double 
lefiPin_propNum(const lefiPin * this,
		int index)
{
  if (index < 0 || index >= this->numProperties_) {
    lefiError("ERROR: bad property number index");
    return 0;
  }
  return this->propNums_[index];
}

void 
lefiPin_print(const lefiPin * this,
	      FILE * f)
{
  int     i;

  lefiGeometries *g;

  fprintf(f, " Pin %s\n", lefiPin_name(this));
  for (i = 0;
       i < lefiPin_numPorts(this);
       i++) {
    fprintf(f, " Port %d ", i);
    g = lefiPin_port(this,
		     i);
    lefiGeometries_print(g,
			 f);
  }
}

/*
 * lefiMacro
 */ lefiMacro *
lefiMacro_CreateUninitialized()
{
  lefiMacro *this = (lefiMacro *) malloc(sizeof(lefiMacro));

  lefiMacro_Init(this);
  return (this);
}

void 
lefiMacro_Init(lefiMacro * this)
{
  this->name_ = 0;
  this->generator_ = 0;
  this->EEQ_ = 0;
  this->LEQ_ = 0;
  this->gen1_ = 0;
  this->gen2_ = 0;
  this->foreign_ = 0;
  this->siteName_ = 0;
  this->clockType_ = 0;
  this->propNames_ = 0;
  this->propValues_ = 0;
  lefiMacro_bump(this,
		 &(this->name_), 16, &(this->nameSize_));
  lefiMacro_bump(this,
		 &(this->generator_), 16, &(this->generatorSize_));
  lefiMacro_bump(this,
		 &(this->EEQ_), 16, &(this->EEQSize_));
  lefiMacro_bump(this,
		 &(this->LEQ_), 16, &(this->LEQSize_));
  lefiMacro_bump(this,
		 &(this->gen1_), 16, &(this->gen1Size_));
  lefiMacro_bump(this,
		 &(this->gen2_), 16, &(this->gen2Size_));
  lefiMacro_bump(this,
		 &(this->foreign_), 16, &(this->foreignSize_));
  lefiMacro_bump(this,
		 &(this->siteName_), 16, &(this->siteNameSize_));
  lefiMacro_bump(this,
		 &(this->clockType_), 16, &(this->clockTypeSize_));
  this->propertiesAllocated_ = 2;
  this->numProperties_ = 0;
  this->propNames_ = (char **) malloc(sizeof(char *) * 2);
  this->propValues_ = (char **) malloc(sizeof(char *) * 2);
  this->propNums_ = (double *) malloc(sizeof(double) * 2);
  this->numSites_ = 0;
  this->sitesAllocated_ = 0;
  this->pattern_ = 0;
}

void 
lefiMacro_Destroy(lefiMacro * this)
{
  lefiMacro_clear(this);
  free(this->name_);
  free(this->generator_);
  free(this->EEQ_);
  free(this->LEQ_);
  free(this->gen1_);
  free(this->gen2_);
  free(this->clockType_);
  free((char *) (this->propNames_));
  free((char *) (this->propValues_));
  free((char *) (this->propNums_));
  if (this->pattern_)
    free((char *) (this->pattern_));
}

void
lefiMacro_Delete(lefiMacro * this)
{
  if (this == NULL)
    return;
  lefiMacro_Destroy(this);
  free(this);
}

void 
lefiMacro_clear(lefiMacro * this)
{
  int     i;

  this->hasClass_ = 0;
  this->hasGenerator_ = 0;
  this->hasGenerate_ = 0;
  this->hasPower_ = 0;
  this->hasOrigin_ = 0;
  this->hasSource_ = 0;
  this->hasEEQ_ = 0;
  this->hasLEQ_ = 0;
  this->hasSymmetry_ = 0;
  this->hasSiteName_ = 0;
  this->hasClockType_ = 0;
  this->hasSize_ = 0;
  this->foreignOrient_ = -1;
  this->hasForeign_ = 0;
  this->hasForeignOrigin_ = 0;
  this->hasForeignPoint_ = 0;
  this->isInverter_ = 0;
  this->isBuffer_ = 0;
  if (this->pattern_) {
    for (i = 0;
	 i < this->numSites_;
	 i++) {
      lefiSitePattern_Destroy(this->pattern_[i]);
      free((char *) (this->pattern_[i]));
    }
    this->pattern_ = 0;
    this->numSites_ = 0;
    this->sitesAllocated_ = 0;
  }
  for (i = 0;
       i < this->numProperties_;
       i++) {
    free(this->propNames_[i]);
    free(this->propValues_[i]);
  }
  this->numProperties_ = 0;
}

void 
lefiMacro_bump(lefiMacro * this,
	       char **array,
	       int len,
	       int *size)
{
  if (*array)
    free(*array);
  *array = (char *) malloc(len);
  *size = len;
}

void 
lefiMacro_setName(lefiMacro * this,
		  const char *name)
{
  int     len = strlen(name) + 1;

  if (len > this->nameSize_)
    lefiMacro_bump(this,
		   &(this->name_), len, &(this->nameSize_));
  strcpy(this->name_, CASE(name));
}

void 
lefiMacro_setGenerate(lefiMacro * this,
		      const char *name,
		      const char *n2)
{
  int     len = strlen(name) + 1;

  if (len > this->gen1Size_)
    lefiMacro_bump(this,
		   &(this->gen1_), len, &(this->gen1Size_));
  strcpy(this->gen1_, CASE(name));
  len = strlen(n2) + 1;
  if (len > this->gen2Size_)
    lefiMacro_bump(this,
		   &(this->gen2_), len, &(this->gen2Size_));
  strcpy(this->gen2_, n2);
}

void 
lefiMacro_setGenerator(lefiMacro * this,
		       const char *name)
{
  int     len = strlen(name) + 1;

  if (len > this->generatorSize_)
    lefiMacro_bump(this,
		   &(this->generator_), len, &(this->generatorSize_));
  strcpy(this->generator_, CASE(name));
  this->hasGenerator_ = 1;
}

void 
lefiMacro_setInverter(lefiMacro * this)
{
  this->isInverter_ = 1;
}

void 
lefiMacro_setBuffer(lefiMacro * this)
{
  this->isBuffer_ = 1;
}

void 
lefiMacro_setSource(lefiMacro * this,
		    const char *name)
{
  strcpy(this->source_, CASE(name));
  this->hasSource_ = 1;
}

void 
lefiMacro_setClass(lefiMacro * this,
		   const char *name)
{
  strcpy(this->macroClass_, CASE(name));
  this->hasClass_ = 1;
}

void 
lefiMacro_setOrigin(lefiMacro * this,
		    double x,
		    double y)
{
  this->originX_ = x;
  this->originY_ = y;
  this->hasOrigin_ = 1;
}

void 
lefiMacro_setPower(lefiMacro * this,
		   double p)
{
  this->power_ = p;
  this->hasPower_ = 1;
}

void 
lefiMacro_setEEQ(lefiMacro * this,
		 const char *name)
{
  int     len = strlen(name) + 1;

  if (len > this->EEQSize_)
    lefiMacro_bump(this,
		   &(this->EEQ_), len, &(this->EEQSize_));
  strcpy(this->EEQ_, CASE(name));
  this->hasEEQ_ = 1;
}

void 
lefiMacro_setLEQ(lefiMacro * this,
		 const char *name)
{
  int     len = strlen(name) + 1;

  if (len > this->LEQSize_)
    lefiMacro_bump(this,
		   &(this->LEQ_), len, &(this->LEQSize_));
  strcpy(this->LEQ_, CASE(name));
  this->hasLEQ_ = 1;
}

void 
lefiMacro_setProperty(lefiMacro * this,
		      const char *name,
		      const char *value)
{
  int     len;

  if (this->numProperties_ == this->propertiesAllocated_)
    lefiMacro_bumpProps(this);
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char *) malloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProperties_] = (char *) malloc(len);
  strcpy(this->propValues_[this->numProperties_], CASE(value));
  this->propNums_[this->numProperties_] = 0.0;
  this->numProperties_ += 1;
}

void 
lefiMacro_setNumProperty(lefiMacro * this,
			 const char *name,
			 double value)
{
  int     len;

  if (this->numProperties_ == this->propertiesAllocated_)
    lefiMacro_bumpProps(this);
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char *) malloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  this->propValues_[this->numProperties_] = 0;
  this->propNums_[this->numProperties_] = value;
  this->numProperties_ += 1;
}

void 
lefiMacro_bumpProps(lefiMacro * this)
{
  int     lim = this->propertiesAllocated_;

  int     news = lim + lim;

  char  **newpn = (char **) malloc(sizeof(char *) * news);

  char  **newpv = (char **) malloc(sizeof(char *) * news);

  double *newd = (double *) malloc(sizeof(double) * news);

  int     i;

  this->propertiesAllocated_ = news;
  for (i = 0;
       i < lim;
       i++) {
    newpn[i] = this->propNames_[i];
    newpv[i] = this->propValues_[i];
    newd[i] = this->propNums_[i];
  }
  free((char *) (this->propNames_));
  free((char *) (this->propValues_));
  free((char *) (this->propNums_));
  this->propNames_ = newpn;
  this->propValues_ = newpv;
  this->propNums_ = newd;
}

void 
lefiMacro_setForeign(lefiMacro * this,
		     const char *name,
		     int hasPnt,
		     double x,
		     double y,
		     int orient)
{
  int     len = strlen(name) + 1;

  this->hasForeignPoint_ = hasPnt;
  this->foreignOrient_ = orient;
  this->foreignX_ = x;
  this->foreignY_ = y;
  this->hasForeign_ = 1;
  if (len > this->foreignSize_)
    lefiMacro_bump(this,
		   &(this->foreign_), len, &(this->foreignSize_));
  strcpy(this->foreign_, CASE(name));
}

void 
lefiMacro_setXSymmetry(lefiMacro * this)
{
  this->hasSymmetry_ |= 1;
}

void 
lefiMacro_setYSymmetry(lefiMacro * this)
{
  this->hasSymmetry_ |= 2;
}

void 
lefiMacro_set90Symmetry(lefiMacro * this)
{
  this->hasSymmetry_ |= 4;
}

void 
lefiMacro_setSiteName(lefiMacro * this,
		      const char *name)
{
  int     len = strlen(name) + 1;

  if (len > this->siteNameSize_)
    lefiMacro_bump(this,
		   &(this->siteName_), len, &(this->siteNameSize_));
  strcpy(this->siteName_, CASE(name));
  this->hasSiteName_ = 1;
}

void 
lefiMacro_setClockType(lefiMacro * this,
		       const char *name)
{
  int     len = strlen(name) + 1;

  if (len > this->clockTypeSize_)
    lefiMacro_bump(this,
		   &(this->clockType_), len, &(this->clockTypeSize_));
  strcpy(this->clockType_, CASE(name));
  this->hasClockType_ = 1;
}

void 
lefiMacro_setSitePattern(lefiMacro * this,
			 lefiSitePattern * p)
{
  if (this->numSites_ == this->sitesAllocated_) {
    lefiSitePattern **np;

    int     i, lim;

    if (this->sitesAllocated_ == 0) {
      lim = this->sitesAllocated_ = 4;
      np = (lefiSitePattern **) malloc(sizeof(lefiSitePattern *) * lim);
    }
    else {
      lim = this->sitesAllocated_ * 2;
      this->sitesAllocated_ = lim;
      np = (lefiSitePattern **) malloc(sizeof(lefiSitePattern *) * lim);
      lim /= 2;
      for (i = 0;
	   i < lim;
	   i++)
	np[i] = this->pattern_[i];
      free((char *) (this->pattern_));
    }
    this->pattern_ = np;
  }
  this->pattern_[this->numSites_] = p;
  this->numSites_ += 1;
}

void 
lefiMacro_setSize(lefiMacro * this,
		  double x,
		  double y)
{
  this->hasSize_ = 1;
  this->sizeX_ = x;
  this->sizeY_ = y;
}

int 
lefiMacro_hasClass(const lefiMacro * this)
{
  return this->hasClass_;
}

int 
lefiMacro_hasSiteName(const lefiMacro * this)
{
  return this->hasSiteName_;
}

int 
lefiMacro_hasGenerator(const lefiMacro * this)
{
  return this->hasGenerator_;
}

int 
lefiMacro_hasPower(const lefiMacro * this)
{
  return this->hasPower_;
}

int 
lefiMacro_hasOrigin(const lefiMacro * this)
{
  return this->hasOrigin_;
}

int 
lefiMacro_hasSource(const lefiMacro * this)
{
  return this->hasSource_;
}

int 
lefiMacro_hasEEQ(const lefiMacro * this)
{
  return this->hasEEQ_;
}

int 
lefiMacro_hasLEQ(const lefiMacro * this)
{
  return this->hasLEQ_;
}

int 
lefiMacro_hasXSymmetry(const lefiMacro * this)
{
  return (this->hasSymmetry_ & 1) ? 1 : 0;
}

int 
lefiMacro_hasYSymmetry(const lefiMacro * this)
{
  return (this->hasSymmetry_ & 2) ? 1 : 0;
}

int 
lefiMacro_has90Symmetry(const lefiMacro * this)
{
  return (this->hasSymmetry_ & 4) ? 1 : 0;
}

int 
lefiMacro_hasSitePattern(const lefiMacro * this)
{
  return (this->pattern_) ? 1 : 0;
}

int 
lefiMacro_hasSize(const lefiMacro * this)
{
  return this->hasSize_;
}

int 
lefiMacro_hasForeign(const lefiMacro * this)
{
  return this->hasForeign_;
}

int 
lefiMacro_hasForeignOrigin(const lefiMacro * this)
{
  return this->hasForeignOrigin_;
}

int 
lefiMacro_hasForeignOrient(const lefiMacro * this)
{
  return (this->foreignOrient_ == -1) ? 0 : 1;
}

int 
lefiMacro_hasForeignPoint(const lefiMacro * this)
{
  return this->hasForeignPoint_;
}

int 
lefiMacro_hasClockType(const lefiMacro * this)
{
  return this->hasClockType_;
}

int 
lefiMacro_numSitePattern(const lefiMacro * this)
{
  return this->numSites_;
}

int 
lefiMacro_numProperties(const lefiMacro * this)
{
  return this->numProperties_;
}

const char *
lefiMacro_propName(const lefiMacro * this,
		   int index)
{
  if (index < 0 || index >= this->numProperties_) {
    lefiError("ERROR: bad property number index");
    return 0;
  }
  return this->propNames_[index];
}

const char *
lefiMacro_propValue(const lefiMacro * this,
		    int index)
{
  if (index < 0 || index >= this->numProperties_) {
    lefiError("ERROR: bad property number index");
    return 0;
  }
  return this->propValues_[index];
}

double 
lefiMacro_propNum(const lefiMacro * this,
		  int index)
{
  if (index < 0 || index >= this->numProperties_) {
    lefiError("ERROR: bad property number index");
    return 0;
  }
  return this->propNums_[index];
}

const char *
lefiMacro_name(const lefiMacro * this)
{
  return this->name_;
}

const char *
lefiMacro_macroClass(const lefiMacro * this)
{
  return this->macroClass_;
}

const char *
lefiMacro_generator(const lefiMacro * this)
{
  return this->generator_;
}

const char *
lefiMacro_EEQ(const lefiMacro * this)
{
  return this->EEQ_;
}

const char *
lefiMacro_LEQ(const lefiMacro * this)
{
  return this->LEQ_;
}

const char *
lefiMacro_source(const lefiMacro * this)
{
  return this->source_;
}

double 
lefiMacro_originX(const lefiMacro * this)
{
  return this->originX_;
}

double 
lefiMacro_originY(const lefiMacro * this)
{
  return this->originY_;
}

double 
lefiMacro_power(const lefiMacro * this)
{
  return this->power_;
}

void 
lefiMacro_generate(const lefiMacro * this,
		   char **name1,
		   char **name2)
{
  if (name1)
    *name1 = this->gen1_;
  if (name2)
    *name2 = this->gen2_;
}

lefiSitePattern *
lefiMacro_sitePattern(const lefiMacro * this,
		      int index)
{
  return this->pattern_[index];
}

const char *
lefiMacro_siteName(const lefiMacro * this)
{
  return this->siteName_;
}

double 
lefiMacro_sizeX(const lefiMacro * this)
{
  return this->sizeX_;
}

double 
lefiMacro_sizeY(const lefiMacro * this)
{
  return this->sizeY_;
}

int 
lefiMacro_foreignOrient(const lefiMacro * this)
{
  return this->foreignOrient_;
}

double 
lefiMacro_foreignX(const lefiMacro * this)
{
  return this->foreignX_;
}

double 
lefiMacro_foreignY(const lefiMacro * this)
{
  return this->foreignY_;
}

const char *
lefiMacro_foreignName(const lefiMacro * this)
{
  return this->foreign_;
}

const char *
lefiMacro_clockType(const lefiMacro * this)
{
  return this->clockType_;
}

int 
lefiMacro_isBuffer(const lefiMacro * this)
{
  return this->isBuffer_;
}

int 
lefiMacro_isInverter(const lefiMacro * this)
{
  return this->isInverter_;
}

void 
lefiMacro_print(const lefiMacro * this,
		FILE * f)
{
  char   *c1;

  char   *c2;

  lefiSitePattern *sp;

  int     i;

  fprintf(f, "MACRO %s\n", lefiMacro_name(this));
  if (lefiMacro_hasClass(this))
    fprintf(f, " Class %s\n", lefiMacro_macroClass(this));
  if (lefiMacro_hasGenerator(this))
    fprintf(f, " Generator %s\n", lefiMacro_generator(this));
  if (lefiMacro_hasGenerator(this)) {
    lefiMacro_generate(this,
		       &c1,
		       &c2);
    fprintf(f, " Generate %s %s\n", c1, c2);
  }
  if (lefiMacro_hasPower(this))
    fprintf(f, " Power %g\n", lefiMacro_power(this));
  if (lefiMacro_hasOrigin(this))
    fprintf(f, " Origin %g,%g\n", lefiMacro_originX(this), lefiMacro_originY(this));
  if (lefiMacro_hasEEQ(this))
    fprintf(f, " EEQ %s\n", lefiMacro_EEQ(this));
  if (lefiMacro_hasLEQ(this))
    fprintf(f, " LEQ %s\n", lefiMacro_LEQ(this));
  if (lefiMacro_hasSource(this))
    fprintf(f, " Source %s\n", lefiMacro_source(this));
  if (lefiMacro_hasXSymmetry(this))
    fprintf(f, " Symmetry X\n");
  if (lefiMacro_hasYSymmetry(this))
    fprintf(f, " Symmetry Y\n");
  if (lefiMacro_has90Symmetry(this))
    fprintf(f, " Symmetry R90\n");
  if (lefiMacro_hasSiteName(this))
    fprintf(f, " Site name %s\n", lefiMacro_siteName(this));
  if (lefiMacro_hasSitePattern(this)) {
    for (i = 0;
	 i < lefiMacro_numSitePattern(this);
	 i++) {
      sp = lefiMacro_sitePattern(this,
				 i);
      fprintf(f, " Site pattern ");
      lefiSitePattern_print(sp,
			    f);
    }
  }
  if (lefiMacro_hasSize(this))
    fprintf(f, " Size %g,%g\n", lefiMacro_sizeX(this), lefiMacro_sizeY(this));
  if (lefiMacro_hasForeign(this)) {
    fprintf(f, " Foreign %s", lefiMacro_foreignName(this));
    if (lefiMacro_hasForeignOrigin(this))
      fprintf(f, " %g,%g", lefiMacro_foreignX(this), lefiMacro_foreignY(this));
    if (lefiMacro_hasForeignOrient(this))
      fprintf(f, " orient %d", lefiMacro_foreignOrient(this));
    fprintf(f, "\n");
  }
  if (lefiMacro_hasClockType(this))
    fprintf(f, " Clock type %s\n", lefiMacro_clockType(this));
  fprintf(f, "END MACRO %s\n", lefiMacro_name(this));
}

/*
 * lefiTiming
 */ lefiTiming *
lefiTiming_CreateUninitialized()
{
  lefiTiming *this = (lefiTiming *) malloc(sizeof(lefiTiming));

  lefiTiming_Init(this);
  return (this);
}

void 
lefiTiming_Init(lefiTiming * this)
{
  this->numFrom_ = 0;
  this->from_ = (char **) malloc(sizeof(char *));
  this->fromAllocated_ = 1;
  this->numTo_ = 0;
  this->to_ = (char **) malloc(sizeof(char *));
  this->toAllocated_ = 1;
  this->numOfAxisNumbers_ = 0;
  this->axisNumbers_ = (double *) malloc(sizeof(double));
  this->axisNumbersAllocated_ = 1;
  this->numOfTableEntries_ = 0;
  this->tableEntriesAllocated_ = 1;
  this->table_ = (double *) malloc(sizeof(double) * 3);
  lefiTiming_clear(this);
}

void 
lefiTiming_Destroy(lefiTiming * this)
{
  lefiTiming_clear(this);
  free((char *) (this->from_));
  free((char *) (this->to_));
  free((char *) (this->axisNumbers_));
  free((char *) (this->table_));
}

void
lefiTiming_Delete(lefiTiming * this)
{
  if (this == NULL)
    return;
  lefiTiming_Destroy(this);
  free(this);
}

void 
lefiTiming_addRiseFall(lefiTiming * this,
		       const char *risefall,
		       double one,
		       double two)
{
  if (*risefall == 'r' || *risefall == 'R') {
    this->hasRiseIntrinsic_ = 1;
    this->nowRise_ = 1;
    this->riseIntrinsicOne_ = one;
    this->riseIntrinsicTwo_ = two;
  }
  else {
    this->nowRise_ = 0;
    this->hasFallIntrinsic_ = 1;
    this->fallIntrinsicOne_ = one;
    this->fallIntrinsicTwo_ = two;
  }
}

void 
lefiTiming_addRiseFallVariable(lefiTiming * this,
			       double one,
			       double two)
{
  if (this->nowRise_ == 1) {
    this->riseIntrinsicThree_ = one;
    this->riseIntrinsicFour_ = two;
  }
  else {
    this->fallIntrinsicThree_ = one;
    this->fallIntrinsicFour_ = two;
  }
}

void 
lefiTiming_setRiseRS(lefiTiming * this,
		     double one,
		     double two)
{
  this->hasRiseRS_ = 1;
  this->riseRSOne_ = one;
  this->riseRSTwo_ = two;
}

void 
lefiTiming_setFallRS(lefiTiming * this,
		     double one,
		     double two)
{
  this->hasFallRS_ = 1;
  this->fallRSOne_ = one;
  this->fallRSTwo_ = two;
}

void 
lefiTiming_setRiseCS(lefiTiming * this,
		     double one,
		     double two)
{
  this->hasRiseCS_ = 1;
  this->riseCSOne_ = one;
  this->riseCSTwo_ = two;
}

void 
lefiTiming_setFallCS(lefiTiming * this,
		     double one,
		     double two)
{
  this->hasFallCS_ = 1;
  this->fallCSOne_ = one;
  this->fallCSTwo_ = two;
}

void 
lefiTiming_setRiseAtt1(lefiTiming * this,
		       double one,
		       double two)
{
  this->hasRiseAtt1_ = 1;
  this->riseAtt1One_ = one;
  this->riseAtt1Two_ = two;
}

void 
lefiTiming_setFallAtt1(lefiTiming * this,
		       double one,
		       double two)
{
  this->hasFallAtt1_ = 1;
  this->fallAtt1One_ = one;
  this->fallAtt1Two_ = two;
}

void 
lefiTiming_setRiseTo(lefiTiming * this,
		     double one,
		     double two)
{
  this->hasRiseTo_ = 1;
  this->riseToOne_ = one;
  this->riseToTwo_ = two;
}

void 
lefiTiming_setFallTo(lefiTiming * this,
		     double one,
		     double two)
{
  this->hasFallTo_ = 1;
  this->fallToOne_ = one;
  this->fallToTwo_ = two;
}

void 
lefiTiming_addUnateness(lefiTiming * this,
			const char *typ)
{
  this->hasUnateness_ = 1;
  this->unateness_ = (char *) typ;
}

void 
lefiTiming_setStable(lefiTiming * this,
		     double one,
		     double two,
		     const char *typ)
{
  this->hasStableTiming_ = 1;
  this->stableSetup_ = one;
  this->stableHold_ = two;
  this->stableRiseFall_ = (char *) typ;
}

void 
lefiTiming_addTableEntry(lefiTiming * this,
			 double one,
			 double two,
			 double three)
{
  int     i;

  double *n;

  if (this->numOfTableEntries_ >= this->tableEntriesAllocated_) {
    int     lim;

    lim = this->tableEntriesAllocated_ *= 2;
    n = (double *) malloc(sizeof(double) * 3 * lim);
    lim = this->numOfTableEntries_ * 3;
    for (i = 0;
	 i < lim;
	 i++) {
      n[i] = this->table_[i];
    }
    free((char *) (this->table_));
    this->table_ = n;
  }
  i = this->numOfTableEntries_ * 3;
  this->table_[i++] = one;
  this->table_[i++] = two;
  this->table_[i] = three;
  this->numOfTableEntries_ += 1;
}

void 
lefiTiming_addTableAxisNumber(lefiTiming * this,
			      double one)
{
  if (this->numOfAxisNumbers_ == this->axisNumbersAllocated_) {
    int     i;

    int     lim;

    double *n;

    lim = this->axisNumbersAllocated_ *= 2;
    n = (double *) malloc(sizeof(double) * lim);
    lim = this->numOfAxisNumbers_;
    for (i = 0;
	 i < lim;
	 i++)
      n[i] = this->axisNumbers_[i];
    free((char *) (this->axisNumbers_));
    this->axisNumbers_ = n;
  }
  this->axisNumbers_[(this->numOfAxisNumbers_)++] = one;
}

void 
lefiTiming_addRiseFallSlew(lefiTiming * this,
			   double one,
			   double two,
			   double three,
			   double four)
{
  if (this->nowRise_) {
    this->hasRiseSlew_ = 1;
    this->riseSlewOne_ = one;
    this->riseSlewTwo_ = two;
    this->riseSlewThree_ = three;
    this->riseSlewFour_ = four;
  }
  else {
    this->hasFallSlew_ = 1;
    this->fallSlewOne_ = one;
    this->fallSlewTwo_ = two;
    this->fallSlewThree_ = three;
    this->fallSlewFour_ = four;
  }
}

void 
lefiTiming_addRiseFallSlew2(lefiTiming * this,
			    double one,
			    double two,
			    double three)
{
  if (this->nowRise_) {
    this->hasRiseSlew2_ = 1;
    this->riseSlewFive_ = one;
    this->riseSlewSix_ = two;
    this->riseSlewSeven_ = three;
  }
  else {
    this->hasFallSlew2_ = 1;
    this->fallSlewFive_ = one;
    this->fallSlewSix_ = two;
    this->fallSlewSeven_ = three;
  }
}

void 
lefiTiming_addFromPin(lefiTiming * this,
		      const char *name)
{
  if (this->numFrom_ == this->fromAllocated_) {
    int     lim;

    int     i;

    char  **n;

    lim = this->fromAllocated_ *= 2;
    n = (char **) malloc(sizeof(char *) * lim);
    lim = this->numFrom_;
    for (i = 0;
	 i < lim;
	 i++)
      n[i] = this->from_[i];
    free((char *) (this->from_));
    this->from_ = n;
  }
  this->from_[(this->numFrom_)++] = (char *) name;
}

void 
lefiTiming_addToPin(lefiTiming * this,
		    const char *name)
{
  if (this->numTo_ == this->toAllocated_) {
    int     lim;

    int     i;

    char  **n;

    lim = this->toAllocated_ *= 2;
    n = (char **) malloc(sizeof(char *) * lim);
    lim = this->numTo_;
    for (i = 0;
	 i < lim;
	 i++)
      n[i] = this->to_[i];
    free((char *) (this->to_));
    this->to_ = n;
  }
  this->to_[(this->numTo_)++] = (char *) name;
}

void 
lefiTiming_addDelay(lefiTiming * this,
		    const char *risefall,
		    const char *unateness,
		    double one,
		    double two,
		    double three)
{
  this->hasDelay_ = 1;
  this->delayRiseOrFall_ = (char *) risefall;
  this->delayUnateness_ = (char *) unateness;
  this->delayTableOne_ = one;
  this->delayTableTwo_ = two;
  this->delayTableThree_ = three;
}

void 
lefiTiming_addTransition(lefiTiming * this,
			 const char *risefall,
			 const char *unateness,
			 double one,
			 double two,
			 double three)
{
  this->hasTransition_ = 1;
  this->transitionRiseOrFall_ = (char *) risefall;
  this->transitionUnateness_ = (char *) unateness;
  this->transitionTableOne_ = one;
  this->transitionTableTwo_ = two;
  this->transitionTableThree_ = three;
}

void 
lefiTiming_addSDF1Pin(lefiTiming * this,
		      const char *trigType,
		      double one,
		      double two,
		      double three)
{
  this->hasSDFonePinTrigger_ = 1;
  this->SDFtriggerType_ = (char *) trigType;
  this->SDFtriggerTableOne_ = one;
  this->SDFtriggerTableTwo_ = two;
  this->SDFtriggerTableThree_ = three;
}

void 
lefiTiming_addSDF2Pins(lefiTiming * this,
		       const char *trigType,
		       const char *fromTrig,
		       const char *toTrig,
		       double one,
		       double two,
		       double three)
{
  this->hasSDFtwoPinTrigger_ = 1;
  this->SDFtriggerType_ = (char *) trigType;
  this->SDFfromTrigger_ = (char *) fromTrig;
  this->SDFtoTrigger_ = (char *) toTrig;
  this->SDFtriggerTableOne_ = one;
  this->SDFtriggerTableTwo_ = two;
  this->SDFtriggerTableThree_ = three;
}

void 
lefiTiming_setSDFcondStart(lefiTiming * this,
			   const char *condStart)
{
  this->SDFcondStart_ = (char *) condStart;
}

void 
lefiTiming_setSDFcondEnd(lefiTiming * this,
			 const char *condEnd)
{
  this->SDFcondEnd_ = (char *) condEnd;
}

void 
lefiTiming_setSDFcond(lefiTiming * this,
		      const char *cond)
{
  this->SDFcond_ = (char *) cond;
}

int 
lefiTiming_hasData(lefiTiming * this)
{
  return ((this->numFrom_) ? 1 : 0);
}

void 
lefiTiming_clear(lefiTiming * this)
{
  this->numFrom_ = 0;
  this->numTo_ = 0;
  this->numOfAxisNumbers_ - 0;
  this->numOfTableEntries_ = 0;
  this->nowRise_ = 0;
  this->hasTransition_ = 0;
  this->hasDelay_ = 0;
  this->hasFallSlew_ = 0;
  this->hasFallSlew2_ = 0;
  this->hasRiseSlew_ = 0;
  this->hasRiseSlew2_ = 0;
  this->hasRiseIntrinsic_ = 0;
  this->hasFallIntrinsic_ = 0;
  this->hasRiseSlew_ = 0;
  this->hasFallSlew_ = 0;
  this->hasRiseSlew2_ = 0;
  this->hasFallSlew2_ = 0;
  this->hasRiseRS_ = 0;
  this->hasRiseCS_ = 0;
  this->hasFallRS_ = 0;
  this->hasFallCS_ = 0;
  this->hasUnateness_ = 0;
  this->hasFallAtt1_ = 0;
  this->hasRiseAtt1_ = 0;
  this->hasFallTo_ = 0;
  this->hasRiseTo_ = 0;
  this->hasStableTiming_ = 0;
  this->hasSDFonePinTrigger_ = 0;
  this->hasSDFtwoPinTrigger_ = 0;
  this->hasSDFcondStart_ = 0;
  this->hasSDFcondEnd_ = 0;
  this->hasSDFcond_ = 0;
}

int 
lefiTiming_numFromPins(lefiTiming * this)
{
  return this->numFrom_;
}

const char *
lefiTiming_fromPin(lefiTiming * this,
		   int index)
{
  return this->from_[index];
}

int 
lefiTiming_numToPins(lefiTiming * this)
{
  return this->numTo_;
}

const char *
lefiTiming_toPin(lefiTiming * this,
		 int index)
{
  return this->to_[index];
}

int 
lefiTiming_hasTransition(lefiTiming * this)
{
  return this->hasTransition_;
}

int 
lefiTiming_hasDelay(lefiTiming * this)
{
  return this->hasDelay_;
}

int 
lefiTiming_hasRiseSlew(lefiTiming * this)
{
  return this->hasRiseSlew_;
}

int 
lefiTiming_hasRiseSlew2(lefiTiming * this)
{
  return this->hasRiseSlew2_;
}

int 
lefiTiming_hasFallSlew(lefiTiming * this)
{
  return this->hasFallSlew_;
}

int 
lefiTiming_hasFallSlew2(lefiTiming * this)
{
  return this->hasFallSlew2_;
}

int 
lefiTiming_hasRiseIntrinsic(lefiTiming * this)
{
  return this->hasRiseIntrinsic_;
}

int 
lefiTiming_hasFallIntrinsic(lefiTiming * this)
{
  return this->hasFallIntrinsic_;
}

int 
lefiTiming_hasSDFonePinTrigger(lefiTiming * this)
{
  return this->hasSDFonePinTrigger_;
}

int 
lefiTiming_hasSDFtwoPinTrigger(lefiTiming * this)
{
  return this->hasSDFtwoPinTrigger_;
}

int 
lefiTiming_hasSDFcondStart(lefiTiming * this)
{
  return this->hasSDFcondStart_;
}

int 
lefiTiming_hasSDFcondEnd(lefiTiming * this)
{
  return this->hasSDFcondEnd_;
}

int 
lefiTiming_hasSDFcond(lefiTiming * this)
{
  return this->hasSDFcond_;
}

int 
lefiTiming_numOfAxisNumbers(lefiTiming * this)
{
  return this->numOfAxisNumbers_;
}

double *
lefiTiming_axisNumbers(lefiTiming * this)
{
  return this->axisNumbers_;
}

int 
lefiTiming_numOfTableEntries(lefiTiming * this)
{
  return this->numOfTableEntries_;
}

void 
lefiTiming_tableEntry(lefiTiming * this,
		      int num,
		      double *one,
		      double *two,
		      double *three)
{
  num *= 3;
  *one = this->table_[num];
  num++;
  *two = this->table_[num];
  num++;
  *three = this->table_[num];
}

const char *
lefiTiming_delayRiseOrFall(lefiTiming * this)
{
  return this->delayRiseOrFall_;
}

const char *
lefiTiming_delayUnateness(lefiTiming * this)
{
  return this->delayUnateness_;
}

double 
lefiTiming_delayTableOne(lefiTiming * this)
{
  return this->delayTableOne_;
}

double 
lefiTiming_delayTableTwo(lefiTiming * this)
{
  return this->delayTableTwo_;
}

double 
lefiTiming_delayTableThree(lefiTiming * this)
{
  return this->delayTableThree_;
}

const char *
lefiTiming_transitionRiseOrFall(lefiTiming * this)
{
  return this->transitionRiseOrFall_;
}

const char *
lefiTiming_transitionUnateness(lefiTiming * this)
{
  return this->transitionUnateness_;
}

double 
lefiTiming_transitionTableOne(lefiTiming * this)
{
  return this->transitionTableOne_;
}

double 
lefiTiming_transitionTableTwo(lefiTiming * this)
{
  return this->transitionTableTwo_;
}

double 
lefiTiming_transitionTableThree(lefiTiming * this)
{
  return this->transitionTableThree_;
}

double 
lefiTiming_riseIntrinsicOne(lefiTiming * this)
{
  return this->riseIntrinsicOne_;
}

double 
lefiTiming_riseIntrinsicTwo(lefiTiming * this)
{
  return this->riseIntrinsicTwo_;
}

double 
lefiTiming_riseIntrinsicThree(lefiTiming * this)
{
  return this->riseIntrinsicThree_;
}

double 
lefiTiming_riseIntrinsicFour(lefiTiming * this)
{
  return this->riseIntrinsicFour_;
}

double 
lefiTiming_fallIntrinsicOne(lefiTiming * this)
{
  return this->fallIntrinsicOne_;
}

double 
lefiTiming_fallIntrinsicTwo(lefiTiming * this)
{
  return this->fallIntrinsicTwo_;
}

double 
lefiTiming_fallIntrinsicThree(lefiTiming * this)
{
  return this->fallIntrinsicThree_;
}

double 
lefiTiming_fallIntrinsicFour(lefiTiming * this)
{
  return this->fallIntrinsicFour_;
}

double 
lefiTiming_riseSlewOne(lefiTiming * this)
{
  return this->riseSlewOne_;
}

double 
lefiTiming_riseSlewTwo(lefiTiming * this)
{
  return this->riseSlewTwo_;
}

double 
lefiTiming_riseSlewThree(lefiTiming * this)
{
  return this->riseSlewThree_;
}

double 
lefiTiming_riseSlewFour(lefiTiming * this)
{
  return this->riseSlewFour_;
}

double 
lefiTiming_riseSlewFive(lefiTiming * this)
{
  return this->riseSlewFive_;
}

double 
lefiTiming_riseSlewSix(lefiTiming * this)
{
  return this->riseSlewSix_;
}

double 
lefiTiming_riseSlewSeven(lefiTiming * this)
{
  return this->riseSlewSeven_;
}

double 
lefiTiming_fallSlewOne(lefiTiming * this)
{
  return this->fallSlewOne_;
}

double 
lefiTiming_fallSlewTwo(lefiTiming * this)
{
  return this->fallSlewTwo_;
}

double 
lefiTiming_fallSlewThree(lefiTiming * this)
{
  return this->fallSlewThree_;
}

double 
lefiTiming_fallSlewFour(lefiTiming * this)
{
  return this->fallSlewFour_;
}

double 
lefiTiming_fallSlewFive(lefiTiming * this)
{
  return this->fallSlewFive_;
}

double 
lefiTiming_fallSlewSix(lefiTiming * this)
{
  return this->fallSlewSix_;
}

double 
lefiTiming_fallSlewSeven(lefiTiming * this)
{
  return this->fallSlewSeven_;
}

int 
lefiTiming_hasRiseRS(lefiTiming * this)
{
  return this->hasRiseRS_;
}

double 
lefiTiming_riseRSOne(lefiTiming * this)
{
  return this->riseRSOne_;
}

double 
lefiTiming_riseRSTwo(lefiTiming * this)
{
  return this->riseRSTwo_;
}

int 
lefiTiming_hasRiseCS(lefiTiming * this)
{
  return this->hasRiseCS_;
}

double 
lefiTiming_riseCSOne(lefiTiming * this)
{
  return this->riseCSOne_;
}

double 
lefiTiming_riseCSTwo(lefiTiming * this)
{
  return this->riseCSTwo_;
}

int 
lefiTiming_hasFallRS(lefiTiming * this)
{
  return this->hasFallRS_;
}

double 
lefiTiming_fallRSOne(lefiTiming * this)
{
  return this->fallRSOne_;
}

double 
lefiTiming_fallRSTwo(lefiTiming * this)
{
  return this->fallRSTwo_;
}

int 
lefiTiming_hasFallCS(lefiTiming * this)
{
  return this->hasFallCS_;
}

double 
lefiTiming_fallCSOne(lefiTiming * this)
{
  return this->fallCSOne_;
}

double 
lefiTiming_fallCSTwo(lefiTiming * this)
{
  return this->fallCSTwo_;
}

int 
lefiTiming_hasUnateness(lefiTiming * this)
{
  return this->hasUnateness_;
}

const char *
lefiTiming_unateness(lefiTiming * this)
{
  return this->unateness_;
}

int 
lefiTiming_hasFallAtt1(lefiTiming * this)
{
  return this->hasFallAtt1_;
}

double 
lefiTiming_fallAtt1One(lefiTiming * this)
{
  return this->fallAtt1One_;
}

double 
lefiTiming_fallAtt1Two(lefiTiming * this)
{
  return this->fallAtt1Two_;
}

int 
lefiTiming_hasRiseAtt1(lefiTiming * this)
{
  return this->hasRiseAtt1_;
}

double 
lefiTiming_riseAtt1One(lefiTiming * this)
{
  return this->riseAtt1One_;
}

double 
lefiTiming_riseAtt1Two(lefiTiming * this)
{
  return this->riseAtt1Two_;
}

int 
lefiTiming_hasFallTo(lefiTiming * this)
{
  return this->hasFallTo_;
}

double 
lefiTiming_fallToOne(lefiTiming * this)
{
  return this->fallToOne_;
}

double 
lefiTiming_fallToTwo(lefiTiming * this)
{
  return this->fallToTwo_;
}

int 
lefiTiming_hasRiseTo(lefiTiming * this)
{
  return this->hasRiseTo_;
}

double 
lefiTiming_riseToOne(lefiTiming * this)
{
  return this->riseToOne_;
}

double 
lefiTiming_riseToTwo(lefiTiming * this)
{
  return this->riseToTwo_;
}

int 
lefiTiming_hasStableTiming(lefiTiming * this)
{
  return this->hasStableTiming_;
}

double 
lefiTiming_stableSetup(lefiTiming * this)
{
  return this->stableSetup_;
}

double 
lefiTiming_stableHold(lefiTiming * this)
{
  return this->stableHold_;
}

const char *
lefiTiming_stableRiseFall(lefiTiming * this)
{
  return this->stableRiseFall_;
}

const char *
lefiTiming_SDFonePinTriggerType(lefiTiming * this)
{
  return this->SDFtriggerType_;
}

const char *
lefiTiming_SDFtwoPinTriggerType(lefiTiming * this)
{
  return this->SDFtriggerType_;
}

const char *
lefiTiming_SDFfromTrigger(lefiTiming * this)
{
  return this->SDFfromTrigger_;
}

const char *
lefiTiming_SDFtoTrigger(lefiTiming * this)
{
  return this->SDFtoTrigger_;
}

double 
lefiTiming_SDFtriggerOne(lefiTiming * this)
{
  return this->SDFtriggerTableOne_;
}

double 
lefiTiming_SDFtriggerTwo(lefiTiming * this)
{
  return this->SDFtriggerTableTwo_;
}

double 
lefiTiming_SDFtriggerThree(lefiTiming * this)
{
  return this->SDFtriggerTableThree_;
}

const char *
lefiTiming_SDFcondStart(lefiTiming * this)
{
  return this->SDFcondStart_;
}

const char *
lefiTiming_SDFcondEnd(lefiTiming * this)
{
  return this->SDFcondEnd_;
}

const char *
lefiTiming_SDFcond(lefiTiming * this)
{
  return this->SDFcond_;
}
