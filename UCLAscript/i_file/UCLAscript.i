%module UCLAscript
%{
#undef list
#undef assert
#include "DB/dbTypes.h"
#include "DB/dbContext.h"
#include "DB/dbDesign.h"
#include "DB/dbSpatial.h"
#include "DB/dbEasyAccess.h"
#include "DB/dbRoutingInfo.h"
#include "Placement/placeWOri.h"
#include "Placement/placement.h"
//#include "dbTemporal.h"
#include "DB/DB.h"
#include "DB/dbNetlist.h"
#define DB_Parameters DB::Parameters
#include "Capo/capoPlacer.h"
#include "RBPlace/RBPlacement.h"
#define RBPlacement_Parameters RBPlacement::Parameters
#include "MetaPlacer/metaPl.h"
#include "Partitioning/partProb.h"
#define PartitioningProblem_Parameters PartitioningProblem::Parameters
#define PartitioningProblem_Attributes PartitioningProblem::Attributes
#include "MLPart/mlPart.h"
#define BaseMLPart_Parameters BaseMLPart::Parameters
#include "NetTopology/dbHGr.h"
#include "PartEvals/partEvals.h"
#include "FilledHier/fillHier.h"
%}

%typemap(perl5,in) char **argv, char *argv[], const char **argv,
                   const char *argv[]
{
    $target = (char **)
            malloc(n_arg*sizeof(char *));
    for (; n_arg; --n_arg) {
       $target[n_arg-1]=
            (char *)SvPV(ST(n_arg-1),PL_na);
    }
}
%typemap(perl5,ignore) int argc(int n_arg){
    $target=n_arg=items;
    items=1;
}
%typemap(perl5,freearg) char **argv {
    free($source);
}

%typemap(python,ignore) char **argv{
   int i;
   int num_argv=PyTuple_Size(args);
   $target=(char**)malloc(num_argv*sizeof(char *));
   for(i=0;i<num_argv;i++){
      PyObject *o=PyTuple_GetItem(args,i);
      if(PyString_Check(o))
         $target[i]=PyString_AsString(o);
      else
         $target[i]="(not char *)";
   }
   args=Py_BuildValue("(i)",num_argv);
}

%typemap(python,freearg) char **argv{
   free($source);
   Py_DECREF(args);
}

class dbNetlist;
class dbCell;
class dbContext;
class dbLayer;
class dbLayerGeom;
class dbMasterCell;
class dbMasterPin;
class dbNet;
class dbNetlist;
class dbPin;
class dbRow;
class dbTrack;
class dbGCellGrid;
class dbSite;
class dbSpatial;
class Orient;
class Symmetry;
class dbMasterVia;
class dbRoutingElement;
class dbRoutedNet;
class dbTemporal;

class dbMasterCellType 
{

// do not rely on these numbers !!! they can change. Use identifiers  -ILM
public:
    enum MClass
    {
        COVER    = 0,
        RING     = 1,
        BLOCK    = 2,
        CORE     = 3,
        PAD      = 4,
        ENDCAP   = 5,
        EXTPIN   = 6,
        MCUNKNOWN= 7
    };
   
    //valid only for MClass == CORE.  this will not be set by parsers 
    enum CoreClass
    {
        FLIPFLOP = 0,
        LATCH    = 1,
        INVERTER = 2,
        BUFFER   = 3,
        NAND     = 4,
        NOR      = 5,
        CoreUNKNOWN= 6
    };

   
   //valid only for MClass == PAD
    enum PadSubClass    //this will be set by parsers
    {
        INPUT = 0,
        OUTPUT= 1,
        INOUT = 2,
        POWER = 3,
        SPACER= 4,
        PadUNKNOWN = 5
    };
 
public:
        
        dbMasterCellType(char *mtype, char *stype);

	dbMasterCellType(MClass  mClass, PadSubClass padClass=PadUNKNOWN)
			: _mClass(mClass), _subClass(padClass)
                         {
                         if (_mClass == ENDCAP) _mClass = PAD;
                         }

 	bool setCoreClass(char* lgc_type);

        
        bool isCover()  const {return _mClass == COVER; }
        bool isRing()   const {return _mClass == RING;  }
        bool isBlock()  const {return _mClass == BLOCK; }
        bool isCore()   const {return _mClass == CORE;  }
        bool isPad()    const {return _mClass == PAD;   }
        bool isEndCap() const {return _mClass == ENDCAP;}
        bool isExtPin() const {return _mClass == EXTPIN;}
        bool isUnknown()const {return _mClass == MCUNKNOWN;}
        
        bool isFlipFlop() const
        {
           abkassert(_mClass == CORE, "inspector for CORE subtype on non-core");
           return _subClass == FLIPFLOP;
        } 
        
        bool isLatch()   const
        {
           abkassert(_mClass == CORE, "inspector for CORE subtype on non-core");
           return _subClass == LATCH;  
        }
        
        bool isInverter() const
        {
           abkassert(_mClass == CORE, "inspector for CORE subtype on non-core");
           return _subClass == INVERTER;
        }
        
        bool isBuffer()  const
        {
           abkassert(_mClass == CORE, "inspector for CORE subtype on non-core");
           return _subClass == BUFFER; 
        }
        
        bool isNAND()    const
        {
           abkassert(_mClass == CORE, "inspector for CORE subtype on non-core");
           return _subClass == NAND;
        }
        
        bool isNOR()     const
        {
           abkassert(_mClass == CORE, "inspector for CORE subtype on non-core");
           return _subClass == NOR;
        }
       
	bool isCoreUnknown() const
	{
	   abkassert(_mClass == CORE, "inspector for CORE subtype on non-core");
	   return _subClass ==  CoreUNKNOWN;
	}


	bool isPI() 	const
	{
	    abkassert(_mClass == PAD, "inspector for PAD subtype on non-pad");
	    return _subClass == INPUT;
	}	

	 bool isPO()     const
        {
            abkassert(_mClass == PAD, "inspector for PAD subtype on non-pad");
            return _subClass == OUTPUT;
        }

 	bool isPIO()     const
        {
            abkassert(_mClass == PAD, "inspector for PAD subtype on non-pad");
            return _subClass == INOUT;
        }

 	bool isPPower()     const
        {
            abkassert(_mClass == PAD, "inspector for PAD subtype on non-pad");
            return _subClass == POWER;
        }

 	bool isPSpacer()     const
        {
            abkassert(_mClass == PAD, "inspector for PAD subtype on non-pad");
            return _subClass == SPACER;
        }

 	bool isPUnknown()     const
        {
            abkassert(_mClass == PAD, "inspector for PAD subtype on non-pad");
            return _subClass == PadUNKNOWN;
        }

 
        
        
};   



//________________________________________________
// CellType (for cell instances)
//________________________________________________


class dbCellType
{

public:
// do not rely on these numbers !!! they can change. Use identifiers  -ILM

    enum MacroType { MacroUNINITIALIZED=0, IO =1, BLOCK=2, CORE=3,
                     MacroOTHER=4, MacroUNKNOWN=5 };
    enum IOType    { PI=0, PO=1, PIO=2, POTHER=3,PUNKNOWN=4 };
    enum CoreType  { SC=0, CC=1, CoreOTHER=2 };

public:
    dbCellType(): _mType(MacroUNINITIALIZED) {}
    dbCellType(char* txt);

             // Note that subType can be of IOType or CoreType
    dbCellType(MacroType mType, char subType): 
        _mType(mType), _subType(subType) {}
       
    bool isInitialized() const { return _mType != MacroUNINITIALIZED; } 
    bool isIO()     const  {return _mType == IO;}
    bool isBlock()  const  {return _mType == BLOCK;}
    bool isCore()   const  {return _mType == CORE;}
    bool isOther()  const  {return _mType == MacroOTHER;}
    bool isUnknown() const {return _mType == MacroUNKNOWN;}
       
    bool isPI()       const 
    { 	abkassert(_mType == IO, "isPI is invalid for non-Pad cells");
        return _subType == PI;}
    bool isPO()       const 
    {   abkassert(_mType == IO, "isPO is invalid for non-Pad cells");
        return _subType == PO;}
    bool isPIO()      const 
    {   abkassert(_mType == IO, "isPIO is invalid for non-Pad cells");
        return _subType == PIO;}
    bool isPOther()   const 
    {   abkassert(_mType == IO, "isPOther is invalid for non-Pad cells");
        return _subType == POTHER;}
    bool isPUnknown() const 
    {   abkassert(_mType == IO, "isPUnknown is invalid for non-Pad cells");
        return _subType == PUNKNOWN;}
        
    bool isCC()          const 
    {   abkassert(_mType == CORE, "isCC is invlaid for non-Core cells");
        return _subType == CC;}
    bool isSC()          const 
    {   abkassert(_mType == CORE, "isSC is invalid for non-Core cells");
        return _subType == SC;}
    bool isCoreOther()   const
    {   abkassert(_mType == CORE, "isCoreOther is invalid for non-Core cells");
        return _subType == CoreOTHER;}
        
};






//_______________________________________
//  dbNetType
//_______________________________________

/*
AXIS NetUse
netUseSIGNAL  // "regular"
netUsePOWER
netUseGROUND
netUseCLOCK
netUseTIEOFF
netUseANALOG
netUseUNKNOWN
*/

class dbNetType 
{
public:
// do not rely on these numbers !!! they can change. Use identifiers  -ILM
    enum NetUse { SIGNAL=0, POWER=1, GROUND=2, CLOCK=3, TIEOFF=4, 
                  ANALOG=5, UNKNOWN=6};

public:
        
        dbNetType(char* txt);
        dbNetType(NetUse use) : _use(use) {}
        
        bool isSignal()  {return _use == SIGNAL; }
        bool isPower()   {return _use == POWER;  }
        bool isGround()  {return _use == GROUND; }
        bool isClock()   {return _use == CLOCK;  }
        bool isTieOff()  {return _use == TIEOFF; }
        bool isAnalog()  {return _use == ANALOG; }
        bool isUnknown() {return _use == UNKNOWN;}
        
};

//______________________________________
//    dbPinType
//______________________________________


class dbMasterPinType 
{
    
public:
// do not rely on these numbers !!! they can change. Use identifiers  -ILM
// note that, e.g., FEEDTHROUGH can be Shape and Dir, that's why names are long

    enum Shape 
    {
	ShapeABUTMENT = 0, 
	ShapeFEEDTHROUGH, 
	ShapeRING, 
	ShapeUNKNOWN 
    };

    enum Use   
   {
	SIGNAL = 0, 
	ANALOG, 
	POWER, 
	GROUND, 
	CLOCK, 
	TIEOFF,
	UseUNKNOWN 
    };

    enum Dir   
    {
	OUT = 0, 
	IN, 
	INOUT, 
	OUTTRISTATE, 
	DirFEEDTHROUGH, 
	DirUNKNOWN
    };

public:
        
