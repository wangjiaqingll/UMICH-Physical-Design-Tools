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

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "lefiLayer.hpp"
#include "lefiDebug.hpp"

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiLayerDensity
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////

lefiLayerDensity::lefiLayerDensity() {
}

void lefiLayerDensity::Init(const char* type) {
  int len = strlen(type) + 1;
  this->type_ = (char*)malloc(len);
  strcpy(this->type_, CASE(type));
  this->oneEntry_ = 0;
  this->numFrequency_ = 0;
  this->frequency_ = 0;
  this->numWidths_ = 0;
  this->widths_ = 0;
  this->numTableEntries_ = 0;
  this->tableEntries_ = 0;
  this->numCutareas_ = 0;
  this->cutareas_ = 0;
}

void lefiLayerDensity::Destroy() {
  if (this->type_) free((char*)(this->type_));
  if (this->frequency_) free((char*)(this->frequency_));
  if (this->widths_) free((char*)(this->widths_));
  if (this->tableEntries_) free((char*)(this->tableEntries_));
  if (this->cutareas_) free((char*)(this->cutareas_));
}

lefiLayerDensity::~lefiLayerDensity() {
  this->lefiLayerDensity::Destroy();
}

void lefiLayerDensity::setOneEntry(double entry) {
  this->oneEntry_ = entry;
}

void lefiLayerDensity::addFrequency(int num, double* frequency) {
  this->numFrequency_ = num;
  this->frequency_ = frequency;
}

void lefiLayerDensity::addWidth(int num, double* width) {
  this->numWidths_ = num;
  this->widths_ = width;
}

void lefiLayerDensity::addTableEntry(int num, double* entry) {
  this->numTableEntries_ = num;
  this->tableEntries_ = entry;
}

void lefiLayerDensity::addCutarea(int num, double* cutarea) {
  this->numCutareas_ = num;
  this->cutareas_ = cutarea;
}

int lefiLayerDensity::hasOneEntry() {
  return this->oneEntry_ ? 1 : 0 ;
}

char* lefiLayerDensity::type() {
  return this->type_;
}

double lefiLayerDensity::oneEntry() {
  return this->oneEntry_;
}

int lefiLayerDensity::numFrequency() {
  return this->numFrequency_;
}

double lefiLayerDensity::frequency(int index) {
  return this->frequency_[index];
}

int lefiLayerDensity::numWidths() {
  return this->numWidths_;
}

double lefiLayerDensity::width(int index) {
  return this->widths_[index];
}

int lefiLayerDensity::numTableEntries() {
  return this->numTableEntries_;
}

double lefiLayerDensity::tableEntry(int index) {
  return this->tableEntries_[index];
}

int lefiLayerDensity::numCutareas() {
  return this->numCutareas_;
}

