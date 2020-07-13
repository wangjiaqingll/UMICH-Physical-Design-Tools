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





//created by Saurabh Adya, Dec 3 2000
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <list>
#include <ABKCommon/sgi_hash_map.h>
#include "ABKCommon/abkcommon.h"
#include "RBPlacement.h"

#ifdef _MSC_VER
#ifndef rint
#define rint(a) floor((a)+0.5)
#endif
#endif


//function to search a list
bool findInList(itRBPSubRow its, list<itRBPSubRow>& seenList)
{
  std::list<itRBPSubRow>::iterator listElem;
  bool foundInList = 0;
  for(listElem = seenList.begin();
      listElem != seenList.end();
      ++listElem)
    if(its == *listElem)
      {
	foundInList = 1;
	break;
      }
  return(foundInList);
}
//added by sadya to remove overlaps in each subrow
void RBPlacement::remOverlapsSR(void)
{
  if (_params.verb.forActions) cout << " Running overlap removal ..." << endl;
  double initHPWL=evalHPWL();
  if (_params.verb.forMajStats)
                   cout << "  HPWL before overlap remover is "<<initHPWL<<endl;
  Timer tm;

  itRBPCoreRow itc;
  itRBPSubRow its;

  unsigned initOverlaps = getNumOverlaps();

  //for each core row, for each subrow scan left to right if overlap found then take action
  for(itc=_coreRows.begin();itc!=_coreRows.end();++itc)
    {
      for(its=itc->subRowsBegin();its!=itc->subRowsEnd();++its)
	shuffleSR(its);
    }
  tm.stop();
  double HPWLafter=evalHPWL();
  double HPWLwPins=evalHPWL(true);
  unsigned finalOverlaps = getNumOverlaps();

  if (_params.verb.forMajStats) 
  {
    cout<<" Removing overlaps took " << tm << endl;
    cout<<"  Discovered " << initOverlaps << " overlaps "<<endl;
    cout<<"  Overlaps remaining: "<<finalOverlaps<<endl;
    cout<<"  HPWL after  overlap removal is "<<HPWLafter<<endl;
    cout<<"  HPWL w pins                 is "<<HPWLwPins<<endl;
    cout<<"  % change in HPWL is "<<(HPWLafter-initHPWL)*100/HPWLafter<<endl;
  }
}

