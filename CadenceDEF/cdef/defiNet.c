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
#include "defiNet.h"
#include "defiPath.h"
#include "defiDebug.h"

/*
 * defiSubnet
 */ defiSubnet *
defiSubnet_CreateUninitialized()
{
  defiSubnet *this = (defiSubnet *) malloc(sizeof(defiSubnet));

  defiSubnet_Init(this);
  return (this);
}

void 
defiSubnet_Init(defiSubnet * this)
{
  this->name_ = 0;
  defiSubnet_bumpName(this,
		      16);
  this->instances_ = 0;
  this->pins_ = 0;
  this->musts_ = 0;
  this->synthesized_ = 0;
  this->numPins_ = 0;
  defiSubnet_bumpPins(this,
		      16);
  this->paths_ = 0;
  this->numPaths_ = 0;
  this->pathsAllocated_ = 0;
  this->nonDefaultRule_ = 0;
  defiSubnet_clear(this);
}

void 
defiSubnet_Destroy(defiSubnet * this)
{
  defiSubnet_clear(this);
  free(this->name_);
  free((char *) (this->instances_));
  free((char *) (this->pins_));
  free(this->musts_);
  free(this->synthesized_);
}

void
defiSubnet_Delete(defiSubnet * this)
{
  if (this == NULL)
    return;
  defiSubnet_Destroy(this);
  free(this);
}

void 
defiSubnet_setName(defiSubnet * this,
		   const char *name)
{
  int     len = strlen(name) + 1;

  if (len > this->nameSize_)
    defiSubnet_bumpName(this,
			len);
  strcpy(this->name_, DEFCASE(name));
}

void 
defiSubnet_setNonDefault(defiSubnet * this,
			 const char *name)
{
  int     len = strlen(name) + 1;

  this->nonDefaultRule_ = (char *) malloc(len);
  strcpy(this->nonDefaultRule_, DEFCASE(name));
}

void 
defiSubnet_addMustPin(defiSubnet * this,
		      const char *instance,
		      const char *pin,
		      int syn)
{
  defiSubnet_addPin(this,
		    instance,
		    pin,
		    syn);
  this->musts_[this->numPins_ - 1] = 1;
}

void 
defiSubnet_addPin(defiSubnet * this,
		  const char *instance,
		  const char *pin,
		  int syn)
{
  int     len;

  if (this->numPins_ == this->pinsAllocated_)
    defiSubnet_bumpPins(this,
			this->pinsAllocated_ * 2);
  len = strlen(instance) + 1;
  this->instances_[this->numPins_] = (char *) malloc(len);
  strcpy(this->instances_[this->numPins_], DEFCASE(instance));
  len = strlen(pin) + 1;
  this->pins_[this->numPins_] = (char *) malloc(len);
  strcpy(this->pins_[this->numPins_], DEFCASE(pin));
  this->musts_[this->numPins_] = 0;
  this->synthesized_[this->numPins_] = syn;
  (this->numPins_)++;
}

void 
defiSubnet_setType(defiSubnet * this,
		   const char *typ)
{
  if (*typ == 'F') {
    this->isFixed_ = 1;
  }
  else if (*typ == 'C') {
    this->isCover_ = 1;
  }
  else if (*typ == 'R') {
    this->isRouted_ = 1;
  }
  else {
  }
}

void 
defiSubnet_addPath(defiSubnet * this,
		   defiPath * p)
{
  if (this->numPaths_ >= this->pathsAllocated_)
    defiSubnet_bumpPaths(this,
		     this->pathsAllocated_ ? this->pathsAllocated_ * 2 : 8);
  this->paths_[this->numPaths_++] = p;
}

const char *
defiSubnet_name(const defiSubnet * this)
{
  return this->name_;
}

int 
defiSubnet_hasNonDefaultRule(const defiSubnet * this)
{
  return this->nonDefaultRule_ ? 1 : 0;
}

const char *
defiSubnet_nonDefaultRule(const defiSubnet * this)
{
  return this->nonDefaultRule_;
}

int 
defiSubnet_numConnections(defiSubnet * this)
{
  return this->numPins_;
}

const char *
defiSubnet_instance(defiSubnet * this,
		    int index)
{
  if (index >= 0 && index < this->numPins_)
    return this->instances_[index];
  return 0;
}

const char *
defiSubnet_pin(defiSubnet * this,
	       int index)
{
  if (index >= 0 && index < this->numPins_)
    return this->pins_[index];
  return 0;
}

int 
defiSubnet_pinIsMustJoin(defiSubnet * this,
			 int index)
{
  if (index >= 0 && index < this->numPins_)
    return (int) (this->musts_[index]);
  return 0;
}

int 
defiSubnet_pinIsSynthesized(defiSubnet * this,
			    int index)
{
  if (index >= 0 && index < this->numPins_)
    return (int) (this->synthesized_[index]);
  return 0;
}

int 
defiSubnet_isFixed(const defiSubnet * this)
{
  return (int) (this->isFixed_);
}

int 
defiSubnet_isRouted(const defiSubnet * this)
{
  return (int) (this->isRouted_);
}

int 
defiSubnet_isCover(const defiSubnet * this)
{
  return (int) (this->isCover_);
}

void 
defiSubnet_bumpName(defiSubnet * this,
		    int size)
{
  if (this->name_)
    free(this->name_);
  this->name_ = (char *) malloc(size);
  this->nameSize_ = size;
  this->name_[0] = '\0';
}

