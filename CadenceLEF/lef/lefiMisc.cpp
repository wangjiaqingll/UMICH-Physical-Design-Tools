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

#include "lefiMisc.hpp"
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "lefiDebug.hpp"
#include "FEF_stringhash.h"


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiSpacing
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiSpacing::lefiSpacing() {
  this->lefiSpacing::Init();
}


void lefiSpacing::Init() {
  this->name1_ = (char*)malloc(16);
  this->name2_ = (char*)malloc(16);
  this->name1Size_ = 16;
  this->name2Size_ = 16;
  this->distance_ = 0;
  this->hasStack_ = 0;
}


void lefiSpacing::Destroy() {
  if (this->name1_)
     free(this->name1_);
  if (this->name2_)
     free(this->name2_);
}


lefiSpacing::~lefiSpacing() {
  this->lefiSpacing::Destroy();
}


lefiSpacing* lefiSpacing::clone() {
  lefiSpacing* sp = (lefiSpacing*)malloc(sizeof(lefiSpacing));
  sp->name1Size_ = strlen(this->name1_)+1;
  sp->name1_ = (char*)malloc(sp->name1Size_);
  strcpy(sp->name1_, this->name1_);
  sp->name2Size_ = strlen(this->name2_)+1;
  sp->name2_ = (char*)malloc(sp->name2Size_);
  strcpy(sp->name2_, this->name2_);
  sp->distance_ = this->distance_;
  sp->hasStack_ = this->hasStack_;
  return sp;
}


void lefiSpacing::set(const char* name1, const char* name2, double d,
     int hasStack) {
  int len = strlen(name1) + 1;
  if (len > this->name1Size_) {
    free(this->name1_);
    this->name1_ = (char*)malloc(len);
    this->name1Size_ = len;
  }
  len = strlen(name2) + 1;
  if (len > this->name2Size_) {
    free(this->name2_);
    this->name2_ = (char*)malloc(len);
    this->name2Size_ = len;
  }
  strcpy(this->name1_, CASE(name1));
  strcpy(this->name2_, CASE(name2));
  this->distance_ = d;
  this->hasStack_ = hasStack;
}


const char* lefiSpacing::name1() const {
  return this->name1_;
}


const char* lefiSpacing::name2() const {
  return this->name2_;
}


double lefiSpacing::distance() const {
  return this->distance_;
}


int lefiSpacing::hasStack() const {
  return this->hasStack_;
}



void lefiSpacing::print(FILE* f) const {
  fprintf(f, "Spacing: %s %s %g", this->lefiSpacing::name1(),
      this->lefiSpacing::name2(), this->lefiSpacing::distance());

  if (this->lefiSpacing::hasStack())
    fprintf(f, "  STACK");

  fprintf(f, "\n");
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiIRDrop
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiIRDrop::lefiIRDrop() {
  this->lefiIRDrop::Init();
}


void lefiIRDrop::Init() {
  this->nameSize_ = 16;
  this->value1Size_ = 16;
  this->value2Size_ = 16;
  this->name_ = (char*)malloc(16);
  this->numValues_ = 0;
  this->valuesAllocated_ = 2;
  this->value1_ = (double*)malloc(sizeof(double)*2);
  this->value2_ = (double*)malloc(sizeof(double)*2);
}


void lefiIRDrop::Destroy() {
  free(this->name_);
  this->lefiIRDrop::clear();
  free((char*)(this->value1_)); 
  free((char*)(this->value2_)); 
}


lefiIRDrop::~lefiIRDrop() {
  this->lefiIRDrop::Destroy();
}


void lefiIRDrop::clear() {
  this->numValues_ = 0;
}


void lefiIRDrop::setTableName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char*)malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
  this->lefiIRDrop::clear();
/*
  *(this->value1_) = '\0';
  *(this->value2_) = '\0';
*/
}


