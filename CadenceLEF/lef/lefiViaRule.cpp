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
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "lefiViaRule.hpp"
#include "lefiDebug.hpp"

///////////////////////////////////////////
///////////////////////////////////////////
//
//    lefiViaRuleLayer
//
///////////////////////////////////////////
///////////////////////////////////////////

lefiViaRuleLayer::lefiViaRuleLayer() {
  this->lefiViaRuleLayer::Init();
}


void lefiViaRuleLayer::Init() {
  this->name_ = 0;
}


void lefiViaRuleLayer::setName(const char* name) {
  int len = strlen(name) + 1;
  if (this->name_) free(this->name_);
  this->name_ = (char*)malloc(len);
  strcpy(this->name_, CASE(name));
  this->direction_ = '\0';
  this->hasWidth_ = 0;
  this->hasResistance_ = 0;
  this->hasOverhang_ = 0;
  this->hasMetalOverhang_ = 0;
  this->hasSpacing_ = 0;
  this->hasRect_ = 0;
}


void lefiViaRuleLayer::Destroy() {
  if (this->name_) free(this->name_);
}


lefiViaRuleLayer::~lefiViaRuleLayer() {
  // Destroy will be called explicitly
  // so do nothing here.
}


void lefiViaRuleLayer::setHorizontal() {
  this->direction_ = 'H';
}


void lefiViaRuleLayer::setVertical() {
  this->direction_ = 'V';
}


void lefiViaRuleLayer::setWidth(double minW, double maxW) {
  this->hasWidth_ = 1;
  this->widthMin_ = minW;
  this->widthMax_ = maxW;
}


void lefiViaRuleLayer::setOverhang(double d) {
  this->hasOverhang_ = 1;
  this->overhang_ = d;
}


void lefiViaRuleLayer::setMetalOverhang(double d) {
  this->hasMetalOverhang_ = 1;
  this->metalOverhang_ = d;
}


void lefiViaRuleLayer::setResistance(double d) {
  this->hasResistance_ = 1;
  this->resistance_ = d;
}


void lefiViaRuleLayer::setSpacing(double x, double y) {
  this->hasSpacing_ = 1;
  this->spacingStepX_ = x;
  this->spacingStepY_ = y;
}


void lefiViaRuleLayer::setRect(double xl, double yl,
          double xh, double yh) {
  this->hasRect_ = 1;
  this->xl_ = xl;
  this->yl_ = yl;
  this->xh_ = xh;
  this->yh_ = yh;
}


int lefiViaRuleLayer::hasRect() const {
  return this->hasRect_;
}


int lefiViaRuleLayer::hasDirection() const {
  return this->direction_ ? 1 : 0 ;
}


int lefiViaRuleLayer::hasWidth() const {
  return this->hasWidth_;
}


int lefiViaRuleLayer::hasResistance() const {
  return this->hasResistance_;
}


int lefiViaRuleLayer::hasOverhang() const {
  return this->hasOverhang_;
}


int lefiViaRuleLayer::hasMetalOverhang() const {
  return this->hasMetalOverhang_;
}


int lefiViaRuleLayer::hasSpacing() const {
  return this->hasSpacing_;
}


char* lefiViaRuleLayer::name() const {
  return this->name_;
}


int lefiViaRuleLayer::isHorizontal() const {
  return this->direction_ == 'H' ? 1 : 0;
}


int lefiViaRuleLayer::isVertical() const {
  return this->direction_ == 'V' ? 1 : 0;
}


double lefiViaRuleLayer::widthMin() const {
  return this->widthMin_;
}


double lefiViaRuleLayer::widthMax() const {
  return this->widthMax_;
}


double lefiViaRuleLayer::overhang() const {
  return this->overhang_;
}


double lefiViaRuleLayer::metalOverhang() const {
  return this->metalOverhang_;
}


double lefiViaRuleLayer::resistance() const {
  return this->resistance_;
}


double lefiViaRuleLayer::spacingStepX() const {
  return this->spacingStepX_;
} 


double lefiViaRuleLayer::spacingStepY() const {
  return this->spacingStepY_;
}


double lefiViaRuleLayer::xl() const {
  return this->xl_;
}