        dbMasterPinType(const char * nDir, const char * nUse, const char * nShape);
        dbMasterPinType(Dir dir, Use use, Shape shape):
			                _shape(shape), _use(use), _dir(dir) {};
        
        bool isShapeAbutment()    const { return _shape == ShapeABUTMENT;}
        bool isShapeFeedthrough() const { return _shape == ShapeFEEDTHROUGH;}
        bool isShapeRing()        const { return _shape == ShapeRING;}
        bool isShapeUnknown()     const { return _shape == ShapeUNKNOWN;}
        
        bool isUseSignal()  const {return _use == SIGNAL;}
        bool isUseAnalog()  const {return _use == ANALOG;}
        bool isUsePower()   const {return _use == POWER; }
        bool isUseGround()  const {return _use == GROUND;}
        bool isUseClock()   const {return _use == CLOCK; }
   	bool isUseTieoff()  const {return _use == TIEOFF;}
        bool isUseUnknown() const {return _use == UseUNKNOWN;}
        
        bool isDirOutput()     const {return _dir == OUT;}
        bool isDirInput()      const {return _dir == IN; }
        bool isDirInOut()      const {return _dir == INOUT;}
        bool isDirOutputTristate() const {return _dir == OUTTRISTATE;}
        bool isDirFeedthrough() const {return _dir == DirFEEDTHROUGH;}
        bool isDirUnknown()     const {return _dir == DirUNKNOWN;}
        
        
        
};

//idea in the following typedefs:

// Anything that starts with "ctainer" is a container type.  Right
// now they're all vectors, but this provides a central place to
// change that should we ever want to.  In some cases I've provided
// two "ctainer" types, a "local" and a "global" one.  In this case
// the "global" applies to the entire design (or context) whereas
// the "local" applies to something smaller -- e.g. ctainerCellsLocal
// is for all the cells on a particular net, whereas ctainerCellsGlobal
// is for all the cells in the netlist.

// Corresponding to the "ctainer" types are "it" types.  These are
// constant iterators for the container in question.

// Finally there are special typedefs to support logically-equivalent
// or electrically-equivalent pins and cells.  The ones that start
// with "eqClass" are a container for an equivalence class of pins
// or cells, whereas those that start with "equivClasses" are
// containers for the entire collection of equivalence classes
// (each class corresponds to an element)

typedef vector<dbMasterPin*>                 ctainerMasterPinsLocal;
typedef vector<dbMasterPin*>                 ctainerMasterPinsGlobal;
typedef vector<dbMasterCell*>                ctainerMasterCells;
typedef vector<dbPin*>                       ctainerPinsLocal;
typedef vector<dbPin*>                       ctainerPinsGlobal;
typedef vector<dbNet*>                       ctainerNetsLocal;
typedef vector<dbNet*>                       ctainerNetsGlobal;
typedef vector<dbSite*>                      ctainerSites;
typedef vector<dbLayer*>                     ctainerLayers;
typedef vector<dbLayerGeom*>                 ctainerLayerGeoms;
typedef vector<dbCell*>                      ctainerCellsLocal;
typedef vector<dbCell*>                      ctainerCellsGlobal;
typedef vector<dbRow*>                       ctainerRowsGlobal;
typedef vector<dbRow*>                       ctainerRowsLocal;
typedef vector<dbTrack*>                     ctainerTracksGlobal;
typedef vector<dbTrack*>                     ctainerTracksLocal;
typedef vector<dbGCellGrid*>                 ctainerGCellGridsGlobal;
typedef vector<dbGCellGrid*>                 ctainerGCellGridsLocal;
typedef vector<dbMasterVia*>                 ctainerMasterVias;
// dbRoute part
typedef vector<dbRoutingElement>             ctainerRoutingElements;
typedef vector<dbRoutedNet*>                 ctainerRoutedNets;


typedef ctainerMasterPinsLocal::const_iterator   itMasterPinLocal;
typedef ctainerMasterPinsGlobal::const_iterator  itMasterPinGlobal;
typedef ctainerMasterCells::const_iterator	 itMasterCell;
typedef ctainerPinsLocal::const_iterator         itPinLocal;
typedef ctainerPinsGlobal::const_iterator        itPinGlobal;
typedef ctainerNetsLocal::const_iterator         itNetLocal;
typedef ctainerNetsGlobal::const_iterator        itNetGlobal;
typedef ctainerSites::const_iterator             itSite;
typedef ctainerLayers::const_iterator            itLayer;
typedef ctainerLayerGeoms::const_iterator        itLayerGeom;
typedef ctainerCellsLocal::const_iterator        itCellLocal;
typedef ctainerCellsGlobal::const_iterator       itCellGlobal;
typedef ctainerRowsGlobal::const_iterator        itRowGlobal;
typedef ctainerRowsLocal::const_iterator         itRowLocal;
typedef ctainerTracksGlobal::const_iterator      itTrackGlobal;
typedef ctainerTracksLocal::const_iterator       itTrackLocal;
typedef ctainerGCellGridsGlobal::const_iterator  itGCellGridGlobal;
typedef ctainerGCellGridsLocal::const_iterator   itGCellGridLocal;
typedef ctainerMasterVias::const_iterator        itMasterVia;
// dbRoute part
typedef ctainerRoutingElements::const_iterator  itRoutingElement;
typedef ctainerRoutedNets::const_iterator       itRoutedNet;

typedef vector<dbMasterCell*>                eqClassMasterCells;
typedef vector<eqClassMasterCells>           equivClassesMasterCells;
typedef vector<dbMasterPin*>                 eqClassMasterPins;
typedef vector<eqClassMasterPins>            equivClassesMasterPins;




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
     // recommendation: create the vectors and then 
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

};

//=======================================================================

class dbLayerGeom
{


    const dbLayer &_layer;   //    i.e. where it lives (?)
    Rectangle _rect;       //    ilm: Rectangle is typedef from BBox
    //    same members as in dbRect (xMin,yMin etc)
    //    plus additinal functionality like
    //    += Point, contains(Point) etc (see plGeom.h)

    vector<Point> _polygon;

    char      _isVia:1;
 public:
   dbLayerGeom(const Rectangle& rect, const dbLayer  &layer, bool isVia=false);
 
   dbLayerGeom(const vector<Point>& polygon, const dbLayer  &layer,
       bool isVia=false);

          // ilm: if you need to change
    const vector<Point> &getPolygon() const {return _polygon; }
    bool            isVia()    const  { return _isVia; }
};

//======================================================================


class dbSite
{
public:
    enum Type { PAD, CORE};
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
//     
public:
    enum Type { MASTERSLICE, CUT, OVERLAP, ROUTING};
    enum Dir  { HORIZ, VERT, NOPREFERRED};
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
   

    public:    
      enum Dir  {HORIZ,VERT};
    public:
        dbRow(dbSite &s, Point orig, Dir dir, unsigned spc,unsigned n,
            const char *name,
            Orient ori) // default orientation is vertically face up
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
   %readonly const dbLayer& layer;
    %readwrite
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


class dbMasterCell 
{
    

    char*      _name;
    
    dbMasterCellType _type;
    
    double      _fallDelay;
    double      _riseDelay;
    
    const double     _width;
    const double     _height;
    
    Point            _origin;    // iml: this is a technicality.
                                 // It shifts the origin from the lower left
                                 // corner in macro descrition(LEFDEF5.0 p.1-48)
    
    unsigned         _inDegree;  // iml: why do we need these in master?
    unsigned         _outDegree; // abk says: keep these two for now
                               // iml: supposedly they can be difft in instances
    
    ctainerMasterPinsLocal _pins;
    vector<ctainerMasterPinsLocal> _setsOfJointPins;
    
    //  These are for Detailed Placement. We keep them always since the
    //  overhead is small. Note: this is still a very approximate sketch.
    
    ctainerLayerGeoms    _obstacles; //from MACRO OBSTRUCTION in LEF
    const Symmetry       _symmetry;
    const dbSite&        _site;    // added &    - ILM

    dbMasterCell(const dbMasterCell& from): _type(from._type),
         _width(from._width),_height(from._height),_site(from._site)
        { abkfatal(0,"Can't copy-construct dbMasterCell"); }

    
    public:
        
         dbMasterCell(char* name, char* type, char* sType, 
                      double wth, double ht, Point orig, 
                      const ctainerMasterPinsLocal& pns, Symmetry sym, 
                      const dbSite & site);
        

         dbMasterCell(char* name, dbMasterCellType::MClass type,
                      dbMasterCellType::PadSubClass stype,
                      double wth, double ht, Point orig, 
                      const ctainerMasterPinsLocal& pns,
                      const ctainerLayerGeoms &obs,
                      Symmetry sym, const dbSite & site);

        ~dbMasterCell();
        
        itMasterPinLocal pinsBegin() const { return _pins.begin(); }
        itMasterPinLocal pinsEnd()   const { return _pins.end();   }
        unsigned getNumPins()        const { return _pins.size(); }
        
        const    char* getName()     const { return _name;     }
        double   getHeight()         const { return _height;   }
        double   getWidth()          const { return _width;    }
        unsigned getIndegree()       const { return _inDegree; }
        unsigned getOutdegree()      const { return _outDegree;}
	dbMasterCellType getType()   const { return _type;     }	

        const ctainerLayerGeoms& getObstacles() const { return _obstacles; }
        Symmetry                 getSymmetry()  const { return _symmetry;  }
        const dbSite&            getSite()      const { return _site;      }
        
        //type inspectors.  the alternative is to just have a 'get type fctn'
        bool isCover()   const {return _type.isCover();  }
        bool isRing()    const {return _type.isRing();   }
        bool isBlock()   const {return _type.isBlock();  }
        bool isCore()    const {return _type.isCore();   }
        bool isPad()     const {return _type.isPad();    }
        bool isEndCap()  const {return _type.isEndCap(); }
        bool isExtPin()  const {return _type.isExtPin(); }
        bool isUnknown() const {return _type.isUnknown();}
        
        bool isFlipFlop() const {return _type.isFlipFlop();}
        bool isLatch()    const {return _type.isLatch();   }
        bool isInverter() const {return _type.isInverter();}
        bool isBuffer()   const {return _type.isBuffer();  }
        bool isNAND()     const {return _type.isNAND();    }
        bool isNOR()      const {return _type.isNOR();     }
	bool isCoreUnknown()
			  const {return _type.isCoreUnknown();}

	bool isPI()	  const {return _type.isPI();	   }
	bool isPO()	  const {return _type.isPO();      }
	bool isPIO()	  const {return _type.isPIO();	   }
	bool isPPower()   const {return _type.isPPower();  }
	bool isPSpacer()  const {return _type.isPSpacer(); }
	bool isPUnknown() const {return _type.isPUnknown();}
        
