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
#include "defiPinCap.hpp"
#include "defiComponent.hpp"
#include "defiDebug.hpp"


///////////////////////////////////////////////
///////////////////////////////////////////////
//
//     defiPinCap
//
///////////////////////////////////////////////
///////////////////////////////////////////////


void defiPinCap::setPin(int p) {
  this->pin_ = p;
}


void defiPinCap::setCap(double d) {
  this->cap_ = d;
}


int defiPinCap::pin() const {
  return this->pin_;
}


double defiPinCap::cap() const {
  return this->cap_;
}


void defiPinCap::print(FILE* f) const {
  fprintf(f, "PinCap  %d %5.2lf\n", this->pin_, this->cap_);
}



///////////////////////////////////////////////
///////////////////////////////////////////////
//
//     defiPin
//
///////////////////////////////////////////////
///////////////////////////////////////////////


defiPin::defiPin() {
  this->defiPin::Init();
}


void defiPin::Init() {
  this->pinNameLength_ = 0;
  this->pinName_ = 0;
  this->netNameLength_ = 0;
  this->netName_ = 0;
  this->useLength_ = 0;
  this->use_ = 0;
  this->directionLength_ = 0;
  this->direction_ = 0;
  this->hasDirection_ = 0;
  this->hasUse_ = 0;
  this->hasLayer_ = 0;
  this->placeType_ = DEFI_COMPONENT_UNPLACED;
  this->orient_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->x_ = 0;
  this->y_ = 0;
  this->layer_ = 0;
  this->layerLength_ = 0;
}


defiPin::~defiPin() {
  this->defiPin::Destroy();
}


void defiPin::clear() {
  this->hasDirection_ = 0;
  this->hasUse_ = 0;
  this->hasLayer_ = 0;
  this->hasSpecial_ = 0;
  this->placeType_ = DEFI_COMPONENT_UNPLACED;
  this->orient_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->x_ = 0;
  this->y_ = 0;
}


void defiPin::Destroy() {
  if (this->pinName_) free(this->pinName_);
  if (this->netName_) free(this->netName_);
  if (this->use_) free(this->use_);
  if (this->direction_) free(this->direction_);
  if (this->layer_) free(this->layer_);
  this->pinName_ = 0;
  this->netName_ = 0;
  this->use_ = 0;
  this->direction_ = 0;
  this->layer_ = 0;
  this->pinNameLength_ = 0;
  this->netNameLength_ = 0;
  this->useLength_ = 0;
  this->directionLength_ = 0;
  this->layerLength_ = 0;
  this->defiPin::clear();
}


void defiPin::Setup(const char* pinName, const char* netName) {
  int len = strlen(pinName) + 1;
  if (this->pinNameLength_ < len) {
    if (this->pinName_) free(this->pinName_);
    this->pinName_ = (char*)malloc(len);
    this->pinNameLength_ = len;
  }
  strcpy(this->pinName_, DEFCASE(pinName));

  len = strlen(netName) + 1;
  if (this->netNameLength_ < len) {
    if (this->netName_) free(this->netName_);
    this->netName_ = (char*)malloc(len);
    this->netNameLength_ = len;
  }
  strcpy(this->netName_, DEFCASE(netName));

  this->defiPin::clear();

}


void defiPin::setDirection(const char* dir) {
  int len = strlen(dir) + 1;
  if (this->directionLength_ < len) {
    if (this->direction_) free(this->direction_);
    this->direction_ = (char*)malloc(len);
    this->directionLength_ = len;
  }
  strcpy(this->direction_, DEFCASE(dir));
  this->hasDirection_ = 1;
}


void defiPin::setUse(const char* use) {
  int len = strlen(use) + 1;
  if (this->useLength_ < len) {
    if (this->use_) free(this->use_);
    this->use_ = (char*)malloc(len);
    this->useLength_ = len;
  }
  strcpy(this->use_, DEFCASE(use));
  this->hasUse_ = 1;
}


void defiPin::setLayer(const char* layer, int xl, int yl, int xh, int yh) {
  int len = strlen(layer) + 1;
  if (this->layerLength_ < len) {
    if (this->layer_) free(this->layer_);
    this->layer_ = (char*)malloc(len);
    this->layerLength_ = len;
  }
  strcpy(this->layer_, DEFCASE(layer));
  this->hasLayer_ = 1;
  this->xl_ = xl;
  this->yl_ = yl;
  this->xh_ = xh;
  this->yh_ = yh;
}


void defiPin::setPlacement(int typ, int x, int y, int orient) {
  this->x_ = x;
  this->y_ = y;
  this->orient_ = orient;
  this->placeType_ = typ;
}



const char* defiPin::pinName() const {
  return this->pinName_;
}


const char* defiPin::netName() const {
  return this->netName_;
}


int defiPin::hasDirection() const {
  return (int)(this->hasDirection_);
}


int defiPin::hasUse() const {
  return (int)(this->hasUse_);
}


int defiPin::hasLayer() const {
  return (int)(this->hasLayer_);
}


int defiPin::hasPlacement() const {
  return this->placeType_ == DEFI_COMPONENT_UNPLACED ? 0 : 1 ;
}


int defiPin::isPlaced() const {
  return this->placeType_ == DEFI_COMPONENT_PLACED ? 1 : 0 ;
}


int defiPin::isCover() const {
  return this->placeType_ == DEFI_COMPONENT_COVER ? 1 : 0 ;
}


int defiPin::isFixed() const {
  return this->placeType_ == DEFI_COMPONENT_FIXED ? 1 : 0 ;
}


int defiPin::placementX() const {
  return this->x_;
}


int defiPin::placementY() const {
  return this->y_;
}


const char* defiPin::direction() const {
  return this->direction_;
}


const char* defiPin::use() const {
  return this->use_;
}


const char* defiPin::layer() const {
  return this->layer_;
}


void defiPin::bounds(int* xl, int* yl, int* xh, int* yh) const {
  if (xl) *xl = this->xl_;
  if (yl) *yl = this->yl_;
  if (xh) *xh = this->xh_;
  if (yh) *yh = this->yh_;
}


int defiPin::orient() const {
  return this->orient_;
}


void defiPin::setSpecial() {
  this->hasSpecial_ = 1;
}


int defiPin::hasSpecial() const {
  return (int)this->hasSpecial_;
}


void defiPin::print(FILE* f) const {
  int xl, yl, xh,yh;

  fprintf(f, "Pin '%s' on net '%s'\n", this->defiPin::pinName(),
	    this->defiPin::netName());
  if (this->defiPin::hasDirection())
    fprintf(f, "  direction '%s'\n", this->defiPin::direction());
  if (this->defiPin::hasUse())
    fprintf(f, "  use '%s'\n", this->defiPin::use());
  if (this->defiPin::hasLayer()) {
    this->defiPin::bounds(&xl, &yl, &xh, &yh);
    fprintf(f, "  layer '%s' %d %d %d %d\n",
	       this->defiPin::layer(), xl, yl, xh, yh);
    }
  if (this->defiPin::hasPlacement())
    fprintf(f, "  placed %s%s%d %d\n",
		this->defiPin::isFixed() ? " FIXED" : "",
		this->defiPin::isCover() ? " COVER" : "",
		this->defiPin::placementX(),
		this->defiPin::placementY());
  if (this->defiPin::hasSpecial())
    fprintf(f, "  SPECIAL\n");
}