double lefiLayerDensity::cutArea(int index) {
  return this->cutareas_[index];
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiLayer
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////

lefiLayer::lefiLayer() {
  this->lefiLayer::Init();
}


void lefiLayer::Init() {
  this->name_ = (char*)malloc(16);
  this->nameSize_ = 16;
  this->type_ = (char*)malloc(16);
  this->typeSize_ = 16;
  this->numSpacings_ = 0;
  this->spacingsAllocated_ = 0;
  this->numCurrentPoints_ = 0;
  this->currentPointsAllocated_ = 2;
  this->currentWidths_ = (double*)malloc(sizeof(double)*2);
  this->current_ = (double*)malloc(sizeof(double)*2);
  this->numResistancePoints_ = 0;
  this->resistancePointsAllocated_ = 2;
  this->resistanceWidths_ = (double*)malloc(sizeof(double)*2);
  this->resistances_ = (double*)malloc(sizeof(double)*2);
  this->numCapacitancePoints_ = 0;
  this->capacitancePointsAllocated_ = 2;
  this->capacitanceWidths_ = (double*)malloc(sizeof(double)*2);
  this->capacitances_ = (double*)malloc(sizeof(double)*2);
  this->numProps_ = 0;
  this->propsAllocated_ = 1;
  this->names_ = (char**)malloc(sizeof(char*));
  this->values_ = (char**)malloc(sizeof(char*));
  this->lefiLayer::clear();
  this->numAccurrents_ = 0;
  this->accurrentAllocated_ = 0;
  this->numDccurrents_ = 0;
  this->dccurrentAllocated_ = 0;
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->lengthThreshold_ = 0;
}


void lefiLayer::Destroy() {
  this->lefiLayer::clear();
  free(this->name_);
  free(this->type_);
  if (this->spacing_) free((char*)(this->spacing_));
  if (this->spacingName_) free((char*)(this->spacingName_));
  free((char*)(this->currentWidths_));
  free((char*)(this->current_));
  free((char*)(this->resistanceWidths_));
  free((char*)(this->resistances_));
  free((char*)(this->capacitanceWidths_));
  free((char*)(this->capacitances_));
  free((char*)(this->names_));
  free((char*)(this->values_));
  if (this->nums_) free((char*)(this->nums_));
}


lefiLayer::~lefiLayer() {
  this->lefiLayer::Destroy();
}


void lefiLayer::clear() {
  int i;
  lefiLayerDensity* p;

  if (this->name_)
   *(this->name_) = 0;
  if (this->type_)
   *(this->type_) = 0;
  this->hasPitch_ = 0;
  this->hasOffset_ = 0;
  this->hasWidth_ = 0;
  this->hasWireExtension_ = 0;
  this->hasSpacing_ = 0;
  this->hasDirection_ = 0;
  this->hasResistance_ = 0;
  this->hasCapacitance_ = 0;
  this->hasHeight_ = 0;
  this->hasThickness_ = 0;
  this->hasShrinkage_ = 0;
  this->hasCapMultiplier_ = 0;
  this->hasEdgeCap_ = 0;
  this->hasAntennaArea_ = 0;
  this->hasAntennaLength_ = 0;
  this->hasCurrentDensityPoint_ = 0;
  this->hasUseLengthThreshold_ = 0;
  for (i = 0; i < this->numSpacings_; i++)
    if (this->spacingName_[i])
       free(this->spacingName_[i]);
  for (i = 0; i < this->numProps_; i++) {
    free(this->names_[i]);
    free(this->values_[i]);
  }
  this->numProps_ = 0;
  this->numSpacings_ = 0;
  this->numCurrentPoints_ = 0;
  if (this->numAccurrents_) {
     for (i = 0; i < this->numAccurrents_; i++) {
       p = this->accurrents_[i];
       p->lefiLayerDensity::Destroy();
       free((char*)p);
     }
     this->numAccurrents_ = 0;
     this->accurrentAllocated_ = 0;
     free((char*)(this->accurrents_));
     this->accurrents_ = 0;
  }
  if (this->numDccurrents_) {
     for (i = 0; i < this->numDccurrents_; i++) {
       p = this->dccurrents_[i];
       p->lefiLayerDensity::Destroy();
       free((char*)p);
     }
     this->numDccurrents_ = 0;
     this->dccurrentAllocated_ = 0;
     free((char*)(this->dccurrents_));
     this->dccurrents_ = 0;
  }
  this->lengthThreshold_ = 0;
}
  

void lefiLayer::setName(const char* name) {
  int len = strlen(name) + 1;
  this->lefiLayer::clear();
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char*)malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
}


void lefiLayer::setType(const char* typ) {
  int len = strlen(typ) + 1;
  if (len > this->typeSize_) {
    free(this->type_);
    this->type_ = (char*)malloc(len);
    this->typeSize_ = len;
  }
  strcpy(this->type_, CASE(typ));
}


void lefiLayer::setPitch(double num) {
  this->hasPitch_ = 1;
  this->pitch_ = num;
}


void lefiLayer::setOffset(double num) {
  this->hasOffset_ = 1;
  this->offset_ = num;
}


void lefiLayer::setWidth(double num) {
  this->hasWidth_ = 1;
  this->width_ = num;
}


void lefiLayer::setWireExtension(double num) {
  this->hasWireExtension_ = 1;
  this->wireExtension_ = num;
}


