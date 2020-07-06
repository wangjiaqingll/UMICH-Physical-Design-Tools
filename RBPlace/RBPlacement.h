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












// Created 970804 by Igor Markov and Paul Tucker, VLSI CAD ABKGROUP, UCLA/CS
// 970821 pat  added spacing to subRows
// 970823 ilm  multiple minor corrections (removing redundancies, adding refs)
// 970823 ilm  added orientations to RBPCoreRow and RBPIORow (Maogang's request)
//                  they are initialized now
// 970902 ilm  RBPlacement::RBPlacement(const DB&) does not populate rows
//                  with cell Id numbers anymore. This is done with
//                  RBPlacement::populateRowsWith(const Placement&)
//                  (the code has to be exteneded to put IOPads intoIORows,
//                  and a lot more)
// 970918 pat  add DB argument to populateRowsWith
// 970919 pat  added missing field to RBPSubRow copier
// 970930 ilm  added RBPCoreSubrow::getSymmetry() const;
// 970930 pat  added findSubRow and findCoreRow signatures
// 971001 pat  moved comparison functors from *.cxx
// 980222 pat  handle additional wierd cases in subrow construction
// 980222  dv  changed the interface of populateRowsWith, added two more
//             unsigned Data Members, added support for isPopulated.
// 980603  dv  removed orientedRBPlacement class
// 980309 pat  more const declarations; added siteRoundup method
// 980427 pat  added RBPCoreRow::findSubRow
// 990120 aec  complete revision. Merged with RBPwCheckups, changed
//		permissions, modified CoreRows and SubRows, added placement
//		functions, etc.
// 990312 aec  added parameters object
// 990527 aec  split RBPl into two packages. The base does not use DB
// 990608 s.m  added spaceCellsWithPinDimensionsAlg1 
// 990911 aec  added file I/O.  changed Placement to PlacementWOrient
// 000803 ilm  fixed savePlacement() to save cell names
// 001128 s.a  added a plotter function
// 001128 s.a  added a function for overlap detection and removal

#ifndef _RBPLACEMENT_H_
#define _RBPLACEMENT_H_

#include <vector>
using std::vector;
using std::bit_vector;
#include "ABKCommon/abkcommon.h"
#include "Placement/placeWOri.h"
#include "RBRows.h"
#include "RBCellCoord.h"
#include "HGraphWDims/hgWDims.h"
#include "Constraints/constraints.h"

#ifdef _MSC_VER
  #pragma warning(disable:4355)
#endif

typedef std::vector<RBPCoreRow>::const_iterator itRBPCoreRow;

// This is used as the return type for a lookup member function
typedef pair<const RBPCoreRow *, const RBPSubRow *> RBRowPtrs;

enum clusterNodeOrient {N, FN, S, FS, W, FW, E, FE, I};

class RBPlacement
{
    friend class HGraphFixed;
  public:
    class Parameters : public HGraphParameters
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
	
