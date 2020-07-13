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






//created by Andrew Caldwell on 02/02/99

//CHANGES
// 990511 aec	ver1.1	added getPinOffset functions that use an Orientation
// 990830 aec	ver2.0  updated to HGraph3.0 (parses nodes/nets, 
//				supports multi-bin weights)
// 010721 ilm   ver3.1  added gate/wire delay info

#ifndef _HGRAPHWithDIMS_H_
#define _HGRAPHWithDIMS_H_

#include "HGraph/hgFixed.h"
#include "Geoms/point.h"
#include "Placement/symOri.h"
#include "hgDelayInfo.h"


//HGraphWDimensions adds the following data to a HyperGraph
//1) for each node, and height and width
//2) for each node, the pin-offset for it's connection with adj nets
//3) for each edge, the pin-offset and cellId's of adj nodes

struct HGWDimsPin	//this stores nodeId, edgeId & pinOffset during
			//construction
{
    HGWDimsPin(unsigned n, unsigned e, const Point& pt)
	: nodeId(n), edgeId(e), pOffset(pt)  {}

    unsigned nodeId, edgeId;
    Point    pOffset;
};

class HGraphWDimensions : public HGraphFixed
{
   friend class STA;
  protected:

    vector<double>   _nodeHeights;	//height of each node
    vector<double>   _nodeWidths;	//width  of each node
    vector<Symmetry> _nodeSymmetries;   //symmetry of each node
    vector<bool>     _isMacro;	        //is a node a macro

	//collection of all pinOffsets, in no particular order	
    vector<Point>             _pinOffsets;
    HGDelayInfo               _delayInfo; // includes a collection of Rs and Cs

	//the unsigneds are indices into _pinOffsets.
	//For each node, we store the beginning index for _edgesOnNodePinIdx,
	//in _eonBeginEnd,
	//and for each edge, the beginning index for _nodesOnEdgePinIdx
	//in _noeBeginEnd.
	//both have a sentinel value at the end (so that edgeId+1 gives
	//the id of the first pinoffset NOT in the edge)
	//the pin-offsets are given in terms of the offset of the nth
	//node on the edge.
    vector<unsigned> 		_nodesOnEdgePinIdx;
    vector<unsigned>   		_edgesOnNodePinIdx;

    vector<unsigned> 		_noeBeginEnd;
    vector<unsigned> 		_eonBeginEnd;

	//these are used in the expanded addS* and finalize functions
	//rather than the _src,_snk,_srcSnk lists the baseclass uses
    vector<HGWDimsPin>		_srcsWPins;
    vector<HGWDimsPin>		_snksWPins;
    vector<HGWDimsPin>		_srcSnksWPins;

    HGraphWDimensions*	      _nonConstThis;

    HGraphWDimensions(  unsigned   numNodes   = 0, 
			unsigned   numWeights = 1, 
			HGraphParameters param      = HGraphParameters(),
			const vector<double>& heights = vector<double>(),
			const vector<double>& widths  = vector<double>());

		//side-effect!! modifies pinOffset
    void     calcRealOffset(unsigned nodeId, Point& pinOffset, const Orient&);

    void setUnitWeights();

    virtual void readNets	(const char* filename);
    virtual void readNodes	(const char* filename);

    void    setupUnitDimensions();
    void    setupEmptySymmetries();
    void    setupCenterPinOffsets();

    void addSrcWPin    (HGFNode& node, HGFEdge& edge, Point pOffset=Point(0,0));
    void addSnkWPin    (HGFNode& node, HGFEdge& edge, Point pOffset=Point(0,0));
    void addSrcSnkWPin (HGFNode& node, HGFEdge& edge, Point pOffset=Point(0,0));
   
    virtual void finalize(); 

  public:

    HGraphWDimensions(  const HGraphFixed& hg,
			const vector<double>& heights,
			const vector<double>& widths );

    HGraphWDimensions(  const char* fileName1,
			const char* fileName2 = NULL,
			const char* fileName3 = NULL,
			HGraphParameters  param       = HGraphParameters());

    virtual ~HGraphWDimensions(){}

    double getNodeHeight(unsigned id) const { return _nodeHeights[id];}
    void   setNodeWidth (double width ,unsigned id) {_nodeWidths[id] = width;}
    void   setNodeHeight(double height, unsigned id) 
      {_nodeHeights[id] = height;}
    double   getAvgNodeWidth (void);
    double   getAvgNodeHeight (void);
    double getNodeWidth (unsigned id) const { return _nodeWidths[id];}
    Symmetry getNodeSymmetry (unsigned id) const { return _nodeSymmetries[id];}
    double getNodesArea();
    bool isNodeMacro(unsigned id) const { return _isMacro[id]; }
    void updateNodeMacroInfo(unsigned id, bool isMacro) 
      { _isMacro[id] = isMacro; }
    //mark all nodes>maxArea as macros
    void markNodesAsMacro(double maxWidth, double maxHeight); 

