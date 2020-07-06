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

#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "lefiNonDefault.hpp"
#include "lefiDebug.hpp"
#include "lefrCallBacks.hpp"

// 6/16/2000 - Wanda da Rosa
// Make these variables in globals.  Can't use those defined
// in the class because it generates warning when it casts.
// Can't assign for example lefrViaCbkFnType to oldViaCbk_
// in the class because it requires to include lefrReader.hpp
// in lefiNonDefault.hpp.  But in lefrReader.hpp, it requires
// include lefiNonDefault.hpp, this creates a loop and is
// problematic...

static lefrViaCbkFnType oldViaCbk;
static lefrSpacingCbkFnType oldSpacingCbk;
static lefrVoidCbkFnType oldSpacingBeginCbk;
static lefrVoidCbkFnType oldSpacingEndCbk;

/////////////////////////////////////////
/////////////////////////////////////////
//
//     lefiNonDefault
//
/////////////////////////////////////////
/////////////////////////////////////////


lefiNonDefault::lefiNonDefault() { 
  this->lefiNonDefault::Init();
}


void lefiNonDefault::Init() { 
  this->nameSize_ = 16;
  this->name_ = (char*)malloc(16);

  this->layersAllocated_ = 2;
  this->numLayers_ = 0;
  this->layerName_ = (char**)malloc(sizeof(char*)*2);
  this->width_ = (double*)malloc(sizeof(double)*2);
  this->spacing_ = (double*)malloc(sizeof(double)*2);
  this->wireExtension_ = (double*)malloc(sizeof(double)*2);
  this->hasWidth_ = (char*)malloc(sizeof(char)*2);
  this->hasSpacing_ = (char*)malloc(sizeof(char)*2);
  this->hasWireExtension_ = (char*)malloc(sizeof(char)*2);

  this->allocatedVias_ = 2;
  this->numVias_ = 0;
  this->viaRules_ = (lefiVia**)malloc(sizeof(lefiVia*)*2);

  this->allocatedSpacing_ = 2;
  this->numSpacing_ = 0;
  this->spacingRules_ = (lefiSpacing**)malloc(
          sizeof(lefiSpacing*)*2);

  this->numProps_ = 0;
  this->propsAllocated_ = 1;
  this->names_ = (char**)malloc(sizeof(char*));
  this->values_ = (char**)malloc(sizeof(char*));
}


void lefiNonDefault::Destroy() { 
  this->lefiNonDefault::clear();

  free(this->name_);

  free((char*)(this->layerName_));
  free((char*)(this->width_));
  free((char*)(this->spacing_));
  free((char*)(this->wireExtension_));
  free(this->hasWidth_);
  free(this->hasSpacing_);
  free(this->hasWireExtension_);

  free((char*)(this->viaRules_));

  free((char*)(this->spacingRules_));
}


lefiNonDefault::~lefiNonDefault() { 
  this->lefiNonDefault::Destroy();
}


void lefiNonDefault::clear() { 
  int i;
  lefiSpacing* sr;
  lefiVia* vr;

  for (i = 0; i < this->numProps_; i++) {
    free(this->names_[i]);
    free(this->values_[i]);
  }
  this->numProps_ = 0;
  for (i = 0; i < this->numLayers_; i++) {
    free(this->layerName_[i]);
    this->layerName_[i] = 0;
  }
  this->numLayers_ = 0;
  for (i = 0; i < this->numVias_; i++) {
    vr = this->viaRules_[i];
    vr->lefiVia::Destroy();
    free((char*)(this->viaRules_[i]));
    this->viaRules_[i] = 0;
  }
  this->numVias_ = 0;
  for (i = 0; i < this->numSpacing_; i++) {
    sr = this->spacingRules_[i];
    sr->lefiSpacing::Destroy();
    free((char*)(this->spacingRules_[i]));
    this->spacingRules_[i] = 0;
  }
  this->numSpacing_ = 0;
}


void lefiNonDefault::addViaRule(lefiVia* v) {
	if (this->numVias_ == this->allocatedVias_) {
		int i;
		lefiVia** nv;
		this->allocatedVias_ *= 2;
		nv = (lefiVia**)malloc(sizeof(lefiVia*)* this->allocatedVias_);
		for (i = 0; i < this->numVias_; i++) {
			nv[i] = this->viaRules_[i];
		}
		free((char*)(this->viaRules_));
		this->viaRules_ = nv;
	}
	this->viaRules_[this->numVias_++] = v->lefiVia::clone();
}


void lefiNonDefault::addSpacingRule(lefiSpacing* s) {
	if (this->numSpacing_ == this->allocatedSpacing_) {
		int i;
		lefiSpacing** ns;
		this->allocatedSpacing_ *= 2;
		ns = (lefiSpacing**)malloc(sizeof(lefiSpacing*)*
                        this->allocatedSpacing_);
		for (i = 0; i < this->numSpacing_; i++) {
			ns[i] = this->spacingRules_[i];
		}
		free((char*)(this->spacingRules_));
		this->spacingRules_ = ns;
	}
	this->spacingRules_[this->numSpacing_++] = s->lefiSpacing::clone();
}


