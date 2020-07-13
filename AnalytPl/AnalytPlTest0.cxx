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







#include "analytPl.h"

int main(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  BoolParam        floorplan   ("floorplan",   argc,argv);
  StringParam auxFile("f", argc, argv);
  UnsignedParam    skipNetsLargerThan("skipNetsLargerThan", argc, argv);

  if (noParams.found() || helpRequest.found() || helpRequest1.found())
    {
      cout << "  Use '-help' or '-f filename.aux' " << endl;
      cout <<"Options\n"
	   <<"-solve  (solve using SOR)\n"
           <<"-skipNetsLargerThan <unsigned>\n\n";
      exit(0);
    }

  RBPlacement::Parameters rbParams(argc,argv);

  abkfatal(auxFile.found(), "must have -f <auxfilename>");

  RBPlacement rbplace(auxFile, rbParams);
  BBox layoutBBox = rbplace.getBBox(true);
  const HGraphWDimensions& hgWDims = rbplace.getHGraph();
  
  //layoutBBox.ShrinkTo(0.8);

  vector< vector<unsigned> > nodeIds(1);
  vector<BBox> binBBox(1);
  for(unsigned i=0; i<hgWDims.getNumNodes(); ++i)
    {
      if(!hgWDims.isTerminal(i))
	nodeIds[0].push_back(i);
    }
  binBBox[0].xMin = layoutBBox.xMin;
  binBBox[0].xMax = layoutBBox.xMax;
  binBBox[0].yMin = layoutBBox.yMin;
  binBBox[0].yMax = layoutBBox.yMax;

//double change=DBL_MAX;
//double binMaxChange=0;

  AnalyticalSolver solver(rbplace, nodeIds, binBBox);
  solver.initNodeLocsToBinCenter();
  if(skipNetsLargerThan.found())
    solver.setSkipNetsLargerThan(skipNetsLargerThan);

  double maxDimension = max(layoutBBox.getHeight(),layoutBBox.getWidth());
  double epsilon = maxDimension/100;

  Timer totalTime;
  solver.solveQuadraticMin(epsilon);
  totalTime.stop();
  cout<<"Time taken = "<<totalTime.getUserTime()<<" seconds\n";

  vector< vector<Point> >& nodeLocs = solver.getNodeLocs();
  for(unsigned binIdx=0; binIdx<nodeIds.size(); ++binIdx)
    {
      for(unsigned i=0; i<nodeIds[binIdx].size(); ++i)
	{
	  rbplace.updatePlacement(nodeIds[binIdx][i], nodeLocs[binIdx][i]);
	}
    }
  double minx,maxx,miny,maxy;
  DoubleParam xmin("xmin",argc,argv);
  DoubleParam xmax("xmax",argc,argv);
  DoubleParam ymin("ymin",argc,argv);
  DoubleParam ymax("ymax",argc,argv);

  minx  = ( xmin.found() ?  xmin:-DBL_MAX);
  maxx  = ( xmax.found() ?  xmax:DBL_MAX);
  miny  = ( ymin.found() ?  ymin:-DBL_MAX);
  maxy  = ( ymax.found() ?  ymax:DBL_MAX);
  rbplace.saveAsPlot("plotNodes",minx,maxx,miny,maxy,"out");

  return 0;
}





