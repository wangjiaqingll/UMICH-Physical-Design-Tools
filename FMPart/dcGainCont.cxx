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



#include "dcGainCont.h"

ostream& operator<<
(ostream& os, const DestCentricGainContainerT& gc)
{
    if(gc.getMaxGain() == -1)
	cout<<"setMaxGain has not been called. "
	    <<"Can't print DestCentricGainContainerT";

    os<<"  MaxGain:  "<<gc.getMaxGain()<<endl;

    for(unsigned k = 0; k < gc.getNumPrioritizers(); k++)
    {
        os<<"Moves To Partition "<<k<<endl;
        if(gc._params.mapBasedGC)
	{
        for(BucketArrayM::const_reverse_iterator it = gc.getPrioritizerM(k).rbegin(); it != gc.getPrioritizerM(k).rend(); ++it)
        {
            os << setw(4) << (it->first) - gc.getMaxGain() <<") ";
            SVGainElement* cur =
                static_cast<SVGainElement* const>(it->second._next);
            if(cur == NULL)
                os<<" EMPTY "<<endl;
            else
            {
                for( ; cur != NULL;
                        cur = static_cast<SVGainElement* const>(cur->_next))
                {

                    os<<"[#:"<<gc.getRepository().getElementId(*cur);
                    os<<" g:"<<cur->getGain()<<" + ";
                    os<<cur->getRealGain() - cur->getGain()<<"]";

                }
                os<<endl;
            }
        }
	}
	else
	{
        for(int g = gc.getPrioritizer(k).size()-1; g >= 0; g--)
        {
            os << setw(4) << g - gc.getMaxGain() <<") ";
            SVGainElement* cur =
                static_cast<SVGainElement* const>(gc.getPrioritizer(k)[g]._next);
            if(cur == NULL)
                os<<" EMPTY "<<endl;
            else
            {
                for( ; cur != NULL;
                        cur = static_cast<SVGainElement* const>(cur->_next))
                {

                    os<<"[#:"<<gc.getRepository().getElementId(*cur);
                    os<<" g:"<<cur->getGain()<<" + ";
                    os<<cur->getRealGain() - cur->getGain()<<"]";

                }
                os<<endl;
            }
        }

	}
    }
    return os;
}

void DestCentricGainContainerT::reinitialize()
{
    if(_params.mapBasedGC)
    {
      for(unsigned k = 0; k < _prioritizer2.size(); k++)
        _prioritizer2[k].removeAll();
    }
    else
    {
      for(unsigned k = 0; k < _prioritizer1.size(); k++)
        _prioritizer1[k].removeAll();
    }
    _highestArray = 0;
}

void DestCentricGainContainerT::setupClip()
{
    if(_params.mapBasedGC)
    {
      for(unsigned k = 0; k < _prioritizer2.size(); k++)
	_prioritizer2[k].setupForClip();
    }
    else
    {
      for(unsigned k = 0; k < _prioritizer1.size(); k++)
	_prioritizer1[k].setupForClip();
    }
}

void DestCentricGainContainerT::setMaxGain(unsigned maxG)
{
    _maxGain = maxG;
    if(_params.mapBasedGC)
    {
      for(unsigned k = 0; k < _prioritizer2.size(); k++)
        _prioritizer2[k].setMaxGain(maxG);
    }
    else
    {
      for(unsigned k = 0; k < _prioritizer1.size(); k++)
        _prioritizer1[k].setMaxGain(maxG);
    }

}

bool DestCentricGainContainerT::isConsistent()
{
    bool consistent = true;
    if(_params.mapBasedGC)
    {
      for(unsigned k = 0; k < _prioritizer2.size(); k++)
      {
          if(! _prioritizer2[k].isConsistent())
          {
              consistent = false;
              cout<<"BucketArray "<<k<<" is not consistent"<<endl;
          }
      }
    }
    else
    {
      for(unsigned k = 0; k < _prioritizer1.size(); k++)
      {
          if(! _prioritizer1[k].isConsistent())
          {
              consistent = false;
              cout<<"BucketArray "<<k<<" is not consistent"<<endl;
          }
      }
    }

    
    return consistent;
}
