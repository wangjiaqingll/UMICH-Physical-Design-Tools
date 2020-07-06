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





#include <vector>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include "Node.h"

namespace parquetfp
{
class Nets;
using namespace std;
typedef std::vector<Node>::iterator itNode;

class Nodes
{
 private:
  vector<Node> _nodes;
  vector<Node> _terminals;
  
 public:
  Nodes(char* baseName);
  Nodes(void)
    {}

  unsigned getNumNodes(void);

  void parseNodes(char* fnameBl);
  void parsePl(char* fnamePl);

  Node& getNode(unsigned index)
    { return _nodes[index]; }

  Node& getTerm(unsigned index)
    { return _terminals[index]; }

  void putNewNode(Node& node)
    { _nodes.push_back(node); }

  void putNewTerm(Node& term)
    { _terminals.push_back(term); }

  void clearNodes(void)
    { _nodes.resize(0); }

  void clearTerm(void)
    { _terminals.resize(0); }

  void clean(void)
    {
      _nodes.clear();
      _terminals.clear();
    }

  itNode nodesBegin(void)
    { return _nodes.begin(); }

  itNode nodesEnd(void)
    { return _nodes.end(); }


  unsigned getNumTerminals(void)
    { return _terminals.size(); }
  
  Node& getTerminal(unsigned index)
    { return _terminals[index]; }

  itNode terminalsBegin(void)
    { return _terminals.begin(); }
  itNode terminalsEnd(void)
    { return _terminals.end(); }
  
  vector<double> getNodeWidths();
  vector<double> getNodeHeights();
  vector<double> getXLocs();
  vector<double> getYLocs();
  double getNodeWidth(unsigned index);
  double getNodeHeight(unsigned index);
  double getNodesArea();
  double getMinHeight();

  void putNodeWidth(unsigned index, double width);
  void putNodeHeight(unsigned index, double height);

  void changeOrient(unsigned index, ORIENT newOrient, Nets& nets);
  void updatePinsInfo(Nets& nets);
 
  void updatePlacement(int index, bool type, double xloc, double yloc);
  void updateOrient(int index, bool type, ORIENT newOrient);
  void updateHW(int index, bool type, double width, double height);

  void savePl(char* baseFileName);
  void saveNodes(char* baseFileName);

  //following functions save in Capo format
  void saveCapoNodes(char* baseFileName);
  void saveCapoPl(char* baseFileName);
  void saveCapoScl(char* baseFileName, double reqdAR);
};
}
//using namespace parquetfp;
