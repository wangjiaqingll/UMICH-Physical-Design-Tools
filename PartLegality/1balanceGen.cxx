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

#include "1balanceGen.h"
#include "HGraph/hgFixed.h"
#include "Stats/stats.h"


unsigned AllToOneGen::counter=0;

vector<double> AllToOneGen::generateSoln(Partitioning& curPart)
{
   unsigned            numPart  = _problem.getNumPartitions();
   const HGraphFixed&  hgraph   = _problem.getHGraph();
   unsigned            numNodes = _problem.getHGraph().getNumNodes(); 
   const Partitioning& fixed    = _problem.getFixedConstr(); 
   vector<double>      balances(numPart);

   for(unsigned node=0; node!=numNodes; node++)
   {
     unsigned orig=counter % numPart;
     unsigned nodeGoesTo=orig;
     while ( ! fixed[node].isInPart(nodeGoesTo) )
     {
        nodeGoesTo= (nodeGoesTo + 1) % numPart;
        abkfatal3(nodeGoesTo!=orig," Node ",node,
                  " can not go into any partition\n");
     }
     curPart[node].setToPart(nodeGoesTo);
     balances[nodeGoesTo]+=hgraph.getNodeByIdx(node).getWeight();
   }
   counter++;
   return balances;
};


// SBGWeightRand: implemented by ILM
// produces a simple-minded randomized partitioning which is balanced
// with high probability; FM, Sanchis, SA and other consumers will need
// to make improving passes -- they can combine such passes with improving
// their objectives, which is why this methods does not do additional improving
// passes (this can be added as an option if needed)
//
// Possible improvements:
//  1. some "local" random permuting needed for nodes of approx. equal sizes
//   because the locations of consequtive nodes follownig a node with
//   much bigger weight can be highly correlated
//   (permuting all nodes is not good since it can decrease the probability
//   of getting a balances partition if there very big nodes)
//  2. Can we estimate the probability of getting a balanced solution ?
//     How do we improve the probability

 
vector<double> SBGWeightRand::generateSoln(Partitioning& curPart)
{
    unsigned numPartitions = _problem.getNumPartitions();
    const HGraphFixed& hgraph = _problem.getHGraph();

    abkfatal(hgraph.getNumNodes() >= numPartitions, 
             "not enough nodes / partition");
    
    vector<double> areaToMin    (numPartitions),
                   areaToFill   (numPartitions),
		   targetAreas  (numPartitions),
                   availableArea(numPartitions),
                   chancesOfGoingToPart(numPartitions);

    const vector<vector<double> >& maxCaps = _problem.getMaxCapacities();
    const vector<vector<double> >& minCaps = _problem.getMinCapacities();
    const vector<vector<double> >& caps =    _problem.getCapacities();

    unsigned k;
    for(k = 0; k < numPartitions; k++)
    {
        availableArea[k] = maxCaps[k][0];
        areaToMin[k]     = minCaps[k][0];
        areaToFill[k]    = caps[k][0];
        targetAreas[k]   = caps[k][0];
    }

    unsigned m, numMovables = hgraph.getNumNodes();
 
    const Permutation& sortAsc_1=hgraph.getNodesSortedByWeights();
    RandomRawUnsigned ru;
    
    for(m=numMovables-1 ; m != static_cast<unsigned>(-1); m--)
    {
        unsigned nodeIdx= sortAsc_1[m];
        const HGFNode& node=hgraph.getNodeByIdx(nodeIdx);
        double   nodeWt = node.getWeight();
        double   minNonZero=DBL_MAX;
        unsigned nonZeroChances=0; // to how many partitions a node can go
  
        if (nodeWt==0.0)
        {
	   unsigned p;
           for(p = 0; p != numPartitions; p++)
             if (curPart[nodeIdx].isInPart(p)) nonZeroChances++;
           if (nonZeroChances==0)
           {
              cerr << " Node " << nodeIdx << " is constrainted to "
                   << curPart[nodeIdx];
              abkfatal(0," Node can't go to any partition ");
           }
           k=ru % nonZeroChances;
           nonZeroChances=0;
           for(p=0; p != numPartitions; p++)
           {
              if (curPart[nodeIdx].isInPart(p))
                 if (nonZeroChances++ == k) { k=p;  goto setpart; }
           }
           abkfatal(0," Internal error: control should not reach here");
        }
 
        // compute chancesOfGoingToPart[] assuming some partitions 
        // are still underfilled
        for(k = 0; k < numPartitions; k++)
        {
            if ( curPart[nodeIdx].isInPart(k) && nodeWt <= areaToMin[k] + 1e-4)
            {
                nonZeroChances++;
                chancesOfGoingToPart[k] = areaToMin[k];
                if (chancesOfGoingToPart[k] < minNonZero)
                   minNonZero=chancesOfGoingToPart[k];
            }
            else  chancesOfGoingToPart[k] = 0.0;
        }

        // If chances of going to some partition are nonZero ...

        if (nonZeroChances) goto selection;

        // otherwise (e.g. all partitions are filled at least to minimum size),
        // recompute chancesOfGoingToPart[]
        // assuming some partitions are below their targets
 
        for(k = 0; k < numPartitions; k++)
        {
            if ( curPart[nodeIdx].isInPart(k) && nodeWt <= areaToFill[k] + 1e-4)            {  
                nonZeroChances++;
                chancesOfGoingToPart[k] = areaToFill[k];
                if (chancesOfGoingToPart[k] < minNonZero)
                   minNonZero=chancesOfGoingToPart[k];
            }
            else  chancesOfGoingToPart[k] = 0;
        }

        // If chances of going to some partition are nonZero ...

        if (nonZeroChances) goto selection;

        // otherwise (e.g. all partitions are filled at least to target size),
        // recompute chancesOfGoingToPart[]
        // assuming some partitions are below their max
 
        for(k = 0; k < numPartitions; k++)
        {
          if ( curPart[nodeIdx].isInPart(k) && nodeWt <= availableArea[k] +1e-4)          {  
              nonZeroChances++;
              chancesOfGoingToPart[k] = availableArea[k];
              if (chancesOfGoingToPart[k] < minNonZero)
                 minNonZero=chancesOfGoingToPart[k];
          }
          else  chancesOfGoingToPart[k] = 0;
        }
 
        // If chances of going to some partition are nonZero ...

        if (nonZeroChances) goto selection;

        // otherwise (e.g. all partitions are filled at least to max size),
        // recompute chancesOfGoingToPart[] to minimizing overfill
 
        for(k = 0; k < numPartitions; k++)
        {
          if ( curPart[nodeIdx].isInPart(k) )
          {
             nonZeroChances++;
             chancesOfGoingToPart[k] = 1.0 / abs( nodeWt - availableArea[k] );
             if (chancesOfGoingToPart[k] < minNonZero)
                 minNonZero=chancesOfGoingToPart[k];
          }
          else chancesOfGoingToPart[k] = 0;
        }

        // if chance of going to all partitions are 0 i
        // (e.g. the node was not allowed anywhere), issue fatal error messg.

        abkfatal3(nonZeroChances,
               " Node ", nodeIdx, " can't go into any partition ");
 
 selection : minNonZero *= 1e-3;

        if (nonZeroChances==1) 
        {
            for(k = 0; k < numPartitions; k++)
            if (chancesOfGoingToPart[k]>0.0) goto setpart;
            abkfatal(0," Internal error: control should not reach here");
        }
        else
        {
            for(k = 0; k < numPartitions; k++)
            chancesOfGoingToPart[k] /= minNonZero;
            k = BiasedRandomSelection(chancesOfGoingToPart, _ru);
        }
 
setpart: curPart[nodeIdx].setToPart(k);
        areaToMin    [k] -= nodeWt;
        areaToFill   [k] -= nodeWt;
        availableArea[k] -= nodeWt;
    }

    vector<double> partSizes(numPartitions);
    for(k = 0; k < numPartitions; k++)
        partSizes[k]= targetAreas[k] - areaToFill[k];
 
    return partSizes;
}

