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









// Created by Igor Markov, Aug 17, 1997
//  from dbCell.h (code by Andy Caldwell)
// 970818   ilm    got rid of the UMRs (bug) in the dMasterCells ctors
// 970818   aec    updated setTypeFromMasterCell to spec in dbTypes1.0
// 970818   aec    fixed bug in induceNetsFromPins
// 970819   ilm    initialized _fallDelay and _riseDelay with -1.234567
// 970819   aec    removed sets from induceNetsFromPins
// 970820   mro    added const_casts to _nets.erase() call if _MSC_VER
// 970825   ilm    merged in the static member initializers from dbDesign
// 970826   ilm    reworked ~dbMasterCell
// 970908   aec    fixed bug in EXTPIN type directionality
// 970911   mro    made non-text-based ctor for dbMasterCellType include
//                 the pad subclass
// 970919   pat    added reOrientRec
// 971004   pat    ...and added the missing lines to case 180.
// 020811   ilm    ported to g++ 3.0
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif


#include <functional>
using std::less;
#include <algorithm>
#include "Placement/symOri.h"
#include "dbDefines.h"
#include "dbTypes.h"
#include "dbPin.h"
#include "dbCell.h"
#include "dbCommonPin.h"
#include "dbContext.h"

char dbCell:: _forAllPinsFlag = false;
char dbCell:: _forAllNetsFlag = false;

dbMasterCell::dbMasterCell(char* name, char* type, char* sType,
                           double wth, double ht, Point orig,
                           const ctainerMasterPinsLocal& pns, Symmetry sym,
                           const dbSite & site)
            :_type(type, sType), 
            _fallDelay(-1.234567), _riseDelay(-1.234567),
             _width(wth), _height(ht),
            _origin(orig), _inDegree(0), _outDegree(0),
            _pins(pns), _symmetry(sym), _site(site)
{
    _name = new char[strlen(name)+1];
    strcpy(_name,name);

    itMasterPinLocal pItr;
    for(pItr = _pins.begin(); pItr != _pins.end(); pItr++)
    {
        if((*pItr)->isDirOutput() || (*pItr)->isDirOutputTristate())
            _outDegree++;
        else if( (*pItr)->isDirInput() )
            _inDegree++;
        else if( (*pItr)->isDirInOut() )
        {
            _inDegree++;
            _outDegree++;
        }
            //note..Feedthrough and Unknown are not counted in this
    }
}

dbMasterCell::dbMasterCell(char* name, dbMasterCellType::MClass type,
                           dbMasterCellType::PadSubClass stype,
                           double wth, double ht, Point orig,
                           const ctainerMasterPinsLocal& pns,
                           const ctainerLayerGeoms &obs,
                           Symmetry sym, const dbSite & site)
            :_type(type,stype), _fallDelay(-1.234567), _riseDelay(-1.234567),
             _width(wth), _height(ht),
            _origin(orig), _inDegree(0), _outDegree(0),
            _pins(pns), _obstacles(obs), _symmetry(sym), _site(site)
{
    _name = new char[strlen(name)+1];
    strcpy(_name,name);

    itMasterPinLocal pItr;
    for(pItr = _pins.begin(); pItr != _pins.end(); pItr++)
    {
        if((*pItr)->isDirOutput() || (*pItr)->isDirOutputTristate())
            _outDegree++;
        else if( (*pItr)->isDirInput() )
            _inDegree++;
        else if( (*pItr)->isDirInOut() )
        {
            _inDegree++;
            _outDegree++;
        }
            //note..Feedthrough and Unknown are not counted in this
    }
}

dbMasterCell::~dbMasterCell()
{
    unsigned k, size=_obstacles.size();
    for(k=0;k<size;k++) delete _obstacles[k];
    delete[] _name;
}

dbCell::~dbCell()
{
   delete[] _name;
}

