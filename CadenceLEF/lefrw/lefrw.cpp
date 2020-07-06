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


char* orientStr(int orient) {
  switch (orient) {
      case 0: return ((char*)"N");
      case 1: return ((char*)"W");
      case 2: return ((char*)"S");
      case 3: return ((char*)"E");
      case 4: return ((char*)"FN");
      case 5: return ((char*)"FE");
      case 6: return ((char*)"FS");
      case 7: return ((char*)"FW");
  };
  return ((char*)"BOGUS");
}

void lefVia(lefiVia* via) {
  int i, j;

  fprintf(fout, "VIA %s ", via->lefiVia::name());
  if (via->lefiVia::hasDefault())
     fprintf(fout, "DEFAULT\n");
  else
     fprintf(fout, "\n");
  if (via->lefiVia::hasTopOfStack())
     fprintf(fout, "  TOPOFSTACKONLY\n");
  if (via->lefiVia::hasForeign()) {
     fprintf(fout, "  FOREIGN %s ", via->lefiVia::foreign());
     if (via->lefiVia::hasForeignPnt()) {
        fprintf(fout, "( %g %g ) ", via->lefiVia::foreignX(),
                via->lefiVia::foreignY());
        if (via->lefiVia::hasForeignOrient())
           fprintf(fout, "%s ", orientStr(via->lefiVia::foreignOrient()));
     }
     fprintf(fout, ";\n");
  }
  if (via->lefiVia::hasProperties()) {
     fprintf(fout, "  PROPERTY ");
     for (i = 0; i < via->lefiVia::numProperties(); i++) {
        fprintf(fout, "%s ", via->lefiVia::propName(i));
        if (via->lefiVia::propIsNumber(i))
           fprintf(fout, "%g ", via->lefiVia::propNumber(i));
        if (via->lefiVia::propIsString(i))
           fprintf(fout, "%s ", via->lefiVia::propValue(i));
/*
        if (i+1 == via->lefiVia::numProperties())  // end of properties
           fprintf(fout, ";\n");
        else      // just add new line
           fprintf(fout, "\n");
*/
     }
     fprintf(fout, ";\n");
  }
  if (via->lefiVia::hasResistance())
     fprintf(fout, "  RESISTANCE %g ;\n", via->lefiVia::resistance());
  if (via->lefiVia::numLayers() > 0) {
     for (i = 0; i < via->lefiVia::numLayers(); i++) {
        fprintf(fout, "  LAYER %s\n", via->lefiVia::layerName(i));
        for (j = 0; j < via->lefiVia::numRects(i); j++)
           fprintf(fout, "    RECT ( %g %g ) ( %g %g ) ;\n",
                  via->lefiVia::xl(i, j), via->lefiVia::yl(i, j),
                  via->lefiVia::xh(i, j), via->lefiVia::yh(i, j));
     }
  }
  fprintf(fout, "END %s\n", via->lefiVia::name());

  return;
}

void lefSpacing(lefiSpacing* spacing) {
  fprintf(fout, "  SAMENET %s %s %g ", spacing->lefiSpacing::name1(),
          spacing->lefiSpacing::name2(), spacing->lefiSpacing::distance());
  if (spacing->lefiSpacing::hasStack())
     fprintf(fout, "STACK ");
  fprintf(fout,";\n");
  return;
}

void lefViaRuleLayer(lefiViaRuleLayer* vLayer) {
  fprintf(fout, "  LAYER %s ;\n", vLayer->lefiViaRuleLayer::name());
  if (vLayer->lefiViaRuleLayer::hasDirection()) {
     if (vLayer->lefiViaRuleLayer::isHorizontal())
        fprintf(fout, "    DIRECTION HORIZONTAL ;\n");
     if (vLayer->lefiViaRuleLayer::isVertical())
        fprintf(fout, "    DIRECTION VERTICAL ;\n");
  }
  if (vLayer->lefiViaRuleLayer::hasWidth())
     fprintf(fout, "    WIDTH %g TO %g ;\n",
             vLayer->lefiViaRuleLayer::widthMin(),
             vLayer->lefiViaRuleLayer::widthMax());
  if (vLayer->lefiViaRuleLayer::hasResistance())
     fprintf(fout, "    RESISTANCE %g ;\n",
             vLayer->lefiViaRuleLayer::resistance());
  if (vLayer->lefiViaRuleLayer::hasOverhang())
     fprintf(fout, "    OVERHANG %g ;\n",
             vLayer->lefiViaRuleLayer::overhang());
  if (vLayer->lefiViaRuleLayer::hasMetalOverhang())
     fprintf(fout, "    METALOVERHANG %g ;\n",
             vLayer->lefiViaRuleLayer::metalOverhang());
  if (vLayer->lefiViaRuleLayer::hasSpacing())
     fprintf(fout, "    SPACING %g BY %g ;\n",
             vLayer->lefiViaRuleLayer::spacingStepX(),
             vLayer->lefiViaRuleLayer::spacingStepY());
  if (vLayer->lefiViaRuleLayer::hasRect())
     fprintf(fout, "    RECT ( %g %g ) ( %g %g ) ;\n",
             vLayer->lefiViaRuleLayer::xl(), vLayer->lefiViaRuleLayer::yl(),
             vLayer->lefiViaRuleLayer::xh(), vLayer->lefiViaRuleLayer::yh());
  return;
}
 
