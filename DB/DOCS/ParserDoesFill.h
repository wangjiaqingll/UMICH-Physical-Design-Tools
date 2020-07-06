class dbNetlist 
{
    ctainerMasterCells       _cellLib;
    
    ctainerCellsGlobal       _cells;
    ctainerNetsGlobal        _nets;
    ctainerNetsGlobal        _sNets; //special nets
   


    XXXX  equivClassesMasterCells _cellLEQs;
    XXXX  equivClassesMasterCells _cellEEQs;
    XXXX  equivClassesMasterPins  _pinLEQs;
    
};

class dbNet
{

    char *      	_name;
    unsigned   		_index;
    const dbNetType  	_type;        // regular, timing, clock
    ctainerPinsLocal   	_pins;


    XXXX  ctainerLayerGeoms  	_wiring; 
		     // should be empty except for special nets.
    
};


class dbMasterPin 
{
        char                * _name;
        dbMasterPinType       _type;
        dbMasterCell const   *_cell; //can't be a reference because it's set
                                     //after construction
        const BBox            _bbox;
        const Point           _center;
       


        XXXX  double _riseDelay;
        XXXX  double _fallDelay;
        XXXX  double _riseResist;
        XXXX  double _fallResist;
        XXXX  double _inputCap;
        
        // these are for detailed placement
        XXXX  ctainerLayerGeoms     _ports;
        XXXX  bool                  _mustJoin;
};


class dbPin 
{

    unsigned             _index;
    const dbMasterPin  & _master;
    const dbCell       * _cell;  // cell and net ptrs can't be references
    const dbNet        * _net;   // because they are set after pin is
                                 // included into a cell/net, while
                                 // the pin is constructed before that
};

class dbMasterCell 
{
    char*      _name;
    
    dbMasterCellType _type;
   
	 
    XXXX  double      _fallDelay;
    XXXX  double      _riseDelay;
    
    const double     _width;
    const double     _height;
    
    Point            _origin;    // iml: this is a technicality.
                                 // It shifts the origin from the lower left
                                 // corner in macro descrition(LEFDEF5.0 p.1-48)
    
    const ctainerMasterPinsLocal _pins;
    
    //  These are for Detailed Placement. We keep them always since the
    //  overhead is small. Note: this is still a very approximate sketch.
    
    XXXX  ctainerLayerGeoms    _obstacles; //from MACRO OBSTRUCTION in LEF
    const Symmetry       _symmetry;
    const dbSite&        _site;    // added &    - ILM
};

class dbCell 
{
        char*               _name;
        unsigned            _index;
        dbMasterCell const &_master; 
        ctainerPinsLocal _pins;
};