void dbCell::setTypeFromMaster()
{
    if( _master.isCover())
        _type = dbCellType(dbCellType::MacroOTHER, dbCellType::PUNKNOWN);

    else if( _master.isRing())
        _type = dbCellType(dbCellType::MacroOTHER, dbCellType::PUNKNOWN);

    else if( _master.isBlock())
        _type = dbCellType(dbCellType::BLOCK, dbCellType::PUNKNOWN);

    else if( _master.isCore())
    {
        if(_master.isFlipFlop() || _master.isLatch() )
                _type = dbCellType(dbCellType::CORE, dbCellType::SC);
        else
                _type = dbCellType(dbCellType::CORE, dbCellType::CC);
    }
    else if( _master.isPad())
    {   
	if(_master.isPI() )
		_type = dbCellType(dbCellType::IO, dbCellType::PI);
	else if(_master.isPO() )
		_type = dbCellType(dbCellType::IO, dbCellType::PO);
	else if(_master.isPIO() )
		_type = dbCellType(dbCellType::IO, dbCellType::PIO);
	else if(_master.isPPower() || _master.isPSpacer() )
		_type = dbCellType(dbCellType::IO, dbCellType::POTHER);
	else 
	{		//UNKNOWN..see if we can find the direction

            unsigned numSnk, numSrc;
            numSnk = numSrc = 0;
            itNetLocal nItr;
            for(nItr = netsBegin(); nItr != netsEnd(); nItr++)
            {
            	if(commonPinIsCellInput(*nItr) )
                    numSnk++;
            	if(commonPinIsCellOutput(*nItr) )
                    numSrc++;
            }

            if( (numSrc > 0) && (numSnk > 0) )
                _type = dbCellType(dbCellType::IO, dbCellType::PIO);
            else if( numSrc > 0 )
                _type = dbCellType(dbCellType::IO, dbCellType::PI);
            else if( numSnk > 0)
                _type = dbCellType(dbCellType::IO, dbCellType::PO);
            else
                _type = dbCellType(dbCellType::IO, dbCellType::PUNKNOWN);
	}
    }
   else if( _master.isEndCap())
        _type = dbCellType(dbCellType::MacroOTHER, dbCellType::PUNKNOWN);

    else if( _master.isExtPin())
    {
        abkfatal(_master.getNumPins() == 1, "ExtPin with wrong number of pins");
        dbMasterPin* mpin;
        mpin = *(_master.pinsBegin());

        if(mpin->isDirInput() )
            _type = dbCellType(dbCellType::IO, dbCellType::PO);

        else if(mpin->isDirOutput() || mpin->isDirOutputTristate() )
            _type = dbCellType(dbCellType::IO, dbCellType::PI);

        else if(mpin->isDirInOut() )
            _type = dbCellType(dbCellType::IO, dbCellType::PIO);

        else if(mpin->isDirFeedthrough() )
            _type = dbCellType(dbCellType::IO, dbCellType::POTHER);

        else //unknown
            _type = dbCellType(dbCellType::IO, dbCellType::PUNKNOWN);
    }

    else if( _master.isUnknown())
    _type = dbCellType(dbCellType::MacroUNKNOWN, dbCellType::PUNKNOWN);

}

void dbCell::induceNetsFromPins(vector<dbNet*> *sNets)
{
        //create the vector of nets, from the vector of pins. to be called
        //by the post-processor.  removes the duplicate nets, and the
        //special nets.

    itPinLocal        pinItr;
    const dbNet*      tmpNet; // changed by ILM
    itNetGlobal       sNetItr;

	//first, put all connected nets that are not special in _nets
    for(pinItr = pinsBegin(); pinItr != pinsEnd(); pinItr++)
    {
        if(!(*pinItr)->isConnected() ) continue;

        tmpNet= &((*pinItr)->getNet() );

        for(sNetItr = sNets->begin(); sNetItr != sNets->end(); sNetItr++)
                if( (*sNetItr) == tmpNet ) break;

        if (sNetItr == sNets->end())//not a special net
                _nets.push_back(const_cast<dbNet*>(tmpNet));
    }
	//sort the net, by pointers.
    std::sort(_nets.begin(), _nets.end(), less<dbNet*>());

	//remove duplicates
//    itNetLocal 
    ctainerNetsLocal::iterator dupItr = std::unique(_nets.begin(), _nets.end());
    _nets.erase(dupItr, _nets.end());
//    _nets.erase(const_cast<dbNet**> (dupItr),
//                const_cast<dbNet**> (_nets.end()));
}

