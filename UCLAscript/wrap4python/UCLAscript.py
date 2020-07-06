# This file was created automatically by SWIG.
import UCLAscriptc
class dbMasterCellType:
    COVER = UCLAscriptc.dbMasterCellType_COVER
    RING = UCLAscriptc.dbMasterCellType_RING
    BLOCK = UCLAscriptc.dbMasterCellType_BLOCK
    CORE = UCLAscriptc.dbMasterCellType_CORE
    PAD = UCLAscriptc.dbMasterCellType_PAD
    ENDCAP = UCLAscriptc.dbMasterCellType_ENDCAP
    EXTPIN = UCLAscriptc.dbMasterCellType_EXTPIN
    MCUNKNOWN = UCLAscriptc.dbMasterCellType_MCUNKNOWN
    FLIPFLOP = UCLAscriptc.dbMasterCellType_FLIPFLOP
    LATCH = UCLAscriptc.dbMasterCellType_LATCH
    INVERTER = UCLAscriptc.dbMasterCellType_INVERTER
    BUFFER = UCLAscriptc.dbMasterCellType_BUFFER
    NAND = UCLAscriptc.dbMasterCellType_NAND
    NOR = UCLAscriptc.dbMasterCellType_NOR
    CoreUNKNOWN = UCLAscriptc.dbMasterCellType_CoreUNKNOWN
    INPUT = UCLAscriptc.dbMasterCellType_INPUT
    OUTPUT = UCLAscriptc.dbMasterCellType_OUTPUT
    INOUT = UCLAscriptc.dbMasterCellType_INOUT
    POWER = UCLAscriptc.dbMasterCellType_POWER
    SPACER = UCLAscriptc.dbMasterCellType_SPACER
    PadUNKNOWN = UCLAscriptc.dbMasterCellType_PadUNKNOWN
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbMasterCellType,args)
        self.thisown = 1

    def setCoreClass(*args):
        val = apply(UCLAscriptc.dbMasterCellType_setCoreClass,args)
        return val
    def isCover(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isCover,args)
        return val
    def isRing(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isRing,args)
        return val
    def isBlock(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isBlock,args)
        return val
    def isCore(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isCore,args)
        return val
    def isPad(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isPad,args)
        return val
    def isEndCap(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isEndCap,args)
        return val
    def isExtPin(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isExtPin,args)
        return val
    def isUnknown(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isUnknown,args)
        return val
    def isFlipFlop(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isFlipFlop,args)
        return val
    def isLatch(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isLatch,args)
        return val
    def isInverter(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isInverter,args)
        return val
    def isBuffer(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isBuffer,args)
        return val
    def isNAND(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isNAND,args)
        return val
    def isNOR(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isNOR,args)
        return val
    def isCoreUnknown(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isCoreUnknown,args)
        return val
    def isPI(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isPI,args)
        return val
    def isPO(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isPO,args)
        return val
    def isPIO(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isPIO,args)
        return val
    def isPPower(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isPPower,args)
        return val
    def isPSpacer(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isPSpacer,args)
        return val
    def isPUnknown(*args):
        val = apply(UCLAscriptc.dbMasterCellType_isPUnknown,args)
        return val
    def __repr__(self):
        return "<C dbMasterCellType instance at %s>" % (self.this,)
class dbMasterCellTypePtr(dbMasterCellType):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbMasterCellType



class dbCellType:
    MacroUNINITIALIZED = UCLAscriptc.dbCellType_MacroUNINITIALIZED
    IO = UCLAscriptc.dbCellType_IO
    BLOCK = UCLAscriptc.dbCellType_BLOCK
    CORE = UCLAscriptc.dbCellType_CORE
    MacroOTHER = UCLAscriptc.dbCellType_MacroOTHER
    MacroUNKNOWN = UCLAscriptc.dbCellType_MacroUNKNOWN
    PI = UCLAscriptc.dbCellType_PI
    PO = UCLAscriptc.dbCellType_PO
    PIO = UCLAscriptc.dbCellType_PIO
    POTHER = UCLAscriptc.dbCellType_POTHER
    PUNKNOWN = UCLAscriptc.dbCellType_PUNKNOWN
    SC = UCLAscriptc.dbCellType_SC
    CC = UCLAscriptc.dbCellType_CC
    CoreOTHER = UCLAscriptc.dbCellType_CoreOTHER
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbCellType,args)
        self.thisown = 1

    def isInitialized(*args):
        val = apply(UCLAscriptc.dbCellType_isInitialized,args)
        return val
    def isIO(*args):
        val = apply(UCLAscriptc.dbCellType_isIO,args)
        return val
    def isBlock(*args):
        val = apply(UCLAscriptc.dbCellType_isBlock,args)
        return val
    def isCore(*args):
        val = apply(UCLAscriptc.dbCellType_isCore,args)
        return val
    def isOther(*args):
        val = apply(UCLAscriptc.dbCellType_isOther,args)
        return val
    def isUnknown(*args):
        val = apply(UCLAscriptc.dbCellType_isUnknown,args)
        return val
    def isPI(*args):
        val = apply(UCLAscriptc.dbCellType_isPI,args)
        return val
    def isPO(*args):
        val = apply(UCLAscriptc.dbCellType_isPO,args)
        return val
    def isPIO(*args):
        val = apply(UCLAscriptc.dbCellType_isPIO,args)
        return val
    def isPOther(*args):
        val = apply(UCLAscriptc.dbCellType_isPOther,args)
        return val
    def isPUnknown(*args):
        val = apply(UCLAscriptc.dbCellType_isPUnknown,args)
        return val
    def isCC(*args):
        val = apply(UCLAscriptc.dbCellType_isCC,args)
        return val
    def isSC(*args):
        val = apply(UCLAscriptc.dbCellType_isSC,args)
        return val
    def isCoreOther(*args):
        val = apply(UCLAscriptc.dbCellType_isCoreOther,args)
        return val
    def __repr__(self):
        return "<C dbCellType instance at %s>" % (self.this,)
class dbCellTypePtr(dbCellType):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbCellType



class dbNetType:
    SIGNAL = UCLAscriptc.dbNetType_SIGNAL
    POWER = UCLAscriptc.dbNetType_POWER
    GROUND = UCLAscriptc.dbNetType_GROUND
    CLOCK = UCLAscriptc.dbNetType_CLOCK
    TIEOFF = UCLAscriptc.dbNetType_TIEOFF
    ANALOG = UCLAscriptc.dbNetType_ANALOG
    UNKNOWN = UCLAscriptc.dbNetType_UNKNOWN
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbNetType,args)
        self.thisown = 1

    def isSignal(*args):
        val = apply(UCLAscriptc.dbNetType_isSignal,args)
        return val
    def isPower(*args):
        val = apply(UCLAscriptc.dbNetType_isPower,args)
        return val
    def isGround(*args):
        val = apply(UCLAscriptc.dbNetType_isGround,args)
        return val
    def isClock(*args):
        val = apply(UCLAscriptc.dbNetType_isClock,args)
        return val
    def isTieOff(*args):
        val = apply(UCLAscriptc.dbNetType_isTieOff,args)
        return val
    def isAnalog(*args):
        val = apply(UCLAscriptc.dbNetType_isAnalog,args)
        return val
    def isUnknown(*args):
        val = apply(UCLAscriptc.dbNetType_isUnknown,args)
        return val
    def __repr__(self):
        return "<C dbNetType instance at %s>" % (self.this,)
class dbNetTypePtr(dbNetType):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbNetType



class dbMasterPinType:
    ShapeABUTMENT = UCLAscriptc.dbMasterPinType_ShapeABUTMENT
    ShapeFEEDTHROUGH = UCLAscriptc.dbMasterPinType_ShapeFEEDTHROUGH
    ShapeRING = UCLAscriptc.dbMasterPinType_ShapeRING
    ShapeUNKNOWN = UCLAscriptc.dbMasterPinType_ShapeUNKNOWN
    SIGNAL = UCLAscriptc.dbMasterPinType_SIGNAL
    ANALOG = UCLAscriptc.dbMasterPinType_ANALOG
    POWER = UCLAscriptc.dbMasterPinType_POWER
    GROUND = UCLAscriptc.dbMasterPinType_GROUND
    CLOCK = UCLAscriptc.dbMasterPinType_CLOCK
    TIEOFF = UCLAscriptc.dbMasterPinType_TIEOFF
    UseUNKNOWN = UCLAscriptc.dbMasterPinType_UseUNKNOWN
    OUT = UCLAscriptc.dbMasterPinType_OUT
    IN = UCLAscriptc.dbMasterPinType_IN
    INOUT = UCLAscriptc.dbMasterPinType_INOUT
    OUTTRISTATE = UCLAscriptc.dbMasterPinType_OUTTRISTATE
    DirFEEDTHROUGH = UCLAscriptc.dbMasterPinType_DirFEEDTHROUGH
    DirUNKNOWN = UCLAscriptc.dbMasterPinType_DirUNKNOWN
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbMasterPinType,args)
        self.thisown = 1

    def isShapeAbutment(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isShapeAbutment,args)
        return val
    def isShapeFeedthrough(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isShapeFeedthrough,args)
        return val
    def isShapeRing(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isShapeRing,args)
        return val
    def isShapeUnknown(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isShapeUnknown,args)
        return val
    def isUseSignal(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isUseSignal,args)
        return val
    def isUseAnalog(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isUseAnalog,args)
        return val
    def isUsePower(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isUsePower,args)
        return val
    def isUseGround(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isUseGround,args)
        return val
    def isUseClock(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isUseClock,args)
        return val
    def isUseTieoff(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isUseTieoff,args)
        return val
    def isUseUnknown(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isUseUnknown,args)
        return val
    def isDirOutput(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isDirOutput,args)
        return val
    def isDirInput(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isDirInput,args)
        return val
    def isDirInOut(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isDirInOut,args)
        return val
    def isDirOutputTristate(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isDirOutputTristate,args)
        return val
    def isDirFeedthrough(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isDirFeedthrough,args)
        return val
    def isDirUnknown(*args):
        val = apply(UCLAscriptc.dbMasterPinType_isDirUnknown,args)
        return val
    def __repr__(self):
        return "<C dbMasterPinType instance at %s>" % (self.this,)
class dbMasterPinTypePtr(dbMasterPinType):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbMasterPinType



class dbContext:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbContext,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_dbContext(self)
    def getMinFeatureX(*args):
        val = apply(UCLAscriptc.dbContext_getMinFeatureX,args)
        return val
    def getMinFeatureY(*args):
        val = apply(UCLAscriptc.dbContext_getMinFeatureY,args)
        return val
    def getUnits(*args):
        val = apply(UCLAscriptc.dbContext_getUnits,args)
        return val
    def getBBox(*args):
        val = apply(UCLAscriptc.dbContext_getBBox,args)
        return val
    def getNumSites(*args):
        val = apply(UCLAscriptc.dbContext_getNumSites,args)
        return val
    def getNumLayers(*args):
        val = apply(UCLAscriptc.dbContext_getNumLayers,args)
        return val
    def getNumRows(*args):
        val = apply(UCLAscriptc.dbContext_getNumRows,args)
        return val
    def getNumTracks(*args):
        val = apply(UCLAscriptc.dbContext_getNumTracks,args)
        return val
    def beginSites(*args):
        val = apply(UCLAscriptc.dbContext_beginSites,args)
        return val
    def endSites(*args):
        val = apply(UCLAscriptc.dbContext_endSites,args)
        return val
    def getSiteByIdx(*args):
        val = apply(UCLAscriptc.dbContext_getSiteByIdx,args)
        if val: val = dbSitePtr(val) ; val.thisown = 1
        return val
    def beginLayers(*args):
        val = apply(UCLAscriptc.dbContext_beginLayers,args)
        return val
    def endLayers(*args):
        val = apply(UCLAscriptc.dbContext_endLayers,args)
        return val
    def getLayerByIdx(*args):
        val = apply(UCLAscriptc.dbContext_getLayerByIdx,args)
        if val: val = dbLayerPtr(val) ; val.thisown = 1
        return val
    def rowsBegin(*args):
        val = apply(UCLAscriptc.dbContext_rowsBegin,args)
        return val
    def rowsEnd(*args):
        val = apply(UCLAscriptc.dbContext_rowsEnd,args)
        return val
    def getRowByIdx(*args):
        val = apply(UCLAscriptc.dbContext_getRowByIdx,args)
        if val: val = dbRowPtr(val) ; val.thisown = 1
        return val
    def beginTracks(*args):
        val = apply(UCLAscriptc.dbContext_beginTracks,args)
        return val
    def endTracks(*args):
        val = apply(UCLAscriptc.dbContext_endTracks,args)
        return val
    def getTracks(*args):
        val = apply(UCLAscriptc.dbContext_getTracks,args)
        return val
    def getTrackByIdx(*args):
        val = apply(UCLAscriptc.dbContext_getTrackByIdx,args)
        if val: val = dbTrackPtr(val) ; val.thisown = 1
        return val
    def getNumHorizTracks(*args):
        val = apply(UCLAscriptc.dbContext_getNumHorizTracks,args)
        return val
    def getNumVertTracks(*args):
        val = apply(UCLAscriptc.dbContext_getNumVertTracks,args)
        return val
    def beginGCellGrids(*args):
        val = apply(UCLAscriptc.dbContext_beginGCellGrids,args)
        return val
    def endGCellGrids(*args):
        val = apply(UCLAscriptc.dbContext_endGCellGrids,args)
        return val
    def getGCellGrids(*args):
        val = apply(UCLAscriptc.dbContext_getGCellGrids,args)
        return val
    def addGCellGrid(*args):
        val = apply(UCLAscriptc.dbContext_addGCellGrid,args)
        return val
    def isConsistent(*args):
        val = apply(UCLAscriptc.dbContext_isConsistent,args)
        return val
    def __repr__(self):
        return "<C dbContext instance at %s>" % (self.this,)
