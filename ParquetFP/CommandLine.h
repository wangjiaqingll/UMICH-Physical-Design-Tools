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









#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H
#include <string>

namespace parquetfp
{
class Command_Line
{
	public:
		Command_Line (int argc, const char *argv[]);
		Command_Line ();
		
		// public member variables to store params
		bool getSeed, budgetTime;
		bool softBlocks;
		bool initQP;     //initialize a QP soln
		char inFileName[200];
		char outPlFile [200];
		char capoPlFile [200];
		char capoBaseFile [200];
		char baseFile [200];

		int seed;      //fixed seed
		int iterations;//number of runs
		int maxIterHier;//max # iterations during hierarchical flow

		double seconds;
                bool plot;             //plot to out.gpl
		bool plotNoNets;       //do not plot nets 
		bool plotNoSlacks;     //do not plot slacks
		bool plotNoNames;      //do not plot names
                bool savePl;           //save .pl file as output
		bool saveCapoPl;       //save .pl file in Capo format
		bool saveCapo;         //save files in Capo format
		bool save;             //save files in bookshelf format

		bool takePl;           //takes a Placement and converts it 
				       //to sequence pair for initial soln
		//Parameters for 2-level hierarchy
		bool solveMulti;       //use multilevel heirarchy
		bool clusterPhysical;  //use physical hierarchy instead of 
		                       //closest node
		bool solveTop;         //solve only top level of heirarchy
		bool compact;          //compact final soln
		double maxWSHier;      //maximum WS for heirarchical blocks
		                       //default 15%
		bool usePhyLocHier;    //use physical locs which updating 
		   //locs of sub-blocks of clustered blocks, if using physical 
		   //clustering. usefull for eco purposes
		bool dontClusterMacros; //keep macros out of clustering
		                        //default false
		int maxTopLevelNodes; //number of top-level nodes required during clustering. if -9999 then use sqrt(#nodes)

		//Annealer performance parameters next
		double timeInit;     //initial temperature default 30000
		double timeCool;     //cooling temperature default 0.01
		double startTime;    //default to timeInit
		           //(this is the time the area & WL are normalized to)
		double reqdAR;       //required Aspect Ratio of fixed outline
				     //default -9999(means no fixed outline
				     //desired)
		double maxWS;        //if fixed-outline then maximum whitespace
		                     //acceptable
		bool minWL;          //whether HPWL minimization desired
		double areaWeight;   //weight for area minimization
		double wireWeight;   //weight for WL minimization
		bool initCompact;    //whether to use compaction to generate
		                     //initial SP
		
		bool useFastSP;      //whether to use fast SP(O(nlog n)) algo 
		                     //for SPEval
		
	public:
		// Print usage info. to cerr
		void printHelp ();
		//print the Annealer params
		void printAnnealerParams();
		
};
}
//using namespace parquetfp;
#endif // COMMAND_LINE_H
