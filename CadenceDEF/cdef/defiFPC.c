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
#include "defiFPC.h"
#include "defiDebug.h"

/*
 * defiFPC
 */ defiFPC *
defiFPC_CreateUninitialized()
{
  defiFPC *this = (defiFPC *) malloc(sizeof(defiFPC));

  defiFPC_Init(this);
  return (this);
}

void 
defiFPC_Init(defiFPC * this)
{
  this->name_ = 0;
  this->nameLength_ = 0;
  this->namesAllocated_ = 0;
  this->namesUsed_ = 0;
  this->names_ = 0;
  this->rowOrComp_ = 0;
  defiFPC_clear(this);
}

void
defiFPC_Delete(defiFPC * this)
{
  if (this == NULL)
    return;
  defiFPC_Destroy(this);
  free(this);
}

void 
defiFPC_Destroy(defiFPC * this)
{
  defiFPC_clear(this);
  if (this->name_)
    free(this->name_);
  this->name_ = 0;
  this->nameLength_ = 0;
  free((char *) (this->names_));
  free((char *) (this->rowOrComp_));
  this->namesAllocated_ = 0;
}

void 
defiFPC_clear(defiFPC * this)
{
  int     i;

  this->direction_ = 0;
  this->hasAlign_ = 0;
  this->hasMin_ = 0;
  this->hasMax_ = 0;
  this->hasEqual_ = 0;
  this->corner_ = 0;
  for (i = 0;
       i < this->namesUsed_;
       i++) {
    if (this->names_[i])
      free(this->names_[i]);
  }
  this->namesUsed_ = 0;
}

void 
defiFPC_setName(defiFPC * this,
		const char *name,
		const char *direction)
{
  int     len = strlen(name) + 1;

  defiFPC_clear(this);
  if (len > this->nameLength_) {
    if (this->name_)
      free(this->name_);
    this->nameLength_ = len;
    this->name_ = (char *) malloc(len);
  }
  strcpy(this->name_, DEFCASE(name));
  if (*direction == 'H')
    this->direction_ = 'H';
  else if (*direction == 'V')
    this->direction_ = 'V';
  else
    defiError("Bad direction for defFPC::setName");
}

void 
defiFPC_print(const defiFPC * this,
	      FILE * f)
{
  fprintf(f, "FPC '%s'\n", this->name_);
}

const char *
defiFPC_name(const defiFPC * this)
{
  return this->name_;
}

int 
defiFPC_isVertical(const defiFPC * this)
{
  return this->direction_ == 'V' ? 1 : 0;
}

int 
defiFPC_isHorizontal(const defiFPC * this)
{
  return this->direction_ == 'H' ? 1 : 0;
}

int 
defiFPC_hasAlign(const defiFPC * this)
{
  return (int) (this->hasAlign_);
}

int 
defiFPC_hasMax(const defiFPC * this)
{
  return (int) (this->hasMax_);
}

int 
defiFPC_hasMin(const defiFPC * this)
{
  return (int) (this->hasMin_);
}

int 
defiFPC_hasEqual(const defiFPC * this)
{
  return (int) (this->hasEqual_);
}

double 
defiFPC_min(const defiFPC * this)
{
  return this->minMaxEqual_;
}

double 
defiFPC_max(const defiFPC * this)
{
  return this->minMaxEqual_;
}

double 
defiFPC_equal(const defiFPC * this)
{
  return this->minMaxEqual_;
}

int 
defiFPC_numParts(const defiFPC * this)
{
  return this->namesUsed_;
}

void 
defiFPC_setAlign(defiFPC * this)
{
  this->hasAlign_ = 0;
}

void 
defiFPC_setMin(defiFPC * this,
	       double num)
{
  this->minMaxEqual_ = num;
}

void 
defiFPC_setMax(defiFPC * this,
	       double num)
{
  this->minMaxEqual_ = num;
}

void 
defiFPC_setEqual(defiFPC * this,
		 double num)
{
  this->minMaxEqual_ = num;
}

void 
defiFPC_setDoingBottomLeft(defiFPC * this)
{
  this->corner_ = 'B';
}

void 
defiFPC_setDoingTopRight(defiFPC * this)
{
  this->corner_ = 'T';
}

void 
defiFPC_getPart(const defiFPC * this,
		int index,
		int *corner,
		int *typ,
		char **name)
{
  if (index >= 0 && index <= this->namesUsed_) {
    if (corner)
      *corner = (int) ((this->rowOrComp_[index] & 4) ? 'B' : 'T');
    if (typ)
      *typ = (int) ((this->rowOrComp_[index] & 2) ? 'R' : 'C');
    if (name)
      *name = this->names_[index];
  }
}

void 
defiFPC_addRow(defiFPC * this,
	       const char *name)
{
  defiFPC_addItem(this,
		  'R',
		  DEFCASE(name));
}

void 
defiFPC_addComps(defiFPC * this,
		 const char *name)
{
  defiFPC_addItem(this,
		  'C',
		  DEFCASE(name));
}

void 
defiFPC_addItem(defiFPC * this,
		char item,
		const char *name)
{
  int     len = strlen(name) + 1;

  if (this->namesUsed_ >= this->namesAllocated_) {
    char   *newR;

    char  **newN;

    int     i;

    this->namesAllocated_ = this->namesAllocated_ ? this->namesAllocated_ * 2 : 8;
    newN = (char **) malloc(sizeof(char *) * this->namesAllocated_);
    newR = (char *) malloc(sizeof(char) * this->namesAllocated_);
    for (i = 0;
	 i < this->namesUsed_;
	 i++) {
      newN[i] = this->names_[i];
      newR[i] = this->rowOrComp_[i];
    }
    if (this->names_)
      free((char *) (this->names_));
    if (this->rowOrComp_)
      free(this->rowOrComp_);
    this->names_ = newN;
    this->rowOrComp_ = newR;
  }
  this->names_[this->namesUsed_] = (char *) malloc(len);
  strcpy(this->names_[this->namesUsed_], name);
  this->rowOrComp_[this->namesUsed_] = (char) (((this->corner_ == 'B') ? 4 : 0) | (item == 'R' ? 2 : 0));
  this->namesUsed_ += 1;
}
