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

#include <vector>
using std::vector;
#include <algorithm>
#include "Ctainers/bitBoard.h"
#include "capoPlacer.h"
#include "capoBlock.h"
#include "partProbForCapo.h"
#include "FMPart/fmPart.h"

// Created by Igor Markov on March 24, 1999
// the algorithm is gnarly, so don't try to modify for no reason

#define DEBUGINFO 0
// could be 1

class CapoBlockMatchRec
{
      CapoBlock * _block0;
      CapoBlock * _block1;
      double      _deltaWS;
 
   public:
      bool used;

      CapoBlockMatchRec(): 
         _block0(NULL), _block1(NULL), _deltaWS(-DBL_MAX), used(false) {};
      CapoBlockMatchRec(CapoBlock * b0, CapoBlock * b1): 
         _block0(b0), _block1(b1), used(false) 
         { recomputeDeltaWS(); }
      CapoBlock* getBlock0() { return _block0;}
      CapoBlock* getBlock1() { return _block1;}
      unsigned   getIndex0() { return _block0->getIndex(); }
      unsigned   getIndex1() { return _block1->getIndex(); }

      double getDeltaWS()   const { return _deltaWS; }
      void   recomputeDeltaWS();
      bool   hasSiblings() const { return _block0->_sibling==_block1; }
};

bool matchRecUsed(const CapoBlockMatchRec& mr) { return mr.used; }

void CapoBlockMatchRec::recomputeDeltaWS()
{
 _deltaWS= abs(_block0->getRelativeWhitespace()- 
               _block1->getRelativeWhitespace()
              )*100;  // to have units in %
}

bool operator<(const CapoBlockMatchRec& r1, const CapoBlockMatchRec& r2)
{ return r1.getDeltaWS() >r2.getDeltaWS(); }
// so that sort() results in a *descending* order

ostream& operator<<(ostream& os, CapoBlockMatchRec&  curB)
{
  os <<" Indices: "<< curB.getBlock1()->getIndex()
            << "," << curB.getBlock0()->getIndex()
                   << " imbalance " << curB.getDeltaWS() << "%";
  return os;
}

void CapoPlacer::doOverlapping(vector<CapoBlock*>& layout)
{
   vector<CapoBlockMatchRec> edges;
   vector<CapoBlockMatchRec> siblings;
   edges.reserve(layout.size());    // may be several x more
   siblings.reserve(layout.size()); // may be several x more
 
   for(unsigned b=0; b!=layout.size(); b++)
   {
        CapoBlock* curB=layout[b]; 

	if(curB->getNumCells() <= 10) continue;

        unsigned k;

//      cout << "----------" << *curB << "-----" << endl;

        for(k=0; k!=curB->_neighborsAbove.size(); k++)
        {
           CapoBlock* otherB=curB->_neighborsAbove[k];
	   if(otherB->getNumCells() <= 10) continue;

           CapoBlockMatchRec tmp(curB,otherB);
           if (tmp.hasSiblings()) siblings.push_back(tmp);
           else edges.push_back(tmp);
        };

        for(k=0; k!=curB->_leftNeighbors.size(); k++)
        {
           CapoBlock* otherB=curB->_leftNeighbors[k];
	   if(otherB->getNumCells() <= 10) continue;

           CapoBlockMatchRec tmp(curB,otherB);
           if (tmp.hasSiblings()) siblings.push_back(tmp);
           else edges.push_back(tmp);
        };
   };

   std::sort(edges.begin(), edges.end());

   if (edges.empty()) return;

   edges.reserve(edges.size()+siblings.size());

 
   if(DEBUGINFO || _params.verb.forMajStats > 2)
   { 
      cout << " --- Overlapping candidates : " << endl;
      if (edges.empty()) cout << "      none" << endl;
      else for(unsigned k=0; k!=edges.size(); k++) cout << edges[k] << endl;

      cout << " --- Siblings : " << endl;
      if (siblings.empty()) cout << "      none" << endl;
      else for(unsigned k=0; k!=siblings.size(); k++) cout <<siblings[k]<<endl;
      cout << endl;
   }

 
   vector<CapoBlockMatchRec>   matching; matching.reserve(layout.size());
   std::vector<CapoBlockMatchRec>::iterator firstAvailable=edges.begin(),
                                       counter       =edges.begin();
   BitBoard takenBlocks(layout.size());
   abkassert(layout.size()," Empty layout ?");
   unsigned firstIndex=layout[0]->getIndex();


/* Pass one out of four */

   while(firstAvailable<edges.end() && matching.size()<layout.size())
   {
      unsigned blockNum0=firstAvailable->getIndex0()-firstIndex,
               blockNum1=firstAvailable->getIndex1()-firstIndex;
      if (takenBlocks.isBitSet(blockNum0) || takenBlocks.isBitSet(blockNum1))
          firstAvailable++;  // can't use firstAvailable for matching
      else // can use for matching
      {
          takenBlocks.setBit(blockNum0), takenBlocks.setBit(blockNum1);
          matching.push_back(*firstAvailable);
          firstAvailable++->used=true;
      }
   }

 
   edges.erase(std::remove_if(edges.begin(), edges.end(), matchRecUsed), 
             edges.end());

   // now have matching, and potentially useful neighboring pairs in the range
   // [firstAvailable, edges.end()] 

   if(DEBUGINFO || _params.verb.forMajStats > 2)
   {
       cout << " --- First matching : " << endl;
       if (matching.empty()) cout << "      empty" << endl;
       else for(counter=matching.begin(); counter!=matching.end(); counter++)
         cout << *counter << endl;
       cout << endl;
   }


   // apply matching here
   unsigned k;
   for(k=0; k!=matching.size(); k++)
       repartitionBlocks(matching[k].getBlock0(), matching[k].getBlock1());

/* Passes two to four */

   // add siblings
   edges.insert(edges.end(),siblings.begin(),siblings.end());
   firstAvailable=edges.begin();

   unsigned passNum;
   for(passNum=2; passNum!=5; passNum++)
   {
     matching.clear();
     takenBlocks.clear();

     for(counter=firstAvailable; counter!=edges.end(); counter++)
       counter->recomputeDeltaWS();
     std::sort(firstAvailable, edges.end());

     if(DEBUGINFO || _params.verb.forMajStats > 2)
     {
       cout << " --- Candidates for matching #"<<passNum<<" : " << endl;
       if (firstAvailable>=edges.end()) cout << "     none" << endl;
       else for(counter=firstAvailable; counter!=edges.end(); counter++)
          cout << *counter << endl;
       cout << endl;
     }

     while(firstAvailable<edges.end() && matching.size()<layout.size())
     {
       unsigned blockNum0=firstAvailable->getIndex0()-firstIndex,
                blockNum1=firstAvailable->getIndex1()-firstIndex;
       if (takenBlocks.isBitSet(blockNum0) || takenBlocks.isBitSet(blockNum1))
          firstAvailable++;  // can't use firstAvailable for matching
       else // can use for matching
       {
          takenBlocks.setBit(blockNum0), takenBlocks.setBit(blockNum1);
          matching.push_back(*firstAvailable);
          firstAvailable++->used=true;
       }
     }

    edges.erase(std::remove_if(edges.begin(), edges.end(), matchRecUsed), 
             edges.end());
    firstAvailable=edges.begin();

    // now have matching, and potentially useful neighboring pairs in the range
    // [firstAvailable, edges.end()] 

    if(DEBUGINFO || _params.verb.forMajStats > 2)
    {
       cout << " --- Matching # " << passNum <<" : " << endl;
       if (matching.empty()) cout << "      empty" << endl;
       else for(counter=matching.begin(); counter!=matching.end(); counter++)
            cout << *counter << endl;
       cout << endl;
    }

    // apply matching here
    for(k=0; k!=matching.size(); k++)
        repartitionBlocks(matching[k].getBlock0(), matching[k].getBlock1());
    
    if (matching.empty()) break;
  }
}


