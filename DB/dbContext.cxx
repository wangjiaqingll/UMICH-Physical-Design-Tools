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









// Created 970814 by Igor Markov and Andy Caldwell, VLSI CAD ABKGROUP, UCLA/CS
// CHANGES
// 980118 mro clean up tracks in dtor


//======================================================================
//                          Context Package
//====================================================================== 
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <vector>
#include "ABKCommon/abkcommon.h"
#include "Geoms/plGeom.h"
#include "Placement/symOri.h"          // Symmetry and Orientation classes
#include "dbDefines.h"
#include "dbTypes.h"
#include "dbPin.h"
#include "dbCell.h"
#include "dbNet.h"
#include "dbContext.h"
#include <ABKCommon/sgi_stl_compat.h>

using std::vector;
using std::min;
using std::max;

bool dbContext::isConsistent() const
{ // TODO
   bool IamFine=true; 
   return IamFine;
}

dbContext::~dbContext()
{
   unsigned k;
   for(k=0;k<_sites.size();k++)   delete _sites[k];
   for(k=0;k<_layers.size();k++)  delete _layers[k];
   for(k=0;k<_rows.size();k++)    delete _rows[k];
   for(k=0;k<_tracks.size();k++)    delete _tracks[k];
   for(k=0;k<_gcellGrids.size();k++)  delete _gcellGrids[k];
}

unsigned dbContext::getNumHorizTracks(const BBox& box) const
{
    double hCount = 0;
    for(itTrackLocal track = _tracks.begin(); track != _tracks.end(); track++)
    {
        double end = (*track)->start +
                     (((*track)->numTracks - 1) * (*track)->spacing);
        if(!(*track)->horizontal)   // a horizontal track
        {
          if (!((*track)->start >= box.yMax || end < box.yMin))
          {
            if (end == box.yMax) end -= 1;
            double startTrack = ceil((max((*track)->start, box.yMin) -
                                      (*track)->start)/(*track)->spacing),
                   endTrack = floor((min(end, box.yMax - 1) -
                                     (*track)->start)/(*track)->spacing);
            //cout << "HCount : " << endTrack - startTrack + 1 << endl;
            hCount += endTrack - startTrack + 1;
          }
        }
    }
    return static_cast<unsigned>(hCount);
}

unsigned dbContext::getNumVertTracks(const BBox& box) const
{
    double vCount = 0;
    for(itTrackLocal track = _tracks.begin(); track != _tracks.end(); track++)
    {
        double end = (*track)->start +
                     (((*track)->numTracks - 1) * (*track)->spacing);
        if((*track)->horizontal)   // a vertical track
        {
          if (!((*track)->start >= box.xMax || end < box.xMin))
          {
            if (end == box.xMax) end -= 1;
            double startTrack = ceil((max((*track)->start, box.xMin) -
                                      (*track)->start)/(*track)->spacing),
                   endTrack = floor((min(end, box.xMax - 1) -
                                     (*track)->start)/(*track)->spacing);
            //cout << "VCount : " << endTrack - startTrack + 1 << endl;
            vCount += endTrack - startTrack + 1;
          }
        }
    }
    return static_cast<unsigned>(vCount);
}

void dbContext::addGCellGrid(bool horiz, double start,
                            unsigned nDivs, double space)
{
    _gcellGrids.push_back(new dbGCellGrid(horiz, start, nDivs, space));
}


dbLayerGeom::dbLayerGeom(const vector<Point>& polygon, dbLayer const &layer,
                         bool isVia):
          _layer(layer),_isVia(isVia)
{
    std::vector<Point>::const_iterator iP=polygon.begin();
    for (;iP!=polygon.end();iP++)
    {
        const Point &pt=*iP;
        _rect += pt;
    }
}
dbLayerGeom::dbLayerGeom(const Rectangle& rect, dbLayer const &layer, bool isVia):
          _layer(layer),_rect(rect),_polygon(4),_isVia(isVia)
{
    _polygon[0].x=rect.xMin;_polygon[0].y=rect.yMin;
    _polygon[1].x=rect.xMax;_polygon[1].y=rect.yMin;
    _polygon[2].x=rect.xMin;_polygon[2].y=rect.yMax;
    _polygon[3].x=rect.xMax;_polygon[3].y=rect.yMax;
}
