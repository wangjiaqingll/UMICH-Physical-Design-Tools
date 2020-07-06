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

#ifndef DB_ROUTED_NET_H
#define DB_ROUTED_NET_H

#include <iostream>
#include "dbDefines.h"
#include "dbTypes.h"
#include "dbContext.h"
#include "dbRoutingElement.h"
#include "Geoms/plGeom.h"

class dbRoutedNet
{
  public:
    enum RouteType { NONE, ROUTED, FIXED, COVER };

  private:
    char *                  _name;
    unsigned                _netIndex;  // net index from dbNetlist
               // NOTE:
               // _netIndex is pointed to the actual index of the
               // net in the dbNetlist. This index has to be 
               // updated if the net order in dbNetlist is changed.

    RouteType               _type;

    ctainerRoutingElements  _route;
    ctainerRoutingElements  _segments;
    ctainerRoutingElements  _vias;
    unsigned                _nVias;
    unsigned                _horizSegments;
    unsigned                _vertSegments;
    unsigned                _nJogs;
    double                  _horizLength;
    double                  _vertLength;
    double                  _horizWWLength;
    double                  _vertWWLength;

    void print(ostream & out) const;
    dbRoutedNet(const dbRoutedNet& from):_type(from._type)
             { abkfatal(0,"Can't copy-construct dbRoutedNet"); }
    dbRoutedNet &operator=(const dbRoutedNet&)
             { abkfatal(0,"Can't assign dbRoutedNet");return *this;}

  public:
    dbRoutedNet(const char * name, unsigned net = UINT_MAX);
    ~dbRoutedNet() { delete [] _name; }

    void addElement(const dbRoutingElement& element, const dbLayer& layer);

    const char* getName()     const { return _name; }
    unsigned getNumVias()     const { return _nVias; }
    unsigned getNumSegments() const { return _horizSegments + _vertSegments; }
    unsigned getNumJogs()     const { return _nJogs; }
    double getTotalLength()   const { return _horizLength + _vertLength; }
    double getHorizLength()   const { return _horizLength; }
    double getVertLength()    const { return _vertLength; }
    double getTotalWWLength() const { return _horizWWLength + _vertWWLength; }
    double getHorizWWLength() const { return _horizWWLength; }
    double getVertWWLength()  const { return _vertWWLength; }

    const char* getType()     const
    {
        switch(_type)
        {
            case ROUTED: return "ROUTED";
            case FIXED:  return "FIXED";
            case COVER:  return "COVER";
            default:
                return "NONE";
        }
    }
    void setType(const char * type) 
    {
        if (!strcasecmp(type,"ROUTED")) _type = ROUTED;
        else if (!strcasecmp(type,"FIXED")) _type = FIXED;
        else if (!strcasecmp(type,"COVER")) _type = COVER;
        else _type = NONE;
    }

    unsigned getNetIdx() const { return _netIndex; }
    bool isInDB()        const { return _netIndex != UINT_MAX; }
    bool isRouted()      const { return _type != NONE; }

    itRoutingElement   elementsBegin() const { return _route.begin(); }
    itRoutingElement   elementsEnd()   const { return _route.end(); }
    itRoutingElement   segmentsBegin() const { return _segments.begin(); }
    itRoutingElement   segmentsEnd()   const { return _segments.end(); }
    itRoutingElement   viasBegin()     const { return _vias.begin(); }
    itRoutingElement   viasEnd()       const { return _vias.end(); }

    friend ostream& operator<< (ostream & out, const dbRoutedNet& arg)
    {
        arg.print(out);
        return out;
    }
};
#endif
