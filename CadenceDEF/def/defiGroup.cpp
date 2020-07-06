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
#include <malloc.h>
#include <stdlib.h>
#include "defiGroup.hpp"
#include "defiDebug.hpp"


//////////////////////////////////////////////
//////////////////////////////////////////////
//
//   defiGroup
//
//////////////////////////////////////////////
//////////////////////////////////////////////


defiGroup::defiGroup() {
  this->defiGroup::Init();
}


void defiGroup::Init() {
  this->name_ = 0;
  this->nameLength_ = 0;
  this->region_ = 0;
  this->regionLength_ = 0;

  this->numRects_ = 0;
  this->rectsAllocated_ = 2;
  this->xl_ = (int*)malloc(sizeof(int)*2);
  this->yl_ = (int*)malloc(sizeof(int)*2);
  this->xh_ = (int*)malloc(sizeof(int)*2);
  this->yh_ = (int*)malloc(sizeof(int)*2);

  this->numProps_ = 0; 
  this->propsAllocated_ = 2;
  this->propNames_ = (char**)malloc(sizeof(char*)*2);
  this->propValues_ = (char**)malloc(sizeof(char*)*2);
}


defiGroup::~defiGroup() {
  this->defiGroup::Destroy();
}


void defiGroup::Destroy() {

  if (this->name_) free(this->name_);
  if (this->region_) free(this->region_);
  this->name_ = 0;
  this->nameLength_ = 0;
  this->region_ = 0;
  this->regionLength_ = 0;

  this->defiGroup::clear();
  free((char*)(this->propNames_));
  free((char*)(this->propValues_));
  free((char*)(this->xl_));
  free((char*)(this->yl_));
  free((char*)(this->xh_));
  free((char*)(this->yh_));
}


void defiGroup::clear() {
  int i;
  this->hasRegionName_ = 0;
  this->hasPerim_ = 0;
  this->hasMaxX_ = 0;
  this->hasMaxY_ = 0;
  for (i = 0; i < this->numProps_; i++) {
    free(this->propNames_[i]);
    free(this->propValues_[i]);
  }
  this->numProps_ = 0;
  this->numRects_ = 0;
}


void defiGroup::setup(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameLength_) {
    if (this->name_) free(this->name_);
    this->nameLength_ = len;
    this->name_ = (char*)malloc(len);
  }
  strcpy(this->name_, DEFCASE(name));
  this->defiGroup::clear();

}


void defiGroup::addRegionRect(int xl, int yl, int xh, int yh) {
  int i;
  if (this->numRects_ == this->rectsAllocated_) {
    int max = this->numRects_ * 2;
    int* nxl = (int*)malloc(sizeof(int)*max);
    int* nyl = (int*)malloc(sizeof(int)*max);
    int* nxh = (int*)malloc(sizeof(int)*max);
    int* nyh = (int*)malloc(sizeof(int)*max);
    max = this->numRects_;
    for (i = 0; i < max; i++) {
      nxl[i] = this->xl_[i];
      nyl[i] = this->yl_[i];
      nxh[i] = this->xh_[i];
      nyh[i] = this->yh_[i];
    }
    free((char*)(this->xl_));
    free((char*)(this->yl_));
    free((char*)(this->xh_));
    free((char*)(this->yh_));
    this->xl_ = nxl;
    this->yl_ = nyl;
    this->xh_ = nxh;
    this->yh_ = nyh;
    this->rectsAllocated_ *= 2;
  }

  i = this->numRects_;
  this->xl_[i] = xl;
  this->yl_[i] = yl;
  this->xh_[i] = xh;
  this->yh_[i] = yh;
  this->numRects_ += 1;
}


void defiGroup::regionRects(int* size, int** xl,
   int**yl, int** xh, int** yh) {
  *size = this->numRects_;
  *xl = this->xl_;
  *yl = this->yl_;
  *xh = this->xh_;
  *yh = this->yh_;
}


