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

#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "lefiMacro.hpp"
#include "lefiMisc.hpp"
#include "lefiDebug.hpp"


////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiGeometries
//
////////////////////////////////////////////
////////////////////////////////////////////


lefiGeometries::lefiGeometries() {
  this->lefiGeometries::Init();
}


void lefiGeometries::Init() {
  this->itemsAllocated_ = 2;
  this->numItems_ = 0;
  this->itemType_ = (enum lefiGeomEnum*)malloc(sizeof(enum lefiGeomEnum)*2);
  this->items_ = (void**)malloc(sizeof(void*)*2);
  this->pointsAllocated_ = 0;
  this->x_ = 0;
  this->y_ = 0;
}


void lefiGeometries::Destroy() {
  this->lefiGeometries::clear();
  free((char*)(this->items_));
  free((char*)(this->itemType_));
  if (this->x_)
    { free((double*)(this->x_)); free((double*)(this->y_)); }
}


lefiGeometries::~lefiGeometries() {
  this->lefiGeometries::Destroy();
}


void lefiGeometries::clear() {
   int i;

   for (i = 0; i < this->numItems_; i++) {
     if (this->itemType_[i] == lefiGeomViaE)
       free(((struct lefiGeomVia*)(this->items_[i]))->name);
     if (this->itemType_[i] == lefiGeomViaIterE)
       free(((struct lefiGeomViaIter*)(this->items_[i]))->name);
     if (this->itemType_[i] == lefiGeomPathE) {
       ((struct lefiGeomPath*)(this->items_[i]))->numPoints = 0;
       free((double*)((struct lefiGeomPath*)this->items_[i])->x);
       free((double*)((struct lefiGeomPath*)this->items_[i])->y);
     }
     free((char*)(this->items_[i]));
   }
   this->numItems_ = 0;
}


void lefiGeometries::add(void* v, enum lefiGeomEnum e) {
  if (this->numItems_ == this->itemsAllocated_) {
    int i;
    void** newi;
    enum lefiGeomEnum* newe;
    this->itemsAllocated_ *= 2;
    newe = (enum lefiGeomEnum*)malloc(sizeof(enum lefiGeomEnum)*this->itemsAllocated_);
    newi= (void**)malloc(sizeof(void*)*this->itemsAllocated_);
    for (i = 0; i < this->numItems_; i++) {
      newe[i] = this->itemType_[i];
      newi[i] = this->items_[i];
    }
    free((char*)(this->items_));
    free((char*)(this->itemType_));
    this->items_ = newi;
    this->itemType_ = newe;
  }
  this->items_[this->numItems_] = v;
  this->itemType_[this->numItems_] = e;
  this->numItems_ += 1;
}


void lefiGeometries::addLayer(const char* name) {
  char* c = (char*)malloc(strlen(name)+1);
  strcpy(c, CASE(name));
  this->lefiGeometries::add((void*)c, lefiGeomLayerE);
}


void lefiGeometries::addClass(const char* name) {
  char* c = (char*)malloc(strlen(name)+1);
  strcpy(c, CASE(name));
  this->lefiGeometries::add((void*)c, lefiGeomClassE);
}


void lefiGeometries::addWidth(double w) {
  double* d = (double*)malloc(sizeof(double));
  *d = w;
  this->lefiGeometries::add((void*)d, lefiGeomWidthE);
}


void lefiGeometries::addPath() {
  int i;
  int lim;
  struct lefiGeomPath* p = (struct lefiGeomPath*)malloc(
		 sizeof(struct lefiGeomPath));
  lim = p->numPoints = this->numPoints_;
  p->x = (double*)malloc(sizeof(double)*lim);
  p->y = (double*)malloc(sizeof(double)*lim);
  for (i = 0; i < lim; i++) {
    p->x[i] = this->x_[i];
    p->y[i] = this->y_[i];
  }
  this->lefiGeometries::add((void*)p, lefiGeomPathE);
}


void lefiGeometries::addPathIter() {
  int i;
  int lim;
  struct lefiGeomPathIter* p = (struct lefiGeomPathIter*)malloc(
		 sizeof(struct lefiGeomPathIter));
  lim = p->numPoints = this->numPoints_;
  p->x = (double*)malloc(sizeof(double)*lim);
  p->y = (double*)malloc(sizeof(double)*lim);
  for (i = 0; i < lim; i++) {
    p->x[i] = this->x_[i];
    p->y[i] = this->y_[i];
  }
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  this->lefiGeometries::add((void*)p, lefiGeomPathIterE);
}


void lefiGeometries::addRect(double xl, double yl, double xh, double yh) {
  struct lefiGeomRect* p = (struct lefiGeomRect*)malloc(
		 sizeof(struct lefiGeomRect));
  p->xl = xl;
  p->yl = yl;
  p->xh = xh;
  p->yh = yh;
  this->lefiGeometries::add((void*)p, lefiGeomRectE);
}


void lefiGeometries::addRectIter(double xl, double yl,
				 double xh, double yh) {
  struct lefiGeomRectIter* p = (struct lefiGeomRectIter*)malloc(
		 sizeof(struct lefiGeomRectIter));
  p->xl = xl;
  p->yl = yl;
  p->xh = xh;
  p->yh = yh;
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  this->lefiGeometries::add((void*)p, lefiGeomRectIterE);
}


void lefiGeometries::addPolygon() {
  int i;
  int lim;
  struct lefiGeomPolygon* p = (struct lefiGeomPolygon*)malloc(
		 sizeof(struct lefiGeomPolygon));
  lim = p->numPoints = this->numPoints_;
  p->x = (double*)malloc(sizeof(double)*lim);
  p->y = (double*)malloc(sizeof(double)*lim);
  for (i = 0; i < lim; i++) {
    p->x[i] = this->x_[i];
    p->y[i] = this->y_[i];
  }
  this->lefiGeometries::add((void*)p, lefiGeomPolygonE);
}


void lefiGeometries::addPolygonIter() {
  int i;
  int lim;
  struct lefiGeomPolygonIter* p = (struct lefiGeomPolygonIter*)malloc(
		 sizeof(struct lefiGeomPolygonIter));
  lim = p->numPoints = this->numPoints_;
  p->x = (double*)malloc(sizeof(double)*lim);
  p->y = (double*)malloc(sizeof(double)*lim);
  for (i = 0; i < lim; i++) {
    p->x[i] = this->x_[i];
    p->y[i] = this->y_[i];
  }
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  this->lefiGeometries::add((void*)p, lefiGeomPolygonIterE);
}


void lefiGeometries::addVia(double x, double y, const char* name) {
  struct lefiGeomVia* p = (struct lefiGeomVia*)malloc(
		 sizeof(struct lefiGeomVia));
  char* c = (char*)malloc(strlen(name)+1);
  strcpy(c, CASE(name));
  p->x = x;
  p->y = y;
  p->name = c;
  this->lefiGeometries::add((void*)p, lefiGeomViaE);
}


void lefiGeometries::addViaIter(double x, double y, const char* name) {
  struct lefiGeomViaIter* p = (struct lefiGeomViaIter*)malloc(
		 sizeof(struct lefiGeomViaIter));
  char* c = (char*)malloc(strlen(name)+1);
  strcpy(c, CASE(name));
  p->x = x;
  p->y = y;
  p->name = c;
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  this->lefiGeometries::add((void*)p, lefiGeomViaIterE);
}


void lefiGeometries::addStepPattern(double xStart, double yStart,
				    double xStep, double yStep) {
  this->xStart_ = xStart;
  this->yStart_ = yStart;
  this->xStep_ = xStep;
  this->yStep_ = yStep;
}


void lefiGeometries::startList(double x, double y) {
  if (!this->x_) {
    this->numPoints_ = 0;
    this->pointsAllocated_ = 16;
    this->x_ = (double*)malloc(sizeof(double)*16);
    this->y_ = (double*)malloc(sizeof(double)*16);
  } else {  // reset the numPoits to 0
    this->numPoints_ = 0;
  } 
  this->lefiGeometries::addToList(x,y);
}


void lefiGeometries::addToList(double x, double y) {
  if (this->numPoints_ == this->pointsAllocated_) {
    int i;
    double* nx;
    double* ny;
    this->pointsAllocated_ *= 2;
    nx = (double*)malloc(sizeof(double)*this->pointsAllocated_);
    ny = (double*)malloc(sizeof(double)*this->pointsAllocated_);
    for (i = 0; i < this->numPoints_; i++) {
      nx[i] = this->x_[i];
      ny[i] = this->y_[i];
    }
    free((char*)(this->x_));
    free((char*)(this->y_));
    this->x_ = nx;
    this->y_ = ny;
  }
  this->x_[this->numPoints_] = x;
  this->y_[this->numPoints_] = y;
  this->numPoints_ += 1;
}


