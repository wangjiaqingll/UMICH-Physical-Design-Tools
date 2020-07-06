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









// CHANGES:
//  970817  ilm created from dbTypes.h
//  970819  mro added #includes dbPin.h, dbCell.h, dbNet.h, dbContext.h
//  970825  ilm merged dbDesign.cxx into this file
//  970908  aec fixed bug in dbMasterCellType:PAD output function
//  970911  mro changed order of args to strstr()
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "ABKCommon/abkcommon.h"
#include "dbDefines.h"
#include "dbTypes.h"
#include "dbPin.h"
#include "dbCell.h"
#include "dbNet.h"
#include "dbContext.h"

char dbMasterCellType:: text[17] = "";
char dbCellType:: 	text[17] = "";
char dbNetType::	text[9]  = "";
char dbMasterPinType::	text[35] = "";

bool dbMasterCellType::setCoreClass(char* lgcType)
{
    if(_mClass != CORE)
	return false;

    if(strstr(lgcType,  "LOGIC_FLIPFLOP"))     { _subClass = FLIPFLOP;}
    else if(strstr(lgcType,     "LOGIC_LATCH")){ _subClass = LATCH; }
    else if(strstr(lgcType,  "LOGIC_INVERTER")){ _subClass = INVERTER;}
    else if(strstr(lgcType,    "LOGIC_BUFFER")){ _subClass = BUFFER;  }
    else if(strstr(lgcType,      "LOGIC_NAND")){ _subClass = NAND;  }
    else if(strstr(lgcType,       "LOGIC_NOR")){ _subClass = NOR;}
    else if(strstr(lgcType,"LOGIC_OTHERLOGIC")){ _subClass = CoreUNKNOWN;}
    else abkfatal(0, "Invalid string for MasterCellType")

	return true;
}

dbMasterCellType::dbMasterCellType(char* mtype, char* stype)
{
    
    if     (strlen(mtype) == 0){      _mClass = MCUNKNOWN; }
    else if(strstr(mtype,  "COVER")){ _mClass = COVER;}
    else if(strstr(mtype,   "RING")){ _mClass = RING; }
    else if(strstr(mtype,  "BLOCK")){ _mClass = BLOCK;}
    else if(strstr(mtype,    "PAD")){ _mClass = PAD;  }
    else if(strstr(mtype, "ENDCAP")){ _mClass = PAD;  }
    else if(strstr(mtype, "EXTPIN")){ _mClass = EXTPIN;}
    else if(strstr(mtype,   "CORE")){ _mClass = CORE;}
    else abkfatal(0, "Invalid string for MasterCellType");
   
    if(isCore() )
    { 
      	if     (strlen(stype) == 0){      _subClass = CoreUNKNOWN; }
      	else if(strstr(stype,  "FLIPFLOP")){ _subClass = FLIPFLOP;}
      	else if(strstr(stype,     "LATCH")){ _subClass = LATCH;   }
      	else if(strstr(stype,  "INVERTER")){ _subClass = INVERTER;}
      	else if(strstr(stype,    "BUFFER")){ _subClass = BUFFER;  }
      	else if(strstr(stype,      "NAND")){ _subClass = NAND;  }
      	else if(strstr(stype,       "NOR")){ _subClass = NOR;}
      	else abkfatal(0, "Invalid string for MasterCellType")
    } else if (isPad() )
    {
      	if (strlen(stype) == 0) 	     { _subClass = PadUNKNOWN;}
      	else if(strstr(stype,      "INPUT")) { _subClass = INPUT; }
      	else if(strstr(stype,     "OUTPUT")) { _subClass = OUTPUT;}
      	else if(strstr(stype,      "INOUT")) { _subClass = INOUT; }
      	else if(strstr(stype,      "POWER")) { _subClass = POWER; }
      	else if(strstr(stype,     "SPACER")) { _subClass = SPACER;}
      	else if(strstr(stype, "PadUNKNOWN")) { _subClass = PadUNKNOWN;}
    } else  
	_subClass = PadUNKNOWN;
     
   
}

dbMasterCellType::operator char*() const
{
    strcpy(text,"");
    switch (_mClass)
    {
        case COVER    : strcat(text, "COVER ");   break;
        case RING     : strcat(text, "RING ");    break;    
        case BLOCK    : strcat(text, "BLOCK ");   break;    
        case ENDCAP   : strcat(text, "ENDCAP ");  break;    
        case EXTPIN   : strcat(text, "EXTPIN ");  break;   
        case MCUNKNOWN: strcat(text, "UNKNOWN "); break; 
        case CORE     : strcat(text, "CORE ");
            switch(_subClass)
            {
                case FLIPFLOP    : strcat(text, "FLIPFLOP "); break;
                case LATCH       : strcat(text, "LATCH ");    break;
                case INVERTER    : strcat(text, "INVERTER "); break;
                case BUFFER      : strcat(text, "BUFFER ");   break;
                case NAND        : strcat(text, "NAND ");     break;
                case NOR         : strcat(text, "NOR ");      break;
                case CoreUNKNOWN : strcat(text, "UNKNOWN ");  break;
             }; 
             break;
	case PAD     : strcat(text, "PAD ");
	    switch(_subClass)
	    {
		case INPUT      : strcat(text, "INPUT ");   break;
                case OUTPUT     : strcat(text, "OUTPUT ");  break;
                case INOUT      : strcat(text, "INOUT ");   break;
                case POWER      : strcat(text, "POWER ");   break;
                case SPACER     : strcat(text, "SPACER ");  break;
                case PadUNKNOWN : strcat(text, "UNKNOWN "); break;
             }; 
             break; 
    };
    return text;
}   



