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















// Created: 98/02/15 by Andrew Caldwell (caldwell@cs.ucla.edu)

//CHANGES
//980215 aec created capoParams.cxx from capoPlacer.cxx
//980313 ilm added CapoPlacer::Parameters ctor from argc, argv
//           reworked CapoPlacer::Parameters and its op<<
//980325 ilm added n,m and partFuzziness to CapoPlacer::Parameters
//980401 ilm added savePartProb parameter

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "ABKCommon/abkcommon.h"
#include "capoParams.h"

void CapoParameters::setDefaults()
{
    verb=Verbosity("1 1 1");

    stopAtBlocks=0;	
    saveAtBlocks=0;
    mlParams.netThreshold = 0;//don't threshold

    plotBlocks        =false;
    boost=false;
    saveLayerBBoxes   =false;
    saveBlocks        =false;
    saveSmallPlProb   =false;
    saveBlocksFloorplan =false;

    smallPartThreshold  = 35;
    smallPlaceThreshold = 7;
    smallSplitThreshold = 200;

    boostLayer = 6;
    boostFactor = 2;

    doOverlapping         = false;
    startOverlappingLayer = 0;
    endOverlappingLayer   = UINT_MAX;

    replaceSmallBlocks = AtTheEnd;
    useActualPinLocs   = true;

    tdCongestionCtl    = false;
    capoNE             = false;
    noCOG              = false;
    useQuad            = false;
    quadSkipNetsLargerThan = UINT_MAX;

    lookAhead          = 0;
    allowRowSplits     = false;
    propFromPl	       = false;
    useKPl	       = false;
    useGrpConstr       = false;
}

CapoParameters::CapoParameters()
	: verb("1 1 1")
{  setDefaults(); }

CapoParameters::CapoParameters(const CapoParameters &srcParams)
	:verb(srcParams.verb),
	 stopAtBlocks(srcParams.stopAtBlocks), 
	 saveAtBlocks(srcParams.saveAtBlocks), 
	 replaceSmallBlocks(srcParams.replaceSmallBlocks),
	 useActualPinLocs(srcParams.useActualPinLocs),
	 smallPartThreshold(srcParams.smallPartThreshold),
	 smallPlaceThreshold(srcParams.smallPlaceThreshold),
         smallSplitThreshold(srcParams.smallSplitThreshold),
	 doOverlapping(srcParams.doOverlapping),
	 startOverlappingLayer(srcParams.startOverlappingLayer),
	 endOverlappingLayer(srcParams.endOverlappingLayer),
	 boostLayer(srcParams.boostLayer),
	 boostFactor(srcParams.boostFactor),
	 propFromPl(srcParams.propFromPl),
	 useKPl(srcParams.useKPl),
	 useGrpConstr(srcParams.useGrpConstr),
	 mlParams(srcParams.mlParams), 
	 smplParams(srcParams.smplParams),
	 tdCongestionCtl(srcParams.tdCongestionCtl),
	 capoNE(srcParams.capoNE),
	 noCOG(srcParams.noCOG),
	 useQuad(srcParams.useQuad),
	 quadSkipNetsLargerThan(srcParams.quadSkipNetsLargerThan),
	 lookAhead(srcParams.lookAhead),
	 allowRowSplits(srcParams.allowRowSplits),
	 splitterParams(srcParams.splitterParams),
         plotBlocks(srcParams.plotBlocks),
	 boost(srcParams.boost),
	 saveLayerBBoxes(srcParams.saveLayerBBoxes),
	 saveBlocks(srcParams.saveBlocks),
         saveSmallPlProb(srcParams.saveSmallPlProb),
	 saveBlocksFloorplan(srcParams.saveBlocksFloorplan)		  
{}