int lefiGeometries::numItems() const {
  return this->numItems_;
}


enum lefiGeomEnum lefiGeometries::itemType(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return lefiGeomUnknown;
  }
  return this->itemType_[index];
}


struct lefiGeomRect* lefiGeometries::getRect(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomRect*)(this->items_[index]);
}


struct lefiGeomRectIter* lefiGeometries::getRectIter(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomRectIter*)(this->items_[index]);
}


struct lefiGeomPath* lefiGeometries::getPath(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomPath*)(this->items_[index]);
}


struct lefiGeomPathIter* lefiGeometries::getPathIter(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomPathIter*)(this->items_[index]);
}


char* lefiGeometries::getLayer(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (char*)(this->items_[index]);
}

 
double lefiGeometries::getWidth(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return *((double*)(this->items_[index]));
}

 
struct lefiGeomPolygon* lefiGeometries::getPolygon(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomPolygon*)(this->items_[index]);
}


struct lefiGeomPolygonIter* lefiGeometries::getPolygonIter(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomPolygonIter*)(this->items_[index]);
}


char* lefiGeometries::getClass(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (char*)(this->items_[index]);
}

 
struct lefiGeomVia* lefiGeometries::getVia(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomVia*)(this->items_[index]);
}

 
struct lefiGeomViaIter* lefiGeometries::getViaIter(int index) const {
  if (index < 0 || index >= this->numItems_) {
    lefiError("Bad index for geometry item");
    return 0;
  }
  return (struct lefiGeomViaIter*)(this->items_[index]);
}

 
void lefiGeometries::print(FILE* f) const {
  int i;
  int l;
  struct lefiGeomRect* rect;
  struct lefiGeomRectIter* rectiter;
  struct lefiGeomPath* path;
  struct lefiGeomPathIter* pathiter;
  struct lefiGeomPolygon* polygon;
  struct lefiGeomPolygonIter* polygoniter;

  for (i = 0; i < this->numItems_; i++) {
    switch (this->lefiGeometries::itemType(i)) {

    case lefiGeomLayerE: 
	 fprintf(f, "Layer %s\n", this->lefiGeometries::getLayer(i));
	 break;

    case lefiGeomWidthE: 
	 fprintf(f, "Width %g\n", this->lefiGeometries::getWidth(i));
	 break;

    case lefiGeomPathE: 
	 path = this->lefiGeometries::getPath(i);
	 fprintf(f, "Path");
	 for (l = 0; l < path->numPoints; l++)
	   fprintf(f, " %g,%g", path->x[l], path->y[l]);
	 fprintf(f, "\n");
	 break;

    case lefiGeomPathIterE: 
	 pathiter = this->lefiGeometries::getPathIter(i);
	 fprintf(f, "Path iter  start %g,%g  step %g,%g\n",
	 pathiter->xStart, pathiter->yStart,
	 pathiter->xStep, pathiter->yStep);
	 for (l = 0; l < pathiter->numPoints; l++)
	   fprintf(f, " %g,%g", pathiter->x[l], pathiter->y[l]);
	 fprintf(f, "\n");
	 break;

    case lefiGeomRectE: 
	 rect = this->lefiGeometries::getRect(i);
	 fprintf(f, "Rect %g,%g  %g,%g\n", rect->xl, rect->yl,
	 rect->xh, rect->yh);
	 break;

    case lefiGeomRectIterE: 
	 rectiter = this->lefiGeometries::getRectIter(i);
	 fprintf(f, "Rect iter  start %g,%g  step %g,%g\n",
	 rectiter->xStart, rectiter->yStart,
	 rectiter->xStep, rectiter->yStep);
	 fprintf(f, "     %g,%g  %g,%g\n", rectiter->xl, rectiter->yl,
	 rectiter->xh, rectiter->yh);
	 break;

    case lefiGeomPolygonE: 
	 polygon = this->lefiGeometries::getPolygon(i);
	 fprintf(f, "Polygon");
	 for (l = 0; l < polygon->numPoints; l++)
	   fprintf(f, " %g,%g", polygon->x[l], polygon->y[l]);
	 fprintf(f, "\n");
	 break;

    case lefiGeomPolygonIterE: 
	 polygoniter = this->lefiGeometries::getPolygonIter(i);
	 fprintf(f, "Polygon iter  start %g,%g  step %g,%g\n",
	 polygoniter->xStart, polygoniter->yStart,
	 polygoniter->xStep, polygoniter->yStep);
	 for (l = 0; l < polygoniter->numPoints; l++)
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

    default: lefiError("unknown geometry type");
	  fprintf(f, "Unknown geometry type %d\n",
	  (int)(this->lefiGeometries::itemType(i)));
	  break;
    }
  }
}







////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiObstruction
//
////////////////////////////////////////////
////////////////////////////////////////////



lefiObstruction::lefiObstruction() {
  this->lefiObstruction::Init();
}


void lefiObstruction::Init() {
  this->geometries_ = 0;
}


lefiObstruction::~lefiObstruction() {
  this->lefiObstruction::Destroy();
}


void lefiObstruction::Destroy() {
  this->lefiObstruction::clear();
}


void lefiObstruction::clear() {
 if (this->geometries_) {
   this->geometries_->lefiGeometries::Destroy();
   free((char*)(this->geometries_));
  }
 this->geometries_ = 0;
}


void lefiObstruction::setGeometries(lefiGeometries* g) {
  this->lefiObstruction::clear();
  this->geometries_ = g;
}


lefiGeometries* lefiObstruction::geometries() const {
  return this->geometries_;
}


void lefiObstruction::print(FILE* f) const {
  lefiGeometries* g;

  fprintf(f, "  Obstruction\n");

  g = this->geometries_;
  g->lefiGeometries::print(f);

}





////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiPin
//
////////////////////////////////////////////
////////////////////////////////////////////


lefiPin::lefiPin() {
  this->lefiPin::Init();
}


void lefiPin::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)malloc(16);
  this->portsAllocated_ = 2;
  this->ports_ = (lefiGeometries**)malloc(sizeof(lefiGeometries*)*2);
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

  this->lefiPin::bump(&(this->foreign_), 16, &(this->foreignSize_));
  this->lefiPin::bump(&(this->LEQ_), 16, &(this->LEQSize_));
  this->lefiPin::bump(&(this->mustjoin_), 16, &(this->mustjoinSize_));
  this->lefiPin::bump(&(this->lowTable_), 16, &(this->lowTableSize_));
  this->lefiPin::bump(&(this->highTable_), 16, &(this->highTableSize_));

  this->numAntennaSize_ = 0;
  this->antennaSizeAllocated_ = 1;
  this->antennaSize_ = (double*)malloc(sizeof(double));
  this->antennaSizeLayer_ = (char**)malloc(sizeof(char*));

  this->numAntennaMetalArea_ = 0;
  this->antennaMetalAreaAllocated_ = 1;
  this->antennaMetalArea_ = (double*)malloc(sizeof(double));
  this->antennaMetalAreaLayer_ = (char**)malloc(sizeof(char*));

  this->numAntennaMetalLength_ = 0;
  this->antennaMetalLengthAllocated_ = 1;
  this->antennaMetalLength_ = (double*)malloc(sizeof(double));
  this->antennaMetalLengthLayer_ = (char**)malloc(sizeof(char*));

}


lefiPin::~lefiPin() {
  this->lefiPin::Destroy();
}


void lefiPin::Destroy() {
  this->lefiPin::clear();
  free(this->name_);
  free((char*)(this->ports_));
  free(this->foreign_);
  free(this->LEQ_);
  free(this->mustjoin_);
  free(this->lowTable_);
  free(this->highTable_);
  if (this->propNames_)
     free((char*)(this->propNames_));
  if (this->propValues_)
     free((char*)(this->propValues_));
  if (this->propNums_)
     free((char*)(this->propNums_));
  free((char*)(this->antennaSize_));
  free((char*)(this->antennaSizeLayer_));
  free((char*)(this->antennaMetalArea_));
  free((char*)(this->antennaMetalAreaLayer_));
  free((char*)(this->antennaMetalLength_));
  free((char*)(this->antennaMetalLengthLayer_));
}


void lefiPin::clear() {
  int i;
  lefiGeometries* g;
  for (i = 0; i < this->numPorts_; i++) {
    g = this->ports_[i];
    g->lefiGeometries::Destroy();
    free((char*)g);
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

  for (i = 0; i < this->numAntennaSize_; i++)
    free(this->antennaSizeLayer_[i]);
  this->numAntennaSize_ = 0;

  for (i = 0; i < this->numAntennaMetalLength_; i++)
    free(this->antennaMetalLengthLayer_[i]);
  this->numAntennaMetalLength_ = 0;

  for (i = 0; i < this->numAntennaMetalArea_; i++)
    free(this->antennaMetalAreaLayer_[i]);
  this->numAntennaMetalArea_ = 0;

  for (i = 0; i < this->numProperties_; i++) {
    free(this->propNames_[i]);
    free(this->propValues_[i]);
  }
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
  if (this->taperRule_) { free(this->taperRule_); this->taperRule_ = 0; }

}


void lefiPin::bump(char** array, int len, int* size) {
  if (*array) free(*array);
  *array = (char*)malloc(len);
  *size = len;
}
 

void lefiPin::setName(const char* name) {
  int len = strlen(name) + 1;
  this->lefiPin::clear();
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char*)malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
}


