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
#include "defiComponent.h"
#include "defiDebug.h"
defiComponent *
defiComponent_CreateUninitialized()
{
  defiComponent *this = (defiComponent *) malloc(sizeof(defiComponent));

  defiComponent_Init(this);
  return (this);
}

void 
defiComponent_Init(defiComponent * this)
{
  this->id_ = 0;
  this->name_ = 0;
  this->regionName_ = 0;
  this->foreignName_ = 0;
  this->Fori_ = 0;
  this->EEQ_ = 0;
  this->generateName_ = 0;
  this->macroName_ = 0;
  this->generateNameSize_ = 0;
  this->macroNameSize_ = 0;
  this->nets_ = 0;
  this->source_ = 0;
  this->numNets_ = 0;
  defiComponent_bumpName(this,
			 16);
  defiComponent_bumpId(this,
		       16);
  defiComponent_bumpRegionName(this,
			       16);
  defiComponent_bumpEEQ(this,
			16);
  defiComponent_bumpNets(this,
			 16);
  defiComponent_bumpForeignName(this,
				16);
  defiComponent_bumpFori(this,
			 16);
  this->numProps_ = 0;
  this->propsAllocated_ = 2;
  this->names_ = (char **) malloc(sizeof(char *) * 2);
  this->values_ = (char **) malloc(sizeof(char *) * 2);
  defiComponent_clear(this);
  this->numRects_ = 0;
  this->rectsAllocated_ = 1;
  this->rectXl_ = (int *) malloc(sizeof(int) * 1);
  this->rectYl_ = (int *) malloc(sizeof(int) * 1);
  this->rectXh_ = (int *) malloc(sizeof(int) * 1);
  this->rectYh_ = (int *) malloc(sizeof(int) * 1);
}

void 
defiComponent_Destroy(defiComponent * this)
{
  defiComponent_clear(this);
  free(this->name_);
  free(this->regionName_);
  free(this->id_);
  free(this->EEQ_);
  free((char *) (this->nets_));
  if (this->source_)
    free(this->source_);
  if (this->foreignName_)
    free(this->foreignName_);
  if (this->Fori_)
    free(this->Fori_);
  if (this->generateName_)
    free(this->generateName_);
  if (this->macroName_)
    free(this->macroName_);
  free((char *) (this->names_));
  free((char *) (this->values_));
  free((char *) (this->rectXl_));
  free((char *) (this->rectYl_));
  free((char *) (this->rectXh_));
  free((char *) (this->rectYh_));
}

void
defiComponent_Delete(defiComponent * this)
{
  if (this == NULL)
    return;
  defiComponent_Destroy(this);
  free(this);
}

void 
defiComponent_IdAndName(defiComponent * this,
			const char *id,
			const char *name)
{
  int     len;

  defiComponent_clear(this);
  if ((len = strlen(id) + 1) > this->idSize_)
    defiComponent_bumpId(this,
			 len);
  strcpy(this->id_, DEFCASE(id));
  if ((len = strlen(name) + 1) > this->nameSize_)
    defiComponent_bumpName(this,
			   len);
  strcpy(this->name_, DEFCASE(name));
}

const char *
defiComponent_source(const defiComponent * this)
{
  return this->source_;
}

int 
defiComponent_weight(const defiComponent * this)
{
  return this->weight_;
}

void 
defiComponent_setWeight(defiComponent * this,
			int w)
{
  this->weight_ = w;
  this->hasWeight_ = 1;
}

void 
defiComponent_setGenerate(defiComponent * this,
			  const char *newName,
			  const char *macroName)
{
  int     len = strlen(newName) + 1;

  if (this->generateNameSize_ < len) {
    if (this->generateName_)
      free(this->generateName_);
    this->generateName_ = (char *) malloc(len);
    this->generateNameSize_ = len;
  }
  strcpy(this->generateName_, DEFCASE(newName));
  len = strlen(macroName) + 1;
  if (this->macroNameSize_ < len) {
    if (this->macroName_)
      free(this->macroName_);
    this->macroName_ = (char *) malloc(len);
    this->macroNameSize_ = len;
  }
  strcpy(this->macroName_, DEFCASE(macroName));
}

void 
defiComponent_setSource(defiComponent * this,
			const char *name)
{
  int     len = strlen(name) + 1;

  this->source_ = (char *) malloc(len);
  strcpy(this->source_, DEFCASE(name));
}

void 
defiComponent_setRegionName(defiComponent * this,
			    const char *name)
{
  int     len;

  if ((len = strlen(name) + 1) > this->regionNameSize_)
    defiComponent_bumpRegionName(this,
				 len);
  strcpy(this->regionName_, DEFCASE(name));
  this->hasRegionName_ = 1;
}