        double getRiseDelay() const {return _riseDelay;}
        double getFallDelay() const {return _fallDelay;}

        Rectangle reOrientRec (const Rectangle& rec,
			       const Orientation& orien) const;


};

//====================================================================== 

class dbCell 
{
    

    public: 
        
        dbCell(const char* name, unsigned index, const dbMasterCell  &master,
               ctainerPinsLocal &pins)
              : _index(index),_master(master),_pins(pins)
              {_name = new char [strlen(name)+1];strcpy(_name,name);}
        //constructor should create the nets vector (from the pins)
        //first Pins are created, then the cell containing them,
        //then the cell* in each pin is updated (in this constructor)
        
        ~dbCell();
        
        itPinLocal pinsBegin() const {return _pins.begin();}
        itPinLocal pinsEnd()  const  {return _pins.end();}
        itNetLocal netsBegin() const {return _nets.begin();}
        itNetLocal netsEnd()  const  {return _nets.end();} 
        
        void loopPinsBegin() const;
        void loopPinsEnd(itPinLocal itr) const;
        void loopNetsBegin() const;
        void loopNetsEnd(itNetLocal itr) const;
        
        const dbMasterCell &getMaster() const {return _master;}
        unsigned            getIndex()  const {return _index;}
        char*               getName()   const {return _name;}
	dbCellType          getType()   const {return _type;}
        double   getHeight()         const { return _master.getHeight();  }
        double   getWidth()          const { return _master.getWidth();   }
        unsigned getIndegree()       const { return _master.getIndegree();}
        unsigned getOutdegree()      const { return _master.getOutdegree();}
        
        unsigned getNumNets() const {return _nets.size();}
        unsigned getNumPins() const {return _pins.size();}
        
        //inspectors for dbCell Type
        bool isIO()     const {return _type.isIO();     }
        bool isBlock()  const {return _type.isBlock();  }
        bool isCore()   const {return _type.isCore();   }
        bool isOther()  const {return _type.isOther();  }
        bool isUnknown()const {return _type.isUnknown();}
       
        bool isPI()       const {return _type.isPI();      }
        bool isPO()       const {return _type.isPO();      }
        bool isPIO()      const {return _type.isPIO();     }
        bool isPOther()   const {return _type.isPOther();  }
        bool isPUnknown() const {return _type.isPUnknown();}
        
        bool isCC()          const {return _type.isCC();         }
        bool isSC()          const {return _type.isSC();         }
        bool isCoreOther()   const {return _type.isCoreOther();  }
        
        bool isTimingNode()    const; // belongs to the timing graph
                                      // same as !isDisconnected()
        bool isLatchNode()     const; // belongs to the latch graph
                                      // *not* the same as isLatch()
        bool isIgnoredTimingNode() const; // belongs to the timing graph
                                      // but ignored for latch timing analysis
        
        double getRiseDelay() const {return _master.getRiseDelay();}
        double getFallDelay() const {return _master.getFallDelay();}
        
        const dbPin  *commonPin(const dbNet* net) const;
        bool commonPinIsCellInput(const dbNet*  net) const;
        bool commonPinIsCellOutput(const dbNet* net) const;
        
        bool isInputPad() const;
        bool isOutputPad() const;


};

class CompareCellsByName
{
   public:
    CompareCellsByName(){}

};

class CompareCellsByStatus
{
   public:
    CompareCellsByStatus(){}

};

class CompareMCsByName
{
   public:
    CompareMCsByName(){}

};

class CompareCellsByCellIndex
{
   public:
    CompareCellsByCellIndex(){}

};

class CompareCellsByDir
{
   public:
    CompareCellsByDir(dbNet *curNet): _curNet(curNet) {}

};

class CompareObstaclesByXmin
{
  public:
    CompareObstaclesByXmin(){}

};

// ======================  IMPLEMENTATIONS ========================









class dbMasterPin 
{            // create MasterPins first, MasterCell afterwards
    

    public:
        dbMasterPin(const char * name, const char * dir,
                    const char * use, const char * shape, BBox bbox,
                    Point center)
                                 :_type(dir, use, shape), _cell(NULL),
			          _bbox(bbox),_center(center),_mustJoin(NULL)
            {_name = new char[strlen(name)+1];strcpy(_name,name);}
        
        ~dbMasterPin();
        
        void setMasterCell(const dbMasterCell  &nCell) {_cell = &nCell;}
        
        const dbMasterCell    &getCell()   const { return *_cell; }
        const char*     getName()    const { return _name;  }
              // changed _offset to _center according to Wenyong's comment from
        Point           getCenter()  const { return _center;}
        BBox            getBBox()    const { return _bbox;  }
	const ctainerLayerGeoms& getPorts() const { return _ports; } 
     	char* 		getType()    const { return _type;  }
        
        double getRiseDelay()  const { return _riseDelay; }
        double getFallDelay()  const { return _fallDelay; }
        double getRiseResist() const { return _riseResist;}
        double getFallResist() const { return _fallResist;}
        double getInputCap()   const { return _inputCap;  }
        
        bool isShapeAbutment()    const { return _type.isShapeAbutment();   }
        bool isShapeFeedthrough() const { return _type.isShapeFeedthrough();}
        bool isShapeRing()        const { return _type.isShapeRing();       }
        bool isShapeUnknown()     const { return _type.isShapeUnknown();    }
        
        bool isUseSignal()  const {return _type.isUseSignal(); }
        bool isUseAnalog()  const {return _type.isUseAnalog(); }
        bool isUsePower()   const {return _type.isUsePower();  }
        bool isUseGround()  const {return _type.isUseGround(); }
        bool isUseClock()   const {return _type.isUseClock();  }
        bool isUseUnknown() const {return _type.isUseUnknown();}
        
        bool isDirOutput()     const {return _type.isDirOutput();  }
        bool isDirInput()      const {return _type.isDirInput();   }
        bool isDirInOut()      const {return _type.isDirInOut();   }
        bool isDirOutputTristate() const {return _type.isDirOutputTristate();}
        bool isDirFeedthrough() const {return _type.isDirFeedthrough();}
        bool isDirUnknown()     const {return _type.isDirUnknown();}

        bool isJoint()       const { return _mustJoin!=NULL; }
        ctainerMasterPinsLocal&   getJointPins() const 
                              { abkassert(_mustJoin," No joint masterPins");
                                return *_mustJoin; 
                              }
        
};

//====================================================================== 

class dbPin 
{
    

//   If pin names are ever used, make sure that they are initialized to NULL
//   by the parsers which don't use them (see ctor), e.g. LEFDEFq. ILM
//  char               * _name ;
    unsigned             _index;
    const dbMasterPin  & _master;
    const dbCell       * _cell;  // cell and net ptrs can't be references
    const dbNet        * _net;   // because they are set after pin is
                                 // included into a cell/net, while
                                 // the pin is constructed before that
     dbPin(const dbPin& from):_master(from._master)
      { abkfatal(0,"Can't copy-construct dbPin"); }

    public:
//   If pin names are ever used, make sure that they are initialized to NULL
//   by the parsers which don't use them (see ctor), e.g. LEFDEFq. ILM
        dbPin(const dbMasterPin  &master, unsigned index/*,const char * name */):
              _index(index), _master(master),_cell(NULL),_net(NULL) 
       { /* if (_name) _name = new char[strlen(name)+1];strcpy(_name,name); */}
        
        ~dbPin() { /* if (_name) delete[] _name; */ }

        void setCell(const dbCell & cell){_cell = &cell;}
        void setNet (const dbNet &  net){_net  = &net; } 
        
        const char  *getMasterPinName() const { return _master.getName(); }
        const dbCell& getCell() const { 
		abkassert(_cell != NULL, "Null cell* in pin");
		return *_cell; } 
        const dbNet & getNet()  const {
		abkfatal(_net != NULL, "Null net* in pin");
		return *_net;  }
        unsigned getIndex()            const { return _index;  }
        const dbMasterPin& getMaster() const { return _master; }

	char*  getType()       const { return _master.getType();      }

        double getRiseDelay()  const { return _master.getRiseDelay(); }
        double getFallDelay()  const { return _master.getFallDelay(); }
        double getRiseResist() const { return _master.getRiseResist();}
        double getFallResist() const { return _master.getFallResist();}
        double getInputCap()   const { return _master.getInputCap();  }
        
        
        bool isShapeAbutment()    const { return _master.isShapeAbutment();   }
        bool isShapeFeedthrough() const { return _master.isShapeFeedthrough();}
        bool isShapeRing()        const { return _master.isShapeRing();       }
        bool isShapeUnknown()     const { return _master.isShapeUnknown();    }
        
        bool isUseSignal()  const {return _master.isUseSignal(); }
        bool isUseAnalog()  const {return _master.isUseAnalog(); }
        bool isUsePower()   const {return _master.isUsePower();  }
        bool isUseGround()  const {return _master.isUseGround(); }
        bool isUseClock()   const {return _master.isUseClock();  }
        bool isUseUnknown() const {return _master.isUseUnknown();}
        
        bool isDirOutput()     const {return _master.isDirOutput();  }
        bool isDirInput()      const {return _master.isDirInput();   }
        bool isDirInOut()      const {return _master.isDirInOut();   }
        bool isDirOutputTristate() const {return _master.isDirOutputTristate();}
        bool isDirFeedthrough() const {return _master.isDirFeedthrough();}
        bool isDirUnknown()     const {return _master.isDirUnknown();}

	bool isConnected() const {return _net != NULL;}

};

class ComparePinsByDirAndName
{	//output, inout, then input
   public:
    ComparePinsByDirAndName(){}

};

class ComparePinsByCellIndexAndDir
{
   public:
    ComparePinsByCellIndexAndDir(){}

};




class dbNet
{
    

    char *      	_name;
    unsigned   		_index;
    const dbNetType  	_type;        // regular, timing, clock

 public:
    ctainerCellsLocal  	_cells;       // source cell(s) come<s> first
    ctainerPinsLocal   	_pins;
    ctainerLayerGeoms  	_wiring;      // should be empty except for special
                                      // nets.
    unsigned		_numOut;
    unsigned		_numInOut;

    bool                _hasLogicalPin; 

    public:
        dbNet(const char * name, unsigned idx, dbNetType type,
              const ctainerPinsLocal& pins):
             _index(idx),_type(type),_pins(pins),_hasLogicalPin(false)
              {  /* Extrapolate vector<dbCell*> cells from pins */
              /* Go through each pins and set the cell ptr */
              /* This should be verified as part of a future integrity check */
              _name = new char[strlen(name)+1];strcpy(_name,name);
              }
        ~dbNet();
        unsigned  getNumCells() const { return _cells.size(); }
        unsigned  getNumPins()  const { return _pins.size() ; }
        unsigned  getDegree()   const { return _cells.size(); }
        unsigned  getIndex()    const { return _index; }
        dbNetType getType()     const { return _type; }
        const char* getName()   const { return _name; }
 	unsigned  getNumOut()   const { return _numOut; }
	unsigned  getNumInOut() const { return _numInOut; }
	unsigned  getNumIn()	const { return _cells.size() - 
					(_numOut + _numInOut); }

