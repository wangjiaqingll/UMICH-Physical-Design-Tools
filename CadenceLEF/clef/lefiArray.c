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
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "lefiArray.h"
#include "lefiMisc.h"
#include "lefiDebug.h"

/*
 * lefiArrayFloorPlan
 */ void 
lefiArrayFloorPlan_Init(lefiArrayFloorPlan * this,
			const char *name)
{
  int     len = strlen(name) + 1;

  this->name_ = (char *) malloc(len);
  strcpy(this->name_, CASE(name));
  this->numPatterns_ = 0;
  this->patternsAllocated_ = 2;
  this->types_ = (char **) malloc(sizeof(char *) * 2);
  this->patterns_ = (lefiSitePattern **) malloc(sizeof(lefiSitePattern *) * 2);
}

void 
lefiArrayFloorPlan_Destroy(lefiArrayFloorPlan * this)
{
  lefiSitePattern *s;

  int     i;

  for (i = 1;
       i < this->numPatterns_;
       i++) {
    s = this->patterns_[i];
    lefiSitePattern_Destroy(s);
    free((char *) s);
    free((char *) (this->types_[i]));
  }
  free((char *) (this->types_));
  free((char *) (this->patterns_));
  free(this->name_);
}

void 
lefiArrayFloorPlan_addSitePattern(lefiArrayFloorPlan * this,
				  const char *typ,
				  lefiSitePattern * s)
{
  int     len = strlen(typ) + 1;

  if (this->numPatterns_ == this->patternsAllocated_) {
    int     i;

    int     lim = this->patternsAllocated_ = this->patternsAllocated_ * 2;

    char  **nc = (char **) malloc(sizeof(char *) * lim);

    lefiSitePattern **np = (lefiSitePattern **) malloc(sizeof(lefiSitePattern *) * lim);

    lim /= 2;
    for (i = 0;
	 i < lim;
	 i++) {
      nc[i] = this->types_[i];
      np[i] = this->patterns_[i];
    }
    free((char *) (this->types_));
    free((char *) (this->patterns_));
    this->types_ = nc;
    this->patterns_ = np;
  }
  this->types_[this->numPatterns_] = (char *) malloc(len);
  strcpy(this->types_[this->numPatterns_], typ);
  this->patterns_[this->numPatterns_] = s;
  this->numPatterns_ += 1;
}

int 
lefiArrayFloorPlan_numPatterns(const lefiArrayFloorPlan * this)
{
  return this->numPatterns_;
}

lefiSitePattern *
lefiArrayFloorPlan_pattern(lefiArrayFloorPlan * this,
			   int index)
{
  return this->patterns_[index];
}

char   *
lefiArrayFloorPlan_typ(lefiArrayFloorPlan * this,
		       int index)
{
  return this->types_[index];
}

const char *
lefiArrayFloorPlan_name(lefiArrayFloorPlan * this)
{
  return this->name_;
}

/*
 * lefiArray
 */ lefiArray *
lefiArray_CreateUninitialized()
{
  lefiArray *this = (lefiArray *) malloc(sizeof(lefiArray));

  lefiArray_Init(this);
  return (this);
}

void 
lefiArray_Init(lefiArray * this)
{
  this->nameSize_ = 16;
  this->name_ = (char *) malloc(16);
  this->numPatterns_ = 0;
  this->patternsAllocated_ = 0;
  lefiArray_bump(this,
		 (void ***) (&(this->pattern_)), this->numPatterns_, &(this->patternsAllocated_));
  this->numCan_ = 0;
  this->canAllocated_ = 0;
  lefiArray_bump(this,
    (void ***) (&(this->canPlace_)), this->numCan_, &(this->canAllocated_));
  this->numCannot_ = 0;
  this->cannotAllocated_ = 0;
  lefiArray_bump(this,
		 (void ***) (&(this->cannotOccupy_)), this->numCannot_, &(this->cannotAllocated_));
  this->numTracks_ = 0;
  this->tracksAllocated_ = 0;
  lefiArray_bump(this,
  (void ***) (&(this->track_)), this->numTracks_, &(this->tracksAllocated_));
  this->numG_ = 0;
  this->gAllocated_ = 0;
  lefiArray_bump(this,
	   (void ***) (&(this->gcell_)), this->numG_, &(this->gAllocated_));
  this->numDefault_ = 0;
  this->defaultAllocated_ = 4;
  this->minPins_ = (int *) malloc(sizeof(int) * 4);
  this->caps_ = (double *) malloc(sizeof(double) * 4);
  this->floorPlansAllocated_ = 0;
  this->numFloorPlans_ = 0;
  lefiArray_bump(this,
		 (void ***) (&(this->floors_)), this->numFloorPlans_, &(this->floorPlansAllocated_));
}

