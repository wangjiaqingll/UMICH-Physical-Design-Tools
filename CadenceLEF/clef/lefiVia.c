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
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "lefiVia.h"
#include "lefiDebug.h"

/*
 * lefiViaLayer
 */ lefiViaLayer *
lefiViaLayer_CreateUninitialized()
{
  lefiViaLayer *this = (lefiViaLayer *) malloc(sizeof(lefiViaLayer));

  lefiViaLayer_Init(this);
  return (this);
}

void 
lefiViaLayer_Init(lefiViaLayer * this)
{
  this->name_ = 0;
  this->rectsAllocated_ = 2;
  this->numRects_ = 0;
  this->xl_ = (double *) malloc(sizeof(double) * 2);
  this->yl_ = (double *) malloc(sizeof(double) * 2);
  this->xh_ = (double *) malloc(sizeof(double) * 2);
  this->yh_ = (double *) malloc(sizeof(double) * 2);
}

void 
lefiViaLayer_Destroy(lefiViaLayer * this)
{
  if (this->xl_) {
    free((char *) (this->xl_));
    free((char *) (this->yl_));
    free((char *) (this->xh_));
    free((char *) (this->yh_));
  }
  free(this->name_);
}

void
lefiViaLayer_Delete(lefiViaLayer * this)
{
  if (this == NULL)
    return;
  lefiViaLayer_Destroy(this);
  free(this);
}

void 
lefiViaLayer_setName(lefiViaLayer * this,
		     const char *name)
{
  int     len = strlen(name) + 1;

  this->name_ = (char *) malloc(len);
  strcpy(this->name_, CASE(name));
}

void 
lefiViaLayer_addRect(lefiViaLayer * this,
		     double xl,
		     double yl,
		     double xh,
		     double yh)
{
  if (this->numRects_ == this->rectsAllocated_) {
    int     i;

    double *newxl;

    double *newyl;

    double *newxh;

    double *newyh;

    this->rectsAllocated_ = (this->rectsAllocated_ == 0) ? 2 : this->rectsAllocated_ * 2;
    newxl = (double *) malloc(sizeof(double) * this->rectsAllocated_);
    newyl = (double *) malloc(sizeof(double) * this->rectsAllocated_);
    newxh = (double *) malloc(sizeof(double) * this->rectsAllocated_);
    newyh = (double *) malloc(sizeof(double) * this->rectsAllocated_);
    for (i = 0;
	 i < this->numRects_;
	 i++) {
      newxl[i] = this->xl_[i];
      newyl[i] = this->yl_[i];
      newxh[i] = this->xh_[i];
      newyh[i] = this->yh_[i];
    }
    if (this->xl_) {
      free((char *) (this->xl_));
      free((char *) (this->yl_));
      free((char *) (this->xh_));
      free((char *) (this->yh_));
    }
    this->xl_ = newxl;
    this->yl_ = newyl;
    this->xh_ = newxh;
    this->yh_ = newyh;
  }
  this->xl_[this->numRects_] = xl;
  this->yl_[this->numRects_] = yl;
  this->xh_[this->numRects_] = xh;
  this->yh_[this->numRects_] = yh;
  this->numRects_ += 1;
}

int 
lefiViaLayer_numRects(lefiViaLayer * this)
{
  return this->numRects_;
}

lefiViaLayer *
lefiViaLayer_clone(lefiViaLayer * this)
{
  lefiViaLayer *vl = (lefiViaLayer *) malloc(sizeof(lefiViaLayer));

  int     i;

  vl->xl_ = (double *) malloc(sizeof(double) * this->numRects_);
  vl->yl_ = (double *) malloc(sizeof(double) * this->numRects_);
  vl->xh_ = (double *) malloc(sizeof(double) * this->numRects_);
  vl->yh_ = (double *) malloc(sizeof(double) * this->numRects_);
  vl->numRects_ = this->numRects_;
  vl->rectsAllocated_ = this->numRects_;
  for (i = 0;
       i < this->numRects_;
       i++) {
    vl->xl_[i] = this->xl_[i];
    vl->yl_[i] = this->yl_[i];
    vl->xh_[i] = this->xh_[i];
    vl->yh_[i] = this->yh_[i];
  }
  vl->name_ = (char *) malloc(strlen(this->name_) + 1);
  strcpy(vl->name_, this->name_);
  return vl;
}

