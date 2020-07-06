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








//by sadya
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <ctype.h>
#include "ABKCommon/pathDelims.h"
#include "Geoms/bbox.h"
#include "RBPlacement.h"

void RBPlacement::saveAsPlot(const char* command,double xmin,double xmax,
			     double ymin,double ymax,
			     const char* baseFileName) const
{
  vector<unsigned> nodes;
  for(unsigned i=0; i<_hgWDims->getNumNodes(); i++)
    {
      if(_placement[i].x < xmax && _placement[i].x > xmin &&
	 _placement[i].y < ymax && _placement[i].y > ymin)
	nodes.push_back(i);
    }
  plot(command, nodes, xmin, xmax, ymin, ymax, baseFileName);
  nodes.clear();
}

void RBPlacement::plotDummy(const char* command, double xmin, double xmax, 
		      double ymin, double ymax, const char* baseFileName) const
{
  vector<unsigned> nodes;
  for(unsigned i=0; i<_hgWDims->getNumNodes(); i++)
    {
      const char* nodeName = _hgWDims->getNodeByIdx(i).getName();
      if (strstr(nodeName,"dummy"))
	nodes.push_back(i);
    }
  plot(command, nodes, xmin, xmax, ymin, ymax, baseFileName);
}

void RBPlacement::plotGrpConstr(const char* command, double xmin, double xmax, 
		      double ymin, double ymax, const char* baseFileName) const
{
  vector<unsigned> nodes;
  for(unsigned i=0; i<groupRegionConstr.size(); ++i)
    {
      for(unsigned j=0; j<groupRegionConstr[i].second.size(); ++j)
	{
	  nodes.push_back(groupRegionConstr[i].second[j]);
	}
    }
  plot(command, nodes, xmin, xmax, ymin, ymax, baseFileName);
}