        unsigned   hasLogicalPin() const {return _hasLogicalPin;}
          
        itCellLocal cellsBegin() const { return _cells.begin(); }
        itCellLocal cellsEnd()   const { return _cells.end();   }

	itCellLocal sourcesBegin() const { return _cells.begin(); }
	itCellLocal sourcesEnd()   const { return _cells.begin() + _numOut; }

	itCellLocal sourceSinksBegin() const 
		{ return _cells.begin() + _numOut; }
	itCellLocal sourceSinksEnd()   const 
		{ return _cells.begin() + _numOut + _numInOut; }

	itCellLocal sinksBegin() const 
		{ return _cells.begin() + _numOut + _numInOut; }
	itCellLocal sinksEnd()   const 
		{ return _cells.end(); }

        const dbCell&  getCellByIdx(unsigned i) const
        {
          abkassert(i < _cells.size(),"Cell index out of range");
          return *(_cells[i]);
        }

        itPinLocal pinsBegin()  const { return _pins.begin();  }
        itPinLocal pinsEnd()    const { return _pins.end();    }

        const dbPin&  getPinByIdx(unsigned i) const
        {
          abkassert(i < _pins.size(),"Pin index out of range");
          return *(_pins[i]);
        }

        itLayerGeom wiringBegin() const { return _wiring.begin();}
        itLayerGeom wiringEnd()  const { return _wiring.end();  }

        const dbLayerGeom& getWiringLayerGeom(unsigned i) const
              {return *(_wiring[i]);}

        unsigned getNumWiringLayerGeoms() const {return _wiring.size();}
          
        void loopCellsBegin() const;
        void loopCellsEnd(itCellLocal itr) const;
        void loopPinsBegin() const;
        void loopPinsEnd(itPinLocal itr) const;
          
          
        const dbPin *commonPin(const dbCell* cell) const;
        bool   commonPinIsCellInput (const dbCell* cell) const;
        bool   commonPinIsCellOutput(const dbCell* cell) const;
          
        double getInputCap(const dbCell* cell) const;
        double getDriverResist(const dbCell* cell) const;

        bool isSpecial() const {return _index >= snetOffset;} //CAREFUL:
                                                              //works only
                                                              //*after*
                                                              //postprocessing

};

class CompareNetsByCellIndex    //comp. class for sorting the netlist's nets
{
   public:
    CompareNetsByCellIndex(){}

};

class CompareNetsByDegree
{
    public:
     CompareNetsByDegree(){}

};

class CompareNetsByName
{
    public:
     CompareNetsByName(){}

};

class CompareNetsByDirection
{
    const dbCell& _cell;
  public:
    CompareNetsByDirection(const dbCell& cell)
	: _cell(cell)
	{}
};

// =========================  IMPLEMENTATIONS ========================









class CompareNetsByCellIndex;
class CompareMCsByName;
class CompareCellsByName;
class CompareCellsByCellIndex;

class dbNetlist 
{
    
    
    //  maxArea and totalArea may be added later
    
    ctainerMasterCells       _cellLib;
    ctainerMasterPinsGlobal  _pinLib;
    
    ctainerCellsGlobal       _cells;
    ctainerPinsGlobal        _pins;
    ctainerNetsGlobal        _nets;
    ctainerNetsGlobal        _sNets; //special nets
    
    static char _forAllCellsFlag;
    static char _forAllNetsFlag;
   
    unsigned		  _numIOPads; 
    unsigned		  _numAllPads;
    unsigned              _numIOPins;
    unsigned              _numCoreCells;

    unsigned		  _numCCs;
    unsigned		  _numSCs;
    unsigned	      	  _numDummy1;	//no current use
    unsigned	 	  _numDummy2;	

    double		  _coreCellArea;
    
    equivClassesMasterCells _cellLEQs;
    equivClassesMasterCells _cellEEQs;
    equivClassesMasterPins  _pinLEQs;

        // this is what you can't do  --
        // default copy-ctor and assignment are wrong

        dbNetlist(const dbNetlist&)
                 { abkfatal(0,"Can't copy-construct dbNetlist"); }
    
    public:
        
        dbNetlist(); // will be populated by parsers
        ~dbNetlist();
        // copy-ctor and assignment are prohibited: see below

        eqClassMasterCells getCellLEQClass(dbMasterCell*) const;
        eqClassMasterCells getCellEEQClass(dbMasterCell*) const;
        eqClassMasterPins  getPinLEQClass(dbMasterPin*) const;
        
        
        bool isConsistent() const;
        
        unsigned getNumCells() const {return _cells.size();}
        unsigned getNumPins()  const {return _pins.size();}    
        unsigned getNumNets()        const { return _nets.size();   }
        unsigned getNumSNets()       const { return _sNets.size();  }
        unsigned getNumMasterCells() const { return _cellLib.size();}
        unsigned getNumMasterPins()  const { return _pinLib.size(); }

	unsigned getNumCCs()	const	{ return _numCCs; }
	unsigned getNumSCs()	const	{ return _numSCs; }
	unsigned getNumIOPads() const	{ return _numIOPads;}
	unsigned getNumAllPads() const 	{ return _numAllPads;}
	unsigned getNumIOPins()	const	{ return _numIOPins; }
	unsigned getNumCoreCells() const{ return _numCoreCells; }
        
        itCellGlobal cellsBegin() const { return _cells.begin();}
        itCellGlobal cellsEnd()   const { return _cells.end();  }

        itNetGlobal  netsBegin()  const { return _nets.begin(); }
        itNetGlobal  netsEnd()    const { return _nets.end();   }
        itNetGlobal  sNetsBegin() const { return _sNets.begin();}
        itNetGlobal  sNetsEnd()   const { return _sNets.end();  }

	itPinGlobal  pinsBegin()  const { return _pins.begin(); }
	itPinGlobal  pinsEnd()	  const { return _pins.end();   }

	itMasterPinGlobal pinLibBegin() const { return _pinLib.begin(); }
	itMasterPinGlobal pinLibEnd()   const { return _pinLib.end();   }
	itMasterCell	  cellLibBegin() const {return _cellLib.begin();}
	itMasterCell      cellLibEnd()   const {return _cellLib.end();  } 
 
        
        const dbCell& getCellByIdx(unsigned i) const;
        const dbNet&  getNetByIdx(unsigned i) const;
        const dbNet&  getSNetByIdx(unsigned i) const;
        const dbPin&  getPinByIdx(unsigned i) const;
        
        void loopCellsBegin() const;
        void loopCellsEnd(itCellGlobal itr) const;
        void loopNetsBegin() const;
        void loopNetsEnd(itNetGlobal itr) const;
	void loopSNetsBegin() const;
	void loopSNetsEnd(itNetGlobal itr) const;

  // It would be nice to provide iterators like forAllCoreCells()
  // which will get the next cell, check if it is of a given type
  // and, if not, skip to the next.
  
  // In the worst case (e.g. unsorted order, or alpha-sorted order)
// it will take O(N)  (N == tot#ofCells)  to traverse all k<=N cells of a kind.
  // If the cells are sorted so that cells of a kind go together, it will
  // take O(k) to traverse all k cells of a kind (at least for some kinds)

	void postProcess();        
	void processLGC( ifstream &lgcf,const Verbosity &verbose);

	double evalHalfPerimCost(const Placement &pl, 
                unsigned threshold=0, bool IONetsOnly=false) const
                { return 
                  evalHalfPerimCost(pl, NULL, threshold, IONetsOnly); 
                }

	double evalHalfPerimCost(const Placement &pl,
                                 const vector<Orientation>& ori,
               unsigned threshold=0,  bool IONetsOnly=false) const
		{ return 
                  evalHalfPerimCost(pl, &(ori), threshold, IONetsOnly);
                }

	double evalWHalfPerimCost(const Placement &pl, 
               unsigned threshold=0, bool IONetsOnly=false) const
                // weighted
		{ return 
                   evalWHalfPerimCost(pl, NULL, threshold, IONetsOnly);
                }

	double evalWHalfPerimCost(const Placement &pl, 
                                  const vector<Orientation>&ori, 
                unsigned threshold=0, bool IONetsOnly=false) const
		{ return
                  evalWHalfPerimCost(pl, &(ori), threshold,IONetsOnly);
                }

        double evalMSTCost(const Placement &pl) const
                { return evalMSTCost(pl, NULL); }
        double evalMSTCost(const Placement &pl, 
                  const vector<Orientation> &ori) const
                { return evalMSTCost(pl, &(ori)); }

	double evalHalfPerimCost(dbNet *net, const Placement &pl,
                        const vector<Orientation> *orientations,
                        bool IONetsOnly=false) const;

 	double evalHalfPerimCostNoPins(dbNet *net, const Placement &pl,
                        bool IONetsOnly=false) const;

//        void printNetStats(ostream& out) const;
        void printNetStats()             const  { printNetStats(cout); }


};










 


class dbMasterCell;
class dbCell;
class dbMasterPin;
class dbPin;
class dbNet;
class dbNetlist;



class dbEasyAccess;

//========================== CLASS INTERFACES =============================

class dbEasyAccess
{


  public:
        enum Examples { IntRandom, IntLessRandom };

  protected:
  
        // no constructors, no public data members:
        // DB postprocessor will call method populate()
        // to fill in the members below;
        // recommendation: create members with their own ctors 
        // and then assign them to private fields of dbContext; ???

        double 			   _coreCellArea;
  	double			   _totalCellWidth;

        // These members are indexed by cell ID
        vector<double>             _cellWidths;
	vector<double>             _cellAreas;

	// This is a vector of IntervalSeqLayers, each of
	// which is indexed by cell ID
        vector<IntervalSeqLayer> _whitespace;

	// Like _whitespace, this is a vector indexed by layer,
	// of vectors indexed by pin ID
        vector<ISPairLayer>  _portLocations;

        vector<unsigned>           _netIds; // .. that is for each pin -ILM
		//important Note:  not all pins are connected to a net.
		//unconnected bins will have a netId == (unsigned)(-1) in
		//_netIds.
        Subset                     _coreCellEmbedding;
        BBox                       _coreBBox;

        // stores the number of containing cell for each pin 
        // not a Mappingm, because not injective
        vector<unsigned>           _pin2CellIdx;

   hash_map<const char*, const dbCell *, hash<const char*>, CompareNameEqual>
                                   _cellNameTable;
   hash_map<const char*, const dbNet *,  hash<const char*>, CompareNameEqual>  
                                   _netNameTable;
	