class dbContextPtr(dbContext):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbContext



class dbLayerGeom:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbLayerGeom,args)
        self.thisown = 1

    def getPolygon(*args):
        val = apply(UCLAscriptc.dbLayerGeom_getPolygon,args)
        return val
    def isVia(*args):
        val = apply(UCLAscriptc.dbLayerGeom_isVia,args)
        return val
    def __repr__(self):
        return "<C dbLayerGeom instance at %s>" % (self.this,)
class dbLayerGeomPtr(dbLayerGeom):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbLayerGeom



class dbSite:
    PAD = UCLAscriptc.dbSite_PAD
    CORE = UCLAscriptc.dbSite_CORE
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbSite,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_dbSite(self)
    def getName(*args):
        val = apply(UCLAscriptc.dbSite_getName,args)
        return val
    def getType(*args):
        val = apply(UCLAscriptc.dbSite_getType,args)
        return val
    def getSymmetry(*args):
        val = apply(UCLAscriptc.dbSite_getSymmetry,args)
        if val: val = SymmetryPtr(val) ; val.thisown = 1
        return val
    def getHeight(*args):
        val = apply(UCLAscriptc.dbSite_getHeight,args)
        return val
    def getWidth(*args):
        val = apply(UCLAscriptc.dbSite_getWidth,args)
        return val
    def __repr__(self):
        return "<C dbSite instance at %s>" % (self.this,)
class dbSitePtr(dbSite):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbSite



class dbLayer:
    MASTERSLICE = UCLAscriptc.dbLayer_MASTERSLICE
    CUT = UCLAscriptc.dbLayer_CUT
    OVERLAP = UCLAscriptc.dbLayer_OVERLAP
    ROUTING = UCLAscriptc.dbLayer_ROUTING
    HORIZ = UCLAscriptc.dbLayer_HORIZ
    VERT = UCLAscriptc.dbLayer_VERT
    NOPREFERRED = UCLAscriptc.dbLayer_NOPREFERRED
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbLayer,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_dbLayer(self)
    def getName(*args):
        val = apply(UCLAscriptc.dbLayer_getName,args)
        return val
    def getType(*args):
        val = apply(UCLAscriptc.dbLayer_getType,args)
        return val
    def getDirection(*args):
        val = apply(UCLAscriptc.dbLayer_getDirection,args)
        return val
    def getPitch(*args):
        val = apply(UCLAscriptc.dbLayer_getPitch,args)
        return val
    def getHeight(*args):
        val = apply(UCLAscriptc.dbLayer_getHeight,args)
        return val
    def getThickness(*args):
        val = apply(UCLAscriptc.dbLayer_getThickness,args)
        return val
    def getShrinkage(*args):
        val = apply(UCLAscriptc.dbLayer_getShrinkage,args)
        return val
    def getCapMult(*args):
        val = apply(UCLAscriptc.dbLayer_getCapMult,args)
        return val
    def getEdgeCap(*args):
        val = apply(UCLAscriptc.dbLayer_getEdgeCap,args)
        return val
    def getWidth(*args):
        val = apply(UCLAscriptc.dbLayer_getWidth,args)
        return val
    def getSpacing(*args):
        val = apply(UCLAscriptc.dbLayer_getSpacing,args)
        return val
    def getUnitRes(*args):
        val = apply(UCLAscriptc.dbLayer_getUnitRes,args)
        return val
    def getUnitCap(*args):
        val = apply(UCLAscriptc.dbLayer_getUnitCap,args)
        return val
    def getNumTracks(*args):
        val = apply(UCLAscriptc.dbLayer_getNumTracks,args)
        return val
    def getIndex(*args):
        val = apply(UCLAscriptc.dbLayer_getIndex,args)
        return val
    def __repr__(self):
        return "<C dbLayer instance at %s>" % (self.this,)
class dbLayerPtr(dbLayer):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbLayer



class dbRow:
    HORIZ = UCLAscriptc.dbRow_HORIZ
    VERT = UCLAscriptc.dbRow_VERT
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbRow,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_dbRow(self)
    def getSite(*args):
        val = apply(UCLAscriptc.dbRow_getSite,args)
        if val: val = dbSitePtr(val) ; val.thisown = 1
        return val
    def getOrigin(*args):
        val = apply(UCLAscriptc.dbRow_getOrigin,args)
        return val
    def getDirection(*args):
        val = apply(UCLAscriptc.dbRow_getDirection,args)
        return val
    def getSpacing(*args):
        val = apply(UCLAscriptc.dbRow_getSpacing,args)
        return val
    def getOrientation(*args):
        val = apply(UCLAscriptc.dbRow_getOrientation,args)
        if val: val = OrientPtr(val) ; val.thisown = 1
        return val
    def getNumSites(*args):
        val = apply(UCLAscriptc.dbRow_getNumSites,args)
        return val
    def getName(*args):
        val = apply(UCLAscriptc.dbRow_getName,args)
        return val
    def getLength(*args):
        val = apply(UCLAscriptc.dbRow_getLength,args)
        return val
    def __repr__(self):
        return "<C dbRow instance at %s>" % (self.this,)
class dbRowPtr(dbRow):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbRow



class dbTrack:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbTrack,args)
        self.thisown = 1

    __setmethods__ = {
        "horizontal" : UCLAscriptc.dbTrack_horizontal_set,
        "start" : UCLAscriptc.dbTrack_start_set,
        "numTracks" : UCLAscriptc.dbTrack_numTracks_set,
        "spacing" : UCLAscriptc.dbTrack_spacing_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = dbTrack.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "layer" : lambda x : dbLayerPtr(UCLAscriptc.dbTrack_layer_get(x)),
        "horizontal" : UCLAscriptc.dbTrack_horizontal_get,
        "start" : UCLAscriptc.dbTrack_start_get,
        "numTracks" : UCLAscriptc.dbTrack_numTracks_get,
        "spacing" : UCLAscriptc.dbTrack_spacing_get,
    }
    def __getattr__(self,name):
        method = dbTrack.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C dbTrack instance at %s>" % (self.this,)
class dbTrackPtr(dbTrack):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbTrack



class dbGCellGrid:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbGCellGrid,args)
        self.thisown = 1

    __setmethods__ = {
        "horizontal" : UCLAscriptc.dbGCellGrid_horizontal_set,
        "start" : UCLAscriptc.dbGCellGrid_start_set,
        "numDivisions" : UCLAscriptc.dbGCellGrid_numDivisions_set,
        "spacing" : UCLAscriptc.dbGCellGrid_spacing_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = dbGCellGrid.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "horizontal" : UCLAscriptc.dbGCellGrid_horizontal_get,
        "start" : UCLAscriptc.dbGCellGrid_start_get,
        "numDivisions" : UCLAscriptc.dbGCellGrid_numDivisions_get,
        "spacing" : UCLAscriptc.dbGCellGrid_spacing_get,
    }
    def __getattr__(self,name):
        method = dbGCellGrid.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C dbGCellGrid instance at %s>" % (self.this,)
class dbGCellGridPtr(dbGCellGrid):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbGCellGrid



class dbMasterCell:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbMasterCell,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_dbMasterCell(self)
    def pinsBegin(*args):
        val = apply(UCLAscriptc.dbMasterCell_pinsBegin,args)
        return val
    def pinsEnd(*args):
        val = apply(UCLAscriptc.dbMasterCell_pinsEnd,args)
        return val
    def getNumPins(*args):
        val = apply(UCLAscriptc.dbMasterCell_getNumPins,args)
        return val
    def getName(*args):
        val = apply(UCLAscriptc.dbMasterCell_getName,args)
        return val
    def getHeight(*args):
        val = apply(UCLAscriptc.dbMasterCell_getHeight,args)
        return val
    def getWidth(*args):
        val = apply(UCLAscriptc.dbMasterCell_getWidth,args)
        return val
    def getIndegree(*args):
        val = apply(UCLAscriptc.dbMasterCell_getIndegree,args)
        return val
    def getOutdegree(*args):
        val = apply(UCLAscriptc.dbMasterCell_getOutdegree,args)
        return val
    def getType(*args):
        val = apply(UCLAscriptc.dbMasterCell_getType,args)
        if val: val = dbMasterCellTypePtr(val) ; val.thisown = 1
        return val
    def getObstacles(*args):
        val = apply(UCLAscriptc.dbMasterCell_getObstacles,args)
        return val
    def getSymmetry(*args):
        val = apply(UCLAscriptc.dbMasterCell_getSymmetry,args)
        if val: val = SymmetryPtr(val) ; val.thisown = 1
        return val
    def getSite(*args):
        val = apply(UCLAscriptc.dbMasterCell_getSite,args)
        if val: val = dbSitePtr(val) ; val.thisown = 1
        return val
    def isCover(*args):
        val = apply(UCLAscriptc.dbMasterCell_isCover,args)
        return val
    def isRing(*args):
        val = apply(UCLAscriptc.dbMasterCell_isRing,args)
        return val
    def isBlock(*args):
        val = apply(UCLAscriptc.dbMasterCell_isBlock,args)
        return val
    def isCore(*args):
        val = apply(UCLAscriptc.dbMasterCell_isCore,args)
        return val
    def isPad(*args):
        val = apply(UCLAscriptc.dbMasterCell_isPad,args)
        return val
    def isEndCap(*args):
        val = apply(UCLAscriptc.dbMasterCell_isEndCap,args)
        return val
    def isExtPin(*args):
        val = apply(UCLAscriptc.dbMasterCell_isExtPin,args)
        return val
    def isUnknown(*args):
        val = apply(UCLAscriptc.dbMasterCell_isUnknown,args)
        return val
    def isFlipFlop(*args):
        val = apply(UCLAscriptc.dbMasterCell_isFlipFlop,args)
        return val
    def isLatch(*args):
        val = apply(UCLAscriptc.dbMasterCell_isLatch,args)
        return val
    def isInverter(*args):
        val = apply(UCLAscriptc.dbMasterCell_isInverter,args)
        return val
    def isBuffer(*args):
        val = apply(UCLAscriptc.dbMasterCell_isBuffer,args)
        return val
    def isNAND(*args):
        val = apply(UCLAscriptc.dbMasterCell_isNAND,args)
        return val
    def isNOR(*args):
        val = apply(UCLAscriptc.dbMasterCell_isNOR,args)
        return val
    def isCoreUnknown(*args):
        val = apply(UCLAscriptc.dbMasterCell_isCoreUnknown,args)
        return val
    def isPI(*args):
        val = apply(UCLAscriptc.dbMasterCell_isPI,args)
        return val
    def isPO(*args):
        val = apply(UCLAscriptc.dbMasterCell_isPO,args)
        return val
    def isPIO(*args):
        val = apply(UCLAscriptc.dbMasterCell_isPIO,args)
        return val
    def isPPower(*args):
        val = apply(UCLAscriptc.dbMasterCell_isPPower,args)
        return val
    def isPSpacer(*args):
        val = apply(UCLAscriptc.dbMasterCell_isPSpacer,args)
        return val
    def isPUnknown(*args):
        val = apply(UCLAscriptc.dbMasterCell_isPUnknown,args)
        return val
    def getRiseDelay(*args):
        val = apply(UCLAscriptc.dbMasterCell_getRiseDelay,args)
        return val
    def getFallDelay(*args):
        val = apply(UCLAscriptc.dbMasterCell_getFallDelay,args)
        return val
    def reOrientRec(*args):
        val = apply(UCLAscriptc.dbMasterCell_reOrientRec,args)
        return val
    def __repr__(self):
        return "<C dbMasterCell instance at %s>" % (self.this,)
class dbMasterCellPtr(dbMasterCell):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbMasterCell



