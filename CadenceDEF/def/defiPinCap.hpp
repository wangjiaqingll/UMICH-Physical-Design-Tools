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

#ifndef defiPinCap_h
#define defiPinCap_h


#include "defiKRDefs.hpp"
#include <stdio.h>

class defiPinCap {
public:

  void setPin(int p);
  void setCap(double d);

  int pin() const;
  double cap() const;

  void print(FILE* f) const;

protected:
  int pin_;        // pin num
  double cap_;     // capacitance
};



class defiPin {
public:
  defiPin();
  void Init();

  ~defiPin();
  void Destroy();

  void Setup(const char* pinName, const char* netName);
  void setDirection(const char* dir);
  void setUse(const char* use);
  void setLayer(const char* layer, int xl, int yl, int xh, int yh);
  void setPlacement(int typ, int x, int y, int orient);
  void setSpecial();
  void clear();

  const char* pinName() const;
  const char* netName() const;

  // optional parts
  int hasDirection() const;
  int hasUse() const;
  int hasLayer() const;
  int hasPlacement() const;
  int isPlaced() const;
  int isCover() const;
  int isFixed() const;
  int placementX() const;
  int placementY() const;
  const char* direction() const;
  const char* use() const;
  const char* layer() const;
  void bounds(int* xl, int* yl, int* xh, int* yh) const;
  int orient() const;
  int hasSpecial() const;

  void print(FILE* f) const;

protected:
  int pinNameLength_;    // allocated size of pin name
  char* pinName_;
  int netNameLength_;    // allocated size of net name
  char* netName_;
  char hasDirection_;
  char hasUse_;
  char hasLayer_;
  char placeType_;
  char orient_;          // orient 0-7
  int useLength_;        // allocated size of length
  char* use_;
  int directionLength_;  // allocated size of direction
  char* direction_;
  int layerLength_;    // allocated size of layer
  char* layer_;
  int xl_, yl_, xh_, yh_;
  int x_, y_;            // placement
  int hasSpecial_;
};

#endif
