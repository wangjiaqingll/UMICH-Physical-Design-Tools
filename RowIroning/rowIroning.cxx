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




#include "rowIroning.h"

#include <algorithm>
#include <iostream>
#include "HGraphWDims/hgWDims.h"
#include "RBPlace/RBPlacement.h"
#include "RBPlace/pinLocCalc.h"
#include "SmallPlacement/smallPlaceProb.h"
#include "SmallPlacers/sRowBBPlacer.h"
#include "SmallPlacers/sRowSmPlacer.h"

void ironRows(RBPlacement& rbplace,
              RowIroningParameters params)
{
  Timer totalTime;

  // check that populated ?
  // check params

  abkfatal(params.windowSize>1 && params.windowSize<20,
            " Row ironing window size must be >1 and <20");
  abkfatal(                    params.overlap<params.windowSize,
            " Row ironing overlap must be        smaller than window size\n");
  abkfatal(params.numPasses<20, "Row ironing cannot have 20+ passes");

  double initHPWL = rbplace.evalHPWL(true);
  for(unsigned pass=0; pass<params.numPasses; pass++)
    {
      Timer passTime;
      bool twoDim = 0;
      if(params.twoDim)
	twoDim = 1;

      bool cluster = params.cluster;
      
      if(params.mixed && !params.twoDim)
	{
	  if((pass%8)%2 == 0)
	    twoDim = 1;
	  else
	    twoDim = 0;

	  switch(pass%8)
	    {
	    case 0:
	    case 1:
	      ironRows2DLR(rbplace, params, 0, twoDim, cluster);
	      break;
	    case 2:
	    case 3:
	      ironRows2DLR(rbplace, params, 1, twoDim, cluster);
	      break;
	    case 4:
	    case 5:
	      ironRows2DRL(rbplace, params, 0, twoDim, cluster);
	      break;
	    case 6:
	    case 7:
	      ironRows2DRL(rbplace, params, 1, twoDim, cluster);
	      break;
	    }
	}
      else
	{
	  if(pass%4 == 0)
	    ironRows2DLR(rbplace, params, 0, twoDim, cluster);
	  else if(pass%4 == 1)
	    ironRows2DLR(rbplace, params, 1, twoDim, cluster);
	  else if(pass%4 == 2)
	    ironRows2DRL(rbplace, params, 0, twoDim, cluster);
	  else
	    ironRows2DRL(rbplace, params, 1, twoDim, cluster);
	}
      passTime.stop();
      if (params.verb.forMajStats || params.verb.forSysRes )
	{
	  cout << " Pass " << pass << " took : " << passTime << endl;
	  cout << "  Pin-to-Pin HPWL after pass: "
	       <<rbplace.evalHPWL(true)<<endl;
	}
    }
  totalTime.stop();
  double finalHPWL = rbplace.evalHPWL(true);
  
  if (params.verb.forMajStats || params.verb.forSysRes )
    {
      cout << " Row ironing took : " << totalTime << endl;
      cout << " % improvement in HPWL : "<<100*(initHPWL-finalHPWL)/initHPWL
	   <<endl;
    }

  //rbplace.snapCellsInSites();
  //rbplace.remOverlaps();
}


RowIroningParameters::RowIroningParameters(int argc, const char *argv[])
 : numPasses(2), windowSize(7), overlap(3), twoDim(0), mixed(0), cluster(0),
   smplParams(argc,argv), verb(argc,argv)
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  UnsignedParam numPasses_ ("ironPasses",argc,argv);
  UnsignedParam windowSize_("ironWindow",argc,argv);
  UnsignedParam overlap_   ("ironOverlap",argc,argv);
  BoolParam     twoDim_    ("ironTwoDim",argc,argv);
  BoolParam     mixed_     ("ironMixed",argc,argv);
  BoolParam     cluster_   ("ironCluster",argc,argv);
  
  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
     cout << " Row Ironing Parameters: " 
          << "\n   -ironPasses <unsigned> "
          << "\n   -ironWindow <unsigned> "
          << "\n   -ironOverlap <unsigned>" 
	  << "\n   -ironTwoDim "
	  << "\n   -ironMixed    (1D and 2D)"
	  << "\n   -ironCluster "<<endl;
     return;
  }
  if (numPasses_. found())  numPasses =numPasses_;
  if (windowSize_.found())  windowSize=windowSize_;
  if (overlap_.   found())  overlap   =overlap_;
  if (twoDim_.found()) twoDim = 1;
  if (mixed_.found()) mixed = 1;
  if (cluster_.found()) cluster = 1;
  
  smplParams.verb = verb;
  smplParams.verb.forActions /= 10;
  smplParams.verb.forMajStats/= 10;
  smplParams.verb.forSysRes  /= 10;
}

ostream& operator<<(ostream& out, const RowIroningParameters& params)
{
   out << " Row ironing paramerers : "
       << "\n   Num. passes    : " << params.numPasses 
       << "\n   Window size    : " << params.windowSize
       << "\n   Overlap        : " << params.overlap
       << "\n   Two Dimensional: " << params.twoDim
       << "\n   Mixed(1D&2D)   : " << params.mixed
       << "\n   Cluster        : " << params.cluster
       << "\n   Verbosity      : " << params.verb << endl;
   
  return out;
}