void prtGeometry(lefiGeometries* geometry) {
  int                   numItems = geometry->lefiGeometries::numItems();
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
     switch (geometry->lefiGeometries::itemType(i)) {
        case  lefiGeomClassE:
             fprintf(fout, "CLASS %s ",
                     geometry->lefiGeometries::getClass(i));
             break;
        case lefiGeomLayerE:
             fprintf(fout, "      LAYER %s ;\n",
                     geometry->lefiGeometries::getLayer(i));
             break;
        case lefiGeomWidthE:
             fprintf(fout, "      WIDTH %g ;\n",
                     geometry->lefiGeometries::getWidth(i));
             break;
        case lefiGeomPathE:
             path = geometry->lefiGeometries::getPath(i);
             fprintf(fout, "      PATH ");
             for (j = 0; j < path->numPoints; j++) {
                if (j+1 == path->numPoints) // last one on the list
                   fprintf(fout, "      ( %g %g ) ;\n", path->x[j], path->y[j]);
                else
                   fprintf(fout, "      ( %g %g )\n", path->x[j], path->y[j]);
             }
             break;
        case lefiGeomPathIterE:
             pathIter = geometry->lefiGeometries::getPathIter(i);
             fprintf(fout, "      PATH ITERATED ");
             for (j = 0; j < pathIter->numPoints; j++)
                fprintf(fout, "      ( %g %g )\n", pathIter->x[j],
                        pathIter->y[j]);
             fprintf(fout, "      DO %g BY %g STEP %g %g ;\n", pathIter->xStart,
                     pathIter->yStart, pathIter->xStep, pathIter->yStep);
             break;
        case lefiGeomRectE:
             rect = geometry->lefiGeometries::getRect(i);
             fprintf(fout, "      RECT ( %g %g ) ( %g %g ) ;\n", rect->xl, 
                     rect->yl, rect->xh, rect->yh);
             break;
        case lefiGeomRectIterE:
             rectIter = geometry->lefiGeometries::getRectIter(i);
             fprintf(fout, "      RECT ITERATE ( %g %g ) ( %g %g )\n",
                     rectIter->xl, rectIter->yl, rectIter->xh, rectIter->yh);
             fprintf(fout, "      DO %g BY %g STEP %g %g ;\n",
                     rectIter->xStart, rectIter->yStart, rectIter->xStep,
                     rectIter->yStep);
             break;
        case lefiGeomPolygonE:
             polygon = geometry->lefiGeometries::getPolygon(i);
             fprintf(fout, "      POLYGON ");
             for (j = 0; j < polygon->numPoints; j++) {
                if (j+1 == polygon->numPoints) // last one on the list
                   fprintf(fout, "      ( %g %g ) ;\n", polygon->x[j],
                           polygon->y[j]);
                else
                   fprintf(fout, "      ( %g %g )\n", polygon->x[j],
                           polygon->y[j]);
             }
             break;
        case lefiGeomPolygonIterE:
             polygonIter = geometry->lefiGeometries::getPolygonIter(i);
             fprintf(fout, "      POLYGON ITERATE");
             for (j = 0; j < polygonIter->numPoints; j++)
                   fprintf(fout, "      ( %g %g )\n", polygonIter->x[j],
                           polygonIter->y[j]);
             fprintf(fout, "      DO %g BY %g STEP %g %g ;\n",
                     polygonIter->xStart, polygonIter->yStart,
                     polygonIter->xStep, polygonIter->yStep);
             break;
        case lefiGeomViaE:
             via = geometry->lefiGeometries::getVia(i);
             fprintf(fout, "      VIA ( %g %g ) %s ;\n", via->x, via->y,
                     via->name);
             break;
        case lefiGeomViaIterE:
             viaIter = geometry->lefiGeometries::getViaIter(i);
             fprintf(fout, "      VIA ITERATE ( %g %g ) %s\n", viaIter->x,
                     viaIter->y, viaIter->name);
             fprintf(fout, "      DO %g BY %g STEP %g %g ;\n",
                     viaIter->xStart, viaIter->yStart,
                     viaIter->xStep, viaIter->yStep);
             break;
        default:
             fprintf(fout, "BOGUS geometries type.\n");
             break;
     }
  }
}

int arrayBeginCB(lefrCallbackType_e c, const char* name, lefiUserData ud) {
  int  status;

  checkType(c);
  if ((long)ud != userData) dataError();
  // use the lef writer to write the data out
  status = lefwStartArray(name);
  if (status != LEFW_OK)
     return status;
  return 0;
}

