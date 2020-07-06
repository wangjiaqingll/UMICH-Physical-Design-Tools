//
//       Copyright (C) 1997 Cadence Design Systems Inc.
//                     All Rights Reserved.
//
 
// This program is the diffLef core program.  It has all the callback
// routines and write it out to a temporary file

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "lefrReader.hpp"
#include "lefwWriter.hpp"
#include "lefiDebug.hpp"

char defaultName[128];
char defaultOut[128];
FILE* fout;
int printing = 0;     // Printing the output.
int userData;

// TX_DIR:TRANSLATION ON
 
void dataError() {
  fprintf(fout, "ERROR: returned user data is not correct!\n");
}
 

void checkType(lefrCallbackType_e c) {
  if (c >= 0 && c <= lefrLibraryEndCbkType) {
    // OK
  } else {
    fprintf(fout, "ERROR: callback type is out of bounds!\n");
  }
}

// Print Via
void lefVia(lefiVia* via, char* inName) {
  int i, j;

  fprintf(fout, "%s %s", inName, via->name());
  if (via->hasDefault())
     fprintf(fout, " DEFAULT");
  if (via->hasTopOfStack())
     fprintf(fout, " TOPOFSTACKONLY");
  if (via->hasForeign()) {
     fprintf(fout, " FOREIGN %s", via->foreign());
     if (via->hasForeignPnt()) {
        fprintf(fout, " ( %g %g )", via->foreignX(), via->foreignY());
        if (via->hasForeignOrient())
           fprintf(fout, " %d", via->foreignOrient());
     }
  }
  fprintf(fout, "\n");
  if (via->hasProperties()) {
     fprintf(fout, "%s %s PROPERTY", inName, via->name());
     for (i = 0; i < via->numProperties(); i++) {
        fprintf(fout, " %s", via->propName(i));
        if (via->propIsNumber(i))
           fprintf(fout, " %g", via->propNumber(i));
        if (via->propIsString(i))
           fprintf(fout, " %s", via->propValue(i));
     }
     fprintf(fout, "\n");
  }
  if (via->hasResistance())
     fprintf(fout, "%s %s RESISTANCE %g\n",inName, via->name(),
             via->resistance());
  if (via->numLayers() > 0) {
     for (i = 0; i < via->numLayers(); i++) {
        fprintf(fout, "%s %s LAYER %s\n", inName, via->name(),
                via->layerName(i));
        for (j = 0; j < via->numRects(i); j++)
           fprintf(fout, "%s %s RECT ( %g %g ) ( %g %g )\n", inName,
                   via->name(), via->xl(i, j), via->yl(i, j), via->xh(i, j),
                   via->yh(i, j));
     }
  }
  return;
}

// Print Spacing
void lefSpacing(lefiSpacing* spacing, char *inName) {
  fprintf(fout, "%s SAMENET %s %s %g ", inName, spacing->name1(),
          spacing->name2(), spacing->distance());
  if (spacing->hasStack())
     fprintf(fout, " STACK");
  fprintf(fout,"\n");
  return;
}

// Print Via Rule Layer
void lefViaRuleLayer(lefiViaRuleLayer* vLayer, char* viaName) {
  fprintf(fout, "VIARULE %s LAYER %s", viaName, vLayer->name());
  if (vLayer->hasDirection()) {
     if (vLayer->isHorizontal())
        fprintf(fout, " DIRECTION HORIZONTAL");
     if (vLayer->isVertical())
        fprintf(fout, " DIRECTION VERTICAL");
  }
  if (vLayer->hasWidth())
     fprintf(fout, " WIDTH %g TO %g", vLayer->widthMin(), vLayer->widthMax());
  if (vLayer->hasResistance())
     fprintf(fout, " RESISTANCE %g", vLayer->resistance());
  if (vLayer->hasOverhang())
     fprintf(fout, " OVERHANG %g", vLayer->overhang());
  if (vLayer->hasMetalOverhang())
     fprintf(fout, " METALOVERHANG %g", vLayer->metalOverhang());
  if (vLayer->hasSpacing())
     fprintf(fout, " SPACING %g BY %g", vLayer->spacingStepX(),
             vLayer->spacingStepY());
  if (vLayer->hasRect())
     fprintf(fout, " RECT ( %g %g ) ( %g %g )", vLayer->xl(), vLayer->yl(),
             vLayer->xh(), vLayer->yh());
  fprintf(fout, "\n");
  return;
}
 
