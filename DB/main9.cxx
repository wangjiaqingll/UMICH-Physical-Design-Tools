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









#include "abkcommon.h"
#include "DB.h"

struct PlotRec
{
   const char * name;
   Point loc;

   PlotRec():name(NULL), loc(0,0) {}
};

bool operator<(const PlotRec& pr0, const PlotRec& pr1)
{ return strcmp(pr0.name,pr1.name)<0; }

ostream& operator<<(ostream& os, PlotRec& pr)
{ return os << pr.name << "   @ " << pr.loc; }

int main(int argc, char *argv[])
{
  StringParam auxFileName("f",argc,argv);
  abkfatal(auxFileName.found(),
    "Usage: prog -f filename.aux -nl <numlevels> -min <cluster threshold>");
  DB db(auxFileName);

  UnsignedParam numLev("nl",argc,argv);
  unsigned NL=1;
  if (numLev.found()) NL=numLev;
  cout << " Number of clustering levels for output is " << NL << endl;

  UnsignedParam minSizePar("min",argc,argv);
  unsigned minSize=10;
  if (minSizePar.found()) minSize=minSizePar;
  cout << " Clusters with fewer than " << minSize << " cells will be ignored "<<endl;

  abkwarn(db.spatial.allCellsPlaced()," Not all cells are placed");

  vector<PlotRec> cells;
  const dbNetlist& netlist=db.getNetlist();
  itCellGlobal cit=netlist.cellsBegin();
 
  for(; cit!=netlist.cellsEnd(); cit++)
  {
     const dbCell& cell=(**cit);
     PlotRec pr;
     pr.name=cell.getName();
     pr.loc=db.spatial.locations[cell.getIndex()];
     cells.push_back(pr);
  } 

  unsigned k,idx;
//  DB sorts cells already
//  for(idx=0; idx!=cells.size(); idx++) cout << cells[idx] << endl;
    sort(cells.begin(),cells.end());
//  cout << " --------- " << endl;
//for(idx=0; idx!=cells.size(); idx++) cout << cells[idx] << endl;

  char form[300], curName[30]="/dev/null", gnuplotCommand[10000];
  sprintf(gnuplotCommand,"# set terminal postscript color\n"
                         "# set output \"%s-NC-L%d.eps\"\n"
                         "  set title \"%s / level %d/ threshold %d\"\n plot",
                         db.getAuxName(), NL, db.getAuxName(), NL, minSize);

  sprintf(form,"%s-NC-L%d-%%d.xy",db.getAuxName(),NL);

  char delim='/'; //db.getDelimiter();
  cout << " DB delimiter is " << delim << endl;
  cout << " " << cells.size() << " total " << endl;

  unsigned firstChars=0;
  PlotRec pr=cells[0];
  ofstream out(curName);
  unsigned missingCells=0;

  vector<PlotRec> toSave;
  toSave.push_back(pr);
  k=0;

  for(idx=1;idx<cells.size();)
  {
      const char*  curCellName=cells[idx].name;
      const char* prevCellName=pr.name;
      unsigned curLen=strlen(curCellName), prevLen=strlen(prevCellName);
      const char* shorter=(curLen<=prevLen ? curCellName:prevCellName);
      const char* tmpName=shorter;
      for(unsigned j=0; j<NL; j++)
      {
        tmpName=strchr(tmpName+1,delim);
        if (tmpName==NULL) j=NL;
      }
      bool compareall=false;
      if (tmpName==NULL) compareall=true;
      else firstChars=tmpName-shorter;

  if (idx==600) 
     sin(2.3);

      if ( compareall&& strcasecmp(curCellName,prevCellName)
         ||
     (!compareall&& strncasecmp(curCellName,prevCellName,firstChars))
         ) 
      {
        pr=cells[idx];
        if (toSave.size()<minSize)
        {
           missingCells+=toSave.size();
           toSave.clear();
           continue; 
        }
        out.close();
        sprintf(curName,form,k++);
        sprintf(gnuplotCommand,"%s \"%s\" w d,",gnuplotCommand,curName);
//      sprintf(gnuplotCommand,"%s \"%s\"    ,",gnuplotCommand,curName);
        out.open(curName);
        cout << "Writing " << curName << " ... " << endl;
        for(unsigned i=0;i!=toSave.size();i++)
           out << toSave[i].loc << "     #  " << toSave[i].name << endl;
        toSave.clear();
        continue;
      }
      pr=cells[idx++];
      toSave.push_back(pr);
  }

  if (!toSave.empty())
  {
      if (toSave.size()<minSize) missingCells+=toSave.size();
      else
    { 
      out.close();
      sprintf(curName,form,k++);
      out.open(curName);
      cout << "Writing " << curName << " ... " << endl;
      for(unsigned i=0;i!=toSave.size();i++)
         out << toSave[i].loc << "     #  " << toSave[i].name << endl;
    }
      toSave.clear();
  }
  out.close();
  cout << "  " << missingCells << " were in clusters of <"<<minSize <<endl;

  gnuplotCommand[strlen(gnuplotCommand)-1]='\n';
  ofstream("toplot.gp") << gnuplotCommand << endl << "pause -1" << endl;
  cout << " Writing toplot.gp" << endl;

  return 0;
}
