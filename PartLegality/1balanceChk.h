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






//created by Igor Markov on 05/26/98

#ifndef __1BALANCECHK_H__
#define __1BALANCECHK_H__

#include "partLegXFace.h"

// Note: derived classes should only be used when the precise class
//       is clear (not to compile any methods as virtual)


class SingleBalanceLegality : public PartLegalityXFace
{
protected:

  unsigned       _nParts;
  vector<double> _maxPartBalances;
  vector<double> _minPartBalances;
  vector<double> _targetPartBalances;
  vector<double> _actualPartBalances;

public:

  SingleBalanceLegality
       (const PartitioningProblem& problem,       Partitioning& part);
  SingleBalanceLegality (const PartitioningProblem& problem) ;

   bool isPartLegal() const;  // check current state 
                     // may be slow for single-cell moves in k-way part'ing

   double getViolation()      const { return getMaxViolation(); }
   double getDiffFromIdeal()  const { return getMaxDiffFromIdeal(); }
   unsigned getUnderfilledPartition() const
   {
      if(_targetPartBalances[0]-_actualPartBalances[0] > 
	 _targetPartBalances[1]-_actualPartBalances[1])  return 0;
     
      return 1;
   }

   double getMaxViolation() const;
   double getMaxDiffFromIdeal() const;

// Reinitializes the checker if the partition changed "non-incrementally"
// Must be redefined in derived classes by calling 
// same method in the immediate parent (with scope resolution operator ::)
  void reinitialize();

  bool isLegalMove(unsigned moduleNumber, unsigned from, unsigned to) const
  {
     double weight=_hg.getNodeByIdx(moduleNumber).getWeight();
     return (_actualPartBalances[from]-weight >= _minPartBalances[from] &&
             _actualPartBalances[to]  +weight <= _maxPartBalances[to] );
  }

  bool isLegalMove(unsigned moduleNumber, PartitionIds old, PartitionIds newIds)
  const
  { return 
      isLegalMove(moduleNumber, old.lowestNumPart(), newIds.lowestNumPart());}

  bool isLegalMoveWithReplication(unsigned moduleNumber,
             PartitionIds oldVal, PartitionIds newVal) const
  { 
    double weight=_hg.getNodeByIdx(moduleNumber).getWeight();
    for(unsigned k=0; k!=_nParts; k++)
    {
       if (oldVal.isInPart(k))
       { 
         if (!newVal.isInPart(k)) 
         if (_actualPartBalances[k]-weight < _minPartBalances[k]) return false;
       }
       else if (newVal.isInPart(k)) 
         if (_actualPartBalances[k]+weight > _maxPartBalances[k]) return false;
    }
    return true;
  }

// These represent incremental update by one move 
// return true if the moved cell does not make things worse
// when inlining, the compiler should detect when the return value is not used

  bool moveModuleTo(unsigned moduleNumber, unsigned from, unsigned to)
  {
    double weight=_hg.getNodeByIdx(moduleNumber).getWeight();
    _actualPartBalances[from]-=weight;
    _actualPartBalances[to]  +=weight;
    return (_actualPartBalances[from] >= _minPartBalances[from] &&
            _actualPartBalances[to]   <= _maxPartBalances[to]   );   
  }
   
  bool
  moveModuleTo(unsigned moduleNumber, PartitionIds old, PartitionIds newIds)
  { return 
    moveModuleTo(moduleNumber, old.lowestNumPart(), newIds.lowestNumPart());}

  bool moveWithReplication(unsigned moduleNumber,
                         PartitionIds oldVal, PartitionIds newVal)
  { 
    double weight=_hg.getNodeByIdx(moduleNumber).getWeight();
    for(unsigned k=0; k!=_nParts; k++)
    {
       if (oldVal.isInPart(k))
          { if (!newVal.isInPart(k)) _actualPartBalances[k]-=weight; }
       else if (newVal.isInPart(k)) _actualPartBalances[k]+=weight;
    }
    return isPartLegal();
  }

// --------- Legality-specific interface

  const vector<double>& getActualBalances() const { return _actualPartBalances;}

  double getMaxLegalMoveArea(unsigned from, unsigned to)
  { 
     double deltaFrom=_actualPartBalances[from]-_minPartBalances[from];
     double deltaTo  =_maxPartBalances[to]-_actualPartBalances[to];
     return max(0.0,min(deltaFrom,deltaTo));
  }

  unsigned balance(const Partitioning& fixedConstr); // returns moveCount
  unsigned enforce(const Partitioning& fixedConstr);

  friend ostream& operator<<(ostream&,const SingleBalanceLegality&);
};

ostream& operator<<(ostream&,const SingleBalanceLegality&);

#endif