void lefiPin::addPort(lefiGeometries* g) {
  if (this->numPorts_ == this->portsAllocated_) {
    int i;
    lefiGeometries** ng;
    if (this->portsAllocated_ == 0)
       this->portsAllocated_ = 2;
    else
       this->portsAllocated_ *= 2;
    ng = (lefiGeometries**)malloc(
       sizeof(lefiGeometries*)*this->portsAllocated_);
    for (i = 0; i < this->numPorts_; i++)
      ng[i] = this->ports_[i];
    free((char*)(this->ports_));
    this->ports_ = ng;
  }
  this->ports_[this->numPorts_++] = g;
}


void lefiPin::setForeign(const char* name, int hasPnt,
              double x, double y, int orient) {
  // orient=-1 means no orient was specified.
  int len = strlen(name) + 1;
 
  this->hasForeignPoint_ = hasPnt;
  this->foreignOrient_ = orient;
  this->foreignX_ = x;
  this->foreignY_ = y;
  this->hasForeign_ = 1;
 
  if (len > this->foreignSize_)
    this->lefiPin::bump(&(this->foreign_), len, &(this->foreignSize_));
  strcpy(this->foreign_, CASE(name));
 
  if (orient != -1)
    this->hasForeignOrient_ = 1; 
}


void lefiPin::setLEQ(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->LEQSize_)
    this->lefiPin::bump(&(this->LEQ_), len, &(this->LEQSize_));
  strcpy(this->LEQ_, CASE(name));
  this->hasLEQ_ = 1;
}


void lefiPin::setDirection(const char* name) {
  strcpy(this->direction_, CASE(name));
  this->hasDirection_ = 1;
}


void lefiPin::setUse(const char* name) {
  strcpy(this->use_, CASE(name));
  this->hasUse_ = 1;
}


void lefiPin::setShape(const char* name) {
  strcpy(this->shape_, CASE(name));
  this->hasShape_ = 1;
}


void lefiPin::setMustjoin(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->mustjoinSize_)
    this->lefiPin::bump(&(this->mustjoin_), len, &(this->mustjoinSize_));
  strcpy(this->mustjoin_, CASE(name));
  this->hasMustjoin_ = 1;
}


void lefiPin::setOutMargin(double high, double low) {
  this->outMarginH_ = high;
  this->outMarginL_ = low;
  this->hasOutMargin_ = 1;
}


void lefiPin::setOutResistance(double high, double low) {
  this->outResistanceH_ = high;
  this->outResistanceL_ = low;
  this->hasOutResistance_ = 1;
}


void lefiPin::setInMargin(double high, double low) {
  this->inMarginH_ = high;
  this->inMarginL_ = low;
  this->hasInMargin_ = 1;
}


void lefiPin::setPower(double power) {
  this->power_ = power;
  this->hasPower_ = 1;
}


void lefiPin::setLeakage(double current) {
  this->leakage_ = current;
  this->hasLeakage_ = 1;
}


void lefiPin::setMaxload(double capacitance) {
  this->maxload_ = capacitance;
  this->hasMaxload_ = 1;
}


void lefiPin::setMaxdelay(double dtime) {
  this->maxdelay_ = dtime;
  this->hasMaxdelay_ = 1;
}


void lefiPin::setCapacitance(double capacitance) {
  this->capacitance_ = capacitance;
  this->hasCapacitance_ = 1;
}


void lefiPin::setResistance(double resistance) {
  this->resistance_ = resistance;
  this->hasResistance_ = 1;
}


void lefiPin::setPulldownres(double resistance) {
  this->pulldownres_ = resistance;
  this->hasPulldownres_ = 1;
}


void lefiPin::setTieoffr(double resistance) {
  this->tieoffr_ = resistance;
  this->hasTieoffr_ = 1;
}


void lefiPin::setVHI(double voltage) {
  this->VHI_ = voltage;
  this->hasVHI_ = 1;
}


void lefiPin::setVLO(double voltage) {
  this->VLO_ = voltage;
  this->hasVLO_ = 1;
}


void lefiPin::setRiseVoltage(double voltage) {
  this->riseVoltage_ = voltage;
  this->hasRiseVoltage_ = 1;
}


void lefiPin::setFallVoltage(double voltage) {
  this->fallVoltage_ = voltage;
  this->hasFallVoltage_ = 1;
}


void lefiPin::setFallSlewLimit(double num) {
  this->riseSlewLimit_ = num;
  this->hasFallSlewLimit_ = 1;
}


void lefiPin::setRiseSlewLimit(double num) {
  this->riseSlewLimit_ = num;
  this->hasRiseSlewLimit_ = 1;
}


void lefiPin::setRiseThresh(double capacitance) {
  this->riseThresh_ = capacitance;
  this->hasRiseThresh_ = 1;
}


void lefiPin::setTaperRule(const char* name) {
  int len = strlen(name) + 1;
  this->taperRule_ = (char*)malloc(len);
  strcpy(this->taperRule_, name);
}


void lefiPin::setFallThresh(double capacitance) {
  this->fallThresh_ = capacitance;
  this->hasFallThresh_ = 1;
}


void lefiPin::setRiseSatcur(double current) {
  this->riseSatcur_ = current;
  this->hasRiseSatcur_ = 1;
}


void lefiPin::setFallSatcur(double current) {
  this->fallSatcur_ = current;
  this->hasFallSatcur_ = 1;
}


void lefiPin::setCurrentSource(const char* name) {
  strcpy(this->currentSource_, CASE(name));
  this->hasCurrentSource_ = 1;
}


void lefiPin::setTables(const char* highName, const char* lowName) {
  int len = strlen(highName) + 1;
  if (len > this->highTableSize_)
    this->lefiPin::bump(&(this->highTable_), len, &(this->highTableSize_));
  strcpy(this->highTable_, CASE(highName));
  len = strlen(lowName) + 1;
  if (len > this->lowTableSize_)
    this->lefiPin::bump(&(this->lowTable_), len, &(this->lowTableSize_));
  strcpy(this->lowTable_, CASE(lowName));
  this->hasTables_ = 1;
}


void lefiPin::setProperty(const char* name, const char* value) {
  int len;
  if (this->numProperties_ == this->propertiesAllocated_)
    this->lefiPin::bumpProps();
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char*)malloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProperties_] = (char*)malloc(len);
  strcpy(this->propValues_[this->numProperties_], CASE(value));
  this->numProperties_ += 1;
}


void lefiPin::setNumProperty(const char* name, double value) {
  int len;
 
  if (this->numProperties_ == this->propertiesAllocated_)
    this->lefiPin::bumpProps();
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char*)malloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  this->propValues_[this->numProperties_] = 0;
  this->propNums_[this->numProperties_] = value;
  this->numProperties_ += 1;
}


void lefiPin::bumpProps() {
  int lim = this->propertiesAllocated_;
  int news ;
  char** newpn;
  char** newpv;
  double* newd;

  news = lim ? lim + lim : 2;

  newpn = (char**)malloc(sizeof(char*)*news);
  newpv = (char**)malloc(sizeof(char*)*news);
  newd = (double*)malloc(sizeof(double)*news);
 
  lim = this->propertiesAllocated_ = news;
 
  if (lim > 2) {
     int i;
     for (i = 0; i < this->numProperties_; i++) {
       newpn[i] = this->propNames_[i];
       newpv[i] = this->propValues_[i];
       newd[i] = this->propNums_[i];
     }
     free((char*)(this->propNames_));
     free((char*)(this->propValues_));
     free((char*)(this->propNums_));
  }
  this->propNames_ = newpn;
  this->propValues_ = newpv;
  this->propNums_ = newd;
}
 

int lefiPin::hasForeign() const {
  return this->hasForeign_;
}


int lefiPin::hasForeignOrient() const {
  return this->hasForeignOrient_;
}


int lefiPin::hasForeignPoint() const {
  return this->hasForeignPoint_;
}


int lefiPin::hasLEQ() const {
  return this->hasLEQ_;
}


int lefiPin::hasDirection() const {
  return this->hasDirection_;
}


int lefiPin::hasUse() const {
  return this->hasUse_;
}


int lefiPin::hasShape() const {
  return this->hasShape_;
}