int arrayCB(lefrCallbackType_e c, lefiArray* a, lefiUserData ud) {
  int              status, i, j, defCaps;
  lefiSitePattern* pattern;
  lefiTrackPattern* track;
  lefiGcellPattern* gcell;

  checkType(c);
  if ((long)ud != userData) dataError();
   
  if (a->lefiArray::numSitePattern() > 0) {
     for (i = 0; i < a->lefiArray::numSitePattern(); i++) {
        pattern = a->lefiArray::sitePattern(i);
        status = lefwArraySite(pattern->lefiSitePattern::name(),
                               pattern->lefiSitePattern::x(),
                               pattern->lefiSitePattern::y(),
                               pattern->lefiSitePattern::orient(),
                               pattern->lefiSitePattern::xStart(),
                               pattern->lefiSitePattern::yStart(),
                               pattern->lefiSitePattern::xStep(),
                               pattern->lefiSitePattern::yStep());
        if (status != LEFW_OK)
           dataError();
     }
  }
  if (a->lefiArray::numCanPlace() > 0) {
     for (i = 0; i < a->lefiArray::numCanPlace(); i++) {
        pattern = a->lefiArray::canPlace(i);
        status = lefwArrayCanplace(pattern->lefiSitePattern::name(),
                                   pattern->lefiSitePattern::x(),
                                   pattern->lefiSitePattern::y(),
                                   pattern->lefiSitePattern::orient(),
                                   pattern->lefiSitePattern::xStart(),
                                   pattern->lefiSitePattern::yStart(),
                                   pattern->lefiSitePattern::xStep(),
                                   pattern->lefiSitePattern::yStep());
        if (status != LEFW_OK)
           dataError();
     }
  }
  if (a->lefiArray::numCannotOccupy() > 0) {
     for (i = 0; i < a->lefiArray::numCannotOccupy(); i++) {
        pattern = a->lefiArray::cannotOccupy(i);
        status = lefwArrayCannotoccupy(pattern->lefiSitePattern::name(),
                                       pattern->lefiSitePattern::x(),
                                       pattern->lefiSitePattern::y(),
                                       pattern->lefiSitePattern::orient(),
                                       pattern->lefiSitePattern::xStart(),
                                       pattern->lefiSitePattern::yStart(),
                                       pattern->lefiSitePattern::xStep(),
                                       pattern->lefiSitePattern::yStep());
        if (status != LEFW_OK)
           dataError();
     }
  }

  if (a->lefiArray::numTrack() > 0) {
     for (i = 0; i < a->lefiArray::numTrack(); i++) {
        track = a->lefiArray::track(i);
        fprintf(fout, "  TRACKS %s, %g DO %d STEP %g\n",
                track->lefiTrackPattern::name(),
                track->lefiTrackPattern::start(), 
                track->lefiTrackPattern::numTracks(), 
                track->lefiTrackPattern::space()); 
        if (track->lefiTrackPattern::numLayers() > 0) {
           fprintf(fout, "  LAYER ");
           for (j = 0; j < track->lefiTrackPattern::numLayers(); j++)
              fprintf(fout, "%s ", track->lefiTrackPattern::layerName(j));
           fprintf(fout, ";\n"); 
        }
     }
  }

  if (a->lefiArray::numGcell() > 0) {
     for (i = 0; i < a->lefiArray::numGcell(); i++) {
        gcell = a->lefiArray::gcell(i);
        fprintf(fout, "  GCELLGRID %s, %g DO %d STEP %g\n",
                gcell->lefiGcellPattern::name(),
                gcell->lefiGcellPattern::start(), 
                gcell->lefiGcellPattern::numCRs(), 
                gcell->lefiGcellPattern::space()); 
     }
  }

  if (a->lefiArray::numFloorPlans() > 0) {
     for (i = 0; i < a->lefiArray::numFloorPlans(); i++) {
        status = lefwStartArrayFloorplan(a->lefiArray::floorPlanName(i));
        if (status != LEFW_OK)
           dataError();
        for (j = 0; j < a->lefiArray::numSites(i); j++) {
           pattern = a->lefiArray::site(i, j);
           status = lefwArrayFloorplan(a->lefiArray::siteType(i, j),
                                       pattern->lefiSitePattern::name(),
                                       pattern->lefiSitePattern::x(),
                                       pattern->lefiSitePattern::y(),
                                       pattern->lefiSitePattern::orient(),
                                       pattern->lefiSitePattern::xStart(),
                                       pattern->lefiSitePattern::yStart(),
                                       pattern->lefiSitePattern::xStep(),
                                       pattern->lefiSitePattern::yStep());
           if (status != LEFW_OK)
              dataError();
        }
     status = lefwEndArrayFloorplan(a->lefiArray::floorPlanName(i));
     if (status != LEFW_OK)
        dataError();
     }
  }

  defCaps = a->lefiArray::numDefaultCaps();
  if (defCaps > 0) {
     status = lefwStartArrayDefaultCap(defCaps);
     if (status != LEFW_OK)
        dataError();
     for (i = 0; i < defCaps; i++) {
        status = lefwArrayDefaultCap(a->lefiArray::defaultCapMinPins(i),
                                     a->lefiArray::defaultCap(i));
        if (status != LEFW_OK)
           dataError();
     }
     status = lefwEndArrayDefaultCap();
     if (status != LEFW_OK)
        dataError();
  }
  return 0;
}

int arrayEndCB(lefrCallbackType_e c, const char* name, lefiUserData ud) {
  int  status;

  checkType(c);
  if ((long)ud != userData) dataError();
  // use the lef writer to write the data out
  status = lefwEndArray(name);
  if (status != LEFW_OK)
     return status;
  return 0;
}

int busBitCharsCB(lefrCallbackType_e c, const char* busBit, lefiUserData ud)
{
  int status;

  checkType(c);
  if ((long)ud != userData) dataError();
  // use the lef writer to write out the data
  status = lefwBusBitChars(busBit);
  if (status != LEFW_OK)
     dataError();
  return 0;
}

int caseSensCB(lefrCallbackType_e c, int caseSense, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();

  if (caseSense == TRUE)
     fprintf(fout, "NAMESCASESENSITIVE ON ;\n");
  else
     fprintf(fout, "NAMESCASESENSITIVE OFF ;\n");
  return 0;
}

int dielectricCB(lefrCallbackType_e c, double dielectric, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();

  fprintf(fout, "DIELECTRIC %g ;\n", dielectric);
  return 0;
}

int irdropBeginCB(lefrCallbackType_e c, void* ptr, lefiUserData ud){
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "IRDROP\n");
  return 0;
}

int irdropCB(lefrCallbackType_e c, lefiIRDrop* irdrop, lefiUserData ud) {
  int i;
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "  TABLE %s ", irdrop->lefiIRDrop::name());
  for (i = 0; i < irdrop->lefiIRDrop::numValues(); i++) 
     fprintf(fout, "%g %g ", irdrop->lefiIRDrop::value1(i),
             irdrop->lefiIRDrop::value2(i));
  fprintf(fout, ";\n");
  return 0;
}

int irdropEndCB(lefrCallbackType_e c, void* ptr, lefiUserData ud){
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "END IRDROP\n");
  return 0;
}