class dbCell:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbCell,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_dbCell(self)
    def pinsBegin(*args):
        val = apply(UCLAscriptc.dbCell_pinsBegin,args)
        return val
    def pinsEnd(*args):
        val = apply(UCLAscriptc.dbCell_pinsEnd,args)
        return val
    def netsBegin(*args):
        val = apply(UCLAscriptc.dbCell_netsBegin,args)
        return val
    def netsEnd(*args):
        val = apply(UCLAscriptc.dbCell_netsEnd,args)
        return val
    def loopPinsBegin(*args):
        val = apply(UCLAscriptc.dbCell_loopPinsBegin,args)
        return val
    def loopPinsEnd(*args):
        val = apply(UCLAscriptc.dbCell_loopPinsEnd,args)
        return val
    def loopNetsBegin(*args):
        val = apply(UCLAscriptc.dbCell_loopNetsBegin,args)
        return val
    def loopNetsEnd(*args):
        val = apply(UCLAscriptc.dbCell_loopNetsEnd,args)
        return val
    def getMaster(*args):
        val = apply(UCLAscriptc.dbCell_getMaster,args)
        if val: val = dbMasterCellPtr(val) ; val.thisown = 1
        return val
    def getIndex(*args):
        val = apply(UCLAscriptc.dbCell_getIndex,args)
        return val
    def getName(*args):
        val = apply(UCLAscriptc.dbCell_getName,args)
        return val
    def getType(*args):
        val = apply(UCLAscriptc.dbCell_getType,args)
        if val: val = dbCellTypePtr(val) ; val.thisown = 1
        return val
    def getHeight(*args):
        val = apply(UCLAscriptc.dbCell_getHeight,args)
        return val
    def getWidth(*args):
        val = apply(UCLAscriptc.dbCell_getWidth,args)
        return val
    def getIndegree(*args):
        val = apply(UCLAscriptc.dbCell_getIndegree,args)
        return val
    def getOutdegree(*args):
        val = apply(UCLAscriptc.dbCell_getOutdegree,args)
        return val
    def getNumNets(*args):
        val = apply(UCLAscriptc.dbCell_getNumNets,args)
        return val
    def getNumPins(*args):
        val = apply(UCLAscriptc.dbCell_getNumPins,args)
        return val
    def isIO(*args):
        val = apply(UCLAscriptc.dbCell_isIO,args)
        return val
    def isBlock(*args):
        val = apply(UCLAscriptc.dbCell_isBlock,args)
        return val
    def isCore(*args):
        val = apply(UCLAscriptc.dbCell_isCore,args)
        return val
    def isOther(*args):
        val = apply(UCLAscriptc.dbCell_isOther,args)
        return val
    def isUnknown(*args):
        val = apply(UCLAscriptc.dbCell_isUnknown,args)
        return val
    def isPI(*args):
        val = apply(UCLAscriptc.dbCell_isPI,args)
        return val
    def isPO(*args):
        val = apply(UCLAscriptc.dbCell_isPO,args)
        return val
    def isPIO(*args):
        val = apply(UCLAscriptc.dbCell_isPIO,args)
        return val
    def isPOther(*args):
        val = apply(UCLAscriptc.dbCell_isPOther,args)
        return val
    def isPUnknown(*args):
        val = apply(UCLAscriptc.dbCell_isPUnknown,args)
        return val
    def isCC(*args):
        val = apply(UCLAscriptc.dbCell_isCC,args)
        return val
    def isSC(*args):
        val = apply(UCLAscriptc.dbCell_isSC,args)
        return val
    def isCoreOther(*args):
        val = apply(UCLAscriptc.dbCell_isCoreOther,args)
        return val
    def isTimingNode(*args):
        val = apply(UCLAscriptc.dbCell_isTimingNode,args)
        return val
    def isLatchNode(*args):
        val = apply(UCLAscriptc.dbCell_isLatchNode,args)
        return val
    def isIgnoredTimingNode(*args):
        val = apply(UCLAscriptc.dbCell_isIgnoredTimingNode,args)
        return val
    def getRiseDelay(*args):
        val = apply(UCLAscriptc.dbCell_getRiseDelay,args)
        return val
    def getFallDelay(*args):
        val = apply(UCLAscriptc.dbCell_getFallDelay,args)
        return val
    def commonPin(*args):
        val = apply(UCLAscriptc.dbCell_commonPin,args)
        if val: val = dbPinPtr(val) 
        return val
    def commonPinIsCellInput(*args):
        val = apply(UCLAscriptc.dbCell_commonPinIsCellInput,args)
        return val
    def commonPinIsCellOutput(*args):
        val = apply(UCLAscriptc.dbCell_commonPinIsCellOutput,args)
        return val
    def isInputPad(*args):
        val = apply(UCLAscriptc.dbCell_isInputPad,args)
        return val
    def isOutputPad(*args):
        val = apply(UCLAscriptc.dbCell_isOutputPad,args)
        return val
    def __repr__(self):
        return "<C dbCell instance at %s>" % (self.this,)
class dbCellPtr(dbCell):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbCell



class CompareCellsByName:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareCellsByName,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareCellsByName instance at %s>" % (self.this,)
class CompareCellsByNamePtr(CompareCellsByName):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareCellsByName



class CompareCellsByStatus:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareCellsByStatus,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareCellsByStatus instance at %s>" % (self.this,)
class CompareCellsByStatusPtr(CompareCellsByStatus):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareCellsByStatus



class CompareMCsByName:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareMCsByName,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareMCsByName instance at %s>" % (self.this,)
class CompareMCsByNamePtr(CompareMCsByName):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareMCsByName



class CompareCellsByCellIndex:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareCellsByCellIndex,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareCellsByCellIndex instance at %s>" % (self.this,)
class CompareCellsByCellIndexPtr(CompareCellsByCellIndex):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareCellsByCellIndex



class CompareCellsByDir:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareCellsByDir,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareCellsByDir instance at %s>" % (self.this,)
class CompareCellsByDirPtr(CompareCellsByDir):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareCellsByDir



class CompareObstaclesByXmin:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareObstaclesByXmin,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareObstaclesByXmin instance at %s>" % (self.this,)
class CompareObstaclesByXminPtr(CompareObstaclesByXmin):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareObstaclesByXmin



class dbMasterPin:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbMasterPin,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_dbMasterPin(self)
    def setMasterCell(*args):
        val = apply(UCLAscriptc.dbMasterPin_setMasterCell,args)
        return val
    def getCell(*args):
        val = apply(UCLAscriptc.dbMasterPin_getCell,args)
        if val: val = dbMasterCellPtr(val) ; val.thisown = 1
        return val
    def getName(*args):
        val = apply(UCLAscriptc.dbMasterPin_getName,args)
        return val
    def getCenter(*args):
        val = apply(UCLAscriptc.dbMasterPin_getCenter,args)
        return val
    def getBBox(*args):
        val = apply(UCLAscriptc.dbMasterPin_getBBox,args)
        return val
    def getPorts(*args):
        val = apply(UCLAscriptc.dbMasterPin_getPorts,args)
        return val
    def getType(*args):
        val = apply(UCLAscriptc.dbMasterPin_getType,args)
        return val
    def getRiseDelay(*args):
        val = apply(UCLAscriptc.dbMasterPin_getRiseDelay,args)
        return val
    def getFallDelay(*args):
        val = apply(UCLAscriptc.dbMasterPin_getFallDelay,args)
        return val
    def getRiseResist(*args):
        val = apply(UCLAscriptc.dbMasterPin_getRiseResist,args)
        return val
    def getFallResist(*args):
        val = apply(UCLAscriptc.dbMasterPin_getFallResist,args)
        return val
    def getInputCap(*args):
        val = apply(UCLAscriptc.dbMasterPin_getInputCap,args)
        return val
    def isShapeAbutment(*args):
        val = apply(UCLAscriptc.dbMasterPin_isShapeAbutment,args)
        return val
    def isShapeFeedthrough(*args):
        val = apply(UCLAscriptc.dbMasterPin_isShapeFeedthrough,args)
        return val
    def isShapeRing(*args):
        val = apply(UCLAscriptc.dbMasterPin_isShapeRing,args)
        return val
    def isShapeUnknown(*args):
        val = apply(UCLAscriptc.dbMasterPin_isShapeUnknown,args)
        return val
    def isUseSignal(*args):
        val = apply(UCLAscriptc.dbMasterPin_isUseSignal,args)
        return val
    def isUseAnalog(*args):
        val = apply(UCLAscriptc.dbMasterPin_isUseAnalog,args)
        return val
    def isUsePower(*args):
        val = apply(UCLAscriptc.dbMasterPin_isUsePower,args)
        return val
    def isUseGround(*args):
        val = apply(UCLAscriptc.dbMasterPin_isUseGround,args)
        return val
    def isUseClock(*args):
        val = apply(UCLAscriptc.dbMasterPin_isUseClock,args)
        return val
    def isUseUnknown(*args):
        val = apply(UCLAscriptc.dbMasterPin_isUseUnknown,args)
        return val
    def isDirOutput(*args):
        val = apply(UCLAscriptc.dbMasterPin_isDirOutput,args)
        return val
    def isDirInput(*args):
        val = apply(UCLAscriptc.dbMasterPin_isDirInput,args)
        return val
    def isDirInOut(*args):
        val = apply(UCLAscriptc.dbMasterPin_isDirInOut,args)
        return val
    def isDirOutputTristate(*args):
        val = apply(UCLAscriptc.dbMasterPin_isDirOutputTristate,args)
        return val
    def isDirFeedthrough(*args):
        val = apply(UCLAscriptc.dbMasterPin_isDirFeedthrough,args)
        return val
    def isDirUnknown(*args):
        val = apply(UCLAscriptc.dbMasterPin_isDirUnknown,args)
        return val
    def isJoint(*args):
        val = apply(UCLAscriptc.dbMasterPin_isJoint,args)
        return val
    def getJointPins(*args):
        val = apply(UCLAscriptc.dbMasterPin_getJointPins,args)
        return val
    def __repr__(self):
        return "<C dbMasterPin instance at %s>" % (self.this,)
class dbMasterPinPtr(dbMasterPin):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbMasterPin



class dbPin:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbPin,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_dbPin(self)
    def setCell(*args):
        val = apply(UCLAscriptc.dbPin_setCell,args)
        return val
    def setNet(*args):
        val = apply(UCLAscriptc.dbPin_setNet,args)
        return val
    def getMasterPinName(*args):
        val = apply(UCLAscriptc.dbPin_getMasterPinName,args)
        return val
    def getCell(*args):
        val = apply(UCLAscriptc.dbPin_getCell,args)
        if val: val = dbCellPtr(val) ; val.thisown = 1
        return val
    def getNet(*args):
        val = apply(UCLAscriptc.dbPin_getNet,args)
        if val: val = dbNetPtr(val) ; val.thisown = 1
        return val
    def getIndex(*args):
        val = apply(UCLAscriptc.dbPin_getIndex,args)
        return val
    def getMaster(*args):
        val = apply(UCLAscriptc.dbPin_getMaster,args)
        if val: val = dbMasterPinPtr(val) ; val.thisown = 1
        return val
    def getType(*args):
        val = apply(UCLAscriptc.dbPin_getType,args)
        return val
    def getRiseDelay(*args):
        val = apply(UCLAscriptc.dbPin_getRiseDelay,args)
        return val
    def getFallDelay(*args):
        val = apply(UCLAscriptc.dbPin_getFallDelay,args)
        return val
    def getRiseResist(*args):
        val = apply(UCLAscriptc.dbPin_getRiseResist,args)
        return val
    def getFallResist(*args):
        val = apply(UCLAscriptc.dbPin_getFallResist,args)
        return val
    def getInputCap(*args):
        val = apply(UCLAscriptc.dbPin_getInputCap,args)
        return val
    def isShapeAbutment(*args):
        val = apply(UCLAscriptc.dbPin_isShapeAbutment,args)
        return val
    def isShapeFeedthrough(*args):
        val = apply(UCLAscriptc.dbPin_isShapeFeedthrough,args)
        return val
    def isShapeRing(*args):
        val = apply(UCLAscriptc.dbPin_isShapeRing,args)
        return val
    def isShapeUnknown(*args):
        val = apply(UCLAscriptc.dbPin_isShapeUnknown,args)
        return val
    def isUseSignal(*args):
        val = apply(UCLAscriptc.dbPin_isUseSignal,args)
        return val
    def isUseAnalog(*args):
        val = apply(UCLAscriptc.dbPin_isUseAnalog,args)
        return val
    def isUsePower(*args):
        val = apply(UCLAscriptc.dbPin_isUsePower,args)
        return val
    def isUseGround(*args):
        val = apply(UCLAscriptc.dbPin_isUseGround,args)
        return val
    def isUseClock(*args):
        val = apply(UCLAscriptc.dbPin_isUseClock,args)
        return val
    def isUseUnknown(*args):
        val = apply(UCLAscriptc.dbPin_isUseUnknown,args)
        return val
    def isDirOutput(*args):
        val = apply(UCLAscriptc.dbPin_isDirOutput,args)
        return val
    def isDirInput(*args):
        val = apply(UCLAscriptc.dbPin_isDirInput,args)
        return val
    def isDirInOut(*args):
        val = apply(UCLAscriptc.dbPin_isDirInOut,args)
        return val
    def isDirOutputTristate(*args):
        val = apply(UCLAscriptc.dbPin_isDirOutputTristate,args)
        return val
    def isDirFeedthrough(*args):
        val = apply(UCLAscriptc.dbPin_isDirFeedthrough,args)
        return val
    def isDirUnknown(*args):
        val = apply(UCLAscriptc.dbPin_isDirUnknown,args)
        return val
    def isConnected(*args):
        val = apply(UCLAscriptc.dbPin_isConnected,args)
        return val
    def __repr__(self):
        return "<C dbPin instance at %s>" % (self.this,)
class dbPinPtr(dbPin):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbPin



class ComparePinsByDirAndName:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_ComparePinsByDirAndName,args)
        self.thisown = 1

    def __repr__(self):
        return "<C ComparePinsByDirAndName instance at %s>" % (self.this,)
class ComparePinsByDirAndNamePtr(ComparePinsByDirAndName):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = ComparePinsByDirAndName