int lefiPin::hasMustjoin() const {
  return this->hasMustjoin_;
}


int lefiPin::hasOutMargin() const {
  return this->hasOutMargin_;
}


int lefiPin::hasOutResistance() const {
  return this->hasOutResistance_;
}


int lefiPin::hasInMargin() const {
  return this->hasInMargin_;
}


int lefiPin::hasPower() const {
  return this->hasPower_;
}


int lefiPin::hasLeakage() const {
  return this->hasLeakage_;
}


int lefiPin::hasMaxload() const {
  return this->hasMaxload_;
}


int lefiPin::hasMaxdelay() const {
  return this->hasMaxdelay_;
}


int lefiPin::hasCapacitance() const {
  return this->hasCapacitance_;
}


int lefiPin::hasResistance() const {
  return this->hasResistance_;
}


int lefiPin::hasPulldownres() const {
  return this->hasPulldownres_;
}


int lefiPin::hasTieoffr() const {
  return this->hasTieoffr_;
}


int lefiPin::hasVHI() const {
  return this->hasVHI_;
}


int lefiPin::hasVLO() const {
  return this->hasVLO_;
}


int lefiPin::hasFallSlewLimit() const {
  return this->hasFallSlewLimit_;
}


int lefiPin::hasRiseSlewLimit() const {
  return this->hasRiseSlewLimit_;
}


int lefiPin::hasRiseVoltage() const {
  return this->hasRiseVoltage_;
}


int lefiPin::hasFallVoltage() const {
  return this->hasFallVoltage_;
}


int lefiPin::hasRiseThresh() const {
  return this->hasRiseThresh_;
}


int lefiPin::hasFallThresh() const {
  return this->hasFallThresh_;
}


int lefiPin::hasRiseSatcur() const {
  return this->hasRiseSatcur_;
}


int lefiPin::hasFallSatcur() const {
  return this->hasFallSatcur_;
}


int lefiPin::hasCurrentSource() const {
  return this->hasCurrentSource_;
}


int lefiPin::hasTables() const {
  return this->hasTables_;
}


int lefiPin::hasAntennaSize() const {
  return this->numAntennaSize_ ? 1 : 0;
}


int lefiPin::hasAntennaMetalLength() const {
  return this->numAntennaMetalLength_ ? 1 : 0;
}


int lefiPin::hasAntennaMetalArea() const {
  return this->numAntennaMetalArea_ ? 1 : 0;
}


int lefiPin::hasTaperRule() const {
  return this->taperRule_ ? 1 : 0 ;
}


const char* lefiPin::name() const {
  return this->name_;
}


const char* lefiPin::taperRule() const {
  return this->taperRule_;
}


int lefiPin::numPorts() const {
  return this->numPorts_;
}


lefiGeometries* lefiPin::port(int index) const {
  if (index <0 || index > this->numPorts_) {
    lefiError("bad pin number index");
    return 0;
  }
  return this->ports_[index];
}


const char* lefiPin::foreignName() const {
  return this->foreign_;
}


int lefiPin::foreignOrient() const {
  return this->foreignOrient_;
}


double lefiPin::foreignX() const {
  return this->foreignX_;
}


double lefiPin::foreignY() const {
  return this->foreignY_;
}


const char* lefiPin::LEQ() const {
  return this->LEQ_;
}


const char* lefiPin::direction() const {
  return this->direction_;
}


const char* lefiPin::use() const {
  return this->use_;
}


const char* lefiPin::shape() const {
  return this->shape_;
}


const char* lefiPin::mustjoin() const {
  return this->mustjoin_;
}


double lefiPin::outMarginHigh() const {
  return this->outMarginH_;
}


double lefiPin::outMarginLow() const {
  return this->outMarginL_;
}


double lefiPin::outResistanceHigh() const {
  return this->outResistanceH_;
}


double lefiPin::outResistanceLow() const {
  return this->outResistanceL_;
}


double lefiPin::inMarginHigh() const {
  return this->inMarginH_;
}


double lefiPin::inMarginLow() const {
  return this->inMarginL_;
}


double lefiPin::power() const {
  return this->power_;
}


double lefiPin::leakage() const {
  return this->leakage_;
}


double lefiPin::maxload() const {
  return this->maxload_;
}


double lefiPin::maxdelay() const {
  return this->maxdelay_;
}


double lefiPin::capacitance() const {
  return this->capacitance_;
}


double lefiPin::resistance() const {
  return this->resistance_;
}


double lefiPin::pulldownres() const {
  return this->pulldownres_;
}


double lefiPin::tieoffr() const {
  return this->tieoffr_;
}


double lefiPin::VHI() const {
  return this->VHI_;
}


double lefiPin::VLO() const {
  return this->VLO_;
}


double lefiPin::fallSlewLimit() const {
  return this->fallSlewLimit_;
}


double lefiPin::riseSlewLimit() const {
  return this->riseSlewLimit_;
}


double lefiPin::riseVoltage() const {
  return this->riseVoltage_;
}


double lefiPin::fallVoltage() const {
  return this->fallVoltage_;
}


double lefiPin::riseThresh() const {
  return this->riseThresh_;
}


double lefiPin::fallThresh() const {
  return this->fallThresh_;
}


double lefiPin::riseSatcur() const {
  return this->riseSatcur_;
}


double lefiPin::fallSatcur() const {
  return this->fallSatcur_;
}


const char* lefiPin::currentSource() const {
  return this->currentSource_;
}


const char* lefiPin::tableHighName() const {
  return this->highTable_;
}


const char* lefiPin::tableLowName() const {
  return this->lowTable_;
}


const char* lefiPin::antennaSizeLayer(int i) const {
  return this->antennaSizeLayer_[i];
}


const char* lefiPin::antennaMetalAreaLayer(int i) const {
  return this->antennaMetalAreaLayer_[i];
}


const char* lefiPin::antennaMetalLengthLayer(int i) const {
  return this->antennaMetalLengthLayer_[i];
}


int lefiPin::numAntennaSize() const {
  return this->numAntennaSize_;
}


int lefiPin::numAntennaMetalArea() const {
  return this->numAntennaMetalArea_;
}


int lefiPin::numAntennaMetalLength() const {
  return this->numAntennaMetalLength_;
}


double lefiPin::antennaSize(int i) const {
  return this->antennaSize_[i];
}


double lefiPin::antennaMetalArea(int i) const {
  return this->antennaMetalArea_[i];
}


double lefiPin::antennaMetalLength(int i) const {
  return this->antennaMetalLength_[i];
}


void lefiPin::addAntennaMetalLength(double val, const char* layer) {
  int len;
  if (this->numAntennaMetalLength_ == this->antennaMetalLengthAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaMetalLength_;
    double* nd;
    char** nl;

    max = this->antennaMetalLengthAllocated_ *= 2;
    nd = (double*)malloc(sizeof(double)*max);
    nl = (char**)malloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaMetalLengthLayer_[i];
      nd[i] = this->antennaMetalLength_[i];
    }
    free((char*)(this->antennaMetalLengthLayer_));
    free((char*)(this->antennaMetalLength_));
    this->antennaMetalLengthLayer_ = nl;
    this->antennaMetalLength_ = nd;
  }
  this->antennaMetalLength_[this->numAntennaMetalLength_] = val;
  if (layer) {    // layer can be null, since it is optional
     len = strlen(layer) + 1;
     this->antennaMetalLengthLayer_[this->numAntennaMetalLength_] =
        (char*)malloc(len);
     strcpy(this->antennaMetalLengthLayer_[this->numAntennaMetalLength_],
        layer);
  } else
    this->antennaSizeLayer_[this->numAntennaSize_] = NULL;
  this->numAntennaMetalLength_ += 1;
}


void lefiPin::addAntennaSize(double val, const char* layer) {
  int len;
  if (this->numAntennaSize_ == this->antennaSizeAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaSize_;
    double* nd;
    char** nl;

    max = this->antennaSizeAllocated_ *= 2;
    nd = (double*)malloc(sizeof(double)*max);
    nl = (char**)malloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaSizeLayer_[i];
      nd[i] = this->antennaSize_[i];
    }
    free((char*)(this->antennaSizeLayer_));
    free((char*)(this->antennaSize_));
    this->antennaSizeLayer_ = nl;
    this->antennaSize_ = nd;
  }
  this->antennaSize_[this->numAntennaSize_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaSizeLayer_[this->numAntennaSize_] =
       (char*)malloc(len);
    strcpy(this->antennaSizeLayer_[this->numAntennaSize_],
       layer);
  } else
    this->antennaSizeLayer_[this->numAntennaSize_] = NULL;
  this->numAntennaSize_ += 1;
}


