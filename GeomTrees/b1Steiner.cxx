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






// Most of the codes are taken from University of Virginia with the
// permission from Professor Gabriel Robins.   The original code was
// written in C and was translated to C++ style by Stefanus Mantik (UCLA).
// The original Copyright note is included below

/**********************************************************************/
/*                                                                    */
/*            (c) Copyright, 1997 by Professor Gabriel Robins         */
/*                                                                    */
/*      Department of Computer Science, University of Virginia        */
/*          Charlottesville, VA 22903-2442  (804) 982-2207            */
/*   robins@cs.virginia.edu    http://www.cs.virginia.edu/~robins/    */
/*                                                                    */
/*   This code may be freely used for all non-commercial purposes.    */
/*   All copies/portions of this code must contain this header.       */
/*                                                                    */
/**********************************************************************/
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include "b1Steiner.h"

BatchedI1Steiner::BatchedI1Steiner(const vector<Point>& points):
    _points(points), _nPoints(points.size()), _idNum(0), _mstCost(0),
    _steinerCost(0) //, _treeCtr(points.size()+2)  //, _tree(_treeCtr[1])
{
    //cout << _tree[-1].id << " " << _tree[-1].parent << endl;
    //_tree = _treeCtr.begin() + 1;
    _tree[-1] = TreeNode();
    _points.push_back(Point());    // add extra dummy space
    generateSteiner();
    extractEdges();
}

/**********************************************************************
**
**      This function finds up to four closest neighbours for a node
**      in each quadrant and put the closest one as the first neighbour
**********************************************************************/

void BatchedI1Steiner::findNeighbour(int index,
                                     vector<Neighbour>& neighbour)
{
    int i;
    for(i = 0; i < 5; i++)
    {
        neighbour[i].id = -1;
        neighbour[i].weight = DBL_MAX;
    }
    for(i = 0; i < index; i++)
    {
        int quadrant;
        double dx, dy, dist;

        dy= _points[i].y - _points[index].y;
        dx= _points[i].x - _points[index].x;
        dist = abs(dy) + abs(dx);

        if(dx > dy)
            if(dx + dy < 0)  quadrant = 3;
            else             quadrant = 0;
        else 
            if(dx + dy < 0)  quadrant = 2;
            else             quadrant = 1;

        if (dist < neighbour[quadrant].weight)
        {
            neighbour[quadrant].id = i;
            neighbour[quadrant].weight = dist;
        }
    }

    // the following looks for the closest neighbour
    int smallest_edge = 0;
    for (i = 0; i < 5; i++)
        if (neighbour[i].weight < neighbour[smallest_edge].weight)
            smallest_edge = i;
    if (smallest_edge != 0)
    {
        Neighbour tmp;
        tmp = neighbour[0];
        neighbour[0] = neighbour[smallest_edge];
        neighbour[smallest_edge] = tmp;
    }
}

