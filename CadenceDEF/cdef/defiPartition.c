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
#include "defiPartition.h"
#include "defiDebug.h"

/*
 * defiPartition
 */ defiPartition *
defiPartition_CreateUninitialized()
{
  defiPartition *this = (defiPartition *) malloc(sizeof(defiPartition));

  defiPartition_Init(this);
  return (this);
}

void 
defiPartition_Init(defiPartition * this)
{
  this->name_ = 0;
  this->nameLength_ = 0;
  this->pin_ = 0;
  this->pinLength_ = 0;
  this->inst_ = 0;
  this->instLength_ = 0;
  this->pinsAllocated_ = 0;
  this->numPins_ = 0;
  this->pins_ = 0;
  defiPartition_clear(this);
}

void
defiPartition_Delete(defiPartition * this)
{
  if (this == NULL)
    return;
  defiPartition_Destroy(this);
  free(this);
}

void 
defiPartition_Destroy(defiPartition * this)
{
  if (this->name_)
    free(this->name_);
  this->name_ = 0;
  this->nameLength_ = 0;
  if (this->pin_)
    free(this->pin_);
  this->pin_ = 0;
  this->pinLength_ = 0;
  if (this->inst_)
    free(this->inst_);
  this->inst_ = 0;
  this->instLength_ = 0;
  defiPartition_clear(this);
  if (this->pins_)
    free((char *) (this->pins_));
  this->pins_ = 0;
  this->pinsAllocated_ = 0;
}

void 
defiPartition_clear(defiPartition * this)
{
  int     i;

  this->setup_ = ' ';
  this->hold_ = ' ';
  this->direction_ = ' ';
  this->type_ = ' ';
  if (this->name_)
    *(this->name_) = '\0';
  if (this->pin_)
    *(this->pin_) = '\0';
  if (this->inst_)
    *(this->inst_) = '\0';
  this->hasMin_ = 0;
  this->hasMax_ = 0;
  if (this->numPins_) {
    for (i = 0;
	 i < this->numPins_;
	 i++) {
      free(this->pins_[i]);
      this->pins_[i] = 0;
    }
    this->numPins_ = 0;
  }
  this->hasRiseMin_ = 0;
  this->hasFallMin_ = 0;
  this->hasRiseMax_ = 0;
  this->hasFallMax_ = 0;
  this->hasRiseMinRange_ = 0;
  this->hasFallMinRange_ = 0;
  this->hasRiseMaxRange_ = 0;
  this->hasFallMaxRange_ = 0;
}

void 
defiPartition_setName(defiPartition * this,
		      const char *name)
{
  int     len = strlen(name) + 1;

  defiPartition_clear(this);
  if (len > this->nameLength_) {
    if (this->name_)
      free(this->name_);
    this->nameLength_ = len;
    this->name_ = (char *) malloc(len);
  }
  strcpy(this->name_, DEFCASE(name));
}

void 
defiPartition_print(const defiPartition * this,
		    FILE * f)
{
  int     i;

  fprintf(f, "Partition '%s' %c\n", defiPartition_name(this), defiPartition_direction(this));
  fprintf(f, " inst %s pin %s type %s\n", defiPartition_instName(this), defiPartition_pinName(this), defiPartition_itemType(this));
  for (i = 0;
       i < defiPartition_numPins(this);
       i++)
    fprintf(f, " %s\n", defiPartition_pin(this,
					  i));
  if (defiPartition_isSetupRise(this))
    fprintf(f, " SETUP RISE\n");
  if (defiPartition_isSetupFall(this))
    fprintf(f, " SETUP FALL\n");
  if (defiPartition_isHoldRise(this))
    fprintf(f, " HOLD RISE\n");
  if (defiPartition_isHoldFall(this))
    fprintf(f, " HOLD FALL\n");
  if (defiPartition_hasMin(this))
    fprintf(f, " MIN %g\n", defiPartition_min(this));
  if (defiPartition_hasMax(this))
    fprintf(f, " MAX %g\n", defiPartition_max(this));
  if (defiPartition_hasRiseMin(this))
    fprintf(f, " RISE MIN %g\n", defiPartition_riseMin(this));
  if (defiPartition_hasFallMin(this))
    fprintf(f, " FALL MIN %g\n", defiPartition_fallMin(this));
  if (defiPartition_hasRiseMax(this))
    fprintf(f, " RISE MAX %g\n", defiPartition_riseMax(this));
  if (defiPartition_hasFallMax(this))
    fprintf(f, " FALL MAX %g\n", defiPartition_fallMax(this));
  if (defiPartition_hasFallMinRange(this))
    fprintf(f, " FALL MIN %g %g\n", defiPartition_fallMinLeft(this), defiPartition_fallMinRight(this));
  if (defiPartition_hasRiseMinRange(this))
    fprintf(f, " RISE MIN %g %g\n", defiPartition_riseMinLeft(this), defiPartition_riseMinRight(this));
  if (defiPartition_hasFallMaxRange(this))
    fprintf(f, " FALL MAX %g %g\n", defiPartition_fallMaxLeft(this), defiPartition_fallMaxRight(this));
  if (defiPartition_hasRiseMaxRange(this))
    fprintf(f, " RISE MAX %g %g\n", defiPartition_riseMaxLeft(this), defiPartition_riseMaxRight(this));
}