//added by sadya to remove overlaps in each subrow
void RBPlacement::remOverlapsVert(void)
{
  if (_params.verb.forActions) 
    cout << " Running overlap removal with Vertical Moves..." << endl;
  double initHPWL=evalHPWL();
  if (_params.verb.forMajStats)
    cout << "  HPWL before overlap remover is "<<initHPWL<<endl;
  Timer tm;
  
  itRBPCoreRow itc;
  itRBPSubRow its;
  itRBPCellIds itn;
  itRBPCellIds itntemp;
  itRBPCoreRow itcNext;   //used to scan immediate next row
  itRBPSubRow itsNext;    //used to scan immediate next sub row 
  itRBPCellIds itnNext;   //used to scan immediate next sub row cells
  itRBPCellIds itnright;   //used to scan immediate next sub row cells
  itRBPCellIds itnNextLeft; //used to scan 
  itRBPCellIds itnNextRight; //used to scan 
  
  RBPSubRow tempSR;
  
  bool leftRight=false;   //false means move left else move right
  double WS=0;
  unsigned numOverlaps=0;
  unsigned totalOverlaps=0;
  double rowHeight=0;
  double xDiff=0;
  //for each core row, for each subrow scan left to right if overlap found then take action
  for(itc=_coreRows.begin();itc!=_coreRows.end();++itc)
    {
      rowHeight=itc->getHeight();
      for(its=itc->subRowsBegin();its!=itc->subRowsEnd();++its)
	{
	  for(itn=its->cellIdsBegin();itn!=its->cellIdsEnd();++itn)
	    {
	      //check for overlaps in the same row
	      xDiff=1;
	      if(itn!=its->cellIdsEnd()-1)
		{
		  itntemp=itn+1;
		  xDiff= _placement[*itn].x + 
		    _hgWDims->getNodeWidth(*itn)-_placement[*itntemp].x;
		  if(xDiff > 1e-10)
		    {
		      //overlap found, now deal with it
		      numOverlaps++;
		      totalOverlaps++;

		      itnright = itntemp;

		      //check for WS in upper rows
		      if(itc!=_coreRows.end()-1)
			{
			  itcNext = itc+1;
			  for(itsNext=itcNext->subRowsBegin();
			      itsNext!=itcNext->subRowsEnd();++itsNext)
			    {
			      for(itnNext=itsNext->cellIdsBegin();
				  itnNext!=itsNext->cellIdsEnd()-1;++itnNext)
				{
				  //check for WS in the upper adjacent  row
				  if((_placement[*itnNext].x <=
				      _placement[*itn].x) &&
				     (_placement[*(itnNext+1)].x >=
				      _placement[*itn].x))
				    break;   //closest cell found in upper
				             //row
				}
			      itnNextLeft = itnNext;
			      itnNextRight = itnNext;
			      
			      while(itnNextRight!=itsNext->cellIdsEnd()-1 ||
				    itnNextLeft!=itsNext->cellIdsBegin())
				{
				  if(leftRight==false && 
				     itnNextRight!=itsNext->cellIdsEnd()-1)
				    leftRight=true;
				  else if(leftRight==true && 
					  itnNextLeft!=itsNext->cellIdsBegin())
				    leftRight=false;
				  
				  if(leftRight == true)  //search right
				    {
				      itntemp=itnNextRight+1;
				      WS=_placement[*itntemp].x -
					_placement[*itnNextRight].x -
					_hgWDims->getNodeWidth(*itnNextRight);
				      if(WS > 0)   //white space found
					{
					  if(WS >=_hgWDims->getNodeWidth(*itn))
					    {
					      tempSR = *its;
					      tempSR.removeCell(*itn);
					      _placement[*itn].x = 
						_placement[*itnNextRight].x;
					      _placement[*itn].y = 
						itsNext->getYCoord();
					      tempSR = *itsNext;
					      tempSR.addCell(*itn);
					      --numOverlaps;
					      break;
					    }
					  else if
					    (WS>=
					     _hgWDims->getNodeWidth(*itnright))
					    {
					      tempSR = *its;
					      tempSR.removeCell(*itnright);
					      _placement[*itnright].x = 
						_placement[*itnNextRight].x;
					      _placement[*itnright].y = 
						itsNext->getYCoord();
					      tempSR = *itsNext;
					      tempSR.addCell(*itnright);
					      --numOverlaps;
					      break;
					    }
					  else
					    {
					      
					    }
					}
				      ++itnNextRight;
				    }
				  else //search left for WS
				    {
				      itntemp=itnNextLeft-1;
				      WS = _placement[*itnNextLeft].x -
					_placement[*itntemp].x -
					_hgWDims->getNodeWidth(*itntemp);
				      if(WS > 0)   //white space found
					{
					  if(WS >=_hgWDims->getNodeWidth(*itn))
					    {
					      tempSR = *its;
					      tempSR.removeCell(*itn);
					      _placement[*itn].x=
						_placement[*itntemp].x;
					      _placement[*itn].y=
						itsNext->getYCoord();
					      tempSR = *itsNext;
					      tempSR.addCell(*itn);
					      --numOverlaps;
					      break;
					    }
					  else if
					    (WS >= 
					     _hgWDims->getNodeWidth(*itnright))
					    {
					      tempSR = *its;
					      tempSR.removeCell(*itnright);
					      _placement[*itnright].x = 
						_placement[*itntemp].x;
					      _placement[*itnright].y=
						itsNext->getYCoord();
					      tempSR = *itsNext;
					      tempSR.addCell(*itnright);
					      --numOverlaps;
					      break;
					    }
					}
				      itnNextLeft--;
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
  tm.stop();
  double HPWLafter=evalHPWL();
  double HPWLwPins=evalHPWL(true);
  if (_params.verb.forMajStats) 
    {
      cout<<" Removing overlaps took " << tm << endl;
      cout<<"  Discovered " << totalOverlaps << " overlaps "<<endl;
      cout<<"  Overlaps remaining: "<<numOverlaps<<endl;
      cout<<"  HPWL after  overlap removal is "<<HPWLafter<<endl;
      cout<<"  HPWL w pins                 is "<<HPWLwPins<<endl;
      cout<<"  % change in HPWL is "<<(HPWLafter-initHPWL)*100/HPWLafter<<endl;
    }
}

void RBPlacement::snapCellsInSites(void)
{
  itRBPCoreRow itc;
  itRBPSubRow its;
  itRBPCellIds itn;
  //itRBPCellIds itntemp;
  double siteSpacing;
  int index;
  
  for(itc=_coreRows.begin();itc!=_coreRows.end();++itc)
    {
      for(its=itc->subRowsBegin();its!=itc->subRowsEnd();++its)
	{
	  siteSpacing = its->getSpacing();
	  for(itn=its->cellIdsBegin();itn!=its->cellIdsEnd();++itn)
	    {
	      if(int(_placement[*itn].x)%int(siteSpacing) != 0 || 
		 _placement[*itn].x != int(_placement[*itn].x))
		{
		  //cout<<"cell not in site "<<endl;
		  index = *itn;
		  int newPosX = int(_placement[*itn].x) 
		    - int(_placement[*itn].x)%int(siteSpacing);

		  _placement[*itn].x = newPosX;
		}
	    }
	}
    }  
}


void RBPlacement::remOverlaps(void)
{
  if (_params.verb.forActions) cout << " Running overlap removal ..." << endl;
  double initHPWL=evalHPWL();
  if (_params.verb.forMajStats)
                   cout << "  HPWL before overlap remover is "<<initHPWL<<endl;
  Timer tm;

  itRBPCoreRow itc;
  itRBPCoreRow itcNext;   //used to scan immediate next row
  itRBPCoreRow itcCurr;   //used to scan immediate next row
  itRBPSubRow its;
  itRBPSubRow itsCurr;    //used to track current sub row in bfs
  itRBPCellIds itn;
  itRBPCellIds itntemp;
  itRBPCellIds itnToMove;
  itRBPSubRow itsNext;    //used to scan next sub row 
  double rowHeight=0;
  double nodeHeight=0;
  double xDiff=0;
  const unsigned threshold = 50;

  unsigned initOverlaps = getNumOverlaps();

  
  //for each core row, for each subrow scan left to right if overlap found then take action
  for(itc=_coreRows.begin();itc!=_coreRows.end();++itc)
    {
      rowHeight=itc->getHeight();
      for(its=itc->subRowsBegin();its!=itc->subRowsEnd();++its)
	{
	  shuffleSR(its);
	  for(itn=its->cellIdsBegin();itn!=its->cellIdsEnd();++itn)
	    {
	      nodeHeight=_hgWDims->getNodeHeight(*itn);
	      //check for overlaps in the same row
	      xDiff=1;
	      if(itn!=its->cellIdsEnd()-1)
		{
		  itntemp=itn+1;
		  xDiff= _placement[*itn].x + 
		    _hgWDims->getNodeWidth(*itn)-_placement[*itntemp].x;

		  if(xDiff > 1e-10)
		    {
		      //overlap found, now deal with it

		      double srOrigStart = its->getXStart();
		      double srOrigEnd = srOrigStart + its->getNumSites()*
			                               its->getSiteWidth();
		      list<itRBPSubRow> bfsList;
		      list<itRBPSubRow> seenList;
		      bfsList.push_back(its);
		      seenList.push_back(its);

		      //try to move the smallest node
		      if(_hgWDims->getNodeWidth(*itn) > 
			 _hgWDims->getNodeWidth(*(itn+1)))
			itnToMove = itn+1;
		      else
			itnToMove = itn;

		      unsigned cutoffCtr = 0;
		      while(cutoffCtr < threshold && !bfsList.empty())
			{
			  ++cutoffCtr;
			  itsCurr = *(bfsList.begin());
			  bfsList.pop_front();
			  itcCurr = static_cast<itRBPCoreRow>
				      (&itsCurr->getCoreRow());

			  double srStart = itsCurr->getXStart();
			  double srEnd = srStart + itsCurr->getNumSites()*
			                           itsCurr->getSiteWidth();

			  double srWidth = srEnd - srStart;
			  double cellWidths=0;
			  for(itntemp = itsCurr->cellIdsBegin(); 
			      itntemp != itsCurr->cellIdsEnd(); ++itntemp)
			    cellWidths += _hgWDims->getNodeWidth(*itntemp);

			  double widthToMove = 
			    _hgWDims->getNodeWidth(*itnToMove);

//cout<<" overlap x "<<_placement[*itn].x<<" YCoord: "<<itsCurr->getYCoord()<<" XCoords: "<<srStart<<"  "<<srEnd<<" list Size: "<<bfsList.size()<<" widthToMove "<<widthToMove<<" space "<<(srWidth-cellWidths)<<endl;
			  //found sr to move
			  if(widthToMove <= (srWidth-cellWidths)) 
			    {
//cout<<"found subrow "<<widthToMove<<"  "<<(srWidth-cellWidths)<<" "<<_hgWDims->getNodeByIdx(*itnToMove).getName()<<" "<<*itnToMove<<endl;
			      double xLocToMoveTo=0;

			      if(srEnd <= srOrigStart) //move left most cell
				{
				  double leftNodeWidth = 
				  _hgWDims->getNodeWidth(*its->cellIdsBegin());
				  if(leftNodeWidth <= widthToMove)
				    {
				      itnToMove = its->cellIdsBegin();
				      widthToMove = leftNodeWidth;
				    }
				  xLocToMoveTo = srEnd - widthToMove;
				}
			      else if(srStart >= srOrigEnd) //move right cell
				{
				  double rightNodeWidth = 
				  _hgWDims->getNodeWidth(*(its->cellIdsEnd()-1));
				  if(rightNodeWidth <= widthToMove)
				    {
				      itnToMove = its->cellIdsEnd()-1;
				      widthToMove = rightNodeWidth;
				    }
				  xLocToMoveTo = srStart;
				}
			      else//move the current cell
				{
				  if(srEnd <= _placement[*itnToMove].x)
				    xLocToMoveTo = srEnd - widthToMove;
				  else if(srStart >= _placement[*itnToMove].x)
				    xLocToMoveTo = srStart;
				  else
				    xLocToMoveTo = _placement[*itnToMove].x;
				}
		              unsigned indexToMove = *itnToMove;

		              RBPSubRow& tempSR1=const_cast<RBPSubRow&>(*its);
			      tempSR1.removeCell(indexToMove);
			      _placement[indexToMove].x = xLocToMoveTo;
			      _placement[indexToMove].y = itsCurr->getYCoord();
			      RBPSubRow& tempSR2 = 
				const_cast<RBPSubRow&>(*itsCurr);
			      tempSR2.addCell(indexToMove);
			      shuffleSR(itsCurr);
			      shuffleSR(its);
			      break;
			    }

			  //add new elements to bfsList
			  if(itsCurr != itcCurr->subRowsEnd()-1)
			    {
			      itsNext = itsCurr+1;
			      if(findInList(itsNext, seenList) == 0)
				{
				  bfsList.push_back(itsNext);
				  seenList.push_back(itsNext);
				}
			    }
			  if(itsCurr != itcCurr->subRowsBegin())
			    {
			      itsNext = itsCurr-1;
			      if(findInList(itsNext, seenList) == 0)
				{
				  bfsList.push_back(itsNext);
				  seenList.push_back(itsNext);
				}
			    }
			  if(itcCurr != _coreRows.end()-1)
			    {
			      itcNext = itcCurr+1;
			      for(itsNext = itcNext->subRowsBegin();
				  itsNext!=itcNext->subRowsEnd();++itsNext)
				{
				  double nextsrStart = itsNext->getXStart();
				  double nextsrEnd = nextsrStart + 
				    itsNext->getNumSites()*
				    itsNext->getSiteWidth();
				  
				  if((nextsrStart <= srStart && 
				      nextsrEnd >= srStart) || 
				     (nextsrStart >= srStart &&
				      nextsrEnd <= srEnd) ||
				     (nextsrStart <= srEnd &&
				      nextsrEnd >= srEnd)) //overlapping SR
				    {
				      if(findInList(itsNext, seenList) == 0)
					{
					  bfsList.push_back(itsNext);
					  seenList.push_back(itsNext);
					}
				    }
				}
			    }
			  if(itcCurr != _coreRows.begin())
			    {
			      itcNext = itcCurr-1;
			      for(itsNext = itcNext->subRowsBegin();
				  itsNext!=itcNext->subRowsEnd();++itsNext)
				{
				  double nextsrStart = itsNext->getXStart();
				  double nextsrEnd = nextsrStart + 
				    itsNext->getNumSites()*
				    itsNext->getSiteWidth();
				  
				  if((nextsrStart <= srStart && 
				      nextsrEnd >= srStart) || 
				     (nextsrStart >= srStart &&
				      nextsrEnd <= srEnd) ||
				     (nextsrStart <= srEnd &&
				      nextsrEnd >= srEnd)) //overlapping SR
				    {
				      if(findInList(itsNext, seenList) == 0)
					{
					  bfsList.push_back(itsNext);
					  seenList.push_back(itsNext);
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
  tm.stop();
  double HPWLafter=evalHPWL();
  double HPWLwPins=evalHPWL(true);
  unsigned finalOverlaps = getNumOverlaps();

  if (1/*_params.verb.forMajStats*/) 
  {
    cout<<" Removing overlaps took " << tm << endl;
    cout<<"  Discovered " << initOverlaps << " overlaps "<<endl;
    cout<<"  Overlaps remaining: "<<finalOverlaps<<endl;
    cout<<"  HPWL after  overlap removal is "<<HPWLafter<<endl;
    cout<<"  HPWL w pins                 is "<<HPWLwPins<<endl;
    cout<<"  % change in HPWL is "<<(HPWLafter-initHPWL)*100/HPWLafter<<endl;
  }
}

void RBPlacement::shuffleSR(itRBPSubRow its)
{
  itRBPCellIds itn;
  itRBPCellIds itntemp;
  itRBPCellIds itnleft;
  itRBPCellIds itnright;

  if(its->getNumCells() == 0)
    return;

  if(its->getNumCells() == 1)
    {
      itn = its->cellIdsBegin();
      if(_placement[*itn].x < its->getXStart())
        _placement[*itn].x = its->getXStart();
      else if(its->getXEnd() < 
	      (_placement[*itn].x + _hgWDims->getNodeWidth(*itn)))
       _placement[*itn].x = its->getXEnd()-_hgWDims->getNodeWidth(*itn);

      return;
    }

  //itRBPSubRow itsNext;    //used to scan immediate next sub row 
  //itRBPCellIds itnNext;   //used to scan immediate next sub row cells

  bool leftRight=false;   //false means move left else move right
  double WS=0;
  //double rowHeight=0;
  double nodeHeight=0;
  double xDiff=0;  
  double totalOverlap=0;
  double totalCellWidths=0;

  //snap all cells outside row to the row
  for(itn=its->cellIdsBegin();itn!=its->cellIdsEnd();++itn)
    {
      if(_placement[*itn].x < its->getXStart())
        _placement[*itn].x = its->getXStart();
      else if(its->getXEnd() < 
	      (_placement[*itn].x + _hgWDims->getNodeWidth(*itn)))
       _placement[*itn].x = its->getXEnd()-_hgWDims->getNodeWidth(*itn);

    }

  for(itn=its->cellIdsBegin();itn!=its->cellIdsEnd();++itn)
    {
      //check for overlaps in the same row
      xDiff=1;
      totalCellWidths += _hgWDims->getNodeWidth(*itn);
      itntemp=itn+1;
      if(itntemp != its->cellIdsEnd())
        {
          xDiff= _placement[*itn].x +
            _hgWDims->getNodeWidth(*itn)-_placement[*itntemp].x;

          if(xDiff > 1e-10)
            {
              //overlap found, now deal with it
	      totalOverlap += xDiff;
	    }
        }
    }

  if(totalOverlap <= 1e-10)
    return;

  double totalSiteWidths = its->getXEnd()-its->getXStart();
  itn = its->cellIdsBegin();
  double startWS = _placement[*itn].x - its->getXStart();
  itn = its->cellIdsEnd()-1;
  double endWS = its->getXEnd()-_placement[*itn].x-_hgWDims->getNodeWidth(*itn);
  double WSNeeded = totalCellWidths+startWS+endWS-totalSiteWidths;
  if(WSNeeded > 0)
    {
      itn = its->cellIdsBegin();
      if(WSNeeded <= startWS)
        {
   	  _placement[*itn].x = _placement[*itn].x - WSNeeded;
          WSNeeded = 0; 
        }  
      else if(startWS > 0)
       {
  	 _placement[*itn].x = _placement[*itn].x - startWS;
	 WSNeeded = WSNeeded - startWS;	 
       }
      if(_placement[*itn].x < its->getXStart())
        _placement[*itn].x = its->getXStart();

      itn = its->cellIdsEnd()-1;
      if(WSNeeded <= endWS)
        {
          _placement[*itn].x = _placement[*itn].x + WSNeeded;
          WSNeeded = 0;
        }
      else if(endWS > 0)
       {
         _placement[*itn].x = _placement[*itn].x + endWS;
         WSNeeded = WSNeeded - endWS;
       }
     if(its->getXEnd() < (_placement[*itn].x + _hgWDims->getNodeWidth(*itn)))
       _placement[*itn].x = its->getXEnd()-_hgWDims->getNodeWidth(*itn);
    }

  WS = 0;
  xDiff = 0;

  for(itn=its->cellIdsBegin();itn!=its->cellIdsEnd();++itn)
    {
      nodeHeight=_hgWDims->getNodeHeight(*itn);
      //check for overlaps in the same row
      xDiff=1;
      if(itn!=its->cellIdsEnd()-1)
	{
	  itntemp=itn+1;
	  xDiff= _placement[*itn].x + 
	    _hgWDims->getNodeWidth(*itn)-_placement[*itntemp].x;
	  
	  if(xDiff > 1e-10)
	    {
	      //overlap found, now deal with it
	      
	      itnleft=itn;
	      itnright=itn;
	      leftRight=false;
	      
	      while(xDiff>1e-10 && (itnleft!=(its->cellIdsBegin()) || 
				    itnright!=(its->cellIdsEnd()-2)))
		{
		  // chose the direction of search
		  if(leftRight==false && itnright!=its->cellIdsEnd()-2)
		    leftRight=true;
		  else if(leftRight==true && 
			  itnleft!=its->cellIdsBegin())
		    leftRight=false;
		  
		  if(leftRight==true)//search right for WS
		    {
		      ++itnright;
		      itntemp=itnright+1;
		      WS=_placement[*itntemp].x -
			_placement[*itnright].x -
			_hgWDims->getNodeWidth(*itnright);
		      if(WS > 0) //WS found
			{
			  if(WS > xDiff)
			    WS=xDiff;
			  xDiff-=WS;
			  for(itntemp=itn+1;itntemp!=itnright+1;
			      ++itntemp)
			    _placement[*itntemp].x+=WS;
			}
		    }
		  else //search left for WS
		    {
		      itntemp=itnleft;
		      --itnleft;
		      WS=_placement[*itntemp].x -
			_placement[*itnleft].x -
			_hgWDims->getNodeWidth(*itnleft);
		      if(WS > 0) //WS found
			{
			  if(WS > xDiff)
			    WS=xDiff;
			  xDiff-=WS;
			  for(itntemp=itn;itntemp!=itnleft;--itntemp)
			    {
			      _placement[*itntemp].x-=WS;
			    }
			}
		    }
		}
	    }
	}
    }

  //again snap all cells outside row to the row
  for(itn=its->cellIdsBegin();itn!=its->cellIdsEnd();++itn)
    {
      if(_placement[*itn].x < its->getXStart())
        _placement[*itn].x = its->getXStart();
      else if(its->getXEnd() < 
	      (_placement[*itn].x + _hgWDims->getNodeWidth(*itn)))
       _placement[*itn].x = its->getXEnd()-_hgWDims->getNodeWidth(*itn);
    }
}

unsigned RBPlacement::getNumOverlaps()
{
  unsigned overlaps = 0;
  itRBPCellIds itn;
  itRBPCellIds itntemp;
  itRBPCoreRow itc;
  itRBPSubRow its;

  double xDiff=0;
  for(itc=_coreRows.begin();itc!=_coreRows.end();++itc)
    {
      for(its=itc->subRowsBegin();its!=itc->subRowsEnd();++its)
	{
	  for(itn=its->cellIdsBegin();itn!=its->cellIdsEnd();++itn)
	    {
	      //check for overlaps in the same row
	      xDiff=1;
	      if(itn!=its->cellIdsEnd()-1)
		{
		  itntemp=itn+1;
		  xDiff= _placement[*itn].x + 
		    _hgWDims->getNodeWidth(*itn)-_placement[*itntemp].x;
		  
		  if(xDiff > 1e-10)
		    overlaps++;
		}
	    }
	}
    }
  return(overlaps);
}

bool RBPlacement::checkOutOfCoreCells(void)
{
  BBox layout = getBBox(false);
  bool consistent=true;
  double rowHeight=coreRowsBegin()->getHeight();

  for(unsigned i=0; i<getNumCells(); i++)
    {
      if(_isCoreCell[i] && !_isFixed[i])
	{
	  double cellWidth=0;
	  double cellHeight=0;
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

	  if(_placement[i].x < layout.xMin || 
	     (_placement[i].x + cellWidth) > layout.xMax ||
	     _placement[i].y < layout.yMin || 
	     (_placement[i].y + cellHeight) > layout.yMax)
	    {
	      consistent = false;
	      if(cellHeight > rowHeight)
		cout<<"Macro ";
	      else
		cout<<"Cell ";
	      cout<<_hgWDims->getNodeByIdx(i).getName()<<" ("
		  <<_placement[i].x<<" "<<_placement[i].y<<"->"
		  <<_placement[i].x+cellWidth<<" "<<_placement[i].y+cellHeight
		  <<")"<<" out of core "<<layout;
	    }
	}
    }
  return consistent;
}


double RBPlacement::calcOverlapGeneric(bool onlyMacros)
{
  const double P = 1.0;
  double overlap = 0;
  double rowHeight = coreRowsBegin()->getHeight();
  unsigned numNodes = getNumCells() - _hgWDims->getNumTerminals();
  unsigned numGridRows = unsigned(sqrt(numNodes/P));
  unsigned numGridCol = numGridRows;
  
  BBox layout = getBBox(false);
  double gridXSize = layout.getWidth()/numGridCol;
  double gridYSize = layout.getHeight()/numGridRows;

  vector< vector< vector<unsigned> > > grid(numGridRows);
  for(unsigned i=0; i<numGridRows; ++i)
    grid[i].resize(numGridCol);

  for(unsigned i=0; i<getNumCells(); i++)
    {
      if(_isCoreCell[i])
	{
	  if(onlyMacros)
	    {
	      //if(!_hgWDims->isNodeMacro(i))
	      if(_hgWDims->getNodeHeight(i) <= rowHeight)
		continue;
	    }

	  BBox cell;
          cell += _placement[i];
	  Orient nodeOrient = _placement.getOrient(i);
	  double cellWidth;
	  double cellHeight;
	  if(nodeOrient == Orient("N") || nodeOrient == Orient("S") ||
             nodeOrient == Orient("FN") || nodeOrient == Orient("FS"))
            {
              cellWidth=_hgWDims->getNodeWidth(i);
              cellHeight=_hgWDims->getNodeHeight(i);
            }
          else
            {
              cellWidth=_hgWDims->getNodeHeight(i);
              cellHeight=_hgWDims->getNodeWidth(i);
            }

          cell.add(_placement[i].x+cellWidth, _placement[i].y+cellHeight);
	  unsigned startGridCol = unsigned(floor((cell.xMin - layout.xMin)/gridXSize));
	  unsigned startGridRow = unsigned(floor((cell.yMin - layout.yMin)/gridYSize));
	  unsigned endGridCol = unsigned(ceil((cell.xMax - layout.xMin)/gridXSize));
          unsigned endGridRow = unsigned(ceil((cell.yMax - layout.yMin)/gridYSize));
	  if(startGridRow < 0)
	    startGridRow = 0;
	  if(startGridRow >= numGridRows)
	    startGridRow = numGridRows-1;
          if(endGridRow < 0)
            endGridRow = 0;
          if(endGridRow > numGridRows)
            endGridRow = numGridRows;
          if(startGridCol < 0)
            startGridCol = 0;
          if(startGridCol >= numGridCol)
            startGridCol = numGridCol-1;
          if(endGridCol < 0)
            endGridCol = 0;
          if(endGridCol > numGridCol)
            endGridCol = numGridCol;

	  for(unsigned rIdx = startGridRow; rIdx < endGridRow; ++rIdx)
	    for(unsigned cIdx = startGridCol; cIdx < endGridCol; ++cIdx)
	      grid[rIdx][cIdx].push_back(i);
	}
    }

  for(unsigned i=0; i<getNumCells(); i++)
    {
      if(_isCoreCell[i])
	{
	  if(onlyMacros)
	    {
	      //if(!_hgWDims->isNodeMacro(i))
	      if(_hgWDims->getNodeHeight(i) <= rowHeight)
		continue;
	    }

          BBox cell;
          cell += _placement[i];
	  Orient nodeOrient = _placement.getOrient(i);
	  double cellWidth;
	  double cellHeight;
	  if(nodeOrient == Orient("N") || nodeOrient == Orient("S") ||
             nodeOrient == Orient("FN") || nodeOrient == Orient("FS"))
            {
              cellWidth=_hgWDims->getNodeWidth(i);
              cellHeight=_hgWDims->getNodeHeight(i);
            }
          else
            {
              cellWidth=_hgWDims->getNodeHeight(i);
              cellHeight=_hgWDims->getNodeWidth(i);
            }

          cell.add(_placement[i].x+cellWidth, _placement[i].y+cellHeight);

          unsigned startGridCol = unsigned(floor((cell.xMin - layout.xMin)/gridXSize));
          unsigned startGridRow = unsigned(floor((cell.yMin - layout.yMin)/gridYSize));
          unsigned endGridCol = unsigned(ceil((cell.xMax - layout.xMin)/gridXSize));
          unsigned endGridRow = unsigned(ceil((cell.yMax - layout.yMin)/gridYSize));	  
          if(startGridRow < 0)
            startGridRow = 0;
          if(startGridRow >= numGridRows)
            startGridRow = numGridRows-1;
          if(endGridRow < 0)
            endGridRow = 0;
          if(endGridRow > numGridRows)
            endGridRow = numGridRows;
          if(startGridCol < 0)
            startGridCol = 0;
          if(startGridCol >= numGridCol)
            startGridCol = numGridCol-1;
          if(endGridCol < 0)
            endGridCol = 0;
          if(endGridCol > numGridCol)
            endGridCol = numGridCol;

	  vector<unsigned> seenNodes; 
	  for(unsigned rIdx = startGridRow; rIdx < endGridRow; ++rIdx)
	    {
	      for(unsigned cIdx = startGridCol; cIdx < endGridCol; ++cIdx)
		{
		  for(unsigned idx=0; idx<grid[rIdx][cIdx].size();++idx)
		    {
		      unsigned j=grid[rIdx][cIdx][idx];
		      if(i!=j)
			{
			  bool needThis=true;
			  for(unsigned k=0; k<seenNodes.size(); ++k)
			    {
			      if(j==seenNodes[k])
				{
				  needThis = false;
				  break;
				}
			    }
			  if(needThis)
			    {
			      seenNodes.push_back(j);
			      BBox cell2;
			      Orient nodeOrient2 = _placement.getOrient(j);
			      cell2 += _placement[j];
			      double cellWidth2;
			      double cellHeight2;
			      if(nodeOrient2 == Orient("N") || 
				 nodeOrient2 == Orient("S") ||
				 nodeOrient2 == Orient("FN") || 
				 nodeOrient2 == Orient("FS"))
				{
				  cellWidth2=_hgWDims->getNodeWidth(j);
				  cellHeight2=_hgWDims->getNodeHeight(j);
				}
			      else
				{
				  cellWidth2=_hgWDims->getNodeHeight(j);
				  cellHeight2=_hgWDims->getNodeWidth(j);
				}
			      
			      cell2.add(_placement[j].x+cellWidth2, 
					_placement[j].y+cellHeight2);

			      BBox intersection = cell.intersect(cell2);
			      overlap += intersection.getArea()/2.0;
			      
			      /*
			      if(intersection.getArea() > 0)
				{
				  cout<<_hgWDims->getNodeByIdx(i).getName()<<" "<<_hgWDims->getNodeByIdx(j).getName()<<" "<<intersection.getArea()<<endl;
				}
			      */
			    }
			}
		    }
		}
	    }
	}
    }
  double percentOverlap = 100.0*overlap/layout.getArea();
  if(!onlyMacros)
    cout<<"Total Overlap is "<<overlap<<" OR "<<percentOverlap<<" % of layout Area"<<endl;
  else
    cout<<"Total Macro Overlap is "<<overlap<<" OR "<<percentOverlap<<" % of layout Area"<<endl;

  /*
  overlap=0;
  for(unsigned i=0; i<getNumCells(); i++)
    {
      if(_isCoreCell[i])
	{
	  BBox cell1;
	  cell1 += _placement[i];
	  cell1.add(_placement[i].x+_hgWDims->getNodeWidth(i), 
		    _placement[i].y+_hgWDims->getNodeHeight(i));
	  for(unsigned j=0; j<getNumCells(); j++)
	    {
	      if(_isCoreCell[j] && i!=j)
		{
		  BBox cell2;
		  cell2 += _placement[j];
		  cell2.add(_placement[j].x+_hgWDims->getNodeWidth(j),
			    _placement[j].y+_hgWDims->getNodeHeight(j));
		  BBox intersection = cell1.intersect(cell2);
		  overlap += intersection.getArea()/2.0;
		}
	    }
	}
    }
  cout<<"Total Overlap is "<<overlap<<endl;
  */
  return(overlap);
}

// added by MRG to calculate total linear overlap 
double RBPlacement::calcOverlap()
{
  itRBPCoreRow itc;
  itRBPSubRow its;
  itRBPCellIds itn;
  itRBPCellIds itntemp;

  unsigned numOverlaps=0;
  unsigned totalOverlaps=0;
  double xDiff=0;
  double totalOverlapLength=0;

  //for each core row, for each subrow scan left to right if overlap found 
  //then take action
  for(itc=_coreRows.begin();itc!=_coreRows.end();++itc)
    {
      for(its=itc->subRowsBegin();its!=itc->subRowsEnd();++its)
	{
	  for(itn=its->cellIdsBegin();itn!=its->cellIdsEnd();++itn)
	    {
	      //check for overlaps in the same row
	      xDiff=1;
	      if(itn!=its->cellIdsEnd()-1)
		{
		  itntemp=itn+1;
		  if(_placement[*itntemp].x < 
		     _placement[*itn].x+_hgWDims->getNodeWidth(*itn))
		    {
		      //overlap found, now deal with it
		      numOverlaps++;
		      totalOverlaps++;
		      
		      xDiff= _placement[*itn].x + 
			_hgWDims->getNodeWidth(*itn)-_placement[*itntemp].x;
		      totalOverlapLength+=(xDiff*itc->getHeight());
		    }
		}
	    }
	}
    }
  return(totalOverlapLength);
}

// added by MRG to incrementally calculate total linear overlap
double RBPlacement::calcInstOverlap(vector<unsigned>& movables)
{
  double xDiff=0;
  double totalOverlapLength=0;
  itRBPCellIds itn;
  for(unsigned i=0;i<movables.size();i++)
    {
      abkassert(_isInSubRow[movables[i]], 
		"Requested cellCoord for cell not in a row");
      
      RBCellCoord& coord = _cellCoords[movables[i]];
      RBPSubRow& sr = _coreRows[coord.coreRowId][coord.subRowId];
      RBPCoreRow& cr = _coreRows[coord.coreRowId];
      double rowHeight = cr.getHeight();
      double myLeftEdge = _placement[movables[i]].x;
      double myRightEdge = _placement[movables[i]].x +
	_hgWDims->getNodeWidth(movables[i]);
      for(itn=sr.cellIdsBegin();itn!=sr.cellIdsEnd();++itn)
	{
	  double theirLeftEdge = _placement[*itn].x;
	  double theirRightEdge = _placement[*itn].x + 
	    _hgWDims->getNodeWidth(*itn);

	  // we're done when we've past the right of our cell
	  if (theirLeftEdge > myRightEdge)
	    break;
	  if (movables[i]==*itn)
	    continue;
	  
	  // i'm contained within another
	  if((myLeftEdge >= theirLeftEdge) && (myRightEdge <= theirRightEdge))
	    {
	      totalOverlapLength += 
		_hgWDims->getNodeWidth(movables[i])*rowHeight;
	    }
	  // another is contained within me	
	  else if((myLeftEdge <= theirLeftEdge) && 
		  (myRightEdge >= theirRightEdge))
	    {
	      totalOverlapLength += 
		_hgWDims->getNodeWidth(*itn)*rowHeight;
	    }
	  // left side overlap
	  else if((myLeftEdge > theirLeftEdge) && 
		  (myLeftEdge < theirRightEdge))
	    {
	      xDiff= theirRightEdge-myLeftEdge;
	      totalOverlapLength+=(xDiff*rowHeight);
	    }
	  // right side overlap
	  else if((myRightEdge > theirLeftEdge) && 
		  (myRightEdge < theirRightEdge))
	    {
	      xDiff= myRightEdge-theirLeftEdge;
	      totalOverlapLength+=(xDiff*rowHeight);
	    }
	}
    }
  return(totalOverlapLength);
}


bool RBPlacement::findClosestWS(Point& loc, Point& WSLoc, double width)
{
  RBRowPtrs rowPtrs = findBothRows(loc);
  RBPSubRow* its = const_cast<RBPSubRow*>(rowPtrs.second);

  itRBPCellIds itn;
  itRBPCellIds itntemp;
  itRBPCellIds itnleft;
  itRBPCellIds itnright;

  bool leftRight=false;   //false means move left else move right
  double WS=0;
  double xDiff=0;  

  itn=its->cellIdsBegin();
  if(itn == its->cellIdsEnd()) //no cells in subrow
    {
      return(true);
    }
  for(itn=its->cellIdsBegin();itn!=(its->cellIdsEnd()-1);++itn)
    {
      xDiff=1;
      itntemp=itn+1;
      if(_placement[*itn].x <= loc.x && _placement[*itntemp].x >= loc.x)
	break;
    }
  if(itn != (its->cellIdsEnd()-1))
    xDiff = _placement[*itntemp].x - 
      (_placement[*itn].x + _hgWDims->getNodeWidth(*itn));
  else 
    xDiff = its->getXEnd() - 
      (_placement[*itn].x + _hgWDims->getNodeWidth(*itn));

  if(xDiff >= width)
    {
      WSLoc.x = _placement[*itn].x + _hgWDims->getNodeWidth(*itn);
      return(true);
    }

  //closest node found, now try to find WS
      
  itnleft=itn;
  itnright=itn;
  leftRight=false;
      
  while((itnleft!=(its->cellIdsBegin()) || 
	 itnright!=(its->cellIdsEnd()-1)))
    {
      // chose the direction of search
      if(leftRight==false && itnright!=its->cellIdsEnd()-1)
	leftRight=true;
      else if(leftRight==true && itnleft!=its->cellIdsBegin())
	leftRight=false;
      
      if(leftRight==true)//search right for WS
	{
	  itntemp=itnright+1;
	  WS=_placement[*itntemp].x -
	    _placement[*itnright].x -
	    _hgWDims->getNodeWidth(*itnright);
	  if(WS >= width) //WS found
	    {
	      WSLoc.x = _placement[*itnright].x + 
		_hgWDims->getNodeWidth(*itnright);
	      return(true);
	    }
	  ++itnright;
	}
      else //search left for WS
	{
	  itntemp=itnleft-1;
	  WS=_placement[*itnleft].x -
	    _placement[*itntemp].x -
	    _hgWDims->getNodeWidth(*itntemp);
	  if(WS >= width) //WS found
	    {
	      WSLoc.x =  _placement[*itnleft].x - width;
	      return(true);
	    }
	  --itnleft;
	}
    }
  return(false);
}

