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





#ifndef PARSERLEFDEFQ_H
#define PARSERLEFDEFQ_H

#include "lefrReader.hpp"
#include "lefwWriter.hpp"
#include "defrReader.hpp"
#include "defiAlias.hpp"
#include "DB/DB.h"
#include <ABKCommon/sgi_hash_map.h>

#define HAVE_Q_PARSER

struct eqstr
{
  bool operator()(char const *s1,char const *s2) const
  {
    return (strcmp(s1, s2) == 0);
  }
};

class BBoxExtended : public BBox
{
 public:
  BBoxExtended() : BBox() {}
  BBoxExtended(double xMinP, double yMinP, double xMaxP, double yMaxP) : 
    BBox(xMinP,yMinP,xMaxP,yMaxP) {}
  BBox &snip(BBox& overlapping)
    {
      bool ll,lr,ul,ur;
      ll=overlapping.contains(xMin,yMin);
      lr=overlapping.contains(xMax,yMin);
      ul=overlapping.contains(xMin,yMax);
      ur=overlapping.contains(xMax,yMax);
      
      if (ll && lr && !ul)
        yMin = overlapping.yMax;
      else if (ll && ul && !lr)
        xMin = overlapping.xMax;
      else if (lr && ur && !ll)
        xMax = overlapping.xMin;
      else if (ul && ur && !ll)
        yMax = overlapping.yMin;
      return (*this);
    }
};

struct parserRegion
{
  vector<BBox> boxes;
  bool         bSoft;
  int          maxX;
  int          maxY;
  int          maxHalfPerim;
  unsigned     index;
  
  parserRegion(unsigned idx, bool soft = false, int mx = INT_MAX,
	       int  my = INT_MAX, int  mhp = INT_MAX) :
               bSoft(soft), maxX(mx), maxY(my), maxHalfPerim(mhp), index(idx) {}
  
    //next constructor is used *only* in GROUPS section when
    //the SOFT keyword is specified together with a region.  Then
    //one looks up the named region and copies its parameters -- but
    //since you can't specify "soft" in the regions section, we have
    //to make a new one with different mx,my,mhp
    parserRegion(unsigned idx, parserRegion const & pr, int mx, int my, int mhp) : 
                 boxes(pr.boxes), bSoft(true), maxX(mx), maxY(my), maxHalfPerim(mhp),
                 index(idx) {};
};

struct CompareLayerGeoms
{
  bool operator()(dbLayerGeom const *lg1,dbLayerGeom const *lg2)
  {
    unsigned idx1 = lg1->getLayer().getIndex();
    unsigned idx2 = lg2->getLayer().getIndex();
    if ( idx1 < idx2 )
      return true;
    else if (idx1 > idx2 )
      return false;
    else 
    {
      Rectangle const &r1 = *lg1;
      Rectangle const &r2 = *lg2;
      if (r1.xMin < r2.xMin) return true;
      else if (r1.xMin > r2.xMin) return false;
      else if (r1.yMin < r2.yMin) return true;
      else if (r1.yMin > r2.yMin) return false;
      else if (r1.xMax < r2.xMax) return true;
      else if (r1.xMax > r2.xMax) return false;
      else if (r1.yMin < r2.yMin) return true;
      else if (r1.yMax > r2.yMax) return false;
    }
    return false;
  }
};

struct CompPairsByRegion
{
  bool operator()(pair<dbCell*, parserRegion*> const &p1, pair<dbCell*, parserRegion*> const &p2)
  {
    if (p1.second->index < p2.second->index) return true;
    else return false;
  }
};

class dbVia
{
  MakeFriendsWithParsers
    public:
  dbVia(char const *name, double resis, ctainerLayerGeoms const &geoms): 
    _resis(resis), 
    _geoms(geoms)
  {
    _name = new char[strlen(name)+1];
    strcpy(_name,name);
  }
  ~dbVia()
  {
    delete [] _name;
    ctainerLayerGeoms::iterator iLG;
    for (iLG=_geoms.begin();iLG!=_geoms.end();iLG++)
      delete (*iLG);
  }
  
  double                  getResistance()  const {return _resis;}
  char const             *getName ()       const {return _name;}
  ctainerLayerGeoms const &getGeometries() const {return _geoms;}
  
 private:
  char             *_name;
  double            _resis;
  ctainerLayerGeoms _geoms; //note carefully: the rectangles
  //in these geometries are *offsets*
  //from a point.
  
};

