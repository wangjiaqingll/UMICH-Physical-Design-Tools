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






// Created by Igor Markov, January, 1998

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <iostream>
#include "DB/DB.h"
#include "topDep.h"

TopDepths::TopDepths(const DB& db, unsigned threshold, LevelZero lz)
{
   if (threshold) 
   cout << " Thresholding all nets with " 
        << threshold << " or more pins on them " << endl;
   
   const dbNetlist& netlist=db.getNetlist();
   vector<unsigned>& result=(*this);

   unsigned numCells=netlist.getNumCells();
   vector<unsigned> cellLevels(numCells,UINT_MAX);
   vector<unsigned> cellsOnCurLevel;
   unsigned                curLevel=0;

   // populate cellsOnCurLevel with pads
   for (itCellGlobal iC=netlist.cellsBegin();iC!=netlist.cellsEnd();iC++)
   {
         const dbCell& cell=**iC;

         if ( ! cell.isIO() ) continue;
         if ( (lz==AllPads) || 
              ((lz==PIs) && !cell.isPO()) ||
              ((lz==POs) && !cell.isPI()) 
            ) 
         { 
            cellsOnCurLevel.push_back(cell.getIndex());
            cellLevels[cell.getIndex()]=curLevel;
         }
   }
  
   while (cellsOnCurLevel.size())
   {
      result.push_back(cellsOnCurLevel.size());
      vector<unsigned> cellsOnNextLevel;
      curLevel++;
      for(unsigned k=0;k!=cellsOnCurLevel.size();k++)
      {
         const dbCell& curCell=netlist.getCellByIdx(cellsOnCurLevel[k]);
         for (itNetLocal  iN=curCell.netsBegin();iN!=curCell.netsEnd();iN++)
         {
            if (threshold && threshold<=(*iN)->getNumCells()) continue;
            for (itCellLocal iCl=(*iN)->cellsBegin();
                             iCl!=(*iN)->cellsEnd();iCl++)
            {
	       unsigned cellIdx=(*iCl)->getIndex();
               if (cellLevels[cellIdx]>curLevel) 
               { 
                 cellsOnNextLevel.push_back(cellIdx);
                 cellLevels[cellIdx]=curLevel; 
               }
           }
         }
      }
      cellsOnCurLevel = cellsOnNextLevel;
   }
   cout << " Topological depth stats for " << db.getAuxName() << endl
        << "    Level zero --- all I/O cells "<< endl << result ;

// unsigned w=0;
// for (unsigned n=1;n!=result.size();n++)
// {
//      w += n * result[n]; 
// }
// cout <<" Topological weight : " << w << endl;

// ofstream deepCells("deepCell.names");
// cout << " Writing names of cells at level " << curLevel-1 
//      << " to deepCell.names" << endl;
// for (unsigned k=0;k!=netlist.getNumCells();k++)
// if  (cellLevels[k] == curLevel-1)
// {
//     deepCells << netlist.getCellByIdx(k).getName() << endl;
// }
}
