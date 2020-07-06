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
#include "defiVia.h"
#include "defiDebug.h"

/*
 * defiVia
 */ defiVia *
defiVia_CreateUninitialized()
{
  defiVia *this = (defiVia *) malloc(sizeof(defiVia));

  defiVia_Init(this);
  return (this);
}

void 
defiVia_Init(defiVia * this)
{
  this->name_ = 0;
  this->nameLength_ = 0;
  this->pattern_ = 0;
  this->patternLength_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->numLayers_ = 0;
  this->layersLength_ = 0;
  this->layers_ = 0;
}

void
defiVia_Delete(defiVia * this)
{
  if (this == NULL)
    return;
  defiVia_Destroy(this);
  free(this);
}

void 
defiVia_Destroy(defiVia * this)
{
  int     i;

  if (this->name_)
    free(this->name_);
  if (this->pattern_)
    free(this->pattern_);
  if (this->layers_) {
    for (i = 0;
	 i < this->numLayers_;
	 i++)
      if (this->layers_[i])
	free(this->layers_[i]);
    free((char *) (this->layers_));
    free((char *) (this->xl_));
    free((char *) (this->yl_));
    free((char *) (this->xh_));
    free((char *) (this->yh_));
  }
}

void 
defiVia_setup(defiVia * this,
	      const char *name)
{
  int     i;

  int     len = strlen(name) + 1;

  if (len > this->nameLength_) {
    this->nameLength_ = len;
    this->name_ = (char *) realloc(this->name_, len);
  }
  strcpy(this->name_, DEFCASE(name));
  if (this->pattern_)
    *(this->pattern_) = 0;
  if (this->layers_) {
    for (i = 0;
	 i < this->numLayers_;
	 i++) {
      if (this->layers_[i])
	free(this->layers_[i]);
      this->layers_[i] = 0;
    }
  }
  this->numLayers_ = 0;
}

void 
defiVia_addPattern(defiVia * this,
		   const char *pattern)
{
  int     len = strlen(pattern) + 1;

  if (len > this->patternLength_) {
    this->patternLength_ = len;
    this->pattern_ = (char *) realloc(this->pattern_, len);
  }
  strcpy(this->pattern_, DEFCASE(pattern));
}

void 
defiVia_addLayer(defiVia * this,
		 const char *layer,
		 int xl,
		 int yl,
		 int xh,
		 int yh)
{
  char   *l;

  int     len;

  if (this->numLayers_ >= this->layersLength_) {
    int     i;

    char  **newl;

    int    *ints;

    this->layersLength_ = this->layersLength_ ? 2 * this->layersLength_ : 8;
    newl = (char **) malloc(this->layersLength_ * sizeof(char *));
    for (i = 0;
	 i < this->numLayers_;
	 i++)
      newl[i] = this->layers_[i];
    if (this->layers_)
      free((char *) (this->layers_));
    this->layers_ = newl;
    ints = (int *) malloc(this->layersLength_ * sizeof(int));
    for (i = 0;
	 i < this->numLayers_;
	 i++)
      ints[i] = this->xl_[i];
    if (this->xl_)
      free((char *) (this->xl_));
    this->xl_ = ints;
    ints = (int *) malloc(this->layersLength_ * sizeof(int));
    for (i = 0;
	 i < this->numLayers_;
	 i++)
      ints[i] = this->yl_[i];
    if (this->yl_)
      free((char *) (this->yl_));
    this->yl_ = ints;
    ints = (int *) malloc(this->layersLength_ * sizeof(int));
    for (i = 0;
	 i < this->numLayers_;
	 i++)
      ints[i] = this->xh_[i];
    if (this->xh_)
      free((char *) (this->xh_));
    this->xh_ = ints;
    ints = (int *) malloc(this->layersLength_ * sizeof(int));
    for (i = 0;
	 i < this->numLayers_;
	 i++)
      ints[i] = this->yh_[i];
    if (this->yh_)
      free((char *) (this->yh_));
    this->yh_ = ints;
  }
  len = strlen(layer) + 1;
  l = (char *) malloc(len);
  strcpy(l, DEFCASE(layer));
  this->layers_[this->numLayers_] = l;
  this->xl_[this->numLayers_] = xl;
  this->yl_[this->numLayers_] = yl;
  this->xh_[this->numLayers_] = xh;
  this->yh_[this->numLayers_] = yh;
  this->numLayers_++;
}

int 
defiVia_hasPattern(const defiVia * this)
{
  return this->pattern_ && *(this->pattern_) ? 1 : 0;
}

const char *
defiVia_pattern(const defiVia * this)
{
  return this->pattern_;
}

const char *
defiVia_name(const defiVia * this)
{
  return this->name_;
}

int 
defiVia_numLayers(const defiVia * this)
{
  return this->numLayers_;
}

void 
defiVia_layer(const defiVia * this,
	      int index,
	      char **layer,
	      int *xl,
	      int *yl,
	      int *xh,
	      int *yh)
{
  if (index >= 0 && index < this->numLayers_) {
    if (layer)
      *layer = this->layers_[index];
    if (xl)
      *xl = this->xl_[index];
    if (yl)
      *yl = this->yl_[index];
    if (xh)
      *xh = this->xh_[index];
    if (yh)
      *yh = this->yh_[index];
  }
}

void 
defiVia_print(const defiVia * this,
	      FILE * f)
{
  int     i;

  int     xl, yl, xh, yh;

  char   *c;

  fprintf(f, "via '%s'\n", defiVia_name(this));
  if (defiVia_hasPattern(this))
    fprintf(f, " pattern '%s'\n", defiVia_pattern(this));
  for (i = 0;
       i < defiVia_numLayers(this);
       i++) {
    defiVia_layer(this,
		  i,
		  &c,
		  &xl,
		  &yl,
		  &xh,
		  &yh);
    fprintf(f, " layer '%s' %d,%d %d,%d\n", c, xl, yl, xh, yh);
  }
}
