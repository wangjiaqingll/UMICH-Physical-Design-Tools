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






















#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <ctype.h>
#include "hgWDims.h"
#include "Ctainers/bitBoard.h"

void HGraphWDimensions::readNodes ( const char* nodesFileName)
{
	//the base-class parser just populates the cell names, etc.
	//this allows us to do lookups by cell name when populating
	//things like height, width, etc.
    HGraphFixed::readNodes(nodesFileName);

    _nodeSymmetries = vector<Symmetry>(getNumNodes());
    _nodeWidths     = vector<double>  (getNumNodes(), 1);
    _nodeHeights    = vector<double>  (getNumNodes(), 1);

    ifstream nodes(nodesFileName);
    abkfatal2(nodes, " Could not open ",nodesFileName);

    int lineno = 1;
    nodes>>needcaseword("UCLA")>>needcaseword("nodes")
        >>needword("1.0")>>skiptoeol>>eathash(lineno);

    unsigned junkUnsigned;
    nodes>>needcaseword("NumNodes")>>needword(":")
        >>my_isnumber(lineno)>>junkUnsigned>>skiptoeol;
    lineno++;
   
    nodes>>needcaseword("NumTerminals")>>needword(":")
        >>my_isnumber(lineno)>>junkUnsigned>>skiptoeol;
    lineno++;

    char tmpStr[1024];
    nodes>>tmpStr;

    for(unsigned nodeCtr = 0; nodeCtr < _nodes.size(); nodeCtr++)
    {
        abkfatal(tmpStr, "not enough nodeNames in names file");
	HGFNode& node = getNodeByName(tmpStr);
 	unsigned nId  = node.getIndex();

	nodes>>eatblank;
	char nxtChar;

        while(!nodes.eof() && nodes.peek() != '\n' && nodes.peek() != '\r')
        {
	    nxtChar = nodes.peek();
	    if(nxtChar == 't' || nxtChar == 'T')
	    {
                nodes>>tmpStr>>eatblank;
		continue;	//base class processes this...
	    }
	    if(nxtChar == ':' || nxtChar == '/')
	    {
                char c;
//              nodes>>needword(":")>>eatblank;
		nodes>>c>>eatblank;
		tmpStr[0] = 0;	//makes strcat start from the beginning
		char symStr[10];

 		while(nodes.peek() == 'X' || 
	              nodes.peek() == 'Y' || 
                      nodes.peek() == 'R' ||
 		      nodes.peek() == 'x' || 
	              nodes.peek() == 'y' || 
                      nodes.peek() == 'r')
        	{
                    nodes>>symStr>>eatblank;
                    strcat(tmpStr,symStr);
        	}
		_nodeSymmetries[nId] = Symmetry(tmpStr);
	    }
	    else if(isdigit(nxtChar))	//node dimensions
		nodes>>_nodeWidths[nId]>>_nodeHeights[nId]>>eatblank; 
	    else
		abkfatal2(0, 
      "Error in .nodes (missing 'Terminals'?), unexpected string: ", tmpStr);
        }
        lineno++;

        if(!nodes.eof()) nodes>>tmpStr;
    }

    nodes.close();
}

