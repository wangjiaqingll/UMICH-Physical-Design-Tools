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

#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "defiNet.hpp"
#include "defiPath.hpp"
#include "defiDebug.hpp"


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiSubnet
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


defiSubnet::defiSubnet() {
  this->defiSubnet::Init();
}


void defiSubnet::Init() {
  this->name_ = 0;
  this->defiSubnet::bumpName(16);

  this->instances_ = 0;
  this->pins_ = 0;
  this->musts_ = 0;
  this->synthesized_ = 0;
  this->numPins_ = 0;
  this->defiSubnet::bumpPins(16);

  this->paths_ = 0;
  this->numPaths_ = 0;
  this->pathsAllocated_ = 0;
  this->nonDefaultRule_ = 0;

  this->defiSubnet::clear();
}


void defiSubnet::Destroy() {
  this->defiSubnet::clear();
  free(this->name_);
  free((char*)(this->instances_));
  free((char*)(this->pins_));
  free(this->musts_);
  free(this->synthesized_);
}


defiSubnet::~defiSubnet() {
  this->defiSubnet::Destroy();
}


void defiSubnet::setName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) this->defiSubnet::bumpName(len);
  strcpy(this->name_, DEFCASE(name));
}


void defiSubnet::setNonDefault(const char* name) {
  int len = strlen(name) + 1;
  this->nonDefaultRule_ = (char*)malloc(len);
  strcpy(this->nonDefaultRule_, DEFCASE(name));
}

 
void defiSubnet::addMustPin(const char* instance, const char* pin, int syn) {
  this->defiSubnet::addPin(instance, pin, syn);
  this->musts_[this->numPins_ - 1] = 1;
}


void defiSubnet::addPin(const char* instance, const char* pin, int syn) {
  int len;

  if (this->numPins_ == this->pinsAllocated_)
    this->defiSubnet::bumpPins(this->pinsAllocated_ * 2);

  len = strlen(instance)+ 1;
  this->instances_[this->numPins_] = (char*)malloc(len);
  strcpy(this->instances_[this->numPins_], DEFCASE(instance));

  len = strlen(pin)+ 1;
  this->pins_[this->numPins_] = (char*)malloc(len);
  strcpy(this->pins_[this->numPins_], DEFCASE(pin));

  this->musts_[this->numPins_] = 0;
  this->synthesized_[this->numPins_] = syn;

  (this->numPins_)++;
}


void defiSubnet::setType(const char* typ) {
  if (*typ == 'F') {
    this->isFixed_ = 1;
  } else if (*typ == 'C') {
    this->isCover_ = 1;
  } else if (*typ == 'R') {
    this->isRouted_ = 1;
  } else {
    // Silently do nothing with bad input.
  }

}


void defiSubnet::addPath(defiPath* p) {
  if (this->numPaths_ >= this->pathsAllocated_)
    this->defiSubnet::bumpPaths(
      this->pathsAllocated_ ? this->pathsAllocated_*2 : 8);
  this->paths_[this->numPaths_++] = p;
}


const char* defiSubnet::name() const {
  return this->name_;
}


int defiSubnet::hasNonDefaultRule() const {
  return this->nonDefaultRule_ ? 1 : 0;
}


const char* defiSubnet::nonDefaultRule() const {
  return this->nonDefaultRule_;
}


int defiSubnet::numConnections() {
  return this->numPins_;
}


const char* defiSubnet::instance(int index) {
  if (index >= 0 && index < this->numPins_)
    return this->instances_[index];
  return 0;
}


const char* defiSubnet::pin(int index) {
  if (index >= 0 && index < this->numPins_)
    return this->pins_[index];
  return 0;
}


int defiSubnet::pinIsMustJoin(int index) {
  if (index >= 0 && index < this->numPins_)
    return (int)(this->musts_[index]);
  return 0;
}


int defiSubnet::pinIsSynthesized(int index) {
  if (index >= 0 && index < this->numPins_)
    return (int)(this->synthesized_[index]);
  return 0;
}


int defiSubnet::isFixed() const {
  return (int)(this->isFixed_);
}


int defiSubnet::isRouted() const {
  return (int)(this->isRouted_);
}


int defiSubnet::isCover() const {
  return (int)(this->isCover_);
}


void defiSubnet::bumpName(int size) {
  if (this->name_) free(this->name_);
  this->name_ = (char*)malloc(size);
  this->nameSize_ = size;
  this->name_[0] = '\0';
}


void defiSubnet::bumpPins(int size) {
  char** newInstances = (char**)malloc(sizeof(char*)*size);
  char** newPins = (char**)malloc(sizeof(char*)*size);
  char* newMusts = (char*)malloc(size);
  char* newSyn = (char*)malloc(size);
  int i;

  if (this->instances_) {
    for (i = 0; i < this->pinsAllocated_; i++) {
      newInstances[i] = this->instances_[i];
      newPins[i] = this->pins_[i];
      newMusts[i] = this->musts_[i];
      newSyn[i] = this->synthesized_[i];
    }
    free((char*)(this->instances_));
    free((char*)(this->pins_));
    free(this->musts_);
    free(this->synthesized_);
  }

  this->instances_ = newInstances;
  this->pins_ = newPins;
  this->musts_ = newMusts;
  this->synthesized_ = newSyn;
  this->pinsAllocated_ = size;
}


