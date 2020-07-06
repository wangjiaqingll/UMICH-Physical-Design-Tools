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








// adapted by Igor Markov from Max Moroz original
// 020822  ilm  ported to g++ 3.1.1
// TODO add Class Verbosity to HGfromDB ctor
//      and use the hooks Max left for this in the code below


#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "DB/dbDesign.h"
#include "DB/DB.h"
#include "dbHGr.h"
#include "ABKCommon/sgi_hash_set.h"

// will be removed when HGraph supports verbosity
struct DummyVerbosity
{
    unsigned verbosity;
    DummyVerbosity() : verbosity(0) { }
} fParams;

    struct eqstrX
    {
         bool operator()(const char* s1, const char* s2) const
         { return strcmp(s1, s2) == 0; }
    };

// this ctor *doesn't* remove: self-loops, duplicate edges, non-TimingNode's
// node indexing corresponds to the cell indexing in the netlist
// edge indexing corresponds to the net indexing in the netlist

HGfromDB::HGfromDB(const DB& db) 
    : HGraphFixed(db.getNetlist().getNumCells(), 1)
{
    if (db.spatial.constraints.hasFGC())
      _numTerminals=db.spatial.constraints.getFGC().getSize();
    else _numTerminals=0;

    _nodeNames = vector<const char*>(getNumNodes());

    const dbNetlist& netlist = db.getNetlist();

    for(unsigned i=0; i<netlist.getNumCells(); i++)
    {
        const dbCell &cell=netlist.getCellByIdx(i);
        _nodes[i]->setWeight(cell.getHeight()*cell.getWidth());

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
            killFile >> buf ;
            netsToKill.insert(strdup(buf));
            k++;
         }
         cout << " Read " << k << " nets to kill " << endl;
      }
    }

    bool numberMismatch = false;

    itNetGlobal nItr;
    for(nItr = netlist.netsBegin(); nItr != netlist.netsEnd(); nItr++)
    {
        if (fParams.verbosity>=100)
           cout<<"net #"<<(*nItr)->getIndex()<<" "<<(*nItr)->getName()<<"\n"
           <<"   cells: "<<(*nItr)->getNumCells()<<"\n"
           <<"      includes OUTs: "<<(*nItr)->getNumOut()<<"\n"
                <<"      includes IN/OUTs: "<<(*nItr)->getNumInOut()<<"\n";

	if((*nItr)->getDegree() <= 1) continue;

        const dbNet& net = **nItr;

        if ( netsToKill.find(net.getName())!=netsToKill.end() )
        {
            cout << " Killing net " << net.getName() << endl;
            continue;
        }

        HGFEdge* edge=addEdge();
//	if(edge->getIndex()!=(*nItr)->getIndex())
//		numberMismatch = true;

        itCellLocal c;
        for(c=(*nItr)->sourcesBegin(); c!=(*nItr)->sourcesEnd(); c++)
        {
            if (fParams.verbosity>=100)
               cout<<"source cell #"<<(*c)->getIndex()
                    <<" "<<(*c)->getName()<<"\n";
	    addSrc(getNodeByIdx((*c)->getIndex()), *edge);
        }
        for(c=(*nItr)->sourceSinksBegin(); c!=(*nItr)->sourceSinksEnd(); c++)
        {
            if (fParams.verbosity>=100)
               cout<<"sourceSink cell #"<<(*c)->getIndex()
                    <<" "<<(*c)->getName()<<"\n";
	    addSrcSnk(getNodeByIdx((*c)->getIndex()), *edge);
        }
        for(c=(*nItr)->sinksBegin(); c!=(*nItr)->sinksEnd(); c++)
        {
            if (fParams.verbosity>=100)
               cout<<"sink cell #"<<(*c)->getIndex()
                    <<" "<<(*c)->getName()<<"\n";
	    addSnk(getNodeByIdx((*c)->getIndex()), *edge);
        }
        //edge->SetWeight() can be called here; star-model graph weights were:
    }

    abkwarn(!numberMismatch,
		"net traversed out of order, or there were degree 1 edges");

    finalize();
    sortAsDB();
}