// Print Geometry
void prtGeometry(lefiGeometries* geometry, char* inName) {
  int                   numItems = geometry->numItems();
  int                   i, j;
  lefiGeomPath*         path;
  lefiGeomPathIter*     pathIter;
  lefiGeomRect*         rect;
  lefiGeomRectIter*     rectIter;
  lefiGeomPolygon*      polygon;
  lefiGeomPolygonIter*  polygonIter;
  lefiGeomVia*          via;
  lefiGeomViaIter*      viaIter;
 
  for (i = 0; i < numItems; i++) {
     switch (geometry->itemType(i)) {
        case  lefiGeomClassE:
             fprintf(fout, "%s CLASS %s\n", inName, geometry->getClass(i));
             break;
        case lefiGeomLayerE:
             fprintf(fout, "%s LAYER %s\n", inName, geometry->getLayer(i));
             break;
        case lefiGeomWidthE:
             fprintf(fout, "%s WIDTH %g\n", inName, geometry->getWidth(i));
             break;
        case lefiGeomPathE:
             path = geometry->getPath(i);
             fprintf(fout, "%s PATH ", inName);
             for (j = 0; j < path->numPoints; j++) {
                if (j+1 == path->numPoints) // last one on the list
                   fprintf(fout, " ( %g %g )\n", path->x[j], path->y[j]);
                else
                   fprintf(fout, " ( %g %g )", path->x[j], path->y[j]);
             }
             break;
        case lefiGeomPathIterE:
             pathIter = geometry->getPathIter(i);
             fprintf(fout, "%s PATH ITERATED ", inName);
             for (j = 0; j < pathIter->numPoints; j++)
                fprintf(fout, " ( %g %g )", pathIter->x[j],
                        pathIter->y[j]);
             fprintf(fout, " DO %g BY %g STEP %g %g\n", pathIter->xStart,
                     pathIter->yStart, pathIter->xStep, pathIter->yStep);
             break;
        case lefiGeomRectE:
             rect = geometry->getRect(i);
             fprintf(fout, "%s RECT ( %g %g ) ( %g %g )\n", inName, rect->xl, 
                     rect->yl, rect->xh, rect->yh);
             break;
        case lefiGeomRectIterE:
             rectIter = geometry->getRectIter(i);
             fprintf(fout, "%s RECT ITERATE ( %g %g ) ( %g %g )", inName,
                     rectIter->xl, rectIter->yl, rectIter->xh, rectIter->yh);
             fprintf(fout, " DO %g BY %g STEP %g %g ;\n",
                     rectIter->xStart, rectIter->yStart, rectIter->xStep,
                     rectIter->yStep);
             break;
        case lefiGeomPolygonE:
             polygon = geometry->getPolygon(i);
             fprintf(fout, "%s POLYGON ", inName);
             for (j = 0; j < polygon->numPoints; j++) {
                if (j+1 == polygon->numPoints) // last one on the list
                   fprintf(fout, " ( %g %g )\n", polygon->x[j],
                           polygon->y[j]);
                else
                   fprintf(fout, " ( %g %g )", polygon->x[j],
                           polygon->y[j]);
             }
             break;
        case lefiGeomPolygonIterE:
             polygonIter = geometry->getPolygonIter(i);
             fprintf(fout, "%s POLYGON ITERATE", inName);
             for (j = 0; j < polygonIter->numPoints; j++)
                   fprintf(fout, " ( %g %g )", polygonIter->x[j],
                           polygonIter->y[j]);
             fprintf(fout, " DO %g BY %g STEP %g %g\n",
                     polygonIter->xStart, polygonIter->yStart,
                     polygonIter->xStep, polygonIter->yStep);
             break;
        case lefiGeomViaE:
             via = geometry->getVia(i);
             fprintf(fout, "%s VIA ( %g %g ) %s\n", inName, via->x, via->y,
                     via->name);
             break;
        case lefiGeomViaIterE:
             viaIter = geometry->getViaIter(i);
             fprintf(fout, "%s VIA ITERATE ( %g %g ) %s", inName, viaIter->x,
                     viaIter->y, viaIter->name);
             fprintf(fout, " DO %g BY %g STEP %g %g\n",
                     viaIter->xStart, viaIter->yStart,
                     viaIter->xStep, viaIter->yStep);
             break;
        default:
             fprintf(fout, "BOGUS geometries type.\n");
             break;
     }
  }
}

// Array
int arrayCB(lefrCallbackType_e c, lefiArray* a, lefiUserData ud) {
  int              i, j, defCaps;
  lefiSitePattern* pattern;
  lefiTrackPattern* track;
  lefiGcellPattern* gcell;

  checkType(c);
  if ((long)ud != userData) dataError();
   
  if (a->numSitePattern() > 0) {
     for (i = 0; i < a->numSitePattern(); i++) {
        pattern = a->sitePattern(i);
        fprintf(fout, "ARRAY %s SITE %s %g %g %d DO %g BY %g STEP %g %g\n",
               a->name(), pattern->name(), pattern->x(), pattern->y(),
               pattern->orient(), pattern->xStart(), pattern->yStart(),
               pattern->xStep(), pattern->yStep());
     }
  }
  if (a->numCanPlace() > 0) {
     for (i = 0; i < a->numCanPlace(); i++) {
        pattern = a->canPlace(i);
        fprintf(fout, "ARRAY %s CANPLACE %s %g %g %d DO %g BY %g STEP %g %g\n",
               a->name(), pattern->name(), pattern->x(), pattern->y(),
               pattern->orient(), pattern->xStart(), pattern->yStart(),
               pattern->xStep(), pattern->yStep());
     }
  }
  if (a->numCannotOccupy() > 0) {
     for (i = 0; i < a->numCannotOccupy(); i++) {
        pattern = a->cannotOccupy(i);
        fprintf(fout, "ARRAY %s CANNOTOCCUPY %s %g %g %d DO %g BY %g STEP %g %g\n",
               a->name(), pattern->name(), pattern->x(), pattern->y(),
               pattern->orient(), pattern->xStart(), pattern->yStart(),
               pattern->xStep(), pattern->yStep());
     }
  }

  if (a->numTrack() > 0) {
     for (i = 0; i < a->numTrack(); i++) {
        track = a->track(i);
        fprintf(fout, "ARRAY %s TRACKS %s, %g DO %d STEP %g\n", a->name(),
                track->name(), track->start(), track->numTracks(), 
                track->space()); 
        if (track->numLayers() > 0) {
           fprintf(fout, "ARRAY %s TRACKS %s LAYER ", a->name(), track->name());
           for (j = 0; j < track->numLayers(); j++)
              fprintf(fout, "%s ", track->layerName(j));
           fprintf(fout, ";\n"); 
        }
     }
  }

  if (a->numGcell() > 0) {
     for (i = 0; i < a->numGcell(); i++) {
        gcell = a->gcell(i);
        fprintf(fout, "ARRAY %s GCELLGRID %s, %g DO %d STEP %g\n", a->name(),
                gcell->name(), gcell->start(), gcell->numCRs(), gcell->space()); 
     }
  }

  if (a->numFloorPlans() > 0) {
     for (i = 0; i < a->numFloorPlans(); i++) {
        for (j = 0; j < a->numSites(i); j++) {
           pattern = a->site(i, j);
           fprintf(fout,
               "ARRAY %s FLOORPLAN %s %s %s %g %g %s DO %d BY %d STEP %g %g\n",
               a->name(), a->floorPlanName(i), a->siteType(i, j),
               pattern->name(), pattern->x(), pattern->y(), pattern->orient(),
               pattern->xStart(), pattern->yStart(), pattern->xStep(),
               pattern->yStep());
        }
     }
  }

  defCaps = a->numDefaultCaps();
  if (defCaps > 0) {
     for (i = 0; i < defCaps; i++)
        fprintf(fout, "ARRAY %s DEFAULTCAP %d MINPINS %g WIRECAP %g\n",
            a->name(), defCaps, a->defaultCapMinPins(i), a->defaultCap(i));
  }
  return 0;
}