void defiSubnet::clear() {
  int i;

  this->isFixed_ = 0;
  this->isRouted_ = 0;
  this->isCover_ = 0;
  this->name_[0] = '\0';

  for (i = 0; i < this->numPins_; i++) {
    free(this->instances_[i]);
    free(this->pins_[i]);
    this->instances_[i] = 0;
    this->pins_[i] = 0;
    this->musts_[i] = 0;
    this->synthesized_[i] = 0;
  }
  this->numPins_ = 0;

  if (this->paths_) {
    for (i = 0; i < this->numPaths_; i++) {
      this->paths_[i]->defiPath::Destroy();
      free((char*)(this->paths_[i]));
    }
    free((char*)(this->paths_));
    this->paths_ = 0;
    this->numPaths_ = 0;
    this->pathsAllocated_ = 0;
  }

  if (this->nonDefaultRule_) {
    free(this->nonDefaultRule_);
    this->nonDefaultRule_ = 0;
  }
}


void defiSubnet::print(FILE* f) {
  int i;
  defiPath* p;

  fprintf(f, " subnet '%s'", this->name_);
  if (this->defiSubnet::isFixed())
    fprintf(f, " FIXED");
  if (this->defiSubnet::isRouted())
    fprintf(f, " ROUTED");
  if (this->defiSubnet::isCover())
    fprintf(f, " COVER");
  fprintf(f, "\n");

  if (this->defiSubnet::hasNonDefaultRule())
    fprintf(f, "  nondefault rule %s\n",
    this->defiSubnet::nonDefaultRule());

  if (this->defiSubnet::numConnections()) {
    fprintf(f, "  Pins:\n");
    for (i = 0; i < this->defiSubnet::numConnections(); i++) {
    fprintf(f, "   '%s' '%s'%s%s\n", 
      this->defiSubnet::instance(i),
      this->defiSubnet::pin(i),
      this->defiSubnet::pinIsMustJoin(i) ? " MUSTJOIN" : "",
      this->defiSubnet::pinIsSynthesized(i) ? " SYNTHESIZED" : "");
    }
  }

  if (this->defiSubnet::numPaths()) {
    fprintf(f, "  Paths:\n");
    for (i = 0; i < this->defiSubnet::numPaths(); i++) {
      p = this->defiSubnet::path(i);
      p->defiPath::print(f);
    }
  }

}


defiPath* defiSubnet::path(int index) {
  if (index >= 0 && index < this->numPaths_)
    return this->paths_[index];
  return 0;
}


int defiSubnet::numPaths() const {
  return this->numPaths_;
}