char   *
lefiViaLayer_name(lefiViaLayer * this)
{
  return this->name_;
}

double 
lefiViaLayer_xl(lefiViaLayer * this,
		int index)
{
  if (index < 0 || index >= this->numRects_) {
    lefiError("index out of bounds");
    return 0;
  }
  return this->xl_[index];
}

double 
lefiViaLayer_yl(lefiViaLayer * this,
		int index)
{
  if (index < 0 || index >= this->numRects_) {
    lefiError("index out of bounds");
    return 0;
  }
  return this->yl_[index];
}

double 
lefiViaLayer_xh(lefiViaLayer * this,
		int index)
{
  if (index < 0 || index >= this->numRects_) {
    lefiError("index out of bounds");
    return 0;
  }
  return this->xh_[index];
}

double 
lefiViaLayer_yh(lefiViaLayer * this,
		int index)
{
  if (index < 0 || index >= this->numRects_) {
    lefiError("index out of bounds");
    return 0;
  }
  return this->yh_[index];
}

/*
 * lefiVia
 */ lefiVia *
lefiVia_CreateUninitialized()
{
  lefiVia *this = (lefiVia *) malloc(sizeof(lefiVia));

  lefiVia_Init(this);
  return (this);
}

void 
lefiVia_Init(lefiVia * this)
{
  this->nameSize_ = 16;
  this->name_ = (char *) malloc(16);
  this->foreign_ = 0;
  this->numProps_ = 0;
  this->propsAllocated_ = 0;
  this->layersAllocated_ = 3;
  this->layers_ = (lefiViaLayer **) malloc(sizeof(lefiViaLayer *) * 3);
  this->numLayers_ = 0;
  lefiVia_clear(this);
}

void 
lefiVia_Destroy(lefiVia * this)
{
  lefiVia_clear(this);
  free(this->name_);
  free(this->layers_);
}

void
lefiVia_Delete(lefiVia * this)
{
  if (this == NULL)
    return;
  lefiVia_Destroy(this);
  free(this);
}

lefiVia *
lefiVia_clone(lefiVia * this)
{
  int     i;

  lefiViaLayer *l;

  lefiVia *v = (lefiVia *) malloc(sizeof(lefiVia));

  v->nameSize_ = strlen(this->name_) + 1;
  v->name_ = (char *) malloc(v->nameSize_);
  strcpy(v->name_, this->name_);
  v->foreign_ = 0;
  if (lefiVia_hasForeign(this)) {
    lefiVia_setForeign(v,
		       this->foreign_,
		       lefiVia_hasForeignPnt(this), this->foreignX_, this->foreignY_, this->foreignOrient_);
  }
  v->hasDefault_ = this->hasDefault_;
  v->hasResistance_ = this->hasResistance_;
  v->hasForeignPnt_ = this->hasForeignPnt_;
  v->hasTopOfStack_ = this->hasTopOfStack_;
  v->numProps_ = this->numProps_;
  v->propsAllocated_ = this->numProps_;
  v->propName_ = (char **) malloc(sizeof(char *) * this->numProps_);
  v->propValue_ = (char **) malloc(sizeof(char *) * this->numProps_);
  v->propDValue_ = (double *) malloc(sizeof(double) * this->numProps_);
  for (i = 0;
       i < this->numProps_;
       i++) {
    v->propName_[i] = (char *) malloc(strlen(this->propName_[i]) + 1);
    strcpy(v->propName_[i], this->propName_[i]);
    if (this->propValue_[i]) {
      v->propValue_[i] = (char *) malloc(strlen(this->propValue_[i]) + 1);
      strcpy(v->propValue_[i], this->propValue_[i]);
    }
    else
      v->propValue_[i] = 0;
    v->propDValue_[i] = this->propDValue_[i];
  }
  v->layersAllocated_ = this->numLayers_;
  v->numLayers_ = this->numLayers_;
  v->layers_ = (lefiViaLayer **) malloc(sizeof(lefiViaLayer *) * this->numLayers_);
  for (i = 0;
       i < this->numLayers_;
       i++) {
    l = this->layers_[i];
    v->layers_[i] = lefiViaLayer_clone(l);
  }
  v->resistance_ = this->resistance_;
  if (this->foreignOrient_ == 0)
    v->foreignOrient_ = -1;
  else
    v->foreignOrient_ = this->foreignOrient_;
  return v;
}

