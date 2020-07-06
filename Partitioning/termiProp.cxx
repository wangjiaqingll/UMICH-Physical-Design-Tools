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

#include "Geoms/plGeom.h"
#include "partitioning.h"
#include "termiProp.h"

void 
TerminalPropagatorRough
::doOneTerminal(const BBox& termiBox, PartitionIds& parts)
{
   double minDist=DBL_MAX;
   std::vector<double>::iterator dIt=_mdists.begin();
   std::vector<BBox>::const_iterator bIt;
   for(bIt=_partitions.begin();bIt!=_partitions.end();bIt++,dIt++)
   {
      (*dIt)=termiBox.mdistTo(*bIt);
      if (*dIt<minDist) minDist=*dIt;
   }
   minDist*=_fuzzyFactor;
   for(dIt=_mdists.begin();dIt!=_mdists.end();dIt++)
      if (*dIt<=minDist) parts.addToPart(dIt-_mdists.begin());

   abkassert(parts.numberPartitions() >=1 , "all terminals must propagate");
}

TerminalPropagatorFine::TerminalPropagatorFine 
           (const vector<BBox>& partitions, double partFuzziness)
           : _partitions(partitions), _partitionsBloated(partitions),
             _fuzzyFactor(1.0+0.01* partFuzziness)
{
    std::vector<BBox>::iterator bIt;
   for(bIt=_partitionsBloated.begin(); bIt!=_partitionsBloated.end(); bIt++)
     bIt->ShrinkTo(_fuzzyFactor);
}

void TerminalPropagatorFine
::doOneTerminal(const BBox& termiBox, PartitionIds& parts)
{
   double minDist=DBL_MAX;
   std::vector<BBox>::const_iterator bIt;
   for(bIt=_partitions.begin();bIt!=_partitions.end();bIt++)
   {
      double dist=termiBox.mdistTo(*bIt);
      if (dist<minDist) minDist=dist;
   }

   std::vector<BBox>::iterator bbIt;
   for(bbIt=_partitionsBloated.begin();bbIt!=_partitionsBloated.end();bbIt++)
       if (termiBox.mdistTo(*bbIt)<=minDist+1e-5) 
           parts.addToPart(bbIt-_partitionsBloated.begin());

   abkassert(parts.numberPartitions() >=1 , "all terminals must propagate");
}
