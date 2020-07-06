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








#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

#include <stdio.h>
#include <algorithm>
#include "Combi/mapping.h"
#include "Constraints/regionConstr.h"
#include "parserLEFDEF.h"

extern FILE *lefin,*defin;
extern int lefparse();
extern int defparse();

ParserLEFDEF *pp;
DB           *pdb;

void processRegions(ParserLEFDEF &p)
    {
    std::vector<compRegionPair>::iterator left,right,current; 
                    //left is the smallest index, and right one past the
                    //greatest index, of a block of compRegionPair's
                    //with the same region.

    CompPairsByRegion c;
    left=right=p.compRegions.begin();

    while (right!=p.compRegions.end())
        {
        //find how far we can go in the same region
        while(true)
            {
            if (!(c(*left,*right) || c(*right,*left))) //i.e. if regions are
                                                       // "equal"
                right++;
            else
                break;

            if (right==p.compRegions.end()) break;
            }

        //now we have to find the indices of the cells in that region
        //so we can construct a SubPlacement object.
        vector<unsigned> cellindices;
        for (current=left;current!=right;current++)
            {
            cellindices.push_back(((*current).first)->getIndex());
            }

        std::sort(cellindices.begin(),cellindices.end());

        Mapping m(cellindices.size(),p._db.spatial.locations.getSize());

        unsigned i;
        for (i=0;i<cellindices.size();i++)
            {
            m[i] = cellindices[i];
            }

        SubPlacement sp(p._db.spatial.locations,m);

        typedef parserRegion &rParserRegion;
        rParserRegion reg(*((*left).second)); //all regions from left
                                              //to right-1 must have the
                                              //same properties wrt the
                                              //following code.
        if (reg.bSoft)
            {
            SoftGroupConstraint sgc(sp,reg.maxX,reg.maxY,reg.maxHalfPerim);
            p._db.spatial.constraints.add(sgc);
            }

        if (reg.boxes.size()==1)
            {
            RectRegionConstraint rrc(sp,reg.boxes[0]);
            p._db.spatial.constraints.add(rrc);
            }
        else if (reg.boxes.size()>1)
            {
            MultRectRegionConstraint mrrc(sp,reg.boxes);
            p._db.spatial.constraints.add(mrrc);
            }

        left = right;
        }
    //process the utilization constraints now . by sadya in SYNP
    for(unsigned i=0;i<p.regions.size();++i)
      {
	if(p.regions[i]->utilConstraint)
	  {
	    Mapping m(0, p._db.spatial.locations.getSize());
	    SubPlacement sp(p._db.spatial.locations,m);
	    UtilRectRegionConstraint urrc(sp, p.regions[i]->boxes[0],
					  p.regions[i]->utilization);
            p._db.spatial.constraints.add(urrc);
	  }
      }
    }

ParserLEFDEF::ParserLEFDEF(DB& db,
                           const char *lname,
                           const char *dname):
                              _db(db)
{
bool retcode;
pp = this;
pdb = &_db;
//set default values
_db.context._units = 100;

lefin=fopen(lname,"r");
if (!lefin)
    {
    fprintf(stdout,"Unable to open LEF file %s\n",lname);
    printf("Unable to open LEF file %s\n",lname);
    exit(-1);
    }
defin=fopen(dname,"r");
if (!defin)
    {
    fprintf(stderr,"Unable to open DEF file %s\n",dname);
    printf("Unable to open DEF file %s\n",dname);
    exit(-1);
    }
retcode =   lefparse();
if (!retcode)
      {
      fprintf(stdout,"LEF file %s parsed\n",lname);
      fflush(stdout);
      }
else
    {
    printf("LEF parse failed, so we don't proceed to DEF\n");
    fprintf(stderr,"LEF parse failed, so we don't proceed to DEF\n");
    fflush(stdout);
    fflush(stderr);
    exit(-1);
    }

//make bogus site for external pins
_db.context._sites.push_back(pExtPinSite=new dbSite("FakedSiteForExtPins",
                             dbSite::PAD,
                             Symmetry(false,false,false),
                             0.0,
                             0.0));

retcode =   defparse();
_db.context._rows = tempAllRows;
_db.context._tracks = tempAllTracks;
_db.design._cells = tempAllCells;
_db.design._nets  = tempAllNets;
_db.design._sNets = tempAllSNets;
_db.design._pins  = tempAllPins;
_db.design._cellLEQs = tempCellLEQs;
_db.design._cellEEQs = tempCellEEQs;
_db.spatial.locations = Placement(tempAllLocs);
_db.spatial.locationValid = tempAllLocsValid;
_db.spatial.orientations = tempAllOrients;

if (fixedCompIndices.size()>0)
    {
    Mapping fixIndexToGlobalIndex(fixedCompIndices.size(),
                                  tempAllLocs.size());
    Placement fixedPlacement(fixedCompIndices.size());
    std::vector<unsigned>::iterator iU;
    unsigned i;
    for (iU=fixedCompIndices.begin(),i=0;iU!=fixedCompIndices.end();iU++,i++)
        {
        fixIndexToGlobalIndex[i] = *iU;
        fixedPlacement[i] = _db.spatial.locations[(*iU)];
        }
    SubPlacement sp(_db.spatial.locations,fixIndexToGlobalIndex);
    FixedGroupConstraint fgc(sp,fixedPlacement);
    _db.spatial.constraints.add(fgc);
    }

std::sort(compRegions.begin(),compRegions.end(),CompPairsByRegion());
processRegions(*this);


std::vector<char*>::iterator iS;

for (iS=stringsToClean.begin();iS!=stringsToClean.end();iS++)
    free(*iS);

if (!retcode)
      {
      fprintf(stdout,"DEF file %s parsed\n",dname);
      fflush(stdout);
      }

fclose(lefin);
fclose(defin);


}

ParserLEFDEF::~ParserLEFDEF()
    {
    //Note:  when vias are moved into DB, must move destruction below
    //       into DB.
    std::vector<dbVia*>::iterator iV;
    for (iV=tempAllVias.begin();iV!=tempAllVias.end();iV++)
        {
        delete (*iV); (*iV)=NULL;
        }

    std::vector<parserRegion*>::iterator iR;

    for (iR=regions.begin();iR!=regions.end();iR++)
        {
        delete (*iR);(*iR)=NULL;
        }
    }

BBox &BBoxExtended::snip(BBox &overlapping)
    {
    bool ll,lr,ul,ur;
    ll=overlapping.contains(xMin,yMin);
    lr=overlapping.contains(xMax,yMin);
    ul=overlapping.contains(xMin,yMax);
    ur=overlapping.contains(xMax,yMax);

    if (ll && lr && !ul)
        {
        yMin = overlapping.yMax;
        }
    else if (ll && ul && !lr)
        {
        xMin = overlapping.xMax;
        }
    else if (lr && ur && !ll)
        {
        xMax = overlapping.xMin;
        }
    else if (ul && ur && !ll)
        {
        yMax = overlapping.yMin;
        }
    return (*this);
    }