void 
defiComponent_setEEQ(defiComponent * this,
		     const char *name)
{
  int     len;

  if ((len = strlen(name) + 1) > this->EEQSize_)
    defiComponent_bumpEEQ(this,
			  len);
  strcpy(this->EEQ_, DEFCASE(name));
  this->hasEEQ_ = 1;
}

void 
defiComponent_setPlacementStatus(defiComponent * this,
				 int n)
{
  this->status_ = n;
}

void 
defiComponent_setPlacementLocation(defiComponent * this,
				   int x,
				   int y,
				   int orient)
{
  this->x_ = x;
  this->y_ = y;
  this->orient_ = orient;
}

void 
defiComponent_setRegionBounds(defiComponent * this,
			      int xl,
			      int yl,
			      int xh,
			      int yh)
{
  int     i;

  i = this->numRects_;
  if (i == this->rectsAllocated_) {
    int     max = this->rectsAllocated_ * 2;

    int    *nxl = (int *) malloc(sizeof(int) * max);

    int    *nyl = (int *) malloc(sizeof(int) * max);

    int    *nxh = (int *) malloc(sizeof(int) * max);

    int    *nyh = (int *) malloc(sizeof(int) * max);

    for (i = 0;
	 i < this->numRects_;
	 i++) {
      nxl[i] = this->rectXl_[i];
      nyl[i] = this->rectYl_[i];
      nxh[i] = this->rectXh_[i];
      nyh[i] = this->rectYh_[i];
    }
    free((char *) (this->rectXl_));
    free((char *) (this->rectYl_));
    free((char *) (this->rectXh_));
    free((char *) (this->rectYh_));
    this->rectXl_ = nxl;
    this->rectYl_ = nyl;
    this->rectXh_ = nxh;
    this->rectYh_ = nyh;
    this->rectsAllocated_ = max;
  }
  this->rectXl_[i] = xl;
  this->rectYl_[i] = yl;
  this->rectXh_[i] = xh;
  this->rectYh_[i] = yh;
  this->numRects_ += 1;
}

const char *
defiComponent_id(const defiComponent * this)
{
  return this->id_;
}

const char *
defiComponent_name(const defiComponent * this)
{
  return this->name_;
}

int 
defiComponent_placementStatus(const defiComponent * this)
{
  return this->status_;
}

int 
defiComponent_placementX(const defiComponent * this)
{
  return this->x_;
}

int 
defiComponent_placementY(const defiComponent * this)
{
  return this->y_;
}

int 
defiComponent_placementOrient(const defiComponent * this)
{
  return this->orient_;
}

const char *
defiComponent_regionName(const defiComponent * this)
{
  return this->regionName_;
}

const char *
defiComponent_EEQ(const defiComponent * this)
{
  return this->EEQ_;
}

const char *
defiComponent_generateName(const defiComponent * this)
{
  return this->generateName_;
}

const char *
defiComponent_macroName(const defiComponent * this)
{
  return this->macroName_;
}

void 
defiComponent_regionBounds(defiComponent * this,
			   int *size,
			   int **xl,
			   int **yl,
			   int **xh,
			   int **yh)
{
  *size = this->numRects_;
  *xl = this->rectXl_;
  *yl = this->rectYl_;
  *xh = this->rectXh_;
  *yh = this->rectYh_;
}

void 
defiComponent_bumpId(defiComponent * this,
		     int size)
{
  if (this->id_)
    free(this->id_);
  this->id_ = (char *) malloc(size);
  this->idSize_ = size;
  *(this->id_) = '\0';
}

void 
defiComponent_bumpName(defiComponent * this,
		       int size)
{
  if (this->name_)
    free(this->name_);
  this->name_ = (char *) malloc(size);
  this->nameSize_ = size;
  *(this->name_) = '\0';
}

void 
defiComponent_bumpRegionName(defiComponent * this,
			     int size)
{
  if (this->regionName_)
    free(this->regionName_);
  this->regionName_ = (char *) malloc(size);
  this->regionNameSize_ = size;
  *(this->regionName_) = '\0';
}

void 
defiComponent_bumpEEQ(defiComponent * this,
		      int size)
{
  if (this->EEQ_)
    free(this->EEQ_);
  this->EEQ_ = (char *) malloc(size);
  this->EEQSize_ = size;
  *(this->EEQ_) = '\0';
}

