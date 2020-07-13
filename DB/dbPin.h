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









//  970729 ilm   changed dbMasterPin::_offset to dbMasterPin::_center
//               according to Wenyong's comment
//  970729 ilm   commented out dbPin::_name
//  970731 ilm   added const to dbPin::getMasterPinName()
//                        (compiler complained)
//  970807 aec   added isConnected()
//			added abkassert in getNet()
//  970807 ilm   added _index to dbPin, changed ctor etc
//
//  970812 aec   added getPorts
//  970812 ilm   added #include "plGeom.h"
//  970816 aec   added getType to both pin and masterpin
//  970819 aec   added friend class dbNetlist to dbPin
//  970826 ilm   corrected a bug in dbMasterPin::getPorts()
//  970826 ilm   changed dbMasterPin::_mustJoin to ctainerMasterPinsLocal
//                    added bool isJoint() and getJointPins()
//  970905 aec   added ComparePinsByCellAndDir and ComparePinsByDirAndName
//			for dbPins and dbMasterPins
//  971130 ilm   added "prohibiting" copy-ctor and op= for dbMasterPin and dbPin

#ifndef     __DB_PIN_H_
#define     __DB_PIN_H_

#include "Geoms/plGeom.h"
#include "dbDefines.h"
#include "dbCell.h"
#include "dbTypes.h"
#include "dbContext.h"

class dbMasterPin 
{            // create MasterPins first, MasterCell afterwards
    MakeFriendsWithParsers

    private:

        char                * _name;
        dbMasterPinType       _type;
        dbMasterCell const   *_cell; //can't be a reference because it's set
                                     //after construction
        const BBox            _bbox;
        const Point           _center;
        
        double _riseDelay;
        double _fallDelay;
        double _riseResist;
        double _fallResist;
        double _inputCap;
        
        // these are for detailed placement
        ctainerLayerGeoms     _ports;
        ctainerMasterPinsLocal*   _mustJoin;
        // significant changes to storing LEQtypes are being discussed

        dbMasterPin(const dbMasterPin& from):_type(from._type)
        { abkfatal(0,"Can't copy-construct dbMasterPin"); }
        void operator=(const dbMasterPin&) const
        { abkfatal(0,"Can't assign dbMasterPin"); }

    public:
        dbMasterPin(char const* name, char const* dir,
                    char const* use, char const* shape, BBox bbox,
                    Point center)
                                 :_type(dir, use, shape), _cell(NULL),
			          _bbox(bbox),_center(center),_mustJoin(NULL)
            {_name = new char[strlen(name)+1];strcpy(_name,name);}
        
        ~dbMasterPin();
        
        void setMasterCell(dbMasterCell const &nCell) {_cell = &nCell;}
        
        dbMasterCell const   &getCell()   const { return *_cell; }
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
    MakeFriendsWithParsers
    friend class dbNetlist;

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
     void operator=(const dbPin&) const
      { abkfatal(0,"Can't assign dbPin"); }

    public:
//   If pin names are ever used, make sure that they are initialized to NULL
//   by the parsers which don't use them (see ctor), e.g. LEFDEFq. ILM
        dbPin(dbMasterPin const &master, unsigned index/*,const char * name */):
              _index(index), _master(master),_cell(NULL),_net(NULL) 
       { /* if (_name) _name = new char[strlen(name)+1];strcpy(_name,name); */}
        
        ~dbPin() { /* if (_name) delete[] _name; */ }

        void setCell(dbCell const& cell){_cell = &cell;}
        void setNet (dbNet  const&  net){_net  = &net; } 
        
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

    bool operator() ( dbPin* p1, dbPin* p2) const;
    bool operator() ( dbMasterPin* p1, dbMasterPin* p2) const;
};

class ComparePinsByCellIndexAndDir
{
   public:
    ComparePinsByCellIndexAndDir(){}

    bool operator() ( dbPin* p1, dbPin* p2) const;
    bool operator() ( dbMasterPin* p1, dbMasterPin* p2) const;
};


#endif


