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
#include "defiIOTiming.h"
#include "defiDebug.h"

/*
 * defiIOTiming
 */ defiIOTiming *
defiIOTiming_CreateUninitialized()
{
  defiIOTiming *this = (defiIOTiming *) malloc(sizeof(defiIOTiming));

  defiIOTiming_Init(this);
  return (this);
}

void 
defiIOTiming_Init(defiIOTiming * this)
{
  this->inst_ = 0;
  this->instLength_ = 0;
  this->pin_ = 0;
  this->pinLength_ = 0;
  this->from_ = 0;
  this->fromLength_ = 0;
  this->to_ = 0;
  this->toLength_ = 0;
  this->driveCell_ = 0;
  this->driveCellLength_ = 0;
  this->hasVariableRise_ = 0;
  this->hasVariableFall_ = 0;
  this->hasSlewRise_ = 0;
  this->hasSlewFall_ = 0;
  this->hasCapacitance_ = 0;
  this->hasDriveCell_ = 0;
  this->hasFrom_ = 0;
  if (this->pin_)
    free(this->pin_);
  this->pin_ = 0;
  this->pinLength_ = 0;
  this->hasTo_ = 0;
  this->hasParallel_ = 0;
  this->variableFallMin_ = 0.0;
  this->variableRiseMin_ = 0.0;
  this->variableFallMax_ = 0.0;
  this->variableRiseMax_ = 0.0;
  this->slewFallMin_ = 0.0;
  this->slewRiseMin_ = 0.0;
  this->slewFallMax_ = 0.0;
  this->slewRiseMax_ = 0.0;
  this->capacitance_ = 0.0;
  this->parallel_ = 0.0;
}

void
defiIOTiming_Delete(defiIOTiming * this)
{
  if (this == NULL)
    return;
  defiIOTiming_Destroy(this);
  free(this);
}

void 
defiIOTiming_Destroy(defiIOTiming * this)
{
  if (this->inst_)
    free(this->inst_);
  this->inst_ = 0;
  this->instLength_ = 0;
  if (this->pin_)
    free(this->pin_);
  this->pin_ = 0;
  this->pinLength_ = 0;
  if (this->from_)
    free(this->from_);
  this->from_ = 0;
  this->fromLength_ = 0;
  if (this->to_)
    free(this->to_);
  this->to_ = 0;
  this->toLength_ = 0;
  if (this->driveCell_)
    free(this->driveCell_);
  this->driveCell_ = 0;
  this->driveCellLength_ = 0;
  defiIOTiming_clear(this);
}

void 
defiIOTiming_clear(defiIOTiming * this)
{
  this->hasVariableRise_ = 0;
  this->hasVariableFall_ = 0;
  this->hasSlewRise_ = 0;
  this->hasSlewFall_ = 0;
  this->hasCapacitance_ = 0;
  this->hasDriveCell_ = 0;
  this->hasFrom_ = 0;
  this->hasTo_ = 0;
  this->hasParallel_ = 0;
  this->variableFallMin_ = 0.0;
  this->variableRiseMin_ = 0.0;
  this->variableFallMax_ = 0.0;
  this->variableRiseMax_ = 0.0;
  this->slewFallMin_ = 0.0;
  this->slewRiseMin_ = 0.0;
  this->slewFallMax_ = 0.0;
  this->slewRiseMax_ = 0.0;
  this->capacitance_ = 0.0;
  this->parallel_ = 0.0;
}

void 
defiIOTiming_setName(defiIOTiming * this,
		     const char *inst,
		     const char *pin)
{
  int     len;

  defiIOTiming_clear(this);
  len = strlen(inst) + 1;
  if (len > this->instLength_) {
    if (this->inst_)
      free(this->inst_);
    this->instLength_ = len;
    this->inst_ = (char *) malloc(len);
  }
  strcpy(this->inst_, DEFCASE(inst));
  len = strlen(pin) + 1;
  if (len > this->pinLength_) {
    if (this->pin_)
      free(this->pin_);
    this->pinLength_ = len;
    this->pin_ = (char *) malloc(len);
  }
  strcpy(this->pin_, DEFCASE(pin));
}

void 
defiIOTiming_print(const defiIOTiming * this,
		   FILE * f)
{
  fprintf(f, "IOTiming '%s' '%s'\n", this->inst_, this->pin_);
  if (defiIOTiming_hasSlewRise(this))
    fprintf(f, " Slew rise %lf5.2 %lf5.2\n", defiIOTiming_slewRiseMin(this), defiIOTiming_slewRiseMax(this));
  if (defiIOTiming_hasSlewFall(this))
    fprintf(f, " Slew fall %lf5.2 %lf5.2\n", defiIOTiming_slewFallMin(this), defiIOTiming_slewFallMax(this));
  if (defiIOTiming_hasVariableRise(this))
    fprintf(f, " variable rise %lf5.2 %lf5.2\n", defiIOTiming_variableRiseMin(this), defiIOTiming_variableRiseMax(this));
  if (defiIOTiming_hasVariableFall(this))
    fprintf(f, " variable fall %lf5.2 %lf5.2\n", defiIOTiming_variableFallMin(this), defiIOTiming_variableFallMax(this));
  if (defiIOTiming_hasCapacitance(this))
    fprintf(f, " capacitance %lf5.2\n", defiIOTiming_capacitance(this));
  if (defiIOTiming_hasDriveCell(this))
    fprintf(f, " drive cell '%s'\n", defiIOTiming_driveCell(this));
  if (defiIOTiming_hasFrom(this))
    fprintf(f, " from pin '%s'\n", defiIOTiming_from(this));
  if (defiIOTiming_hasTo(this))
    fprintf(f, " to pin '%s'\n", defiIOTiming_to(this));
  if (defiIOTiming_hasParallel(this))
    fprintf(f, " parallel %lf5.2\n", defiIOTiming_parallel(this));
}