vector<double> SBGRandDutt::generateSoln(Partitioning& curPart)
{
    unsigned numPartitions    = _problem.getNumPartitions();
    const HGraphFixed& hgraph = _problem.getHGraph();

    abkfatal(numPartitions==2,
     " This ini soln generator is only good for 2 parts");

    abkfatal(hgraph.getNumNodes() >= numPartitions, 
             "not enough nodes / partition");

//  cout << " Ini solution :" << curPart << endl;

    const vector<vector<double> >& maxCaps = _problem.getMaxCapacities();
//  const vector<vector<double> >& targets = _problem.getCapacities();
    vector<double> availableArea(numPartitions);

    unsigned k;
    for(k = 0; k < numPartitions; k++) availableArea[k]=maxCaps[k][0];
  //for(unsigned k = 0; k < numPartitions; k++) availableArea[k]=targets[k][0];

    unsigned m, numMovables = hgraph.getNumNodes();
 
    const Permutation& sortAsc_1=hgraph.getNodesSortedByWeights();
    double minCellSize=hgraph.getNodeByIdx(0).getWeight();

    RandomRawUnsigned ru;
    PartitionIds cantMove;
    
    for(m=numMovables-1 ; m != static_cast<unsigned>(-1); m--)
    {
        unsigned nodeIdx= sortAsc_1[m];
        //unsigned nodeIdx=m;

        abkfatal3 (curPart[nodeIdx] != cantMove, "Node ", nodeIdx,
                   " cannot be assigned to a partition\n");

        double   nodeWt = hgraph.getNodeByIdx(nodeIdx).getWeight();

        if (curPart[nodeIdx].isInPart(0) && 
            curPart[nodeIdx].isInPart(1))
        { 
           unsigned side= (ru % 2 ? 1 : 0);
           if (availableArea[side]>minCellSize+nodeWt)
           {
               curPart[nodeIdx].setToPart(side);
               availableArea[side]-=nodeWt;
           }
	   else
           {
              curPart[nodeIdx].setToPart(1-side);
             availableArea[1-side]-=nodeWt;
           }
        }
        else
        {
           unsigned side= (curPart[nodeIdx].isInPart(0) ? 0 : 1);
           availableArea[side]-=nodeWt;
        }
    }

    vector<double> partSizes(numPartitions);
    for(k = 0; k < numPartitions; k++) 
       partSizes[k]=maxCaps[k][0]-availableArea[k];
       //partSizes[k]=targets[k][0]-availableArea[k];

    return partSizes;
} 