void lefiIRDrop::setValues(double value1, double value2) {
  if (this->numValues_ == this->valuesAllocated_) {
    int i;
    double* v1;
    double* v2;
    if (this->valuesAllocated_ == 0) {
      this->valuesAllocated_ = 2;
      v1 = (double*)malloc(sizeof(double)*this->valuesAllocated_);
      v2 = (double*)malloc(sizeof(double)*this->valuesAllocated_);
    }
    else {
      this->valuesAllocated_ *= 2;
      v1 = (double*)malloc(sizeof(double)*this->valuesAllocated_);
      v2 = (double*)malloc(sizeof(double)*this->valuesAllocated_);
      for (i = 0; i < this->numValues_; i++) {
        v1[i] = this->value1_[i];
        v2[i] = this->value2_[i];
      }
      free((char*)(this->value1_));
      free((char*)(this->value2_));
    }
    this->value1_ = v1;
    this->value2_ = v2;
  }
  this->value1_[this->numValues_] = value1;
  this->value2_[this->numValues_] = value2;
  this->numValues_ += 1;
}


const char* lefiIRDrop::name() const {
  return this->name_;
}


int lefiIRDrop::numValues() const {
  return this->numValues_;
}


double lefiIRDrop::value1(int index) const {
  if (index < 0 || index >= this->numValues_) {
    lefiError("bad index for IRDrop current");
    return 0;
  }
  return this->value1_[index];
}


double lefiIRDrop::value2(int index) const {
  if (index < 0 || index >= this->numValues_) {
    lefiError("bad index for IRDrop volts");
    return 0;
  }
  return this->value2_[index];
}


void lefiIRDrop::print(FILE* f) const {
  int i;
  fprintf(f, "IRDrop %s ", this->lefiIRDrop::name());
  for (i = 0; i < this->lefiIRDrop::numValues(); i++) {
     fprintf(f, "%g %g ", this->lefiIRDrop::value1(i),
             this->lefiIRDrop::value2(i));
  }
  fprintf(f, "\n");
  fprintf(f, "END IRDrop\n");
}



////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefitMinFeature
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////



lefiMinFeature::lefiMinFeature() {
  this->lefiMinFeature::Init();
}



void lefiMinFeature::Init() {
  // nothing to do
}



void lefiMinFeature::Destroy() {
  // nothing to do
}



lefiMinFeature::~lefiMinFeature() {
  this->lefiMinFeature::Destroy();
}



void lefiMinFeature::set(double one, double two) {
  this->one_ = one;
  this->two_ = two;
}



double lefiMinFeature::one() const {
  return this->one_;
}



double lefiMinFeature::two() const {
  return this->two_;
}



void lefiMinFeature::print(FILE* f) const {
  fprintf(f, "MinFeature %g %g\n", this->lefiMinFeature::one(),
     this->lefiMinFeature::two());
}



////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiSite
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiSite::lefiSite() {
  this->lefiSite::Init();
}


void lefiSite::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)malloc(16);
}


void lefiSite::Destroy() {
  free(this->name_);
}


lefiSite::~lefiSite() {
  this->lefiSite::Destroy();
}


void lefiSite::setName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char*)malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
  this->hasClass_ = 0;
  *(this->siteClass_) = 0;
  this->hasSize_ = 0;
  this->symmetry_ = 0;
}


void lefiSite::setClass(const char* cls) {
  strcpy(this->siteClass_, cls);
  this->hasClass_ = 1;
}


void lefiSite::setSize(double x, double y) {
  this->hasSize_ = 1;
  this->sizeX_ = x;
  this->sizeY_ = y;
}


void lefiSite::setXSymmetry() {
  this->symmetry_ |= 1;
}


void lefiSite::setYSymmetry() {
  this->symmetry_ |= 2;
}


void lefiSite::set90Symmetry() {
  this->symmetry_ |= 4;
}


const char* lefiSite::name() const {
  return this->name_;
}


int lefiSite::hasClass() const {
  return this->hasClass_;
}


const char* lefiSite::siteClass() const {
  return this->siteClass_;
}


double lefiSite::sizeX() const {
  return this->sizeX_;
}


double lefiSite::sizeY() const {
  return this->sizeY_;
}


int lefiSite::hasSize() const {
  return this->hasSize_;
}


int lefiSite::hasXSymmetry() const {
  return (this->symmetry_ & 1) ? 1 : 0 ;
}


