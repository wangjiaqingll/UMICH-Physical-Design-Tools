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




#include "ABKCommon/abkcommon.h"
#include "RBPlacement.h"

int main(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  BoolParam        remOverlaps    ("legal",argc,argv);
  BoolParam        snapToSites("snapToSites", argc, argv);
  BoolParam        stdNodeHeights("stdNodeHeights", argc, argv);

  RBPlacement::Parameters rbParams(argc,argv);
  StringParam   saveAsNodes("saveAsNodes", argc, argv);
  StringParam   saveAsCplace("saveAsCplace", argc, argv);
  StringParam   saveAsPlato("saveAsPlato", argc, argv);
  StringParam   saveLEFDEF ("saveLEFDEF",  argc, argv);
  BoolParam     markMacrosAsBlocks("markMacrosAsBlocks", argc, argv);
  BoolParam   remSitesMacro("remSitesMacro", argc, argv);
  BoolParam   fixMacros("fixMacros", argc, argv);
  DoubleParam   remSitesCong("remSitesCong", argc, argv);
  DoubleParam   addNodesCong("addNodesCong",argc,argv);
  StringParam   saveAsNodesFloorplan("saveAsNodesFloorplan", argc, argv);
  StringParam   saveMacrosAsNodesFloorplan("saveMacrosAsNodesFloorplan", 
					   argc, argv);
  StringParam   saveAsNodesShredHW("saveAsNodesShredHW", argc, argv);
  StringParam   saveAsNodesWPinAssign("saveAsNodesWPinAssign", argc, argv);
  StringParam   savePlacementUnShred("savePlacementUnShred", argc, argv);
  StringParam   savePl("savePl", argc, argv);
  StringParam   savePlNoDummy("savePlNoDummy", argc, argv);
  StringParam   plotNets("plotNets", argc, argv);
  StringParam   plotNodes("plotNodes", argc, argv);
  StringParam   plotNodesAndNets("plot", argc, argv);
  StringParam   plotNodesWNames("plotNodesWNames", argc, argv);
  StringParam   plotNodesAndNetsWNames("plotWNames", argc, argv);
  StringParam   plotSites("plotSites", argc, argv);
  StringParam   plotNodesWSites("plotNodesWSites", argc, argv);  
  StringParam   plotRows("plotRows", argc, argv);

  BoolParam   spaceCellsEqually("spaceCellsEqually", argc, argv);
  BoolParam   spaceCellsWCongInfo("spaceCellsWCongInfo", argc, argv);
  BoolParam   shiftCellsLeft("shiftCellsLeft", argc, argv);


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


  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
     cout << "  Use '-help' or '-f filename.aux' " << endl;
     cout << "  Saving options: \n"
          << "   -saveAsNodes     base_filename\n"
          << "   -saveAsCplace    base_filename\n"
          << "   -saveAsPlato     base_filename\n"
	  << "   -saveLEFDEF      base_filename\n"
          << "   -markMacrosAsBlocks (for saveLEFDEF)\n"
	  << "   -savePl  filename\n"
	  << "  Misc options: \n"
          << "   -legal  (Use to remove any overlaps in existing placement)\n"
          << "   -snapToSites  (Use to snap cells to sites in existing placement)\n"
	  << "   -spaceCellsEqually   (for each subrow, space cells equally)\n"
       	  << "  Plotting options: \n"
          << "   -xmin,-xmax,-ymin,-ymax  (for plotting range. Default means all)\n"
	  << "   -plotNets        base_fileName\n"
          << "   -plotNodes       base_fileName\n"
          << "   -plotNodesWNames base_fileName\n"
          << "   -plot            base_fileName   (plots nodes and nets)\n"
          << "   -plotWNames      base_fileName   (the above + node names)\n"
	  << "   -plotSites	  base_fileName   (plots the site map)\n"
	  << "   -plotNodesWSites base_fileName   (the above + nodes)\n"
          << "   -plotRows        base_fileName   (plot the rows)\n"
          << endl;
     return 0;
  }
  cout<<"RBPlacement Parameters"<<endl;
  cout<<rbParams<<endl;

  StringParam auxFile("f", argc, argv);
  abkfatal(auxFile.found(), "must have -f <auxfilename>");
  RBPlacement rbplace(auxFile, rbParams);
  
  if(stdNodeHeights.found())
    {
      double rowHeight = (rbplace.coreRowsBegin())->getHeight();
      const_cast<HGraphWDimensions&>(rbplace.getHGraph()).setNodeDims(rowHeight);
    }

  if(spaceCellsWCongInfo.found())
    rbplace.spaceCellsWCongInfoInRows1();

  if(spaceCellsEqually.found())
    rbplace.spaceCellsEquallyInRows();

  if(shiftCellsLeft.found())
    rbplace.shiftCellsLeft();