int layerCB(lefrCallbackType_e c, lefiLayer* layer, lefiUserData ud) {
  int i, j, k;
  int numPoints;
  double *widths, *current;
  lefiLayerDensity* density;
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "LAYER %s\n", layer->lefiLayer::name());
  if (layer->lefiLayer::hasType())
     fprintf(fout, "  TYPE %s ;\n", layer->lefiLayer::type());
  if (layer->lefiLayer::hasPitch())
     fprintf(fout, "  PITCH %g ;\n", layer->lefiLayer::pitch());
  if (layer->lefiLayer::hasOffset())
     fprintf(fout, "  OFFSET %g ;\n", layer->lefiLayer::offset());
  if (layer->lefiLayer::hasWidth())
     fprintf(fout, "  WIDTH %g ;\n", layer->lefiLayer::width());
  if (layer->lefiLayer::hasSpacingNumber()) {
     for (i = 0; i < layer->lefiLayer::numSpacing(); i++) {
       fprintf(fout, "  SPACING %g ", layer->lefiLayer::spacing(i));
       if (layer->lefiLayer::spacingName(i))
          fprintf(fout, "LAYER %s ", layer->lefiLayer::spacingName(i));
     }
     if (layer->lefiLayer::hasSpacingRange()) {
        fprintf(fout, "RANGE %g %g ", layer->lefiLayer::spacingRangeLeft(),
                layer->lefiLayer::spacingRangeRight());
        if (layer->lefiLayer::hasSpacingUseLengthThreshold())
           fprintf(fout, "USELENGTHTHRESHOLD "); 
     }
     if (layer->lefiLayer::hasSpacingLengthThreshold())
           fprintf(fout, "LENGTHTHRESHOLD %g ",
                   layer->lefiLayer::spacingLengthThreshold()); 
     fprintf(fout, ";\n");
  }
  if (layer->lefiLayer::hasCurrentDensityPoint())
     fprintf(fout, "  CURRENTDEN %g ;\n",
             layer->lefiLayer::currentDensityPoint());
  if (layer->lefiLayer::hasCurrentDensityArray()) { 
     layer->lefiLayer::currentDensityArray(&numPoints, &widths, &current);
     for (i = 0; i < numPoints; i++)
         fprintf(fout, "  CURRENTDEN ( %g %g ) ;\n", widths[i], current[i]);
  }
  if (layer->lefiLayer::hasDirection())
     fprintf(fout, "  DIRECTION %s ;\n", layer->lefiLayer::direction());
  if (layer->lefiLayer::hasResistance())
     fprintf(fout, "  RESISTANCE RPERSQ %g ;\n",
             layer->lefiLayer::resistance());
  if (layer->lefiLayer::hasCapacitance())
     fprintf(fout, "  CAPACITANCE CPERSQDIST %g ;\n",
             layer->lefiLayer::capacitance());
  if (layer->lefiLayer::hasHeight())
     fprintf(fout, "  TYPE %g ;\n", layer->lefiLayer::height());
  if (layer->lefiLayer::hasThickness())
     fprintf(fout, "  THICKNESS %g ;\n", layer->lefiLayer::thickness());
  if (layer->lefiLayer::hasWireExtension())
     fprintf(fout, "  WIREEXTENSION %g ;\n", layer->lefiLayer::wireExtension());
  if (layer->lefiLayer::hasShrinkage())
     fprintf(fout, "  SHRINKAGE %g ;\n", layer->lefiLayer::shrinkage());
  if (layer->lefiLayer::hasCapMultiplier())
     fprintf(fout, "  CAPMULTIPLIER %g ;\n", layer->lefiLayer::capMultiplier());
  if (layer->lefiLayer::hasEdgeCap())
     fprintf(fout, "  EDGECAPACITANCE %g ;\n", layer->lefiLayer::edgeCap());
  if (layer->lefiLayer::hasAntennaArea())
     fprintf(fout, "  ANTENNAAREAFACTOR %g ;\n",
             layer->lefiLayer::antennaArea());
  if (layer->lefiLayer::hasAntennaLength())
     fprintf(fout, "  ANTENNALENGTHFACTOR %g ;\n",
             layer->lefiLayer::antennaLength());
  if (layer->lefiLayer::numAccurrentDensity()) {
     for (i = 0; i < layer->lefiLayer::numAccurrentDensity(); i++) {
         density = layer->lefiLayer::accurrent(i);
         fprintf(fout, "  ACCURRENTDENSITY %s", density->type()); 
         if (density->hasOneEntry())
             fprintf(fout, " %g ;\n", density->oneEntry()); 
         else {
             fprintf(fout, "\n");
             if (density->numFrequency()) {
                fprintf(fout, "    FREQUENCY");
                for (j = 0; j < density->numFrequency(); j++)
                   fprintf(fout, " %g", density->frequency(j));
                fprintf(fout, " ;\n");    
             }
             if (density->numWidths()) {
                fprintf(fout, "    WIDTH");
                for (j = 0; j < density->numWidths(); j++)
                   fprintf(fout, " %g", density->width(j));
                fprintf(fout, " ;\n");    
             }
             if (density->numTableEntries()) {
                k = 5;
                fprintf(fout, "    TABLEENTRIES");
                for (j = 0; j < density->numTableEntries(); j++)
                   if (k > 4) {
                      fprintf(fout, "\n     %g", density->tableEntry(j));
                      k = 1;
                   } else {
                      fprintf(fout, " %g", density->tableEntry(j));
                      k++;
                   }
                fprintf(fout, " ;\n");    
             }
         }
     }
  }
  if (layer->lefiLayer::numDccurrentDensity()) {
     for (i = 0; i < layer->lefiLayer::numDccurrentDensity(); i++) {
         density = layer->lefiLayer::dccurrent(i);
         fprintf(fout, "  DCCURRENTDENSITY %s", density->type()); 
         if (density->hasOneEntry())
             fprintf(fout, " %g ;\n", density->oneEntry()); 
         else {
             fprintf(fout, "\n");
             if (density->numCutareas()) {
                fprintf(fout, "    CUTAREA");
                for (j = 0; j < density->numCutareas(); j++)
                   fprintf(fout, " %g", density->cutArea(j));
                fprintf(fout, " ;\n");    
             }
             if (density->numWidths()) {
                fprintf(fout, "    WIDTH");
                for (j = 0; j < density->numWidths(); j++)
                   fprintf(fout, " %g", density->width(j));
                fprintf(fout, " ;\n");    
             }
             if (density->numTableEntries()) {
                fprintf(fout, "    TABLEENTRIES");
                for (j = 0; j < density->numTableEntries(); j++)
                   fprintf(fout, " %g", density->tableEntry(j));
                fprintf(fout, " ;\n");    
             }
         }
     }
  }
  fprintf(fout, "END %s\n", layer->lefiLayer::name()); 
  return 0;
}

int macroBeginCB(lefrCallbackType_e c, const char* macroName, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "MACRO %s\n",  macroName);
  return 0;
}