void 
defiSubnet_bumpPins(defiSubnet * this,
		    int size)
{
  char  **newInstances = (char **) malloc(sizeof(char *) * size);

  char  **newPins = (char **) malloc(sizeof(char *) * size);

  char   *newMusts = (char *) malloc(size);

  char   *newSyn = (char *) malloc(size);

  int     i;

  if (this->instances_) {
    for (i = 0;
	 i < this->pinsAllocated_;
	 i++) {
      newInstances[i] = this->instances_[i];
      newPins[i] = this->pins_[i];
      newMusts[i] = this->musts_[i];
      newSyn[i] = this->synthesized_[i];
    }
    free((char *) (this->instances_));
    free((char *) (this->pins_));
    free(this->musts_);
    free(this->synthesized_);
  }
  this->instances_ = newInstances;
  this->pins_ = newPins;
  this->musts_ = newMusts;
  this->synthesized_ = newSyn;
  this->pinsAllocated_ = size;
}

void 
defiSubnet_clear(defiSubnet * this)
{
  int     i;

  this->isFixed_ = 0;
  this->isRouted_ = 0;
  this->isCover_ = 0;
  this->name_[0] = '\0';
  for (i = 0;
       i < this->numPins_;
       i++) {
    free(this->instances_[i]);
    free(this->pins_[i]);
    this->instances_[i] = 0;
    this->pins_[i] = 0;
    this->musts_[i] = 0;
    this->synthesized_[i] = 0;
  }
  this->numPins_ = 0;
  if (this->paths_) {
    for (i = 0;
	 i < this->numPaths_;
	 i++) {
      defiPath_Destroy(this->paths_[i]);
      free((char *) (this->paths_[i]));
    }
    free((char *) (this->paths_));
    this->paths_ = 0;
    this->numPaths_ = 0;
    this->pathsAllocated_ = 0;
  }
  if (this->nonDefaultRule_) {
    free(this->nonDefaultRule_);
    this->nonDefaultRule_ = 0;
  }
}

void 
defiSubnet_print(defiSubnet * this,
		 FILE * f)
{
  int     i;

  defiPath *p;

  fprintf(f, " subnet '%s'", this->name_);
  if (defiSubnet_isFixed(this))
    fprintf(f, " FIXED");
  if (defiSubnet_isRouted(this))
    fprintf(f, " ROUTED");
  if (defiSubnet_isCover(this))
    fprintf(f, " COVER");
  fprintf(f, "\n");
  if (defiSubnet_hasNonDefaultRule(this))
    fprintf(f, " nondefault rule %s\n", defiSubnet_nonDefaultRule(this));
  if (defiSubnet_numConnections(this)) {
    fprintf(f, " Pins:\n");
    for (i = 0;
	 i < defiSubnet_numConnections(this);
	 i++) {
      fprintf(f, " '%s' '%s'%s%s\n", defiSubnet_instance(this,
						    i), defiSubnet_pin(this,
					  i), defiSubnet_pinIsMustJoin(this,
		    i) ? " MUSTJOIN" : "", defiSubnet_pinIsSynthesized(this,
						  i) ? " SYNTHESIZED" : "");
    }
  }
  if (defiSubnet_numPaths(this)) {
    fprintf(f, " Paths:\n");
    for (i = 0;
	 i < defiSubnet_numPaths(this);
	 i++) {
      p = defiSubnet_path(this,
			  i);
      defiPath_print(p,
		     f);
    }
  }
}

defiPath *
defiSubnet_path(defiSubnet * this,
		int index)
{
  if (index >= 0 && index < this->numPaths_)
    return this->paths_[index];
  return 0;
}

int 
defiSubnet_numPaths(const defiSubnet * this)
{
  return this->numPaths_;
}

void 
defiSubnet_bumpPaths(defiSubnet * this,
		     int size)
{
  int     i;

  defiPath **newPaths = (defiPath **) malloc(sizeof(defiPath *) * size);

  for (i = 0;
       i < this->numPaths_;
       i++)
    newPaths[i] = this->paths_[i];
  this->pathsAllocated_ = size;
  free((char *) (this->paths_));
  this->paths_ = newPaths;
}

/*
 * defiVpin
 */ defiVpin *
defiVpin_CreateUninitialized()
{
  defiVpin *this = (defiVpin *) malloc(sizeof(defiVpin));

  return (this);
}

void 
defiVpin_Init(defiVpin * this,
	      const char *name)
{
  int     len = strlen(name) + 1;

  this->name_ = (char *) malloc(len);
  strcpy(this->name_, DEFCASE(name));
  this->orient_ = -1;
  this->status_ = ' ';
  this->layer_ = 0;
}

void
defiVpin_Delete(defiVpin * this)
{
  if (this == NULL)
    return;
  free(this);
}

void 
defiVpin_Destroy(defiVpin * this)
{
  free(this->name_);
  if (this->layer_)
    free(this->layer_);
}

void 
defiVpin_setBounds(defiVpin * this,
		   int xl,
		   int yl,
		   int xh,
		   int yh)
{
  this->xl_ = xl;
  this->yl_ = yl;
  this->xh_ = xh;
  this->yh_ = yh;
}

void 
defiVpin_setLayer(defiVpin * this,
		  const char *lay)
{
  int     len = strlen(lay) + 1;

  this->layer_ = (char *) malloc(len);
  strcpy(this->layer_, lay);
}

void 
defiVpin_setOrient(defiVpin * this,
		   int orient)
{
  this->orient_ = orient;
}

void 
defiVpin_setLoc(defiVpin * this,
		int x,
		int y)
{
  this->xLoc_ = x;
  this->yLoc_ = y;
}