void CapoPlacer::repartitionBlocks(CapoBlock* block0, CapoBlock* block1)
{
     
   if (block0->getNumCells()<10 || block1->getNumCells()<10) return;

   if((block0->getBBox().yMin >= block1->getBBox().yMax) ||
      (block0->getBBox().xMax <= block1->getBBox().xMin))
   {}//order is correct..do nothing
   else if((block1->getBBox().yMin >= block0->getBBox().yMax) ||
           (block1->getBBox().xMax <= block0->getBBox().xMin))
   {   //blocks reversed
        std::swap(block0, block1);
   }
   else//no clear seperating line....
   {
        Point p0Center = block0->getCenter();
        Point p1Center = block1->getCenter();

        if(p1Center.y > block0->getBBox().yMax &&
           p0Center.y < block1->getBBox().yMin)
	{
           std::swap(block0, block1);
	}
   }

   vector<const CapoBlock*> blocks(2);
   blocks[0]=block0; 
   blocks[1]=block1; 
   PartitioningProblemForCapo problem(blocks, _hgraphOfNetlist, *this, 
    _cellToBlockMap,_params.verb);

   PartitionerParams repartParams;
   repartParams.verb = ("0_0_0");
   FMPartitioner repart(problem, repartParams);

   const SubHGraph&	hgraph = problem.getSubHGraph();
   const Partitioning&  soln   = problem.getBestSoln();

   // produce lists of cells in blocks and update _cellToBlockMap 
   vector<unsigned> cellsInBlock0; cellsInBlock0.reserve(soln.size()/2);
   vector<unsigned> cellsInBlock1; cellsInBlock1.reserve(soln.size()/2);
   Point p0Center = block0->getCenter(); 
   Point p1Center = block1->getCenter(); 
 
   unsigned nId; 
   for(nId = hgraph.getNumTerminals(); nId != hgraph.getNumNodes(); nId++)
   {
	unsigned origId = hgraph.newNode2OrigIdx(nId);
	if (soln[nId].lowestNumPart()==1) 
        {
           cellsInBlock1.push_back(origId); 
           _cellToBlockMap[origId]=block1;
	   _placement[origId] = p1Center;
        }
	else
        {
           cellsInBlock0.push_back(origId);
           _cellToBlockMap[origId]=block0;
	   _placement[origId] = p0Center;
        }
   } 

   // update the blocks and _cellToBlockMap 

   block0->resetCellIds(cellsInBlock0); 
   block1->resetCellIds(cellsInBlock1); 
}

