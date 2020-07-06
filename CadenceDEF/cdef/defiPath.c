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
#include "defiPath.h"
#include "defiDebug.h"
defiPath *
defiPath_CreateUninitialized()
{
  defiPath *this = (defiPath *) malloc(sizeof(defiPath));

  defiPath_Init(this);
  return (this);
}

void
defiPath_Delete(defiPath * this)
{
  if (this == NULL)
    return;
  defiPath_Destroy(this);
  free(this);
}

void 
defiPath_Init(defiPath * this)
{
  this->numUsed_ = 0;
  this->numAllocated_ = 0;
  this->keys_ = 0;
  this->data_ = 0;
  defiPath_bumpSize(this,
		    16);
}

void 
defiPath_clear(defiPath * this)
{
  int     i;

  for (i = 0;
       i < this->numUsed_;
       i++) {
    if (this->keys_[i] == 'L') {
      if (this->data_[i])
	free((char *) (this->data_[i]));
    }
    else if (this->keys_[i] == 'R') {
      if (this->data_[i])
	free((char *) (this->data_[i]));
    }
    else if (this->keys_[i] == 'V') {
      if (this->data_[i])
	free((char *) (this->data_[i]));
    }
    else if (this->keys_[i] == 'W') {
      if (this->data_[i])
	free((char *) (this->data_[i]));
    }
    else if (this->keys_[i] == 'P') {
      if (this->data_[i])
	free((char *) (this->data_[i]));
    }
    else if (this->keys_[i] == 'F') {
      if (this->data_[i])
	free((char *) (this->data_[i]));
    }
    else if (this->keys_[i] == 'S') {
      if (this->data_[i])
	free((char *) (this->data_[i]));
    }
  }
  this->numUsed_ = 0;
}

void 
defiPath_Destroy(defiPath * this)
{
  defiPath_clear(this);
  free((char *) (this->keys_));
  free((char *) (this->data_));
}

void 
defiPath_reverseOrder(defiPath * this)
{
  int     one = 0;

  int     two = this->numUsed_ - 1;

  int     t;

  void   *tptr;

  while (one < two) {
    t = this->keys_[one];
    this->keys_[one] = this->keys_[two];
    this->keys_[two] = t;
    tptr = this->data_[one];
    this->data_[one] = this->data_[two];
    this->data_[two] = tptr;
    one++;
    two--;
  }
}

void 
defiPath_initTraverse(defiPath * this)
{
  this->pointer_ = -1;
}

void 
defiPath_initTraverseBackwards(defiPath * this)
{
  this->pointer_ = this->numUsed_;
}

int 
defiPath_next(defiPath * this)
{
  (this->pointer_)++;
  if (this->pointer_ >= 0 && this->pointer_ < this->numUsed_) {
    if (this->keys_[this->pointer_] == 'L') {
      return DEFIPATH_LAYER;
    }
    else if (this->keys_[this->pointer_] == 'V') {
      return DEFIPATH_VIA;
    }
    else if (this->keys_[this->pointer_] == 'W') {
      return DEFIPATH_WIDTH;
    }
    else if (this->keys_[this->pointer_] == 'P') {
      return DEFIPATH_POINT;
    }
    else if (this->keys_[this->pointer_] == 'F') {
      return DEFIPATH_FLUSHPOINT;
    }
    else if (this->keys_[this->pointer_] == 'T') {
      return DEFIPATH_TAPER;
    }
    else if (this->keys_[this->pointer_] == 'R') {
      return DEFIPATH_TAPERRULE;
    }
    else if (this->keys_[this->pointer_] == 'S') {
      return DEFIPATH_SHAPE;
    }
  }
  return DEFIPATH_DONE;
}

