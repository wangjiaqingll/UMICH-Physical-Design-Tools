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











// Created by Igor Markov, 970223

//  Reworked for standalone ML by Igor Markov,  March 30, 1998

// CHANGES

// 980330   ilm  fixed an ML bug (feeding the same problem to partitioner)
//                pointed by aec
// 980413   ilm  split into doMLdoFlat.cxx, mlAux.cxx and remainder
// 980520   ilm  a series of updates to the new partitioning infrastructure
// 980912   aec  version 4.0 - uses ClusteredHGraph
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <set>
  using std::set;
#include "Geoms/plGeom.h"
#include "ClusteredHGraph/clustHGraph.h"
#include "Partitioning/partitioning.h"
#include "HGraph/hgFixed.h"
#include "mlPart.h"
#include "PartEvals/univPartEval.h"
#include "HGraph/subHGraph.h"

bool MLPart::_go()	//returns true if solns are valid, false
			//if they are not (ie, they were pruned)
{
    unsigned firstLevelKWay = 20 * _problem.getNumPartitions();
    if ((_params.clParams.sizeOfTop < firstLevelKWay) && 
	 (_problem.getNumPartitions() > 2))
    {
       abkwarn3(0,
         "Default 'firstLevelSize' too small for k-way, switching to ",
          firstLevelKWay," \n");
       _params.clParams.sizeOfTop = firstLevelKWay;
    }
    return _doMultiLevel();
}
 