        void populateCellWidths (const dbNetlist&);
	void populateCellAreas (const dbNetlist&);
        void populateWhitespace (const dbContext&, const dbNetlist&,
				 const dbSpatial&);
        void populateNetIds (const dbNetlist&);
	void populatePortLocations (const dbContext&,
				    const dbNetlist&, const dbSpatial&); 
        void populateCoreCellEmbedding(const dbNetlist&);
        void populateCoreBBox(const dbLayout&);
        void populatePin2CellIdx(const dbNetlist&);
        void populateNameHashTables(const dbNetlist&);

  public:

        void populate(const dbContext&, const dbNetlist&,
		      const dbSpatial&);
	void populate(Examples ex, unsigned numCells, 
		      unsigned numNets, unsigned numPins,
		      unsigned seed = 0);

	bool isConsistent (const dbContext&, const dbNetlist&) const;

        const vector<double>&  getCellWidths()   const { return _cellWidths; }
	const vector<double>&  getCellAreas()    const { return _cellAreas;  }
	const vector<IntervalSeqLayer>& getWhitespace() const
	                                               { return _whitespace; }
//      const vector<ISPairLayer>& getPortLocations() const
//                                        { return _portLocations;}
        unsigned getNumPortLocations() const { return _portLocations.size(); }
        const ISPairLayer& getPortLocationByIdx(unsigned idx) const
                                             { return _portLocations[idx];   }
	const vector<unsigned>&      getNetIds() const { return _netIds; }
        const Subset&    getCoreCells() const    { return _coreCellEmbedding;  }
        const BBox&      getCoreBBox()  const    { return _coreBBox;     }

        const vector<unsigned>& getPin2CellIdx()const { return _pin2CellIdx;   }
        unsigned getCellIdxByPinIdx(unsigned k) const { return _pin2CellIdx[k];}

	double getCoreCellArea()                const { return _coreCellArea;  }
	double getTotalCellWidth()              const { return _totalCellWidth;}	
        const dbCell& getCellByName(const char* name) const;
        const dbNet&  getNetByName (const char* name) const;
     
};


//================= IMPLEMENTATION ==============================

/*
inline void dummy (const dbEasyAccess& arg)
// This definition is just a workaround for a very strange 
// compiler error message, associated with the use of STL.
// Without it, a complaint issues
// from STL that the types cast below (which are the real types)
// are incomplete when first used in a file.
{
  abkfatal(false,"This function should never be called.");
  cout << (static_cast<vector<IntervalSeqLayer> >(arg.getWhitespace())).size();
  cout << arg.getNumPortLocations();
  for (unsigned i=0; i<10; i++)
  {
    const ISPairLayer &layer = arg.getPortLocationByIdx(i);
    cout << (static_cast<pair<IntervalSeq,IntervalSeq> >(layer[0])).first;
  }
}
*/



class dbSpatial;

//========================== CLASS INTERFACES =============================

class dbSpatial
{
    public:
        dbSpatial() {};
        // parser will directly populate most of the below
        // recommendation: create the vectors and then assign
        // them to private fields of dbContext;
 
        // copy-ctor and assignment are prohibited: see below
        
        //The next 3 fields are all indexed by cell index.
        //locationsValid[i] should be set true if the corresponding
        //DEF COMPONENTS stmt says PLACED, FIXED or COVER.
        //Furthermore, if it says FIXED or COVER then a corresponding
        //constraint should be added to the member constraints.

        Placement           locations;
        bit_vector          locationValid;  
        vector<Orientation> orientations; // orientations

        Constraints         constraints;  // defined in the Constraints package
                                          // each individual constraint within
                                          // this object will reference the
                                          // member "locations" defined above

        unsigned            getSize()   const  { return locations.getSize();}
        
        bool                isConsistent()  const;

        void                reorderCells(const Permutation&);

        bool                allCellsPlaced() const;

public: // this is what you can't do  --
        // default copy-ctor and assignment are wrong

        dbSpatial(const dbSpatial&)
                 { abkfatal(0,"Can't copy-construct dbSpatial"); }

};

class DB
{
public:


 public:   
        
        dbSpatial   	spatial;
	dbEasyAccess	easyAccess;
        dbRoutingInfo   routedNets;
        dbTemporal  	*timing;
        
        DB(const char *auxFileName, DB_Parameters params);
       ~DB();
        bool  isConsistent()            const;
        const char *getOrigFileName()   const { return origFileName;   }
        const char *getAuxName()        const; // derived from origFileName
        const char *getDirName()         const { return dirName;       }
        const vector<char*>& getFileNames() const { return allFileNamesInAUX; }
        const char *getOrigFileFormat() const { return origFileFormat; }
        char        getDelimiter()      const { return delimiter;      }
        
        const dbLayout&  getLayout()  const { return context; }
        const dbNetlist& getNetlist() const { return design;  }
              dbNetlist& getNetlistForSurgery() { return design;  }
        const dbTemporal &getTiming() const { return *timing; }

        void saveDEF(const char *defFileName,
                     bool bSaveUnPlacedLocs=false,
                     bool bSaveRoutingInfo=true) const;
        void saveXYLineDrawing(const char *xyFileName) const;

        bool hasDesignName() const { return strlen(designName)>0; }
        bool hasTechnoName() const { return strlen(technoName)>0; }

        const char* getDesignName() const { return designName; }
        const char* getTechnoName() const { return technoName; }

        double evalHPWLcost(unsigned threshold=0, bool IONetsOnly=false) const;
        // Half-Perimeter wirelength cost estimate (0 -- no thresholding)
        double evalWHPWLcost(unsigned threshold=0, bool IONetsOnly=false) const;
        // same, but net costs are weighted

//        void printNetStats(ostream& out) const  { design.printNetStats(out); }
        void printNetStats()             const  { printNetStats(cout); }

        Point locatePin(const dbPin& pin, const Placement& placement,
                        const vector<Orient>& orientations) const;
        // These two methods are for calculating the actual location
        // of a given pin in the layout
        Point locatePin(const dbPin& pin) const
        { return locatePin(pin, spatial.locations, spatial.orientations); }
        Point locatePin(const dbPin& pin,
                        const vector<Orient>& orientations) const
        { return locatePin(pin, spatial.locations, orientations); }

	void setPlaceAndOrient(const PlacementWOrient& plWOri);
        
};
    class DB_Parameters
    {
      public:
          bool  ignoreLogicalPins;
          bool  ignoreGlobalRoutingInfo;
          bool  ignoreDetailedRoutingInfo;
          bool  ignoreTimingLibs;
          bool  fixPlacedPads;
          Verbosity  verbose;
          bool  routedDesign;
          bool  alwaysCheckConsistency;
          unsigned placeHolder2;
          unsigned placeHolder3;
          double placeHolder4;
          double placeHolder5;

      DB_Parameters();
    };



class RBPlacement
{
  public:

  protected:

    HGraphWDimensions*   _hgWDims;  //owned by RBPl, but 
				//a pointer so it can be populated by
				//ctors of derived classes
    bool 		_populated;
    unsigned 		_cellsNotInRows;	//the #of cells not in a subrow

    vector<RBPCoreRow> 	_coreRows;    

    PlacementWOrient	_placement;		//location of each cell
    bit_vector		_isInSubRow;
    vector<RBCellCoord> _cellCoords;		//for cells in subRows,
						//cellCoords allows for O(1)
						//time lookups
    bit_vector		_isFixed;
    bit_vector		_isCoreCell;
    bit_vector          _storElt;

    RBPlacement_Parameters		_params;

    RBPlacement*	_nonConstThis;

    char origFileName[255];
    char dirName     [1023];
    vector<char*>  allFileNamesInAUX;

    void splitRowByObstacle(RBPCoreRow& cr, const BBox& obstacle);

    void setCellsInRows();
    void sortSubRowsByX();
    void populateCC();

    double oneNetHPWL (unsigned netId, bool usePinLocs);
    double oneNetMSTWL(unsigned netId, bool usePinLocs);

    void   spaceCellsEqually(RBPSubRow& sr, 
			     unsigned firstCellOffset, 
			     unsigned numCells);

    void   spaceCellsWithPinDimensionsAlg1(RBPSubRow& sr, 
			                   unsigned firstCellOffset, 
			                   unsigned numCells);

    void   spaceCellsWithPinDimensionsAlg2(RBPSubRow& sr, 
			                   unsigned firstCellOffset, 
			                   unsigned numCells);

    void constructRows(const char* sclFile);


    RBPlacement(unsigned numCells, const Placement& pl, 
		const vector<Orient>& ori, const RBPlacement_Parameters& params) 
   	: _hgWDims(NULL), _populated(false), _placement(pl,ori), 
	  _isInSubRow(numCells, false), _cellCoords(numCells),
          _isFixed(numCells, false), 
	  _isCoreCell(numCells, false), 
	  _storElt(numCells, false), 
	  _params(params), _nonConstThis(this)
        {}

    RBPlacement(unsigned numCells, const PlacementWOrient& pl, 
		const RBPlacement_Parameters& params) 
   	: _hgWDims(NULL), _populated(false),
	  _placement(pl), _isInSubRow(numCells, false),
	  _cellCoords(numCells), 
          _isFixed(numCells, false), 
	  _isCoreCell(numCells, false), 
	  _storElt(numCells, false), 
	  _params(params), _nonConstThis(this)
   	{}

  public:

   RBPlacement(const char* auxFileName, const RBPlacement_Parameters& params);

	//this ctor reads only the netlist from the auxfile.
	//it will create a layout region with the target aspect ratio
	//and whitespace.  It also assumes the following:
	//core node widths are integer
	//core node heights are all the same
	//pad node dimensions are all the same
	//whitespace is a percent (i.e. 10 == 10%)
   RBPlacement(const char* auxFile, double aspectRatio, double whiteSpace,
		const RBPlacement_Parameters& params);

   ~RBPlacement();

   const char *getOrigFileName()   const { return origFileName;   }
   const char *getAuxName()        const; // derived from origFileName
   const char *getDirName()         const { return dirName;       }
   const vector<char*>& getFileNames() const { return allFileNamesInAUX; }

   void resetPlacementWOri(const PlacementWOrient& pl);
   void resetPlacement(const Placement& pl);

   unsigned     getNumCells()           const {return  _placement.getSize();}
   bool 	isPopulated() 	 	const {return  _populated;}
   bool		allCellsPlaced() 	const {return !_cellsNotInRows;}
   unsigned	numCellsNotPlaced() 	const {return  _cellsNotInRows;}

   bool		isCoreCell(unsigned id) const {return _isCoreCell[id];}
   bool		isFixed(unsigned id)    const {return _isFixed[id];}
   bool		isStorElt(unsigned id)  const {return _storElt[id];}
   bool		isInSubRow(unsigned id) const {return _isInSubRow[id];}