void 
lefiVia_clear(lefiVia * this)
{
  int     i;

  if (this->name_)
    *(this->name_) = '\0';
  if (this->foreign_)
    free(this->foreign_);
  this->foreign_ = 0;
  this->hasDefault_ = 0;
  this->hasResistance_ = 0;
  this->hasForeignPnt_ = 0;
  this->hasTopOfStack_ = 0;
  this->foreignOrient_ = -1;
  for (i = 0;
       i < this->numProps_;
       i++) {
    free(this->propName_[i]);
    this->propName_[i] = 0;
    if (this->propValue_[i])
      free(this->propValue_[i]);
    this->propValue_[i] = 0;
  }
  this->numProps_ = 0;
  for (i = 0;
       i < this->numLayers_;
       i++) {
    lefiViaLayer_Destroy(this->layers_[i]);
    free((char *) (this->layers_[i]));
    this->layers_[i] = 0;
  }
  this->numLayers_ = 0;
}

void 
lefiVia_setName(lefiVia * this,
		const char *name,
		int deflt)
{
  int     len;

  lefiVia_clear(this);
  this->hasDefault_ = deflt ? 1 : 0;
  len = strlen(name) + 1;
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char *) malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
}

void 
lefiVia_setResistance(lefiVia * this,
		      double num)
{
  this->hasResistance_ = 1;
  this->resistance_ = num;
}

void 
lefiVia_bumpProps(lefiVia * this)
{
  int     i;

  double *d;

  char  **n;

  char  **v;

  if (this->propsAllocated_ == 0)
    this->propsAllocated_ = 2;
  else
    this->propsAllocated_ *= 2;
  d = (double *) malloc(sizeof(double) * this->propsAllocated_);
  n = (char **) malloc(sizeof(char *) * this->propsAllocated_);
  v = (char **) malloc(sizeof(char *) * this->propsAllocated_);
  for (i = 0;
       i < this->numProps_;
       i++) {
    d[i] = this->propDValue_[i];
    n[i] = this->propName_[i];
    v[i] = this->propValue_[i];
  }
  if (this->numProps_ > 0) {
    free((char *) (this->propDValue_));
    free((char *) (this->propName_));
    free((char *) (this->propValue_));
  }
  this->propDValue_ = d;
  this->propName_ = n;
  this->propValue_ = v;
}

void 
lefiVia_addProp(lefiVia * this,
		const char *name,
		const char *value)
{
  int     len = strlen(name) + 1;

  if (this->numProps_ == this->propsAllocated_)
    lefiVia_bumpProps(this);
  this->propName_[this->numProps_] = (char *) malloc(len);
  strcpy(this->propName_[this->numProps_], CASE(name));
  len = strlen(value) + 1;
  this->propValue_[this->numProps_] = (char *) malloc(len);
  strcpy(this->propValue_[this->numProps_], CASE(value));
  this->propDValue_[this->numProps_] = 0.0;
  this->numProps_ += 1;
}

void 
lefiVia_addNumProp(lefiVia * this,
		   const char *name,
		   double d)
{
  int     len = strlen(name) + 1;

  if (this->numProps_ == this->propsAllocated_)
    lefiVia_bumpProps(this);
  this->propName_[this->numProps_] = (char *) malloc(len);
  strcpy(this->propName_[this->numProps_], CASE(name));
  this->propValue_[this->numProps_] = 0;
  this->propDValue_[this->numProps_] = d;
  this->numProps_ += 1;
}

void 
lefiVia_setForeign(lefiVia * this,
		   const char *name,
		   int hasPnt,
		   double x,
		   double y,
		   int orient)
{
  int     len = strlen(name) + 1;

  this->hasForeignPnt_ = hasPnt;
  this->foreignOrient_ = orient;
  this->foreignX_ = x;
  this->foreignY_ = y;
  this->foreign_ = (char *) malloc(len);
  strcpy(this->foreign_, CASE(name));
}