void lefiLayer::setSpacing(double dist, double left, double right) {
  if (this->numSpacings_ == this->spacingsAllocated_) {
    double* nd;
    char** nn;     // Also set up the spacing name
    int i, lim;
    if (this->spacingsAllocated_ == 0) {
       lim = this->spacingsAllocated_ = 2;
       nd = (double*)malloc(sizeof(double)*lim);   
       nn = (char**)malloc(sizeof(char*)*lim);
    } else {
       lim = this->spacingsAllocated_ * 2;
       this->spacingsAllocated_ = lim;
       nd = (double*)malloc(sizeof(double)*lim);
       nn = (char**)malloc(sizeof(char*)*lim);
       lim /= 2;
       for (i = 0; i < lim; i++) {
         nd[i] = this->spacing_[i];
         if (this->spacingName_[i])  // is null if is not CUT layer
           nn[i] = this->spacingName_[i];
       }
       free((char*)(this->spacing_));
       free((char*)(this->spacingName_));
    }
    this->spacing_ = nd;
    this->spacingName_ = nn;
  }
  this->hasSpacing_ = 1;
  this->spacing_[this->numSpacings_] = dist;
  this->spacingName_[this->numSpacings_] = 0;
  this->numSpacings_ += 1;
  this->left_ = left;
  this->right_ = right;
}


void lefiLayer::setSpacingName(const char* spacingName) {
  if (spacingName) {
     int len = strlen(spacingName) + 1;
     this->spacingName_[this->numSpacings_-1] = (char*)malloc(len);
     strcpy(this->spacingName_[this->numSpacings_-1], CASE(spacingName));
  }
}


void lefiLayer::setDirection(const char* dir) {
  this->direction_ = (char*)dir;
  this->hasDirection_ = 1;
}


void lefiLayer::setResistance(double num) {
  this->hasResistance_ = 1;
  this->resistance_ = num;
}


void lefiLayer::setCapacitance(double num) {
  this->hasCapacitance_ = 1;
  this->capacitance_ = num;
}


void lefiLayer::setHeight(double num) {
  this->hasHeight_ = 1;
  this->height_ = num;
}


void lefiLayer::setThickness(double num) {
  this->hasThickness_ = 1;
  this->thickness_ = num;
}


void lefiLayer::setShrinkage(double num) {
  this->hasShrinkage_ = 1;
  this->shrinkage_ = num;
}


void lefiLayer::setCapMultiplier(double num) {
  this->hasCapMultiplier_ = 1;
  this->capMultiplier_ = num;
}


void lefiLayer::setEdgeCap(double num) {
  this->hasEdgeCap_ = 1;
  this->edgeCap_ = num;
}


void lefiLayer::setAntennaLength(double num) {
  this->hasAntennaLength_ = 1;
  this->antennaLength_ = num;
}


void lefiLayer::setAntennaArea(double num) {
  this->hasAntennaArea_ = 1;
  this->antennaArea_ = num;
}


void lefiLayer::setCurrentDensity(double num) {
  this->hasCurrentDensityPoint_ = 1;
  this->currentDensity_ = num;
}


void lefiLayer::setCurrentPoint(double width, double current) {
  if (this->numCurrentPoints_ == this->currentPointsAllocated_) {
    int max = this->numCurrentPoints_;
    int len;
    int i;
    double* nc;
    double* nw;

    len = this->currentPointsAllocated_ *= 2;
    nc = (double*)malloc(sizeof(double)*len);
    nw = (double*)malloc(sizeof(double)*len);

    for (i = 0; i < max; i++) {
      nc[i] = this->current_[i];
      nw[i] = this->currentWidths_[i];
    }
    free((char*)(this->current_));
    free((char*)(this->currentWidths_));
    this->current_ = nc;
    this->currentWidths_ = nw;
  }
  this->current_[this->numCurrentPoints_] = current;
  this->currentWidths_[this->numCurrentPoints_] = width;
  this->numCurrentPoints_ += 1;
}


