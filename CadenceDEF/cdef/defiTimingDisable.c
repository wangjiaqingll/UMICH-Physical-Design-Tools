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
#include "defiTimingDisable.h"
#include "defiDebug.h"

/*
 * defiTimingDisable
 */ defiTimingDisable *
defiTimingDisable_CreateUninitialized()
{
  defiTimingDisable *this = (defiTimingDisable *) malloc(sizeof(defiTimingDisable));

  defiTimingDisable_Init(this);
  return (this);
}

void 
defiTimingDisable_Init(defiTimingDisable * this)
{
  this->fromInst_ = 0;
  this->fromInstLength_ = 0;
  this->toInst_ = 0;
  this->toInstLength_ = 0;
  this->toPin_ = 0;
  this->toPinLength_ = 0;
  this->fromPin_ = 0;
  this->fromPinLength_ = 0;
}

void
defiTimingDisable_Delete(defiTimingDisable * this)
{
  if (this == NULL)
    return;
  defiTimingDisable_Destroy(this);
  free(this);
}

void 
defiTimingDisable_Destroy(defiTimingDisable * this)
{
  defiTimingDisable_clear(this);
  if (this->fromInst_)
    free(this->fromInst_);
  this->fromInst_ = 0;
  this->fromInstLength_ = 0;
  if (this->toInst_)
    free(this->toInst_);
  this->toInst_ = 0;
  this->toInstLength_ = 0;
  if (this->toPin_)
    free(this->toPin_);
  this->toPin_ = 0;
  this->toPinLength_ = 0;
  if (this->fromPin_)
    free(this->fromPin_);
  this->fromPin_ = 0;
  this->fromPinLength_ = 0;
}

void 
defiTimingDisable_clear(defiTimingDisable * this)
{
  this->hasFromTo_ = 0;
  this->hasThru_ = 0;
  this->hasMacro_ = 0;
  this->hasReentrantPathsFlag_ = 0;
}

void 
defiTimingDisable_setReentrantPathsFlag(defiTimingDisable * this)
{
  this->hasReentrantPathsFlag_ = 1;
}

void 
defiTimingDisable_setFromTo(defiTimingDisable * this,
			    const char *fromInst,
			    const char *fromPin,
			    const char *toInst,
			    const char *toPin)
{
  int     len;

  defiTimingDisable_clear(this);
  this->hasFromTo_ = 1;
  len = strlen(fromInst) + 1;
  if (len > this->fromInstLength_) {
    if (this->fromInst_)
      free(this->fromInst_);
    this->fromInstLength_ = len;
    this->fromInst_ = (char *) malloc(len);
  }
  strcpy(this->fromInst_, DEFCASE(fromInst));
  len = strlen(fromPin) + 1;
  if (len > this->fromPinLength_) {
    if (this->fromPin_)
      free(this->fromPin_);
    this->fromPinLength_ = len;
    this->fromPin_ = (char *) malloc(len);
  }
  strcpy(this->fromPin_, DEFCASE(fromPin));
  len = strlen(toInst) + 1;
  if (len > this->toInstLength_) {
    if (this->toInst_)
      free(this->toInst_);
    this->toInstLength_ = len;
    this->toInst_ = (char *) malloc(len);
  }
  strcpy(this->toInst_, toInst);
  len = strlen(toPin) + 1;
  if (len > this->toPinLength_) {
    if (this->toPin_)
      free(this->toPin_);
    this->toPinLength_ = len;
    this->toPin_ = (char *) malloc(len);
  }
  strcpy(this->toPin_, toPin);
}

void 
defiTimingDisable_setThru(defiTimingDisable * this,
			  const char *fromInst,
			  const char *fromPin)
{
  int     len;

  defiTimingDisable_clear(this);
  this->hasThru_ = 1;
  len = strlen(fromInst) + 1;
  if (len > this->fromInstLength_) {
    if (this->fromInst_)
      free(this->fromInst_);
    this->fromInstLength_ = len;
    this->fromInst_ = (char *) malloc(len);
  }
  strcpy(this->fromInst_, DEFCASE(fromInst));
  len = strlen(fromPin) + 1;
  if (len > this->fromPinLength_) {
    if (this->fromPin_)
      free(this->fromPin_);
    this->fromPinLength_ = len;
    this->fromPin_ = (char *) malloc(len);
  }
  strcpy(this->fromPin_, DEFCASE(fromPin));
}

