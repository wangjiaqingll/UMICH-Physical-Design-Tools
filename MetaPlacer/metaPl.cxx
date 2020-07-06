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














// Created: Dec 1, 1999 by Igor Markov <imarkov@cs.ucla.edu>
// Changed: Nov 30, 2000 by Saurabh Adya <sadya@umich.edu>

#include <iostream>
using std::cout;
using std::endl;
#include <fstream>
using std::ostream;
using std::ifstream;
#include "metaPl.h"
#include "ABKCommon/abkcommon.h"
#include "DB/DB.h"
#include "Capo/capoPlacer.h"
#include "RBPlFromDB/RBPlaceFromDB.h"
#include "NetTopology/hgWDimsFromDB.h"
#include "FixOr/greedyOrientOpt.h"
#include "RowIroning/rowIroning.h"
#include "Stats/trivStats.h"
#include "Stats/expMins.h"
#include "CongestionMaps/CongestionMaps.h"

MetaPlacerParams::MetaPlacerParams(int argc, const char *argv[])
:verb(argc,argv)
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  UnsignedParam    numParam       ("num", argc,argv);
  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
    cout <<
    " Available MetaPlacer parameters: " << endl <<
    " -verb <1_1_1,1_2_3 etc>     - verbosity\n"
    " -save           - output will be saved into files with base out \n"
    " -saveXorder     - output will be saved into left2right.ord \n"
//  " -flip           - request cell flipping done after placement\n"
    " -noRowIroning   - skip row ironing\n"
    " -num <int> (repeat everything this many times)\n"
    << endl;
    return;
  }

  save             = BoolParam ("save",argc,argv);
  flip             = true; // BoolParam ("flip",argc,argv);
  skipRowIroning   = BoolParam ("noRowIroning",argc,argv);
  saveXorder       = BoolParam ("saveXorder",argc,argv);
  if (numParam.found()) num=numParam;
  else                  num=1;
}

ostream& operator<< (ostream& out, const MetaPlacerParams& params)
{
  const char *trueFalse[2]={"false","true"};
  out << "MetaPlacerParams:"
      << "\n  Verbosity:     " << params.verb 
      << "\n  Save:          " << trueFalse[params.save] 
      << "\n  Cell Flipping: " << trueFalse[params.flip]
      << "\n  Row Ironing:   " << trueFalse[!params.skipRowIroning]
      << endl;
  return out;
}

int MetaPlacer::_greetingEtc(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);

  CapoParameters capoParams(argc,argv);
  RowIroningParameters RIparams(argc, argv);

  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
   cout << "  \nOther options: \n"
        << "  Plotting options: \n"
	<< "   -minx,-maxx,-miny,-maxy  (for required range. Default means all)\n"
	<< "   -plotNets        base_fileName\n"
	<< "   -plotNodes       base_fileName\n"
	<< "   -plotNodesWNames base_fileName\n"
	<< "   -plot            base_fileName   (plots nodes and nets)\n"
	<< "   -plotWNames      base_fileName   (the above + node names)\n"
	<< "   -plotSites	base_fileName   (plots the site map)\n"
        << "   -plotRows        base_fileName   (plot the rows)\n"
	<< "   -plotNodesWSites base_fileName   (the above + nodes)\n"
        << "   -plotCongMap     base_fileName   (congestion map in Matlab format)\n"
        << "   -plotCongMapGP   base_fileName   (congestion map in Gnuplot format)\n"
	<< "   -plotCongMapXPM  base_fileName   (congestion map in XPM format)\n"
	<< "   -skipLegal (Use to remove any overlaps in existing placement)\n"
	<< endl;
     cout << "Usage: prog -f filename.aux (also see -help)\n"<<endl;
   return 1;
  }
  return 0;


}