    const  HGDelayInfo&   getDelayInfo()  const  { return _delayInfo; }
    const  vector<Point>& getPinOffsets() const  { return _pinOffsets; }
    bool   haveDelayInfo()  const { return _delayInfo.populated;   }
    double getCIntC() const { return _delayInfo.cInt_c; }
    double getCIntK() const { return _delayInfo.cInt_k; }
    double getRIntC() const { return _delayInfo.rInt_c; }
    double getRIntK() const { return _delayInfo.rInt_k; }
    unsigned nodesOnEdgePinIdx(unsigned nodeOffset, unsigned edgeId) const;
    unsigned edgesOnNodePinIdx(unsigned edgeOffset, unsigned nodeId) const;
    Point getRealOffset(unsigned nodeId, const Point& pinOffset,
                        const Orient& ori) const;

	//sets all node's height to newHeight, and their widths to
	//their area(1st weight)/newHeight
    void     setNodeDims(double newHeight);
	//sets all node's height to newHeight, and their widths to
	//their area(1st weight)/newHeight For large Macros use macros
    void     setNodeDimsMacros(double newHeight);

    Point nodesOnEdgePinOffset(unsigned nOffset, unsigned eId,
			      Orient cellOri=Orient()) const;
    Point edgesOnNodePinOffset(unsigned eOffset, unsigned nId,
			      Orient cellOri=Orient()) const;
    //double nodesOnEdgePinLoadCap(unsigned nOffset, unsigned eId) const;
    //double nodesOnEdgePinDrivRes(unsigned nOffset, unsigned eId) const;

    //double edgesOnNodePinLoadCap(unsigned eOffset, unsigned nId) const;
    //double edgesOnNodePinDrivRes(unsigned eOffset, unsigned nId) const;

    virtual void saveAsNodesNetsWts(const char* baseFileName, 
				    bool fixMacros = false) const;

    // by sadya to add dummy cells
    virtual void saveAsNodesNetsWtsWDummy(const char* baseFileName, 
					  double siteWidth, double siteHeight,
			     double areaDummy=0, bool fixMacros = false) const;
    // by sadya to add tethers to cells
    virtual void saveAsNodesNetsWtsWTether(const char* baseFileName,
			  double fract, vector<unsigned>& tetheredCells,
			  bool takeConstrFrmFile=false) const;
    void getCellsToTetherBFS(vector<unsigned>& cellToTether, 
			     unsigned numTetherCells,
			     bool takeConstrFrmFile=false) const;

    //added by sadya to shred the tall cells
    virtual void saveAsNodesNetsWtsShred(const char* baseFileName, 
					 double maxCoreRowHeight) const;
    virtual void saveAsNodesNetsWtsUnShred(const char* baseFileName, 
					   double maxCoreRowHeight) const;
    virtual void saveAsNodesNetsWtsShredHW(const char* baseFileName, 
				double minWidthNode, double maxCoreRowHeight,
				unsigned singleNetWt=0) const;

    virtual void saveAsNodesNetsFloorplan(const char* baseFileName) const;
    virtual void saveMacrosAsNodesNetsFloorplan(const char* baseFileName) 
                                                                         const;

    //added by sadya to add nodes connected to congested regions
    virtual void saveAsNodesNetsWtsWCongInfo(const char* baseFileName,
                                       vector<double>& nodesXCongestion, 
				       vector<double>& nodesYCongestion,
				       double whitespace,
				       double rowHeight);

    virtual bool isConsistent() const;

    virtual void sortNodes(); 
    virtual void sortEdges(); 
    virtual void sortAsDB();

    //added by sadya to make existing poffsets at center    
    void    makeCenterPinOffsets();
    //added by sadya to update pinoffsets externally
    void    updateNOEPinOffsets(unsigned nodeOffset, unsigned edgeId, Point newPinOffset);
    void    updateEONPinOffsets(unsigned edgeOffset, unsigned nodeId, Point newPinOffset);

};

inline unsigned HGraphWDimensions::nodesOnEdgePinIdx(unsigned nodeOffset,
                unsigned edgeId) const
{
    abkassert(edgeId < _edges.size(),
              "edgeId out of range in nodesOnEdgePinIdx");
    abkassert(nodeOffset < _edges[edgeId]->getDegree(),
              "nodeOffset of range in nodesOnEdgePinIdx");
    return _nodesOnEdgePinIdx[_noeBeginEnd[edgeId]+nodeOffset];
}

