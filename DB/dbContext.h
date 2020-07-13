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









// Created 970711 by Igor Markov and Andy Caldwell, VLSI CAD ABKGROUP, UCLA/CS

//   CHANGES

// 970729 ilm  deleted dbLayer::_startCoord. dbLayer::_numTracks will 
//             eventually be set by DB::postProcessor(), but parsers can
//             put someinfo there for postprocessor (I guess, 
//             we'll standardize what exactly after the first parser works ;-).
// 970731 ilm  added missing dbLayout::getNumRows() const
// 970804 ilm  dbLayer ctor now sets _height; old ctor is still there
//                                for compatibility, but will be deleted soon
//             parsers are not friends to dbLayer since ctor does everything
// 970806 ilm  the old dbLayer ctor is gone
// 970807 ilm  added row/site/layer accessors to class dbContext
// 970814 ilm  moved implementations into dbContext.cxx
// 970820 ilm  added copy-ctor and assignment with abkfatals to dbContext
// 970903 ilm  added dbRow::getLength()
// 970904 ilm  added class dbTrack
// 970915 aec  added rowsBegin, rowsEnd()
// 971210 mro  made parsers friends with dbLayerGeom

//======================================================================
//                          Context Package
//====================================================================== 

#ifndef _DBCONTEXT_H_
#define _DBCONTEXT_H_

#include <vector>
using std::vector;
#include "ABKCommon/abkcommon.h"
#include "Geoms/plGeom.h"
#include "Placement/symOri.h"          // Symmetry and Orientation classes
#include "dbDefines.h"
#include "dbTypes.h"

#ifdef _MSC_VER
#pragma warning(disable:4800)
#endif

class dbContext;
class dbLayerGeom;
class dbSite;
class dbLayer;
class dbRow;
class dbTrack;
class dbGCellGrid;

typedef dbContext dbLayout; // ilm: even though dbLayout will never be used

//========================== CLASS INTERFACES =============================
// for Consistency check, 
// do we want to make sure all dbLayerGeom's are not empty ?

class dbContext            // stores all "layout environment" information
{
    // parser will directly populate most of the below
    MakeFriendsWithParsers // recommendation: create the vectors and then 
                               // assign them to private fields of dbContext;
    
    
    unsigned               _units; // from UNITS DATABASE MICRONS in LEF
                                   // must match UNITS DISTANCE MICRONS in
                                   // DEF.
    BBox                   _bbox;
    ctainerSites           _sites;  
    ctainerLayers          _layers;
    ctainerRowsGlobal      _rows;
    ctainerTracksGlobal    _tracks;
    ctainerGCellGridsGlobal _gcellGrids;

    double                 _minFeatureX;   
    double                 _minFeatureY;   
    
    public:

        dbContext() {}
        // will be populated by friend class parser
        ~dbContext();
        // copy-ctor and assignment are prohibited: see below

        double getMinFeatureX() const   { return _minFeatureX; }
        double getMinFeatureY() const   { return _minFeatureY; }
        unsigned     getUnits() const   { return _units;       }
        
        BBox     getBBox()      const   { return _bbox;        }
        
        unsigned getNumSites()  const   { return _sites.size();  }
        unsigned getNumLayers() const   { return _layers.size(); }
        unsigned getNumRows()   const   { return _rows.size();   }
        unsigned getNumTracks() const   { return _tracks.size();  }
        
        itSite beginSites()   const { return _sites.begin(); }
        itSite endSites()     const { return _sites.end();   }

        const dbSite& getSiteByIdx(unsigned i)
        { abkassert(i<_sites.size()," Index out of range ");
          return *_sites[i];
        }
                
        itLayer beginLayers() const { return _layers.begin();}
        itLayer endLayers()   const { return _layers.end();  }
  
        const dbLayer& getLayerByIdx(unsigned i)
        { abkassert(i<_layers.size()," dbLayer index out of range ");
          return *_layers[i];
        }

        itRowGlobal	rowsBegin()   const { return _rows.begin();}
	itRowGlobal	rowsEnd()     const { return _rows.end();  }	
        const dbRow& getRowByIdx(unsigned i) const // pat 97.8.17
        { abkassert(i<_rows.size()," dbRow index out of range ");
          return *_rows[i];
        }

        itTrackLocal  beginTracks() const { return _tracks.begin(); }
        itTrackLocal  endTracks()   const { return _tracks.end();   }
        const ctainerTracksGlobal&  getTracks() const { return _tracks; }
        const dbTrack& getTrackByIdx(unsigned i) const 
        {
           abkassert(i<_tracks.size(), "dbTrack index out of range");
           return *_tracks[i];
        }
        unsigned getNumHorizTracks(const BBox& rect) const;
        unsigned getNumHorizTracks(const Point& p1, const Point& p2) const
        {
            BBox box(p1);
            box += p2;
            return getNumHorizTracks(box);
        }
        unsigned getNumVertTracks(const BBox& rect) const;
        unsigned getNumVertTracks(const Point& p1, const Point& p2) const
        {
            BBox box(p1);
            box += p2;
            return getNumVertTracks(box);
        }
                