int lefiSite::hasYSymmetry() const {
  return (this->symmetry_ & 2) ? 1 : 0 ;
}


int lefiSite::has90Symmetry() const {
  return (this->symmetry_ & 4) ? 1 : 0 ;
}


void lefiSite::print(FILE* f) const {
  fprintf(f, "Site %s", this->lefiSite::name());

  if (this->lefiSite::hasClass())
    fprintf(f, " CLASS %s", this->lefiSite::siteClass());

  if (this->lefiSite::hasSize())
    fprintf(f, " Size %g %g", this->lefiSite::sizeX(),
       this->lefiSite::sizeY());

  if (this->lefiSite::hasXSymmetry())
    fprintf(f, " X-symmetry");

  if (this->lefiSite::hasYSymmetry())
    fprintf(f, " Y-symmetry");

  if (this->lefiSite::has90Symmetry())
    fprintf(f, " R90-symmetry");

  fprintf(f, "\n");
}




////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiSitePattern
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiSitePattern::lefiSitePattern() {
  this->lefiSitePattern::Init();
}


void lefiSitePattern::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)malloc(16);
}


void lefiSitePattern::Destroy() {
  free(this->name_);
}


lefiSitePattern::~lefiSitePattern() {
  this->lefiSitePattern::Destroy();
}


void lefiSitePattern::set(const char* name, double x, double y, int orient,
      double xStart, double yStart, double xStep, double yStep) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char*)malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));

  this->x_ = x;
  this->y_ = y;
  this->xStep_ = xStep;
  this->yStep_ = yStep;
  this->xStart_ = xStart;
  this->yStart_ = yStart;
  this->orient_ = orient;
}


const char* lefiSitePattern::name() const {
  return this->name_;
}


int lefiSitePattern::orient() const {
  return this->orient_;
}


double lefiSitePattern::x() const {
  return this->x_;
}


double lefiSitePattern::y() const {
  return this->y_;
}


double lefiSitePattern::xStart() const {
  return this->xStart_;
}


double lefiSitePattern::yStart() const {
  return this->yStart_;
}


double lefiSitePattern::xStep() const {
  return this->xStep_;
}


double lefiSitePattern::yStep() const {
  return this->yStep_;
}


void lefiSitePattern::print(FILE* f) const {
  fprintf(f, "  Site Pattern %s  %g,%g %d\n",
    this->lefiSitePattern::name(),
    this->lefiSitePattern::x(),
    this->lefiSitePattern::y(),
    this->lefiSitePattern::orient());
  fprintf(f, "    %g,%g step %g,%g\n", 
    this->lefiSitePattern::xStart(),
    this->lefiSitePattern::yStart(),
    this->lefiSitePattern::xStep(),
    this->lefiSitePattern::yStep());
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiTrackPattern
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiTrackPattern::lefiTrackPattern() {
  this->lefiTrackPattern::Init();
}


void lefiTrackPattern::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)malloc(16);
  this->start_ = 0;
  this->numTracks_ = 0;
  this->space_ = 0;
  this->numLayers_ = 0;
  this->layerAllocated_ = 2;
  this->layerNames_ = (char**) malloc(sizeof(char*)*2);
  this->lefiTrackPattern::clear();
}


void lefiTrackPattern::Destroy() {
  if (this->name_) free(this->name_);
  this->lefiTrackPattern::clear();
  this->name_ = 0;
  this->start_ = 0;
  this->numTracks_ = 0;
  this->space_ = 0;
  free((char*)(this->layerNames_));
}


void lefiTrackPattern::clear() {
  int i;
  for (i = 0; i < this->numLayers_; i++) {
    free(this->layerNames_[i]);
  }
}


lefiTrackPattern::~lefiTrackPattern() {
  this->lefiTrackPattern::Destroy();
}


void lefiTrackPattern::set(const char* name, double start, int numTracks,
      double space) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char*)malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));

  this->start_ = start;
  this->numTracks_ = numTracks;
  this->space_ = space;
}


