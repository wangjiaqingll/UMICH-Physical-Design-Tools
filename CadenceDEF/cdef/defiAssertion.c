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
#include "defiAssertion.h"
#include "defiDebug.h"
struct defiAssertPath {
  char   *fromPin_;
  char   *toPin_;
  char   *fromInst_;
  char   *toInst_;
};

defiAssertion *
defiAssertion_CreateUninitialized()
{
  defiAssertion *this = (defiAssertion *) malloc(sizeof(defiAssertion));

  defiAssertion_Init(this);
  return (this);
}

void
defiAssertion_Delete(defiAssertion * this)
{
  if (this == NULL)
    return;
  defiAssertion_Destroy(this);
  free(this);
}

void 
defiAssertion_Init(defiAssertion * this)
{
  this->netName_ = (char *) malloc(32);
  this->netNameLength_ = 32;
  defiAssertion_clear(this);
  this->numItemsAllocated_ = 16;
  this->items_ = (int **) malloc(sizeof(char *) * 16);
  this->itemTypes_ = (char *) malloc(16);
}

void 
defiAssertion_Destroy(defiAssertion * this)
{
  free(this->netName_);
  free((char *) (this->itemTypes_));
  free((char *) (this->items_));
}

void 
defiAssertion_clear(defiAssertion * this)
{
  int     i;

  struct defiAssertPath *s;

  if (this->netName_)
    *(this->netName_) = '\0';
  this->isSum_ = 0;
  this->isDiff_ = 0;
  this->isAssertion_ = 0;
  this->hasRiseMin_ = 0;
  this->hasRiseMax_ = 0;
  this->hasFallMin_ = 0;
  this->hasFallMax_ = 0;
  this->isDelay_ = 0;
  this->isWiredlogic_ = 0;
  for (i = 0;
       i < this->numItems_;
       i++) {
    if (this->itemTypes_[i] == 'p') {
      s = (struct defiAssertPath *) (this->items_[i]);
      free(s->fromPin_);
      free(s->toPin_);
      free(s->fromInst_);
      free(s->toInst_);
      free((char *) s);
    }
    else if (this->itemTypes_[i] == 'n') {
      free((char *) (this->items_[i]));
    }
    else {
      defiError("Internal error in Assertion #1");
    }
    this->itemTypes_[i] = 'B';
    this->items_[i] = 0;
  }
  this->numItems_ = 0;
}

void 
defiAssertion_setConstraintMode(defiAssertion * this)
{
  this->isAssertion_ = 0;
}

void 
defiAssertion_setAssertionMode(defiAssertion * this)
{
  this->isAssertion_ = 1;
}

void 
defiAssertion_setWiredlogicMode(defiAssertion * this)
{
  this->isWiredlogic_ = 1;
}

void 
defiAssertion_setWiredlogic(defiAssertion * this,
			    const char *name,
			    double dist)
{
  int     len = strlen(name) + 1;

  if (this->isDelay_)
    defiError("mixing WIREDLOGIC and DELAY in constraint/assertion");
  this->isWiredlogic_ = 1;
  if (this->netNameLength_ < len) {
    free(this->netName_);
    this->netName_ = (char *) malloc(len);
    this->netNameLength_ = len;
  }
  strcpy(this->netName_, DEFCASE(name));
  this->fallMax_ = dist;
}

void 
defiAssertion_setDelay(defiAssertion * this)
{
  if (this->isWiredlogic_)
    defiError("mixing WIREDLOGIC and DELAY in constraint/assertion");
  this->isDelay_ = 1;
}

void 
defiAssertion_setSum(defiAssertion * this)
{
  if (this->isDiff_)
    defiError("mixing SUM and DIFF in constraint/assertion");
  this->isSum_ = 1;
}

void 
defiAssertion_unsetSum(defiAssertion * this)
{
  this->isSum_ = 0;
}

void 
defiAssertion_setDiff(defiAssertion * this)
{
  if (this->isSum_)
    defiError("mixing SUM and DIFF in constraint/assertion");
  this->isDiff_ = 1;
}

const char *
defiAssertion_netName(const defiAssertion * this)
{
  return this->netName_;
}

void 
defiAssertion_setNetName(defiAssertion * this,
			 const char *name)
{
  int     len = strlen(name) + 1;

  defiAssertion_clear(this);
  if (len > this->netNameLength_) {
    free(this->netName_);
    this->netName_ = (char *) malloc(len);
    this->netNameLength_ = len;
  }
  strcpy(this->netName_, DEFCASE(name));
}

int 
defiAssertion_isDelay(const defiAssertion * this)
{
  return this->isDelay_ ? 1 : 0;
}

int 
defiAssertion_isAssertion(const defiAssertion * this)
{
  return this->isAssertion_ ? 1 : 0;
}

int 
defiAssertion_isConstraint(const defiAssertion * this)
{
  return this->isAssertion_ ? 0 : 1;
}

int 
defiAssertion_isSum(const defiAssertion * this)
{
  return this->isSum_;
}

int 
defiAssertion_isDiff(const defiAssertion * this)
{
  return this->isDiff_;
}

int 
defiAssertion_isWiredlogic(const defiAssertion * this)
{
  return this->isWiredlogic_;
}

int 
defiAssertion_hasRiseMin(const defiAssertion * this)
{
  return this->hasRiseMin_;
}

int 
defiAssertion_hasRiseMax(const defiAssertion * this)
{
  return this->hasRiseMax_;
}

