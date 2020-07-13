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






// Created by Igor Markov, July 1997, VLSICAD ABKGroup UCLA/CS

#include "ABKCommon/abkcommon.h"
#include "geomTreeBase.h"
#include "b1Steiner.h"
#include "mst.h"
#include "primMST.h"

int main(int argc, char* argv[])
{
    StringParam inputFileName("in", argc, argv);
    vector<Point> points; 
    ifstream input(inputFileName);
    int lineNo = 0;
    double x, y;
    while(!input.eof())
    {
        input >> eathash(lineNo);
        if(input.eof()) break;
        input  >> x >> y >> skiptoeol;
        points.push_back(Point(x,y));
    }

    Timer b1sTime;
    BatchedI1Steiner st(points);
    b1sTime.stop();

    Timer mstTime;
    MST8 mst(points);
    mstTime.stop();

    Timer primMSTTime;
    PrimMST prim(points);
    primMSTTime.stop();

    cout << "MST Cost from B1St: " << st.getMSTCost() << endl;
    cout << "RSMT Cost : " << st.getCost() << "  with "
         << st.getNumSteinerPoints() << " steiner points\n";
    cout << "MST8 Cost : " << mst.getCost() << endl;
    cout << "Prim MST Cost : " << prim.getCost() <<endl;

    vector<GeomTreeEdge> MSTEdges = mst.getEdges(),
                         SteinerEdges = st.getEdges();
    cout << "MST Edges:\n";
    unsigned i;
    for(i = 0; i < MSTEdges.size(); i++)
        cout << "  " << MSTEdges[i].first
             << "  " << MSTEdges[i].second << endl;
    cout << "Steiner Edges:\n";
    for(i = 0; i < SteinerEdges.size(); i++)
        cout << "  " << SteinerEdges[i].first
             << "  " << SteinerEdges[i].second << endl;

    cout << "BI1Steiner took " << b1sTime << endl
         << "MST8 took " << mstTime << endl
         << "PrimMST took " << primMSTTime << endl;

   return 0;
}
