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
  BoolParam        checkLegal     ("checkLegal",argc,argv);
  BoolParam        markMacros     ("markMacros",argc,argv);
  BoolParam        snapToRows     ("snapToRows", argc, argv);
  BoolParam        snapToSites("snapToSites", argc, argv);
  BoolParam        stdNodeHeights("stdNodeHeights", argc, argv);

  RBPlacement::Parameters rbParams(argc,argv);
  DoubleParam   addDummy("addDummy",argc,argv);
  DoubleParam   tetherCells("tetherCells",argc,argv);
  BoolParam     takeTetherConstrFrmFile("takeTetherConstrFrmFile", argc, argv);
  DoubleParam   rgnSizePercent("rgnSizePercent",argc,argv);
  DoubleParam   tetherNewAR("tetherNewAR",argc,argv);
  DoubleParam   tetherNewWS("tetherNewWS",argc,argv);
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
  UnsignedParam useSingleNet("useSingleNet", argc, argv);
  DoubleParam   maxCellHeight("maxCellHeight", argc, argv);
  DoubleParam   maxCellWidth("maxCellWidth", argc, argv);
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
  StringParam   plotDummy("plotDummy", argc, argv);
  StringParam   plotWSMap("plotWSMap", argc, argv);

  BoolParam   spaceCellsEqually("spaceCellsEqually", argc, argv);
  BoolParam   spaceCellsWCongInfo("spaceCellsWCongInfo", argc, argv);
  BoolParam   shiftCellsLeft("shiftCellsLeft", argc, argv);

  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
     cout << "  Use '-help' or '-f filename.aux' " << endl;
     cout << "  Other options: \n"
          << "   -remSitesMacro   (remove Sites below Macros)\n"
          << "   -remSitesCong double (remove % of whitespace from congested regions)\n"
          << "   -addNodesCong double (attach % of whitespace dummy nodes to congested nodes)\n"
          << "   -stdNodeHeights  (set all node heights to row height)\n"
          << "   -fixMacros   (mark Macros as terminals while saving)\n"
          << "   -tetherCells double (add tethering nets to fract % of cells to achieve stability)\n"
          << "   -rgnSizePercent double (constraining region size for each cell during tethering. % of layout)\n"
          << "   -takeTetherConstrFrmFile (Take part of the nodes to tether from constraints.nodes input file\n"
          << "   -tetherNewAR  (Tether for a new reshaped layout)\n"
	  << "   -tetherNewWS  (Tether for a new rescaled layout)\n"
          << "   -addDummy double (add dummy filler cells. keep min % of whitespace)\n"
          << "   -saveAsNodes     base_filename\n"
	  << "   -saveLEFDEF      base_filename\n"
          << "   -saveAsCplace    base_filename\n"
          << "   -saveAsPlato     base_filename\n"
          << "   -markMacrosAsBlocks (for saveLEFDEF)\n"
	  << "   -savePl  filename\n"
	  << "   -savePlNoDummy  filename  (savePl with no dummy objects)\n"
          << "   -saveAsNodesWPinAssign     base_filename\n"
          << "   -saveAsNodesFloorplan base_filename\n"
          << "   -saveMacrosAsNodesFloorplan base_filename\n"
	  << "   -saveAsNodesShredHW  base_filename\n"
          << "   -useSingleNet   unsigned (use weighted single net construction while shredding)\n"
          << "   -maxCellWidth   double (width of shredded cells)\n"
	  << "   -maxCellHeight  double (height of shredded cells)\n"
 	  << "   -savePlacementUnShred  filename\n"
          << "   -xmin,-xmax,-ymin,-ymax  (for plotting range. Default means all)\n"
	  << "   -plotNets        base_fileName\n"
          << "   -plotNodes       base_fileName\n"
          << "   -plotNodesWNames base_fileName\n"
          << "   -plot            base_fileName   (plots nodes and nets)\n"
          << "   -plotWNames      base_fileName   (the above + node names)\n"
	  << "   -plotSites	  base_fileName   (plots the site map)\n"
	  << "   -plotNodesWSites base_fileName   (the above + nodes)\n"
          << "   -plotRows        base_fileName   (plot the rows)\n"
          << "   -plotDummy       base_fileName   (plot only dummy nodes+nets)\n"
          << "   -plotWSMap       base_fileName   (plot the whitespace histogram)\n"
          << "   -checkLegal (Use to check for out of core cells / overlaps)\n"
          << "   -legal  (Use to remove any overlaps in existing placement)\n"
	  << "   -snapToRows  (Use to snap cells to closest rows. e.g. for Kraftwerk Placements )\n"
          << "   -snapToSites  (Use to snap cells to sites in existing placement)\n"
	  << "   -spaceCellsEqually   (for each subrow, space cells equally)\n"
          << "   -spaceCellsWCongInfo (for each subrow, space cells w congestion info)\n"
          << "   -shiftCellsLeft      (pack Cells to the left of each subrow)\n"
          << endl;
     return 0;
  }
  cout<<"RBPlacement Parameters"<<endl;
  cout<<rbParams<<endl;

  StringParam auxFile("f", argc, argv);
  abkfatal(auxFile.found(), "must have -f <auxfilename>");
  RBPlacement rbplace(auxFile, rbParams);

  if(markMacros.found())
    {
      double maxCoreHeight = rbplace.getMaxHeightCoreRow();
      const_cast<HGraphWDimensions&>(rbplace.getHGraph()).markNodesAsMacro(DBL_MAX, maxCoreHeight);
    }

  if(snapToRows.found())
    {
      rbplace.alignCellsToRows();
      rbplace.snapCellsInSites();
    }
  
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

  if(remSitesCong.found())
    rbplace.removeSitesFromCongestedRgn(remSitesCong);

  if(remOverlaps.found())
    {
      rbplace.snapCellsInSites();
      rbplace.remOverlaps();
    }

  if(snapToSites.found())
    rbplace.snapCellsInSites();

  if(checkLegal.found())
    {
      unsigned numCellsNotPlaced = rbplace.numCellsNotPlaced();
      unsigned numOverlaps = rbplace.getNumOverlaps();
      if(numCellsNotPlaced > 0 || numOverlaps > 0)
	cout<<" Output of legality checker : FALSE \n";
      else
	cout<<" Output of legality checker : TRUE \n";
      cout<<" RBPlacement:"<<numCellsNotPlaced<<" core cell(s) not in rows"<<endl;
      cout<<" RBPlacement:"<<numOverlaps<<" overlaps found in placed core cells"<<endl;

      rbplace.checkOutOfCoreCells();
      cout<<" RBPlacement: Generic Overlap \n";
      rbplace.calcOverlapGeneric();
      rbplace.calcOverlapGeneric(true);
      cout<<endl;
    }

  if(remSitesMacro)
    rbplace.removeSitesBelowMacros();

  if(saveLEFDEF.found()) rbplace.saveLEFDEF(saveLEFDEF, markMacrosAsBlocks);

  if(saveAsNodes.found())
    {
      if(addDummy.found())
	{
	  rbplace.saveAsNodesNetsWtsWDummy(saveAsNodes, addDummy);
	}
      else if(addNodesCong.found())
	{
	  rbplace.saveAsNodesNetsWtsWCongInfo(saveAsNodes, addNodesCong);
	}
      else if(tetherCells.found())
	{
	  double regionSizePercent = 0.005;
	  double newAR = -1;
	  double newWS = -1;
	  if(rgnSizePercent.found())
	    regionSizePercent=rgnSizePercent;
	  if(tetherNewAR.found())
	    newAR = tetherNewAR;
	  if(tetherNewWS.found())
	    newWS = tetherNewWS;
	  
	  rbplace.saveAsNodesNetsWtsWTether(saveAsNodes, tetherCells, regionSizePercent, takeTetherConstrFrmFile, newAR, newWS);
	}
      else
	{
	  if(fixMacros.found())
	    rbplace.saveAsNodesNetsWts(saveAsNodes, fixMacros);
	  else
	    rbplace.saveAsNodesNetsWts(saveAsNodes);
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
    {
      unsigned singleNetWt = 0;
      double cellWidth = 0;
      double cellHeight = 0;
      if(useSingleNet.found())
	singleNetWt = useSingleNet;
      if(maxCellWidth.found())
	cellWidth = maxCellWidth;
      if(maxCellHeight.found())
	cellHeight = maxCellHeight;
      
      rbplace.saveAsNodesNetsWtsShredHW(saveAsNodesShredHW, cellHeight, cellWidth, singleNetWt);
    }

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

   if(plotDummy.found())
     rbplace.plotDummy("plot", minx,maxx,miny,maxy,plotDummy);

   if(plotWSMap.found())
     rbplace.plotWSHist(plotWSMap);

   pair<double, double> HPWL = rbplace.evalXYHPWL(false);
   cout<<" Center-to-center HalfPerim WL: "<<HPWL.first+HPWL.second<<" ("<<HPWL.first<<" x , "<<HPWL.second<<" y )"<<endl;;
   HPWL = rbplace.evalXYHPWL(true);
   cout<<" Pin-to-pin HalfPerim WL      : "<<HPWL.first+HPWL.second<<" ("<<HPWL.first<<" x , "<<HPWL.second<<" y )"<<endl;;
   
   //cout<<endl<<endl<<auxFile<<" numMacros is : "<<rbplace.getNumMacros()<<endl<<endl;  

   return 0;
}