class ComparePinsByCellIndexAndDir:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_ComparePinsByCellIndexAndDir,args)
        self.thisown = 1

    def __repr__(self):
        return "<C ComparePinsByCellIndexAndDir instance at %s>" % (self.this,)
class ComparePinsByCellIndexAndDirPtr(ComparePinsByCellIndexAndDir):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = ComparePinsByCellIndexAndDir



class dbNet:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbNet,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_dbNet(self)
    def getNumCells(*args):
        val = apply(UCLAscriptc.dbNet_getNumCells,args)
        return val
    def getNumPins(*args):
        val = apply(UCLAscriptc.dbNet_getNumPins,args)
        return val
    def getDegree(*args):
        val = apply(UCLAscriptc.dbNet_getDegree,args)
        return val
    def getIndex(*args):
        val = apply(UCLAscriptc.dbNet_getIndex,args)
        return val
    def getType(*args):
        val = apply(UCLAscriptc.dbNet_getType,args)
        if val: val = dbNetTypePtr(val) ; val.thisown = 1
        return val
    def getName(*args):
        val = apply(UCLAscriptc.dbNet_getName,args)
        return val
    def getNumOut(*args):
        val = apply(UCLAscriptc.dbNet_getNumOut,args)
        return val
    def getNumInOut(*args):
        val = apply(UCLAscriptc.dbNet_getNumInOut,args)
        return val
    def getNumIn(*args):
        val = apply(UCLAscriptc.dbNet_getNumIn,args)
        return val
    def hasLogicalPin(*args):
        val = apply(UCLAscriptc.dbNet_hasLogicalPin,args)
        return val
    def cellsBegin(*args):
        val = apply(UCLAscriptc.dbNet_cellsBegin,args)
        return val
    def cellsEnd(*args):
        val = apply(UCLAscriptc.dbNet_cellsEnd,args)
        return val
    def sourcesBegin(*args):
        val = apply(UCLAscriptc.dbNet_sourcesBegin,args)
        return val
    def sourcesEnd(*args):
        val = apply(UCLAscriptc.dbNet_sourcesEnd,args)
        return val
    def sourceSinksBegin(*args):
        val = apply(UCLAscriptc.dbNet_sourceSinksBegin,args)
        return val
    def sourceSinksEnd(*args):
        val = apply(UCLAscriptc.dbNet_sourceSinksEnd,args)
        return val
    def sinksBegin(*args):
        val = apply(UCLAscriptc.dbNet_sinksBegin,args)
        return val
    def sinksEnd(*args):
        val = apply(UCLAscriptc.dbNet_sinksEnd,args)
        return val
    def getCellByIdx(*args):
        val = apply(UCLAscriptc.dbNet_getCellByIdx,args)
        if val: val = dbCellPtr(val) ; val.thisown = 1
        return val
    def pinsBegin(*args):
        val = apply(UCLAscriptc.dbNet_pinsBegin,args)
        return val
    def pinsEnd(*args):
        val = apply(UCLAscriptc.dbNet_pinsEnd,args)
        return val
    def getPinByIdx(*args):
        val = apply(UCLAscriptc.dbNet_getPinByIdx,args)
        if val: val = dbPinPtr(val) ; val.thisown = 1
        return val
    def wiringBegin(*args):
        val = apply(UCLAscriptc.dbNet_wiringBegin,args)
        return val
    def wiringEnd(*args):
        val = apply(UCLAscriptc.dbNet_wiringEnd,args)
        return val
    def getWiringLayerGeom(*args):
        val = apply(UCLAscriptc.dbNet_getWiringLayerGeom,args)
        if val: val = dbLayerGeomPtr(val) ; val.thisown = 1
        return val
    def getNumWiringLayerGeoms(*args):
        val = apply(UCLAscriptc.dbNet_getNumWiringLayerGeoms,args)
        return val
    def loopCellsBegin(*args):
        val = apply(UCLAscriptc.dbNet_loopCellsBegin,args)
        return val
    def loopCellsEnd(*args):
        val = apply(UCLAscriptc.dbNet_loopCellsEnd,args)
        return val
    def loopPinsBegin(*args):
        val = apply(UCLAscriptc.dbNet_loopPinsBegin,args)
        return val
    def loopPinsEnd(*args):
        val = apply(UCLAscriptc.dbNet_loopPinsEnd,args)
        return val
    def commonPin(*args):
        val = apply(UCLAscriptc.dbNet_commonPin,args)
        if val: val = dbPinPtr(val) 
        return val
    def commonPinIsCellInput(*args):
        val = apply(UCLAscriptc.dbNet_commonPinIsCellInput,args)
        return val
    def commonPinIsCellOutput(*args):
        val = apply(UCLAscriptc.dbNet_commonPinIsCellOutput,args)
        return val
    def getInputCap(*args):
        val = apply(UCLAscriptc.dbNet_getInputCap,args)
        return val
    def getDriverResist(*args):
        val = apply(UCLAscriptc.dbNet_getDriverResist,args)
        return val
    def isSpecial(*args):
        val = apply(UCLAscriptc.dbNet_isSpecial,args)
        return val
    __setmethods__ = {
        "_cells" : UCLAscriptc.dbNet__cells_set,
        "_pins" : UCLAscriptc.dbNet__pins_set,
        "_wiring" : UCLAscriptc.dbNet__wiring_set,
        "_numOut" : UCLAscriptc.dbNet__numOut_set,
        "_numInOut" : UCLAscriptc.dbNet__numInOut_set,
        "_hasLogicalPin" : UCLAscriptc.dbNet__hasLogicalPin_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = dbNet.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "_cells" : UCLAscriptc.dbNet__cells_get,
        "_pins" : UCLAscriptc.dbNet__pins_get,
        "_wiring" : UCLAscriptc.dbNet__wiring_get,
        "_numOut" : UCLAscriptc.dbNet__numOut_get,
        "_numInOut" : UCLAscriptc.dbNet__numInOut_get,
        "_hasLogicalPin" : UCLAscriptc.dbNet__hasLogicalPin_get,
    }
    def __getattr__(self,name):
        method = dbNet.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C dbNet instance at %s>" % (self.this,)
class dbNetPtr(dbNet):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbNet



class CompareNetsByCellIndex:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareNetsByCellIndex,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareNetsByCellIndex instance at %s>" % (self.this,)
class CompareNetsByCellIndexPtr(CompareNetsByCellIndex):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareNetsByCellIndex



class CompareNetsByDegree:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareNetsByDegree,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareNetsByDegree instance at %s>" % (self.this,)
class CompareNetsByDegreePtr(CompareNetsByDegree):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareNetsByDegree



class CompareNetsByName:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareNetsByName,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareNetsByName instance at %s>" % (self.this,)
class CompareNetsByNamePtr(CompareNetsByName):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareNetsByName



class CompareNetsByDirection:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareNetsByDirection,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareNetsByDirection instance at %s>" % (self.this,)
class CompareNetsByDirectionPtr(CompareNetsByDirection):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareNetsByDirection



class dbNetlist:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbNetlist,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_dbNetlist(self)
    def getCellLEQClass(*args):
        val = apply(UCLAscriptc.dbNetlist_getCellLEQClass,args)
        return val
    def getCellEEQClass(*args):
        val = apply(UCLAscriptc.dbNetlist_getCellEEQClass,args)
        return val
    def getPinLEQClass(*args):
        val = apply(UCLAscriptc.dbNetlist_getPinLEQClass,args)
        return val
    def isConsistent(*args):
        val = apply(UCLAscriptc.dbNetlist_isConsistent,args)
        return val
    def getNumCells(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumCells,args)
        return val
    def getNumPins(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumPins,args)
        return val
    def getNumNets(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumNets,args)
        return val
    def getNumSNets(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumSNets,args)
        return val
    def getNumMasterCells(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumMasterCells,args)
        return val
    def getNumMasterPins(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumMasterPins,args)
        return val
    def getNumCCs(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumCCs,args)
        return val
    def getNumSCs(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumSCs,args)
        return val
    def getNumIOPads(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumIOPads,args)
        return val
    def getNumAllPads(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumAllPads,args)
        return val
    def getNumIOPins(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumIOPins,args)
        return val
    def getNumCoreCells(*args):
        val = apply(UCLAscriptc.dbNetlist_getNumCoreCells,args)
        return val
    def cellsBegin(*args):
        val = apply(UCLAscriptc.dbNetlist_cellsBegin,args)
        return val
    def cellsEnd(*args):
        val = apply(UCLAscriptc.dbNetlist_cellsEnd,args)
        return val
    def netsBegin(*args):
        val = apply(UCLAscriptc.dbNetlist_netsBegin,args)
        return val
    def netsEnd(*args):
        val = apply(UCLAscriptc.dbNetlist_netsEnd,args)
        return val
    def sNetsBegin(*args):
        val = apply(UCLAscriptc.dbNetlist_sNetsBegin,args)
        return val
    def sNetsEnd(*args):
        val = apply(UCLAscriptc.dbNetlist_sNetsEnd,args)
        return val
    def pinsBegin(*args):
        val = apply(UCLAscriptc.dbNetlist_pinsBegin,args)
        return val
    def pinsEnd(*args):
        val = apply(UCLAscriptc.dbNetlist_pinsEnd,args)
        return val
    def pinLibBegin(*args):
        val = apply(UCLAscriptc.dbNetlist_pinLibBegin,args)
        return val
    def pinLibEnd(*args):
        val = apply(UCLAscriptc.dbNetlist_pinLibEnd,args)
        return val
    def cellLibBegin(*args):
        val = apply(UCLAscriptc.dbNetlist_cellLibBegin,args)
        return val
    def cellLibEnd(*args):
        val = apply(UCLAscriptc.dbNetlist_cellLibEnd,args)
        return val
    def getCellByIdx(*args):
        val = apply(UCLAscriptc.dbNetlist_getCellByIdx,args)
        if val: val = dbCellPtr(val) ; val.thisown = 1
        return val
    def getNetByIdx(*args):
        val = apply(UCLAscriptc.dbNetlist_getNetByIdx,args)
        if val: val = dbNetPtr(val) ; val.thisown = 1
        return val
    def getSNetByIdx(*args):
        val = apply(UCLAscriptc.dbNetlist_getSNetByIdx,args)
        if val: val = dbNetPtr(val) ; val.thisown = 1
        return val
    def getPinByIdx(*args):
        val = apply(UCLAscriptc.dbNetlist_getPinByIdx,args)
        if val: val = dbPinPtr(val) ; val.thisown = 1
        return val
    def loopCellsBegin(*args):
        val = apply(UCLAscriptc.dbNetlist_loopCellsBegin,args)
        return val
    def loopCellsEnd(*args):
        val = apply(UCLAscriptc.dbNetlist_loopCellsEnd,args)
        return val
    def loopNetsBegin(*args):
        val = apply(UCLAscriptc.dbNetlist_loopNetsBegin,args)
        return val
    def loopNetsEnd(*args):
        val = apply(UCLAscriptc.dbNetlist_loopNetsEnd,args)
        return val
    def loopSNetsBegin(*args):
        val = apply(UCLAscriptc.dbNetlist_loopSNetsBegin,args)
        return val
    def loopSNetsEnd(*args):
        val = apply(UCLAscriptc.dbNetlist_loopSNetsEnd,args)
        return val
    def postProcess(*args):
        val = apply(UCLAscriptc.dbNetlist_postProcess,args)
        return val
    def processLGC(*args):
        val = apply(UCLAscriptc.dbNetlist_processLGC,args)
        return val
    def evalHalfPerimCost(*args):
        val = apply(UCLAscriptc.dbNetlist_evalHalfPerimCost,args)
        return val
    def evalWHalfPerimCost(*args):
        val = apply(UCLAscriptc.dbNetlist_evalWHalfPerimCost,args)
        return val
    def evalMSTCost(*args):
        val = apply(UCLAscriptc.dbNetlist_evalMSTCost,args)
        return val
    def evalHalfPerimCostNoPins(*args):
        val = apply(UCLAscriptc.dbNetlist_evalHalfPerimCostNoPins,args)
        return val
    def printNetStats(*args):
        val = apply(UCLAscriptc.dbNetlist_printNetStats,args)
        return val
    def __repr__(self):
        return "<C dbNetlist instance at %s>" % (self.this,)
class dbNetlistPtr(dbNetlist):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbNetlist



class dbEasyAccess:
    IntRandom = UCLAscriptc.dbEasyAccess_IntRandom
    IntLessRandom = UCLAscriptc.dbEasyAccess_IntLessRandom
    def __init__(self,this):
        self.this = this

    def populate(*args):
        val = apply(UCLAscriptc.dbEasyAccess_populate,args)
        return val
    def isConsistent(*args):
        val = apply(UCLAscriptc.dbEasyAccess_isConsistent,args)
        return val
    def getCellWidths(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getCellWidths,args)
        return val
    def getCellAreas(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getCellAreas,args)
        return val
    def getWhitespace(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getWhitespace,args)
        return val
    def getNumPortLocations(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getNumPortLocations,args)
        return val
    def getPortLocationByIdx(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getPortLocationByIdx,args)
        return val
    def getNetIds(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getNetIds,args)
        return val
    def getCoreCells(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getCoreCells,args)
        return val
    def getCoreBBox(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getCoreBBox,args)
        return val
    def getPin2CellIdx(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getPin2CellIdx,args)
        return val
    def getCellIdxByPinIdx(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getCellIdxByPinIdx,args)
        return val
    def getCoreCellArea(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getCoreCellArea,args)
        return val
    def getTotalCellWidth(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getTotalCellWidth,args)
        return val
    def getCellByName(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getCellByName,args)
        if val: val = dbCellPtr(val) ; val.thisown = 1
        return val
    def getNetByName(*args):
        val = apply(UCLAscriptc.dbEasyAccess_getNetByName,args)
        if val: val = dbNetPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C dbEasyAccess instance at %s>" % (self.this,)