//to iron in two dimension left to right
void ironRows2DLR(RBPlacement& rbplace,
		  RowIroningParameters params,bool verticalDir, bool twoDim,
		  bool cluster)
{
  const HGraphWDimensions& hgWDims = rbplace.getHGraph();
  bit_vector placed = bit_vector(rbplace.getNumCells(),1);

  char fileName1[15];
  char fileName2[15];
  char fileName3[15];
  char fileName4[15];
  unsigned instance = 0;

  //verticalDir=0 means bottom up pass else
  //top down pass
   itRBPCoreRow rIt1,rIt2;
   itRBPSubRow sIt1,sIt2;

   if(verticalDir == 0)
     rIt1=rbplace.coreRowsBegin();
   else
     rIt1=rbplace.coreRowsEnd()-1;

   while(true)                 //exit only by a break
   {
     unsigned windowSize1=params.windowSize;
     unsigned windowSize2=0;
     rIt2 = rIt1;
     if(verticalDir == 0 && twoDim == 1)
       {
	 if(rIt1 != rbplace.coreRowsEnd()-1)   //if not last core row
	   {
	     ++rIt2;
	     windowSize1 = unsigned(ceil(params.windowSize/2));
	     windowSize2 = params.windowSize - windowSize1;
	   }
       }
     else
       {
	 if(rIt1 != rbplace.coreRowsBegin() && twoDim == 1) 
	                                      //if not first Row
	   {
	     --rIt2;
	     windowSize1 = unsigned(ceil(params.windowSize/2));
	     windowSize2 = params.windowSize - windowSize1;
	   }
       }

     sIt1=rIt1->subRowsBegin();
     sIt2=rIt2->subRowsBegin();

     while(sIt1 != rIt1->subRowsEnd() && sIt2 != rIt2->subRowsEnd())
     {
       const RBPSubRow& subrow1=*sIt1;
       const RBPSubRow& subrow2=*sIt2;

       sIt1++;
       sIt2++;

       SmallPlacementRow smallRow1;
       SmallPlacementRow smallRow2;

       smallRow1.siteInterval=subrow1.getSpacing();
       smallRow1.xMax =smallRow1.xMin=subrow1.getXStart();
       smallRow1.yMax =smallRow1.yMin=subrow1.getYCoord();
       smallRow1.yMax+=subrow1.getHeight();
       unsigned firstCell1=0;
       unsigned totalCells1=subrow1.getNumCells();
       unsigned currCellLoc1=0;
       unsigned nodeCtr1=0;

       smallRow2.siteInterval=subrow2.getSpacing();
       smallRow2.xMax =smallRow2.xMin=subrow2.getXStart();
       smallRow2.yMax =smallRow2.yMin=subrow2.getYCoord();
       smallRow2.yMax+=subrow2.getHeight();
       unsigned firstCell2=0;
       unsigned totalCells2=subrow2.getNumCells();
       unsigned currCellLoc2=0;
       unsigned nodeCtr2=0;

       int numSitesWS=0;
       int sitesPerWS=0;

       if(totalCells1 < 1 || totalCells2 < 1) continue;

       //now start forming instances
       while (firstCell1 < totalCells1)
	 {   
	   ++instance;
	   sprintf(fileName1,"smPbIn-%d",instance);
	   sprintf(fileName2,"smPbOut-%d",instance);
	   sprintf(fileName3,"rbPbIn-%d",instance);
	   sprintf(fileName4,"rbPbOut-%d",instance);

	   vector<SmallPlacementRow> smallPbRows;
	   //form the 1'st row first
	   //get the cells + whitespace cells here
	   vector< vector<unsigned> > movables;
	   vector<double> whiteSpaceWidths;
	   vector<Point> whiteSpaceLocs;
	   unsigned whiteSpaceCtr=0;
	   unsigned totalMovables1=0;
	   unsigned totalMovables2=0;
	   unsigned row1Size=0;

	   bool brokeFromLoop = 0;
	   if(firstCell1 < totalCells1)
	     {
	       for(nodeCtr2=0;nodeCtr2<totalCells2-1;++nodeCtr2)
		 {
		   if((rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x <=
		       rbplace[*(subrow1.cellIdsBegin()+firstCell1)].x) &&
		      (rbplace[*(subrow2.cellIdsBegin()+nodeCtr2+1)].x >
		       rbplace[*(subrow1.cellIdsBegin()+firstCell1)].x))
		     {
		       brokeFromLoop = 1;
		       break;
		     }
		 }
	       if(!brokeFromLoop)
		 nodeCtr2 = totalCells2-1;
	       firstCell2 = nodeCtr2;
	     }
	   else
	     firstCell2 = totalCells2;

	   nodeCtr1=firstCell1;
	   nodeCtr2=firstCell2;

	   smallRow1.xMax = smallRow1.xMin = 
	     rbplace[*(subrow1.cellIdsBegin()+firstCell1)].x;

	   for(nodeCtr1=firstCell1;nodeCtr1<totalCells1;)
	     {
	       vector <unsigned> cellIds;
	       //if not last cell
	       if(nodeCtr1 < totalCells1-1)
		 {
		   //if whitespace found
		   if((rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x+
		       hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+nodeCtr1))) <
		      rbplace[*(subrow1.cellIdsBegin()+nodeCtr1+1)].x)
		     {
		       cellIds.push_back(*(subrow1.cellIdsBegin()+nodeCtr1));
		       movables.push_back(cellIds);
		       cellIds.pop_back();
		       smallRow1.xMax = 
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x+
			 hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+nodeCtr1));
		       ++nodeCtr1;
		       ++totalMovables1;
		       if(totalMovables1 == windowSize1)
			 break;

		       //better white space handling
		       double currWSWidth=
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x-
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1-1)].x-
			 hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+nodeCtr1-1));

		       Point currWSLoc;
		       currWSLoc.x=
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1-1)].x+
			 hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+nodeCtr1-1));
		       currWSLoc.y=
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1-1)].y;
		       	       
		       if(currWSWidth >= 3*smallRow1.siteInterval)
			 {
			   numSitesWS =int(currWSWidth/smallRow1.siteInterval);
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   sitesPerWS = int(floor(numSitesWS/3+0.5));
			   numSitesWS -= sitesPerWS;
			   whiteSpaceWidths.push_back(sitesPerWS*
						      smallRow1.siteInterval);
			   whiteSpaceLocs.push_back(currWSLoc);
			   currWSLoc.x += sitesPerWS*smallRow1.siteInterval ;
			   smallRow1.xMax += sitesPerWS*smallRow1.siteInterval;
			   currWSWidth -= sitesPerWS*smallRow1.siteInterval;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;

			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   sitesPerWS = int(floor(numSitesWS/2+0.5));
			   numSitesWS -= sitesPerWS;
			   whiteSpaceWidths.push_back(sitesPerWS*
						      smallRow1.siteInterval);
			   whiteSpaceLocs.push_back(currWSLoc);
			   currWSLoc.x += sitesPerWS*smallRow1.siteInterval ;
			   smallRow1.xMax += sitesPerWS*smallRow1.siteInterval;
			   currWSWidth -= sitesPerWS*smallRow1.siteInterval;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;

			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   sitesPerWS = numSitesWS;
			   whiteSpaceWidths.push_back(sitesPerWS*
						      smallRow1.siteInterval);
			   whiteSpaceLocs.push_back(currWSLoc);
			   currWSLoc.x += sitesPerWS*smallRow1.siteInterval ;
			   smallRow1.xMax += sitesPerWS*smallRow1.siteInterval;
			   currWSWidth -= sitesPerWS*smallRow1.siteInterval;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;
			 }
		       
		       else if(currWSWidth < 3*smallRow1.siteInterval &&
			       currWSWidth >= 2*smallRow1.siteInterval)
		       
		       //if(currWSWidth >= 2*smallRow.siteInterval)
			 {
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   whiteSpaceWidths.push_back(currWSWidth/2);
			   whiteSpaceLocs.push_back(currWSLoc);
			   currWSLoc.x += currWSWidth/2;
			   smallRow1.xMax += currWSWidth/2;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;

			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   whiteSpaceWidths.push_back(currWSWidth/2);
			   whiteSpaceLocs.push_back(currWSLoc);
			   currWSLoc.x += currWSWidth/2;
			   smallRow1.xMax += currWSWidth/2;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;			   
			 }
		       else
		       
			 {
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   whiteSpaceWidths.push_back(currWSWidth);
			   whiteSpaceLocs.push_back(currWSLoc);
			   currWSLoc.x += currWSWidth;
			   smallRow1.xMax += currWSWidth;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;
			 }
		     }
		   else   //this cell has no WS in front of it
		     {
		       double rd = RandomDouble(-1,1);

		       cellIds.push_back(*(subrow1.cellIdsBegin()+nodeCtr1));
		       smallRow1.xMax=
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x+
			 hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+
						nodeCtr1));
		       ++nodeCtr1;
		       
		       //cluster the next cell based on random value
		       if(rd > 0 && cluster == 1)   //cluster
			 {
			   cellIds.push_back(*(subrow1.cellIdsBegin()+nodeCtr1));
			   smallRow1.xMax=
			     rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x+
			     hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+
						    nodeCtr1));
			   ++nodeCtr1;
			 }
		       movables.push_back(cellIds);
		       cellIds.erase(cellIds.begin(),cellIds.end());
		       ++totalMovables1;
		       if(totalMovables1 == windowSize1)
			 break;
		       
		       //if white space present after clustered cell
		       if(nodeCtr1 <= totalCells1-1 && rd > 0 && cluster == 1)
			 {
			   double currWSWidth = 
			     rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x
			     -rbplace[*(subrow1.cellIdsBegin()+nodeCtr1-1)].x
			     -hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()
						     +nodeCtr1-1));
			   Point currWSLoc;
			   if(currWSWidth >= 2*smallRow1.siteInterval)
			     {
			       numSitesWS = int(currWSWidth/
						smallRow1.siteInterval);
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       sitesPerWS = int(floor(numSitesWS/2+0.5));
			       numSitesWS -= sitesPerWS;
			       whiteSpaceWidths.push_back(sitesPerWS*
						       smallRow1.siteInterval);
			       currWSLoc.x=smallRow1.xMax;
			       currWSLoc.y=smallRow1.yMin;
			       whiteSpaceLocs.push_back(currWSLoc);
			       currWSLoc.x += sitesPerWS*
				 smallRow1.siteInterval ;
			       smallRow1.xMax += sitesPerWS*
				 smallRow1.siteInterval;
			       currWSWidth -= sitesPerWS*
				 smallRow1.siteInterval;
			       ++whiteSpaceCtr;
			       ++totalMovables1;
			       if(totalMovables1 == windowSize1)
				 break;
			       
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       sitesPerWS = numSitesWS;
			       whiteSpaceWidths.push_back(sitesPerWS*
						       smallRow1.siteInterval);
			       whiteSpaceLocs.push_back(currWSLoc);
			       currWSLoc.x += sitesPerWS*
				 smallRow1.siteInterval ;
			       smallRow1.xMax += sitesPerWS*
				 smallRow1.siteInterval;
			       currWSWidth -= sitesPerWS*
				 smallRow1.siteInterval;
			       ++whiteSpaceCtr;
			       ++totalMovables1;
			       if(totalMovables1 == windowSize1)
				 break;
			     }
			   else if(currWSWidth > 0 && 
				   currWSWidth < 2*smallRow1.siteInterval)
			     {
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       whiteSpaceWidths.push_back(currWSWidth);
			       currWSLoc.x=smallRow1.xMax;
			       currWSLoc.y=smallRow1.yMin;
			       whiteSpaceLocs.push_back(currWSLoc);
			       currWSLoc.x += currWSWidth;
			       smallRow1.xMax += currWSWidth;
			       ++whiteSpaceCtr;
			       ++totalMovables1;
			       if(totalMovables1 == windowSize1)
				 break;
			     }
			 }
		     }
		 }
	       else
		 {
		   cellIds.push_back(*(subrow1.cellIdsBegin()+nodeCtr1));
		   movables.push_back(cellIds);
		   cellIds.pop_back();
		   smallRow1.xMax=
		     rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x+
		     hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+nodeCtr1));
		   ++nodeCtr1;
		   ++totalMovables1;
		   if(totalMovables1 == windowSize1)
		     break;		  

		   //if whitespace present after last cell
		   double currWSWidth=subrow1.getXEnd() - smallRow1.xMax;
		   Point currWSLoc;
		   if(currWSWidth >= 2*smallRow1.siteInterval)
		     {
		       numSitesWS = int(currWSWidth/smallRow1.siteInterval);

		       cellIds.push_back(UINT_MAX);
		       movables.push_back(cellIds);
		       cellIds.pop_back();
		       sitesPerWS = int(floor(numSitesWS/2+0.5));
		       numSitesWS -= sitesPerWS;
		       whiteSpaceWidths.push_back(sitesPerWS*
						  smallRow1.siteInterval);
		       currWSLoc.x=smallRow1.xMax;
		       currWSLoc.y=smallRow1.yMin;
		       whiteSpaceLocs.push_back(currWSLoc);
		       currWSLoc.x += sitesPerWS*smallRow1.siteInterval ;
		       smallRow1.xMax += sitesPerWS*smallRow1.siteInterval;
		       currWSWidth -= sitesPerWS*smallRow1.siteInterval;
		       ++whiteSpaceCtr;
		       ++totalMovables1;
		       if(totalMovables1 == windowSize1)
			 break;

		       cellIds.push_back(UINT_MAX);
		       movables.push_back(cellIds);
		       cellIds.pop_back();
		       sitesPerWS = numSitesWS;
		       whiteSpaceWidths.push_back(sitesPerWS*
						  smallRow1.siteInterval);
		       whiteSpaceLocs.push_back(currWSLoc);
		       currWSLoc.x += sitesPerWS*smallRow1.siteInterval ;
		       smallRow1.xMax += sitesPerWS*smallRow1.siteInterval;
		       currWSWidth -= sitesPerWS*smallRow1.siteInterval;
		       ++whiteSpaceCtr;
		       ++totalMovables1;
		       if(totalMovables1 == windowSize1)
			 break;
		     }
		   else if(currWSWidth > 0 && 
			   currWSWidth < 2*smallRow1.siteInterval)
		     {
		       cellIds.push_back(UINT_MAX);
		       movables.push_back(cellIds);
		       cellIds.pop_back();
		       whiteSpaceWidths.push_back(currWSWidth);
		       currWSLoc.x=smallRow1.xMax;
		       currWSLoc.y=smallRow1.yMin;
		       whiteSpaceLocs.push_back(currWSLoc);
		       currWSLoc.x += currWSWidth;
		       smallRow1.xMax += currWSWidth;
		       ++whiteSpaceCtr;
		       ++totalMovables1;
		       if(totalMovables1 == windowSize1)
			 break;
		     }
		 }
	     }
	   smallPbRows.push_back(smallRow1);   
	   row1Size = movables.size();

	   //now start the second row
	   smallRow2.xMax = smallRow2.xMin = 
	     rbplace[*(subrow2.cellIdsBegin()+firstCell2)].x;

	   if(rIt1 != rIt2 && windowSize2 != 0)
	     {
	       for(nodeCtr2=firstCell2;nodeCtr2<totalCells2;)
		 {
		   vector <unsigned> cellIds;
		   //if not last cell
		   if(nodeCtr2 < totalCells2-1)
		     {
		       //if whitespace found
		       if(rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x+
			  hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+nodeCtr2))
			  < rbplace[*(subrow2.cellIdsBegin()+nodeCtr2+1)].x)
			 {
			   cellIds.push_back(*(subrow2.cellIdsBegin()+nodeCtr2));
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   smallRow2.xMax = 
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x+
			     hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+nodeCtr2));
			   ++nodeCtr2;
			   ++totalMovables2;
			   if(totalMovables2 == windowSize2)
			     break;
			   
			   //better white space handling
			   double currWSWidth=
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x-
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2-1)].x-
			     hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+nodeCtr2-1));
			   
			   Point currWSLoc;
			   currWSLoc.x=
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2-1)].x+
			     hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+nodeCtr2-1));
			   currWSLoc.y=
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2-1)].y;
			   
			   if(currWSWidth >= 3*smallRow2.siteInterval)
			     {
			       numSitesWS =int(currWSWidth/smallRow2.siteInterval);
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       sitesPerWS = int(floor(numSitesWS/3+0.5));
			       numSitesWS -= sitesPerWS;
			       whiteSpaceWidths.push_back(sitesPerWS*
							  smallRow2.siteInterval);
			       whiteSpaceLocs.push_back(currWSLoc);
			       currWSLoc.x += sitesPerWS*smallRow2.siteInterval ;
			       smallRow2.xMax += sitesPerWS*smallRow2.siteInterval;
			       currWSWidth -= sitesPerWS*smallRow2.siteInterval;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;
			       
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();

			       sitesPerWS = int(floor(numSitesWS/2+0.5));
			       numSitesWS -= sitesPerWS;
			       whiteSpaceWidths.push_back(sitesPerWS*
							  smallRow2.siteInterval);
			       whiteSpaceLocs.push_back(currWSLoc);
			       currWSLoc.x += sitesPerWS*smallRow2.siteInterval ;
			       smallRow2.xMax += sitesPerWS*smallRow2.siteInterval;
			       currWSWidth -= sitesPerWS*smallRow2.siteInterval;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;
			       
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       sitesPerWS = numSitesWS;
			       whiteSpaceWidths.push_back(sitesPerWS*
							  smallRow2.siteInterval);
			       whiteSpaceLocs.push_back(currWSLoc);
			       currWSLoc.x += sitesPerWS*smallRow2.siteInterval ;
			       smallRow2.xMax += sitesPerWS*smallRow2.siteInterval;
			       currWSWidth -= sitesPerWS*smallRow2.siteInterval;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;
			     }
			   
			   else if(currWSWidth < 3*smallRow2.siteInterval &&
				   currWSWidth >= 2*smallRow2.siteInterval)
			     
			     //if(currWSWidth >= 2*smallRow2.siteInterval)
			     {
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       whiteSpaceWidths.push_back(currWSWidth/2);
			       whiteSpaceLocs.push_back(currWSLoc);
			       currWSLoc.x += currWSWidth/2;
			       smallRow2.xMax += currWSWidth/2;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;
			       
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       whiteSpaceWidths.push_back(currWSWidth/2);
			       whiteSpaceLocs.push_back(currWSLoc);
			       currWSLoc.x += currWSWidth/2;
			       smallRow2.xMax += currWSWidth/2;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;			   
			     }
			   else
			     {
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       whiteSpaceWidths.push_back(currWSWidth);
			       whiteSpaceLocs.push_back(currWSLoc);
			       currWSLoc.x += currWSWidth;
			       smallRow2.xMax += currWSWidth;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;
			     }
			 }
		       else //cluster here
			 {
			   double rd = RandomDouble(-1,1);
			   cellIds.push_back(*(subrow2.cellIdsBegin()+nodeCtr2));
			   smallRow2.xMax=
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x+
			     hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+nodeCtr2));
			   ++nodeCtr2;
			   if(rd > 0 && cluster == 1)
			     {
			       cellIds.push_back(*(subrow2.cellIdsBegin()+nodeCtr2));
			       smallRow2.xMax=
				 rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x+
				 hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+nodeCtr2));
			       ++nodeCtr2;			       
			     }
			   movables.push_back(cellIds);
			   cellIds.erase(cellIds.begin(),cellIds.end());
			   ++totalMovables2;
			   if(totalMovables2 == windowSize2)
			     break;

			   //if white space present after clustered cell
			   if(nodeCtr2 <= totalCells2-1 && rd > 0 && cluster == 1) 
			     {
			       double currWSWidth = 
				 rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x
				 -rbplace[*(subrow2.cellIdsBegin()+nodeCtr2-1)].x
				 -hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()
							 +nodeCtr2-1));

			       Point currWSLoc;
			       if(currWSWidth >= 2*smallRow2.siteInterval)
				 {
				   numSitesWS =
				     int(currWSWidth/smallRow2.siteInterval);
				   cellIds.push_back(UINT_MAX);
				   movables.push_back(cellIds);
				   cellIds.pop_back();
				   sitesPerWS = int(floor(numSitesWS/2+0.5));
				   numSitesWS -= sitesPerWS;
				   whiteSpaceWidths.push_back(sitesPerWS*
						       smallRow2.siteInterval);
				   currWSLoc.x=smallRow2.xMax;
				   currWSLoc.y=smallRow2.yMin;
				   whiteSpaceLocs.push_back(currWSLoc);
				   currWSLoc.x += sitesPerWS*
				     smallRow2.siteInterval ;
				   smallRow2.xMax += sitesPerWS*
				     smallRow2.siteInterval;
				   currWSWidth -= sitesPerWS*
				     smallRow2.siteInterval;
				   ++whiteSpaceCtr;
				   ++totalMovables2;
				   if(totalMovables2 == windowSize2)
				     break;
				   
				   cellIds.push_back(UINT_MAX);
				   movables.push_back(cellIds);
				   cellIds.pop_back();
				   sitesPerWS = numSitesWS;
				   whiteSpaceWidths.push_back(sitesPerWS*
						       smallRow2.siteInterval);
				   whiteSpaceLocs.push_back(currWSLoc);
				   currWSLoc.x += sitesPerWS*
				     smallRow2.siteInterval ;
				   smallRow2.xMax += sitesPerWS*
				     smallRow2.siteInterval;
				   currWSWidth -= sitesPerWS*
				     smallRow2.siteInterval;
				   ++whiteSpaceCtr;
				   ++totalMovables2;
				   if(totalMovables2 == windowSize2)
				     break;
				 }
			       else if(currWSWidth > 0 && 
				       currWSWidth < 2*smallRow2.siteInterval)
				 {
				   cellIds.push_back(UINT_MAX);
				   movables.push_back(cellIds);
				   cellIds.pop_back();
				   whiteSpaceWidths.push_back(currWSWidth);
				   currWSLoc.x=smallRow2.xMax;
				   currWSLoc.y=smallRow2.yMin;
				   whiteSpaceLocs.push_back(currWSLoc);
				   currWSLoc.x += currWSWidth;
				   smallRow2.xMax += currWSWidth;
				   ++whiteSpaceCtr;
				   ++totalMovables2;
				   if(totalMovables2 == windowSize2)
				     break;
				 }
			     }
			 }
		     }
		   else
		     {
		       cellIds.push_back(*(subrow2.cellIdsBegin()+nodeCtr2));
		       movables.push_back(cellIds);
		       cellIds.pop_back();
		       smallRow2.xMax=
			 rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x+
			 hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+nodeCtr2));
		       ++nodeCtr2;
		       ++totalMovables2;
		       if(totalMovables2 == windowSize2)
			 break;		  
		       
		       //if whitespace present after last cell
		       double currWSWidth=subrow2.getXEnd() - smallRow2.xMax;
		       Point currWSLoc;
		       if(currWSWidth >= 2*smallRow2.siteInterval)
			 {
			   numSitesWS = int(currWSWidth/smallRow2.siteInterval);
			   
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   sitesPerWS = int(floor(numSitesWS/2+0.5));
			   numSitesWS -= sitesPerWS;
			   whiteSpaceWidths.push_back(sitesPerWS*
						      smallRow2.siteInterval);
			   currWSLoc.x=smallRow2.xMax;
			   currWSLoc.y=smallRow2.yMin;
			   whiteSpaceLocs.push_back(currWSLoc);
			   currWSLoc.x += sitesPerWS*smallRow2.siteInterval ;
			   smallRow2.xMax += sitesPerWS*smallRow2.siteInterval;
			   currWSWidth -= sitesPerWS*smallRow2.siteInterval;
			   ++whiteSpaceCtr;
			   ++totalMovables2;
			   if(totalMovables2 == windowSize2)
			     break;
			   
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   sitesPerWS = numSitesWS;
			   whiteSpaceWidths.push_back(sitesPerWS*
						      smallRow2.siteInterval);
			   whiteSpaceLocs.push_back(currWSLoc);
			   currWSLoc.x += sitesPerWS*smallRow2.siteInterval ;
			   smallRow2.xMax += sitesPerWS*smallRow2.siteInterval;
			   currWSWidth -= sitesPerWS*smallRow2.siteInterval;
			   ++whiteSpaceCtr;
			   ++totalMovables2;
			   if(totalMovables2 == windowSize2)
			     break;
			 }
		       else if(currWSWidth > 0 && 
			       currWSWidth < 2*smallRow2.siteInterval)
			 {
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   whiteSpaceWidths.push_back(currWSWidth);
			   currWSLoc.x=smallRow2.xMax;
			   currWSLoc.y=smallRow2.yMin;
			   whiteSpaceLocs.push_back(currWSLoc);
			   currWSLoc.x += currWSWidth;
			   smallRow2.xMax += currWSWidth;
			   ++whiteSpaceCtr;
			   ++totalMovables2;
			   if(totalMovables2 == windowSize2)
			     break;
			 }
		     }
		 }
	       smallPbRows.push_back(smallRow2);
	     }

	   if(movables.size() < 2)
	     {
	       firstCell1=nodeCtr1;
	       continue;
	     }

	   currCellLoc1=nodeCtr1;
	   currCellLoc2=nodeCtr2;

	   //assume all the cells are placed
	   //assume all cells are not fixed

	   bit_vector fixed(movables.size(),0);
	   for(unsigned i=0; i<movables.size(); ++i)
	     {
	       bool oneFixedCell=false;
	       for(unsigned j=0; j<movables[i].size(); ++j)
		 {
		   if(movables[i][j] != UINT_MAX)
		     {
		       if(rbplace.isFixed(movables[i][j]))
			 {
			   oneFixedCell = true;
			   break;
			 }
		     }
		 }
	       if(oneFixedCell)
		 fixed[i] = true;
	     }

           SmallPlacementProblem smallProblem(hgWDims, rbplace,
	      placed, fixed, movables, smallPbRows, whiteSpaceWidths);

           SmallPlacementSolution& solution=smallProblem.getSolnBuffer()[0];
           solution.populated=true;

	   unsigned currWSLocation=0;
	   for(unsigned cell=0; cell!=movables.size(); cell++)
	     {
	       if(movables[cell][0] != UINT_MAX)
		 {
		   solution.placement[cell]   =rbplace[movables[cell][0]];
		   solution.orientations[cell]=rbplace.getOrient(movables[cell][0]);
		 }
	       else
		 {
		   Point WSLoc = whiteSpaceLocs[currWSLocation];
		   ++currWSLocation;
		   solution.placement[cell] = WSLoc;
		 }
	     }
	   if(smallProblem.getNumNets() > 63)
	     {
	       abkwarn(0,"skipping problem with > 63 nets\n");
	       firstCell1 = nodeCtr1;
	       continue;
	     }

	   //invoke the SmallPlacer to solve the problem
	   //smallProblem.plot("smPbIn");
	   //smallProblem.save("smPbIn");
	   SmallPlacer placer(smallProblem,params.smplParams);
	   //smallProblem.plot("smPbOut");
	   //smallProblem.save("smPbOut");

	   //get best solution
	   const Placement& soln = smallProblem.getBestSoln().placement;
	   abkassert(soln.getSize() == movables.size(),"undersized placement");
	   
	   //rbplace.saveAsPlot("plotWNames",-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,"RBPlIn");
	   //double initHPWL = rbplace.evalHPWL(true);
	   rbplace.updateIronedCellsLR(movables,soln,
				       const_cast<RBPSubRow&>(subrow1),
				       const_cast<RBPSubRow&>(subrow2) );

	   //double finalHPWL = rbplace.evalHPWL(true);
	   /*
	   if(finalHPWL > initHPWL)
	     {
	       rbplace.saveAsPlot("plotWNames",-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,"RBPlOut");
	       cout<<"increase in HPWL is "<<finalHPWL-initHPWL<<endl;
	       abkfatal(0,"bye\n");
	     }
	   */
	   nodeCtr1=firstCell1;
	   for(unsigned i=0;i<movables.size();++i)
	     {
	       if(movables[i][0] != UINT_MAX)
		 if(soln[i].y == subrow1.getYCoord())
		   ++firstCell1;
	     }

	   //to avoid a deadlock condition
	   unsigned overlap=0;
	   if(row1Size == 0)
	     overlap = 0;
	   else if(params.overlap > row1Size)
	     overlap = row1Size-1;
	   else
	     overlap = params.overlap;
	   
	   if(nodeCtr1 < (firstCell1 - overlap))
	     firstCell1 = (firstCell1 - overlap);
	   totalCells1=subrow1.getNumCells();
	   totalCells2=subrow2.getNumCells();
	 }
     }
     
     if(verticalDir == 0)
       {
	 ++rIt1;
	 if(rIt1 == rbplace.coreRowsEnd())
	   break;
       }
     else
       {
	 if(rIt1 == rbplace.coreRowsBegin())
	   break;
	 else
	   --rIt1;
       }
   }
}


