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








#include "FPcommon.h"
#include "SeqPair.h"
using namespace parquetfp;

SeqPair::SeqPair(unsigned size) //ctor randomly generates the seqPair
{
  _XX.resize(size);
  _YY.resize(size);
  for(unsigned i=0; i<size; ++i)
    {
      _XX[i] = i;
      _YY[i] = i;
    }
  random_shuffle(_XX.begin(),_XX.end());
  random_shuffle(_YY.begin(),_YY.end());
}

SeqPair::SeqPair(vector<unsigned>& X, vector<unsigned>& Y):_XX(X), _YY(Y)
{
  if(X.size() != Y.size())
    cout<<"ERROR: Input Sequence Pairs of different sizes"<<endl;
}

vector<unsigned>& SeqPair::getX(void)
{
  return _XX;
}

vector<unsigned>& SeqPair::getY(void)
{
  return _YY;
}

void SeqPair::putX(vector<unsigned>& X)
{ _XX = X; }

void SeqPair::putY(vector<unsigned>& Y)
{ _YY = Y; }

unsigned SeqPair::getSize(void)
{  return _XX.size(); }

void SeqPair::printX(void)
{
  cout<<"X Seq Pair"<<endl;
  for(unsigned i=0; i<_XX.size(); ++i)
    {
      cout<<_XX[i]<<" ";
    }
  cout<<endl;
}

void SeqPair::printY(void)
{
  cout<<"Y Seq Pair"<<endl;
  for(unsigned i=0; i<_XX.size(); ++i)
    {
      cout<<_YY[i]<<" ";
    }
  cout<<endl;
}
