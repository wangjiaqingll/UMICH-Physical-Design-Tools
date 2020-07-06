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









#ifndef __DB_COMMON_PIN_
#define __DB_COMMON_PIN_

#include "dbDefines.h"
#include "dbTypes.h"
#include "dbPin.h"
#include "dbNet.h"
#include <ABKCommon/sgi_stl_compat.h>
//CHANGES
//970825 aec  added const to all functions


// member function for dbCell 
inline const dbPin* dbCell::commonPin(const dbNet* n) const
{
    itPinLocal pItr;
    if (getNumPins()>n->getNumPins())
    {
       for(pItr = n->pinsBegin(); pItr != n->pinsEnd(); pItr++)
          if (& ((*pItr)->getCell()) == this) return(*pItr);
    }
    else
    {
       for(pItr = pinsBegin(); pItr != pinsEnd(); pItr++)
          if ((*pItr)->isConnected() && &((*pItr)->getNet()) == n) return(*pItr);
    }
        
    return (NULL);
}


inline bool dbCell::commonPinIsCellInput(const dbNet* n) const
{
    const dbPin* pin=commonPin(n);
    return ( pin->isDirInput() || pin->isDirInOut() );
}

inline bool dbCell::commonPinIsCellOutput(const dbNet* n) const
{
    const dbPin* pin=commonPin(n);
    return 
     ( pin->isDirOutput() || pin->isDirInOut() || pin->isDirOutputTristate() );
}


// member function for dbNet
inline const dbPin* dbNet::commonPin(const dbCell* c) const
{
    itPinLocal pItr;
 
    if (getNumPins()<=c->getNumPins())
    {
       for(pItr = pinsBegin(); pItr != pinsEnd(); pItr++)
          if (& ((*pItr)->getCell()) == c) return(*pItr);
    }
    else
    {
       for(pItr = c->pinsBegin(); pItr != c->pinsEnd(); pItr++)
          if ((*pItr)->isConnected() && &((*pItr)->getNet()) == this) return(*pItr);
    }

    return (NULL);
}

inline bool dbNet::commonPinIsCellInput(const dbCell* c) const
{
    const dbPin* pin=commonPin(c);
    return ( pin->isDirInput() || pin->isDirInOut() );
}

inline bool dbNet::commonPinIsCellOutput(const dbCell* c) const
{
    const dbPin* pin=commonPin(c);
    return 
     ( pin->isDirOutput() || pin->isDirInOut() || pin->isDirOutputTristate());
}

inline double dbNet::getInputCap(const dbCell* c) const
{
    const dbPin*  p = this->commonPin(c);
    return p->getInputCap();
}

inline double dbNet::getDriverResist(const dbCell* c) const
{
    const dbPin*  p = this->commonPin(c);
    return  std::max(p->getRiseResist(), p->getFallResist() );
}

inline bool dbCell::isInputPad() const
{
    if(isPI() || isPIO())   {return true; }
    if(isPO() || isPOther()){return false;}
    
    //else, it's PUnknown. check to see if it's an input to some net it's on.
    itNetLocal  nItr;
  
    for(nItr = netsBegin(); nItr != netsEnd(); nItr++)
    {
            if( (*nItr)->commonPinIsCellInput(const_cast<dbCell*>(this) ) ) 
			return true;
			//isn't this backwards???INPUT pins are inputs
			//to the cell they're on...aren't they?? -aec
    }
    return false;
}

inline bool dbCell::isOutputPad() const
{
    if(isPI() || isPOther())   {return false; }
    if(isPO() || isPIO())      {return true;} 
    
    //else, it's PUnknown. check to see if it's an input to some net it's on.
    itNetLocal  nItr;
    
    for(nItr = netsBegin(); nItr != netsEnd(); nItr++)
    { 
            if( (*nItr)->commonPinIsCellOutput(const_cast<dbCell*>(this)) ) 
			return true; 
    }
    return false;
}

#endif