   const bit_vector& getFixed()         const {return _isFixed;}
   const bit_vector& getStorElts()      const {return _storElt;}


   void  	 setLocation (unsigned id, const Point& pt);
   Orient&       getOrient   (unsigned id) { return _placement.getOrient(id);} 
   void  	 setOrient   (unsigned id, Orient ori)
			{ _placement.setOrient(id,ori); }

   // These return the row and subrow where a point would lie
   // These can't const be ..it messes up the equal_range function
   const RBPCoreRow* findCoreRow (const Point& point) const;
   const RBPSubRow*  findSubRow  (const Point& point) const;
   RBRowPtrs findBothRows       (const Point& point);

   itRBPCoreRow coreRowsBegin()  const { return _coreRows.begin();}
   itRBPCoreRow coreRowsEnd()    const { return _coreRows.end();  }
   unsigned     getNumCoreRows() const { return _coreRows.size(); }

   bool         isCellInRow(unsigned id) const { return _isInSubRow[id];}

   const RBCellCoord& getCellCoord(unsigned id) const 
   {    abkassert(_isInSubRow[id], "Requested cellCorrd for cell not in a row");
 	return _cellCoords[id];
   }

	//the following functions (un-)place cells from core or sub rows.

	//'extract' functions remove the cells from rows,
	//'embed' functions place the cells in rows.
	
	//see documentation in the DOCS subdirectory for more information

   void   extractCellsFromSR (RBPSubRow& sr, vector<unsigned>& cellIds,
				vector<double>& offsets,
				double beginOffset=0,  
				double endOffset=DBL_MAX);

   void   extractCellsFromSR (RBPSubRow& sr, vector<unsigned>& cellIds,
				vector<unsigned>& siteOffsets,
				unsigned beginSite=0,unsigned endSite=UINT_MAX);

   void   extractCellsFromSR (RBPSubRow& sr, vector<unsigned>& cellIds,
				double beginOffset=0,double endOffset=DBL_MAX);


   void   embedCellsInSR   (RBPSubRow& sr, const vector<unsigned>& cellIds,
				const vector<double>& offsets);

   void   embedCellsInSR   (RBPSubRow& sr, const vector<unsigned>& cellIds,
				const vector<unsigned>& siteOffsets);

   void   embedCellsInSR   (RBPSubRow& sr, const vector<unsigned>& cellIds,
				unsigned beginSite=0,unsigned endSite=UINT_MAX);

   void   permuteCellsInSR (RBPSubRow& sr, unsigned firstCellOffset,
				const vector<unsigned>& newCellOrder);

   void   placeTerms();	//equally spaces the terminals around the edge of
			//the layout region.


   double evalHPWL       (bool usePinLocs=false)  const;
   double evalMSTWL      (bool usePinLocs=false) const;
   double evalWeightedWL (bool usePinLocs=false) const;

   double evalNetsHPWL   (const vector<unsigned>& netsToEval, 
			  bool usePinLocs=false) const;


   const HGraphWDimensions&	getHGraph() const { return *_hgWDims; }


   bool isConsistent();	

	//save a gnu-plot usable file drawing the sub-rows
   void printRows(const char* filename);

   void   saveAsSCL         (const char* baseFileName) const; 
   void   saveAsNodesNetsWts(const char* baseFileName) const;
   void   saveAsNetDAre     (const char* baseFileName) const;
   void   savePlacement     (const char* plFileName)   const;
// 	{ _placement.save(plFileName, _hgWDims->getNumTerminals());}

   void   saveAsSpatialConstraints (const char* constFileName) const;

   //added by sadya. plots the nodes in gnuplot readable format
   void   saveAsPlot (char* command,double xmin,double xmax,
 		      double ymin,double ymax, const char* baseFileName) const;

   // added by sadya to remove overlaps in a given placement by using the
   //whitespace in a subrow
   void remOverlaps(void);

   // added by sadya to remove overlaps in a given placement by using the
   //whitespace in a subrow and above subrows
   void remOverlapsVert(void);


   //added by sadya to improve row ironing
   void   permuteCellsAndWSInSR (RBPSubRow& sr, unsigned firstCellOffset,
				const vector<unsigned>& newCellOrder);

   //added by sadya to update placement of 2 dim ironed cells
   void   updateIronedCells(const vector<unsigned>& movables,
			    const Placement& soln,
			    RBPSubRow& subrow1,
			    RBPSubRow& subrow2 );


   //added by sadya to update placement of 2 dim ironed cells with clustering
   //L->R
   void updateIronedCellsLR(const vector< vector<unsigned> >& movables,
			    const Placement& soln,
			    RBPSubRow& subrow1,
			    RBPSubRow& subrow2 );

   //added by sadya to update placement of 2 dim ironed cells with clustering
   //R->L
   void updateIronedCellsRL(const vector< vector<unsigned> >& movables,
			    const Placement& soln,
			    RBPSubRow& subrow1,
			    RBPSubRow& subrow2 );


   //added by sadya to calculate the HPWL of a group of cells
   // will be used in overlap removal and rowIroning
   double calcInstHPWL(vector<unsigned> cellIds);

   //added by sadya to save LEF/DEF pair from current RB Placement
   //Generates own macro cells and routing information
   void saveLEFDEF(const char* baseFileName);

   //added by sadya to get the BBox of the layout core region
   BBox getBBox(void);
};
    class RBPlacement_Parameters
    {
     public:

        enum spaceCellsAlgType { EQUAL_SPACE, WITH_PIN_ALG1, WITH_PIN_ALG2 };

	Verbosity       verb;
 	unsigned	numRowsToRemove; //rbplace will remove this
					//many coreRows from the bottom of 
					//the design
        spaceCellsAlgType spaceCellsAlg;  // spaceCells algorithm to use
	//added by sadya
	bool            remCongestion;  //used to remove congestion by blowing
	                                //up cell widths. to be used only on
	                                // routed placements
	
	RBPlacement_Parameters(int argc, const char **argv);
    };

typedef RBPlacement::Parameters RBPlaceParams;

class CompareCellIdsByX
{
    const Placement& _placement;
  public:
    CompareCellIdsByX(const Placement& place)
	:_placement(place)
	{}

};


//added by sadya to generate LEF/DEF files from RBPlacement format
//used to sort the master nodes
struct masterNodeInfo
{
  vector<double> nodeInfo;
  char name[20];
  unsigned index;
  vector<char *> pinNames;
  vector<Point> pinOffsets;
};

class nodesMasInfo //this class maintains the mapping between node and master
                   //node
{
public:
  vector<unsigned> masterNodeIndex;
  
  nodesMasInfo(unsigned numCells)
  {
    masterNodeIndex.resize(numCells);
  }

  void putIndex(unsigned masIndex, unsigned index)
  {
    masterNodeIndex[index] = masIndex;
  }

  unsigned getMasterIndex(unsigned index)
  {
    return masterNodeIndex[index];
  }
};


class PartitioningProblemForCapo;
class CapoSplitterParams;

class CapoPlacer
{

  public:

    CapoPlacer(RBPlacement& rbplace, const CapoParameters& params);
//             const vector<char*>* altCellNames=0);
    CapoPlacer(RBPlacement& rbplace, const char* hclFileName,
	       const char* blockPLFileName, const CapoParameters& params);

    ~CapoPlacer()
    {
       unsigned k;
       for(k=0; k!=_layout[0].size(); k++)
       {
          if (_layout[0][k]) delete _layout[0][k];
	  _layout[0][k] = NULL;
       }
       for(k=0; k!=_layout[1].size(); k++)
       {
          if (_layout[1][k]) delete _layout[1][k];
	  _layout[1][k] = NULL;
       }
    }

    double estimateWL();

    const vector<double>&   getBBoxPerLayer() const 
		{ return  _estBBoxWLPerLayer; }
    const vector<unsigned>& getCutPerLayer()  const
		{ return  _totCutPerLayer;    }

    const vector<unsigned>& getNodesInEachBlock() const
		{ return _nodesInEachBlock; }
    const vector<unsigned>& getTerminalsToEachBlock() const
		{ return _terminalsToEachBlock; }
    const vector<const CapoBlock*>& getCellToBlockMap() const
		{ return _cellToBlockMap;}

    Point getPinLocation(unsigned cellId, unsigned netId) const;

    const HGraphWDimensions& getNetlistHGraph() const
		{ return _hgraphOfNetlist;}

    const CapoParameters& getParams() const { return _params;}

    const PlacementWOrient& getPlacement() const { return _placement;}

//  const vector<char*>* getAltCellNames() const { return _altCellNames; }

   public:

    void printCutDensityStats();
    void printSmallProblemStats();
    void printNetlistStats();

    void getNetCutInfo(vector<unsigned>& externalCutNets,
                       vector<unsigned>& containedNets) const;
    unsigned getTotalNetCut() const;

    void getBlockMembership(vector<int>& nodeBlock) const;
//    void printBlockMembership(ostream& os) const;
    void printBlockMembership(const char* fileName) const;

    void getHierCellNames(vector<const char*>& cellNames) const;
//    void printHierCellNames(ostream& os) const;
    void printHierCellNames(const char* fileName) const;

    void saveSmallProblem(const PartitioningProblemForCapo& problem,
                                  const CapoBlock& block);


};


class CapoSplitterParams
{
  public:

        //if doRepartitioning, then the first tolerance is always
        //20%, and the second tolerance follows the parameters below
    bool        doRepartitioning;
 
        //if <useWSTolMethod>, then use the computation from the
	//whitespace report for horizontal cuts.  Otherwise,
	//use <constantTolerance>. Note that this is only for
	//h-cuts.  V-cuts always use a tolerance of 
	//<constantTolerance>, and shift the cutline.  
	//If doRepartitioning is enabled, then v-cuts will 
	//repartition with 1% tolerance after shifting.
    bool        useWSTolMethod;
    double      constantTolerance;
 
    unsigned    numMLSets;

    bool	useNameHierarchy;
    char*       delimiters;
 
    Verbosity   verb;
 
    CapoSplitterParams(int argc, const char **argv);
/*
    CapoSplitterParams(Verbosity verbosity = Verbosity("0_0_0"))
        : doRepartitioning(true), useWSTolMethod(true),
          constantTolerance(10), numMLSets(2), 
	  useNameHierarchy(false), delimiters((char*)"/"), verb(verbosity)
        {}
*/
 
   friend ostream& operator<<(ostream&, const CapoSplitterParams& params);
};
                   

enum SeedPlacerType {DumbPlacerSeed, BaryPlacerSeed, WeiszPlacerSeed};
enum ReplaceSmallBlocksType {Never, AtTheEnd, AtEveryLayer};        

class CapoSplitterParams;

class CapoParameters 
{
    void setDefaults();

  public: 
    Verbosity verb;

