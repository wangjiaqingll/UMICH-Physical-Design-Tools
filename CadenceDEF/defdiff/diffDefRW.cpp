//
//       Copyright (C) 1997 Cadence Design Systems Inc.
//                     All Rights Reserved.
//
 
// This program is the diffDef core program.  It has all the callback
// routines and write it out to a temporary file

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "defrReader.hpp"
#include "defiAlias.hpp"

char defaultName[64];
char defaultOut[64];

// Global variables
FILE* fout;
int userData;
int numObjs;
int isSumSet;      // to keep track if within SUM
int isProp = 0;    // for PROPERTYDEFINITIONS
int begOperand;    // to keep track for constraint, to print - as the 1st char

// TX_DIR:TRANSLATION ON

void dataError() {
  fprintf(fout, "ERROR: returned user data is not correct!\n");
}


void checkType(defrCallbackType_e c) {
  if (c >= 0 && c <= defrDesignEndCbkType) {
    // OK
  } else {
    fprintf(fout, "ERROR: callback type is out of bounds!\n");
  }
}


// Component
int compf(defrCallbackType_e c, defiComponent* co, defiUserData ud) {
  int i;

  checkType(c);
  if ((long)ud != userData) dataError();
//  missing GENERATE, FOREIGN
    fprintf(fout, "COMPONENT %s %s", co->id(), co->name());
    if (co->hasNets()) {
        for (i = 0; i < co->numNets(); i++)
             fprintf(fout, " %s", co->net(i));
        fprintf(fout,"\n");
    } else
        fprintf(fout,"\n");
    if (co->isFixed()) 
        fprintf(fout, "COMPONENT %s FIXED ( %d %d ) %d\n", co->id(),
                co->placementX(), co->placementY(), co->placementOrient());
    if (co->isCover()) 
        fprintf(fout, "COMPONENT %s COVER ( %d %d ) %d\n", co->id(),
                co->placementX(), co->placementY(), co->placementOrient());
    if (co->isPlaced()) 
        fprintf(fout,"COMPONENT %s PLACED ( %d %d ) %d\n", co->id(),
                co->placementX(), co->placementY(), co->placementOrient());
    if (co->hasSource())
        fprintf(fout, "COMPONENT %s SOURCE %s\n", co->id(), co->source());
    if (co->hasGenerate())
        fprintf(fout, "COMPONENT %s GENERATE %s %s\n", co->id(),
                co->generateName(), co->macroName());
    if (co->hasForeignName())
        fprintf(fout, "COMPONENT %s FOREIGN %s %d %d %s\n", co->id(),
                co->foreignName(), co->foreignX(), co->foreignY(),
                co->foreignOri());
    if (co->hasWeight())
        fprintf(fout, "COMPONENT %s WEIGHT %d\n", co->id(), co->weight());
    if (co->hasEEQ())
        fprintf(fout, "COMPONENT %s EEQMASTER %s\n", co->id(), co->EEQ());
    if (co->hasRegionName())
        fprintf(fout, "COMPONENT %s REGION %s\n", co->id(), co->regionName());
    if (co->hasRegionBounds()) {
        int *xl, *yl, *xh, *yh;
        int size;
        co->regionBounds(&size, &xl, &yl, &xh, &yh);
        for (i = 0; i < size; i++) { 
            fprintf(fout, "COMPONENT %s REGION ( %d %d ) ( %d %d )\n", co->id(),
                    co->id(), xl[i], yl[i], xh[i], yh[i]);
        }
    }
    --numObjs;
  return 0;
}