void RBPlacement::plot(const char* command,vector<unsigned>& nodes, 
		       double xmin, double xmax, double ymin, double ymax, 
		       const char* baseFileName) const
{
  cout<<"Saving "<<baseFileName<<".gpl"<<endl;

  char* fname = new char[strlen(baseFileName)+5];
  strcpy(fname, baseFileName);
  strcat(fname, ".gpl");
  ofstream gpFile(fname);
  unsigned i=0;
  double xCentre=0,yCentre=0;
  double rowHeight=0;
  double nodeWidth=0;
  double rowLength=1;
  vector<double> xcoord;
  vector<double> ycoord;

  Point minRange;
  Point maxRange;

  minRange.x  = xmin;
  maxRange.x  = xmax;
  minRange.y  = ymin;
  maxRange.y  = ymax;

  gpFile<<"#Use this file as a script for gnuplot"<<endl;
  gpFile<<"#(See http://www.cs.dartmouth.edu/gnuplot_info.html for details)"
	<<endl;
  gpFile<<"#"<< TimeStamp() << User() << Platform() << endl << endl;
  gpFile << "set title \" "<<baseFileName<<" HPWL= "<<evalHPWL(true)
	 <<", #Cells= "<<getNumCells()<<", #Nets= "<<_hgWDims->getNumEdges()
	 <<" \" font \"Times , 40\""
	 <<endl<<endl;
  gpFile << "set nokey"<<endl;

  gpFile<<"#   Uncomment these two lines starting with \"set\""<<endl ;
  gpFile<<"#   to save an EPS file for inclusion into a latex document"<<endl;
  gpFile << "# set terminal postscript eps color 20"<<endl;
  gpFile << "# set output \""<<baseFileName<<".eps\""<<endl<<endl;

  gpFile<<"#   Uncomment these two lines starting with \"set\""<<endl ;
  gpFile<<"#   to save a PS file for printing"<<endl;
  gpFile<<"# set terminal postscript portrait color 20"<<endl;
  gpFile << "# set output \""<<baseFileName<<".ps\""<<endl<<endl<<endl;

  if(strcmp(command,"plotNodesWNames")==0 ||
     strcmp(command,"plotWNames")==0)
    {
      for(i=0; i<nodes.size(); i++)
	{
	  unsigned index = nodes[i];
	  gpFile<<"set label \""<<_hgWDims->getNodeByIdx(index).getName()
		<<"\" at "
		<< _placement[index].x+_hgWDims->getNodeWidth(index)/4
		<<" , "<<_placement[index].y+_hgWDims->getNodeHeight(index)/3
		<<endl;
	}
    }
  gpFile << endl<<endl;

  gpFile << " plot[";
  if(minRange.x>-DBL_MAX)        gpFile<<minRange.x;
  gpFile<<":";
  if(maxRange.x<DBL_MAX)        gpFile<<maxRange.x;
  gpFile<<"][";
  if(minRange.y>-DBL_MAX)        gpFile<<minRange.y;
  gpFile<<":";
  if(maxRange.y<DBL_MAX)        gpFile<<maxRange.y;
  gpFile<<"]";
  if(strcmp(command,"plotNets")==0)
    gpFile<<" '-' w l ";
  if(strcmp(command,"plotNodes")==0)
    gpFile << " '-' w l 2, '-' w l 3 ";
  if(strcmp(command,"plot")==0)
    gpFile << " '-' w l, '-' w l 2, '-' w l 3 ";
  if(strcmp(command,"plotNodesWNames")==0)
    gpFile << " '-' w l 2, '-' w l 3 ";
  if(strcmp(command,"plotWNames")==0)
    gpFile << " '-' w l, '-' w l 2, '-' w l 3 ";
  if(strcmp(command,"plotSites")==0)
    gpFile << " '-' w l 4 ";
  if(strcmp(command,"plotNodesWSites")==0)
    gpFile << " '-' w l 2, '-' w l 3, '-' w l 4 ";
  if(strcmp(command,"plotRows")==0)
    gpFile << " '-' w l 4 ";

  gpFile << endl<<endl;

  if(strcmp(command,"plotNets")==0 || strcmp(command,"plot")==0
     || strcmp(command,"plotWNames")==0)
    {
      // the hyperedges info goes inside
      vector<bool> seenNets(_hgWDims->getNumEdges(), false);
      const char* cellName ;
      unsigned netDegree = 0;
      Point lastRowPoint;
      
      for(unsigned k=0; k<nodes.size(); ++k)
	{
	  unsigned index = nodes[k];
	  const HGFNode& node=_hgWDims->getNodeByIdx(index);

	  for(itHGFEdgeLocal e = node.edgesBegin();e != node.edgesEnd();e++)
	    {
	      const HGFEdge& edge=(**e);
	      unsigned edgeId =  edge.getIndex();
	      if(!seenNets[edgeId])
		{
		  seenNets[edgeId] = true;
		  netDegree=edge.getDegree();
		  gpFile<<"#NetDegree : "<<netDegree<<endl;

		  xcoord.resize(netDegree);
		  ycoord.resize(netDegree);
		  unsigned nodeOffset = 0;

		  itHGFNodeLocal v;
		  unsigned j=0;

		  unsigned withinRange=0;
		  unsigned status=0;
		  xCentre = 0;
		  yCentre = 0;
		  double ycordRow = 0;

		  // for all nodes
		  for (v=edge.nodesBegin(),j=0; v!=edge.nodesEnd(); 
		       v++, nodeOffset++)
		    {
		      cellName = (*v)->getName();
		      gpFile<<"#"<<setw(10)<<cellName<<" O "<<endl;
		      HGFNode& node = _hgWDims->getNodeByName(cellName);
		      i=node.getIndex();
		      Point tempRowPoint(_placement[i].x,_placement[i].y);
		      lastRowPoint=tempRowPoint;
		      
		      if(j==0)  //ycordRow gives the ycord of first cell of row
			ycordRow=_placement[i].y; //used to determine if cells in 1 row
		      
		      Point pOffset = _hgWDims->nodesOnEdgePinOffset(nodeOffset
					     , edgeId,_placement.getOrient(i));
		      
		      ycoord[j]= _placement[i].y+pOffset.y;
		      xcoord[j] = _placement[i].x+pOffset.x;
		      rowHeight=_hgWDims->getNodeHeight(i);
		      nodeWidth=_hgWDims->getNodeWidth(i);
		      
		      if((_placement[i].x>minRange.x && 
			  _placement[i].x<maxRange.x)
			 &&(_placement[i].y>minRange.y && 
			    _placement[i].y<maxRange.y))
			withinRange=1;   //atleast 1 cell of Hedge is within range
		      //so plot the whole net
		      if(ycordRow!=_placement[i].y)
			{
			  status = 1;   //all cells of Hedge are not in single row
			}
		      
		      xCentre += xcoord[j];
		      yCentre += ycoord[j];
		      j++;
		    }
		  if(withinRange == 1)
		    {
		      if(status==1)
			{
			  xCentre /= netDegree;
			  yCentre /= netDegree;
			}
		      else
			{
			  const RBPCoreRow* rowName;
			  rowName = findCoreRow(lastRowPoint);
			  if(rowName!=NULL)
			    rowLength = fabs(rowName->getXEnd()
					     - rowName->getXStart());
			  double maxXcoord=0;
			  double minXcoord=0;
			  
			  minXcoord=
			    *std::min_element(xcoord.begin(),xcoord.end());
			  maxXcoord=
			    *std::max_element(xcoord.begin(),xcoord.end());
			  double ydifference = maxXcoord-minXcoord;
			  double rd = RandomDouble(-1,1);
			  
			  int starOrient=0;
			  starOrient = ((rd>0) ? 1:-1);//get orientation of star point
			  
			  rd = RandomDouble(-1,1);
			  ydifference = (1-exp(0-ydifference*ydifference/
				        (rowLength)))*rowHeight*0.4*starOrient;
			  yCentre = ycoord[0]+ydifference;
			  xCentre = (maxXcoord+minXcoord)/2+rd*nodeWidth*0.25;
			}
		      if(status == 0 || netDegree != 2)
			{
			  if(abs(xCentre)<1e+306 && abs(yCentre)<1e+306)
			    {
			      gpFile << setw(10) << xCentre << "  "
				     << setw(10) << yCentre+0.025*rowHeight 
				     << "  "<<endl;
			      gpFile << setw(10) << xCentre+0.025*rowHeight 
				     << "  "<< setw(10) << yCentre << "  "
				     <<endl;
			      gpFile << setw(10) << xCentre << "  "
				     << setw(10) << yCentre-0.025*rowHeight 
				     << "  "<<endl;
			      gpFile << setw(10) << xCentre-0.025*rowHeight 
				     << "  "
				     << setw(10) << yCentre << "  "<<endl;
			      gpFile << setw(10) << xCentre << "  "
				     << setw(10) << yCentre+0.025*rowHeight 
				     <<"  "<<endl<<endl;
			    }
			}
		      
		      for(i=0;i<netDegree;++i)
			{
			  if(abs(xcoord[i])<DBL_MAX && abs(ycoord[i])<DBL_MAX &&
			     abs(xCentre)<DBL_MAX && abs(yCentre)<DBL_MAX)
			    {
			      gpFile << setw(10) << xCentre << "  "
				     << setw(10) << yCentre << "  "<<endl;
			      gpFile << setw(10) << xcoord[i] << "  "
				     << setw(10) << ycoord[i] << "  "<<endl;
			      gpFile << setw(10) << xCentre << "  "
				     << setw(10) << yCentre << "  "
				     <<endl<<endl;
			    }
			}
		    }
		}
	    }
	}
      gpFile <<"EOF"<<endl;
    }

  //added by sadya to plot nodes
  //plot only the fixed nodes and large cells now 
  double maxCRHeight= getMaxHeightCoreRow();
  if(strcmp(command,"plotNodes")==0 || strcmp(command,"plot")==0 ||
     strcmp(command,"plotNodesWNames")==0 ||strcmp(command,"plotWNames")==0 ||
     strcmp(command,"plotNodesWSites")==0)
    {
      double cellWidth=0;
      double cellHeight=0;
      unsigned j;
      for(j=0; j<nodes.size(); j++)
	{
	  i = nodes[j];
	  Orient nodeOrient = _placement.getOrient(i);
	  if(nodeOrient == Orient("N") || nodeOrient == Orient("S") ||
	     nodeOrient == Orient("FN") || nodeOrient == Orient("FS"))
	    {
	      cellWidth=_hgWDims->getNodeWidth(i);
	      cellHeight=_hgWDims->getNodeHeight(i)*0.9;
	    }
	  else
	    {
	      cellWidth=_hgWDims->getNodeHeight(i);
	      cellHeight=_hgWDims->getNodeWidth(i)*0.9;
	    }
	  
	  if (_hgWDims->isTerminal(i) || cellHeight > maxCRHeight)
	  {
	  double offset = 0;
	  offset = ((cellWidth > cellHeight) ? 0.1*cellHeight:0.1*cellWidth);
	  gpFile <<"# Cell Name "<<_hgWDims->getNodeByIdx(i).getName()<<endl;

	  if(abs(_placement[i].x) < 1e+306 && abs(_placement[i].y) < 1e+306)
	    {
	      gpFile << setw(10) << _placement[i].x << "  "
		     << setw(10) << _placement[i].y+offset+offset << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x+offset << "  "
		     << setw(10) << _placement[i].y+offset << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x+cellWidth-offset << "  "
		     << setw(10) << _placement[i].y+offset << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x+cellWidth << "  "
		     << setw(10) << _placement[i].y+offset+offset << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x+cellWidth << "  "
		     << setw(10) << _placement[i].y+cellHeight-offset << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x+cellWidth-offset << "  "
		     << setw(10) << _placement[i].y+cellHeight << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x+offset << "  "
		     << setw(10) << _placement[i].y+cellHeight << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x << "  "
		     << setw(10) << _placement[i].y+cellHeight-offset << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x << "  "
		     << setw(10) << _placement[i].y+offset+offset << "   "
		     <<endl;
	      gpFile <<endl<<endl;


	      offset = offset*0.8;
	      double innerOffsetX = cellWidth*0.1;
	      double innerOffsetY = cellHeight*0.1;
	      
	      if(cellWidth < cellHeight)
		{
		  innerOffsetX = cellWidth*0.1;
		  innerOffsetY = innerOffsetX;
		}
	      else
		{
		  innerOffsetX = cellHeight*0.1;
		  innerOffsetY = innerOffsetX;
		}

	      gpFile << setw(10) << _placement[i].x+innerOffsetX << "  "
		     << setw(10) << _placement[i].y+innerOffsetY+offset+offset << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x+innerOffsetX+offset << "  "
		     << setw(10) << _placement[i].y+innerOffsetY+offset << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x+cellWidth-innerOffsetX-offset << "  "
		     << setw(10) << _placement[i].y+innerOffsetY+offset << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x+cellWidth-innerOffsetX<< "  "
		     << setw(10) << _placement[i].y+innerOffsetY+offset+offset << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x+cellWidth-innerOffsetX<< "  "
		     << setw(10) << _placement[i].y+cellHeight-innerOffsetY-offset << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x+cellWidth-innerOffsetX-offset << "  "
		     << setw(10) << _placement[i].y+cellHeight-innerOffsetY << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x+innerOffsetX+offset << "  "
		     << setw(10) << _placement[i].y-innerOffsetY+cellHeight << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x+innerOffsetX << "  "
		     << setw(10) << _placement[i].y+cellHeight-innerOffsetY-offset << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x+innerOffsetX << "  "
		     << setw(10) << _placement[i].y+innerOffsetY+offset+offset << "   "
		     <<endl;
	      gpFile <<endl<<endl;
	      
	    }
	  gpFile<<endl<<endl;
	  }
	}
      gpFile << "EOF"<<endl<<endl;
    }


  //added by sadya to plot nodes
  //plot the actual nodes now. terminals and macros are not plotted
  if(strcmp(command,"plotNodes")==0 || strcmp(command,"plot")==0 ||
     strcmp(command,"plotNodesWNames")==0 ||strcmp(command,"plotWNames")==0 ||
     strcmp(command,"plotNodesWSites")==0)
    {
      double cellWidth=0;
      double cellHeight=0;
      unsigned j;
      for(j=0; j<nodes.size(); j++)
	{
	  i = nodes[j];
	  Orient nodeOrient = _placement.getOrient(i);
	  if(nodeOrient == Orient("N") || nodeOrient == Orient("S") ||
	     nodeOrient == Orient("FN") || nodeOrient == Orient("FS"))
	    {
	      cellWidth=_hgWDims->getNodeWidth(i);
	      cellHeight=_hgWDims->getNodeHeight(i)*0.9;
	    }
	  else
	    {
	      cellWidth=_hgWDims->getNodeHeight(i);
	      cellHeight=_hgWDims->getNodeWidth(i)*0.9;
	    }
	  if (!_hgWDims->isTerminal(i) && cellHeight <= maxCRHeight)
	  {
	  double offset = 0;
	  offset = ((cellWidth > cellHeight) ? 0.1*cellHeight:0.1*cellWidth);
	  gpFile <<"# Cell Name "<<_hgWDims->getNodeByIdx(i).getName()<<endl;

	  if(abs(_placement[i].x) < 1e+306 && abs(_placement[i].y) < 1e+306)
	    {
	      gpFile << setw(10) << _placement[i].x << "  "
		     << setw(10) << _placement[i].y+offset+offset << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x+offset << "  "
		     << setw(10) << _placement[i].y+offset << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x+cellWidth-offset << "  "
		     << setw(10) << _placement[i].y+offset << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x+cellWidth << "  "
		     << setw(10) << _placement[i].y+offset+offset << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x+cellWidth << "  "
		     << setw(10) << _placement[i].y+cellHeight-offset << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x+cellWidth-offset << "  "
		     << setw(10) << _placement[i].y+cellHeight << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x+offset << "  "
		     << setw(10) << _placement[i].y+cellHeight << "   "
		     <<endl;
	      gpFile << setw(10) << _placement[i].x << "  "
		     << setw(10) << _placement[i].y+cellHeight-offset << "   "
		     <<endl;

	      gpFile << setw(10) << _placement[i].x << "  "
		     << setw(10) << _placement[i].y+offset+offset << "   "
		     <<endl;
	      gpFile <<endl<<endl;
	    }
	  }
	}
      gpFile << "EOF"<<endl<<endl;
    }

  //plot the site map now.
  if(strcmp(command,"plotSites")==0 || strcmp(command,"plotNodesWSites")==0)
    {
      double rowYCoord=0;
      double siteWidth=0;
      double rowHeight=0;
      double rowSpacing=0;
      unsigned numSitesSubRow=0;

      itRBPCoreRow CR;
      itRBPSubRow SR;
      for(CR=coreRowsBegin();CR!=coreRowsEnd();++CR)
	{
	  rowYCoord=(CR)->getYCoord();
	  siteWidth=(CR)->getSiteWidth();
	  rowHeight=(CR)->getHeight();
	  rowSpacing=(CR)->getSpacing();

	  for(SR=(CR)->subRowsBegin();SR!=(CR)->subRowsEnd();++SR)
	    {
	      double xStart = SR->getXStart();
	      numSitesSubRow=(SR)->getNumSites();
	      for(i=0;i<numSitesSubRow;++i)
		{
		  double siteXstart=xStart+i*rowSpacing;
		  double siteXend=siteXstart+siteWidth;

		  gpFile<<setw(10)<<siteXstart<<"  " <<(rowYCoord+0.1*rowHeight)
			<<endl;
		  gpFile<<setw(10)<<siteXstart+0.1*siteWidth<<"  "
			<<(rowYCoord)<<endl;
		  gpFile<<setw(10)<<siteXend-0.1*siteWidth<<"  "
			<<(rowYCoord)<<endl;
		  gpFile<<setw(10)<<siteXend<<"  " <<(rowYCoord+0.1*rowHeight)
			<<endl<<endl;
		}
	    }
	}
      gpFile << "EOF"<<endl;
    }

  //plot the row map now.
  if(strcmp(command,"plotRows")==0)
    {
      double rowYCoord=0;
      double siteWidth=0;
      double rowHeight=0;
      double rowSpacing=0;
      unsigned numSitesSubRow=0;

      itRBPCoreRow CR;
      itRBPSubRow SR;
      for(CR=coreRowsBegin();CR!=coreRowsEnd();++CR)
	{
	  rowYCoord=(CR)->getYCoord();
	  siteWidth=(CR)->getSiteWidth();
	  rowHeight=(CR)->getHeight();
	  rowSpacing=(CR)->getSpacing();
	  for(SR=(CR)->subRowsBegin();SR!=(CR)->subRowsEnd();++SR)
	    {
	      double xStart = SR->getXStart();
	      numSitesSubRow=(SR)->getNumSites();
	      double xEnd=xStart+numSitesSubRow*siteWidth;

	      gpFile<<setw(10)<<xStart<<"  " <<(rowYCoord+0.1*rowHeight)
		    <<endl;
	      gpFile<<setw(10)<<xStart+0.1*siteWidth<<"  "
		    <<(rowYCoord)<<endl;
	      gpFile<<setw(10)<<xEnd-0.1*siteWidth<<"  "
		    <<(rowYCoord)<<endl;
	      gpFile<<setw(10)<<xEnd<<"  " <<(rowYCoord+0.1*rowHeight)
		    <<endl<<endl;
	    }
	}
      gpFile << "EOF"<<endl;
    }
  gpFile << "pause -1 'Press any key' "<<endl;
  gpFile.close();
}

