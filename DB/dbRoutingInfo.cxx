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









// Created by Stefanus Mantik, January 7, 1999
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <fstream>
#include <string.h>
#include "dbRoutingInfo.h"
#include "ABKCommon/pathDelims.h"

void dbRoutingInfo::print(ostream& out) const
{
    out << "Vias:\n";
    unsigned i;
    for(i = 0; i < _masterVias.size(); i++)
        out << *(_masterVias[i]) << endl;
    out << "\nNets:\n";
    for(i = 0; i < _netlistRoute.size(); i++)
        out << *(_netlistRoute[i]) ;
}

void dbRoutingInfo::fillHash(const dbNetlist& netlist)
{
    for(itNetGlobal n = netlist.netsBegin(); n != netlist.netsEnd(); n++)
        _netHash[(*n)->getName()] = (*n)->getIndex();
}

void dbRoutingInfo::readLEFDEF(const dbNetlist& netlist,
                    const dbContext& layout, const char * auxFileName)
{
    Timer tm;
    abkfatal(auxFileName!=NULL,"NULL aux file name passed");
    ifstream auxFile(auxFileName);
    abkfatal(auxFile,"Can\'t open aux file to construct DBRoutInfo");

    cout << "Reading " << auxFileName << endl;
    char word1[100],word2[100];
    char tmpw1[255],tmpw2[255];
    char dir[255];
    const char *auxFileDirEnd=strrchr(auxFileName,pathDelim);
    if (auxFileDirEnd)
    {
       strncpy(dir,auxFileName,auxFileDirEnd-auxFileName+1);
       dir[auxFileDirEnd-auxFileName+1]=0;
    }
    else strcpy(dir,"");

    int lineNo = 1;
    auxFile >> eathash(lineNo) >> word1 >> noeol(lineNo) >> word2
            >> noeol(lineNo);
    abkfatal(!strcmp(word1,"LEFDEF")    || !strcmp(word1,"LEFDEFq") ||
             !strcmp(word1,"LEFDEFLGC") || !strcmp(word1,"LEFDEFqLGC"),
             "invalid aux file format");
    auxFile >> word1 >> noeol(lineNo) >> word2;
    if (word1[0]==pathDelimWindows || word1[0] == pathDelimUnix)
        strcpy(tmpw1,word1);
    else { strcpy(tmpw1,dir); strcat(tmpw1,word1); }
    if (word2[0]==pathDelimWindows || word1[0] == pathDelimUnix)
        strcpy(tmpw2,word2);
    else { strcpy(tmpw2,dir); strcat(tmpw2,word2);  }

    readLEFDEF(netlist, layout, _abk_cpd(tmpw1), _abk_cpd(tmpw2));
    tm.stop();
    cout << "DB setup for routed net took " << tm << endl;
}

void dbRoutingInfo::readLEFDEF(const dbNetlist& netlist,
            const dbContext& layout,
            const char * lefFileName, const char* defFileName)
{
    fillHash(netlist);
    abkfatal(lefFileName!=NULL,"NULL LEF file name passed");
    abkfatal(defFileName!=NULL,"NULL DEF file name passed");
    parseLEF(layout, lefFileName);
    parseDEF(netlist, layout, defFileName);
}


unsigned dbRoutingInfo::findLayer(const dbContext& layout,
                                  const char* layerName)
{
    for(itLayer ly = layout.beginLayers(); ly != layout.endLayers(); ly++)
        if (!strcasecmp(layerName,(*ly)->getName()))
            return (*ly)->getIndex();
    abkfatal(0,"Layer not found");
    return 0;
}

dbMasterVia * dbRoutingInfo::findVia(const char* viaName)
{
    for(itMasterVia itV = _masterVias.begin();
        itV != _masterVias.end(); itV++)
        if (!strcasecmp(viaName,(*itV)->getName()))
            return *itV;
    abkfatal(0,"Via not found");
    return NULL;
}

unsigned dbRoutingInfo::findNet(const char * netName)
{
    if (_netHash.find(netName) != _netHash.end())
        return _netHash[netName];
    else
        return UINT_MAX;
}

