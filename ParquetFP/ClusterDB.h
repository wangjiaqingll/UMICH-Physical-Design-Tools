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
#include <map>

using namespace std;

namespace parquetfp
{
class DB;
class Command_Line;

class ClusterDB
{
 private:

  Command_Line *_params; //parameters
  DB *_db;        //just a pointer to base DB
  DB *_newDB;     //used in cluster
  DB *_oldDB;     //used in cluster

 public:

  ClusterDB(DB* db, Command_Line *params);
  ~ClusterDB();

  void clusterMulti(DB * newDB);

  void cluster1Layer(unsigned layerNum);
  void addNetsToNewDB(Nets* nets, Nets* newNets, Nodes* nodes, Nodes* newNodes,
		      map<unsigned, unsigned>& mapping);

  Node& getClosestNode(Node& currNode, Nodes* nodes, Nets* nets, 
		       vector<bool>& seenNodes);
  void addWSPerNode(void);

  void clusterMultiPhysical(DB * newDB); //cluster with physical constraints
};

struct sortNumConnections //sort connectivities
{
  bool operator()(IntPoint pt1, IntPoint pt2)
  {
    return (pt1.y < pt2.y);
  }
};
}
//using namespace parquetfp;