int 
defiPath_prev(defiPath * this)
{
  (this->pointer_)--;
  if (this->pointer_ >= 0 && this->pointer_ < this->numUsed_) {
    if (this->keys_[this->pointer_] == 'L') {
      return DEFIPATH_LAYER;
    }
    else if (this->keys_[this->pointer_] == 'V') {
      return DEFIPATH_VIA;
    }
    else if (this->keys_[this->pointer_] == 'W') {
      return DEFIPATH_WIDTH;
    }
    else if (this->keys_[this->pointer_] == 'P') {
      return DEFIPATH_POINT;
    }
    else if (this->keys_[this->pointer_] == 'F') {
      return DEFIPATH_FLUSHPOINT;
    }
    else if (this->keys_[this->pointer_] == 'T') {
      return DEFIPATH_TAPER;
    }
    else if (this->keys_[this->pointer_] == 'R') {
      return DEFIPATH_TAPERRULE;
    }
    else if (this->keys_[this->pointer_] == 'S') {
      return DEFIPATH_SHAPE;
    }
  }
  return DEFIPATH_DONE;
}

const char *
defiPath_getTaperRule(defiPath * this)
{
  if (this->keys_[this->pointer_] != 'R')
    return 0;
  return (char *) (this->data_[this->pointer_]);
}

const char *
defiPath_getLayer(defiPath * this)
{
  if (this->keys_[this->pointer_] != 'L')
    return 0;
  return (char *) (this->data_[this->pointer_]);
}

const char *
defiPath_getVia(defiPath * this)
{
  if (this->keys_[this->pointer_] != 'V')
    return 0;
  return (char *) (this->data_[this->pointer_]);
}

const char *
defiPath_getShape(defiPath * this)
{
  if (this->keys_[this->pointer_] != 'S')
    return 0;
  return (char *) (this->data_[this->pointer_]);
}

int 
defiPath_getWidth(defiPath * this)
{
  int    *wptr;

  if (this->keys_[this->pointer_] != 'W')
    return 0;
  wptr = (int *) (this->data_[this->pointer_]);
  return *wptr;
}

void 
defiPath_getFlushPoint(defiPath * this,
		       int *x,
		       int *y,
		       int *ext)
{
  if (this->keys_[this->pointer_] != 'P')
    return;
  *x = ((struct defiPnt *) (this->data_[this->pointer_]))->x;
  *y = ((struct defiPnt *) (this->data_[this->pointer_]))->y;
  *ext = ((struct defiPnt *) (this->data_[this->pointer_]))->ext;
}

void 
defiPath_getPoint(defiPath * this,
		  int *x,
		  int *y)
{
  if (this->keys_[this->pointer_] != 'P')
    return;
  *x = ((struct defiPnt *) (this->data_[this->pointer_]))->x;
  *y = ((struct defiPnt *) (this->data_[this->pointer_]))->y;
}

void 
defiPath_addWidth(defiPath * this,
		  int w)
{
  int    *wValue;

  if (this->numUsed_ == this->numAllocated_)
    defiPath_bumpSize(this,
		      this->numAllocated_ * 2);
  wValue = (int *) malloc(sizeof(int));
  *wValue = w;
  this->keys_[this->numUsed_] = 'W';
  this->data_[this->numUsed_] = wValue;
  (this->numUsed_)++;
}

