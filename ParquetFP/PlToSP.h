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










#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdlib.h>

using namespace std;

namespace parquetfp
{
enum PL2SP_ALGO{NAIVE_ALGO, TCG_ALGO};

class Pl2SP
{
  private:
    vector<double> _xloc;
    vector<double> _yloc;
    vector<double> _widths;
    vector<double> _heights;

    vector<unsigned> _XX;
    vector<unsigned> _YY;

    int _cnt;
  public:
    Pl2SP(vector<double>& xloc, vector<double>& yloc, vector<double>& widths,
          vector<double>& heights, PL2SP_ALGO whichAlgo);

    ~Pl2SP() {}
    
    void naiveAlgo(void);
    void TCGAlgo(void);

    //Floyd Marshal to find TCG
    void TCG_FM(vector< vector <bool> >& TCGMatrixHoriz, 
		vector< vector <bool> >& TCGMatrixVert);

    //DP to find TCG
    void TCG_DP(vector< vector <bool> >& TCGMatrixHoriz, 
		vector< vector <bool> >& TCGMatrixVert);
    void TCGDfs(vector< vector <bool> >& TCGMatrix, 
		vector< vector <bool> >& adjMatrix, int v, 
		vector<int>& pre);

    vector<unsigned>& getXSP(void)
     { return _XX; }
     
    vector<unsigned>& getYSP(void)
     { return _YY; }

    void print(void);

};

struct RowElem
{
  unsigned index;
  double xloc;
};


struct less_mag 
{
  bool operator()(const RowElem &elem1, const RowElem &elem2) 
  { return(elem1.xloc < elem2.xloc); }
};

class SPXRelation
{
  vector< vector<bool> >& TCGMatrixHoriz;
  vector< vector<bool> >& TCGMatrixVert;

  public:
  SPXRelation(vector< vector<bool> >& TCGMatrixHorizIP, 
	      vector< vector<bool> >& TCGMatrixVertIP) : 
    TCGMatrixHoriz(TCGMatrixHorizIP), TCGMatrixVert(TCGMatrixVertIP)
    {}

  bool operator()(unsigned i, unsigned j) 
    {
      if(TCGMatrixHoriz[i][j] == 1)
	return 1;
      else if(TCGMatrixHoriz[j][i] == 1)
	return 0;
      else if(TCGMatrixVert[j][i] == 1)
	return 1;
      else if(TCGMatrixVert[i][j] == 1)
	return 0;
      else
	{
	  //cout<<"ERROR IN PL2SP SPX "<<i<<"\t"<<j<<endl;
	  if(i<j)
	    return 1;
	  else
	    return 0;
	}
    }
};

class SPYRelation
{
  vector< vector<bool> >& TCGMatrixHoriz;
  vector< vector<bool> >& TCGMatrixVert;

  public:
  SPYRelation(vector< vector<bool> >& TCGMatrixHorizIP, 
	      vector< vector<bool> >& TCGMatrixVertIP) : 
    TCGMatrixHoriz(TCGMatrixHorizIP), TCGMatrixVert(TCGMatrixVertIP)
    {}
  bool operator()(unsigned i, unsigned j) 
    {
      if(TCGMatrixHoriz[i][j] == 1)
	return 1;
      else if(TCGMatrixHoriz[j][i] == 1)
	return 0;
      else if(TCGMatrixVert[j][i] == 1)
	return 0;
      else if(TCGMatrixVert[i][j] == 1)
	return 1;
      else
	{
	  //cout<<"ERROR IN PL2SP SPY "<<i<<"\t"<<j<<endl;
	  if(i<j)
	    return 1 ;
	  else
	    return 0;
	}
    }
};
}
//using namespace parquetfp;