int macroCB(lefrCallbackType_e c, lefiMacro* macro, lefiUserData ud) {
  lefiSitePattern* pattern;
  int              propNum, i, hasPrtSym = 0;

  checkType(c);
  if ((long)ud != userData) dataError();
  if (macro->lefiMacro::hasClass())
     fprintf(fout, "  CLASS %s ;\n", macro->lefiMacro::macroClass());
  if (macro->lefiMacro::hasEEQ())
     fprintf(fout, "  EEQ %s ;\n", macro->lefiMacro::EEQ());
  if (macro->lefiMacro::hasLEQ())
     fprintf(fout, "  LEQ %s ;\n", macro->lefiMacro::LEQ());
  if (macro->lefiMacro::hasSource())
     fprintf(fout, "  SOURCE %s ;\n", macro->lefiMacro::source());
  if (macro->lefiMacro::hasXSymmetry()) {
     fprintf(fout, "  SYMMETRY X ");
     hasPrtSym = 1;
  }
  if (macro->lefiMacro::hasYSymmetry()) {   // print X Y & R90 in one line
     if (!hasPrtSym) {
        fprintf(fout, "  SYMMETRY Y ");
        hasPrtSym = 1;
     }
     else
        fprintf(fout, "Y ");
  }
  if (macro->lefiMacro::has90Symmetry()) {
     if (!hasPrtSym) {
        fprintf(fout, "  SYMMETRY R90 ");
        hasPrtSym = 1;
     }
     else
        fprintf(fout, "R90 ");
  }
  if (hasPrtSym) {
     fprintf (fout, ";\n");
     hasPrtSym = 0;
  }
  if (macro->lefiMacro::hasSiteName())
     fprintf(fout, "  SITE %s ;\n", macro->lefiMacro::siteName());
  if (macro->lefiMacro::hasSitePattern()) {
     for (i = 0; i < macro->lefiMacro::numSitePattern(); i++ ) {
       pattern = macro->lefiMacro::sitePattern(i);
       fprintf(fout, "  SITE %s %g %g %s DO %g BY %g STEP %g %g ;\n",
             pattern->lefiSitePattern::name(), pattern->lefiSitePattern::x(),
             pattern->lefiSitePattern::y(),
             orientStr(pattern->lefiSitePattern::orient()),
             pattern->lefiSitePattern::xStart(),
             pattern->lefiSitePattern::yStart(),
             pattern->lefiSitePattern::xStep(),
             pattern->lefiSitePattern::yStep());
     }
  }
  if (macro->lefiMacro::hasSize())
     fprintf(fout, "  SIZE %g BY %g ;\n", macro->lefiMacro::sizeX(),
             macro->lefiMacro::sizeY());
  if (macro->lefiMacro::hasForeign()) {
     fprintf(fout, "  FOREIGN %s ", macro->lefiMacro::foreignName());
     if (macro->lefiMacro::hasForeignPoint()) {
        fprintf(fout, "( %g %g ) ", macro->lefiMacro::foreignX(),
                macro->lefiMacro::foreignY());
        if (macro->lefiMacro::hasForeignOrient())
           fprintf(fout, "%s ", orientStr( macro->lefiMacro::foreignOrient()));
     }
     fprintf(fout, ";\n");
  }
  if (macro->lefiMacro::hasOrigin())
     fprintf(fout, "  ORIGIN ( %g %g ) ;\n", macro->lefiMacro::originX(),
             macro->lefiMacro::originY());
  if (macro->lefiMacro::hasPower())
     fprintf(fout, "  POWER %g ;\n", macro->lefiMacro::power());
  propNum = macro->lefiMacro::numProperties();
  if (propNum > 0) {
     fprintf(fout, "  PROPERTY ");
     for (i = 0; i < propNum; i++) {
        // value can either be a string or number
        if (macro->lefiMacro::propValue(i)) {
           fprintf(fout, "%s %s ", macro->lefiMacro::propName(i),
                   macro->lefiMacro::propValue(i));
        }
        else
           fprintf(fout, "%s %g ", macro->lefiMacro::propName(i),
                   macro->lefiMacro::propNum(i));
     }
     fprintf(fout, ";\n");
  }
  fprintf(fout, "END %s\n", macro->lefiMacro::name());
  return 0;
}

int minFeatureCB(lefrCallbackType_e c, lefiMinFeature* min, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "MINFEATURE %g %g ;\n", min->lefiMinFeature::one(),
          min->lefiMinFeature::two());
  return 0;
}

int nonDefaultCB(lefrCallbackType_e c, lefiNonDefault* def, lefiUserData ud) {
  int          i;
  lefiVia*     via;
  lefiSpacing* spacing;

  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "NONDEFAULTRULE %s\n", def->lefiNonDefault::name());
  for (i = 0; i < def->lefiNonDefault::numLayers(); i++) {
     fprintf(fout, "  LAYER %s\n", def->lefiNonDefault::layerName(i));
     if (def->lefiNonDefault::hasLayerWidth(i))
        fprintf(fout, "    WIDTH %g ;\n", def->lefiNonDefault::layerWidth(i));
     if (def->lefiNonDefault::hasLayerSpacing(i))
        fprintf(fout, "    SPACING %g ;\n",
                def->lefiNonDefault::layerSpacing(i));
     fprintf(fout, "  END %s\n", def->lefiNonDefault::layerName(i));
  }

  // handle via in nondefaultrule
  for (i = 0; i < def->lefiNonDefault::numVias(); i++) {
     via = def->lefiNonDefault::viaRule(i);
     lefVia(via);
  }

  // handle spacing in nondefaultrule
  for (i = 0; i < def->lefiNonDefault::numSpacingRules(); i++) {
     spacing = def->lefiNonDefault::spacingRule(i);
     lefSpacing(spacing);
  }

  return 0;
}

int obstructionCB(lefrCallbackType_e c, lefiObstruction* obs,
                  lefiUserData ud) {
  lefiGeometries* geometry;

  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "  OBS\n");
  geometry = obs->lefiObstruction::geometries();
  prtGeometry(geometry);
  fprintf(fout, "  END\n");
  return 0;
}

