/**************************************************************************
***    
*** Copyright (c) 1995-2000 Regents of the University of California,
***               Andrew E. Caldwell, Andrew B. Kahng and Igor L. Markov
*** Copyright (c) 2000-2002 Regents of the University of Michigan,
***               Saurabh N. Adya and Igor L. Markov
***
***  Contact author(s): abk@cs.ucsd.edu, imarkov@umich.edu
***  Original Affiliation:   UCLA, Computer Science Department,
***                          Los Angeles, CA 90095-1596 USA
***
***  Permission is hereby granted, free of charge, to any person obtaining 
***  a copy of this software and associated documentation files (the
***  "Software"), to deal in the Software without restriction, including
***  without limitation 
***  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
***  and/or sell copies of the Software, and to permit persons to whom the 
***  Software is furnished to do so, subject to the following conditions:
***
***  The above copyright notice and this permission notice shall be included
***  in all copies or substantial portions of the Software.
***
*** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
*** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
*** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
*** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
*** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
*** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***
***
***************************************************************************/










#include "ParserLEFDEFq.h"
#include "Geoms/plGeom.h"
#include "Constraints/regionConstr.h"

#include <vector>
using std::vector;

#define PRINTBOOL(x) if(x) cout << "true" ; else cout << "false"; 
#define PRINTTYPE(x) switch(x) { case 0: cout << "MASTERSLICE"; break; \
                                 case 1: cout << "CUT"; break; \
                                 case 2: cout << "OVERLAP"; break; \
                                 case 3: cout << "ROUTING"; break; }
#define PRINTDIR(x) switch(x) { case 0: cout << "HORIZONTAL"; break; \
                                case 1: cout << "VERTICAL"; break; \
                                case 2: cout << "NOPREFERRED"; break; }

void ParserLEFDEFq::processRegions(ParserLEFDEFq &p)
{
  CompRegionListType::iterator left,right,current; 
  //left is the smallest index, and right one past the
  //greatest index, of a block of compRegionPair's
  //with the same region.

  CompPairsByRegion c;
  left=right=p.compRegionList.begin();

  while (right!=p.compRegionList.end())
  {
    //find how far we can go in the same region
    while(true)
    {
      if (!(c(*left,*right) || c(*right,*left))) //i.e. if regions are
	// "equal"
	right++;
      else
	break;
      
      if (right==p.compRegionList.end()) break;
    }
    
    //now we have to find the indices of the cells in that region
    //so we can construct a SubPlacement object.
    vector<unsigned> cellindices;
    for (current=left;current!=right;current++)
    {
      cellindices.push_back(((*current).first)->getIndex());
    }
    
    std::sort(cellindices.begin(),cellindices.end());
    
    Mapping m(cellindices.size(),p.uclaDB.spatial.locations.getSize());

    unsigned i;
    for (i=0;i<cellindices.size();i++)
    {
      m[i] = cellindices[i];
    }
    
    SubPlacement sp(p.uclaDB.spatial.locations,m);
    
    typedef parserRegion &rParserRegion;
    rParserRegion reg(*((*left).second)); //all regions from left
    //to right-1 must have the
    //same properties wrt the
    //following code.
    if (reg.bSoft)
    {
      SoftGroupConstraint sgc(sp,reg.maxX,reg.maxY,reg.maxHalfPerim);
      p.uclaDB.spatial.constraints.add(sgc);
    }

    if (reg.boxes.size()==1)
    {
      RectRegionConstraint rrc(sp,reg.boxes[0]);
      p.uclaDB.spatial.constraints.add(rrc);
    }
    else if (reg.boxes.size()>1)
    {
      MultRectRegionConstraint mrrc(sp,reg.boxes);
      p.uclaDB.spatial.constraints.add(mrrc);
    }
    
    left = right;
  }
}


ParserLEFDEFq::ParserLEFDEFq(DB& db, const char *lname, const char *dname) :
  uclaDB(db),
  caseSensitiveLEF(false),
  caseSensitiveDEF(false),
  layerCount(0),
  netCount(0),
  specialNetCount(0),
  regionCount(0)
{
  uclaDB.context._units = 100;
  lefparse(lname);
  
  //make bogus site for external pins
  externalPinSite = new dbSite("FakedSiteForExtPins",
			       dbSite::PAD,
			       Symmetry(false,false,false),
			       0.0,
			       0.0);
  uclaDB.context._sites.push_back(externalPinSite);

  defparse(dname);

  uclaDB.design._cellLEQs = macroLEQList;
  uclaDB.design._cellEEQs = macroEEQList;
  
  uclaDB.context._rows = rowList;
  uclaDB.context._tracks = trackList;
  uclaDB.design._cells = cellList;
  uclaDB.design._nets  = netList;
  uclaDB.design._sNets = specialNetList;
  uclaDB.design._pins  = pinList;
  uclaDB.spatial.locations = Placement(locationList);
  uclaDB.spatial.locationValid = locationsValidList;
  uclaDB.spatial.orientations = orientList;

  if(fixedCompIndices.size() > 0)
  {
    Mapping fixIndexToGlobalIndex(fixedCompIndices.size(), locationList.size());
    Placement fixedPlacement(fixedCompIndices.size());
    std::vector<unsigned>::iterator iU;
    unsigned i;
    for(iU=fixedCompIndices.begin(),i=0;iU!=fixedCompIndices.end();iU++,i++)
    {
      fixIndexToGlobalIndex[i] = *iU;
      fixedPlacement[i] = uclaDB.spatial.locations[(*iU)];
    }
    SubPlacement sp(uclaDB.spatial.locations, fixIndexToGlobalIndex);
    FixedGroupConstraint fgc(sp,fixedPlacement);
    uclaDB.spatial.constraints.add(fgc);
  }

  std::sort(compRegionList.begin(),compRegionList.end(), CompPairsByRegion());
  processRegions(*this);
}

ParserLEFDEFq::~ParserLEFDEFq()
{
  ViaListType::iterator viaIndex = viaList.begin();
  for(; viaIndex != viaList.end(); viaIndex++)
  {
     delete (*viaIndex);
     (*viaIndex) = NULL;
  }
}

int ParserLEFDEFq::HandleLEFVersion(lefrCallbackType_e typ, double variable, lefiUserData data)
{
  // Do nothing
  return 0;
}

int ParserLEFDEFq::HandleLEFCaseSensitive(lefrCallbackType_e typ, int variable, lefiUserData data)
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  if(variable == 1)
     parserPtr->caseSensitiveLEF = true;
  else
     parserPtr->caseSensitiveLEF = false;

  return 0;
}

int ParserLEFDEFq::HandleLEFNoWireExtension(lefrCallbackType_e typ, const char* variable, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFBusBitChars(lefrCallbackType_e typ, const char* variable, lefiUserData data)
{
  // Do nothing
  return 0;
}

int ParserLEFDEFq::HandleLEFDividerChar(lefrCallbackType_e typ, const char* variable, lefiUserData data)
{
  // Do nothing
  return 0;
}

int ParserLEFDEFq::HandleLEFUnits(lefrCallbackType_e typ, lefiUnits* unit, lefiUserData data)
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;
  double value;

  // Could lose data converting from double to int
  if(unit->hasDatabase())
    value = unit->databaseNumber();

  // Don't know why 1e-5 is there, but it is in ParserLEFDEF so we'll keep it around
  parserPtr->uclaDB.context._units = (unsigned int)(value + 1e-5);

  return 0;
}

int ParserLEFDEFq::HandleLEFPropBegin(lefrCallbackType_e typ, void* ptr, lefiUserData data)
{
  // Do nothing
  return 0;
}

int ParserLEFDEFq::HandleLEFProp(lefrCallbackType_e typ, lefiProp* p, lefiUserData data)
{
  // Do nothing
  return 0;
}

int ParserLEFDEFq::HandleLEFPropEnd(lefrCallbackType_e typ, void* ptr, lefiUserData data)
{
  // Do nothing
  return 0;
}

int ParserLEFDEFq::HandleLEFLayer(lefrCallbackType_e typ, lefiLayer* layer, lefiUserData data)
{        
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;
  char* layerName = NULL;
  dbLayer::Type type = dbLayer::Type(-1);
  dbLayer::Dir direction = dbLayer::NOPREFERRED;
  unsigned int pitch = unsigned(-1);
  unsigned int height = unsigned(-1);
  unsigned int width = unsigned(-1);
  unsigned int thickness = unsigned(-1);
  unsigned int shrinkage = unsigned(-1);
  unsigned int lastSpacing = unsigned(-1);
  double capMult = DBL_MAX;
  double edgeCap = DBL_MAX;
  double resistance = DBL_MAX;
  double capacitance = DBL_MAX;
  dbLayer* newLayer = NULL;

  layerName = new char[strlen(layer->name()) + 1];
  strcpy(layerName, layer->name());

  if(!layer->hasType())
    cerr << "Error: Layer must have a type" << endl;
  else if(strncmp(layer->type(), "MASTERSLICE", strlen("MASTERSLICE")) == 0)
    type = dbLayer::MASTERSLICE;
  else if(strncmp(layer->type(), "CUT", strlen("CUT")) == 0)
    type = dbLayer::CUT;
  else if(strncmp(layer->type(), "OVERLAP", strlen("OVERLAP")) == 0)
    type = dbLayer::OVERLAP;
  else if(strncmp(layer->type(), "ROUTING", strlen("ROUTING")) == 0)
    type = dbLayer::ROUTING;

  if(layer->hasPitch())
    pitch = parserPtr->DBconvertUnit(layer->pitch());
  if(layer->hasHeight())
    height = parserPtr->DBconvertUnit(layer->height());
  if(layer->hasWidth())
    width = parserPtr->DBconvertUnit(layer->width());
  if(layer->hasThickness())
    thickness = parserPtr->DBconvertUnit(layer->thickness());
  if(layer->hasShrinkage())
    shrinkage = parserPtr->DBconvertUnit(layer->shrinkage());
  if(layer->hasCapMultiplier())
    capMult = layer->capMultiplier();
  if(layer->hasEdgeCap())
    edgeCap = layer->edgeCap();
  if(layer->hasResistance())
    resistance = layer->resistance();
  if(layer->hasCapacitance())
    capacitance = layer->capacitance();
  // Does not support multiple space declarations. Only uses the last one.
  if(layer->hasSpacingNumber())
    lastSpacing = parserPtr->DBconvertUnit(layer->spacing(layer->numSpacing() - 1));
  if(layer->numSpacing() > 1)
    cerr << "This version of the parser only supports one spacing declaration in "
	 << "a LAYER section. All but the last one will be ignored. " << endl;
  if(layer->hasDirection())
  {
    if(strncmp(layer->direction(), "HORIZONTAL", strlen("HORIZONTAL")) == 0)
      direction = dbLayer::HORIZ;
    else if(strncmp(layer->direction(), "VERTICAL", strlen("VERTICAL")) == 0)
      direction = dbLayer::VERT;
  }

  newLayer = new dbLayer(layerName, parserPtr->layerCount, type, direction,
			 pitch, height, width, thickness, lastSpacing, shrinkage,
			 capMult, edgeCap, resistance, capacitance);

  LayerMapEntry newEntry(strdup(layerName), newLayer);
  pair<LayerMapType::iterator, bool> result;
  result = parserPtr->layerDictionary.insert(newEntry); // a 64-bit porting issue?

  if(!result.second)
  {
    cerr << "Duplicate declaration of layer " << newEntry.first << " in LEF, tossing this one" << endl;
    delete newLayer;
    delete layerName;
    return 0;
  }
  else
    parserPtr->uclaDB.context._layers.push_back(newLayer);

  parserPtr->layerCount++;

  return 0;
}

int ParserLEFDEFq::HandleLEFVia(lefrCallbackType_e typ, lefiVia* via, lefiUserData data)
{
   ParserLEFDEFq*    parserPtr = (ParserLEFDEFq*)data;
   char*             viaName = NULL;
   double            viaResistance = DBL_MAX;
   dbVia*            newVia = NULL;
   ctainerLayerGeoms viaLG;

   // Probably won't contain data but let's clear it anyways
   viaLG.clear();

   viaName = new char[strlen(via->name()) + 1];
   strcpy(viaName, via->name());

   if(via->hasResistance())
     viaResistance = via->resistance();

   // For all the layers specified in this via
   for(int index = 0; index < via->numLayers(); index++)
   {    
     LayerMapType::iterator result = parserPtr->layerDictionary.find(via->layerName(index));

     if(result == parserPtr->layerDictionary.end())
     {
       cerr << "Layer " << via->layerName(index) << " in via " << viaName
	    << " was not found" << endl;
       return 1;
     }
     else
     {
       // For all the rectangles for this layer
       for(int rectIndex = 0; rectIndex < via->numRects(index); rectIndex++)
       {	
 	Rectangle rect(parserPtr->DBconvertUnit(via->xl(index, rectIndex)), 
 		       parserPtr->DBconvertUnit(via->yl(index, rectIndex)), 
 		       parserPtr->DBconvertUnit(via->xh(index, rectIndex)), 
 		       parserPtr->DBconvertUnit(via->yh(index, rectIndex)));
 	// add a new Layer Geom with the specified rectangular shape
 	viaLG.push_back(new dbLayerGeom(rect, *(*result).second, true));
       }
     }
   }

   std::sort(viaLG.begin(), viaLG.end(), CompareLayerGeoms());
  
   newVia = new dbVia(viaName, viaResistance, viaLG);
   ViaMapEntry newEntry(strdup(viaName), newVia);
   pair<ViaMapType::iterator, bool> result = parserPtr->viaDictionary.insert(newEntry);
        // a 64-bit porting issue?

   if(!result.second)
   {
     cerr << "Duplicate declaration of layer " << viaName << " in LEF, tossing this one" << endl;
     delete newVia;
     delete viaName;
     return 0;
   }
     
  return 0;
}

int ParserLEFDEFq::HandleLEFViaRule(lefrCallbackType_e typ, lefiViaRule* viaRule, lefiUserData data)
{
  // Do nothing
  return 0;
}

int ParserLEFDEFq::HandleLEFNonDefault(lefrCallbackType_e typ, lefiNonDefault* def, lefiUserData data)
{
  // Do nothing
  return 0;
}

