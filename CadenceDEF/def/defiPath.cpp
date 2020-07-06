/*
 *     This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 *  Distribution,  Product Version 5.3, and is subject to the Cadence
 *  LEF/DEF Open Source License Agreement.   Your  continued  use  of
 *  this file constitutes your acceptance of the terms of the LEF/DEF
 *  Open Source License and an agreement to abide by its  terms.   If
 *  you  don't  agree  with  this, you must remove this and any other
 *  files which are part of the distribution and  destroy any  copies
 *  made.
 *
 *     For updates, support, or to become part of the LEF/DEF Community,
 *  check www.openeda.org for details.
 */

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "defiPath.hpp"
#include "defiDebug.hpp"

defiPath::defiPath() {
  this->defiPath::Init();
}


defiPath::~defiPath() {
  this->defiPath::Destroy();
}


void defiPath::Init() {
  this->numUsed_ = 0;
  this->numAllocated_ = 0;
  this->keys_ = 0;
  this->data_ = 0;
  this->defiPath::bumpSize(16);
}


void defiPath::clear() {
  int i;

  for (i = 0; i < this->numUsed_; i++) {

    if (this->keys_[i] == 'L') {
      if (this->data_[i]) free((char*)(this->data_[i]));

    } else if (this->keys_[i] == 'R') {
      if (this->data_[i]) free((char*)(this->data_[i]));

    } else if (this->keys_[i] == 'V') {
      if (this->data_[i]) free((char*)(this->data_[i]));

    } else if (this->keys_[i] == 'W') {
      if (this->data_[i]) free((char*)(this->data_[i]));

    } else if (this->keys_[i] == 'P') {
      if (this->data_[i]) free((char*)(this->data_[i]));

    } else if (this->keys_[i] == 'F') {
      if (this->data_[i]) free((char*)(this->data_[i]));

    } else if (this->keys_[i] == 'S') {
      if (this->data_[i]) free((char*)(this->data_[i]));
    }
  }
  this->numUsed_ = 0;
}


void defiPath::Destroy() {
  this->defiPath::clear();
  free((char*)(this->keys_));
  free((char*)(this->data_));
}


