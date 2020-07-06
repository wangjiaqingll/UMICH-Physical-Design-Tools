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





#include "ParquetDBFromRBP.h"

int main(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  BoolParam        floorplan   ("floorplan",   argc,argv);
  StringParam saveFile ("save", argc, argv);
  StringParam auxFile("f", argc, argv);
  if (noParams.found() || helpRequest.found() || helpRequest1.found())
    {
      cout << "  Use '-help' or '-f filename.aux' " << endl;
      cout <<"Options\n"
	   <<"-save base_file_name  (save design in floorplan bookshelf format)\n\n";
      exit(0);
    }

  RBPlacement::Parameters rbParams(argc,argv);

  abkfatal(auxFile.found(), "must have -f <auxfilename>");

  RBPlacement rbplace(auxFile, rbParams);
  BBox layoutBBox = rbplace.getBBox(false);
  vector<unsigned> nodeIds;
  const HGraphWDimensions& hgWDims = rbplace.getHGraph();
  double nodesArea = 0;
  
  for(unsigned i=0; i<hgWDims.getNumNodes(); ++i)
    {
      if(!hgWDims.isTerminal(i))
	{
	  nodeIds.push_back(i);
	  nodesArea += hgWDims.getNodeWidth(i)*hgWDims.getNodeHeight(i);
	}
    }
  double reqdArea = layoutBBox.getArea();
  double reqdAR = layoutBBox.getWidth()/layoutBBox.getHeight();
  double maxWS = 100*(reqdArea-nodesArea)/nodesArea;
  cout<<"reqd Aspect Ratio "<<reqdAR<<endl;
  cout<<"reqdArea "<<reqdArea<<endl;
  cout<<"nodesArea "<<nodesArea<<endl;
  cout<<"%WS  "<<maxWS<<" %"<<endl;

  ParquetDBFromRBP* fpDB = new ParquetDBFromRBP(rbplace, nodeIds,layoutBBox);
  parquetfp::Command_Line* params = new parquetfp::Command_Line(argc, argv);
  params->reqdAR = reqdAR;
  params->maxWS = maxWS;
  params->printAnnealerParams();

  /*
  double avgHeight = fpDB->getAvgHeight();
  fpDB->markTallNodesAsMacros(avgHeight);
  fpDB->reduceCoreCellsArea(0.99*reqdArea, 0.15);
  */

  if(floorplan.found())
    {
      parquetfp::Annealer annealer(params, fpDB);
      annealer.go();
    }

  /*
  parquetfp::SolveMulti solveMulti(fpDB, params);
  solveMulti.go();
  */
  /*
  vector<Point>& nodeLocs = fpDB->getNodeLocs();
  vector<Orient>& nodeOrients = fpDB->getNodeOrients();
  for(unsigned i=0; i<nodeIds.size(); ++i)
    {
      const HGFNode& node = hgWDims.getNodeByIdx(nodeIds[i]);
      cout<<node.getName()<<" "<<nodeLocs[i]<<" "<<nodeOrients[i]<<endl;
    }
  */

  char saveFileName[1000];
  if(saveFile.found())
    {
      strcpy(saveFileName,saveFile);
      fpDB->save(saveFileName);
    }

  delete fpDB;
  return 0;
}