MLPart::MLPart(PartitioningProblem& problem, const MLPartParams& params,
		FillableHierarchy* fill)
       : BaseMLPart(problem, params, fill)
{ 
   abkfatal(problem.getNumPartitions()==2,"More than 2-way"
   "partitioning is not supported in this distribution. For more"
   "details contact Igor Markov <imarkov@eecs.umich.edu>");

   Timer tm;
   const HGraphFixed*  graph=problem.getHGraphPointer();
  
   unsigned numNodes=graph->getNumNodes();
   unsigned numEdges=graph->getNumEdges();
   if (numEdges <= numNodes/2)
   {
     cerr << "\n NumEdges must be > numNodes/2 \n "
          << "The actual values are " << numEdges
          << " and " << numNodes << flush;
//    abkfatal(0," Cannot perform clustering ");
   }

   unsigned sizeOfTop = _params.clParams.sizeOfTop;

   if (sizeOfTop==0 || 
       graph->getNumNodes() - graph->getNumTerminals() <= sizeOfTop)
   {
       	if(_params.verb.forActions)
          cout << endl << "Problem too small for ML, running flat" << endl;
       	_soln2Buffers=NULL;
       	_callPartitionerWoML(problem);
	const PartitioningSolution& bestSol = _partitioner->getResult(_bestSolnNum);
	_bestSolnPartWeights = bestSol.partArea;
       	_figureBestCost(problem);
   }
   else
   {

      if (_params.verb.forActions > 1 || _params.verb.forMajStats > 1)
        cout << "\n Multilevel partitioning on "
             << problem.getHGraph().getNumNodes() << " nodes " 
             << " including "<<problem.getHGraph().getNumTerminals()
             << " terminals ";
     

      // allocate ML's double buffer --- an implicit parameter 
      // to the call _startWithPartProblem() below
      _soln2Buffers= new PartitioningDoubleBuffer(problem.getSolnBuffers());

      // setup for multiple clusterings
      unsigned solnBegin=_soln2Buffers->beginUsedSoln(),
               solnEnd  =_soln2Buffers->endUsedSoln();
  
      unsigned step=_params.runsPerClTree;
      double   numClTreesDbl = max(double(1),ceil((solnEnd-solnBegin)/(1.0*step)));
      unsigned numClTrees= static_cast<unsigned>(rint(numClTreesDbl));

      if (_params.verb.forActions || _params.verb.forMajStats)
      {
         cout << "\n Will now create "<<numClTrees<<" clusterTree";
         if (numClTrees!=1) cout <<"s and use each"; 
         cout << " to find ";
         if (step==1) cout <<"one solution"<<endl;
         else cout <<"(up to) "<<step<<" solutions"<<endl;
      }

// these for are primarily for debug output
     vector<double>   bestCosts(numClTrees,DBL_MAX);
     vector<unsigned> bestSolnNums(numClTrees,UINT_MAX);
     vector<double>   aveCosts(numClTrees,DBL_MAX);
     vector<unsigned> numLegalSolns(numClTrees,UINT_MAX);
// these allow to find "cre`me de la cre`me" of all solutions
     double    totalCost=0.0;
     unsigned  totalNumLegalSolns=0;
     double    curBestCost=UINT_MAX;
     unsigned  curBestSolnNum=UINT_MAX; 

     problem.propagateTerminals();

     for(unsigned clTreeNum=0, curBegin=0; clTreeNum!=numClTrees;
                  clTreeNum++, curBegin+=step)
     {
       // narrow the buffers
        unsigned curEnd=min(curBegin+step,solnEnd);
        _soln2Buffers->setBeginUsedSoln(curBegin);
        _soln2Buffers->setEndUsedSoln(curEnd);
        if (numClTrees>1)
        if (_params.verb.forActions>1 || _params.verb.forMajStats>1)
           cout << "  --- Tree " <<clTreeNum<<endl;


	PartitioningBuffer *mBuf, *sBuf;
	_soln2Buffers->checkOutBuf(mBuf, sBuf);

	const Partitioning& treePart = (*mBuf)[curBegin];
	abkwarn(curEnd-curBegin == 1, 
	  "Warning: multiple starts/tree. Dangerous with an initial solution");

        // setup to construct clustering
        ClustHGraphParameters clParams=_params.clParams;
        clParams.verb.forActions  /= 10;
        clParams.verb.forMajStats /= 10;
        clParams.verb.forSysRes   /= 10;


	if(clTreeNum%2)	//every other tree is HEM
	{
 	    clParams.clType       = ClHG_ClusteringType::HEM;
            clParams.weightOption = 6;
	}

	Timer clTreeCreationTime;
	ClusteredHGraph* hgraphs;

	if(_hierarchy != NULL)	//use the hierarchy
	{
	    hgraphs = new ClusteredHGraph(*_hierarchy, *graph, clParams);

	    //the clusteredHGraph ctor that takes a hierarchy does not
	    //deal with fixed constraints.  However, terminals aren't
	    //clustered, so they will have the same Id's in all 
	    //partitionings.
	    int           topLvlIdx = hgraphs->getNumLevels()-1;
	    Partitioning& topLvlFixed   =
		const_cast<Partitioning&>(hgraphs->getFixedConst(topLvlIdx));
	    Partitioning& topLvlPart    = 
		const_cast<Partitioning&>(hgraphs->getTopLevelPart());

	    for(unsigned t = 0; t < graph->getNumTerminals(); t++)
		topLvlFixed[t] = topLvlPart[t]  = problem.getFixedConstr()[t];

		//propigate the fixed constraints down...
	    for(topLvlIdx-- ; topLvlIdx >= 0; topLvlIdx--)
	    {
	      Partitioning& aboveFixed   =
		const_cast<Partitioning&>(hgraphs->getFixedConst(topLvlIdx+1));
	      Partitioning& belowFixed   =
		const_cast<Partitioning&>(hgraphs->getFixedConst(topLvlIdx));

	      hgraphs->mapPartitionings(aboveFixed, belowFixed, topLvlIdx);
	    }
	}
	else
            hgraphs = new ClusteredHGraph(*graph, clParams, 
				problem.getFixedConstr(), treePart);

	clTreeCreationTime.stop();
	_totalClusteringTime += clTreeCreationTime.getUserTime();

        MemUsage m1;
        _peakMemUsage = max(_peakMemUsage,m1.getPeakMem());

	_soln2Buffers->checkInBuf(mBuf, sBuf);

        _doingCycling=false;
	Timer unClusteringTime;


        _startWithPartProblem(problem, *hgraphs);
	delete hgraphs;
        MemUsage m2;
        _peakMemUsage = max(_peakMemUsage,m2.getPeakMem());

	unClusteringTime.stop();
	_totalUnClusteringTime += unClusteringTime.getUserTime();

        // collect stats
        bestCosts[clTreeNum]    =_bestCost;
        bestSolnNums[clTreeNum] =_bestSolnNum;
        aveCosts[clTreeNum]     =_aveCost;
        numLegalSolns[clTreeNum]=_numLegalSolns;

        totalNumLegalSolns += _numLegalSolns;
        totalCost          += _numLegalSolns * _aveCost;

        if(_bestCost < curBestCost) 
	{ 
	    curBestCost=_bestCost; curBestSolnNum=_bestSolnNum; 
	    const PartitioningSolution& bestSol = _partitioner->getBestResult();
	    _bestSolnPartWeights = bestSol.partArea;
   	}
	
     }

     if (_params.Vcycling!=Parameters::NoVcycles)
     {
        _doingCycling=true;

        if(_params.verb.forActions) 
		cout<<" Performing 1 V-Cycle"<< endl;

		//Vcycle on the best 1 solution 
        _soln2Buffers->setBeginUsedSoln(curBestSolnNum);
        _soln2Buffers->setEndUsedSoln(curBestSolnNum+1);

	_params.pruningPercent     = 1000; //turn pruning off 
	
	ClustHGraphParameters curParams = _params.clParams;
 	curParams.clType       = ClHG_ClusteringType::HEM;
        curParams.weightOption = 6;
	curParams.sizeOfTop = 
		static_cast<unsigned>(curParams.sizeOfTop * curParams.levelGrowth);
        curParams.verb.forActions  /= 10;
        curParams.verb.forMajStats /= 10;
        curParams.verb.forSysRes   /= 10;

        Partitioning& part=problem.getSolnBuffers()[curBestSolnNum];
	Timer clTreeCreationTime;
		//this is the VCycling clustering...don't use
		//the hierarchy here.
        ClusteredHGraph hgraphs(*graph, curParams, 
				problem.getFixedConstr(), part);
	clTreeCreationTime.stop();
	_totalClusteringTime += clTreeCreationTime.getUserTime();

        MemUsage m4;
        _peakMemUsage = max(_peakMemUsage,m4.getPeakMem());
	
        Timer unClusteringTime;

        _startWithPartProblem(problem, hgraphs);

	unClusteringTime.stop();
	_totalUnClusteringTime += unClusteringTime.getUserTime();
        
        MemUsage m3;
        _peakMemUsage = max(_peakMemUsage,m3.getPeakMem());

	curBestCost = _bestCost;
	const PartitioningSolution& bestSol = _partitioner->getBestResult();
	_bestSolnPartWeights = bestSol.partArea;
     }

// restore the initial buffer state
     _soln2Buffers->setBeginUsedSoln(solnBegin);
     _soln2Buffers->setEndUsedSoln(solnEnd);

     _bestCost     =curBestCost;
     _problem.setBestSolnNum(_bestSolnNum=curBestSolnNum);
     _numLegalSolns=totalNumLegalSolns;
     _aveCost      =totalCost / totalNumLegalSolns;

     _aveClusteringTime = _totalClusteringTime / numClTrees;
     _aveUnClusteringTime = _totalUnClusteringTime / numClTrees;
     _callPartTime /= numClTrees;

     if(_params.verb.forMajStats > 10)
     {
         UniversalPartEval lastCheck;
	 unsigned checkedBestCost = lastCheck.computeCost(_problem, 
				_problem.getBestSoln());
	 cout<<endl<<endl<<
		"Final Checked Best Cost is "<<checkedBestCost<<endl;
     }

   }
 
  tm.stop();
  if (_params.verb.forSysRes || _params.verb.forMajStats)
  {
      unsigned numStartsDone;
      if (_soln2Buffers==NULL) // was running flat
      {
         const PartitioningBuffer& _solnBuffer=problem.getSolnBuffers();
         numStartsDone = _solnBuffer.endUsedSoln()-_solnBuffer.beginUsedSoln();
      }
      else 
      {
         numStartsDone = _soln2Buffers->endUsedSoln() - 	
			       _soln2Buffers->beginUsedSoln();
      }

      cout<< " Best Cost in one ML ("<<numStartsDone<<" starts):"<< _bestCost
          << "   average :    " << _aveCost<<endl;
      cout<<" Part Areas: "<<getPartitionArea(0)
                <<" / "<<getPartitionArea(1)<<endl;
                                                            
      {
        unsigned pinB0=0, pinB1=0;
        const Partitioning& part= _problem.getBestSoln();
        const HGraphFixed& hg=_problem.getHGraph();
        for(unsigned n=0; n!=hg.getNumNodes(); n++)
        {
            unsigned deg=hg.getNodeByIdx(n).getDegree();
            if ( part[n].isInPart(0) ) pinB0 += deg;
            else  pinB1 += deg;
        }         
        cout<< " Pin balances :   " << pinB0 << " / " << pinB1 << endl;
      }

      cout << " One ML took           " << tm << endl;
      if (_soln2Buffers!=NULL) // non-trivial ML
      {
	if(_params.verb.forMajStats > 1)
	{
	
            cout<<" Ave Clustering Time Per Tree   : "
			<<_aveClusteringTime<<endl;
       	    cout<<" Ave UnClustering Time Per Tree : "
			<<_aveUnClusteringTime<<endl;
       	    cout<<" Ave CallPartitioner Time Per Tree: "
			<<_callPartTime<<endl;
	}
      }
   }
   _userTime = tm.getUserTime();
}