void lefiTrackPattern::addLayer(const char* name) {
  int len;
  if (this->numLayers_ == this->layerAllocated_) {
    int i;
    char** nn;
    this->layerAllocated_ *= 2;
    nn = (char**) malloc(sizeof(char*)*this->layerAllocated_);
    for (i = 0; i < this->numLayers_; i++)
      nn[i] = this->layerNames_[i];
    free((char*)(this->layerNames_));
    this->layerNames_ = nn;
  }
  len = strlen(name) + 1;
  this->layerNames_[this->numLayers_] = (char*)malloc(len);
  strcpy(this->layerNames_[this->numLayers_], CASE(name));
  this->numLayers_ += 1;
}


const char* lefiTrackPattern::name() const {
  return this->name_;
}


double lefiTrackPattern::start() const {
  return this->start_;
}


int lefiTrackPattern::numTracks() const {
  return this->numTracks_;
}


double lefiTrackPattern::space() const {
  return this->space_;
}


int lefiTrackPattern::numLayers() const {
  return this->numLayers_;
}


const char* lefiTrackPattern::layerName(int index) const {
  if (index < 0 || index >= this->numLayers_) {
    lefiError("ERROR: bad layer number index");
    return 0;
  }
  return this->layerNames_[index];
}


void lefiTrackPattern::print(FILE* f) const {
  int i;
  fprintf(f, "  Track Pattern %s  %g DO %d STEP %g\n",
    this->lefiTrackPattern::name(),
    this->lefiTrackPattern::start(),
    this->lefiTrackPattern::numTracks(),
    this->lefiTrackPattern::space());
  if (this->lefiTrackPattern::numLayers() > 0) {
    fprintf(f, "    LAYER "); 
    for (i = 0; i < this->lefiTrackPattern::numLayers(); i++) 
      fprintf(f, "%s ", this->lefiTrackPattern::layerName(i));
    fprintf(f, "\n");
  }
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiGcellPattern
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiGcellPattern::lefiGcellPattern() {
  this->lefiGcellPattern::Init();
}


void lefiGcellPattern::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)malloc(16);
  this->start_ = 0;
  this->numCRs_ = 0;
  this->space_ = 0;
}


void lefiGcellPattern::Destroy() {
  if (this->name_) free(this->name_);
  this->name_ = 0;
  this->start_ = 0;
  this->numCRs_ = 0;
  this->space_ = 0;
}


lefiGcellPattern::~lefiGcellPattern() {
  this->lefiGcellPattern::Destroy();
}


void lefiGcellPattern::set(const char* name, double start, int numCRs,
      double space) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char*)malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));

  this->start_ = start;
  this->numCRs_ = numCRs;
  this->space_ = space;
}


const char* lefiGcellPattern::name() const {
  return this->name_;
}


double lefiGcellPattern::start() const {
  return this->start_;
}


int lefiGcellPattern::numCRs() const {
  return this->numCRs_;
}


double lefiGcellPattern::space() const {
  return this->space_;
}


void lefiGcellPattern::print(FILE* f) const {
  fprintf(f, "  Track Pattern %s  %g DO %d STEP %g\n",
    this->lefiGcellPattern::name(),
    this->lefiGcellPattern::start(),
    this->lefiGcellPattern::numCRs(),
    this->lefiGcellPattern::space());
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiAlias_itr
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiAlias_itr::lefiAlias_itr() {
  this->lefiAlias_itr::Init();
}


void lefiAlias_itr::Init() {
  this->num_ = -1;
}
 

void lefiAlias_itr::Destroy() {
}


lefiAlias_itr::~lefiAlias_itr() {
  this->lefiAlias_itr::Destroy();
}

 
int lefiAlias_itr::Next() {
  while (1) {
    this->num_ += 1;
    if (this->num_ <= -1 || this->num_ >= aliasLimit()) return 0;
    if (aliasKey(this->num_)) break;
  }
  return 1;
}


const char* lefiAlias_itr::Key() {
  if (this->num_ <= -1 || this->num_ >= aliasLimit()) return 0;
  return aliasKey(this->num_);
}


const char* lefiAlias_itr::Data() {
  if (this->num_ <= -1 || this->num_ >= aliasLimit()) return 0;
  return aliasData(this->num_);
}

 