        itGCellGridLocal  beginGCellGrids() const
            { return _gcellGrids.begin(); }
        itGCellGridLocal  endGCellGrids()   const
            { return _gcellGrids.end();   }
        const ctainerGCellGridsGlobal&  getGCellGrids() const
            { return _gcellGrids; }
        void addGCellGrid(bool horiz, double start,
                          unsigned nGrids, double space);
/*
        const dbGCellGrid getGCellGridByIdx(unsigned i) const 
        {
           abkassert(i<_gcellGrids.size(), "dbGCellGrid index out of range");
           return *_gcellGrids[i];
        }
*/

        bool    isConsistent()  const;

public: // this is what you can't do  --
        // default copy-ctor and assignment are wrong

        dbContext(const dbContext&) 
                 { abkfatal(0,"Can't copy-construct dbContext"); }
        dbContext& operator=(const dbContext&)
                 { abkfatal(0,"Can't assign dbContext"); return *this; }

};

//=======================================================================

class dbLayerGeom
{
MakeFriendsWithParsers

    const dbLayer &_layer;   //    i.e. where it lives (?)
    Rectangle _rect;       //    ilm: Rectangle is typedef from BBox
    //    same members as in dbRect (xMin,yMin etc)
    //    plus additinal functionality like
    //    += Point, contains(Point) etc (see plGeom.h)

    vector<Point> _polygon;

    char      _isVia:1;
 public:
   dbLayerGeom(const Rectangle& rect, dbLayer const &layer, bool isVia=false);
 
   dbLayerGeom(const vector<Point>& polygon, dbLayer const &layer,
       bool isVia=false);

    operator Rectangle()      const { return _rect; } 
          // ilm: if you need to change
          // the rectangle, add operator Rectangle&()
    const dbLayer  &getLayer() const  { return _layer; }
    const vector<Point> &getPolygon() const {return _polygon; }
    bool            isVia()    const  { return _isVia; }
};

//======================================================================


class dbSite
{
public:
    enum Type { PAD, CORE};
private:
    MakeFriendsWithParsers

    char*      _name;
    Type       _type;
    Symmetry   _symmetry; // any subset of 8 available symmetries
    //note: this gives which of the 8 possible orientations
    //are legal, but should this not be in Masters?
    //don't the masters have limits on how you can rotate/flip them?
    
    double      _height;
    double      _width; 

    dbSite(const dbSite&)
             { abkfatal(0,"Can't copy-construct dbSite"); }
    dbSite &operator=(const dbSite&)
             { abkfatal(0,"Can't assign dbSite");return *this;}
    
    public:
        
     dbSite(const char* name, Type type, Symmetry sym, double ht, double wth) 
            :_type(type), _symmetry(sym), _height(ht), _width(wth)
            {_name = new char[strlen(name)+1];strcpy(_name,name);}
        
        ~dbSite()   { delete[] _name; }
        
        const char*     getName()const     { return _name; }
        dbSite::Type    getType()const     { return _type; }
        Symmetry        getSymmetry()const { return _symmetry; }
        double  getHeight()const   { return _height; }
        double  getWidth()const    { return _width; }   
        
    };

//======================================================================

class dbLayer
{
//     MakeFriendsWithParsers
public:
    enum Type { MASTERSLICE, CUT, OVERLAP, ROUTING};
    enum Dir  { HORIZ, VERT, NOPREFERRED};
private:

    char*       _name;
    unsigned    _index;
    Type        _type;
    Dir         _direction;

    // the following six fields should be stored in DEF units
    // i.e. value from LEF in microns should be multiplied by
    // DB::context._units
    unsigned    _pitch;
    unsigned    _height;
    unsigned    _width;
    unsigned    _thickness;
    unsigned    _spacing;
    unsigned    _shrinkage;

    double      _capMult;
    double      _edgeCap;
    double      _unitRes;      // ohm/micron
    double      _unitCap;      // pF/micron

    // ILM: this will be set by DB::postProcessor(), but parsers can put
    //      info here
    unsigned    _numTracks; 
    //(Note: it's an error to have tracks info when the direction is "neither")

    dbLayer(const dbLayer &){abkfatal(0,"can't copy-ctruct dbLayer");}
    dbLayer &operator=(const dbLayer &)
        {abkfatal(0,"can't assign dbLayer");return *this;}
    