class dbEasyAccessPtr(dbEasyAccess):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbEasyAccess



class dbSpatial:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_dbSpatial,args)
        self.thisown = 1

    def getSize(*args):
        val = apply(UCLAscriptc.dbSpatial_getSize,args)
        return val
    def isConsistent(*args):
        val = apply(UCLAscriptc.dbSpatial_isConsistent,args)
        return val
    def reorderCells(*args):
        val = apply(UCLAscriptc.dbSpatial_reorderCells,args)
        return val
    def allCellsPlaced(*args):
        val = apply(UCLAscriptc.dbSpatial_allCellsPlaced,args)
        return val
    __setmethods__ = {
        "locations" : UCLAscriptc.dbSpatial_locations_set,
        "locationValid" : UCLAscriptc.dbSpatial_locationValid_set,
        "orientations" : UCLAscriptc.dbSpatial_orientations_set,
        "constraints" : UCLAscriptc.dbSpatial_constraints_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = dbSpatial.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "locations" : UCLAscriptc.dbSpatial_locations_get,
        "locationValid" : UCLAscriptc.dbSpatial_locationValid_get,
        "orientations" : UCLAscriptc.dbSpatial_orientations_get,
        "constraints" : UCLAscriptc.dbSpatial_constraints_get,
    }
    def __getattr__(self,name):
        method = dbSpatial.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C dbSpatial instance at %s>" % (self.this,)
class dbSpatialPtr(dbSpatial):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = dbSpatial



class DB:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_DB,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_DB(self)
    def isConsistent(*args):
        val = apply(UCLAscriptc.DB_isConsistent,args)
        return val
    def getOrigFileName(*args):
        val = apply(UCLAscriptc.DB_getOrigFileName,args)
        return val
    def getAuxName(*args):
        val = apply(UCLAscriptc.DB_getAuxName,args)
        return val
    def getDirName(*args):
        val = apply(UCLAscriptc.DB_getDirName,args)
        return val
    def getFileNames(*args):
        val = apply(UCLAscriptc.DB_getFileNames,args)
        return val
    def getOrigFileFormat(*args):
        val = apply(UCLAscriptc.DB_getOrigFileFormat,args)
        return val
    def getDelimiter(*args):
        val = apply(UCLAscriptc.DB_getDelimiter,args)
        return val
    def getLayout(*args):
        val = apply(UCLAscriptc.DB_getLayout,args)
        if val: val = dbContextPtr(val) ; val.thisown = 1
        return val
    def getNetlist(*args):
        val = apply(UCLAscriptc.DB_getNetlist,args)
        if val: val = dbNetlistPtr(val) ; val.thisown = 1
        return val
    def getNetlistForSurgery(*args):
        val = apply(UCLAscriptc.DB_getNetlistForSurgery,args)
        if val: val = dbNetlistPtr(val) ; val.thisown = 1
        return val
    def getTiming(*args):
        val = apply(UCLAscriptc.DB_getTiming,args)
        if val: val = dbTemporalPtr(val) ; val.thisown = 1
        return val
    def saveDEF(*args):
        val = apply(UCLAscriptc.DB_saveDEF,args)
        return val
    def saveXYLineDrawing(*args):
        val = apply(UCLAscriptc.DB_saveXYLineDrawing,args)
        return val
    def hasDesignName(*args):
        val = apply(UCLAscriptc.DB_hasDesignName,args)
        return val
    def hasTechnoName(*args):
        val = apply(UCLAscriptc.DB_hasTechnoName,args)
        return val
    def getDesignName(*args):
        val = apply(UCLAscriptc.DB_getDesignName,args)
        return val
    def getTechnoName(*args):
        val = apply(UCLAscriptc.DB_getTechnoName,args)
        return val
    def evalHPWLcost(*args):
        val = apply(UCLAscriptc.DB_evalHPWLcost,args)
        return val
    def evalWHPWLcost(*args):
        val = apply(UCLAscriptc.DB_evalWHPWLcost,args)
        return val
    def printNetStats(*args):
        val = apply(UCLAscriptc.DB_printNetStats,args)
        return val
    def locatePin(*args):
        val = apply(UCLAscriptc.DB_locatePin,args)
        return val
    def setPlaceAndOrient(*args):
        val = apply(UCLAscriptc.DB_setPlaceAndOrient,args)
        return val
    __setmethods__ = {
        "spatial" : UCLAscriptc.DB_spatial_set,
        "easyAccess" : UCLAscriptc.DB_easyAccess_set,
        "routedNets" : UCLAscriptc.DB_routedNets_set,
        "timing" : UCLAscriptc.DB_timing_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = DB.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "spatial" : lambda x : dbSpatialPtr(UCLAscriptc.DB_spatial_get(x)),
        "easyAccess" : lambda x : dbEasyAccessPtr(UCLAscriptc.DB_easyAccess_get(x)),
        "routedNets" : UCLAscriptc.DB_routedNets_get,
        "timing" : lambda x : dbTemporalPtr(UCLAscriptc.DB_timing_get(x)),
    }
    def __getattr__(self,name):
        method = DB.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C DB instance at %s>" % (self.this,)
class DBPtr(DB):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = DB



class DB_Parameters:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_DB_Parameters,args)
        self.thisown = 1

    __setmethods__ = {
        "ignoreLogicalPins" : UCLAscriptc.DB_Parameters_ignoreLogicalPins_set,
        "ignoreGlobalRoutingInfo" : UCLAscriptc.DB_Parameters_ignoreGlobalRoutingInfo_set,
        "ignoreDetailedRoutingInfo" : UCLAscriptc.DB_Parameters_ignoreDetailedRoutingInfo_set,
        "ignoreTimingLibs" : UCLAscriptc.DB_Parameters_ignoreTimingLibs_set,
        "fixPlacedPads" : UCLAscriptc.DB_Parameters_fixPlacedPads_set,
        "verbose" : UCLAscriptc.DB_Parameters_verbose_set,
        "routedDesign" : UCLAscriptc.DB_Parameters_routedDesign_set,
        "alwaysCheckConsistency" : UCLAscriptc.DB_Parameters_alwaysCheckConsistency_set,
        "placeHolder2" : UCLAscriptc.DB_Parameters_placeHolder2_set,
        "placeHolder3" : UCLAscriptc.DB_Parameters_placeHolder3_set,
        "placeHolder4" : UCLAscriptc.DB_Parameters_placeHolder4_set,
        "placeHolder5" : UCLAscriptc.DB_Parameters_placeHolder5_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = DB_Parameters.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "ignoreLogicalPins" : UCLAscriptc.DB_Parameters_ignoreLogicalPins_get,
        "ignoreGlobalRoutingInfo" : UCLAscriptc.DB_Parameters_ignoreGlobalRoutingInfo_get,
        "ignoreDetailedRoutingInfo" : UCLAscriptc.DB_Parameters_ignoreDetailedRoutingInfo_get,
        "ignoreTimingLibs" : UCLAscriptc.DB_Parameters_ignoreTimingLibs_get,
        "fixPlacedPads" : UCLAscriptc.DB_Parameters_fixPlacedPads_get,
        "verbose" : UCLAscriptc.DB_Parameters_verbose_get,
        "routedDesign" : UCLAscriptc.DB_Parameters_routedDesign_get,
        "alwaysCheckConsistency" : UCLAscriptc.DB_Parameters_alwaysCheckConsistency_get,
        "placeHolder2" : UCLAscriptc.DB_Parameters_placeHolder2_get,
        "placeHolder3" : UCLAscriptc.DB_Parameters_placeHolder3_get,
        "placeHolder4" : UCLAscriptc.DB_Parameters_placeHolder4_get,
        "placeHolder5" : UCLAscriptc.DB_Parameters_placeHolder5_get,
    }
    def __getattr__(self,name):
        method = DB_Parameters.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C DB_Parameters instance at %s>" % (self.this,)
class DB_ParametersPtr(DB_Parameters):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = DB_Parameters



class RBPlacement:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_RBPlacement,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_RBPlacement(self)
    def getOrigFileName(*args):
        val = apply(UCLAscriptc.RBPlacement_getOrigFileName,args)
        return val
    def getAuxName(*args):
        val = apply(UCLAscriptc.RBPlacement_getAuxName,args)
        return val
    def getDirName(*args):
        val = apply(UCLAscriptc.RBPlacement_getDirName,args)
        return val
    def getFileNames(*args):
        val = apply(UCLAscriptc.RBPlacement_getFileNames,args)
        return val
    def resetPlacementWOri(*args):
        val = apply(UCLAscriptc.RBPlacement_resetPlacementWOri,args)
        return val
    def resetPlacement(*args):
        val = apply(UCLAscriptc.RBPlacement_resetPlacement,args)
        return val
    def getNumCells(*args):
        val = apply(UCLAscriptc.RBPlacement_getNumCells,args)
        return val
    def isPopulated(*args):
        val = apply(UCLAscriptc.RBPlacement_isPopulated,args)
        return val
    def allCellsPlaced(*args):
        val = apply(UCLAscriptc.RBPlacement_allCellsPlaced,args)
        return val
    def numCellsNotPlaced(*args):
        val = apply(UCLAscriptc.RBPlacement_numCellsNotPlaced,args)
        return val
    def isCoreCell(*args):
        val = apply(UCLAscriptc.RBPlacement_isCoreCell,args)
        return val
    def isFixed(*args):
        val = apply(UCLAscriptc.RBPlacement_isFixed,args)
        return val
    def isStorElt(*args):
        val = apply(UCLAscriptc.RBPlacement_isStorElt,args)
        return val
    def isInSubRow(*args):
        val = apply(UCLAscriptc.RBPlacement_isInSubRow,args)
        return val
    def getFixed(*args):
        val = apply(UCLAscriptc.RBPlacement_getFixed,args)
        return val
    def getStorElts(*args):
        val = apply(UCLAscriptc.RBPlacement_getStorElts,args)
        return val
    def setLocation(*args):
        val = apply(UCLAscriptc.RBPlacement_setLocation,args)
        return val
    def getOrient(*args):
        val = apply(UCLAscriptc.RBPlacement_getOrient,args)
        if val: val = OrientPtr(val) ; val.thisown = 1
        return val
    def setOrient(*args):
        val = apply(UCLAscriptc.RBPlacement_setOrient,args)
        return val
    def findCoreRow(*args):
        val = apply(UCLAscriptc.RBPlacement_findCoreRow,args)
        return val
    def findSubRow(*args):
        val = apply(UCLAscriptc.RBPlacement_findSubRow,args)
        return val
    def findBothRows(*args):
        val = apply(UCLAscriptc.RBPlacement_findBothRows,args)
        return val
    def coreRowsBegin(*args):
        val = apply(UCLAscriptc.RBPlacement_coreRowsBegin,args)
        return val
    def coreRowsEnd(*args):
        val = apply(UCLAscriptc.RBPlacement_coreRowsEnd,args)
        return val
    def getNumCoreRows(*args):
        val = apply(UCLAscriptc.RBPlacement_getNumCoreRows,args)
        return val
    def isCellInRow(*args):
        val = apply(UCLAscriptc.RBPlacement_isCellInRow,args)
        return val
    def getCellCoord(*args):
        val = apply(UCLAscriptc.RBPlacement_getCellCoord,args)
        return val
    def extractCellsFromSR(*args):
        val = apply(UCLAscriptc.RBPlacement_extractCellsFromSR,args)
        return val
    def embedCellsInSR(*args):
        val = apply(UCLAscriptc.RBPlacement_embedCellsInSR,args)
        return val
    def permuteCellsInSR(*args):
        val = apply(UCLAscriptc.RBPlacement_permuteCellsInSR,args)
        return val
    def placeTerms(*args):
        val = apply(UCLAscriptc.RBPlacement_placeTerms,args)
        return val
    def evalHPWL(*args):
        val = apply(UCLAscriptc.RBPlacement_evalHPWL,args)
        return val
    def evalMSTWL(*args):
        val = apply(UCLAscriptc.RBPlacement_evalMSTWL,args)
        return val
    def evalWeightedWL(*args):
        val = apply(UCLAscriptc.RBPlacement_evalWeightedWL,args)
        return val
    def evalNetsHPWL(*args):
        val = apply(UCLAscriptc.RBPlacement_evalNetsHPWL,args)
        return val
    def getHGraph(*args):
        val = apply(UCLAscriptc.RBPlacement_getHGraph,args)
        return val
    def isConsistent(*args):
        val = apply(UCLAscriptc.RBPlacement_isConsistent,args)
        return val
    def printRows(*args):
        val = apply(UCLAscriptc.RBPlacement_printRows,args)
        return val
    def saveAsSCL(*args):
        val = apply(UCLAscriptc.RBPlacement_saveAsSCL,args)
        return val
    def saveAsNodesNetsWts(*args):
        val = apply(UCLAscriptc.RBPlacement_saveAsNodesNetsWts,args)
        return val
    def saveAsNetDAre(*args):
        val = apply(UCLAscriptc.RBPlacement_saveAsNetDAre,args)
        return val
    def savePlacement(*args):
        val = apply(UCLAscriptc.RBPlacement_savePlacement,args)
        return val
    def saveAsSpatialConstraints(*args):
        val = apply(UCLAscriptc.RBPlacement_saveAsSpatialConstraints,args)
        return val
    def saveAsPlot(*args):
        val = apply(UCLAscriptc.RBPlacement_saveAsPlot,args)
        return val
    def remOverlaps(*args):
        val = apply(UCLAscriptc.RBPlacement_remOverlaps,args)
        return val
    def remOverlapsVert(*args):
        val = apply(UCLAscriptc.RBPlacement_remOverlapsVert,args)
        return val
    def permuteCellsAndWSInSR(*args):
        val = apply(UCLAscriptc.RBPlacement_permuteCellsAndWSInSR,args)
        return val
    def updateIronedCells(*args):
        val = apply(UCLAscriptc.RBPlacement_updateIronedCells,args)
        return val
    def updateIronedCellsLR(*args):
        val = apply(UCLAscriptc.RBPlacement_updateIronedCellsLR,args)
        return val
    def updateIronedCellsRL(*args):
        val = apply(UCLAscriptc.RBPlacement_updateIronedCellsRL,args)
        return val
    def calcInstHPWL(*args):
        val = apply(UCLAscriptc.RBPlacement_calcInstHPWL,args)
        return val
    def saveLEFDEF(*args):
        val = apply(UCLAscriptc.RBPlacement_saveLEFDEF,args)
        return val
    def getBBox(*args):
        val = apply(UCLAscriptc.RBPlacement_getBBox,args)
        return val
    def __repr__(self):
        return "<C RBPlacement instance at %s>" % (self.this,)
