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







#include "hgFixed.h"
#include "Stats/trivStats.h"

int main(int argc, const char *argv[])
{

    StringParam  baseFileName("f",argc,argv);

    abkfatal(baseFileName.found()," -f baseFileName not found");

    HGraphFixed hgraph(baseFileName);

    hgraph.printEdgeSizeStats(cout);
    cout<<endl;
    hgraph.printEdgeWtStats(cout);
    cout<<endl;
    hgraph.printNodeDegreeStats(cout);
    cout<<endl;
    hgraph.printNodeWtStats(cout);
    cout<<endl;

  cout<<" Total Edges: "<<hgraph.getNumEdges()<<endl;
  cout<<" Total Nodes: "<<hgraph.getNumNodes()<<endl;
  cout<<"   Terminals: "<<hgraph.getNumTerminals()<<endl;
  cout<<"   Non-Terms: "<<hgraph.getNumNodes() - hgraph.getNumTerminals()<<endl;

    return 0;
}