    public:
        dbLayer(const char* name, unsigned index, Type type, Dir dir, 
                unsigned pitch, unsigned height, unsigned width, 
                unsigned thickness, unsigned spacing, unsigned shrinkage,
                double capMult, double edgeCap, double unitRes, double unitCap):
            _index(index), _type(type), _direction(dir), _pitch(pitch), 
            _height(height), _width(width), _thickness(thickness), 
            _spacing(spacing), _shrinkage(shrinkage), _capMult(capMult),
            _edgeCap(edgeCap), _unitRes(unitRes), _unitCap(unitCap)
             { 
                _name = new char[strlen(name)+1];  
                strcpy(_name,name);
             }

// the old ctor is gone -ILM
        
        ~dbLayer(){ delete[] _name;}
        
        const char   *getName()     const {return _name;     }
        dbLayer::Type getType()     const {return _type;     }
        dbLayer::Dir getDirection() const {return _direction;}
        unsigned     getPitch()     const {return _pitch;    }
        unsigned     getHeight()    const {return _height;   }
        unsigned     getThickness() const {return _thickness;}
        unsigned     getShrinkage() const {return _shrinkage;}
        double       getCapMult()   const {return _capMult;  }
        double       getEdgeCap()   const {return _edgeCap;  }
        unsigned     getWidth()     const {return _width;    }
        unsigned     getSpacing()   const {return _spacing;  }
        double       getUnitRes()   const {return _unitRes;  }
        double       getUnitCap()   const {return _unitCap;  }
        unsigned     getNumTracks() const {return _numTracks; }
        unsigned     getIndex()     const {return _index;     }
        
    };

//=======================================================================

// for meanings of fields in dbRow, see LEF/DEF manual 5.0, p 6-17
// if using the DEF SITE stmt instead, everything is the same except
// the name should be an empty string.
class dbRow
{
   MakeFriendsWithParsers

    public:    
      enum Dir  {HORIZ,VERT};
    private:

    dbSite          & _site; //this site has name rowType in DEF ROW stmt 
    
    Point             _origin;     // (origX,origY) in DEF ROW stmt
    Dir               _direction;  // horizontal or vertical row

    unsigned          _spacing;  // spaceX or spaceY in DEF ROW stmt 
                                 // (use DEF units)

    Orientation       _orient;   // orient in DEF ROW stmt
    unsigned          _numSites; // numX or numY in DEF ROW stmt
    char            * _name;     // rowName in DEF ROW stmt

    dbRow(const dbRow& from):_site(from._site)
             { abkfatal(0,"Can't copy-construct dbRow"); }
    dbRow &operator=(const dbRow&)
             { abkfatal(0,"Can't assign dbRow");return *this;}
    public:
        dbRow(dbSite &s, Point orig, Dir dir, unsigned spc,unsigned n,
            const char *name,
            Orient ori=Orient(0,1)) // default orientation is vertically face up
            :_site(s), _origin(orig), _direction(dir), _spacing(spc),
            _orient(ori), _numSites(n)
            {_name = new char[strlen(name)+1];strcpy(_name,name);}

        ~dbRow(){delete [] _name;}
        
        const dbSite &getSite()        const {return _site;}
        Point         getOrigin()      const {return _origin;   }
        dbRow::Dir    getDirection()   const {return _direction;}
        unsigned      getSpacing()     const {return _spacing;  }
        Orient        getOrientation() const {return _orient;   }
        unsigned      getNumSites()    const {return _numSites; }
        const char   *getName()        const {return _name;}
        
        double        getLength()      const 
            { return _spacing*(_numSites-1)+_site.getWidth() ;}
};

class dbTrack
{
public:
   const dbLayer& layer;
   bool      horizontal; // vertical if false;
             // IMPORTANT!!!
             // if horizontal is true, this means the track *IS*
             // a VERTICAL track (look at this definition in LEFDEF manual)
   double    start;      // X if horizontal, Y if vertical
   unsigned  numTracks;
   double    spacing; 

   dbTrack(const dbLayer& layr,bool horiz,double strt,
              unsigned nTracks, double spac):
              layer(layr),horizontal(horiz),start(strt),
              numTracks(nTracks),spacing(spac) {}

};

class dbGCellGrid
{
public:
   bool     horizontal;  // vertical if false
            // if horizontal, this means the grid is for the column coordinate
   double   start;       // X if horizontal, Y if vertical
   unsigned numDivisions;
   double   spacing;

   dbGCellGrid(bool horiz, double strt, unsigned nDivs, double spac):
         horizontal(horiz), start(strt), numDivisions(nDivs), spacing(spac) {}
};

//===========================================================================

#endif

