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



// Created 990210 by Stefanus Mantik, VLSI CAD ABKGROUP, UCLA/CS

#include "greedyOrientOpt.h"
#include "heapGainCtr.h"
#include "Ctainers/bitBoard.h"

GreedyOrientOpt::GreedyOrientOpt(const DB& db,
                                 const OrientOptParameters& params) :
    OrientOptimizerInterface(db,params)
{
    unsigned numNets = db.getNetlist().getNumNets(),
             numCells = db.getNetlist().getNumCells();
    unsigned i;
    vector<BBox>   netBBox(numNets), minPossibleBBox(numNets);
    vector<double> oldCost(numCells, 0);
    vector<double> newCost(numCells, 0);
    vector<double> gain(numCells, DBL_MIN);
    vector<vector<unsigned> > needUpdate(numCells);
    BitBoard       flipableCells(numCells);
    vector<Orient>& ors = *this;
    vector<pair<Point, Point> > pinLocations(db.getNetlist().getNumPins());

    itCellGlobal c;
    for(c = db.getNetlist().cellsBegin(); c != db.getNetlist().cellsEnd(); c++)
    {
        if(!(*c)->isCore()) continue;    // only consider core cells
        unsigned symm = static_cast<unsigned>((*c)->getMaster().getSymmetry());
        if(!(symm == 2 || symm == 3 || symm == 6 || symm == 7))
            continue;        // ignore the cell if it is not Y symmetry
        flipableCells.setBit((*c)->getIndex());
    }
    if(flipableCells.getNumBitsSet() == 0) 
    {
        abkwarn(0, "no orientation change is permitted");
        return;
    }

    // Store the actual location of pins for flipped and unflipped
    itPinLocal p;
    for(c = db.getNetlist().cellsBegin();
        c != db.getNetlist().cellsEnd(); c++)
    {
        unsigned cellIdx = (*c)->getIndex();
        for(p = (*c)->pinsBegin(); p != (*c)->pinsEnd(); p++)
            pinLocations[(*p)->getIndex()].first = db.locatePin(**p, ors);
        if(flipableCells.isBitSet(cellIdx))
        {
            (operator[](cellIdx)).flipHoriz();
            for(p = (*c)->pinsBegin(); p != (*c)->pinsEnd(); p++)
                pinLocations[(*p)->getIndex()].second = db.locatePin(**p, ors);
            (operator[](cellIdx)).flipHoriz();
        }
        else
            for(p = (*c)->pinsBegin(); p != (*c)->pinsEnd(); p++)
                pinLocations[(*p)->getIndex()].second = 
                    pinLocations[(*p)->getIndex()].first;
    }

    // Store net BBox and minimum possible BBox
    itNetGlobal n;
    for(n = db.getNetlist().netsBegin(); n != db.getNetlist().netsEnd(); n++)
    {
        unsigned netIdx = (*n)->getIndex();
        BBox nBox;
        double minX = DBL_MAX, maxX = DBL_MIN;
        for(p = (*n)->pinsBegin(); p != (*n)->pinsEnd(); p++)
        {
            const Point& fPt = pinLocations[(*p)->getIndex()].first,
                       & sPt = pinLocations[(*p)->getIndex()].second;
            nBox += fPt; //pinLocations[(*p)->getIndex()].first;
            if(fPt.x < sPt.x)
            {
                minX  = min(minX, sPt.x);
                maxX  = max(maxX, fPt.x);
            }
            else
            {
                minX  = min(minX, fPt.x);
                maxX  = max(maxX, sPt.x);
            }
        }
        netBBox[netIdx] = nBox;
        nBox.xMin = minX;
        nBox.xMax = maxX;
        minPossibleBBox[netIdx] = nBox;
    }

    // Build needUpdate list
    for(n = db.getNetlist().netsBegin(); n != db.getNetlist().netsEnd(); n++)
    {
        unsigned netIdx = (*n)->getIndex();
        BBox nBox = minPossibleBBox[netIdx];
        double halfPerim = netBBox[netIdx].getHalfPerim();
        _initialWL += halfPerim;
                   // looking for dependency between nets end cells
                   // this is for checking whether when a cell flipped,
                   // an update to the net is necessary or not
        itCellLocal lc;
        for(lc = (*n)->cellsBegin(); lc != (*n)->cellsEnd(); lc++)
        {
            unsigned cellIdx = (*lc)->getIndex();
            abkfatal(cellIdx < oldCost.size(), "Index too much");
            if(!flipableCells.isBitSet(cellIdx)) continue;
            oldCost[cellIdx] += halfPerim;
            Point rightEdge, leftEdge;
            rightEdge = leftEdge = db.spatial.locations[cellIdx];
            rightEdge.x += (*lc)->getWidth();   // only X axis 
                                               // because we only flip on Y
            if(nBox.contains(leftEdge) && nBox.contains(rightEdge))
                continue;
            abkfatal(cellIdx < needUpdate.size(), "Index too much");
            needUpdate[cellIdx].push_back(netIdx);
        }
    }

    for(c = db.getNetlist().cellsBegin();
        c != db.getNetlist().cellsEnd(); c++)
    {
        unsigned cellIdx = (*c)->getIndex();
        if (!flipableCells.isBitSet(cellIdx)) continue;
        newCost[cellIdx] = oldCost[cellIdx];
        for(i = 0; i < needUpdate[cellIdx].size(); i++)
        {
            unsigned netIdx = needUpdate[cellIdx][i];
            newCost[cellIdx] -= netBBox[netIdx].getHalfPerim();
            const dbNet& net = db.getNetlist().getNetByIdx(netIdx);
            BBox nBox;
            for(p = net.pinsBegin(); p != net.pinsEnd(); p++)
                if((*p)->getCell().getIndex() == cellIdx)
                    nBox += pinLocations[(*p)->getIndex()].second;
                else
                    nBox += pinLocations[(*p)->getIndex()].first;
            newCost[cellIdx] += nBox.getHalfPerim();
        }
        if(_params.minimizeCost)
            gain[cellIdx] = oldCost[cellIdx] - newCost[cellIdx];
        else
            gain[cellIdx] = newCost[cellIdx] - oldCost[cellIdx];
    }
    BitBoard visitedCells(numCells);
    HeapGainContainer  heapGain(gain, flipableCells);

                   // Optimization loop starts here
    while(gain[heapGain.getTop()] > _params.cutOffGain)
    {
        visitedCells.clear();
        unsigned cellIdx = heapGain.getTop(); 
        operator[](cellIdx).flipHoriz();
        _nFlips++;

                          // problem with swap so use "old-style" one
        double swapBucket = oldCost[cellIdx];
        oldCost[cellIdx] = newCost[cellIdx];
        newCost[cellIdx] = swapBucket;
        gain[cellIdx] = -gain[cellIdx];
        heapGain.heapify(cellIdx);

        if(_params.verb.forActions > 1)
            cout << "delta : " << oldCost[cellIdx] - newCost[cellIdx] << endl;
        for(i = 0; i < needUpdate[cellIdx].size(); i++)
        {
            unsigned netIdx = needUpdate[cellIdx][i];
            const dbNet& net = db.getNetlist().getNetByIdx(netIdx);
            BBox nBox;
            for(p = net.pinsBegin(); p != net.pinsEnd(); p++)
                nBox += db.locatePin(**p, ors);
            double delta = nBox.getHalfPerim() -
                           netBBox[netIdx].getHalfPerim();
            netBBox[netIdx] = nBox;
            itCellLocal lc;
            for(lc = net.cellsBegin(); lc != net.cellsEnd(); lc++)
            {
                unsigned lCellIdx = (*lc)->getIndex();
                if(!flipableCells.isBitSet(lCellIdx)) continue;
                if(lCellIdx == cellIdx) continue;
                oldCost[lCellIdx] += delta;
                if(visitedCells.isBitSet(lCellIdx)) continue;
                visitedCells.setBit(lCellIdx);
            }
        }
        const vector<unsigned>& 
            updateCellList = visitedCells.getIndicesOfSetBits();

        for(i = 0; i < updateCellList.size(); i++)
        {
            unsigned lCellIdx = updateCellList[i];
            operator[](lCellIdx).flipHoriz();
            newCost[lCellIdx] = oldCost[lCellIdx];
            unsigned j;
            for(j = 0; j < needUpdate[lCellIdx].size(); j++)
            {
                unsigned netIdx = needUpdate[lCellIdx][j];
                const dbNet& net = db.getNetlist().getNetByIdx(netIdx);
                BBox nBox;
                for(p = net.pinsBegin(); p != net.pinsEnd(); p++)
                    nBox += db.locatePin(**p, ors);
                newCost[lCellIdx] += nBox.getHalfPerim() -
                                     netBBox[netIdx].getHalfPerim();
            }
            operator[](lCellIdx).flipHoriz();
            if(_params.minimizeCost)
                gain[lCellIdx] = oldCost[lCellIdx] - newCost[lCellIdx];
            else
                gain[lCellIdx] = newCost[lCellIdx] - oldCost[lCellIdx];
            heapGain.heapify(lCellIdx);
        }
    }
    for(i = 0; i < numNets; i++)
        _finalWL += netBBox[i].getHalfPerim();
}

