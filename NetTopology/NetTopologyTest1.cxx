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






#include "ABKCommon/abkcommon.h"
#include "DB/DB.h"
#include "dbHGr.h"

// Test for consistency between dbNetlist and HGraph produced therefrom

int main(int argc, char *argv[])
{
  StringParam auxFileName("f",argc,argv);
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux");
  BoolParam check("checkDB",argc,argv);

  DB::Parameters dbParams;
  dbParams.alwaysCheckConsistency = check;

  DB db(auxFileName,dbParams);

  dbNetlist const &netlist=db.getNetlist();
  HGfromDB graph(db);

  abkfatal (netlist.getNumCells()==graph.getNumNodes(),"Mismatch in number"
      " of cells");

    abkwarn (netlist.getNumNets()==graph.getNumEdges(),"Mismatch in number"
        " of nets");

  unsigned i,j;
  for (i=0;i<netlist.getNumNets();i++)
      {
      dbNet const &net = netlist.getNetByIdx(i);
      HGFEdge const &edge = graph.getEdgeByIdx(i);
      abkfatal(net.getNumCells()==edge.getDegree(),"degree mismatch on net/edge"
);

      unsigned sumCellIndices=0,sumNodeIndices=0;
      itCellLocal iC=net.cellsBegin();
      itHGFNodeLocal iNode=edge.nodesBegin();

      for (j=0;j<net.getNumCells();j++,iC++,iNode++)
          {
          dbCell const &cell = *(*iC);
          HGFNode const &node = *(*iNode);
          sumCellIndices += cell.getIndex();
          sumNodeIndices += node.getIndex();
          }

      abkfatal(sumCellIndices==sumNodeIndices,"Mismatch in sum of indices "
          " (cells on net versus nodes on edge) ");
      }

  for (i=0;i<netlist.getNumCells();i++)
      {
      dbCell const &cell = netlist.getCellByIdx(i);
      HGFNode const &node = graph.getNodeByIdx(i);

      abkfatal(cell.getWidth()*cell.getHeight()==node.getWeight(),
          "Weight mismatch");
      abkfatal(cell.getNumNets()==node.getDegree(),"Degree mismatch on cell/node");
      unsigned sumNetIndices=0,sumEdgeIndices=0;
      itNetLocal iN=cell.netsBegin();
      itHGFEdgeLocal iE=node.edgesBegin();

      for (j=0;j<cell.getNumNets();j++,iN++,iE++)
          {
          dbNet const &net = *(*iN);
          HGFEdge const &edge = *(*iE);
          sumNetIndices += net.getIndex();
          sumEdgeIndices += edge.getIndex();
          }

      abkfatal(sumNetIndices==sumEdgeIndices,"Mismatch in sum of indices "
          " (nets on cell versus edges on node) ");
      }

  for (i=0;i<netlist.getNumNets();i++)
      {
      dbNet const &net = netlist.getNetByIdx(i);
      HGFEdge const &edge = graph.getEdgeByIdx(i);

      itCellLocal iC=net.cellsBegin();
      itHGFNodeLocal iNode=edge.nodesBegin();

      for (j=0;j<net.getNumCells();j++,iC++,iNode++)
          {
          dbCell const &cell = *(*iC);
          HGFNode const &node = *(*iNode);
          abkfatal(cell.getIndex()==node.getIndex(),"Order mismatch (cells on "
              "net versus nodes on edge)");
          }
      }

  for (i=0;i<netlist.getNumCells();i++)
      {
      dbCell const &cell = netlist.getCellByIdx(i);
      HGFNode const &node = graph.getNodeByIdx(i);

      itNetLocal iN=cell.netsBegin();
      itHGFEdgeLocal iE=node.edgesBegin();

      for (j=0;j<cell.getNumNets();j++,iN++,iE++)
          {
          dbNet const &net = *(*iN);
          HGFEdge const &edge = *(*iE);
          if (net.getIndex()!=edge.getIndex())
              {
              cerr << "Cell = " << cell.getName() << 
                  " Index = " << cell.getIndex() << endl;

              itNetLocal iN2=cell.netsBegin();

              unsigned k;
              for (k=0;k<cell.getNumNets();k++,iN2++)
                  {
                  dbNet const &net2 = *(*iN2);
                  cerr << "        Net " << k <<" = " << net2.getName()
                      <<" Index = " << net2.getIndex() << endl;
                  }

              cerr << endl << "Node index = " << node.getIndex() << endl;
              itHGFEdgeLocal iE2;
              k=0;
              cerr << "    Srcs" << endl;
              for (iE2=node.srcEdgesBegin();iE2!=node.srcEdgesEnd();iE2++,k++)
                  {
                  HGFEdge const &edge2=*(*iE2);
                  cerr << "        Edge " << k << " Index = " << edge2.getIndex()
                      <<endl;
                  }

              cerr << "    SrcSnks" << endl;
              for (iE2=node.srcSnkEdgesBegin();iE2!=node.srcSnkEdgesEnd();iE2++,k++)
                  {
                  HGFEdge const &edge2=*(*iE2);
                  cerr << "        Edge " << k << " Index = " << edge2.getIndex()
                      <<endl;
                  }

              cerr << "    Snks" << endl;
              for (iE2=node.snkEdgesBegin();iE2!=node.snkEdgesEnd();iE2++,k++)
                  {
                  HGFEdge const &edge2=*(*iE2);
                  cerr << "        Edge " << k << " Index = " << edge2.getIndex()
                      <<endl;
                  }

              char errtxt[1023];
              sprintf(errtxt,"Order mismatch in position %d "
                  "(nets on cell versus edges on node)",j);

              abkfatal(0,errtxt);
              }
          }
      }

  return 0;
}
