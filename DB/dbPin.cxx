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









//  CREATED 970826 by ilm
//  CHANGES
//
//  970905  aec  added ComparePinsBy* operator() functions
//  970908  aec  changed sorting to put UNKNOWNS last
//  980108  mro  changed tiebreak in ComparePinsByDirAnName to cell name
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "dbPin.h"
#include "dbNet.h"


bool compareMPinsByName(dbMasterPin* p1, dbMasterPin* p2);

dbMasterPin::~dbMasterPin()
{
   delete[] _name;
   unsigned k,size=_ports.size();
   for(k=0;k<size;k++) delete _ports[k];
}


bool ComparePinsByDirAndName::operator() ( dbPin* p1, dbPin* p2) const
{
    if(p1->isDirOutput() || p1->isDirOutputTristate() )
    {
        if(p2->isDirOutput() || p2->isDirOutputTristate() )
        {
            int strCmpResult = strcmp(p1->getMaster().getName(),
                                  p2->getMaster().getName());
            if(strCmpResult < 0)      return true;
            else if(strCmpResult > 0) return false;
            else
                {
                CompareCellsByName cc;
                return cc(&(p1->getCell()),&(p2->getCell()));
                }
        } else
            return true;
    } else if(p2->isDirOutput() || p2->isDirOutputTristate() )
        return false;
 
    if(p1->isDirInOut() )
    {
        if(p2->isDirInOut() )
        {
            int strCmpResult = strcmp(p1->getMaster().getName(),
                                  p2->getMaster().getName());
            if(strCmpResult < 0)      return true;
            else if(strCmpResult > 0) return false;
            else
                {
                CompareCellsByName cc;
                return cc(&(p1->getCell()),&(p2->getCell()));
                }
        } else
            return true;
    } else if(p2->isDirInOut() )
        return false;

    if(p1->isDirInput() )
    {
        if(p2->isDirInput() )
        {
            int strCmpResult = strcmp(p1->getMaster().getName(),
                                  p2->getMaster().getName());
            if(strCmpResult < 0)      return true;
            else if(strCmpResult > 0) return false;
            else
                {
                CompareCellsByName cc;
                return cc(&(p1->getCell()),&(p2->getCell()));
                }
        } else
            return true;
    } else if(p2->isDirInput() )
        return false;
 
    int strCmpResult = strcmp(p1->getMaster().getName(),
                          p2->getMaster().getName());
    if(strCmpResult < 0)      return true;
    else if(strCmpResult > 0) return false;
    else
        {
        CompareCellsByName cc;
        return cc(&(p1->getCell()),&(p2->getCell()));
        }
}
 
bool ComparePinsByCellIndexAndDir::operator() ( dbPin* p1, dbPin* p2) const
{
    if(p1->getCell().getIndex() != p2->getCell().getIndex() )
        return (p1->getCell().getIndex() < p2->getCell().getIndex());
 
        //pins on the same cell..sort them by dir.
 
    ComparePinsByDirAndName comp;
    return comp(p1, p2);
}

bool ComparePinsByDirAndName::operator()(dbMasterPin* p1, dbMasterPin* p2) const
{
    if(p1->isDirOutput() || p1->isDirOutputTristate() )
    {
        if(p2->isDirOutput() || p2->isDirOutputTristate() )
        {
            return compareMPinsByName(p1, p2);
        } else
            return true;
    } else if(p2->isDirOutput() || p2->isDirOutputTristate() )
        return false;
 
    if(p1->isDirInOut() )
    {
        if(p2->isDirInOut() )
        {
            return compareMPinsByName(p1, p2);
        } else
            return true;
    } else if(p2->isDirInOut() )
        return false;


    if(p1->isDirInput() )
    {
        if(p2->isDirInput() )
        {
            return compareMPinsByName(p1, p2);
        } else
            return true;
    } else if(p2->isDirInput() )
        return false; 

    return compareMPinsByName(p1, p2);
        
}

	//added to get around comp. problems with optimization
bool compareMPinsByName(dbMasterPin* p1, dbMasterPin* p2)
{
    int strCmpResult;
    strCmpResult = strcmp(p1->getName(), p2->getName());
    if(strCmpResult < 0)      return true;
    else if(strCmpResult > 0) return false;
    else return p1 < p2;
}

 
bool ComparePinsByCellIndexAndDir::operator()(dbMasterPin* p1, dbMasterPin* p2) const
{
    int strCmpResult;
   
    strCmpResult = strcmp(p1->getCell().getName(), p2->getCell().getName());
    if(strCmpResult < 0) return true;
    else if(strCmpResult > 0) return false;
    else
    { 
        //pins on the same cell..sort them by dir.
        ComparePinsByDirAndName comp;
        return comp(p1, p2);
    }
}

