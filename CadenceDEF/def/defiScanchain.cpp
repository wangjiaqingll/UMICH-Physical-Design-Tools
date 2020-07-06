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

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "defiScanchain.hpp"
#include "defiDebug.hpp"


void defiOrdered::clear() {
  int i;
  for (i = 0; i < this->num_; i++) {
    free((char*)(this->inst_));
    if (this->in_) free((char*)(this->in_));
    if (this->out_) free((char*)(this->out_));
  }
  this->num_ = 0;
}


void defiOrdered::Destroy() {
  int i;
  for (i = 0; i < this->num_; i++) {
     free((char*)(this->inst_[i]));
     free((char*)(this->in_[i]));
     free((char*)(this->out_[i]));
  }
  free((char*)(this->inst_));
  free((char*)(this->in_));
  free((char*)(this->out_));
}


void defiOrdered::Init() {
  this->num_ = 0;
  this->allocated_ = 32;
  this->inst_ = (char**)malloc(sizeof(char*)*32);
  this->in_ = (char**)malloc(sizeof(char*)*32);
  this->out_ = (char**)malloc(sizeof(char*)*32);
}


void defiOrdered::bump() {
  int max = this->allocated_ * 2;
  int i;
  char** nin = (char**)malloc(sizeof(char*)*max);
  char** nout = (char**)malloc(sizeof(char*)*max);
  char** ninst = (char**)malloc(sizeof(char*)*max);

  for (i = 0; i < this->num_; i++) {
    nin[i] = this->in_[i];
    nout[i] = this->out_[i];
    ninst[i] = this->inst_[i];
  }
  free((char*)(this->inst_));
  free((char*)(this->in_));
  free((char*)(this->out_));
  this->allocated_ = max;
  this->inst_ = ninst;
  this->in_ = nin;
  this->out_ = nout;
}


void defiOrdered::addOrdered(const char* inst) {
  if (this->num_ == this->allocated_) this->defiOrdered::bump();
  this->inst_[this->num_] = (char*)malloc(strlen(inst)+1);
  strcpy(this->inst_[this->num_], DEFCASE(inst));
  this->in_[this->num_] = 0;
  this->out_[this->num_] = 0;
  this->num_ += 1;
}


void defiOrdered::addIn(const char* pin) {
  this->in_[this->num_-1] = (char*)malloc(strlen(pin)+1);
  strcpy(this->in_[this->num_-1], DEFCASE(pin));
}


void defiOrdered::addOut(const char* pin) {
  this->out_[this->num_-1] = (char*)malloc(strlen(pin)+1);
  strcpy(this->out_[this->num_-1], DEFCASE(pin));
}


int defiOrdered::num() const {
  return this->num_;
}


char** defiOrdered::inst() const {
  return this->inst_;
}


char** defiOrdered::in() const {
  return this->in_;
}


char** defiOrdered::out() const {
  return this->out_;
}


defiScanchain::defiScanchain() {
  this->defiScanchain::Init();
}


void defiScanchain::Init() {
  this->name_ = (char*)malloc(32);
  this->nameLength_ = 32;

  this->numOrdered_ = 0;
  this->numOrderedAllocated_ = 4;
  this->ordered_ = (defiOrdered**)malloc(sizeof(defiOrdered*)*4);

  this->numFloating_ = 0;
  this->numFloatingAllocated_ = 4;
  this->floatInst_ = (char**)malloc(sizeof(char*)*4);
  this->floatIn_ = (char**)malloc(sizeof(char*)*4);
  this->floatOut_ = (char**)malloc(sizeof(char*)*4);

  this->stopInst_ = 0;
  this->stopPin_ = 0;
  this->startInst_ = 0;
  this->startPin_ = 0;
  this->hasStart_ = 0;
  this->hasStop_ = 0;
  this->commonInPin_ = 0;
  this->commonOutPin_ = 0;
}


void defiScanchain::clear() {
  int i;
  defiOrdered* o;

  for (i = 0; i < this->numOrdered_; i++) {
    o = this->ordered_[i];
    o->defiOrdered::Destroy();
    free((char*)o);
    this->ordered_[i] = 0;
  }
  this->numOrdered_ = 0;

  for (i = 0; i < this->numFloating_; i++) {
    if (this->floatIn_[i]) free(this->floatIn_[i]);
    if (this->floatOut_[i]) free(this->floatOut_[i]);
    free(this->floatInst_[i]);
    this->floatInst_[i] = 0;
  }
  this->numFloating_ = 0;

  if (this->stopInst_) free(this->stopInst_);
  if (this->stopPin_) free(this->stopPin_);
  if (this->startInst_) free(this->startInst_);
  if (this->startPin_) free(this->startPin_);
  if (this->commonInPin_) free(this->commonInPin_);
  if (this->commonOutPin_) free(this->commonOutPin_);

  this->stopInst_ = 0;
  this->stopPin_ = 0;
  this->startInst_ = 0;
  this->startPin_ = 0;
  this->hasStart_ = 0;
  this->hasStop_ = 0;
  this->commonInPin_ = 0;
  this->commonOutPin_ = 0;
}