void 
defiVpin_setStatus(defiVpin * this,
		   char st)
{
  this->status_ = st;
}

int 
defiVpin_xl(const defiVpin * this)
{
  return this->xl_;
}

int 
defiVpin_yl(const defiVpin * this)
{
  return this->yl_;
}

int 
defiVpin_xh(const defiVpin * this)
{
  return this->xh_;
}

int 
defiVpin_yh(const defiVpin * this)
{
  return this->yh_;
}

char 
defiVpin_status(const defiVpin * this)
{
  return this->status_;
}

int 
defiVpin_orient(const defiVpin * this)
{
  return this->orient_;
}

int 
defiVpin_xLoc(const defiVpin * this)
{
  return this->xLoc_;
}

int 
defiVpin_yLoc(const defiVpin * this)
{
  return this->yLoc_;
}

const char *
defiVpin_name(const defiVpin * this)
{
  return this->name_;
}

const char *
defiVpin_layer(const defiVpin * this)
{
  return this->layer_;
}

/*
 * defiShield
 */ defiShield *
defiShield_CreateUninitialized()
{
  defiShield *this = (defiShield *) malloc(sizeof(defiShield));

  return (this);
}

void 
defiShield_Init(defiShield * this,
		const char *name)
{
  int     len = strlen(name) + 1;

  this->name_ = (char *) malloc(len);
  strcpy(this->name_, DEFCASE(name));
  this->numPaths_ = 0;
  this->pathsAllocated_ = 0;
  this->paths_ = 0;
}

void 
defiShield_Destroy(defiShield * this)
{
  defiShield_clear(this);
}

void
defiShield_Delete(defiShield * this)
{
  if (this == NULL)
    return;
  defiShield_Destroy(this);
  free(this);
}

void 
defiShield_addPath(defiShield * this,
		   defiPath * p)
{
  if (this->numPaths_ >= this->pathsAllocated_)
    defiShield_bumpPaths(this,
		     this->pathsAllocated_ ? this->pathsAllocated_ * 2 : 8);
  this->paths_[this->numPaths_++] = p;
}

void 
defiShield_clear(defiShield * this)
{
  defiPath *p;

  int     i;

  if (this->name_)
    free(this->name_);
  if (this->paths_) {
    for (i = 0;
	 i < this->numPaths_;
	 i++) {
      p = this->paths_[i];
      defiPath_Destroy(p);
      free((char *) p);
    }
    free((char *) (this->paths_));
    this->paths_ = 0;
    this->numPaths_ = 0;
    this->pathsAllocated_ = 0;
  }
}

void 
defiShield_bumpPaths(defiShield * this,
		     int size)
{
  int     i;

  defiPath **newPaths = (defiPath **) malloc(sizeof(defiPath) * size);

  for (i = 0;
       i < this->numPaths_;
       i++)
    newPaths[i] = this->paths_[i];
  this->pathsAllocated_ = size;
  free((char *) (this->paths_));
  this->paths_ = newPaths;
}

int 
defiShield_numPaths(const defiShield * this)
{
  return this->numPaths_;
}

const char *
defiShield_shieldName(const defiShield * this)
{
  return this->name_;
}

defiPath *
defiShield_path(defiShield * this,
		int index)
{
  if (index >= 0 && index < this->numPaths_)
    return this->paths_[index];
  return 0;
}

/*
 * defiNet
 */ defiNet *
defiNet_CreateUninitialized()
{
  defiNet *this = (defiNet *) malloc(sizeof(defiNet));

  defiNet_Init(this);
  return (this);
}

void 
defiNet_Init(defiNet * this)
{
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
  defiNet_bumpProps(this,
		    2);
  defiNet_bumpName(this,
		   16);
  defiNet_bumpPins(this,
		   16);
  defiNet_bumpSubnets(this,
		      2);
  defiNet_clear(this);
}

void 
defiNet_Destroy(defiNet * this)
{
  defiNet_clear(this);
  free(this->name_);
  free((char *) (this->instances_));
  free((char *) (this->pins_));
  free(this->musts_);
  free(this->synthesized_);
  free((char *) (this->propNames_));
  free((char *) (this->propValues_));
  free((char *) (this->subnets_));
  if (this->source_)
    free(this->source_);
  if (this->pattern_)
    free(this->pattern_);
  if (this->style_)
    free(this->style_);
  if (this->shieldNet_)
    free(this->shieldNet_);
  if (this->original_)
    free(this->original_);
  if (this->use_)
    free(this->use_);
  if (this->nonDefaultRule_)
    free(this->nonDefaultRule_);
  if (this->wlayers_)
    free((char *) (this->wlayers_));
  if (this->slayers_)
    free((char *) (this->slayers_));
  if (this->sdist_)
    free((char *) (this->sdist_));
  if (this->wdist_)
    free((char *) (this->wdist_));
  if (this->sleft_)
    free((char *) (this->sleft_));
  if (this->sright_)
    free((char *) (this->sright_));
}

void
defiNet_Delete(defiNet * this)
{
  if (this == NULL)
    return;
  defiNet_Destroy(this);
  free(this);
}

void 
defiNet_setName(defiNet * this,
		const char *name)
{
  int     len = strlen(name) + 1;

  defiNet_clear(this);
  if (len > this->nameSize_)
    defiNet_bumpName(this,
		     len);
  strcpy(this->name_, DEFCASE(name));
}