double lefiViaRuleLayer::yl() const {
  return this->yl_;
}


double lefiViaRuleLayer::xh() const {
  return this->xh_;
}


double lefiViaRuleLayer::yh() const {
  return this->yh_;
}


void lefiViaRuleLayer::print(FILE* f) const {
  fprintf(f, "  Layer %s", this->name_);

  if (this->lefiViaRuleLayer::isHorizontal()) fprintf(f, " HORIZONTAL");
  if (this->lefiViaRuleLayer::isVertical()) fprintf(f, " VERTICAL");
  fprintf(f, "\n");

  if (this->lefiViaRuleLayer::hasWidth())
    fprintf(f, "    WIDTH %lg %lg\n", this->lefiViaRuleLayer::widthMin(),
      this->lefiViaRuleLayer::widthMax());

  if (this->lefiViaRuleLayer::hasResistance())
    fprintf(f, "    RESISTANCE %lg\n", this->lefiViaRuleLayer::resistance());

  if (this->lefiViaRuleLayer::hasOverhang())
    fprintf(f, "    OVERHANG %lg\n", this->lefiViaRuleLayer::overhang());

  if (this->lefiViaRuleLayer::hasMetalOverhang())
    fprintf(f, "    METALOVERHANG %lg\n",
    this->lefiViaRuleLayer::metalOverhang());
 
  if (this->lefiViaRuleLayer::hasSpacing())
    fprintf(f, "    SPACING %lg %lg\n",
    this->lefiViaRuleLayer::spacingStepX(),
    this->lefiViaRuleLayer::spacingStepY());

  if (this->lefiViaRuleLayer::hasRect())
    fprintf(f, "    RECT %lg,%lg %lg,%lg\n",
    this->lefiViaRuleLayer::xl(), this->lefiViaRuleLayer::yl(),
    this->lefiViaRuleLayer::xh(), this->lefiViaRuleLayer::yh());

}



///////////////////////////////////////////
///////////////////////////////////////////
//
//    lefiViaRule
//
///////////////////////////////////////////
///////////////////////////////////////////



lefiViaRule::lefiViaRule() {
  this->lefiViaRule::Init();
}


void lefiViaRule::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)malloc(16);
  this->viasAllocated_ = 2;
  this->vias_ = (char**)malloc(sizeof(char*)*2);
  this->layers_[0].lefiViaRuleLayer::Init();
  this->layers_[1].lefiViaRuleLayer::Init();
  this->layers_[2].lefiViaRuleLayer::Init();
  this->numLayers_ = 0;
  this->numVias_ = 0;
  this->numProps_ = 0;
  this->propsAllocated_ = 1;
  this->names_ = (char**)malloc(sizeof(char*));
  this->values_ = (char**)malloc(sizeof(char*));
}


void lefiViaRule::clear() {
  int i;
  this->hasGenerate_ = 0;
  for (i = 0; i < this->numProps_; i++) {
    free(this->names_[i]);
    free(this->values_[i]);
  }
  this->numProps_ = 0;
  this->numLayers_ = 0;
  for (i = 0; i < this->numVias_; i++) {
    free(this->vias_[i]);
  }
  this->numVias_ = 0;
}


void lefiViaRule::setName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char*)malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
  this->lefiViaRule::clear();
}


void lefiViaRule::Destroy() {
  this->lefiViaRule::clear();
  free(this->name_);
  free((char*)(this->vias_));
  free((char*)(this->names_));
  free((char*)(this->values_));
  this->layers_[0].lefiViaRuleLayer::Destroy();
  this->layers_[1].lefiViaRuleLayer::Destroy();
  this->layers_[2].lefiViaRuleLayer::Destroy();
}


lefiViaRule::~lefiViaRule() {
  this->lefiViaRule::Destroy();
}


void lefiViaRule::setGenerate() {
  this->hasGenerate_ = 1;
}


