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

#ifndef DB_ROUTE_H
#define DB_ROUTE_H

#include <iostream>
#include <ABKCommon/sgi_hash_map.h>
#include "dbDefines.h"
#include "dbTypes.h"
#include "dbNetlist.h"
#include "dbContext.h"
#include "dbMasterVia.h"
#include "dbRoutedNet.h"

class dbRoutingInfo
{
  protected:

    ctainerMasterVias     _masterVias;
    ctainerRoutedNets     _netlistRoute;

    struct eqstr
    {
        bool operator()(const char* s1, const char* s2) const
        { return strcmp(s1, s2) == 0; }
    };
    hash_map<const char*, unsigned, hash<const char*>, eqstr> _netHash;

    void print(ostream& out) const;
    void parseLEF(const dbContext& layout, const char* lefFileName);
    void parseDEF(const dbNetlist& netlist, const dbContext& layout,
                  const char* lefFileName);
    void fillHash(const dbNetlist& netlist);
    unsigned findLayer(const dbContext& layout, const char * layerName);
    dbMasterVia * findVia(const char * viaName);
    unsigned findNet(const char * netName);

  public:
    dbRoutingInfo() {}
    ~dbRoutingInfo()
    {
        unsigned i;
        for(i = 0; i < _masterVias.size(); i++)
            delete _masterVias[i];
        _masterVias.clear();
        for(i = 0; i < _netlistRoute.size(); i++)
            delete _netlistRoute[i];
        _netlistRoute.clear();
    }

    void readLEFDEF(const dbNetlist& netlist, const dbContext& layout,
                    const char * lefFileName, const char* defFileName);
    void readLEFDEF(const dbNetlist& netlist, const dbContext& layout,
                    const char * auxFileName);

    itMasterVia  masterViasBegin()  const { return _masterVias.begin(); }
    itMasterVia  masterViasEnd()    const { return _masterVias.end(); }
    itRoutedNet  routedNetsBegin()  const { return _netlistRoute.begin(); }
    itRoutedNet  routedNetsEnd()    const { return _netlistRoute.end(); }

    const dbMasterVia& getMasterViaByIdx(unsigned index) const
    { return *(_masterVias[index]); }

    bool isRouted() const { return _netlistRoute.size() != 0; }
    double getTotalWL() const;
    double getTotalHorizWL() const;
    double getTotalVertWL() const;

    friend ostream& operator<<(ostream& out, const dbRoutingInfo& arg)
    {
        arg.print(out);
        return out;
    }
};
#endif
