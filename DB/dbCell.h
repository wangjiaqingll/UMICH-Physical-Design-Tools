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









// CHANGES
// 970729  ilm    made dbMasterCell::_site a reference to dbSite (was dbSite)
// 970729  ilm    dbCell::dbCell does not take char * type anymore as
//                _type will be set by postProcessor
// 970801  aec    Added induceNetsFromPins, to be called by the post-processor
// 970804  ilm    Added three accessors to dbMasterCell
// 970804  aec    Changed MasterCell c'tor to set in/outDegree
// 970808  mro    Added new dbMasterCell ctor using enum types and obstacles
// 970814  aec    Changed isMarked..addes isCore * isPad to clauses.
//			added friend class DB
// 970815  aec    added const to 'loopBegin/End' functions
// 970816  aec    changed setTypeFromMaster.
//			added dbMasterCell::isCoreUnknown
// 970817  ilm    added "compare" classes from dbSortComp.h
// 970817  ilm    changed dbCell::isMarked() to dbCell::isLatchNode()
//                        introduced isTimingNode(), isIgnoredTimingNode()
//                        scrapped isUnMarked() and isIgnored()
// 970817  aec    added isPXXX() to dbMasterCell to use new dbMasterCellType
// 970820  ilm    changed char in bit-fields in dbCell to unsigned char
//                        for Windows portability
// 970819  mro    added #include dbContext.h
// 970825  ilm    improved diagnostics for nested forall macros
// 970827  ilm    added dbMasterCell::_setsOfJointPins (no accessors yet)
// 970827  aec    added class CompareObstaclesByXmin
// 970827  aec    removed dbCell::isCoreUNKNOWN
// 970905  aec    removed const from dbMasterCell::_pins, so they can be
//			sorted
// 970911  mro    made non-text-based ctor for dbMasterCellType include
//                the pad subclass
// 970919  pat    add reOrientRec signature to dbMasterCell
// 971130  ilm    added "prohibiting" copy-ctor and op= 
//                      for dbMasterCell and dbCell
// 971130  ilm    corrected a bug in dbMasterCell: 
//                 getOutdegree() actually returned _indegree
//         ilm    changed return type of dbMasterCell::getType()
//                and dbCell::getType() to dbMasterCellType and dbCellType resp.
//         ilm    added getHeight(), getWidth(), getIndegree() and getOutdegre()
//                 for dbCell (looks up the master's params)
// 980108  mro    constified inputs to CompareCellsByName

//all classes and enums for newDB Cells
#ifndef     __DB_CELL_H_
#define     __DB_CELL_H_

#include "Placement/symOri.h"
#include "dbDefines.h"
#include "dbPin.h"
#include "dbContext.h"
#include "dbTypes.h"

//: macro
class dbMasterCell 
{
    MakeFriendsWithParsers
    friend class dbNetlist;
    friend class DB;

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
    void operator=(const dbMasterCell&) const
        { abkfatal(0,"Can't assign dbMasterCell"); }

    
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
    MakeFriendsWithParsers
    friend class dbNetlist;
    friend class DB;

    private:
        
        char*               _name;
        unsigned            _index;
        dbMasterCell const &_master; 
        dbCellType          _type;
        
        ctainerPinsLocal _pins;  // _cellPins can have two pins in same net
        ctainerNetsLocal _nets;  // _cellNets contains no duplicates
        
        static char  _forAllPinsFlag;
        static char  _forAllNetsFlag;

        //next member should only be used by parser
        dbCell *_nextCellSameHash;
        
        // orientation, row number, origin, loc_x, loc_y etc will
        // be stored in spatial part of the database

        bool isDisconnected()  const; // no incident nets

    dbCell(const dbCell& from):_master(from._master),_type(from._type)
        { abkfatal(0,"Can't copy-construct dbCell"); }
    void operator=(const dbCell&) const
        { abkfatal(0,"Can't assign dbCell"); }

        
    public: 
        
