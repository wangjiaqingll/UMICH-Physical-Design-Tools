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





//! author="Stefanus Mantik, September 1998"

#ifndef _LAYOUT_BBOXES_H_
#define _LAYOUT_BBOXES_H_

#include <iostream>
#include <vector>
using std::vector;
#include "Geoms/bbox.h"

//: A vector of BBoxes, represents a set of BBoxes of layout
class LayoutBBoxes : public vector<BBox>
{
    void readBlocks(istream& in);
    void saveBlocks(ostream& out) const;

public:
    LayoutBBoxes() {}
    LayoutBBoxes(unsigned n) : vector<BBox>(n) {}
    LayoutBBoxes(unsigned n, BBox b) : vector<BBox>(n,b) {}
    LayoutBBoxes(const_iterator first, const_iterator last)
           : vector<BBox>(first,last) {}
    LayoutBBoxes(const vector<BBox>& bboxes)
	   : vector<BBox>(bboxes) {}
    explicit LayoutBBoxes(istream& in);
	// Construct LayoutBBoxes by the data from istream 
    explicit LayoutBBoxes(const char * blockFileName);
	// Construct LayoutBBoxes by the BlockFile which describe a set of BBoxes

    bool contains(const Point& x) const;  
    // the following two functions are slow!!
    // Check the contain relationship between Point and each BBox
	//	Return true if one holds, else false. 
    unsigned locate(const Point& x) const;   
    // find the index of the first bbox that has this point.
    // Returns LayoutBBoxes.size() if not found

    void save(ostream& out);
    void save(const char * blockFileName);
	// save the vector of BBox in blockFileName
    void xyDraw(ostream& xyStream);

    friend ostream& operator<<(ostream &out, const LayoutBBoxes &arg)
    {
        arg.saveBlocks(out);
        return out;
    }
    friend istream& operator>>(istream &in, LayoutBBoxes &arg)
    {
        arg.readBlocks(in);
        return in;
    }
};

#endif
