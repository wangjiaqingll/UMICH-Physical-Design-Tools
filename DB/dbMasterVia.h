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

#ifndef DB_MASTER_VIA_H 
#define DB_MASTER_VIA_H

#include "Geoms/plGeom.h"
#include "dbDefines.h"
#include "dbTypes.h"
#include "dbContext.h"

class dbMasterVia
{
    char *             _name;
    unsigned           _index;

    const dbLayer&     _layer1;
    const dbLayer&     _cutLayer;
    const dbLayer&     _layer2;

    const BBox         _rect1;
    const BBox         _cutRect;
    const BBox         _rect2;

    const double       _resistance;
    const bool         _default;

    // this is for detail layout
    ctainerLayerGeoms  _ports;    // will be populated later

    void print(ostream & out) const;
    dbMasterVia(const dbMasterVia& from):_layer1(from._layer1),
                                    _cutLayer(from._cutLayer),
                                    _layer2(from._layer2),
                                    _resistance(from._resistance),
                                    _default(from._default)
        {abkfatal(0,"can't copy-ctruct dbMasterVia");}
    dbMasterVia &operator=(const dbMasterVia&){abkfatal(0,
        "can't assign dbMasterVia");return *this;}
 
  public:
    dbMasterVia(const char * name, unsigned idx, 
                const dbLayer & layer1, const BBox& rect1,
                const dbLayer & cutLayer, const BBox& cutRect,
                const dbLayer & layer2, const BBox& rect2, 
                bool isDefault = false, double resistance = -1):
        _index(idx), _layer1(layer1), _cutLayer(cutLayer), _layer2(layer2),
        _rect1(rect1), _cutRect(cutRect),_rect2(rect2), 
	_resistance(resistance), _default(isDefault)
    {
        _name = new char[strlen(name)+1];
        strcpy(_name,name); 
    }

    ~dbMasterVia()
    {
        delete[] _name;
    }

    const dbLayer& getFirstLayer()  const { return _layer1; }
    const dbLayer& getCutLayer()    const { return _cutLayer; }
    const dbLayer& getSecondLayer() const { return _layer2; }

    BBox getFirstRect()  const { return _rect1; }
    BBox getCutRect()    const { return _cutRect; }
    BBox getSecondRect() const { return _rect2; }

    unsigned getIndex()    const { return _index; }
    const char* getName()  const { return _name; }

    bool isDefault() const { return _default; }

    double getResistance() const { return _resistance; }

    const ctainerLayerGeoms& getPorts() const { return _ports; }

    friend ostream& operator<<(ostream & out, const dbMasterVia& arg)
    {
        arg.print(out);
        return out;
    }
};
#endif