void HGraphWDimensions::readNets
	(const char* netsFileName)
{
    ifstream nets(netsFileName);
    abkfatal2(nets, " Could not open ",netsFileName);
    cout << " Reading " << netsFileName << " ... " << endl;

    int lineno = 1;
    nets>>needcaseword("UCLA")>>needcaseword("nets")
        >>needcaseword("1.0")>>skiptoeol;
    nets>>eathash(lineno);

    unsigned expectedNumPins = _numPins = 0;
    char     cName[1024];

    nets>>cName;
    if(!strcmp(cName, "NumNets"))
    {
        nets>>skiptoeol;
        nets>>needcaseword("NumPins")>>needword(":")
            >>my_isnumber(lineno) >>expectedNumPins>>skiptoeol;
    }
    else if(!strcmp(cName, "NumPins"))
        nets>>needword(":")>>my_isnumber(lineno)>>expectedNumPins>>skiptoeol;


    _srcsWPins.reserve(expectedNumPins);
    _snksWPins.reserve(expectedNumPins);
    _srcSnksWPins.reserve(expectedNumPins);

    BitBoard nodesSeen(_nodes.size());

    unsigned netDegree;
    char     tmpChar;
    char tmpNetName[510];

    while(_numPins < expectedNumPins)
    {
        nets>>eathash(lineno)>>needcaseword("NetDegree")>>needword(":")
                >>my_isnumber(lineno)>>netDegree>>eatblank;
        char c=nets.peek();
        if (c!='\n' && c!='\r') nets >> tmpNetName >> skiptoeol;
        else tmpNetName[0]=0;

        HGFEdge* edge;
	if(netDegree > 1)
        {
            edge = addEdge();
            if (tmpNetName[0]!=0)
            {
                char * newName=new char[strlen(tmpNetName)+1];
                strcpy(newName, tmpNetName);
                _netNames.push_back(newName);
                _netNamesMap[newName] = edge->getIndex();
            }
            else _netNames.push_back(NULL);
        }
        else
        {
          //cerr << " Net of degree 1 found ";
					// MRG - to run golem3
          //if (strlen(tmpNetName)) cerr << " named " << tmpNetName;
          //abkfatal(0,"This parser does not allow nets of degree < 2");
        //makingEdge = false;
	  edge = NULL;
        }


	char	 pinDir;
  	Point	 offset;

        nodesSeen.clear();

        for(unsigned n = 0; n < netDegree; n++)
        {
					//MRG - to skip degree 1 nets
	    if(!edge){ nets>>skiptoeol>>eathash(lineno)>>cName>>eatblank>>skiptoeol; _numPins++; continue;}

	    nets>>eathash(lineno)>>cName>>eatblank;

            abkfatal(strcmp(cName, "NetDegree"),
                "did not find expected number of node names");
            abkfatal2(haveSuchNode(cName), "no such node: ",cName);
            HGFNode& node = getNodeByName(cName);
	    unsigned nId  = node.getIndex();

	    if(nodesSeen.isBitSet(nId))
	    {
                _numPins++;
	        nets>>skiptoeol;
	        nets>>eathash(lineno);
                lineno++;
		continue;
	    }

	    pinDir = 'B';
	    offset.x = _nodeWidths [nId]/2.0;
	    offset.y = _nodeHeights[nId]/2.0;

	    tmpChar = nets.peek();

	    while(tmpChar != '\n' && tmpChar != '\r')
	    {
		nets>>tmpChar;

                tmpChar = toupper(tmpChar);
		if(tmpChar == 'B' || tmpChar == 'I' || tmpChar == 'O')
	  	{
		    pinDir = tmpChar;
                    if(_param.makeAllSrcSnk && 
		       (tmpChar == 'O' || tmpChar == 'I'))
                         pinDir='B';
		}
		else if(tmpChar == ':' || tmpChar == '/') 
                {
                   nets>>offset.x>>offset.y;
                   offset.x+=_nodeWidths [nId]/2.0;
                   offset.y+=_nodeHeights[nId]/2.0;
                }
		else abkfatal2(0, "unknown char in nets format: ", tmpChar);

		nets>>eatblank;
	 	tmpChar = nets.peek();
	    }

            switch(pinDir)
            {
                case('B'): addSrcSnkWPin(node, *edge, offset); break;
                case('I'): addSnkWPin   (node, *edge, offset); break;
                case('O'): addSrcWPin   (node, *edge, offset); break;
                default:        //error
                  abkfatal3(0, pinDir, ": unexpected symbol in line ", lineno);
            };

	    nodesSeen.setBit(nId);
            _numPins++;
	    nets>>skiptoeol;
	    nets>>eathash(lineno);
            lineno++;
        }      
    }
    nets.close();
}