int 
defiAssertion_hasFallMin(const defiAssertion * this)
{
  return this->hasFallMin_;
}

int 
defiAssertion_hasFallMax(const defiAssertion * this)
{
  return this->hasFallMax_;
}

double 
defiAssertion_distance(const defiAssertion * this)
{
  return this->fallMax_;
}

double 
defiAssertion_riseMin(const defiAssertion * this)
{
  return this->riseMin_;
}

double 
defiAssertion_riseMax(const defiAssertion * this)
{
  return this->riseMax_;
}

double 
defiAssertion_fallMin(const defiAssertion * this)
{
  return this->fallMin_;
}

double 
defiAssertion_fallMax(const defiAssertion * this)
{
  return this->fallMax_;
}

void 
defiAssertion_setRiseMin(defiAssertion * this,
			 double d)
{
  this->riseMin_ = d;
  this->hasRiseMin_ = 1;
}

void 
defiAssertion_setRiseMax(defiAssertion * this,
			 double d)
{
  this->riseMax_ = d;
  this->hasRiseMax_ = 1;
}

void 
defiAssertion_setFallMin(defiAssertion * this,
			 double d)
{
  this->fallMin_ = d;
  this->hasFallMin_ = 1;
}

void 
defiAssertion_setFallMax(defiAssertion * this,
			 double d)
{
  this->fallMax_ = d;
  this->hasFallMax_ = 1;
}

int 
defiAssertion_numItems(const defiAssertion * this)
{
  return this->numItems_;
}

int 
defiAssertion_isPath(const defiAssertion * this,
		     int index)
{
  if (index >= 0 && index < this->numItems_) {
    return (this->itemTypes_[index] == 'p') ? 1 : 0;
  }
  return 0;
}

int 
defiAssertion_isNet(const defiAssertion * this,
		    int index)
{
  if (index >= 0 && index < this->numItems_) {
    return (this->itemTypes_[index] == 'n') ? 1 : 0;
  }
  return 0;
}

void 
defiAssertion_path(const defiAssertion * this,
		   int index,
		   char **fromInst,
		   char **fromPin,
		   char **toInst,
		   char **toPin)
{
  struct defiAssertPath *ap;

  if (index >= 0 && index < this->numItems_ && this->itemTypes_[index] == 'p') {
    ap = (struct defiAssertPath *) (this->items_[index]);
    if (fromInst)
      *fromInst = ap->fromInst_;
    if (fromPin)
      *fromPin = ap->fromPin_;
    if (toInst)
      *toInst = ap->toInst_;
    if (toPin)
      *toPin = ap->toPin_;
  }
}

void 
defiAssertion_net(const defiAssertion * this,
		  int index,
		  char **netName)
{
  if (index >= 0 && index < this->numItems_ && this->itemTypes_[index] == 'n') {
    if (netName)
      *netName = (char *) (this->items_[index]);
  }
}

void 
defiAssertion_bumpItems(defiAssertion * this)
{
  int     i;

  char   *newTypes;

  int   **newItems;

  (this->numItemsAllocated_) *= 2;
  newTypes = (char *) malloc(this->numItemsAllocated_ * sizeof(char));
  newItems = (int **) malloc(this->numItemsAllocated_ * sizeof(int *));
  for (i = 0;
       i < this->numItems_;
       i++) {
    newItems[i] = this->items_[i];
    newTypes[i] = this->itemTypes_[i];
  }
  free((char *) this->items_);
  free((char *) this->itemTypes_);
  this->items_ = newItems;
  this->itemTypes_ = newTypes;
}

void 
defiAssertion_addNet(defiAssertion * this,
		     const char *name)
{
  int     i;

  char   *s;

  this->isWiredlogic_ = 0;
  i = strlen(name) + 1;
  s = (char *) malloc(i);
  strcpy(s, DEFCASE(name));
  if (this->numItems_ >= this->numItemsAllocated_)
    defiAssertion_bumpItems(this);
  i = this->numItems_;
  this->items_[i] = (int *) s;
  this->itemTypes_[i] = 'n';
  this->numItems_ = i + 1;
}

void 
defiAssertion_addPath(defiAssertion * this,
		      const char *fromInst,
		      const char *fromPin,
		      const char *toInst,
		      const char *toPin)
{
  int     i;

  struct defiAssertPath *s;

  this->isWiredlogic_ = 0;
  s = (struct defiAssertPath *) malloc(sizeof(struct defiAssertPath));
  i = strlen(fromInst) + 1;
  s->fromInst_ = (char *) malloc(i);
  strcpy(s->fromInst_, DEFCASE(fromInst));
  i = strlen(toInst) + 1;
  s->toInst_ = (char *) malloc(i);
  strcpy(s->toInst_, DEFCASE(toInst));
  i = strlen(fromPin) + 1;
  s->fromPin_ = (char *) malloc(i);
  strcpy(s->fromPin_, DEFCASE(fromPin));
  i = strlen(toPin) + 1;
  s->toPin_ = (char *) malloc(i);
  strcpy(s->toPin_, DEFCASE(toPin));
  if (this->numItems_ >= this->numItemsAllocated_)
    defiAssertion_bumpItems(this);
  i = this->numItems_;
  this->items_[i] = (int *) s;
  this->itemTypes_[i] = 'p';
  this->numItems_ = i + 1;
}

void 
defiAssertion_print(const defiAssertion * this,
		    FILE * f)
{
  fprintf(f, "Assertion %s\n", defiAssertion_netName(this));
}
