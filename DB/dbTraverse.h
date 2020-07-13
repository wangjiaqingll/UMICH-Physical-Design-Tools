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









#ifndef _DBTRAVERSE_H
#define _DBTRAVERSE_H

#include "dbDefines.h"
//CHANGES

//970819  aec  added forallSNets


/*
_________ for each CELL in NETLIST______________

static itCellGlobal __CItr;

#define forallCells(cell, nl) \
    for((cell)= *(__CItr=(nl)->cellsBegin()), (nl)->loopCellsBegin();\
        __CItr != (nl)->cellsEnd(); \
        (cell)= *(++__CItr), (nl)->loopCellsEnd(__CItr) )


_________ for each NET in NETLIST________________

static itNetGlobal __NItr; 

#define forallNets(net, nl)\
    for(net= *(__NItr=(nl)->netsBegin()), (nl)->loopNetsBegin();\
        __NItr != (nl)->netsEnd(); \
        net=(*(++__NItr)), (nl)->loopNetsEnd(__NItr) ) 


_________for each SNET in NETLIST________________


#define forallSNets(net, nl)\
    for(net= *(__NItr=(nl)->sNetsBegin()), (nl)->loopSNetsBegin();\
        __NItr != (nl)->sNetsEnd(); \
        net=(*(++__NItr)), (nl)->loopSNetsEnd(__NItr) )


__________ for each CELL on a NET_________


static itCellLocal __CNItr; 

#define forallIncidentCells(cell, net) \
    for(cell= *(__CNItr=(net)->cellsBegin()), (net)->loopCellsBegin();\
        __CNItr != (net)->cellsEnd();\
        cell= *(++__CNItr), (net)->loopCellsEnd(__CNItr) ) 


__________ for each NET on a CELL________


static itNetLocal __NCItr;

#define forallIncidentNets(net, cell) \
    for(net= *(__NCItr=(cell)->netsBegin()), (cell)->loopNetsBegin();\
        __NCItr != (cell)->netsEnd();\
        net= *(++__NCItr), (cell)->loopNetsEnd(__NCItr) )

static itPinLocal __PCItr;

#define forallPinsOnCell(pin, cell) \
    for( pin= *(__PCItr=(cell)->pinsBegin()), (cell)->loopPinsBegin();\
         __PCItr != (cell)->pinsEnd();\
         pin= *(++__PCItr), (cell)->loopPinsEnd(__PCItr) )

__________for each PIN on a NET__________________

static itPinLocal __PNItr;

#define forallPinsOnNet(pin, net) \
    for( pin= *(__PNItr=(net)->pinsBegin()), (net)->loopPinsBegin();\
         __PNItr != (net)->pinsEnd(); \
         pin= *(++__PNItr), (net)->loopPinsEnd(__PNItr) )
*/


#endif