void lefiPin::addAntennaMetalArea(double val, const char* layer) {
  int len;
  if (this->numAntennaMetalArea_ == this->antennaMetalAreaAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaMetalArea_;
    double* nd;
    char** nl;

    max = this->antennaMetalAreaAllocated_ *= 2;
    nd = (double*)malloc(sizeof(double)*max);
    nl = (char**)malloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaMetalAreaLayer_[i];
      nd[i] = this->antennaMetalArea_[i];
    }
    free((char*)(this->antennaMetalAreaLayer_));
    free((char*)(this->antennaMetalArea_));
    this->antennaMetalAreaLayer_ = nl;
    this->antennaMetalArea_ = nd;
  }
  this->antennaMetalArea_[this->numAntennaMetalArea_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaMetalAreaLayer_[this->numAntennaMetalArea_] =
       (char*)malloc(len);
    strcpy(this->antennaMetalAreaLayer_[this->numAntennaMetalArea_],
       layer);
  } else
    this->antennaSizeLayer_[this->numAntennaSize_] = NULL;
  this->numAntennaMetalArea_ += 1;
}


int lefiPin::numProperties() const {
  return this->numProperties_;
}


const char* lefiPin::propName(int index) const {
  if (index < 0 || index >= this->numProperties_)
    { lefiError("ERROR: bad property number index"); return 0; }
  return this->propNames_[index];
}
 
 
const char* lefiPin::propValue(int index) const {
  if (index < 0 || index >= this->numProperties_)
    { lefiError("ERROR: bad property number index"); return 0; }
  return this->propValues_[index];
}


double lefiPin::propNum(int index) const {
  if (index < 0 || index >= this->numProperties_)
    { lefiError("ERROR: bad property number index"); return 0; }
  return this->propNums_[index];
}
 

void lefiPin::print(FILE* f) const {
  int i;
  lefiGeometries* g;

  fprintf(f, "  Pin %s\n", this->lefiPin::name());

  for (i = 0; i < this->lefiPin::numPorts(); i++) {
    fprintf(f, "    Port %d ", i);
    g = this->lefiPin::port(i);
    g->lefiGeometries::print(f);
  }

}




////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiMacro
//
////////////////////////////////////////////
////////////////////////////////////////////


lefiMacro::lefiMacro() {
  this->lefiMacro::Init();
}


void lefiMacro::Init() {
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

  this->lefiMacro::bump(&(this->name_), 16, &(this->nameSize_));
  this->lefiMacro::bump(&(this->generator_), 16, &(this->generatorSize_));
  this->lefiMacro::bump(&(this->EEQ_), 16, &(this->EEQSize_));
  this->lefiMacro::bump(&(this->LEQ_), 16, &(this->LEQSize_));
  this->lefiMacro::bump(&(this->gen1_), 16, &(this->gen1Size_));
  this->lefiMacro::bump(&(this->gen2_), 16, &(this->gen2Size_));
  this->lefiMacro::bump(&(this->foreign_), 16, &(this->foreignSize_));
  this->lefiMacro::bump(&(this->siteName_), 16, &(this->siteNameSize_));
  this->lefiMacro::bump(&(this->clockType_), 16, &(this->clockTypeSize_));

  this->propertiesAllocated_ = 2;
  this->numProperties_ = 0;
  this->propNames_ = (char**)malloc(sizeof(char*)*2);
  this->propValues_ = (char**)malloc(sizeof(char*)*2);
  this->propNums_ = (double*)malloc(sizeof(double)*2);

  this->numSites_ = 0;
  this->sitesAllocated_ = 0;
  this->pattern_ = 0;
}


void lefiMacro::Destroy() {
  this->lefiMacro::clear();
  free(this->name_);
  free(this->generator_);
  free(this->EEQ_);
  free(this->LEQ_);
  free(this->gen1_);
  free(this->gen2_);
  free(this->clockType_);
  free((char*)(this->propNames_));
  free((char*)(this->propValues_));
  free((char*)(this->propNums_));
  if (this->pattern_)
     free((char*)(this->pattern_));
}


lefiMacro::~lefiMacro() {
  this->lefiMacro::Destroy();
}


void lefiMacro::clear() {
  int i;

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
    for (i = 0; i < this->numSites_; i++) {
       this->pattern_[i]->lefiSitePattern::Destroy();
       free((char*)(this->pattern_[i]));
    }
    this->pattern_ = 0;
    this->numSites_ = 0;
    this->sitesAllocated_ = 0;
  }

  for (i = 0; i < this->numProperties_; i++) {
    free(this->propNames_[i]);
    free(this->propValues_[i]);
  }
  this->numProperties_ = 0;

}


void lefiMacro::bump(char** array, int len, int* size) {
  if (*array) free(*array);
  *array = (char*)malloc(len);
  *size = len;
}

void lefiMacro::setName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_)
    this->lefiMacro::bump(&(this->name_), len, &(this->nameSize_));
  strcpy(this->name_, CASE(name));
}


void lefiMacro::setGenerate(const char* name, const char* n2) {
  int len = strlen(name) + 1;
  if (len > this->gen1Size_)
    this->lefiMacro::bump(&(this->gen1_), len, &(this->gen1Size_));
  strcpy(this->gen1_, CASE(name));
  len = strlen(n2) + 1;
  if (len > this->gen2Size_)
    this->lefiMacro::bump(&(this->gen2_), len, &(this->gen2Size_));
  strcpy(this->gen2_, n2);
}


void lefiMacro::setGenerator(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->generatorSize_)
    this->lefiMacro::bump(&(this->generator_), len, &(this->generatorSize_));
  strcpy(this->generator_, CASE(name));
  this->hasGenerator_ = 1;
}


void lefiMacro::setInverter() {
  this->isInverter_ = 1;
}


void lefiMacro::setBuffer() {
  this->isBuffer_ = 1;
}


void lefiMacro::setSource(const char* name) {
  strcpy(this->source_, CASE(name));
  this->hasSource_ = 1;
}


void lefiMacro::setClass(const char* name) {
  strcpy(this->macroClass_, CASE(name));
  this->hasClass_ = 1;
}


void lefiMacro::setOrigin(double x, double y) {
  this->originX_ = x;
  this->originY_ = y;
  this->hasOrigin_ = 1;
}


void lefiMacro::setPower(double p) {
  this->power_ = p;
  this->hasPower_ = 1;
}


void lefiMacro::setEEQ(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->EEQSize_)
    this->lefiMacro::bump(&(this->EEQ_), len, &(this->EEQSize_));
  strcpy(this->EEQ_, CASE(name));
  this->hasEEQ_ = 1;
}


void lefiMacro::setLEQ(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->LEQSize_)
    this->lefiMacro::bump(&(this->LEQ_), len, &(this->LEQSize_));
  strcpy(this->LEQ_, CASE(name));
  this->hasLEQ_ = 1;
}


void lefiMacro::setProperty(const char* name, const char* value) {
  int len;
  if (this->numProperties_ == this->propertiesAllocated_)
    this->lefiMacro::bumpProps();
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char*)malloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProperties_] = (char*)malloc(len);
  strcpy(this->propValues_[this->numProperties_], CASE(value));
  this->propNums_[this->numProperties_] = 0.0;
  this->numProperties_ += 1;
}


void lefiMacro::setNumProperty(const char* name, double value) {
  int len;

  if (this->numProperties_ == this->propertiesAllocated_)
    this->lefiMacro::bumpProps();
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char*)malloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  this->propValues_[this->numProperties_] = 0;
  this->propNums_[this->numProperties_] = value;
  this->numProperties_ += 1;
}

 
void lefiMacro::bumpProps() {
  int lim = this->propertiesAllocated_;
  int news = lim + lim;
  char** newpn = (char**)malloc(sizeof(char*)*news);
  char** newpv = (char**)malloc(sizeof(char*)*news);
  double* newd = (double*)malloc(sizeof(double)*news);
  int i;

  this->propertiesAllocated_ = news;

  for (i = 0; i < lim; i++) {
    newpn[i] = this->propNames_[i];
    newpv[i] = this->propValues_[i];
    newd[i] = this->propNums_[i];
  }
  free((char*)(this->propNames_));
  free((char*)(this->propValues_));
  free((char*)(this->propNums_));
  this->propNames_ = newpn;
  this->propValues_ = newpv;
  this->propNums_ = newd;
}


void lefiMacro::setForeign(const char* name, int hasPnt,
               double x, double y, int orient) {
  // orient=-1 means no orient was specified.
  int len = strlen(name) + 1;

  this->hasForeignPoint_ = hasPnt;
  this->foreignOrient_ = orient;
  this->foreignX_ = x;
  this->foreignY_ = y;
  this->hasForeign_ = 1;

  if (len > this->foreignSize_) 
    this->lefiMacro::bump(&(this->foreign_), len, &(this->foreignSize_));
  strcpy(this->foreign_, CASE(name));

}

 
void lefiMacro::setXSymmetry() {
  this->hasSymmetry_ |= 1;
}