void 
defiNet_addMustPin(defiNet * this,
		   const char *instance,
		   const char *pin,
		   int syn)
{
  defiNet_addPin(this,
		 instance,
		 pin,
		 syn);
  this->musts_[this->numPins_ - 1] = 1;
}

void 
defiNet_addPin(defiNet * this,
	       const char *instance,
	       const char *pin,
	       int syn)
{
  int     len;

  if (this->numPins_ == this->pinsAllocated_)
    defiNet_bumpPins(this,
		     this->pinsAllocated_ * 2);
  len = strlen(instance) + 1;
  this->instances_[this->numPins_] = (char *) malloc(len);
  strcpy(this->instances_[this->numPins_], DEFCASE(instance));
  len = strlen(pin) + 1;
  this->pins_[this->numPins_] = (char *) malloc(len);
  strcpy(this->pins_[this->numPins_], DEFCASE(pin));
  this->musts_[this->numPins_] = 0;
  this->synthesized_[this->numPins_] = syn;
  (this->numPins_)++;
}

void 
defiNet_setWeight(defiNet * this,
		  int w)
{
  this->hasWeight_ = 1;
  this->weight_ = w;
}

void 
defiNet_addProp(defiNet * this,
		const char *name,
		const char *value)
{
  int     len;

  if (this->numProps_ == this->propsAllocated_)
    defiNet_bumpProps(this,
		      this->propsAllocated_ * 2);
  len = strlen(name) + 1;
  this->propNames_[this->numProps_] = (char *) malloc(len);
  strcpy(this->propNames_[this->numProps_], DEFCASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProps_] = (char *) malloc(len);
  strcpy(this->propValues_[this->numProps_], DEFCASE(value));
  (this->numProps_)++;
}

void 
defiNet_addSubnet(defiNet * this,
		  defiSubnet * subnet)
{
  if (this->numSubnets_ >= this->subnetsAllocated_)
    defiNet_bumpSubnets(this,
			this->subnetsAllocated_ * 2);
  this->subnets_[this->numSubnets_++] = subnet;
}

void 
defiNet_setType(defiNet * this,
		const char *typ)
{
  if (*typ == 'F') {
    this->isFixed_ = 1;
  }
  else if (*typ == 'C') {
    this->isCover_ = 1;
  }
  else if (*typ == 'R') {
    this->isRouted_ = 1;
  }
  else {
  }
}

void 
defiNet_addPath(defiNet * this,
		defiPath * p)
{
  if (this->numPaths_ >= this->pathsAllocated_)
    defiNet_bumpPaths(this,
		      this->pathsAllocated_ ? this->pathsAllocated_ * 2 : 8);
  this->paths_[this->numPaths_++] = p;
}

void 
defiNet_addShield(defiNet * this,
		  const char *name)
{
  defiShield *shield;

  if (this->numShields_ == this->shieldsAllocated_) {
    defiShield **array;

    int     i;

    this->shieldsAllocated_ = this->shieldsAllocated_ ? this->shieldsAllocated_ * 2 : 2;
    array = (defiShield **) malloc(sizeof(defiShield *) * this->shieldsAllocated_);
    for (i = 0;
	 i < this->numShields_;
	 i++)
      array[i] = this->shields_[i];
    if (this->shields_)
      free((char *) (this->shields_));
    this->shields_ = array;
  }
  shield = this->shields_[this->numShields_] = (defiShield *) malloc(sizeof(defiShield));
  this->numShields_ += 1;
  defiShield_Init(shield,
		  name);
}

void 
defiNet_addShieldPath(defiNet * this,
		      defiPath * p)
{
  defiShield_addPath(this->shields_[this->numShields_ - 1],
		     p);
}

void 
defiNet_addShieldNet(defiNet * this,
		     const char *name)
{
  int     len;

  if (this->numShieldNet_ == this->shieldNetsAllocated_) {
    if (this->shieldNetsAllocated_ == 0)
      defiNet_bumpShieldNets(this,
			     2);
    else
      defiNet_bumpShieldNets(this,
			     this->shieldNetsAllocated_ * 2);
  }
  len = strlen(name) + 1;
  this->shieldNet_[this->numShieldNet_] = (char *) malloc(len);
  strcpy(this->shieldNet_[this->numShieldNet_], DEFCASE(name));
  (this->numShieldNet_)++;
}

const char *
defiNet_name(const defiNet * this)
{
  return this->name_;
}

int 
defiNet_weight(const defiNet * this)
{
  return this->weight_;
}

int 
defiNet_numProps(const defiNet * this)
{
  return this->numProps_;
}

int 
defiNet_hasProps(const defiNet * this)
{
  return this->numProps_ ? 1 : 0;
}

int 
defiNet_hasWeight(const defiNet * this)
{
  return (int) (this->hasWeight_);
}

const char *
defiNet_propName(const defiNet * this,
		 int index)
{
  if (index >= 0 && index < this->numProps_)
    return this->propNames_[index];
  return 0;
}

const char *
defiNet_propValue(const defiNet * this,
		  int index)
{
  if (index >= 0 && index < this->numProps_)
    return this->propValues_[index];
  return 0;
}

int 
defiNet_numConnections(const defiNet * this)
{
  return this->numPins_;
}

int 
defiNet_numShieldNets(const defiNet * this)
{
  return this->numShieldNet_;
}

const char *
defiNet_instance(const defiNet * this,
		 int index)
{
  if (index >= 0 && index < this->numPins_)
    return this->instances_[index];
  return 0;
}