/**********************************************************************
**
**      This function updates the MST by connecting the new node with
**      its up to four neighbours in the tree and deleting the longest
**      edge along the induced cycle
**********************************************************************/
double BatchedI1Steiner::linearMST(int newPoint,
                                   const vector<Neighbour>& neighbour)
{
    double cost = 0, savings = 0;

    _tree[newPoint].parent = neighbour[0].id;
    if (newPoint)          /* if not first point */
        _tree[newPoint].weight = cost = neighbour[0].weight;
    for (int i = 1; i < 4 ; i++)
    {
        if (neighbour[i].id != -1)
        {
            int parent1, parent2, index1, index2, po1, po2, PATH = 0;
            double   weight1, weight2, dif = 0;

            abkfatal(_idNum != INT_MAX, "idNum reach max");
            po2 = index2 = neighbour[i].id;
            _tree[index2].id = ++_idNum;
            po1 = index1 = newPoint;
            _tree[index1].id = _idNum;
            parent1 = _tree[index1].parent;
            parent2 = _tree[index2].parent;

            while (_tree[parent1].id != _idNum
                    && _tree[parent2].id != _idNum
                    && parent1 != parent2)
            {
                moveUp(index1, po1, _idNum);
                moveUp(index2, po2, _idNum);

                parent1 = _tree[index1].parent;
                parent2 = _tree[index2].parent;
            }

            if (_tree[parent1].id == _idNum)
            {
                if (parent1 == neighbour[i].id) PATH = 1;
                else po2 = resetPO(neighbour[i].id, parent1);
            }
            if (_tree[parent2].id == _idNum)
            {
                if (parent2 == newPoint) PATH = 2;
                else po1 = resetPO(newPoint, parent2);
            }

            weight1 = _tree[po1].weight;
            weight2 = _tree[po2].weight;
            if (!PATH) PATH = (weight1 > weight2) ? 1 : 2;
            switch (PATH)
            {
              case 1:
                if (weight1 > neighbour[i].weight)
                {
                    dif = weight1 - neighbour[i].weight;
                    updateTree(newPoint, po1);
                    _tree[newPoint].parent = neighbour[i].id;
                    _tree[newPoint].weight = neighbour[i].weight;
                }
                break;
              case 2:
                if (weight2 > neighbour[i].weight)
                {
                    dif = weight2 - neighbour[i].weight;
                    updateTree(neighbour[i].id, po2);
                    _tree[neighbour[i].id].parent = newPoint;
                    _tree[neighbour[i].id].weight = neighbour[i].weight;
                }
                break;
            }
            savings += dif;
        }
    }
    return (savings - cost);
}

/**********************************************************************/
/*      This function generates MST by adding the new nodes one at a time
**      calling linear_MST
**
***********************************************************************/
double BatchedI1Steiner::generateMST()
{
    vector<Neighbour> neighbour(5);
    double mstCost = 0;
    _tree[0] = TreeNode();
    for(unsigned i = 0; i < _points.size() - 1; i++)
    {
        findNeighbour(i, neighbour);
        mstCost -= linearMST(i, neighbour);
    }
    return mstCost;
}

/***********************************************************************
**
**      This function updates the tree by deleting the longest edge along
**      the cycle generated by adding the new point
**      called by  linearMST
***********************************************************************/

void BatchedI1Steiner::updateTree(int first, int last)
{
    int prev, curr, next;
    TreeNode preNode,curNode;
 
    if (first== last) return;

    if (_tree[first].parent == last)
    {
        abkfatal(last != INT_MAX, "index out of range for last");
        _tree[last]= _tree[first];
        _tree[last].parent = first;
        return;
    }
    else
    {
        prev = first;
        curr= _tree[prev].parent;
        abkfatal(curr != INT_MAX, "index out of range for curr");
        next = _tree[curr].parent;
 
        preNode = _tree[prev];
        curNode = _tree[curr];
  
        while (prev != last)
        {
            _tree[curr]= preNode;
            _tree[curr].parent = prev;
            preNode = curNode;
            //abkfatal(next != UINT_MAX, "index out of range for next");
            //if(next == UINT_MAX) curNode = TreeNode();
            //else curNode = _tree[next];
            curNode = _tree[next];
            prev= curr;
            curr= next;
            //if(next == UINT_MAX) break;
            next= _tree[next].parent;
        }
    }
}

/**********************************************************************
**
**      traverse the tree from a bottom node to a up level one
**********************************************************************/
void BatchedI1Steiner::moveUp(int& index, int& PO, int id)
{
    if (_tree[index].parent != -1)
    {
        index = _tree[index].parent;
        _tree[index].id = id;
        if (_tree[index].weight > _tree[PO].weight)
            PO = index;
    }
}

/**********************************************************************
**
**      trace back to the starting point
***********************************************************************/
int BatchedI1Steiner::resetPO(int first, int last)
{
    int tmp = first;
    for(int i = first; i != last; i = _tree[i].parent)
    {
        //abkfatal(i != UINT_MAX || i == last, "Index out of range");
        //if(i == UINT_MAX) break;
        if (_tree[i].weight > _tree[tmp].weight)
            tmp = i;
    }
    return tmp;
}


