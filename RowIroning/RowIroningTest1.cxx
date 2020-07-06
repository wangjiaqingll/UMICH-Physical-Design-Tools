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
#include "ABKCommon/abkversion.h"
#include "DB/DB.h"
#include "RBPlFromDB/RBPlaceFromDB.h"
#include "NetTopology/hgWDimsFromDB.h"
#include "rowIroning.h"
#include "FixOr/greedyOrientOpt.h"


int main(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  BoolParam        doFlipping     ("flip",argc,argv);
  StringParam      auxFileName    ("f",   argc,argv);
  BoolParam        save           ("save",argc,argv);
  BoolParam        noRowIroning   ("noRowIroning",argc,argv);

  StringParam   plotNets("plotNets", argc, argv);
  StringParam   plotNodes("plotNodes", argc, argv);
  StringParam   plotNodesAndNets("plot", argc, argv);
  StringParam   plotNodesWNames("plotNodesWNames", argc, argv);
  StringParam   plotNodesAndNetsWNames("plotWNames", argc, argv);
  StringParam   plotSites("plotSites", argc, argv);
  StringParam   plotNodesWSites("plotNodesWSites", argc, argv);  
  BoolParam     keepOverlaps    ("skipLegal",argc,argv);
  DoubleParam xmin("xmin",argc,argv);
  DoubleParam xmax("xmax",argc,argv);
  DoubleParam ymin("ymin",argc,argv);
  DoubleParam ymax("ymax",argc,argv);
  
  double minx  = ( xmin.found() ?  xmin:-DBL_MAX);
  double maxx  = ( xmax.found() ?  xmax:DBL_MAX);
  double miny  = ( ymin.found() ?  ymin:-DBL_MAX);
  double maxy  = ( ymax.found() ?  ymax:DBL_MAX);

  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
     cout << " Use '-help' or '-f filename.aux' " << endl;
     return 0;
  }

  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux <more params>"); 

  RBPlaceParams rbParam(argc,argv);
  RBPlacement rbplace(auxFileName, rbParam);   

  RowIroningParameters RIparams(argc,argv);

  if(!keepOverlaps.found())
    {
      rbplace.remOverlaps();
    }

  if(!noRowIroning.found())
    {
      cout << " ====== Launching row ironing ... " << endl;
      cout << RIparams;
      
      cout <<"Initial Origin-to-Origin WL: "<<rbplace.evalHPWL(false)<<endl;
      cout <<"Initial Pin-to-Pin WL:       "<<rbplace.evalHPWL(true)<<endl;
      
      Timer RITimer;

      ironRows(rbplace, RIparams);
      rbplace.remOverlaps();

      RITimer.stop();
      
      cout << " Final Origin-to-Origin WL: "<<rbplace.evalHPWL(false)<<endl;
      cout << " Final Pin-to-Pin WL:       "<<rbplace.evalHPWL(true)<<endl;
      
    }

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

  if(save.found())
    {
      cout << "Saving out.pl" << endl;
      rbplace.savePlacement("out.pl");
    }
  
  return 0;
}