void defiPath::reverseOrder() {
  int one = 0;
  int two = this->numUsed_ - 1;
  int t;
  void *tptr;
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


void defiPath::initTraverse() {
  this->pointer_ = -1;
}


void defiPath::initTraverseBackwards() {
  this->pointer_ = this->numUsed_;
}


int defiPath::next() {
  (this->pointer_)++;
  if (this->pointer_ >= 0 && this->pointer_ < this->numUsed_) {
    if (this->keys_[this->pointer_] == 'L') {
      return DEFIPATH_LAYER;
    } else if (this->keys_[this->pointer_] == 'V') {
      return DEFIPATH_VIA;
    } else if (this->keys_[this->pointer_] == 'W') {
      return DEFIPATH_WIDTH;
    } else if (this->keys_[this->pointer_] == 'P') {
      return DEFIPATH_POINT;
    } else if (this->keys_[this->pointer_] == 'F') {
      return DEFIPATH_FLUSHPOINT;
    } else if (this->keys_[this->pointer_] == 'T') {
      return DEFIPATH_TAPER;
    } else if (this->keys_[this->pointer_] == 'R') {
      return DEFIPATH_TAPERRULE;
    } else if (this->keys_[this->pointer_] == 'S') {
      return DEFIPATH_SHAPE;
    }
  }

  return DEFIPATH_DONE;
}


int defiPath::prev() {
  (this->pointer_)--;
  if (this->pointer_ >= 0 && this->pointer_ < this->numUsed_) {
    if (this->keys_[this->pointer_] == 'L') {
      return DEFIPATH_LAYER;
    } else if (this->keys_[this->pointer_] == 'V') {
      return DEFIPATH_VIA;
    } else if (this->keys_[this->pointer_] == 'W') {
      return DEFIPATH_WIDTH;
    } else if (this->keys_[this->pointer_] == 'P') {
      return DEFIPATH_POINT;
    } else if (this->keys_[this->pointer_] == 'F') {
      return DEFIPATH_FLUSHPOINT;
    } else if (this->keys_[this->pointer_] == 'T') {
      return DEFIPATH_TAPER;
    } else if (this->keys_[this->pointer_] == 'R') {
      return DEFIPATH_TAPERRULE;
    } else if (this->keys_[this->pointer_] == 'S') {
      return DEFIPATH_SHAPE;
    }
  }

  return DEFIPATH_DONE;
}


const char* defiPath::getTaperRule() {
  if (this->keys_[this->pointer_] != 'R') return 0;
  return (char*)(this->data_[this->pointer_]);
}

const char* defiPath::getLayer() {
  if (this->keys_[this->pointer_] != 'L') return 0;
  return (char*)(this->data_[this->pointer_]);
}


const char* defiPath::getVia() {
  if (this->keys_[this->pointer_] != 'V') return 0;
  return (char*)(this->data_[this->pointer_]);
}


const char* defiPath::getShape() {
  if (this->keys_[this->pointer_] != 'S') return 0;
  return (char*)(this->data_[this->pointer_]);
}


int defiPath::getWidth() {
  int *wptr;
  if (this->keys_[this->pointer_] != 'W') return 0;
  wptr = (int*)(this->data_[this->pointer_]);
  return *wptr;
}


void defiPath::getFlushPoint(int* x, int* y, int* ext) {
  if (this->keys_[this->pointer_] != 'P') return ;
  *x = ((struct defiPnt*)(this->data_[this->pointer_]))->x;
  *y = ((struct defiPnt*)(this->data_[this->pointer_]))->y;
  *ext = ((struct defiPnt*)(this->data_[this->pointer_]))->ext;
}


void defiPath::getPoint(int* x, int* y) {
  if (this->keys_[this->pointer_] != 'P') return ;
  *x = ((struct defiPnt*)(this->data_[this->pointer_]))->x;
  *y = ((struct defiPnt*)(this->data_[this->pointer_]))->y;
}


void defiPath::addWidth(int w) {
  int * wValue;
  if (this->numUsed_ == this->numAllocated_)
    this->defiPath::bumpSize(this->numAllocated_ * 2);
  wValue = (int*)malloc(sizeof(int));
  *wValue = w;
  this->keys_[this->numUsed_] = 'W';
  this->data_[this->numUsed_] = wValue;
  (this->numUsed_)++;
}


void defiPath::addVia(const char* l) {
  int len = strlen(l)+1;
  if (this->numUsed_ == this->numAllocated_)
    this->defiPath::bumpSize(this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'V';
  this->data_[this->numUsed_] = malloc(len);
  strcpy((char*)(this->data_[this->numUsed_]), DEFCASE(l));
  (this->numUsed_)++;
}


void defiPath::addLayer(const char* l) {
  int len = strlen(l)+1;
  if (this->numUsed_ == this->numAllocated_)
    this->defiPath::bumpSize(this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'L' ;
  this->data_[this->numUsed_] = malloc(len);
  strcpy((char*)(this->data_[this->numUsed_]), DEFCASE(l));
  (this->numUsed_)++;
}


void defiPath::addTaperRule(const char* l) {
  int len = strlen(l)+1;
  if (this->numUsed_ == this->numAllocated_)
    this->defiPath::bumpSize(this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'R' ;
  this->data_[this->numUsed_] = malloc(len);
  strcpy((char*)(this->data_[this->numUsed_]), DEFCASE(l));
  (this->numUsed_)++;
}


void defiPath::addPoint(int x, int y) {
  if (this->numUsed_ == this->numAllocated_)
    this->defiPath::bumpSize(this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'P';
  this->data_[this->numUsed_] = malloc(sizeof(struct defiPnt));
  ((struct defiPnt*)(this->data_[this->numUsed_])) -> x = x;
  ((struct defiPnt*)(this->data_[this->numUsed_])) -> y = y;
  (this->numUsed_)++;
}


void defiPath::addFlushPoint(int x, int y, int ext) {
  if (this->numUsed_ == this->numAllocated_)
    this->defiPath::bumpSize(this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'F';
  this->data_[this->numUsed_] = malloc(sizeof(struct defiPnt));
  ((struct defiPnt*)(this->data_[this->numUsed_])) -> x = x;
  ((struct defiPnt*)(this->data_[this->numUsed_])) -> y = y;
  ((struct defiPnt*)(this->data_[this->numUsed_])) -> ext = ext;
  (this->numUsed_)++;
}


void defiPath::setTaper() {
  if (this->numUsed_ == this->numAllocated_)
    this->defiPath::bumpSize(this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'T';
  this->data_[this->numUsed_] = 0;
  (this->numUsed_)++;
}


void defiPath::addShape(const char* l) {
  int len = strlen(l)+1;
  if (this->numUsed_ == this->numAllocated_)
    this->defiPath::bumpSize(this->numAllocated_ * 2);
  this->keys_[this->numUsed_] = 'S' ;
  this->data_[this->numUsed_] = malloc(len);
  strcpy((char*)(this->data_[this->numUsed_]), DEFCASE(l));
  (this->numUsed_)++;
}


void defiPath::print(FILE* fout) {
  int i;
  int *wptr;
  if (fout == 0) fout = stdout;
  fprintf(fout, "Path:\n");
  for (i = 0; i < this->numUsed_; i++) {

    if (this->keys_[i] == 'L') {
      fprintf(fout, " layer %s\n", (char*)(this->data_[i]));

    } else if (this->keys_[i] == 'R') {
      fprintf(fout, " taperrule %s\n", this->data_[i] ? (char*)(this->data_[i])
      : "" );

    } else if (this->keys_[i] == 'T') {
      fprintf(fout, " taper %s\n", this->data_[i] ? (char*)(this->data_[i])
      : "" );

    } else if (this->keys_[i] == 'S') {
      fprintf(fout, " shape %s\n", this->data_[i] ? (char*)(this->data_[i])
      : "" );

    } else if (this->keys_[i] == 'V') {
      fprintf(fout, " via %s\n", this->data_[i] ? (char*)(this->data_[i])
      : "" );

    } else if (this->keys_[i] == 'W') {
      wptr = (int*)(this->data_[i]);
      fprintf(fout, " width %d\n", *wptr);

    } else if (this->keys_[i] == 'P') {
      fprintf(fout, " point %d,%d\n", ((struct defiPnt*)(this->data_[i]))->x,
                                      ((struct defiPnt*)(this->data_[i]))->y);

    } else if (this->keys_[i] == 'F') {
      fprintf(fout, " flushpoint %d,%d,%d\n",
                                      ((struct defiPnt*)(this->data_[i]))->x,
                                      ((struct defiPnt*)(this->data_[i]))->y,
                                      ((struct defiPnt*)(this->data_[i]))->ext);

    } else {
      fprintf(fout, " ERROR\n");
    }
  }
}


void defiPath::bumpSize(int size) {
  int i;
  int* newKeys = (int*)malloc(size * sizeof(int*));
  void** newData = (void**)malloc(size * sizeof(void*));

  for (i = 0; i < this->numUsed_; i++) {
    newKeys[i] = this->keys_[i];
    newData[i] = this->data_[i];
  }

  if (this->keys_) free((char*)(this->keys_));
  if (this->data_) free((char*)(this->data_));

  this->keys_ = newKeys;
  this->data_ = newData;
  this->numAllocated_ = size;
}