void lefiViaRule::addViaName(const char* name) {
  // Add one of possibly many via names
  int len = strlen(name) + 1;
  if (this->numVias_ == this->viasAllocated_) {
    int i;
    char** nn;
    if (this->viasAllocated_ == 0)
       this->viasAllocated_ = 2;
    else
       this->viasAllocated_ *= 2;
    nn = (char**)malloc(sizeof(char*) * this->viasAllocated_);
    for (i = 0; i < this->numVias_; i++)
      nn[i] = this->vias_[i];
    free((char*)(this->vias_));
    this->vias_ = nn;
  }
  this->vias_[this->numVias_] = (char*)malloc(len);
  strcpy(this->vias_[this->numVias_], CASE(name));
  this->numVias_ += 1;
}


void lefiViaRule::setRect(double xl, double yl, double xh, double yh) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setRect(xl, yl, xh, yh);
}


void lefiViaRule::setSpacing(double x, double y) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setSpacing(x, y);
}


void lefiViaRule::setWidth(double x, double y) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setWidth(x, y);
}


void lefiViaRule::setResistance(double d) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setResistance(d);
}


void lefiViaRule::setOverhang(double d) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setOverhang(d);
}


void lefiViaRule::setMetalOverhang(double d) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setMetalOverhang(d);
}


void lefiViaRule::setVertical() {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setVertical();
}


void lefiViaRule::setHorizontal() {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setHorizontal();
}


void lefiViaRule::setLayer(const char* name) {
  if (this->numLayers_ == 3)
    { lefiError("ERROR: too many via rule layers"); return; }
  // This routine sets and creates the active layer.
  this->layers_[this->numLayers_].lefiViaRuleLayer::setName(name);
  this->numLayers_ += 1;
}


int lefiViaRule::hasGenerate() const {
  return this->hasGenerate_;
} 


int lefiViaRule::numLayers() const {
  // There are 2 or 3 layers in a rule.
  // numLayers() tells how many.
  // If a third layer exists then it is the cut layer.
  return this->numLayers_;
}


lefiViaRuleLayer* lefiViaRule::layer(int index) {
  if (index < 0 || index > 2) return 0;
  return &(this->layers_[index]);
}


char* lefiViaRule::name() const {
  return this->name_;
}


void lefiViaRule::print(FILE* f) const {
  int i;
  fprintf(f, "VIA RULE %s", this->lefiViaRule::name());
  if (this->lefiViaRule::hasGenerate())
    fprintf(f, " GENERATE");
  fprintf(f, "\n");

  for (i = 0; i < this->lefiViaRule::numLayers(); i++) {
    this->layers_[i].lefiViaRuleLayer::print(f);
  }

  for (i = 0; i < this->lefiViaRule::numVias(); i++) {
    fprintf(f, "  Via %s\n", this->lefiViaRule::viaName(i));
  }
}


int lefiViaRule::numVias() const {
  return this->numVias_;
}


char* lefiViaRule::viaName(int index) const {
  if (index < 0 || index >= this->numVias_) return 0;
  return this->vias_[index];
}



int lefiViaRule::numProps() const {
  return this->numProps_;
}


void lefiViaRule::addProp(const char* name, const char* value) {
  int len = strlen(name) + 1;
  if (this->numProps_ == this->propsAllocated_) {
    int i;
    int max;
    int lim;
    char** nn;
    char** nv;
    if (this->propsAllocated_ == 0)
       this->propsAllocated_ = 1;    // initialize propsAllocated_
    max = this->propsAllocated_ *= 2;
    lim = this->numProps_;
    nn = (char**)malloc(sizeof(char*)*max);
    nv = (char**)malloc(sizeof(char*)*max);
    for (i = 0; i < lim; i++) {
      nn[i] = this->names_[i];
      nv[i] = this->values_[i];
    }
    free((char*)(this->names_));
    free((char*)(this->values_));
    this->names_ = nn;
    this->values_ = nv;
  }
  this->names_[this->numProps_] = (char*)malloc(sizeof(char)*len);
  strcpy(this->names_[this->numProps_],name);
  len = strlen(value) + 1;
  this->values_[this->numProps_] = (char*)malloc(sizeof(char)*len);
  strcpy(this->values_[this->numProps_],value);
  this->numProps_ += 1;
}


const char* lefiViaRule::propName(int i) const {
  return this->names_[i];
}


const char* lefiViaRule::propValue(int i) const {
  return this->values_[i];
}