int pinCB(lefrCallbackType_e c, lefiPin* pin, lefiUserData ud) {
  int              numPorts, i;
  lefiGeometries*  geometry;
 
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "  PIN %s\n", pin->lefiPin::name());
  if (pin->lefiPin::hasForeign()) {
     if (pin->lefiPin::hasForeignOrient())
        fprintf(fout, "    FOREIGN %s STRUCTURE ( %g %g ) %s ;\n",
                pin->lefiPin::foreignName(), pin->lefiPin::foreignX(),
                pin->lefiPin::foreignY(),
                orientStr(pin->lefiPin::foreignOrient()));
     else if (pin->lefiPin::hasForeignPoint())
        fprintf(fout, "    FOREIGN %s STRUCTURE ( %g %g ) ;\n",
                pin->lefiPin::foreignName(), pin->lefiPin::foreignX(),
                pin->lefiPin::foreignY());
     else
        fprintf(fout, "    FOREIGN %s ;\n", pin->lefiPin::foreignName());
  }
  if (pin->lefiPin::hasLEQ())
     fprintf(fout, "    LEQ %s ;\n", pin->lefiPin::LEQ());
  if (pin->lefiPin::hasDirection())
     fprintf(fout, "    DIRECTION %s ;\n", pin->lefiPin::direction());
  if (pin->lefiPin::hasUse())
     fprintf(fout, "    USE %s ;\n", pin->lefiPin::use());
  if (pin->lefiPin::hasShape())
     fprintf(fout, "    SHAPE %s ;\n", pin->lefiPin::shape());
  if (pin->lefiPin::hasMustjoin())
     fprintf(fout, "    MUSTJOIN %s ;\n", pin->lefiPin::mustjoin());
  if (pin->lefiPin::hasOutMargin())
     fprintf(fout, "    OUTPUTNOISEMARGIN %g %g ;\n",
             pin->lefiPin::outMarginHigh(), pin->lefiPin::outMarginLow());
  if (pin->lefiPin::hasOutResistance())
     fprintf(fout, "    OUTPUTRESISTANCE %g %g ;\n",
             pin->lefiPin::outResistanceHigh(),
             pin->lefiPin::outResistanceLow());
  if (pin->lefiPin::hasInMargin())
     fprintf(fout, "    INPUTNOISEMARGIN %g %g ;\n",
             pin->lefiPin::inMarginHigh(), pin->lefiPin::inMarginLow());
  if (pin->lefiPin::hasPower())
     fprintf(fout, "    POWER %g ;\n", pin->lefiPin::power());
  if (pin->lefiPin::hasLeakage())
     fprintf(fout, "    LEAKAGE %g ;\n", pin->lefiPin::leakage());
  if (pin->lefiPin::hasMaxload())
     fprintf(fout, "    MAXLOAD %g ;\n", pin->lefiPin::maxload());
  if (pin->lefiPin::hasCapacitance())
     fprintf(fout, "    CAPACITANCE %g ;\n", pin->lefiPin::capacitance());
  if (pin->lefiPin::hasResistance())
     fprintf(fout, "    RESISTANCE %g ;\n", pin->lefiPin::resistance());
  if (pin->lefiPin::hasPulldownres())
     fprintf(fout, "    PULLDOWNRES %g ;\n", pin->lefiPin::pulldownres());
  if (pin->lefiPin::hasTieoffr())
     fprintf(fout, "    TIEOFFR %g ;\n", pin->lefiPin::tieoffr());
  if (pin->lefiPin::hasVHI())
     fprintf(fout, "    VHI %g ;\n", pin->lefiPin::VHI());
  if (pin->lefiPin::hasVLO())
     fprintf(fout, "    VLO %g ;\n", pin->lefiPin::VLO());
  if (pin->lefiPin::hasRiseVoltage())
     fprintf(fout, "    RISEVOLTAGETHRESHOLD %g ;\n",
             pin->lefiPin::riseVoltage());
  if (pin->lefiPin::hasFallVoltage())
     fprintf(fout, "    FALLVOLTAGETHRESHOLD %g ;\n",
             pin->lefiPin::fallVoltage());
  if (pin->lefiPin::hasRiseThresh())
     fprintf(fout, "    RISETHRESH %g ;\n", pin->lefiPin::riseThresh());
  if (pin->lefiPin::hasFallThresh())
     fprintf(fout, "    FALLTHRESH %g ;\n", pin->lefiPin::fallThresh());
  if (pin->lefiPin::hasRiseSatcur())
     fprintf(fout, "    RISESATCUR %g ;\n", pin->lefiPin::riseSatcur());
  if (pin->lefiPin::hasFallSatcur())
     fprintf(fout, "    FALLSATCUR %g ;\n", pin->lefiPin::fallSatcur());
  if (pin->lefiPin::hasCurrentSource())
     fprintf(fout, "    CURRENTSOURCE %s ;\n", pin->lefiPin::currentSource());
  if (pin->lefiPin::hasTables())
     fprintf(fout, "    IV_TABLES %s %s ;\n", pin->lefiPin::tableHighName(),
             pin->lefiPin::tableLowName());
  if (pin->lefiPin::hasAntennaSize()) {
     for (i = 0; i < pin->lefiPin::numAntennaSize(); i++) {
        fprintf(fout, "    ANTENNASIZE %g ", pin->lefiPin::antennaSize(i));
        if (pin->lefiPin::antennaSizeLayer(i))
           fprintf(fout, "LAYER %s ", pin->lefiPin::antennaSizeLayer(i));
        fprintf(fout, ";\n");
     }
  }
  if (pin->lefiPin::hasAntennaMetalArea()) {
     for (i = 0; i < pin->lefiPin::numAntennaMetalArea(); i++) {
        fprintf(fout, "    ANTENNAMETALAREA %g ",
           pin->lefiPin::antennaMetalArea(i));
        if (pin->lefiPin::antennaMetalAreaLayer(i))
           fprintf(fout, "LAYER %s ", pin->lefiPin::antennaMetalAreaLayer(i));
        fprintf(fout, ";\n");
     }
  }
  if (pin->lefiPin::hasAntennaMetalLength()) {
     for (i = 0; i < pin->lefiPin::numAntennaMetalLength(); i++) {
        fprintf(fout, "    ANTENNAMETALLENGTH %g ",
           pin->lefiPin::antennaMetalLength(i));
        if (pin->lefiPin::antennaMetalLengthLayer(i))
           fprintf(fout, "LAYER %s ", pin->lefiPin::antennaMetalLengthLayer(i));
        fprintf(fout, ";\n");
     }
  }

  if (pin->lefiPin::numProperties() > 0) {
     fprintf(fout, "    PROPERTY ");
     for (i = 0; i < pin->lefiPin::numProperties(); i++) {
        // value can either be a string or number
        if (pin->lefiPin::propValue(i)) {
           fprintf(fout, "%s %s ", pin->lefiPin::propName(i),
                   pin->lefiPin::propValue(i));
        }
        else
           fprintf(fout, "%s %g ", pin->lefiPin::propName(i),
                   pin->lefiPin::propNum(i));
     }
     fprintf(fout, ";\n");
  }
  
  numPorts = pin->lefiPin::numPorts();
  for (i = 0; i < numPorts; i++) {
     fprintf(fout,"    PORT\n");
     geometry = pin->lefiPin::port(i);
     prtGeometry(geometry);
     fprintf(fout, "    END\n");
  }
  fprintf(fout, "  END %s\n", pin->lefiPin::name());
  return 0;  
}