void dbRoutingInfo::parseLEF(const dbContext& layout, const char* lefFileName)
{
  char word1[200], word2[100], word3[100], name[100];
  double num1, num2, num3, num4;
  int lineNo = 1;
  cout << "Opening " << lefFileName << endl;
  ifstream lefFile(lefFileName);
  abkfatal (lefFile, "Can\'t open LEF file");
  while(!lefFile.eof())
  {
    lefFile >> eathash(lineNo) >> word1;
    if (!strcasecmp(word1,"NAMESCASESENSITIVE"))
    { lefFile >> skiptoeol; lineNo++; }
    else if(!strcasecmp(word1,"UNITS"))
    {
      lefFile >> needeol(lineNo++);
      while(!lefFile.eof() && strcasecmp(word1,"END"))
      { lefFile >> word1 >> skiptoeol; lineNo++; }
    }
    else if (!strcasecmp(word1,"LAYER") ||
             !strcasecmp(word1,"PROPERTYDEFINITIONS") ||
             !strcasecmp(word1,"VIARULE"))
    {
      lefFile >> skiptoeol; lineNo++;
      while(!lefFile.eof() && strcasecmp(word1,"END"))
      { lefFile >> eathash(lineNo) >> word1 >> skiptoeol; lineNo++; }
    }
    else if(!strcasecmp(word1,"VIA"))
    {
      bool isDefault = false;
      double resistance = -1;
      unsigned layer1, layer2, cutLayer;
      unsigned  index;
      BBox rect1, rect2, cutRect;

      lefFile >> noeol(lineNo) >> word2 >> eatblank;
      strcpy(name, word2);
      if (lefFile.peek() != '\n' && lefFile.peek() != '\r')
      {
        lefFile >> noeol(lineNo) >> word3;
        if (!strcasecmp(word3,"DEFAULT"))
            isDefault = true;
      }
      lefFile >> needeol(lineNo++);
      while(!lefFile.eof() && strcasecmp(word1,"END"))
      {
        lefFile >> eathash(lineNo) >> word1;
        if(!strcasecmp(word1,"RESISTANCE"))
        {
          lefFile >> noeol(lineNo) >> num1 >> noeol(lineNo)
                  >> word2 >> eathash(lineNo);
          resistance = num1;
        }
        else if(!strcasecmp(word1,"LAYER"))
        {
          lefFile >> noeol(lineNo) >> word2 >> noeol(lineNo) 
                  >> word3 >> eathash(lineNo);
          layer1 = findLayer(layout,word2);
          lefFile >> eathash(lineNo) >> word1 >> noeol(lineNo) >> num1 
              >> noeol(lineNo) >> num2 >> noeol(lineNo) >> num3>>noeol(lineNo)
              >> num4 >> noeol(lineNo) >> word2 >> eathash(lineNo); 
          rect1=BBox(num1,num2,num3,num4);
          lefFile >> eathash(lineNo) >> word1;
          while(strcasecmp(word1,"LAYER") && strcasecmp(word1,"END"))
          { lefFile >> skiptoeol >> eathash(lineNo) >> word1; lineNo++; }
          if(!strcasecmp(word1,"END")) continue;
          lefFile >> noeol(lineNo) >> word2 >> noeol(lineNo)
                  >> word3 >> eathash(lineNo);
          cutLayer = findLayer(layout,word2);
          lefFile >> eathash(lineNo) >> word1 >> noeol(lineNo) >> num1
              >> noeol(lineNo) >> num2 >> noeol(lineNo) >> num3>>noeol(lineNo)
              >> num4 >> noeol(lineNo) >> word2 >> eathash(lineNo);
          cutRect=BBox(num1,num2,num3,num4);
          lefFile >> eathash(lineNo) >> word1;
          while(strcasecmp(word1,"LAYER") && strcasecmp(word1,"END"))
          { lefFile >> skiptoeol >> eathash(lineNo) >> word1; lineNo++; }
          if(!strcasecmp(word1,"END")) continue;
          lefFile >> noeol(lineNo) >> word2 >> noeol(lineNo)
                  >> word3 >> eathash(lineNo);
          layer2 = findLayer(layout,word2);
          lefFile >> eathash(lineNo) >> word1 >> noeol(lineNo) >> num1
              >> noeol(lineNo) >> num2 >> noeol(lineNo) >> num3>>noeol(lineNo)
              >> num4 >> noeol(lineNo) >> word2 >> eathash(lineNo);
          rect2=BBox(num1,num2,num3,num4);
        }
        else
        { lefFile >> skiptoeol; lineNo++; }
      }
      index = _masterVias.size();
      _masterVias.push_back(new dbMasterVia(name, index, 
             **(layout.beginLayers()+layer1), rect1,
             **(layout.beginLayers()+cutLayer), cutRect, 
             **(layout.beginLayers()+layer2), rect2,
             isDefault, resistance));
    }
    else if(!strcasecmp(word1,"MACRO") || !strcasecmp(word1,"PIN"))
    {
      lefFile >> noeol(lineNo) >> word2 >> needeol(lineNo++);
      strcpy(word3,"");
      while(!lefFile.eof()&&(strcasecmp(word1,"END")||strcasecmp(word2,word3)))
      {
        lefFile >> word1 >> eatblank;
        if (!strcasecmp(word1,"END") && lefFile.peek() != '\n' 
                                     && lefFile.peek() != '\r')
          lefFile >> noeol(lineNo) >> word3 >> skiptoeol;
        else
          lefFile >> skiptoeol;
        lineNo++;
      }
    }
    else if(!strcasecmp(word1,"OBS"))
    {
      lefFile >> needeol(lineNo++);
      while(!lefFile.eof() && strcasecmp(word1,"END"))
      { lefFile >> word1 >> skiptoeol; lineNo++; }
    }
    else
    { lefFile >> skiptoeol; lineNo++; }
  }
  lefFile.close();
  cout << "LEF file " << lefFileName << " parsed\n";
}

