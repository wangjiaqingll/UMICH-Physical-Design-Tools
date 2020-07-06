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











//Created 970718 by Igor Markov, ABKGROUP VLSI CAD UCLA

#ifndef _PARSERLEFDEF_H_
#define _PARSERLEFDEF_H_

#include "DB/DB.h"

#ifdef _MSC_VER
#pragma warning (disable:4786)
#include <malloc.h>
#endif

#include <utility>
#include <ABKCommon/sgi_hash_map.h>
#include "Geoms/plGeom.h"
#include "localhash.h"

// The reason for the existence of the following struct, which has
// less functionality than a straight BBox, is that it must be
// included in a union for Bison purposes.  You can't put something
// in a union if it has a default ctor, which BBox does.

struct BBoxSimple
    {
    double xMin;
    double yMin;
    double xMax;
    double yMax;

    operator BBox()
        {
        BBox b(xMin,yMin,xMax,yMax);
        return b;
        }
    };

class BBoxExtended : public BBox
    {
    public:
        BBoxExtended():BBox()
            {
            }
        BBoxExtended(double xMinP,double yMinP,
                     double xMaxP,double yMaxP):BBox(xMinP,yMinP,xMaxP,yMaxP)
            {
            }
    // this function "snips off" the part of the current
    // BBox that is covered by "overlapping", provided that
    // what is left is still a rectangle.
        BBox &snip(BBox& overlapping);
    };



struct parserRegion
    {
    vector<BBox> boxes;
    bool         bSoft;
    int          maxX;
    int          maxY;
    int          maxHalfPerim;
    unsigned     index;
    bool         utilConstraint;
    double       utilization;

    parserRegion(unsigned idx,
                 bool utilConstr=false,
		 double util = DBL_MAX,
                 bool soft=false,
                 int  mx  =INT_MAX,
                 int  my  =INT_MAX,
                 int  mhp =INT_MAX):bSoft(soft),
                                    maxX(mx),
                                    maxY(my),
                                    maxHalfPerim(mhp),
                                    index(idx),
	                            utilConstraint(utilConstr),
                                    utilization(util)
                                    {}

    //next constructor is used *only* in GROUPS section when
    //the SOFT keyword is specified together with a region.  Then
    //one looks up the named region and copies its parameters -- but
    //since you can't specify "soft" in the regions section, we have
    //to make a new one with different mx,my,mhp
    parserRegion(unsigned             idx,
                 parserRegion const & pr,
                 int                  mx,
                 int                  my,
                 int                  mhp):boxes(pr.boxes), bSoft(true),
                                           maxX(mx),
                                           maxY(my),
                                           maxHalfPerim(mhp),
                                           index(idx),
                                           utilConstraint(pr.utilConstraint),
                                           utilization(pr.utilization){};
    };

typedef dbCell *pDbCell;
typedef parserRegion *pParserRegion;
typedef pair<pDbCell,pParserRegion> compRegionPair;

struct CompPairsByRegion
    {
    bool operator()(compRegionPair const &p1,compRegionPair const &p2)
        {
        if (p1.second->index < p2.second->index) return true;
        else return false;
        }
    };

class dbVia
    {
    MakeFriendsWithParsers
    public:
        dbVia(char const             *name,
              double                  resis,
              ctainerLayerGeoms const &geoms): _resis(resis), _geoms(geoms)
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


// The class below declared as friend by many classes in the DB package
// (see the MakeFriendsWithParsers in dbDefines.h)
// If should probably be instantiated inside DB::parseLEFDEF()
//  (see Parser/DBreadLEFDEF.cc).
//   Among other uses, this class can accomodate the first pass for a 
// multipass LEFDEF reader, setting up a parsing tree, computing 
// numbers of components (e.g. for preallocation), supporting 
// global buffering etc.

class ParserLEFDEF
    {
    friend int lefparse();
    friend int defparse();
    friend void processRegions(ParserLEFDEF &p);

    DB& _db; // ParserXXX knows which DB object instantiated it
            // and populates its members

    ctainerCellsGlobal tempAllCells;
    ctainerNetsGlobal  tempAllNets;
    ctainerNetsGlobal  tempAllSNets;
    ctainerPinsGlobal  tempAllPins;
    ctainerRowsGlobal  tempAllRows;
    ctainerTracksGlobal tempAllTracks;
    vector<Point>      tempAllLocs;
    vector<Orient>     tempAllOrients;
    vector<dbVia*>     tempAllVias;
    bit_vector         tempAllLocsValid;
    equivClassesMasterCells tempCellLEQs;
    equivClassesMasterCells tempCellEEQs;
    vector<unsigned>   fixedCompIndices;
    vector<char*>      stringsToClean;
                   //regions for components
    vector<compRegionPair> compRegions; 

    typedef hash_map<char const*,dbLayer*,localhash, eqstr> layerHashMapType;
    layerHashMapType layerHash;

    typedef hash_map<char const*,dbSite*,localhash, eqstr> siteHashMapType;
    siteHashMapType siteHash;

    typedef hash_map<char const*,dbMasterCell*,localhash, eqstr> 
                                                     masterCellHashMapType;
    masterCellHashMapType masterCellHash;

    typedef hash_map<char const*,dbCell*,localhash, eqstr> cellHashMapType;
    cellHashMapType cellHash;

    typedef hash_map<char const*,dbNet*,localhash, eqstr> netHashMapType;
    netHashMapType netHash;
    netHashMapType SnetHash;

    typedef hash_map<char const*,parserRegion*,localhash,eqstr> regionHashMapType;
    regionHashMapType regionHash;

    typedef hash_map<char const*,dbVia*,localhash,eqstr> viaHashMapType;
    viaHashMapType viaHash;

    vector<parserRegion*> regions; // the regions we have allocated
                                   // keep this for cleanup purposes

    dbSite         *pExtPinSite;
    /*
    bool           bIncludeLogicalPins; //if this is set, include
                                        //logical external pins in
                                        //netlist

    bool           bIncludeSpecialPins; //if this and bIncludeLogical
                                        //pins are *both* set,
                                        //include external "special" pins
    */

    public:
            // a typical instatiation in DB::parseLEFDEF() will be
            // ParserLEFDEF(*this /*, other params */);
    ParserLEFDEF(DB& db,
                 const char *lname, 
                 const char *dname);
                 /*,
                 bool  includeLogicalPins=true,
                 bool  includeSpecialPins=false);
                 */

    ~ParserLEFDEF();

    };

class IndexedPt
    {
    friend int defparse();

      Point     pt;
      unsigned  idx;

      IndexedPt(Point p,unsigned n):pt(p),idx(n) {}
    public:
      IndexedPt():pt(0,0),idx(0) {}

      bool operator<(const IndexedPt& npt)  const
          { 
          if (idx < npt.idx) return true;
          if (idx > npt.idx) return false;
          if (pt  <  npt.pt) return true;
          return false;
          }
    };


void processRegions(ParserLEFDEF &p);

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

//#ifdef _MSC_VER
//#pragma warning (default:4786)
//#endif

#endif 