void defiScanchain::Destroy() {
  this->defiScanchain::clear();
  free(this->name_);
  free((char*)(this->ordered_));
  free((char*)(this->floatInst_));
  free((char*)(this->floatIn_));
  free((char*)(this->floatOut_));
}


defiScanchain::~defiScanchain() {
  this->defiScanchain::Destroy();
}


void defiScanchain::setName(const char* name) {
  int len = strlen(name) + 1;

  this->defiScanchain::clear();

  if (len > this->nameLength_) {
    free(this->name_);
    this->name_ = (char*)malloc(len);
    this->nameLength_ = len;
  }
  strcpy(this->name_, DEFCASE(name));
}


const char* defiScanchain::name() const {
  return this->name_;
}


void defiScanchain::addFloatingInst(const char* name) {

  if (this->numFloating_ >= this->numFloatingAllocated_) {
    int max = 2 * this->numFloatingAllocated_;
    int i;
    char** ninst = (char**)malloc(sizeof(char*)*max);
    char** nin = (char**)malloc(sizeof(char*)*max);
    char** nout = (char**)malloc(sizeof(char*)*max);
    for (i = 0; i < this->numFloating_; i++) {
      ninst[i] = this->floatInst_[i];
      nin[i] = this->floatIn_[i];
      nout[i] = this->floatOut_[i];
    }
    free((char*)(this->floatInst_));
    free((char*)(this->floatIn_));
    free((char*)(this->floatOut_));
    this->floatInst_ = ninst;
    this->floatOut_ = nout;
    this->floatIn_ = nin;
    this->numFloatingAllocated_ = max;
  }

  this->floatInst_[this->numFloating_] =
     (char*)malloc(strlen(name) + 1);
  strcpy(this->floatInst_[this->numFloating_], DEFCASE(name));
  this->floatIn_[this->numFloating_] = 0;
  this->floatOut_[this->numFloating_] = 0;
  this->numFloating_ += 1;
}


void defiScanchain::addFloatingIn(const char* name) {
  int len = strlen(name) + 1;
  this->floatIn_[this->numFloating_-1] = (char*)malloc(len);
  strcpy(this->floatIn_[this->numFloating_-1], DEFCASE(name));
}


void defiScanchain::addFloatingOut(const char* name) {
  int len = strlen(name) + 1;
  this->floatOut_[this->numFloating_-1] = (char*)malloc(len);
  strcpy(this->floatOut_[this->numFloating_-1], DEFCASE(name));
}


void defiScanchain::addOrderedIn(const char* name) {
  defiOrdered* o = this->ordered_[this->numOrdered_-1];
  o->defiOrdered::addIn(name);
}


void defiScanchain::addOrderedOut(const char* name) {
  defiOrdered* o = this->ordered_[this->numOrdered_-1];
  o->defiOrdered::addOut(name);
}


void defiScanchain::addOrderedInst(const char* name) {
  defiOrdered* o = this->ordered_[this->numOrdered_-1];
  o->defiOrdered::addOrdered(name);
}


void defiScanchain::addOrderedList() {
  defiOrdered* o;

  if (this->numOrdered_ == this->numOrderedAllocated_) {
    int max = 2 * this->numFloatingAllocated_;
    int i;
    defiOrdered** no = (defiOrdered**)malloc(sizeof(defiOrdered*)*max);
    for (i = 0; i < this->numFloating_; i++) {
      no[i] = this->ordered_[i];
    }
    free((char*)(this->ordered_));
    this->ordered_ = no;
    this->numOrderedAllocated_ = max;
  }

  o = (defiOrdered*)malloc(sizeof(defiOrdered));
  this->ordered_[this->numOrdered_] = o;
  o->defiOrdered::Init();
  this->numOrdered_ += 1;
}


void defiScanchain::setStart(const char* inst, const char* pin) {
   int len;
   if (this->startInst_) defiError("duplicate start in scan chain");
   len = strlen(inst) + 1;
   this->startInst_ = (char*)malloc(len);
   strcpy(this->startInst_, DEFCASE(inst));
   len = strlen(pin) + 1;
   this->startPin_ = (char*)malloc(len);
   strcpy(this->startPin_, DEFCASE(pin));
   this->hasStart_ = 1;
}


