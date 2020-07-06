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









//the X component of BBox only - used in the B&B placer
//   created by Andrew Caldwell  981216

#ifndef __XINTERVAL_H__
#define __XINTERVAL_H__

class XInterval
{
  public:

   double min, max;

   XInterval()
	: min(DBL_MAX), max(-DBL_MAX)
	{}

   XInterval(double newMin, double newMax)
	: min(newMin), max(newMax)
	{}
   XInterval(const XInterval& ivl)
	: min(ivl.min), max(ivl.max)
	{}

   ~XInterval(){}

   XInterval& operator+=(double newPt)
   {  
	//min = (newPt < min) ? newPt : min; 
	//max = (newPt > max) ? newPt : max; 

	if(newPt < min) min = newPt;
	if(newPt > max) max = newPt;

	return *this;
   }

   double getLength() const 
   {
	abkassert(!(min>max), "can't take the length of an empty interval");
	return max-min;
   }

   bool isEmpty() const
   {    return min > max; }
};

#endif