// add code that produces gnuplot files by traversing the hypergraph
// and the layout from rbplace   

  if(remSitesCong.found())
    rbplace.removeSitesFromCongestedRgn(remSitesCong);

  if(remOverlaps.found())
    {
      rbplace.snapCellsInSites();
      rbplace.remOverlaps();
    }

  if(snapToSites.found())
    rbplace.snapCellsInSites();

  if(remSitesMacro)
         rbplace.removeSitesBelowMacros();

  if(saveLEFDEF.found()) rbplace.saveLEFDEF(saveLEFDEF, markMacrosAsBlocks);

  if(saveAsNodes.found())
    {
      if(!addNodesCong.found())
	{
	  if(fixMacros.found())
	    rbplace.saveAsNodesNetsWts(saveAsNodes, fixMacros);
	  else
	    rbplace.saveAsNodesNetsWts(saveAsNodes);
	}
      else
	{
	  rbplace.saveAsNodesNetsWtsWCongInfo(saveAsNodes, addNodesCong);
	}
    }

  if(saveAsNodesWPinAssign.found())
    {
      rbplace.assignPinsToCells();
      rbplace.saveAsNodesNetsWts(saveAsNodesWPinAssign);
    }

  if(saveAsNodesFloorplan.found())
    rbplace.saveAsNodesNetsPlFloorplan(saveAsNodesFloorplan);

  if(saveMacrosAsNodesFloorplan.found())
    rbplace.saveMacrosAsNodesNetsPlFloorplan(saveMacrosAsNodesFloorplan);

  if(saveAsNodesShredHW.found()) 
    rbplace.saveAsNodesNetsWtsShredHW(saveAsNodesShredHW);

  if(savePlacementUnShred.found()) 
    rbplace.savePlacementUnShredHW(savePlacementUnShred);

  if(savePl.found()) 
    rbplace.savePlacement(savePl);

  if(savePlNoDummy.found()) 
    rbplace.savePlNoDummy(savePlNoDummy);

  if(saveAsCplace.found()) rbplace.saveAsCplace(saveAsCplace);

  if(saveAsPlato.found()) rbplace.saveAsPlato(saveAsPlato);

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
    	 rbplace.saveAsPlot("plotNets",minx,maxx,miny,maxy,plotNets);

   if(plotNodes.found())
    	 rbplace.saveAsPlot("plotNodes",minx,maxx,miny,maxy,plotNodes);

   if(plotNodesAndNets.found())
	 rbplace.saveAsPlot("plot",minx,maxx,miny,maxy,plotNodesAndNets);

  if(plotNodesWNames.found())
    	 rbplace.saveAsPlot("plotNodesWNames",minx,maxx,miny,maxy,plotNodesWNames);

   if(plotNodesAndNetsWNames.found())
	 rbplace.saveAsPlot("plotWNames",minx,maxx,miny,maxy,plotNodesAndNetsWNames);

   if(plotSites.found())
	 rbplace.saveAsPlot("plotSites",minx,maxx,miny,maxy,plotSites);

   if(plotNodesWSites.found())
         rbplace.saveAsPlot("plotNodesWSites",minx,maxx,miny,maxy,plotNodesWSites);

   if(plotRows.found())
	 rbplace.saveAsPlot("plotRows",minx,maxx,miny,maxy,plotRows);

   //cout<<endl<<endl<<auxFile<<" numMacros is : "<<rbplace.getNumMacros()<<endl<<endl;


  return 0;
}