void defiScanchain::setStop(const char* inst, const char* pin) {
   int len;
   if (this->stopInst_) defiError("duplicate stop in scan chain");
   len = strlen(inst) + 1;
   this->stopInst_ = (char*)malloc(len);
   strcpy(this->stopInst_, DEFCASE(inst));
   len = strlen(pin) + 1;
   this->stopPin_ = (char*)malloc(len);
   strcpy(this->stopPin_, DEFCASE(pin));
   this->hasStop_ = 1;
}


int defiScanchain::hasStart() const {
  return (int)(this->hasStart_);
}


int defiScanchain::hasStop() const {
  return (int)(this->hasStop_);
}


int defiScanchain::hasFloating() const {
  return this->numFloating_ ? 1 : 0 ;
}


int defiScanchain::hasOrdered() const {
  return this->numOrdered_ ? 1 : 0 ;
}


void defiScanchain::start(char** inst, char** pin) const {
  if (inst) *inst = this->startInst_;
  if (pin) *pin = this->startPin_;
}


void defiScanchain::stop(char** inst, char** pin) const {
  if (inst) *inst = this->stopInst_;
  if (pin) *pin = this->stopPin_;
}


int defiScanchain::numOrderedLists() const {
  return this->numOrdered_;
}


void defiScanchain::ordered(int index, int* size, char*** inst,
                        char*** inPin, char*** outPin) const {
  defiOrdered* o;
  if (index >= 0 && index <= this->numOrdered_) {
    o = this->ordered_[index];
    *size = o->defiOrdered::num();
    *inst = o->defiOrdered::inst();
    *inPin = o->defiOrdered::in();
    *outPin = o->defiOrdered::out();
  } else {
    *size = 10;
    *inst = 0;
  }
}


void defiScanchain::floating(int* size, char*** inst,
                 char*** inPin, char*** outPin) const {
  *size = this->numFloating_;
  *inst = this->floatInst_;
  *inPin = this->floatIn_;
  *outPin = this->floatOut_;
}


void defiScanchain::setCommonOut(const char* pin) {
  int len = strlen(pin) + 1;
  this->commonOutPin_ = (char*)malloc(len);
  strcpy(this->commonOutPin_, DEFCASE(pin));
}


void defiScanchain::setCommonIn(const char* pin) {
  int len = strlen(pin) + 1;
  this->commonInPin_ = (char*)malloc(len);
  strcpy(this->commonInPin_, DEFCASE(pin));
}


int defiScanchain::hasCommonInPin() const {
  return this->commonInPin_ ? 1 : 0;
}


int defiScanchain::hasCommonOutPin() const {
  return this->commonOutPin_ ? 1 : 0;
}


const char* defiScanchain::commonInPin() const {
  return this->commonInPin_;
}


const char* defiScanchain::commonOutPin() const {
  return this->commonOutPin_;
}


void defiScanchain::print(FILE* f) const {
  char* a;
  char* b;

  char** inst;
  char** in;
  char** out;
  int num;
  int i;
  int h;

  fprintf(f, "Scan chain '%s'  %d things\n",
      this->defiScanchain::name(),
      this->defiScanchain::hasStart() +
      this->defiScanchain::hasStop() +
      this->numFloating_ +
      this->defiScanchain::numOrderedLists());

  if (this->defiScanchain::hasStart()) {
    this->defiScanchain::start(&a, &b);
    fprintf(f, "  start inst '%s' pin '%s'\n", a, b);
  }

  if (this->defiScanchain::hasStop()) {
    this->defiScanchain::stop(&a, &b);
    fprintf(f, "  stop inst '%s' pin '%s'\n", a, b);
  }

  this->defiScanchain::floating(&num, &inst, &in, &out);
  for (i = 0; i < num; i++) {
    fprintf(f, "  floating '%s' IN=%s  OUT=%s\n", inst[i],
       in[i]?in[i]:"NIL", out[i]?out[i]:"NIL");
  }

  for (i = 0; i < this->defiScanchain::numOrderedLists(); i++) {
    this->defiScanchain::ordered(i, &num, &inst, &in, &out);
    fprintf(f, "  %d ordered\n", i);
    for (h = 0; h < num; h++) {
      fprintf(f, "        '%s' IN=%s OUT=%s\n", inst[h],
       in[h]?in[h]:"NIL", out[h]?out[h]:"NIL");
    }
  }

  if (this->defiScanchain::hasCommonInPin())
    fprintf(f, "  common in pin %s\n", this->defiScanchain::commonInPin());

  if (this->defiScanchain::hasCommonOutPin())
    fprintf(f, "  common out pin %s\n", this->defiScanchain::commonOutPin());
}


