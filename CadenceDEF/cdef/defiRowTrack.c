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
#include <malloc.h>
#include <stdlib.h>
#include "defiRowTrack.h"
#include "defiDebug.h"

/*
 * defiRow
 */ defiRow *
defiRow_CreateUninitialized()
{
  defiRow *this = (defiRow *) malloc(sizeof(defiRow));

  defiRow_Init(this);
  return (this);
}

void 
defiRow_Init(defiRow * this)
{
  this->nameLength_ = 0;
  this->name_ = 0;
  this->macroLength_ = 0;
  this->macro_ = 0;
  this->orient_ = 0;
  this->x_ = 0.0;
  this->y_ = 0.0;
  this->xStep_ = 0.0;
  this->yStep_ = 0.0;
  this->xNum_ = 0.0;
  this->yNum_ = 0.0;
  this->numProps_ = 0;
  this->propsAllocated_ = 2;
  this->propNames_ = (char **) malloc(sizeof(char *) * 2);
  this->propValues_ = (char **) malloc(sizeof(char *) * 2);
}

void
defiRow_Delete(defiRow * this)
{
  if (this == NULL)
    return;
  defiRow_Destroy(this);
  free(this);
}

void 
defiRow_Destroy(defiRow * this)
{
  defiRow_clear(this);
  if (this->name_)
    free(this->name_);
  if (this->macro_)
    free(this->macro_);
  free((char *) (this->propNames_));
  free((char *) (this->propValues_));
}

void 
defiRow_clear(defiRow * this)
{
  int     i;

  for (i = 0;
       i < this->numProps_;
       i++) {
    free(this->propNames_[i]);
    free(this->propValues_[i]);
  }
  this->numProps_ = 0;
}

void 
defiRow_setup(defiRow * this,
	      const char *name,
	      const char *macro,
	      double x,
	      double y,
	      int orient)
{
  int     len = strlen(name) + 1;

  defiRow_clear(this);
  if (len > this->nameLength_) {
    if (this->name_)
      free(this->name_);
    this->nameLength_ = len;
    this->name_ = (char *) malloc(len);
  }
  strcpy(this->name_, DEFCASE(name));
  len = strlen(macro) + 1;
  if (len > this->macroLength_) {
    if (this->macro_)
      free(this->macro_);
    this->macroLength_ = len;
    this->macro_ = (char *) malloc(len);
  }
  strcpy(this->macro_, DEFCASE(macro));
  this->x_ = x;
  this->y_ = y;
  this->xStep_ = 0.0;
  this->yStep_ = 0.0;
  this->xNum_ = 0.0;
  this->yNum_ = 0.0;
  this->orient_ = orient;
}

void 
defiRow_setDo(defiRow * this,
	      double x_num,
	      double y_num,
	      double x_step,
	      double y_step)
{
  this->xStep_ = x_step;
  this->yStep_ = y_step;
  this->xNum_ = x_num;
  this->yNum_ = y_num;
}

void 
defiRow_addProperty(defiRow * this,
		    const char *name,
		    const char *value)
{
  int     len;

  if (this->numProps_ == this->propsAllocated_) {
    int     i;

    char  **nn;

    char  **nv;

    this->propsAllocated_ *= 2;
    nn = (char **) malloc(sizeof(char *) * this->propsAllocated_);
    nv = (char **) malloc(sizeof(char *) * this->propsAllocated_);
    for (i = 0;
	 i < this->numProps_;
	 i++) {
      nn[i] = this->propNames_[i];
      nv[i] = this->propValues_[i];
    }
    free((char *) (this->propNames_));
    free((char *) (this->propValues_));
    this->propNames_ = nn;
    this->propValues_ = nv;
  }
  len = strlen(name) + 1;
  this->propNames_[this->numProps_] = (char *) malloc(len);
  strcpy(this->propNames_[this->numProps_], DEFCASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProps_] = (char *) malloc(len);
  strcpy(this->propValues_[this->numProps_], DEFCASE(value));
  this->numProps_ += 1;
}

