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
#include "defiRegion.h"
#include "defiDebug.h"

/*
 * defiRegion
 */ defiRegion *
defiRegion_CreateUninitialized()
{
  defiRegion *this = (defiRegion *) malloc(sizeof(defiRegion));

  defiRegion_Init(this);
  return (this);
}

void 
defiRegion_Init(defiRegion * this)
{
  this->name_ = 0;
  this->nameLength_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->numProps_ = 0;
  this->propsAllocated_ = 2;
  this->propNames_ = (char **) malloc(sizeof(char *) * 2);
  this->propValues_ = (char **) malloc(sizeof(char *) * 2);
  defiRegion_clear(this);
  this->numRectangles_ = 0;
  this->rectanglesAllocated_ = 1;
  this->xl_ = (int *) malloc(sizeof(int) * 1);
  this->yl_ = (int *) malloc(sizeof(int) * 1);
  this->xh_ = (int *) malloc(sizeof(int) * 1);
  this->yh_ = (int *) malloc(sizeof(int) * 1);
}

void
defiRegion_Delete(defiRegion * this)
{
  if (this == NULL)
    return;
  defiRegion_Destroy(this);
  free(this);
}

void 
defiRegion_clear(defiRegion * this)
{
  int     i;

  for (i = 0;
       i < this->numProps_;
       i++) {
    free(this->propNames_[i]);
    free(this->propValues_[i]);
  }
  this->numProps_ = 0;
  this->numRectangles_ = 0;
}

void 
defiRegion_Destroy(defiRegion * this)
{
  if (this->name_)
    free(this->name_);
  defiRegion_clear(this);
  this->name_ = 0;
  this->nameLength_ = 0;
  free((char *) (this->xl_));
  free((char *) (this->yl_));
  free((char *) (this->xh_));
  free((char *) (this->yh_));
  free((char *) (this->propNames_));
  free((char *) (this->propValues_));
}

void 
defiRegion_addRect(defiRegion * this,
		   int xl,
		   int yl,
		   int xh,
		   int yh)
{
  if (this->numRectangles_ == this->rectanglesAllocated_) {
    int     i;

    int     max = this->rectanglesAllocated_ = this->rectanglesAllocated_ * 2;

    int    *newxl = (int *) malloc(sizeof(int) * max);

    int    *newyl = (int *) malloc(sizeof(int) * max);

    int    *newxh = (int *) malloc(sizeof(int) * max);

    int    *newyh = (int *) malloc(sizeof(int) * max);

    for (i = 0;
	 i < this->numRectangles_;
	 i++) {
      newxl[i] = this->xl_[i];
      newyl[i] = this->yl_[i];
      newxh[i] = this->xh_[i];
      newyh[i] = this->yh_[i];
    }
    free((char *) (this->xl_));
    free((char *) (this->yl_));
    free((char *) (this->xh_));
    free((char *) (this->yh_));
    this->xl_ = newxl;
    this->yl_ = newyl;
    this->xh_ = newxh;
    this->yh_ = newyh;
  }
  this->xl_[this->numRectangles_] = xl;
  this->yl_[this->numRectangles_] = yl;
  this->xh_[this->numRectangles_] = xh;
  this->yh_[this->numRectangles_] = yh;
  this->numRectangles_ += 1;
}

void 
defiRegion_setup(defiRegion * this,
		 const char *name)
{
  int     len = strlen(name) + 1;

  defiRegion_clear(this);
  if (len > this->nameLength_) {
    if (this->name_)
      free(this->name_);
    this->nameLength_ = len;
    this->name_ = (char *) malloc(len);
  }
  strcpy(this->name_, DEFCASE(name));
}

void 
defiRegion_addProperty(defiRegion * this,
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
defiRegion_numRectangles(const defiRegion * this)
{
  return this->numRectangles_;
}

int 
defiRegion_numProps(const defiRegion * this)
{
  return this->numProps_;
}

const char *
defiRegion_propName(const defiRegion * this,
		    int index)
{
  if (index < 0 || index >= this->numProps_) {
    defiError("bad index for region property name");
    return 0;
  }
  return this->propNames_[index];
}

const char *
defiRegion_propValue(const defiRegion * this,
		     int index)
{
  if (index < 0 || index >= this->numProps_) {
    defiError("bad index for region property value");
    return 0;
  }
  return this->propValues_[index];
}

const char *
defiRegion_name(const defiRegion * this)
{
  return this->name_;
}

int 
defiRegion_xl(const defiRegion * this,
	      int index)
{
  if (index < 0 || index >= this->numRectangles_) {
    defiError("bad index for region xl");
    return 0;
  }
  return this->xl_[index];
}

int 
defiRegion_yl(const defiRegion * this,
	      int index)
{
  if (index < 0 || index >= this->numRectangles_) {
    defiError("bad index for region yl");
    return 0;
  }
  return this->yl_[index];
}

int 
defiRegion_xh(const defiRegion * this,
	      int index)
{
  if (index < 0 || index >= this->numRectangles_) {
    defiError("bad index for region xh");
    return 0;
  }
  return this->xh_[index];
}

int 
defiRegion_yh(const defiRegion * this,
	      int index)
{
  if (index < 0 || index >= this->numRectangles_) {
    defiError("bad index for region yh");
    return 0;
  }
  return this->yh_[index];
}

void 
defiRegion_print(const defiRegion * this,
		 FILE * f)
{
  int     i;

  fprintf(f, "Region '%s'", defiRegion_name(this));
  for (i = 0;
       i < defiRegion_numRectangles(this);
       i++) {
    fprintf(f, " %d %d %d %d", defiRegion_xl(this,
					     i), defiRegion_yl(this,
						     i), defiRegion_xh(this,
						     i), defiRegion_yh(this,
								       i));
  }
  fprintf(f, "\n");
}