int lefiNonDefaultViaCbk(lefrCallbackType_e t, lefiVia* v,
       lefiUserData ud) {
   lefiNonDefault* nd = (lefiNonDefault*)ud;
   
   if (t != lefrViaCbkType)
     lefiError("Bad nondefaultspacing callback");

   // We got a via in a nondefault rule so add it to
   // our internal list.
   nd->lefiNonDefault::addViaRule(v);
   return 0;
}


int lefiNonDefaultSpacingCbk(lefrCallbackType_e t,
       lefiSpacing* s, lefiUserData ud) {
   lefiNonDefault* nd = (lefiNonDefault*)ud;

   if (t != lefrSpacingCbkType)
     lefiError("Bad nondefaultspacing callback");

   // We got a spacing in a nondefault rule so add it to
   // our internal list.
   nd->lefiNonDefault::addSpacingRule(s);
   return 0;
}


void lefiNonDefault::setName(const char* name) { 
  int len = strlen(name);
  this->lefiNonDefault::clear();

  // Use our callback functions because a via and spacing
  // rule is really part of the non default section.
  // this->oldViaCbk_ = (void*)(lefrViaCbk);
  // this->oldSpacingCbk_ = (void*)(lefrSpacingCbk);
  // this->oldSpacingBeginCbk_ = (void*)(lefrSpacingBeginCbk);
  // this->oldSpacingEndCbk_ = (void*)(lefrSpacingEndCbk);
  this->oldUserData_ = lefrGetUserData();
  oldViaCbk = lefrViaCbk;
  oldSpacingCbk = lefrSpacingCbk;
  oldSpacingBeginCbk = lefrSpacingBeginCbk;
  oldSpacingEndCbk = lefrSpacingEndCbk;
  lefrViaCbk = lefiNonDefaultViaCbk;
  lefrSpacingCbk = lefiNonDefaultSpacingCbk;
  lefrSpacingBeginCbk = 0;
  lefrSpacingEndCbk = 0;
  lefrSetUserData((lefiUserData)this);

  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char*)malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
}


void lefiNonDefault::addLayer(const char* name) { 
  int len = strlen(name) + 1;
  if (this->numLayers_ == this->layersAllocated_) {
    int i;
    char** newl;
    double* neww;
    double* news;
    double* newe;
    char* newhw;
    char* newhs;
    char* newhe;
    this->layersAllocated_ *= 2;
    newl = (char**)malloc(sizeof(char*)*
		  this->layersAllocated_);
    newe = (double*)malloc(sizeof(double)*
		  this->layersAllocated_);
    neww = (double*)malloc(sizeof(double)*
		  this->layersAllocated_);
    news = (double*)malloc(sizeof(double)*
		  this->layersAllocated_);
    newhe = (char*)malloc(sizeof(char)*
		  this->layersAllocated_);
    newhw = (char*)malloc(sizeof(char)*
		  this->layersAllocated_);
    newhs = (char*)malloc(sizeof(char)*
		  this->layersAllocated_);
    for (i = 0; i < this->numLayers_; i++) {
      newl[i] = this->layerName_[i];
      neww[i] = this->width_[i];
      news[i] = this->spacing_[i];
      newe[i] = this->wireExtension_[i];
      newhe[i] = this->hasWireExtension_[i];
      newhw[i] = this->hasWidth_[i];
      newhs[i] = this->hasSpacing_[i];
    }
    free((char*)(this->layerName_));
    free((char*)(this->width_));
    free((char*)(this->spacing_));
    free((char*)(this->wireExtension_));
    free((char*)(this->hasWireExtension_));
    free((char*)(this->hasWidth_));
    free((char*)(this->hasSpacing_));
    this->layerName_ = newl;
    this->width_ = neww;
    this->spacing_ = news;
    this->wireExtension_ = newe;
    this->hasWidth_ = newhw;
    this->hasSpacing_ = newhs;
    this->hasWireExtension_ = newhe;
  }
  this->layerName_[this->numLayers_] = (char*)malloc(len);
  strcpy(this->layerName_[this->numLayers_], CASE(name));
  this->width_[this->numLayers_] = 0.0;
  this->spacing_[this->numLayers_] = 0.0;
  this->wireExtension_[this->numLayers_] = 0.0;
  this->hasWidth_[this->numLayers_] = '\0';
  this->hasSpacing_[this->numLayers_] = '\0';
  this->hasWireExtension_[this->numLayers_] = '\0';
  this->numLayers_ += 1;
}


void lefiNonDefault::addWidth(double num) { 
  this->width_[this->numLayers_-1] = num;
  this->hasWidth_[this->numLayers_-1] = 1;
}


void lefiNonDefault::addSpacing(double num) { 
  this->spacing_[this->numLayers_-1] = num;
  this->hasSpacing_[this->numLayers_-1] = 1;
}


void lefiNonDefault::addWireExtension(double num) { 
  this->wireExtension_[this->numLayers_-1] = num;
  this->hasWireExtension_[this->numLayers_-1] = 1;
}


