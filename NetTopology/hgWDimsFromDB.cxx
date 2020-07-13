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











//created by Andrew Caldwell on 02/02/99
// 020822  ilm  ported to g++ 3.1

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "ABKCommon/sgi_hash_set.h"
#include "hgWDimsFromDB.h"

    struct eqstrX
    {
         bool operator()(const char* s1, const char* s2) const
         { return strcmp(s1, s2) == 0; }
    };

//most of this code is copied from the HGfromDB ctor

HGraphWDimensionsFromDB::HGraphWDimensionsFromDB(const DB& db)
    :HGraphWDimensions(db.getNetlist().getNumCells(), 1) 
{
    const dbNetlist& netlist = db.getNetlist();
//  cout << " Num pins: " << netlist.getNumPins() << endl;

    unsigned numCells = netlist.getNumCells();
    if (db.spatial.constraints.hasFGC())
      _numTerminals=db.spatial.constraints.getFGC().getSize();
    else _numTerminals=0;

    _nodeNames = vector<const char*>(getNumNodes());

    for(unsigned i=0; i < numCells; i++)
    {
        const dbCell &cell=netlist.getCellByIdx(i);
	_nodeWidths[i]  = cell.getWidth();
	_nodeHeights[i] = cell.getHeight();
	_nodeSymmetries[i] = cell.getMaster().getSymmetry();
        _nodes[i]->setWeight(_nodeHeights[i] * _nodeWidths[i]);

        char* tmpName;
        if(cell.getName() == NULL)
        {
            tmpName = new char[9];
            sprintf(tmpName, "cell%d", i);
        }
        else
        {
            tmpName = new char[strlen(cell.getName())+1];
            strcpy(tmpName, cell.getName());
        }
        _nodeNames[i] = tmpName;
        _nodeNamesMap[tmpName] = i;
    }

    const vector<char*>& fileNames=db.getFileNames();
    unsigned k;
    char buf[1024], *ptr=NULL;
    for(k=0; k!=fileNames.size(); k++)
    {
       if (!strstr(fileNames[k],".kll") && !strstr(fileNames[k],".KLL"))
          continue;
       cout << " Found potential kill-file name " << fileNames[k] << "...\n";
       if(fileNames[k][0] == '/')
       {
         strcpy(buf,fileNames[k]);
         ptr=buf;
       }
       else
       {
         cout << " will assume it is in directory " << db.getDirName() << endl;
         strcpy(buf,db.getDirName());
         ptr=buf+strlen(db.getDirName());
         strcpy(ptr,fileNames[k]);
       }
    }

    hash_set<const char*, hash<const char*>, eqstrX> netsToKill;

    if (!ptr) 
    { cout << " HGraphWDimsFromDB: no kill-file name found in DB" << endl; }
    else
    {
      cout << " Trying to open file " << buf << " ... " << flush << endl;
      ifstream killFile(buf);
      k=0;
      if (!killFile) { cout << " failed " << endl; }
      else
      {
         while(killFile)
         {
            killFile >> buf;
            if (strlen(buf)==0) continue;
            netsToKill.insert(strdup(buf));
            k++;
         }
         cout << " Read " << k << " nets to kill " << endl;
      }
    }

    itNetGlobal nItr;
    unsigned idx=0;
    for(nItr = netlist.netsBegin(); nItr != netlist.netsEnd(); nItr++, idx++)
    {
	const dbNet& net = **nItr;

        if ( netsToKill.find(net.getName())!=netsToKill.end() )
        {
            cout << "  killed net " << net.getName() << endl;
            continue;
        }

        HGFEdge* edge;
	if(net._weight == 1.0)
	  edge=addEdge();
	else
	  edge=addEdge(net._weight);

        _netNames.push_back(strdup(net.getName()));
        _netNamesMap[_netNames.back()]=idx;

//      abkassert(edge->getIndex()==(*nItr)->getIndex(),
//      			"netlist traversed out of order");
	abkassert(edge->getWeight() > 0, "constructed 0 weight edge");

        itCellLocal c;
        for(c=(*nItr)->sourcesBegin(); c!=(*nItr)->sourcesEnd(); c++)
	{
	    const dbCell &cell=**c;
	    const dbPin*  pin  = cell.commonPin(&net);
	    abkassert(pin != NULL, "null commonPin return");

	    addSrcWPin(getNodeByIdx(cell.getIndex()), *edge, 
					pin->getMaster().getCenter());
	}

        for(c=(*nItr)->sourceSinksBegin(); c!=(*nItr)->sourceSinksEnd(); c++)
	{
	    const dbCell &cell=**c;
	    const dbPin*  pin  = cell.commonPin(&net);
	    abkassert(pin != NULL, "null commonPin return");

	    addSrcSnkWPin(getNodeByIdx(cell.getIndex()), *edge,
					pin->getMaster().getCenter());
	}

        for(c=(*nItr)->sinksBegin(); c!=(*nItr)->sinksEnd(); c++)
	{
	    const dbCell &cell=**c;
	    const dbPin*  pin  = cell.commonPin(&net);
	    abkassert(pin != NULL, "null commonPin return");

	    addSnkWPin(getNodeByIdx(cell.getIndex()), *edge,
					pin->getMaster().getCenter());
	}
    }

    finalize();
    sortAsDB();

/*    const dbSimpleTiming& simpleTiming(db.easyAccess._simpleTiming);*/
    const dbSimpleTiming& simpleTiming = db.easyAccess._simpleTiming;

/*
       cout << " pins : " << getNumPins() << endl;
       cout << " pins1: " << _edgesOnNodePinIdx.size() << endl;
       cout << " offsets " << _pinOffsets.size() << endl;

       cout << "EoN" << _edgesOnNodePinIdx << endl;
*/

    if (simpleTiming.populated)
    {
       cout << " Will now convert dbSimpleTiming into hgDelayInfo" << endl;
       _delayInfo.populated=true;
       _delayInfo.rInt_c=simpleTiming.rInt_c;
       _delayInfo.rInt_k=simpleTiming.rInt_k;
       _delayInfo.cInt_c=simpleTiming.cInt_c;
       _delayInfo.cInt_k=simpleTiming.cInt_k;
       _delayInfo.drivRes.resize(_pinOffsets.size());
       _delayInfo.sinkCap.resize(_pinOffsets.size());

       char combinedName[1024];
       unsigned k=0;
       for(;k!=_nodeNames.size();k++)
       {
//        cout << " processing node " << k << " Name: " << _nodeNames[k]<<endl;
          bool fakedCell= strstr(_nodeNames[k], "FakedInstantiatedCellFor");
          const dbCell& cell=db.easyAccess.getCellByName(_nodeNames[k]);
          if (!fakedCell)
               sprintf(combinedName,"%s\t",cell.getMaster().getName());
          else
               sprintf(combinedName,"%s\tFakedInstantiatedPin",_nodeNames[k]);

          char *namePtr=combinedName+strlen(combinedName);
          const HGFNode& node=getNodeByIdx(k);
          unsigned eIdx=_eonBeginEnd[k];
//        cout << " eonBeginEnd["<<k<<"]=" << eIdx 
//             << " eonPinIdx["<<eIdx<<"]="<< _edgesOnNodePinIdx[eIdx] << endl;
          itHGFEdgeLocal eIt=node.edgesBegin();
          for(;eIt!=node.edgesEnd(); eIt++, eIdx++)
          {
/*           if (eIdx>=getNumPins())
             {
                cout << " eIdx:" << eIdx << " NumPins:" <<getNumPins()<<endl;
                abkfatal(0," eIdx too large ");
             }
*/
             if (fakedCell)
             {
                 const dbSimpleTiming::PinRecord* prpad;
                 prpad=simpleTiming.getPinRecordByName(combinedName);
// /*
                 if(prpad)
                 {
                     if(prpad->drivRes != DBL_MAX)
                         _delayInfo.drivRes[_edgesOnNodePinIdx[eIdx]]=
                             prpad->drivRes;
                     else
                         _delayInfo.drivRes[_edgesOnNodePinIdx[eIdx]]=0.0;
                     if(prpad->sinkCap != DBL_MAX)
                         _delayInfo.sinkCap[_edgesOnNodePinIdx[eIdx]]=
                             prpad->sinkCap;
                     else
                         _delayInfo.sinkCap[_edgesOnNodePinIdx[eIdx]]=0.0; 
                 }
                 else
// */
                 {
                     _delayInfo.drivRes[_edgesOnNodePinIdx[eIdx]]=0.0;
                     _delayInfo.sinkCap[_edgesOnNodePinIdx[eIdx]]=0.0; 
                 }

/*
                 cout << "PAD: " << combinedName
                      << " R=" << _delayInfo.drivRes[_edgesOnNodePinIdx[eIdx]]
                      << " C=" << _delayInfo.sinkCap[_edgesOnNodePinIdx[eIdx]]
                      << "\n    " << _edgesOnNodePinIdx[eIdx] 
                      << endl;
// */
                 continue;
             }

             const char* edgeName=getNetNameByIndex((*eIt)->getIndex());
//           cout << " looking at edge " 
//                << (*eIt)->getIndex() << " " << edgeName << endl;
             const dbNet& net=db.easyAccess.getNetByName(edgeName);
             const dbPin* pinPtr=cell.commonPin(&net);
             abkfatal(pinPtr," No common pin!");
             
             // now will extend combinedName
             if (pinPtr->isDirOutput()) strcpy(namePtr,"DRIVER-PIN__");
             else            strcpy(namePtr,pinPtr->getMasterPinName()); 
//           cout << " Combined name: " << combinedName << endl;
             const dbSimpleTiming::PinRecord* pr;
             pr=simpleTiming.getPinRecordByName(combinedName);
             abkfatal3(pr,"Didn't find pin with combined name : ",
                           combinedName,"\n");
//           cout << pr->drivRes << " " << pr->sinkCap << endl;
             if (pinPtr->isDirOutput())
             {
                 abkfatal3(pr->drivRes!=DBL_MAX,"Driver pin ",
                           pinPtr->getMasterPinName(),
                           " does not have drivRes defined\n");
                 _delayInfo.drivRes[_edgesOnNodePinIdx[eIdx]]=pr->drivRes;
             }
             else
             {
                 abkfatal3(pr->sinkCap!=DBL_MAX,"Sink   pin ",
                           pinPtr->getMasterPinName(),
                           " does not have sinkCap defined\n");
//            cout << "Name: " << _nodeNames[k] << ": ";
//            cout << pr->sinkCap << endl;
//            cout << eIdx << " " << _edgesOnNodePinIdx[eIdx] << endl;
                 _delayInfo.sinkCap[_edgesOnNodePinIdx[eIdx]]=pr->sinkCap;
             }
          }
       }
       cout << " hgDelayInfo constructed successfully " << endl;
    }
}