//to iron Rows in two dimension right to left
void ironRows2DRL(RBPlacement& rbplace,
		  RowIroningParameters params,bool verticalDir, bool twoDim, 
		  bool cluster)
{
  const HGraphWDimensions& hgWDims = rbplace.getHGraph();
  bit_vector placed = bit_vector(rbplace.getNumCells(),1);

  char fileName1[15];
  char fileName2[15];
  char fileName3[15];
  char fileName4[15];

  unsigned instance=0;

  //verticalDir=0 means bottom up pass else
  //top down pass
   itRBPCoreRow rIt1,rIt2;
   itRBPSubRow sIt1,sIt2;

   if(verticalDir == 0)
     rIt1=rbplace.coreRowsBegin();
   else
     rIt1=rbplace.coreRowsEnd()-1;

   while(true)                 //exit only by a break
   {
     unsigned windowSize1=params.windowSize;
     unsigned windowSize2=0;
     rIt2 = rIt1;
     if(verticalDir == 0)
       {
	 if(rIt1 != rbplace.coreRowsEnd()-1  && twoDim == 1) 
	                                    //if not last core row
	   {
	     ++rIt2;
	     windowSize1 = unsigned(ceil(params.windowSize/2));
	     windowSize2 = params.windowSize - windowSize1;
	   }
       }
     else
       {
	 if(rIt1 != rbplace.coreRowsBegin() && twoDim == 1)   
	                                    //if not first Row
	   {
	     --rIt2;
	     windowSize1 = unsigned(ceil(params.windowSize/2));
	     windowSize2 = params.windowSize - windowSize1;
	   }
       }

     sIt1=rIt1->subRowsEnd()-1;
     sIt2=rIt2->subRowsEnd()-1;

     while(true)
     {
       const RBPSubRow& subrow1=*sIt1;
       const RBPSubRow& subrow2=*sIt2;

       SmallPlacementRow smallRow1;
       SmallPlacementRow smallRow2;

       smallRow1.siteInterval=subrow1.getSpacing();
       smallRow1.xMax =smallRow1.xMin=subrow1.getXEnd();
       smallRow1.yMax =smallRow1.yMin=subrow1.getYCoord();
       smallRow1.yMax+=subrow1.getHeight();
       unsigned firstCell1=0;
       unsigned totalCells1=subrow1.getNumCells();
       unsigned currCellLoc1=0;
       int nodeCtr1=0;
       int lastCell1 = totalCells1-1;

       smallRow2.siteInterval=subrow2.getSpacing();
       smallRow2.xMax =smallRow2.xMin=subrow2.getXEnd();
       smallRow2.yMax =smallRow2.yMin=subrow2.getYCoord();
       smallRow2.yMax+=subrow2.getHeight();
//       unsigned firstCell2=0;
       unsigned totalCells2=subrow2.getNumCells();
       unsigned currCellLoc2=0;
       int nodeCtr2=0;
       int lastCell2 = totalCells2-1;

       int numSitesWS=0;
       int sitesPerWS=0;

       if(totalCells1 < 1 || totalCells2 < 1) 
	 {
	   if(sIt1 == rIt1->subRowsBegin() || sIt2 == rIt2->subRowsBegin())
	     break;
	   --sIt1;
	   --sIt2;
	   continue;
	 }

       //now start forming instances
       while (lastCell1 >= 0)
	 { 
	   ++instance;
	   sprintf(fileName1,"SmPbIn-%d",instance);
	   sprintf(fileName2,"SmPbOut-%d",instance);
	   sprintf(fileName3,"RbPbIn-%d",instance);
	   sprintf(fileName4,"RbPbOut-%d",instance);

	   vector<SmallPlacementRow> smallPbRows;
	   //form the 1'st row first
	   //get the cells + whitespace cells here
	   vector< vector<unsigned> > movables;
	   vector<double> whiteSpaceWidths;
	   vector<Point> whiteSpaceLocs;
	   unsigned whiteSpaceCtr=0;
	   unsigned totalMovables1=0;
	   unsigned totalMovables2=0;
	   unsigned row1Size=0;

	   bool brokeFromLoop = 0;
	   if(lastCell1 >= 0 )
	     {
	       for(nodeCtr2=0;nodeCtr2<int(totalCells2-1);++nodeCtr2)
		 {
		   if((rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x <=
		       rbplace[*(subrow1.cellIdsBegin()+lastCell1)].x) &&
		      (rbplace[*(subrow2.cellIdsBegin()+nodeCtr2+1)].x >
		       rbplace[*(subrow1.cellIdsBegin()+lastCell1)].x))
		     {
		       brokeFromLoop = 1;
		       break;
		     }
		 }
	       if(!brokeFromLoop)
		 nodeCtr2 = totalCells2-1;
	       lastCell2 = nodeCtr2;
	     }
	   else
	     lastCell2 = -1;

	   nodeCtr1=lastCell1;
	   nodeCtr2=lastCell2;

	   smallRow1.xMax = smallRow1.xMin = 
	     rbplace[*(subrow1.cellIdsBegin()+lastCell1)].x+
	     hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+lastCell1));

	   while(nodeCtr1 >=0)
	     {
	       vector<unsigned> cellIds;

	       //if not last cell
	       if(nodeCtr1 > 0)
		 {
		   //if whitespace found
		   if((rbplace[*(subrow1.cellIdsBegin()+nodeCtr1-1)].x+
		      hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+nodeCtr1-1))) <
		      rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x)
		     {
		       cellIds.push_back(*(subrow1.cellIdsBegin()+nodeCtr1));
		       movables.push_back(cellIds);
		       cellIds.pop_back();
		       smallRow1.xMin = 
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x;
		       --nodeCtr1;
		       ++totalMovables1;
		       if(totalMovables1 == windowSize1)
			 break;

		       //better white space handling
		       double currWSWidth=
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1+1)].x-
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x-
			 hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+nodeCtr1));

		       Point currWSLoc;
		       currWSLoc.x=
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1+1)].x;
		       currWSLoc.y=
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1+1)].y;
		       	       
		       if(currWSWidth >= 3*smallRow1.siteInterval)
			 {
			   numSitesWS =int(currWSWidth/smallRow1.siteInterval);

			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   sitesPerWS = int(floor(numSitesWS/3+0.5));
			   numSitesWS -= sitesPerWS;
			   whiteSpaceWidths.push_back(sitesPerWS*
						      smallRow1.siteInterval);
			   currWSLoc.x -= sitesPerWS*smallRow1.siteInterval ;
			   whiteSpaceLocs.push_back(currWSLoc);
			   smallRow1.xMin -= sitesPerWS*smallRow1.siteInterval;
			   currWSWidth -= sitesPerWS*smallRow1.siteInterval;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;

			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   sitesPerWS = int(floor(numSitesWS/2+0.5));
			   numSitesWS -= sitesPerWS;
			   whiteSpaceWidths.push_back(sitesPerWS*
						      smallRow1.siteInterval);
			   currWSLoc.x -= sitesPerWS*smallRow1.siteInterval ;
			   whiteSpaceLocs.push_back(currWSLoc);
			   smallRow1.xMin -= sitesPerWS*smallRow1.siteInterval;
			   currWSWidth -= sitesPerWS*smallRow1.siteInterval;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;

			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   sitesPerWS = numSitesWS;
			   whiteSpaceWidths.push_back(sitesPerWS*
						      smallRow1.siteInterval);
			   currWSLoc.x -= sitesPerWS*smallRow1.siteInterval ;
			   whiteSpaceLocs.push_back(currWSLoc);
			   smallRow1.xMin -= sitesPerWS*smallRow1.siteInterval;
			   currWSWidth -= sitesPerWS*smallRow1.siteInterval;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;
			 }
		       
		       else if(currWSWidth < 3*smallRow1.siteInterval &&
			       currWSWidth >= 2*smallRow1.siteInterval)
		       
		       //if(currWSWidth >= 2*smallRow.siteInterval)
			 {
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   whiteSpaceWidths.push_back(currWSWidth/2);
			   currWSLoc.x -= currWSWidth/2;
			   whiteSpaceLocs.push_back(currWSLoc);
			   smallRow1.xMin -= currWSWidth/2;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;

			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   whiteSpaceWidths.push_back(currWSWidth/2);
			   currWSLoc.x -= currWSWidth/2;
			   whiteSpaceLocs.push_back(currWSLoc);
			   smallRow1.xMin -= currWSWidth/2;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;			   
			 }
		       else
		       
			 {
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   whiteSpaceWidths.push_back(currWSWidth);
			   currWSLoc.x -= currWSWidth;
			   whiteSpaceLocs.push_back(currWSLoc);
			   smallRow1.xMin -= currWSWidth;
			   ++whiteSpaceCtr;
			   ++totalMovables1;
			   if(totalMovables1 == windowSize1)
			     break;
			 }
		     }
		   else  //cluster the cells here
		     {
		       double rd = RandomDouble(-1,1);
		       cellIds.push_back(*(subrow1.cellIdsBegin()+nodeCtr1));
		       smallRow1.xMin=
			 rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x;
		       --nodeCtr1;
		       //cluster the next cell based on random value
		       if(rd > 0 && cluster == 1)  //cluster
			 {
			 //cellIds.push_back(*(subrow1.cellIdsBegin()+nodeCtr1));
			   cellIds.insert(cellIds.begin(),
			                  *(subrow1.cellIdsBegin()+nodeCtr1));
			   smallRow1.xMin=
			     rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x;
			   --nodeCtr1; 
			 }

		       movables.push_back(cellIds);
		       cellIds.erase(cellIds.begin(),cellIds.end());
		       ++totalMovables1;
		       if(totalMovables1 == windowSize1)
			 break;

		       //if white space present after clustered cell
		       if(nodeCtr1 >= 0 && rd > 0 && cluster == 1)
			 //if not first cell
			 {
			   double currWSWidth=
			     rbplace[*(subrow1.cellIdsBegin()+nodeCtr1+1)].x -
			     rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x-
			     hgWDims.getNodeWidth(*(subrow1.cellIdsBegin()+
						    nodeCtr1));
			   
			   Point currWSLoc;
			   if(currWSWidth >= 2*smallRow1.siteInterval)
			     {
			       numSitesWS = int(currWSWidth/
						smallRow1.siteInterval);
			       
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       sitesPerWS = int(floor(numSitesWS/2+0.5));
			       numSitesWS -= sitesPerWS;
			       whiteSpaceWidths.push_back(sitesPerWS*
						       smallRow1.siteInterval);
			       currWSLoc.x=smallRow1.xMin;
			       currWSLoc.y=smallRow1.yMin;
			       currWSLoc.x -=sitesPerWS*smallRow1.siteInterval;
			       whiteSpaceLocs.push_back(currWSLoc);
			       smallRow1.xMin -= sitesPerWS*
				 smallRow1.siteInterval;
			       currWSWidth -=sitesPerWS*smallRow1.siteInterval;
			       ++whiteSpaceCtr;
			       ++totalMovables1;
			       if(totalMovables1 == windowSize1)
				 break;
			       
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       sitesPerWS = numSitesWS;
			       whiteSpaceWidths.push_back(sitesPerWS*
						       smallRow1.siteInterval);
			       currWSLoc.x -= sitesPerWS*smallRow1.siteInterval ;
			       whiteSpaceLocs.push_back(currWSLoc);
			       smallRow1.xMin -= sitesPerWS*
				 smallRow1.siteInterval;
			       currWSWidth -= sitesPerWS*
				 smallRow1.siteInterval;
			       ++whiteSpaceCtr;
			       ++totalMovables1;
			       if(totalMovables1 == windowSize1)
				 break;
			     }
			   else if(currWSWidth > 0 && 
				   currWSWidth < 2*smallRow1.siteInterval)
			     {
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       whiteSpaceWidths.push_back(currWSWidth);
			       currWSLoc.x=smallRow1.xMin;
			       currWSLoc.y=smallRow1.yMin;
			       currWSLoc.x -= currWSWidth;
			       whiteSpaceLocs.push_back(currWSLoc);
			       smallRow1.xMin -= currWSWidth;
			       ++whiteSpaceCtr;
			       ++totalMovables1;
			       if(totalMovables1 == windowSize1)
				 break;
			     }
			 }
		     }
		 }
	       else
		 {
		   cellIds.push_back(*(subrow1.cellIdsBegin()+nodeCtr1));
		   movables.push_back(cellIds);
		   cellIds.pop_back();
		   smallRow1.xMin=
		     rbplace[*(subrow1.cellIdsBegin()+nodeCtr1)].x;
		   --nodeCtr1;
		   ++totalMovables1;
		   if(totalMovables1 == windowSize1)
		     break;		  

		   //if whitespace present after last cell
		   double currWSWidth=subrow1.getXStart() - smallRow1.xMin;
		   Point currWSLoc;
		   if(currWSWidth >= 2*smallRow1.siteInterval)
		     {
		       numSitesWS = int(currWSWidth/smallRow1.siteInterval);

		       cellIds.push_back(UINT_MAX);
		       movables.push_back(cellIds);
		       cellIds.pop_back();
		       sitesPerWS = int(floor(numSitesWS/2+0.5));
		       numSitesWS -= sitesPerWS;
		       whiteSpaceWidths.push_back(sitesPerWS*
						  smallRow1.siteInterval);
		       currWSLoc.x=smallRow1.xMin;
		       currWSLoc.y=smallRow1.yMin;
		       currWSLoc.x -= sitesPerWS*smallRow1.siteInterval ;
		       whiteSpaceLocs.push_back(currWSLoc);
		       smallRow1.xMin -= sitesPerWS*smallRow1.siteInterval;
		       currWSWidth -= sitesPerWS*smallRow1.siteInterval;
		       ++whiteSpaceCtr;
		       ++totalMovables1;
		       if(totalMovables1 == windowSize1)
			 break;

		       cellIds.push_back(UINT_MAX);
		       movables.push_back(cellIds);
		       cellIds.pop_back();
		       sitesPerWS = numSitesWS;
		       whiteSpaceWidths.push_back(sitesPerWS*
						  smallRow1.siteInterval);
		       currWSLoc.x -= sitesPerWS*smallRow1.siteInterval ;
		       whiteSpaceLocs.push_back(currWSLoc);
		       smallRow1.xMin -= sitesPerWS*smallRow1.siteInterval;
		       currWSWidth -= sitesPerWS*smallRow1.siteInterval;
		       ++whiteSpaceCtr;
		       ++totalMovables1;
		       if(totalMovables1 == windowSize1)
			 break;
		     }
		   else if(currWSWidth > 0 && 
			   currWSWidth < 2*smallRow1.siteInterval)
		     {
		       cellIds.push_back(UINT_MAX);
		       movables.push_back(cellIds);
		       cellIds.pop_back();
		       whiteSpaceWidths.push_back(currWSWidth);
		       currWSLoc.x=smallRow1.xMin;
		       currWSLoc.y=smallRow1.yMin;
		       currWSLoc.x -= currWSWidth;
		       whiteSpaceLocs.push_back(currWSLoc);
		       smallRow1.xMin -= currWSWidth;
		       ++whiteSpaceCtr;
		       ++totalMovables1;
		       if(totalMovables1 == windowSize1)
			 break;
		     }
		 }
	     }
	   smallPbRows.push_back(smallRow1);   
	   row1Size = movables.size();
	   firstCell1 = nodeCtr1+1;

	   //now start the second row
	   nodeCtr2=lastCell2;
	   smallRow2.xMax = smallRow2.xMin = 
	     rbplace[*(subrow2.cellIdsBegin()+lastCell2)].x+
	     hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+lastCell2));

	   if(rIt1 != rIt2 && windowSize2 != 0)
	     {
	       while(nodeCtr2 >= 0)
		 {
		   vector<unsigned> cellIds;
		   //if not last cell
		   if(nodeCtr2 > 0)
		     {
		       //if whitespace found
		       if(rbplace[*(subrow2.cellIdsBegin()+nodeCtr2-1)].x+
			  hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+nodeCtr2-1))
			  < rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x)
			 {
			   cellIds.push_back(*(subrow2.cellIdsBegin()+nodeCtr2));
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   smallRow2.xMin = 
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x;
			   --nodeCtr2;
			   ++totalMovables2;
			   if(totalMovables2 == windowSize2)
			     break;
			   
			   //better white space handling
			   double currWSWidth=
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2+1)].x-
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x-
			     hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+nodeCtr2));
			   
			   Point currWSLoc;
			   currWSLoc.x=
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2+1)].x;
			   currWSLoc.y=
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2+1)].y;
			   
			   if(currWSWidth >= 3*smallRow2.siteInterval)
			     {
			       numSitesWS =int(currWSWidth/smallRow2.siteInterval);
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       sitesPerWS = int(floor(numSitesWS/3+0.5));
			       numSitesWS -= sitesPerWS;
			       whiteSpaceWidths.push_back(sitesPerWS*
							  smallRow2.siteInterval);
			       currWSLoc.x-=sitesPerWS*smallRow2.siteInterval;
			       whiteSpaceLocs.push_back(currWSLoc);
			       smallRow2.xMin -= sitesPerWS*smallRow2.siteInterval;
			       currWSWidth -= sitesPerWS*smallRow2.siteInterval;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;
			       
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       sitesPerWS = int(floor(numSitesWS/2+0.5));
			       numSitesWS -= sitesPerWS;
			       whiteSpaceWidths.push_back(sitesPerWS*
							  smallRow2.siteInterval);
			       currWSLoc.x -= sitesPerWS*smallRow2.siteInterval ;
			       whiteSpaceLocs.push_back(currWSLoc);
			       smallRow2.xMin -= sitesPerWS*smallRow2.siteInterval;
			       currWSWidth -= sitesPerWS*smallRow2.siteInterval;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;
			       
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       sitesPerWS = numSitesWS;
			       whiteSpaceWidths.push_back(sitesPerWS*
							  smallRow2.siteInterval);
			       currWSLoc.x -= sitesPerWS*smallRow2.siteInterval ;
			       whiteSpaceLocs.push_back(currWSLoc);
			       smallRow2.xMin -= sitesPerWS*smallRow2.siteInterval;
			       currWSWidth -= sitesPerWS*smallRow2.siteInterval;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;
			     }
			   
			   else if(currWSWidth < 3*smallRow2.siteInterval &&
				   currWSWidth >= 2*smallRow2.siteInterval)
			     
			     //if(currWSWidth >= 2*smallRow2.siteInterval)
			     {
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       whiteSpaceWidths.push_back(currWSWidth/2);
			       currWSLoc.x -= currWSWidth/2;
			       whiteSpaceLocs.push_back(currWSLoc);
			       smallRow2.xMin -= currWSWidth/2;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;
			       
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       whiteSpaceWidths.push_back(currWSWidth/2);
			       currWSLoc.x -= currWSWidth/2;
			       whiteSpaceLocs.push_back(currWSLoc);
			       smallRow2.xMin -= currWSWidth/2;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;			   
			     }
			   else
			     {
			       cellIds.push_back(UINT_MAX);
			       movables.push_back(cellIds);
			       cellIds.pop_back();
			       whiteSpaceWidths.push_back(currWSWidth);
			       currWSLoc.x -= currWSWidth;
			       whiteSpaceLocs.push_back(currWSLoc);
			       smallRow2.xMin -= currWSWidth;
			       ++whiteSpaceCtr;
			       ++totalMovables2;
			       if(totalMovables2 == windowSize2)
				 break;
			     }
			 }
		       else    //cluster here
			 {
			   double rd = RandomDouble(-1,1);
			   cellIds.push_back(*(subrow2.cellIdsBegin()+nodeCtr2));
			   smallRow2.xMin=
			     rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x;
			   --nodeCtr2;
			   //cluster the next cell based on random value
			   if(rd > 0 && cluster == 1)  //cluster
			     {
			       //cellIds.push_back(*(subrow2.cellIdsBegin()+nodeCtr2));
                               cellIds.insert(cellIds.begin(),
			                    *(subrow2.cellIdsBegin()+nodeCtr2));
								     
			       smallRow2.xMin=
				 rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x;
			       --nodeCtr2; 
			     }
			   
			   movables.push_back(cellIds);
			   cellIds.erase(cellIds.begin(),cellIds.end());
			   ++totalMovables2;
			   if(totalMovables2 == windowSize2)
			     break;

			   //if white space present after clustered cell
			   if(nodeCtr2 >= 0 && rd > 0 && cluster == 1)
			          //if not beyond first cell
			     {
			       double currWSWidth=
				 rbplace[*(subrow2.cellIdsBegin()+nodeCtr2+1)].x-
				 rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x-
				 hgWDims.getNodeWidth(*(subrow2.cellIdsBegin()+
							nodeCtr2));
			   
			       Point currWSLoc;
			       if(currWSWidth >= 2*smallRow2.siteInterval)
				 {
				   numSitesWS = int(currWSWidth/
						    smallRow2.siteInterval);
				   cellIds.push_back(UINT_MAX);
				   movables.push_back(cellIds);
				   cellIds.pop_back();
				   sitesPerWS = int(floor(numSitesWS/2+0.5));
				   numSitesWS -= sitesPerWS;
				   whiteSpaceWidths.push_back(sitesPerWS*
						       smallRow2.siteInterval);
				   currWSLoc.x=smallRow2.xMin;
				   currWSLoc.y=smallRow2.yMin;
				   currWSLoc.x -= sitesPerWS*
				     smallRow2.siteInterval ;
				   whiteSpaceLocs.push_back(currWSLoc);
				   smallRow2.xMin -= sitesPerWS*
				     smallRow2.siteInterval;
				   currWSWidth -= sitesPerWS*
				     smallRow2.siteInterval;
				   ++whiteSpaceCtr;
				   ++totalMovables2;
				   if(totalMovables2 == windowSize2)
				     break;
				   
				   cellIds.push_back(UINT_MAX);
				   movables.push_back(cellIds);
				   cellIds.pop_back();
				   sitesPerWS = numSitesWS;
				   whiteSpaceWidths.push_back(sitesPerWS*
						       smallRow2.siteInterval);
				   currWSLoc.x -= sitesPerWS*
				     smallRow2.siteInterval ;
				   whiteSpaceLocs.push_back(currWSLoc);
				   smallRow2.xMin -= sitesPerWS*
				     smallRow2.siteInterval;
				   currWSWidth -= sitesPerWS*
				     smallRow2.siteInterval;
				   ++whiteSpaceCtr;
				   ++totalMovables2;
				   if(totalMovables2 == windowSize2)
				     break;
				 }
			       else if(currWSWidth > 0 && 
				       currWSWidth < 2*smallRow2.siteInterval)
				 {
				   cellIds.push_back(UINT_MAX);
				   movables.push_back(cellIds);
				   cellIds.pop_back();
				   whiteSpaceWidths.push_back(currWSWidth);
				   currWSLoc.x=smallRow2.xMin;
				   currWSLoc.y=smallRow2.yMin;
				   currWSLoc.x -= currWSWidth;
				   whiteSpaceLocs.push_back(currWSLoc);
				   smallRow2.xMin -= currWSWidth;
				   ++whiteSpaceCtr;
				   ++totalMovables2;
				   if(totalMovables2 == windowSize2)
				     break;
				 }
			     }
			 }
		     }
		   else
		     {
		       cellIds.push_back(*(subrow2.cellIdsBegin()+nodeCtr2));
		       movables.push_back(cellIds);
		       cellIds.pop_back();
		       smallRow2.xMin=
			 rbplace[*(subrow2.cellIdsBegin()+nodeCtr2)].x;
		       --nodeCtr2;
		       ++totalMovables2;
		       if(totalMovables2 == windowSize2)
			 break;		  
		       
		       //if whitespace present after last cell
		       double currWSWidth=subrow2.getXStart() - smallRow2.xMin;
		       Point currWSLoc;
		       if(currWSWidth >= 2*smallRow2.siteInterval)
			 {
			   numSitesWS = int(currWSWidth/smallRow2.siteInterval);
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   sitesPerWS = int(floor(numSitesWS/2+0.5));
			   numSitesWS -= sitesPerWS;
			   whiteSpaceWidths.push_back(sitesPerWS*
						      smallRow2.siteInterval);
			   currWSLoc.x=smallRow2.xMin;
			   currWSLoc.y=smallRow2.yMin;
			   currWSLoc.x -= sitesPerWS*smallRow2.siteInterval ;
			   whiteSpaceLocs.push_back(currWSLoc);
			   smallRow2.xMin -= sitesPerWS*smallRow2.siteInterval;
			   currWSWidth -= sitesPerWS*smallRow2.siteInterval;
			   ++whiteSpaceCtr;
			   ++totalMovables2;
			   if(totalMovables2 == windowSize2)
			     break;
			   
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   sitesPerWS = numSitesWS;
			   whiteSpaceWidths.push_back(sitesPerWS*
						      smallRow2.siteInterval);
			   currWSLoc.x -= sitesPerWS*smallRow2.siteInterval ;
			   whiteSpaceLocs.push_back(currWSLoc);
			   smallRow2.xMin -= sitesPerWS*smallRow2.siteInterval;
			   currWSWidth -= sitesPerWS*smallRow2.siteInterval;
			   ++whiteSpaceCtr;
			   ++totalMovables2;
			   if(totalMovables2 == windowSize2)
			     break;
			 }
		       else if(currWSWidth > 0 && 
			       currWSWidth < 2*smallRow2.siteInterval)
			 {
			   cellIds.push_back(UINT_MAX);
			   movables.push_back(cellIds);
			   cellIds.pop_back();
			   whiteSpaceWidths.push_back(currWSWidth);
			   currWSLoc.x=smallRow2.xMin;
			   currWSLoc.y=smallRow2.yMin;
			   currWSLoc.x -= currWSWidth;
			   whiteSpaceLocs.push_back(currWSLoc);
			   smallRow2.xMin -= currWSWidth;
			   ++whiteSpaceCtr;
			   ++totalMovables2;
			   if(totalMovables2 == windowSize2)
			     break;
			 }
		     }
		 }
	       smallPbRows.push_back(smallRow2);
	     }

	   if(movables.size() < 2)
	     {
	       if(nodeCtr1 == -1)
		 break;
	       lastCell1=nodeCtr1;
	       continue;
	     }

	   currCellLoc1=nodeCtr1;
	   currCellLoc2=nodeCtr2;

	   //assume all the cells are placed
	   //assume all cells are not fixed
	   vector< vector<unsigned> > movablesLR;

	   vector<double> whiteSpaceWidthsLR;
	   vector<Point> whiteSpaceLocsLR;
	   unsigned i;
	   for(i=1;i<=movables.size();++i)
	     {
	       movablesLR.push_back(movables[movables.size()-i]);
	       /*
	       if(movables[i-1][0] != UINT_MAX)
		 cout<<hgWDims.getNodeByIdx(movables[i-1][0])<<endl;
	       else
		 cout<<UINT_MAX<<endl;
	       */
	     }
	   for(i=1;i<=whiteSpaceWidths.size();++i)
	     {
	       whiteSpaceWidthsLR.push_back(whiteSpaceWidths
					    [whiteSpaceWidths.size()-i]);
	     }
	   for(i=1;i<=whiteSpaceLocs.size();++i)
	     {
	       whiteSpaceLocsLR.push_back(whiteSpaceLocs
					    [whiteSpaceLocs.size()-i]);
	     }

	   bit_vector fixed(movablesLR.size(),0);
	   for(unsigned i=0; i<movablesLR.size(); ++i)
	     {
	       bool oneFixedCell=false;
	       for(unsigned j=0; j<movablesLR[i].size(); ++j)
		 {
		   if(rbplace.isFixed(movablesLR[i][j]))
		     {
		       oneFixedCell = true;
		       break;
		     }
		 }
	       if(oneFixedCell)
		 fixed[i] = true;
	     }

	   //reverse the smallRows order to confirm with what SmallPlacer
	   //expects
	   std::reverse(smallPbRows.begin(),smallPbRows.end());

           SmallPlacementProblem smallProblem(hgWDims, rbplace,
            placed, fixed, movablesLR, smallPbRows, whiteSpaceWidthsLR);

           SmallPlacementSolution& solution=smallProblem.getSolnBuffer()[0];
           solution.populated=true;

	   unsigned currWSLocation=0;
	   for(unsigned cell=0; cell!=movablesLR.size(); cell++)
	     {
	       if(movablesLR[cell][0] != UINT_MAX)
		 {
		   solution.placement[cell]   =rbplace[movablesLR[cell]
						  [movablesLR[cell].size()-1]];
		   solution.orientations[cell]=rbplace.getOrient(movablesLR[cell]
						  [movablesLR[cell].size()-1]);
		 }
	       else
		 {
		   Point WSLoc = whiteSpaceLocsLR[currWSLocation];
		   ++currWSLocation;
		   solution.placement[cell] = WSLoc;
		 }
	     }
	   if(smallProblem.getNumNets() > 63)
	     {
	       abkwarn(0,"skipping problem with > 63 nets\n");
	       lastCell1 = nodeCtr1;
	       continue;
	     }
	   //get the no: of cells in initial instance
	   unsigned inst1PrevSize=0;
	   for(unsigned i=0;i<movablesLR.size();++i)
	     {
	       if(movablesLR[i][0] != UINT_MAX)
		 if(solution.placement[i].y == subrow1.getYCoord())
		   {
		     ++inst1PrevSize;
		   }
	     }

	   //invoke the SmallPlacer to solve the problem
	   //smallProblem.plot("SmPbIn");
	   //smallProblem.save("SmPbIn");
	   SmallPlacer placer(smallProblem,params.smplParams);
	   //smallProblem.plot("SmPbOut");
	   //smallProblem.save("SmPbOut");

	   //get best solution
	   const Placement& soln = smallProblem.getBestSoln().placement;
	   abkassert(soln.getSize() == movables.size(),"undersized placement");
	   
	   //rbplace.saveAsPlot("plotWNames",-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,fileName3);
	   //double initHPWL = rbplace.evalHPWL(true);
	   
	   rbplace.updateIronedCellsRL(movablesLR,soln,
				     const_cast<RBPSubRow&>(subrow1),
				     const_cast<RBPSubRow&>(subrow2) );
	   
	   //double finalHPWL = rbplace.evalHPWL(true);
	   /*
	   if(finalHPWL > initHPWL)
	     {
	       cout<<"increase in HPWL is "<<finalHPWL-initHPWL<<endl;
	       rbplace.saveAsPlot("plotWNames",-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,"RBPlOut");
	       abkfatal(0,"bye\n");
	     }
	   */
	   nodeCtr1=lastCell1;
	   //get the no: of cells in the final instance
	   lastCell1 = lastCell1 - inst1PrevSize;
	   unsigned inst1Size=0;
	   for(unsigned i=0;i<movablesLR.size();++i)
	     {
	       if(movablesLR[i][0] != UINT_MAX)
		 if(soln[i].y == subrow1.getYCoord())
		   {
		     ++inst1Size;
		     --lastCell1;
		   }
	     }
	   lastCell1 = lastCell1 + inst1Size;
	   nodeCtr1=lastCell1;
	   	   
	   //to avoid a deadlock condition

	   unsigned overlap=0;
	   if(inst1Size == 0)
	     overlap = 0;
	   else if(params.overlap >= inst1Size)
	     overlap = inst1Size-1;
	   else
	     overlap = params.overlap;
	   if(nodeCtr1 > int(lastCell1 + overlap))
	     lastCell1 = (lastCell1 + overlap);
	   
	   totalCells1=subrow1.getNumCells();
	   totalCells2=subrow2.getNumCells();
	 }

       if(sIt1 == rIt1->subRowsBegin() || sIt2 == rIt2->subRowsBegin())
	 break;
       sIt1--;
       sIt2--;
     }
     
     if(verticalDir == 0)
       {
	 ++rIt1;
	 if(rIt1 == rbplace.coreRowsEnd())
	   break;
       }
     else
       {
	 if(rIt1 == rbplace.coreRowsBegin())
	   break;
	 else
	   --rIt1;
       }
   }
}