void lefiLayer::setResistancePoint(double width, double resistance) {
  if (this->numResistancePoints_ == this->resistancePointsAllocated_) {
    int max = this->numResistancePoints_;
    int len;
    int i;
    double* nc;
    double* nw;

    len = this->resistancePointsAllocated_ *= 2;
    nc = (double*)malloc(sizeof(double)*len);
    nw = (double*)malloc(sizeof(double)*len);
    for (i = 0; i < max; i++) {
      nc[i] = this->resistances_[i];
      nw[i] = this->resistanceWidths_[i];
    }
    free((char*)(this->resistances_));
    free((char*)(this->resistanceWidths_));
    this->resistances_ = nc;
    this->resistanceWidths_ = nw;
  }
  this->resistances_[this->numResistancePoints_] = resistance;
  this->resistanceWidths_[this->numResistancePoints_] = width;
  this->numResistancePoints_ += 1;
}


void lefiLayer::setCapacitancePoint(double width, double capacitance) {
  if (this->numCapacitancePoints_ == this->capacitancePointsAllocated_) {
    int max = this->numCapacitancePoints_;
    int len;
    int i;
    double* nc;
    double* nw;

    len = this->capacitancePointsAllocated_ *= 2;
    nc = (double*)malloc(sizeof(double)*len);
    nw = (double*)malloc(sizeof(double)*len);
    for (i = 0; i < max; i++) {
      nc[i] = this->capacitances_[i];
      nw[i] = this->capacitanceWidths_[i];
    }
    free((char*)(this->capacitances_));
    free((char*)(this->capacitanceWidths_));
    this->capacitances_ = nc;
    this->capacitanceWidths_ = nw;
  }
  this->capacitances_[this->numCapacitancePoints_] = capacitance;
  this->capacitanceWidths_[this->numCapacitancePoints_] = width;
  this->numCapacitancePoints_ += 1;
}


int lefiLayer::hasType() const {
  return (this->type_[0] != '\0') ? 1 : 0 ;
}


int lefiLayer::hasPitch() const {
  return this->hasPitch_;
}


int lefiLayer::hasOffset() const {
  return this->hasOffset_;
}


int lefiLayer::hasWidth() const {
  return this->hasWidth_;
}


int lefiLayer::hasWireExtension() const {
  return this->hasWireExtension_;
}


int lefiLayer::hasSpacingNumber() const {
  return ((this->hasSpacing_ != 0) && (this->numSpacings_ > 0)) ? 1 : 0;
}


int lefiLayer::hasSpacingRange() const {
  return ((this->hasSpacing_ != 0) && (this->left_ != this->right_)) ? 1 : 0;
}


int lefiLayer::hasDirection() const {
  return this->hasDirection_;
}


int lefiLayer::hasResistance() const {
  return this->hasResistance_;
}


int lefiLayer::hasCapacitance() const {
  return this->hasCapacitance_;
}


int lefiLayer::hasHeight() const {
  return this->hasHeight_;
}


int lefiLayer::hasThickness() const {
  return this->hasThickness_;
}


int lefiLayer::hasShrinkage() const {
  return this->hasShrinkage_;
}


int lefiLayer::hasCapMultiplier() const {
  return this->hasCapMultiplier_;
}


int lefiLayer::hasEdgeCap() const {
  return this->hasEdgeCap_;
}


int lefiLayer::hasAntennaArea() const {
  return this->hasAntennaArea_;
}


int lefiLayer::hasAntennaLength() const {
  return this->hasAntennaLength_;
}


int lefiLayer::hasCurrentDensityPoint() const {
  return this->hasCurrentDensityPoint_;
}


int lefiLayer::hasCurrentDensityArray() const {
  return this->numCurrentPoints_ ? 1 : 0 ;
}


int lefiLayer::hasResistanceArray() const {
  return this->numResistancePoints_ ? 1 : 0 ;
}


int lefiLayer::hasCapacitanceArray() const {
  return this->numCapacitancePoints_ ? 1 : 0 ;
}


void lefiLayer::currentDensityArray(int* numPoints,
	   double** widths, double** current) const {
  *numPoints = this->numCurrentPoints_;
  *widths = this->currentWidths_;
  *current = this->current_;
}


