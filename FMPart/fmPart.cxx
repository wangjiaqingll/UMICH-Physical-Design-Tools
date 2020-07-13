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







#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "fmPart.h"
#include "PartEvals/netCut2way.h"
#include "PartEvals/netCut2wayWWeights.h"
#include "PartLegality/1balanceGen.h"
#include "PartLegality/bfsGen.h"
#include "dcGainCont.h"
#include "mmSwitchBox.h"
#include "PartLegality/solnGenRegistry.h"

FMPartitioner::FMPartitioner(
		PartitioningProblem& problem,
                const Parameters& params, 
		bool  skipSolnGen,
		unsigned randSeed)
	: MultiStartPartitioner(problem, params, randSeed),
          _activeMoveMgr(NULL), _moveMgrLIFO(NULL), _moveMgr2(NULL),
          _switchBox(NULL),
	  _totalMovesMade(0), _totalTime(0.0), _peakMemUsage(0.0),
          _skipSolnGen(skipSolnGen)
{
  abkfatal(problem.getNumPartitions()==2,"More than 2-way"
   "partitioning is not supported in this distribution. For more"
   "details contact Igor Markov <imarkov@eecs.umich.edu>");


    if(_params.verb.forMajStats > 5)
        cout << " Using " << _params.eval << " evaluator " << endl;

    setMoveManagerAndSwitchBox(); // once for all starts

    MemUsage m;
    _peakMemUsage = max(_peakMemUsage,m.getPeakMem());
  

    runMultiStart(); 

    MemUsage m1;
    _peakMemUsage = max(_peakMemUsage,m1.getPeakMem());


    if(_params.verb.forMajStats > 4)
    {
        cout<<" Made "<<_totalMovesMade<<" in "<<_totalTime<<" seconds.  ";
        cout<<(double)_totalMovesMade / _totalTime<<" moves per second"<<endl;
    }
    if (_params.printHillStats)
    {
      cout  << " Max hill seen (h/w) : "
            << _maxHillHeightSeen    << "/" << _maxHillWidthSeen << endl;
      cout  << " Max good hill seen (h/w) : "
            << _maxGoodHillHeightSeen << "/" << _maxGoodHillWidthSeen << endl;
    }
    MemUsage m2;
    _peakMemUsage = max(_peakMemUsage,m2.getPeakMem());
}

 
void FMPartitioner::doOneFM(unsigned initSoln)
{
    Timer moveTimer;
 
    Partitioning& curPart = const_cast<Partitioning&>
                                (_solutions[initSoln]->part);

    if(!_skipSolnGen)
    {
	if(_problem.getNumPartitions() == 2)
	{
          switch (_params.solnGen)
          {
            case SolnGenType::RandomizedEngineers :
                {
		//	    SingleBalanceGenerator2way sGen(_problem);
			    SBGWeightRand              sGen(_problem);
			    sGen.generateSoln(curPart);
		            break;
                }
            case SolnGenType::AllToOne  : {AllToOneGen  sGen(_problem);
                                         sGen.generateSoln(curPart);
                                         break; }
            case SolnGenType::Bfs	: 
            {
                            SBGWeightBfs           sGen(_problem);
                            sGen.generateSoln(curPart);
                            break;
            }
            default : abkfatal(0,"Unknown solution generator");
          }
	}
	else 
	{
            SingleBalanceGenerator sGen(_problem);
            sGen.generateSoln(curPart);
	}

    }

    _moveMgrLIFO->resetTo(*_solutions[initSoln]);
     if (_moveMgrLIFO!=_moveMgr2) _moveMgr2->resetTo(*_solutions[initSoln]);
    _activeMoveMgr=_moveMgrLIFO;

    if(_params.verb.forMajStats > 5)
    {
   	cout<<"Min Areas  ["<<_problem.getMinCapacities()[0][0]<<" ";
   	cout<<_problem.getMinCapacities()[1][0]<<"]"<<endl;
  	cout<<"Init Areas ["<<_solutions[initSoln]->partArea[0][0]<<" ";
   	cout<<_solutions[initSoln]->partArea[1][0]<<"]"<<endl;
   	cout<<"Max Areas  ["<<_problem.getMaxCapacities()[0][0]<<" ";
  	cout<<_problem.getMaxCapacities()[1][0]<<"]"<<endl;
    }

    double oldCost 	= _activeMoveMgr->getCost();
    _bestPassCost	= _activeMoveMgr->getCost();
    unsigned useClip 	= _params.useClip;
    bool     done 	= false;

    unsigned passNumber = 0;
    unsigned maxNumPasses
             = ( _params.maxNumPasses ? _params.maxNumPasses : UINT_MAX);

    _switchBox->reinitialize();

    while (!done && passNumber < maxNumPasses)
    { 
        if ( _switchBox->quitNow() ) { done= true; break; }
        if ( _switchBox->swapMMs() )
        { _activeMoveMgr =
             ( (_activeMoveMgr==_moveMgrLIFO) ? _moveMgr2 : _moveMgrLIFO ); 
        }

	oldCost = _bestPassCost;

	if(_params.verb.forActions > 3)
	{
	    if (_switchBox->isSecondMMinUse())
            {
               if (useClip) cout<<"Pass "<<setw(3)<<passNumber<<" (Clip)";
               else         cout<<"Pass "<<setw(3)<<passNumber<<" (LIFO2)";
            }
	    else            cout<<"Pass "<<setw(3)<<passNumber<<" (LIFO)";
	}

        _totalMovesMade +=doOneFMPass(useClip && _switchBox->isSecondMMinUse());

	if( (oldCost-_bestPassCost) <= oldCost*_params.minPassImprovement*0.01)
              _switchBox->passFailed();
        else  _switchBox->passImprovedCost();

    	if(_params.verb.forMajStats > 3)
        {
	    cout<< "  cost: " << setw(5)<<_bestPassCost<<" [";
	    for(unsigned k = 0; k < _problem.getNumPartitions(); k++)
		cout<<setw(10)<<_bestPassAreas[k]<<" ";
	    cout<<"]\n";
	} 
	passNumber++;
    };
/*
    Timer uncutTimer;
    _activeMoveMgr=_moveMgr2;
    _activeMoveMgr->uncutNets();
    uncutTimer.stop();
    double lastCost=_activeMoveMgr->getCost();
    if (_params.verb.forMajStats > 2 || _params.verb.forSysRes >2)
       cout << "Net uncutting took: " << uncutTimer << endl;
    
    if(_params.verb.forMajStats > 2)
    {
       if (lastCost < _bestPassCost)
         cout << " Net uncutting improved by " << _bestPassCost-lastCost
              << (100.0*(_bestPassCost-lastCost))/_bestPassCost<<"%"<<endl;
       else cout << "Net uncutting did not help" << endl;
    } 
   
    if (lastCost < _bestPassCost)
    {
       _bestPassCost=lastCost;
       _bestPassViolation=_activeMoveMgr->getViolation();
       _bestPassImbalance=_activeMoveMgr->getImbalance();
    }
*/
    PartitioningSolution& soln = *_solutions[initSoln];

    soln.cost      = static_cast<unsigned>(rint(_bestPassCost));
    soln.violation = _bestPassViolation;
    soln.imbalance = _bestPassImbalance;

    abkassert(soln.partArea.size() == _bestPassAreas.size(),
	"size mismatch for soln areas");

    for(unsigned k=0; k<_problem.getNumPartitions(); k++) 
	soln.partArea[k][0] = _bestPassAreas[k];

    moveTimer.stop();
    double uTime = moveTimer.getUserTime();
    _totalTime += uTime;
    if(_params.verb.forMajStats > 5)
    {
    	cout<<" Made "<<_totalMovesMade<<" in "<<uTime<<" secs.  ";
    	cout<<(double) _totalMovesMade / uTime<<" moves per second"<<endl;
    }


   #ifdef ABKDEBUG
    for(unsigned nId = 0; nId < _problem.getHGraph().getNumNodes(); nId++)
	abkassert(soln.part[nId].numberPartitions() == 1,
		"finished partitioning w. node in <> 1 partition");
   #endif

}