const char *
defiNet_pin(const defiNet * this,
	    int index)
{
  if (index >= 0 && index < this->numPins_)
    return this->pins_[index];
  return 0;
}

int 
defiNet_pinIsMustJoin(const defiNet * this,
		      int index)
{
  if (index >= 0 && index < this->numPins_)
    return (int) (this->musts_[index]);
  return 0;
}

int 
defiNet_pinIsSynthesized(const defiNet * this,
			 int index)
{
  if (index >= 0 && index < this->numPins_)
    return (int) (this->synthesized_[index]);
  return 0;
}

int 
defiNet_hasSubnets(const defiNet * this)
{
  return this->numSubnets_ ? 1 : 0;
}

int 
defiNet_numSubnets(const defiNet * this)
{
  return this->numSubnets_;
}

defiSubnet *
defiNet_subnet(defiNet * this,
	       int index)
{
  if (index >= 0 && index < this->numSubnets_)
    return this->subnets_[index];
  return 0;
}

int 
defiNet_isFixed(const defiNet * this)
{
  return (int) (this->isFixed_);
}

int 
defiNet_isRouted(const defiNet * this)
{
  return (int) (this->isRouted_);
}

int 
defiNet_isCover(const defiNet * this)
{
  return (int) (this->isCover_);
}

void 
defiNet_print(defiNet * this,
	      FILE * f)
{
  int     i;

  defiPath *p;

  defiSubnet *s;

  defiVpin *vp;

  fprintf(f, "Net '%s'", this->name_);
  if (defiNet_isFixed(this))
    fprintf(f, " FIXED");
  if (defiNet_isRouted(this))
    fprintf(f, " ROUTED");
  if (defiNet_isCover(this))
    fprintf(f, " COVER");
  fprintf(f, "\n");
  if (defiNet_hasWeight(this))
    fprintf(f, " weight=%d\n", defiNet_weight(this));
  if (defiNet_hasCap(this))
    fprintf(f, " cap=%lf\n", defiNet_cap(this));
  if (defiNet_hasSource(this))
    fprintf(f, " source='%s'\n", defiNet_source(this));
  if (defiNet_hasPattern(this))
    fprintf(f, " pattern='%s'\n", defiNet_pattern(this));
  if (defiNet_hasOriginal(this))
    fprintf(f, " original='%s'\n", defiNet_original(this));
  if (defiNet_hasUse(this))
    fprintf(f, " use='%s'\n", defiNet_use(this));
  if (defiNet_hasNonDefaultRule(this))
    fprintf(f, " nonDefaultRule='%s'\n", defiNet_nonDefaultRule(this));
  if (defiNet_hasXTalk(this))
    fprintf(f, " xtalk=%d\n", defiNet_XTalk(this));
  if (defiNet_hasStyle(this))
    fprintf(f, " style='%s'\n", defiNet_style(this));
  if (defiNet_hasProps(this)) {
    fprintf(f, " Props:\n");
    for (i = 0;
	 i < defiNet_numProps(this);
	 i++) {
      fprintf(f, " '%s' '%s'\n", defiNet_propName(this,
						  i), defiNet_propValue(this,
									i));
    }
  }
  if (defiNet_numConnections(this)) {
    fprintf(f, " Pins:\n");
    for (i = 0;
	 i < defiNet_numConnections(this);
	 i++) {
      fprintf(f, " '%s' '%s'%s%s\n", defiNet_instance(this,
						      i), defiNet_pin(this,
					     i), defiNet_pinIsMustJoin(this,
		       i) ? " MUSTJOIN" : "", defiNet_pinIsSynthesized(this,
						  i) ? " SYNTHESIZED" : "");
    }
  }
  for (i = 0;
       i < this->numVpins_;
       i++) {
    vp = defiNet_vpin(this,
		      i);
    fprintf(f, " VPIN %s status '%c' layer %s %d,%d orient %d bounds %d,%d to %d,%d\n", defiVpin_name(vp), defiVpin_status(vp), defiVpin_layer(vp) ? defiVpin_layer(vp) : "", defiVpin_xLoc(vp), defiVpin_yLoc(vp), defiVpin_orient(vp), defiVpin_xl(vp), defiVpin_yl(vp), defiVpin_xh(vp), defiVpin_yh(vp));
  }
  if (defiNet_numPaths(this)) {
    fprintf(f, " Paths:\n");
    for (i = 0;
	 i < defiNet_numPaths(this);
	 i++) {
      p = defiNet_path(this,
		       i);
      defiPath_print(p,
		     f);
    }
  }
  if (defiNet_hasSubnets(this)) {
    fprintf(f, " Subnets:\n");
    for (i = 0;
	 i < defiNet_numSubnets(this);
	 i++) {
      s = defiNet_subnet(this,
			 i);
      defiSubnet_print(s,
		       f);
    }
  }
}

void 
defiNet_bumpName(defiNet * this,
		 int size)
{
  if (this->name_)
    free(this->name_);
  this->name_ = (char *) malloc(size);
  this->nameSize_ = size;
  this->name_[0] = '\0';
}

void 
defiNet_bumpPins(defiNet * this,
		 int size)
{
  char  **newInstances = (char **) malloc(sizeof(char *) * size);

  char  **newPins = (char **) malloc(sizeof(char *) * size);

  char   *newMusts = (char *) malloc(size);

  char   *newSyn = (char *) malloc(size);

  int     i;

  if (this->instances_) {
    for (i = 0;
	 i < this->pinsAllocated_;
	 i++) {
      newInstances[i] = this->instances_[i];
      newPins[i] = this->pins_[i];
      newMusts[i] = this->musts_[i];
      newSyn[i] = this->synthesized_[i];
    }
    free((char *) (this->instances_));
    free((char *) (this->pins_));
    free(this->musts_);
    free(this->synthesized_);
  }
  this->instances_ = newInstances;
  this->pins_ = newPins;
  this->musts_ = newMusts;
  this->synthesized_ = newSyn;
  this->pinsAllocated_ = size;
}