void lefiMacro::setYSymmetry() {
  this->hasSymmetry_ |= 2;
}


void lefiMacro::set90Symmetry() {
  this->hasSymmetry_ |= 4;
}


void lefiMacro::setSiteName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->siteNameSize_) 
    this->lefiMacro::bump(&(this->siteName_), len, &(this->siteNameSize_));
  strcpy(this->siteName_, CASE(name));
  this->hasSiteName_ = 1;
}


void lefiMacro::setClockType(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->clockTypeSize_)
    this->lefiMacro::bump(&(this->clockType_), len, &(this->clockTypeSize_));
  strcpy(this->clockType_, CASE(name));
  this->hasClockType_ = 1;
}


void lefiMacro::setSitePattern(lefiSitePattern* p) {
  if (this->numSites_ == this->sitesAllocated_) {
     lefiSitePattern** np;
     int i, lim;
     if (this->sitesAllocated_ == 0) {
        lim = this->sitesAllocated_ = 4;
        np = (lefiSitePattern**) malloc(sizeof(lefiSitePattern*)*lim);
     } else {
        lim = this->sitesAllocated_ * 2;
        this->sitesAllocated_ = lim;
        np = (lefiSitePattern**)malloc(sizeof(lefiSitePattern*)*lim);
        lim /= 2;
        for (i = 0; i < lim; i++)
          np[i] = this->pattern_[i];
        free((char*)(this->pattern_));
     }
     this->pattern_ = np;
  }
  this->pattern_[this->numSites_] = p;
  this->numSites_ += 1;
}


void lefiMacro::setSize(double x, double y) {
  this->hasSize_ = 1;
  this->sizeX_ = x;
  this->sizeY_ = y;
}
 

int lefiMacro::hasClass() const {
  return this->hasClass_;
}


int lefiMacro::hasSiteName() const {
  return this->hasSiteName_;
}


int lefiMacro::hasGenerator() const {
  return this->hasGenerator_;
}


int lefiMacro::hasPower() const {
  return this->hasPower_;
}


int lefiMacro::hasOrigin() const {
  return this->hasOrigin_;
}


int lefiMacro::hasSource() const {
  return this->hasSource_;
}


int lefiMacro::hasEEQ() const {
  return this->hasEEQ_;
}


int lefiMacro::hasLEQ() const {
  return this->hasLEQ_;
}


int lefiMacro::hasXSymmetry() const {
  return (this->hasSymmetry_ & 1) ? 1 : 0 ;
}


int lefiMacro::hasYSymmetry() const {
  return (this->hasSymmetry_ & 2) ? 1 : 0 ;
}


int lefiMacro::has90Symmetry() const {
  return (this->hasSymmetry_ & 4) ? 1 : 0 ;
}


int lefiMacro::hasSitePattern() const {
  return (this->pattern_) ? 1 : 0 ;
}


int lefiMacro::hasSize() const {
  return this->hasSize_;
}


int lefiMacro::hasForeign() const {
  return this->hasForeign_;
}


int lefiMacro::hasForeignOrigin() const {
  return this->hasForeignOrigin_;
}


int lefiMacro::hasForeignOrient() const {
  return (this->foreignOrient_ == -1) ? 0 : 1 ;
}


int lefiMacro::hasForeignPoint() const {
  return this->hasForeignPoint_;
}


int lefiMacro::hasClockType() const {
  return this->hasClockType_;
}


int lefiMacro::numSitePattern() const {
  return this->numSites_;
}


int lefiMacro::numProperties() const {
  return this->numProperties_;
}


const char* lefiMacro::propName(int index) const {
  if (index < 0 || index >= this->numProperties_)
    { lefiError("ERROR: bad property number index"); return 0; }
  return this->propNames_[index];
}


const char* lefiMacro::propValue(int index) const {
  if (index < 0 || index >= this->numProperties_)
    { lefiError("ERROR: bad property number index"); return 0; }
  return this->propValues_[index];
}


double lefiMacro::propNum(int index) const {
  if (index < 0 || index >= this->numProperties_)
    { lefiError("ERROR: bad property number index"); return 0; }
  return this->propNums_[index];
}


const char* lefiMacro::name() const {
  return this->name_;
}


const char* lefiMacro::macroClass() const {
  return this->macroClass_;
}


const char* lefiMacro::generator() const {
  return this->generator_;
}


const char* lefiMacro::EEQ() const {
  return this->EEQ_;
}


const char* lefiMacro::LEQ() const {
  return this->LEQ_;
}


const char* lefiMacro::source() const {
  return this->source_;
}


double lefiMacro::originX() const {
  return this->originX_;
}


double lefiMacro::originY() const {
  return this->originY_;
}


double lefiMacro::power() const {
  return this->power_;
}


void lefiMacro::generate(char** name1, char** name2) const {
  if (name1) *name1 = this->gen1_;
  if (name2) *name2 = this->gen2_;
}


lefiSitePattern* lefiMacro::sitePattern(int index) const {
  return this->pattern_[index];
}


const char* lefiMacro::siteName() const {
  return this->siteName_;
}


double lefiMacro::sizeX() const {
  return this->sizeX_;
}


double lefiMacro::sizeY() const {
  return this->sizeY_;
}


int lefiMacro::foreignOrient() const {
  return this->foreignOrient_;
}


double lefiMacro::foreignX() const {
  return this->foreignX_;
}


double lefiMacro::foreignY() const {
  return this->foreignY_;
}


const char* lefiMacro::foreignName() const {
  return this->foreign_;
}


const char* lefiMacro::clockType() const {
  return this->clockType_;
}


int lefiMacro::isBuffer() const {
  return this->isBuffer_;
}


int lefiMacro::isInverter() const {
  return this->isInverter_;
}


void lefiMacro::print(FILE* f) const {
  char* c1;
  char* c2;
  lefiSitePattern* sp;
  int i;

  fprintf(f, "MACRO %s\n", this->lefiMacro::name());

  if (this->lefiMacro::hasClass())
    fprintf(f, "  Class %s\n", this->lefiMacro::macroClass());

  if (this->lefiMacro::hasGenerator())
    fprintf(f, "  Generator %s\n", this->lefiMacro::generator());

  if (this->lefiMacro::hasGenerator()) {
    this->lefiMacro::generate(&c1, &c2);
    fprintf(f, "  Generate %s %s\n", c1, c2);
  }

  if (this->lefiMacro::hasPower())
    fprintf(f, "  Power %g\n", this->lefiMacro::power());

  if (this->lefiMacro::hasOrigin())
    fprintf(f, "  Origin %g,%g\n", this->lefiMacro::originX(),
       this->lefiMacro::originY());

  if (this->lefiMacro::hasEEQ())
    fprintf(f, "  EEQ %s\n", this->lefiMacro::EEQ());

  if (this->lefiMacro::hasLEQ())
    fprintf(f, "  LEQ %s\n", this->lefiMacro::LEQ());

  if (this->lefiMacro::hasSource())
    fprintf(f, "  Source %s\n", this->lefiMacro::source());

  if (this->lefiMacro::hasXSymmetry())
    fprintf(f, "  Symmetry X\n");

  if (this->lefiMacro::hasYSymmetry())
    fprintf(f, "  Symmetry Y\n");

  if (this->lefiMacro::has90Symmetry())
    fprintf(f, "  Symmetry R90\n");

  if (this->lefiMacro::hasSiteName())
    fprintf(f, "  Site name %s\n", this->lefiMacro::siteName());

  if (this->lefiMacro::hasSitePattern()) {
    for (i = 0; i < this->lefiMacro::numSitePattern(); i++) {
      sp = this->lefiMacro::sitePattern(i);
      fprintf(f, "  Site pattern ");
      sp->lefiSitePattern::print(f);
    }
  }

  if (this->lefiMacro::hasSize())
    fprintf(f, "  Size %g,%g\n", this->lefiMacro::sizeX(),
       this->lefiMacro::sizeY());

  if (this->lefiMacro::hasForeign()) {
    fprintf(f, "  Foreign %s", this->lefiMacro::foreignName());
    if (this->lefiMacro::hasForeignOrigin())
      fprintf(f, "  %g,%g", this->lefiMacro::foreignX(),
      this->lefiMacro::foreignY());
    if (this->lefiMacro::hasForeignOrient())
      fprintf(f, "  orient %d", this->lefiMacro::foreignOrient());
    fprintf(f, "\n");
  }

  if (this->lefiMacro::hasClockType())
    fprintf(f, "  Clock type %s\n", this->lefiMacro::clockType());

  fprintf(f, "END MACRO %s\n", this->lefiMacro::name());
}


////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiTiming
//
////////////////////////////////////////////
////////////////////////////////////////////