unsigned FMPartitioner::doOneFMPass(bool useClip)
{
    Timer passInitTime;
    Timer passTotalTime;

    _activeMoveMgr->reinitialize();	//we still do 1 of these / pass, but,
				//at the begining of the pass the GainCo &
				//move log are already empty, so it's faster

    _activeMoveMgr->computeGains();
    if(useClip) _activeMoveMgr->setupClip();

    _bestPassCost 		= _activeMoveMgr->getCost();
    _bestPassImbalance          = _activeMoveMgr->getImbalance();
    _bestPassViolation          = _activeMoveMgr->getViolation();
    _bestPassAreas              = _activeMoveMgr->getAreas();

    if(_params.verb.forMajStats > 5)
	cout<<"Start of the pass cost : "<<_bestPassCost<<endl;


    unsigned bestMoveNum = 0;
    unsigned movesMade   = 0;
    unsigned moveLimit   = UINT_MAX;
    if(_params.maxNumMoves != 0 && _params.useEarlyStop)
	moveLimit = _params.maxNumMoves;

    double maxHillHeight=_params.maxHillHeightFactor;
    if (maxHillHeight==0) maxHillHeight=1000;

    unsigned numModules = _problem.getHGraph().getNumNodes();
    unsigned maxConsecNonImprovingMoves=static_cast<unsigned>(
                                  floor(0.01*_params.maxHillWidth*numModules));
    unsigned consecNonImprovingMoves=0;

    passInitTime.stop();
    Timer passMovesTime;

 if (!_params.printHillStats)
 { 
    if (! _params.useEarlyStop ||
          (maxHillHeight >= 99 && _params.maxHillWidth >= 100) )
       while(movesMade < moveLimit && _activeMoveMgr->pickMoveApplyIt())
       {
     	   movesMade++;
           double curCost       = _activeMoveMgr->getCost();
	   double curViolation  = _activeMoveMgr->getViolation();

	   if(curViolation < _bestPassViolation) //a more legal soln..take it
	   {
   	       _bestPassCost   = curCost;
   	       bestMoveNum     = movesMade;
               _bestPassImbalance = _activeMoveMgr->getImbalance();
	       _bestPassViolation = curViolation;
	       _bestPassAreas  = _activeMoveMgr->getAreas();
	   }
	   else if(curViolation == _bestPassViolation)
	   {
   	     if(curCost < _bestPassCost)
   	     {
   	       _bestPassCost   = curCost;
   	       bestMoveNum     = movesMade;
               _bestPassImbalance = _activeMoveMgr->getImbalance();
	       _bestPassViolation = _activeMoveMgr->getViolation();
	       _bestPassAreas  = _activeMoveMgr->getAreas();
	     }
             else if (curCost==_bestPassCost)
             {
               double newImbalance=_activeMoveMgr->getImbalance();
               if (newImbalance< _bestPassImbalance)
               {
                  bestMoveNum = movesMade;
                  _bestPassImbalance = newImbalance;
	          _bestPassViolation = _activeMoveMgr->getViolation();
                  _bestPassAreas  = _activeMoveMgr->getAreas();
	       }
             }
	   }
        }
    else
       while(movesMade < moveLimit && _activeMoveMgr->pickMoveApplyIt())
       {
     	   movesMade++;
           double curCost      = _activeMoveMgr->getCost();
	   double curViolation = _activeMoveMgr->getViolation();

	   if(curViolation < _bestPassViolation) //a more legal soln..take it
	   {
   	       _bestPassCost   = curCost;
   	       bestMoveNum     = movesMade;
               _bestPassImbalance = _activeMoveMgr->getImbalance();
	       _bestPassViolation = curViolation;
	       _bestPassAreas  = _activeMoveMgr->getAreas();
               consecNonImprovingMoves=0;
	   }
	   else if(curViolation == _bestPassViolation)
	   {
   	     if(curCost < _bestPassCost)
   	     {
   	       _bestPassCost = curCost;
   	       bestMoveNum   = movesMade;
               _bestPassImbalance = _activeMoveMgr->getImbalance();
	       _bestPassViolation = _activeMoveMgr->getViolation();
	       _bestPassAreas = _activeMoveMgr->getAreas();
               consecNonImprovingMoves=0;
	     }
             else if (curCost==_bestPassCost)
             {
               double newImbalance=_activeMoveMgr->getImbalance();
               if (newImbalance<_bestPassImbalance)
               {
                  bestMoveNum = movesMade;
                  _bestPassImbalance = newImbalance;
	          _bestPassViolation = _activeMoveMgr->getViolation();
	          _bestPassAreas = _activeMoveMgr->getAreas();
                  consecNonImprovingMoves=0;
               } 
               else consecNonImprovingMoves++;
             }
             else 
             {  
             if ( consecNonImprovingMoves++ >= maxConsecNonImprovingMoves
                 || curCost > _bestPassCost *  maxHillHeight)  break;
             }
	   }
        }
 }
 else	//track hill heights
 {
   _maxHillHeightSeenThisPass=1;
   _maxHillWidthSeenThisPass=0;
   while(movesMade < moveLimit && _activeMoveMgr->pickMoveApplyIt())
   {
        movesMade++;
        double curCost      = _activeMoveMgr->getCost();
	double curViolation = _activeMoveMgr->getViolation();

	if(curViolation < _bestPassViolation) //a more legal soln..take it
	{
   	    _bestPassCost   = curCost;
   	    bestMoveNum     = movesMade;
            _bestPassImbalance = _activeMoveMgr->getImbalance();
	    _bestPassViolation = curViolation;
	    _bestPassAreas  = _activeMoveMgr->getAreas();
	}
	else if(curViolation == _bestPassViolation)
	{
          if(curCost < _bestPassCost)
          {
            _bestPassCost = curCost;
            bestMoveNum   = movesMade;
            _bestPassImbalance = _activeMoveMgr->getImbalance();

            if (_maxHillHeightSeenThisPass > _maxGoodHillHeightSeen)
                         _maxGoodHillHeightSeen = _maxHillHeightSeenThisPass;
            if (_maxHillWidthSeenThisPass  > _maxGoodHillWidthSeen)
                         _maxGoodHillWidthSeen  = _maxHillWidthSeenThisPass;

            consecNonImprovingMoves=0;
          }
          else
          {
            if (curCost==_bestPassCost)
            {
               double newImbalance=_activeMoveMgr->getImbalance();
               if (newImbalance<_bestPassImbalance)
               {
                  _bestPassCost   = curCost;
                  bestMoveNum     = movesMade;
                  _bestPassImbalance = newImbalance;
		  _bestPassViolation = _activeMoveMgr->getViolation();
		  _bestPassAreas  = _activeMoveMgr->getAreas();
                  consecNonImprovingMoves=0;
               }
               else consecNonImprovingMoves++;
            }
            else consecNonImprovingMoves++;

            double curHillHeight = curCost / _bestPassCost;
            double curHillWidth=consecNonImprovingMoves * (100.0 / numModules);
            if (curHillHeight > _maxHillHeightSeenThisPass) 
                                    _maxHillHeightSeenThisPass = curHillHeight;
            if (curHillWidth  > _maxHillWidthSeenThisPass)
                                    _maxHillWidthSeenThisPass  = curHillWidth;
            if (curHillHeight >  maxHillHeight || 
                consecNonImprovingMoves > maxConsecNonImprovingMoves) break;
          }
	}
   }
   if (_maxHillHeightSeenThisPass > _maxHillHeightSeen)
            _maxHillHeightSeen=_maxHillHeightSeenThisPass;
   if (_maxHillWidthSeenThisPass > _maxHillWidthSeen)
            _maxHillWidthSeen=_maxHillWidthSeenThisPass;
 }

    if(_params.verb.forMajStats > 5)
    {	
	cout<<" best Cost was :"<<_bestPassCost<<" at move "<<bestMoveNum
		<<" out of "<<movesMade<<" total moves"<<endl;
    	cout<<"undoing "<<movesMade-bestMoveNum<<endl;
    }

    passMovesTime.stop();
    Timer passUndoTime;
     if (_params.saveMoveLog) 
//   if (passNumber < maxNumPasses &&
//       (oldCost-_bestPassCost) <= oldCost*_params.minPassImprovement*0.01)
       _moveLog=_activeMoveMgr->getMoveLog();
     _activeMoveMgr->undo(movesMade - bestMoveNum);
    passUndoTime.stop();

    passTotalTime.stop();
  
	if(_params.verb.forMajStats > 4)
        { 
	  cout<<"Setup: "<<passInitTime.getUserTime()
	    <<" Moves: "<<passMovesTime.getUserTime()
	    <<" Undo: "<<passUndoTime.getUserTime()
	    <<" Total Pass Time: "<<passTotalTime.getUserTime()<<endl;
    	}

    return movesMade;
}