void MetaPlacer::_workWithRBPlaceNtimes (RBPlacement& rbplace, 
int argc, const char *argv[])
{
     if (_params.num==0) 
     { abkwarn(0," Zero runs specified by -num 0"); return; }
     else 
     if (_params.num==1) _workWithRBPlace(rbplace,argc,argv);
     else
     {
        unsigned k=0;
        for(; k!=_params.num; k++)
        {
            cout << " =========== MetaPlacer run " << k << " (" 
                 << _params.num << ") ==============" << endl;
           _workWithRBPlace(rbplace,argc,argv);
        }

	abkfatal(wls.size(),
        " Not enough data to produce overall results ");
        cout<<"________ Overall Results for " << wls.size() 
            << " experiment" << (wls.size()==1 ? "" : "s")  
            << " ___________ \n" << endl;

        cout<<"** WLs **:"<<TrivialStatsWithStdDev(wls)<<endl;

        cout << "  -------------------------------------------------" 
                "------------------------"
             << endl;
	double totalTime=0;
        for(k=0; k!=_params.num; k++)
	  {
	    cout<< "         "
                << setw(12) << wls[k] << " " << setw(10) << runTs[k] << endl;
	    totalTime += runTs[k];
	  }
	double runtimePer = totalTime/_params.num;
	if(_params.num >= 10)
	  {
	    ExpectedMins sampler(wls);

	    cout<<"___(Sampled)Average Best Of N / Average Runtime___\n";
	    cout<<" Ave Best Of 1: "<<sampler[1]<<"("<<runtimePer<<")"<<endl;
            cout<<" Ave Best Of 2: "<<sampler[2]<<"("<<runtimePer*2<<")"<<endl;
	    if(_params.num >= 20)
	      {
		cout<<" Ave Best Of 3: "<<sampler[3]<<"("<<runtimePer*3<<")"<<endl;
		cout<<" Ave Best Of 4: "<<sampler[4]<<"("<<runtimePer*4<<")"<<endl;
	      }
            if(_params.num >= 30)
	      {
                cout<<" Ave Best Of 5: "<<sampler[5]<<"("<<runtimePer*5<<")"<<endl;
                cout<<" Ave Best Of 6: "<<sampler[6]<<"("<<runtimePer*6<<")"<<endl;
	      }

	    if(_params.num >= 40)
	      {
                cout<<" Ave Best Of 7: "<<sampler[7]<<"("<<runtimePer*7<<")"<<endl;
		cout<<" Ave Best Of 8: "<<sampler[8]<<"("<<runtimePer*8<<")"<<endl;
	      }

            if(_params.num >= 60)
	      {
		cout<<" Ave Best Of 9: "<<sampler[9]<<"("<<runtimePer*9<<")"<<endl;
		cout<<" Ave Best Of 10: "<<sampler[10]<<"("<<runtimePer*10<<")"<<endl;
	      }
	    //cout << SysInfo();
	  }
     }
}