// Busbitchar
int busBitCharsCB(lefrCallbackType_e c, const char* busBit, lefiUserData ud)
{
  checkType(c);
  if ((long)ud != userData) dataError();
  // use the lef writer to write out the data
  fprintf(fout, "BUSBITCHARS %s\n", busBit);
  return 0;
}

// CaseSensitive
int caseSensCB(lefrCallbackType_e c, int caseSense, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();

  if (caseSense == TRUE)
     fprintf(fout, "NAMESCASESENSITIVE ON\n");
  else
     fprintf(fout, "NAMESCASESENSITIVE OFF\n");
  return 0;
}

// Crosstalk correcttable
int correctTableCB(lefrCallbackType_e c, lefiCorrectionTable* table,
                   lefiUserData ud) {
  int                      i, j, k, l;
  lefiCorrectionEdge       *edge;
  lefiCorrectionResistance *resist;
  lefiCorrectionVictim     *victim;
 
  checkType(c);
  if ((long)ud != userData) dataError();

  fprintf(fout, "CROSSTALK CORRECTIONTABLE %d\n", table->num());
  for (i = 0; i < table->numEdges(); i++) {
     edge = table->edge(i);
     // resistances
     if (edge->numResistances()) {
        for (j = 0; j < edge->numResistances(); j++) {
           resist = edge->resistance(j);
           fprintf(fout, "CROSSTALK CORRECTIONTABLE %d RESISTANCE %g\n",
                   table->num(), resist->num(j));
           // victims
           for (k = 0; k < resist->numVictims(); k++) {
              victim = resist->victim(k);
              if (victim->length() > 0)
                 fprintf(fout,
                    "CROSSTALK CORRECTIONTABLE %d RESISTANCE %g VICTIMLEN %g\n",
                         table->num(), resist->num(j), victim->length());
                 for (l = 0; l < victim->numCorrections(); l++)
                    fprintf(fout,
                    "CROSSTALK CORRECTIONTABLE %d RESISTANCE %g VICTIM GTH %g CORRECTIONFACTOR %g\n",
                       table->num(), resist->num(j), victim->length(),
                       victim->correction(l));
           }
        }
     }
  }
  return 0;
}

// Dielectric
int dielectricCB(lefrCallbackType_e c, double dielectric, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();

  fprintf(fout, "DIELECTRIC %g\n", dielectric);
  return 0;
}

// Divider
int dividerCB(lefrCallbackType_e c, const char* divideChar, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "DIVIDER %s\n", divideChar);
  return 0;
}

// Crosstalk edgeRate
int edgeRateCB(lefrCallbackType_e c, double rate, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "CROSSTALK EDGERATESCALEFACTOR %g\n", rate);
  return 0;
}

// Callback routine for edgeratethreshold1
int edgeRate1CB(lefrCallbackType_e c, double rate, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "CROSSTALK EDGERATETHRESHOLD1 %g\n", rate);
  return 0;
}

// Callback routine for edgeratethreshold2
int edgeRate2CB(lefrCallbackType_e c, double rate, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "CROSSTALK EDGERATETHRESHOLD2 %g\n", rate);
  return 0;
}

// InputAntenna
int inputAntCB(lefrCallbackType_e c, double antenna, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "INPUTINANTENNASIZE %g\n", antenna);
  return 0;
}

// OutputAntenna
int outputAntCB(lefrCallbackType_e c, double antenna, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "OUTPUTINANTENNASIZE %g\n", antenna);
  return 0;
}


// InOutAntenna
int inoutAntCB(lefrCallbackType_e c, double antenna, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "INOUTPUTINANTENNASIZE %g\n", antenna);
  return 0;
}