void MLPart::_startWithPartProblem(PartitioningProblem& problem, 
                                   ClusteredHGraph& hgraphs)
{ 
   abkfatal(&problem.getPartitions()!=NULL, " No partition BBoxes available ");

   const HGraphFixed*  graph=problem.getHGraphPointer();

  
   const vector<BBox>&       terminalBBoxes=problem.getPadBlocks();
   const vector<unsigned>&   terminalToBlock=problem.getTerminalToBlock();

   unsigned numTerminals=graph->getNumTerminals();

   char strbuf[100];
   sprintf(strbuf,"(%d vs %d)",terminalToBlock.size(),numTerminals);

   abkfatal2(terminalToBlock.size()>=numTerminals,
                                    " #terminals mismatch ",strbuf);

   populate(hgraphs, terminalToBlock, terminalBBoxes);
   bool validSoln = _go();
	//validSoln is true if the tree generated valid solns, false if not.

// Copy solutions from the double buffer to part. problem

   PartitioningBuffer *mainBuf=NULL, *shadowBuf=NULL;

   if(validSoln)
   {
   	_figureBestCost(problem);	//set's _bestSolnNum to the best
					//solution for this tree

       	_soln2Buffers->checkOutBuf(mainBuf,shadowBuf);

   	Partitioning& bestPart = problem.getSolnBuffers()[_bestSolnNum];

        for(unsigned nId = 0; nId < problem.getHGraph().getNumNodes(); nId++)
	{ bestPart[nId] = (*mainBuf)[_bestSolnNum][nId]; }

	if(_params.verb.forMajStats > 10)
	{
	    UnivPartEval checker;
	    unsigned checkedCost = checker.computeCost(problem, bestPart);
	    cout<<" Start W. PartProblem :Net Cut is "<<checkedCost<<endl;
	}

       	_soln2Buffers->checkInBuf(mainBuf,shadowBuf);
   }   
   else
	cout<<endl<<"No legal solutions were found"<<endl;
}