// Net
int netf(defrCallbackType_e c, defiNet* net, defiUserData ud) {
  // For net and special net.
  int i, j, x, y, count, newLayer;
  defiPath* p;
  defiSubnet *s;
  int path;

  checkType(c);
  if ((long)ud != userData) dataError();
  if (c != defrNetCbkType)
      fprintf(fout, "BOGUS NET TYPE  ");
  if (net->pinIsMustJoin(0))
      fprintf(fout, "NET MUSTJOIN ");
  else
      fprintf(fout, "NET %s ", net->name());
 
  count = 0;
  // compName & pinName
  for (i = 0; i < net->numConnections(); i++) {
      // set the limit of only 5 items per line
      count++;
      if (count >= 5) {
          fprintf(fout, "\nNET %s ", net->name());
          count = 0;
      }
      fprintf(fout, "( %s %s ) ", net->instance(i), net->pin(i));
  }

  // regularWiring
  if (net->isFixed())
    fprintf(fout, " FIXED ");
  if (net->isRouted())
    fprintf(fout, " ROUTED ");
  if (net->isCover())
    fprintf(fout, " COVER ");
 
  if (net->numPaths()) {
    newLayer = 0;
    for (i = 0; i < net->numPaths(); i++) {
      p = net->path(i);
      p->initTraverse();
      while ((path = (int)p->next()) != DEFIPATH_DONE) {
        count++;
        // Don't want the line to be too long
        if (count >= 5) {
            fprintf(fout, "\nNET %s", net->name());
            count = 0;
        } 
        switch (path) {
          case DEFIPATH_LAYER:
               if (newLayer == 0) {
                   fprintf(fout, "%s ", p->getLayer());
                   newLayer = 1;
               } else
                   fprintf(fout, "NEW %s ", p->getLayer());
               break;
          case DEFIPATH_VIA:
               fprintf(fout, "%s ", p->getVia());
               break;
          case DEFIPATH_WIDTH:
               fprintf(fout, "%d ", p->getWidth());
               break;
          case DEFIPATH_POINT:
               p->getPoint(&x, &y);
               fprintf(fout, "( %d %d ) ", x, y);
               break;
          case DEFIPATH_TAPER:
               fprintf(fout, "TAPER ");
               break;
        }
      }
    }
  }

  if (net->hasSubnets()) {
    for (i = 0; i < net->numSubnets(); i++) {
      s = net->subnet(i);
      if (s->isFixed())
        fprintf(fout, "NET %s FIXED", s->name());
      if (s->isRouted())
        fprintf(fout, "NET %s ROUTED", s->name());
      if (s->isCover())
        fprintf(fout, "NET %s COVER", s->name());
      fprintf(fout, "\n");
 
      if (s->numConnections()) {
          if (s->pinIsMustJoin(0))
              fprintf(fout, "NET MUSTJOIN ");
          else
              fprintf(fout, "NET %s ", s->name());
          for (j = 0; j < s->numConnections(); j++) {
              fprintf(fout, " ( %s %s )", s->instance(j),
                      s->pin(j));
        }
        fprintf(fout, "\n");
      }
 
      if (s->numPaths()) {
         for (j = 0; j < s->numPaths(); j++) {
            int elem;
            p = s->path(j);
            p->initTraverse();
            elem = p->next();
            while (elem) { 
               switch (elem) {
                  case DEFIPATH_LAYER:
                       fprintf(fout, "NET %s LAYER %s\n", s->name(),
                               p->getLayer());
                       break;
                  case DEFIPATH_VIA:
                       fprintf(fout, "NET %s VIA %s\n", s->name(),
                               p->getVia());
                       break;
                  case DEFIPATH_WIDTH:
                       fprintf(fout, "NET %s WIDTH %d\n", s->name(),
                               p->getWidth());
                       break;
                  case DEFIPATH_POINT:
                       p->getPoint(&x, &y);
                       fprintf(fout, "NET %s POINT %d %d\n", s->name(), x, y);
                       break;
                  //case DEFIPATH_FLUSHPOINT:
                       //l = 0;
                       //p->getFlushPoint(i1, i2, ext);
                       //while (i1[l] && i2[l] && ext[l]) {
                          //fprintf(fout, "NET %s FLUSHPOINT %d %d %d\n",
                                  //s->name(), i1[l], i2[l], ext[l]);
                          //l++;
                       //}
                       //break;
                  case DEFIPATH_TAPERRULE:
                       fprintf(fout, "NET %s TAPERRULE %s\n", s->name(),
                               p->getTaperRule());
                       break;
                  case DEFIPATH_SHAPE:
                       fprintf(fout, "NET %s SHAPE %s\n", s->name(),
                               p->getShape());
                       break;
               }
               elem = p->next();
            }
        }
      }
    }
    // Since a new line is there, make sure the pin name is printed
    fprintf(fout, "NET %s", s->name());
  }

  if (net->hasWeight())
    fprintf(fout, " WEIGHT %d ", net->weight());
  if (net->hasCap())
    fprintf(fout, " ESTCAP %g ", net->cap());
  if (net->hasSource())
    fprintf(fout, " SOURCE %s ", net->source());
  if (net->hasPattern())
    fprintf(fout, " PATTERN %s ", net->pattern());
  if (net->hasOriginal())
    fprintf(fout, " ORIGINAL %s ", net->original());
  if (net->hasUse())
    fprintf(fout, " USE %s ", net->use());

  fprintf (fout, "\n");
  --numObjs;
  return 0;
}


