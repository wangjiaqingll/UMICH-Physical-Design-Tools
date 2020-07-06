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
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "defiDebug.h"
#include "defiPinProp.h"

/*
 * defiPinProp
 */ defiPinProp *
defiPinProp_CreateUninitialized()
{
  defiPinProp *this = (defiPinProp *) malloc(sizeof(defiPinProp));

  defiPinProp_Init(this);
  return (this);
}

void 
defiPinProp_Init(defiPinProp * this)
{
  this->instName_ = (char *) malloc(16);
  this->pinName_ = (char *) malloc(16);
  this->pinNameSize_ = 16;
  this->instNameSize_ = 16;
  this->isPin_ = 0;
  this->numProps_ = 0;
  this->propsAllocated_ = 2;
  this->propNames_ = (char **) malloc(sizeof(char *) * 2);
  this->propValues_ = (char **) malloc(sizeof(char *) * 2);
}

void
defiPinProp_Delete(defiPinProp * this)
{
  if (this == NULL)
    return;
  defiPinProp_Destroy(this);
  free(this);
}

void 
defiPinProp_Destroy(defiPinProp * this)
{
  defiPinProp_clear(this);
  free(this->instName_);
  free(this->pinName_);
  free((char *) (this->propNames_));
  free((char *) (this->propValues_));
}

void 
defiPinProp_clear(defiPinProp * this)
{
  int     i;

  for (i = 0;
       i < this->numProps_;
       i++) {
    free(this->propNames_[i]);
    free(this->propValues_[i]);
  }
  this->numProps_ = 0;
  this->isPin_ = 0;
}

void 
defiPinProp_setName(defiPinProp * this,
		    const char *inst,
		    const char *pin)
{
  int     len = strlen(inst) + 1;

  if ((strcmp(inst, "PIN") == 0) || (strcmp(inst, "pin") == 0)) {
    this->isPin_ = 1;
    this->instName_[0] = '\0';
  }
  else {
    if (this->instNameSize_ < len) {
      this->instNameSize_ = len;
      free(this->instName_);
      this->instName_ = (char *) malloc(len);
    }
    strcpy(this->instName_, DEFCASE(inst));
  }
  len = strlen(pin) + 1;
  if (this->pinNameSize_ < len) {
    this->pinNameSize_ = len;
    free(this->pinName_);
    this->pinName_ = (char *) malloc(len);
  }
  strcpy(this->pinName_, DEFCASE(pin));
}

int 
defiPinProp_isPin(const defiPinProp * this)
{
  return this->isPin_ ? 1 : 0;
}

const char *
defiPinProp_instName(const defiPinProp * this)
{
  return this->instName_;
}

const char *
defiPinProp_pinName(const defiPinProp * this)
{
  return this->pinName_;
}

void 
defiPinProp_addProperty(defiPinProp * this,
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
defiPinProp_numProps(const defiPinProp * this)
{
  return this->numProps_;
}

const char *
defiPinProp_propName(const defiPinProp * this,
		     int index)
{
  if (index < 0 || index >= this->numProps_) {
    defiError("bad index for PinProp property");
    return 0;
  }
  return this->propNames_[index];
}

const char *
defiPinProp_propValue(const defiPinProp * this,
		      int index)
{
  if (index < 0 || index >= this->numProps_) {
    defiError("bad index for PinProp property");
    return 0;
  }
  return this->propValues_[index];
}

void 
defiPinProp_print(const defiPinProp * this,
		  FILE * f)
{
  int     i;

  fprintf(f, "PinProp %s %s\n", defiPinProp_instName(this), defiPinProp_pinName(this));
  for (i = 0;
       i < defiPinProp_numProps(this);
       i++) {
    fprintf(f, " %s %s\n", defiPinProp_propName(this,
					     i), defiPinProp_propValue(this,
								       i));
  }
}