void 
defiTimingDisable_setMacroFromTo(defiTimingDisable * this,
				 const char *fromPin,
				 const char *toPin)
{
  int     len;

  defiTimingDisable_clear(this);
  this->hasFromTo_ = 1;
  len = strlen(fromPin) + 1;
  if (len > this->fromPinLength_) {
    if (this->fromPin_)
      free(this->fromPin_);
    this->fromPinLength_ = len;
    this->fromPin_ = (char *) malloc(len);
  }
  strcpy(this->fromPin_, DEFCASE(fromPin));
  len = strlen(toPin) + 1;
  if (len > this->toPinLength_) {
    if (this->toPin_)
      free(this->toPin_);
    this->toPinLength_ = len;
    this->toPin_ = (char *) malloc(len);
  }
  strcpy(this->toPin_, DEFCASE(toPin));
}

void 
defiTimingDisable_setMacroThru(defiTimingDisable * this,
			       const char *thru)
{
  int     len;

  defiTimingDisable_clear(this);
  this->hasThru_ = 1;
  len = strlen(thru) + 1;
  if (len > this->fromPinLength_) {
    if (this->fromPin_)
      free(this->fromPin_);
    this->fromPinLength_ = len;
    this->fromPin_ = (char *) malloc(len);
  }
  strcpy(this->fromPin_, DEFCASE(thru));
}

void 
defiTimingDisable_setMacro(defiTimingDisable * this,
			   const char *name)
{
  int     len;

  this->hasMacro_ = 1;
  len = strlen(name) + 1;
  if (len > this->fromInstLength_) {
    if (this->fromInst_)
      free(this->fromInst_);
    this->fromInstLength_ = len;
    this->fromInst_ = (char *) malloc(len);
  }
  strcpy(this->fromInst_, DEFCASE(name));
}

void 
defiTimingDisable_print(const defiTimingDisable * this,
			FILE * f)
{
  if (defiTimingDisable_hasMacroFromTo(this)) {
    fprintf(f, "TimingDisable macro '%s' thru '%s'\n", this->fromInst_, this->fromPin_);
  }
  else if (defiTimingDisable_hasMacroThru(this)) {
    fprintf(f, "TimingDisable macro '%s' from '%s' to '%s'\n", this->fromInst_, this->fromPin_, this->toPin_);
  }
  else if (defiTimingDisable_hasFromTo(this)) {
    fprintf(f, "TimingDisable from '%s' '%s' to '%s' '%s'\n", this->fromInst_, this->fromPin_, this->toInst_, this->toPin_);
  }
  else if (defiTimingDisable_hasThru(this)) {
    fprintf(f, "TimingDisable thru '%s' '%s'\n", this->fromInst_, this->fromPin_);
  }
  else {
    defiError("Bad TimingDisable type");
  }
}

int 
defiTimingDisable_hasReentrantPathsFlag(const defiTimingDisable * this)
{
  return this->hasReentrantPathsFlag_;
}

int 
defiTimingDisable_hasMacroFromTo(const defiTimingDisable * this)
{
  return (this->hasMacro_ && this->hasFromTo_) ? 1 : 0;
}

int 
defiTimingDisable_hasMacroThru(const defiTimingDisable * this)
{
  return (this->hasMacro_ && this->hasThru_) ? 1 : 0;
}

int 
defiTimingDisable_hasThru(const defiTimingDisable * this)
{
  return (this->hasMacro_ == 0 && this->hasThru_) ? 1 : 0;
}

int 
defiTimingDisable_hasFromTo(const defiTimingDisable * this)
{
  return (this->hasMacro_ == 0 && this->hasFromTo_) ? 1 : 0;
}

const char *
defiTimingDisable_toPin(const defiTimingDisable * this)
{
  return this->toPin_;
}

const char *
defiTimingDisable_fromPin(const defiTimingDisable * this)
{
  return this->fromPin_;
}

const char *
defiTimingDisable_toInst(const defiTimingDisable * this)
{
  return this->toInst_;
}

const char *
defiTimingDisable_fromInst(const defiTimingDisable * this)
{
  return this->fromInst_;
}

const char *
defiTimingDisable_macroName(const defiTimingDisable * this)
{
  return this->fromInst_;
}

const char *
defiTimingDisable_thruPin(const defiTimingDisable * this)
{
  return this->fromPin_;
}

const char *
defiTimingDisable_thruInst(const defiTimingDisable * this)
{
  return this->fromInst_;
}