void lefiLayer::resistanceArray(int* numPoints,
	   double** widths, double** res) const {
  *numPoints = this->numResistancePoints_;
  *widths = this->resistanceWidths_;
  *res = this->resistances_;
}


void lefiLayer::capacitanceArray(int* numPoints,
	   double** widths, double** cap) const {
  *numPoints = this->numCapacitancePoints_;
  *widths = this->capacitanceWidths_;
  *cap = this->capacitances_;
}


int lefiLayer::numSpacing() const {
  return this->numSpacings_;
}


char* lefiLayer::name() const {
  return this->name_;
}


const char* lefiLayer::type() const {
  return this->type_;
}


double lefiLayer::pitch() const {
  return this->pitch_;
}


double lefiLayer::offset() const {
  return this->offset_;
}


double lefiLayer::width() const {
  return this->width_;
}


double lefiLayer::wireExtension() const {
  return this->wireExtension_;
}


double lefiLayer::spacing(int index) const {
  return this->spacing_[index];
}


char* lefiLayer::spacingName(int index) const {
  return this->spacingName_[index];
}


double lefiLayer::spacingRangeLeft() const {
  return this->left_;
}


double lefiLayer::spacingRangeRight() const {
  return this->right_;
}


const char* lefiLayer::direction() const {
  return this->direction_;
}


double lefiLayer::currentDensityPoint() const {
  return this->currentDensity_;
}


double lefiLayer::resistance() const {
  return this->resistance_;
}


double lefiLayer::capacitance() const {
  return this->capacitance_;
}


double lefiLayer::height() const {
  return this->height_;
}


double lefiLayer::thickness() const {
  return this->thickness_;
}


double lefiLayer::shrinkage() const {
  return this->shrinkage_;
}


double lefiLayer::capMultiplier() const {
  return this->capMultiplier_;
}


double lefiLayer::edgeCap() const {
  return this->edgeCap_;
}


double lefiLayer::antennaLength() const {
  return this->antennaLength_;
}

double lefiLayer::antennaArea() const {
  return this->antennaArea_;
}


void lefiLayer::print(FILE* f) const {
  int i,max;
  double* j;
  double* k;
  fprintf(f, "Layer %s:\n", this->lefiLayer::name());
  if (this->lefiLayer::hasType())
     fprintf(f, "  type %s\n", this->lefiLayer::type());
  if (this->lefiLayer::hasPitch())
     fprintf(f, "  pitch %lg\n", this->lefiLayer::pitch());
  if (this->lefiLayer::hasWireExtension())
     fprintf(f, "  wireextension %lg\n", this->lefiLayer::wireExtension());
  if (this->lefiLayer::hasWidth())
     fprintf(f, "  width %lg\n", this->lefiLayer::width());
  if (this->lefiLayer::hasSpacingNumber()) {
     for (i = 0; i < this->lefiLayer::numSpacing(); i++) 
        fprintf(f, "  spacing %lg\n", this->lefiLayer::spacing(i));
  }
  if (this->lefiLayer::hasSpacingRange())
     fprintf(f, "  spacing %lg %lg\n", this->lefiLayer::spacingRangeLeft(),
                                       this->lefiLayer::spacingRangeRight());
  if (this->lefiLayer::hasDirection())
     fprintf(f, "  direction %s\n", this->lefiLayer::direction());

  if (this->lefiLayer::hasResistance())
     fprintf(f, "  resistance %lg\n", this->lefiLayer::resistance());
  if (this->lefiLayer::hasResistanceArray()) {
     this->lefiLayer::resistanceArray(&max,&j,&k);
     fprintf(f, "  resistance PWL");
     for (i = 0; i < max; i++)
       fprintf(f, " %lg %lg", j[i], k[i]);
     fprintf(f, "\n");
    }

  if (this->lefiLayer::hasCapacitance())
     fprintf(f, "  capacitance %lg\n", this->lefiLayer::capacitance());
  if (this->lefiLayer::hasCapacitanceArray()) {
     this->lefiLayer::capacitanceArray(&max,&j,&k);
     fprintf(f, "  capacitance PWL");
     for (i = 0; i < max; i++)
       fprintf(f, " %lg %lg", j[i], k[i]);
     fprintf(f, "\n");
    }

  if (this->lefiLayer::hasHeight())
     fprintf(f, "  height %lg\n", this->lefiLayer::height());
  if (this->lefiLayer::hasThickness())
     fprintf(f, "  thickness %lg\n", this->lefiLayer::thickness());
  if (this->lefiLayer::hasShrinkage())
     fprintf(f, "  shrinkage %lg\n", this->lefiLayer::shrinkage());
  if (this->lefiLayer::hasCapMultiplier())
     fprintf(f, "  cap muptiplier %lg\n", this->lefiLayer::capMultiplier());
  if (this->lefiLayer::hasEdgeCap())
     fprintf(f, "  edge cap %lg\n", this->lefiLayer::edgeCap());

  if (this->lefiLayer::hasCurrentDensityPoint())
     fprintf(f, "  currentden %lg\n", this->lefiLayer::currentDensityPoint());
  if (this->lefiLayer::hasCurrentDensityArray()) {
     this->lefiLayer::currentDensityArray(&max,&j,&k);
     fprintf(f, "  currentden PWL");
     for (i = 0; i < max; i++)
       fprintf(f, " %lg %lg", j[i], k[i]);
     fprintf(f, "\n");
    }
}