bool CompareCellsByDir::operator() ( dbCell* c1, dbCell* c2) const
{
    const dbPin* c1Pin = c1->commonPin(_curNet);
    const dbPin* c2Pin = c2->commonPin(_curNet);
    if(c1Pin == NULL || c2Pin == NULL)
            return (c1->getIndex() < c2->getIndex() );


    if(c1Pin->isDirOutput() || c1Pin->isDirOutputTristate())//src
    {
        if(c2Pin->isDirOutput() || c2Pin->isDirOutputTristate())//both src
            return (c1->getIndex() < c2->getIndex() );
        else
            return true;
    }

	//only the 2nd one is a source.. 
    if(c2Pin->isDirOutput() || c2Pin->isDirOutputTristate())
	return false;

    if(c1Pin->isDirInOut())
    {
	if(c2Pin->isDirInOut())	//both inouts
            return (c1->getIndex() < c2->getIndex() );
        else
            return true;
    }

    if(c2Pin->isDirInOut())  //only 2nd one is inout
	return false;
 
    //neither one is a source..so put them in index order
    return (c1->getIndex() < c2->getIndex() );
}

bool CompareObstaclesByXmin::operator() (dbLayerGeom* o1, dbLayerGeom* o2) const
{
    Rectangle r1 = *o1;
    Rectangle r2 = *o2;
    if(r1.xMin < r2.xMin)
       return true;
    if(r2.xMin < r1.xMin)
       return false;
    if(r1.yMin < r2.yMin)
       return true;
    if(r2.yMin < r1.yMin)
       return false;
    if(r1.xMax < r2.xMax)
       return true;
    if(r2.xMax < r1.xMax)
       return false;
    if(r1.yMax < r2.yMax)
       return true;
    if(r2.yMax < r1.yMax)
       return false;
    int compareNames=strcmp(o1->getLayer().getName(),
                            o2->getLayer().getName());
    if (compareNames<0)
        return true;
    if (compareNames>0)
        return false;
    return (o1 < o2);
}


Rectangle dbMasterCell::reOrientRec (const Rectangle& rec,
				     const Orientation& orien)
const
/*
   Assuming rec is given in coordinates relative to lower-lefthand
   corner of master, return rectangle after transformation of
   coordinates to cell instance orientation.

   If the _origin field of dbMasterCell is ever used for
   a non-(0,0) value, then this function can be used to
   normalize coordinates w.r.t. lower-lefthand corner.
   (But now there is no public access to _origin, so we
   assume that value is not important.)
*/
{
  Rectangle newRec = rec;
  double w = getWidth();
  double h = getHeight();
  double temp;

  // It can happen in our test cases that the port coordinates
  // lie outside of the cell.  Should this be an error?
  //abkfatal(w >= rec.xMax, "Width less than xMax");
  //abkfatal(h >= rec.yMax, "Height less than yMax");
  
  if (!orien.isFaceUp())     // flip around y axis before rotating
  {
    newRec.xMin = w - rec.xMax;
    newRec.xMax = w - rec.xMin;
  }

  switch (orien.getAngle())
  {
  case 0:
    break;
  case 90:
    newRec.yMin = w - newRec.xMax;
    newRec.yMax = w - newRec.xMin;
    newRec.xMin = rec.yMin;
    newRec.xMax = rec.yMax;
    break;
  case 180:
    temp = newRec.xMin;
    newRec.xMin = w - newRec.xMax;
    newRec.xMax = w - temp;
    newRec.yMin = h - rec.yMax;
    newRec.yMax = h - rec.yMin;
    break;
  case 270:
    newRec.yMin = newRec.xMin;
    newRec.yMax = newRec.xMax;
    newRec.xMin = h - rec.yMax;
    newRec.xMax = h - rec.yMin;
    break;
  default:
    abkfatal(false,"Unanticipated orientation angle");
  }
  
  abkfatal(newRec.xMin <= newRec.xMax, "Reversed coordinates");
  abkfatal(newRec.yMin <= newRec.yMax, "Reversed coordinates");
  return newRec;
}
   
void dbCell::sortNets()
{	//sort the nets on the cell by direction first, then,
	//by net ID
   std::sort(_nets.begin(), _nets.end(), CompareNetsByDirection(*this));
}

ostream& operator<<(ostream& os, const dbCell& cell)
{
    os<<" DBCell #"<<cell.getIndex();

    const char* name = cell.getName();
    if(name != NULL)
	os<<" ("<<name<<")"<<endl;
    else
	os<<endl; 

    os<<"Type: "<<static_cast<const char*>(cell.getType())<<endl;
    os<<"height: "<<cell.getHeight()<<"  width: "<<cell.getWidth()<<endl;
    os<<"degree: "<<cell.getNumNets()<<endl;
  
    return os;  
}