class RBPlacementPtr(RBPlacement):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = RBPlacement



class RBPlacement_Parameters:
    EQUAL_SPACE = UCLAscriptc.RBPlacement_Parameters_EQUAL_SPACE
    WITH_PIN_ALG1 = UCLAscriptc.RBPlacement_Parameters_WITH_PIN_ALG1
    WITH_PIN_ALG2 = UCLAscriptc.RBPlacement_Parameters_WITH_PIN_ALG2
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_RBPlacement_Parameters,args)
        self.thisown = 1

    __setmethods__ = {
        "verb" : UCLAscriptc.RBPlacement_Parameters_verb_set,
        "numRowsToRemove" : UCLAscriptc.RBPlacement_Parameters_numRowsToRemove_set,
        "spaceCellsAlg" : UCLAscriptc.RBPlacement_Parameters_spaceCellsAlg_set,
        "remCongestion" : UCLAscriptc.RBPlacement_Parameters_remCongestion_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = RBPlacement_Parameters.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "verb" : UCLAscriptc.RBPlacement_Parameters_verb_get,
        "numRowsToRemove" : UCLAscriptc.RBPlacement_Parameters_numRowsToRemove_get,
        "spaceCellsAlg" : UCLAscriptc.RBPlacement_Parameters_spaceCellsAlg_get,
        "remCongestion" : UCLAscriptc.RBPlacement_Parameters_remCongestion_get,
    }
    def __getattr__(self,name):
        method = RBPlacement_Parameters.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C RBPlacement_Parameters instance at %s>" % (self.this,)
class RBPlacement_ParametersPtr(RBPlacement_Parameters):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = RBPlacement_Parameters



class CompareCellIdsByX:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CompareCellIdsByX,args)
        self.thisown = 1

    def __repr__(self):
        return "<C CompareCellIdsByX instance at %s>" % (self.this,)
class CompareCellIdsByXPtr(CompareCellIdsByX):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CompareCellIdsByX



class masterNodeInfo:
    def __init__(self,this):
        self.this = this

    __setmethods__ = {
        "nodeInfo" : UCLAscriptc.masterNodeInfo_nodeInfo_set,
        "index" : UCLAscriptc.masterNodeInfo_index_set,
        "pinNames" : UCLAscriptc.masterNodeInfo_pinNames_set,
        "pinOffsets" : UCLAscriptc.masterNodeInfo_pinOffsets_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = masterNodeInfo.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "nodeInfo" : UCLAscriptc.masterNodeInfo_nodeInfo_get,
        "name" : UCLAscriptc.masterNodeInfo_name_get,
        "index" : UCLAscriptc.masterNodeInfo_index_get,
        "pinNames" : UCLAscriptc.masterNodeInfo_pinNames_get,
        "pinOffsets" : UCLAscriptc.masterNodeInfo_pinOffsets_get,
    }
    def __getattr__(self,name):
        method = masterNodeInfo.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C masterNodeInfo instance at %s>" % (self.this,)
class masterNodeInfoPtr(masterNodeInfo):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = masterNodeInfo



class nodesMasInfo:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_nodesMasInfo,args)
        self.thisown = 1

    def putIndex(*args):
        val = apply(UCLAscriptc.nodesMasInfo_putIndex,args)
        return val
    def getMasterIndex(*args):
        val = apply(UCLAscriptc.nodesMasInfo_getMasterIndex,args)
        return val
    __setmethods__ = {
        "masterNodeIndex" : UCLAscriptc.nodesMasInfo_masterNodeIndex_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = nodesMasInfo.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "masterNodeIndex" : UCLAscriptc.nodesMasInfo_masterNodeIndex_get,
    }
    def __getattr__(self,name):
        method = nodesMasInfo.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C nodesMasInfo instance at %s>" % (self.this,)
class nodesMasInfoPtr(nodesMasInfo):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = nodesMasInfo



class CapoPlacer:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CapoPlacer,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_CapoPlacer(self)
    def estimateWL(*args):
        val = apply(UCLAscriptc.CapoPlacer_estimateWL,args)
        return val
    def getBBoxPerLayer(*args):
        val = apply(UCLAscriptc.CapoPlacer_getBBoxPerLayer,args)
        return val
    def getCutPerLayer(*args):
        val = apply(UCLAscriptc.CapoPlacer_getCutPerLayer,args)
        return val
    def getNodesInEachBlock(*args):
        val = apply(UCLAscriptc.CapoPlacer_getNodesInEachBlock,args)
        return val
    def getTerminalsToEachBlock(*args):
        val = apply(UCLAscriptc.CapoPlacer_getTerminalsToEachBlock,args)
        return val
    def getCellToBlockMap(*args):
        val = apply(UCLAscriptc.CapoPlacer_getCellToBlockMap,args)
        return val
    def getPinLocation(*args):
        val = apply(UCLAscriptc.CapoPlacer_getPinLocation,args)
        return val
    def getNetlistHGraph(*args):
        val = apply(UCLAscriptc.CapoPlacer_getNetlistHGraph,args)
        return val
    def getParams(*args):
        val = apply(UCLAscriptc.CapoPlacer_getParams,args)
        if val: val = CapoParametersPtr(val) ; val.thisown = 1
        return val
    def getPlacement(*args):
        val = apply(UCLAscriptc.CapoPlacer_getPlacement,args)
        return val
    def printCutDensityStats(*args):
        val = apply(UCLAscriptc.CapoPlacer_printCutDensityStats,args)
        return val
    def printSmallProblemStats(*args):
        val = apply(UCLAscriptc.CapoPlacer_printSmallProblemStats,args)
        return val
    def printNetlistStats(*args):
        val = apply(UCLAscriptc.CapoPlacer_printNetlistStats,args)
        return val
    def getNetCutInfo(*args):
        val = apply(UCLAscriptc.CapoPlacer_getNetCutInfo,args)
        return val
    def getTotalNetCut(*args):
        val = apply(UCLAscriptc.CapoPlacer_getTotalNetCut,args)
        return val
    def getBlockMembership(*args):
        val = apply(UCLAscriptc.CapoPlacer_getBlockMembership,args)
        return val
    def printBlockMembership(*args):
        val = apply(UCLAscriptc.CapoPlacer_printBlockMembership,args)
        return val
    def getHierCellNames(*args):
        val = apply(UCLAscriptc.CapoPlacer_getHierCellNames,args)
        return val
    def printHierCellNames(*args):
        val = apply(UCLAscriptc.CapoPlacer_printHierCellNames,args)
        return val
    def saveSmallProblem(*args):
        val = apply(UCLAscriptc.CapoPlacer_saveSmallProblem,args)
        return val
    def __repr__(self):
        return "<C CapoPlacer instance at %s>" % (self.this,)
class CapoPlacerPtr(CapoPlacer):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CapoPlacer



class CapoSplitterParams:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CapoSplitterParams,args)
        self.thisown = 1

    __setmethods__ = {
        "doRepartitioning" : UCLAscriptc.CapoSplitterParams_doRepartitioning_set,
        "useWSTolMethod" : UCLAscriptc.CapoSplitterParams_useWSTolMethod_set,
        "constantTolerance" : UCLAscriptc.CapoSplitterParams_constantTolerance_set,
        "numMLSets" : UCLAscriptc.CapoSplitterParams_numMLSets_set,
        "useNameHierarchy" : UCLAscriptc.CapoSplitterParams_useNameHierarchy_set,
        "delimiters" : UCLAscriptc.CapoSplitterParams_delimiters_set,
        "verb" : UCLAscriptc.CapoSplitterParams_verb_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = CapoSplitterParams.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "doRepartitioning" : UCLAscriptc.CapoSplitterParams_doRepartitioning_get,
        "useWSTolMethod" : UCLAscriptc.CapoSplitterParams_useWSTolMethod_get,
        "constantTolerance" : UCLAscriptc.CapoSplitterParams_constantTolerance_get,
        "numMLSets" : UCLAscriptc.CapoSplitterParams_numMLSets_get,
        "useNameHierarchy" : UCLAscriptc.CapoSplitterParams_useNameHierarchy_get,
        "delimiters" : UCLAscriptc.CapoSplitterParams_delimiters_get,
        "verb" : UCLAscriptc.CapoSplitterParams_verb_get,
    }
    def __getattr__(self,name):
        method = CapoSplitterParams.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C CapoSplitterParams instance at %s>" % (self.this,)
class CapoSplitterParamsPtr(CapoSplitterParams):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CapoSplitterParams



class CapoParameters:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_CapoParameters,args)
        self.thisown = 1

    def printHelp(*args):
        val = apply(UCLAscriptc.CapoParameters_printHelp,args)
        return val
    __setmethods__ = {
        "verb" : UCLAscriptc.CapoParameters_verb_set,
        "stopAtBlocks" : UCLAscriptc.CapoParameters_stopAtBlocks_set,
        "replaceSmallBlocks" : UCLAscriptc.CapoParameters_replaceSmallBlocks_set,
        "useActualPinLocs" : UCLAscriptc.CapoParameters_useActualPinLocs_set,
        "smallPartThreshold" : UCLAscriptc.CapoParameters_smallPartThreshold_set,
        "smallPlaceThreshold" : UCLAscriptc.CapoParameters_smallPlaceThreshold_set,
        "doOverlapping" : UCLAscriptc.CapoParameters_doOverlapping_set,
        "startOverlappingLayer" : UCLAscriptc.CapoParameters_startOverlappingLayer_set,
        "endOverlappingLayer" : UCLAscriptc.CapoParameters_endOverlappingLayer_set,
        "propFromPl" : UCLAscriptc.CapoParameters_propFromPl_set,
        "useKPl" : UCLAscriptc.CapoParameters_useKPl_set,
        "mlParams" : UCLAscriptc.CapoParameters_mlParams_set,
        "smplParams" : UCLAscriptc.CapoParameters_smplParams_set,
        "lookAhead" : UCLAscriptc.CapoParameters_lookAhead_set,
        "allowRowSplits" : UCLAscriptc.CapoParameters_allowRowSplits_set,
        "splitterParams" : UCLAscriptc.CapoParameters_splitterParams_set,
        "plotBlocks" : UCLAscriptc.CapoParameters_plotBlocks_set,
        "saveLayerBBoxes" : UCLAscriptc.CapoParameters_saveLayerBBoxes_set,
        "saveBlocks" : UCLAscriptc.CapoParameters_saveBlocks_set,
        "saveSmallPlProb" : UCLAscriptc.CapoParameters_saveSmallPlProb_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = CapoParameters.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "verb" : UCLAscriptc.CapoParameters_verb_get,
        "stopAtBlocks" : UCLAscriptc.CapoParameters_stopAtBlocks_get,
        "replaceSmallBlocks" : UCLAscriptc.CapoParameters_replaceSmallBlocks_get,
        "useActualPinLocs" : UCLAscriptc.CapoParameters_useActualPinLocs_get,
        "smallPartThreshold" : UCLAscriptc.CapoParameters_smallPartThreshold_get,
        "smallPlaceThreshold" : UCLAscriptc.CapoParameters_smallPlaceThreshold_get,
        "doOverlapping" : UCLAscriptc.CapoParameters_doOverlapping_get,
        "startOverlappingLayer" : UCLAscriptc.CapoParameters_startOverlappingLayer_get,
        "endOverlappingLayer" : UCLAscriptc.CapoParameters_endOverlappingLayer_get,
        "propFromPl" : UCLAscriptc.CapoParameters_propFromPl_get,
        "useKPl" : UCLAscriptc.CapoParameters_useKPl_get,
        "mlParams" : UCLAscriptc.CapoParameters_mlParams_get,
        "smplParams" : UCLAscriptc.CapoParameters_smplParams_get,
        "lookAhead" : UCLAscriptc.CapoParameters_lookAhead_get,
        "allowRowSplits" : UCLAscriptc.CapoParameters_allowRowSplits_get,
        "splitterParams" : lambda x : CapoSplitterParamsPtr(UCLAscriptc.CapoParameters_splitterParams_get(x)),
        "plotBlocks" : UCLAscriptc.CapoParameters_plotBlocks_get,
        "saveLayerBBoxes" : UCLAscriptc.CapoParameters_saveLayerBBoxes_get,
        "saveBlocks" : UCLAscriptc.CapoParameters_saveBlocks_get,
        "saveSmallPlProb" : UCLAscriptc.CapoParameters_saveSmallPlProb_get,
    }
    def __getattr__(self,name):
        method = CapoParameters.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C CapoParameters instance at %s>" % (self.this,)