void lefiLayer::addProp(const char* name, const char* value) {
  int len = strlen(name) + 1;
  if (this->numProps_ == this->propsAllocated_) {
    int i;
    int max;
    int lim = this->numProps_;
    char** nn;
    char** nv;

    max = this->propsAllocated_ *= 2;
    nn = (char**)malloc(sizeof(char*)*max);
    nv = (char**)malloc(sizeof(char*)*max);
    for (i = 0; i < lim; i++) {
      nn[i] = this->names_[i];
      nv[i] = this->values_[i];
    }
    free((char*)(this->names_));
    free((char*)(this->values_));
    this->names_ = nn;
    this->values_ = nv;
  }
  this->names_[this->numProps_] = (char*)malloc(sizeof(char)*len);
  strcpy(this->names_[this->numProps_],name);
  len = strlen(value) + 1;
  this->values_[this->numProps_] = (char*)malloc(sizeof(char)*len);
  strcpy(this->values_[this->numProps_],value);
  this->numProps_ += 1;
}


int lefiLayer::numProps() const {
  return this->numProps_;
}


const char* lefiLayer::propName(int i) const {
  return this->names_[i];
}


const char* lefiLayer::propValue(int i) const {
  return this->values_[i];
}


void lefiLayer::addAccurrentDensity(const char* type) {
  lefiLayerDensity* density;
  if (this->numAccurrents_ == this->accurrentAllocated_) {
    lefiLayerDensity** array;
    int i;
    this->accurrentAllocated_ = this->accurrentAllocated_ ?
                                this->accurrentAllocated_ * 2 : 2 ;
    array = (lefiLayerDensity**)malloc(sizeof(lefiLayerDensity*)
                                *this->accurrentAllocated_);
    for (i = 0; i < this->numAccurrents_; i++)
       array[i] = this->accurrents_[i];
    if (this->accurrents_) free((char*)(this->accurrents_));
    this->accurrents_ = array;
  }
  density = this->accurrents_[this->numAccurrents_] =
            (lefiLayerDensity*)malloc(sizeof(lefiLayerDensity));
  this->numAccurrents_ += 1;
  density->lefiLayerDensity::Init(type);
}


void lefiLayer::setAcOneEntry(double num) {
  lefiLayerDensity* density;
  density = this->accurrents_[this->numAccurrents_-1];
  density->lefiLayerDensity::setOneEntry(num);
}