void 
lefiArray_Destroy(lefiArray * this)
{
  lefiArray_clear(this);
  free((char *) (this->name_));
  free((char *) (this->caps_));
  free((char *) (this->minPins_));
  free((char *) (this->floors_));
  free((char *) (this->track_));
  free((char *) (this->cannotOccupy_));
  free((char *) (this->canPlace_));
  free((char *) (this->pattern_));
}

void
lefiArray_Delete(lefiArray * this)
{
  if (this == NULL)
    return;
  lefiArray_Destroy(this);
  free(this);
}

void 
lefiArray_addSitePattern(lefiArray * this,
			 lefiSitePattern * s)
{
  if (this->numPatterns_ == this->patternsAllocated_)
    lefiArray_bump(this,
		   (void ***) (&(this->pattern_)), this->numPatterns_, &(this->patternsAllocated_));
  this->pattern_[this->numPatterns_] = s;
  this->numPatterns_ += 1;
}

void 
lefiArray_setName(lefiArray * this,
		  const char *name)
{
  int     len = strlen(name) + 1;

  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char *) malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
}

void 
lefiArray_setTableSize(lefiArray * this,
		       int tsize)
{
  this->tableSize_ = tsize;
  this->hasDefault_ = 1;
}

void 
lefiArray_addDefaultCap(lefiArray * this,
			int minPins,
			double cap)
{
  if (this->numDefault_ == this->defaultAllocated_) {
    int     i;

    int     lim;

    double *nc;

    int    *np;

    lim = this->defaultAllocated_ * 2;
    this->defaultAllocated_ = lim;
    nc = (double *) malloc(sizeof(double) * lim);
    np = (int *) malloc(sizeof(int) * lim);
    lim /= 2;
    for (i = 0;
	 i < lim;
	 i++) {
      nc[i] = this->caps_[i];
      np[i] = this->minPins_[i];
    }
    free((char *) (this->caps_));
    free((char *) (this->minPins_));
    this->caps_ = nc;
    this->minPins_ = np;
  }
  this->caps_[this->numDefault_] = cap;
  this->minPins_[this->numDefault_] = minPins;
  this->numDefault_ += 1;
}

void 
lefiArray_addCanPlace(lefiArray * this,
		      lefiSitePattern * s)
{
  if (this->numCan_ == this->canAllocated_)
    lefiArray_bump(this,
    (void ***) (&(this->canPlace_)), this->numCan_, &(this->canAllocated_));
  this->canPlace_[this->numCan_] = s;
  this->numCan_ += 1;
}

void 
lefiArray_addCannotOccupy(lefiArray * this,
			  lefiSitePattern * s)
{
  if (this->numCannot_ == this->cannotAllocated_)
    lefiArray_bump(this,
		   (void ***) (&(this->cannotOccupy_)), this->numCannot_, &(this->cannotAllocated_));
  this->cannotOccupy_[this->numCannot_] = s;
  this->numCannot_ += 1;
}

void 
lefiArray_addTrack(lefiArray * this,
		   lefiTrackPattern * t)
{
  if (this->numTracks_ == this->tracksAllocated_)
    lefiArray_bump(this,
		   (void ***) (&(this->track_)), this->numTracks_, &(this->tracksAllocated_));
  this->track_[this->numTracks_] = t;
  this->numTracks_ += 1;
}

void 
lefiArray_addGcell(lefiArray * this,
		   lefiGcellPattern * g)
{
  if (this->numG_ == this->gAllocated_)
    lefiArray_bump(this,
	   (void ***) (&(this->gcell_)), this->numG_, &(this->gAllocated_));
  this->gcell_[this->numG_] = g;
  this->numG_ += 1;
}

void 
lefiArray_addFloorPlan(lefiArray * this,
		       const char *name)
{
  lefiArrayFloorPlan *f;

  if (this->numFloorPlans_ == this->floorPlansAllocated_) {
    lefiArray_bump(this,
		   (void ***) (&(this->floors_)), this->numFloorPlans_, &(this->floorPlansAllocated_));
  }
  f = (lefiArrayFloorPlan *) malloc(sizeof(lefiArrayFloorPlan));
  lefiArrayFloorPlan_Init(f,
			  name);
  this->floors_[this->numFloorPlans_] = f;
  this->numFloorPlans_ += 1;
}

void 
lefiArray_addSiteToFloorPlan(lefiArray * this,
			     const char *typ,
			     lefiSitePattern * s)
{
  lefiArrayFloorPlan *f = this->floors_[this->numFloorPlans_ - 1];

  lefiArrayFloorPlan_addSitePattern(f,
				    typ,
				    s);
}