struct ctrSortWSMap
{
  unsigned horizIdx;
  unsigned vertIdx;
  double WS;
};

struct wsMap_less_mag
{
  bool operator()(const ctrSortWSMap &elem1, const ctrSortWSMap &elem2)
  { return(elem1.WS < elem2.WS); }
};

void RBPlacement::plotWSHist(const char * baseFileName)
{
  cout<<"Saving "<<baseFileName<<".gpl"<<endl;

  char* fname = new char[strlen(baseFileName)+5];
  strcpy(fname, baseFileName);
  strcat(fname, ".gpl");
  ofstream gpFile(fname);

  const HGraphWDimensions& hgWDims = getHGraph();
  unsigned numCells = hgWDims.getNumNodes()-hgWDims.getNumTerminals();
  BBox layoutBBox = getBBox(false);
  double xSize = layoutBBox.xMax - layoutBBox.xMin;
  double ySize = layoutBBox.yMax - layoutBBox.yMin;
  double horizGridSize = 10*sqrt(xSize*ySize/numCells);//try to have 100 cells
  double vertGridSize = horizGridSize;                //in each grid box

  double rowSpacing = (coreRowsBegin()+1)->getYCoord() - 
                       coreRowsBegin()->getYCoord();
  horizGridSize = ceil(horizGridSize/rowSpacing)*rowSpacing;
  vertGridSize = horizGridSize;

  if(horizGridSize > xSize)
    horizGridSize = xSize/2;
  if(vertGridSize > ySize)
    vertGridSize = ySize/2;

  unsigned maxHorizIdx = unsigned(ceil(xSize/horizGridSize));
  unsigned maxVertIdx = unsigned(ceil(ySize/vertGridSize));

  vector< vector<double> > wsMap(maxVertIdx);
  for(unsigned i=0 ; i<maxVertIdx; ++i)
    {
      wsMap[i].resize(maxHorizIdx, 0);
    }
  vector<ctrSortWSMap> sortWSBins;
  sortWSBins.resize(maxVertIdx*maxHorizIdx);

  for(unsigned i=hgWDims.getNumTerminals(); i<hgWDims.getNumNodes(); ++i)
    {
//    const HGFNode& node=hgWDims.getNodeByIdx(i);
      BBox nodeBBox;
      nodeBBox.xMin = _placement[i].x;
      nodeBBox.yMin = _placement[i].y;
      nodeBBox.xMax = nodeBBox.xMin+hgWDims.getNodeWidth(i);
      nodeBBox.yMax = nodeBBox.yMin+hgWDims.getNodeHeight(i);

      if(nodeBBox.xMin < layoutBBox.xMin)
	nodeBBox.xMin = layoutBBox.xMin;
      if(nodeBBox.xMax > layoutBBox.xMax)
        nodeBBox.xMax = layoutBBox.xMax;
      if(nodeBBox.yMin < layoutBBox.yMin)
        nodeBBox.yMin = layoutBBox.yMin;
      if(nodeBBox.yMax > layoutBBox.yMax)
        nodeBBox.yMax = layoutBBox.yMax;
      if(nodeBBox.xMax < layoutBBox.xMin)
        nodeBBox.xMax = layoutBBox.xMin;
      if(nodeBBox.xMin > layoutBBox.xMax)
        nodeBBox.xMin = layoutBBox.xMax;
      if(nodeBBox.yMax < layoutBBox.yMin)
        nodeBBox.yMax = layoutBBox.yMin;
      if(nodeBBox.yMin > layoutBBox.yMax)
        nodeBBox.yMin = layoutBBox.yMax;

      int minHorizIndex =
	int(floor((nodeBBox.xMin-layoutBBox.xMin)/horizGridSize));
      int maxHorizIndex =
	int(ceil((nodeBBox.xMax-layoutBBox.xMin)/horizGridSize));
      int minVertIndex =
	int(floor((nodeBBox.yMin-layoutBBox.yMin)/vertGridSize));
      int maxVertIndex =
	int(ceil((nodeBBox.yMax-layoutBBox.yMin)/vertGridSize));

      for(int k=minVertIndex ;k<maxVertIndex; ++k)
	{
	  for(int j=minHorizIndex; j<maxHorizIndex; ++j)
	    {
	      double regXMin = j*horizGridSize+layoutBBox.xMin;
	      double regXMax = (j+1)*horizGridSize+layoutBBox.xMin;
	      double regYMin = k*vertGridSize+layoutBBox.yMin;
	      double regYMax = (k+1)*vertGridSize+layoutBBox.yMin;

	      if(regXMax > layoutBBox.xMax)
		regXMax = layoutBBox.xMax;
	      if(regYMax > layoutBBox.yMax)
		regYMax = layoutBBox.yMax;

	      BBox region;
	      region.add(regXMin,regYMin);
	      region.add(regXMax,regYMax);

	      BBox intersect = nodeBBox.intersect(region);
	      double area = intersect.getHeight()*intersect.getWidth();
	      wsMap[k][j] += area;
	    }
	}
    }

  for(unsigned k=0 ;k<maxVertIdx; ++k)
    {
      for(unsigned j=0; j<maxHorizIdx; ++j)
	{
	  double regXMin = j*horizGridSize+layoutBBox.xMin;
	  double regXMax = (j+1)*horizGridSize+layoutBBox.xMin;
	  double regYMin = k*vertGridSize+layoutBBox.yMin;
	  double regYMax = (k+1)*vertGridSize+layoutBBox.yMin;

	  if(regXMax > layoutBBox.xMax)
	    regXMax = layoutBBox.xMax;
	  if(regYMax > layoutBBox.yMax)
	    regYMax = layoutBBox.yMax;

	  BBox region;
	  region.add(regXMin,regYMin);
	  region.add(regXMax,regYMax);
	  
	  //double regionArea = region.getArea();
	  double regionArea = getContainedSiteAreaInBBox(region);
	  double regionWS=1;
	  if(regionArea != 0)
	    regionWS = (regionArea-wsMap[k][j])/regionArea;
	  wsMap[k][j] = regionWS;
	}
    }  
  
  for(unsigned i=0; i<maxVertIdx; ++i)
    {
      for(unsigned j=0; j<maxHorizIdx; ++j)
        {
          unsigned idx = i*maxHorizIdx+j;
          sortWSBins[idx].horizIdx = j;
          sortWSBins[idx].vertIdx = i;
          sortWSBins[idx].WS = wsMap[i][j];
        }
    }
  sort(sortWSBins.begin(), sortWSBins.end(), wsMap_less_mag());

  int numWSBins = maxVertIdx*maxHorizIdx;

  cout<<"Whitespace Map Statistics "<<endl;
  cout<<"Core Layout BBox "<<layoutBBox;
  cout<<"Whitespace grid is "<<maxVertIdx<<" x "<<maxHorizIdx<<" = "
      <<numWSBins<<" bins"<<endl;
  double maxWS = sortWSBins[numWSBins-1].WS;
  double minWS = sortWSBins[0].WS;
  double avgWS = 0;
  double medianWS = sortWSBins[int(numWSBins/2)].WS;

  for(int i = 0 ; i<numWSBins; ++i)
    avgWS += sortWSBins[i].WS;
  avgWS /= numWSBins;

  int numLevels = 20;
  double rangeLevel = 1.0/double(numLevels);
  vector<int> binsPerLevel(numLevels,0);

  double currLevel = 0+rangeLevel;
  int currLevelIdx = 0;
  int currIdx = 0;
  while (currIdx < numWSBins)
    {
      while(currIdx < numWSBins)
        {
          if(sortWSBins[currIdx].WS <= currLevel)
            {
              ++binsPerLevel[currLevelIdx];
              ++currIdx;
            }
          else
            break;
        }
      if(currLevelIdx < numLevels - 1)//boundary case because of numerical pbm
	++currLevelIdx;
      currLevel += rangeLevel;
    }
  
  for(int i=0; i<numLevels; ++i)
    {
      double minLevel = i*100/numLevels;
      double maxLevel = (i+1)*100/numLevels;

      double percentBinsInLevel=100*binsPerLevel[i]/double(numWSBins);
      cout<<"% bins with "<<minLevel<<"% - "<<maxLevel<<"% whitespace : "
          <<percentBinsInLevel<<endl;
    }
  cout<<endl;
  cout<<"Median Whitespace % : "<<100*medianWS<<endl;
  cout<<"Average Whitespace % : "<<100*avgWS<<endl;
  cout<<"Max Whitespace % : "<<100*maxWS<<endl;
  cout<<"Min Whitespace % : "<<100*minWS<<endl;


  //now plot in gnuplot format
  gpFile<<"#Use this file as a script for gnuplot"<<endl;
  gpFile<<"#(See http://www.cs.dartmouth.edu/gnuplot_info.html for details)"
        <<endl;
  //  gpFile<<"#"<< TimeStamp() << User() << Platform() << endl << endl;
  gpFile << "set title \" "<<baseFileName
         <<", #Cells= "<<getNumCells()<<", #Bins= "<<numWSBins
         <<" \" font \"Times , 40\""
         <<endl<<endl;

  gpFile<<"set xlabel \" % Whitespace in Bins\" "<<endl;
  gpFile<<"set ylabel \" % #Bins \" "<<endl;
  gpFile<<"set xtics 10 "<<endl;
  gpFile<<"set ytics 10 "<<endl;
  gpFile << "set nokey"<<endl;

  gpFile<<"#   Uncomment these two lines starting with \"set\""<<endl ;
  gpFile<<"#   to save an EPS file for inclusion into a latex document"<<endl;
  gpFile << "# set terminal postscript eps color 20"<<endl;
  gpFile << "# set output \""<<baseFileName<<".eps\""<<endl<<endl;

  gpFile<<"#   Uncomment these two lines starting with \"set\""<<endl ;
  gpFile<<"#   to save a PS file for printing"<<endl;
  gpFile<<"# set terminal postscript portrait color 20"<<endl;
  gpFile << "# set output \""<<baseFileName<<".ps\""<<endl<<endl<<endl;

  gpFile << " plot[0:100][0:100] '-' w l "<<endl;
  gpFile << endl<<endl;

  for(int i=0; i<numLevels; ++i)
    {
      double minLevel = i*100/numLevels;
      double maxLevel = (i+1)*100/numLevels;

      double percentBinsInLevel=100*binsPerLevel[i]/double(numWSBins);
      gpFile<<"  "<<minLevel<<"  0 "<<endl;
      gpFile<<"  "<<minLevel<<"  "<<percentBinsInLevel<<endl;
      gpFile<<"  "<<maxLevel<<"  "<<percentBinsInLevel<<endl;
      gpFile<<"  "<<maxLevel<<"  0 "<<endl;
      gpFile<<endl;
    }
  gpFile << "EOF"<<endl<<endl;
  gpFile << "pause -1 'Press any key' "<<endl;
  gpFile.close();
}