CapoParameters::CapoParameters(int argc, const char *argv[])
	:mlParams(argc,argv), smplParams(argc,argv),
	 splitterParams(argc, argv)
{   
  setDefaults();

  BoolParam      helpRequest0   ("help",  argc, argv);
  BoolParam      helpRequest1   ("h",     argc, argv);

  if (helpRequest0.found() || helpRequest1.found() )
  {  printHelp(); return; }

//----------------------------------------------------------------

  StringParam    verb_          ("verb",         argc, argv);

  //general Capo params..
  UnsignedParam  stopAtBlocks_  ("stopAtBlocks", argc, argv);
  UnsignedParam  saveAtBlocks_  ("saveAtBlocks", argc, argv);
  UnsignedParam  smallPartThreshold_("smallPartThreshold", argc, argv);
  UnsignedParam  smallPlaceThreshold_("smallPlaceThreshold", argc, argv);
  UnsignedParam  smallSplitThreshold_("smallSplitThreshold", argc, argv);
  UnsignedParam  boostLayer_("boostLayer", argc, argv);
  UnsignedParam  boostFactor_("boostFactor", argc, argv);
  StringParam    replaceSmallBlocks_("replaceSmallBlocks",   argc, argv);
  BoolParam      useActualPinLocs_("useActualPinLocs", argc,argv);
  BoolParam      tdCongestionCtl_("tdCongestionCtl",argc,argv);
  BoolParam      capoNE_("capoNE",argc,argv);
  BoolParam      noCOG_("noCOG",argc,argv);
  BoolParam      useQuad_("useQuad",argc,argv);
  UnsignedParam  quadSkipNetsLargerThan_("quadSkipNetsLargerThan",argc,argv);

  //Partitioner params...
  UnsignedParam  lookAhead_      ("lookAhead", argc, argv);
  BoolParam      propFromPl_     ("propFromPl",     argc, argv);              
  BoolParam      useKPl_         ("useKPl",     argc, argv);
  BoolParam      allowRowSplits_ ("allowRowSplits", argc, argv);

  //Optional info-tracking params
  BoolParam	plotBlocks_       ("plotBlocks",       argc, argv);
  BoolParam	boost_       ("boost",       argc, argv);
  BoolParam	saveLayerBBoxes_  ("saveLayerBBoxes",  argc, argv);
  BoolParam     saveBlocks_       ("saveBlocks",       argc, argv);
  BoolParam     saveSmallPlProb_  ("saveSmallPlProb",  argc, argv);
  BoolParam     saveBlocksFloorplan_ ("saveBlocksFloorplan",       argc, argv);

  //Overlapping Parameters
  BoolParam     doOverlapping_  ("doOverlapping",argc,argv);
  UnsignedParam startOverlappingLayer_("startOverlappingLayer",argc,argv);
  UnsignedParam endOverlappingLayer_  ("endOverlappingLayer",argc,argv);

  //To handle group constraints in global placement
  BoolParam     useGrpConstr_     ("useGrpConstr", argc, argv);

//----------------------------------------------------------------

  if(smallPartThreshold_.found())
	smallPartThreshold = smallPartThreshold_;
  if(smallPlaceThreshold_.found())
	smallPlaceThreshold = smallPlaceThreshold_;
  if(smallSplitThreshold_.found())
        smallSplitThreshold = smallSplitThreshold_;
  if(boostLayer_.found()) 
    boostLayer = boostLayer_;
  if(boostFactor_.found()) 
    boostFactor = boostFactor_;

  if(replaceSmallBlocks_.found())
  {
      if(!strcasecmp(replaceSmallBlocks_,"Never"))
	replaceSmallBlocks = Never;
      else if(!strcasecmp(replaceSmallBlocks_,"AtTheEnd"))
	replaceSmallBlocks = AtTheEnd;
      else if(!strcasecmp(replaceSmallBlocks_,"AtEveryLayer"))
	replaceSmallBlocks = AtEveryLayer;
      else
	abkfatal(0,"invalid string follows '-replaceSmallBlocks'");
  }

  if (useActualPinLocs_.found())
	useActualPinLocs = true;

  if (verb_.found()) verb=Verbosity(verb_);

  if (stopAtBlocks_.found()) stopAtBlocks=stopAtBlocks_;
  if (saveAtBlocks_.found()) saveAtBlocks=saveAtBlocks_;

  if(propFromPl_.found()) propFromPl = true;
  if(useKPl_.found())     useKPl     = true;
  if(useGrpConstr_.found()) useGrpConstr = true;

  plotBlocks        = plotBlocks_.found();
  boost = boost_.found();
  saveLayerBBoxes   = saveLayerBBoxes_.found();
  saveBlocks        = saveBlocks_.found();
  saveSmallPlProb   = saveSmallPlProb_.found();
  saveBlocksFloorplan = saveBlocksFloorplan_.found();

  if(doOverlapping_.found())
	doOverlapping = true;
  if(startOverlappingLayer_.found())
	startOverlappingLayer = startOverlappingLayer_;
  if(endOverlappingLayer_.found())
      	endOverlappingLayer = endOverlappingLayer_;

  if(tdCongestionCtl_.found())
	tdCongestionCtl = tdCongestionCtl_;

  if(capoNE_.found())
	capoNE = capoNE_;

  if(noCOG_.found())
    noCOG = noCOG_;

  if(useQuad_.found())
    useQuad = useQuad_;

  if(quadSkipNetsLargerThan_.found())
    quadSkipNetsLargerThan = quadSkipNetsLargerThan_;

  if(lookAhead_.found())
	lookAhead = lookAhead_;

  if(allowRowSplits_.found())
	allowRowSplits = true;
}