void dbRoutingInfo::parseDEF(const dbNetlist& netlist, const dbContext& layout,
                             const char* defFileName)
{
  char word1[700], word2[100], name[100];
  unsigned numNet; //, origNumNet = netlist.getNumNets();
  double num1, num2; 
  int lineNo = 1;
  cout << "Opening " << defFileName << endl;
  ifstream defFile(defFileName);
  abkfatal (defFile, "Can\'t open DEF file");
  while(!defFile.eof())
  {
    defFile >> eathash(lineNo) >> word1;
    if(!strcasecmp(word1,"NETS"))
    {
      defFile >> noeol(lineNo) >> numNet >> noeol(lineNo) >> word2
              >> needeol(lineNo++);
      //abkguess(numNet == origNumNet, 
      //        "number of nets does not match with current DB");
      for(unsigned i = 0; i < numNet; i++)
      {
        defFile >> eathash(lineNo) >> word1 >> noeol(lineNo)
                >> word2 >> eathash(lineNo);
        abkfatal(!strcasecmp(word1,"-"),
                 "NET declaration does not start with \"-\"");
        strcpy(name, word2);
        dbRoutedNet * rNet = new dbRoutedNet(name, findNet(name));
        while(strcasecmp(word1,";"))
        {
          defFile >> eathash(lineNo) >> word1;
          if (!strcasecmp(word1,"+"))
          {
            defFile >> eathash(lineNo) >> word1;
            if(!strcasecmp(word1,"ROUTED") || !strcasecmp(word1,"FIXED") ||
               !strcasecmp(word1,"COVER"))
            {
              rNet->setType(word1);
              // dbLayer * crntLayer, * nextLayer;
              unsigned crntLayer, nextLayer;
              Point crntPos, nextPos;
              bool segmentBreak = true;
              while(strcasecmp(word1,";") && strcasecmp(word1,"+"))
              {
                if(segmentBreak)
                {
                  segmentBreak = false;
                  defFile >> eathash(lineNo) >> word1;
                  crntLayer = findLayer(layout,word1);     // get layer name
                  defFile >> eathash(lineNo) >> word1;
                  if(!strcasecmp(word1, "TAPER"))
                      defFile >> eathash(lineNo) >> word1;
                  abkfatal(!strcasecmp(word1,"("),"must start with point");
                  defFile >> eathash(lineNo) >> num1 >> noeol(lineNo)
                          >> num2 >> noeol(lineNo) >> word2;
                  crntPos = Point(num1, num2);
                }
                defFile >> eathash(lineNo) >> word1;
                if(!strcasecmp(word1,"NEW"))
                  segmentBreak = true;
                else if(!strcasecmp(word1,"("))
                {             // segment
                  defFile >> eatblank;
                  if(defFile.peek() == '*')
                  {
                    defFile >> word2 >> eatblank;
                    num1 = crntPos.x;
                  }
                  else
                    defFile >> num1 >> eatblank;
                  if(defFile.peek() == '*')
                  {
                    defFile >> word2 >> eatblank;
                    num2 = crntPos.y;
                  }
                  else
                    defFile >> num2 >> eatblank;
                  defFile >> eathash(lineNo) >> word2;
                  nextPos = Point(num1,num2);
                  dbRoutingElement tmpEl(crntPos, nextPos, crntLayer);
                  rNet->addElement(tmpEl, **(layout.beginLayers()+crntLayer));
                  crntPos = nextPos;
                }
                else if(strcasecmp(word1,";") && strcasecmp(word1,"+"))
                {             // via
                  dbMasterVia * mVia = findVia(word1);
                  unsigned firstLayer = mVia->getFirstLayer().getIndex(),
                           secondLayer = mVia->getSecondLayer().getIndex();
                  if(firstLayer == crntLayer)
                      nextLayer = secondLayer;
                  else
                      nextLayer = firstLayer;
                  rNet->addElement(
                          dbRoutingElement(crntPos, crntLayer,
                                           nextLayer, mVia->getIndex()),
                          **(layout.beginLayers()+crntLayer));
                  crntLayer = nextLayer;
                }
              }
            }
          }
        }
        _netlistRoute.push_back(rNet);
      }
    }
    else
    {
      defFile >> skiptoeol;
      lineNo++;
      defFile >> eathash(lineNo);
    }
  }
  defFile.close();
  cout << "DEF file " << defFileName << " parsed\n";
}

double dbRoutingInfo::getTotalWL() const
{
    double totalWL = 0;
    for(itRoutedNet n = _netlistRoute.begin(); n != _netlistRoute.end(); n++)
        totalWL += (*n)->getTotalLength();
    return totalWL;
}

double dbRoutingInfo::getTotalHorizWL() const
{
    double totalHWL = 0;
    for(itRoutedNet n = _netlistRoute.begin(); n != _netlistRoute.end(); n++)
        totalHWL += (*n)->getHorizLength();
    return totalHWL;
}

double dbRoutingInfo::getTotalVertWL() const
{
    double totalVWL = 0;
    for(itRoutedNet n = _netlistRoute.begin(); n != _netlistRoute.end(); n++)
        totalVWL += (*n)->getVertLength();
    return totalVWL;
}