lefiTiming::lefiTiming() {
  this->lefiTiming::Init();
}


void lefiTiming::Init() {
  this->numFrom_ = 0;
  this->from_ = (char**)malloc(sizeof(char*));
  this->fromAllocated_ = 1;
  this->numTo_ = 0;
  this->to_ = (char**)malloc(sizeof(char*));
  this->toAllocated_ = 1;

  this->numOfAxisNumbers_ = 0;
  this->axisNumbers_ = (double*)malloc(sizeof(double));
  this->axisNumbersAllocated_ = 1;

  this->numOfTableEntries_ = 0;
  this->tableEntriesAllocated_ = 1;
  this->table_ = (double*)malloc(sizeof(double)*3);  /* three numbers per entry */

  this->lefiTiming::clear();
}


void lefiTiming::Destroy() {
  this->lefiTiming::clear();
  free((char*)(this->from_));
  free((char*)(this->to_));
  free((char*)(this->axisNumbers_));
  free((char*)(this->table_));
}


lefiTiming::~lefiTiming() {
  this->lefiTiming::Destroy();
}


void lefiTiming::addRiseFall(const char* risefall, double one, double two) {
  if (*risefall == 'r' || *risefall == 'R') {
    this->hasRiseIntrinsic_ = 1;
    this->nowRise_ = 1;
    this->riseIntrinsicOne_ = one;
    this->riseIntrinsicTwo_ = two;
  } else {
    this->nowRise_ = 0;
    this->hasFallIntrinsic_ = 1;
    this->fallIntrinsicOne_ = one;
    this->fallIntrinsicTwo_ = two;
  }
}


void lefiTiming::addRiseFallVariable(double one, double two) {
  if (this->nowRise_ == 1) {
    this->riseIntrinsicThree_ = one;
    this->riseIntrinsicFour_ = two;
  } else {
    this->fallIntrinsicThree_ = one;
    this->fallIntrinsicFour_ = two;
  }
}


void lefiTiming::setRiseRS(double one, double two) {
  this->hasRiseRS_ = 1;
  this->riseRSOne_ = one;
  this->riseRSTwo_ = two;
}


void lefiTiming::setFallRS(double one, double two) {
  this->hasFallRS_ = 1;
  this->fallRSOne_ = one;
  this->fallRSTwo_ = two;
}


void lefiTiming::setRiseCS(double one, double two) {
  this->hasRiseCS_ = 1;
  this->riseCSOne_ = one;
  this->riseCSTwo_ = two;
}


void lefiTiming::setFallCS(double one, double two) {
  this->hasFallCS_ = 1;
  this->fallCSOne_ = one;
  this->fallCSTwo_ = two;
}


void lefiTiming::setRiseAtt1(double one, double two) {
  this->hasRiseAtt1_ = 1;
  this->riseAtt1One_ = one;
  this->riseAtt1Two_ = two;
}


void lefiTiming::setFallAtt1(double one, double two) {
  this->hasFallAtt1_ = 1;
  this->fallAtt1One_ = one;
  this->fallAtt1Two_ = two;
}


void lefiTiming::setRiseTo(double one, double two) {
  this->hasRiseTo_ = 1;
  this->riseToOne_ = one;
  this->riseToTwo_ = two;
}


void lefiTiming::setFallTo(double one, double two) {
  this->hasFallTo_ = 1;
  this->fallToOne_ = one;
  this->fallToTwo_ = two;
}


void lefiTiming::addUnateness(const char* typ) {
  this->hasUnateness_ = 1;
  this->unateness_ = (char*)typ;
}


void lefiTiming::setStable(double one, double two, const char* typ) {
  this->hasStableTiming_ = 1;
  this->stableSetup_ = one;
  this->stableHold_ = two;
  this->stableRiseFall_ = (char*)typ;
}


void lefiTiming::addTableEntry(double one, double two, double three) {
  int i;
  double *n;
  if (this->numOfTableEntries_ >= this->tableEntriesAllocated_) {
    int lim;

    lim = this->tableEntriesAllocated_ *= 2;
    n = (double*)malloc(sizeof(double)*3*lim);
    lim = this->numOfTableEntries_ * 3;
    for (i = 0; i < lim; i++) {
      n[i] = this->table_[i];
    }
    free((char*)(this->table_));
    this->table_ = n;
  }
  i = this->numOfTableEntries_ * 3;
  this->table_[i++] = one;
  this->table_[i++] = two;
  this->table_[i] = three;
  this->numOfTableEntries_ += 1;
}


void lefiTiming::addTableAxisNumber(double one) {
  if (this->numOfAxisNumbers_ == this->axisNumbersAllocated_) {
    int i;
    int lim;
    double* n;

    lim = this->axisNumbersAllocated_ *= 2;
    n = (double*)malloc(sizeof(double)*lim);
    lim = this->numOfAxisNumbers_;
    for (i = 0; i < lim; i++)
      n[i] = this->axisNumbers_[i];
    free((char*)(this->axisNumbers_));
    this->axisNumbers_ = n;
  }
  this->axisNumbers_[(this->numOfAxisNumbers_)++] = one;
}


void lefiTiming::addRiseFallSlew(double one, double two,
	   double three, double four) {
  if (this->nowRise_) {
    this->hasRiseSlew_ = 1;
    this->riseSlewOne_ = one;
    this->riseSlewTwo_ = two;
    this->riseSlewThree_ = three;
    this->riseSlewFour_ = four;
  } else {
    this->hasFallSlew_ = 1;
    this->fallSlewOne_ = one;
    this->fallSlewTwo_ = two;
    this->fallSlewThree_ = three;
    this->fallSlewFour_ = four;
  }
}


void lefiTiming::addRiseFallSlew2(double one, double two, double three) {
  if (this->nowRise_) {
    this->hasRiseSlew2_ = 1;
    this->riseSlewFive_ = one;
    this->riseSlewSix_ = two;
    this->riseSlewSeven_ = three;
  } else {
    this->hasFallSlew2_ = 1;
    this->fallSlewFive_ = one;
    this->fallSlewSix_ = two;
    this->fallSlewSeven_ = three;
  }
}


void lefiTiming::addFromPin(const char* name) {
  if (this->numFrom_ == this->fromAllocated_) {
    int lim;
    int i;
    char** n;

    lim = this->fromAllocated_ *= 2;
    n = (char**)malloc(sizeof(char*)*lim);
    lim = this->numFrom_;
    for (i = 0; i < lim; i++)
      n[i] = this->from_[i];
    free((char*)(this->from_));
    this->from_ = n;
  }
  this->from_[(this->numFrom_)++] = (char*)name;
}


void lefiTiming::addToPin(const char* name) {
  if (this->numTo_ == this->toAllocated_) {
    int lim;
    int i;
    char** n;

    lim = this->toAllocated_ *= 2;
    n = (char**)malloc(sizeof(char*)*lim);
    lim = this->numTo_;
    for (i = 0; i < lim; i++)
      n[i] = this->to_[i];
    free((char*)(this->to_));
    this->to_ = n;
  }
  this->to_[(this->numTo_)++] = (char*)name;
}


void lefiTiming::addDelay(const char* risefall, const char* unateness,
        double one, double two, double three) {
  this->hasDelay_ = 1;
  this->delayRiseOrFall_ = (char*)risefall;
  this->delayUnateness_ = (char*)unateness;
  this->delayTableOne_ = one;
  this->delayTableTwo_ = two;
  this->delayTableThree_ = three;
}


void lefiTiming::addTransition(const char* risefall, const char* unateness,
        double one, double two, double three) {
  this->hasTransition_ = 1;
  this->transitionRiseOrFall_ = (char*)risefall;
  this->transitionUnateness_ = (char*)unateness;
  this->transitionTableOne_ = one;
  this->transitionTableTwo_ = two;
  this->transitionTableThree_ = three;
}


void lefiTiming::addSDF1Pin(const char* trigType, double one, double two,
        double three) {
  this->hasSDFonePinTrigger_ = 1;
  this->SDFtriggerType_ = (char*) trigType;
  this->SDFtriggerTableOne_ = one;
  this->SDFtriggerTableTwo_ = two;
  this->SDFtriggerTableThree_ = three;
}


void lefiTiming::addSDF2Pins(const char* trigType, const char* fromTrig,
        const char* toTrig, double one, double two, double three) {
  this->hasSDFtwoPinTrigger_ = 1;
  this->SDFtriggerType_ = (char*) trigType;
  this->SDFfromTrigger_ = (char*) fromTrig;
  this->SDFtoTrigger_ = (char*) toTrig;
  this->SDFtriggerTableOne_ = one;
  this->SDFtriggerTableTwo_ = two;
  this->SDFtriggerTableThree_ = three;
}