void 
defiNet_bumpProps(defiNet * this,
		  int size)
{
  char  **newNames = (char **) malloc(sizeof(char *) * size);

  char  **newValues = (char **) malloc(sizeof(char *) * size);

  int     i;

  if (this->propNames_) {
    for (i = 0;
	 i < this->numProps_;
	 i++) {
      newNames[i] = this->propNames_[i];
      newValues[i] = this->propValues_[i];
    }
    free((char *) (this->propNames_));
    free((char *) (this->propValues_));
  }
  this->propNames_ = newNames;
  this->propValues_ = newValues;
  this->propsAllocated_ = size;
}

void 
defiNet_bumpSubnets(defiNet * this,
		    int size)
{
  defiSubnet **newSubnets = (defiSubnet **) malloc(sizeof(defiSubnet *) * size);

  int     i;

  if (this->subnets_) {
    for (i = 0;
	 i < this->numSubnets_;
	 i++) {
      newSubnets[i] = this->subnets_[i];
    }
    free((char *) (this->subnets_));
  }
  this->subnets_ = newSubnets;
  this->subnetsAllocated_ = size;
}

void 
defiNet_clear(defiNet * this)
{
  int     i;

  defiSubnet *s;

  defiPath *p;

  defiVpin *vp;

  this->isFixed_ = 0;
  this->isRouted_ = 0;
  this->isCover_ = 0;
  this->hasWeight_ = 0;
  this->hasCap_ = 0;
  this->hasVoltage_ = 0;
  this->xTalk_ = -1;
  for (i = 0;
       i < this->numVpins_;
       i++) {
    vp = this->vpins_[i];
    defiVpin_Destroy(vp);
    free((char *) vp);
  }
  this->numVpins_ = 0;
  for (i = 0;
       i < this->numProps_;
       i++) {
    free(this->propNames_[i]);
    free(this->propValues_[i]);
    this->propNames_[i] = 0;
    this->propValues_[i] = 0;
  }
  this->numProps_ = 0;
  for (i = 0;
       i < this->numPins_;
       i++) {
    free(this->instances_[i]);
    free(this->pins_[i]);
    this->instances_[i] = 0;
    this->pins_[i] = 0;
    this->musts_[i] = 0;
    this->synthesized_[i] = 0;
  }
  this->numPins_ = 0;
  for (i = 0;
       i < this->numSubnets_;
       i++) {
    s = this->subnets_[i];
    defiSubnet_Destroy(s);
    free((char *) (this->subnets_[i]));
    this->subnets_[i] = 0;
  }
  this->numSubnets_ = 0;
  if (this->name_)
    this->name_[0] = '\0';
  if (this->paths_) {
    for (i = 0;
	 i < this->numPaths_;
	 i++) {
      p = this->paths_[i];
      defiPath_Destroy(p);
      free((char *) p);
    }
    free((char *) (this->paths_));
    this->paths_ = 0;
    this->numPaths_ = 0;
    this->pathsAllocated_ = 0;
  }
  if (this->source_) {
    free(this->source_);
    this->source_ = 0;
  }
  if (this->pattern_) {
    free(this->pattern_);
    this->pattern_ = 0;
  }
  if (this->style_) {
    free(this->style_);
    this->style_ = 0;
  }
  if (this->original_) {
    free(this->original_);
    this->original_ = 0;
  }
  if (this->use_) {
    free(this->use_);
    this->use_ = 0;
  }
  if (this->nonDefaultRule_) {
    free(this->nonDefaultRule_);
    this->nonDefaultRule_ = 0;
  }
  if (this->numShields_) {
    for (i = 0;
	 i < this->numShields_;
	 i++) {
      defiShield_Destroy(this->shields_[i]);
      free((char *) (this->shields_[i]));
      this->shields_[i] = 0;
    }
    free((char *) (this->shields_));
    this->shields_ = 0;
    this->numShields_ = 0;
    this->shieldsAllocated_ = 0;
  }
  if (this->numWidths_) {
    for (i = 0;
	 i < this->numWidths_;
	 i++)
      free(this->wlayers_[i]);
    this->numWidths_ = 0;
  }
  if (this->numSpacing_) {
    for (i = 0;
	 i < this->numSpacing_;
	 i++)
      free(this->slayers_[i]);
    this->numSpacing_ = 0;
  }
  if (this->numShieldNet_) {
    for (i = 0;
	 i < this->numShieldNet_;
	 i++)
      free(this->shieldNet_[i]);
    this->numShieldNet_ = 0;
  }
}

int 
defiNet_hasSource(const defiNet * this)
{
  return this->source_ ? 1 : 0;
}

int 
defiNet_hasPattern(const defiNet * this)
{
  return this->pattern_ ? 1 : 0;
}

int 
defiNet_hasOriginal(const defiNet * this)
{
  return this->original_ ? 1 : 0;
}

int 
defiNet_hasCap(const defiNet * this)
{
  return (int) (this->hasCap_);
}

int 
defiNet_hasUse(const defiNet * this)
{
  return this->use_ ? 1 : 0;
}