ostream& operator<<(ostream& os, CapoParameters &params)
{
    static const char *tfStr[2] = {"false", "true"} ;

    os<<"---- Capo Parameters     "<<endl<<endl;

    os<<" StopAtBlocks:  ";
    if(params.stopAtBlocks)
        os<< params.stopAtBlocks      <<endl;
    else
	os  <<" complete placement"<<endl;
    os<<" SaveAtBlocks:  ";
    if(params.saveAtBlocks)
        os<< params.saveAtBlocks      <<endl;
    else
	os  <<" no block save"<<endl;

    os<<" SmallPartThreshold:  " << params.smallPartThreshold<<endl;
    os<<" SmallPlaceThreshold: " << params.smallPlaceThreshold<<endl;
    os<<" SmallSplitThreshold: " << params.smallSplitThreshold<<endl;

    os<<" ReplaceSmallBlocks:  ";
      if(params.replaceSmallBlocks == Never)
	os  <<"Never"<<endl;
      else if(params.replaceSmallBlocks == AtTheEnd)
	os  <<"AtTheEnd"<<endl;
      else if(params.replaceSmallBlocks == AtEveryLayer)
	os  <<"AtEveryLayer"<<endl;
      else 
	abkfatal(0,"invalid value for replaceSmallBlocks");

    os<<" Use Actual Pin Locations: "<<tfStr[params.useActualPinLocs]<<endl;
  
     os  <<" Propagate terminals from globally placed locs: "
	<<tfStr[params.propFromPl]<<endl;

     os  <<" Use (KPlacer)BoxPlacer for splitting: "
	<<tfStr[params.useKPl]<<endl;

    os<<" Allow RowSplits :  "<<tfStr[params.allowRowSplits]<<endl;

    os<<" Top Down Congestion Control :  "<<tfStr[params.tdCongestionCtl]<<endl;
    os<<" Capo Net Effect :  "<<tfStr[params.capoNE]<<endl;

    os<<" No Center of Gravity :  "<<tfStr[params.noCOG]<<endl;

    os<<" Use Quadratic Solution :  "<<tfStr[params.useQuad]<<endl;

    os<<" In SOR SkipNetsLargerThan : "<<params.quadSkipNetsLargerThan<<endl;

    os<<" Use Boosting :         " << tfStr[params.boost] <<endl;
    os<<"     Boosting Layer :         " << params.boostLayer <<endl;
    os<<"     Boosting Factor :         " << params.boostFactor <<endl;

    os<<endl<<"----Block Splitting Parameters----"<<endl;
    os<<" LookAhead       :  "<< params.lookAhead<<endl;
    os<<params.splitterParams<<endl;

    os<<endl<<"----Overlapping Parameters----"<<endl;
    os<<" Do Overlapping:      " << tfStr[params.doOverlapping]<<endl;
    if(params.doOverlapping)
    {
	if(params.startOverlappingLayer != 0)
	    os<<" StartOverlappingLayer:    "<<
			params.startOverlappingLayer<<endl;
	if(params.endOverlappingLayer != UINT_MAX)
	    os<<" EndOverlappingLayer:      "<<
			params.endOverlappingLayer<<endl;
    }
    os<<" Honour Group  Constraints:    " <<tfStr[params.useGrpConstr]<<endl;

    os<<endl<<"----Small Placer Parameters----"<<endl;
    os<<params.smplParams<<endl;

    os<<endl<<"----Data Tracking Parameters----"<<endl<<endl;
    os<<" Plot blocks:         " << tfStr[params.plotBlocks     ] <<endl;
    os<<" Save Layer BBoxes:   " << tfStr[params.saveLayerBBoxes] <<endl;
    os<<" Save Blocks:         " << tfStr[params.saveBlocks     ] <<endl;
    os<<" Save Small Pl. Prob  " << tfStr[params.saveSmallPlProb] <<endl;
    os<<" Save blocks in floorplan format:         " 
      << tfStr[params.saveBlocksFloorplan] <<endl;

    os<<endl<<params.mlParams;

    return os;
}