int 
defiRow_numProps(const defiRow * this)
{
  return this->numProps_;
}

const char *
defiRow_propName(const defiRow * this,
		 int index)
{
  if (index < 0 || index >= this->numProps_) {
    defiError("bad index for region property");
    return 0;
  }
  return this->propNames_[index];
}

const char *
defiRow_propValue(const defiRow * this,
		  int index)
{
  if (index < 0 || index >= this->numProps_) {
    defiError("bad index for region property");
    return 0;
  }
  return this->propValues_[index];
}

const char *
defiRow_name(const defiRow * this)
{
  return this->name_;
}

const char *
defiRow_macro(const defiRow * this)
{
  return this->macro_;
}

double 
defiRow_x(const defiRow * this)
{
  return this->x_;
}

double 
defiRow_y(const defiRow * this)
{
  return this->y_;
}

double 
defiRow_xNum(const defiRow * this)
{
  return this->xNum_;
}

double 
defiRow_yNum(const defiRow * this)
{
  return this->yNum_;
}

int 
defiRow_orient(const defiRow * this)
{
  return this->orient_;
}

double 
defiRow_xStep(const defiRow * this)
{
  return this->xStep_;
}

double 
defiRow_yStep(const defiRow * this)
{
  return this->yStep_;
}

void 
defiRow_print(const defiRow * this,
	      FILE * f)
{
  fprintf(f, "Row '%s' '%s' %g,%g orient %d\n", defiRow_name(this), defiRow_macro(this), defiRow_x(this), defiRow_y(this), defiRow_orient(this));
  fprintf(f, " DO X %g STEP %g\n", defiRow_xNum(this), defiRow_xStep(this));
  fprintf(f, " DO Y %g STEP %g\n", defiRow_yNum(this), defiRow_yStep(this));
}

/*
 * defiTrack
 */ defiTrack *
defiTrack_CreateUninitialized()
{
  defiTrack *this = (defiTrack *) malloc(sizeof(defiTrack));

  defiTrack_Init(this);
  return (this);
}

void 
defiTrack_Init(defiTrack * this)
{
  this->macro_ = 0;
  this->macroLength_ = 0;
  this->x_ = 0.0;
  this->xNum_ = 0.0;
  this->xStep_ = 0.0;
  this->numLayers_ = 0;
  this->layers_ = 0;
}

void
defiTrack_Delete(defiTrack * this)
{
  if (this == NULL)
    return;
  defiTrack_Destroy(this);
  free(this);
}

void 
defiTrack_Destroy(defiTrack * this)
{
  int     i;

  if (this->macro_)
    free(this->macro_);
  if (this->layers_) {
    for (i = 0;
	 i < this->numLayers_;
	 i++)
      if (this->layers_[i])
	free(this->layers_[i]);
    free((char *) (this->layers_));
  }
}

void 
defiTrack_setup(defiTrack * this,
		const char *macro)
{
  int     i;

  int     len = strlen(macro) + 1;

  if (len > this->macroLength_) {
    if (this->macro_)
      free(this->macro_);
    this->macroLength_ = len;
    this->macro_ = (char *) malloc(len);
  }
  strcpy(this->macro_, DEFCASE(macro));
  if (this->layers_) {
    for (i = 0;
	 i < this->numLayers_;
	 i++)
      if (this->layers_[i])
	free(this->layers_[i]);
    this->layers_[i] = 0;
  }
  this->numLayers_ = 0;
  this->x_ = 0.0;
  this->xStep_ = 0.0;
  this->xNum_ = 0.0;
}

void 
defiTrack_setDo(defiTrack * this,
		double x,
		double x_num,
		double x_step)
{
  this->x_ = x;
  this->xStep_ = x_step;
  this->xNum_ = x_num;
}