void lefiTiming::setSDFcondStart(const char* condStart) {
  this->SDFcondStart_ = (char*)condStart;
}


void lefiTiming::setSDFcondEnd(const char* condEnd) {
  this->SDFcondEnd_ = (char*)condEnd;
}


void lefiTiming::setSDFcond(const char* cond) {
  this->SDFcond_ = (char*)cond;
}


int lefiTiming::hasData() {
  return ((this->numFrom_) ? 1 : 0 );
}


void lefiTiming::clear() {
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


int lefiTiming::numFromPins() {
  return this->numFrom_;
}


const char* lefiTiming::fromPin(int index) {
  return this->from_[index];
}


int lefiTiming::numToPins() {
  return this->numTo_;
}


const char* lefiTiming::toPin(int index) {
  return this->to_[index];
}


int lefiTiming::hasTransition() {
  return this->hasTransition_;
}


int lefiTiming::hasDelay() {
  return this->hasDelay_;
}


int lefiTiming::hasRiseSlew() {
  return this->hasRiseSlew_;
}


int lefiTiming::hasRiseSlew2() {
  return this->hasRiseSlew2_;
}


int lefiTiming::hasFallSlew() {
  return this->hasFallSlew_;
}


int lefiTiming::hasFallSlew2() {
  return this->hasFallSlew2_;
}


int lefiTiming::hasRiseIntrinsic() {
  return this->hasRiseIntrinsic_;
}


int lefiTiming::hasFallIntrinsic() {
  return this->hasFallIntrinsic_;
}


int lefiTiming::hasSDFonePinTrigger() {
  return this->hasSDFonePinTrigger_;
}


int lefiTiming::hasSDFtwoPinTrigger() {
  return this->hasSDFtwoPinTrigger_;
}


int lefiTiming::hasSDFcondStart() {
  return this->hasSDFcondStart_;
}


int lefiTiming::hasSDFcondEnd() {
  return this->hasSDFcondEnd_;
}


int lefiTiming::hasSDFcond() {
  return this->hasSDFcond_;
}


int lefiTiming::numOfAxisNumbers() {
  return this->numOfAxisNumbers_;
}


double* lefiTiming::axisNumbers() {
  return this->axisNumbers_;
}


int lefiTiming::numOfTableEntries() {
  return this->numOfTableEntries_;
}


void lefiTiming::tableEntry(int num, double* one, double* two,
          double* three) {
  num *= 3;
  *one = this->table_[num];
  num++;
  *two = this->table_[num];
  num++;
  *three = this->table_[num];
}


const char* lefiTiming::delayRiseOrFall() {
  return this->delayRiseOrFall_;
}


const char* lefiTiming::delayUnateness() {
  return this->delayUnateness_;
}


double lefiTiming::delayTableOne() {
  return this->delayTableOne_;
}


double lefiTiming::delayTableTwo() {
  return this->delayTableTwo_;
}


double lefiTiming::delayTableThree() {
  return this->delayTableThree_;
}


const char* lefiTiming::transitionRiseOrFall() {
  return this->transitionRiseOrFall_;
}


const char* lefiTiming::transitionUnateness() {
  return this->transitionUnateness_;
}


double lefiTiming::transitionTableOne() {
  return this->transitionTableOne_;
}


double lefiTiming::transitionTableTwo() {
  return this->transitionTableTwo_;
}


double lefiTiming::transitionTableThree() {
  return this->transitionTableThree_;
}


double lefiTiming::riseIntrinsicOne() {
  return this->riseIntrinsicOne_;
}


double lefiTiming::riseIntrinsicTwo() {
  return this->riseIntrinsicTwo_;
}


double lefiTiming::riseIntrinsicThree() {
  return this->riseIntrinsicThree_;
}


double lefiTiming::riseIntrinsicFour() {
  return this->riseIntrinsicFour_;
}


double lefiTiming::fallIntrinsicOne() {
  return this->fallIntrinsicOne_;
}


double lefiTiming::fallIntrinsicTwo() {
  return this->fallIntrinsicTwo_;
}


double lefiTiming::fallIntrinsicThree() {
  return this->fallIntrinsicThree_;
}


double lefiTiming::fallIntrinsicFour() {
  return this->fallIntrinsicFour_;
}


double lefiTiming::riseSlewOne() {
  return this->riseSlewOne_;
}


double lefiTiming::riseSlewTwo() {
  return this->riseSlewTwo_;
}


double lefiTiming::riseSlewThree() {
  return this->riseSlewThree_;
}


double lefiTiming::riseSlewFour() {
  return this->riseSlewFour_;
}


double lefiTiming::riseSlewFive() {
  return this->riseSlewFive_;
}


double lefiTiming::riseSlewSix() {
  return this->riseSlewSix_;
}


double lefiTiming::riseSlewSeven() {
  return this->riseSlewSeven_;
}


double lefiTiming::fallSlewOne() {
  return this->fallSlewOne_;
}


double lefiTiming::fallSlewTwo() {
  return this->fallSlewTwo_;
}


double lefiTiming::fallSlewThree() {
  return this->fallSlewThree_;
}


double lefiTiming::fallSlewFour() {
  return this->fallSlewFour_;
}


double lefiTiming::fallSlewFive() {
  return this->fallSlewFive_;
}


double lefiTiming::fallSlewSix() {
  return this->fallSlewSix_;
}


double lefiTiming::fallSlewSeven() {
  return this->fallSlewSeven_;
}


int lefiTiming::hasRiseRS() {
  return this->hasRiseRS_;
}


double lefiTiming::riseRSOne() {
  return this->riseRSOne_;
}


double lefiTiming::riseRSTwo() {
  return this->riseRSTwo_;
}


int lefiTiming::hasRiseCS() {
  return this->hasRiseCS_;
}


double lefiTiming::riseCSOne() {
  return this->riseCSOne_;
}


double lefiTiming::riseCSTwo() {
  return this->riseCSTwo_;
}


int lefiTiming::hasFallRS() {
  return this->hasFallRS_;
}


double lefiTiming::fallRSOne() {
  return this->fallRSOne_;
}


double lefiTiming::fallRSTwo() {
  return this->fallRSTwo_;
}


int lefiTiming::hasFallCS() {
  return this->hasFallCS_;
}


double lefiTiming::fallCSOne() {
  return this->fallCSOne_;
}


double lefiTiming::fallCSTwo() {
  return this->fallCSTwo_;
}


int lefiTiming::hasUnateness() {
  return this->hasUnateness_;
}


const char* lefiTiming::unateness() {
  return this->unateness_;
}


int lefiTiming::hasFallAtt1() {
  return this->hasFallAtt1_;
}


double lefiTiming::fallAtt1One() {
  return this->fallAtt1One_;
}


double lefiTiming::fallAtt1Two() {
  return this->fallAtt1Two_;
}


int lefiTiming::hasRiseAtt1() {
  return this->hasRiseAtt1_;
}


double lefiTiming::riseAtt1One() {
  return this->riseAtt1One_;
}


double lefiTiming::riseAtt1Two() {
  return this->riseAtt1Two_;
}


int lefiTiming::hasFallTo() {
  return this->hasFallTo_;
}


double lefiTiming::fallToOne() {
  return this->fallToOne_;
}


double lefiTiming::fallToTwo() {
  return this->fallToTwo_;
}


int lefiTiming::hasRiseTo() {
  return this->hasRiseTo_;
}


double lefiTiming::riseToOne() {
  return this->riseToOne_;
}


double lefiTiming::riseToTwo() {
  return this->riseToTwo_;
}


int lefiTiming::hasStableTiming() {
  return this->hasStableTiming_;
}


double lefiTiming::stableSetup() {
  return this->stableSetup_;
}


double lefiTiming::stableHold() {
  return this->stableHold_;
}


const char* lefiTiming::stableRiseFall() {
  return this->stableRiseFall_;
}


const char* lefiTiming::SDFonePinTriggerType() {
  return this->SDFtriggerType_;
}


const char* lefiTiming::SDFtwoPinTriggerType() {
  return this->SDFtriggerType_;
}


const char* lefiTiming::SDFfromTrigger() {
  return this->SDFfromTrigger_;
}


const char* lefiTiming::SDFtoTrigger() {
  return this->SDFtoTrigger_;
}


double lefiTiming::SDFtriggerOne() {
  return this->SDFtriggerTableOne_;
}


double lefiTiming::SDFtriggerTwo() {
  return this->SDFtriggerTableTwo_;
}


double lefiTiming::SDFtriggerThree() {
  return this->SDFtriggerTableThree_;
}


const char* lefiTiming::SDFcondStart() {
  return this->SDFcondStart_;
}


const char* lefiTiming::SDFcondEnd() {
  return this->SDFcondEnd_;
}


const char* lefiTiming::SDFcond() {
  return this->SDFcond_;
}