class ParserLEFDEFq
{
 private:
  typedef std::pair<const char*, dbLayer*>                                     LayerMapEntry;
  typedef hash_map<const char*, dbLayer*, hash<const char*>, eqstr>       LayerMapType;
  typedef std::pair<const char*, dbVia*>                                       ViaMapEntry;
  typedef hash_map<const char*, dbVia*, hash<const char*>, eqstr>         ViaMapType;
  typedef std::pair<const char*, dbSite*>                                      SiteMapEntry;
  typedef hash_map<const char*, dbSite*, hash<const char*>, eqstr>        SiteMapType;
  typedef std::pair<const char*, dbMasterCell*>                                MasterCellMapEntry;
  typedef hash_map<char const*,dbMasterCell*, hash<const char*>, eqstr>   MasterCellMapType;
  typedef std::pair<const char*, dbCell*>                                      CellMapEntry;
  typedef hash_map<char const*, dbCell*, hash<const char*>, eqstr>        CellMapType;
  typedef std::pair<const char*, dbNet*>                                       NetMapEntry;
  typedef hash_map<char const*, dbNet*, hash<const char*>, eqstr>         NetMapType;
  typedef std::pair<const char*, parserRegion*>                                RegionMapEntry;
  typedef hash_map<char const*, parserRegion*, hash<const char*>, eqstr>  RegionMapType;

  typedef ctainerMasterPinsLocal                                               MacroPinListType;
  typedef ctainerLayerGeoms                                                    MacroObsType;
  typedef ctainerRowsGlobal                                                    RowListType;
  typedef ctainerTracksGlobal                                                  TrackListType;
  typedef equivClassesMasterCells                                              MacroEQListType;
  typedef std::vector<dbVia*>                                                  ViaListType;
  typedef bit_vector                                                           ValidListType;
  typedef ctainerPinsGlobal                                                    PinListType;
  typedef ctainerCellsGlobal                                                   CellListType;
  typedef std::vector<Orient>                                                  OrientationListType;
  typedef std::vector<Point>                                                   LocationListType;
  typedef ctainerNetsGlobal                                                    NetListType;
  typedef std::vector<parserRegion*>                                           RegionListType;
  typedef std::pair<dbCell*, parserRegion*>                                    CompRegionEntry;
  typedef std::vector<CompRegionEntry>                                         CompRegionListType;

  // Note that E and W are flipped in comparison with the Cadence 
  // LEF/DEF parser. For some reason, the east and west are flipped
  // than what is documented
  enum OrientationEnum {N, W, S, E, FN, FE, FS, FW};

  void processRegions(ParserLEFDEFq &p);

 public:
  ParserLEFDEFq(DB& db, const char *lname, const char *dname);
  ~ParserLEFDEFq();

  static void printDBLayers(const DB* db);
  static void printDBSites(const DB* db);
  static void printDBPinLib(const DB* db);
  static void printDBPinLEQs(const DB* db);
  static void printDBCellLib(const DB* db);
  static void printDBCellLEQs(const DB* db);
  static void printDBCellEEQs(const DB* db);
  static void printDBRows(const DB* db);
  static void printDBTracks(const DB* db);
  static void printDBPins(const DB* db);
  static void printDBCells(const DB* db);
  static void printDBNets(const DB* db);
  static void printDBSpecialNets(const DB* db);
  static void printDBSpatial(const DB* db);

  #include "LEFcallbacks.h"
  #include "DEFcallbacks.h"

 private:
  void lefparse(const char* lname);
  void defparse(const char* dname);

  void dump();
  int DBconvertUnit(double value);

  LayerMapType      layerDictionary;
  ViaMapType        viaDictionary;
  SiteMapType       siteDictionary;
  MasterCellMapType macroDictionary;

  CellMapType         cellDictionary;
  NetMapType          netDictionary;
  NetMapType          specialNetDictionary;
  RegionMapType       regionDictionary;
  
  // Pin list is used to communicate between the pin callback and the macro callback
  // Sometimes, the macro maybe be ignored, in which case, we don't want to add pins
  MacroPinListType    macroPinList;
  MacroObsType        macroObs;
  ViaListType         viaList;
  MacroEQListType     macroLEQList;
  MacroEQListType     macroEEQList;
  ValidListType       locationsValidList;

  RowListType         rowList;
  PinListType         pinList;
  CellListType        cellList;
  TrackListType       trackList;
  OrientationListType orientList;
  LocationListType    locationList;
  NetListType         specialNetList;
  NetListType         netList;
  RegionListType      regionList;
  CompRegionListType  compRegionList;

  std::vector<unsigned>    fixedCompIndices;
    
  dbSite* externalPinSite;

  DB&  uclaDB;
  bool caseSensitiveLEF;
  bool caseSensitiveDEF;
  int  layerCount;
  int  netCount;
  int  specialNetCount;
  int  regionCount;

  // We need these to communicate between the different DEF Group
  // callbacks.
  std::vector<dbCell*> tempCompList;
}; 
 
#endif