void CapoParameters::printHelp()
{
     cout << " ******* CAPO PARAMETERS HELP ********* " << endl
          << "== Top-down parameters == \n"
          << "  -verb  Verbosity \n"
          << "  -stopAtBlocks #  \n"
          << "  -saveAtBlocks #  \n"
	  << "  -smallPartThreshold  <unsigned> \n"
	  << "  -smallPlaceThreshold <unsigned> \n"
          << "  -smallSplitThreshold <unsigned> \n"
	  << "  -replaceSmallBlocks <Never|AtTheEnd|AtEveryLayer> \n"
	  << "  -useActualPinLocs   \n"
	  << "  -tdCongestionCtl   \n"
	  << "  -capoNE   \n"
          << "  -noCOG    \n"
	  << "  -useQuad  \n"
          << "  -quadSkipNetsLargerThan <unsigned> \n\n"

	  << "== Block Splitting Parameters =="<<endl
          << "  -repart          <bool>         \n"
          << "  -constTol        <double %>[10] \n"
          << "  -nonUniformWS    <bool> \n"
	  << "  -branchingFactor <unsigned> [2] \n"
	  << "  -useNameHier     <delimiting char> [none] \n"
          << "  -lookAhead       <unsigned [0]  \n"
	  << "  -propFromPl      <bool [false]> \n"
	  << "  -useKPl          <bool [false]> \n"
	  << "  -noRowSplits             <bool> \n\n"
          << "  -noRepartSmallWS   <bool>         \n"
          << "  -useQuadCluster  <bool>         \n"
	  << "  -boost             <bool> [false] \n"
          << "  -boostLayer        <unsigned> [6] \n"
          << "  -boostFactor        <unsigned> [2] \n"
 
	  << "== Optional info-tracking parameters == \n"
	  << "  -plotBlocks        <bool> [false] \n"
          << "  -saveLayerBBoxes   <bool> [false] \n"
          << "  -saveBlocks        <bool> [false] \n"
          << "  -saveSmallPlProb   <bool> [false] \n"
          << "  -saveBlocksFloorplan <bool> [false] \n\n"

	  << "== Overlapping parameters == \n"
          << "  -doOverlapping \n"
          << "  -startOverlappingLayer <unsigned> \n"
	  << "  -endOverlappingLayer <unsigned> \n\n"

	  << "== Constraints parameters == \n"
	  << "  -useRgnConstr \n"
	  << "  -useGrpConstr \n"
	  << "  -fillerCellsPartCaps  \n\n"
//        << "== Small Placer Parameters == \n"
//	  << smplParams
	
	  <<endl;
}


// ---------------------------------------------------
// implementation of CapoSplitterParams

CapoSplitterParams::CapoSplitterParams(int argc, const char *argv[])
        : doRepartitioning(false), repartSmallWS(true), useQuadCluster(false),
	  useWSTolMethod(true), constantTolerance(20), uniformWS(true),
	  numMLSets(2), useNameHierarchy(false), delimiters((char*)"/"), 
	  useRgnConstr(false), fillerCellsPartCaps(false), verb(argc,argv)
{
   BoolParam     repart_("repart", argc, argv);
   BoolParam     noRepartSmallWS_("noRepartSmallWS", argc, argv);
   BoolParam     useQuadCluster_("useQuadCluster", argc, argv);
   UnsignedParam constTol_("constTol", argc, argv);
   UnsignedParam bFactor_("branchingFactor", argc, argv);
   StringParam   useNameHier_("useNameHier", argc, argv);
   BoolParam     useRgnConstr_("useRgnConstr", argc, argv);
   BoolParam     fillerCellsPartCaps_("fillerCellsPartCaps", argc, argv);
   BoolParam     nonUniformWS_("nonUniformWS", argc, argv);

   if(repart_.found()) doRepartitioning = true;

   if(noRepartSmallWS_.found()) repartSmallWS = false;

   if(useQuadCluster_.found()) useQuadCluster = true;

   if(useRgnConstr_.found()) useRgnConstr = true;
   
   if(fillerCellsPartCaps_.found()) fillerCellsPartCaps = true;

   if(constTol_.found())
   {
        useWSTolMethod =false;
        constantTolerance = constTol_;
   }

   if(nonUniformWS_.found())
     uniformWS = false;

   if(bFactor_.found())
        numMLSets = bFactor_;

   if(useNameHier_.found())
   {
	useNameHierarchy = true;
	delimiters = new char[strlen(useNameHier_)+2];
	strcpy(delimiters, useNameHier_);
   }
}
 
ostream& operator<<(ostream& os, const CapoSplitterParams& params)
{
  const char* tfStr[2];
  tfStr[0] = "false";
  tfStr[1] = "true";
 
  os<<"---Capo Splitter Parameters---"<<endl;
  os<<"  doRepartitioning   "<<tfStr[params.doRepartitioning]<<endl;
  os<<"  noRepartSmallWS   "<<tfStr[!params.repartSmallWS]<<endl;
  if(params.useWSTolMethod)
    os<<"  h-cuts use WS Tolerance Method     "<<endl;
  else
    os<<"  h-cuts use constant tolerance "<<params.constantTolerance
		<<"%"<<endl;
  if(params.uniformWS)
    os<<"  v-cuts use constant tolerance "<<params.constantTolerance<<"%"<<endl;
  else
    os<<"  v-cuts use WS Tolerance Method     "<<endl;

  os<<"  use name hierarchy    "<<tfStr[params.useNameHierarchy]<<endl;
  if(params.useNameHierarchy)
	os<<"   delimiter          "<<params.delimiters<<endl;
  os<<"  respect region utilization constraints "<<tfStr[params.useRgnConstr]
    <<endl;
  os<<"  in presence of filler cells put std-cells in center of die "<<tfStr[params.fillerCellsPartCaps]<<endl;

  return os;
}