// Irdrop
int irdropCB(lefrCallbackType_e c, lefiIRDrop* irdrop, lefiUserData ud) {
  int i;
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "IRDROP TABLE %s ", irdrop->lefiIRDrop::name());
  for (i = 0; i < irdrop->lefiIRDrop::numValues(); i++) 
     fprintf(fout, "%g %g ", irdrop->lefiIRDrop::value1(i),
             irdrop->lefiIRDrop::value2(i));
  fprintf(fout, "\n");
  return 0;
}

// Layer
int layerCB(lefrCallbackType_e c, lefiLayer* layer, lefiUserData ud) {
  int i;
  checkType(c);
  if ((long)ud != userData) dataError();
  if (layer->hasType())
     fprintf(fout, "LAYER %s TYPE %s\n", layer->name(), layer->type());
  if (layer->hasPitch())
     fprintf(fout, "LAYER %s PITCH %g\n", layer->name(), layer->pitch());
  if (layer->hasOffset())
     fprintf(fout, "LAYER %s OFFSET %g\n", layer->name(), layer->offset());
  if (layer->hasWidth())
     fprintf(fout, "LAYER %s WIDTH %g\n", layer->name(), layer->width());
  if (layer->hasSpacingNumber()) {
     for (i = 0; i < layer->numSpacing(); i++) {
       fprintf(fout, "LAYER %s SPACING %g", layer->name(), layer->spacing(i));
       if (layer->spacingName(i))
          fprintf(fout, " LAYER %s", layer->spacingName(i));
       fprintf(fout,"\n");
     }
     if (layer->hasSpacingRange())
        fprintf(fout, "LAYER %s RANGE %g %g\n", layer->name(),
                layer->spacingRangeLeft(), layer->spacingRangeRight());
  }
  if (layer->hasDirection())
     fprintf(fout, "LAYER %s DIRECTION %s\n", layer->name(),
             layer->direction());
  if (layer->hasResistance())
     fprintf(fout, "LAYER %s RESISTANCE RPERSQ %g\n", layer->name(),
             layer->resistance());
  if (layer->hasCapacitance())
     fprintf(fout, "LAYER %s CAPACITANCE CPERSQDIST %g\n", layer->name(),
             layer->capacitance());
  if (layer->hasHeight())
     fprintf(fout, "LAYER %s TYPE %g\n", layer->name(), layer->height());
  if (layer->hasThickness())
     fprintf(fout, "LAYER %s THICKNESS %g\n", layer->name(),
             layer->thickness());
  if (layer->hasShrinkage())
     fprintf(fout, "LAYER %s SHRINKAGE %g\n", layer->name(),
             layer->shrinkage());
  if (layer->hasCapMultiplier())
     fprintf(fout, "LAYER %s CAPMULTIPLIER %g\n", layer->name(),
             layer->capMultiplier());
  if (layer->hasEdgeCap())
     fprintf(fout, "LAYER %s EDGECAPACITANCE %g\n", layer->name(),
             layer->edgeCap());
  if (layer->hasAntennaArea())
     fprintf(fout, "LAYER %s ANTENNAAREAFACTOR %g\n", layer->name(),
             layer->antennaArea());
  if (layer->hasAntennaLength())
     fprintf(fout, "LAYER %s ANTENNALENGTHFACTOR %g\n", layer->name(),
             layer->antennaLength());
  return 0;
}

// Macro
int macroCB(lefrCallbackType_e c, lefiMacro* macro, lefiUserData ud) {
  lefiSitePattern* pattern;
  int              propNum, i, hasPrtSym = 0;

  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "MACRO %s", macro->name());
  if (macro->hasClass())
     fprintf(fout, " CLASS %s", macro->macroClass());
  if (macro->hasEEQ())
     fprintf(fout, " EEQ %s", macro->EEQ());
  if (macro->hasLEQ())
     fprintf(fout, " LEQ %s", macro->LEQ());
  if (macro->hasSource())
     fprintf(fout, " SOURCE %s", macro->source());
  if (macro->hasXSymmetry()) {
     fprintf(fout, " SYMMETRY X ");
     hasPrtSym = 1;
  }
  if (macro->hasYSymmetry()) {   // print X Y & R90 in one line
     if (!hasPrtSym) {
        fprintf(fout, "  SYMMETRY Y ");
        hasPrtSym = 1;
     }
     else
        fprintf(fout, "Y ");
  }
  if (macro->has90Symmetry()) {
     if (!hasPrtSym) {
        fprintf(fout, "  SYMMETRY R90 ");
        hasPrtSym = 1;
     }
     else
        fprintf(fout, "R90 ");
  }
  fprintf (fout, "\n");
  if (macro->hasSiteName())
     fprintf(fout, "MACRO %s SITE %s\n", macro->name(), macro->siteName());
  if (macro->hasSitePattern()) {
     for (i = 0; i < macro->numSitePattern(); i++ ) {
       pattern = macro->sitePattern(i);
       fprintf(fout, "MACRO %s SITE %s %g %g %s DO %g BY %g STEP %g %g\n",
             macro->name(), pattern->name(), pattern->x(), pattern->y(),
             pattern->orient(), pattern->xStart(), pattern->yStart(),
             pattern->xStep(), pattern->yStep());
     }
  }
  if (macro->hasSize())
     fprintf(fout, "MACRO %s SIZE %g BY %g\n", macro->name(), macro->sizeX(),
             macro->sizeY());
  if (macro->hasForeign()) {
     fprintf(fout, "MACRO %s FOREIGN %s", macro->name(), macro->foreignName());
     if (macro->hasForeignPoint()) {
        fprintf(fout, " ( %g %g )", macro->foreignX(), macro->foreignY());
        if (macro->hasForeignOrient())
           fprintf(fout, " %d", macro->foreignOrient());
     }
     fprintf(fout, "\n");
  }
  if (macro->hasOrigin())
     fprintf(fout, "MACRO %s ORIGIN ( %g %g )\n", macro->name(),
             macro->originX(), macro->originY());
  if (macro->hasPower())
     fprintf(fout, "MACRO %s POWER %g\n", macro->name(), macro->power());
  propNum = macro->numProperties();
  if (propNum > 0) {
     for (i = 0; i < propNum; i++) {
        fprintf(fout, "MACRO %s PROPERTY", macro->name());
        // value can either be a string or number
        if (macro->propValue(i)) {
           fprintf(fout, " %s %s", macro->propName(i), macro->propValue(i));
        }
        else
           fprintf(fout, " %s %g", macro->propName(i), macro->propNum(i));
        fprintf(fout, "\n");
     }
  }
  return 0;
}

