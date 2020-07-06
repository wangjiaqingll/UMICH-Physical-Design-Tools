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











// Created: Feb 15, 2000 by Andrew Caldwell from capoPlacer.h

#ifndef __CAPOPARAMS_H__
#define __CAPOPARAMS_H__

#include "MLPart/mlPart.h"
#include "SmallPlacers/baseSmallPl.h"

class CapoSplitterParams
{
  public:

        //if doRepartitioning, then the first tolerance is always
        //20%, and the second tolerance follows the parameters below
    bool        doRepartitioning;
 
    // if repartSmallWS then whenever partitioning a block with < 2% WhiteSpace
    // use Repartitioning 
    bool repartSmallWS;

    // if useQuadCluster then use quadratic Clustering during MLPart
    bool useQuadCluster;

        //if <useWSTolMethod>, then use the computation from the
	//whitespace report for horizontal cuts.  Otherwise,
	//use <constantTolerance>.
	//If doRepartitioning is enabled, then v-cuts will 
	//repartition with 1% tolerance after shifting.
    bool        useWSTolMethod;
    double      constantTolerance;

    //now both H-cuts and V-cuts use useWSTolMethod.
    //if uniformWS is enabled then V-cuts always use a tolerance of 
    //<constantTolerance>, and shift the cutline.  
    bool        uniformWS;
 
    unsigned    numMLSets;

    bool	useNameHierarchy;
    char*       delimiters;

     // by sadya in SYNP to honour region constraints
    bool useRgnConstr; 
    
    //by sadya in SYNP to try different partition capacities in presence of
    //filler cells to force the std-cells in the center of the design
    bool fillerCellsPartCaps;

    Verbosity   verb;
 
    CapoSplitterParams(int argc, const char *argv[]);
    CapoSplitterParams(Verbosity verbosity = Verbosity("0_0_0"))
        : doRepartitioning(true), repartSmallWS(true), useWSTolMethod(true),
          constantTolerance(10), uniformWS(true), numMLSets(2), 
	  useNameHierarchy(false), delimiters((char*)"/"), 
          useRgnConstr(false), fillerCellsPartCaps(false), verb(verbosity)
        {}
 
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

    unsigned saveAtBlocks;  //if !=0, save when >= this many blocks
                            //default == 0 => run till end.

    ReplaceSmallBlocksType 	replaceSmallBlocks;

    bool     useActualPinLocs;

      //   PARAMETERS EQUALLY APPLICABLE TO EACH LEVEL
    unsigned smallPartThreshold; //for problems of size <= the
				//threshold, use smallPartitioner
    unsigned smallPlaceThreshold; //call the end-case placer on 
				//one-row problems with <= threshold #cells
    unsigned smallSplitThreshold; //call the small block splitter on
                                 //blocks with <= threshold # cells


    bool     doOverlapping;
    unsigned startOverlappingLayer;//the first level to overlap on
    unsigned endOverlappingLayer;  //the last level to overlap at

    //parameters for capo boosting. by sherief. ported by sadya
    unsigned boostLayer;
    unsigned boostFactor;

    bool	 propFromPl;	//do termi-prop w/ results of boxPl
    bool	 useKPl;

    //by sadya in SYNP to honour group region constraints
    bool useGrpConstr;  

      //   PARTITIONER PARAMETERS
    MLPartParams	mlParams;
    SmallPlParams	smplParams;

    //by sadya for top down congestion driven partitioning
    bool                tdCongestionCtl;
    //by sadya to incorporate net effect
    bool                capoNE;
    bool                noCOG; //don't use center of gravity constraints
    bool                useQuad; //generate quadratic min soln before each layer. usefull for terminal propagation
    unsigned            quadSkipNetsLargerThan; //during SOR for quadratic min soln, skip nets larger than this number for time efficiency

      //lookAhead > 1 requires use of the LASplitter
    unsigned	lookAhead;

    bool		allowRowSplits;
    CapoSplitterParams  splitterParams;

      //  INFO-TRACKING PARAMETERS
    bool            plotBlocks;
    bool            boost;
    bool            saveLayerBBoxes;
    bool            saveBlocks;
    bool            saveSmallPlProb;

    bool saveBlocksFloorplan;       //save blocks before detailed placement

    CapoParameters();
    CapoParameters(int argc, const char *argv[]);
    CapoParameters(const CapoParameters &srcParams);

    void printHelp();

    friend ostream& operator<<(ostream& os, CapoParameters &params);
};


#endif