int siteCB(lefrCallbackType_e c, lefiSite* site, lefiUserData ud) {
  int hasPrtSym = 0;

  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "SITE %s\n", site->lefiSite::name());
  if (site->lefiSite::hasClass())
     fprintf(fout, "  CLASS %s ;\n", site->lefiSite::siteClass());
  if (site->lefiSite::hasXSymmetry()) {
     fprintf(fout, "  SYMMETRY X ");
     hasPrtSym = 1;
  }
  if (site->lefiSite::hasYSymmetry()) {
     if (hasPrtSym)
        fprintf(fout, "Y ");
     else {
        fprintf(fout, "  SYMMETRY Y ");
        hasPrtSym = 1;
     }
  }
  if (site->lefiSite::has90Symmetry()) {
     if (hasPrtSym)
        fprintf(fout, "R90 ");
     else {
        fprintf(fout, "  SYMMETRY R90 ");
        hasPrtSym = 1;
     }
  }
  if (hasPrtSym)
     fprintf(fout, ";\n");
  if (site->lefiSite::hasSize())
     fprintf(fout, "  SIZE %g BY %g ;\n", site->lefiSite::sizeX(),
             site->lefiSite::sizeY());
  fprintf(fout, "END %s\n", site->lefiSite::name());
  return 0;
}

int spacingBeginCB(lefrCallbackType_e c, void* ptr, lefiUserData ud){
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "SPACING\n");
  return 0;
}

int spacingCB(lefrCallbackType_e c, lefiSpacing* spacing, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  lefSpacing(spacing);
  return 0;
}

int spacingEndCB(lefrCallbackType_e c, void* ptr, lefiUserData ud){
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "END SPACING\n");
  return 0;
}

int timingCB(lefrCallbackType_e c, lefiTiming* timing, lefiUserData ud) {
  int i;
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "TIMING\n");
  for (i = 0; i < timing->numFromPins(); i++)
     fprintf(fout, " FROMPIN %s ;\n", timing->fromPin(i));
  for (i = 0; i < timing->numToPins(); i++)
     fprintf(fout, " TOPIN %s ;\n", timing->toPin(i));
     fprintf(fout, " RISE SLEW1 %g %g %g %g ;\n", timing->riseSlewOne(),
             timing->riseSlewTwo(), timing->riseSlewThree(),
             timing->riseSlewFour());
  if (timing->hasRiseSlew2())
     fprintf(fout, " RISE SLEW2 %g %g %g %g ;\n", timing->riseSlewFive(),
             timing->riseSlewSix(), timing->riseSlewSeven());
  if (timing->hasFallSlew())
     fprintf(fout, " FALL SLEW1 %g %g %g %g ;\n", timing->fallSlewOne(),
             timing->fallSlewTwo(), timing->fallSlewThree(),
             timing->fallSlewFour());
  if (timing->hasFallSlew2())
     fprintf(fout, " FALL SLEW2 %g %g %g %g ;\n", timing->fallSlewFive(),
             timing->fallSlewSix(), timing->riseSlewSeven());
  if (timing->hasRiseIntrinsic()) {
     fprintf(fout, "TIMING RISE INTRINSIC %g %g ;\n",
             timing->riseIntrinsicOne(), timing->riseIntrinsicTwo());
     fprintf(fout, "TIMING RISE VARIABLE %g %g ;\n",
             timing->riseIntrinsicThree(), timing->riseIntrinsicFour());
  }
  if (timing->hasFallIntrinsic()) {
     fprintf(fout, "TIMING FALL INTRINSIC %g %g ;\n",
             timing->fallIntrinsicOne(), timing->fallIntrinsicTwo());
     fprintf(fout, "TIMING RISE VARIABLE %g %g ;\n",
             timing->fallIntrinsicThree(), timing->fallIntrinsicFour());
  }
  if (timing->hasRiseRS())
     fprintf(fout, "TIMING RISERS %g %g ;\n",
             timing->riseRSOne(), timing->riseRSTwo());
     if (timing->hasRiseCS())
     fprintf(fout, "TIMING RISECS %g %g ;\n",
             timing->riseCSOne(), timing->riseCSTwo());
  if (timing->hasFallRS())
     fprintf(fout, "TIMING FALLRS %g %g ;\n",
             timing->fallRSOne(), timing->fallRSTwo());
  if (timing->hasFallCS())
     fprintf(fout, "TIMING FALLCS %g %g ;\n",
             timing->fallCSOne(), timing->fallCSTwo());
  if (timing->hasUnateness())
     fprintf(fout, "TIMING UNATENESS %s ;\n", timing->unateness());
  if (timing->hasRiseAtt1())
     fprintf(fout, "TIMING RISESATT1 %g %g ;\n", timing->riseAtt1One(),
             timing->riseAtt1Two());
  if (timing->hasFallAtt1())
     fprintf(fout, "TIMING FALLSATT1 %g %g ;\n", timing->fallAtt1One(),
             timing->fallAtt1Two());
  if (timing->hasRiseTo())
     fprintf(fout, "TIMING RISET0 %g %g ;\n", timing->riseToOne(),
             timing->riseToTwo());
  if (timing->hasFallTo())
     fprintf(fout, "TIMING FALLT0 %g %g ;\n", timing->fallToOne(),
             timing->fallToTwo());
  if (timing->hasSDFonePinTrigger())
     fprintf(fout, " %s TABLEDIMENSION %g %g %g ;\n",
             timing->SDFonePinTriggerType(), timing->SDFtriggerOne(),
             timing->SDFtriggerTwo(), timing->SDFtriggerThree());
  if (timing->hasSDFtwoPinTrigger())
     fprintf(fout, " %s %s %s TABLEDIMENSION %g %g %g ;\n",
             timing->SDFtwoPinTriggerType(), timing->SDFfromTrigger(),
             timing->SDFtoTrigger(), timing->SDFtriggerOne(),
             timing->SDFtriggerTwo(), timing->SDFtriggerThree());
  fprintf(fout, "END TIMING\n");
  return 0;
}