const char *
defiPartition_name(const defiPartition * this)
{
  return this->name_;
}

void 
defiPartition_setFromIOPin(defiPartition * this,
			   const char *pin)
{
  defiPartition_set(this,
		    'F',
		    'I',
		    "",
		    pin);
}

char 
defiPartition_direction(const defiPartition * this)
{
  return this->direction_;
}

const char *
defiPartition_instName(const defiPartition * this)
{
  return this->inst_;
}

const char *
defiPartition_pinName(const defiPartition * this)
{
  return this->pin_;
}

static char *
ad(const char *in)
{
  return (char *) in;
}

const char *
defiPartition_itemType(const defiPartition * this)
{
  char   *c;

  if (this->type_ == 'L')
    c = ad("CLOCK");
  else if (this->type_ == 'I')
    c = ad("IO");
  else if (this->type_ == 'C')
    c = ad("COMP");
  else
    c = ad("BOGUS");
  return c;
}

const char *
defiPartition_pin(const defiPartition * this,
		  int index)
{
  return this->pins_[index];
}

int 
defiPartition_numPins(const defiPartition * this)
{
  return this->numPins_;
}

int 
defiPartition_isSetupRise(const defiPartition * this)
{
  return this->setup_ == 'R' ? 1 : 0;
}

int 
defiPartition_isSetupFall(const defiPartition * this)
{
  return this->setup_ == 'F' ? 1 : 0;
}

int 
defiPartition_isHoldRise(const defiPartition * this)
{
  return this->hold_ == 'R' ? 1 : 0;
}

int 
defiPartition_isHoldFall(const defiPartition * this)
{
  return this->hold_ == 'F' ? 1 : 0;
}

void 
defiPartition_addTurnOff(defiPartition * this,
			 const char *setup,
			 const char *hold)
{
  if (*setup == ' ') {
    this->setup_ = *setup;
  }
  else if (*setup == 'R') {
    this->setup_ = *setup;
  }
  else if (*setup == 'F') {
    this->setup_ = *setup;
  }
  else {
    defiError("Illegal setup for parition");
  }
  if (*hold == ' ') {
    this->hold_ = *hold;
  }
  else if (*hold == 'R') {
    this->hold_ = *hold;
  }
  else if (*hold == 'F') {
    this->hold_ = *hold;
  }
  else {
    defiError("Illegal hold for parition");
  }
}

void 
defiPartition_setFromClockPin(defiPartition * this,
			      const char *inst,
			      const char *pin)
{
  defiPartition_set(this,
		    'F',
		    'L',
		    inst,
		    pin);
}

void 
defiPartition_setToClockPin(defiPartition * this,
			    const char *inst,
			    const char *pin)
{
  defiPartition_set(this,
		    'T',
		    'L',
		    inst,
		    pin);
}

void 
defiPartition_set(defiPartition * this,
		  char dir,
		  char typ,
		  const char *inst,
		  const char *pin)
{
  int     len = strlen(pin) + 1;

  this->direction_ = dir;
  this->type_ = typ;
  if (this->pinLength_ <= len) {
    if (this->pin_)
      free(this->pin_);
    this->pin_ = (char *) malloc(len);
    this->pinLength_ = len;
  }
  strcpy(this->pin_, DEFCASE(pin));
  len = strlen(inst) + 1;
  if (this->instLength_ <= len) {
    if (this->inst_)
      free(this->inst_);
    this->inst_ = (char *) malloc(len);
    this->instLength_ = len;
  }
  strcpy(this->inst_, DEFCASE(inst));
}

void 
defiPartition_setMin(defiPartition * this,
		     double min,
		     double max)
{
  this->min_ = min;
  this->max_ = max;
  this->hasMin_ = 1;
}

void 
defiPartition_setFromCompPin(defiPartition * this,
			     const char *inst,
			     const char *pin)
{
  defiPartition_set(this,
		    'F',
		    'C',
		    inst,
		    pin);
}

void 
defiPartition_setMax(defiPartition * this,
		     double min,
		     double max)
{
  this->min_ = min;
  this->max_ = max;
  this->hasMax_ = 1;
}

void 
defiPartition_setToIOPin(defiPartition * this,
			 const char *pin)
{
  defiPartition_set(this,
		    'T',
		    'I',
		    "",
		    pin);
}

void 
defiPartition_setToCompPin(defiPartition * this,
			   const char *inst,
			   const char *pin)
{
  defiPartition_set(this,
		    'T',
		    'C',
		    inst,
		    pin);
}

