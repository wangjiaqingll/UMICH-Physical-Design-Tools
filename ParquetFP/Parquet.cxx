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





#include "Parquet.h"
#include "ABKCommon/paramproc.h"
using namespace parquetfp;

int main(int argc, const char *argv[])
{
	BoolParam help1 ("h", argc, argv);
	BoolParam help2 ("help", argc, argv);
        NoParams  noParams(argc,argv);  // this acts as a flag
	Command_Line* params = new Command_Line(argc, argv);
	params->printAnnealerParams();

	if (noParams.found() || help1.found() || help2.found())
	{
	  params->printHelp ();
	  exit (0);
	}	

        Timer T;
	T.stop();
	double totalTime=0;
	double successTime = 0;
	double successAvgWL = 0;
	double successAvgArea = 0;

	double minArea=1e100;
	double minWS=1e100;
	double minWL=1e100;
	double aveArea=0;
	double aveWS=0;
	double aveWL=0;
	double currArea;
	double lastArea;
	double currWS;
	double currWL;
	double currXSize;
	double currYSize;
	double successAR=0;
		
	for (int i=0; i<params->iterations; i++)
	{
	   DB* db = new DB(params->inFileName);
	   double blocksArea = db->getNodesArea();
	   double reqdArea = (1+(params->maxWS/100))*blocksArea;
	   double reqdWidth = sqrt(reqdArea*params->reqdAR);
	   double reqdHeight = reqdWidth/params->reqdAR;
	   bool gotBetterSol = false;

	   T.start(0.0);

	   if(!params->solveMulti)
	     {
	       Annealer annealer(params, db);
	       if(params->takePl)
		 {
		   //convert placement to sequence pair
		   annealer.takeSPfromDB();
		   annealer.eval();
		 }

	       if(params->initQP)
		 {
		   annealer.solveQP();
		   annealer.takeSPfromDB();
		 }
	       
	       if(params->initCompact)
		 {
		   annealer.eval();
		   currArea = annealer.getXSize()*annealer.getYSize();
		   bool whichDir = 0;
		   annealer.evalCompact(whichDir);
		   do
		     {
		       whichDir = !whichDir;
		       lastArea = currArea;
		       annealer.takeSPfromDB();
		       annealer.evalCompact(whichDir);
		       currArea = annealer.getXSize()*annealer.getYSize();
		     }
		   while(int(currArea) < int(lastArea));
		 }
	       annealer.go();
	     }
	   else
	     {
	       SolveMulti solveMulti(db, params);
	       solveMulti.go();
	       
	       if(params->compact)
		 {
		   //compact the design
		   Annealer annealer(params, db);
		   bool whichDir = 0;
		   annealer.takeSPfromDB();
		   annealer.eval();
		   currArea = annealer.getXSize()*annealer.getYSize();
		   annealer.evalCompact(whichDir);
		   do
		     {
		       whichDir = !whichDir;
		       lastArea = currArea;
		       annealer.takeSPfromDB();
		       annealer.evalCompact(whichDir);
		       currArea = annealer.getXSize()*annealer.getYSize();
		       cout<<currArea<<"\t"<<lastArea<<endl;
		     }
		   while(int(currArea) < int(lastArea));
		 }
	     }
	   T.stop();

	   totalTime += T.getUserTime();
	   currXSize = db->getXSize();
	   currYSize = db->getYSize();
	   currArea = currXSize*currYSize;
	   currWS = 100*(currArea - blocksArea)/currArea;
	   currWL = db->evalHPWL();
	   aveArea += currArea;
	   aveWS += currWS;
	   aveWL += currWL;
	   if(minArea > currArea)
	     {
	       minArea = currArea;
	       gotBetterSol = true;
	     }
	   if(minWS > currWS)
	     minWS = currWS;
	   if(minWL > currWL)
	     minWL = currWL;

	   if(params->reqdAR != -9999 && ((currArea<=reqdArea && 
	      currXSize<=reqdWidth && currYSize<=reqdHeight) || db->successAR))
	      {
	        ++successAR;
		successTime += T.getUserTime();
		successAvgWL += currWL;
		successAvgArea += currArea;
	      }
	   //plot and save the best solution

	   if(gotBetterSol)
	    {
	      if(params->plot)
	      {
	        double currAR = currXSize/currYSize;
                bool plotSlacks = !params->plotNoSlacks;
                bool plotNets = !params->plotNoNets;
	        bool plotNames = !params->plotNoNames;
                db->plot("out.gpl", currArea, currWS, currAR, T.getUserTime(), 
	      	  	  currWL, plotSlacks, plotNets, plotNames);
	      }
	      if(params->savePl)
	        db->getNodes()->savePl(params->outPlFile);

	      if(params->saveCapoPl)
		db->getNodes()->saveCapoPl(params->capoPlFile);

	      if(params->saveCapo)
	        db->saveCapo(params->capoBaseFile, params->reqdAR);

	      if(params->save)
		db->save(params->baseFile);
	      
	      //if(db->successAR)
	      //db->saveBestCopyPl("best.pl");
	    }
	   cout<<endl;
	}
        aveArea /= params->iterations;
	aveWS /= params->iterations;
	aveWL /= params->iterations;
        totalTime /= params->iterations;
	successTime /= successAR;
	successAvgWL /= successAR;
	successAvgArea /= successAR;
	successAR /= params->iterations;
	
	cout<<endl<<"Average Area: "<<aveArea<<" Minimum Area: "<<minArea<<endl
	    <<"Average HPWL: "<<aveWL<<" Minimum HPWL: "<<minWL<<endl
	    <<"Average WhiteSpace: "<<aveWS<<"%"<<" Minimum WhiteSpace: "
	    <<minWS<<"%"<<endl
	    <<"Average Time: "<<totalTime<<endl;
	if(params->reqdAR != -9999)
	 {
	   cout<<endl<<"Success Rate of satisfying fixed outline: "
	       <<100*successAR<<" %"<<endl;
	   cout<<"Average Time for successfull AR runs: "<<successTime<<endl;
	   cout<<"Average Area for successfull AR runs: "<<successAvgArea<<endl;
	   cout<<"Average WL for successfull AR runs: "<<successAvgWL<<endl;
	 }

	return 0;
}