void defiGroup::setRegionName(const char* region) {
  int len = strlen(region) + 1;
  if (len > this->regionLength_) {
    if (this->region_) free(this->region_);
    this->regionLength_ = len;
    this->region_ = (char*)malloc(len);
  }
  strcpy(this->region_, DEFCASE(region));
  this->hasRegionName_ = 1;

}


void defiGroup::setMaxX(int x) {
  this->hasMaxX_ = 1;
  this->maxX_ = x;
}


void defiGroup::setMaxY(int y) {
  this->hasMaxY_ = 1;
  this->maxY_ = y;
}


void defiGroup::setPerim(int p) {
  this->hasPerim_ = 1;
  this->perim_ = p;
}


void defiGroup::addProperty(const char* name, const char* value) {
  int len;
  if (this->numProps_ == this->propsAllocated_) {
    int i;
    char** nn;
    char** nv;
    this->propsAllocated_ *= 2;
    nn = (char**)malloc(sizeof(char*)*this->propsAllocated_);
    nv = (char**)malloc(sizeof(char*)*this->propsAllocated_);
    for (i = 0; i < this->numProps_; i++) {
      nn[i] = this->propNames_[i];
      nv[i] = this->propValues_[i];
    }
    free((char*)(this->propNames_));
    free((char*)(this->propValues_));
    this->propNames_ = nn;
    this->propValues_ = nv;
  }
  len = strlen(name) + 1;
  this->propNames_[this->numProps_] = (char*)malloc(len);
  strcpy(this->propNames_[this->numProps_], DEFCASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProps_] = (char*)malloc(len);
  strcpy(this->propValues_[this->numProps_], DEFCASE(value));
  this->numProps_ += 1;
}


int defiGroup::numProps() const {
  return this->numProps_;
}


const char* defiGroup::propName(int index) const {
  if (index < 0 || index >= this->numProps_) {
    defiError("bad index for region property");
    return 0;
  }
  return this->propNames_[index];
}


const char* defiGroup::propValue(int index) const {
  if (index < 0 || index >= this->numProps_) {
    defiError("bad index for region property");
    return 0;
  }
  return this->propValues_[index];
}


const char* defiGroup::regionName() const {
  return this->region_;
}


const char* defiGroup::name() const {
  return this->name_;
}


int defiGroup::perim() const {
  return this->perim_;
}


int defiGroup::maxX() const {
  return this->maxX_;
}


int defiGroup::maxY() const {
  return this->maxY_;
}


int defiGroup::hasMaxX() const {
  return this->hasMaxX_;
}


int defiGroup::hasMaxY() const {
  return this->hasMaxY_;
}


int defiGroup::hasPerim() const {
  return this->hasPerim_;
}


int defiGroup::hasRegionBox() const {
  return this->numRects_ ? 1 : 0 ;
}


int defiGroup::hasRegionName() const {
  return this->hasRegionName_;
}


void defiGroup::print(FILE* f) const {
  int i;

  fprintf(f, "Group '%s'\n", this->defiGroup::name());

  if (this->defiGroup::hasRegionName()) {
    fprintf(f, "  region name '%s'\n", this->defiGroup::regionName());
  }

  if (this->defiGroup::hasRegionBox()) {
    int size = this->numRects_;
    int* xl = this->xl_;
    int* yl = this->yl_;
    int* xh = this->xh_;
    int* yh = this->yh_;
    for (i = 0; i < size; i++)
      fprintf(f, "  region box %d,%d %d,%d\n", xl[i], yl[i], xh[i], yh[i]);
  }

  if (this->defiGroup::hasMaxX()) {
    fprintf(f, "  max x %d\n", this->defiGroup::maxX());
  }

  if (this->defiGroup::hasMaxY()) {
    fprintf(f, "  max y %d\n", this->defiGroup::maxY());
  }

  if (this->defiGroup::hasPerim()) {
    fprintf(f, "  perim %d\n", this->defiGroup::perim());
  }

}