void 
defiPartition_addPin(defiPartition * this,
		     const char *name)
{
  int     len;

  int     i;

  char  **newp;

  if (this->numPins_ >= this->pinsAllocated_) {
    this->pinsAllocated_ = this->pinsAllocated_ ? 2 * this->pinsAllocated_ : 8;
    newp = (char **) malloc(sizeof(char *) * this->pinsAllocated_);
    for (i = 0;
	 i < this->numPins_;
	 i++)
      newp[i] = this->pins_[i];
    if (this->pins_)
      free((char *) (this->pins_));
    this->pins_ = newp;
  }
  len = strlen(name) + 1;
  this->pins_[this->numPins_] = (char *) malloc(len);
  strcpy(this->pins_[this->numPins_], DEFCASE(name));
  this->numPins_ += 1;
}

int 
defiPartition_hasMin(const defiPartition * this)
{
  return (int) (this->hasMin_);
}

int 
defiPartition_hasMax(const defiPartition * this)
{
  return (int) (this->hasMax_);
}

double 
defiPartition_min(const defiPartition * this)
{
  return (this->min_);
}

double 
defiPartition_max(const defiPartition * this)
{
  return (this->max_);
}

int 
defiPartition_hasRiseMin(const defiPartition * this)
{
  return (int) (this->hasRiseMin_);
}

int 
defiPartition_hasFallMin(const defiPartition * this)
{
  return (int) (this->hasFallMin_);
}

int 
defiPartition_hasRiseMax(const defiPartition * this)
{
  return (int) (this->hasRiseMax_);
}

int 
defiPartition_hasFallMax(const defiPartition * this)
{
  return (int) (this->hasFallMax_);
}

int 
defiPartition_hasRiseMinRange(const defiPartition * this)
{
  return (int) (this->hasRiseMinRange_);
}

int 
defiPartition_hasFallMinRange(const defiPartition * this)
{
  return (int) (this->hasFallMinRange_);
}

int 
defiPartition_hasRiseMaxRange(const defiPartition * this)
{
  return (int) (this->hasRiseMaxRange_);
}

int 
defiPartition_hasFallMaxRange(const defiPartition * this)
{
  return (int) (this->hasFallMaxRange_);
}

double 
defiPartition_riseMin(const defiPartition * this)
{
  return this->riseMin_;
}

double 
defiPartition_fallMin(const defiPartition * this)
{
  return this->fallMin_;
}

double 
defiPartition_riseMax(const defiPartition * this)
{
  return this->riseMax_;
}

double 
defiPartition_fallMax(const defiPartition * this)
{
  return this->fallMax_;
}

double 
defiPartition_riseMinLeft(const defiPartition * this)
{
  return this->riseMinLeft_;
}

double 
defiPartition_fallMinLeft(const defiPartition * this)
{
  return this->fallMinLeft_;
}

double 
defiPartition_riseMaxLeft(const defiPartition * this)
{
  return this->riseMaxLeft_;
}

double 
defiPartition_fallMaxLeft(const defiPartition * this)
{
  return this->fallMaxLeft_;
}

double 
defiPartition_riseMinRight(const defiPartition * this)
{
  return this->riseMinRight_;
}

double 
defiPartition_fallMinRight(const defiPartition * this)
{
  return this->fallMinRight_;
}

double 
defiPartition_riseMaxRight(const defiPartition * this)
{
  return this->riseMaxRight_;
}

double 
defiPartition_fallMaxRight(const defiPartition * this)
{
  return this->fallMaxRight_;
}

void 
defiPartition_addRiseMin(defiPartition * this,
			 double d)
{
  this->hasRiseMin_ = 1;
  this->riseMin_ = d;
}

void 
defiPartition_addRiseMax(defiPartition * this,
			 double d)
{
  this->hasRiseMax_ = 1;
  this->riseMax_ = d;
}

void 
defiPartition_addFallMin(defiPartition * this,
			 double d)
{
  this->hasFallMin_ = 1;
  this->fallMin_ = d;
}

void 
defiPartition_addFallMax(defiPartition * this,
			 double d)
{
  this->hasFallMax_ = 1;
  this->fallMax_ = d;
}

void 
defiPartition_addRiseMinRange(defiPartition * this,
			      double l,
			      double h)
{
  this->hasRiseMinRange_ = 1;
  this->riseMinLeft_ = l;
  this->riseMinRight_ = h;
}

void 
defiPartition_addRiseMaxRange(defiPartition * this,
			      double l,
			      double h)
{
  this->hasRiseMaxRange_ = 1;
  this->riseMaxLeft_ = l;
  this->riseMaxRight_ = h;
}

void 
defiPartition_addFallMinRange(defiPartition * this,
			      double l,
			      double h)
{
  this->hasFallMinRange_ = 1;
  this->fallMinLeft_ = l;
  this->fallMinRight_ = h;
}

void 
defiPartition_addFallMaxRange(defiPartition * this,
			      double l,
			      double h)
{
  this->hasFallMaxRange_ = 1;
  this->fallMaxLeft_ = l;
  this->fallMaxRight_ = h;
}