int ParserLEFDEFq::HandleLEFNoiseMargin(lefrCallbackType_e typ, struct lefiNoiseMargin* p, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFEdgeRateThreshold1(lefrCallbackType_e typ, double variable, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFEdgeRateThreshold2(lefrCallbackType_e typ, double variable, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFEdgeRateScaleFactor(lefrCallbackType_e typ, double variable, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFNoiseTable(lefrCallbackType_e typ, lefiNoiseTable* p, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFCorrectionTable(lefrCallbackType_e typ, lefiCorrectionTable* p, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFSpacingBegin(lefrCallbackType_e typ, void* ptr, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFSpacing(lefrCallbackType_e typ, lefiSpacing* spacing, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFSpacingEnd(lefrCallbackType_e typ, void* ptr, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFMinFeature(lefrCallbackType_e typ, lefiMinFeature* min, lefiUserData data)
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  parserPtr->uclaDB.context._minFeatureX = parserPtr->DBconvertUnit(min->one());
  parserPtr->uclaDB.context._minFeatureY = parserPtr->DBconvertUnit(min->two());

  return 0;
}

int ParserLEFDEFq::HandleLEFDielectric(lefrCallbackType_e typ, double variable, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFIRDropBegin(lefrCallbackType_e typ, void* ptr, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFIRDrop(lefrCallbackType_e typ, lefiIRDrop* irdrop, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFIRDropEnd(lefrCallbackType_e typ, void* ptr, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFSite(lefrCallbackType_e typ, lefiSite* site, lefiUserData data)
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;
  char* siteName = NULL;
  dbSite::Type siteClass = dbSite::Type(-1);
  Symmetry siteSymmetry(false, false, false);
  unsigned int siteHeight = unsigned(-1);
  unsigned int siteWidth = unsigned(-1);

  siteName = new char[strlen(site->name()) + 1];
  strcpy(siteName, site->name());
  
  if(site->hasClass())
  {
    if(strcmp(site->siteClass(), "PAD") == 0)
      siteClass = dbSite::PAD;
    else if(strcmp(site->siteClass(), "CORE") == 0)
      siteClass = dbSite::CORE;
    else
      cerr << "Unknown site class for SITE " << site->name() << endl;
  }

  if(site->hasXSymmetry())
    siteSymmetry.x = true;
  if(site->hasYSymmetry())
    siteSymmetry.y = true;
  if(site->has90Symmetry())
    siteSymmetry.rot90 = true;
  
  // Since DB stores it as unsigned ints, let's round it
  siteWidth = parserPtr->DBconvertUnit(site->sizeX());
  siteHeight = parserPtr->DBconvertUnit(site->sizeY());

  dbSite* newSite = new dbSite(siteName, siteClass, siteSymmetry, siteHeight, siteWidth);
  
  pair<SiteMapType::iterator, bool> result;
  result = parserPtr->siteDictionary.insert(SiteMapEntry(strdup(siteName), newSite));
             // a 64-bit porting issue?
  
  if(!result.second)
  {
    cerr << "Duplicate declaration of site " << siteName << " in LEF file, tossing this one" << endl;
    delete newSite;
    delete siteName;
    return 0;
  }
  else
    parserPtr->uclaDB.context._sites.push_back(newSite);

  return 0;
}

int ParserLEFDEFq::HandleLEFArrayBegin(lefrCallbackType_e typ, const char* variable, lefiUserData data)
{
  // Do nothing
  return 0;
}

int ParserLEFDEFq::HandleLEFArray(lefrCallbackType_e typ, lefiArray* array_data, lefiUserData data)
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  // Doesn't store any data yet, just checks to make sure site names
  // match up
  for(int siteNum = 0; siteNum < array_data->numSitePattern(); siteNum++)
  {
    lefiSitePattern* currentSite = array_data->sitePattern(siteNum);
    SiteMapType::iterator result = parserPtr->siteDictionary.find(currentSite->name());
    if(result == parserPtr->siteDictionary.end())
    {
      cerr << "SITE name " << currentSite->name() << " unrecognized in ARRAY " 
	   << array_data->name() << endl;
      return 1;
    }
  }

  return 0;
}

int ParserLEFDEFq::HandleLEFArrayEnd(lefrCallbackType_e typ, const char* variable, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFMacroBegin(lefrCallbackType_e typ, const char* variable, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFMacro(lefrCallbackType_e typ, lefiMacro* macro, lefiUserData data)
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  char*                         macroName = NULL;
  dbMasterCellType::MClass      macroType = dbMasterCellType::CORE;  
  dbMasterCellType::PadSubClass padClass = dbMasterCellType::PadUNKNOWN;
  double                        macroWidth = -1;
  double                        macroHeight = -1;
  Point                         macroOrigin(0, 0);
  Symmetry                      macroSymmetry(false, false, false);
  dbSite*                       macroSite = NULL;


  macroName = new char[strlen(macro->name()) + 1];
  strcpy(macroName, macro->name());
  
  // Class block
  if(macro->hasClass())
  {
    const char* stringIndex;
    stringIndex = macro->macroClass();

    // Current only acquires the pad's subclass, others are ignored
    if(strcmp(stringIndex, "COVER") == 0)
      macroType = dbMasterCellType::COVER;
    else if(strcmp(stringIndex, "RING") == 0)
      macroType = dbMasterCellType::RING;
    else if(strcmp(stringIndex, "BLOCK") == 0)
      macroType = dbMasterCellType::BLOCK;
    else if(strncmp(stringIndex, "PAD", strlen("PAD")) == 0)
    {
      macroType = dbMasterCellType::PAD;
      stringIndex += (strlen("PAD") + 1); // one more for the space
      // Assuming there are no trailing characters
      if(strcmp(stringIndex, "INPUT") == 0)
	padClass = dbMasterCellType::INPUT;
      else if(strcmp(stringIndex, "OUTPUT") == 0)
	padClass = dbMasterCellType::OUTPUT;
      else if(strcmp(stringIndex, "INOUT") == 0)
	padClass = dbMasterCellType::INOUT;
      else if(strcmp(stringIndex, "POWER") == 0)
	padClass = dbMasterCellType::POWER;
      else if(strcmp(stringIndex, "SPACER") == 0)
	padClass = dbMasterCellType::SPACER;
    }
    else if(strncmp(macro->macroClass(), "CORE", strlen("CORE")) == 0)
      macroType = dbMasterCellType::CORE;      
    else if(strncmp(macro->macroClass(), "ENDCAP", strlen("ENDCAP")) == 0)
      macroType = dbMasterCellType::ENDCAP;
  }
  
  if(macro->hasOrigin())
  {
    macroOrigin.x = parserPtr->DBconvertUnit(macro->originX());
    macroOrigin.y = parserPtr->DBconvertUnit(macro->originY());
  }

  if(!macro->hasSize())
  {
    cerr << "Error: Missing a required field, MACRO SIZE" << endl
	 << "\tAttempting to continue" << endl;
  }
  else
  {
    macroWidth = parserPtr->DBconvertUnit(macro->sizeX());
    macroHeight = parserPtr->DBconvertUnit(macro->sizeY());
  }
  
  if(macro->hasXSymmetry())
    macroSymmetry.x = true;
  if(macro->hasYSymmetry())
    macroSymmetry.y = true;
  if(macro->has90Symmetry())
    macroSymmetry.rot90 = true;

  if(macro->hasSiteName())
  {
    SiteMapType::iterator index = parserPtr->siteDictionary.find(macro->siteName());
    if(index == parserPtr->siteDictionary.end())
    {
      cerr << "SITE " << macro->siteName() << " unrecognized in MACRO " 
	   << macroName << endl;
      return 1;
    }
    else
      macroSite = (*index).second;
  }
  else
  {
     for(int siteNum = 0; siteNum < macro->numSitePattern(); siteNum++)
     {
       lefiSitePattern* currentSite = macro->sitePattern(siteNum);
       SiteMapType::iterator result = parserPtr->siteDictionary.find(currentSite->name());
       if(result == parserPtr->siteDictionary.end())
       {
         cerr << "SITE name " << currentSite->name() << " unrecognized in MACRO " 
	      << macroName << endl;
         return 1;
       }
       else
          macroSite = result->second;
     }
  }
 
  dbMasterCell* newMacro = new dbMasterCell(macroName, macroType, padClass, macroWidth,
					    macroHeight, macroOrigin, parserPtr->macroPinList, 
					    parserPtr->macroObs, macroSymmetry, *macroSite);
					    
  pair<MasterCellMapType::iterator, bool> result;
  result = parserPtr->macroDictionary.insert(MasterCellMapEntry(strdup(macroName), newMacro)); // a 64-bit porting issue?
  
  if(!result.second)
  {
    cerr << "Duplicate declaration of MACRO " << macroName << " in LEF file, ignoring this one" << endl;
    MacroPinListType::iterator pinIndex = parserPtr->macroPinList.begin();
    MacroObsType::iterator geomIndex = parserPtr->macroObs.begin();
    for(; pinIndex != parserPtr->macroPinList.end(); pinIndex++)
      delete (*pinIndex);
    for(; geomIndex != parserPtr->macroObs.end(); geomIndex++)
      delete (*geomIndex);
    delete newMacro;
    delete macroName;
    parserPtr->macroPinList.clear();
    parserPtr->macroObs.clear();
    return 0;
  }
  else
    parserPtr->uclaDB.design._cellLib.push_back(newMacro);

  eqClassMasterCells*           LEQclass = NULL;
  eqClassMasterCells*           EEQclass = NULL;
  if(macro->hasLEQ())
  {
    // Look for the equivalent macro
    MasterCellMapType::iterator macroLEQIndex = parserPtr->macroDictionary.begin();
    
    macroLEQIndex = parserPtr->macroDictionary.find(macro->LEQ());
    
    if(macroLEQIndex == parserPtr->macroDictionary.end())
    {
      cerr << "Marco " << macroName << " allegedly equivalent to unknown pin " << macro->LEQ() << endl;
      return 1;
    }
    else
    {
      equivClassesMasterCells::iterator classIndex;
      eqClassMasterCells::const_iterator macroIndex;
    
      bool classFound = false;
      for(classIndex = parserPtr->macroLEQList.begin(); classIndex != parserPtr->macroLEQList.end(); classIndex++)
      {
	for(macroIndex = (*classIndex).begin(); macroIndex != (*classIndex).end(); macroIndex++)
	{
	  if(*macroIndex == (*macroLEQIndex).second)
	  {
	    classFound = true;	    
	    LEQclass = &(*classIndex);
	    break;
	  }
	}
	if(classFound)
	{
	  LEQclass->push_back(newMacro);
	  break;
	}
      }
      if(!classFound)
      {
	eqClassMasterCells newClass;
	newClass.push_back((*macroLEQIndex).second);
	newClass.push_back(newMacro);
	parserPtr->macroLEQList.push_back(newClass);
      }
    }
  }
  if(macro->hasEEQ())
  {
    // Look for the equivalent macro
    MasterCellMapType::iterator macroEEQIndex = parserPtr->macroDictionary.begin();
    
    macroEEQIndex = parserPtr->macroDictionary.find(macro->EEQ());
    
    if(macroEEQIndex == parserPtr->macroDictionary.end())
    {
      cerr << "Marco " << macroName << " allegedly equivalent to unknown pin " << macro->EEQ() << endl;
      return 1;
    }
    else
    {
      equivClassesMasterCells::iterator classIndex;
      eqClassMasterCells::const_iterator macroIndex;
    
      bool classFound = false;
      for(classIndex = parserPtr->macroEEQList.begin(); classIndex != parserPtr->macroEEQList.end(); classIndex++)
      {
	for(macroIndex = (*classIndex).begin(); macroIndex != (*classIndex).end(); macroIndex++)
	{
	  if(*macroIndex == (*macroEEQIndex).second)
	  {
	    classFound = true;	    
	    EEQclass = &(*classIndex);
	    break;
	  }
	}
	if(classFound)
	{
	  EEQclass->push_back(newMacro);
	  break;
	}
      }
      if(!classFound)
      {
	eqClassMasterCells newClass;
	newClass.push_back((*macroEEQIndex).second);
	newClass.push_back(newMacro);
	parserPtr->macroEEQList.push_back(newClass);
      }
    }
  }

  // Translate geometries and add to pin library

  MacroPinListType::iterator pinIndex = parserPtr->macroPinList.begin();
  itLayerGeom geomIndex;

  // Set the pins to be relative to the macro
  for(; pinIndex != parserPtr->macroPinList.end(); pinIndex++)
  {
    (*pinIndex)->_cell = newMacro;
    
    ctainerLayerGeoms &geoms = (*pinIndex)->_ports; 
    // shift all port locations by the origin
    for(geomIndex = geoms.begin(); geomIndex != geoms.end(); geomIndex++)
    {
      (*geomIndex)->_rect.TranslateBy(macroOrigin);
    }
  }

  // Set the obstacles to be relative to the macro
  for(geomIndex = parserPtr->macroObs.begin(); geomIndex != parserPtr->macroObs.end(); geomIndex++)
  {
    (*geomIndex)->_rect.TranslateBy(macroOrigin);
  }

  for(pinIndex = parserPtr->macroPinList.begin(); pinIndex != parserPtr->macroPinList.end(); pinIndex++)
  {
    parserPtr->uclaDB.design._pinLib.push_back(*pinIndex);
  }

  parserPtr->macroPinList.clear();
  parserPtr->macroObs.clear();
  
  return 0;
}

int ParserLEFDEFq::HandleLEFPin(lefrCallbackType_e typ, lefiPin* pin, lefiUserData data)
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;
  char* pinName = NULL;
  char* pinShape = "\0";
  char* pinUse = "\0";
  char* pinDirection = "\0";
  BBox geomBoundingBox;
  ctainerLayerGeoms portGeom;
  dbMasterPin* mustjoinPin = NULL;
  eqClassMasterPins* equivalentClass = NULL;
  dbMasterPin* equivalentPin = NULL;

  portGeom.clear();
  geomBoundingBox.clear();

  pinName = new char[strlen(pin->name()) + 1];
  strcpy(pinName, pin->name());

  // Handle the MUSTJOIN statement, will join it later, for now, just detect
  if(pin->hasMustjoin())
  {
    MacroPinListType::const_iterator pinIndex = parserPtr->macroPinList.begin();

    for(; pinIndex != parserPtr->macroPinList.end(); pinIndex++)
    {
      if(strcmp((*pinIndex)->getName(), pin->mustjoin()) == 0)
      {
	mustjoinPin = *pinIndex;
	break;
      }
    }
    if(mustjoinPin == NULL)
    {
      cerr << "Pin " << pinName << " allegedly must join pin "  
	   << pin->mustjoin() << " which is not found in the pin list" << endl;
    }
  }

  if(pin->hasShape())
  {
    pinShape = new char[strlen(pin->shape()) + 1];
    strcpy(pinShape, pin->shape());
  }

  if(pin->hasUse())
  {
    pinUse = new char[strlen(pin->use()) + 1];
    strcpy(pinUse, pin->use());
  }

  if(pin->hasDirection())
  {
    pinDirection = new char[strlen(pin->direction()) + 1];
    strcpy(pinDirection, pin->direction());
  }

  // Handle all the PORT statements
  for(int portNum = 0; portNum < pin->numPorts(); portNum++)
  {
    // Iterate through all the characteristics of the geometry of this port
    // and set up the portGeom list
    lefiGeometries* geometries = pin->port(portNum);
    // Assumes that we get it in the proper order of layer, then a set of data, then next layer...
    dbLayer* geomLayer = NULL;
    for(int geoNum = 0; geoNum < geometries->numItems(); geoNum++)
    {
      switch(geometries->itemType(geoNum))
      {
	// Make sure the layer exists and keep track of it
	case lefiGeomLayerE:
	{
	  LayerMapType::iterator result;
	  result = parserPtr->layerDictionary.find(geometries->getLayer(geoNum));
	  if(result == parserPtr->layerDictionary.end())
	  {
	    cerr << "LAYER " << geometries->getLayer(geoNum) << " specified in "
		 << "PIN " << pinName << " could not be found" << endl;
	  }
	  geomLayer = (*result).second;
	  break;
	}

	case lefiGeomWidthE:
	{
	  // Do nothing for now
	  break;
	}
  
	// Add the Path points to the bounding box, for now, that's all it does
	case lefiGeomPathE:
	{
  	  if(geomLayer)
  	  {
  	    for(int pointNum = 0; pointNum < geometries->getPolygon(geoNum)->numPoints; pointNum++)
	    {
  	      geomBoundingBox += Point(parserPtr->DBconvertUnit(geometries->getPath(geoNum)->x[pointNum]), 
				       parserPtr->DBconvertUnit(geometries->getPath(geoNum)->y[pointNum]));
	    }
  	  }
  	  else 
	  {
  	    cerr << "Missing LAYER declaration in PORT for PIN " << pinName << endl;
	    cerr << "Error occurred while acquiring PATH" << endl;
	    return 1;
	  }
	  break;
	}

	// Add a rectangle to the geometry list and add points to Bounding box
        case lefiGeomRectE:
	{
	  if(geomLayer)
	  {
	    Rectangle rect(parserPtr->DBconvertUnit(geometries->getRect(geoNum)->xl), 
			   parserPtr->DBconvertUnit(geometries->getRect(geoNum)->yl), 
			   parserPtr->DBconvertUnit(geometries->getRect(geoNum)->xh), 
			   parserPtr->DBconvertUnit(geometries->getRect(geoNum)->yh));
	    geomBoundingBox += Point(rect.xMin, rect.yMin);	    
	    geomBoundingBox += Point(rect.xMax, rect.yMax);
	    portGeom.push_back(new dbLayerGeom(rect, *geomLayer));
	  }
	  else 
	  {
	    cerr << "Missing LAYER declaration in PORT for PIN " << pinName << endl;
	    cerr << "Error occurred while acquiring RECT" << endl;
	    return 1;
	  }
	  break;
	}

	// Add a polgyon to the geometry list and add poitns to Bounding box
	case lefiGeomPolygonE:
	{
	  if(geomLayer)
	  {
	    vector<Point>     polygon;
	    for(int pointNum = 0; pointNum < geometries->getPolygon(geoNum)->numPoints; pointNum++)
	    {
	      polygon.push_back(Point(parserPtr->DBconvertUnit(geometries->getPolygon(geoNum)->x[pointNum]),
				      parserPtr->DBconvertUnit(geometries->getPolygon(geoNum)->y[pointNum])));
	      geomBoundingBox += Point(parserPtr->DBconvertUnit(geometries->getPolygon(geoNum)->x[pointNum]), 
				       parserPtr->DBconvertUnit(geometries->getPolygon(geoNum)->y[pointNum]));
	    }
	    portGeom.push_back(new dbLayerGeom(polygon, *geomLayer));
	  }
	  else 
	  {
	    cerr << "Missing LAYER declaration in PORT for PIN " << pinName << endl;
	    cerr << "Occurred while acquiring POLYGON" << endl;
	    return 1;
	  }
	  break;
	}
	
	case lefiGeomViaE:
	{
	  // Find the via
	  ViaMapType::iterator result;
	  result = parserPtr->viaDictionary.find(geometries->getVia(geoNum)->name);
	  if(result == parserPtr->viaDictionary.end())
	  {
	    cerr << "VIA " << geometries->getVia(geoNum)->name << " specified in "
		 << "PIN " << pinName << " could not be found" << endl;
	    return 1;
	  }

	  ctainerLayerGeoms const &viaGeoms=(*result).second->getGeometries();
	  ctainerLayerGeoms::const_iterator geomIndex = viaGeoms.begin();

	  for(; geomIndex != viaGeoms.end(); geomIndex++)
	  {
	    BBox box = *(*geomIndex);
	    box.TranslateBy(Point(parserPtr->DBconvertUnit(geometries->getVia(geoNum)->x), 
				  parserPtr->DBconvertUnit(geometries->getVia(geoNum)->y)));
	    portGeom.push_back(new dbLayerGeom(box, (*geomIndex)->getLayer()));
	    geomBoundingBox += Point(box.xMin, box.yMin);    
	    geomBoundingBox += Point(box.xMax, box.yMax);
	  }
	  break;
	}

	case lefiGeomPathIterE:
	{
	  // For each point
  	  for(int pointNum = 0; pointNum < geometries->getPathIter(geoNum)->numPoints; pointNum++)
  	  {
	    // For each x step
	    for(int xindex = 0; xindex < geometries->getPathIter(geoNum)->xStart; xindex++)
	    {
	      // For each y step
	      for(int yindex = 0; yindex < geometries->getPathIter(geoNum)->yStart; yindex++)
	      {
		geomBoundingBox += Point(parserPtr->DBconvertUnit(geometries->getPathIter(geoNum)->x[pointNum] + 
					 xindex * geometries->getPathIter(geoNum)->xStep),
					 parserPtr->DBconvertUnit(geometries->getPathIter(geoNum)->y[pointNum] +
					 yindex * geometries->getPathIter(geoNum)->yStep));
	      }
	    }
	  }
	  cerr << "Warning: Path Iterator declare in LAYER " << geometries->getLayer(geoNum) << " specified in "
		 << "PIN " << pinName << ", unsupported by DB at this time...ignoring" << endl;
	  break;
	}

	case lefiGeomRectIterE:
	{
	  for(int xindex = 0; xindex < geometries->getRectIter(geoNum)->xStart; xindex++)
	  {
	    for(int yindex = 0; yindex < geometries->getRectIter(geoNum)->yStart; yindex++)
	    {
	      Rectangle rect(parserPtr->DBconvertUnit(geometries->getRectIter(geoNum)->xl + 
						      xindex * geometries->getRectIter(geoNum)->xStep), 
			     parserPtr->DBconvertUnit(geometries->getRectIter(geoNum)->yl + 
						      yindex * geometries->getRectIter(geoNum)->yStep), 
			     parserPtr->DBconvertUnit(geometries->getRectIter(geoNum)->xh + 
						      xindex * geometries->getRectIter(geoNum)->xStep),
			     parserPtr->DBconvertUnit(geometries->getRectIter(geoNum)->yh + 
						      yindex * geometries->getRectIter(geoNum)->yStep));
	      geomBoundingBox += Point(rect.xMin, rect.yMin);	    
	      geomBoundingBox += Point(rect.xMax, rect.yMax);
	    }
	  }
	  cerr << "Warning: Rect Iterator declare in LAYER " << geometries->getLayer(geoNum) << " specified in "
		 << "PIN " << pinName << ", unsupported by DB at this time...ignoring" << endl;
	  break;
	}

	case lefiGeomPolygonIterE:
	{
	  for(int pointNum = 0; pointNum < geometries->getPolygonIter(geoNum)->numPoints; pointNum++)
	  {
	    for(int xindex = 0; xindex < geometries->getPolygonIter(geoNum)->xStart; xindex++)
	    {
	      for(int yindex = 0; yindex < geometries->getPolygonIter(geoNum)->yStart; yindex++)
	      {
		geomBoundingBox += Point(parserPtr->DBconvertUnit(geometries->getPolygonIter(geoNum)->x[pointNum] +
					 xindex * geometries->getPolygonIter(geoNum)->xStep), 
					 parserPtr->DBconvertUnit(geometries->getPolygonIter(geoNum)->y[pointNum] +
					 yindex * geometries->getPolygonIter(geoNum)->yStep));		
	      }
	    }
	  }	
	  cerr << "Warning: Polygon Iterator declare in LAYER " << geometries->getLayer(geoNum) << " specified in "
	       << "PIN " << pinName << ", unsupported by DB at this time...ignoring" << endl;	
	  break;
	}

	case lefiGeomViaIterE:
	{
	  // Find the via
	  ViaMapType::iterator result;
	  result = parserPtr->viaDictionary.find(geometries->getViaIter(geoNum)->name);
	  if(result == parserPtr->viaDictionary.end())
	  {
	    cerr << "VIA " << geometries->getViaIter(geoNum)->name << " specified in "
		 << "PIN " << pinName << " could not be found" << endl;
	    return 1;
	  }

	  ctainerLayerGeoms const &viaGeoms=(*result).second->getGeometries();
	  ctainerLayerGeoms::const_iterator geomIndex = viaGeoms.begin();

	  for(; geomIndex != viaGeoms.end(); geomIndex++)
	  {
	    BBox box = *(*geomIndex);
	    for(int xindex = 0; xindex < geometries->getViaIter(geoNum)->xStart; xindex++)
	    {
	      for(int yindex = 0; yindex < geometries->getViaIter(geoNum)->yStart; yindex++)
	      {
		box.TranslateBy(Point(parserPtr->DBconvertUnit(geometries->getViaIter(geoNum)->x +
				      xindex * geometries->getViaIter(geoNum)->xStep), 
				      parserPtr->DBconvertUnit(geometries->getViaIter(geoNum)->y +
				      yindex * geometries->getViaIter(geoNum)->yStep)));
		geomBoundingBox += Point(box.xMin, box.yMin);    
		geomBoundingBox += Point(box.xMax, box.yMax);
	      }
	    }
	  }  
	  cerr << "Warning: Via Iterator declare in LAYER " << geometries->getLayer(geoNum) << " specified in "
	       << "PIN " << pinName << ", unsupported by DB at this time...ignoring" << endl;
	  break;
	}

	case lefiGeomClassE:
	{
	  // Do nothing
	  break;
	}
      
        case lefiGeomEnd:
	{
	  geomLayer = NULL;
	  break;
	}
      
        case lefiGeomUnknown:
	{
	  cerr << "Error: Unknown item in port for LAYER  " << geometries->getLayer(geoNum) << " specified in "
	       << "PIN " << pinName << endl;
	  return 1;
	}
      }
    }

    LayerMapType::iterator result;
    int antennaNum = 0;

    for(antennaNum = 0; antennaNum < pin->numAntennaSize(); antennaNum++)
    {
      result = parserPtr->layerDictionary.find(pin->antennaSizeLayer(antennaNum));
      if(result == parserPtr->layerDictionary.end())
      {
	cerr << "ANTENNA LAYER " << pin->antennaSizeLayer(antennaNum) << " specified in "
	     << "PIN " << pinName << " could not be found" << endl;
      }
    }
    for(antennaNum = 0; antennaNum < pin->numAntennaMetalArea(); antennaNum++)
    {
      result = parserPtr->layerDictionary.find(pin->antennaMetalAreaLayer(antennaNum));
      if(result == parserPtr->layerDictionary.end())
      {
	cerr << "ANTENNA METAL AREA LAYER " << pin->antennaMetalAreaLayer(antennaNum) << " specified in "
	     << "PIN " << pinName << " could not be found" << endl;
      }
    }
    for(antennaNum = 0; antennaNum < pin->numAntennaMetalLength(); antennaNum++)
    {
      result = parserPtr->layerDictionary.find(pin->antennaMetalLengthLayer(antennaNum));
      if(result == parserPtr->layerDictionary.end())
      {
	cerr << "ANTENNA METAL LENGTH LAYER " << pin->antennaMetalLengthLayer(antennaNum) << " specified in "
	     << "PIN " << pinName << " could not be found" << endl;
      }
    }
  }

  if(portGeom.size() == 0)
  {
    geomBoundingBox.add(0,0);
  }
  if(geomBoundingBox.isEmpty())
  {
    cerr << "Pin " << pinName << " has empty bounding box. Adding point (0,0) "
	 << "to prevent crash" << endl;
    geomBoundingBox.add(0,0);
  }

  dbMasterPin* newPin = new dbMasterPin(pinName, pinDirection, pinUse, pinShape, 
					geomBoundingBox, geomBoundingBox.getGeomCenter());
  newPin->_riseDelay = -1.0;
  newPin->_fallDelay = -1.0;
  newPin->_riseResist = -1.0;
  newPin->_fallResist = -1.0;
  newPin->_inputCap = -1.0;

  parserPtr->macroPinList.push_back(newPin);
  
  newPin->_ports = portGeom;
  
  if(mustjoinPin != NULL)
  {
    if(mustjoinPin->_mustJoin)
      mustjoinPin->_mustJoin->push_back(newPin);
    else
    {
      ctainerMasterPinsLocal* mustjoinList = new ctainerMasterPinsLocal;
      mustjoinList->push_back(mustjoinPin);
      mustjoinList->push_back(newPin);
      mustjoinPin->_mustJoin = mustjoinList;
      newPin->_mustJoin = mustjoinList;
    }
  }
  

  if(pin->hasLEQ())
  {
    // Look for the equivalent pin
    MacroPinListType::iterator pinLEQIndex = parserPtr->macroPinList.begin();
    bool pinFound = false;

    for(; pinLEQIndex != parserPtr->macroPinList.end(); pinLEQIndex++)
    {
      if(strcmp((*pinLEQIndex)->getName(), pin->LEQ()) == 0)
      {
	pinFound = true;
	equivalentPin = (*pinLEQIndex);
	break;
      }
    }
    
    if(!pinFound)
    {
      cerr << "Pin " << pinName << " allegedly equivalent to unknown pin " << pin->LEQ() << endl;
      return 1;
    }
    else
    {
      equivClassesMasterPins::iterator classIndex;
      eqClassMasterPins::const_iterator pinIndex;
    
      bool classFound = false;
      for(classIndex = parserPtr->uclaDB.design._pinLEQs.begin(); classIndex != parserPtr->uclaDB.design._pinLEQs.end(); classIndex++)
      {
	for(pinIndex = (*classIndex).begin(); pinIndex != (*classIndex).end(); pinIndex++)
	{
	  if(*pinIndex == *pinLEQIndex)
	  {
	    classFound = true;	    
	    equivalentClass = &(*classIndex);
	    break;
	  }
	}
	if(classFound)
	{
	  equivalentClass->push_back(newPin);
	  break;
	}
      }
      if(!classFound)
      {
	eqClassMasterPins newClass;
	newClass.push_back(equivalentPin);
	newClass.push_back(newPin);
	parserPtr->uclaDB.design._pinLEQs.push_back(newClass);
      }
    }
  }

  return 0;
}

int ParserLEFDEFq::HandleLEFObstruction(lefrCallbackType_e typ, lefiObstruction* obs, lefiUserData data)
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  BBox geomBoundingBox;

  geomBoundingBox.clear();
  parserPtr->macroObs.clear();


  lefiGeometries* geometries = obs->geometries();
  dbLayer* geomLayer = NULL;
  // Assumes that we get it in the proper order of layer, then a set of data, then next layer...
  for(int geoNum = 0; geoNum < geometries->numItems(); geoNum++)
  {  
      switch(geometries->itemType(geoNum))
      {
	// Make sure the layer exists and keep track of it
	case lefiGeomLayerE:
	{
	  LayerMapType::iterator result;
	  result = parserPtr->layerDictionary.find(geometries->getLayer(geoNum));
	  if(result == parserPtr->layerDictionary.end())
	  {
	    cerr << "LAYER " << geometries->getLayer(geoNum) << " specified in "
		 << "OBS could not be found" << endl;
	    return 1;
	  }
	  geomLayer = (*result).second;
	  break;
	}

	case lefiGeomWidthE:
	{
	  // Do nothing for now
	  break;
	}
  
	// Add the Path points to the bounding box, for now, that's all it does
	case lefiGeomPathE:
	{
  	  if(geomLayer)
  	  {
  	    for(int pointNum = 0; pointNum < geometries->getPolygon(geoNum)->numPoints; pointNum++)
	    {
  	      geomBoundingBox += Point(parserPtr->DBconvertUnit(geometries->getPath(geoNum)->x[pointNum]), 
				       parserPtr->DBconvertUnit(geometries->getPath(geoNum)->y[pointNum]));
	    }
  	  }
  	  else 
	  {
  	    cerr << "Missing LAYER declaration in OBS" << endl;
	    cerr << "Error occurred while acquiring PATH" << endl;
	    return 1;
	  }
	  break;
	}

	// Add a rectangle to the geometry list and add points to Bounding box
        case lefiGeomRectE:
	{
	  if(geomLayer)
	  {
	    Rectangle rect(parserPtr->DBconvertUnit(geometries->getRect(geoNum)->xl), 
			   parserPtr->DBconvertUnit(geometries->getRect(geoNum)->yl), 
			   parserPtr->DBconvertUnit(geometries->getRect(geoNum)->xh), 
			   parserPtr->DBconvertUnit(geometries->getRect(geoNum)->yh));
	    geomBoundingBox += Point(rect.xMin, rect.yMin);	    
	    geomBoundingBox += Point(rect.xMax, rect.yMax);
	    parserPtr->macroObs.push_back(new dbLayerGeom(rect, *geomLayer));
	  }
	  else 
	  {
	    cerr << "Missing LAYER declaration in OBS"<< endl;
	    cerr << "Occurred while acquiring RECT" << endl;
	    return 1;
	  }
	  break;
	}

	// Add a polgyon to the geometry list and add poitns to Bounding box
	case lefiGeomPolygonE:
	{
	  if(geomLayer)
	  {
	    vector<Point>     polygon;
	    for(int pointNum = 0; pointNum < geometries->getPolygon(geoNum)->numPoints; pointNum++)
	    {
	      polygon.push_back(Point(parserPtr->DBconvertUnit(geometries->getPolygon(geoNum)->x[pointNum]),
				      parserPtr->DBconvertUnit(geometries->getPolygon(geoNum)->y[pointNum])));
	      geomBoundingBox += Point(parserPtr->DBconvertUnit(geometries->getPolygon(geoNum)->x[pointNum]), 
				       parserPtr->DBconvertUnit(geometries->getPolygon(geoNum)->y[pointNum]));
	    }
	    parserPtr->macroObs.push_back(new dbLayerGeom(polygon, *geomLayer));
	  }
	  else 
	  {
	    cerr << "Missing LAYER declaration in OBS" << endl;
	    cerr << "Error occurred while acquiring POLYGON" << endl;
	    return 1;
	  }
	  break;
	}
	
	case lefiGeomViaE:
	{
	  // Find the via
	  ViaMapType::iterator result;
	  result = parserPtr->viaDictionary.find(geometries->getVia(geoNum)->name);
	  if(result == parserPtr->viaDictionary.end())
	  {
	    cerr << "VIA " << geometries->getVia(geoNum)->name << " specified in "
		 << "OBS could not be found" << endl;
	    return 1;
	  }

	  ctainerLayerGeoms const &viaGeoms=(*result).second->getGeometries();
	  ctainerLayerGeoms::const_iterator geomIndex = viaGeoms.begin();

	  for(; geomIndex != viaGeoms.end(); geomIndex++)
	  {
	    BBox box = *(*geomIndex);
	    box.TranslateBy(Point(parserPtr->DBconvertUnit(geometries->getVia(geoNum)->x), 
				  parserPtr->DBconvertUnit(geometries->getVia(geoNum)->y)));
	    parserPtr->macroObs.push_back(new dbLayerGeom(box, (*geomIndex)->getLayer()));
	    geomBoundingBox += Point(box.xMin, box.yMin);    
	    geomBoundingBox += Point(box.xMax, box.yMax);
	  }
	  break;
	}

	case lefiGeomPathIterE:
	{
	  // For each point
  	  for(int pointNum = 0; pointNum < geometries->getPathIter(geoNum)->numPoints; pointNum++)
  	  {
	    // For each x step
	    for(int xindex = 0; xindex < geometries->getPathIter(geoNum)->xStart; xindex++)
	    {
	      // For each y step
	      for(int yindex = 0; yindex < geometries->getPathIter(geoNum)->yStart; yindex++)
	      {
		geomBoundingBox += Point(parserPtr->DBconvertUnit(geometries->getPathIter(geoNum)->x[pointNum] + 
					 xindex * geometries->getPathIter(geoNum)->xStep),
					 parserPtr->DBconvertUnit(geometries->getPathIter(geoNum)->y[pointNum] +
					 yindex * geometries->getPathIter(geoNum)->yStep));
	      }
	    }
	  }
	  cerr << "Warning: Path Iterator declare in LAYER " << geometries->getLayer(geoNum) << " specified in "
		 << "OBS, unsupported by DB at this time...ignoring" << endl;
	  break;
	}

	case lefiGeomRectIterE:
	{
	  for(int xindex = 0; xindex < geometries->getRectIter(geoNum)->xStart; xindex++)
	  {
	    for(int yindex = 0; yindex < geometries->getRectIter(geoNum)->yStart; yindex++)
	    {
	      Rectangle rect(parserPtr->DBconvertUnit(geometries->getRectIter(geoNum)->xl + 
						      xindex * geometries->getRectIter(geoNum)->xStep), 
			     parserPtr->DBconvertUnit(geometries->getRectIter(geoNum)->yl + 
						      yindex * geometries->getRectIter(geoNum)->yStep), 
			     parserPtr->DBconvertUnit(geometries->getRectIter(geoNum)->xh + 
						      xindex * geometries->getRectIter(geoNum)->xStep),
			     parserPtr->DBconvertUnit(geometries->getRectIter(geoNum)->yh + 
						      yindex * geometries->getRectIter(geoNum)->yStep));
	      geomBoundingBox += Point(rect.xMin, rect.yMin);	    
	      geomBoundingBox += Point(rect.xMax, rect.yMax);
	    }
	  }
	  cerr << "Warning: Rect Iterator declare in LAYER " << geometries->getLayer(geoNum) << " specified in "
		 << "OBS, unsupported by DB at this time...ignoring" << endl;
	  break;
	}

	case lefiGeomPolygonIterE:
	{
	  for(int pointNum = 0; pointNum < geometries->getPolygonIter(geoNum)->numPoints; pointNum++)
	  {
	    for(int xindex = 0; xindex < geometries->getPolygonIter(geoNum)->xStart; xindex++)
	    {
	      for(int yindex = 0; yindex < geometries->getPolygonIter(geoNum)->yStart; yindex++)
	      {
		geomBoundingBox += Point(parserPtr->DBconvertUnit(geometries->getPolygonIter(geoNum)->x[pointNum] +
					 xindex * geometries->getPolygonIter(geoNum)->xStep), 
					 parserPtr->DBconvertUnit(geometries->getPolygonIter(geoNum)->y[pointNum] +
					 yindex * geometries->getPolygonIter(geoNum)->yStep));		
	      }
	    }
	  }	
	  cerr << "Warning: Polygon Iterator declare in LAYER " << geometries->getLayer(geoNum) << " specified in "
	       << "OBS, unsupported by DB at this time...ignoring" << endl;	
	  break;
	}

	case lefiGeomViaIterE:
	{
	  // Find the via
	  ViaMapType::iterator result;
	  result = parserPtr->viaDictionary.find(geometries->getViaIter(geoNum)->name);
	  if(result == parserPtr->viaDictionary.end())
	  {
	    cerr << "VIA " << geometries->getViaIter(geoNum)->name << " specified in "
		 << "OBS could not be found" << endl;
	    return 1;
	  }

	  ctainerLayerGeoms const &viaGeoms=(*result).second->getGeometries();
	  ctainerLayerGeoms::const_iterator geomIndex = viaGeoms.begin();

	  for(; geomIndex != viaGeoms.end(); geomIndex++)
	  {
	    BBox box = *(*geomIndex);
	    for(int xindex = 0; xindex < geometries->getViaIter(geoNum)->xStart; xindex++)
	    {
	      for(int yindex = 0; yindex < geometries->getViaIter(geoNum)->yStart; yindex++)
	      {
		box.TranslateBy(Point(parserPtr->DBconvertUnit(geometries->getViaIter(geoNum)->x +
				      xindex * geometries->getViaIter(geoNum)->xStep), 
				      parserPtr->DBconvertUnit(geometries->getViaIter(geoNum)->y +
				      yindex * geometries->getViaIter(geoNum)->yStep)));
		geomBoundingBox += Point(box.xMin, box.yMin);    
		geomBoundingBox += Point(box.xMax, box.yMax);
	      }
	    }
	  }  
	  cerr << "Warning: Via Iterator declare in LAYER " << geometries->getLayer(geoNum) << " specified in "
	       << "OBS, unsupported by DB at this time...ignoring" << endl;
	  break;
	}

	case lefiGeomClassE:
	{
	  // Do nothing
	  break;
	}
      
        case lefiGeomEnd:
	{
	  geomLayer = NULL;
	  break;
	}

        case lefiGeomUnknown:
	{
	  cerr << "Error: Unknown item in port for LAYER  " << geometries->getLayer(geoNum) 
	       << " specified in OBS"<< endl;
	  return 1;
	  break;
	}
      }
    }

  return 0;
}

int ParserLEFDEFq::HandleLEFTiming(lefrCallbackType_e typ, lefiTiming* p, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFInputAntenna(lefrCallbackType_e typ, double variable, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFOutputAntenna(lefrCallbackType_e typ, double variable, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFInOutAntenna(lefrCallbackType_e typ, double variable, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleLEFLibraryEnd(lefrCallbackType_e typ, void* ptr, lefiUserData data)
{
  return 0;
}

int ParserLEFDEFq::DBconvertUnit(double value)
{
  return (int)floor((value * uclaDB.context._units) + 0.5);
}

void ParserLEFDEFq::lefparse(const char* lname)
{
  FILE* filePtr;
  int res;

  // Open the lef file for the reader to read
  if ((filePtr = fopen(lname,"r")) == 0) 
  {
    printf("Couldn't open input file '%s'\n", lname);
    exit(0);
  }

  lefrInit();
  lefrSetUserData((void*)this);

  lefrSetVersionCbk(HandleLEFVersion);
  lefrSetCaseSensitiveCbk(HandleLEFCaseSensitive);
  lefrSetNoWireExtensionCbk(HandleLEFNoWireExtension);
  lefrSetBusBitCharsCbk(HandleLEFBusBitChars);
  lefrSetDividerCharCbk(HandleLEFDividerChar);
  lefrSetUnitsCbk(HandleLEFUnits);
  lefrSetPropBeginCbk(HandleLEFPropBegin);
  lefrSetPropCbk(HandleLEFProp);
  lefrSetPropEndCbk(HandleLEFPropEnd);
  lefrSetLayerCbk(HandleLEFLayer);
  lefrSetViaCbk(HandleLEFVia);
  lefrSetViaRuleCbk(HandleLEFViaRule);
  lefrSetNonDefaultCbk(HandleLEFNonDefault);
  lefrSetNoiseMarginCbk(HandleLEFNoiseMargin);
  lefrSetEdgeRateThreshold1Cbk(HandleLEFEdgeRateThreshold1);
  lefrSetEdgeRateThreshold2Cbk(HandleLEFEdgeRateThreshold2);
  lefrSetEdgeRateScaleFactorCbk(HandleLEFEdgeRateScaleFactor);
  lefrSetNoiseTableCbk(HandleLEFNoiseTable);
  lefrSetCorrectionTableCbk(HandleLEFCorrectionTable);
  lefrSetSpacingBeginCbk(HandleLEFSpacingBegin);
  lefrSetSpacingCbk(HandleLEFSpacing);
  lefrSetSpacingEndCbk(HandleLEFSpacingEnd);
  lefrSetMinFeatureCbk(HandleLEFMinFeature);
  lefrSetDielectricCbk(HandleLEFDielectric);
  lefrSetIRDropBeginCbk(HandleLEFIRDropBegin);
  lefrSetIRDropCbk(HandleLEFIRDrop);
  lefrSetIRDropEndCbk(HandleLEFIRDropEnd);
  lefrSetSiteCbk(HandleLEFSite);
  lefrSetArrayBeginCbk(HandleLEFArrayBegin);
  lefrSetArrayCbk(HandleLEFArray);
  lefrSetArrayEndCbk(HandleLEFArrayEnd);
  lefrSetMacroBeginCbk(HandleLEFMacroBegin);
  lefrSetMacroCbk(HandleLEFMacro);
  lefrSetPinCbk(HandleLEFPin);
  lefrSetObstructionCbk(HandleLEFObstruction);
  lefrSetTimingCbk(HandleLEFTiming);
  lefrSetInputAntennaCbk(HandleLEFInputAntenna);
  lefrSetOutputAntennaCbk(HandleLEFOutputAntenna);
  lefrSetInoutAntennaCbk(HandleLEFInOutAntenna);
  lefrSetLibraryEndCbk(HandleLEFLibraryEnd);

  // Invoke the parser
  res = lefrRead(filePtr, lname, (void*)this);
  if (res != 0) 
  {
    printf("LEF parser returns an error\n");
    exit(0);
  }
  else
  {
    fprintf(stdout, "LEF file %s parsed\n", lname);
  }

  fclose(filePtr);
}

void ParserLEFDEFq::defparse(const char* dname)
{
  FILE* filePtr;
  int res;

  // Open the def file for the reader to read
  if ((filePtr = fopen(dname,"r")) == 0) 
  {
    printf("Couldn't open input file '%s'\n", dname);
    exit(0);
  }

  defrInit();

  defrSetUserData((void*)this);
  defrSetAddPathToNet();

  // Here are some unset callbacks
//    defrSetAssertionCbk(Insert Function Here);  
//    defrSetAssertionsStartCbk(Insert Function Here);
//    defrSetStartPinsCbk(Insert Function Here);
//    defrSetFPCStartCbk(Insert Function Here);
//    defrSetTimingDisablesStartCbk(Insert Function Here);
//    defrSetPartitionsExtCbk(Insert Function Here);
//    defrSetFPCCbk(Insert Function Here);
//    defrSetTimingDisableCbk(Insert Function Here);
//    defrSetPartitionCbk(Insert Function Here);

  defrSetArrayNameCbk(HandleDEFArrayName);
  defrSetBusBitCbk(HandleDEFBusBit);
  defrSetCannotOccupyCbk(HandleDEFCannotOccupy);
  defrSetCanplaceCbk(HandleDEFCanplace);
  defrSetComponentStartCbk(HandleDEFComponentStart);
  defrSetComponentCbk(HandleDEFComponent);
  defrSetComponentEndCbk(HandleDEFComponentEnd);
  defrSetConstraintsStartCbk(HandleDEFConstraintsStart);
  defrSetConstraintCbk(HandleDEFConstraint);
  defrSetConstraintsEndCbk(HandleDEFConstraintsEnd);
  defrSetPathCbk(HandleDEFPath);
  defrSetDefaultCapCbk(HandleDEFDefaultCap);
  defrSetPinCapCbk(HandleDEFPinCap);
  defrSetDesignCbk(HandleDEFDesign);
  defrSetDesignEndCbk(HandleDEFDesignEnd);
  defrSetDieAreaCbk(HandleDEFDieArea);
  defrSetDividerCbk(HandleDEFDivider);
  defrSetComponentExtCbk(HandleDEFComponentExt);
  defrSetGroupExtCbk(HandleDEFGroupExt);
  defrSetIoTimingsExtCbk(HandleDEFIoTimingsExt);
  defrSetNetExtCbk(HandleDEFNetExt);
  defrSetNetConnectionExtCbk(HandleDEFNetConnectionExt);
  defrSetPartitionsExtCbk(HandleDEFPartitionsExt);
  defrSetPinExtCbk(HandleDEFPinExt);
  defrSetScanChainExtCbk(HandleDEFScanChainExt);
  defrSetViaExtCbk(HandleDEFViaExt);
  defrSetFloorPlanNameCbk(HandleDEFFloorPlanName);
  defrSetGcellGridCbk(HandleDEFGcellGrid);
  defrSetGroupsStartCbk(HandleDEFGroupsStart);
  defrSetGroupNameCbk(HandleDEFGroupName);
  defrSetGroupMemberCbk(HandleDEFGroupMember);
  defrSetGroupCbk(HandleDEFGroup);
  defrSetGroupsEndCbk(HandleDEFGroupsEnd);
  defrSetHistoryCbk(HandleDEFHistory);
  defrSetIOTimingsStartCbk(HandleDEFIOTimingsStart);
  defrSetIOTimingCbk(HandleDEFIOTiming);
  defrSetIOTimingsEndCbk(HandleDEFIOTimingsEnd);
  defrSetCaseSensitiveCbk(HandleDEFCaseSensitive);
  defrSetNetStartCbk(HandleDEFNetStart);
  defrSetNetCbk(HandleDEFNet);
  defrSetNetEndCbk(HandleDEFNetEnd);
  defrSetStartPinsCbk(HandleDEFStartPins);
  defrSetPinCbk(HandleDEFPin);
  defrSetPinEndCbk(HandleDEFPinEnd);
  defrSetPinPropStartCbk(HandleDEFPinPropStart);
  defrSetPinPropCbk(HandleDEFPinProp);
  defrSetPinPropEndCbk(HandleDEFPinPropEnd);
  defrSetPropDefStartCbk(HandleDEFPropDefStart);
  defrSetPropCbk(HandleDEFProp);
  defrSetPropDefEndCbk(HandleDEFPropDefEnd);
  defrSetRegionStartCbk(HandleDEFRegionStart);
  defrSetRegionCbk(HandleDEFRegion);
  defrSetRegionEndCbk(HandleDEFRegionEnd);
  defrSetRowCbk(HandleDEFRow);
  defrSetScanchainsStartCbk(HandleDEFScanchainsStart);
  defrSetScanchainCbk(HandleDEFScanchain);
  defrSetScanchainsEndCbk(HandleDEFScanchainsEnd);
  defrSetSiteCbk(HandleDEFSite);
  defrSetSNetStartCbk(HandleDEFSNetStart);
  defrSetSNetCbk(HandleDEFSNet);
  defrSetSNetEndCbk(HandleDEFSNetEnd);
  defrSetTechnologyCbk(HandleDEFTechnology);
  defrSetTrackCbk(HandleDEFTrack);
  defrSetUnitsCbk(HandleDEFUnits);
  defrSetVersionCbk(HandleDEFVersion);
  defrSetViaStartCbk(HandleDEFViaStart);
  defrSetViaCbk(HandleDEFVia);
  defrSetViaEndCbk(HandleDEFViaEnd);

  // Invoke the parser
  // Note: Got a compile error with defrRead without the 1 even though the
  // API says it takes in three parameters. Don't know what the zero does,
  // but it works for now
  res = defrRead(filePtr, dname, (void*)this, 1);
  if (res != 0) 
  {
    printf("DEF parser returns an error\n");
    exit(0);
  }
  else
  {
    fprintf(stdout, "DEF file %s parsed\n", dname);
  }

  fclose(filePtr);
}

int ParserLEFDEFq::HandleDEFArrayName(defrCallbackType_e typ, const char* string, defiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleDEFBusBit(defrCallbackType_e typ, const char* string, defiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleDEFCannotOccupy(defrCallbackType_e typ, defiSite* site, defiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleDEFCanplace(defrCallbackType_e typ, defiSite* site, defiUserData data)
{
  return 0;
}

int ParserLEFDEFq::HandleDEFComponentStart(defrCallbackType_e typ, int variable, defiUserData data)
{                        
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  parserPtr->cellList.reserve((int)(variable * 1.2)); //20% leeway for extpins

  return 0;
}

int ParserLEFDEFq::HandleDEFComponent(defrCallbackType_e typ, defiComponent* comp, defiUserData data) 
{  
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  char* compName = NULL;
  ctainerPinsLocal localPinList;
  Point compLocation(DBL_MAX, DBL_MAX);
  bool locationValid = false;
  bool fixed = false;
  Orient compOrient;
  parserRegion* compRegion = NULL;
  
  compName = new char[strlen(comp->id()) + 1];
  strcpy(compName, comp->id());

  MasterCellMapType::iterator masterCellIndex = parserPtr->macroDictionary.find(comp->name());
  if(masterCellIndex == parserPtr->macroDictionary.end())
  {
    cerr << "Master cell " << comp->name() << " not found" << endl;
    return 1;
  }
  
  ctainerMasterPinsLocal::const_iterator pinIndex = masterCellIndex->second->pinsBegin();
  int pinCount = 0;
  for(; pinIndex != masterCellIndex->second->pinsEnd(); pinIndex++)
  {
    dbPin* newPin = new dbPin(*(*pinIndex), pinCount++);
    parserPtr->pinList.push_back(newPin);
    localPinList.push_back(newPin);
  }

  if(comp->hasNets())
  {
    for(int netNum = 0; netNum < comp->numNets(); netNum++)
      cerr << "Net connection " << comp->net(netNum) << " specified in component ignored, unsupported" << endl;
  }

  if(comp->hasGenerate())
    cerr << "Component " << compName << " GENERATE option ignored, unsupported" << endl;

  if(comp->hasRegionBounds())
  {
    compRegion = new parserRegion(parserPtr->regionCount++);
    int *xl, *yl, *xh, *yh;
    int size;
    comp->regionBounds(&size, &xl, &yl, &xh, &yh);
    compRegion->boxes.push_back(BBox(*xl, *yl, *xh, *yh));
    parserPtr->regionList.push_back(compRegion);
  }

  if(comp->hasRegionName())
  {
    RegionMapType::iterator result = parserPtr->regionDictionary.find(comp->regionName());

    if(result != parserPtr->regionDictionary.end())
    {
      cerr << "REGION " << comp->regionName() << " not found for COMPONENT " << compName << endl;
      return 1;
    }
    else
      compRegion = result->second;
  }

  if(comp->isFixed())
  {
    locationValid = true;
    fixed = true;
  }
  else if(comp->isCover())
  {
    locationValid = true;
    fixed = true;
  }
  else if(comp->isPlaced())
    locationValid = true;

  if(locationValid)
  {
    compLocation.x = comp->placementX();
    compLocation.y = comp->placementY();
  }

  switch(comp->placementOrient())
  {
    case N:
      compOrient = Orient(0, true);
      break;
    case E:
      compOrient = Orient(90, true);
      break;
    case S:
      compOrient = Orient(180, true);
      break;
    case W:
      compOrient = Orient(270, true);
      break;
    case FN:
      compOrient = Orient(0, false);
      break;
    case FE:
      compOrient = Orient(90, false);
      break;
    case FS:
      compOrient = Orient(180, false);
      break;
    case FW:
      compOrient = Orient(270, false);
      break;
  }

  unsigned cellIndex = parserPtr->cellList.size();
  dbCell* newCell = new dbCell(compName, cellIndex, *masterCellIndex->second, localPinList);

  CellMapEntry newEntry(strdup(compName), newCell);
  pair<CellMapType::iterator, bool> insertResult = parserPtr->cellDictionary.insert(newEntry);  // 
  if(!insertResult.second)
  {
    delete newCell;
    cerr << "Duplicate declaration of cell " << compName << endl;
    return 1;
  }

  parserPtr->cellList.push_back(newCell);

  ctainerPinsLocal::iterator localPinIndex = localPinList.begin();
  for(; localPinIndex != localPinList.end(); localPinIndex++)
    (*localPinIndex)->_cell = newCell;

  parserPtr->locationsValidList.push_back(locationValid);
  parserPtr->orientList.push_back(compOrient);
  parserPtr->locationList.push_back(compLocation);
  if(compRegion)
  {
    CompRegionEntry newCompRegion(newCell, compRegion);
    parserPtr->compRegionList.push_back(newCompRegion);
  }

  if(fixed)
    parserPtr->fixedCompIndices.push_back(newCell->getIndex());
  
  return 0;
}

int ParserLEFDEFq::HandleDEFComponentEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFConstraintsStart(defrCallbackType_e typ, int variable, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFConstraint(defrCallbackType_e c, defiAssertion* a, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFConstraintsEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFPath(defrCallbackType_e typ, defiPath* path, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFDefaultCap(defrCallbackType_e typ, int variable, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFPinCap(defrCallbackType_e typ, defiPinCap* pincap, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFDesign(defrCallbackType_e typ, const char* value, defiUserData data) 
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  strcpy(parserPtr->uclaDB.designName, value);

  return 0;
}

int ParserLEFDEFq::HandleDEFDesignEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFDieArea(defrCallbackType_e typ, defiBox* box, defiUserData data) 
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  parserPtr->uclaDB.context._bbox = BBox(box->xl(), box->yl(), box->xh(), box->yh()); 
 
  return 0;
}

int ParserLEFDEFq::HandleDEFDivider(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFComponentExt(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFGroupExt(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFIoTimingsExt(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFNetExt(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFNetConnectionExt(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFPartitionsExt(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFPinExt(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFScanChainExt(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFViaExt(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFFloorPlanName(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFGcellGrid(defrCallbackType_e typ, defiGcellGrid* grid, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFGroupsStart(defrCallbackType_e typ, int variable, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFGroupName(defrCallbackType_e typ, const char* string, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFGroupMember(defrCallbackType_e typ, const char* name, defiUserData data) 
{   
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  CellMapType::iterator result = parserPtr->cellDictionary.find(name);
  if(result == parserPtr->cellDictionary.end())
  {
    cerr << "Cell " << name << " not found in group" << endl;
    return 1;
  }
 
  parserPtr->tempCompList.push_back(result->second);
  return 0;
}

int ParserLEFDEFq::HandleDEFGroup(defrCallbackType_e typ, defiGroup* group, defiUserData data)  
{ 
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  int maxX = int(parserPtr->uclaDB.context._bbox.getHalfPerim());
  int maxY = int(parserPtr->uclaDB.context._bbox.getHalfPerim());
  int perim = int(parserPtr->uclaDB.context._bbox.getHalfPerim());
  char* groupName = NULL;
  bool isSoft = false;
  parserRegion* region = NULL;

  groupName = new char[strlen(group->name()) + 1];
  strcpy(groupName, group->name());

  if(group->hasRegionName() | group->hasRegionBox())
  {
    if(group->hasRegionBox())
    {
      int *xl, *yl, *xh, *yh;
      int size;

      group->regionRects(&size, &xl, &yl, &xh, &yh);
      for(int i = 0; i < size; i++)
      {
	region = new parserRegion(parserPtr->regionCount++);
	region->boxes.push_back(BBox(xl[i], yl[i], xh[i], yh[i]));
	parserPtr->regionList.push_back(region);
      }
    }
    if(group->hasRegionName())
    {
      RegionMapType::iterator result = parserPtr->regionDictionary.find(group->regionName());
      if(result == parserPtr->regionDictionary.end())
      {
	cerr << "REGION " << group->regionName() << " not found" << endl;
	return 1;
      }
      else 
	region = result->second;
    }
  }

  if(group->hasPerim() | group->hasMaxX() | group->hasMaxY())
  {
    isSoft = true;
    if(group->hasMaxX())
      maxX = group->maxX();
    if(group->hasMaxY())
      maxY = group->maxY();
    if(group->hasPerim())
      perim = group->perim();
  } 
  
  for(int propNum = 0; propNum < group->numProps(); propNum++)
    cerr << "Properties not supported in groups" << endl;

  if(isSoft)
  {
    //LOOK! this is somewhat wasteful in the case that the region
    //has been specified here by points (instead of by name); we're
    //creating essentially the same object twice.

    if(region) // region specified
      region = new parserRegion(parserPtr->regionCount++, *region, maxX, maxY, perim);
    else
      region = new parserRegion(parserPtr->regionCount++, true, maxX, maxY, perim);
    
    parserPtr->regionList.push_back(region);
  }

  std::vector<dbCell*>::iterator compIndex = parserPtr->tempCompList.begin();
  for(;compIndex != parserPtr->tempCompList.end(); compIndex++)
  {
      CompRegionEntry newEntry(*compIndex, region);
      parserPtr->compRegionList.push_back(newEntry);
  }

  parserPtr->tempCompList.clear();

  return 0;
}

int ParserLEFDEFq::HandleDEFGroupsEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFHistory(defrCallbackType_e typ, const char* string, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFIOTimingsStart(defrCallbackType_e typ, int variable, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFIOTiming(defrCallbackType_e typ, defiIOTiming* iot, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFIOTimingsEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFCaseSensitive(defrCallbackType_e typ, int variable, defiUserData data) 
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  if(variable == 1)
     parserPtr->caseSensitiveDEF = true;
  else
     parserPtr->caseSensitiveDEF = false;

  return 0;
}

int ParserLEFDEFq::HandleDEFNetStart(defrCallbackType_e typ, int variable, defiUserData data) 
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  parserPtr->netList.reserve(parserPtr->netList.size() + variable);

  return 0;
}

int ParserLEFDEFq::HandleDEFNet(defrCallbackType_e typ, defiNet* net, defiUserData data) 
{ 
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  dbNetType::NetUse netUse = dbNetType::SIGNAL;
  char* netName = NULL;
  ctainerPinsLocal localPinList;
  dbNet* currentNet;
  bool hasLogicalPin = false;

  netName = new char[strlen(net->name()) + 1];
  strcpy(netName, net->name());

  if(net->hasUse())
  {   
    if(strcmp(net->use(), "SIGNAL") == 0)
      netUse = dbNetType::SIGNAL;
    else if(strcmp(net->use(),  "POWER") == 0)
      netUse = dbNetType::POWER; 
    else if(strcmp(net->use(), "GROUND") == 0)
      netUse = dbNetType::GROUND;  
    else if(strcmp(net->use(),  "CLOCK") == 0)
      netUse = dbNetType::CLOCK;   
    else if(strcmp(net->use(), "TIEOFF") == 0)
      netUse = dbNetType::TIEOFF;  
    else if(strcmp(net->use(), "ANALOG") == 0)
      netUse = dbNetType::ANALOG;  
    else 
    {
      cerr << "Unrecognized net type" << endl;
      return 1;
    }
  }

  for(int pathNum = 0; pathNum < net->numPaths(); pathNum++)
  {
    defiPath* path = net->path(pathNum);
    path->initTraverse();
    
    while(path->next() != DEFIPATH_DONE)
    {
      switch(path->next())
      {
	case DEFIPATH_LAYER:     
	{
	  LayerMapType::iterator layerIndex = parserPtr->layerDictionary.find(path->getLayer());
	  if(layerIndex == parserPtr->layerDictionary.end())
	  {
	    cerr << "LAYER " << path->getLayer() << " not found in NET section" << endl;
	    return 1;
	  }
	  break;
	}

	// Snet value
        case DEFIPATH_VIA:   
	{
	  ViaMapType::iterator viaIndex = parserPtr->viaDictionary.find(path->getVia());
	  if(viaIndex == parserPtr->viaDictionary.end())
	  {
	    cerr << "VIA " << path->getVia() << " not found in NET section" << endl;
	    return 1;
	  }
	  break;
	}

	// Snet value
        case DEFIPATH_WIDTH:
	  break;
	
	// Snet value
        case DEFIPATH_POINT:
	  break;
	
        // Don't care if we see taper or not
        case DEFIPATH_TAPER:
	  break;
	
	// Snet value
        case DEFIPATH_SHAPE:
	  break;
      }
    }
  }

  for(int pinNum = 0; pinNum < net->numConnections(); pinNum++)
  {
    bool isIOpin = false;
    char* pinName = new char[strlen(net->pin(pinNum)) + 1];
    char actualCellName[1023];
    char actualPinName[1023];
    dbCell* cell = NULL;
    dbPin* pin = NULL;

    strcpy(pinName, net->pin(pinNum));

    if (strcmp(net->instance(pinNum), "PIN") == 0)
    {
      strcpy(actualCellName,"FakedInstantiatedCellFor");
      strcat(actualCellName, pinName);
      strcpy(actualPinName,"FakedMasterPinFor");
      strcat(actualPinName, pinName);
      isIOpin=true;
    }
    else
    {
      strcpy(actualCellName, net->instance(pinNum));
      strcpy(actualPinName, net->pin(pinNum));
    }

    CellMapType::iterator cellIndex = parserPtr->cellDictionary.find(actualCellName);
    if(cellIndex == parserPtr->cellDictionary.end())
    {
      if(!isIOpin)
      {
	cerr << "Cell " << actualCellName << " not found in net " << netName << endl;
	return 1;
      }

      //assume this is a logical pin.  NOTE!!! if the name is
      //just spelled wrong, this code WILL NOT NOTICE!!! mro 001012
      
      hasLogicalPin = true;
    }
    else
    {
      cell = cellIndex->second;

      ctainerPinsLocal::iterator cellPinIndex = cell->_pins.begin();
      for(; cellPinIndex != cell->_pins.end(); cellPinIndex++)
      {
	if(strcmp((*cellPinIndex)->_master._name, actualPinName) == 0)
	{
	  pin = *cellPinIndex;
	  break;
	}
      }

      if(!pin)
      {
	cerr << "Pin " << actualPinName << " not found in cell " << actualCellName << " in net " << netName << endl;
	return 1;
      }

      localPinList.push_back(pin);
    }
  }

  NetMapType::iterator netIndex = parserPtr->netDictionary.find(netName); // a 64-bit issue?

  // If the current net does not exist, add it
  if(netIndex == parserPtr->netDictionary.end())
  {
    currentNet = new dbNet(netName, parserPtr->netCount++, dbNetType(dbNetType::UNKNOWN), localPinList);
    
    NetMapEntry newEntry(strdup(netName), currentNet);
    pair<NetMapType::iterator, bool> result = parserPtr->netDictionary.insert(newEntry);
    
    if(!result.second)
    {
      delete currentNet;
      cerr << "Internal error at net " << netName << endl;
      return 1;
    }

    //by sadya for weight processing
    if(net->hasWeight())
      currentNet->_weight = net->weight();

    parserPtr->netList.push_back(currentNet);
  }
  // Else, add the pin the to already existing net
  else
  {
    currentNet = netIndex->second;

    // we have to add connections to an existing net from
    // an external pin
    ctainerPinsLocal::iterator localPinIndex = localPinList.begin();
    for(; localPinIndex != localPinList.end(); localPinIndex++)
    {
      dbPin &pin = *(*localPinIndex);
      if(pin._net)
      {
	if(pin._net != currentNet)
	{
	  cerr << "Pin " << pin.getMasterPinName() << "of cell " << pin.getCell().getName()
	       << " found on two nets, " << pin._net->getName() << " and " 
	       << netName << endl;
	  return 1;
	}
      }
      else
	currentNet->_pins.push_back(*localPinIndex);
    }
  }

  if(hasLogicalPin)
    currentNet->_hasLogicalPin = true;

  ctainerPinsLocal::iterator localPinIndex = localPinList.begin();
  for(; localPinIndex != localPinList.end(); localPinIndex++)
  {
    dbPin &pin = *(*localPinIndex);
    if(pin._net)
    {
      if(pin._net != currentNet)
      {
	cerr << "Pin " << pin.getMasterPinName() << "of cell " << pin.getCell().getName()
	     << " found on two nets, " << pin._net->getName() << " and " 
	     << netName << endl;
	return 1;
      }
    }
    pin._net = currentNet;
  }

  *(const_cast<dbNetType*>(&(currentNet->_type))) = dbNetType(netUse);
 
  return 0;
}

int ParserLEFDEFq::HandleDEFNetEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFStartPins(defrCallbackType_e typ, int variable, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFPin(defrCallbackType_e typ, defiPin* pin, defiUserData data)  
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  char* pinName = NULL;
  dbMasterCellType::PadSubClass padClass = dbMasterCellType::PadUNKNOWN;
  char* pinDirection = NULL;
  char* pinUse = NULL;
  bool isSpecial = false;
  bool locationValid = false;
  bool fixed = false;
  Point pinLocation(DBL_MAX, DBL_MAX);
  Orient pinOrient;
  dbLayerGeom* layerGeom = NULL;
  bool isLogical = true;
  ctainerPinsLocal localPinList;

  pinName = new char[strlen(pin->pinName()) + 1];
  strcpy(pinName, pin->pinName());

  // Directions are reversed because input to chip is output of fake cell
  if(pin->hasDirection())
  {
    if(strcmp(pin->direction(), "INPUT") == 0)
    {
      padClass = dbMasterCellType::INPUT;
      pinDirection = new char[strlen("OUTPUT") + 1];
      strcpy(pinDirection, "OUTPUT");
    }
    else if(strcmp(pin->direction(), "INOUT") == 0)
    {     
      padClass = dbMasterCellType::INOUT;
      pinDirection = new char[strlen("INOUT") + 1];
      strcpy(pinDirection, "INOUT");
    }
    else if(strcmp(pin->direction(), "OUTPUT") == 0)
    {
      padClass = dbMasterCellType::OUTPUT;
      pinDirection = new char[strlen("INPUT") + 1];
      strcpy(pinDirection, "INPUT");
    }
    else if(strcmp(pin->direction(), "FEEDTHRU") == 0)
    {
      // Not sure if this is right, but its what ParserLEFDEF does
      padClass = dbMasterCellType::PadUNKNOWN;
      pinDirection = new char[strlen("INOUT") + 1];
      strcpy(pinDirection, "INOUT");
    }
  }

  if(pin->hasSpecial())
    isSpecial = true;

  if(pin->isPlaced())
  {
    locationValid = true;
  }
  else if(pin->isCover())
  {
    locationValid = true;
    fixed = true;
  }
  else if(pin->isFixed())
  {
    locationValid = true;
    fixed = true;
  }

  if(pin->hasPlacement())
  {
    pinLocation.x = pin->placementX();
    pinLocation.y = pin->placementY();
  }

  switch(pin->orient())
  {
    case N:
      pinOrient = Orient(0, true);
      break;
    case E:
      pinOrient = Orient(90, true);
      break;
    case S:
      pinOrient = Orient(180, true);
      break;
    case W:
      pinOrient = Orient(270, true);
      break;
    case FN:
      pinOrient = Orient(0, false);
      break;
    case FE:
      pinOrient = Orient(90, false);
      break;
    case FS:
      pinOrient = Orient(180, false);
      break;
    case FW:
      pinOrient = Orient(270, false);
      break;
  }

  if(pin->hasUse())
  {
    if(strcmp(pin->use(), "TIEOFF") == 0)
    {
      cerr << "Illegal USE specification " << pin->use() << " for PIN " << pin->pinName() << endl;
      return 1;
    }
    pinUse = new char[strlen(pin->use()) + 1];
    strcpy(pinUse, pin->use());
  }
  else
  {
    pinUse = new char[strlen("SIGNAL")];
    strcpy(pinUse, "SIGNAL");
  }

  if(pin->hasLayer())
  {
    LayerMapType::iterator result = parserPtr->layerDictionary.find(pin->layer());
    int xl, yl, xh, yh;

    if(result == parserPtr->layerDictionary.end())
    {
      cerr << "Layer " << pin->layer() << " not found for pin " << pinName << endl;
      return 1;
    }
  
    pin->bounds(&xl, &yl, &xh, &yh);
    BBox masterPinBox(xl, yl, xh, yh);
    layerGeom = new dbLayerGeom(masterPinBox, *(result->second));

    isLogical = false;
    char masterPinName[1023];
    strcpy(masterPinName, "FakedMasterPinFor");
    strcat(masterPinName, pinName);
      
    if(masterPinBox.isEmpty())
    {
      cerr << "I/O pin " << pinName << " has empty bounding box, adding (0, 0) to avoid crash" << endl;
      masterPinBox += Point(0,0);
    }
      
    dbMasterPin* newMasterPin = new dbMasterPin(masterPinName, pinDirection, pinUse, "", masterPinBox, masterPinBox.getGeomCenter());
    parserPtr->uclaDB.design._pinLib.push_back(newMasterPin);
    newMasterPin->_ports.push_back(layerGeom);

    char masterCellName[1023];
    ctainerMasterPinsLocal masterPins;
    ctainerLayerGeoms      emptyGeoms;
      
    masterPins.push_back(newMasterPin);
    strcpy(masterCellName, "FakedMasterCellFor");
    strcat(masterCellName, pinName);
      
    dbMasterCell* newMasterCell = new dbMasterCell(masterCellName, dbMasterCellType::EXTPIN, padClass, 0.0, 0.0, Point(0.0,0.0),
						   masterPins, emptyGeoms, Symmetry(false, false, false),
						   *(parserPtr->externalPinSite));
    newMasterPin->_cell = newMasterCell;
    
    MasterCellMapEntry newEntry(strdup(masterCellName), newMasterCell);
    
    pair<MasterCellMapType::iterator, bool> insertResult = parserPtr->macroDictionary.insert(newEntry);
    if(!insertResult.second)
    {
      cerr << "Duplicate declaration of I/O pin master cell" << endl;
      delete newMasterCell;
      return 1;
    }
    parserPtr->uclaDB.design._cellLib.push_back(newMasterCell);
    
    int numPins = parserPtr->pinList.size();
    
    dbPin* newPin = new dbPin(*newMasterPin, numPins);
    parserPtr->pinList.push_back(newPin);
      
    localPinList.clear();
    localPinList.push_back(newPin);
    
    char cellName[1023];
    strcpy(cellName,"FakedInstantiatedCellFor");
    strcat(cellName, pinName);
    
    int numCells = parserPtr->cellList.size();
    
    dbCell* newCell = new dbCell(cellName, numCells, *newMasterCell, localPinList);
    
    newPin->_cell = newCell;
    
    CellMapEntry newCellEntry(strdup(cellName), newCell); 
    pair<CellMapType::iterator, bool> insertCellResult = parserPtr->cellDictionary.insert(newCellEntry);

    if(!insertCellResult.second)
    {
      cerr << "Duplicate IO pin name " << cellName << endl;
      delete newCell;
      return 1;
    }
    parserPtr->cellList.push_back(newCell);

    parserPtr->locationsValidList.push_back(locationValid);
    parserPtr->orientList.push_back(pinOrient);
    parserPtr->locationList.push_back(pinLocation);
    if(fixed)
      parserPtr->fixedCompIndices.push_back(newCell->getIndex());    
  }
  else
    localPinList.clear();

  dbNet* pinNet = NULL;
  char netName[1023];
  
  strcpy(netName, pin->netName());
  
  NetMapType::iterator netIndex;
  if(isSpecial)
  {
    netIndex = parserPtr->specialNetDictionary.find(netName);
    if(netIndex != parserPtr->specialNetDictionary.end())
      pinNet = netIndex->second;
  }
  else
  {
    netIndex = parserPtr->netDictionary.find(netName);
    if(netIndex != parserPtr->netDictionary.end())
      pinNet = netIndex->second;
  }

  if(!pinNet)
  {
    pinNet = new dbNet(netName, (isSpecial)?(parserPtr->specialNetCount++):(parserPtr->netCount++), 
		       dbNetType(pinUse), localPinList);
    
    NetMapEntry newNetEntry(strdup(netName), pinNet);
    pair<NetMapType::iterator, bool> insertNetResult;
    
    if(isSpecial)
      insertNetResult = parserPtr->specialNetDictionary.insert(newNetEntry);
    else
      insertNetResult = parserPtr->netDictionary.insert(newNetEntry);

    if(!insertNetResult.second)
    {
      cerr << "Duplicate net " << netName << " declared " << endl;
      delete pinNet;
      return 1;
    }
    
    if(isSpecial)
      parserPtr->specialNetList.push_back(pinNet);
    else
      parserPtr->netList.push_back(pinNet);
  }
  else
  {
    ctainerPinsLocal::iterator pinIndex = localPinList.begin();
    for(; pinIndex != localPinList.end(); pinIndex++)
      pinNet->_pins.push_back((*pinIndex));
  }
  
  if(isLogical)
    pinNet->_hasLogicalPin = true;

  ctainerPinsLocal::iterator pinIndex = localPinList.begin();
  for(; pinIndex != localPinList.end(); pinIndex++)
  {
    if((*pinIndex)->_net)
    {
      cerr << "Pin " << pinName << " of cell " << (*pinIndex)->getCell().getName() << " found on two nets, "
	   << (*pinIndex)->_net->getName() << " and " << netName << endl;
      return 1;
    }
    (*pinIndex)->_net = pinNet;
  }
  
  return 0; 
}

int ParserLEFDEFq::HandleDEFPinEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFPinPropStart(defrCallbackType_e typ, int variable, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFPinProp(defrCallbackType_e typ, defiPinProp* pp, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFPinPropEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFPropDefStart(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFProp(defrCallbackType_e typ, defiProp* prop, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFPropDefEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFRegionStart(defrCallbackType_e typ, int variable, defiUserData data) 
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  parserPtr->regionList.reserve(variable);

  return 0;
}

int ParserLEFDEFq::HandleDEFRegion(defrCallbackType_e typ, defiRegion* reg, defiUserData data)  
{                       
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;
  parserRegion* newRegion = NULL;
  char* regionName = NULL;

  regionName = new char[strlen(reg->name()) + 1];
  strcpy(regionName, reg->name());
  
  newRegion = new parserRegion(parserPtr->regionCount++);

  if(reg->numProps() > 0)
    cerr << "Properties not supported in regions. Declared in REGION " << regionName << endl;

  for(int rectNum = 0; rectNum < reg->numRectangles(); rectNum++)
    newRegion->boxes.push_back(BBox(reg->xl(rectNum), reg->yl(rectNum), reg->xh(rectNum), reg->yh(rectNum)));
  
  RegionMapEntry newEntry(strdup(regionName), newRegion);
  pair<RegionMapType::iterator, bool> result = parserPtr->regionDictionary.insert(newEntry);

  if(!result.second)
  {
    cerr << "Internal error at REGION " << regionName << endl;
    return 1;
  }

  parserPtr->regionList.push_back(newRegion);

  return 0;
}

int ParserLEFDEFq::HandleDEFRegionEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFRow(defrCallbackType_e typ, defiRow* row, defiUserData data) 
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  char* rowName = NULL;
  dbSite* rowSite = NULL;
  unsigned int numSites = unsigned(-1);
  unsigned int rowSpacing = unsigned(-1);
  Point rowOrig(DBL_MAX, DBL_MAX);
  Orient rowOrient;
  dbRow::Dir rowDir = static_cast<dbRow::Dir>(INT_MAX);

  rowName = new char[strlen(row->name()) + 1];
  strcpy(rowName, row->name());

  SiteMapType::iterator siteIndex = parserPtr->siteDictionary.find(row->macro());
  if(siteIndex == parserPtr->siteDictionary.end())
  {
    cerr << "Site " << row->macro() << " declared in ROW " << row->name() << " not found" << endl;
    return 1;
  }
  else
    rowSite = siteIndex->second;

  rowOrig.x = row->x();
  rowOrig.y = row->y();

  switch(row->orient())
  {
    case N:
      rowOrient = Orient(0, true);
      break;
    case E:
      rowOrient = Orient(90, true);
      break;
    case S:
      rowOrient = Orient(180, true);
      break;
    case W:
      rowOrient = Orient(270, true);
      break;
    case FN:
      rowOrient = Orient(0, false);
      break;
    case FE:
      rowOrient = Orient(90, false);
      break;
    case FS:
      rowOrient = Orient(180, false);
      break;
    case FW:
      rowOrient = Orient(270, false);
      break;
  }

  if(row->xNum() > 1)
  {
    numSites = unsigned(floor(row->xNum() + 0.5));
    rowDir = dbRow::HORIZ;
    rowSpacing = unsigned(floor(row->xStep() + 0.5));
    if(row->yNum() > 1)
    {
      cerr << "Ambigous row direction for ROW " << rowName;
      return 1;
    }
  }
  else if(row->yNum() > 1)
  {
    numSites = unsigned(floor(row->yNum() + 0.5));
    rowDir = dbRow::VERT;    
    rowSpacing = unsigned(floor(row->yStep() + 0.5));
  }
  else // 1 by 1 case
  {
    if(floor(row->xNum() + 0.5) != 1 || floor(row->yNum() + 0.5) != 1)
    {
      cerr << "Illegal num sites for ROW " << rowName << endl;
      return 1;
    }
    numSites = 1;
    rowDir = dbRow::HORIZ; // shouldn't matter if its horiz or vertical
    rowSpacing = 0;
  }
  
  parserPtr->rowList.push_back(new dbRow(*rowSite, rowOrig, rowDir, rowSpacing, numSites, rowName, rowOrient));

  return 0;
}

int ParserLEFDEFq::HandleDEFScanchainsStart(defrCallbackType_e typ, int variable, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFScanchain(defrCallbackType_e typ, defiScanchain* sc, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFScanchainsEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFSite(defrCallbackType_e typ, defiSite* site, defiUserData data) 
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  dbSite* theSite = NULL;
  char* siteName = NULL;
  unsigned int numSites = unsigned(-1);
  unsigned int siteSpacing = unsigned(-1);
  Point siteOrig(DBL_MAX, DBL_MAX);
  Orient siteOrient;
  dbRow::Dir siteDir = static_cast<dbRow::Dir>(INT_MAX);

  siteName = new char[strlen("Anonymous Row ") + 2];
  sprintf(siteName, "Anonymous Row %d", parserPtr->rowList.size());
  siteName[strlen("Anonymous Row ") + 2] = '\0';

  SiteMapType::iterator siteIndex = parserPtr->siteDictionary.find(site->name());
  if(siteIndex == parserPtr->siteDictionary.end())
  {
    cerr << "SITE " << site->name() << " not found" << endl;
    return 1;
  }
  else
    theSite = siteIndex->second;

  siteOrig.x = site->x_orig();
  siteOrig.y = site->y_orig();

  switch(site->orient())
  {
    case N:
      siteOrient = Orient(0, true);
      break;
    case E:
      siteOrient = Orient(90, true);
      break;
    case S:
      siteOrient = Orient(180, true);
      break;
    case W:
      siteOrient = Orient(270, true);
      break;
    case FN:
      siteOrient = Orient(0, false);
      break;
    case FE:
      siteOrient = Orient(90, false);
      break;
    case FS:
      siteOrient = Orient(180, false);
      break;
    case FW:
      siteOrient = Orient(270, false);
      break;
  }

  if(site->x_num() > 1)
  {
    numSites = unsigned(floor(site->x_num() + 0.5));
    siteDir = dbRow::HORIZ;
    siteSpacing = unsigned(floor(site->x_step() + 0.5));
    if(site->y_num() > 1)
    {
      cerr << "Ambigous site direction for SITE " << siteName << endl;
      return 1;
    }
  }
  else if(site->y_num() > 1)
  {
    numSites = unsigned(floor(site->y_num() + 0.5));
    siteDir = dbRow::VERT;    
    siteSpacing = unsigned(floor(site->y_step() + 0.5));
  }
  else // 1 by 1 case
  {
    if(floor(site->x_num() + 0.5) != 1 || floor(site->y_num() + 0.5) != 1)
    {
      cerr << "Illegal num sites for SITE " << siteName << endl;
      return 1;
    }
    numSites = 1;
    siteDir = dbRow::HORIZ; // shouldn't matter if its horiz or vertical
    siteSpacing = 0;
  }
  
  parserPtr->rowList.push_back(new dbRow(*theSite, siteOrig, siteDir, siteSpacing, numSites, siteName, siteOrient));

  return 0;
}

int ParserLEFDEFq::HandleDEFSNetStart(defrCallbackType_e typ, int variable, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFSNet(defrCallbackType_e typ, defiNet* net, defiUserData data) 
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  dbNetType::NetUse netUse = dbNetType::SIGNAL;
  char* netName = NULL;
  ctainerPinsLocal localPinList;
  dbNet* currentNet;
  bool hasLogicalPin = false;
  Point prevPoint(DBL_MAX, DBL_MAX);
  dbLayer::Dir prevDirection = dbLayer::NOPREFERRED;
  dbLayer::Dir pathDirection = dbLayer::NOPREFERRED;
  double width = DBL_MAX;
  ctainerLayerGeoms netWiring;

  netName = new char[strlen(net->name()) + 1];
  strcpy(netName, net->name());

  if(net->hasUse())
  {   
    if(strcmp(net->use(), "SIGNAL") == 0)
      netUse = dbNetType::SIGNAL;
    else if(strcmp(net->use(),  "POWER") == 0)
      netUse = dbNetType::POWER; 
    else if(strcmp(net->use(), "GROUND") == 0)
      netUse = dbNetType::GROUND;  
    else if(strcmp(net->use(),  "CLOCK") == 0)
      netUse = dbNetType::CLOCK;   
    else if(strcmp(net->use(), "TIEOFF") == 0)
      netUse = dbNetType::TIEOFF;  
    else if(strcmp(net->use(), "ANALOG") == 0)
      netUse = dbNetType::ANALOG;  
    else 
    {
      cerr << "Unrecognized net type" << endl;
      return 1;
    }
  }

  for(int pathNum = 0; pathNum < net->numPaths(); pathNum++)
  {
    defiPath* path = net->path(pathNum);
    path->initTraverse();
    
    while(path->next() != DEFIPATH_DONE)
    {
      switch(path->next())
      {
	case DEFIPATH_LAYER:     
	{
	  LayerMapType::iterator layerIndex = parserPtr->layerDictionary.find(path->getLayer());
	  if(layerIndex == parserPtr->layerDictionary.end())
	  {
	    cerr << "LAYER " << path->getLayer() << " not found in NET section" << endl;
	    return 1;
	  }
	  break;
	}

	// Snet value
        case DEFIPATH_VIA:   
	{
	  ViaMapType::iterator viaIndex = parserPtr->viaDictionary.find(path->getVia());
	  if(viaIndex == parserPtr->viaDictionary.end())
	  {
	    cerr << "VIA " << path->getVia() << " not found in NET section" << endl;
	    return 1;
	  }
	  break;
	}

	// Snet value
        case DEFIPATH_WIDTH:
	  break;
	
	// Snet value
        case DEFIPATH_POINT:
	  break;
	
        // Don't care if we see taper or not
        case DEFIPATH_TAPER:
	  break;
	
	// Snet value
        case DEFIPATH_SHAPE:
	  break;
      }
    }
  }

  for(int pinNum = 0; pinNum < net->numConnections(); pinNum++)
  {
    bool isIOpin = false;
    char* pinName = new char[strlen(net->pin(pinNum)) + 1];
    char actualCellName[1023];
    char actualPinName[1023];
    dbCell* cell = NULL;
    dbPin* pin = NULL;

    strcpy(pinName, net->pin(pinNum));

    if (strcmp(net->instance(pinNum), "PIN") == 0)
    {
      strcpy(actualCellName,"FakedInstantiatedCellFor");
      strcat(actualCellName, pinName);
      strcpy(actualPinName,"FakedMasterPinFor");
      strcat(actualPinName, pinName);
      isIOpin=true;
    }
    else
    {
      strcpy(actualCellName, net->instance(pinNum));
      strcpy(actualPinName, net->pin(pinNum));
    }

    CellMapType::iterator cellIndex = parserPtr->cellDictionary.find(actualCellName);
    if(cellIndex == parserPtr->cellDictionary.end())
    {
      if(!isIOpin)
      {
	cerr << "Cell " << actualCellName << " not found in net " << netName << endl;
	return 1;
      }

      //assume this is a logical pin.  NOTE!!! if the name is
      //just spelled wrong, this code WILL NOT NOTICE!!! mro 001012
      
      hasLogicalPin = true;
    }
    else
    {
      cell = cellIndex->second;

      ctainerPinsLocal::iterator cellPinIndex = cell->_pins.begin();
      for(; cellPinIndex != cell->_pins.end(); cellPinIndex++)
      {
	if(strcmp((*cellPinIndex)->_master._name, actualPinName) == 0)
	{
	  pin = *cellPinIndex;
	  break;
	}
      }

      if(!pin)
      {
	cerr << "Pin " << actualPinName << " not found in cell " << actualCellName << " in net " << netName << endl;
	return 1;
      }

      localPinList.push_back(pin);
    }
  }

  NetMapType::iterator netIndex = parserPtr->specialNetDictionary.find(netName);

  // If the current net does not exist, add it
  if(netIndex == parserPtr->specialNetDictionary.end())
  {
    currentNet = new dbNet(netName, parserPtr->netCount++, dbNetType(dbNetType::UNKNOWN), localPinList);
    
    NetMapEntry newEntry(strdup(netName), currentNet);
    pair<NetMapType::iterator, bool> result = parserPtr->specialNetDictionary.insert(newEntry);
    
    if(!result.second)
    {
      delete currentNet;
      cerr << "Internal error at net " << netName << endl;
      return 1;
    }

    parserPtr->specialNetList.push_back(currentNet);
  }
  // Else, add the pin the to already existing net
  else
  {
    currentNet = netIndex->second;

    // we have to add connections to an existing net from
    // an external pin
    ctainerPinsLocal::iterator localPinIndex = localPinList.begin();
    for(; localPinIndex != localPinList.end(); localPinIndex++)
    {
      dbPin &pin = *(*localPinIndex);
      if(pin._net)
      {
	if(pin._net != currentNet)
	{
	  cerr << "Pin " << pin.getMasterPinName() << "of cell " << pin.getCell().getName()
	       << " found on two nets, " << pin._net->getName() << " and " 
	       << netName << endl;
	  return 1;
	}
      }
      else
	currentNet->_pins.push_back(*localPinIndex);
    }
  }

  if(hasLogicalPin)
    currentNet->_hasLogicalPin = true;

  ctainerPinsLocal::iterator localPinIndex = localPinList.begin();
  for(; localPinIndex != localPinList.end(); localPinIndex++)
  {
    dbPin &pin = *(*localPinIndex);
    if(pin._net)
    {
      if(pin._net != currentNet)
      {
	cerr << "Pin " << pin.getMasterPinName() << "of cell " << pin.getCell().getName()
	     << " found on two nets, " << pin._net->getName() << " and " 
	     << netName << endl;
	return 1;
      }
    }
    pin._net = currentNet;
  }

  if(net->numPaths() > 0) 
  {
    bool pointSpecified = false;

    for(int pathNum = 0; pathNum < net->numPaths(); pathNum++) 
    {
      defiPath* path = net->path(pathNum);
      dbLayer* pathLayer = NULL;
      int element;
      double wireMinX, wireMinY, wireMaxX, wireMaxY;
      vector<BBoxExtended>    wireGeoms;
      vector<bool> wrongWay;

      path->initTraverse();

      while((element = path->next()) != DEFIPATH_DONE) 
      {
	switch(element) 
	{
            case DEFIPATH_LAYER:
	    {	
	      pointSpecified = false;

	      LayerMapType::iterator layerIndex = parserPtr->layerDictionary.find(path->getLayer());

	      if(layerIndex == parserPtr->layerDictionary.end())
	      {
		cerr << "Layer " << path->getLayer() << "not found in SNET section" << endl;
		return 1;
	      }
	      else
		pathLayer = layerIndex->second;

	      break;
	    }
	    case DEFIPATH_VIA:
	    {
	      ViaMapType::iterator viaIndex = parserPtr->viaDictionary.find(path->getVia());

	      if(viaIndex == parserPtr->viaDictionary.end())
	      {
		cerr << "Via " << path->getVia() << " could not be found in SPECIALNET" << endl;
		return 1;
	      }
	      else
	      {
		ctainerLayerGeoms const& layerGeoms=viaIndex->second->getGeometries();
		ctainerLayerGeoms::const_iterator geomIndex = layerGeoms.begin();

		for(; geomIndex != layerGeoms.end(); geomIndex++)
		{
		  BBox geomRect = *(*geomIndex);
		  geomRect.TranslateBy(prevPoint);

		  dbLayerGeom* newLayerGeom = new dbLayerGeom(geomRect, (*geomIndex)->getLayer());

		  netWiring.push_back(newLayerGeom);
		}
	      }
	      break;    
	    }
            case DEFIPATH_WIDTH:
	    {
	      width = path->getWidth();
	      break;
	    }
            case DEFIPATH_POINT:
	    {
	      int x, y;
	      path->getPoint(&x, &y);

	      if(!pointSpecified)
	      {
		prevPoint = Point(x, y); 
		pathDirection = prevDirection = dbLayer::NOPREFERRED;
		wireMaxX = wireMinX = x;
		wireMaxY = wireMinY = y;
		wireGeoms.clear();
		wrongWay.clear();
		pointSpecified = true;
	      }
	      else
	      {
		if(x == prevPoint.x)
		  pathDirection = dbLayer::VERT;
		else if(y == prevPoint.y)
		  pathDirection = dbLayer::HORIZ;

		if(pathDirection != prevDirection && prevDirection != dbLayer::NOPREFERRED)
		{
		  BBoxExtended newGeom(wireMinX - width/2, wireMinY - width/2,
				       wireMaxX + width/2, wireMaxY + width/2);
		  
		  wireGeoms.push_back(newGeom);
		  
		  wrongWay.push_back(prevDirection != pathLayer->getDirection());
		  
		  wireMinX = wireMaxX = prevPoint.x;
		  wireMinY = wireMaxY = prevPoint.y;
		}

		prevPoint = Point(x, y);
		if (x < wireMinX) 
		  wireMinX = x;
		if (x > wireMaxX) 
		  wireMaxX = x;
		if (y < wireMinY) 
		  wireMinY = y;
		if (y > wireMaxY) 
		  wireMaxY = y;
		prevDirection = pathDirection;
	      }	      
	      break;
	    }

            case DEFIPATH_SHAPE:
	    {
	      // Do nothing
	      break;
	    }
        }
      }
      if(fabs(wireMinX - wireMaxX) > 1e-4 || fabs(wireMinY - wireMaxY) > 1e-4)
      {
        BBoxExtended newGeom(wireMinX - width/2, wireMinY - width/2,
			     wireMaxX + width/2, wireMaxY + width/2);
        wireGeoms.push_back(newGeom);                                  
        wrongWay.push_back(pathDirection != pathLayer->getDirection());
      }
      
      for(unsigned int i = 0; i < wireGeoms.size(); i++)
      {
	//Next block says:  If we have overlapping rectangles, pare
        //redundant pieces off the wrong-way-routed wires.
	
        if (wrongWay[i])
	{
	  if (i > 0)
	    wireGeoms[i].snip(wireGeoms[i-1]);
	  if (i < wireGeoms.size() - 1)
	    wireGeoms[i].snip(wireGeoms[i+1]);
	}
	
	dbLayerGeom* newLayerGeom = new dbLayerGeom(wireGeoms[i], *pathLayer);
        netWiring.push_back(newLayerGeom);
      }
    }
  }
  
  *(const_cast<dbNetType*>(&(currentNet->_type))) = dbNetType(netUse);
  currentNet->_wiring = netWiring;

  return 0;
}

int ParserLEFDEFq::HandleDEFSNetEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

int ParserLEFDEFq::HandleDEFTechnology(defrCallbackType_e typ, const char* value, defiUserData data) 
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  strcpy(parserPtr->uclaDB.technoName, value);

  return 0;
}

int ParserLEFDEFq::HandleDEFTrack(defrCallbackType_e typ, defiTrack* track, defiUserData data) 
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;
  bool     isHorizontal = false;

  if(strcmp(track->macro(), "X") == 0)
    isHorizontal = true;
  else if(strcmp(track->macro(), "Y") == 0)
    isHorizontal = false;
  
  for(int layerNum = 0; layerNum < track->numLayers(); layerNum++)
  {
    LayerMapType::iterator layerIndex = parserPtr->layerDictionary.find(track->layer(layerNum));
    
    if(layerIndex == parserPtr->layerDictionary.end())
    {
      cerr << "Layer " << track->layer(layerNum) << " not found in TRACKS section" << endl;
      return 1;
    }

    parserPtr->trackList.push_back(new dbTrack(*(layerIndex->second), isHorizontal, track->x(), track->xNum(), track->xStep()));
  }
				   
  return 0;
}

int ParserLEFDEFq::HandleDEFUnits(defrCallbackType_e typ, double variable, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFVersion(defrCallbackType_e typ, double variable, defiUserData data) 
{
  return 0;
}

int ParserLEFDEFq::HandleDEFViaStart(defrCallbackType_e typ, int numVias, defiUserData data) 
{       
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;   

  parserPtr->viaList.reserve(parserPtr->viaList.size() + (int)(numVias + 0.01));

  return 0;
}

int ParserLEFDEFq::HandleDEFVia(defrCallbackType_e typ, defiVia* via, defiUserData data)  
{
  ParserLEFDEFq* parserPtr = (ParserLEFDEFq*)data;

  char* viaName = NULL;
  dbVia* newVia = NULL;
  ctainerLayerGeoms layerGeoms;

  viaName = new char[strlen(via->name()) + 1];
  strcpy(viaName, via->name());

  for(int layerNum = 0; layerNum < via->numLayers(); layerNum++)
  {
    char* layerName;
    int xl, yl, xh, yh;

    via->layer(layerNum, &layerName, &xl, &yl, &xh, &yh);
    LayerMapType::iterator layerIndex = parserPtr->layerDictionary.find(layerName);
    if(layerIndex == parserPtr->layerDictionary.end())
    {
      cerr << "LAYER " << layerName << " defined in VIA " << viaName << " not found" << endl;
      return 1;
    }
    layerGeoms.push_back(new dbLayerGeom(BBox(xl, yl, xh, yh), *(layerIndex->second), true));
  }

  std::sort(layerGeoms.begin(), layerGeoms.end(), CompareLayerGeoms());

  ViaMapType::iterator viaIndex = parserPtr->viaDictionary.find(viaName);
  if(viaIndex != parserPtr->viaDictionary.end())
  {            
    if(viaIndex->second->getGeometries().size() != layerGeoms.size())
    {
      cerr << "Size of via geometry size for VIA " << viaName << " does not match "
	   << " number of actual geoms for via. Internal Error." << endl;
      return 1;
    }
    
    ctainerLayerGeoms::const_iterator declaredGeomIndex = layerGeoms.begin();
    ctainerLayerGeoms::const_iterator viaGeomIndex = viaIndex->second->getGeometries().begin();
    
    for(; viaGeomIndex != viaIndex->second->getGeometries().end(); viaGeomIndex++, declaredGeomIndex++)
    {
	Rectangle r1 = *(*declaredGeomIndex);
	Rectangle r2 = *(*viaGeomIndex);
	if (fabs(r1.xMin - r2.xMin) > 1e-4 || fabs(r1.xMax - r2.xMax) > 1e-4 || 
	    fabs(r1.yMin - r2.yMin) > 1e-4 || fabs(r1.yMax - r2.yMax) > 1e-4)
	{
	  cerr << "Mismatch between layer geoms in VIA " << viaName << endl;
	  break;
	} 
    }
    
    // Now we have to clean up as the layerGeoms won't be getting into viaList   
    declaredGeomIndex = layerGeoms.begin();
    for(; declaredGeomIndex != layerGeoms.end(); declaredGeomIndex++)
      delete (*declaredGeomIndex);
  }
  else
  {
    newVia = new dbVia(viaName, -1.0, layerGeoms);
    parserPtr->viaList.push_back(newVia);
    ViaMapEntry viaEntry(strdup(viaName), newVia);
    pair<ViaMapType::iterator, bool> result = parserPtr->viaDictionary.insert(viaEntry);
    if(!result.second)
    {
      cerr << "ParserLEFDEFq::Error inserting into via Dictionary" << endl;
      return 1;
    }
  }

  return 0;
}

int ParserLEFDEFq::HandleDEFViaEnd(defrCallbackType_e typ, void* variable, defiUserData data)  
{
  return 0;
}

void ParserLEFDEFq::printDBLayers(const DB* db)
{
  ctainerLayers::const_iterator index = db->context._layers.begin();
  for(; index != db->context._layers.end(); index++)
  {
    cout << "LAYER: " << (*index)->getName() << endl
	 << "\tLayerType: ";
    PRINTTYPE((*index)->getType());
    cout << endl;
    cout << "\tLayerDirection: ";
    PRINTDIR((*index)->getDirection());
    cout << endl
	 << "\tLayerPitch: " << (*index)->getPitch() << endl
	 << "\tLayerWidth: " << (*index)->getWidth() << endl
	 << "\tLayerHeight: " << (*index)->getHeight() << endl
	 << "\tLayerThickness: " << (*index)->getThickness() << endl
	 << "\tLayerShrinkage: " << (*index)->getShrinkage() << endl
	 << "\tLayerCapMult: " << (*index)->getCapMult() << endl
	 << "\tLayerEdgeCap: " << (*index)->getEdgeCap() << endl
	 << "\tLayerSpacing: " << (*index)->getSpacing() << endl
	 << "\tLayerUnitRes: " << (*index)->getUnitRes() << endl
	 << "\tLayerUnitCap: " << (*index)->getUnitCap() << endl
	 << "\tLayerIndex: " << (*index)->getIndex() << endl;
  }
  cout << endl;
}

void ParserLEFDEFq::printDBSites(const DB* db)
{
  ctainerSites::const_iterator index = db->context._sites.begin();
  for(; index != db->context._sites.end(); index++)
  {
    cout << "SITE " << (*index)->getName() << endl;
    cout << "\tSiteType: ";
    PRINTTYPE((*index)->getType());
    cout << endl;
    cout << "\tSiteSymmetry: X: ";
    PRINTBOOL((*index)->getSymmetry().x); 
    cout << " Y: ";
    PRINTBOOL((*index)->getSymmetry().y); 
    cout << " R90: ";
    PRINTBOOL((*index)->getSymmetry().rot90); 
    cout << endl;
    cout << "\tWidth: " << (*index)->getWidth() << endl;
    cout << "\tHeight: " << (*index)->getHeight() << endl;
  }
  cout << endl;
}

void ParserLEFDEFq::printDBPinLEQs(const DB* db)
{  
  equivClassesMasterPins::const_iterator classIndex = db->design._pinLEQs.begin();
  int classCount;

  for(classCount = 0; classIndex != db->design._pinLEQs.end(); classIndex++, classCount++)
  {
    cout << "CLASS " << classCount << endl;
    eqClassMasterPins::const_iterator pinIndex = (*classIndex).begin();
    for(; pinIndex != (*classIndex).end(); pinIndex++)
    {
      cout << "\tPIN " << (*pinIndex)->getName() << endl; 
    }
  }
}

void ParserLEFDEFq::printDBPinLib(const DB* db)
{
  ctainerMasterPinsGlobal::const_iterator index = db->design._pinLib.begin();
  
  for(; index != db->design._pinLib.end(); index++)
  {
    cout << "PIN " << (*index)->getName() << endl;
    cout << "\tType: " << (*index)->getType() << endl
	 << "\tMasterCell: " << (*index)->getCell().getName() << endl
    	 << "\tCenter: X: " << (*index)->getCenter().x << " Y: " << (*index)->getCenter().y << endl
    	 << "\tBox: " << (*index)->getBBox().xMin << " " << (*index)->getBBox().yMin << " "
    	 << (*index)->getBBox().xMax << " " <<(*index)->getBBox().yMax << endl 
    	 << "\tRiseDelay: " << (*index)->getRiseDelay() << endl
    	 << "\tFallDelay: " << (*index)->getFallDelay() << endl
    	 << "\tRiseResist: " << (*index)->getRiseResist() << endl
    	 << "\tFallResist: " << (*index)->getFallResist() << endl
    	 << "\tInputCap: " << (*index)->getInputCap() << endl;
      cout << "\tShape: ";
      if((*index)->isShapeAbutment())
        cout << "ABUTMENT" << endl;
      else if((*index)->isShapeFeedthrough())
        cout << "FEEDTHROUGH" << endl;
      else if((*index)->isShapeRing())
	cout << "RING" << endl;
      else if((*index)->isShapeUnknown())
	cout << "UNKNOWN" << endl;
      cout << "\tUSE: ";
      if((*index)->isUseSignal())
	cout << "SIGNAL" << endl;
      else if((*index)->isUseAnalog())
	cout << "ANALOG" << endl;
      else if((*index)->isUsePower())
	cout << "POWER" << endl;
      else if((*index)->isUseGround())
	cout << "GROUND" << endl;
      else if((*index)->isUseClock())
      cout << "CLOCK" << endl;
      else if((*index)->isUseUnknown())
	cout << "UNKNOWN" << endl;
      cout << "\tDirection: ";
      if((*index)->isDirOutput())
	cout << "OUTPUT" << endl;
      else if((*index)->isDirInput())
	cout << "INPUT" << endl;
      else if((*index)->isDirInOut())
	cout << "INOUT" << endl;
      else if((*index)->isDirOutputTristate())
	cout << "OUTPUT TRISTATE" << endl;
      else if((*index)->isDirFeedthrough())
	cout << "FEEDTHROUGH" << endl;
      else if((*index)->isDirUnknown())
	cout << "UNKNOWN" << endl;
      cout << "\tMustJoin: ";
      PRINTBOOL((*index)->isJoint());
      cout << endl;
      cout << "\tPORTS: " << endl;
      ctainerLayerGeoms const &geoms = (*index)->getPorts();
      ctainerLayerGeoms::const_iterator geomIndex = geoms.begin();
      for(; geomIndex != geoms.end(); geomIndex++)
      {
	cout << "\t\tLAYER " << (*geomIndex)->getLayer().getName() << endl;
	cout << "\t\t\tIsVia: ";
	PRINTBOOL((*geomIndex)->isVia());
	cout << endl;
	Rectangle rect = *(*geomIndex);
	cout << "\t\t\tRECT: " << rect.xMin << " " << rect.yMin << " "
	     << rect.xMax << " " << rect.yMax << endl;
	cout << "\t\t\tPOLY: ";
	std::vector<Point>::const_iterator pointIndex = (*geomIndex)->getPolygon().begin();
	for(; pointIndex != (*geomIndex)->getPolygon().end(); pointIndex++)
	cout << (*pointIndex).x << "," << (*pointIndex).y << " ";
	cout << endl;
      }
  }
  cout << endl;
}  

void ParserLEFDEFq::printDBCellLib(const DB* db)
{  
  ctainerMasterCells::const_iterator index = db->design._cellLib.begin();

  for(; index != db->design._cellLib.end(); index++)
  {
    char* type = (*index)->getType();
    cout << "MACRO " << (*index)->getName() << endl
         << "\tNumPins: " << (*index)->getNumPins() << endl
         << "\tWidth: " << (*index)->getWidth() << endl
	 << "\tHeight: " << (*index)->getHeight() << endl
	 << "\tInDegree: " << (*index)->getIndegree() << endl
	 << "\tOutDegree: " << (*index)->getOutdegree() << endl
	 << "\tType: " << type << endl
	 << "\tRiseDelay: " << (*index)->getRiseDelay() << endl
	 << "\tFallDelay: " << (*index)->getFallDelay() << endl
	 << "\tSite: " << (*index)->getSite().getName() << endl;
/*	 << "\tSymmetry: " << (*index)->getSymmetry() << endl;

    ctainerLayerGeoms const &geoms = (*index)->getObstacles();
    ctainerLayerGeoms::const_iterator geomIndex = geoms.begin();
    for(; geomIndex != geoms.end(); geomIndex++)
    {
      cout << "\t\tLAYER " << (*geomIndex)->getLayer().getName() << endl;
      cout << "\t\t\tIsVia: ";
      PRINTBOOL((*geomIndex)->isVia());
      cout << endl;
      Rectangle rect = *(*geomIndex);
      cout << "\t\t\tRECT: " << rect.xMin << " " << rect.yMin << " "
	   << rect.xMax << " " << rect.yMax << endl;
      cout << "\t\t\tPOLY: ";
      vector<Point>::const_iterator pointIndex = (*geomIndex)->getPolygon().begin();
      for(; pointIndex != (*geomIndex)->getPolygon().end(); pointIndex++)
	cout << (*pointIndex).x << "," << (*pointIndex).y << " ";
      cout << endl;
    }*/
  }
}

void ParserLEFDEFq::printDBCellLEQs(const DB* db)
{  
  equivClassesMasterCells::const_iterator classIndex = db->design._cellLEQs.begin();
  int classCount;

  for(classCount = 0; classIndex != db->design._cellLEQs.end(); classIndex++, classCount++)
  {
    cout << "CLASS " << classCount << endl;
    eqClassMasterCells::const_iterator cellIndex = (*classIndex).begin();
    for(; cellIndex != (*classIndex).end(); cellIndex++)
    {
      cout << "\tMACRO " << (*cellIndex)->getName() << endl; 
    }
  }
}

void ParserLEFDEFq::printDBCellEEQs(const DB* db)
{  
  equivClassesMasterCells::const_iterator classIndex = db->design._cellEEQs.begin();
  int classCount;

  for(classCount = 0; classIndex != db->design._cellEEQs.end(); classIndex++, classCount++)
  {
    cout << "CLASS " << classCount << endl;
    eqClassMasterCells::const_iterator cellIndex = (*classIndex).begin();
    for(; cellIndex != (*classIndex).end(); cellIndex++)
    {
      cout << "\tMACRO " << (*cellIndex)->getName() << endl; 
    }
  }
}

void ParserLEFDEFq::printDBRows(const DB* db)
{
  ctainerRowsGlobal::const_iterator rowIndex = db->context._rows.begin();

  for(; rowIndex != db->context._rows.end(); rowIndex++)
  {
    cout << "ROW " << (*rowIndex)->getName() << endl
	 << "\tOrigin: " << (*rowIndex)->getOrigin() << endl;
    if((*rowIndex)->getDirection() == dbRow::HORIZ)
      cout << "\tDirection: HORIZ" << endl;
    else if((*rowIndex)->getDirection() == dbRow::VERT)
      cout << "\tDirection: VERT" << endl;
    else
      cerr << "printDBRows: Error, unrecognized direction" << endl;
    cout << "\tSpacing: " << (*rowIndex)->getSpacing() << endl
	 << "\tOrientation: " << (*rowIndex)->getOrientation() << endl
	 << "\tLength: " << (*rowIndex)->getLength() << endl
	 << "\tNumSites: " << (*rowIndex)->getNumSites() << endl;

    const dbSite& tempSite = (*rowIndex)->getSite();
    cout << "\tSITE " << tempSite.getName() << endl;
    cout << "\t\tSiteType: ";
    PRINTTYPE(tempSite.getType());
    cout << endl;
    cout << "\t\tSiteSymmetry: X: ";
    PRINTBOOL(tempSite.getSymmetry().x); 
    cout << " Y: ";
    PRINTBOOL(tempSite.getSymmetry().y); 
    cout << " R90: ";
    PRINTBOOL(tempSite.getSymmetry().rot90); 
    cout << endl;
    cout << "\t\tWidth: " << tempSite.getWidth() << endl;
    cout << "\t\tHeight: " << tempSite.getHeight() << endl;
  }
  cout << endl;
}

void ParserLEFDEFq::printDBCells(const DB* db)
{  
  ctainerCellsGlobal::const_iterator cellIndex = db->design._cells.begin();

  for(; cellIndex != db->design._cells.end(); cellIndex++)
  {
    cout << "CELL " << (*cellIndex)->getName() << endl
	 << "\tMaster: " << (*cellIndex)->getMaster().getName() << endl
	 << "\tNumNets: " << (*cellIndex)->getNumNets() << endl
	 << "\tNumPins: " << (*cellIndex)->getNumPins() << endl
	 << "\tIndex: " << (*cellIndex)->getIndex() << endl
      	 << "\tType: " << (char*)(*cellIndex)->getType() << endl
	 << "\tHeight: " << (*cellIndex)->getHeight() << endl
	 << "\tWidth: " << (*cellIndex)->getWidth() << endl
	 << "\tInDegree: " << (*cellIndex)->getIndegree() << endl
	 << "\tOutDegree: " << (*cellIndex)->getOutdegree() << endl
	 << "\tRiseDelay: " << (*cellIndex)->getRiseDelay() << endl
	 << "\tFallDelay: " << (*cellIndex)->getFallDelay() << endl
	 << "\tTimingNode: ";
    PRINTBOOL((*cellIndex)->isTimingNode());
    cout << endl
	 << "\tLatchNode: ";
    PRINTBOOL((*cellIndex)->isLatchNode());
    cout << endl
	 << "\tIgnoredTimingNode: ";
    PRINTBOOL((*cellIndex)->isIgnoredTimingNode());
    cout << endl;
    if((*cellIndex)->getType().isIO())
    {
      cout << "\tInputPad: ";
      PRINTBOOL((*cellIndex)->isInputPad());
      cout << endl
	   << "\tOutputPad: ";
      PRINTBOOL((*cellIndex)->isOutputPad());
      cout << endl
	   << "\tPins: " << endl;
    }
    ctainerPinsLocal::const_iterator pinIndex = (*cellIndex)->_pins.begin(); 
    for(; pinIndex != (*cellIndex)->_pins.end(); pinIndex++)
    {
      cout << "\t\tPIN " << (*pinIndex)->getMasterPinName() << endl;
    }
    ctainerNetsLocal::const_iterator netIndex = (*cellIndex)->_nets.begin();
    for(; netIndex != (*cellIndex)->_nets.end(); netIndex++)
    {
      cout << "\t\tNET " << (*netIndex)->getName() << endl
	   << "\t\t\tCommonPin: " << (*cellIndex)->commonPin(*netIndex)->getMasterPinName() << endl
	   << "\t\t\tCellInput: ";
      PRINTBOOL((*cellIndex)->commonPinIsCellInput(*netIndex));
      cout << endl
	   << "\t\t\tCellOutput: ";
      PRINTBOOL((*cellIndex)->commonPinIsCellOutput(*netIndex));
      cout << endl;
    }        
  }
}

void ParserLEFDEFq::printDBTracks(const DB* db)
{
  ctainerTracksGlobal::const_iterator trackIndex = db->context._tracks.begin();
  for(; trackIndex != db->context._tracks.end(); trackIndex++)
  {  
    cout << "TRACK" << endl
	 << "\tLayer: " << (*trackIndex)->layer.getName() << endl
	 << "\tStart: " << (*trackIndex)->start << endl
	 << "\tNumTracks: " << (*trackIndex)->numTracks << endl
	 << "\tSpacing: " << (*trackIndex)->spacing << endl
	 << "Horizontal: ";
    PRINTBOOL((*trackIndex)->horizontal);
    cout << endl;
  }
}

void ParserLEFDEFq::printDBPins(const DB* db)
{
  ctainerPinsGlobal::const_iterator pinIndex = db->design._pins.begin();

  cout << "TOTALPINS: " << db->design._pins.size() << endl;

  for(; pinIndex != db->design._pins.end(); pinIndex++)
  {
    cout << "PIN " << (*pinIndex)->getMasterPinName() << endl;
    cout << "\tType: " << (*pinIndex)->getType() << endl
	 << "\tIndex: " << (*pinIndex)->getIndex() << endl
	 << "\tMasterCell: " << (*pinIndex)->getCell().getName() << endl
    	 << "\tRiseDelay: " << (*pinIndex)->getRiseDelay() << endl
    	 << "\tFallDelay: " << (*pinIndex)->getFallDelay() << endl
    	 << "\tRiseResist: " << (*pinIndex)->getRiseResist() << endl
    	 << "\tFallResist: " << (*pinIndex)->getFallResist() << endl
    	 << "\tInputCap: " << (*pinIndex)->getInputCap() << endl
	 << "\tConnected: ";
    PRINTBOOL((*pinIndex)->isConnected());
    cout << endl;
    if((*pinIndex)->isConnected())
      cout << "\t\tNet: " << (*pinIndex)->getNet().getName() << endl;
    cout << "\tShape: ";
    if((*pinIndex)->isShapeAbutment())
      cout << "ABUTMENT" << endl;
    else if((*pinIndex)->isShapeFeedthrough())
      cout << "FEEDTHROUGH" << endl;
    else if((*pinIndex)->isShapeRing())
      cout << "RING" << endl;
    else if((*pinIndex)->isShapeUnknown())
      cout << "UNKNOWN" << endl;
    cout << "\tUSE: ";
    if((*pinIndex)->isUseSignal())
      cout << "SIGNAL" << endl;
    else if((*pinIndex)->isUseAnalog())
      cout << "ANALOG" << endl;
    else if((*pinIndex)->isUsePower())
      cout << "POWER" << endl;
    else if((*pinIndex)->isUseGround())
      cout << "GROUND" << endl;
    else if((*pinIndex)->isUseClock())
      cout << "CLOCK" << endl;
    else if((*pinIndex)->isUseUnknown())
      cout << "UNKNOWN" << endl;
    cout << "\tDirection: ";
    if((*pinIndex)->isDirOutput())
      cout << "OUTPUT" << endl;
    else if((*pinIndex)->isDirInput())
      cout << "INPUT" << endl;
    else if((*pinIndex)->isDirInOut())
      cout << "INOUT" << endl;
    else if((*pinIndex)->isDirOutputTristate())
      cout << "OUTPUT TRISTATE" << endl;
    else if((*pinIndex)->isDirFeedthrough())
      cout << "FEEDTHROUGH" << endl;
    else if((*pinIndex)->isDirUnknown())
      cout << "UNKNOWN" << endl;
  }
}

void ParserLEFDEFq::printDBNets(const DB* db)
{
  ctainerNetsGlobal::const_iterator netIndex = db->design._nets.begin();

  cout << "TOTALNETS: " << db->design._nets.size() << endl;

  for(; netIndex != db->design._nets.end(); netIndex++)
  {
    cout << "NET " << (*netIndex)->getName() << endl
	 << "\tType: " << (char*)(*netIndex)->getType() << endl
	 << "\tNumCells: " << (*netIndex)->getNumCells() << endl
	 << "\tNumPins: " << (*netIndex)->getNumPins() << endl
	 << "\tDegree: " << (*netIndex)->getDegree() << endl
	 << "\tIndex: " << (*netIndex)->getType() << endl
	 << "\tNumOut: " << (*netIndex)->getNumOut() << endl
	 << "\tNumInOut: " << (*netIndex)->getNumInOut() << endl
	 << "\tNumIn: " << (*netIndex)->getNumIn() << endl
	 << "\tHasLogicalPin: " << (*netIndex)->hasLogicalPin() << " ";
    // Unsure because it doesn't return a bool, but a unsigned int
    // so i print both
    PRINTBOOL((*netIndex)->hasLogicalPin());
    cout << endl;
    cout << "\tCELLS: " << endl;
    ctainerCellsLocal::iterator cellIndex = (*netIndex)->_cells.begin();
    for(; cellIndex != (*netIndex)->_cells.end(); cellIndex++)
    {
      cout << "\t\t" << (*cellIndex)->getName() << endl;
      cout << "\t\t\tCommonPin: " << (*netIndex)->commonPin((*cellIndex))->getMasterPinName() << endl;
      cout << "\t\t\tCommonPinInput: ";
      PRINTBOOL((*netIndex)->commonPinIsCellInput((*cellIndex)));
      cout << endl;
      cout << "\t\t\tCommonPinOutput: ";
      PRINTBOOL((*netIndex)->commonPinIsCellOutput((*cellIndex)));
      cout << endl;
      cout << "\t\t\tInputCap: " << (*netIndex)->getInputCap((*cellIndex)) << endl
	   << "\t\t\tDriverResist: " << (*netIndex)->getDriverResist((*cellIndex)) << endl;
    }
    cout << "\tPINS: " << endl;
    ctainerPinsLocal::iterator pinIndex = (*netIndex)->_pins.begin();
    for(; pinIndex != (*netIndex)->_pins.end(); pinIndex++)
    {
      cout << "\t\t" << (*pinIndex)->getMasterPinName() << endl;
    }
  }
  cout << endl;
}

void ParserLEFDEFq::printDBSpecialNets(const DB* db)
{
  ctainerNetsGlobal::const_iterator netIndex = db->design._sNets.begin();

  cout << "TOTALSPECIALNETS: " << db->design._sNets.size() << endl;

  for(; netIndex != db->design._sNets.end(); netIndex++)
  {
    cout << "SPECIAL NET " << (*netIndex)->getName() << endl
	 << "\tType: " << (char*)(*netIndex)->getType() << endl
	 << "\tNumCells: " << (*netIndex)->getNumCells() << endl
	 << "\tNumPins: " << (*netIndex)->getNumPins() << endl
	 << "\tDegree: " << (*netIndex)->getDegree() << endl
	 << "\tIndex: " << (*netIndex)->getType() << endl
	 << "\tNumOut: " << (*netIndex)->getNumOut() << endl
	 << "\tNumInOut: " << (*netIndex)->getNumInOut() << endl
	 << "\tNumIn: " << (*netIndex)->getNumIn() << endl
	 << "\tHasLogicalPin: " << (*netIndex)->hasLogicalPin() << " ";
    // Unsure because it doesn't return a bool, but a unsigned int
    // so i print both
    PRINTBOOL((*netIndex)->hasLogicalPin());
    cout << endl;
    cout << "\tCELLS: " << (*netIndex)->_cells.size() << endl;
    ctainerCellsLocal::iterator cellIndex = (*netIndex)->_cells.begin();
    for(; cellIndex != (*netIndex)->_cells.end(); cellIndex++)
    {
      cout << "\t\t" << (*cellIndex)->getName() << endl;
      cout << "\t\t\tCommonPin: " << (*netIndex)->commonPin((*cellIndex))->getMasterPinName() << endl;
      cout << "\t\t\tCommonPinInput: ";
      PRINTBOOL((*netIndex)->commonPinIsCellInput((*cellIndex)));
      cout << endl;
      cout << "\t\t\tCommonPinOutput: ";
      PRINTBOOL((*netIndex)->commonPinIsCellOutput((*cellIndex)));
      cout << endl;
      cout << "\t\t\tInputCap: " << (*netIndex)->getInputCap((*cellIndex)) << endl
	   << "\t\t\tDriverResist: " << (*netIndex)->getDriverResist((*cellIndex)) << endl;
      cout << "\t\t\tType: " << (char*)(*cellIndex)->getType() << endl
	   << "\t\t\tIndex: " << (*cellIndex)->getIndex() << endl
	   << "\t\t\tHeight: " << (*cellIndex)->getHeight() << endl
	   << "\t\t\tWidth: " << (*cellIndex)->getWidth() << endl
	   << "\t\t\tIndegree: " << (*cellIndex)->getIndegree() << endl
	   << "\t\t\tOutdegree: " << (*cellIndex)->getOutdegree() << endl
	   << "\t\t\tNumNets: " << (*cellIndex)->getNumNets() << endl
	   << "\t\t\tNumPins: " << (*cellIndex)->getNumPins() << endl
	   << "\t\t\tRiseDelay: " << (*cellIndex)->getRiseDelay() << endl
	   << "\t\t\tFallDelay: " << (*cellIndex)->getFallDelay() << endl
	   << "\t\t\tTimingNode: ";
      PRINTBOOL((*cellIndex)->isTimingNode());
      cout << endl
	   << "\t\t\tLatchNode: ";
      PRINTBOOL((*cellIndex)->isLatchNode());
      cout << endl
	   << "\t\t\tIgnoredTimingNode: ";
      PRINTBOOL((*cellIndex)->isIgnoredTimingNode());
      cout << endl;
      if((*cellIndex)->getType().isIO())
      {
	cout << "\t\t\tInputPad: ";
	PRINTBOOL((*cellIndex)->isInputPad());
	cout << endl
	     << "\t\t\tOutputPad: ";
	PRINTBOOL((*cellIndex)->isOutputPad());
	cout << endl
	     << "\t\t\tPins: " << endl;
      }
    }
    cout << "\tPINS: " << (*netIndex)->_pins.size() << endl;
    ctainerPinsLocal::iterator pinIndex = (*netIndex)->_pins.begin();
    for(; pinIndex != (*netIndex)->_pins.end(); pinIndex++)
    {
      cout << "\t\t" << (*pinIndex)->getMasterPinName() << endl;
      cout << "\t\t\tType: " << (char*)(*pinIndex)->getType() << endl;
      cout << "\t\t\tDirection: ";
      if((*pinIndex)->isDirOutput())
	cout << "OUTPUT" << endl;
      else if((*pinIndex)->isDirInput())
	cout << "INPUT" << endl;
      else if((*pinIndex)->isDirInOut())
	cout << "INOUT" << endl;
      cout << "\t\t\tCell: " << (*pinIndex)->getCell().getName() << endl;
    }
    ctainerLayerGeoms::iterator wiringIndex = (*netIndex)->_wiring.begin();
    for(; wiringIndex != (*netIndex)->_wiring.end(); wiringIndex++)
    {
      cout << "\t\tLAYER " << (*wiringIndex)->getLayer().getName() << endl;
      cout << "\t\t\tIsVia: ";
      PRINTBOOL((*wiringIndex)->isVia());
      cout << endl;
      Rectangle rect = *(*wiringIndex);
      cout << "\t\t\tRECT: " << rect.xMin << " " << rect.yMin << " "
	   << rect.xMax << " " << rect.yMax << endl;
      cout << "\t\t\tPOLY: ";
      std::vector<Point>::const_iterator pointIndex = (*wiringIndex)->getPolygon().begin();
      for(; pointIndex != (*wiringIndex)->getPolygon().end(); pointIndex++)
	cout << (*pointIndex).x << "," << (*pointIndex).y << " ";
      cout << endl;    
    }
  }
  cout << endl;
}

void ParserLEFDEFq::printDBSpatial(const DB* db)
{
  cout << "SPATIAL" << endl
       << "\tAllCellsPlaced: ";
  PRINTBOOL(db->spatial.allCellsPlaced());
  cout << endl
       << "\tIsConsistent: ";
  PRINTBOOL(db->spatial.isConsistent());
  cout << endl;
  cout << db->spatial.locations << endl;
  cout << "\tSize: " << db->spatial.getSize() << endl;
  for(unsigned int numLocs=0; numLocs < db->spatial.getSize(); numLocs++)
  {
    cout << "\tLocationValid[" << numLocs << "] = ";
    PRINTBOOL(db->spatial.locationValid[numLocs]);
    cout << endl;
    cout << "\tOrientation[" << numLocs << "] = " << db->spatial.orientations[numLocs] << endl;
  }
  cout << db->spatial.constraints << endl;
}
