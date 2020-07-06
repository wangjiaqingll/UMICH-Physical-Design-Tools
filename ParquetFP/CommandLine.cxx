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









#include "CommandLine.h"
#include "ABKCommon/paramproc.h"
#include <iostream>
#include <string.h>
#include <time.h>
using namespace std;
using namespace parquetfp;

Command_Line::Command_Line () : getSeed(true),
	      budgetTime(0), softBlocks(0), initQP(0), seed(0), iterations(1),
	      maxIterHier(10),
	      seconds(0), plot(0), savePl(0), saveCapoPl(0), saveCapo(0), 
	      save(0), takePl(0), solveMulti(0), clusterPhysical(0), 
	      solveTop(0), compact(0), maxWSHier(15), usePhyLocHier(0),
	      dontClusterMacros(0), maxTopLevelNodes(-9999),
	      timeInit(30000), timeCool(0.01), 
	      startTime(30000), reqdAR(-9999), maxWS(15), minWL(0), 
	      areaWeight(0.4), wireWeight(0.4), initCompact(0), 
	      useFastSP(false)
{
}

Command_Line::Command_Line (int argc, const char *argv[]) : getSeed(0),
              budgetTime(0), softBlocks(0), initQP(0), seed(0), iterations(0),
   	      maxIterHier(10),
	      seconds(0), plot(0), savePl(0), saveCapoPl(0), saveCapo(0), 
	      save(0), takePl(0), solveMulti(0), clusterPhysical(0), 
	      solveTop(0), compact(0), maxWSHier(15), usePhyLocHier(0),
	      dontClusterMacros(0), maxTopLevelNodes(-9999), 
	      timeInit(30000), timeCool(0.01), 
	      reqdAR(-9999), maxWS(15), minWL(0), areaWeight(0.4), 
	      wireWeight(0.4), initCompact(0), useFastSP(false)
{
        StringParam argInfile ("f", argc, argv);
	StringParam plOutFile ("savePl", argc, argv);
	StringParam saveCapoPlFile ("saveCapoPl", argc, argv);
	StringParam saveCapoFile ("saveCapo", argc, argv);
	StringParam saveFile ("save", argc, argv);

	BoolParam help1 ("h", argc, argv);
	BoolParam help2 ("help", argc, argv);
        NoParams  noParams(argc,argv);  // this acts as a flag
    	IntParam  fixSeed ("s",argc,argv);
	IntParam  numberOfRuns("n",argc,argv);	
	DoubleParam timeReq("t",argc,argv);
	IntParam  maxIterHier_("maxIterHier",argc,argv);	

	DoubleParam timeInit_("timeInit",argc,argv);
	DoubleParam timeCool_("timeCool",argc,argv);
	DoubleParam startTime_("startTime",argc,argv);
	DoubleParam reqdAR_("AR",argc,argv);
	DoubleParam maxWS_("maxWS",argc,argv);
	BoolParam   minWL_("minWL",argc,argv);
	DoubleParam wireWeight_("wireWeight",argc,argv);
	DoubleParam areaWeight_("areaWeight",argc,argv);
	BoolParam softBlocks_("soft", argc, argv);
	BoolParam initQP_("initQP", argc, argv);
	BoolParam fastSP_("fastSP", argc, argv);

	BoolParam plot_("plot",argc,argv);
	BoolParam plotNoNets_("plotNoNets", argc, argv);
	BoolParam plotNoSlacks_("plotNoSlacks", argc, argv);
	BoolParam plotNoNames_("plotNoNames",argc,argv);
	BoolParam takePl_("takePl",argc,argv);
	BoolParam solveMulti_("solveMulti",argc,argv);
	BoolParam clusterPhysical_("clusterPhysical",argc,argv);
	BoolParam solveTop_("solveTop",argc,argv);
	DoubleParam maxWSHier_("maxWSHier",argc,argv);
        BoolParam usePhyLocHier_("usePhyLocHier",argc,argv);
	BoolParam dontClusterMacros_("dontClusterMacros",argc,argv);
	IntParam maxTopLevelNodes_("maxTopLevelNodes",argc,argv);
	BoolParam compact_("compact",argc,argv);
	BoolParam initCompact_("initCompact", argc, argv);

	// now set up member vars
	if (argInfile.found())
	  {
	    string temp;
	    temp=argInfile;
	    strcpy(inFileName,temp.c_str());
	  }
	else
	  {
	    strcpy(inFileName,"TESTS/ami49");
	  }

	if(plOutFile.found())
        {
	  strcpy(outPlFile, plOutFile);
	  savePl = 1;
	}

	if(saveCapoPlFile.found())
        {
	  strcpy(capoPlFile, saveCapoPlFile);
	  saveCapoPl = 1;
	}

	if(saveCapoFile.found())
        {
	  strcpy(capoBaseFile, saveCapoFile);
	  saveCapo = 1;
	}

	if(saveFile.found())
        {
	  strcpy(baseFile, saveFile);
	  save = 1;
	}
		
	if (fixSeed.found())
	{
		getSeed=false;
		seed = fixSeed;
	}
	else
		getSeed=true;//get sys time as seed

	if (numberOfRuns.found())
		iterations = numberOfRuns;
	else
		iterations = 1;
	
	if (maxIterHier_.found())
	  maxIterHier = maxIterHier_;

	if (timeReq.found())
	{
		budgetTime=true;//limit number of runs
		seconds = timeReq;
	}
	else
		budgetTime=false;

	  
	if(timeInit_.found())
	  timeInit = timeInit_;

	if(startTime_.found())
	  startTime = startTime_;
	else
	  startTime = timeInit;

	  
	if(timeCool_.found())
	  timeCool = timeCool_;
	  
	if(reqdAR_.found())
	  reqdAR = reqdAR_;   //default -9999 means no fixed outline desired
	
	if(maxWS_.found())
	  maxWS = maxWS_;

	if(maxWSHier_.found())
	  maxWSHier = maxWSHier_;

        if(usePhyLocHier_.found())
          usePhyLocHier = 1;

	if(maxTopLevelNodes_.found())
	  maxTopLevelNodes = maxTopLevelNodes_;

	if(minWL_.found())
	  {
	    minWL = 1;
	  }

	if(areaWeight_.found())
	  {
	    areaWeight = areaWeight_;
	    if(areaWeight > 1 || areaWeight < 0)
	      {
		cout<<"areaWeight should be : 0 <= areaWeight <= 1"<<endl;  
		exit(0);
	      }
	  }

	if(wireWeight_.found())
	  {
	    wireWeight = wireWeight_;
	    if(wireWeight > 1 || wireWeight < 0)
	      {
		cout<<"wireWeight should be : 0 <= wireWeight <= 1"<<endl;  
		exit(0);
	      }

	    if(wireWeight == 0) //turn off minWL if wireWeight is 0
	      minWL = 0;
	  }

	if(takePl_.found())
	  takePl = 1;

	if(solveMulti_.found())
	  solveMulti = 1;

	if(clusterPhysical_.found())
	  clusterPhysical = 1;

	if(solveTop_.found())
	  solveTop = 1;

	if(dontClusterMacros_.found())
	  dontClusterMacros = 1;

	if(softBlocks_.found())
	  softBlocks = 1;
	  
        if(initQP_.found())
	  initQP = 1;

        if(fastSP_.found())
	  useFastSP = 1;

        if(compact_.found())
	  compact = 1;

        if(initCompact_.found())
	  initCompact = 1;
 
	if(plot_.found() || plotNoNets_.found() || plotNoSlacks_.found() ||
	   plotNoNames_.found())
	  plot = 1;
	
	plotNoNets = plotNoNets_;
	plotNoSlacks = plotNoSlacks_;
	plotNoNames = plotNoNames_;

}

