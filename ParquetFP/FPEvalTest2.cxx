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





#include "FPcommon.h"
#include "Annealer.h"
#include "CommandLine.h"
#include "ABKCommon/paramproc.h"
#include "ABKCommon/infolines.h"
using namespace parquetfp;

int main(int argc, const char *argv[])
{
           double currXSize;
	   double currYSize;
	   double currArea = -1e100;
	   double lastArea;
	   double currWS;
	   double currWL;
	   double currAR=0;

	   BoolParam help1 ("h", argc, argv);
	   BoolParam help2 ("help", argc, argv);
	   NoParams  noParams(argc,argv);  // this acts as a flag
	   Command_Line* params = new Command_Line(argc, argv);
	   params->printAnnealerParams();
	   if (noParams.found() || help1.found() || help2.found())
	     {
	       cerr<<"This test case test different functionalities\n"
		   <<"Options \n"
		   <<"FPEvalTest2.exe\n"
		   <<"-f baseFileName\n"
		   <<"-takePl   (construct SP from existing floorplan)\n"
		   <<"-initCompact (compact the initial solution)\n"
		   <<"-soft     (soft blocks in design. pack.)\n"
		   <<"-plot     (plot floorplan)\n"
		   <<"-save     (save in bookshelf floorplan format)\n"
		   <<"-savePl   (save only .pl file)\n"
		   <<"-saveCapoPl (save .pl in Capo bookshelf format)\n"
		   <<"-saveCapo  (save design in Capo bookshelf format)\n";
	       exit (0);
	     }	
	
	   DB* db = new DB(params->inFileName);
	   double blocksArea = db->getNodesArea();
	   
	   Annealer annealer(params, db);
	   if(params->takePl)
	     {
	       //convert placement to sequence pair
	       annealer.takeSPfromDB();
	     }

	   Timer T;
	   T.stop();
	   double totalTime=0;
           T.start(0.0);
	   for(unsigned iter=0; iter<10; ++iter)
	     annealer.eval();
           T.stop();
           totalTime += T.getUserTime();

	   currArea = annealer.getXSize()*annealer.getYSize();
	   annealer.evalSlacks();

	   if(params->initCompact)
	     {
	       bool whichDir = 0;
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

	   if(params->softBlocks)
	     {
	       annealer.packSoftBlocks(100);
	       annealer.updatePlacement();	       
	     }

	   currXSize = annealer.getXSize();
	   currYSize = annealer.getYSize();
	   currArea = currXSize*currYSize;
	   currWS = 100*(currArea - blocksArea)/currArea;
 	   currWL = db->evalHPWL();
	   currAR = currXSize/currYSize;

	   if(params->plot)
	    {
	      currAR = currXSize/currYSize;
              bool plotSlacks = !params->plotNoSlacks;
              bool plotNets = !params->plotNoNets;
	      bool plotNames = !params->plotNoNames;
              db->plot("out.gpl", currArea, currWS, currAR, 0, 
	      		currWL, plotSlacks, plotNets, plotNames);
	    }
	    
	   if(params->save)
	     db->save(params->baseFile);

           if(params->savePl)
	     db->getNodes()->savePl(params->outPlFile);
			      
	   if(params->saveCapoPl)
	     db->getNodes()->saveCapoPl(params->capoPlFile);

	   if(params->saveCapo)
	     db->saveCapo(params->capoBaseFile, params->reqdAR);
	     
           cout<<"Final Area: "<<currArea<<" WhiteSpace "<<currWS<<"%"
	       <<" currAR "<<currAR<<" HPWL "<<currWL<<endl;
	   cout<<"Time Taken  "<<totalTime<<endl;
	  
	return 0;
}