void lefiNonDefault::end() { 
  // Return the callbacks to their normal state.
  // lefrSetViaCbk((lefrViaCbkFnType)(this->oldViaCbk_));
  // lefrSetSpacingCbk((lefrSpacingCbkFnType)(this->oldSpacingCbk_));
  // lefrSetSpacingBeginCbk((lefrVoidCbkFnType)(this->oldSpacingBeginCbk_));
  // lefrSetSpacingEndCbk((lefrVoidCbkFnType)(this->oldSpacingEndCbk_));
  // lefrSetUserData(this->oldUserData_);
  lefrSetViaCbk(oldViaCbk);
  lefrSetSpacingCbk(oldSpacingCbk);
  lefrSetSpacingBeginCbk(oldSpacingBeginCbk);
  lefrSetSpacingEndCbk(oldSpacingEndCbk);
  lefrSetUserData(this->oldUserData_);
}



int lefiNonDefault::numLayers() const { 
  return this->numLayers_;
}



const char* lefiNonDefault::layerName(int index) const { 
  if (index < 0 || index >= this->numLayers_) {
    lefiError("Bad index in non default");
    return 0;
  }
  return this->layerName_[index];
}



int lefiNonDefault::hasLayerWidth(int index) const { 
  if (index < 0 || index >= this->numLayers_) {
    lefiError("Bad index in non default");
    return 0;
  }
  return this->hasWidth_[index];
}



double lefiNonDefault::layerWidth(int index) const { 
  if (index < 0 || index >= this->numLayers_) {
    lefiError("Bad index in non default");
    return 0.0;
  }
  return this->width_[index];
}



int lefiNonDefault::hasLayerWireExtension(int index) const { 
  if (index < 0 || index >= this->numLayers_) {
    lefiError("Bad index in non default");
    return 0;
  }
  return this->hasWireExtension_[index];
}


int lefiNonDefault::hasLayerSpacing(int index) const { 
  if (index < 0 || index >= this->numLayers_) {
    lefiError("Bad index in non default");
    return 0;
  }
  return this->hasSpacing_[index];
}



double lefiNonDefault::layerWireExtension(int index) const { 
  if (index < 0 || index >= this->numLayers_) {
    lefiError("Bad index in non default");
    return 0.0;
  }
  return this->wireExtension_[index];
}


double lefiNonDefault::layerSpacing(int index) const { 
  if (index < 0 || index >= this->numLayers_) {
    lefiError("Bad index in non default");
    return 0.0;
  }
  return this->spacing_[index];
}



int lefiNonDefault::numVias() const { 
  return this->numVias_;
}



lefiVia* lefiNonDefault::viaRule(int index) const { 
  if (index < 0 || index >= this->numVias_) {
    lefiError("Bad index in non default");
    return 0;
  }
  return this->viaRules_[index];
}



int lefiNonDefault::numSpacingRules() const { 
  return this->numSpacing_;
}



lefiSpacing* lefiNonDefault::spacingRule(int index) const { 
  if (index < 0 || index >= this->numVias_) {
    lefiError("Bad index in non default");
    return 0;
  }
  return this->spacingRules_[index];
}


const char* lefiNonDefault::name() const {
	return this->name_;
}


void lefiNonDefault::print(FILE* f) { 
  int i;
  lefiSpacing* s;
  lefiVia* v;

  fprintf(f, "Nondefault rule %s\n",
    this->lefiNonDefault::name());
  fprintf(f, "%d layers   %d vias   %d spacing rules\n",
    this->lefiNonDefault::numLayers(),
    this->lefiNonDefault::numVias(),
    this->lefiNonDefault::numSpacingRules());

  for (i = 0; i < this->lefiNonDefault::numLayers(); i++) {
    fprintf(f, "  Layer %s", this->lefiNonDefault::layerName(i));
    if (this->lefiNonDefault::hasLayerWidth(i))
      fprintf(f, " width %g", this->lefiNonDefault::layerWidth(i));
    if (this->lefiNonDefault::hasLayerSpacing(i))
      fprintf(f, " spacing %g", this->lefiNonDefault::layerSpacing(i));
    if (this->lefiNonDefault::hasLayerWireExtension(i))
      fprintf(f, " wireextension %g",
      this->lefiNonDefault::layerWireExtension(i));
    fprintf(f, "\n");
  }

  for (i = 0; i < this->lefiNonDefault::numVias(); i++) {
    v = this->lefiNonDefault::viaRule(i);
    v->lefiVia::print(f);
    }

  for (i = 0; i < this->lefiNonDefault::numSpacingRules(); i++) {
    s = this->lefiNonDefault::spacingRule(i);
    s->lefiSpacing::print(f);
    }
}


int lefiNonDefault::numProps() const {
  return this->numProps_;
}


void lefiNonDefault::addProp(const char* name, const char* value) {
  int len = strlen(name) + 1;
  if (this->numProps_ == this->propsAllocated_) {
    int i;
    int max;
    int lim = this->numProps_;
    char** nn;
    char** nv;

    max = this->propsAllocated_ *= 2;
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


const char* lefiNonDefault::propName(int index) const {
  return this->names_[index];
}


const char* lefiNonDefault::propValue(int index) const {
  return this->values_[index];
}