void defiSubnet::bumpPaths(int size) {
  int i;
  defiPath** newPaths = (defiPath**)malloc(sizeof(defiPath*)*size);

  for (i = 0; i < this->numPaths_; i++)
    newPaths[i] = this->paths_[i];

  this->pathsAllocated_ = size;
  free((char*)(this->paths_));
  this->paths_ = newPaths;
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiVpin
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


defiVpin::defiVpin() {
}


void defiVpin::Init(const char* name) {
  int len = strlen(name) + 1;
  this->name_ = (char*)malloc(len);
  strcpy(this->name_, DEFCASE(name));
  this->orient_ = -1;
  this->status_ = ' ';
  this->layer_ = 0;
}


defiVpin::~defiVpin() {
}


void defiVpin::Destroy() {
  free(this->name_);
  if (this->layer_) free(this->layer_);
}


void defiVpin::setBounds(int xl, int yl, int xh, int yh) {
  this->xl_ = xl;
  this->yl_ = yl;
  this->xh_ = xh;
  this->yh_ = yh;
}


void defiVpin::setLayer(const char* lay) {
  int len = strlen(lay)+1;
  this->layer_ = (char*)malloc(len);
  strcpy(this->layer_, lay);
}


void defiVpin::setOrient(int orient) {
  this->orient_ = orient;
}


void defiVpin::setLoc(int x, int y) {
  this->xLoc_ = x;
  this->yLoc_ = y;
}


void defiVpin::setStatus(char st) {
  this->status_ = st;
}


int defiVpin::xl() const  {
  return this->xl_;
}


int defiVpin::yl() const  {
  return this->yl_;
}


int defiVpin::xh() const  {
  return this->xh_;
}


int defiVpin::yh() const  {
  return this->yh_;
}


char defiVpin::status() const {
  return this->status_;
}


int defiVpin::orient() const  {
  return this->orient_;
}


int defiVpin::xLoc() const {
  return this->xLoc_;
}


int defiVpin::yLoc() const {
  return this->yLoc_;
}


const char* defiVpin::name() const {
  return this->name_;
}


const char* defiVpin::layer() const {
  return this->layer_;
}



////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiShield
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


defiShield::defiShield() {
}


void defiShield::Init(const char* name) {
  int len = strlen(name) + 1;
  this->name_ = (char*)malloc(len);
  strcpy(this->name_, DEFCASE(name));
  this->numPaths_ = 0;
  this->pathsAllocated_ = 0;
  this->paths_ = 0;
}


void defiShield::Destroy() {
  this->defiShield::clear();
}


defiShield::~defiShield() {
  this->defiShield::Destroy();
}


void defiShield::addPath(defiPath* p) {
  if (this->numPaths_ >= this->pathsAllocated_)
    this->defiShield::bumpPaths(
      this->pathsAllocated_ ? this->pathsAllocated_*2 : 8);
  this->paths_[this->numPaths_++] = p;
}


void defiShield::clear() {
  defiPath* p;
  int       i;

  if (this->name_) free(this->name_);
  if (this->paths_) {
    for (i = 0; i < this->numPaths_; i++) {
      p = this->paths_[i];
      p->defiPath::Destroy();
      free((char*)p);
    }
    free((char*)(this->paths_));
    this->paths_ = 0;
    this->numPaths_ = 0;
    this->pathsAllocated_ = 0;
  }
}


void defiShield::bumpPaths(int size) {
  int i;
  defiPath** newPaths = (defiPath**)malloc(sizeof(defiPath)*size);

  for (i = 0; i < this->numPaths_; i++)
    newPaths[i] = this->paths_[i];

  this->pathsAllocated_ = size;
  free((char*)(this->paths_));
  this->paths_ = newPaths;
}


int defiShield::numPaths() const {
  return this->numPaths_;
}


const char* defiShield::shieldName() const {
  return this->name_;
}

defiPath* defiShield::path(int index) {
  if (index >= 0 && index < this->numPaths_)
    return this->paths_[index];
  return 0;
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiNet
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


defiNet::defiNet() {
  this->defiNet::Init();
}


void defiNet::Init() {
  this->name_ = 0;
  this->instances_ = 0;
  this->numPins_ = 0;
  this->numProps_ = 0;
  this->propNames_ = 0;
  this->subnets_ = 0;
  this->source_ = 0;
  this->pattern_ = 0;
  this->style_ = 0;
  this->original_ = 0;
  this->use_ = 0;
  this->nonDefaultRule_ = 0;
  this->paths_ = 0;
  this->numPaths_ = 0;
  this->pathsAllocated_ = 0;

  this->numWidths_ = 0;
  this->widthsAllocated_ = 0;
  this->wlayers_ = 0;
  this->wdist_ = 0;

  this->numSpacing_ = 0;
  this->spacingAllocated_ = 0;
  this->slayers_ = 0;
  this->sdist_ = 0;
  this->sleft_ = 0;
  this->sright_ = 0;

  this->vpins_ = 0;
  this->numVpins_ = 0;
  this->vpinsAllocated_ = 0;

  this->shields_ = 0;
  this->numShields_ = 0;
  this->shieldsAllocated_ = 0;
  this->numShieldNet_ = 0;
  this->shieldNetsAllocated_ = 0;

  this->defiNet::bumpProps(2);
  this->defiNet::bumpName(16);
  this->defiNet::bumpPins(16);
  this->defiNet::bumpSubnets(2);

  this->defiNet::clear();
}


void defiNet::Destroy() {
  this->defiNet::clear();
  free(this->name_);
  free((char*)(this->instances_));
  free((char*)(this->pins_));
  free(this->musts_);
  free(this->synthesized_);
  free((char*)(this->propNames_));
  free((char*)(this->propValues_));
  free((char*)(this->subnets_));
  if (this->source_) free(this->source_);
  if (this->pattern_) free(this->pattern_);
  if (this->style_) free(this->style_);
  if (this->shieldNet_) free(this->shieldNet_);
  if (this->original_) free(this->original_);
  if (this->use_) free(this->use_);
  if (this->nonDefaultRule_) free(this->nonDefaultRule_);
  if (this->wlayers_) free((char*)(this->wlayers_));
  if (this->slayers_) free((char*)(this->slayers_));
  if (this->sdist_) free((char*)(this->sdist_));
  if (this->wdist_) free((char*)(this->wdist_));
  if (this->sleft_) free((char*)(this->sleft_));
  if (this->sright_) free((char*)(this->sright_));
}


defiNet::~defiNet() {
  this->defiNet::Destroy();
}


void defiNet::setName(const char* name) {
  int len = strlen(name) + 1;
  this->defiNet::clear();
  if (len > this->nameSize_) this->defiNet::bumpName(len);
  strcpy(this->name_, DEFCASE(name));
}


void defiNet::addMustPin(const char* instance, const char* pin, int syn) {
  this->defiNet::addPin(instance, pin, syn);
  this->musts_[this->numPins_ - 1] = 1;
}


void defiNet::addPin(const char* instance, const char* pin, int syn) {
  int len;

  if (this->numPins_ == this->pinsAllocated_)
    this->defiNet::bumpPins(this->pinsAllocated_ * 2);

  len = strlen(instance)+ 1;
  this->instances_[this->numPins_] = (char*)malloc(len);
  strcpy(this->instances_[this->numPins_], DEFCASE(instance));

  len = strlen(pin)+ 1;
  this->pins_[this->numPins_] = (char*)malloc(len);
  strcpy(this->pins_[this->numPins_], DEFCASE(pin));

  this->musts_[this->numPins_] = 0;
  this->synthesized_[this->numPins_] = syn;

  (this->numPins_)++;
}


void defiNet::setWeight(int w) {
  this->hasWeight_ = 1;
  this->weight_ = w;
}


void defiNet::addProp(const char* name, const char* value) {
  int len;

  if (this->numProps_ == this->propsAllocated_)
    this->defiNet::bumpProps(this->propsAllocated_ * 2);

  len = strlen(name)+ 1;
  this->propNames_[this->numProps_] = (char*)malloc(len);
  strcpy(this->propNames_[this->numProps_], DEFCASE(name));

  len = strlen(value)+ 1;
  this->propValues_[this->numProps_] = (char*)malloc(len);
  strcpy(this->propValues_[this->numProps_], DEFCASE(value));

  (this->numProps_)++;
}


void defiNet::addSubnet(defiSubnet* subnet) {

  if (this->numSubnets_ >= this->subnetsAllocated_)
    this->defiNet::bumpSubnets(this->subnetsAllocated_ * 2);

  this->subnets_[this->numSubnets_++] = subnet;
}


void defiNet::setType(const char* typ) {
  if (*typ == 'F') {
    this->isFixed_ = 1;
  } else if (*typ == 'C') {
    this->isCover_ = 1;
  } else if (*typ == 'R') {
    this->isRouted_ = 1;
  } else {
    // Silently do nothing with bad input.
  }
}


void defiNet::addPath(defiPath* p) {
  if (this->numPaths_ >= this->pathsAllocated_)
    this->defiNet::bumpPaths(
      this->pathsAllocated_ ? this->pathsAllocated_*2 : 8);
  this->paths_[this->numPaths_++] = p;
}


void defiNet::addShield(const char* name) {
  defiShield* shield;
  if (this->numShields_ == this->shieldsAllocated_) {
    defiShield** array;
    int i;
    this->shieldsAllocated_ = this->shieldsAllocated_ ?
	                      this->shieldsAllocated_ * 2 : 2 ;
    array = (defiShield**)malloc(sizeof(defiShield*)*this->shieldsAllocated_);
    for (i = 0; i < this->numShields_; i++)
      array[i] = this->shields_[i];
    if (this->shields_) free((char*)(this->shields_));
    this->shields_ = array;
  }
  shield = this->shields_[this->numShields_] = 
           (defiShield*)malloc(sizeof(defiShield));
  this->numShields_ += 1;
  shield->defiShield::Init(name);
}


void defiNet::addShieldPath(defiPath* p) {
  this->shields_[this->numShields_-1]->defiShield::addPath(p);
}


void defiNet::addShieldNet(const char* name) {
  int len;

  if (this->numShieldNet_ == this->shieldNetsAllocated_) {
     if (this->shieldNetsAllocated_ == 0)
        this->defiNet::bumpShieldNets(2);
     else
        this->defiNet::bumpShieldNets(this->shieldNetsAllocated_ * 2);

  }
 
  len = strlen(name) + 1;
  this->shieldNet_[this->numShieldNet_] = (char*)malloc(len);
  strcpy(this->shieldNet_[this->numShieldNet_], DEFCASE(name));
  (this->numShieldNet_)++;
}


const char* defiNet::name() const {
  return this->name_;
}


int defiNet::weight() const {
  return this->weight_;
}


int defiNet::numProps() const {
  return this->numProps_;
}


int defiNet::hasProps() const {
  return this->numProps_ ? 1 : 0 ;
}


int defiNet::hasWeight() const {
  return (int)(this->hasWeight_);
}


const char* defiNet::propName(int index) const {
  if (index >= 0 &&  index < this->numProps_)
    return this->propNames_[index];
  return 0;
}


const char* defiNet::propValue(int index) const {
  if (index >= 0 &&  index < this->numProps_)
    return this->propValues_[index];
  return 0;
}


int defiNet::numConnections() const {
  return this->numPins_;
}


int defiNet::numShieldNets() const {
  return this->numShieldNet_;
}


const char* defiNet::instance(int index) const {
  if (index >= 0 &&  index < this->numPins_)
    return this->instances_[index];
  return 0;
}


const char* defiNet::pin(int index) const {
  if (index >= 0 &&  index < this->numPins_)
    return this->pins_[index];
  return 0;
}


int defiNet::pinIsMustJoin(int index) const {
  if (index >= 0 &&  index < this->numPins_)
    return (int)(this->musts_[index]);
  return 0;
}


int defiNet::pinIsSynthesized(int index) const {
  if (index >= 0 &&  index < this->numPins_)
    return (int)(this->synthesized_[index]);
  return 0;
}


int defiNet::hasSubnets() const {
  return this->numSubnets_ ? 1 : 0 ;
}


int defiNet::numSubnets() const {
  return this->numSubnets_;
}


defiSubnet* defiNet::subnet(int index) {
  if (index >= 0 &&  index < this->numSubnets_)
    return this->subnets_[index];
  return 0;
}


int defiNet::isFixed() const {
  return (int)(this->isFixed_);
}


int defiNet::isRouted() const {
  return (int)(this->isRouted_);
}


int defiNet::isCover() const {
  return (int)(this->isCover_);
}


void defiNet::print(FILE* f) {
  int i;
  defiPath* p;
  defiSubnet* s;
  defiVpin* vp;

  fprintf(f, "Net '%s'", this->name_);
  if (this->defiNet::isFixed())
    fprintf(f, "  FIXED");
  if (this->defiNet::isRouted())
    fprintf(f, " ROUTED");
  if (this->defiNet::isCover())
    fprintf(f, " COVER");
  fprintf(f, "\n");

  if (this->defiNet::hasWeight())
    fprintf(f, "  weight=%d\n", this->defiNet::weight());

  if (this->defiNet::hasCap())
    fprintf(f, "  cap=%lf\n", this->defiNet::cap());

  if (this->defiNet::hasSource())
    fprintf(f, "  source='%s'\n", this->defiNet::source());

  if (this->defiNet::hasPattern())
    fprintf(f, "  pattern='%s'\n", this->defiNet::pattern());

  if (this->defiNet::hasOriginal())
    fprintf(f, "  original='%s'\n", this->defiNet::original());

  if (this->defiNet::hasUse())
    fprintf(f, "  use='%s'\n", this->defiNet::use());

  if (this->defiNet::hasNonDefaultRule())
    fprintf(f, "  nonDefaultRule='%s'\n", this->defiNet::nonDefaultRule());

  if (this->defiNet::hasXTalk())
    fprintf(f, "  xtalk=%d\n", this->defiNet::XTalk());

  if (this->defiNet::hasStyle())
    fprintf(f, "  style='%s'\n", this->defiNet::style());

  if (this->defiNet::hasProps()) {
    fprintf(f, " Props:\n");
    for (i = 0; i < this->defiNet::numProps(); i++) {
      fprintf(f, "  '%s' '%s'\n", this->defiNet::propName(i),
      this->defiNet::propValue(i));
    }
  }

  if (this->defiNet::numConnections()) {
    fprintf(f, " Pins:\n");
    for (i = 0; i < this->defiNet::numConnections(); i++) {
    fprintf(f, "  '%s' '%s'%s%s\n", 
      this->defiNet::instance(i),
      this->defiNet::pin(i),
      this->defiNet::pinIsMustJoin(i) ? " MUSTJOIN" : "",
      this->defiNet::pinIsSynthesized(i) ? " SYNTHESIZED" : "");
    }
  }
 
  for (i = 0; i < this->numVpins_; i++) {
    vp = this->defiNet::vpin(i);
    fprintf(f,
    "  VPIN %s status '%c' layer %s %d,%d orient %d bounds %d,%d to %d,%d\n",
    vp->defiVpin::name(),
    vp->defiVpin::status(),
    vp->defiVpin::layer() ? vp->defiVpin::layer() : "",
    vp->defiVpin::xLoc(),
    vp->defiVpin::yLoc(),
    vp->defiVpin::orient(),
    vp->defiVpin::xl(),
    vp->defiVpin::yl(),
    vp->defiVpin::xh(),
    vp->defiVpin::yh());
  }

  if (this->defiNet::numPaths()) {
    fprintf(f, "  Paths:\n");
    for (i = 0; i < this->defiNet::numPaths(); i++) {
      p = this->defiNet::path(i);
      p->defiPath::print(f);
    }
  }

  if (this->defiNet::hasSubnets()) {
    fprintf(f, " Subnets:\n");
    for (i = 0; i < this->defiNet::numSubnets(); i++) {
      s = this->defiNet::subnet(i);
      s->defiSubnet::print(f);
    }
  }

}


void defiNet::bumpName(int size) {
  if (this->name_) free(this->name_);
  this->name_ = (char*)malloc(size);
  this->nameSize_ = size;
  this->name_[0] = '\0';
}


void defiNet::bumpPins(int size) {
  char** newInstances = (char**)malloc(sizeof(char*)*size);
  char** newPins = (char**)malloc(sizeof(char*)*size);
  char* newMusts = (char*)malloc(size);
  char* newSyn = (char*)malloc(size);
  int i;

  if (this->instances_) {
    for (i = 0; i < this->pinsAllocated_; i++) {
      newInstances[i] = this->instances_[i];
      newPins[i] = this->pins_[i];
      newMusts[i] = this->musts_[i];
      newSyn[i] = this->synthesized_[i];
    }
    free((char*)(this->instances_));
    free((char*)(this->pins_));
    free(this->musts_);
    free(this->synthesized_);
  }

  this->instances_ = newInstances;
  this->pins_ = newPins;
  this->musts_ = newMusts;
  this->synthesized_ = newSyn;
  this->pinsAllocated_ = size;
}


void defiNet::bumpProps(int size) {
  char** newNames = (char**)malloc(sizeof(char*)*size);
  char** newValues = (char**)malloc(sizeof(char*)*size);
  int i;

  if (this->propNames_) {
    for (i = 0; i < this->numProps_; i++) {
      newNames[i] = this->propNames_[i];
      newValues[i] = this->propValues_[i];
    }
    free((char*)(this->propNames_));
    free((char*)(this->propValues_));
  }

  this->propNames_ = newNames;
  this->propValues_ = newValues;
  this->propsAllocated_ = size;
}


void defiNet::bumpSubnets(int size) {
  defiSubnet** newSubnets = (defiSubnet**)malloc(sizeof(defiSubnet*)*size);
  int i;
  if (this->subnets_) {
    for (i = 0; i < this->numSubnets_; i++) {
      newSubnets[i] = this->subnets_[i];
    }
    free((char*)(this->subnets_));
  }

  this->subnets_ = newSubnets;
  this->subnetsAllocated_ = size;
}


void defiNet::clear() {
  int i;
  defiSubnet* s;
  defiPath* p;
  defiVpin* vp;

  this->isFixed_ = 0;
  this->isRouted_ = 0;
  this->isCover_ = 0;
  this->hasWeight_ = 0;
  this->hasCap_ = 0;
  this->hasVoltage_ = 0;
  this->xTalk_ = -1;

  for (i = 0; i < this->numVpins_; i++) {
    vp = this->vpins_[i];
    vp->defiVpin::Destroy();
    free((char*)vp);
  }
  this->numVpins_ = 0;

  for (i = 0; i < this->numProps_; i++) {
    free(this->propNames_[i]);
    free(this->propValues_[i]);
    this->propNames_[i] = 0;
    this->propValues_[i] = 0;
  }
  this->numProps_ = 0;

  for (i = 0; i < this->numPins_; i++) {
    free(this->instances_[i]);
    free(this->pins_[i]);
    this->instances_[i] = 0;
    this->pins_[i] = 0;
    this->musts_[i] = 0;
    this->synthesized_[i] = 0;
  }
  this->numPins_ = 0;

  for (i = 0; i < this->numSubnets_; i++) {
    s = this->subnets_[i];
    s->defiSubnet::Destroy();
    free((char*)(this->subnets_[i]));
    this->subnets_[i] = 0;
  }
  this->numSubnets_ = 0;

  if (this->name_)
     this->name_[0] = '\0';

  if (this->paths_) {
    for (i = 0; i < this->numPaths_; i++) {
      p = this->paths_[i];
      p->defiPath::Destroy();
      free((char*)p);
    }
    free((char*)(this->paths_));
    this->paths_ = 0;
    this->numPaths_ = 0;
    this->pathsAllocated_ = 0;
  }

  if (this->source_) { free(this->source_); this->source_ = 0; }
  if (this->pattern_) { free(this->pattern_); this->pattern_ = 0; }
  if (this->style_) { free(this->style_); this->style_ = 0; }
  if (this->original_) { free(this->original_); this->original_ = 0; }
  if (this->use_) { free(this->use_); this->use_ = 0; }
  if (this->nonDefaultRule_) { free(this->nonDefaultRule_);
		    this->nonDefaultRule_ = 0; }

  if (this->numShields_) {
    for (i = 0; i < this->numShields_; i++) {
      this->shields_[i]->defiShield::Destroy();
      free((char*)(this->shields_[i]));
      this->shields_[i] = 0;
    }
    free((char*)(this->shields_));
    this->shields_ = 0;
    this->numShields_ = 0;
    this->shieldsAllocated_ = 0;
  }

  if (this->numWidths_) {
   for (i = 0; i < this->numWidths_; i++)
     free(this->wlayers_[i]);
  this->numWidths_ = 0;
  }

  if (this->numSpacing_) {
   for (i = 0; i < this->numSpacing_; i++)
     free(this->slayers_[i]);
  this->numSpacing_ = 0;
  }

  if (this->numShieldNet_) {
   for (i = 0; i < this->numShieldNet_; i++)
     free(this->shieldNet_[i]);
   this->numShieldNet_ = 0;
  }
}


int defiNet::hasSource() const {
   return this->source_ ? 1 : 0;
}


int defiNet::hasPattern() const {
   return this->pattern_ ? 1 : 0;
}



int defiNet::hasOriginal() const {
   return this->original_ ? 1 : 0;
}


int defiNet::hasCap() const {
  return (int)(this->hasCap_);
}


int defiNet::hasUse() const {
   return this->use_ ? 1 : 0;
}


int defiNet::hasStyle() const {
   return this->style_ ? 1 : 0;
}


int defiNet::hasXTalk() const {
   return (this->xTalk_ != -1) ? 1 : 0;
}


int defiNet::hasNonDefaultRule() const {
   return this->nonDefaultRule_ ? 1 : 0;
}


void defiNet::setSource(const char* typ) {
  int len;
  if (this->source_) free(this->source_);
  len = strlen(typ) + 1;
  this->source_ = (char*)malloc(len);
  strcpy(this->source_, DEFCASE(typ));
}


void defiNet::setOriginal(const char* typ) {
  int len;
  if (this->original_) free(this->original_);
  len = strlen(typ) + 1;
  this->original_ = (char*)malloc(len);
  strcpy(this->original_, DEFCASE(typ));
}


void defiNet::setPattern(const char* typ) {
  int len;
  if (this->pattern_) free(this->pattern_);
  len = strlen(typ) + 1;
  this->pattern_ = (char*)malloc(len);
  strcpy(this->pattern_, DEFCASE(typ));
}


void defiNet::setCap(double w) {
  this->cap_ = w;
  this->hasCap_ = 1;
}


void defiNet::setUse(const char* typ) {
  int len;
  if (this->use_) free(this->use_);
  len = strlen(typ) + 1;
  this->use_ = (char*)malloc(len);
  strcpy(this->use_, DEFCASE(typ));
}


void defiNet::setStyle(const char* typ) {
  int len;
  if (this->style_) free(this->style_);
  len = strlen(typ) + 1;
  this->style_ = (char*)malloc(len);
  strcpy(this->style_, DEFCASE(typ));
}


void defiNet::setNonDefaultRule(const char* typ) {
  int len;
  if (this->nonDefaultRule_) free(this->nonDefaultRule_);
  len = strlen(typ) + 1;
  this->nonDefaultRule_ = (char*)malloc(len);
  strcpy(this->nonDefaultRule_, DEFCASE(typ));
}


const char* defiNet::source() const {
  return this->source_;
}


const char* defiNet::original() const {
  return this->original_;
}


const char* defiNet::pattern() const {
  return this->pattern_;
}


double defiNet::cap() const {
  return (this->hasCap_ ? this->cap_ : 0.0);
}


const char* defiNet::use() const {
  return this->use_;
}


const char* defiNet::style() const {
  return this->style_;
}


const char* defiNet::shieldNet(int index) const {
  return this->shieldNet_[index];
}


const char* defiNet::nonDefaultRule() const {
  return this->nonDefaultRule_;
}


void defiNet::bumpPaths(int size) {
  int i;
  defiPath** newPaths = (defiPath**)malloc(sizeof(defiPath)*size);

  for (i = 0; i < this->numPaths_; i++)
    newPaths[i] = this->paths_[i];

  this->pathsAllocated_ = size;
  free((char*)(this->paths_));
  this->paths_ = newPaths;
}


int defiNet::numPaths() const {
  return this->numPaths_;
}


defiPath* defiNet::path(int index) {
  if (index >= 0 && index < this->numPaths_)
    return this->paths_[index];
  return 0;
}


void defiNet::bumpShieldNets(int size) {
  char** newShieldNets = (char**)malloc(sizeof(char*)*size);
  int i;
 
  if (this->shieldNet_) {
    for (i = 0; i < this->shieldNetsAllocated_; i++) {
      newShieldNets[i] = this->shieldNet_[i];
    }
    free((char*)(this->shieldNet_));
  }
 
  this->shieldNet_ = newShieldNets;
  this->shieldNetsAllocated_ = size;
}


int defiNet::numShields() const {
  return this->numShields_;
}


defiShield* defiNet::shield(int index) {
  if (index >= 0 && index < this->numShields_)
    return this->shields_[index];
  return 0;
}


int defiNet::numNoShields() const {
  return this->numShields_;
}


defiShield* defiNet::noShield(int index) {
  if (index >= 0 && index < this->numShields_)
    return this->shields_[index];
  return 0;
}


int defiNet::hasVoltage() const {
  return (int)(this->hasVoltage_);
}


double defiNet::voltage() const {
  return this->voltage_;
}


int defiNet::numWidthRules() const {
  return this->numWidths_;
}


int defiNet::numSpacingRules() const {
  return this->numSpacing_;
}


int defiNet::hasWidthRules() const {
  return this->numWidths_;
}


int defiNet::hasSpacingRules() const {
  return this->numSpacing_;
}


void defiNet::setXTalk(int i) {
  this->xTalk_ = i;
}


int defiNet::XTalk() const {
  return this->xTalk_;
}


void defiNet::addVpin(const char* name) {
  defiVpin* vp;
  if (this->numVpins_ == this->vpinsAllocated_) {
    defiVpin** array;
    int i;
    this->vpinsAllocated_ = this->vpinsAllocated_ ?
	      this->vpinsAllocated_ * 2 : 2 ;
    array = (defiVpin**)malloc(sizeof(defiVpin*)*this->vpinsAllocated_);
    for (i = 0; i < this->numVpins_; i++)
      array[i] = this->vpins_[i];
    if (this->vpins_) free((char*)(this->vpins_));
    this->vpins_ = array;
  }
  vp = this->vpins_[this->numVpins_] = (defiVpin*)malloc(sizeof(defiVpin));
  this->numVpins_ += 1;
  vp->defiVpin::Init(name);
}


void defiNet::addVpinLayer(const char* name) {
  defiVpin* vp = this->vpins_[this->numVpins_-1];
  vp->defiVpin::setLayer(name);
}


void defiNet::addVpinLoc(const char* status, int x, int y, int orient) {
  defiVpin* vp = this->vpins_[this->numVpins_-1];
  vp->defiVpin::setStatus(*status);
  vp->defiVpin::setLoc(x,y);
  vp->defiVpin::setOrient(orient);
}


void defiNet::addVpinBounds(int xl, int yl, int xh, int yh) {
  defiVpin* vp = this->vpins_[this->numVpins_-1];
  vp->defiVpin::setBounds(xl, yl, xh, yh);
}


int defiNet::numVpins() const {
  return this->numVpins_;
}


defiVpin* defiNet::vpin(int index) const {
  if (index < 0 || index >= this->numVpins_) return 0;
  return this->vpins_[index];
}


void defiNet::spacingRule(int index, char** layer, double* dist,
	 double* left, double* right) {
  if (index >= 0 && index < this->numSpacing_) {
    if (layer) *layer = this->slayers_[index];
    if (dist) *dist = this->sdist_[index];
    if (left) *left = this->sleft_[index];
    if (right) *right = this->sright_[index];
  }
}


void defiNet::widthRule(int index, char** layer, double* dist) {
  if (index >= 0 && index < this->numWidths_) {
    if (layer) *layer = this->wlayers_[index];
    if (dist) *dist = this->wdist_[index];
  }
}


void defiNet::setVoltage(double v) {
  this->voltage_ = v;
  this->hasVoltage_ = 1;
}


void defiNet::setWidth(const char* layer, double d) {
  int len = strlen(layer) + 1;
  char* l = (char*)malloc(len);
  strcpy(l, DEFCASE(layer));

  if (this->numWidths_ >= this->widthsAllocated_) {
    int i;
    char** nl;
    double* nd;
    this->widthsAllocated_ = this->widthsAllocated_ ?
       this->widthsAllocated_ * 2 : 4 ;
    nl = (char**)malloc(sizeof(char*) * this->widthsAllocated_);
    nd = (double*)malloc(sizeof(double) * this->widthsAllocated_);
    for (i = 0; i < this->numWidths_; i++) {
      nl[i] = this->wlayers_[i];
      nd[i] = this->wdist_[i];
    }
    free((char*)(this->wlayers_));
    free((char*)(this->wdist_));
    this->wlayers_ = nl;
    this->wdist_ = nd;
  }

  this->wlayers_[this->numWidths_] = l;
  this->wdist_[this->numWidths_] = d;
  (this->numWidths_)++;
}


void defiNet::setSpacing(const char* layer, double d) {
  int len = strlen(layer) + 1;
  char* l = (char*)malloc(len);
  strcpy(l, DEFCASE(layer));

  if (this->numSpacing_ >= this->spacingAllocated_) {
    int i;
    char** nl;
    double* nd;
    double* n1;
    double* n2;
    this->spacingAllocated_ = this->spacingAllocated_ ?
       this->spacingAllocated_ * 2 : 4 ;
    nl = (char**)malloc(sizeof(char*) * this->spacingAllocated_);
    nd = (double*)malloc(sizeof(double) * this->spacingAllocated_);
    n1 = (double*)malloc(sizeof(double) * this->spacingAllocated_);
    n2 = (double*)malloc(sizeof(double) * this->spacingAllocated_);
    for (i = 0; i < this->numSpacing_; i++) {
      nl[i] = this->slayers_[i];
      nd[i] = this->sdist_[i];
      n1[i] = this->sleft_[i];
      n2[i] = this->sright_[i];
    }
    free((char*)(this->slayers_));
    free((char*)(this->sdist_));
    free((char*)(this->sleft_));
    free((char*)(this->sright_));
    this->slayers_ = nl;
    this->sdist_ = nd;
    this->sleft_ = n1;
    this->sright_ = n2;
  }

  this->slayers_[this->numSpacing_] = l;
  this->sdist_[this->numSpacing_] = d;
  this->sleft_[this->numSpacing_] = d;
  this->sright_[this->numSpacing_] = d;
  (this->numSpacing_)++;
}


void defiNet::setRange(double left, double right) {
  // This is always called right after setSpacing.
  this->sleft_[this->numSpacing_-1] = left;
  this->sright_[this->numSpacing_-1] = right;
}