// Special Net
int snetf(defrCallbackType_e c, defiNet* net, defiUserData ud) {
  // For net and special net.
  int i, j, x, y, count, newLayer;
  char* layerName;
  double dist, left, right;
  defiPath* p;
  defiSubnet *s;
  int path;

  checkType(c);
  if ((long)ud != userData) dataError();
  if (c != defrSNetCbkType)
      fprintf(fout, "BOGUS NET TYPE  ");
  fprintf(fout, "SPECIALNET %s ", net->name());

  count = 0;
  // compName & pinName
  for (i = 0; i < net->numConnections(); i++) {
      // set the limit of only 5 items print out in one line
      count++;
      if (count >= 5) {
          fprintf(fout, "\nSPECIALNET %s", net->name());
          count = 0;
      }
      fprintf (fout, "( %s %s ) ", net->instance(i),
               net->pin(i));
  }

  // specialWiring
  if (net->isFixed())
    fprintf(fout, " FIXED ");
  if (net->isRouted())
    fprintf(fout, " ROUTED ");
  if (net->isCover())
    fprintf(fout, " COVER ");

  if (net->numPaths()) {
    newLayer = 0;
    for (i = 0; i < net->numPaths(); i++) {
      p = net->path(i);
      p->initTraverse();
      while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
        // Don't want the line to be too long
        count++;
        if (count >= 5) {
            fprintf(fout, "\nSPECIALNET %s", net->name());
            count = 0;
        }
        switch (path) {
          case DEFIPATH_LAYER:
               if (newLayer == 0) {
                   fprintf(fout, "%s ", p->defiPath::getLayer());
                   newLayer = 1;
               } else
                   fprintf(fout, "NEW %s ", p->defiPath::getLayer());
               break;
          case DEFIPATH_VIA:
               fprintf(fout, "%s ", p->defiPath::getVia());
               break;
          case DEFIPATH_WIDTH:
               fprintf(fout, "%d ", p->defiPath::getWidth());
               break;
          case DEFIPATH_POINT:
               p->defiPath::getPoint(&x, &y);
               fprintf(fout, "( %d %d ) ", x, y);
               break;
          case DEFIPATH_TAPER:
               fprintf(fout, "TAPER ");
          case DEFIPATH_SHAPE:
               fprintf(fout, " SHAPE %s ", p->defiPath::getShape());
               break;
        }
      }
    }
    fprintf(fout, "\n");
  }
 
  if (net->hasSubnets()) {
    for (i = 0; i < net->numSubnets(); i++) {
      fprintf(fout, "SPECIALNET %s", net->name());
      s = net->subnet(i);
      if (s->isFixed())
        fprintf(fout, " FIXED ");
      if (s->isRouted())
        fprintf(fout, " ROUTED ");
      if (s->isCover())
        fprintf(fout, " COVER ");
      fprintf(fout, "\n");
 
      fprintf(fout, "SPECIALNET %s", net->name());
      if (s->numConnections()) {
          if (s->pinIsMustJoin(0))
              fprintf(fout, " MUSTJOIN ");
          else
              fprintf(fout, " %s ", s->name());
          for (j = 0; j < s->numConnections(); j++) {
              fprintf(fout, " ( %s %s )\n", s->instance(j),
                      s->pin(j));
        }
      }
 
      if (s->numPaths()) {
          for (j = 0; j < s->numPaths(); j++) {
              p = s->path(j);
              p->defiPath::print(fout);
        }
      }
    }
  }

  // layerName width
  if (net->hasWidthRules()) {
    fprintf(fout, "SPECIALNET %s", net->name());
    for (i = 0; i < net->numWidthRules(); i++) {
        net->widthRule(i, &layerName, &dist);
        fprintf (fout, " WIDTH %s %g ", layerName, dist);
    }
    fprintf(fout, "\n");
  }

  // layerName spacing
  if (net->hasSpacingRules()) {
    fprintf(fout, "SPECIALNET %s", net->name());
    for (i = 0; i < net->numSpacingRules(); i++) {
        net->spacingRule(i, &layerName, &dist, &left, &right);
        if (left == right)
            fprintf (fout, " SPACING %s %g ", layerName, dist);
        else
            fprintf (fout, " SPACING %s %g RANGE %g %g ",
                     layerName, dist, left, right);
    }
    fprintf(fout, "\n");
  }

  if (net->hasVoltage())
    fprintf(fout, "SPECIALNET %s VOLTAGE %g\n", net->name(), net->voltage());
  if (net->hasWeight())
    fprintf(fout, "SPECIALNET %s WEIGHT %d\n", net->name(), net->weight());
  if (net->hasCap())
    fprintf(fout, "SPECIALNET %s ESTCAP %g\n", net->name(), net->cap());
  if (net->hasSource())
    fprintf(fout, "SPECIALNET %s SOURCE %s\n", net->name(), net->source());
  if (net->hasPattern())
    fprintf(fout, "SPECIALNET %s PATTERN %s\n", net->name(), net->pattern());
  if (net->hasOriginal())
    fprintf(fout, "SPECIALNET %s ORIGINAL %s\n", net->name(), net->original());
  if (net->hasUse())
    fprintf(fout, "SPECIALNET %s USE %s\n", net->name(), net->use());

  --numObjs;
  return 0;
}


// Technology
int tname(defrCallbackType_e c, const char* string, defiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "TECHNOLOGY %s\n", string);
  return 0;
}

// Design
int dname(defrCallbackType_e c, const char* string, defiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "DESIGN %s\n", string);

  // Test changing the user data.
  userData = 89;
  defrSetUserData((void*)userData);

  return 0;
}


char* address(const char* in) {
  return ((char*)in);
}

