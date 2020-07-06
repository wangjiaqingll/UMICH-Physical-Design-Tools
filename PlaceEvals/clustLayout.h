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






#ifndef  _CLUSTLAYOUT_H_
#define  _CLUSTLAYOUT_H_

#include <iostream>
#include <vector>
using std::vector;

#include "ABKCommon/abkcommon.h"
#include "Placement/layoutBBoxes.h"

class ClusteredLayout
{
  protected:

    vector<LayoutBBoxes>   		_layoutTree;
    vector<vector<vector<unsigned> > >	_childMap;
    vector<vector<unsigned> >		_parentMap;

    unsigned 				_numLevels;

  private:
    void print(ostream &o);

  public:

    ClusteredLayout(const char * fileList);

    unsigned getNumLevels() const { return _numLevels; }

    LayoutBBoxes& getLayout(unsigned level) { return _layoutTree[level]; }
    LayoutBBoxes getChildren(unsigned level, unsigned index)
    {
        LayoutBBoxes result;
        for(unsigned i = 0; i < _childMap[level][index].size(); i++)
            result.push_back(
                _layoutTree[level + 1][_childMap[level][index][i]]);
        return result;
    }

    friend ostream& operator<<(ostream &o, ClusteredLayout & cl)
    {
        cl.print(o);
        return o;
    }

    void printBlocksOnLevel(unsigned level)
    {
	cout<<"From ClusteredLayout:  Level "<<level<<endl;
	cout<<" TotalNumBlocks: "<<_layoutTree[level].size()<<endl;

	for(unsigned i = 0; i < _layoutTree[level].size(); i++)
	    cout<<" Block: "<<i<<"  "<<_layoutTree[level][i]<<endl;
    }

};

#endif