void 
lefiArray_bump(lefiArray * this,
	       void ***arr,
	       int used,
	       int *allocated)
{
  int     size = *allocated * 2;

  int     i;

  void  **newa;

  if (size == 0)
    size = 2;
  newa = (void **) malloc(sizeof(void *) * size);
  for (i = 0;
       i < used;
       i++) {
    newa[i] = (*arr)[i];
  }
  *allocated = size;
  *arr = newa;
}

void 
lefiArray_clear(lefiArray * this)
{
  int     i;

  lefiSitePattern *p;

  lefiGcellPattern *g;

  lefiTrackPattern *t;

  lefiArrayFloorPlan *f;

  for (i = 0;
       i < this->numPatterns_;
       i++) {
    p = this->pattern_[i];
    lefiSitePattern_Destroy(p);
    free((char *) p);
  }
  this->numPatterns_ = 0;
  for (i = 0;
       i < this->numCan_;
       i++) {
    p = this->canPlace_[i];
    lefiSitePattern_Destroy(p);
    free((char *) p);
  }
  this->numCan_ = 0;
  for (i = 0;
       i < this->numCannot_;
       i++) {
    p = this->cannotOccupy_[i];
    lefiSitePattern_Destroy(p);
    free((char *) p);
  }
  this->numCannot_ = 0;
  for (i = 0;
       i < this->numTracks_;
       i++) {
    t = this->track_[i];
    lefiTrackPattern_Destroy(t);
    free((char *) t);
  }
  this->numTracks_ = 0;
  for (i = 0;
       i < this->numG_;
       i++) {
    g = this->gcell_[i];
    lefiGcellPattern_Destroy(g);
    free((char *) g);
  }
  this->numG_ = 0;
  this->hasDefault_ = 0;
  this->tableSize_ = 0;
  this->numDefault_ = 0;
  for (i = 0;
       i < this->numFloorPlans_;
       i++) {
    f = this->floors_[i];
    lefiArrayFloorPlan_Destroy(f);
    free((char *) f);
  }
  this->numFloorPlans_ = 0;
}

int 
lefiArray_numSitePattern(lefiArray * this)
{
  return this->numPatterns_;
}

int 
lefiArray_numCanPlace(lefiArray * this)
{
  return this->numCan_;
}

int 
lefiArray_numCannotOccupy(lefiArray * this)
{
  return this->numCannot_;
}

int 
lefiArray_numTrack(lefiArray * this)
{
  return this->numTracks_;
}

int 
lefiArray_numGcell(lefiArray * this)
{
  return this->numG_;
}

int 
lefiArray_hasDefaultCap(lefiArray * this)
{
  return this->hasDefault_;
}

const char *
lefiArray_name(lefiArray * this)
{
  return this->name_;
}

lefiSitePattern *
lefiArray_sitePattern(lefiArray * this,
		      int index)
{
  return this->pattern_[index];
}

lefiSitePattern *
lefiArray_canPlace(lefiArray * this,
		   int index)
{
  return this->canPlace_[index];
}

lefiSitePattern *
lefiArray_cannotOccupy(lefiArray * this,
		       int index)
{
  return this->cannotOccupy_[index];
}

lefiTrackPattern *
lefiArray_track(lefiArray * this,
		int index)
{
  return this->track_[index];
}

lefiGcellPattern *
lefiArray_gcell(lefiArray * this,
		int index)
{
  return this->gcell_[index];
}

int 
lefiArray_tableSize(lefiArray * this)
{
  return this->tableSize_;
}

int 
lefiArray_numDefaultCaps(lefiArray * this)
{
  return this->numDefault_;
}

int 
lefiArray_defaultCapMinPins(lefiArray * this,
			    int index)
{
  return this->minPins_[index];
}

double 
lefiArray_defaultCap(lefiArray * this,
		     int index)
{
  return this->caps_[index];
}

int 
lefiArray_numFloorPlans(lefiArray * this)
{
  return this->numFloorPlans_;
}

const char *
lefiArray_floorPlanName(lefiArray * this,
			int index)
{
  lefiArrayFloorPlan *f = this->floors_[index];

  return lefiArrayFloorPlan_name(f);
}

int 
lefiArray_numSites(lefiArray * this,
		   int index)
{
  lefiArrayFloorPlan *f = this->floors_[index];

  return lefiArrayFloorPlan_numPatterns(f);
}

const char *
lefiArray_siteType(lefiArray * this,
		   int index,
		   int j)
{
  lefiArrayFloorPlan *f = this->floors_[index];

  return lefiArrayFloorPlan_typ(f,
				j);
}

lefiSitePattern *
lefiArray_site(lefiArray * this,
	       int index,
	       int j)
{
  lefiArrayFloorPlan *f = this->floors_[index];

  return lefiArrayFloorPlan_pattern(f,
				    j);
}

void 
lefiArray_print(lefiArray * this,
		FILE * f)
{
  fprintf(f, "ARRAY %s\n", lefiArray_name(this));
}