	Parameters()
	  :verb("1 1 1"), numRowsToRemove(0), spaceCellsAlg(EQUAL_SPACE),
	  remCongestion(0)
	  {}
	Parameters(int argc, const char *argv[]);
        Parameters(const Parameters& orig)
	  :HGraphParameters(orig), verb(orig.verb), numRowsToRemove(orig.numRowsToRemove),
	   spaceCellsAlg(orig.spaceCellsAlg),remCongestion(orig.remCongestion)
	  {}
	friend ostream& operator<<(ostream&, const Parameters&);
    };

    Constraints constraints;  // defined in the Constraints package
                              // each individual constraint within
                              // this object will reference the
                              // member "locations" defined above
               //These constraints copied directly from DB.spatial.constraints
               //copied only by RBPlFromDB, else remains empty

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

    Parameters		_params;

    RBPlacement*	_nonConstThis;

    //both of following are initialized by initBBox()
    BBox _coreBBox; //bbox of core region
    BBox _fullBBox; //bbox of core region + terminals

    char origFileName[255];
    char dirName     [1023];
    vector<char*>  allFileNamesInAUX;

    void setCellsInRows();
    void sortSubRowsByX();
    void populateCC();

    double oneNetHPWL (unsigned netId, bool usePinLocs);
    double oneNetMSTWL(unsigned netId, bool usePinLocs);

    pair<double, double> oneNetXYHPWL (unsigned netId, bool usePinLocs);

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
		const vector<Orient>& ori, const Parameters& params) 
   	: _hgWDims(NULL), _populated(false), _placement(pl,ori), 
	  _isInSubRow(numCells, false), _cellCoords(numCells),
          _isFixed(numCells, false), 
	  _isCoreCell(numCells, false), 
	  _storElt(numCells, false), 
	  _params(params), _nonConstThis(this)
        {}

    RBPlacement(unsigned numCells, const PlacementWOrient& pl, 
		const Parameters& params) 
   	: _hgWDims(NULL), _populated(false),
	  _placement(pl), _isInSubRow(numCells, false),
	  _cellCoords(numCells), 
          _isFixed(numCells, false), 
	  _isCoreCell(numCells, false), 
	  _storElt(numCells, false), 
	  _params(params), _nonConstThis(this)
   	{}

  public:

   RBPlacement() 
   	: _hgWDims(NULL), _populated(false), _cellsNotInRows(0), 
	  _placement(0), _params(), _nonConstThis(this)
   	{initBBox();}

   RBPlacement(const char* auxFileName, const Parameters& params);

	//this ctor reads only the netlist from the auxfile.
	//it will create a layout region with the target aspect ratio
	//and whitespace.  It also assumes the following:
	//core node widths are integer
	//core node heights are all the same
	//pad node dimensions are all the same
	//whitespace is a percent (i.e. 10 == 10%)
        // if aspect ratio is 0, there will be one row
   RBPlacement(const char* auxFile, double aspectRatio, double whiteSpace,
		const Parameters& params);

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

   operator const PlacementWOrient&()  	const {return _placement;}
   const PlacementWOrient& getPlacement()     {return _placement;}
   //operator const Placement&()  	const {return _placement;}

   const Point&  operator[](unsigned id) const { return _placement[id];}
   void  	 setLocation (unsigned id, const Point& pt);
   Orient&       getOrient   (unsigned id) { return _placement.getOrient(id);} 
   void  	 setOrient   (unsigned id, Orient ori)
			{ _placement.setOrient(id,ori); }

   // added by MRG to find the cell coord of a random point
   unsigned findCellIdx(const Point& point);
   // added by MRG
   const RBPCoreRow&  getCoreRow(unsigned id) const { return _coreRows[id];}
   // added by MRG to calculate total overlap
   double calcOverlap();
   double calcInstOverlap(vector<unsigned>& movables);

   //by sadya to calculate site area in given BBox
   double getContainedSiteAreaInBBox(const BBox& bbox);

   //by sadya to calculate overlaps regardless of rowstructure
   double calcOverlapGeneric(bool onlyMacros = false);
   //by sadya to align cells to closest core row y coordinates
   void alignCellsToRows();

   //check if all core cells have valid CellCoords
   bool checkCC();

   //added by MRG to calculate mean of set of pins
   Point calcMeanLoc(unsigned cellId, bool usePinLocs=false);
   // added by MRG to update placement of an arbitrary set of cells
   void updateCells(const vector<unsigned>& movables, const Placement& soln);

   // These return the row and subrow where a point would lie
   // These can't be const..it messes up the equal_range function
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

   //by sadya to evaluate HPWL in X and Y contributions
   pair<double, double> evalXYHPWL(bool usePinLocs=false) const;

   const HGraphWDimensions&	getHGraph() const { return *_hgWDims; }


   bool isConsistent();	
   friend ostream& operator<<(ostream&, const RBPlacement&);

	//save a gnu-plot usable file drawing the sub-rows
   void printRows(const char* filename);

   //by sadya get total site area of the layout
   double getSitesArea();
   void   saveAsSCL         (const char* baseFileName) const; 
   //save SCL with sites removed from congested regions
   void   saveAsSCLCong         (const char* baseFileName); 
   void   savePlNoDummy   (const char* baseFileName);
   void   removeSitesFromCongestedRgn(double whitespaceRatio=40);

   void   saveAsNodesNetsWts(const char* baseFileName, bool fixMacros = false)
                                                                        const;

   //save with fractWS % of whitespace as dummy cells
   void   saveAsNodesNetsWtsWDummy(const char* baseFileName, 
				   double fractWS=0, bool fixMacros = false);

   //save with fract % of cells tethered
   void   saveAsNodesNetsWtsWTether(const char* baseFileName, 
				    double fract=0, 
				    double rgnSizePercent=0.005,
                                    bool takeConstrFrmFile=false,
				    double tetherNewAR=-1,
				    double tetherNewWS=-1);

   //save with new nodes added to congested nodes
   void   saveAsNodesNetsWtsWCongInfo(const char* baseFileName,
				      double whitespaceRatio=20);

   //added by sadya to shred tall cells
   double getMaxHeightCoreRow(void) const;
   double getMinWidthNode(void) const;
   double getMaxWidthNode(void) const;
   double getAvgWidthNode(void) const;

   void   saveAsNodesNetsWtsShred(const char* baseFileName) const;
   void   saveAsNodesNetsWtsUnShred(const char* baseFileName) const;

   void   saveAsNodesNetsWtsShredHW(const char* baseFileName,
				    double maxHeight=0,
				    double maxWidth=0,
				    unsigned singleNetWt=0) const;
                                                //shred in both H & V direction
   void   savePlacementUnShredHW     (const char* plFileName)   const;
   clusterNodeOrient getOrientSubNode(unsigned index, unsigned indexRight,
				      unsigned indexTop) const;
   char * toChar(clusterNodeOrient orient) const;
   int getQuad(Point& pt) const;

   //save in floorplan format
   void   saveAsNodesNetsPlFloorplan(const char* baseFileName) const;
   void   saveMacrosAsNodesNetsPlFloorplan(const char* baseFileName) const;

   //added by sadya to remove sites occupied by macros
   void   removeSitesBelowMacros(void);
   //added by sadya to get number of macros in a design
   unsigned  getNumMacros(void);

   void   saveAsNetDAre     (const char* baseFileName) const;
   void   savePlacement     (const char* plFileName)   const;