void Command_Line::printHelp ()
{
	cerr<<"Parquet.exe \n-f filename\n"
		<<"-s int        (give a fixed seed)\n"
		<<"-n int        (determine number of runs. default 1)\n"
		<<"-t double     (set a time limit on the annealing run)\n"
		<<"-save basefilename (save design in bookshelf format)\n"
		<<"-savePl baseFilename (save .pl file of solution)\n"
		<<"-saveCapoPl basefilename (save .pl in Capo format)\n"
		<<"-saveCapo basefilename (save design in Capo format)\n"
		<<"-plot         (plot the output solution to out.gpl file)\n"
		<<"-plotNoNets   (plot without the nets)\n"
		<<"-plotNoSlacks (plot without slacks info)\n"
		<<"-plotNoNames  (plot without name of blocks)\n"
		<<"-timeInit double  (initial normalizing time: default 30000)\n"
		<<"-startTime double (annealing initial time: default timeInit)\n"
		<<"-timeCool double  (annealing cool time: default 0.01\n"
		<<"-AR double  (required Aspect Ratio of fixed outline: default no Fixed Outline)\n"
	        <<"-maxWS double (maxWS(%) allowed if fixed outline constraints)\n"
	        <<"-maxWSHier double (maxWS(%) for each hierarchical block)\n"
	        <<"-usePhyLocHier (use physical locs which updating locs of sub-blocks of clustered blocks)\n"
	        <<"-maxTopLevelNodes int (max # top level nodes during clustering)\n"
	        <<"-maxIterHier int (max # of iterations in hierarchical mode to satisfy fixed-outline)\n"
		<<"-minWL        (minimize WL default turned off)\n"
	        <<"-wireWeight double  (default 0.4)\n"
	        <<"-areaWeight double  (default 0.4)\n"
	        <<"-soft         (soft Blocks present in input default no)\n"
		<<"-initQP       (start the annealing with a QP solution)\n"
	        <<"-fastSP       (use O(nlog n) algo for sequence pair evaluation)\n"
		<<"-takePl       (take a placement and convert to sequence pair for use as initial solution)\n"
	        <<"-solveMulti   (solve as multiLevel heirarchy)\n"
	        <<"-clusterPhysical (use Physical Heirarchy)\n"
	        <<"-dontClusterMacros (keep Macros out of Clustering)\n"
	        <<"-solveTop     (solve only top level of heirarchy)\n"
	        <<"-compact      (compact the final solution)\n"
		<<"-initCompact  (construct initial SP by compaction)\n"
		<<endl;
}

void Command_Line::printAnnealerParams()
{
  cout<<"Annealer Params: "<<endl;
  cout<<"\tnormalizing Time  "<<timeInit<<endl;
  cout<<"\tstart Time        "<<startTime<<endl;
  cout<<"\tcooling Time      "<<timeCool<<endl;
  cout<<"\treqd Aspect Ratio "<<reqdAR<<" (-9999 means no fixed shape)"<<endl;
  cout<<"\tminimize WL       "<<minWL;
  if(minWL == 1)
    cout<<"   :  wireWeight = "<<wireWeight;
  cout<<endl;
  cout<<"\tmaximum WS        "<<maxWS<<"% (only for fixed-outline)"<<endl<<endl;
}