// Minfeature
int minFeatureCB(lefrCallbackType_e c, lefiMinFeature* min, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "MINFEATURE %g %g ;\n", min->one(), min->two());
  return 0;
}

// Universalnoisemargin
int noiseMarginCB(lefrCallbackType_e c, lefiNoiseMargin* margin,
                  lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "UNIVERSALNOISEMARGIN %g %g\n", margin->high, margin->low);
  return 0;
}

// NoiseTable
int noiseTableCB(lefrCallbackType_e c, lefiNoiseTable* table, lefiUserData ud) {
  int                 i, j, k, l;
  lefiNoiseEdge       *edge;
  lefiNoiseResistance *resist;
  lefiNoiseVictim     *victim;
 
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "CROSSTALK NOISETABLE %d\n", table->num());
  if (table->numEdges() > 0) {
     for (i = 0; i < table->numEdges(); i++) {
        edge = table->edge(i);
        // resistances
        if (edge->numResistances()) {
           for (j = 0; j < edge->numResistances(); j++) {
              resist = edge->resistance(j);
              fprintf(fout, "NOISETABLE %d RESISTANCE %g\n",
                      table->num(), resist->num(j));
              // victims
              for (k = 0; k < resist->numVictims(); k++) {
                 victim = resist->victim(k);
                 if (victim->length() > 0)
                    fprintf(fout, "CROSSTALK NOISETABLE %d RESISTANCE %g VICTIMLENGTH % g\n",
                            table->num(), resist->num(j), victim->length());
                    for (l = 0; l < victim->numNoises(); l++)
                       fprintf(fout, "CROSSTALK NOISETABLE %d RESISTANCE %g VICTIMLENGT g NOISEFACTOR %g\n",
                          table->num(), resist->num(j), victim->length(),
                          victim->noise(l));
              }
           }
        }
     }
  }
  return 0;
}

// Nondefault
int nonDefaultCB(lefrCallbackType_e c, lefiNonDefault* def, lefiUserData ud) {
  int          i;
  lefiVia*     via;
  lefiSpacing* spacing;
  char         defName[1024];

  checkType(c);
  if ((long)ud != userData) dataError();
  for (i = 0; i < def->numLayers(); i++) {
     fprintf(fout, "NONDEFAULTRULE %s LAYER %s", def->name(),
             def->layerName(i));
     if (def->hasLayerWidth(i))
        fprintf(fout, " WIDTH %g", def->layerWidth(i));
     if (def->hasLayerSpacing(i))
        fprintf(fout, " SPACING %g", def->layerSpacing(i));
     fprintf(fout, "\n");
  }

  sprintf(defName, "NONDEFAULTRULE %s VIA", def->name());
  // handle via in nondefaultrule
  for (i = 0; i < def->numVias(); i++) {
     via = def->viaRule(i);
     lefVia(via, defName);
  }

  sprintf(defName, "NONDEFAULTRULE %s SPACING", def->name());
  // handle spacing in nondefaultrule
  for (i = 0; i < def->numSpacingRules(); i++) {
     spacing = def->spacingRule(i);
     lefSpacing(spacing, defName);
  }

  return 0;
}

// Nowireextension
int noWireExtCB(lefrCallbackType_e c, const char* wireExt, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "NOWIREEXTENSION %s\n", wireExt);
  return 0;
}

// Obstruction
int obstructionCB(lefrCallbackType_e c, lefiObstruction* obs,
                  lefiUserData ud) {
  lefiGeometries* geometry;

  checkType(c);
  if ((long)ud != userData) dataError();
  geometry = obs->geometries();
  prtGeometry(geometry, (char*)"OBS");
  return 0;
}