/***********************************************************************
**      this function compute the saving  induced by the steiner points
**      using linearMST updating scheme
***********************************************************************/
void BatchedI1Steiner::generateSteiner()
{
    int lastPoint;
    double savings;
    vector<Neighbour> neighbour(5);

    _steinerCost = _mstCost = generateMST();

    map<int, TreeNode, std::less<int> > oldTree(_tree);
    //TreeNode *oldTree = oldTreeCtr.begin() + 1;
    lastPoint = _points.size() - 1;

    while(true)
    {
        vector<SPCandidate>  candidates;
        int SPs_this_round = 0;
        unsigned i,j;

        for(i = 0; i< _nPoints; i++)
          for(j = 0; j< _nPoints; j++)
            if(i != j)
            {
                _points[lastPoint].x = _points[i].x;
                _points[lastPoint].y = _points[j].y;
                findNeighbour(lastPoint, neighbour);
                if (neighbour[0].weight != 0)
                {
                    savings = linearMST (lastPoint, neighbour);
                    if(savings > 0)
                        candidates.push_back(SPCandidate(_points[lastPoint],
                                                         savings));
                    for(unsigned k = 0; k < _points.size(); k++)
                        _tree[k] = oldTree[k];
                }
            }            // check all the Hanan points and
                         // set up the array for potential SPs

        if(candidates.size() == 0) break;

// #ifdef __SUNPRO_CC
//   using std::sort;
//   using std::unique;
// #endif

        std::sort(candidates.begin(), candidates.end()); 
        std::vector<SPCandidate>::iterator newCandidateEnd;
        newCandidateEnd = std::unique(candidates.begin(), candidates.end());

        for(std::vector<SPCandidate>::iterator c = candidates.begin();
            c != newCandidateEnd; c++)
        {
            _points[lastPoint] = c->point; //candidates[i].point;
 
            findNeighbour(lastPoint, neighbour);
            if (neighbour[0].weight != 0)
            {
                savings = linearMST(lastPoint, neighbour);
                if(savings >= c->saving) //candidates[i].saving)
                {
                    _steinerCost -= savings;
                    _points.push_back(Point());
                    lastPoint = _points.size() - 1;
                    //_treeCtr.push_back(TreeNode());
                    //oldTreeCtr.push_back(TreeNode());
                    SPs_this_round++;
                    for(unsigned k = 0; k < _points.size(); k++)
                        oldTree[k] = _tree[k];
                }
                else
                    for(unsigned k =0; k < _points.size(); k++)
                        _tree[k] = oldTree[k];
            }
        }      // add all the non-interferred SPs for this round 

        // removing invalid steiner points of degree one and two
        //
        unsigned firstNewSP = _points.size() - 1 - SPs_this_round;
        vector<unsigned> degree(_points.size() - 1, 1);

        for(i = 1; i < _points.size() - 1; i++)
            degree[_tree[i].parent]++;

        for(i = _points.size() - 2;i >= _nPoints; i--)
        if (degree[i] <= 2)
        {
            if(i >= firstNewSP) SPs_this_round--;
            for(unsigned j = i; j < _points.size() - 2; j++)
                _points[j] = _points[j+1];
            _points.pop_back();
            //oldTreeCtr.pop_back();
            //_treeCtr.pop_back();
        }

        _steinerCost = generateMST();
        for(i = 0; i < _points.size() - 1; i++) oldTree[i] = _tree[i];
        if(SPs_this_round == 0) break;
    }  
}


// This is for converting tree into edges (GeomTreeEdge)
void BatchedI1Steiner::extractEdges()
{
    if(!_steinerEdges.empty()) _steinerEdges.clear();
    for(unsigned i = 0; i < _points.size() - 1; i++)
        if(_tree[i].parent != -1)
            _steinerEdges.push_back(
                    GeomTreeEdge(_points[i], _points[_tree[i].parent]));
}

void BatchedI1Steiner::printTree()
{
    cout << "Print Tree:\n";
    for(unsigned x = 0; x < _tree.size() - 1; x++)
    {
        cout << "  " << _tree[x].id << " " << _tree[x].parent << " ";
        if(_tree[x].weight == DBL_MAX) cout << "-1\n" << flush;
        else cout << _tree[x].weight << endl << flush;
    }
}
