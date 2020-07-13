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










#include "hgWDims.h"
#include "Stats/trivStats.h"

int main(int argc, const char** argv)
{
    StringParam   auxFile("f",		argc, argv);

    BoolParam     help1("help",		argc,argv);
    BoolParam     help2("h",		argc,argv);

    StringParam   saveAsNodes   ("saveAsNodes", argc, argv);
    StringParam   saveAsNetD    ("saveAsNetD",  argc, argv);

    DoubleParam   setDims("setDims",     argc, argv);
    DoubleParam   maxAreaPercent("maxAreaPct", argc, argv);

    BoolParam     genMacros("genMacros",		argc,argv);


    cout<<"HGraph Utility:  version 1.0"<<"\n"<<TimeStamp()<<endl;
    cout<<"Copyright (c) 1998-1999 Regents of the University of California\n";
    cout<<"           All rights reserved.\n";
    cout<<"Authors: Andrew E. Caldwell, Andrew B. Kahng, Igor L. Markov\n";
    cout<<"Email:   {caldwell,abk,imarkov}@cs.ucla.edu\n";
    cout<<endl;
    cout<<"Permission is hereby granted, without written agreement and \n"
	<<"without license or royalty fee, to use, copy, modify, and \n"
	<<"distribute and sell this software and its documentation for \n"
	<<"any purpose, provided that the above copyright notice, this \n"
	<<"permission notice, and the following two paragraphs appear \n"
	<<"in all copies of this software as well as in all copies of \n"
	<<"supporting documentation.\n\n";
    cout<<"THIS SOFTWARE AND SUPPORTING DOCUMENTATION IS PROVIDED \"AS IS\".\n"
	<<"The Microelectronics Advanced Research Corporation (MARCO), the\n"
	<<"Gigascale Silicon Research Center (GSRC),  and \n"
	<<"(\"PROVIDERS\") MAKE NO WARRANTIES, whether express \n"
	<<"or implied, including warranties of merchantability or fitness\n"
	<<"for a particular purpose or noninfringement, with respect to \n"
	<<"this software and supporting documentation.\n"
	<<"Providers have NO obligation to provide ANY support, assistance,\n"
	<<"installation, training or other services, updates, enhancements\n"
	<<"or modifications related to this software and supporting\n"
	<<"documentation.\n\n";
    cout<<"Providers shall NOT be liable for ANY costs of procurement of \n"
	<<"substitutes, loss of profits, interruption of business, or any \n"
	<<"other direct, indirect, special, consequential or incidental \n"
	<<"damages arising from the use of this software and its \n"
	<<"documentation, whether or not Providers have been advised of \n"
	<<"the possibility of such damages."<<endl<<endl;

    if(help1.found() || help2.found())
    {
	cout<<"  -f <auxFileName>"<<endl;
	cout<<"  -saveAsNodes <nodes/nets/wts basefilename"<<endl;
	cout<<"  -saveAsNetD  <netD/areM basefilename"<<endl;
	cout<<"  -setDims     <height>"<<endl;
	cout<<"  -maxAreaPct  <max area of 1 cell, as % of total>"<<endl;
	cout<<"  -genMacros  (generate macros for large cells)"<<endl;
	return 0;
    }

    HGraphParameters hgParams(argc, argv);
    hgParams.makeAllSrcSnk = false;

    abkfatal(auxFile.found(), "-f auxFileName is required");
    HGraphWDimensions hg(auxFile, NULL,NULL,hgParams);

    if(maxAreaPercent.found())
    {
	double totalWeight = 0;
	itHGFNodeGlobal n;
	for(n = hg.nodesBegin(); n != hg.nodesEnd(); n++)
	{ totalWeight += (*n)->getWeight();}

        double maxCellArea = totalWeight*(maxAreaPercent/100);
        if(setDims.found())	//make the area a multiple of the height
	{ maxCellArea = floor(maxCellArea/setDims)*setDims;}

	for(n = hg.nodesBegin(); n != hg.nodesEnd(); n++)
	{
	    if((*n)->getWeight() > maxCellArea)
	    {	(*n)->setWeight(maxCellArea);}
	}
    } 

    if(setDims.found())
      {
	if(genMacros.found())
	  hg.setNodeDimsMacros(setDims);
	else
	  hg.setNodeDims(setDims);
	hg.makeCenterPinOffsets();
      }

    vector<double> coreCellWidths;
    vector<double> coreCellDegrees;
    double         coreCellArea = 0;
    double         coreCellHeight = 0;
    double         coreCellWidth  = 0;
    unsigned       numCoreCells = hg.getNumNodes()-hg.getNumTerminals();
    vector<unsigned> degreeDist(10000);
    double maxCoreCellArea = -DBL_MAX;

    for(unsigned n = hg.getNumTerminals(); n != hg.getNumNodes(); n++)
    {
	const HGFNode& node = hg.getNodeByIdx(n);

	coreCellWidths. push_back(hg.getNodeWidth(n));
	coreCellWidth += hg.getNodeWidth(n);
	coreCellHeight = max(hg.getNodeHeight(n), coreCellHeight);

	coreCellArea += node.getWeight();
	if(node.getWeight() > maxCoreCellArea)
	  maxCoreCellArea = node.getWeight();

	coreCellDegrees.push_back(node.getDegree());
	degreeDist[node.getDegree()]++;
    }


    cout<<"Hypergraph Statistics"<<endl<<endl;

    cout<<"Total Edges:  "<<setw(8)<<hg.getNumEdges()<<endl;
    cout<<"Total Nodes:  "<<setw(8)<<hg.getNumNodes()<<endl;
    cout<<"  Terminals:  "<<setw(8)<<hg.getNumTerminals()<<endl;
    cout<<"  Non-Terms:  "<<setw(8)<<numCoreCells<<endl;
    cout<<endl;

    cout<<"Attributes of Non-Terminals"<<endl;
    cout<<"     Total Area:  "<<coreCellArea<<endl;
    cout<<"         Height:  "<<coreCellHeight<<endl;
    cout<<"      Ave Width:  "<<coreCellWidth/(double)numCoreCells<<endl;
    cout<<"  Max Cell Area:  "<<maxCoreCellArea<<endl;
    cout<<"Max % Cell Area:  "<<100*maxCoreCellArea/coreCellArea<<" %"<<endl;
    cout<<endl;

    cout<<"  Distribution of Widths:"<<endl;
    cout<<TrivialStatsWithStdDev(coreCellWidths)<<endl;

    cout<<"  Distribution of Degrees:"<<endl;
    cout<<TrivialStatsWithStdDev(coreCellDegrees)<<endl;

    cout<<"  degree   : num nodes of this degree"<<endl;

    unsigned i;
    for(i = 0; i < degreeDist.size(); i++)
    {
	if(degreeDist[i])
	    cout<<"    "<<i<<": "<<degreeDist[i]<<endl;
    }


    vector<unsigned> netDegrees;
    degreeDist.clear();

    for(itHGFEdgeGlobal e=hg.edgesBegin(); e != hg.edgesEnd(); e++)
    {
	unsigned eDegree = (*e)->getDegree();
	netDegrees.push_back(eDegree);
	if(eDegree >= degreeDist.size())
	    degreeDist.insert(degreeDist.end(), (eDegree - degreeDist.size())+2, 0);
	degreeDist[eDegree]++;
    }


    cout<<"Net Degrees "<<TrivialStatsWithStdDev(netDegrees)<<endl;

    cout<<"degree   : num nets of this degree"<<endl;
    for(i = 0; i < degreeDist.size(); i++)
	if(degreeDist[i])
	    cout<<"  "<<i<<": "<<degreeDist[i]<<endl;

  
    if(saveAsNodes.found())
	hg.saveAsNodesNetsWts(saveAsNodes);

    if(saveAsNetD.found())
	hg.saveAsNetDAre(saveAsNetD);

    return 0;
}


