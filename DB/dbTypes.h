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









// CHANGES:
//  970729  ilm  added a default constructor for dbCellType
//                as well as MacroUNITIALIZED constrant and bool isInitialized()
//  970804  ilm  changed what looked like a typo in class dbMasterCellType
//                         (enum LClass into enum MClass)
//               made enums public in all type classes and added "direct"
//               constructors (as requested by Mike)
//  970805  aec  DB is now a friend to dbMasterType, so postprocessor can
//			set the LGC data.
//		 added private function setCoreClass(char*) to dbMasterCellType
//
//  970808  mro changed "UnknownCC" to "UnknownCoreClass"
//  970814  aec changed error messages on dbCellType
//  970816  aec added dbMasterCellType::isCoreUnknown()
//  970816  aec added const to masterPinType operator()
//  970817  aec fixed dbMasterPin::operator()
//  970817  ilm moved a lot of functionality into dbTypes.cxx
//  970818  aec updated dbMasterCellTypes, dbCellTypes, and dbMasterPinTypes
//			to dbTypes1.0 spec
//  970827  aec  removed dbTypes:: CoreUNKNOWN
//  970904  ilm  removed ctainerRows, itRow, added ctainerRowsGlobal, 
//               ctainerRowsLocal, itRowLocal, itRowGLobal; similarly for
//               tracks
//  971023  mro  Changed direct dbMasterCellTypes ctor so that it maps
//               ENDCAP to PAD, same as the string-based ctor
//  990112  s.m  added ctainerMasterVias, ctainerRoutingElement,
//               ctainerRoutedNet, itMasterVia, itRoutingElement, itRoutedNet

//all classes and enums for newDB Cells
#ifndef __DB_TYPES__
#define __DB_TYPES__
#include <vector>
using std::vector;
#include "ABKCommon/abkcommon.h"
#include "dbDefines.h"

//forward definitions
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
 
private:
        static char text[17];
        
        unsigned char    _mClass      : 3;
        unsigned char    _subClass    : 3;

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

 
        bool operator==(dbMasterCellType mct) const;
        
        bool operator!=(dbMasterCellType mct) const;
        
        operator char*() const;
};   

inline bool dbMasterCellType::operator==(dbMasterCellType mct) const
{ 
    if(_mClass != mct._mClass)    return false;
    if(_mClass != CORE)           return true;
    return _subClass == mct._subClass ;
}

inline bool dbMasterCellType::operator!=(dbMasterCellType mct) const
{ 
   if(_mClass != mct._mClass)    return true;
   if(_mClass != CORE)           return false;
   return _subClass != mct._subClass ;
}

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

private:    
    
    static char text[17];
    
    unsigned char _mType   : 3;
    unsigned char _subType : 3;
    
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
        
    bool operator==(dbCellType ct) const;
    bool operator!=(dbCellType ct) const;
    operator char*() const;
};


inline dbCellType::dbCellType(char* txt)
{
    abkfatal(0,"dbCellType constructor is not implemented yet");
    //dbCellType, possibly different from the master type is not
    // contained in LEF. It can be inferred by the postprocessor
    // (using, e.g., the master type) -ILM
    (void)txt;
} 


inline bool dbCellType::operator==(dbCellType ct) const
{
    if(_mType != ct._mType) return false;
    if(_mType != IO && _mType != CORE) return true;
    if(_subType == ct._subType) return true;
    return false;
}

inline bool dbCellType::operator!=(dbCellType ct) const
{ 
    if(_mType != ct._mType) return true;  
    if(_mType != IO && _mType != CORE) return false;
    if(_subType == ct._subType) return false;
    return true;        
} 

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

private:

    static char text[9];
    unsigned char   _use : 3;
    
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
        
        bool operator==(dbNetType nt){return _use == nt._use;}
        bool operator!=(dbNetType nt){return _use != nt._use;}
        operator char*() const;
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

private:
    static char text[35];
    unsigned char _shape : 2;
    unsigned char _use   : 3;
    unsigned char _dir   : 3;
    
public:
        
        dbMasterPinType(char const* nDir, char const* nUse, char const* nShape);
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
        
        bool operator==(dbMasterPinType mpt) const
        {
          return (_shape == mpt._shape && _use == mpt._use && _dir == mpt._dir);
        }
        
        bool operator!=(dbMasterPinType mpt) const
        {        
          return (_shape != mpt._shape && _use != mpt._use && _dir != mpt._dir);
        }  
        
        operator char*() const;
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



#endif