// Assertion or Constraints
void operand(defrCallbackType_e c, defiAssertion* a, int ind) {
  int i, first = 1;
  char* netName;
  char* fromInst, * fromPin, * toInst, * toPin;

  if (a->isSum()) {
      // Sum in operand, recursively call operand
      fprintf(fout, "ASSERTIONS/CONSTRAINTS SUM ( ");
      a->unsetSum();
      isSumSet = 1;
      begOperand = 0;
      operand (c, a, ind);
      fprintf(fout, ") ");
  } else {
      // operand
      if (ind >= a->numItems()) {
          fprintf(fout, "ERROR: when writing out SUM in Constraints.\n");
          return;
       }
      if (begOperand) {
         fprintf(fout, "ASSERTIONS/CONSTRAINTS ");
         begOperand = 0;
      }
      for (i = ind; i < a->numItems(); i++) {
          if (a->isNet(i)) {
              a->net(i, &netName);
              if (!first)
                  fprintf(fout, ", "); // print , as separator
              fprintf(fout, "NET %s ", netName); 
          } else if (a->isPath(i)) {
              a->path(i, &fromInst, &fromPin, &toInst,
                                     &toPin);
              if (!first)
                  fprintf(fout, ", ");
              fprintf(fout, "PATH %s %s %s %s ", fromInst, fromPin, toInst,
                      toPin);
          } else if (isSumSet) {
              // SUM within SUM, reset the flag
              a->setSum();
              operand(c, a, i);
          }
          first = 0;
      } 
      
  }
}

// Assertion or Constraints
int constraint(defrCallbackType_e c, defiAssertion* a, defiUserData ud) {
  // Handles both constraints and assertions

  checkType(c);
  if ((long)ud != userData) dataError();
  if (a->isWiredlogic())
      // Wirelogic
      fprintf(fout, "ASSERTIONS/CONSTRAINTS WIREDLOGIC %s + MAXDIST %g ;\n",
              a->netName(), a->fallMax());
  else {
      // Call the operand function
      isSumSet = 0;    // reset the global variable
      begOperand = 1;
      operand (c, a, 0);
      // Get the Rise and Fall
      if (a->hasRiseMax())
          fprintf(fout, " RISEMAX %g ", a->riseMax());
      if (a->hasFallMax())
          fprintf(fout, " FALLMAX %g ", a->fallMax());
      if (a->hasRiseMin())
          fprintf(fout, " RISEMIN %g ", a->riseMin());
      if (a->hasFallMin())
          fprintf(fout, " FALLMIN %g ", a->fallMin());
      fprintf(fout, "\n");
  }
  --numObjs;
  return 0;
}


// Property definitions
int prop(defrCallbackType_e c, defiProp* p, defiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  if (strcmp(p->propType(), "design") == 0)
      fprintf(fout, "PROPERTYDEFINITIONS DESIGN %s ", p->propName());
  else if (strcmp(p->propType(), "net") == 0)
      fprintf(fout, "PROPERTYDEFINITIONS NET %s ", p->propName());
  else if (strcmp(p->propType(), "component") == 0)
      fprintf(fout, "PROPERTYDEFINITIONS COMPONENT %s ", p->propName());
  else if (strcmp(p->propType(), "specialnet") == 0)
      fprintf(fout, "PROPERTYDEFINITIONS SPECIALNET %s ", p->propName());
  else if (strcmp(p->propType(), "group") == 0)
      fprintf(fout, "PROPERTYDEFINITIONS GROUP %s ", p->propName());
  else if (strcmp(p->propType(), "row") == 0)
      fprintf(fout, "PROPERTYDEFINITIONS ROW %s ", p->propName());
  else if (strcmp(p->propType(), "componentpin") == 0)
      fprintf(fout, "PROPERTYDEFINITIONS COMPONENTPIN %s ", p->propName());
  else if (strcmp(p->propType(), "region") == 0)
      fprintf(fout, "PROPERTYDEFINITIONS REGION %s ", p->propName());
  if (p->dataType() == 'I')
      fprintf(fout, "INTEGER ");
  if (p->dataType() == 'R')
      fprintf(fout, "REAL ");
  if (p->dataType() == 'S')
      fprintf(fout, "STRING ");
  if (p->dataType() == 'Q')
      fprintf(fout, "STRING ");
  if (p->hasRange()) {
      fprintf(fout, "RANGE %g %g ", p->left(),
              p->right());
  }
  if (p->hasNumber())
      fprintf(fout, "%g ", p->number());
  if (p->hasString())
      fprintf(fout, "\"%s\" ", p->string());
  fprintf(fout, "\n");

  return 0;
}


// History
int hist(defrCallbackType_e c, const char* h, defiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "HISTORY %s\n", h);
  return 0;
}


// Busbitchars
int bbn(defrCallbackType_e c, const char* h, defiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "BUSBITCHARS \"%s\" \n", h);
  return 0;
}


// Version
int vers(defrCallbackType_e c, double d, defiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "VERSION %g\n", d);
  return 0;
}


// Units
int units(defrCallbackType_e c, double d, defiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "UNITS DISTANCE MICRONS %g\n", d);
  return 0;
}


// Casesensitive
int casesens(defrCallbackType_e c, int d, defiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  if (d == 1)
     fprintf(fout, "NAMESCASESENSITIVE OFF\n", d);
  else
     fprintf(fout, "NAMESCASESENSITIVE ON\n", d);
  return 0;
}