void 
lefiVia_setTopOfStack(lefiVia * this)
{
  this->hasTopOfStack_ = 1;
}

void 
lefiVia_addLayer(lefiVia * this,
		 const char *name)
{
  lefiViaLayer *newl;

  if (this->numLayers_ == this->layersAllocated_) {
    int     i;

    lefiViaLayer **l;

    this->layersAllocated_ *= 2;
    l = (lefiViaLayer **) malloc(sizeof(lefiViaLayer *) * this->layersAllocated_);
    for (i = 0;
	 i < this->numLayers_;
	 i++) {
      l[i] = this->layers_[i];
    }
    free((char *) (this->layers_));
    this->layers_ = l;
  }
  newl = (lefiViaLayer *) malloc(sizeof(lefiViaLayer));
  lefiViaLayer_Init(newl);
  this->layers_[this->numLayers_] = newl;
  lefiViaLayer_setName(newl,
		       name);
  this->numLayers_ += 1;
}

void 
lefiVia_addRectToLayer(lefiVia * this,
		       double xl,
		       double yl,
		       double xh,
		       double yh)
{
  lefiViaLayer_addRect(this->layers_[this->numLayers_ - 1],
		       xl,
		       yl,
		       xh,
		       yh);
}

int 
lefiVia_hasDefault(const lefiVia * this)
{
  return this->hasDefault_;
}

int 
lefiVia_hasForeign(const lefiVia * this)
{
  return this->foreign_ ? 1 : 0;
}

int 
lefiVia_hasForeignPnt(const lefiVia * this)
{
  return this->hasForeignPnt_;
}

int 
lefiVia_hasForeignOrient(const lefiVia * this)
{
  return this->foreignOrient_ == -1 ? 0 : 1;
}

int 
lefiVia_hasProperties(const lefiVia * this)
{
  return this->numProps_ ? 1 : 0;
}

int 
lefiVia_hasResistance(const lefiVia * this)
{
  return this->hasResistance_;
}

int 
lefiVia_hasTopOfStack(const lefiVia * this)
{
  return this->hasTopOfStack_;
}

int 
lefiVia_numLayers(const lefiVia * this)
{
  return this->numLayers_;
}

char   *
lefiVia_layerName(const lefiVia * this,
		  int layerNum)
{
  lefiViaLayer *vl;

  if (layerNum < 0 || layerNum >= this->numLayers_) {
    lefiError("ERROR: bad layer number index");
    return 0;
  }
  vl = this->layers_[layerNum];
  return lefiViaLayer_name(vl);
}

int 
lefiVia_numRects(const lefiVia * this,
		 int layerNum)
{
  lefiViaLayer *vl;

  if (layerNum < 0 || layerNum >= this->numLayers_) {
    lefiError("ERROR: bad layer number index");
    return 0;
  }
  vl = this->layers_[layerNum];
  return lefiViaLayer_numRects(vl);
}

double 
lefiVia_xl(const lefiVia * this,
	   int layerNum,
	   int rectNum)
{
  lefiViaLayer *vl;

  if (layerNum < 0 || layerNum >= this->numLayers_) {
    lefiError("ERROR: bad layer number index");
    return 0;
  }
  vl = this->layers_[layerNum];
  return lefiViaLayer_xl(vl,
			 rectNum);
}

double 
lefiVia_yl(const lefiVia * this,
	   int layerNum,
	   int rectNum)
{
  lefiViaLayer *vl;

  if (layerNum < 0 || layerNum >= this->numLayers_) {
    lefiError("ERROR: bad layer number index");
    return 0;
  }
  vl = this->layers_[layerNum];
  return lefiViaLayer_yl(vl,
			 rectNum);
}

double 
lefiVia_xh(const lefiVia * this,
	   int layerNum,
	   int rectNum)
{
  lefiViaLayer *vl;

  if (layerNum < 0 || layerNum >= this->numLayers_) {
    lefiError("ERROR: bad layer number index");
    return 0;
  }
  vl = this->layers_[layerNum];
  return lefiViaLayer_xh(vl,
			 rectNum);
}