void 
defiTrack_addLayer(defiTrack * this,
		   const char *layer)
{
  char   *l;

  int     len;

  if (this->numLayers_ >= this->layersLength_) {
    int     i;

    char  **newl;

    this->layersLength_ = this->layersLength_ ? 2 * this->layersLength_ : 8;
    newl = (char **) malloc(this->layersLength_ * sizeof(char *));
    for (i = 0;
	 i < this->numLayers_;
	 i++)
      newl[i] = this->layers_[i];
    if (this->layers_)
      free((char *) (this->layers_));
    this->layers_ = newl;
  }
  len = strlen(layer) + 1;
  l = (char *) malloc(len);
  strcpy(l, DEFCASE(layer));
  this->layers_[this->numLayers_++] = l;
}

const char *
defiTrack_macro(const defiTrack * this)
{
  return this->macro_;
}

double 
defiTrack_x(const defiTrack * this)
{
  return this->x_;
}

double 
defiTrack_xNum(const defiTrack * this)
{
  return this->xNum_;
}

double 
defiTrack_xStep(const defiTrack * this)
{
  return this->xStep_;
}

int 
defiTrack_numLayers(const defiTrack * this)
{
  return this->numLayers_;
}

const char *
defiTrack_layer(const defiTrack * this,
		int index)
{
  if (index >= 0 && index < this->numLayers_) {
    return this->layers_[index];
  }
  return 0;
}

void 
defiTrack_print(const defiTrack * this,
		FILE * f)
{
  int     i;

  fprintf(f, "Track '%s'\n", defiTrack_macro(this));
  fprintf(f, " DO %g %g STEP %g\n", defiTrack_x(this), defiTrack_xNum(this), defiTrack_xStep(this));
  fprintf(f, " %d layers ", defiTrack_numLayers(this));
  for (i = 0;
       i < defiTrack_numLayers(this);
       i++) {
    fprintf(f, " '%s'", defiTrack_layer(this,
					i));
  }
  fprintf(f, "\n");
}

/*
 * defiGcellGrid
 */ defiGcellGrid *
defiGcellGrid_CreateUninitialized()
{
  defiGcellGrid *this = (defiGcellGrid *) malloc(sizeof(defiGcellGrid));

  defiGcellGrid_Init(this);
  return (this);
}

void 
defiGcellGrid_Init(defiGcellGrid * this)
{
  this->macro_ = 0;
  this->macroLength_ = 0;
  this->x_ = 0;
  this->xNum_ = 0;
  this->xStep_ = 0;
}

void
defiGcellGrid_Delete(defiGcellGrid * this)
{
  if (this == NULL)
    return;
  defiGcellGrid_Destroy(this);
  free(this);
}

void 
defiGcellGrid_Destroy(defiGcellGrid * this)
{
  if (this->macro_)
    free(this->macro_);
}

void 
defiGcellGrid_setup(defiGcellGrid * this,
		    const char *macro,
		    int x,
		    int xNum,
		    double xStep)
{
  int     len = strlen(macro) + 1;

  if (len > this->macroLength_) {
    if (this->macro_)
      free(this->macro_);
    this->macroLength_ = len;
    this->macro_ = (char *) malloc(len);
  }
  strcpy(this->macro_, DEFCASE(macro));
  this->x_ = x;
  this->xNum_ = xNum;
  this->xStep_ = xStep;
}

int 
defiGcellGrid_x(const defiGcellGrid * this)
{
  return this->x_;
}

int 
defiGcellGrid_xNum(const defiGcellGrid * this)
{
  return this->xNum_;
}

double 
defiGcellGrid_xStep(const defiGcellGrid * this)
{
  return this->xStep_;
}

const char *
defiGcellGrid_macro(const defiGcellGrid * this)
{
  return this->macro_;
}

void 
defiGcellGrid_print(const defiGcellGrid * this,
		    FILE * f)
{
  fprintf(f, "GcellGrid '%s'\n", defiGcellGrid_macro(this));
  fprintf(f, " DO %d %d STEP %5.1lf\n", defiGcellGrid_x(this), defiGcellGrid_xNum(this), defiGcellGrid_xStep(this));
}
