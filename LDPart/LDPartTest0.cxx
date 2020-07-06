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



// created by Igor Markov, 980609

// This example illustrates stand-alone ML

#include <string.h>
#include "ABKCommon/abkversion.h"
#include "Partitioning/partProb.h"
#include "MLPart/mlPart.h"
#include "DB/DB.h"
#include "NetTopology/dbHGr.h"
#include "PartEvals/partEvals.h"
#include "FilledHier/fillHier.h"

int main(int argc, const char *argv[]) 
{
    BoolParam    helpRequest1("h",   argc,argv),
                 helpRequest2("help",argc,argv);
    NoParams     noParams(argc,argv);

    StringParam  baseFileName("f",argc,argv);
    UnsignedParam numStarts("num", argc, argv);
    DoubleParam  toleranceIn("t", argc, argv);
    BoolParam    verticalCut("vertCut", argc, argv);
    StringParam  saveProb("save", argc, argv);

    StringParam  hclFile("hcl", argc, argv);

    StringParam  wtFileName("wtFileName",argc, argv);

    PRINT_VERSION_INFO
    cout << CmdLine(argc,argv);

    if (helpRequest1.found() || helpRequest2.found() || noParams.found())
    {
        cout << " -f   <aux filename>  \n"
             << " -num <multistarts>   \n"
             << " -t   <tolerance (0.1 = 10%)>\n"
             << " -vertCut  (for vertical cut, default = horizontal)\n"
             << " -save   <partitioning problem filename>\n"
             << " -wtFileName <filename>    (one weight per line)\n"
             << " -help " << endl << endl;
    }

    MLPartParams mlParams(argc,argv);
    PartitioningProblem::Parameters ppParams(argc, argv);
    cout << endl << mlParams;

    if (helpRequest1.found() || helpRequest2.found() || noParams.found())
        return 0;

    abkfatal(baseFileName.found()," -f baseFileName not found");

    BoolParam check("checkDB",argc,argv);
    DB::Parameters dbParams;
    dbParams.alwaysCheckConsistency = check;

    DB db(baseFileName,dbParams);

    HGfromDB hgDB(db);
    if (wtFileName.found())
    {
       cout << " Reading file " << wtFileName << " ..." << endl;
       ifstream wtFile(wtFileName);  
       abkfatal(  !wtFile.eof()  
               && (wtFile.peek() != -1) && wtFile ," Failed to open file ");

       itHGFEdgeGlobal      e;
       int lineNo=1, count=0;
       for(e=hgDB.edgesBegin(); e!=hgDB.edgesEnd(); e++)
       {
        double wt=-1.234567e89;
        wtFile >> eathash(lineNo) >> wt;
        if (wtFile.eof() || (wtFile.peek() == -1))
        {
           cerr << " *** Unexpected file end: read only " 
                << e-hgDB.edgesBegin() << " out of " 
                << hgDB.edgesEnd() - hgDB.edgesBegin() << " weights " << endl;
           abkfatal(0,"");
        }
        needeol(lineNo); 
        abkfatal2(wt!=-1.234567e89,
             "Failed read weight in or near line ", lineNo);
        (*e)->setWeight(wt);
        count++;
      }
      cout << " Successfully read " << hgDB.edgesEnd() - hgDB.edgesBegin() 
           << " net weights from file " << wtFileName << endl;
      hgDB.printEdgeWtStats(cout);
    }

    PartitionIds freeToMove;
    freeToMove.setToAll(2);
    Partitioning fixedConstr(hgDB.getNumNodes());

    // setting up partition BBoxes
    vector<BBox> partitions(2);
    BBox layoutBBox = db.getLayout().getBBox();
    partitions[0] = layoutBBox;
    partitions[1] = layoutBBox;
    if(verticalCut)
    {
        double centerX = (layoutBBox.xMin + layoutBBox.xMax)/2;
        partitions[0].xMax = centerX;
        partitions[1].xMin = centerX;
    }
    else
    {
        double centerY = (layoutBBox.yMin + layoutBBox.yMax)/2;
        partitions[0].yMax = centerY;
        partitions[1].yMin = centerY;
    }

    // compute total cell area for capacities
    double totalCellArea = 0;

    for(unsigned k=0; k!=hgDB.getNumNodes(); k++)
        totalCellArea += hgDB.getNodeByIdx(k).getWeight();

    // 2 partitions, each partition has 1 capacity = total area / 2
    vector<vector<double> > capacities(2, vector<double>(1,totalCellArea / 2));

    // set tolerance to 10% (default) or to user selection
    double desiredTolerance = 0.1;
    if (toleranceIn.found()) desiredTolerance = toleranceIn;
    vector<double> tolerances(1, desiredTolerance);

    // assign all nodes to no-block
    vector<unsigned> terminalToBlock(hgDB.getNumNodes(),UINT_MAX);;

    // storing all fixed terminals
    vector<BBox> padBlocks;
    for(unsigned i = 0; i < hgDB.getNumTerminals(); i++)
    {
        terminalToBlock[i] = padBlocks.size();
        padBlocks.push_back(db.spatial.locations[i]);
    }

    // set partitioning buffer size to number of starts
    unsigned nStrt = 1;
    if(numStarts.found()) nStrt = numStarts;
    PartitioningBuffer  soln(hgDB.getNumNodes(),nStrt);

    PartitioningProblem problem(hgDB, soln, fixedConstr, partitions,
              capacities, tolerances, terminalToBlock, padBlocks, ppParams);

    problem.propagateTerminals(0);
    if(saveProb.found())
        problem.saveAsNodesNets(saveProb);

    if(numStarts.found()) problem.reserveBuffers(numStarts);
    else problem.reserveBuffers(1);

    FillableHierarchy *hier = NULL;

    if(hclFile.found())
    {
        FillableHierarchyParameters  hierParams(argc, argv); 
	hier = new FillableHierarchy(hclFile, problem.getHGraph(), hierParams);
    }	

    MLPart mlPartitioner(problem,mlParams, hier);
    if(hier) delete hier; 
    hier = NULL;

    Partitioning& curPart =(problem.getSolnBuffers()[problem.getBestSolnNum()]);

    UnivPartEval costCheck(PartEvalType::NetCutWNetVec);
    cout<<"Net Cut:    "<<costCheck.computeCost(problem, curPart)<<endl;

    cout<<"Part Areas: "<<mlPartitioner.getPartitionArea(0)
		<<" / "<<mlPartitioner.getPartitionArea(1)<<endl;

    costCheck.printStatsForNon0CostNets(problem);

    problem.saveBestSol("best.sol");
    cout << SysInfo();

    return 0;
}