        dbCell(const char* name, unsigned index, dbMasterCell const &master,
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
        
        dbPin const *commonPin(const dbNet* net) const;
        bool commonPinIsCellInput(const dbNet*  net) const;
        bool commonPinIsCellOutput(const dbNet* net) const;
        
        bool isInputPad() const;
        bool isOutputPad() const;

	friend ostream& operator<<(ostream& os, const dbCell& cell);

    private: 
    //to be used by the postProcessor
        void induceNetsFromPins(vector<dbNet*> *Snets);
	void sortNets();
        void setTypeFromMaster();
        
};

class CompareCellsByName
{
   public:
    CompareCellsByName(){}

    bool operator() ( dbCell const* c1, dbCell const* c2) const
    {
        int cmpResult = strcasecmp(c1->getName(), c2->getName());
        if(cmpResult < 0) return true;
        else if (cmpResult > 0) return false;
        else  return false;
    }
};

class CompareCellsByStatus
{
   public:
    CompareCellsByStatus(){}

    bool operator() ( dbCell* c1, dbCell* c2) const
    {
        if(c1->isIO() )
        {
            if(c2->isIO() )
                return (c1->getIndex() < c2->getIndex() );
            else
                return true;
        }

        if(c2->isIO() )
            return false;

        return (c1->getIndex() < c2->getIndex() );
    }
};

class CompareMCsByName
{
   public:
    CompareMCsByName(){}

    bool operator() ( dbMasterCell* c1, dbMasterCell* c2) const
    {
        int cmpResult = strcasecmp(c1->getName(), c2->getName());
        if(cmpResult < 0) return true;
        else if (cmpResult > 0) return false;
        else  return false;
    }
};

class CompareCellsByCellIndex
{
   public:
    CompareCellsByCellIndex(){}

    bool operator() ( dbCell* c1, dbCell* c2) const
    {
        return ( c1->getIndex() < c2->getIndex() );
    }
};

class CompareCellsByDir
{
   private:
        dbNet* _curNet;

   public:
    CompareCellsByDir(dbNet *curNet): _curNet(curNet) {}

    bool operator() ( dbCell* c1, dbCell* c2) const;
};

class CompareObstaclesByXmin
{
  public:
    CompareObstaclesByXmin(){}

    bool operator() (dbLayerGeom* o1, dbLayerGeom* o2) const;
};

// ======================  IMPLEMENTATIONS ========================

inline void dbCell::loopPinsBegin() const
{ 
    abkassert(!_forAllPinsFlag, 
        "ERROR: nesting of forallPins macros is invalid"); 
    _forAllPinsFlag = true; 
} 

inline void dbCell::loopPinsEnd(itPinLocal itr) const
{ 
    if(itr == pinsEnd())_forAllPinsFlag = false;
} 

inline void dbCell::loopNetsBegin() const
{
    abkassert(!_forAllNetsFlag,
        "ERROR: nesting of forallNets macros is invalid");
    _forAllNetsFlag = true;
}

inline void dbCell::loopNetsEnd(itNetLocal itr) const
{
    if(itr == netsEnd())_forAllNetsFlag = false;
}

inline bool dbCell::isDisconnected() const
// no incident nets
{
    if(_nets.size()==0) return true;
    return false;
}

inline bool dbCell::isTimingNode() const
{
    return ! isDisconnected();
}

inline bool dbCell::isIgnoredTimingNode() const 
// => isTimingNode(), but ignored for latch timing analysis
{
    if (isDisconnected()) return false;
    else if(isIO() && isPOther())  return true; //Power, Spacer pads
    else if(_master.isBlock() || _master.isRing() || _master.isCover() )
        return true;
    else return false;
}

inline bool dbCell::isLatchNode() const  
// => isTimingNode(), former isMarked()
{
    if (isDisconnected()) return false;
    else if ((isCore() && isSC()) || (isIO() && (isPI() || isPO() || isPIO())))
            return true;
    else return false;
}

#endif
