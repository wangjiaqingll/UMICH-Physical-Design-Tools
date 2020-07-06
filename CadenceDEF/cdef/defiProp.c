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
#include <stdlib.h>
#include <string.h>
#include "defiProp.h"
#include "defiDebug.h"
defiProp *
defiProp_CreateUninitialized()
{
  defiProp *this = (defiProp *) malloc(sizeof(defiProp));

  defiProp_Init(this);
  return (this);
}

void 
defiProp_Init(defiProp * this)
{
  this->stringLength_ = 16;
  this->stringData_ = (char *) malloc(16);
  this->nameSize_ = 16;
  this->propName_ = (char *) malloc(16);
  defiProp_clear(this);
}

void 
defiProp_Destroy(defiProp * this)
{
  free(this->stringData_);
  free(this->propName_);
}

void
defiProp_Delete(defiProp * this)
{
  if (this == NULL)
    return;
  defiProp_Destroy(this);
  free(this);
}

void 
defiProp_setPropType(defiProp * this,
		     const char *typ,
		     const char *string)
{
  int     len;

  this->propType_ = (char *) typ;
  if ((len = strlen(string) + 1) > this->nameSize_)
    defiProp_bumpName(this,
		      len);
  strcpy(this->propName_, DEFCASE(string));
}

void 
defiProp_setRange(defiProp * this,
		  double left,
		  double right)
{
  this->hasRange_ = 1;
  this->left_ = left;
  this->right_ = right;
}

void 
defiProp_setNumber(defiProp * this,
		   double d)
{
  this->hasNumber_ = 1;
  this->d_ = d;
}

void 
defiProp_setPropInteger(defiProp * this)
{
  this->dataType_ = 'I';
}

void 
defiProp_setPropReal(defiProp * this)
{
  this->dataType_ = 'R';
}

void 
defiProp_setPropString(defiProp * this)
{
  this->dataType_ = 'S';
}

void 
defiProp_setPropNameMapString(defiProp * this,
			      const char *string)
{
  int     len;

  this->dataType_ = 'N';
  this->hasNameMapString_ = 1;
  if ((len = strlen(string) + 1) > this->stringLength_)
    defiProp_bumpSize(this,
		      len);
  strcpy(this->stringData_, DEFCASE(string));
}

void 
defiProp_setPropQString(defiProp * this,
			const char *string)
{
  int     len;

  this->dataType_ = 'Q';
  if ((len = strlen(string) + 1) > this->stringLength_)
    defiProp_bumpSize(this,
		      len);
  strcpy(this->stringData_, DEFCASE(string));
}

const char *
defiProp_string(const defiProp * this)
{
  return this->stringData_;
}

const char *
defiProp_propType(const defiProp * this)
{
  return this->propType_;
}

int 
defiProp_hasNameMapString(const defiProp * this)
{
  return (int) (this->hasNameMapString_);
}

int 
defiProp_hasNumber(const defiProp * this)
{
  return (int) (this->hasNumber_);
}

int 
defiProp_hasRange(const defiProp * this)
{
  return (int) (this->hasRange_);
}

double 
defiProp_number(const defiProp * this)
{
  return (int) (this->d_);
}

double 
defiProp_left(const defiProp * this)
{
  return (int) (this->left_);
}

double 
defiProp_right(const defiProp * this)
{
  return (int) (this->right_);
}

void 
defiProp_bumpSize(defiProp * this,
		  int size)
{
  free(this->stringData_);
  this->stringData_ = (char *) malloc(size);
  this->stringLength_ = size;
  *(this->stringData_) = '\0';
}

void 
defiProp_bumpName(defiProp * this,
		  int size)
{
  free(this->propName_);
  this->propName_ = (char *) malloc(size);
  this->nameSize_ = size;
  *(this->propName_) = '\0';
}

void 
defiProp_clear(defiProp * this)
{
  if (this->stringData_)
    *(this->stringData_) = '\0';
  if (this->propName_)
    *(this->propName_) = '\0';
  this->propType_ = 0;
  this->hasRange_ = 0;
  this->hasNumber_ = 0;
  this->hasNameMapString_ = 0;
  this->dataType_ = 'B';
  this->d_ = this->left_ = this->right_ = 0.0;
}

int 
defiProp_hasString(const defiProp * this)
{
  return *(this->stringData_) ? 1 : 0;
}

const char *
defiProp_propName(const defiProp * this)
{
  return (this->propName_);
}

char 
defiProp_dataType(const defiProp * this)
{
  return (this->dataType_);
}

void 
defiProp_print(const defiProp * this,
	       FILE * f)
{
  fprintf(f, "Prop type '%s'\n", defiProp_propType(this));
  if (defiProp_hasString(this)) {
    fprintf(f, " string '%s'\n", defiProp_string(this));
  }
  if (defiProp_hasNumber(this)) {
    fprintf(f, " number %5.2lf\n", defiProp_number(this));
  }
  if (defiProp_hasRange(this)) {
    fprintf(f, " range %5.2lf - %5.2lf\n", defiProp_left(this), defiProp_right(this));
  }
}
