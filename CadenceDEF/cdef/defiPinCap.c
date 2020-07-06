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
#include "defiPinCap.h"
#include "defiComponent.h"
#include "defiDebug.h"

/*
 * defiPinCap
 */ void 
defiPinCap_setPin(defiPinCap * this,
		  int p)
{
  this->pin_ = p;
}

void 
defiPinCap_setCap(defiPinCap * this,
		  double d)
{
  this->cap_ = d;
}

int 
defiPinCap_pin(const defiPinCap * this)
{
  return this->pin_;
}

double 
defiPinCap_cap(const defiPinCap * this)
{
  return this->cap_;
}

void 
defiPinCap_print(const defiPinCap * this,
		 FILE * f)
{
  fprintf(f, "PinCap %d %5.2lf\n", this->pin_, this->cap_);
}

/*
 * defiPin
 */ defiPin *
defiPin_CreateUninitialized()
{
  defiPin *this = (defiPin *) malloc(sizeof(defiPin));

  defiPin_Init(this);
  return (this);
}

void 
defiPin_Init(defiPin * this)
{
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

void
defiPin_Delete(defiPin * this)
{
  if (this == NULL)
    return;
  defiPin_Destroy(this);
  free(this);
}

void 
defiPin_clear(defiPin * this)
{
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

void 
defiPin_Destroy(defiPin * this)
{
  if (this->pinName_)
    free(this->pinName_);
  if (this->netName_)
    free(this->netName_);
  if (this->use_)
    free(this->use_);
  if (this->direction_)
    free(this->direction_);
  if (this->layer_)
    free(this->layer_);
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
  defiPin_clear(this);
}

void 
defiPin_Setup(defiPin * this,
	      const char *pinName,
	      const char *netName)
{
  int     len = strlen(pinName) + 1;

  if (this->pinNameLength_ < len) {
    if (this->pinName_)
      free(this->pinName_);
    this->pinName_ = (char *) malloc(len);
    this->pinNameLength_ = len;
  }
  strcpy(this->pinName_, DEFCASE(pinName));
  len = strlen(netName) + 1;
  if (this->netNameLength_ < len) {
    if (this->netName_)
      free(this->netName_);
    this->netName_ = (char *) malloc(len);
    this->netNameLength_ = len;
  }
  strcpy(this->netName_, DEFCASE(netName));
  defiPin_clear(this);
}

void 
defiPin_setDirection(defiPin * this,
		     const char *dir)
{
  int     len = strlen(dir) + 1;

  if (this->directionLength_ < len) {
    if (this->direction_)
      free(this->direction_);
    this->direction_ = (char *) malloc(len);
    this->directionLength_ = len;
  }
  strcpy(this->direction_, DEFCASE(dir));
  this->hasDirection_ = 1;
}

void 
defiPin_setUse(defiPin * this,
	       const char *use)
{
  int     len = strlen(use) + 1;

  if (this->useLength_ < len) {
    if (this->use_)
      free(this->use_);
    this->use_ = (char *) malloc(len);
    this->useLength_ = len;
  }
  strcpy(this->use_, DEFCASE(use));
  this->hasUse_ = 1;
}

void 
defiPin_setLayer(defiPin * this,
		 const char *layer,
		 int xl,
		 int yl,
		 int xh,
		 int yh)
{
  int     len = strlen(layer) + 1;

  if (this->layerLength_ < len) {
    if (this->layer_)
      free(this->layer_);
    this->layer_ = (char *) malloc(len);
    this->layerLength_ = len;
  }
  strcpy(this->layer_, DEFCASE(layer));
  this->hasLayer_ = 1;
  this->xl_ = xl;
  this->yl_ = yl;
  this->xh_ = xh;
  this->yh_ = yh;
}

void 
defiPin_setPlacement(defiPin * this,
		     int typ,
		     int x,
		     int y,
		     int orient)
{
  this->x_ = x;
  this->y_ = y;
  this->orient_ = orient;
  this->placeType_ = typ;
}

const char *
defiPin_pinName(const defiPin * this)
{
  return this->pinName_;
}

const char *
defiPin_netName(const defiPin * this)
{
  return this->netName_;
}

int 
defiPin_hasDirection(const defiPin * this)
{
  return (int) (this->hasDirection_);
}

int 
defiPin_hasUse(const defiPin * this)
{
  return (int) (this->hasUse_);
}

int 
defiPin_hasLayer(const defiPin * this)
{
  return (int) (this->hasLayer_);
}

int 
defiPin_hasPlacement(const defiPin * this)
{
  return this->placeType_ == DEFI_COMPONENT_UNPLACED ? 0 : 1;
}

int 
defiPin_isPlaced(const defiPin * this)
{
  return this->placeType_ == DEFI_COMPONENT_PLACED ? 1 : 0;
}

int 
defiPin_isCover(const defiPin * this)
{
  return this->placeType_ == DEFI_COMPONENT_COVER ? 1 : 0;
}

int 
defiPin_isFixed(const defiPin * this)
{
  return this->placeType_ == DEFI_COMPONENT_FIXED ? 1 : 0;
}

int 
defiPin_placementX(const defiPin * this)
{
  return this->x_;
}

int 
defiPin_placementY(const defiPin * this)
{
  return this->y_;
}

const char *
defiPin_direction(const defiPin * this)
{
  return this->direction_;
}

const char *
defiPin_use(const defiPin * this)
{
  return this->use_;
}

const char *
defiPin_layer(const defiPin * this)
{
  return this->layer_;
}

void 
defiPin_bounds(const defiPin * this,
	       int *xl,
	       int *yl,
	       int *xh,
	       int *yh)
{
  if (xl)
    *xl = this->xl_;
  if (yl)
    *yl = this->yl_;
  if (xh)
    *xh = this->xh_;
  if (yh)
    *yh = this->yh_;
}

int 
defiPin_orient(const defiPin * this)
{
  return this->orient_;
}

void 
defiPin_setSpecial(defiPin * this)
{
  this->hasSpecial_ = 1;
}

int 
defiPin_hasSpecial(const defiPin * this)
{
  return (int) this->hasSpecial_;
}

void 
defiPin_print(const defiPin * this,
	      FILE * f)
{
  int     xl, yl, xh, yh;

  fprintf(f, "Pin '%s' on net '%s'\n", defiPin_pinName(this), defiPin_netName(this));
  if (defiPin_hasDirection(this))
    fprintf(f, " direction '%s'\n", defiPin_direction(this));
  if (defiPin_hasUse(this))
    fprintf(f, " use '%s'\n", defiPin_use(this));
  if (defiPin_hasLayer(this)) {
    defiPin_bounds(this,
		   &xl,
		   &yl,
		   &xh,
		   &yh);
    fprintf(f, " layer '%s' %d %d %d %d\n", defiPin_layer(this), xl, yl, xh, yh);
  }
  if (defiPin_hasPlacement(this))
    fprintf(f, " placed %s%s%d %d\n", defiPin_isFixed(this) ? " FIXED" : "", defiPin_isCover(this) ? " COVER" : "", defiPin_placementX(this), defiPin_placementY(this));
  if (defiPin_hasSpecial(this))
    fprintf(f, " SPECIAL\n");
}
