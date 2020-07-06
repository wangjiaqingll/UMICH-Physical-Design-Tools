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
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "dbRoutedNet.h"

void dbRoutedNet::print(ostream & out) const
{
    out << "Net: " << _name << endl
        << "  Via = " << _nVias << endl
        << "  Segment = " << _horizSegments << " H, " << _vertSegments << " V\n"
        << "  Length   = " << _horizLength << " H, " << _vertLength << " V\n"
        << "  WWLength = " << _horizWWLength << " H, " << _vertWWLength
        << endl;
    if(_type != NONE)
    {
        out << "  Route:\n";
    
        for(itRoutingElement it = _route.begin(); it != _route.end(); it++)
            out << "    " << *it << " " << endl;
    }
}

dbRoutedNet::dbRoutedNet(const char * name, unsigned net):
    _netIndex(net), _type(NONE),
    _nVias(0), _horizSegments(0), _vertSegments(0),_nJogs(0), 
    _horizLength(0), _vertLength(0), _horizWWLength(0), _vertWWLength(0)
{
    _name = new char[strlen(name)+1];
    strcpy(_name,name);
}

void dbRoutedNet::addElement(
    const dbRoutingElement& element, const dbLayer& layer)
{
    if(_type == NONE) _type = ROUTED;  // mark it "routed"

    if(element.isVia())
    {
        _route.push_back(element);
        _vias.push_back(element);
        _nVias++;
    }
    else
    {
        _route.push_back(element);
        _segments.push_back(element);
        bool wrongWay = false;
        abkfatal(element.isHorizontal() || element.isVertical(),
                 "Segment must be in rectilinear space");
        dbLayer::Dir layerDir = layer.getDirection();
        if((element.isHorizontal() && layerDir == dbLayer::VERT) ||
           (element.isVertical() && layerDir == dbLayer::HORIZ))
            wrongWay = true;
        if(wrongWay) _nJogs++;   // not exactly correct!!!
        if(element.isHorizontal())
        {
            _horizLength += element.getLength();
            _horizSegments++;
            if(wrongWay) _horizWWLength += element.getLength();
        }
        else
        {
            _vertLength += element.getLength();
            _vertSegments++;
            if(wrongWay) _vertWWLength += element.getLength();
        }
    }
}
 