void 
defiComponent_clear(defiComponent * this)
{
  int     i;

  if (this->id_)
    *(this->id_) = '\0';
  if (this->name_)
    *(this->name_) = '\0';
  if (this->regionName_)
    *(this->regionName_) = '\0';
  if (this->foreignName_)
    *(this->foreignName_) = '\0';
  if (this->Fori_)
    *(this->Fori_) = '\0';
  if (this->EEQ_)
    *(this->EEQ_) = '\0';
  this->status_ = DEFI_COMPONENT_UNPLACED;
  this->hasRegionName_ = 0;
  this->hasForeignName_ = 0;
  this->hasFori_ = 0;
  this->hasEEQ_ = 0;
  this->hasWeight_ = 0;
  this->hasGenerate_ = 0;
  if (this->source_)
    free(this->source_);
  for (i = 0;
       i < this->numNets_;
       i++) {
    free(this->nets_[i]);
  }
  this->numNets_ = 0;
  this->source_ = 0;
  for (i = 0;
       i < this->numProps_;
       i++) {
    free(this->names_[i]);
    free(this->values_[i]);
  }
  this->numProps_ = 0;
  this->numRects_ = 0;
}

int 
defiComponent_isPlaced(const defiComponent * this)
{
  return this->status_ == DEFI_COMPONENT_PLACED ? 1 : 0;
}

int 
defiComponent_isFixed(const defiComponent * this)
{
  return this->status_ == DEFI_COMPONENT_FIXED ? 1 : 0;
}

int 
defiComponent_isCover(const defiComponent * this)
{
  return this->status_ == DEFI_COMPONENT_COVER ? 1 : 0;
}

void 
defiComponent_print(defiComponent * this,
		    FILE * fout)
{
  fprintf(fout, "Component id '%s' name '%s'", defiComponent_id(this), defiComponent_name(this));
  if (defiComponent_isPlaced(this)) {
    fprintf(fout, " Placed at %d,%d orient %d", defiComponent_placementX(this), defiComponent_placementY(this), defiComponent_placementOrient(this));
  }
  if (defiComponent_isFixed(this)) {
    fprintf(fout, " Fixed at %d,%d orient %d", defiComponent_placementX(this), defiComponent_placementY(this), defiComponent_placementOrient(this));
  }
  if (defiComponent_isCover(this)) {
    fprintf(fout, " Cover at %d,%d orient %d", defiComponent_placementX(this), defiComponent_placementY(this), defiComponent_placementOrient(this));
  }
  fprintf(fout, "\n");
  if (defiComponent_hasGenerate(this)) {
    fprintf(fout, " generate %s %s\n", defiComponent_generateName(this), defiComponent_macroName(this));
  }
  if (defiComponent_hasWeight(this)) {
    fprintf(fout, " weight %d\n", defiComponent_weight(this));
  }
  if (defiComponent_hasSource(this)) {
    fprintf(fout, " source '%s'\n", defiComponent_source(this));
  }
  if (defiComponent_hasEEQ(this)) {
    fprintf(fout, " EEQ '%s'\n", defiComponent_EEQ(this));
  }
  if (defiComponent_hasRegionName(this)) {
    fprintf(fout, " Region '%s'\n", defiComponent_regionName(this));
  }
  if (defiComponent_hasRegionBounds(this)) {
    int     size;

    int    *xl, *yl, *xh, *yh;

    int     j;

    defiComponent_regionBounds(this,
			       &size,
			       &xl,
			       &yl,
			       &xh,
			       &yh);
    for (j = 0;
	 j < size;
	 j++)
      fprintf(fout, " Region bounds %d,%d %d,%d\n", xl[j], yl[j], xh[j], yh[j]);
  }
  if (defiComponent_hasNets(this)) {
    int     i;

    fprintf(fout, " Net connections:\n");
    for (i = 0;
	 i < defiComponent_numNets(this);
	 i++) {
      fprintf(fout, " '%s'\n", defiComponent_net(this,
						 i));
    }
  }
}

int 
defiComponent_hasRegionName(const defiComponent * this)
{
  return (int) (this->hasRegionName_);
}

int 
defiComponent_hasGenerate(const defiComponent * this)
{
  return (int) (this->hasGenerate_);
}

int 
defiComponent_hasWeight(const defiComponent * this)
{
  return (int) (this->hasWeight_);
}

int 
defiComponent_hasSource(const defiComponent * this)
{
  return this->source_ ? 1 : 0;
}

int 
defiComponent_hasRegionBounds(const defiComponent * this)
{
  return this->numRects_ ? 1 : 0;
}

int 
defiComponent_hasEEQ(const defiComponent * this)
{
  return (int) (this->hasEEQ_);
}

int 
defiComponent_hasNets(const defiComponent * this)
{
  return this->numNets_ ? 1 : 0;
}

int 
defiComponent_numNets(const defiComponent * this)
{
  return this->numNets_;
}

void 
defiComponent_reverseNetOrder(defiComponent * this)
{
  int     one = 0;

  int     two = this->numNets_ - 1;

  char   *t;

  while (one < two) {
    t = this->nets_[one];
    this->nets_[one] = this->nets_[two];
    this->nets_[two] = t;
    one++;
    two--;
  }
}