void lefiLayer::addAcFrequency() {
  lefiLayerDensity* density;
  density = this->accurrents_[this->numAccurrents_-1];
  density->lefiLayerDensity::addFrequency(this->numNums_, this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}


void lefiLayer::addAcTableEntry() {
  lefiLayerDensity* density;
  density = this->accurrents_[this->numAccurrents_-1];
  density->lefiLayerDensity::addTableEntry(this->numNums_, this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}


void lefiLayer::addAcWidth() {
  lefiLayerDensity* density;
  density = this->accurrents_[this->numAccurrents_-1];
  density->lefiLayerDensity::addWidth(this->numNums_, this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}


void lefiLayer::setDcOneEntry(double num) {
  lefiLayerDensity* density;
  density = this->dccurrents_[this->numDccurrents_-1];
  density->lefiLayerDensity::setOneEntry(num);
}


void lefiLayer::addDccurrentDensity(const char* type) {
  lefiLayerDensity* density;
  if (this->numDccurrents_ == this->dccurrentAllocated_) {
    lefiLayerDensity** array;
    int i;
    this->dccurrentAllocated_ = this->dccurrentAllocated_ ?
                                this->dccurrentAllocated_ * 2 : 2 ;
    array = (lefiLayerDensity**)malloc(sizeof(lefiLayerDensity*)
                                *this->dccurrentAllocated_);
    for (i = 0; i < this->numDccurrents_; i++)
       array[i] = this->dccurrents_[i];
    if (this->dccurrents_) free((char*)(this->dccurrents_));
    this->dccurrents_ = array;
  }
  density = this->dccurrents_[this->numDccurrents_] =
            (lefiLayerDensity*)malloc(sizeof(lefiLayerDensity));
  this->numDccurrents_ += 1;
  density->lefiLayerDensity::Init(type);
}

void lefiLayer::addDcCutarea() {
  lefiLayerDensity* density;
  density = this->dccurrents_[this->numDccurrents_-1];
  density->lefiLayerDensity::addCutarea(this->numNums_, this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}


void lefiLayer::addDcTableEntry() {
  lefiLayerDensity* density;
  density = this->dccurrents_[this->numDccurrents_-1];
  density->lefiLayerDensity::addTableEntry(this->numNums_, this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}


void lefiLayer::addDcWidth() {
  lefiLayerDensity* density;
  density = this->dccurrents_[this->numDccurrents_-1];
  density->lefiLayerDensity::addWidth(this->numNums_, this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}


void lefiLayer::addNumber(double num) {
  if (this->numNums_ == this->numAllocated_) {
    double* array;
    int i;
    this->numAllocated_ = this->numAllocated_ ?
                          this->numAllocated_ * 2 : 2 ;
    array = (double*)malloc(sizeof(double) *this->numAllocated_);
    for (i = 0; i < this->numNums_; i++)
       array[i] = this->nums_[i];
    if (this->nums_) free((char*)(this->nums_));
    this->nums_ = array;
  }
  this->nums_[this->numNums_++] = num;
}


int lefiLayer::hasAccurrentDensity() const {
  return this->numAccurrents_ ? 1 : 0 ;
}


int lefiLayer::hasDccurrentDensity() const {
  return this->numDccurrents_ ? 1 : 0 ;
}


int lefiLayer::numAccurrentDensity() const {
  return(this->numAccurrents_);
}


int lefiLayer::numDccurrentDensity() const {
  return(this->numDccurrents_);
}


lefiLayerDensity* lefiLayer::accurrent(int index) const {
  if (index >= this->numAccurrents_)
     return 0;
  return(this->accurrents_[index]);
}


lefiLayerDensity* lefiLayer::dccurrent(int index) const {
  if (index >= this->numDccurrents_)
     return 0;
  return(this->dccurrents_[index]);
}


void lefiLayer::setSpacingUseLength() {
  this->hasUseLengthThreshold_ = 1;
}


void lefiLayer::setSpacingLength(double num) {
  this->lengthThreshold_ = num;
  this->left_ = -1;   /* set with the lengthThreshold number in setSpacing */
}


int lefiLayer::hasSpacingUseLengthThreshold() {
  return this->hasUseLengthThreshold_ ? 1 : 0 ;
}


int lefiLayer::hasSpacingLengthThreshold() {
  return this->lengthThreshold_ ? 1 : 0;
}


double lefiLayer::spacingLengthThreshold() const {
  return this->lengthThreshold_;
}