      //  PARAMETERS FOR TOP-DOWN FLOW
    unsigned stopAtBlocks;  //if !=0, stop when >= this many blocks
                            //default == 0 => run till end.

    ReplaceSmallBlocksType 	replaceSmallBlocks;

    bool     useActualPinLocs;

      //   PARAMETERS EQUALLY APPLICABLE TO EACH LEVEL
    unsigned smallPartThreshold; //for problems of size <= the
				//threshold, use smallPartitioner
    unsigned smallPlaceThreshold; //call the end-case placer on 
				//one-row problems with <= threshold #cells

    bool     doOverlapping;
    unsigned startOverlappingLayer;//the first level to overlap on
    unsigned endOverlappingLayer;  //the last level to overlap at

    bool	 propFromPl;	//do termi-prop w/ results of boxPl
    bool	 useKPl;


      //   PARTITIONER PARAMETERS
    MLPartParams	mlParams;
    SmallPlParams	smplParams;

      //lookAhead > 1 requires use of the LASplitter
    unsigned	lookAhead;

    bool		allowRowSplits;
    CapoSplitterParams  splitterParams;

      //  INFO-TRACKING PARAMETERS
    bool            plotBlocks;
    bool            saveLayerBBoxes;
    bool            saveBlocks;
    bool            saveSmallPlProb;

    CapoParameters();
    CapoParameters(int argc, const char *argv[]);
    CapoParameters(const CapoParameters &srcParams);

    void printHelp();

    friend ostream& operator<<(ostream& os, CapoParameters &params);
};

class MetaPlacerParams
{
   public:
     Verbosity verb;
     bool save;
     bool flip;
     bool skipRowIroning;
  
  //MetaPlacerParams():save(false),flip(false),skipRowIroning(false){};
  MetaPlacerParams(int argc, const char *argv[]);
};

//ostream& operator<< (ostream&, const MetaPlacerParams&);

class MetaPlacer 
{
   public:

//    MetaPlacer(int argc, const char *argv[]);
//    MetaPlacer(RBPlacement& rbplace, int argc, const char *argv[]);
    MetaPlacer(RBPlacement& rbplace, MetaPlacerParams par);
   ~MetaPlacer();
};

class Partitioning;

//:  Enscapsulates data necessary to specify most common partitioning formulations
//   and their solutions, including the hgraph, partition sizes, 
//   locations, capacities, balance tolerances, etc.
class PartitioningProblem
{

  public: 
    

  public:

    PartitioningProblem(
		const HGraphFixed& 		hgraph, 
		const PartitioningBuffer&	solnBuffers,
		const Partitioning&		fixedConstr, // size=#nodes
		const vector<BBox>&		partitions,
		const vector<vector<double> >&	capacities,
	    //	capacities for each partition & weight in absolute units
		const vector<double>&		tolerances,
		//tolerance for each weight, as a fraction of area (ie..0.1 == 10%)
		const vector<unsigned>& 	terminalToBlock,
		const vector<BBox>& 		padBlocks,
		PartitioningProblem_Parameters		        parameters);
    PartitioningProblem(      
                        const char* netDFileName,
						///either .are or .areM
                        const char* areFileName,        
                        const char* blkFileName,
                        const char* fixFileName,
                        const char* solFileName,
		        PartitioningProblem_Parameters  parameters);
    // read the separated data files to get the partitioning problem

    PartitioningProblem(const PartitioningProblem &prob);
    PartitioningProblem(const char* fileNameRoot, 
		        PartitioningProblem_Parameters  parameters);
    // read the basefile to get the partitioinging problem


    virtual ~PartitioningProblem() {_freeDynamicData();}
    void propagateTerminals(double fuzziness=0.0); 
    // fuzziness can be 10.0 (%). ?? function of propagateTerminals 

    bool saveAsNetDAre(const char* baseFileName) const;
    bool saveAsNodesNets(const char* baseFileName) const;

 
    bool saveBestSol(const char* solFileName) const;
    void reserveBuffers(unsigned num); 	
    //makes sure there are at least num buffers. abkfatals if the data is not owned.

    bool isDataOwned() const { return _ownsData; }

    bool partsComeFromGrid() const 
            { return _xTics.size()>1 || _yTics.size()>1; }

    unsigned getNumXParts() const
       { abkfatal2(partsComeFromGrid(),"Partition bboxes ",
          " do not come from a grid"); return _xTics.size()-1;
       }

    unsigned getNumYParts() const
       { abkfatal2(partsComeFromGrid(),"Partition bboxes ",
          " do not come from a grid"); return _yTics.size()-1;
       }
  
    const vector<double>& getXTics() const { return _xTics; }
    const vector<double>& getYTics() const { return _yTics; }

    const HGraphFixed& getHGraph() const 
	    { return *_hgraph; }

	//NOTE: This is a workaround for g++ debug libs on Linux.
	//it has problems w/ the above function call...but using
	//the pointer directly seems to work fine!

    const HGraphFixed* getHGraphPointer() const { return _hgraph;}

    const PartitioningProblem_Parameters& getParameters() const { return _params; }

    PartitioningBuffer& getSolnBuffers()
	    { return *_solnBuffers; }
    const PartitioningBuffer& getSolnBuffers() const
	    { return *_solnBuffers; }
    const Partitioning& getBestSoln() const 
            { return (*_solnBuffers)[getBestSolnNum()]; }
    void  setSolnBuffers(PartitioningBuffer* newbufs);
    // LOOK: deletes the old ones !!! will own the new ones !!! -ILM

    //same as setSolnBuffers except does *not* delete old ones.
    //return value is old buffers.
    PartitioningBuffer  *swapOutSolnBuffers(PartitioningBuffer* newbufs);
    Partitioning        *swapOutFixedConst (Partitioning* fixedConstr)
    {  std::swap(_fixedConstr,fixedConstr); return fixedConstr; }
    HGraphFixed         *swapOutHGraph     (HGraphFixed* hgraph)
    {  std::swap(_hgraph,hgraph);           return hgraph; }
   
    const vector<double>& getCosts()      const { return _costs; }
    const vector<double>& getViolations() const { return _violations; }
    const vector<double>& getImbalances() const { return _imbalances; }
    const Partitioning&  getFixedConstr() const { return *_fixedConstr; }
    const vector<vector<double> >& getCapacities() const 
	   { return *_capacities; }
    const vector<vector<double> >& getMaxCapacities() const 
	    { return *_maxCapacities; }
    const vector<vector<double> >& getMinCapacities() const 
	    { return *_minCapacities; }
    const vector<double>& getTotalWeight() const
	    { return *_totalWeight; }
    const vector<BBox>& getPartitions() const
	    { return *_partitions; }
    const vector<Point>& getPartitionCenters() const 
	    { return *_partitionCenters; }
    double getScalingFactorForBBoxQuantization(unsigned quantizationBound)const;
    const vector<BBox>& getPadBlocks() const
	    { return *_padBlocks; }
    const vector<Point>& getPadBlockCenters() const
	    { return *_padBlockCenters; }
    const vector<unsigned>& getTerminalToBlock() const
	    { return *_terminalToBlock; }
    unsigned getNumPartitions() const
	    { return _partitions->size(); }
    unsigned getBestSolnNum() const 
	    { abkfatal2(_bestSolnNum<_solnBuffers->size(),
                " Best Soln Num not set: ",_solnBuffers->size());
              return _bestSolnNum; 
            }
    void setBestSolnNum(unsigned num)
	    { _bestSolnNum = num; }
    void printLargestCellStats(unsigned N=10) const;
    vector<unsigned> getPinBalances() const;
    void printPinBalances() const;

    const PartitioningProblem_Attributes& getAttributes() const 
	    { return _attributes; }

    PartitioningProblem &operator=(const PartitioningProblem &prob);

//    friend ostream& operator<<(ostream& os, 
//			const PartitioningProblem& prob);

//    friend ostream& operator>>(ostream& os, 
//			const PartitioningProblem& prob);
    // note: the following reads a partitioning problem from the stream
    //	format operator << prints it it.  This is not the same as
    //	the .netD, .are, etc files the fileName ctor uses.

};

class PartitioningProblem_Parameters : public HGraphParameters
    //: Derived form base class HGraphBaseParams.
    // Used to read the parameter: verbosity, from commandline
    {
      public:
        Verbosity			verbosity;
      public:
     //   Parameters(Verbosity VERB):verbosity(VERB) {}
     //   Parameters():verbosity("1_1_1") {}
        PartitioningProblem_Parameters(int argc, const char** argv)
		:HGraphParameters (argc,argv), verbosity(argc,argv) {}
    };

struct PartitioningProblem_Attributes
    //: Used to indicate if this PartProblem has a particular
    //  data item populated. The following are not included, as
    //  they are ALWAYS populated:  hgraph, solnBuffers, fixedConstraints
    {
        unsigned char   hasMaxCap      : 1;
        unsigned char   hasMinCap      : 1;
        unsigned char   hasCap         : 1;
        unsigned char   hasPartBBoxes  : 1;
        unsigned char   hasPadBBoxes   : 1;
        unsigned char   hasPartCenters : 1;
        unsigned char   hasPadCenters  : 1;
        unsigned char   hasTotalWeight : 1;
        unsigned char   hasClusterDegrees : 1;
 
        PartitioningProblem_Attributes()
        : hasMaxCap(false), hasMinCap(false), hasCap(false),
          hasPartBBoxes(false), hasPadBBoxes(false), hasPartCenters(false),
          hasPadCenters(false), hasTotalWeight(false), hasClusterDegrees(false)
        {}

	void clearAll()
        { hasMaxCap = false; hasMinCap = false; hasCap = false;
          hasPartBBoxes = false; hasPadBBoxes = false; 
	  hasPartCenters = false; hasPadCenters = false; 
	  hasTotalWeight = false; hasClusterDegrees = false;
	}

	void setAll()
        { hasMaxCap = true; hasMinCap = true; hasCap = true;
          hasPartBBoxes = true; hasPadBBoxes = true; 
	  hasPartCenters = true; hasPadCenters = true; 
	  hasTotalWeight = true; hasClusterDegrees = true;
	}
    };

class PartitionIds;
class Partitioning;
class PartitioningBuffer;
class PartitioningDoubleBuffer;
class PartitioningProblem;

class BaseMLPart
{

 public:

    BaseMLPart(PartitioningProblem& problem, const BaseMLPart_Parameters& params,
		FillableHierarchy* hier = NULL);

    virtual ~BaseMLPart();
	      
    // ------------- various public inspectors --------------