char   *
defiComponent_propName(const defiComponent * this,
		       int index)
{
  return this->names_[index];
}

char   *
defiComponent_propValue(const defiComponent * this,
			int index)
{
  return this->values_[index];
}

int 
defiComponent_numProps(const defiComponent * this)
{
  return this->numProps_;
}

void 
defiComponent_addProperty(defiComponent * this,
			  const char *name,
			  const char *value)
{
  int     len = strlen(name) + 1;

  if (this->numProps_ == this->propsAllocated_) {
    int     i;

    char  **nn;

    char  **nv;

    this->propsAllocated_ *= 2;
    nn = (char **) malloc(sizeof(char *) * this->propsAllocated_);
    nv = (char **) malloc(sizeof(char *) * this->propsAllocated_);
    for (i = 0;
	 i < this->numProps_;
	 i++) {
      nn[i] = this->names_[i];
      nv[i] = this->values_[i];
    }
    free((char *) (this->names_));
    free((char *) (this->values_));
    this->names_ = nn;
    this->values_ = nv;
  }
  this->names_[this->numProps_] = (char *) malloc(len);
  strcpy(this->names_[this->numProps_], DEFCASE(name));
  len = strlen(value) + 1;
  this->values_[this->numProps_] = (char *) malloc(len);
  strcpy(this->values_[this->numProps_], DEFCASE(value));
  this->numProps_ += 1;
}

void 
defiComponent_addNet(defiComponent * this,
		     const char *net)
{
  int     len = strlen(net) + 1;

  if (this->numNets_ == this->netsAllocated_)
    defiComponent_bumpNets(this,
			   this->numNets_ * 2);
  this->nets_[this->numNets_] = (char *) malloc(len);
  strcpy(this->nets_[this->numNets_], DEFCASE(net));
  (this->numNets_)++;
}

void 
defiComponent_bumpNets(defiComponent * this,
		       int size)
{
  int     i;

  char  **newNets = (char **) malloc(sizeof(char *) * size);

  for (i = 0;
       i < this->numNets_;
       i++) {
    newNets[i] = this->nets_[i];
  }
  free((char *) (this->nets_));
  this->nets_ = newNets;
  this->netsAllocated_ = size;
}

const char *
defiComponent_net(const defiComponent * this,
		  int index)
{
  if (index >= 0 && index < this->numNets_) {
    return this->nets_[index];
  }
  return 0;
}

void 
defiComponent_bumpForeignName(defiComponent * this,
			      int size)
{
  if (this->foreignName_)
    free(this->foreignName_);
  this->foreignName_ = (char *) malloc(sizeof(char) * size);
  this->foreignNameSize_ = size;
  *(this->foreignName_) = '\0';
}

void 
defiComponent_setForeignName(defiComponent * this,
			     const char *name)
{
  int     len;

  if (defiComponent_hasForeignName(this))
    defiError("Multiple define of '+ FOREIGN' in COMPONENT is not supported.\n");
  if ((len = strlen(name) + 1) > this->foreignNameSize_)
    defiComponent_bumpForeignName(this,
				  len);
  strcpy(this->foreignName_, DEFCASE(name));
  this->hasForeignName_ = 1;
}

void 
defiComponent_bumpFori(defiComponent * this,
		       int size)
{
  if (this->Fori_)
    free(this->Fori_);
  this->Fori_ = (char *) malloc(sizeof(char) * size);
  this->ForiSize_ = size;
  *(this->Fori_) = '\0';
}

void 
defiComponent_setFori(defiComponent * this,
		      const char *name)
{
  int     len;

  if ((len = strlen(name) + 1) > this->ForiSize_)
    defiComponent_bumpFori(this,
			   len);
  strcpy(this->Fori_, DEFCASE(name));
  this->hasFori_ = 1;
}

void 
defiComponent_setForeignLocation(defiComponent * this,
				 int x,
				 int y,
				 char *ori)
{
  this->Fx_ = x;
  this->Fy_ = y;
  defiComponent_setFori(this,
			ori);
}

int 
defiComponent_hasForeignName(const defiComponent * this)
{
  return (int) (this->hasForeignName_);
}

const char *
defiComponent_foreignName(const defiComponent * this)
{
  return this->foreignName_;
}

int 
defiComponent_foreignX(const defiComponent * this)
{
  return this->Fx_;
}

int 
defiComponent_foreignY(const defiComponent * this)
{
  return this->Fy_;
}

int 
defiComponent_hasFori(const defiComponent * this)
{
  return (int) (this->hasFori_);
}

const char *
defiComponent_foreignOri(const defiComponent * this)
{
  return this->Fori_;
}
