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





#include <ABKCommon/abkcommon.h>
#include <DB/DB.h>
#include "RBPlaceFromDB.h"
#include <RBPlace/pinLocCalc.h>

int main(int argc, const char *argv[])
{
  cout<<"/**************************************************************************\n"
      <<"***\n"
      <<"*** Copyright (c) 1995-2000 Regents of the University of California,\n"
      <<"***               Andrew E. Caldwell, Andrew B. Kahng and Igor L. Markov\n"
      <<"*** Copyright (c) 2000-2003 Regents of the University of Michigan,\n"
      <<"***               Saurabh N. Adya and Igor L. Markov\n"
      <<"***\n"
      <<"***  Contact author(s): abk@cs.ucsd.edu, imarkov@umich.edu\n"
      <<"***  Original Affiliation:   UCLA, Computer Science Department,\n"
      <<"***                          Los Angeles, CA 90095-1596 USA\n"
      <<"***\n"
      <<"***  Permission is hereby granted, free of charge, to any person obtaining \n"
      <<"***  a copy of this software and associated documentation files (the\n"
      <<"***  Software, to deal in the Software without restriction, including\n"
      <<"***  without limitation \n"
      <<"***  the rights to use, copy, modify, merge, publish, distribute, sublicense, \n"
      <<"***  and/or sell copies of the Software, and to permit persons to whom the \n"
      <<"***  Software is furnished to do so, subject to the following conditions:\n"
      <<"***\n"
      <<"***  The above copyright notice and this permission notice shall be included\n"
      <<"***  in all copies or substantial portions of the Software.\n"
      <<"***\n"
      <<"*** THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, \n"
      <<"*** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES\n"
      <<"*** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. \n"
      <<"*** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY\n"
      <<"*** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT\n"
      <<"*** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR\n"
      <<"*** THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n"
      <<"***\n"
      <<"***\n"
      <<"***************************************************************************\n"<<endl;

  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  StringParam auxFileName("f",argc,argv);
  StringParam saveAsNodes   ("saveAsNodes",argc,argv);
  StringParam savePl("savePl", argc, argv);

  StringParam   plotNets("plotNets", argc, argv);
  StringParam   plotNodes("plotNodes", argc, argv);
  StringParam   plotNodesAndNets("plot", argc, argv);
  StringParam   plotNodesWNames("plotNodesWNames", argc, argv);
  StringParam   plotNodesAndNetsWNames("plotWNames", argc, argv);
  StringParam   plotSites("plotSites", argc, argv);
  StringParam   plotNodesWSites("plotNodesWSites", argc, argv);
  StringParam   plotRows("plotRows", argc, argv);

  if (noParams.found() || helpRequest.found() || helpRequest1.found())
    {
      cout << "  Utility for converting LEF/DEF to Bookshelf format"<<endl;
      cout << "  Use '-help' or '-f filename.aux' " << endl;
      cout << "  Other options: \n"
	   << "  -saveAsNodes base_filename)\n"
	   << "  -savePl filename)\n"
	   << "  -xmin,-xmax,-ymin,-ymax  (for plotting range. Default means all)\n"
	   << "  -plotNets        base_fileName\n"
	   << "  -plotNodes       base_fileName\n"
	   << "  -plotNodesWNames base_fileName\n"
	   << "  -plot            base_fileName   (plots nodes and nets)\n"
	   << "  -plotWNames      base_fileName   (the above + node names)\n"
	   << "  -plotSites       base_fileName   (plots the site map)\n"
	   << "  -plotNodesWSites base_fileName   (the above + nodes)\n"
	   << "  -plotRows        base_fileName   (plot the rows)\n"
	   << endl;
      return 0;
    }
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux");

  BoolParam        stdNodeHeights("stdNodeHeights", argc, argv);
  BoolParam check("checkDB",argc,argv);
  DB::Parameters dbParams;
  dbParams.alwaysCheckConsistency = check;

  DB db(auxFileName,dbParams);

  cout<<" Building RBPlacement"<<endl;

  RBPlacement::Parameters rbParams(argc,argv);
  RBPlaceFromDB rbp(db, rbParams);

  cout << " Row-based placement ";
  if (rbp.isConsistent()) cout << "consistent " << endl;
  else cout << "inconsistent " << endl;

  cout<<"c-c HPWL: "<<rbp.evalHPWL()<<endl;
  cout<<"p-p HPWL: "<<rbp.evalHPWL(true)<<endl;

  if(stdNodeHeights.found())
    {
      double rowHeight = (rbp.coreRowsBegin())->getHeight();
      const_cast<HGraphWDimensions&>(rbp.getHGraph()).setNodeDims(rowHeight);
    }
  
  if(saveAsNodes.found())
      rbp.saveAsNodesNetsWts(saveAsNodes);

  if(savePl.found())
    rbp.savePlacement(savePl);

  double minx,maxx,miny,maxy;
  DoubleParam xmin("xmin",argc,argv);
  DoubleParam xmax("xmax",argc,argv);
  DoubleParam ymin("ymin",argc,argv);
  DoubleParam ymax("ymax",argc,argv);

  minx  = ( xmin.found() ?  xmin:-DBL_MAX);
  maxx  = ( xmax.found() ?  xmax:DBL_MAX);
  miny  = ( ymin.found() ?  ymin:-DBL_MAX);
  maxy  = ( ymax.found() ?  ymax:DBL_MAX);

  if(plotNets.found())
    rbp.saveAsPlot("plotNets",minx,maxx,miny,maxy,plotNets);

  if(plotNodes.found())
    rbp.saveAsPlot("plotNodes",minx,maxx,miny,maxy,plotNodes);

  if(plotNodesAndNets.found())
    rbp.saveAsPlot("plot",minx,maxx,miny,maxy,plotNodesAndNets);

  if(plotNodesWNames.found())
    rbp.saveAsPlot("plotNodesWNames",minx,maxx,miny,maxy,plotNodesWNames);

  if(plotNodesAndNetsWNames.found())
    rbp.saveAsPlot("plotWNames",minx,maxx,miny,maxy,plotNodesAndNetsWNames);

  if(plotSites.found())
    rbp.saveAsPlot("plotSites",minx,maxx,miny,maxy,plotSites);

  if(plotNodesWSites.found())
    rbp.saveAsPlot("plotNodesWSites",minx,maxx,miny,maxy,plotNodesWSites);

  if(plotRows.found())
    rbp.saveAsPlot("plotRows",minx,maxx,miny,maxy,plotRows);

  //rbp.calcOverlapGeneric();
  //cout<<endl;
  pair<double, double> HPWL = rbp.evalXYHPWL(false);
  cout<<" Center-to-center HalfPerim WL: "<<HPWL.first+HPWL.second<<" ("<<HPWL.first<<" x , "<<HPWL.second<<" y )"<<endl;;
  HPWL = rbp.evalXYHPWL(true);
  cout<<" Pin-to-pin HalfPerim WL      : "<<HPWL.first+HPWL.second<<" ("<<HPWL.first<<" x , "<<HPWL.second<<" y )"<<endl;;

  return 0;
}