vector<double> SBGRandVeryIllegal::generateSoln(Partitioning& curPart)
{
    unsigned numPartitions    = _problem.getNumPartitions();
    const HGraphFixed& hgraph = _problem.getHGraph();

    abkfatal(numPartitions==2,
     " This ini soln generator is only good for 2 parts");

    abkfatal(hgraph.getNumNodes() >= numPartitions, 
             "not enough nodes / partition");

//  cout << " Ini solution :" << curPart << endl;

//  const vector<vector<double> >& maxCaps = _problem.getMaxCapacities();
//  const vector<vector<double> >& targets = _problem.getCapacities();
    vector<double> availableArea(numPartitions);

    vector<double> partSizes(numPartitions);

    unsigned m, numMovables = hgraph.getNumNodes();
 
    const Permutation& sortAsc_1=hgraph.getNodesSortedByDegrees();
//  double minCellSize=hgraph.getNodeByIdx(0).getWeight();

    RandomRawUnsigned ru;
    PartitionIds cantMove;

    unsigned side= (ru % 2 ? 1 : 0);
    unsigned startRand=max(numMovables/3,unsigned(5));
    for(m=numMovables-1 ; m != startRand; m--)
    {
        unsigned nodeIdx= sortAsc_1[m];
        abkfatal3 (curPart[nodeIdx] != cantMove, "Node ", nodeIdx,
                   " cannot be assigned to a partition\n");
        double   nodeWt = hgraph.getNodeByIdx(nodeIdx).getWeight();
        if (curPart[nodeIdx].isInPart(0) && 
	    curPart[nodeIdx].isInPart(1))
        { 
               curPart[nodeIdx].setToPart(side);
               partSizes[side]+=nodeWt;
        }
        else
        {
           unsigned side1= (curPart[nodeIdx].isInPart(0) ? 0 : 1);
           partSizes[side1]+=nodeWt;
        }
    }
    
    for( ; m != static_cast<unsigned>(-1); m--)
    {
        unsigned nodeIdx= sortAsc_1[m];
        //unsigned nodeIdx=m;

        abkfatal3 (curPart[nodeIdx] != cantMove, "Node ", nodeIdx,
                   " cannot be assigned to a partition\n");

        double   nodeWt = hgraph.getNodeByIdx(nodeIdx).getWeight();

        if (curPart[nodeIdx].isInPart(0) && 
	    curPart[nodeIdx].isInPart(1))
        { 
           side= (ru % 2 ? 1 : 0);
           curPart[nodeIdx].setToPart(side);
           partSizes[side]+=nodeWt;
        }
        else
        {
           unsigned side1= (curPart[nodeIdx].isInPart(0) ? 0 : 1);
           partSizes[side1]+=nodeWt;
        }
    }
    return partSizes;
} 