void MetaPlacer::_workWithRBPlace (RBPlacement& rbplace, 
int argc, const char *argv[])
{
 CapoParameters capoParams(argc,argv);
 RowIroningParameters   RIparams(argc, argv); 

 StringParam   plotNets("plotNets", argc, argv);
 StringParam   plotNodes("plotNodes", argc, argv);
 StringParam   plotNodesAndNets("plot", argc, argv);
 StringParam   plotNodesWNames("plotNodesWNames", argc, argv);
 StringParam   plotNodesAndNetsWNames("plotWNames", argc, argv);
 StringParam   plotSites("plotSites", argc, argv);
 StringParam   plotRows("plotRows", argc, argv);
 StringParam   plotNodesWSites("plotNodesWSites", argc, argv);  
 StringParam   plotCongMap("plotCongMap", argc, argv);  
 StringParam   plotCongMapGP("plotCongMapGP", argc, argv);  
 StringParam   plotCongMapXPM("plotCongMapXPM", argc, argv);  

 BoolParam     keepOverlaps    ("skipLegal",argc,argv);
 BoolParam     snapToSites    ("snapToSites",argc,argv);
 DoubleParam xmin("xmin",argc,argv);
 DoubleParam xmax("xmax",argc,argv);
 DoubleParam ymin("ymin",argc,argv);
 DoubleParam ymax("ymax",argc,argv);

 double minx  = ( xmin.found() ?  xmin:-DBL_MAX);
 double maxx  = ( xmax.found() ?  xmax:DBL_MAX);
 double miny  = ( ymin.found() ?  ymin:-DBL_MAX);
 double maxy  = ( ymax.found() ?  ymax:DBL_MAX);

 if (inputTyp==HGraph_)
 {
    abkwarn(0," Adding parameters for linear placement:\n"
    " -noRowIroning   -replaceSmallBlocks  Never  -constTol 20  -noRepart\n");
    _params.skipRowIroning=true;
     capoParams.replaceSmallBlocks=Never;
     capoParams.splitterParams.doRepartitioning=false;
     capoParams.splitterParams.constantTolerance=20;
 }

 cout << capoParams << endl;
 if(!_params.skipRowIroning)
 {
     cout << " Will do row ironing ... " << endl;
     cout << RIparams;
 }

 CongestionMaps *congMap = NULL;
 if(capoParams.tdCongestionCtl || plotCongMapGP.found() || plotCongMap.found()
    || plotCongMapXPM.found())
   {
     congMap = new CongestionMaps(&rbplace);      
     congMap->initializeFMatrix();
     congMap->createCongestionMapProb();
   }

  Timer totalTime;
  Timer capoTimer;
  if(capoParams.tdCongestionCtl)
    CapoPlacer capo(rbplace,capoParams,congMap);
  else
    CapoPlacer capo(rbplace,capoParams);
  capoTimer.stop();

  cout << "After Capo  "<<endl;
  cout << "  Capo Runtime: "<<capoTimer.getUserTime()<<endl;

  cout << "  RBPl Est HPWL: "<<rbplace.evalHPWL(false)<<endl;;
  cout << "  RBPl Est Pin-to-Pin HPWL: "<<rbplace.evalHPWL(true)<<endl;;
  cout << "  RBPl Est  WWL: "<<rbplace.evalWeightedWL(false)<<endl;;
  cout << "  RBPl Est Pin-to-Pin  WWL: "<<rbplace.evalWeightedWL(true)<<endl;;


  if(!keepOverlaps.found() && inputTyp!=HGraph_)
    {
      cout<<"Removing overlaps after Capo "<<endl;
      rbplace.snapCellsInSites();
      rbplace.remOverlaps();
//    rbplace.remOverlapsVert();
    }

  if(snapToSites.found())
    rbplace.snapCellsInSites();

  if(_params.flip)
 {
  if (!_db)  
    abkwarn(0,"Cannot [yet] optimize cell orientation without LEF/DEF input\n")
  else
  { 
      cout << " ====== Optimizing cell orientations ... " << endl;
      _db->setPlaceAndOrient(rbplace);
      OrientOptParameters flipParams;

      Timer flipTimer;
      GreedyOrientOpt flipper(*_db,flipParams);
      flipTimer.stop();

      *_db<<flipper;

      cout<<"After Greedy Flipping"<<endl;

      cout << "  Flip Runtime: "<<flipTimer.getUserTime()<<endl;
      cout << "  Total Cells flipped: "<<flipper.getNumFlips()<<endl;
      cout << "  DB   Est Pin-to-Pin HPWL: "<<
        _db->getNetlist().evalHalfPerimCost(_db->spatial.locations,
                                  _db->spatial.orientations)<<endl<<endl;
      rbplace.resetPlacementWOri(
                PlacementWOrient(_db->spatial.locations,
                                 _db->spatial.orientations));     
  }
 }

  if(!_params.skipRowIroning)
  {
     cout << " ====== Launching row ironing ... " << endl;
     cout << RIparams;
     Timer RITimer;
     ironRows(rbplace,RIparams);
     RITimer.stop();

     cout << "After RowIroning  "<<endl;
     cout << "  RowIroning Runtime: "<<RITimer.getUserTime()<<endl;
     cout << "  RBPl Est HPWL: "<<rbplace.evalHPWL(false)<<endl;
     cout << "  RBPl Est Pin-to-Pin HPWL: "<<rbplace.evalHPWL(true)<<endl;
     cout << "  RBPl Est  WWL: "<<rbplace.evalWeightedWL(false)<<endl;
     cout <<"  RBPl Est Pin-to-Pin  WWL: "<<rbplace.evalWeightedWL(true)<<endl;
  }          

  if(snapToSites.found())
    rbplace.snapCellsInSites();

  if(!keepOverlaps.found() && inputTyp!=HGraph_)
    {
      cout<<"Removing overlaps after RowIroning "<<endl;
      rbplace.snapCellsInSites();
      rbplace.remOverlaps();
    }

  pair<double, double> HPWL = rbplace.evalXYHPWL(false);
  cout<<"Final HPWL"<<endl;
  cout<<"  Center-to-center HPWL: "<<HPWL.first+HPWL.second<<" ("<<HPWL.first<<" x , "<<HPWL.second<<" y )"<<endl;
  HPWL = rbplace.evalXYHPWL(true);
  cout<<"  Pin-to-pin HPWL      : "<<HPWL.first+HPWL.second<<" ("<<HPWL.first<<" x , "<<HPWL.second<<" y )"<<endl;

  totalTime.stop();

  bool allCellsInCoreRgn = rbplace.checkOutOfCoreCells();
  if(!allCellsInCoreRgn)
    {
      //if(!keepOverlaps.found())      
      //abkfatal(0,"some core cells not placed in core region");
    }

  static double bestWL=DBL_MAX;

  double curWL=rbplace.evalHPWL(true);

  wls.push_back(curWL);
  runTs.push_back(totalTime.getUserTime());

  if(_params.save)
  {
    bool bestSeen=false;

    static unsigned num=0;
    char fileName[64];
    num++;
    
    if(curWL < bestWL)
      bestSeen = true;

    if (_db) 
      {
        sprintf(fileName,"out-%d.def",num);
	_db->setPlaceAndOrient(rbplace);
	if(_params.num != 1)
	  {
	    cout << "Saving " << fileName << " ... " << flush;
	    _db->saveDEF(fileName);
	  }
        if (bestSeen) _db->saveDEF("out.def");
       }
    sprintf(fileName,"out-%d.pl",num);
    if(_params.num != 1)
      {
	cout << "Saving " << fileName << " ... " << flush;
	rbplace.savePlacement(fileName);
      }
    if (bestSeen) rbplace.savePlacement("out.pl");
    cout << "ok" << endl;
  }

  if (curWL<bestWL)   bestWL=curWL;

  if (_params.saveXorder)
  {
    unsigned idx=0, numCells=rbplace.getNumCells();
    vector<unsigned> toSort(numCells);
    for(;idx!=numCells;idx++) toSort[idx]=idx;
    CompareCellIdsByX byX(rbplace);
    std::sort(toSort.begin(),toSort.end(),byX);
    const HGraphWDimensions& hgr=rbplace.getHGraph();
    ofstream of("left2right.ord");
    cout << "Saving left2right.ord ... "<< flush;
    for(idx=0;idx!=numCells;idx++)
       of << setw(12) << hgr.getNodeByIdx(toSort[idx]).getName() << endl;
    cout << "ok" << endl;
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

   if(plotRows.found())
     rbplace.saveAsPlot("plotRows",minx,maxx,miny,maxy,plotRows);
     
   if(plotCongMap.found() || plotCongMapGP.found() || plotCongMapXPM.found())
     congMap->createCongestionMapProb();

   if(plotCongMap.found())
     congMap->plotMatCongMap(plotCongMap);

   if(plotCongMapGP.found())
     congMap->plotCongMap(plotCongMapGP);

   if(plotCongMapXPM.found())
     congMap->plotXPMCongMap(plotCongMapXPM);

   
   if(plotCongMap.found() || plotCongMapGP.found() || plotCongMapXPM.found())
     congMap->printCongestionStats();
   
  delete congMap;

  return;
}

MetaPlacer::MetaPlacer(int argc, const char *argv[])
:_params(argc,argv),_db(NULL)
{
 if (_greetingEtc(argc,argv)) return;	

 StringParam    auxFileName    ("f",   argc,argv);
 abkfatal(auxFileName.found(),"Usage: prog -f filename.aux \n"
                              " See -help for more params");
 //  figure out what kind of AUX file this is
 //  Three cases are possible: DB, RBPlacement and Hgraph (for linear pl)
 
 ifstream ifs(auxFileName);
 abkfatal3(ifs,"Cannot open ",auxFileName,"\n "); 
 unsigned linesRead=0;
 while (!ifs.eof())
 {  
    if (linesRead>=10)
    {
       cerr << "\n No recognized format spec in first 10 lines of file\n    "
            << auxFileName << endl;
       cerr << " Currently supported formats are LEFDEF*, RowBasedPlacement and HGraph*"
            << endl;
       abkfatal(0," ");
    }
    char buf[1000];
    ifs >> buf;
    ifs >> skiptoeol;
    linesRead++;
    if (buf[0]=='#') continue;
    buf[999]='\0';
    if (strstr(buf,"LEFDEF") || strstr(buf,"lefdef"))
	 { // case 1: DB
           cout << " Got LEF/DEF on input ..." << endl;
           inputTyp=DB_;
	   _db=new DB(auxFileName);
	   cout<<"Design Name: "<<_db->getDesignName()<<endl;
	   RBPlaceParams rbParam(argc,argv);
	   if(rbParam.remCongestion)
	     _db->routedNets.readLEFDEF(_db->getNetlist(), _db->getLayout(), 
				       auxFileName);
	   RBPlaceFromDB rbplace(*_db, rbParam); 
	   _workWithRBPlaceNtimes(rbplace,argc,argv); 
	   return;
	 }
     else
     if (!strcasecmp(buf,"RowBasedPlacement") || 
         !strcasecmp(buf,"RBPlace") )
	 { // case 2: RBPlace
           cout << " Got RowBasedPlacement on input ..." << endl;
           inputTyp=RBPlace_;
	   RBPlaceParams rbParam(argc,argv);
	   RBPlacement rbplace(auxFileName, rbParam);   
	   _db=NULL;
	   _workWithRBPlaceNtimes(rbplace,argc,argv);
	   return; 
	 }
     else if (strstr(buf,"raph") || strstr(buf,"RAPH"))
	 { // case 3: HGraph
           cout << " Got HGraph on input ..." << endl;
           inputTyp=HGraph_;
	   RBPlaceParams rbParam(argc,argv);
	   RBPlacement rbplace(auxFileName, 0.0, 0.0, rbParam);   
//         cout << "Saving intermed.pl ..." << flush;
//         rbplace.savePlacement("intermed.pl");
	   _db=NULL;
	   _workWithRBPlaceNtimes(rbplace,argc,argv);
	   return; 
	 }
   }
  cerr << "\n No recognized format spec in first 10 lines of file\n    "
       << auxFileName << endl;
  cerr << " Currently supported formats are LEFDEF* and RowBasedPlacement"
       << endl;
  abkfatal(0," ");
}

MetaPlacer::MetaPlacer(RBPlacement& rbplace, int argc, const char *argv[])
:_params(argc,argv),_db(NULL)
{
 _workWithRBPlace(rbplace,argc,argv); 
}

MetaPlacer::~MetaPlacer()
{
  if (_db) delete _db;
  _db=NULL;
}