class CapoParametersPtr(CapoParameters):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = CapoParameters



class MetaPlacerParams:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_MetaPlacerParams,args)
        self.thisown = 1

    __setmethods__ = {
        "verb" : UCLAscriptc.MetaPlacerParams_verb_set,
        "save" : UCLAscriptc.MetaPlacerParams_save_set,
        "flip" : UCLAscriptc.MetaPlacerParams_flip_set,
        "skipRowIroning" : UCLAscriptc.MetaPlacerParams_skipRowIroning_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = MetaPlacerParams.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "verb" : UCLAscriptc.MetaPlacerParams_verb_get,
        "save" : UCLAscriptc.MetaPlacerParams_save_get,
        "flip" : UCLAscriptc.MetaPlacerParams_flip_get,
        "skipRowIroning" : UCLAscriptc.MetaPlacerParams_skipRowIroning_get,
    }
    def __getattr__(self,name):
        method = MetaPlacerParams.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C MetaPlacerParams instance at %s>" % (self.this,)
class MetaPlacerParamsPtr(MetaPlacerParams):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = MetaPlacerParams



class MetaPlacer:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_MetaPlacer,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_MetaPlacer(self)
    def __repr__(self):
        return "<C MetaPlacer instance at %s>" % (self.this,)
class MetaPlacerPtr(MetaPlacer):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = MetaPlacer



class PartitioningProblem:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_PartitioningProblem,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_PartitioningProblem(self)
    def propagateTerminals(*args):
        val = apply(UCLAscriptc.PartitioningProblem_propagateTerminals,args)
        return val
    def saveAsNetDAre(*args):
        val = apply(UCLAscriptc.PartitioningProblem_saveAsNetDAre,args)
        return val
    def saveAsNodesNets(*args):
        val = apply(UCLAscriptc.PartitioningProblem_saveAsNodesNets,args)
        return val
    def saveBestSol(*args):
        val = apply(UCLAscriptc.PartitioningProblem_saveBestSol,args)
        return val
    def reserveBuffers(*args):
        val = apply(UCLAscriptc.PartitioningProblem_reserveBuffers,args)
        return val
    def isDataOwned(*args):
        val = apply(UCLAscriptc.PartitioningProblem_isDataOwned,args)
        return val
    def partsComeFromGrid(*args):
        val = apply(UCLAscriptc.PartitioningProblem_partsComeFromGrid,args)
        return val
    def getNumXParts(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getNumXParts,args)
        return val
    def getNumYParts(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getNumYParts,args)
        return val
    def getXTics(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getXTics,args)
        return val
    def getYTics(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getYTics,args)
        return val
    def getHGraph(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getHGraph,args)
        if val: val = HGraphFixedPtr(val) ; val.thisown = 1
        return val
    def getHGraphPointer(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getHGraphPointer,args)
        if val: val = HGraphFixedPtr(val) 
        return val
    def getParameters(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getParameters,args)
        if val: val = PartitioningProblem_ParametersPtr(val) ; val.thisown = 1
        return val
    def getSolnBuffers(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getSolnBuffers,args)
        if val: val = PartitioningBufferPtr(val) ; val.thisown = 1
        return val
    def getBestSoln(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getBestSoln,args)
        if val: val = PartitioningPtr(val) ; val.thisown = 1
        return val
    def setSolnBuffers(*args):
        val = apply(UCLAscriptc.PartitioningProblem_setSolnBuffers,args)
        return val
    def swapOutSolnBuffers(*args):
        val = apply(UCLAscriptc.PartitioningProblem_swapOutSolnBuffers,args)
        if val: val = PartitioningBufferPtr(val) 
        return val
    def swapOutFixedConst(*args):
        val = apply(UCLAscriptc.PartitioningProblem_swapOutFixedConst,args)
        if val: val = PartitioningPtr(val) 
        return val
    def swapOutHGraph(*args):
        val = apply(UCLAscriptc.PartitioningProblem_swapOutHGraph,args)
        if val: val = HGraphFixedPtr(val) 
        return val
    def getCosts(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getCosts,args)
        return val
    def getViolations(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getViolations,args)
        return val
    def getImbalances(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getImbalances,args)
        return val
    def getFixedConstr(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getFixedConstr,args)
        if val: val = PartitioningPtr(val) ; val.thisown = 1
        return val
    def getCapacities(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getCapacities,args)
        return val
    def getMaxCapacities(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getMaxCapacities,args)
        return val
    def getMinCapacities(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getMinCapacities,args)
        return val
    def getTotalWeight(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getTotalWeight,args)
        return val
    def getPartitions(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getPartitions,args)
        return val
    def getPartitionCenters(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getPartitionCenters,args)
        return val
    def getScalingFactorForBBoxQuantization(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getScalingFactorForBBoxQuantization,args)
        return val
    def getPadBlocks(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getPadBlocks,args)
        return val
    def getPadBlockCenters(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getPadBlockCenters,args)
        return val
    def getTerminalToBlock(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getTerminalToBlock,args)
        return val
    def getNumPartitions(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getNumPartitions,args)
        return val
    def getBestSolnNum(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getBestSolnNum,args)
        return val
    def setBestSolnNum(*args):
        val = apply(UCLAscriptc.PartitioningProblem_setBestSolnNum,args)
        return val
    def printLargestCellStats(*args):
        val = apply(UCLAscriptc.PartitioningProblem_printLargestCellStats,args)
        return val
    def getPinBalances(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getPinBalances,args)
        return val
    def printPinBalances(*args):
        val = apply(UCLAscriptc.PartitioningProblem_printPinBalances,args)
        return val
    def getAttributes(*args):
        val = apply(UCLAscriptc.PartitioningProblem_getAttributes,args)
        if val: val = PartitioningProblem_AttributesPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C PartitioningProblem instance at %s>" % (self.this,)
class PartitioningProblemPtr(PartitioningProblem):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = PartitioningProblem



class PartitioningProblem_Parameters:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_PartitioningProblem_Parameters,args)
        self.thisown = 1

    __setmethods__ = {
        "verbosity" : UCLAscriptc.PartitioningProblem_Parameters_verbosity_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = PartitioningProblem_Parameters.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "verbosity" : UCLAscriptc.PartitioningProblem_Parameters_verbosity_get,
    }
    def __getattr__(self,name):
        method = PartitioningProblem_Parameters.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C PartitioningProblem_Parameters instance at %s>" % (self.this,)
class PartitioningProblem_ParametersPtr(PartitioningProblem_Parameters):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = PartitioningProblem_Parameters



class PartitioningProblem_Attributes:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_PartitioningProblem_Attributes,args)
        self.thisown = 1

    def clearAll(*args):
        val = apply(UCLAscriptc.PartitioningProblem_Attributes_clearAll,args)
        return val
    def setAll(*args):
        val = apply(UCLAscriptc.PartitioningProblem_Attributes_setAll,args)
        return val
    __setmethods__ = {
        "hasMaxCap" : UCLAscriptc.PartitioningProblem_Attributes_hasMaxCap_set,
        "hasMinCap" : UCLAscriptc.PartitioningProblem_Attributes_hasMinCap_set,
        "hasCap" : UCLAscriptc.PartitioningProblem_Attributes_hasCap_set,
        "hasPartBBoxes" : UCLAscriptc.PartitioningProblem_Attributes_hasPartBBoxes_set,
        "hasPadBBoxes" : UCLAscriptc.PartitioningProblem_Attributes_hasPadBBoxes_set,
        "hasPartCenters" : UCLAscriptc.PartitioningProblem_Attributes_hasPartCenters_set,
        "hasPadCenters" : UCLAscriptc.PartitioningProblem_Attributes_hasPadCenters_set,
        "hasTotalWeight" : UCLAscriptc.PartitioningProblem_Attributes_hasTotalWeight_set,
        "hasClusterDegrees" : UCLAscriptc.PartitioningProblem_Attributes_hasClusterDegrees_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = PartitioningProblem_Attributes.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "hasMaxCap" : UCLAscriptc.PartitioningProblem_Attributes_hasMaxCap_get,
        "hasMinCap" : UCLAscriptc.PartitioningProblem_Attributes_hasMinCap_get,
        "hasCap" : UCLAscriptc.PartitioningProblem_Attributes_hasCap_get,
        "hasPartBBoxes" : UCLAscriptc.PartitioningProblem_Attributes_hasPartBBoxes_get,
        "hasPadBBoxes" : UCLAscriptc.PartitioningProblem_Attributes_hasPadBBoxes_get,
        "hasPartCenters" : UCLAscriptc.PartitioningProblem_Attributes_hasPartCenters_get,
        "hasPadCenters" : UCLAscriptc.PartitioningProblem_Attributes_hasPadCenters_get,
        "hasTotalWeight" : UCLAscriptc.PartitioningProblem_Attributes_hasTotalWeight_get,
        "hasClusterDegrees" : UCLAscriptc.PartitioningProblem_Attributes_hasClusterDegrees_get,
    }
    def __getattr__(self,name):
        method = PartitioningProblem_Attributes.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C PartitioningProblem_Attributes instance at %s>" % (self.this,)
class PartitioningProblem_AttributesPtr(PartitioningProblem_Attributes):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = PartitioningProblem_Attributes



class BaseMLPart:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_BaseMLPart,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_BaseMLPart(self)
    def getPartitionArea(*args):
        val = apply(UCLAscriptc.BaseMLPart_getPartitionArea,args)
        return val
    def getPartitionRatio(*args):
        val = apply(UCLAscriptc.BaseMLPart_getPartitionRatio,args)
        return val
    def getBestSolnCost(*args):
        val = apply(UCLAscriptc.BaseMLPart_getBestSolnCost,args)
        return val
    def getAveSolnCost(*args):
        val = apply(UCLAscriptc.BaseMLPart_getAveSolnCost,args)
        return val
    def getNumLegalSolns(*args):
        val = apply(UCLAscriptc.BaseMLPart_getNumLegalSolns,args)
        return val
    def getUserTime(*args):
        val = apply(UCLAscriptc.BaseMLPart_getUserTime,args)
        return val
    def __repr__(self):
        return "<C BaseMLPart instance at %s>" % (self.this,)
class BaseMLPartPtr(BaseMLPart):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = BaseMLPart