    double getPartitionArea (unsigned partNumber) const;
    double getPartitionRatio(unsigned partNumber) const;
    double getBestSolnCost() const { return _bestCost; }
    double getAveSolnCost()  const { return _aveCost;  }
    unsigned getNumLegalSolns()  const { return _numLegalSolns; }
    double getUserTime()     const { return _userTime; }

};

#define MLPartParams BaseMLPart_Parameters

class BaseMLPart_Parameters : public PartitionerParams
    {
      public:
	enum SavePartProb { NeverSave, AtAllLastLevels, 
			    AtAllLevels, AtFirstLevelOfFirst, 
			    AtLastLevelOfFirst, AtAllLevelsOfFirst };

	enum VcyclType	  { NoVcycles, Exclusive, Relaxed, Relaxed2,
			    Comprehensive };

	enum TwoPartCalls {ALL, TOPONLY, ALLBUTLAST, NEVER};

	SavePartProb	savePartProb;

	PartitionerType flatPartitioner;
        bool            useBBonTop;

	double		partFuzziness; // measured in %% of the mdist from
				       // the terminal to closest partition
				       // 0 means can propagate only to
				       // propagate to partitions with min. dist
	unsigned	runsPerClTree;
	unsigned	solnPoolOnTopLevel;

	double		toleranceMultiple;	//multiple of the largest 
						//node at the top level

	double		toleranceAlpha;		//used to weight 2nd and 
						//subsequent largest
						//cells in computing top 
						//level tol. A value of 1 
						//means they are given full 
						//weight, 0 means only the 
						//largest node is considered

	TwoPartCalls	useTwoPartCalls;	//on which levels should
						//a partitioner first be 
						//called with the relaxed
						//tol, then the orig.

	unsigned	netThreshold; //remove nets of size > netThreshold.
	VcyclType	Vcycling; 
	unsigned	timeLimit;
	bool		expPrint2Costs; // experimental
	bool		clusterToTerminals;
	bool		seedTopLvlSoln; 
				//if an initial partitioning is passed to
				//ML, it will build the ClusteredHGraphs
				//to respect it.  If seedTopLvlSoln is
				//true, the top-level soln will also
				//respect this partitioning, otherwise it 
				//will be randomly generated.

	unsigned	pruningPercent;
	double		pruningPoint;	//in percent. The costs are compared
					//for pruning at #Leaves*pruningPt%

	unsigned	maxNumPassesAtBottom;

		/*Vcycling parameters.	These are used for the trees/ml runs
		 *done during Vcycling.  There are 2 sets of parameters.*/
	unsigned	vcNumFailures;	//Vcycling stops after numFailures
					//consecutive failures.
	double		vcImproveRatio; //soln quality must improve by
					//improveRatio in order to be 
					//considered not a fail.

		
	double		vc1ClusterRatio; //Vcycling patern#1
	unsigned	vc1FirstLevel;
	double		vc1LevelGrowth;

		
	double		vc2ClusterRatio; //Vcycling patern#2 
	unsigned	vc2FirstLevel;
	double		vc2LevelGrowth;


	ClustHGraphParameters clParams;

	BaseMLPart_Parameters(int argc, const char **argv);

/*
	Parameters() : PartitionerParams(Verbosity("1 1 1")),
		       savePartProb(NeverSave),
                       useBBonTop(false),
		       partFuzziness(0), 
		       runsPerClTree(1),
		       solnPoolOnTopLevel(3),
		       toleranceMultiple(2),
		       toleranceAlpha(0),
		       useTwoPartCalls(TOPONLY),
		       netThreshold(0),//don't threshold
		       Vcycling(Exclusive),
		       timeLimit(0),
		       expPrint2Costs(false),
		       clusterToTerminals(false),
		       seedTopLvlSoln(false),
		       pruningPercent(10000),
		       pruningPoint(30),
		       maxNumPassesAtBottom(0),
		       vcNumFailures(1),
        	       vcImproveRatio(1),
        	       vc1ClusterRatio(2),
                       vc1FirstLevel(200),
                       vc1LevelGrowth(2),
                       vc2ClusterRatio(1.3),
                       vc2FirstLevel(200),
                       vc2LevelGrowth(2),
		       clParams()
		{ maxHillHeightFactor=2; useEarlyStop=true; useClip=0; }
*/
//	Parameters(const Parameters &params);
//	Parameters &operator=(const Parameters &params);

    };


class FillableHierarchy;

class MLPart : public BaseMLPart
{
    void _startWithPartProblem(PartitioningProblem&, ClusteredHGraph& );

    bool _go();	//returns true if the solutions generated are valid
    bool _doMultiLevel();	//returns true if the solns generated are valid
    void _figureBestCost(PartitioningProblem&);

  public:

    MLPart(PartitioningProblem& problem, const MLPartParams& params,
		FillableHierarchy* hier = NULL);
    ~MLPart() {}
};

class HGraphFixed : public HGraphBase
{
  public:
    vector<const char*>	_nodeNames;	//vector of all node names,
					//indexed by nodeId
    vector<const char*>	_netNames;	//vector of all net  names,
					//indexed by netId
    HGNodeNamesMap	_nodeNamesMap;	//map from name to nodeId.
					//the names pointed to by this map
					//are owned by the _nodeNames vector
    HGNetNamesMap	_netNamesMap;	//map from name to netId.
					//the names pointed to by this map
					//are owned by the _netNames vector
  public:

    HGraphFixed(unsigned numNodes  , 
		unsigned numWeights, 
		HGraphParameters param)
	{ _param = param; init(numNodes, numWeights); }

	//this ctor takes:
	// an aux file only
	// a netD and (optionally) and areM
	// a nodes, a nets and (optionally) an wts
	//the can be in any order.  The ctor determins the file
	//types from the filename extensions 

    HGraphFixed(const char* Filename1, 
		const char* Filename2, 
		const char* Filename3, 
		HGraphParameters param);

    HGraphFixed(const HGraphFixed&);

    //virtual ~HGraphFixed();

    unsigned getNumNodes() const { return _nodes.size(); }
    unsigned getNumEdges() const { return _edges.size(); }

    double getAvgNodeDegree() const { return 1.0*getNumPins()/getNumNodes(); }
    double getAvgEdgeDegree() const { return 1.0*getNumPins()/getNumEdges(); }

    itHGFNodeGlobal nodesBegin() const { return _nodes.begin(); }
    itHGFNodeGlobal nodesEnd()   const { return _nodes.end();   }

    itHGFNodeGlobal terminalsBegin() const { return _nodes.begin(); }
    itHGFNodeGlobal terminalsEnd()   const { return _nodes.begin()+_numTerminals; }

    itHGFEdgeGlobal edgesBegin() const { return _edges.begin(); }
    itHGFEdgeGlobal edgesEnd()   const { return _edges.end();   }

    itHGFNodeGlobalMutable nodesBegin(){ return _nodes.begin(); }
    itHGFNodeGlobalMutable nodesEnd()  { return _nodes.end();   }
    itHGFEdgeGlobalMutable edgesBegin(){ return _edges.begin(); }
    itHGFEdgeGlobalMutable edgesEnd()  { return _edges.end();   }

    const HGFNode& getNodeByIdx(unsigned nodeIndex) const
    {
        abkassert3(nodeIndex<getNumNodes(), nodeIndex,
           " is an out-of-range index, max=", getNumNodes());
        return *_nodes[nodeIndex];
    }

    bool  haveSuchNode(const char* name) const
    {
        HGNodeNamesMap::const_iterator nameItr = _nodeNamesMap.find(name);
        return nameItr != _nodeNamesMap.end(); 
    }

    bool  haveSuchNet(const char* name) const
    {
        HGNetNamesMap::const_iterator nameItr = _netNamesMap.find(name);
        return nameItr != _netNamesMap.end(); 
    }

    const HGFNode& getNodeByName(const char* name) const
    {
	HGNodeNamesMap::const_iterator nameItr = _nodeNamesMap.find(name);
	abkassert2(nameItr != _nodeNamesMap.end(), 
		"name not found in getNodeByName: ", name);
        return *_nodes[(*nameItr).second];
    }

    const HGFEdge& getNetByName(const char* name) const
    {
	HGNetNamesMap::const_iterator nameItr = _netNamesMap.find(name);
	abkassert2(nameItr != _netNamesMap.end(), 
		"name not found in getNetByName: ", name);
        return *_edges[(*nameItr).second];
    }


    HGFNode& getNodeByIdx(unsigned nodeIndex)
    {
        abkassert3(nodeIndex<getNumNodes(), nodeIndex,
           " is an out-of-range index, max=", getNumNodes());
        return *_nodes[nodeIndex];
    }

    HGFNode& getNodeByName(const char* name)
    {
	HGNodeNamesMap::const_iterator nameItr = _nodeNamesMap.find(name);
	abkassert2(nameItr != _nodeNamesMap.end(), 
		"name not found in getNodeByName: ", name);
        return *_nodes[(*nameItr).second];
    }

    const HGFEdge& getEdgeByIdx(unsigned edgeIndex) const
    {
        abkassert(edgeIndex<getNumEdges(), "edge index too big");
        return *_edges[edgeIndex];
    }

    HGFEdge& getEdgeByIdx(unsigned edgeIndex)
    {
        abkassert(edgeIndex<getNumEdges(), "edge index too big");
        return *_edges[edgeIndex];
    }

    HGFEdge& getNetByName(const char* name)
    {
	HGNetNamesMap::const_iterator nameItr = _netNamesMap.find(name);
	abkassert2(nameItr != _netNamesMap.end(), 
		"name not found in getNetByName: ", name);
        return *_edges[(*nameItr).second];
    }

    const Permutation& getNodesSortedByWeights() const
    {
        if (_weightSort.getSize()==0)
            computeNodesSortedByWeights();
        return _weightSort;
    }

    const Permutation& getNodesSortedByDegrees() const
    {
        if (_degreeSort.getSize()==0)
            computeNodesSortedByDegrees();
        return _degreeSort;
    }

    unsigned maxNodeIndex() const { return _nodes.size()-1; }
    unsigned maxEdgeIndex() const { return _edges.size()-1; }

    virtual HGFEdge* addEdge(HGWeight weight);
    virtual HGFEdge* fastAddEdge(unsigned numPins, HGWeight weight);
    void adviseNodeDegrees(const vector<unsigned>& nodeDegrees);

    virtual void finalize();
    // after changes are made to the graph, this method must be called
    // before any methods can be called that rely on src/snk/srcSnk distinction

    void saveAsNetDAre(const char* baseName) const;
    virtual void saveAsNodesNetsWts(const char* baseName) const;

    virtual bool isConsistent() const;
    virtual void sortAsDB();

    void printEdgeSizeStats(ostream& str=cout)   const;
    void printEdgeWtStats(ostream& str=cout)     const;
    void printNodeWtStats(ostream& str=cout)     const;
    void printNodeDegreeStats(ostream& str=cout) const;

    friend ostream& operator<<(ostream& out, const HGraphFixed& graph);
};

class HGfromDB : public HGraphFixed
{
    public:
        HGfromDB(const DB& db);
};