inline unsigned HGraphWDimensions::edgesOnNodePinIdx(unsigned edgeOffset,
                unsigned nodeId) const
{
    abkassert(nodeId < _nodes.size(),
                "nodeId out of range in edgesOnNodePinOffset");
    abkassert(edgeOffset < _nodes[nodeId]->getDegree(),
                "edgeOffset out of range in edgesOnNodePinOffset");

    return _edgesOnNodePinIdx[_eonBeginEnd[nodeId]+edgeOffset];
}

inline Point HGraphWDimensions::nodesOnEdgePinOffset
                (unsigned nodeOffset, unsigned edgeId,
                 Orient cellOri) const
{
    abkassert(edgeId < _edges.size(),
                "edgeId out of range in nodesOnEdgePinIdx");
    abkassert(nodeOffset < _edges[edgeId]->getDegree(),
                "nodeOffset of range in nodesOnEdgePinIdx");

    unsigned pOffsetIdx = _nodesOnEdgePinIdx[_noeBeginEnd[edgeId]+nodeOffset];

    abkassert2(pOffsetIdx < _pinOffsets.size(),
                "referenced pin is out of range. asked for pin: ", pOffsetIdx);

     //pinOffset is the offset if the cell has orientation 'N'
    Point pinOffset = _pinOffsets[pOffsetIdx];
    itHGFNodeLocal ndItr = _edges[edgeId]->nodesBegin()+nodeOffset;
    unsigned nodeId = (*ndItr)->getIndex();

    _nonConstThis->calcRealOffset(nodeId, pinOffset, cellOri);
    return pinOffset;
}

inline Point HGraphWDimensions::edgesOnNodePinOffset
                (unsigned edgeOffset, unsigned nodeId, Orient cellOri) const
{  
    abkassert(nodeId < _nodes.size(),
                "nodeId out of range in edgesOnNodePinOffset");
    abkassert(edgeOffset < _nodes[nodeId]->getDegree(),
                "edgeOffset out of range in edgesOnNodePinOffset");

    unsigned pOffsetIdx = _edgesOnNodePinIdx[_eonBeginEnd[nodeId]+edgeOffset];

    abkassert(pOffsetIdx < _pinOffsets.size(),
                "referenced pin is out of range");

     //pinOffset is the offset if the cell has orientation 'N'
    Point pinOffset = _pinOffsets[pOffsetIdx];

    _nonConstThis->calcRealOffset(nodeId, pinOffset, cellOri);
    return pinOffset;
}

/*
inline double HGraphWDimensions::nodesOnEdgePinLoadCap
                (unsigned nodeOffset, unsigned edgeId) const
{
    abkassert(edgeId < _edges.size(),
                "edgeId out of range in nodesOnEdgePinIdx");
    abkassert(nodeOffset < _edges[edgeId]->getDegree(),
                "nodeOffset of range in nodesOnEdgePinIdx");

    unsigned pinIdx = _nodesOnEdgePinIdx[_noeBeginEnd[edgeId]+nodeOffset];

    abkassert2(pinIdx < _delayInfo.sinkCap.size(),
                "referenced pin is out of range. asked for pin: ", pinIdx);

    return _delayInfo.sinkCap[pinIdx];
}
*/

inline void HGraphWDimensions::addSrcWPin (HGFNode& node, HGFEdge& edge, Point pt)
{ _srcsWPins.push_back(HGWDimsPin(node.getIndex(), edge.getIndex(), pt)); }

inline void HGraphWDimensions::addSnkWPin (HGFNode& node, HGFEdge& edge, Point pt)
{ _snksWPins.push_back(HGWDimsPin(node.getIndex(), edge.getIndex(), pt)); }

inline void HGraphWDimensions::addSrcSnkWPin(HGFNode& node, HGFEdge& edge, Point pt)
{ _srcSnksWPins.push_back(HGWDimsPin(node.getIndex(), edge.getIndex(), pt)); }

inline Point HGraphWDimensions::getRealOffset(unsigned nodeId,
        const Point& pinOffset, const Orient& ori) const
{
    Point offset = pinOffset;
    _nonConstThis->calcRealOffset(nodeId, offset, ori);
    return offset;
}

//sadya. the following are used for sorting according to value field
//should be part of something common. Here for now
struct ctrSortDoublePair
{
  double index;
  double value;
};

struct doublePair_less_mag 
{
  bool operator()(const ctrSortDoublePair &elem1, 
		  const ctrSortDoublePair &elem2) 
		 { return(elem1.value < elem2.value); }
};

#endif