// 	{ _placement.save(plFileName, _hgWDims->getNumTerminals());}

   void   saveAsSpatialConstraints (const char* constFileName) const;

   //added by sadya to output design in cplace format
   void   saveAsCplace         (const char* baseFileName); 
   //by sadya. output design in plato format
   void saveAsPlato (const char* baseFileName);

   //added by sadya. plots the nodes in gnuplot readable format
   void   saveAsPlot (const char* command,double xmin,double xmax,
 		      double ymin,double ymax, const char* baseFileName) const;

   //plot nodes with prefix dummy in their name
   void plotDummy(const char* command, double xmin, double xmax, double ymin, 
		  double ymax, const char* baseFileName) const;

   //by sadya. plot only the requested nodes and nets connected to them
   void plot(const char* command,vector<unsigned>& nodes, double xmin, 
	     double xmax, double ymin, double ymax, const char* baseFileName) 
             const;

   //plot nodes in the group constraints and their nets
   void plotGrpConstr(const char* command, double xmin, double xmax, 
		      double ymin, double ymax, const char* baseFileName)const;

   //by sadya to plot the local whitespace histogram based on bins
   void plotWSHist(const char* baseFileName);

   //by sadya to pack all cells to the left corner of the layout
   void shiftCellsLeft();

   //by sadya to allocate whitespace equally in all rows
   void spaceCellsEquallyInRows(void);

   //by sadya to allocate whitespace in all rows according to congestion info
   void spaceCellsWCongInfoInRows();
 
   //by sadya to allocate whitespace in all rows according to congestion info
   void spaceCellsWCongInfoInRows1();

   // added by sadya to remove overlaps in a given placement by using the
   //whitespace in a subrow
   void remOverlapsSR(void);

   //added by sadya to fix cells in sites if possible
   void snapCellsInSites(void);

   // added by sadya to remove overlaps in a given placement by using the
   //whitespace in a subrow and adjoining subrows
   void remOverlapsVert(void);
   void shuffleSR(itRBPSubRow its);
   unsigned getNumOverlaps();

   // added by sadya to remove overlaps using BFS search strategy
   void remOverlaps(void);

   // added by sadya. checks for out of core cells after placement
   bool checkOutOfCoreCells(void);

   //by sadya to find closest whitespace to a given point. for annealing
   bool findClosestWS(Point& loc, Point& WSLoc, double width);

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
   void saveLEFDEF(const char* baseFileName, bool markMacrosAsBlocks = false);

   //added by sadya to get the BBox of the layout core region + terminals
   void initBBox(void);
   BBox getBBox(bool addTerminals = true);

   //added by sadya to greedily assign pins to cells. Will change hgraph
   void assignPinsToCells();

   //only updates placement of cell. do not use if need to update row info as 
   //well. instead use setLocation
   void  	 updatePlacement(unsigned id, const Point& pt);
   void updatePlacementWOri(const PlacementWOrient& pl);

   //by sadya. gets ids of cells in given BBox
   void getCellsInBBox(BBox& bbox, vector<unsigned>& cellIds);

   vector< pair<BBox, vector<unsigned> > > groupRegionConstr;// in SYNP
   //by sadya for groups in region constraints. populated from constraints
   vector<unsigned> cellToGrpMapping;
   void initGroupRegionConstr(void); 
   vector< pair<BBox, double> > regionUtilization; //by sadya for region 
                                 //utilization constraints. added while at SYNP
   void initRegionUtilization(void);

   void reshapePlacement(double tetherNewAR, double tetherNewWS, 
			 const char* newPlFileName);

   void splitRowByObstacle(RBPCoreRow& cr, const BBox& obstacle);

   //by sadya assumes the isMacro field in HGraphWDims is updated
   void markMacrosAsFixed(void);
   void unmarkMacrosAsFixed(void);

};

typedef RBPlacement::Parameters RBPlaceParams;

class CompareCellIdsByX
{
    const Placement& _placement;
  public:
    CompareCellIdsByX(const Placement& place)
	:_placement(place)
	{}

    bool operator()(unsigned id1, unsigned id2)
    { return _placement[id1].x < _placement[id2].x;}
};


//added by sadya to generate LEF/DEF files from RBPlacement format
//used to sort the master nodes
struct sort_fn // : public binary_function<vector<double>, vector<double>, bool>
{
  bool operator()(vector<double> node1, vector<double> node2) 
  {
    unsigned size1 = node1.size();
    unsigned size2 = node2.size();
    unsigned minSize = (size1<size2) ? size1 : size2;
    if(node1[0]*node1[1] != node2[0]*node2[1])
      return (node1[0]*node1[1] < node2[0]*node2[1]);

    for(unsigned i=0;i<minSize;++i)
      {
	if(node1[i] != node2[i])
	  return (node1[i] < node2[i]);
      }
    return (node1[0] < node2[0]);
  }
};

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

#endif