double 
lefiVia_yh(const lefiVia * this,
	   int layerNum,
	   int rectNum)
{
  lefiViaLayer *vl;

  if (layerNum < 0 || layerNum >= this->numLayers_) {
    lefiError("ERROR: bad layer number index");
    return 0;
  }
  vl = this->layers_[layerNum];
  return lefiViaLayer_yh(vl,
			 rectNum);
}

char   *
lefiVia_name(const lefiVia * this)
{
  return this->name_;
}

double 
lefiVia_resistance(const lefiVia * this)
{
  return this->resistance_;
}

/*
 * Given an index from 0 to numProperties()-1 return
 *//* information about that property. */ int 
lefiVia_numProperties(const lefiVia * this)
{
  return this->numProps_;
}

char   *
lefiVia_propName(const lefiVia * this,
		 int index)
{
  if (index < 0 || index >= this->numProps_) {
    lefiError("Bad prop number index");
    return 0;
  }
  return this->propName_[index];
}

char   *
lefiVia_propValue(const lefiVia * this,
		  int index)
{
  if (index < 0 || index >= this->numProps_) {
    lefiError("Bad prop number index");
    return 0;
  }
  return this->propValue_[index];
}

double 
lefiVia_propNumber(const lefiVia * this,
		   int index)
{
  if (index < 0 || index >= this->numProps_) {
    lefiError("Bad prop number index");
    return 0;
  }
  return this->propDValue_[index];
}

int 
lefiVia_propIsNumber(const lefiVia * this,
		     int index)
{
  if (index < 0 || index >= this->numProps_) {
    lefiError("Bad prop number index");
    return 0;
  }
  return this->propValue_[index] ? 0 : 1;
}

int 
lefiVia_propIsString(const lefiVia * this,
		     int index)
{
  if (index < 0 || index >= this->numProps_) {
    lefiError("Bad prop number index");
    return 0;
  }
  return this->propValue_[index] ? 1 : 0;
}

char   *
lefiVia_foreign(const lefiVia * this)
{
  return this->foreign_;
}

double 
lefiVia_foreignX(const lefiVia * this)
{
  return this->foreignX_;
}

double 
lefiVia_foreignY(const lefiVia * this)
{
  return this->foreignY_;
}

int 
lefiVia_foreignOrient(const lefiVia * this)
{
  return this->foreignOrient_;
}

/*
 * Debug print
 */ void 
lefiVia_print(const lefiVia * this,
	      FILE * f)
{
  int     i;

  int     h;

  fprintf(f, "Via %s:\n", lefiVia_name(this));
  if (lefiVia_hasDefault(this))
    fprintf(f, " DEFAULT\n");
  if (lefiVia_hasForeign(this)) {
    fprintf(f, " foreign %s", lefiVia_foreign(this));
    if (lefiVia_hasForeignPnt(this)) {
      fprintf(f, " %lg,%lg", lefiVia_foreignX(this), lefiVia_foreignY(this));
    }
    if (lefiVia_hasForeignOrient(this)) {
      fprintf(f, " orient %d", lefiVia_foreignOrient(this));
    }
    fprintf(f, "\n");
  }
  if (lefiVia_hasResistance(this))
    fprintf(f, " RESISTANCE %lg\n", lefiVia_resistance(this));
  if (lefiVia_hasProperties(this)) {
    for (i = 0;
	 i < lefiVia_numProperties(this);
	 i++)
      if (lefiVia_propIsString(this,
			       i))
	fprintf(f, " PROP %s %s\n", lefiVia_propName(this,
						 i), lefiVia_propValue(this,
								       i));
      else
	fprintf(f, " PROP %s %lg\n", lefiVia_propName(this,
						i), lefiVia_propNumber(this,
								       i));
  }
  for (i = 0;
       i < lefiVia_numLayers(this);
       i++) {
    fprintf(f, " LAYER %s\n", lefiVia_layerName(this,
						i));
    for (h = 0;
	 h < lefiVia_numRects(this,
			      i);
	 h++) {
      fprintf(f, " RECT %lg,%lg %lg,%lg\n", lefiVia_xl(this,
						       i,
						       h), lefiVia_yl(this,
								      i,
							h), lefiVia_xh(this,
								       i,
							h), lefiVia_yh(this,
								       i,
								       h));
    }
  }
}