void 
defiPath_addVia(defiPath * this,
		const char *l)
{
  int     len = strlen(l) + 1;

  if (this->numUsed_ == this->numAllocated_)
    defiPath_bumpSize(this,
		      this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'V';
  this->data_[this->numUsed_] = malloc(len);
  strcpy((char *) (this->data_[this->numUsed_]), DEFCASE(l));
  (this->numUsed_)++;
}

void 
defiPath_addLayer(defiPath * this,
		  const char *l)
{
  int     len = strlen(l) + 1;

  if (this->numUsed_ == this->numAllocated_)
    defiPath_bumpSize(this,
		      this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'L';
  this->data_[this->numUsed_] = malloc(len);
  strcpy((char *) (this->data_[this->numUsed_]), DEFCASE(l));
  (this->numUsed_)++;
}

void 
defiPath_addTaperRule(defiPath * this,
		      const char *l)
{
  int     len = strlen(l) + 1;

  if (this->numUsed_ == this->numAllocated_)
    defiPath_bumpSize(this,
		      this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'R';
  this->data_[this->numUsed_] = malloc(len);
  strcpy((char *) (this->data_[this->numUsed_]), DEFCASE(l));
  (this->numUsed_)++;
}

void 
defiPath_addPoint(defiPath * this,
		  int x,
		  int y)
{
  if (this->numUsed_ == this->numAllocated_)
    defiPath_bumpSize(this,
		      this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'P';
  this->data_[this->numUsed_] = malloc(sizeof(struct defiPnt));
  ((struct defiPnt *) (this->data_[this->numUsed_]))->x = x;
  ((struct defiPnt *) (this->data_[this->numUsed_]))->y = y;
  (this->numUsed_)++;
}

void 
defiPath_addFlushPoint(defiPath * this,
		       int x,
		       int y,
		       int ext)
{
  if (this->numUsed_ == this->numAllocated_)
    defiPath_bumpSize(this,
		      this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'F';
  this->data_[this->numUsed_] = malloc(sizeof(struct defiPnt));
  ((struct defiPnt *) (this->data_[this->numUsed_]))->x = x;
  ((struct defiPnt *) (this->data_[this->numUsed_]))->y = y;
  ((struct defiPnt *) (this->data_[this->numUsed_]))->ext = ext;
  (this->numUsed_)++;
}

void 
defiPath_setTaper(defiPath * this)
{
  if (this->numUsed_ == this->numAllocated_)
    defiPath_bumpSize(this,
		      this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'T';
  this->data_[this->numUsed_] = 0;
  (this->numUsed_)++;
}

void 
defiPath_addShape(defiPath * this,
		  const char *l)
{
  int     len = strlen(l) + 1;

  if (this->numUsed_ == this->numAllocated_)
    defiPath_bumpSize(this,
		      this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'S';
  this->data_[this->numUsed_] = malloc(len);
  strcpy((char *) (this->data_[this->numUsed_]), DEFCASE(l));
  (this->numUsed_)++;
}

void 
defiPath_print(defiPath * this,
	       FILE * fout)
{
  int     i;

  int    *wptr;

  if (fout == 0)
    fout = stdout;
  fprintf(fout, "Path:\n");
  for (i = 0;
       i < this->numUsed_;
       i++) {
    if (this->keys_[i] == 'L') {
      fprintf(fout, " layer %s\n", (char *) (this->data_[i]));
    }
    else if (this->keys_[i] == 'R') {
      fprintf(fout, " taperrule %s\n", this->data_[i] ? (char *) (this->data_[i]) : "");
    }
    else if (this->keys_[i] == 'T') {
      fprintf(fout, " taper %s\n", this->data_[i] ? (char *) (this->data_[i]) : "");
    }
    else if (this->keys_[i] == 'S') {
      fprintf(fout, " shape %s\n", this->data_[i] ? (char *) (this->data_[i]) : "");
    }
    else if (this->keys_[i] == 'V') {
      fprintf(fout, " via %s\n", this->data_[i] ? (char *) (this->data_[i]) : "");
    }
    else if (this->keys_[i] == 'W') {
      wptr = (int *) (this->data_[i]);
      fprintf(fout, " width %d\n", *wptr);
    }
    else if (this->keys_[i] == 'P') {
      fprintf(fout, " point %d,%d\n", ((struct defiPnt *) (this->data_[i]))->x, ((struct defiPnt *) (this->data_[i]))->y);
    }
    else if (this->keys_[i] == 'F') {
      fprintf(fout, " flushpoint %d,%d,%d\n", ((struct defiPnt *) (this->data_[i]))->x, ((struct defiPnt *) (this->data_[i]))->y, ((struct defiPnt *) (this->data_[i]))->ext);
    }
    else {
      fprintf(fout, " ERROR\n");
    }
  }
}

void 
defiPath_bumpSize(defiPath * this,
		  int size)
{
  int     i;

  int    *newKeys = (int *) malloc(size * sizeof(int *));

  void  **newData = (void **) malloc(size * sizeof(void *));

  for (i = 0;
       i < this->numUsed_;
       i++) {
    newKeys[i] = this->keys_[i];
    newData[i] = this->data_[i];
  }
  if (this->keys_)
    free((char *) (this->keys_));
  if (this->data_)
    free((char *) (this->data_));
  this->keys_ = newKeys;
  this->data_ = newData;
  this->numAllocated_ = size;
}