// Site, Canplace, Cannotoccupy, Diearea, Pin, Pincap, DefaultCap,
// Row, Gcellgrid, Track, Via, Scanchain, IOtiming, Flooplan,
// Region, Group, TiminDisable, Pin property
int cls(defrCallbackType_e c, void* cl, defiUserData ud) {
  defiSite* site;  // Site and Canplace and CannotOccupy
  defiBox* box;  // DieArea and 
  defiPinCap* pc;
  defiPin* pin;
  int i;
  defiRow* row;
  defiTrack* track;
  defiGcellGrid* gcg;
  defiVia* via;
  defiRegion* re;
  defiGroup* group;
  defiScanchain* sc;
  defiIOTiming* iot;
  defiFPC* fpc;
  defiTimingDisable* td;
  defiPartition* part;
  defiPinProp* pprop;
  int xl, yl, xh, yh;
  char *name, *a1, *b1;
  char **inst, **inPin, **outPin;
  int  size;
  int corner, typ;
  const char *itemT;
  char dir;

  checkType(c);
  if ((long)ud != userData) dataError();
  switch (c) {

  case defrSiteCbkType :
         site = (defiSite*)cl;
         fprintf(fout, "SITE %s %g %g %d ", site->name(),
                 site->x_orig(), site->y_orig(), site->orient());
         fprintf(fout, "DO %d BY %d STEP %g %g\n",
                 site->x_num(), site->y_num(), site->x_step(), site->y_step());
         break;
  case defrCanplaceCbkType :
         site = (defiSite*)cl;
         fprintf(fout, "CANPLACE %s %g %g %d ", site->name(),
                 site->x_orig(), site->y_orig(), site->orient());
         fprintf(fout, "DO %d BY %d STEP %g %g\n",
                 site->x_num(), site->y_num(),
                 site->x_step(), site->y_step());
         break;
  case defrCannotOccupyCbkType : 
         site = (defiSite*)cl;
         fprintf(fout, "CANNOTOCCUPY %s %g %g %d ",
                 site->name(), site->x_orig(), site->y_orig(), site->orient());
         fprintf(fout, "DO %d BY %d STEP %g %g\n",
                 site->x_num(), site->y_num(),
                 site->x_step(), site->y_step());
         break;
  case defrDieAreaCbkType :
         box = (defiBox*)cl;
         fprintf(fout, "DIEAREA ( %d %d ) ( %d %d )\n",
                 box->xl(), box->yl(), box->xh(), box->yh());
         break;
  case defrPinCapCbkType :
         pc = (defiPinCap*)cl;
         fprintf(fout, "MINPINS %d WIRECAP %g\n", pc->pin(), pc->cap());
         --numObjs;
         break;
  case defrPinCbkType :
         pin = (defiPin*)cl;
         fprintf(fout, "PIN %s + NET %s ", pin->pinName(),
                 pin->netName());
         if (pin->hasDirection())
             fprintf(fout, "+ DIRECTION %s ", pin->direction());
         if (pin->hasUse())
             fprintf(fout, "+ USE %s ", pin->use());
         if (pin->hasLayer()) {
             fprintf(fout, "+ LAYER %s ", pin->layer());
             pin->bounds(&xl, &yl, &xh, &yh);
             fprintf(fout, "( %d %d ) ( %d %d ) ", xl, yl, xh, yh);
         }
         if (pin->hasPlacement()) {
             if (pin->isPlaced())
                 fprintf(fout, " PLACED ");
             if (pin->isCover())
                 fprintf(fout, " COVER ");
             if (pin->isFixed())
                 fprintf(fout, " FIXED ");
             fprintf(fout, "( %d %d ) %d ", pin->placementX(),
                     pin->placementY(), pin->orient());
         }
         if (pin->hasSpecial()) {
             fprintf(fout, " SPECIAL ");
         }
         fprintf(fout, "\n");
         --numObjs;
         break;
  case defrDefaultCapCbkType :
         i = (long)cl;
         fprintf(fout, "DEFAULTCAP %d", i);
         numObjs = i;
         break;
  case defrRowCbkType :
         row = (defiRow*)cl;
         fprintf(fout, "ROW %s %s %g %g %d ", row->name(),
                 row->macro(), row->x(), row->y(), row->orient());
         fprintf(fout, "DO %g BY %g STEP %g %g\n",
                 row->xNum(), row->yNum(),
                 row->xStep(), row->yStep());
         if (row->numProps() > 0) {
            for (i = 0; i < row->numProps(); i++) {
                fprintf(fout, "ROW %s PROPERTY %s %s\n", row->name(),
                        row->propName(i),
                        row->propValue(i));
            }
         }
         break;
  case defrTrackCbkType :
         track = (defiTrack*)cl;
         fprintf(fout, "TRACKS %s %g DO %g STEP %g LAYER ",
                 track->macro(), track->x(), track->xNum(), track->xStep());
         for (i = 0; i < track->numLayers(); i++)
            fprintf(fout, "%s ", track->layer(i));
         fprintf(fout, "\n"); 
         break;
  case defrGcellGridCbkType :
         gcg = (defiGcellGrid*)cl;
         fprintf(fout, "GCELLGRID %s %d DO %d STEP %g\n",
                 gcg->defiGcellGrid::macro(), gcg->defiGcellGrid::x(),
                 gcg->defiGcellGrid::xNum(), gcg->defiGcellGrid::xStep());
         break;
  case defrViaCbkType :
         via = (defiVia*)cl;
         fprintf(fout, "VIA %s ", via->name());
         if (via->hasPattern())
             fprintf(fout, " PATTERNNAME %s\n", via->pattern());
         else
             fprintf(fout, "\n");
         for (i = 0; i < via->numLayers(); i++) {
             via->layer(i, &name, &xl, &yl, &xh, &yh);
             fprintf(fout, "VIA %s RECT %s ( %d %d ) ( %d %d ) \n", via->name(),
                     name, xl, yl, xh, yh);
         }
         --numObjs;
         break;
  case defrRegionCbkType :
         re = (defiRegion*)cl;
         for (i = 0; i < re->numRectangles(); i++)
             fprintf(fout, "REGION %s ( %d %d ) ( %d %d )\n",re->name(),
                     re->xl(i), re->yl(i), re->xh(i), re->yh(i));
         --numObjs;
         break;
  case defrGroupCbkType :
         group = (defiGroup*)cl;
         fprintf(fout, "GROUP %s ", group->name());
         if (group->hasMaxX() | group->hasMaxY()
             | group->hasPerim()) {
             fprintf(fout, "SOFT ");
             if (group->hasPerim()) 
                 fprintf(fout, "MAXHALFPERIMETER %d ",
                         group->perim());
             if (group->hasMaxX())
                 fprintf(fout, "MAXX %d ", group->maxX());
             if (group->hasMaxY()) 
                 fprintf(fout, "MAXY %d ", group->maxY());
         } 
         if (group->hasRegionName())
             fprintf(fout, "REGION %s ", group->regionName());
         if (group->hasRegionBox()) {
             int *gxl, *gyl, *gxh, *gyh;
             int size;
             group->regionRects(&size, &gxl, &gyl, &gxh, &gyh);
             for (i = 0; i < size; i++)
                 fprintf(fout, "REGION (%d %d) (%d %d) ", gxl[i], gyl[i],
                         gxh[i], gyh[i]);
         }
         fprintf(fout, "\n");
         --numObjs;
         break;
  case defrScanchainCbkType :
         sc = (defiScanchain*)cl;
         fprintf(fout, "SCANCHAINS %s", sc->name());
         if (sc->hasStart()) {
             sc->start(&a1, &b1);
             fprintf(fout, " START %s %s", sc->name(), a1, b1);
         }
         if (sc->hasStop()) {
             sc->stop(&a1, &b1);
             fprintf(fout, " STOP %s %s", sc->name(), a1, b1);
         }
         if (sc->hasCommonInPin() ||
             sc->hasCommonOutPin()) {
             fprintf(fout, " COMMONSCANPINS ", sc->name());
             if (sc->hasCommonInPin())
                fprintf(fout, " ( IN %s ) ", sc->commonInPin());
             if (sc->hasCommonOutPin())
                fprintf(fout, " ( OUT %s ) ",sc->commonOutPin());
         }
         if (sc->hasFloating()) {
            sc->floating(&size, &inst, &inPin, &outPin);
            for (i = 0; i < size; i++) {
                fprintf(fout, " FLOATING %s ", sc->name(), inst[i]);
                if (inPin[i])
                   fprintf(fout, "( IN %s ) ", inPin[i]);
                if (outPin[i])
                   fprintf(fout, "( OUT %s ) ", outPin[i]);
            }
         }

         if (sc->hasOrdered()) {
            for (i = 0; i < sc->numOrderedLists(); i++) {
                sc->ordered(i, &size, &inst, &inPin, &outPin);
                for (i = 0; i < size; i++) {
                    fprintf(fout, " ORDERED %s ", sc->name(),
                            inst[i]); 
                    if (inPin[i])
                       fprintf(fout, "( IN %s ) ", inPin[i]);
                    if (outPin[i])
                       fprintf(fout, "( OUT %s ) ", outPin[i]);
                }
            }
         }
         fprintf(fout, "\n");
         --numObjs;
         break;
  case defrIOTimingCbkType :
         iot = (defiIOTiming*)cl;
         fprintf(fout, "IOTIMING ( %s %s )\n", iot->inst(), iot->pin());
         if (iot->hasSlewRise())
             fprintf(fout, "IOTIMING %s RISE SLEWRATE %g %g\n", iot->inst(),
                     iot->slewRiseMin(), iot->slewRiseMax());
         if (iot->hasSlewFall())
             fprintf(fout, "IOTIMING %s FALL SLEWRATE %g %g\n", iot->inst(),
                     iot->slewFallMin(), iot->slewFallMax());
         if (iot->hasVariableRise())
             fprintf(fout, "IOTIMING %s RISE VARIABLE %g %g\n", iot->inst(),
                     iot->variableRiseMin(), iot->variableRiseMax());
         if (iot->hasVariableFall())
             fprintf(fout, "IOTIMING %s FALL VARIABLE %g %g\n", iot->inst(),
                     iot->variableFallMin(), iot->variableFallMax());
         if (iot->hasCapacitance())
             fprintf(fout, "IOTIMING %s CAPACITANCE %g\n", iot->inst(),
                     iot->capacitance());
         if (iot->hasDriveCell()) {
             fprintf(fout, "IOTIMING %s DRIVECELL %s ", iot->inst(),
                     iot->driveCell());
             if (iot->hasFrom())
                 fprintf(fout, " FROMPIN %s ",
                         iot->from());
             if (iot->hasTo())
                 fprintf(fout, " TOPIN %s ",
                         iot->to());
             if (iot->hasParallel())
                 fprintf(fout, "PARALLEL %g",
                         iot->parallel());
             fprintf(fout, "\n");
         }
         --numObjs;
         break;
  case defrFPCCbkType :
         fpc = (defiFPC*)cl;
         fprintf(fout, "FLOORPLAN %s ", fpc->name());
         if (fpc->isVertical())
             fprintf(fout, "VERTICAL ");
         if (fpc->isHorizontal())
             fprintf(fout, "HORIZONTAL ");
         if (fpc->hasAlign())
             fprintf(fout, "ALIGN ");
         if (fpc->hasMax())
             fprintf(fout, "%g ", fpc->max());
         if (fpc->hasMin())
             fprintf(fout, "%g ", fpc->min());
         if (fpc->hasEqual())
             fprintf(fout, "%g ", fpc->equal());
         for (i = 0; i < fpc->numParts(); i++) {
             fpc->getPart(i, &corner, &typ, &name);
             if (corner == 'B')
                 fprintf(fout, "BOTTOMLEFT ");
             else
                 fprintf(fout, "TOPRIGHT ");
             if (typ == 'R')
                 fprintf(fout, "ROWS %s ", name);
             else
                 fprintf(fout, "COMPS %s ", name);
         }
         fprintf(fout, "\n");
         --numObjs;
         break;
  case defrTimingDisableCbkType :
         td = (defiTimingDisable*)cl;
         if (td->hasFromTo())
             fprintf(fout, "TIMINGDISABLE FROMPIN %s %s ", td->fromInst(),
                     td->fromPin(), td->toInst(), td->toPin());
         if (td->hasThru())
             fprintf(fout, " THRUPIN %s %s ", td->thruInst(), td->thruPin());
         if (td->hasMacroFromTo())
             fprintf(fout, " MACRO %s FROMPIN %s %s ", td->macroName(),
                     td->fromPin(), td->toPin());
         if (td->hasMacroThru())
             fprintf(fout, " MACRO %s THRUPIN %s %s ", td->macroName(),
                     td->fromPin());
         fprintf(fout, "\n");
         break;
  case defrPartitionCbkType :
         part = (defiPartition*)cl;
         fprintf(fout, "PARTITION %s ", part->name());
         if (part->isSetupRise() | part->isSetupFall() | part->isHoldRise() |
             part->isHoldFall()) {
             // has turnoff 
             fprintf(fout, "TURNOFF "); 
             if (part->isSetupRise())
                 fprintf(fout, "SETUPRISE "); 
             if (part->isSetupFall())
                 fprintf(fout, "SETUPFALL "); 
             if (part->isHoldRise())
                 fprintf(fout, "HOLDRISE "); 
             if (part->isHoldFall())
                 fprintf(fout, "HOLDFALL "); 
         }
         itemT = part->itemType();
         dir = part->direction();
         if (strcmp(itemT, "CLOCK") == 0) {
             if (dir == 'T')    // toclockpin
                 fprintf(fout, " TOCLOCKPIN %s %s ", part->instName(),
                         part->pinName());
             if (dir == 'F')    // fromclockpin
                 fprintf(fout, " FROMCLOCKPIN %s %s ", part->instName(),
                         part->pinName());
             if (part->hasMin())
                 fprintf(fout, "MIN %g %g ", part->min(), part->max());
             if (part->hasMax())
                 fprintf(fout, "MAX %g %g ", part->max(), part->max());
             fprintf(fout, "PINS ");
             for (i = 0; i < part->numPins(); i++)
                  fprintf(fout, "%s ", part->pin(i));
         } else if (strcmp(itemT, "IO") == 0) {
             if (dir == 'T')    // toiopin
                 fprintf(fout, " TOIOPIN %s %s ", part->instName(),
                         part->pinName());
             if (dir == 'F')    // fromiopin
                 fprintf(fout, " FROMIOPIN %s %s ", part->instName(),
                         part->pinName());
         } else if (strcmp(itemT, "COMP") == 0) {
             if (dir == 'T')    // tocomppin
                 fprintf(fout, " TOCOMPPIN %s %s ", part->instName(),
                         part->pinName());
             if (dir == 'F')    // fromcomppin
                 fprintf(fout, " FROMCOMPPIN %s %s ", part->instName(),
                         part->pinName());
         }
         fprintf(fout, "\n");
         --numObjs;
         break;

  case defrPinPropCbkType :
         pprop = (defiPinProp*)cl;
         if (pprop->defiPinProp::isPin())
            fprintf(fout, "PINPROPERTY PIN %s ", pprop->pinName());
         else 
            fprintf(fout, "PINPROPERTY %s %s ", pprop->instName(), pprop->pinName());
         fprintf(fout, "\n");
         if (pprop->numProps() > 0) {
            for (i = 0; i < pprop->numProps(); i++) {
                fprintf(fout, "PINPROPERTY PIN %s PROPERTY %s %s ",
                        pprop->pinName(), pprop->propName(i),
                        pprop->propValue(i));
            }
            fprintf(fout, "\n");
         }
         --numObjs;
         break;
  default: fprintf(fout, "BOGUS callback to cls.\n"); return 1;
  }
  return 0;
}