class BaseMLPart_Parameters:
    NeverSave = UCLAscriptc.BaseMLPart_Parameters_NeverSave
    AtAllLastLevels = UCLAscriptc.BaseMLPart_Parameters_AtAllLastLevels
    AtAllLevels = UCLAscriptc.BaseMLPart_Parameters_AtAllLevels
    AtFirstLevelOfFirst = UCLAscriptc.BaseMLPart_Parameters_AtFirstLevelOfFirst
    AtLastLevelOfFirst = UCLAscriptc.BaseMLPart_Parameters_AtLastLevelOfFirst
    AtAllLevelsOfFirst = UCLAscriptc.BaseMLPart_Parameters_AtAllLevelsOfFirst
    NoVcycles = UCLAscriptc.BaseMLPart_Parameters_NoVcycles
    Exclusive = UCLAscriptc.BaseMLPart_Parameters_Exclusive
    Relaxed = UCLAscriptc.BaseMLPart_Parameters_Relaxed
    Relaxed2 = UCLAscriptc.BaseMLPart_Parameters_Relaxed2
    Comprehensive = UCLAscriptc.BaseMLPart_Parameters_Comprehensive
    ALL = UCLAscriptc.BaseMLPart_Parameters_ALL
    TOPONLY = UCLAscriptc.BaseMLPart_Parameters_TOPONLY
    ALLBUTLAST = UCLAscriptc.BaseMLPart_Parameters_ALLBUTLAST
    NEVER = UCLAscriptc.BaseMLPart_Parameters_NEVER
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_BaseMLPart_Parameters,args)
        self.thisown = 1

    __setmethods__ = {
        "savePartProb" : UCLAscriptc.BaseMLPart_Parameters_savePartProb_set,
        "flatPartitioner" : UCLAscriptc.BaseMLPart_Parameters_flatPartitioner_set,
        "useBBonTop" : UCLAscriptc.BaseMLPart_Parameters_useBBonTop_set,
        "partFuzziness" : UCLAscriptc.BaseMLPart_Parameters_partFuzziness_set,
        "runsPerClTree" : UCLAscriptc.BaseMLPart_Parameters_runsPerClTree_set,
        "solnPoolOnTopLevel" : UCLAscriptc.BaseMLPart_Parameters_solnPoolOnTopLevel_set,
        "toleranceMultiple" : UCLAscriptc.BaseMLPart_Parameters_toleranceMultiple_set,
        "toleranceAlpha" : UCLAscriptc.BaseMLPart_Parameters_toleranceAlpha_set,
        "useTwoPartCalls" : UCLAscriptc.BaseMLPart_Parameters_useTwoPartCalls_set,
        "netThreshold" : UCLAscriptc.BaseMLPart_Parameters_netThreshold_set,
        "Vcycling" : UCLAscriptc.BaseMLPart_Parameters_Vcycling_set,
        "timeLimit" : UCLAscriptc.BaseMLPart_Parameters_timeLimit_set,
        "expPrint2Costs" : UCLAscriptc.BaseMLPart_Parameters_expPrint2Costs_set,
        "clusterToTerminals" : UCLAscriptc.BaseMLPart_Parameters_clusterToTerminals_set,
        "seedTopLvlSoln" : UCLAscriptc.BaseMLPart_Parameters_seedTopLvlSoln_set,
        "pruningPercent" : UCLAscriptc.BaseMLPart_Parameters_pruningPercent_set,
        "pruningPoint" : UCLAscriptc.BaseMLPart_Parameters_pruningPoint_set,
        "maxNumPassesAtBottom" : UCLAscriptc.BaseMLPart_Parameters_maxNumPassesAtBottom_set,
        "vcNumFailures" : UCLAscriptc.BaseMLPart_Parameters_vcNumFailures_set,
        "vcImproveRatio" : UCLAscriptc.BaseMLPart_Parameters_vcImproveRatio_set,
        "vc1ClusterRatio" : UCLAscriptc.BaseMLPart_Parameters_vc1ClusterRatio_set,
        "vc1FirstLevel" : UCLAscriptc.BaseMLPart_Parameters_vc1FirstLevel_set,
        "vc1LevelGrowth" : UCLAscriptc.BaseMLPart_Parameters_vc1LevelGrowth_set,
        "vc2ClusterRatio" : UCLAscriptc.BaseMLPart_Parameters_vc2ClusterRatio_set,
        "vc2FirstLevel" : UCLAscriptc.BaseMLPart_Parameters_vc2FirstLevel_set,
        "vc2LevelGrowth" : UCLAscriptc.BaseMLPart_Parameters_vc2LevelGrowth_set,
        "clParams" : UCLAscriptc.BaseMLPart_Parameters_clParams_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = BaseMLPart_Parameters.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "savePartProb" : UCLAscriptc.BaseMLPart_Parameters_savePartProb_get,
        "flatPartitioner" : UCLAscriptc.BaseMLPart_Parameters_flatPartitioner_get,
        "useBBonTop" : UCLAscriptc.BaseMLPart_Parameters_useBBonTop_get,
        "partFuzziness" : UCLAscriptc.BaseMLPart_Parameters_partFuzziness_get,
        "runsPerClTree" : UCLAscriptc.BaseMLPart_Parameters_runsPerClTree_get,
        "solnPoolOnTopLevel" : UCLAscriptc.BaseMLPart_Parameters_solnPoolOnTopLevel_get,
        "toleranceMultiple" : UCLAscriptc.BaseMLPart_Parameters_toleranceMultiple_get,
        "toleranceAlpha" : UCLAscriptc.BaseMLPart_Parameters_toleranceAlpha_get,
        "useTwoPartCalls" : UCLAscriptc.BaseMLPart_Parameters_useTwoPartCalls_get,
        "netThreshold" : UCLAscriptc.BaseMLPart_Parameters_netThreshold_get,
        "Vcycling" : UCLAscriptc.BaseMLPart_Parameters_Vcycling_get,
        "timeLimit" : UCLAscriptc.BaseMLPart_Parameters_timeLimit_get,
        "expPrint2Costs" : UCLAscriptc.BaseMLPart_Parameters_expPrint2Costs_get,
        "clusterToTerminals" : UCLAscriptc.BaseMLPart_Parameters_clusterToTerminals_get,
        "seedTopLvlSoln" : UCLAscriptc.BaseMLPart_Parameters_seedTopLvlSoln_get,
        "pruningPercent" : UCLAscriptc.BaseMLPart_Parameters_pruningPercent_get,
        "pruningPoint" : UCLAscriptc.BaseMLPart_Parameters_pruningPoint_get,
        "maxNumPassesAtBottom" : UCLAscriptc.BaseMLPart_Parameters_maxNumPassesAtBottom_get,
        "vcNumFailures" : UCLAscriptc.BaseMLPart_Parameters_vcNumFailures_get,
        "vcImproveRatio" : UCLAscriptc.BaseMLPart_Parameters_vcImproveRatio_get,
        "vc1ClusterRatio" : UCLAscriptc.BaseMLPart_Parameters_vc1ClusterRatio_get,
        "vc1FirstLevel" : UCLAscriptc.BaseMLPart_Parameters_vc1FirstLevel_get,
        "vc1LevelGrowth" : UCLAscriptc.BaseMLPart_Parameters_vc1LevelGrowth_get,
        "vc2ClusterRatio" : UCLAscriptc.BaseMLPart_Parameters_vc2ClusterRatio_get,
        "vc2FirstLevel" : UCLAscriptc.BaseMLPart_Parameters_vc2FirstLevel_get,
        "vc2LevelGrowth" : UCLAscriptc.BaseMLPart_Parameters_vc2LevelGrowth_get,
        "clParams" : UCLAscriptc.BaseMLPart_Parameters_clParams_get,
    }
    def __getattr__(self,name):
        method = BaseMLPart_Parameters.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C BaseMLPart_Parameters instance at %s>" % (self.this,)
class BaseMLPart_ParametersPtr(BaseMLPart_Parameters):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = BaseMLPart_Parameters



class MLPart(BaseMLPart):
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_MLPart,args)
        self.thisown = 1

    def __del__(self,UCLAscriptc=UCLAscriptc):
        if self.thisown == 1 :
            UCLAscriptc.delete_MLPart(self)
    def __repr__(self):
        return "<C MLPart instance at %s>" % (self.this,)
class MLPartPtr(MLPart):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = MLPart



class HGraphFixed:
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_HGraphFixed,args)
        self.thisown = 1

    def getNumNodes(*args):
        val = apply(UCLAscriptc.HGraphFixed_getNumNodes,args)
        return val
    def getNumEdges(*args):
        val = apply(UCLAscriptc.HGraphFixed_getNumEdges,args)
        return val
    def getAvgNodeDegree(*args):
        val = apply(UCLAscriptc.HGraphFixed_getAvgNodeDegree,args)
        return val
    def getAvgEdgeDegree(*args):
        val = apply(UCLAscriptc.HGraphFixed_getAvgEdgeDegree,args)
        return val
    def nodesBegin(*args):
        val = apply(UCLAscriptc.HGraphFixed_nodesBegin,args)
        return val
    def nodesEnd(*args):
        val = apply(UCLAscriptc.HGraphFixed_nodesEnd,args)
        return val
    def terminalsBegin(*args):
        val = apply(UCLAscriptc.HGraphFixed_terminalsBegin,args)
        return val
    def terminalsEnd(*args):
        val = apply(UCLAscriptc.HGraphFixed_terminalsEnd,args)
        return val
    def edgesBegin(*args):
        val = apply(UCLAscriptc.HGraphFixed_edgesBegin,args)
        return val
    def edgesEnd(*args):
        val = apply(UCLAscriptc.HGraphFixed_edgesEnd,args)
        return val
    def getNodeByIdx(*args):
        val = apply(UCLAscriptc.HGraphFixed_getNodeByIdx,args)
        return val
    def haveSuchNode(*args):
        val = apply(UCLAscriptc.HGraphFixed_haveSuchNode,args)
        return val
    def haveSuchNet(*args):
        val = apply(UCLAscriptc.HGraphFixed_haveSuchNet,args)
        return val
    def getNodeByName(*args):
        val = apply(UCLAscriptc.HGraphFixed_getNodeByName,args)
        return val
    def getNetByName(*args):
        val = apply(UCLAscriptc.HGraphFixed_getNetByName,args)
        return val
    def getEdgeByIdx(*args):
        val = apply(UCLAscriptc.HGraphFixed_getEdgeByIdx,args)
        return val
    def getNodesSortedByWeights(*args):
        val = apply(UCLAscriptc.HGraphFixed_getNodesSortedByWeights,args)
        return val
    def getNodesSortedByDegrees(*args):
        val = apply(UCLAscriptc.HGraphFixed_getNodesSortedByDegrees,args)
        return val
    def maxNodeIndex(*args):
        val = apply(UCLAscriptc.HGraphFixed_maxNodeIndex,args)
        return val
    def maxEdgeIndex(*args):
        val = apply(UCLAscriptc.HGraphFixed_maxEdgeIndex,args)
        return val
    def addEdge(*args):
        val = apply(UCLAscriptc.HGraphFixed_addEdge,args)
        return val
    def fastAddEdge(*args):
        val = apply(UCLAscriptc.HGraphFixed_fastAddEdge,args)
        return val
    def adviseNodeDegrees(*args):
        val = apply(UCLAscriptc.HGraphFixed_adviseNodeDegrees,args)
        return val
    def finalize(*args):
        val = apply(UCLAscriptc.HGraphFixed_finalize,args)
        return val
    def saveAsNetDAre(*args):
        val = apply(UCLAscriptc.HGraphFixed_saveAsNetDAre,args)
        return val
    def saveAsNodesNetsWts(*args):
        val = apply(UCLAscriptc.HGraphFixed_saveAsNodesNetsWts,args)
        return val
    def isConsistent(*args):
        val = apply(UCLAscriptc.HGraphFixed_isConsistent,args)
        return val
    def sortAsDB(*args):
        val = apply(UCLAscriptc.HGraphFixed_sortAsDB,args)
        return val
    def printEdgeSizeStats(*args):
        val = apply(UCLAscriptc.HGraphFixed_printEdgeSizeStats,args)
        return val
    def printEdgeWtStats(*args):
        val = apply(UCLAscriptc.HGraphFixed_printEdgeWtStats,args)
        return val
    def printNodeWtStats(*args):
        val = apply(UCLAscriptc.HGraphFixed_printNodeWtStats,args)
        return val
    def printNodeDegreeStats(*args):
        val = apply(UCLAscriptc.HGraphFixed_printNodeDegreeStats,args)
        return val
    __setmethods__ = {
        "_nodeNames" : UCLAscriptc.HGraphFixed__nodeNames_set,
        "_netNames" : UCLAscriptc.HGraphFixed__netNames_set,
        "_nodeNamesMap" : UCLAscriptc.HGraphFixed__nodeNamesMap_set,
        "_netNamesMap" : UCLAscriptc.HGraphFixed__netNamesMap_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = HGraphFixed.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "_nodeNames" : UCLAscriptc.HGraphFixed__nodeNames_get,
        "_netNames" : UCLAscriptc.HGraphFixed__netNames_get,
        "_nodeNamesMap" : UCLAscriptc.HGraphFixed__nodeNamesMap_get,
        "_netNamesMap" : UCLAscriptc.HGraphFixed__netNamesMap_get,
    }
    def __getattr__(self,name):
        method = HGraphFixed.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C HGraphFixed instance at %s>" % (self.this,)
class HGraphFixedPtr(HGraphFixed):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = HGraphFixed



class HGfromDB(HGraphFixed):
    def __init__(self,*args):
        self.this = apply(UCLAscriptc.new_HGfromDB,args)
        self.thisown = 1

    __setmethods__ = {
        "_nodeNames" : UCLAscriptc.HGfromDB__nodeNames_set,
        "_netNames" : UCLAscriptc.HGfromDB__netNames_set,
        "_nodeNamesMap" : UCLAscriptc.HGfromDB__nodeNamesMap_set,
        "_netNamesMap" : UCLAscriptc.HGfromDB__netNamesMap_set,
    }
    def __setattr__(self,name,value):
        if (name == "this") or (name == "thisown"): self.__dict__[name] = value; return
        method = HGfromDB.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value
    __getmethods__ = {
        "_nodeNames" : UCLAscriptc.HGfromDB__nodeNames_get,
        "_netNames" : UCLAscriptc.HGfromDB__netNames_get,
        "_nodeNamesMap" : UCLAscriptc.HGfromDB__nodeNamesMap_get,
        "_netNamesMap" : UCLAscriptc.HGfromDB__netNamesMap_get,
    }
    def __getattr__(self,name):
        method = HGfromDB.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C HGfromDB instance at %s>" % (self.this,)
class HGfromDBPtr(HGfromDB):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = HGfromDB





#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

DumbPlacerSeed = UCLAscriptc.DumbPlacerSeed
BaryPlacerSeed = UCLAscriptc.BaryPlacerSeed
WeiszPlacerSeed = UCLAscriptc.WeiszPlacerSeed
Never = UCLAscriptc.Never
AtTheEnd = UCLAscriptc.AtTheEnd
AtEveryLayer = UCLAscriptc.AtEveryLayer
