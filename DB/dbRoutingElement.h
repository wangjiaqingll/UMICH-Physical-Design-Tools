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









// Created by Stefanus Mantik, January 7, 1999

/****
       This is a class that is used in dbRoutedNet to represent vias
    and segments.   The type for each item instantiate from this class
    can be determined by checking isVia() and isSegment().   For each
    type, it has its own restriction on the data that it can use.
    The following are the member description for each type:

    VIA
        x,y :    The (x,y) coordinate of the via.
        _layer1, _layer2 :
                 The two metal layer that this via connects.
                 The value for these variable is the dbLayer index
        _masterViaIndex :
                 Index to the dbMasterVia from the master via 
                 list in the dbRoutingInfo.
        getMasterViaIndex() : returns the _masterViaIndex

        The rest of member function does not give much information because
        they are mostly used for SEGMENT type, however calling those
        functions is still allowed which will return default value
        explained below:
            getXEnd() and getYEnd() will return x and y
            getLength() will return 0
            isHorizontal() and isVertical() will return false

    SEGMENT
        Segment will be represented as a vector with base coordinate,
        direction, and distance.
        x,y :        The (x,y) coordinate of the origin (base coordinate).
        _distance :  The distance where the value can be negative which
                     means that the vector points to the opposite direction.
        _direction : The direction in Manhattan plane where this vector
                     points to. It has to be either HORiZontal or
                     VERTical.
        getXEnd(), getYEnd() :
                     Gives the (x,y) coordinate of the end point of the
                     vector. Note that this coordinate can be less than
                     the origin when the _distance is negative.
        isHorizontal(), isVertical() :
                     Will check whether it is a horizontal or a vertical
                     segment.
        getLength() : Will return the length of the segment which is the
                     absolute value of the _distance (magnitude)

        With this type, calling getMasterViaIndex() will result an
        abkfatal because master via is not defined for segment.
*****/

#ifndef DB_ROUTING_ELEMENT_H
#define DB_ROUTING_ELEMENT_H

#include <iostream>
#include "dbDefines.h"
#include "dbTypes.h"
#include "Geoms/plGeom.h"

class dbRoutingElement
{
    enum routingElementType { UNDEFINED, VIA, SEGMENT };
    enum segmentDirection { UNKN, HORZ, VERT };

    routingElementType _type;
    segmentDirection _direction;
    double _distance;

    void print(ostream & out) const;

    unsigned    _masterViaIndex;    // index to master via
    unsigned    _layer1, _layer2;

  public:
    double x, y;

    dbRoutingElement(Point startCoord, Point endCoord, unsigned ly) :
        _type(SEGMENT), _masterViaIndex(UINT_MAX),_layer1(ly), _layer2(ly), 
         x(startCoord.x), y(startCoord.y)
    {
        double xDist = endCoord.x - x,
               yDist = endCoord.y - y;
        abkfatal(!(abs(xDist) > 0 && abs(yDist) > 0), 
                 "Segment must be in rectilinear plane");
        _distance = xDist + yDist;
        if (xDist == 0 && abs(yDist) > 0)
            _direction = VERT;
        else if (yDist == 0 && abs(xDist) > 0)
            _direction = HORZ;
        else
            _direction = UNKN;
    }

    dbRoutingElement(Point coord, unsigned l1, unsigned l2, 
                   unsigned vId = UINT_MAX) :
        _type(VIA), _direction(UNKN), _distance(0), 
        _masterViaIndex(vId),_layer1(l1), _layer2(l2),
        x(coord.x), y(coord.y)
   { }

    dbRoutingElement(const dbRoutingElement& arg) :
        _type(arg._type), _direction(arg._direction), _distance(arg._distance), 
        _masterViaIndex(arg._masterViaIndex),
        _layer1(arg._layer1), _layer2(arg._layer2),
        x(arg.x), y(arg.y)
   {}

    bool isVia()        const { return _type == VIA; }
    bool isSegment()    const { return _type == SEGMENT; }

    bool isHorizontal() const { return _direction == HORZ; }
    bool isVertical()   const { return _direction == VERT; }

    unsigned getLayer1() const { return _layer1; }
    unsigned getLayer2() const { return _layer2; }

    unsigned getMasterViaIndex() const
    {
        abkfatal(_type == VIA, "Cannot get Master Via Index from Segment");
        return _masterViaIndex;
    }

    double getXEnd() const
    {
        if (_direction == HORZ) return x + _distance;
        else return x;
    }
    double getYEnd() const
    {
        if (_direction == VERT) return y + _distance;
        else return y;
    }
    double getLength() const { return abs(_distance); }

    friend ostream& operator<<(ostream & out, const dbRoutingElement & arg)
    {
        arg.print(out);
        return out;
    }
};
#endif