// Pin
int pinCB(lefrCallbackType_e c, lefiPin* pin, lefiUserData ud) {
  int              numPorts, i;
  lefiGeometries*  geometry;
 
  checkType(c);
  if ((long)ud != userData) dataError();
  if (pin->hasForeign()) {
     if (pin->hasForeignOrient())
        fprintf(fout, "PIN %s FOREIGN %s STRUCTURE %g %g %s\n", pin->name(),
                pin->foreignName(), pin->foreignX(), pin->foreignY(),
                pin->foreignOrient());
     else if (pin->hasForeignPoint())
        fprintf(fout, "PIN %s FOREIGN %s STRUCTURE %g %g", pin->name(),
                pin->foreignName(), pin->foreignX(), pin->foreignY());
     else
        fprintf(fout, "PIN %s FOREIGN %s\n", pin->name(), pin->foreignName());
  }
  if (pin->hasLEQ())
     fprintf(fout, "PIN %s LEQ %s\n", pin->name(), pin->LEQ());
  if (pin->hasDirection())
     fprintf(fout, "PIN %s DIRECTION %s\n", pin->name(), pin->direction());
  if (pin->hasUse())
     fprintf(fout, "PIN %s USE %s\n", pin->name(), pin->use());
  if (pin->hasShape())
     fprintf(fout, "PIN %s SHAPE %s\n", pin->name(), pin->shape());
  if (pin->hasMustjoin())
     fprintf(fout, "PIN %s MUSTJOIN %s\n", pin->name(), pin->mustjoin());
  if (pin->hasOutMargin())
     fprintf(fout, "PIN %s OUTPUTNOISEMARGIN %g %g\n", pin->name(),
             pin->outMarginHigh(), pin->outMarginLow());
  if (pin->hasOutResistance())
     fprintf(fout, "PIN %s OUTPUTRESISTANCE %g %g\n", pin->name(),
             pin->outResistanceHigh(), pin->outResistanceLow());
  if (pin->hasInMargin())
     fprintf(fout, "PIN %s INPUTNOISEMARGIN %g %g\n", pin->name(),
             pin->inMarginHigh(), pin->inMarginLow());
  if (pin->hasPower())
     fprintf(fout, "PIN %s POWER %g\n", pin->name(), pin->power());
  if (pin->hasLeakage())
     fprintf(fout, "PIN %s LEAKAGE %g\n", pin->name(), pin->leakage());
  if (pin->hasMaxload())
     fprintf(fout, "PIN %s MAXLOAD %g\n", pin->name(), pin->maxload());
  if (pin->hasCapacitance())
     fprintf(fout, "PIN %s CAPACITANCE %g\n", pin->name(), pin->capacitance());
  if (pin->hasResistance())
     fprintf(fout, "PIN %s RESISTANCE %g\n", pin->name(), pin->resistance());
  if (pin->hasPulldownres())
     fprintf(fout, "PIN %s PULLDOWNRES %g\n", pin->name(), pin->pulldownres());
  if (pin->hasTieoffr())
     fprintf(fout, "PIN %s TIEOFFR %g\n", pin->name(), pin->tieoffr());
  if (pin->hasVHI())
     fprintf(fout, "PIN %s VHI %g\n", pin->name(), pin->VHI());
  if (pin->hasVLO())
     fprintf(fout, "PIN %s VLO %g\n", pin->name(), pin->VLO());
  if (pin->hasRiseVoltage())
     fprintf(fout, "PIN %s RISEVOLTAGETHRESHOLD %g\n", pin->name(),
             pin->riseVoltage());
  if (pin->hasFallVoltage())
     fprintf(fout, "PIN %s FALLVOLTAGETHRESHOLD %g\n", pin->name(),
             pin->fallVoltage());
  if (pin->hasRiseThresh())
     fprintf(fout, "PIN %s RISETHRESH %g\n", pin->name(), pin->riseThresh());
  if (pin->hasFallThresh())
     fprintf(fout, "PIN %s FALLTHRESH %g\n", pin->name(), pin->fallThresh());
  if (pin->hasRiseSatcur())
     fprintf(fout, "PIN %s RISESATCUR %g\n", pin->name(), pin->riseSatcur());
  if (pin->hasFallSatcur())
     fprintf(fout, "PIN %s FALLSATCUR %g\n", pin->name(), pin->fallSatcur());
  if (pin->hasCurrentSource())
     fprintf(fout, "PIN %s CURRENTSOURCE %s\n", pin->name(),
             pin->currentSource());
  if (pin->hasTables())
     fprintf(fout, "PIN %s IV_TABLES %s %s\n", pin->name(),
             pin->tableHighName(), pin->tableLowName());
  if (pin->hasAntennaSize()) {
     for (i = 0; i < pin->numAntennaSize(); i++)
        fprintf(fout, "PIN %s ANTENNASIZE %g LAYER %s\n", pin->name(),
           pin->antennaSize(i), pin->antennaSizeLayer(i));
  }
  if (pin->hasAntennaMetalArea()) {
     for (i = 0; i < pin->numAntennaMetalArea(); i++)
        fprintf(fout, "PIN %s ANTENNAMETALAREA %g LAYER %s\n", pin->name(),
           pin->antennaMetalArea(i), pin->antennaMetalAreaLayer(i));
  }
  if (pin->hasAntennaMetalLength()) {
     for (i = 0; i < pin->numAntennaMetalLength(); i++)
        fprintf(fout, "PIN %s ANTENNAMETALLENGTH %g LAYER %s\n", pin->name(),
           pin->antennaMetalLength(i), pin->antennaMetalLengthLayer(i));
  }

  if (pin->numProperties() > 0) {
     fprintf(fout, "PIN %s PROPERTY ");
     for (i = 0; i < pin->numProperties(); i++) {
        // value can either be a string or number
        if (pin->propValue(i)) {
           fprintf(fout, "%s %s ", pin->propName(i), pin->propValue(i));
        }
        else
           fprintf(fout, "%s %g ", pin->propName(i), pin->propNum(i));
     }
     fprintf(fout, ";\n");
  }
  
  numPorts = pin->numPorts();

  char pinName[1024];

  sprintf(pinName, "PIN %s PORT", pin->name());
  for (i = 0; i < numPorts; i++) {
     geometry = pin->port(i);
     prtGeometry(geometry, pinName);
  }
  return 0;  
}