void CapoPlacer::repartitionBlocks(CapoBlock* block0, CapoBlock* block1,
				   double relCongestionBlk0, 
				   double relCongestionBlk1)
{
     
   if (block0->getNumCells()<10 || block1->getNumCells()<10) return;

   if((block0->getBBox().yMin >= block1->getBBox().yMax) ||
      (block0->getBBox().xMax <= block1->getBBox().xMin))
   {}//order is correct..do nothing
   else if((block1->getBBox().yMin >= block0->getBBox().yMax) ||
           (block1->getBBox().xMax <= block0->getBBox().xMin))
   {   //blocks reversed
        std::swap(block0, block1);
   }
   else//no clear seperating line....
   {
        Point p0Center = block0->getCenter();
        Point p1Center = block1->getCenter();

        if(p1Center.y > block0->getBBox().yMax &&
           p0Center.y < block1->getBBox().yMin)
	{
           std::swap(block0, block1);
	}
   }

   vector<const CapoBlock*> blocks(2);
   blocks[0]=block0; 
   blocks[1]=block1; 

   //TO DO
   unsigned numCells = block0->getNumCells() + block1->getNumCells();
   vector<double> partitionCapacities(2);
   partitionCapacities[0] = relCongestionBlk1;
   partitionCapacities[1] = relCongestionBlk0;

   PartitioningProblemForCapo problem(blocks, _hgraphOfNetlist, *this, 
    _cellToBlockMap, partitionCapacities, _params.verb);

   const SubHGraph&	hgraph = problem.getSubHGraph();

   if(numCells < 200)
     {
       PartitionerParams repartParams;
       repartParams.verb = ("0_0_0");
       FMPartitioner repart(problem, repartParams);
     }
   else
     {
       MLPart::Parameters mlParams;
       mlParams.verb = ("0_0_0");
       if(hgraph.getNumNodes() < 500)
	 mlParams.clParams.sizeOfTop = 100;

       problem.reserveBuffers(2);	//sets are pairs of runs+a VC
       PartitioningBuffer refinedSolns(hgraph.getNumNodes(),2);
       vector<double>      partAreas(2); //in the BSF soln
       unsigned		bestSet;
       double 		origCut;
       origCut = DBL_MAX;
       
       vector<const char*> nodeNames;    

       unsigned setNum;
       for(setNum = 0; setNum < 2; setNum++)
	 {
	   MLPart*   partitioner1;
	   
	   partitioner1 = new MLPart(problem, mlParams);

	   refinedSolns [setNum] = problem.getBestSoln();
	   if(partitioner1->getBestSolnCost() < origCut)
	     {
	       origCut = partitioner1->getBestSolnCost();
	       bestSet = setNum;
	       partAreas[0] = partitioner1->getPartitionArea(0);
	       partAreas[1] = partitioner1->getPartitionArea(1);
	     }

	   PartitionIds clearId;
	   clearId.setToAll(2);
	   Partitioning& buff0 = problem.getSolnBuffers()[0];
	   Partitioning& buff1 = problem.getSolnBuffers()[1];
	   std::fill(buff0.begin()+hgraph.getNumTerminals(),buff0.end(), clearId);
	   std::fill(buff1.begin()+hgraph.getNumTerminals(),buff1.end(), clearId);
	   delete partitioner1;
	 }
       problem.reserveBuffers(2);
       for(setNum = 0; setNum < 2; setNum++)
	 problem.getSolnBuffers()[setNum] = refinedSolns[setNum];
       problem.setBestSolnNum(bestSet); 
     }

   const Partitioning&  soln   = problem.getBestSoln();

   // produce lists of cells in blocks and update _cellToBlockMap 
   vector<unsigned> cellsInBlock0; cellsInBlock0.reserve(soln.size()/2);
   vector<unsigned> cellsInBlock1; cellsInBlock1.reserve(soln.size()/2);
   Point p0Center = block0->getCenter(); 
   Point p1Center = block1->getCenter(); 
 
   unsigned nId; 
   for(nId = hgraph.getNumTerminals(); nId != hgraph.getNumNodes(); nId++)
   {
	unsigned origId = hgraph.newNode2OrigIdx(nId);
	if (soln[nId].lowestNumPart()==1) 
        {
           cellsInBlock1.push_back(origId); 
           _cellToBlockMap[origId]=block1;
	   _placement[origId] = p1Center;
        }
	else
        {
           cellsInBlock0.push_back(origId);
           _cellToBlockMap[origId]=block0;
	   _placement[origId] = p0Center;
        }
   } 

   // update the blocks and _cellToBlockMap 

   block0->resetCellIds(cellsInBlock0); 
   block1->resetCellIds(cellsInBlock1); 
}