int unitsCB(lefrCallbackType_e c, lefiUnits* unit, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "UNITS\n");
  if (unit->lefiUnits::hasDatabase())
     fprintf(fout, "  DATABASE %s %g ;\n", unit->lefiUnits::databaseName(),
             unit->lefiUnits::databaseNumber());
  if (unit->lefiUnits::hasCapacitance())
     fprintf(fout, "  CAPACITANCE PICOFARADS %g ;\n",
             unit->lefiUnits::capacitance());
  if (unit->lefiUnits::hasResistance())
     fprintf(fout, "  RESISTANCE OHMS %g ;\n", unit->lefiUnits::resistance());
  if (unit->lefiUnits::hasPower())
     fprintf(fout, "  POWER MILLIWATTS %g ;\n", unit->lefiUnits::power());
  if (unit->lefiUnits::hasCurrent())
     fprintf(fout, "  CURRENT MILLIAMPS %g ;\n", unit->lefiUnits::current());
  if (unit->lefiUnits::hasVoltage())
     fprintf(fout, "  VOLTAGE VOLTS %g ;\n", unit->lefiUnits::voltage());
  if (unit->lefiUnits::hasFrequency())
     fprintf(fout, "  FREQUENCY MEGAHERTZ %g ;\n",
             unit->lefiUnits::frequency());
  fprintf(fout, "END UNITS\n");
  return 0;
}

int versionCB(lefrCallbackType_e c, double num, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "VERSION %g ;\n", num);
  return 0;
}

int viaCB(lefrCallbackType_e c, lefiVia* via, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  lefVia(via);
  return 0;
}

int viaRuleCB(lefrCallbackType_e c, lefiViaRule* viaRule, lefiUserData ud) {
  int               numLayers, numVias, i;
  lefiViaRuleLayer* vLayer;

  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "VIARULE %s", viaRule->lefiViaRule::name());
  if (viaRule->lefiViaRule::hasGenerate())
     fprintf(fout, " GENERATE\n");
  else
     fprintf(fout, "\n");

  numLayers = viaRule->lefiViaRule::numLayers();
  // if numLayers == 2, it is VIARULE without GENERATE and has via name
  // if numLayers == 3, it is VIARULE with GENERATE, and the 3rd layer is cut
  for (i = 0; i < numLayers; i++) {
     vLayer = viaRule->lefiViaRule::layer(i); 
     lefViaRuleLayer(vLayer);
  }

  if (numLayers == 2) {    // should have vianames
     numVias = viaRule->lefiViaRule::numVias();
     if (numVias == 0)
        fprintf(fout, "Should have via names in VIARULE.\n");
     else {
        for (i = 0; i < numVias; i++)
           fprintf(fout, "  VIA %s ;\n", viaRule->lefiViaRule::viaName(i));
     }
  }
  fprintf(fout, "END %s\n", viaRule->lefiViaRule::name());
  return 0;
}

int doneCB(lefrCallbackType_e c, void* ptr, lefiUserData ud) {
  checkType(c);
  if ((long)ud != userData) dataError();
  fprintf(fout, "END LIBRARY\n");
  return 0;
}
 
int
main(int argc, char** argv) {
  char* inFile;
  char* outFile;
  FILE* f;
  int res;
  int noCalls = 0;
  long start_mem;
  int num;
  int status;

  start_mem = (long)sbrk(0);

  strcpy(defaultName,"lef.in");
  strcpy(defaultOut,"list");
  inFile = defaultName;
  outFile = defaultOut;
  fout = stdout;
  userData = 0x01020304;

  argc--;
  argv++;
  while (argc--) {

    if (strcmp(*argv, "-d") == 0) {
      argv++;
      argc--;
      sscanf(*argv, "%d", &num);
      lefiSetDebug(num, 1);

    } else if (strcmp(*argv, "-nc") == 0) {
      noCalls = 1;

    } else if (strcmp(*argv, "-p") == 0) {
      printing = 1;

    } else if (strcmp(*argv, "-o") == 0) {
      argv++;
      argc--;
      outFile = *argv;
      if ((fout = fopen(outFile, "w")) == 0) {
	fprintf(stderr, "ERROR: could not open output file\n");
	return 2;
      }

    } else if (argv[0][0] != '-') {
      inFile = *argv;

    } else {
      fprintf(stderr, "ERROR: Illegal command line option: '%s'\n", *argv);
      return 2;
    }

    argv++;
  }

  lefrInit();

  if (noCalls == 0) {
     lefrSetArrayBeginCbk(arrayBeginCB);
     lefrSetArrayCbk(arrayCB);
     lefrSetArrayEndCbk(arrayEndCB);
     lefrSetBusBitCharsCbk(busBitCharsCB);
     lefrSetCaseSensitiveCbk(caseSensCB);
     lefrSetDielectricCbk(dielectricCB);
     lefrSetIRDropBeginCbk(irdropBeginCB);
     lefrSetIRDropCbk(irdropCB);
     lefrSetIRDropEndCbk(irdropEndCB);
     lefrSetLayerCbk(layerCB);
     lefrSetLibraryEndCbk(doneCB); 
     lefrSetMacroBeginCbk(macroBeginCB);
     lefrSetMacroCbk(macroCB);
     lefrSetMinFeatureCbk(minFeatureCB);
     lefrSetNonDefaultCbk(nonDefaultCB);
     lefrSetObstructionCbk(obstructionCB);
     lefrSetPinCbk(pinCB);
     lefrSetSiteCbk(siteCB);
     lefrSetSpacingBeginCbk(spacingBeginCB);
     lefrSetSpacingCbk(spacingCB);
     lefrSetSpacingEndCbk(spacingEndCB);
     lefrSetTimingCbk(timingCB);
     lefrSetUnitsCbk(unitsCB);
     lefrSetUserData((void*)3);
     lefrSetVersionCbk(versionCB);
     lefrSetViaCbk(viaCB);
     lefrSetViaRuleCbk(viaRuleCB);
  }

  if ((f = fopen(inFile,"r")) == 0) {
    fprintf(stderr,"Couldn't open input file '%s'\n", inFile);
    return(2);
  }

  status = lefwInit(fout);  // initialize the lef writer, need to be called 1st
  if (status != LEFW_OK)
     return 1;

  res = lefrRead(f, inFile, (void*)userData);

  fclose(fout);

  return 0;
}