// Property definition
int propDefCB(lefrCallbackType_e c, lefiProp* prop, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "PROPERTYDEFINITION %s %s",
          prop->propType(), prop->propName());
  if (prop->hasRange())
     fprintf(fout, " RANGE %g %g", prop->left(), prop->right());
  if (prop->hasNumber())
     fprintf(fout, " NUMBER %g", prop->number());
  switch (prop->dataType()) {
      case 'I':
           fprintf(fout, " TYPE I");
           break;
      case 'R':
           fprintf(fout, " TYPE R");
           break;
      case 'S':
           fprintf(fout, " TYPE S");
           break;
      case 'Q':
           fprintf(fout, " TYPE Q");
           break;
      case 'N':
           fprintf(fout, " TYPE N");
           break;
  }
  fprintf(fout, "\n");
  return 0;
}

// Site
int siteCB(lefrCallbackType_e c, lefiSite* site, lefiUserData ud) {
  int hasPrtSym = 0;

  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "SITE %s", site->name());
  if (site->hasClass())
     fprintf(fout, " CLASS %s", site->siteClass());
  if (site->hasXSymmetry()) {
     fprintf(fout, " SYMMETRY X ");
     hasPrtSym = 1;
  }
  if (site->hasYSymmetry()) {
     if (hasPrtSym)
        fprintf(fout, "Y ");
     else {
        fprintf(fout, " SYMMETRY Y ");
        hasPrtSym = 1;
     }
  }
  if (site->has90Symmetry()) {
     if (hasPrtSym)
        fprintf(fout, "R90 ");
     else {
        fprintf(fout, " SYMMETRY R90 ");
        hasPrtSym = 1;
     }
  }
  if (site->hasSize())
     fprintf(fout, " SIZE %g BY %g", site->sizeX(), site->sizeY());
  fprintf(fout, "\n");
  return 0;
}

// Spacing
int spacingCB(lefrCallbackType_e c, lefiSpacing* spacing, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  lefSpacing(spacing, (char*)"SPACING");
  return 0;
}

// Timing
int timingCB(lefrCallbackType_e c, lefiTiming* timing, lefiUserData ud) {
  int i;
  checkType(c);
  if ((long)ud != userData) dataError();
  for (i = 0; i < timing->numFromPins(); i++)
     fprintf(fout, "TIMING FROMPIN %s\n", timing->fromPin(i));
  for (i = 0; i < timing->numToPins(); i++)
     fprintf(fout, "TIMING TOPIN %s\n", timing->toPin(i));
     fprintf(fout, "TIMING RISE SLEW1 %g %g %g %g\n", timing->riseSlewOne(),
             timing->riseSlewTwo(), timing->riseSlewThree(),
             timing->riseSlewFour());
  if (timing->hasRiseSlew2())
     fprintf(fout, "TIMING RISE SLEW2 %g %g %g %g\n", timing->riseSlewFive(),
             timing->riseSlewSix(), timing->riseSlewSeven());
  if (timing->hasFallSlew())
     fprintf(fout, "TIMING FALL SLEW1 %g %g %g %g\n", timing->fallSlewOne(),
             timing->fallSlewTwo(), timing->fallSlewThree(),
             timing->fallSlewFour());
  if (timing->hasFallSlew2())
     fprintf(fout, "TIMING FALL SLEW2 %g %g %g %g\n", timing->fallSlewFive(),
             timing->fallSlewSix(), timing->riseSlewSeven());
  if (timing->hasRiseIntrinsic()) {
     fprintf(fout, "TIMING RISE INTRINSIC %g %g\n",
             timing->riseIntrinsicOne(), timing->riseIntrinsicTwo());
     fprintf(fout, "TIMING RISE VARIABLE %g %g\n",
             timing->riseIntrinsicThree(), timing->riseIntrinsicFour());
  }
  if (timing->hasFallIntrinsic()) {
     fprintf(fout, "TIMING FALL INTRINSIC %g %g\n",
             timing->fallIntrinsicOne(), timing->fallIntrinsicTwo());
     fprintf(fout, "TIMING RISE VARIABLE %g %g\n",
             timing->fallIntrinsicThree(), timing->fallIntrinsicFour());
  }
  if (timing->hasRiseRS())
     fprintf(fout, "TIMING RISERS %g %g\n",
             timing->riseRSOne(), timing->riseRSTwo());
     if (timing->hasRiseCS())
     fprintf(fout, "TIMING RISECS %g %g\n",
             timing->riseCSOne(), timing->riseCSTwo());
  if (timing->hasFallRS())
     fprintf(fout, "TIMING FALLRS %g %g\n",
             timing->fallRSOne(), timing->fallRSTwo());
  if (timing->hasFallCS())
     fprintf(fout, "TIMING FALLCS %g %g\n",
             timing->fallCSOne(), timing->fallCSTwo());
  if (timing->hasUnateness())
     fprintf(fout, "TIMING UNATENESS %s\n", timing->unateness());
  if (timing->hasRiseAtt1())
     fprintf(fout, "TIMING RISESATT1 %g %g\n", timing->riseAtt1One(),
             timing->riseAtt1Two());
  if (timing->hasFallAtt1())
     fprintf(fout, "TIMING FALLSATT1 %g %g\n", timing->fallAtt1One(),
             timing->fallAtt1Two());
  if (timing->hasRiseTo())
     fprintf(fout, "TIMING RISET0 %g %g\n", timing->riseToOne(),
             timing->riseToTwo());
  if (timing->hasFallTo())
     fprintf(fout, "TIMING FALLT0 %g %g\n", timing->fallToOne(),
             timing->fallToTwo());
  return 0;
}