dbCellType::operator char *() const
{
    strcpy(text,"");
    switch (_mType)
    {
        case BLOCK        : strcat(text, "BLOCK ");   break;
        case MacroOTHER   : strcat(text, "OTHER ");   break;
        case MacroUNKNOWN : strcat(text, "UNKNOWN "); break;
        case IO   : strcat(text, "IO "); 
            switch(_subType) 
            { 
                case PI       : strcat(text, "PI ");     break; 
                case PO       : strcat(text, "PO ");     break; 
                case PIO      : strcat(text, "PIO ");    break; 
                case POTHER   : strcat(text, "OTHER ");  break; 
                case PUNKNOWN : strcat(text, "UNKNOWN ");break; 
            }; break; 
            
                case CORE    : strcat(text, "CORE ");
                    switch(_subType)
                    {
                        case SC         : strcat(text, "SC ");     break;
                        case CC         : strcat(text, "CC ");     break;
                        case CoreOTHER  : strcat(text, "OTHER ");  break;
                    }; break;
    };
    return text;
}        


//_______________________________________
//  dbNetType
//_______________________________________

dbNetType::dbNetType(char* txt)
{
    
    if(!strlen(txt)){ _use = UNKNOWN;}
    else if(strstr(txt, "SIGNAL")){ _use = SIGNAL;}
    else if(strstr(txt,  "POWER")){ _use = POWER; }  
    else if(strstr(txt, "GROUND")){ _use = GROUND;}  
    else if(strstr(txt,  "CLOCK")){ _use = CLOCK; }  
    else if(strstr(txt, "TIEOFF")){ _use = TIEOFF;}  
    else if(strstr(txt, "ANALOG")){ _use = ANALOG;}  
    else {abkfatal(0, "Unrecognised netType");}
}

dbNetType::operator char*() const
{
    switch(_use)
    {
        case SIGNAL : strcpy(text, "SIGNAL ");
        case POWER  : strcpy(text, "POWER ");
        case GROUND : strcpy(text, "GROUND ");
        case CLOCK  : strcpy(text, "CLOCK ");
        case TIEOFF : strcpy(text, "TIEOFF ");
        case ANALOG : strcpy(text, "ANALOG ");
        case UNKNOWN: strcpy(text, "UNKNOWN ");
        };
    return text;
}

//______________________________________
//    dbPinType
//______________________________________

dbMasterPinType::dbMasterPinType(char const* nDir,
                                        char const* nUse,
                                        char const* nShape)
{
    
    if(!strlen(nDir)){ _dir = DirUNKNOWN;}
    else if(strstr(nDir, "INPUT")) {_dir = IN;}
    else if(strstr(nDir,"OUTPUT")) {
        if(strstr(nDir, "TRISTATE")){_dir = OUTTRISTATE;}
        else { _dir = OUT;}}
    else if(strstr(nDir,"INOUT"))  {_dir = INOUT;}
    else if(strstr(nDir,"FEEDTHRU")) {_dir = DirFEEDTHROUGH;}
    else abkfatal(0, "Invalid DIRECTION for dbMasterPinType");
    
    if(!strlen(nUse)){_use = UseUNKNOWN;}
    else if(strstr(nUse,"SIGNAL")) {_use = SIGNAL;}
    else if(strstr(nUse,"ANALOG")) {_use = ANALOG;}
    else if(strstr(nUse,"POWER"))  {_use = POWER; }
    else if(strstr(nUse,"GROUND")) {_use = GROUND;}
    else if(strstr(nUse,"CLOCK"))  {_use = CLOCK; }
    else if(strstr(nUse,"TIEOFF")) {_use = TIEOFF;}
    else abkfatal(0, "Invalid USE for dbMasterPinType");  
    
    if(!strlen(nShape)){_shape = ShapeUNKNOWN;}
    else if(strstr(nShape,"ABUTMENT")) {_shape = ShapeABUTMENT;}
    else if(strstr(nShape,"RING"))     {_shape = ShapeRING;} 
    else if(strstr(nShape,"FEEDTHRU")) {_shape = ShapeFEEDTHROUGH;} 
    else abkfatal(0, "Invalid SHAPE for dbMasterPinType");      
}

dbMasterPinType::operator char*() const
{
    
    switch(_dir)
    {
        case OUT: strcpy(text, "OUT "); break;
        case IN:  strcpy(text, "IN "); break; 
        case INOUT: strcpy(text, "INOUT "); break;
        case OUTTRISTATE: strcpy(text, "OUTPUT TRISTATE "); break;
        case DirFEEDTHROUGH: strcpy(text, "FEEDTHROUGH "); break;
        case DirUNKNOWN: strcpy(text, "DirUNKNOWN "); break;  
    };    
    
    switch(_use)
    {
        case SIGNAL: strcat(text, "SIGNAL "); break;
        case ANALOG: strcat(text, "ANALOG "); break;
        case POWER:  strcat(text, "POWER ");  break;
        case GROUND: strcat(text, "GROUND "); break;
        case CLOCK:  strcat(text, "CLOCK ");  break;
	case TIEOFF: strcat(text, "TIEOFF "); break;
        case UseUNKNOWN: strcat(text, "UseUNKNOWN "); break;
    }
    
    switch(_shape) 
    { 
        case ShapeABUTMENT:    strcat(text, "ABUTMENT "); break;
        case ShapeFEEDTHROUGH: strcat(text, "FEEDTHRU "); break;
        case ShapeRING:        strcat(text, "RING ");     break;
        case ShapeUNKNOWN:     strcat(text, "ShapeUNKNOWN ");  break; 
    }; 

    return text;
}