void MLPart::_figureBestCost(PartitioningProblem& problem)
{
   _bestSolnNum   = _partitioner->getBestSolnNum();
   _bestCost      = _partitioner->getBestSolnCost();
   _aveCost       = _partitioner->getAveSolnCost();
   _numLegalSolns = _partitioner->getNumLegalSolns();

   problem.setBestSolnNum(_bestSolnNum);
   return;
}








//by sadya+ramania for analytical clustering
MLPart::MLPart(PartitioningProblem& problem, const MLPartParams& params,
	       PlacementWOrient *placement,
	       FillableHierarchy* fill)
       : BaseMLPart(problem, params, fill)
{ 
  abkfatal(problem.getNumPartitions()==2,"More than 2-way"
   "partitioning is not supported in this distribution. For more"
   "details contact Igor Markov <imarkov@eecs.umich.edu>");


   Timer tm;
   const HGraphFixed*  graph=problem.getHGraphPointer();
  
   unsigned numNodes=graph->getNumNodes();
   unsigned numEdges=graph->getNumEdges();
   if (numEdges <= numNodes/2)
   {
     cerr << "\n NumEdges must be > numNodes/2 \n "
          << "The actual values are " << numEdges
          << " and " << numNodes << flush;
//    abkfatal(0," Cannot perform clustering ");
   }

   unsigned sizeOfTop = _params.clParams.sizeOfTop;

   if (sizeOfTop==0 || 
       graph->getNumNodes() - graph->getNumTerminals() <= sizeOfTop)
   {
       	if(_params.verb.forActions)
          cout << endl << "Problem too small for ML, running flat" << endl;
       	_soln2Buffers=NULL;
       	_callPartitionerWoML(problem);
	const PartitioningSolution& bestSol = _partitioner->getResult(_bestSolnNum);
	_bestSolnPartWeights = bestSol.partArea;
       	_figureBestCost(problem);
   }
   else
   {

      if (_params.verb.forActions > 1 || _params.verb.forMajStats > 1)
        cout << "\n Multilevel partitioning on "
             << problem.getHGraph().getNumNodes() << " nodes " 
             << " including "<<problem.getHGraph().getNumTerminals()
             << " terminals ";
     

      // allocate ML's double buffer --- an implicit parameter 
      // to the call _startWithPartProblem() below
      _soln2Buffers= new PartitioningDoubleBuffer(problem.getSolnBuffers());

      // setup for multiple clusterings
      unsigned solnBegin=_soln2Buffers->beginUsedSoln(),
               solnEnd  =_soln2Buffers->endUsedSoln();
  
      unsigned step=_params.runsPerClTree;
      double   numClTreesDbl = max(double(1),ceil((solnEnd-solnBegin)/(1.0*step)));
      unsigned numClTrees= static_cast<unsigned>(rint(numClTreesDbl));

      if (_params.verb.forActions || _params.verb.forMajStats)
      {
         cout << "\n Will now create "<<numClTrees<<" clusterTree";
         if (numClTrees!=1) cout <<"s and use each"; 
         cout << " to find ";
         if (step==1) cout <<"one solution"<<endl;
         else cout <<"(up to) "<<step<<" solutions"<<endl;
      }

// these for are primarily for debug output
     vector<double>   bestCosts(numClTrees,DBL_MAX);
     vector<unsigned> bestSolnNums(numClTrees,UINT_MAX);
     vector<double>   aveCosts(numClTrees,DBL_MAX);
     vector<unsigned> numLegalSolns(numClTrees,UINT_MAX);
// these allow to find "cre`me de la cre`me" of all solutions
     double    totalCost=0.0;
     unsigned  totalNumLegalSolns=0;
     double    curBestCost=UINT_MAX;
     unsigned  curBestSolnNum=UINT_MAX; 

     problem.propagateTerminals();

     for(unsigned clTreeNum=0, curBegin=0; clTreeNum!=numClTrees;
                  clTreeNum++, curBegin+=step)
     {
       // narrow the buffers
        unsigned curEnd=min(curBegin+step,solnEnd);
        _soln2Buffers->setBeginUsedSoln(curBegin);
        _soln2Buffers->setEndUsedSoln(curEnd);
        if (numClTrees>1)
        if (_params.verb.forActions>1 || _params.verb.forMajStats>1)
           cout << "  --- Tree " <<clTreeNum<<endl;


	PartitioningBuffer *mBuf, *sBuf;
	_soln2Buffers->checkOutBuf(mBuf, sBuf);

	const Partitioning& treePart = (*mBuf)[curBegin];
	abkwarn(curEnd-curBegin == 1, 
	  "Warning: multiple starts/tree. Dangerous with an initial solution");

        // setup to construct clustering
        ClustHGraphParameters clParams=_params.clParams;
        clParams.verb.forActions  /= 10;
        clParams.verb.forMajStats /= 10;
        clParams.verb.forSysRes   /= 10;


	if(clTreeNum%2)	//every other tree is HEM
	{
 	    clParams.clType       = ClHG_ClusteringType::HEM;
            clParams.weightOption = 6;
	}

	Timer clTreeCreationTime;
	ClusteredHGraph* hgraphs;

	if(_hierarchy != NULL)	//use the hierarchy
	{
	    hgraphs = new ClusteredHGraph(*_hierarchy, *graph, clParams);

	    //the clusteredHGraph ctor that takes a hierarchy does not
	    //deal with fixed constraints.  However, terminals aren't
	    //clustered, so they will have the same Id's in all 
	    //partitionings.
	    int           topLvlIdx = hgraphs->getNumLevels()-1;
	    Partitioning& topLvlFixed   =
		const_cast<Partitioning&>(hgraphs->getFixedConst(topLvlIdx));
	    Partitioning& topLvlPart    = 
		const_cast<Partitioning&>(hgraphs->getTopLevelPart());

	    for(unsigned t = 0; t < graph->getNumTerminals(); t++)
		topLvlFixed[t] = topLvlPart[t]  = problem.getFixedConstr()[t];

		//propigate the fixed constraints down...
	    for(topLvlIdx-- ; topLvlIdx >= 0; topLvlIdx--)
	    {
	      Partitioning& aboveFixed   =
		const_cast<Partitioning&>(hgraphs->getFixedConst(topLvlIdx+1));
	      Partitioning& belowFixed   =
		const_cast<Partitioning&>(hgraphs->getFixedConst(topLvlIdx));

	      hgraphs->mapPartitionings(aboveFixed, belowFixed, topLvlIdx);
	    }
	}
	else
	  {
            hgraphs = new ClusteredHGraph(*graph, clParams, 
					  problem.getFixedConstr(), treePart,
					  placement);
	  }
	clTreeCreationTime.stop();
	_totalClusteringTime += clTreeCreationTime.getUserTime();

	_soln2Buffers->checkInBuf(mBuf, sBuf);

        _doingCycling=false;
	Timer unClusteringTime;


        _startWithPartProblem(problem, *hgraphs);
	delete hgraphs;

	unClusteringTime.stop();
	_totalUnClusteringTime += unClusteringTime.getUserTime();

        // collect stats
        bestCosts[clTreeNum]    =_bestCost;
        bestSolnNums[clTreeNum] =_bestSolnNum;
        aveCosts[clTreeNum]     =_aveCost;
        numLegalSolns[clTreeNum]=_numLegalSolns;

        totalNumLegalSolns += _numLegalSolns;
        totalCost          += _numLegalSolns * _aveCost;

        if(_bestCost < curBestCost) 
	{ 
	    curBestCost=_bestCost; curBestSolnNum=_bestSolnNum; 
	    const PartitioningSolution& bestSol = _partitioner->getBestResult();
	    _bestSolnPartWeights = bestSol.partArea;
   	}
	
     }

     if (_params.Vcycling!=Parameters::NoVcycles)
     {
        _doingCycling=true;

        if(_params.verb.forActions) 
		cout<<" Performing 1 V-Cycle"<< endl;

		//Vcycle on the best 1 solution 
        _soln2Buffers->setBeginUsedSoln(curBestSolnNum);
        _soln2Buffers->setEndUsedSoln(curBestSolnNum+1);

	_params.pruningPercent     = 1000; //turn pruning off 
	
	ClustHGraphParameters curParams = _params.clParams;
 	curParams.clType       = ClHG_ClusteringType::HEM;
        curParams.weightOption = 6;
	curParams.sizeOfTop = 
		static_cast<unsigned>(curParams.sizeOfTop * curParams.levelGrowth);
        curParams.verb.forActions  /= 10;
        curParams.verb.forMajStats /= 10;
        curParams.verb.forSysRes   /= 10;

        Partitioning& part=problem.getSolnBuffers()[curBestSolnNum];
	Timer clTreeCreationTime;
		//this is the VCycling clustering...don't use
		//the hierarchy here.
        ClusteredHGraph hgraphs(*graph, curParams, 
				problem.getFixedConstr(), part);
	clTreeCreationTime.stop();
	_totalClusteringTime += clTreeCreationTime.getUserTime();

	Timer unClusteringTime;

        _startWithPartProblem(problem, hgraphs);

	unClusteringTime.stop();
	_totalUnClusteringTime += unClusteringTime.getUserTime();

	curBestCost = _bestCost;
	const PartitioningSolution& bestSol = _partitioner->getBestResult();
	_bestSolnPartWeights = bestSol.partArea;
     }

// restore the initial buffer state
     _soln2Buffers->setBeginUsedSoln(solnBegin);
     _soln2Buffers->setEndUsedSoln(solnEnd);

     _bestCost     =curBestCost;
     _problem.setBestSolnNum(_bestSolnNum=curBestSolnNum);
     _numLegalSolns=totalNumLegalSolns;
     _aveCost      =totalCost / totalNumLegalSolns;

     _aveClusteringTime = _totalClusteringTime / numClTrees;
     _aveUnClusteringTime = _totalUnClusteringTime / numClTrees;
     _callPartTime /= numClTrees;

     if(_params.verb.forMajStats > 10)
     {
         UniversalPartEval lastCheck;
	 unsigned checkedBestCost = lastCheck.computeCost(_problem, 
				_problem.getBestSoln());
	 cout<<endl<<endl<<
		"Final Checked Best Cost is "<<checkedBestCost<<endl;
     }

   }
 
  tm.stop();
  if (_params.verb.forSysRes || _params.verb.forMajStats)
  {
      unsigned numStartsDone;
      if (_soln2Buffers==NULL) // was running flat
      {
         const PartitioningBuffer& _solnBuffer=problem.getSolnBuffers();
         numStartsDone = _solnBuffer.endUsedSoln()-_solnBuffer.beginUsedSoln();
      }
      else 
      {
         numStartsDone = _soln2Buffers->endUsedSoln() - 	
			       _soln2Buffers->beginUsedSoln();
      }

      cout<< " Best Cost in one ML ("<<numStartsDone<<" starts):"<< _bestCost
          << "   average :    " << _aveCost<<endl;
      cout<<" Part Areas: "<<getPartitionArea(0)
                <<" / "<<getPartitionArea(1)<<endl;
                                                            
      {
        unsigned pinB0=0, pinB1=0;
        const Partitioning& part= _problem.getBestSoln();
        const HGraphFixed& hg=_problem.getHGraph();
        for(unsigned n=0; n!=hg.getNumNodes(); n++)
        {
            unsigned deg=hg.getNodeByIdx(n).getDegree();
            if ( part[n].isInPart(0) ) pinB0 += deg;
            else  pinB1 += deg;
        }         
        cout<< " Pin balances :   " << pinB0 << " / " << pinB1 << endl;
      }

      cout << " One ML took           " << tm << endl;
      if (_soln2Buffers!=NULL) // non-trivial ML
      {
	if(_params.verb.forMajStats > 1)
	{
	
            cout<<" Ave Clustering Time Per Tree   : "
			<<_aveClusteringTime<<endl;
       	    cout<<" Ave UnClustering Time Per Tree : "
			<<_aveUnClusteringTime<<endl;
       	    cout<<" Ave CallPartitioner Time Per Tree: "
			<<_callPartTime<<endl;
	}
      }
   }
   _userTime = tm.getUserTime();
}