int dn(defrCallbackType_e c, const char* h, defiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "DIVIDERCHAR \"%s\" \n",h);
  return 0;
}


int ext(defrCallbackType_e t, const char* c, defiUserData ud) {
  char* name;

  checkType(t);
  if ((long)ud != userData) dataError();

  switch (t) {
  case defrNetExtCbkType : name = address("net"); break;
  case defrComponentExtCbkType : name = address("component"); break;
  case defrPinExtCbkType : name = address("pin"); break;
  case defrViaExtCbkType : name = address("via"); break;
  case defrNetConnectionExtCbkType : name = address("net connection"); break;
  case defrGroupExtCbkType : name = address("group"); break;
  case defrScanChainExtCbkType : name = address("scanchain"); break;
  case defrIoTimingsExtCbkType : name = address("io timing"); break;
  case defrPartitionsExtCbkType : name = address("partition"); break;
  default: name = address("BOGUS"); return 1;
  }
  fprintf(fout, "EXTENSION %s %s\n", name, c);
  return 0;
}

//========

int diffDefReadFile(char* inFile, char* outFile) {
  FILE* f;
  int   res;

  userData = 0x01020304;
  defrInit();

  defrSetUserData((void*)3);
  defrSetDesignCbk(dname);
  defrSetTechnologyCbk(tname);
  defrSetPropCbk(prop);
  defrSetNetCbk(netf);
  defrSetSNetCbk(snetf);
  defrSetComponentCbk(compf);
  defrSetAddPathToNet();
  defrSetHistoryCbk(hist);
  defrSetConstraintCbk(constraint);
  defrSetAssertionCbk(constraint);
  defrSetDividerCbk(dn);
  defrSetBusBitCbk(bbn);

  // All of the extensions point to the same function.
  defrSetNetExtCbk(ext);
  defrSetComponentExtCbk(ext);
  defrSetPinExtCbk(ext);
  defrSetViaExtCbk(ext);
  defrSetNetConnectionExtCbk(ext);
  defrSetGroupExtCbk(ext);
  defrSetScanChainExtCbk(ext);
  defrSetIoTimingsExtCbk(ext);
  defrSetPartitionsExtCbk(ext);

  defrSetUnitsCbk(units);
  defrSetVersionCbk(vers);
  defrSetCaseSensitiveCbk(casesens);

  // The following calls are an example of using one function "cls"
  // to be the callback for many DIFFERENT types of constructs.
  // We have to cast the function type to meet the requirements
  // of each different set function.
  defrSetSiteCbk((defrSiteCbkFnType)cls);
  defrSetCanplaceCbk((defrSiteCbkFnType)cls);
  defrSetCannotOccupyCbk((defrSiteCbkFnType)cls);
  defrSetDieAreaCbk((defrBoxCbkFnType)cls);
  defrSetPinCapCbk((defrPinCapCbkFnType)cls);
  defrSetPinCbk((defrPinCbkFnType)cls);
  defrSetPinPropCbk((defrPinPropCbkFnType)cls);
  defrSetDefaultCapCbk((defrIntegerCbkFnType)cls);
  defrSetRowCbk((defrRowCbkFnType)cls);
  defrSetTrackCbk((defrTrackCbkFnType)cls);
  defrSetGcellGridCbk((defrGcellGridCbkFnType)cls);
  defrSetViaCbk((defrViaCbkFnType)cls);
  defrSetRegionCbk((defrRegionCbkFnType)cls);
  defrSetGroupCbk((defrGroupCbkFnType)cls);
  defrSetScanchainCbk((defrScanchainCbkFnType)cls);
  defrSetIOTimingCbk((defrIOTimingCbkFnType)cls);
  defrSetFPCCbk((defrFPCCbkFnType)cls);
  defrSetTimingDisableCbk((defrTimingDisableCbkFnType)cls);
  defrSetPartitionCbk((defrPartitionCbkFnType)cls);

  if ((f = fopen(inFile,"r")) == 0) {
    fprintf(stderr,"Couldn't open input file '%s'\n", inFile);
    return(2);
  }
 
  if ((fout = fopen(outFile, "w")) == 0) {
    fprintf(stderr, "Couldn't open output file '%s'\n", outFile);
    return(2);
  }

  res = defrRead(f, inFile, (void*)userData, 1);

  fclose(f);
  fclose(fout);

  return 0;
}