void 
defiIOTiming_setVariable(defiIOTiming * this,
			 const char *riseFall,
			 double min,
			 double max)
{
  if (*riseFall == 'R') {
    this->hasVariableRise_ = 1;
    this->variableRiseMin_ = min;
    this->variableRiseMax_ = max;
  }
  else if (*riseFall == 'F') {
    this->hasVariableFall_ = 1;
    this->variableFallMin_ = min;
    this->variableFallMax_ = max;
  }
  else {
    defiError("Internal error Bad varaible rate rise/fall status");
  }
}

void 
defiIOTiming_setSlewRate(defiIOTiming * this,
			 const char *riseFall,
			 double min,
			 double max)
{
  if (*riseFall == 'R') {
    this->hasSlewRise_ = 1;
    this->slewRiseMin_ = min;
    this->slewRiseMax_ = max;
  }
  else if (*riseFall == 'F') {
    this->hasSlewFall_ = 1;
    this->slewFallMin_ = min;
    this->slewFallMax_ = max;
  }
  else {
    defiError("Internal error Bad slew rate rise/fall status");
  }
}

void 
defiIOTiming_setCapacitance(defiIOTiming * this,
			    double num)
{
  this->hasCapacitance_ = 1;
  this->capacitance_ = num;
}

void 
defiIOTiming_setDriveCell(defiIOTiming * this,
			  const char *name)
{
  int     len = strlen(name) + 1;

  if (this->driveCellLength_ < len) {
    if (this->driveCell_)
      free(this->driveCell_);
    this->driveCell_ = (char *) malloc(len);
    this->driveCellLength_ = len;
  }
  strcpy(this->driveCell_, DEFCASE(name));
  this->hasDriveCell_ = 1;
}

void 
defiIOTiming_setFrom(defiIOTiming * this,
		     const char *name)
{
  int     len = strlen(name) + 1;

  if (this->fromLength_ < len) {
    if (this->from_)
      free(this->from_);
    this->from_ = (char *) malloc(len);
    this->fromLength_ = len;
  }
  strcpy(this->from_, DEFCASE(name));
  this->hasFrom_ = 1;
}

void 
defiIOTiming_setTo(defiIOTiming * this,
		   const char *name)
{
  int     len = strlen(name) + 1;

  if (this->toLength_ < len) {
    if (this->to_)
      free(this->to_);
    this->to_ = (char *) malloc(len);
    this->toLength_ = len;
  }
  strcpy(this->to_, DEFCASE(name));
  this->hasTo_ = 1;
}

void 
defiIOTiming_setParallel(defiIOTiming * this,
			 double num)
{
  this->hasParallel_ = 1;
  this->parallel_ = num;
}

int 
defiIOTiming_hasVariableRise(const defiIOTiming * this)
{
  return this->hasVariableRise_;
}

int 
defiIOTiming_hasVariableFall(const defiIOTiming * this)
{
  return this->hasVariableFall_;
}

int 
defiIOTiming_hasSlewRise(const defiIOTiming * this)
{
  return this->hasSlewRise_;
}

int 
defiIOTiming_hasSlewFall(const defiIOTiming * this)
{
  return this->hasSlewFall_;
}

int 
defiIOTiming_hasCapacitance(const defiIOTiming * this)
{
  return this->hasCapacitance_;
}

int 
defiIOTiming_hasDriveCell(const defiIOTiming * this)
{
  return this->hasDriveCell_;
}

int 
defiIOTiming_hasFrom(const defiIOTiming * this)
{
  return this->hasFrom_;
}

int 
defiIOTiming_hasTo(const defiIOTiming * this)
{
  return this->hasTo_;
}

int 
defiIOTiming_hasParallel(const defiIOTiming * this)
{
  return this->hasParallel_;
}

const char *
defiIOTiming_inst(const defiIOTiming * this)
{
  return this->inst_;
}

const char *
defiIOTiming_pin(const defiIOTiming * this)
{
  return this->pin_;
}

double 
defiIOTiming_variableFallMin(const defiIOTiming * this)
{
  return this->variableFallMin_;
}

double 
defiIOTiming_variableRiseMin(const defiIOTiming * this)
{
  return this->variableRiseMin_;
}

double 
defiIOTiming_variableFallMax(const defiIOTiming * this)
{
  return this->variableFallMax_;
}

double 
defiIOTiming_variableRiseMax(const defiIOTiming * this)
{
  return this->variableRiseMax_;
}

double 
defiIOTiming_slewFallMin(const defiIOTiming * this)
{
  return this->slewFallMin_;
}

double 
defiIOTiming_slewRiseMin(const defiIOTiming * this)
{
  return this->slewRiseMin_;
}

double 
defiIOTiming_slewFallMax(const defiIOTiming * this)
{
  return this->slewFallMax_;
}

double 
defiIOTiming_slewRiseMax(const defiIOTiming * this)
{
  return this->slewRiseMax_;
}

double 
defiIOTiming_capacitance(const defiIOTiming * this)
{
  return this->capacitance_;
}

const char *
defiIOTiming_driveCell(const defiIOTiming * this)
{
  return this->driveCell_;
}

const char *
defiIOTiming_from(const defiIOTiming * this)
{
  return this->from_;
}

const char *
defiIOTiming_to(const defiIOTiming * this)
{
  return this->to_;
}

double 
defiIOTiming_parallel(const defiIOTiming * this)
{
  return this->parallel_;
}
