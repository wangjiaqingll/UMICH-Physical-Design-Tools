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
	   double currArea;
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
	       cerr<<"This test case test analytical techniques\n"
		   <<"Options \n"
		   <<"FPEvalTest3.exe\n"
		   <<"-f baseFileName\n"
		   <<"-initQP   (initial SP is constructed from quadratic min WL)\n"
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
	   annealer.solveQP();
	   if(params->initQP)
	    {
	     annealer.takeSPfromDB();
	     annealer.eval();
	     
	     currXSize = annealer.getXSize();
	     currYSize = annealer.getYSize();
	     currArea = currXSize*currYSize;
	    }
	   else
	    {
              currXSize = db->getXSize();
	      currYSize = db->getYSize();
	      currArea = db->evalArea();
	    }
						       
	   currWS = 100*(currArea - blocksArea)/currArea;
	   currWL = db->evalHPWL();
	   
	   if(params->plot)
	    {
	      currAR = currXSize/currYSize;
              bool plotSlacks = !params->plotNoSlacks;
              bool plotNets = !params->plotNoNets;
	      bool plotNames = !params->plotNoNames;
              db->plot("out.gpl", currArea, currWS, currAR, 0, 
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

           cout<<"Final Area: "<<currArea<<" WhiteSpace "<<currWS<<"%"
	       <<" currAR "<<currAR<<" HPWL "<<currWL<<endl;
	  
	return 0;
}