// Units
int unitsCB(lefrCallbackType_e c, lefiUnits* unit, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  if (unit->hasDatabase())
     fprintf(fout, "UNITS DATABASE %s %g\n", unit->databaseName(),
             unit->databaseNumber());
  if (unit->hasCapacitance())
     fprintf(fout, "UNITS CAPACITANCE PICOFARADS %g\n", unit->capacitance());
  if (unit->hasResistance())
     fprintf(fout, "UNITS RESISTANCE OHMS %g\n", unit->resistance());
  if (unit->hasPower())
     fprintf(fout, "UNITS POWER MILLIWATTS %g\n", unit->power());
  if (unit->hasCurrent())
     fprintf(fout, "UNITS CURRENT MILLIAMPS %g\n", unit->current());
  if (unit->hasVoltage())
     fprintf(fout, "UNITS VOLTAGE VOLTS %g\n", unit->voltage());
  return 0;
}

// Version
int versionCB(lefrCallbackType_e c, double num, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "VERSION %g\n", num);
  return 0;
}

// Via
int viaCB(lefrCallbackType_e c, lefiVia* via, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  lefVia(via, (char*)"VIA");
  return 0;
}

// Viarule
int viaRuleCB(lefrCallbackType_e c, lefiViaRule* viaRule, lefiUserData ud) {
  int               numLayers, numVias, i;
  lefiViaRuleLayer* vLayer;

  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "VIARULE %s", viaRule->name());
  if (viaRule->hasGenerate())
     fprintf(fout, " GENERATE\n");
  else
     fprintf(fout, "\n");

  numLayers = viaRule->numLayers();
  // if numLayers == 2, it is VIARULE without GENERATE and has via name
  // if numLayers == 3, it is VIARULE with GENERATE, and the 3rd layer is cut
  for (i = 0; i < numLayers; i++) {
     vLayer = viaRule->layer(i); 
     lefViaRuleLayer(vLayer, viaRule->name());
  }

  if (numLayers == 2) {    // should have vianames
     numVias = viaRule->numVias();
     if (numVias == 0)
        fprintf(fout, "Should have via names in VIARULE.\n");
     else {
        for (i = 0; i < numVias; i++)
           fprintf(fout, "VIARULE %s  VIA %s ;\n", viaRule->name(),
                   viaRule->viaName(i));
     }
  }
  return 0;
}

//========
 
int diffLefReadFile(char* inFile, char* outFile) {
  FILE* f;
  int   res;
 
  userData = 0x01020304;
  lefrInit();

  lefrSetArrayCbk(arrayCB);
  lefrSetBusBitCharsCbk(busBitCharsCB);
  lefrSetCaseSensitiveCbk(caseSensCB);
  lefrSetCorrectionTableCbk(correctTableCB);
  lefrSetDielectricCbk(dielectricCB);
  lefrSetDividerCharCbk(dividerCB);
  lefrSetEdgeRateScaleFactorCbk(edgeRateCB);
  lefrSetEdgeRateThreshold1Cbk(edgeRate1CB);
  lefrSetEdgeRateThreshold2Cbk(edgeRate2CB);
  lefrSetInputAntennaCbk(inputAntCB);
  lefrSetOutputAntennaCbk(outputAntCB);
  lefrSetInoutAntennaCbk(inoutAntCB);
  lefrSetIRDropCbk(irdropCB);
  lefrSetLayerCbk(layerCB);
  lefrSetMacroCbk(macroCB);
  lefrSetMinFeatureCbk(minFeatureCB);
  lefrSetNoiseMarginCbk(noiseMarginCB);
  lefrSetNoiseTableCbk(noiseTableCB);
  lefrSetNonDefaultCbk(nonDefaultCB);
  lefrSetNoWireExtensionCbk(noWireExtCB);
  lefrSetObstructionCbk(obstructionCB);
  lefrSetPinCbk(pinCB);
  lefrSetPropCbk(propDefCB);
  lefrSetSiteCbk(siteCB);
  lefrSetSpacingCbk(spacingCB);
  lefrSetTimingCbk(timingCB);
  lefrSetUnitsCbk(unitsCB);
  lefrSetUserData((void*)3);
  lefrSetVersionCbk(versionCB);
  lefrSetViaCbk(viaCB);
  lefrSetViaRuleCbk(viaRuleCB);

  if ((f = fopen(inFile,"r")) == 0) {
    fprintf(stderr,"Couldn't open input file '%s'\n", inFile);
    return(2);
  }

  if ((fout = fopen(outFile,"w")) == 0) {
    fprintf(stderr,"Couldn't open output file '%s'\n", outFile);
    return(2);
  }

  res = lefrRead(f, inFile, (void*)userData);

  fclose(f);
  fclose(fout);

  return 0;
}