int 
defiNet_hasStyle(const defiNet * this)
{
  return this->style_ ? 1 : 0;
}

int 
defiNet_hasXTalk(const defiNet * this)
{
  return (this->xTalk_ != -1) ? 1 : 0;
}

int 
defiNet_hasNonDefaultRule(const defiNet * this)
{
  return this->nonDefaultRule_ ? 1 : 0;
}

void 
defiNet_setSource(defiNet * this,
		  const char *typ)
{
  int     len;

  if (this->source_)
    free(this->source_);
  len = strlen(typ) + 1;
  this->source_ = (char *) malloc(len);
  strcpy(this->source_, DEFCASE(typ));
}

void 
defiNet_setOriginal(defiNet * this,
		    const char *typ)
{
  int     len;

  if (this->original_)
    free(this->original_);
  len = strlen(typ) + 1;
  this->original_ = (char *) malloc(len);
  strcpy(this->original_, DEFCASE(typ));
}

void 
defiNet_setPattern(defiNet * this,
		   const char *typ)
{
  int     len;

  if (this->pattern_)
    free(this->pattern_);
  len = strlen(typ) + 1;
  this->pattern_ = (char *) malloc(len);
  strcpy(this->pattern_, DEFCASE(typ));
}

void 
defiNet_setCap(defiNet * this,
	       double w)
{
  this->cap_ = w;
  this->hasCap_ = 1;
}

void 
defiNet_setUse(defiNet * this,
	       const char *typ)
{
  int     len;

  if (this->use_)
    free(this->use_);
  len = strlen(typ) + 1;
  this->use_ = (char *) malloc(len);
  strcpy(this->use_, DEFCASE(typ));
}

void 
defiNet_setStyle(defiNet * this,
		 const char *typ)
{
  int     len;

  if (this->style_)
    free(this->style_);
  len = strlen(typ) + 1;
  this->style_ = (char *) malloc(len);
  strcpy(this->style_, DEFCASE(typ));
}

void 
defiNet_setNonDefaultRule(defiNet * this,
			  const char *typ)
{
  int     len;

  if (this->nonDefaultRule_)
    free(this->nonDefaultRule_);
  len = strlen(typ) + 1;
  this->nonDefaultRule_ = (char *) malloc(len);
  strcpy(this->nonDefaultRule_, DEFCASE(typ));
}

const char *
defiNet_source(const defiNet * this)
{
  return this->source_;
}

const char *
defiNet_original(const defiNet * this)
{
  return this->original_;
}

const char *
defiNet_pattern(const defiNet * this)
{
  return this->pattern_;
}

double 
defiNet_cap(const defiNet * this)
{
  return (this->hasCap_ ? this->cap_ : 0.0);
}

const char *
defiNet_use(const defiNet * this)
{
  return this->use_;
}

const char *
defiNet_style(const defiNet * this)
{
  return this->style_;
}

const char *
defiNet_shieldNet(const defiNet * this,
		  int index)
{
  return this->shieldNet_[index];
}

const char *
defiNet_nonDefaultRule(const defiNet * this)
{
  return this->nonDefaultRule_;
}

void 
defiNet_bumpPaths(defiNet * this,
		  int size)
{
  int     i;

  defiPath **newPaths = (defiPath **) malloc(sizeof(defiPath) * size);

  for (i = 0;
       i < this->numPaths_;
       i++)
    newPaths[i] = this->paths_[i];
  this->pathsAllocated_ = size;
  free((char *) (this->paths_));
  this->paths_ = newPaths;
}

int 
defiNet_numPaths(const defiNet * this)
{
  return this->numPaths_;
}

defiPath *
defiNet_path(defiNet * this,
	     int index)
{
  if (index >= 0 && index < this->numPaths_)
    return this->paths_[index];
  return 0;
}

void 
defiNet_bumpShieldNets(defiNet * this,
		       int size)
{
  char  **newShieldNets = (char **) malloc(sizeof(char *) * size);

  int     i;

  if (this->shieldNet_) {
    for (i = 0;
	 i < this->shieldNetsAllocated_;
	 i++) {
      newShieldNets[i] = this->shieldNet_[i];
    }
    free((char *) (this->shieldNet_));
  }
  this->shieldNet_ = newShieldNets;
  this->shieldNetsAllocated_ = size;
}

int 
defiNet_numShields(const defiNet * this)
{
  return this->numShields_;
}

defiShield *
defiNet_shield(defiNet * this,
	       int index)
{
  if (index >= 0 && index < this->numShields_)
    return this->shields_[index];
  return 0;
}

int 
defiNet_numNoShields(const defiNet * this)
{
  return this->numShields_;
}

defiShield *
defiNet_noShield(defiNet * this,
		 int index)
{
  if (index >= 0 && index < this->numShields_)
    return this->shields_[index];
  return 0;
}

int 
defiNet_hasVoltage(const defiNet * this)
{
  return (int) (this->hasVoltage_);
}

double 
defiNet_voltage(const defiNet * this)
{
  return this->voltage_;
}

int 
defiNet_numWidthRules(const defiNet * this)
{
  return this->numWidths_;
}

int 
defiNet_numSpacingRules(const defiNet * this)
{
  return this->numSpacing_;
}

int 
defiNet_hasWidthRules(const defiNet * this)
{
  return this->numWidths_;
}

int 
defiNet_hasSpacingRules(const defiNet * this)
{
  return this->numSpacing_;
}

void 
defiNet_setXTalk(defiNet * this,
		 int i)
{
  this->xTalk_ = i;
}

int 
defiNet_XTalk(const defiNet * this)
{
  return this->xTalk_;
}

void 
defiNet_addVpin(defiNet * this,
		const char *name)
{
  defiVpin *vp;

  if (this->numVpins_ == this->vpinsAllocated_) {
    defiVpin **array;

    int     i;

    this->vpinsAllocated_ = this->vpinsAllocated_ ? this->vpinsAllocated_ * 2 : 2;
    array = (defiVpin **) malloc(sizeof(defiVpin *) * this->vpinsAllocated_);
    for (i = 0;
	 i < this->numVpins_;
	 i++)
      array[i] = this->vpins_[i];
    if (this->vpins_)
      free((char *) (this->vpins_));
    this->vpins_ = array;
  }
  vp = this->vpins_[this->numVpins_] = (defiVpin *) malloc(sizeof(defiVpin));
  this->numVpins_ += 1;
  defiVpin_Init(vp,
		name);
}

void 
defiNet_addVpinLayer(defiNet * this,
		     const char *name)
{
  defiVpin *vp = this->vpins_[this->numVpins_ - 1];

  defiVpin_setLayer(vp,
		    name);
}

void 
defiNet_addVpinLoc(defiNet * this,
		   const char *status,
		   int x,
		   int y,
		   int orient)
{
  defiVpin *vp = this->vpins_[this->numVpins_ - 1];

  defiVpin_setStatus(vp,
		     *status);
  defiVpin_setLoc(vp,
		  x,
		  y);
  defiVpin_setOrient(vp,
		     orient);
}

void 
defiNet_addVpinBounds(defiNet * this,
		      int xl,
		      int yl,
		      int xh,
		      int yh)
{
  defiVpin *vp = this->vpins_[this->numVpins_ - 1];

  defiVpin_setBounds(vp,
		     xl,
		     yl,
		     xh,
		     yh);
}

int 
defiNet_numVpins(const defiNet * this)
{
  return this->numVpins_;
}

defiVpin *
defiNet_vpin(const defiNet * this,
	     int index)
{
  if (index < 0 || index >= this->numVpins_)
    return 0;
  return this->vpins_[index];
}

void 
defiNet_spacingRule(defiNet * this,
		    int index,
		    char **layer,
		    double *dist,
		    double *left,
		    double *right)
{
  if (index >= 0 && index < this->numSpacing_) {
    if (layer)
      *layer = this->slayers_[index];
    if (dist)
      *dist = this->sdist_[index];
    if (left)
      *left = this->sleft_[index];
    if (right)
      *right = this->sright_[index];
  }
}

void 
defiNet_widthRule(defiNet * this,
		  int index,
		  char **layer,
		  double *dist)
{
  if (index >= 0 && index < this->numWidths_) {
    if (layer)
      *layer = this->wlayers_[index];
    if (dist)
      *dist = this->wdist_[index];
  }
}

void 
defiNet_setVoltage(defiNet * this,
		   double v)
{
  this->voltage_ = v;
  this->hasVoltage_ = 1;
}

void 
defiNet_setWidth(defiNet * this,
		 const char *layer,
		 double d)
{
  int     len = strlen(layer) + 1;

  char   *l = (char *) malloc(len);

  strcpy(l, DEFCASE(layer));
  if (this->numWidths_ >= this->widthsAllocated_) {
    int     i;

    char  **nl;

    double *nd;

    this->widthsAllocated_ = this->widthsAllocated_ ? this->widthsAllocated_ * 2 : 4;
    nl = (char **) malloc(sizeof(char *) * this->widthsAllocated_);
    nd = (double *) malloc(sizeof(double) * this->widthsAllocated_);
    for (i = 0;
	 i < this->numWidths_;
	 i++) {
      nl[i] = this->wlayers_[i];
      nd[i] = this->wdist_[i];
    }
    free((char *) (this->wlayers_));
    free((char *) (this->wdist_));
    this->wlayers_ = nl;
    this->wdist_ = nd;
  }
  this->wlayers_[this->numWidths_] = l;
  this->wdist_[this->numWidths_] = d;
  (this->numWidths_)++;
}

void 
defiNet_setSpacing(defiNet * this,
		   const char *layer,
		   double d)
{
  int     len = strlen(layer) + 1;

  char   *l = (char *) malloc(len);

  strcpy(l, DEFCASE(layer));
  if (this->numSpacing_ >= this->spacingAllocated_) {
    int     i;

    char  **nl;

    double *nd;

    double *n1;

    double *n2;

    this->spacingAllocated_ = this->spacingAllocated_ ? this->spacingAllocated_ * 2 : 4;
    nl = (char **) malloc(sizeof(char *) * this->spacingAllocated_);
    nd = (double *) malloc(sizeof(double) * this->spacingAllocated_);
    n1 = (double *) malloc(sizeof(double) * this->spacingAllocated_);
    n2 = (double *) malloc(sizeof(double) * this->spacingAllocated_);
    for (i = 0;
	 i < this->numSpacing_;
	 i++) {
      nl[i] = this->slayers_[i];
      nd[i] = this->sdist_[i];
      n1[i] = this->sleft_[i];
      n2[i] = this->sright_[i];
    }
    free((char *) (this->slayers_));
    free((char *) (this->sdist_));
    free((char *) (this->sleft_));
    free((char *) (this->sright_));
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

void 
defiNet_setRange(defiNet * this,
		 double left,
		 double right)
{
  this->sleft_[this->numSpacing_ - 1] = left;
  this->sright_[this->numSpacing_ - 1] = right;
}
