/*
 *     This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 *  Distribution,  Product Version 5.3, and is subject to the Cadence
 *  LEF/DEF Open Source License Agreement.   Your  continued  use  of
 *  this file constitutes your acceptance of the terms of the LEF/DEF
 *  Open Source License and an agreement to abide by its  terms.   If
 *  you  don't  agree  with  this, you must remove this and any other
 *  files which are part of the distribution and  destroy any  copies
 *  made.
 *
 *     For updates, support, or to become part of the LEF/DEF Community,
 *  check www.openeda.org for details.
 */
 
// This file contains code for implementing the lefwriter 5.3
// It has all the functions user can call in their callbacks or
// just their writer to write out the correct lef syntax.
//
// Author: Wanda da Rosa
// Date:   Summer, 1998
//
// Revisions:

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "lefwWriter.hpp"
#include "crypt.hpp"

// States of the writer.
#define LEFW_UNINIT                   0
#define LEFW_INIT                     1
#define LEFW_ARRAY_START              2
#define LEFW_BEGINEXT_START           3
#define LEFW_CORRECTTABLE_START       4
#define LEFW_IRDROP_START             5
#define LEFW_LAYER_START              6
#define LEFW_LAYERROUTING_START       7
#define LEFW_MACRO_START              8
#define LEFW_NOISETABLE_START         9
#define LEFW_NONDEFAULTRULE_START    10
#define LEFW_PROPERTYDEF_START       11
#define LEFW_SPACING_START           12
#define LEFW_UNITS_START             13
#define LEFW_VIA_START               14
#define LEFW_VIARULE_START           15
#define LEFW_VIARULEGEN_START        16

#define LEFW_ANTENNASIZE             17
#define LEFW_ARRAY                   18
#define LEFW_BEGINEXT                19
#define LEFW_BUSBITCHARS             20
#define LEFW_CASESENSITIVE           21
#define LEFW_CORRECTTABLE            22
#define LEFW_DIELECTRIC              23
#define LEFW_DIVIDERCHAR             24
#define LEFW_IRDROP                  25
#define LEFW_LAYER                   26
#define LEFW_LAYERACCURRENT          27
#define LEFW_LAYERDCCURRENT          28
#define LEFW_LAYERROUTING            29
#define LEFW_MACRO                   30
#define LEFW_MINFEATURE              31
#define LEFW_NONDEFAULTRULE          32
#define LEFW_NOISEMARGIN             33
#define LEFW_NOISETABLE              34
#define LEFW_NOWIREEXTATPIN          35
#define LEFW_PROPERTYDEF             36
#define LEFW_SCALEFACTOR             37
#define LEFW_SITE                    38
#define LEFW_SPACING                 39
#define LEFW_THRESHOLD1              40
#define LEFW_THRESHOLD2              41
#define LEFW_UNITS                   42
#define LEFW_VERSION                 43
#define LEFW_VIA                     44
#define LEFW_VIARULE                 45
#define LEFW_VIARULEGEN              46

#define LEFW_END                     47
#define LEFW_ARRAY_END               48
#define LEFW_BEGINEXT_END            49
#define LEFW_CORRECTTABLE_END        50
#define LEFW_IRDROP_END              51
#define LEFW_LAYER_END               52
#define LEFW_LAYERROUTING_END        53
#define LEFW_MACRO_END               54
#define LEFW_NOISETABLE_END          55
#define LEFW_NONDEFAULTRULE_END      56
#define LEFW_PROPERTYDEF_END         57
#define LEFW_SPACING_END             58
#define LEFW_VIA_END                 59
#define LEFW_VIARULE_END             60
#define LEFW_VIARULEGEN_END          61
#define LEFW_UNITS_END               62

#define LEFW_DONE                   999

#define MAXSYN                       63

////////////////////////////////////////////
////////////////////////////////////////////
//
//        Global Variables
//
////////////////////////////////////////////
////////////////////////////////////////////
FILE* lefwFile = 0;   // File to write to.
int lefwSynArray[MAXSYN] ; // array of syntax
int lefwLines = 0;    // number of lines written
int lefwState = LEFW_UNINIT;  // Current state of writer
int lefwDidInit = 0; // required section
int lefwDidLayer = 0; // required section
int lefwDidVia = 0; // required section
int lefwDidViaRule = 0; // required section
int lefwDidViaSite = 0; // required section
int lefwDidViaMacro = 0; // required section
int lefwCounter = 0;  // number of nets, components in section
int lefwLineItemCounter = 0; // number of items on current line
int lefwHasArrayReq = 0; // array required data
int lefwIsArrayFloorp = 0; // array floorplan flag
int lefwIsArrayDef = 0; // array default cap flag
int lefwIsCorrectTable = 0; // correctiontable flag
int lefwIsCut = 0; // cut layer
int lefwIsEdgerate = 0; // edgerate within noisetable
int lefwIsFloorp = 0; // floorplan within array
int lefwIsMacroObs = 0; // macro obs flag
int lefwIsMacroObsLayer = 0; // macro obs layer within macro obs
int lefwIsMacroPin = 0; // macro pin flag
int lefwIsMacroPinPortLayer = 0; // macro pin port layer within macro pin port
int lefwIsMacroPinPort = 0; // macro pin port within macro pin
int lefwIsMacroTiming = 0; // macro timing flag
int lefwIsMacroTimingModel = 0; // macro timing model flag
int lefwIsNoiseTable = 0; // noisetable flag
int lefwIsNonDefaultRule = 0; // nondefaultrule flag
int lefwIsOutResist = 0; // outputResistance within edgerate
int lefwIsRouting = 0; // routing
int lefwIsRoutingReqData = 0; // layer routing required data are provided
int lefwNumViaRuleLayers = 0; // number of via rule in a via rule
int lefwOldState = 0;  // the previous state
int lefwTableLen = 0; // width or cutarea for the tableEntries
int lefwHasInit = 0;    // for lefwInit has called
int lefwHasInitCbk = 0; // for lefwInitCbk has called
static int lefwWriteEncrypt = 0; // for writing out encrypted file, default is 0
static int prtSemiColon = 0;  // sometimes ; is not printed yet
static int lefwHasLayerRoutingRange = 0;  // mark if layer routing has range

char lefwStateStr[MAXSYN] [80] = {"UNITS",               //  0
                                  "INIT",                //  1
                                  "ARRAY",               //  2
                                  "BEGINEXT",            //  3
                                  "CORRECTTABLE",        //  4
                                  "IRDROP",              //  5
                                  "LAYER",               //  6
                                  "LAYERROUTING",        //  7
                                  "MACRO",               //  8
                                  "NOISETABLE",          //  9
                                  "NONDEFAULTRULE",      // 10
                                  "PROPERTYDEFINITION",  // 11
                                  "SPACING",             // 12
                                  "UNITS",               // 13
                                  "VIA",                 // 14
                                  "VIARULE",             // 15
                                  "VIARULEGEN",          // 16
                                  "ANTENNASIZE",         // 17
                                  "ARRAY",               // 18
                                  "BEGINEXT",            // 19
                                  "BUSBITCHARS",         // 20
                                  "CASESENSITIVE",       // 21
                                  "CORRECTABLE",         // 22
                                  "DIELECTRIC",          // 23
                                  "DIVIDERCHAR",         // 24
                                  "IRDROP",              // 25
                                  "LAYER",               // 26
                                  "LAYERACCURRENT",      // 27
                                  "LAYERDCCURRENT",      // 28
                                  "LAYERROUTING",        // 29
                                  "MACRO",               // 30
                                  "MINFEATURE",          // 31
                                  "NONDEFAULTRULE",      // 32
                                  "NOISEMARGIN",         // 33
                                  "NOISETABLE",          // 34
                                  "NOWIREEXTSIONATPIN",  // 35
                                  "PROPERTYDEFINITION",  // 36
                                  "SCALEFACTORE",        // 37
                                  "SITE",                // 38
                                  "SPACING",             // 39
                                  "THRESHOLD1",          // 40
                                  "THRESHOLD2",          // 41
                                  "UNTIS",               // 42
                                  "VERSION",             // 43
                                  "VIA",                 // 44
                                  "VIARULE",             // 45
                                  "VIARULEGEN",          // 46
                                  "END",                 // 47
                                  "ARRAY",               // 48
                                  "BEGINEXT",            // 49
                                  "CORRECTABLE",         // 50
                                  "IRDROP",              // 51
                                  "LAYER",               // 52
                                  "LAYERROUTING",        // 53
                                  "MACRO",               // 54
                                  "NOISETABLE",          // 55
                                  "NONDEFAULTRULE",      // 56
                                  "PROERTYDEFINITION",   // 57
                                  "SPACING",             // 58
                                  "VIA",                 // 59
                                  "VIARULE",             // 60
                                  "VIARULEGEN",          // 61
                                  "UNITS",               // 62
                                 };

// internal function
char* lefwAddr(const char* x) {
  return (char*)x;
}


// internal function
char* lefwOrient(int num) {
  switch (num) {
  case 0: return lefwAddr("N");
  case 1: return lefwAddr("W");
  case 2: return lefwAddr("S");
  case 3: return lefwAddr("E");
  case 4: return lefwAddr("FN");
  case 5: return lefwAddr("FE");
  case 6: return lefwAddr("FS");
  case 7: return lefwAddr("FW");
  };
  return lefwAddr("BOGUS");
}


// this function is required to be called first to initialize the array
// Either this function or lefwInitCbk can be called, cannot be both
int lefwInit(FILE* f) {
  int i;

  /* Check if the user is valid */
  /* don't need anymore
  if (!lefiValidUser())
      exit(77);
  */

  // if (lefwFile) return LEFW_BAD_ORDER;
  lefwFile = f;

  if (lefwHasInitCbk == 1) {  // lefwInitCbk has already called, issue an error
     fprintf(lefwFile,
     "ERROR: lefwInitCbk has already called, cannot call lefwInit again.\n");
     fprintf(lefwFile, "Writer Exit.\n");
     exit(LEFW_BAD_ORDER);
  }
  lefwState = LEFW_UNINIT;  // Current state of writer
  lefwDidInit = 0;  // required section

  for (i = 0; i < MAXSYN; i++)
     lefwSynArray[i] = 0;

  lefwDidInit = 1;
  lefwState = LEFW_INIT;
  lefwHasInit = 1;
  return LEFW_OK;
}


// this function is required to be called first to initialize the array
// Either this function or lefwInit can be called, cannot be both
int lefwInitCbk(FILE* f) {
  int i;

  // if (lefwFile) return LEFW_BAD_ORDER;
  lefwFile = f;

  if (lefwHasInit == 1) {  // lefwInit has already called, issue an error
     if (lefwWriteEncrypt) {
        encPrint(lefwFile, (char*)
        "ERROR: lefwInit has already called, cannot call lefwInitCbk again.\n");
        encPrint(lefwFile, (char*)"Writer Exit.\n");
     } else {
        fprintf(lefwFile,
        "ERROR: lefwInit has already called, cannot call lefwInitCbk again.\n");
        fprintf(lefwFile, "Writer Exit.\n");
     }
    exit(LEFW_BAD_ORDER);
  }
  lefwState = LEFW_UNINIT;  // Current state of writer
  lefwDidInit = 0;  // required section

  for (i = 0; i < MAXSYN; i++)
    lefwSynArray[i] = 0;

  lefwDidInit = 1;
  lefwState = LEFW_INIT;
  lefwHasInitCbk = 1;
  return LEFW_OK;
}

// This function sets the global variable lefwEncrypt to 1, which will
// write out an output encrypted file, make sure to call lefwCloseEncrypt
// before closing the output file
int lefwEncrypt() {
  if (!lefwFile || (lefwFile == stdout)) {
    // No output filename is given
    fprintf(stderr, "Need an output file if writing in encrypted format.\n");
    return LEFW_BAD_ORDER;
  }

  // Set the flag in crypt.cpp
  (void)encWritingEncrypted();

  lefwWriteEncrypt = 1; 
  return LEFW_OK;
}

// This function needs to be called if lefwEncrypt is called
// It will clear the encrypted buffer
int lefwCloseEncrypt() {

  if (lefwWriteEncrypt)
     encClearBuf(lefwFile);
  return LEFW_OK;
}

int lefwNewLine() {
  if (! lefwFile) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"\n");
  else
     fprintf(lefwFile, "\n");
  return LEFW_OK;
}

int lefwVersion (int vers1, int vers2) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_VERSION]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"VERSION %d.%d ;\n", vers1, vers2);
  else
     fprintf(lefwFile, "VERSION %d.%d ;\n", vers1, vers2);
  lefwSynArray[LEFW_VERSION] = 1;
  lefwLines++;
  return LEFW_OK;
}
 
 
int lefwCaseSensitive (const char* caseSensitive) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_CASESENSITIVE]) return LEFW_ALREADY_DEFINED;
  if (strcmp(caseSensitive, "ON") && strcmp(caseSensitive, "OFF"))
        return LEFW_BAD_DATA;     // has to be either ON or OFF
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"NAMESCASESENSITIVE %s ;\n", caseSensitive);
  else
     fprintf(lefwFile, "NAMESCASESENSITIVE %s ;\n", caseSensitive);
  lefwSynArray[LEFW_CASESENSITIVE] = 1;
  lefwLines++;
  return LEFW_OK;
}
 
 
int lefwNoWireExtensionAtPin (const char* noWireExt) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_NOWIREEXTATPIN]) return LEFW_ALREADY_DEFINED;
     if (strcmp(noWireExt, "ON") && strcmp(noWireExt, "OFF"))
        return LEFW_BAD_DATA;     // has to be either ON or OFF
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"NOWIREEXTENSIONATPIN %s ;\n", noWireExt);
  else
     fprintf(lefwFile, "NOWIREEXTENSIONATPIN %s ;\n", noWireExt);
  lefwSynArray[LEFW_NOWIREEXTATPIN] = 1;
  lefwLines++;
  return LEFW_OK;
}
 
 
int lefwMinfeature (double minFeatureX, double minFeatureY) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER; 
  if (lefwSynArray[LEFW_MINFEATURE]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"MINFEATURE %g %g ;\n", minFeatureX, minFeatureY);
  else
     fprintf(lefwFile, "MINFEATURE %g %g ;\n", minFeatureX, minFeatureY);
  lefwSynArray[LEFW_MINFEATURE] = 1;
  lefwLines++;
  return LEFW_OK; 
}


int lefwDielectric (double dielectric) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_DIELECTRIC]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"DIELECTRIC %g ;\n", dielectric);
  else
     fprintf(lefwFile, "DIELECTRIC %g ;\n", dielectric);
  lefwSynArray[LEFW_DIELECTRIC] = 1;
  lefwLines++;
  return LEFW_OK; 
}


int lefwBusBitChars (const char* busBitChars) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_BUSBITCHARS]) return LEFW_ALREADY_DEFINED;
  if (busBitChars && busBitChars != 0 && *busBitChars != 0) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"BUSBITCHARS \"%s\" ;\n", busBitChars);
     else
        fprintf(lefwFile, "BUSBITCHARS \"%s\" ;\n", busBitChars);
  }
  lefwLines++;
  lefwSynArray[LEFW_BUSBITCHARS] = 1;
  return LEFW_OK; 
}


int lefwDividerChar (const char* dividerChar) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_DIVIDERCHAR]) return LEFW_ALREADY_DEFINED;
  if (dividerChar && dividerChar != 0 && *dividerChar != 0) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"DIVIDERCHAR \"%s\" ;\n", dividerChar);
     else
        fprintf(lefwFile, "DIVIDERCHAR \"%s\" ;\n", dividerChar);
  }
  lefwLines++;
  lefwSynArray[LEFW_DIVIDERCHAR] = 1;
  return LEFW_OK; 
}


int lefwStartUnits() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwSynArray[LEFW_UNITS_START]) return LEFW_ALREADY_DEFINED;

  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"UNITS\n");
  else
     fprintf(lefwFile, "UNITS\n");
  lefwLines++;
  lefwSynArray[LEFW_UNITS_START] = 1;
  lefwState = LEFW_UNITS_START;
  return LEFW_OK;
}


int lefwUnits(double time,        /* optional */
              double capacitance, /* optional */
              double resistance,  /* optional */
              double power,       /* optional */
              double current,     /* optional */
              double voltage,     /* optional */
              double database     /* optional */
              ) {

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_UNITS_START &&
      lefwState != LEFW_UNITS) return LEFW_BAD_ORDER;

  if (time) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"   TIME NANOSECONDS %g ;\n", time);
     else
        fprintf(lefwFile, "   TIME NANOSECONDS %g ;\n", time);
  }
  if (capacitance) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"   CAPACITANCE PICOFARADS %g ;\n", capacitance);
     else
        fprintf(lefwFile, "   CAPACITANCE PICOFARADS %g ;\n", capacitance);
  }
  if (resistance) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"   RESISTANCE OHMS %g ;\n", resistance);
     else
        fprintf(lefwFile, "   RESISTANCE OHMS %g ;\n", resistance);
  }
  if (power) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"   POWER MILLIWATTS %g ;\n", power);
     else
        fprintf(lefwFile, "   POWER MILLIWATTS %g ;\n", power);
  }
  if (current) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"   CURRENT MILLIAMPS %g ;\n", current);
     else
        fprintf(lefwFile, "   CURRENT MILLIAMPS %g ;\n", current);
  }
  if (voltage) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"   VOLTAGE VOLTS %g ;\n", voltage);
     else
        fprintf(lefwFile, "   VOLTAGE VOLTS %g ;\n", voltage);
  }
  if (database) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"   DATABASE MICRONS %g ;\n", database);
     else
        fprintf(lefwFile, "   DATABASE MICRONS %g ;\n", database);
  }

  lefwState = LEFW_UNITS;
  return LEFW_OK;
}


int lefwUnitsFrequency(double frequency) {  /* required since only by itself */

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_UNITS_START &&
      lefwState != LEFW_UNITS) return LEFW_BAD_ORDER;
  if (!frequency)
     return LEFW_BAD_DATA;

  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   FREQUENCY MEGAHERTZ %g ;\n", frequency);
  else
     fprintf(lefwFile, "   FREQUENCY MEGAHERTZ %g ;\n", frequency);

  lefwState = LEFW_UNITS;
  return LEFW_OK;
}


int lefwEndUnits() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_UNITS_START &&
      lefwState != LEFW_UNITS) return LEFW_BAD_ORDER;

  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END UNITS\n\n");
  else
     fprintf(lefwFile, "END UNITS\n\n");
  lefwLines++;
  lefwState = LEFW_UNITS_END;
  return LEFW_OK;
}


int lefwStartLayer(const char* layerName, const char* type) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_LAYER_START ||
      lefwState == LEFW_LAYER) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (strcmp(type, "CUT") == 0)
     lefwIsCut = 1;
  else if (strcmp(type, "MASTERSLICE") && strcmp(type, "OVERLAP"))
     return LEFW_BAD_DATA;  // type is CUT, MASTERSLICE or OVERLAP only
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"LAYER %s\n", layerName);
     encPrint(lefwFile, (char*)"   TYPE %s ;\n", type);
  } else {
     fprintf(lefwFile, "LAYER %s\n", layerName);
     fprintf(lefwFile, "   TYPE %s ;\n", type);
  }
  lefwIsRouting = 0;
  lefwLines = lefwLines + 2;
  lefwState = LEFW_LAYER_START;
  return LEFW_OK;
}


int lefwLayer(double spacing,          /* optional */
              const char* name2) {   /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYER_START &&
      lefwState != LEFW_LAYER) return LEFW_BAD_ORDER;
  if (!lefwIsCut) return LEFW_BAD_DATA;  // only cut calls this func
  if (spacing) {
     if (lefwWriteEncrypt) {
        encPrint(lefwFile, (char*)"   SPACING %g ", spacing);
        if (name2) 
          encPrint(lefwFile, (char*)"LAYER %s ", name2);
        encPrint(lefwFile, (char*)";\n"); 
     } else {
        fprintf(lefwFile, "   SPACING %g ", spacing);
        if (name2) 
           fprintf(lefwFile, "LAYER %s ", name2);
        fprintf(lefwFile, ";\n"); 
     }
  } else if (name2)
     return LEFW_BAD_ORDER;
     
  lefwLines++;
  lefwState = LEFW_LAYER;
  return LEFW_OK;
}


int lefwEndLayer(const char* layerName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYER_START &&
      lefwState != LEFW_LAYER) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END %s\n\n", layerName);
  else
     fprintf(lefwFile, "END %s\n\n", layerName);

  lefwLines++;
  lefwState = LEFW_LAYER_END;  
  lefwIsCut = 0;
  return LEFW_OK;
}


int lefwStartLayerRouting(const char* layerName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_LAYERROUTING_START ||
      lefwState == LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"LAYER %s\n", layerName);
     encPrint(lefwFile, (char*)"   TYPE ROUTING ;\n");
  } else {
     fprintf(lefwFile, "LAYER %s\n", layerName);
     fprintf(lefwFile, "   TYPE ROUTING ;\n");
  }
  lefwIsRouting = 1;
  lefwIsRoutingReqData = 0;
  lefwLines = lefwLines + 2;
  lefwState = LEFW_LAYERROUTING_START;
  lefwSynArray[LEFW_LAYERROUTING_START] = 1;

  return LEFW_OK;
}


int lefwLayerRouting(const char* direction,
                     double width) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  // this routine should call only once per layer routing
  if (lefwSynArray[LEFW_LAYERROUTING]) return LEFW_ALREADY_DEFINED;
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt) {
     if (!direction || direction == 0 || *direction == 0) {
        encPrint(lefwFile, (char*)"DIRECTION is required in Layer(Routing).\n");    
        return LEFW_BAD_DATA;
     }
     if (strcmp(direction, "HORIZONTAL") && strcmp(direction, "VERTICAL")) {
        encPrint(lefwFile, (char*)
           "DIRECTION in Layer(Routing) can only be HORIZONTAL or VERTICAL.\n");
        return LEFW_BAD_DATA; // direction is either HORIZONTAL or VERTICAL only
     }
     encPrint(lefwFile, (char*)"   DIRECTION %s ;\n", direction);
     encPrint(lefwFile, (char*)"   WIDTH %g ;\n", width);
  } else {
     if (!direction || direction == 0 || *direction == 0) {
        fprintf(lefwFile, "DIRECTION is required in Layer(Routing).\n");    
        return LEFW_BAD_DATA;
     }
     if (strcmp(direction, "HORIZONTAL") && strcmp(direction, "VERTICAL")) {
        fprintf(lefwFile,
          "DIRECTION in Layer(Routing) can only be HORIZONTAL or VERTICAL.\n");
        return LEFW_BAD_DATA; // direction is either HORIZONTAL or VERTICAL only
     }
     fprintf(lefwFile, "   DIRECTION %s ;\n", direction);
     fprintf(lefwFile, "   WIDTH %g ;\n", width);
  }

  lefwLines += 2;
  lefwIsRoutingReqData = 1;
  lefwSynArray[LEFW_LAYERROUTING] = 1;
  return LEFW_OK;
}


int lefwLayerRoutingPitch(double pitch) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  // this routine should call only once per layer routing
  // it is required for SE only, other is optional
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   PITCH %g ;\n", pitch);
  else
     fprintf(lefwFile, "   PITCH %g ;\n", pitch);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingOffset(double offset) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   OFFSET %g ;\n", offset);
  else
     fprintf(lefwFile, "   OFFSET %g ;\n", offset);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingSpacing(double spacing, double leftRange,
                            double rightRange) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  lefwHasLayerRoutingRange = 0;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   SPACING %g ", spacing);
     if (leftRange || rightRange) {
        encPrint(lefwFile, (char*)"RANGE %g %g ", leftRange, rightRange);
        lefwHasLayerRoutingRange = 1;
     }
  } else {
     fprintf(lefwFile, "   SPACING %g ", spacing);
     if (leftRange || rightRange) {
        fprintf(lefwFile, "RANGE %g %g ", leftRange, rightRange);
        lefwHasLayerRoutingRange = 1;
     }
  }
  prtSemiColon = 1;    // mark to the next routine to write out ; first
  lefwLines++;
  return LEFW_OK;
}

int lefwLayerRoutingSpacingUseLengthThreshold() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (!lefwHasLayerRoutingRange) {
     // This routine can be called only if range are non zero
     fprintf(lefwFile,
     "ERROR: lefwLayerRoutingSpacingUseLengthThreshold cannot be called if\n");
     fprintf(lefwFile,
     "       lefRange and rightRange in lefwLayerRoutingSpacing are both zero.\n");
     return LEFW_BAD_DATA; 
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)" USELENGTHTHRESHOLD ;\n");
  else
     fprintf(lefwFile, " USELENGTHTHRESHOLD ;\n");
  lefwHasLayerRoutingRange = 0;
  prtSemiColon = 0;
  return LEFW_OK;
}

int lefwLayerRoutingSpacingLengthThreshold(double lengthValue) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (lefwHasLayerRoutingRange) {
     // This routine can be called only if range are zero
     fprintf(lefwFile,
     "ERROR: lefwLayerRoutingSpacingLengthThreshold cannot be called if\n");
     fprintf(lefwFile,
     "       lefRange and rightRange in lefwLayerRoutingSpacing are non zero.\n");
     return LEFW_BAD_DATA; 
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)" LENGTHTHRESHOLD %g ;\n", lengthValue);
  else
     fprintf(lefwFile, " LENGTHTHRESHOLD %g ;\n", lengthValue);
  prtSemiColon = 0;
  return LEFW_OK;
}

int lefwLayerRoutingWireExtension(double wireExtension) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   WIREEXTENSION %g ;\n", wireExtension);
  else
     fprintf(lefwFile, "   WIREEXTENSION %g ;\n", wireExtension);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingResistance(const char* resistance) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (!resistance || resistance == 0 || *resistance == 0) return LEFW_BAD_DATA;
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   RESISTANCE RPERSQ %s ;\n", resistance);
  else
     fprintf(lefwFile, "   RESISTANCE RPERSQ %s ;\n", resistance);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingCapacitance(const char* capacitance) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (!capacitance || capacitance == 0 || *capacitance == 0)
     return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   CAPACITANCE CPERSQDIST %s ;\n", capacitance);
  else
     fprintf(lefwFile, "   CAPACITANCE CPERSQDIST %s ;\n", capacitance);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingHeight(double height) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   HEIGHT %g ;\n", height);
  else
     fprintf(lefwFile, "   HEIGHT %g ;\n", height);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingThickness(double thickness) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   THICKNESS %g ;\n", thickness);
  else
     fprintf(lefwFile, "   THICKNESS %g ;\n", thickness);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingShrinkage(double shrinkage) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   SHRINKAGE %g ;\n", shrinkage);
  else
     fprintf(lefwFile, "   SHRINKAGE %g ;\n", shrinkage);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingCapMultiplier(double capMultiplier) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   CAPMULTIPLIER %g ;\n", capMultiplier);
  else
     fprintf(lefwFile, "   CAPMULTIPLIER %g ;\n", capMultiplier);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingEdgeCap(double edgeCap) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   EDGECAPACITANCE %g ;\n", edgeCap);
  else
     fprintf(lefwFile, "   EDGECAPACITANCE %g ;\n", edgeCap);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingAntennaArea(double antennaArea) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   ANTENNAAREAFACTOR %g ;\n", antennaArea);
  else
     fprintf(lefwFile, "   ANTENNAAREAFACTOR %g ;\n", antennaArea);
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerRoutingAntennaLength(double antennaLength) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRouting) return LEFW_BAD_DATA;  // only routing calls this func
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   ANTENNALENGTHFACTOR %g ;\n", antennaLength);
  else
     fprintf(lefwFile, "   ANTENNALENGTHFACTOR %g ;\n", antennaLength);
  lefwLines++;
  return LEFW_OK;
}


int lefwEndLayerRouting(const char* layerName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START &&
      lefwState != LEFW_LAYERROUTING) return LEFW_BAD_ORDER;
  if (!lefwIsRoutingReqData) {
      return LEFW_BAD_ORDER; 
  }
  if (prtSemiColon) {
     // the previous statement hasn't written the ; yet
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)";\n");
     else
        fprintf(lefwFile, ";\n");
     prtSemiColon = 0;
  }
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END %s\n\n", layerName);
  else
     fprintf(lefwFile, "END %s\n\n", layerName);

  lefwLines++;
  lefwSynArray[LEFW_LAYERROUTING] = 0;
  lefwState = LEFW_LAYERROUTING_END;  
  lefwIsRouting = 0;
  return LEFW_OK;
}


int lefwLayerACCurrentDensity(const char* type, double value) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START && lefwState != LEFW_LAYERROUTING
      && lefwState != LEFW_LAYER_START && lefwState != LEFW_LAYER)
      return LEFW_BAD_ORDER;
  if (strcmp(type, "PEAK") && strcmp(type, "AVERAGE") && strcmp(type, "RMS")) 
     return LEFW_BAD_DATA;  // type is PEAK, AVERAGE & RMS only
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   ACCURRENTDENSITY %s", type);
     lefwLines++;
     lefwTableLen = 5;
     if (value)
         encPrint(lefwFile, (char*)" %g\n", value); // that's it for accurrentdensity
     else {
         encPrint(lefwFile, (char*)"\n");  // expect to have frequency & tableentries
         lefwOldState = lefwState;   // save the previous state
         lefwState = LEFW_LAYERACCURRENT;  
     }
  } else {
     fprintf(lefwFile, "   ACCURRENTDENSITY %s", type);
     lefwLines++;
     lefwTableLen = 5;
     if (value)
         fprintf(lefwFile, " %g\n", value); // that's it for accurrentdensity
     else {
         fprintf(lefwFile, "\n");  //  expect to have frequency & tableentries
         lefwOldState = lefwState;  // save the previous state
         lefwState = LEFW_LAYERACCURRENT;  
     }
  }
  return LEFW_OK;
}


int lefwLayerACFrequency(int numFrequency, double* frequency) {
  int i;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERACCURRENT) return LEFW_BAD_ORDER;
  
  if (numFrequency <= 0) return LEFW_BAD_DATA;  // frequency are required
 
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      FREQUENCY ");
     for (i = 0; i < numFrequency; i++) {
        encPrint(lefwFile, (char*)"%g ", *frequency++);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      FREQUENCY ");
     for (i = 0; i < numFrequency; i++) {
        fprintf(lefwFile, "%g ", *frequency++);
     }
     fprintf(lefwFile, ";\n");
  }

  lefwLines++;
  return LEFW_OK;
}


int lefwLayerACWidth(int numWidths, double* widths) {
  int i;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERACCURRENT) return LEFW_BAD_ORDER;
  
  if (numWidths <= 0) return LEFW_OK;  // width is optional
 
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      WIDTH ");
     for (i = 0; i < numWidths; i++) {
        encPrint(lefwFile, (char*)"%g ", *widths++);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      WIDTH ");
     for (i = 0; i < numWidths; i++) {
        fprintf(lefwFile, "%g ", *widths++);
     }
     fprintf(lefwFile, ";\n");
  }

  lefwTableLen = numWidths;
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerACCutarea(int numCutareas, double* cutareas) {
  int i;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERACCURRENT) return LEFW_BAD_ORDER;
  
  if (numCutareas <= 0) return LEFW_OK;  // cutarea is optional
 
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      CUTAREA ");
     for (i = 0; i < numCutareas; i++) {
        encPrint(lefwFile, (char*)"%g ", *cutareas++);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      CUTAREA ");
     for (i = 0; i < numCutareas; i++) {
        fprintf(lefwFile, "%g ", *cutareas++);
     }
     fprintf(lefwFile, ";\n");
  }

  lefwTableLen = numCutareas;
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerACTableEntries(int numEntries, double* entries) {
  int i;
  int j = 0;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERACCURRENT) return LEFW_BAD_ORDER;
  
  if (numEntries <= 0) return LEFW_BAD_DATA;  // tableEntries are required
 
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      TABLEENTRIES\n        ");
     for (i = 0; i < numEntries; i++) {
        if (++j > lefwTableLen) {
            encPrint(lefwFile, (char*)"\n        ");
            j = 1;
        }
        encPrint(lefwFile, (char*)"%g ", *entries++);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      TABLEENTRIES\n        ");
     for (i = 0; i < numEntries; i++) {
        if (++j > lefwTableLen) {
            fprintf(lefwFile,"\n        ");
            j = 1;
        }
        fprintf(lefwFile, "%g ", *entries++);
     }
     fprintf(lefwFile, ";\n");
  }

  lefwState = lefwOldState;   // restore the previous state
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerDCCurrentDensity(const char* type, double value) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERROUTING_START && lefwState != LEFW_LAYERROUTING
      && lefwState != LEFW_LAYER_START && lefwState != LEFW_LAYER)
      return LEFW_BAD_ORDER;
  if (strcmp(type, "AVERAGE")) return LEFW_BAD_DATA;  // type is AVERAGE
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   DCCURRENTDENSITY %s", type);
     lefwLines++;
     lefwTableLen = 5;
     if (value)
         encPrint(lefwFile, (char*)" %g\n", value); // that's it for accurrentdensity
     else {
         encPrint(lefwFile, (char*)"\n");    // expect to have frequency & tableentries
         lefwOldState = lefwState;   // save the previous state
         lefwState = LEFW_LAYERDCCURRENT;  
     }
  } else {
     fprintf(lefwFile, "   DCCURRENTDENSITY %s", type);
     lefwLines++;
     lefwTableLen = 5;
     if (value)
         fprintf(lefwFile, " %g\n", value); // that's it for accurrentdensity
     else {
         fprintf(lefwFile, "\n");    // expect to have frequency & tableentries
         lefwOldState = lefwState;   // save the previous state
         lefwState = LEFW_LAYERDCCURRENT;  
     }
  }
  return LEFW_OK;
}


int lefwLayerDCWidth(int numWidths, double* widths) {
  int i;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERDCCURRENT) return LEFW_BAD_ORDER;

  // width is allowed in Layer routing only
  if (lefwOldState != LEFW_LAYER_START && lefwOldState != LEFW_LAYER &&
      lefwOldState != LEFW_LAYERROUTING_START &&
      lefwOldState != LEFW_LAYERROUTING) return LEFW_BAD_DATA;
  
  if (numWidths <= 0) return LEFW_BAD_DATA;  // width is required
 
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      WIDTH ");
     for (i = 0; i < numWidths; i++) {
        encPrint(lefwFile, (char*)"%g ", *widths++);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      WIDTH ");
     for (i = 0; i < numWidths; i++) {
        fprintf(lefwFile, "%g ", *widths++);
     }
     fprintf(lefwFile, ";\n");
  }

  lefwTableLen = numWidths;
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerDCCutarea(int numCutareas, double* cutareas) {
  int i;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERDCCURRENT) return LEFW_BAD_ORDER;

  // cutarea is allowed in Layer non-routing only
  if (lefwOldState != LEFW_LAYER_START && lefwOldState != LEFW_LAYER &&
      lefwOldState != LEFW_LAYERROUTING_START &&
      lefwOldState != LEFW_LAYERROUTING) return LEFW_BAD_DATA;
  
  if (numCutareas <= 0) return LEFW_BAD_DATA;  // cutarea is required
 
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      CUTAREA ");
     for (i = 0; i < numCutareas; i++) {
        encPrint(lefwFile, (char*)"%g ", *cutareas++);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      CUTAREA ");
     for (i = 0; i < numCutareas; i++) {
        fprintf(lefwFile, "%g ", *cutareas++);
     }
     fprintf(lefwFile, ";\n");
  }

  lefwTableLen = numCutareas;
  lefwLines++;
  return LEFW_OK;
}


int lefwLayerDCTableEntries(int numEntries, double* entries) {
  int i;
  int j = 0;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_LAYERDCCURRENT) return LEFW_BAD_ORDER;
  
  if (numEntries <= 0) return LEFW_BAD_DATA;  // tableEntries are required
 
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      TABLEENTRIES\n        ");
     for (i = 0; i < numEntries; i++) {
        if (++j > lefwTableLen) {
            encPrint(lefwFile, (char*)"\n        ");
            j = 1;
        }
        encPrint(lefwFile, (char*)"%g ", *entries++);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      TABLEENTRIES\n        ");
     for (i = 0; i < numEntries; i++) {
        if (++j > lefwTableLen) {
            fprintf(lefwFile,"\n        ");
            j = 1;
        }
        fprintf(lefwFile, "%g ", *entries++);
     }
     fprintf(lefwFile, ";\n");
  }

  lefwState = lefwOldState;   // restore the previous state
  lefwLines++;
  return LEFW_OK;
}


int lefwStartPropDef() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwSynArray[LEFW_PROPERTYDEF_START]) return LEFW_ALREADY_DEFINED;
  lefwSynArray[LEFW_ANTENNASIZE] = 1;
  if (lefwState == LEFW_PROPERTYDEF_START ||
      lefwState == LEFW_PROPERTYDEF) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwWriteEncrypt) 
     encPrint(lefwFile, (char*)"PROPERTYDEFINITIONS\n");
  else
     fprintf(lefwFile, "PROPERTYDEFINITIONS\n");

  lefwLines++;
  lefwState = LEFW_PROPERTYDEF_START;
  lefwSynArray[LEFW_PROPERTYDEF_START] = 1;
  return LEFW_OK;
}


int lefwIntPropDef(const char* objType, 
                   const char* propName,
                   double leftRange, double rightRange,    /* optional */
                   int    propValue                        /* optional */
               ) {

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_PROPERTYDEF_START &&
      lefwState != LEFW_PROPERTYDEF) return LEFW_BAD_ORDER;
  if ((! objType && ! *objType) || (! propName && ! *propName)) // require
     return LEFW_BAD_DATA;

  if (strcmp(objType, "LIBRARY") && strcmp(objType, "VIA") &&
      strcmp(objType, "MACRO") && strcmp(objType, "PIN") &&
      strcmp(objType, "LAYER") && strcmp(objType, "NONDEFAULTRULE") &&
      strcmp(objType, "VIARULE"))
     return LEFW_BAD_DATA;

  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   %s %s INTEGER ", objType, propName);
     if (leftRange || rightRange)
        encPrint(lefwFile, (char*)"RANGE %g %g ", leftRange, rightRange);
   
     if (propValue)
        encPrint(lefwFile, (char*)"%d ", propValue);
   
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "   %s %s INTEGER ", objType, propName);
     if (leftRange || rightRange)
        fprintf(lefwFile, "RANGE %g %g ", leftRange, rightRange);
   
     if (propValue)
        fprintf(lefwFile, "%d ", propValue);
   
     fprintf(lefwFile, ";\n");
  }

  lefwLines++;
  lefwState = LEFW_PROPERTYDEF;
  return LEFW_OK;
}
 

int lefwRealPropDef(const char* objType, 
                    const char* propName,
                    double leftRange, double rightRange,    /* optional */
                    double propValue                        /* optional */
               ) {

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_PROPERTYDEF_START &&
      lefwState != LEFW_PROPERTYDEF) return LEFW_BAD_ORDER;
  if ((! objType && ! *objType) || (! propName && ! *propName)) // require
     return LEFW_BAD_DATA;

  if (strcmp(objType, "LIBRARY") && strcmp(objType, "VIA") &&
      strcmp(objType, "MACRO") && strcmp(objType, "PIN") &&
      strcmp(objType, "LAYER") && strcmp(objType, "NONDEFAULTRULE") &&
      strcmp(objType, "VIARULE"))
     return LEFW_BAD_DATA;

  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   %s %s REAL ", objType, propName);
     if (leftRange || rightRange)
        encPrint(lefwFile, (char*)"RANGE %g %g ", leftRange, rightRange);

     if (propValue)
        encPrint(lefwFile, (char*)"%g ", propValue);

     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "   %s %s REAL ", objType, propName);
     if (leftRange || rightRange)
        fprintf(lefwFile, "RANGE %g %g ", leftRange, rightRange);

     if (propValue)
        fprintf(lefwFile, "%g ", propValue);

     fprintf(lefwFile, ";\n");
  }
 
  lefwLines++;
  lefwState = LEFW_PROPERTYDEF;
  return LEFW_OK;
}
 

int lefwStringPropDef(const char* objType, 
                      const char* propName,
                      double leftRange, double rightRange,    /* optional */
                      const char* propValue                   /* optional */
               ) {

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_PROPERTYDEF_START &&
      lefwState != LEFW_PROPERTYDEF) return LEFW_BAD_ORDER;
  if ((! objType && ! *objType) || (! propName && ! *propName))
     return LEFW_BAD_DATA;

  if (strcmp(objType, "LIBRARY") && strcmp(objType, "VIA") &&
      strcmp(objType, "MACRO") && strcmp(objType, "PIN") &&
      strcmp(objType, "LAYER") && strcmp(objType, "NONDEFAULTRULE") &&
      strcmp(objType, "VIARULE"))
     return LEFW_BAD_DATA;

  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   %s %s STRING ", objType, propName);
     if (leftRange || rightRange)
        encPrint(lefwFile, (char*)"RANGE %g %g ", leftRange, rightRange);

     if (propValue)
        encPrint(lefwFile, (char*)"\"%s\" ", propValue);  // string, set quotes

     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "   %s %s STRING ", objType, propName);
     if (leftRange || rightRange)
        fprintf(lefwFile, "RANGE %g %g ", leftRange, rightRange);

     if (propValue)
        fprintf(lefwFile, "\"%s\" ", propValue);  // string, set quotes

     fprintf(lefwFile, ";\n");
  }

  lefwLines++;
  lefwState = LEFW_PROPERTYDEF;
  return LEFW_OK;
}
 

int lefwEndPropDef() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_PROPERTYDEF_START &&
      lefwState != LEFW_PROPERTYDEF) return LEFW_BAD_ORDER;
 
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END PROPERTYDEFINITIONS\n\n");
  else
     fprintf(lefwFile, "END PROPERTYDEFINITIONS\n\n");
  lefwLines++;
  lefwState = LEFW_PROPERTYDEF_END;
  return LEFW_OK;
}
 

int lefwEnd() {
  if (! lefwFile) return 1;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END LIBRARY\n");
  else
     fprintf(lefwFile, "END LIBRARY\n");
  lefwLines++;
  // lefwFile = 0;
  lefwState = LEFW_DONE;
  return LEFW_OK;
}


int lefwStartVia(const char* viaName, const char* isDefault) {   /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_VIA_START ||
      lefwState == LEFW_VIA) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END &&
      (!lefwIsNonDefaultRule))  /* via defined in nondefaultrule */
     return LEFW_BAD_ORDER;  // not of the ENDs

  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"VIA %s ", viaName);
     if (isDefault && strcmp(isDefault, "DEFAULT") == 0)
        encPrint(lefwFile, (char*)"DEFAULT");
     encPrint(lefwFile, (char*)"\n");
  } else {
     fprintf(lefwFile, "VIA %s ", viaName);
     if (isDefault && strcmp(isDefault, "DEFAULT") == 0)
        fprintf(lefwFile, "DEFAULT");
     fprintf(lefwFile, "\n");
  }
  lefwLines++;

  lefwSynArray[LEFW_VIA_START] = 1;
  lefwState = LEFW_VIA_START;
  lefwDidLayer = 0;           // hasn't set the layer yet
  return LEFW_OK;
}


int lefwViaTopofstackonly() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_VIA_START &&
      lefwState != LEFW_VIA) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   TOPOFSTACKONLY\n");
  else
     fprintf(lefwFile, "   TOPOFSTACKONLY\n");
  lefwLines++;
  return LEFW_OK;
}


int lefwViaForeign(const char* foreignName,
                   double xl,          /* optional */
                   double yl,          /* optional */
                   int orient){        /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_VIA_START &&
      lefwState != LEFW_VIA) return LEFW_BAD_ORDER;

  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   FOREIGN %s ", foreignName);
     if (xl || yl) {
         encPrint(lefwFile, (char*)"%g %g ", xl, yl);
         if (orient >= 0 && orient <= 7)
            encPrint(lefwFile, (char*)"%s ", lefwOrient(orient));
     } else if (orient > 0 && orient <= 7) {
         encPrint(lefwFile, (char*)";\n");
         return LEFW_BAD_ORDER;    // orient should be inside pt
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "   FOREIGN %s ", foreignName);
     if (xl || yl) {
         fprintf(lefwFile, "%g %g ", xl, yl);
         if (orient >= 0 && orient <= 7)
            fprintf(lefwFile, "%s ", lefwOrient(orient));
     } else if (orient > 0 && orient <= 7) {
         fprintf(lefwFile, ";\n");
         return LEFW_BAD_ORDER;    // orient should be inside pt
     }
     fprintf(lefwFile, ";\n");
  }

  lefwLines++;
  lefwState = LEFW_VIA;
  return LEFW_OK;
}


int lefwViaResistance(double resistance) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_VIA_START &&
      lefwState != LEFW_VIA) return LEFW_BAD_ORDER;

  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   RESISTANCE %g ;\n", resistance);
  else
     fprintf(lefwFile, "   RESISTANCE %g ;\n", resistance);

  lefwLines++;
  lefwState = LEFW_VIA;
  return LEFW_OK;
}


int lefwViaLayer(const char* layerName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_VIA_START && lefwState != LEFW_VIA )
      return LEFW_BAD_ORDER;

  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   LAYER %s ;\n", layerName);
  else
     fprintf(lefwFile, "   LAYER %s ;\n", layerName);

  lefwLines++;
  lefwState = LEFW_VIA;
  lefwDidLayer = 1;
  return LEFW_OK;
}


int lefwViaLayerRect(double x1l, double y1l, double x2l, double y2l) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_VIA_START && lefwState != LEFW_VIA)
      return LEFW_BAD_ORDER;
  if (!lefwDidLayer) return LEFW_BAD_ORDER;

  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      RECT %g %g %g %g ;\n", x1l, y1l, x2l, y2l);
  else
     fprintf(lefwFile, "      RECT %g %g %g %g ;\n", x1l, y1l, x2l, y2l);

  lefwLines++;
  lefwState = LEFW_VIA;
  return LEFW_OK;
}


int lefwStringProperty(const char* propName, const char* propValue) {
if ((lefwState != LEFW_VIA) && (lefwState != LEFW_LAYER) &&
      (lefwState != LEFW_VIARULE) && (lefwState != LEFW_MACRO_START) &&
      (lefwState != LEFW_MACRO) && (lefwState != LEFW_VIA_START) &&
      (lefwState != LEFW_VIARULE_START) && (lefwState != LEFW_LAYER_START) &&
      (lefwState != LEFW_BEGINEXT))
      return LEFW_BAD_ORDER;

  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   PROPERTY %s \"%s\" ;\n", propName, propValue);
  else
     fprintf(lefwFile, "   PROPERTY %s \"%s\" ;\n", propName, propValue);
  lefwLines++;
  return LEFW_OK;
}


int lefwRealProperty(const char* propName, double propValue) {
if ((lefwState != LEFW_VIA) && (lefwState != LEFW_LAYER) &&
      (lefwState != LEFW_VIARULE) && (lefwState != LEFW_MACRO_START) &&
      (lefwState != LEFW_MACRO) && (lefwState != LEFW_VIA_START) &&
      (lefwState != LEFW_VIARULE_START) && (lefwState != LEFW_LAYER_START) &&
      (lefwState != LEFW_BEGINEXT))
      return LEFW_BAD_ORDER;
 
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   PROPERTY %s %g ;\n", propName, propValue);
  else
     fprintf(lefwFile, "   PROPERTY %s %g ;\n", propName, propValue);
  lefwLines++;
  return LEFW_OK;
}


int lefwIntProperty(const char* propName, int propValue) {
if ((lefwState != LEFW_VIA) && (lefwState != LEFW_LAYER) &&
      (lefwState != LEFW_VIARULE) && (lefwState != LEFW_MACRO_START) &&
      (lefwState != LEFW_MACRO) && (lefwState != LEFW_VIA_START) &&
      (lefwState != LEFW_VIARULE_START) && (lefwState != LEFW_LAYER_START) &&
      (lefwState != LEFW_BEGINEXT))
      return LEFW_BAD_ORDER;

  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   PROPERTY %s %d ;\n", propName, propValue);
  else
     fprintf(lefwFile, "   PROPERTY %s %d ;\n", propName, propValue);
  lefwLines++;
  return LEFW_OK;
}


int lefwEndVia(const char* viaName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_VIA_START &&
      lefwState != LEFW_VIA) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END %s\n\n", viaName);
  else
     fprintf(lefwFile, "END %s\n\n", viaName);
 
  lefwLines++;
  lefwState = LEFW_VIA_END;
  lefwDidLayer = 0; 
  return LEFW_OK;
}


// internal function
int lefwViaRulePrtLayer (const char* layerName, const char* direction,
                         double minWidth, double maxWidth,
                         double overhang, double metalOverhang) {
  if (!layerName || layerName == 0 || *layerName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   LAYER %s ;\n", layerName);
  else
     fprintf(lefwFile, "   LAYER %s ;\n", layerName);
  if (direction && strcmp(direction, "HORIZONTAL") &&
      strcmp(direction, "VERTICAL"))
      return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      DIRECTION %s ;\n", direction);
     lefwLines = lefwLines + 2;
     if (minWidth || maxWidth) {
         encPrint(lefwFile, (char*)"      WIDTH %g TO %g ;\n", minWidth, maxWidth);
         lefwLines++;
     }
     if (overhang) {
         encPrint(lefwFile, (char*)"      OVERHANG %g ;\n", overhang);
         lefwLines++;
     }
     if (metalOverhang) {
         encPrint(lefwFile, (char*)"      METALOVERHANG %g ;\n", metalOverhang);
         lefwLines++;
     }
  } else {
     fprintf(lefwFile, "      DIRECTION %s ;\n", direction);
     lefwLines = lefwLines + 2;
     if (minWidth || maxWidth) {
         fprintf(lefwFile, "      WIDTH %g TO %g ;\n", minWidth, maxWidth);
         lefwLines++;
     }
     if (overhang) {
         fprintf(lefwFile, "      OVERHANG %g ;\n", overhang);
         lefwLines++;
     }
     if (metalOverhang) {
         fprintf(lefwFile, "      METALOVERHANG %g ;\n", metalOverhang);
         lefwLines++;
     }
  }
  return LEFW_OK;
}


int lefwStartViaRule(const char* viaRuleName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_VIARULE_START ||
      lefwState == LEFW_VIARULE) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
 
  if (!viaRuleName || viaRuleName == 0 || *viaRuleName == 0)
     return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) 
     encPrint(lefwFile, (char*)"VIARULE %s\n", viaRuleName);
  else
     fprintf(lefwFile, "VIARULE %s\n", viaRuleName);
  lefwLines++;
 
  lefwSynArray[LEFW_VIARULE_START] = 1;
  lefwState = LEFW_VIARULE_START;
  lefwDidLayer = 0;           // hasn't set the layer yet
  lefwNumViaRuleLayers = 0;
  return LEFW_OK;
}

int lefwViaRuleLayer(const char* layerName, const char* direction,
                double minWidth, double maxWidth,        /* optional */
                double overhang, double metalOverhang) { /* optional */
  int status ;
 
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_VIARULE_START &&
      lefwState != LEFW_VIARULE) return LEFW_BAD_ORDER;
  if (lefwNumViaRuleLayers >= 2)
      return LEFW_BAD_ORDER;
  if ((status = lefwViaRulePrtLayer(layerName, direction, minWidth, maxWidth,
                                    overhang, metalOverhang)) != LEFW_OK)
      return status;
  lefwNumViaRuleLayers++;
  lefwState = LEFW_VIARULE;
  return LEFW_OK;
}

int lefwViaRuleVia(const char* viaName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_VIARULE) return LEFW_BAD_ORDER;
  if (lefwNumViaRuleLayers != 2)   // this routine has to be after two layers
      return LEFW_BAD_ORDER;      // be written
  if (!viaName || !*viaName)      // viaName is required
      return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) 
     encPrint(lefwFile, (char*)"      VIA %s ;\n", viaName);
  else
     fprintf(lefwFile, "      VIA %s ;\n", viaName);
  lefwLines++;
  return LEFW_OK;
}

int lefwEndViaRule(const char* viaRuleName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_VIARULE_START &&
      lefwState != LEFW_VIARULE) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt) 
     encPrint(lefwFile, (char*)"END %s\n\n", viaRuleName);
  else
     fprintf(lefwFile, "END %s\n\n", viaRuleName);
 
  lefwLines++;
  lefwState = LEFW_VIARULE_END;
  lefwDidLayer = 0;
  lefwNumViaRuleLayers = 0;
  return LEFW_OK;
}

int lefwStartViaRuleGen(const char* viaRuleName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_VIARULEGEN_START ||
      lefwState == LEFW_VIARULEGEN) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
 
  if (!viaRuleName || viaRuleName == 0 || *viaRuleName == 0)
     return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) 
     encPrint(lefwFile, (char*)"VIARULE %s GENERATE\n", viaRuleName);
  else
     fprintf(lefwFile, "VIARULE %s GENERATE\n", viaRuleName);
  lefwLines++;
 
  lefwSynArray[LEFW_VIARULEGEN_START] = 1;
  lefwState = LEFW_VIARULEGEN_START;
  lefwDidLayer = 0;           // hasn't set the layer yet
  lefwNumViaRuleLayers = 0;
  return LEFW_OK;
}

int lefwViaRuleGenLayer(const char* layerName, const char* direction,
                double minWidth, double maxWidth,        /* optional */
                double overhang, double metalOverhang) { /* optional */
  int status ;
 
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_VIARULEGEN_START &&
      lefwState != LEFW_VIARULEGEN) return LEFW_BAD_ORDER;
  if (lefwNumViaRuleLayers >= 2)
      return LEFW_BAD_ORDER;
  if ((status = lefwViaRulePrtLayer(layerName, direction, minWidth, maxWidth,
                                    overhang, metalOverhang)) != LEFW_OK)
      return status;
  lefwNumViaRuleLayers++;
  lefwState = LEFW_VIARULEGEN;
  return LEFW_OK;
}

int lefwViaRuleGenLayer3(const char* layerName,
                        double xl, double yl, double xh, double yh,
                        double xSpacing, double ySpacing,
                        double resistance) {    /* optional */

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_VIARULEGEN) return LEFW_BAD_ORDER;
  if (lefwNumViaRuleLayers != 2)   // this routine has to be after two layers
      return LEFW_BAD_ORDER;      // be written
  if (!layerName || layerName == 0 || *layerName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   LAYER %s ;\n", layerName);
     encPrint(lefwFile, (char*)"      RECT %g %g %g %g ;\n", xl, yl, xh, yh);
     encPrint(lefwFile, (char*)"      SPACING %g BY %g ;\n", xSpacing, ySpacing);
     if (resistance) {
         encPrint(lefwFile, (char*)"      RESISTANCE %g ;\n", resistance);
         lefwLines++;
     }
  } else {
     fprintf(lefwFile, "   LAYER %s ;\n", layerName);
     fprintf(lefwFile, "      RECT %g %g %g %g ;\n", xl, yl, xh, yh);
     fprintf(lefwFile, "      SPACING %g BY %g ;\n", xSpacing, ySpacing);
     if (resistance) {
         fprintf(lefwFile, "      RESISTANCE %g ;\n", resistance);
         lefwLines++;
     }
  }
  lefwLines + 3;
  return LEFW_OK;
}

int lefwEndViaRuleGen(const char* viaRuleName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_VIARULEGEN_START &&
      lefwState != LEFW_VIARULEGEN) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END %s\n\n", viaRuleName);
  else
     fprintf(lefwFile, "END %s\n\n", viaRuleName);
 
  lefwLines++;
  lefwState = LEFW_VIARULEGEN_END;
  lefwDidLayer = 0;
  lefwNumViaRuleLayers = 0;
  return LEFW_OK;
}

int lefwStartNonDefaultRule(const char* ruleName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_NONDEFAULTRULE_START ||
      lefwState == LEFW_NONDEFAULTRULE) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
 
  if (!ruleName || ruleName == 0 || *ruleName == 0)
     return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"NONDEFAULTRULE %s\n", ruleName);
  else
     fprintf(lefwFile, "NONDEFAULTRULE %s\n", ruleName);
  lefwLines++;
 
  lefwSynArray[LEFW_NONDEFAULTRULE_START] = 1;
  lefwState = LEFW_NONDEFAULTRULE_START;
  lefwDidLayer = 0;           // hasn't set the layer yet
  lefwIsNonDefaultRule = 1;
  return LEFW_OK;
}

int lefwNonDefaultRuleLayer(const char* routingLayerName,
                       double width, double minSpacing, double wireExtension) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_NONDEFAULTRULE_START &&
      lefwState != LEFW_NONDEFAULTRULE) return LEFW_BAD_ORDER;
  if (!routingLayerName || routingLayerName == 0 || *routingLayerName == 0)
     return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   LAYER %s\n", routingLayerName);
     encPrint(lefwFile, (char*)"      WIDTH %g ;\n", width);
     encPrint(lefwFile, (char*)"      SPACING %g ;\n", minSpacing);
     if (wireExtension)
        encPrint(lefwFile, (char*)"      WIREEXTENSION %g ;\n", wireExtension);
     encPrint(lefwFile, (char*)"   END %s\n\n", routingLayerName);
  } else {
     fprintf(lefwFile, "   LAYER %s\n", routingLayerName);
     fprintf(lefwFile, "      WIDTH %g ;\n", width);
     fprintf(lefwFile, "      SPACING %g ;\n", minSpacing);
     if (wireExtension)
        fprintf(lefwFile, "      WIREEXTENSION %g ;\n", wireExtension);
     fprintf(lefwFile, "   END %s\n\n", routingLayerName);
  }

  lefwLines = lefwLines + 5;
  lefwSynArray[LEFW_NONDEFAULTRULE] = 1;
  lefwState = LEFW_NONDEFAULTRULE;
  return LEFW_OK;
}

int lefwEndNonDefaultRule(const char* ruleName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_NONDEFAULTRULE_START &&
      lefwState != LEFW_NONDEFAULTRULE &&
      lefwState != LEFW_VIA_END && lefwState != LEFW_SPACING_END)
         return LEFW_BAD_ORDER;
      /* can be right after a via or spacing defined in nondefaultrule */
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END %s\n\n", ruleName);
  else
     fprintf(lefwFile, "END %s\n\n", ruleName);
 
  lefwLines++;
  lefwState = LEFW_NONDEFAULTRULE_END;
  lefwDidLayer = 0;
  lefwIsNonDefaultRule = 0;
  return LEFW_OK;
}

int lefwStartSpacing() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_SPACING_START ||
      lefwState == LEFW_SPACING) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END &&
     (!lefwIsNonDefaultRule))  // spacing defined in nondefaultrule
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"SPACING\n");
  else
     fprintf(lefwFile, "SPACING\n");

  lefwLines++;
  lefwState = LEFW_SPACING_START;
  return LEFW_OK;
}


int lefwSpacing(const char* layerName1, const char* layerName2,
                double minSpace, const char* stack) {  /* optioanl */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_SPACING_START &&
      lefwState != LEFW_SPACING) return LEFW_BAD_ORDER;
  if (!layerName1 || layerName1 == 0 || *layerName1 == 0) return LEFW_BAD_DATA;
  if (!layerName2 || layerName2 == 0 || *layerName2 == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   SAMENET %s %s %g ", layerName1, layerName2,
              minSpace);
     if (stack && strcmp(stack, "STACK") == 0)
        encPrint(lefwFile, (char*)"STACK ");
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "   SAMENET %s %s %g ", layerName1, layerName2,
             minSpace);
     if (stack && strcmp(stack, "STACK") == 0)
        fprintf(lefwFile, "STACK ");
     fprintf(lefwFile, ";\n");
  }

  lefwLines++;
  lefwState = LEFW_SPACING;
  return LEFW_OK;
}


int lefwEndSpacing() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_SPACING_START &&
      lefwState != LEFW_SPACING) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END SPACING\n\n");
  else
     fprintf(lefwFile, "END SPACING\n\n");
 
  lefwLines++;
  lefwState = LEFW_SPACING_END;
  return LEFW_OK;
}


int lefwUniversalNoiseMargin (double high, double low) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_NOISEMARGIN]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"UNIVERSALNOISEMARGIN %g %g ;\n", high, low);
  else
     fprintf(lefwFile, "UNIVERSALNOISEMARGIN %g %g ;\n", high, low);
  lefwSynArray[LEFW_NOISEMARGIN] = 1;
  return LEFW_OK; 
}


int lefwEdgeRateThreshold1 (double num) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_THRESHOLD1]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"EDGERATETHRESHOLD1 %g ;\n", num);
  else
     fprintf(lefwFile, "EDGERATETHRESHOLD1 %g ;\n", num);
  lefwSynArray[LEFW_THRESHOLD1] = 1;
  return LEFW_OK; 
}


int lefwEdgeRateThreshold2 (double num) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_THRESHOLD2]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"EDGERATETHRESHOLD2 %g ;\n", num);
  else
     fprintf(lefwFile, "EDGERATETHRESHOLD2 %g ;\n", num);
  lefwSynArray[LEFW_THRESHOLD2] = 1;
  return LEFW_OK;
}


int lefwEdgeRateScaleFactor (double num) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_SCALEFACTOR]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"EDGERATESCALEFACTOR %g ;\n", num);
  else
     fprintf(lefwFile, "EDGERATESCALEFACTOR %g ;\n", num);
  lefwSynArray[LEFW_SCALEFACTOR] = 1;
  return LEFW_OK;
}
 

int lefwStartNoiseTable(int num) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_NOISETABLE_START ||
      lefwState == LEFW_NOISETABLE) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_NOISETABLE_START]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"NOISETABLE %d ;\n", num);
  else
     fprintf(lefwFile, "NOISETABLE %d ;\n", num);
 
  lefwLines++;
  lefwState = LEFW_NOISETABLE_START;
  lefwIsNoiseTable = 1;
  lefwIsEdgerate = 0;
  lefwSynArray[LEFW_NOISETABLE_START] = 1;
  return LEFW_OK;
}


// for both noisetable and correctiontable
int lefwEdgeRate(int num) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsNoiseTable && ! lefwIsCorrectTable) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   EDGERATE %d ;\n", num);
  else
     fprintf(lefwFile, "   EDGERATE %d ;\n", num);

  lefwLines++;
  if (lefwState == LEFW_NOISETABLE_START)
     lefwState = LEFW_NOISETABLE;
  else if (lefwState == LEFW_CORRECTTABLE_START)
     lefwState = LEFW_CORRECTTABLE;
  lefwIsEdgerate = 1;
  lefwIsOutResist = 0;
  return LEFW_OK;
}


int lefwOutputResistance(int numResists, double* resistance) {
  int i;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsEdgerate) return LEFW_BAD_ORDER;
  if (lefwIsOutResist) return LEFW_BAD_ORDER;

  if (numResists <= 0) return LEFW_OK;

  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   OUTPUTRESISTANCE ");
     for (i = 0; i < numResists; i++) {
        encPrint(lefwFile, (char*)"%g ", *resistance++);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "   OUTPUTRESISTANCE ");
     for (i = 0; i < numResists; i++) {
        fprintf(lefwFile, "%g ", *resistance++);
     }
     fprintf(lefwFile, ";\n");
  }

  lefwIsOutResist = 1;
  lefwLines++;
  return LEFW_OK;
}


int lefwVictims(int length, int numNoises, double* noises) {
  int     i;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsOutResist) return LEFW_BAD_ORDER;

  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      VICTIMLENGTH %d ;\n", length);
     if (numNoises <= 0) return LEFW_BAD_DATA;   // noises are required
     for (i = 0; i < numNoises; i++) {
        if (lefwIsNoiseTable)
           encPrint(lefwFile, (char*)"      VICTIMNOISE ");
        else
           encPrint(lefwFile, (char*)"      CORRECTIONFACTOR ");
        encPrint(lefwFile, (char*)"%g ", *noises++);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      VICTIMLENGTH %d ;\n", length);
     if (numNoises <= 0) return LEFW_BAD_DATA;   // noises are required
     for (i = 0; i < numNoises; i++) {
        if (lefwIsNoiseTable)
           fprintf(lefwFile, "      VICTIMNOISE ");
        else
           fprintf(lefwFile, "      CORRECTIONFACTOR ");
        fprintf(lefwFile, "%g ", *noises++);
     }
     fprintf(lefwFile, ";\n");
  }

  lefwLines++;
  return LEFW_OK;
}


int lefwEndNoiseTable() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_NOISETABLE_START &&
      lefwState != LEFW_NOISETABLE) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END NOISETABLE\n\n");
  else
     fprintf(lefwFile, "END NOISETABLE\n\n");
  lefwLines++;
 
  lefwState = LEFW_NOISETABLE_END;
  lefwIsNoiseTable = 0;
  lefwIsOutResist = 0;
  return LEFW_OK;
}


int lefwStartCorrectTable(int num) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_CORRECTTABLE_START ||
      lefwState == LEFW_CORRECTTABLE) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_CORRECTTABLE_START]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"CORRECTIONTABLE %d ;\n", num);
  else
     fprintf(lefwFile, "CORRECTIONTABLE %d ;\n", num);
 
  lefwLines++;
  lefwState = LEFW_CORRECTTABLE_START;
  lefwIsCorrectTable = 1;
  lefwIsEdgerate = 0;
  lefwSynArray[LEFW_CORRECTTABLE_START] = 1;
  return LEFW_OK;
}


int lefwEndCorrectTable() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_CORRECTTABLE_START &&
      lefwState != LEFW_CORRECTTABLE) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END CORRECTIONTABLE\n\n");
  else
     fprintf(lefwFile, "END CORRECTIONTABLE\n\n");
  lefwLines++;
 
  lefwState = LEFW_CORRECTTABLE_END;
  lefwIsNoiseTable = 0;
  return LEFW_OK;
}


int lefwMinFeature (double x, double y) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_MINFEATURE]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"MINFEATURE %g %g ;\n", x, y);
  else
     fprintf(lefwFile, "MINFEATURE %g %g ;\n", x, y);
  lefwSynArray[LEFW_MINFEATURE] = 1;
  return LEFW_OK; 
}


/*********************
 * int lefwDielectric (float dielectric) {
 *   if (! lefwFile) return LEFW_UNINITIALIZED;
 *   if (! lefwDidInit) return LEFW_BAD_ORDER;
 *   if (lefwState != LEFW_INIT && lefwState < LEFW_END)
 *      return LEFW_BAD_ORDER;
 *   if (lefwSynArray[LEFW_DIELECTRIC]) return LEFW_ALREADY_DEFINED;
 *   fprintf(lefwFile, "DIELECTRIC %g ;\n", dielectric);
 *   lefwSynArray[LEFW_DIELECTRIC] = 1;
 *   return LEFW_OK; 
 * }
 *********************/


int lefwStartIrdrop() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_IRDROP_START ||
     lefwState == LEFW_IRDROP) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (lefwSynArray[LEFW_IRDROP_START]) return LEFW_ALREADY_DEFINED;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"IRDROP\n");
  else
     fprintf(lefwFile, "IRDROP\n");
 
  lefwLines++;
  lefwState = LEFW_IRDROP_START;
  lefwSynArray[LEFW_IRDROP_START] = 1;
  return LEFW_OK;
}


int lefwIrdropTable(const char* tableName, const char* currentsNvolts) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_IRDROP_START &&
     lefwState != LEFW_IRDROP) return LEFW_BAD_ORDER;
  if (!tableName || tableName == 0 || *tableName == 0) return LEFW_BAD_DATA;
  if (!currentsNvolts || currentsNvolts == 0 || *currentsNvolts == 0)
     return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   TABLE %s %s ;\n", tableName, currentsNvolts);
  else
     fprintf(lefwFile, "   TABLE %s %s ;\n", tableName, currentsNvolts);
  lefwLines++;
 
  lefwState = LEFW_IRDROP;
  return LEFW_OK;
}


int lefwEndIrdrop() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_IRDROP_START &&
      lefwState != LEFW_IRDROP) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END IRDROP\n\n");
  else
     fprintf(lefwFile, "END IRDROP\n\n");
  lefwLines++;
 
  lefwState = LEFW_IRDROP_END;
  return LEFW_OK;
}


int lefwSite(const char* siteName, const char* classType, const char* symmetry,
             double width, double height) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (!siteName || siteName == 0 || *siteName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"SITE %s\n", siteName);
     if (classType && strcmp(classType, "PAD") && strcmp(classType, "CORE"))
        return LEFW_BAD_DATA;
     encPrint(lefwFile, (char*)"   CLASS %s ;\n", classType);
     if (symmetry)
        encPrint(lefwFile, (char*)"   SYMMETRY %s ;\n", symmetry);
     encPrint(lefwFile, (char*)"   SIZE %g BY %g ;\n", width, height);
     encPrint(lefwFile, (char*)"END %s\n\n", siteName);
  } else {
     fprintf(lefwFile, "SITE %s\n", siteName);
     if (classType && strcmp(classType, "PAD") && strcmp(classType, "CORE"))
        return LEFW_BAD_DATA;
     fprintf(lefwFile, "   CLASS %s ;\n", classType);
     if (symmetry)
        fprintf(lefwFile, "   SYMMETRY %s ;\n", symmetry);
     fprintf(lefwFile, "   SIZE %g BY %g ;\n", width, height);
     fprintf(lefwFile, "END %s\n\n", siteName);
  }
 
  lefwLines = lefwLines + 5;
  lefwSynArray[LEFW_SITE] = 1;
  return LEFW_OK;
}


int lefwStartArray(const char* arrayName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_ARRAY_START ||
      lefwState == LEFW_ARRAY) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (!arrayName || arrayName == 0 || *arrayName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"ARRAY %s ;\n", arrayName);
  else
     fprintf(lefwFile, "ARRAY %s ;\n", arrayName);
 
  lefwLines++;
  lefwState = LEFW_ARRAY_START;
  lefwSynArray[LEFW_ARRAY_START] = 1;
  lefwHasArrayReq = 0;
  lefwIsArrayFloorp = 0;
  return LEFW_OK;
}


int lefwArraySite(const char* name, double origX, double origY, int orient,
                  double numX, double numY, double spaceX, double spaceY) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_ARRAY_START &&
      lefwState != LEFW_ARRAY) return LEFW_BAD_ORDER;
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   SITE %s %g %g %s DO %g BY %g STEP %g %g ;\n",
          name, origX, origY, lefwOrient(orient), numX, numY, spaceX, spaceY);
  else
     fprintf(lefwFile, "   SITE %s %g %g %s DO %g BY %g STEP %g %g ;\n",
          name, origX, origY, lefwOrient(orient), numX, numY, spaceX, spaceY);

  lefwLines++;
  lefwHasArrayReq = 1;
 
  lefwState = LEFW_ARRAY;
  return LEFW_OK;
}


int lefwArrayCanplace(const char* name, double origX, double origY, int orient,
                      double numX, double numY, double spaceX, double spaceY) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_ARRAY_START &&
      lefwState != LEFW_ARRAY) return LEFW_BAD_ORDER;
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   CANPLACE %s %g %g %s DO %g BY %g STEP %g %g ;\n",
          name, origX, origY, lefwOrient(orient), numX, numY, spaceX, spaceY);
  else
     fprintf(lefwFile, "   CANPLACE %s %g %g %s DO %g BY %g STEP %g %g ;\n",
          name, origX, origY, lefwOrient(orient), numX, numY, spaceX, spaceY);

  lefwLines++;
  lefwHasArrayReq = 1;
 
  lefwState = LEFW_ARRAY;
  return LEFW_OK;
}


int lefwArrayCannotoccupy(const char* name, double origX, double origY,
                          int orient, double numX, double numY, double spaceX,
                          double spaceY) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_ARRAY_START &&
      lefwState != LEFW_ARRAY) return LEFW_BAD_ORDER;
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   CANNOTOCCUPY %s %g %g %s DO %g BY %g STEP %g %g ;\n",
          name, origX, origY, lefwOrient(orient), numX, numY, spaceX, spaceY);
  else
     fprintf(lefwFile, "   CANNOTOCCUPY %s %g %g %s DO %g BY %g STEP %g %g ;\n",
          name, origX, origY, lefwOrient(orient), numX, numY, spaceX, spaceY);

  lefwLines++;
  lefwHasArrayReq = 1;
 
  lefwState = LEFW_ARRAY;
  return LEFW_OK;
}


int lefwArrayTracks(const char* xy, double start, int numTracks, double space,
                    const char* layers) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_ARRAY_START &&
      lefwState != LEFW_ARRAY) return LEFW_BAD_ORDER;
  if (!xy || xy == 0 || *xy == 0) return LEFW_BAD_DATA;
  if (strcmp(xy, "X") && strcmp(xy, "Y")) return LEFW_BAD_DATA;
  if (!layers || layers == 0 || *layers == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   TRACKS %s %g DO %d STEP %g LAYER %s ;\n",
          xy, start, numTracks, space, layers);
  else
     fprintf(lefwFile, "   TRACKS %s %g DO %d STEP %g LAYER %s ;\n",
          xy, start, numTracks, space, layers);

  lefwLines++;
  lefwHasArrayReq = 1;
 
  lefwState = LEFW_ARRAY;
  return LEFW_OK;
}


int lefwStartArrayFloorplan(const char* name) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_ARRAY_START &&
      lefwState != LEFW_ARRAY) return LEFW_BAD_ORDER; 
  if (lefwIsArrayFloorp) return LEFW_BAD_ORDER;
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   FLOORPLAN %s\n", name);
  else
     fprintf(lefwFile, "   FLOORPLAN %s\n", name);

  lefwLines++;
  lefwHasArrayReq = 1;
  lefwIsArrayFloorp = 1;
  return LEFW_OK;
}


int lefwArrayFloorplan(const char* site, const char* name,
                       double origX, double origY, int orient, double numX,
                       double numY, double spaceX, double spaceY) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwIsArrayFloorp) return LEFW_BAD_ORDER;
  if (site && strcmp(site, "CANPLACE") && strcmp(site, "CANNOTOCCUPY"))
     return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      %s %s %g %g %s DO %g BY %g STEP %g %g;\n", site,
          name, origX, origY, lefwOrient(orient), numX, numY, spaceX, spaceY);
  else
     fprintf(lefwFile, "      %s %s %g %g %s DO %g BY %g STEP %g %g;\n", site,
          name, origX, origY, lefwOrient(orient), numX, numY, spaceX, spaceY);
  lefwLines++;
  return LEFW_OK;
}


int lefwEndArrayFloorplan(const char* name) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwIsArrayFloorp) return LEFW_BAD_ORDER;
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   END %s\n\n", name);
  else
     fprintf(lefwFile, "   END %s\n\n", name);

  lefwLines++;
  lefwIsArrayFloorp = 0;
  return LEFW_OK;
}


int lefwArrayGcellgrid(const char * xy, double startXY, int colRows,
                       double spaceXY) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_ARRAY_START &&
      lefwState != LEFW_ARRAY) return LEFW_BAD_ORDER;
  if (!xy || xy == 0 || *xy == 0) return LEFW_BAD_DATA;
  if (strcmp(xy, "X") && strcmp(xy, "Y")) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   GCELLGRID %s %g DO %d STEP %g ;\n", xy,
             startXY, colRows, spaceXY);
  else
     fprintf(lefwFile, "   GCELLGRID %s %g DO %d STEP %g ;\n", xy,
             startXY, colRows, spaceXY);

  lefwLines++;
  lefwHasArrayReq = 1;
 
  lefwState = LEFW_ARRAY;
  return LEFW_OK;
}


int lefwStartArrayDefaultCap(int size) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_ARRAY_START &&
     lefwState != LEFW_ARRAY) return LEFW_BAD_ORDER; 
  if (lefwIsArrayDef) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   DEFAULTCAP %d\n", size);
  else
     fprintf(lefwFile, "   DEFAULTCAP %d\n", size);

  lefwLines++;
  lefwIsArrayDef = 1;
  return LEFW_OK;
}


int lefwArrayDefaultCap(double numPins, double cap) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwIsArrayDef) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      MINPINS %g WIRECAP %g ;\n", numPins, cap);
  else
     fprintf(lefwFile, "      MINPINS %g WIRECAP %g ;\n", numPins, cap);
  lefwLines++;
  return LEFW_OK;
}


int lefwEndArrayDefaultCap() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwIsArrayDef) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   END DEFAULTCAP\n\n");
  else
     fprintf(lefwFile, "   END DEFAULTCAP\n\n");

  lefwLines++;
  lefwIsArrayDef = 0;
  return LEFW_OK;
}


int lefwEndArray(const char* arrayName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_ARRAY_START &&
     lefwState != LEFW_ARRAY) return LEFW_BAD_ORDER;
  if (!arrayName || arrayName == 0 || *arrayName == 0) return LEFW_BAD_DATA;
  if (lefwIsFloorp || lefwIsArrayDef) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END %s\n\n", arrayName);
  else
     fprintf(lefwFile, "END %s\n\n", arrayName);

  lefwLines++;
  lefwState = LEFW_ARRAY_END;
  return LEFW_OK;
}


int lefwStartMacro(const char* macroName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState == LEFW_MACRO_START ||
     lefwState == LEFW_MACRO) return LEFW_BAD_ORDER; 
  if (!macroName || macroName == 0 || *macroName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"MACRO %s\n", macroName);
  else
     fprintf(lefwFile, "MACRO %s\n", macroName);

  lefwLines++;
  lefwState = LEFW_MACRO_START;
  lefwIsMacroPin = 0;
  lefwIsMacroObs = 0;
  lefwIsMacroTiming = 0;
  return LEFW_OK;
}


int lefwMacroClass(const char* value1, const char* value2) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (!value1 || value1 == 0 || *value1 == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     if (strcmp(value1, "COVER") == 0 || strcmp(value1, "RING") == 0 ||
         strcmp(value1, "BLOCK") == 0)
        encPrint(lefwFile, (char*)"   CLASS %s ;\n", value1);
     else if (strcmp(value1, "PAD") == 0) {
        if (value2) {
           if (strcmp(value2, "INPUT") && strcmp(value2, "OUTPUT") &&
              strcmp(value2, "INOUT") && strcmp(value2, "POWER") &&
              strcmp(value2, "SPACER")) return LEFW_BAD_DATA;
           encPrint(lefwFile, (char*)"   CLASS %s %s ;\n", value1, value2);
        } else
           encPrint(lefwFile, (char*)"   CLASS %s ;\n", value1);
     } else if (strcmp(value1, "CORE") == 0) {
        if (value2) {
           if (strcmp(value2, "FEEDTRHU") && strcmp(value2, "TIEHIGH") &&
              strcmp(value2, "TIELOW")) return LEFW_BAD_DATA;
           encPrint(lefwFile, (char*)"   CLASS %s %s ;\n", value1, value2);
        } else
           encPrint(lefwFile, (char*)"   CLASS %s ;\n", value1);
     } else if (strcmp(value1, "ENDCAP") == 0) {
        if (value2) {
           if (strcmp(value2, "PRE") && strcmp(value2, "POST") &&
              strcmp(value2, "TOPLEFT") && strcmp(value2, "TOPRIGHT") &&
              strcmp(value2, "BOTTOMLEFT") && strcmp(value2, "BOTTOMRIGHT"))
              return LEFW_BAD_DATA;
           encPrint(lefwFile, (char*)"   CLASS %s %s ;\n", value1, value2);
        } else
           return LEFW_BAD_DATA;   // require 2nd value
     } else
        return LEFW_BAD_DATA;
  } else {
     if (strcmp(value1, "COVER") == 0 || strcmp(value1, "RING") == 0 ||
         strcmp(value1, "BLOCK") == 0)
        fprintf(lefwFile, "   CLASS %s ;\n", value1);
     else if (strcmp(value1, "PAD") == 0) {
        if (value2) {
           if (strcmp(value2, "INPUT") && strcmp(value2, "OUTPUT") &&
              strcmp(value2, "INOUT") && strcmp(value2, "POWER") &&
              strcmp(value2, "SPACER")) return LEFW_BAD_DATA;
           fprintf(lefwFile, "   CLASS %s %s ;\n", value1, value2);
        } else
           fprintf(lefwFile, "   CLASS %s ;\n", value1);
     } else if (strcmp(value1, "CORE") == 0) {
        if (value2) {
           if (strcmp(value2, "FEEDTRHU") && strcmp(value2, "TIEHIGH") &&
              strcmp(value2, "TIELOW")) return LEFW_BAD_DATA;
           fprintf(lefwFile, "   CLASS %s %s ;\n", value1, value2);
        } else
           fprintf(lefwFile, "   CLASS %s ;\n", value1);
     } else if (strcmp(value1, "ENDCAP") == 0) {
        if (value2) {
           if (strcmp(value2, "PRE") && strcmp(value2, "POST") &&
              strcmp(value2, "TOPLEFT") && strcmp(value2, "TOPRIGHT") &&
              strcmp(value2, "BOTTOMLEFT") && strcmp(value2, "BOTTOMRIGHT"))
              return LEFW_BAD_DATA;
           fprintf(lefwFile, "   CLASS %s %s ;\n", value1, value2);
        } else
           return LEFW_BAD_DATA;   // require 2nd value
     } else
        return LEFW_BAD_DATA;
  }

  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroSource(const char* value1) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (!value1 || value1 == 0 || *value1 == 0) return LEFW_BAD_DATA;
  if (strcmp(value1, "USER") == 0 || strcmp(value1, "GENERATE") == 0 ||
      strcmp(value1, "BLOCK") == 0)
     if (lefwWriteEncrypt) 
        encPrint(lefwFile, (char*)"   SOURCE %s ;\n", value1);
     else
        fprintf(lefwFile, "   SOURCE %s ;\n", value1);
  else
     return LEFW_BAD_DATA;

  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroForeign(const char* name,
                     double xl, double yl, int orient) {    /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"   FOREIGN %s ", name);
     if (xl || yl) {
         encPrint(lefwFile, (char*)"%g %g ", xl, yl);
         if (orient >= 0 && orient <= 7)
            encPrint(lefwFile, (char*)"%s ", lefwOrient(orient));
     } else if (orient > 0 && orient <= 7) {
         encPrint(lefwFile, (char*)";\n");
         return LEFW_BAD_ORDER;    // orient should be inside pt
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "   FOREIGN %s ", name);
     if (xl || yl) {
         fprintf(lefwFile, "%g %g ", xl, yl);
         if (orient >= 0 && orient <= 7)
            fprintf(lefwFile, "%s ", lefwOrient(orient));
     } else if (orient > 0 && orient <= 7) {
         fprintf(lefwFile, ";\n");
         return LEFW_BAD_ORDER;    // orient should be inside pt
     }
     fprintf(lefwFile, ";\n");
  }

  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroOrigin(double xl, double yl) {    /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   ORIGIN %g %g ;\n", xl, yl);
  else
     fprintf(lefwFile, "   ORIGIN %g %g ;\n", xl, yl);

  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroEEQ(const char* macroName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (!macroName || macroName == 0 || *macroName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   EEQ %s ;\n", macroName);
  else
     fprintf(lefwFile, "   EEQ %s ;\n", macroName);

  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroLEQ(const char* macroName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (!macroName || macroName == 0 || *macroName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   LEQ %s ;\n", macroName);
  else
     fprintf(lefwFile, "   LEQ %s ;\n", macroName);

  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroSize(double width, double height) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   SIZE %g BY %g ;\n", width, height);
  else
     fprintf(lefwFile, "   SIZE %g BY %g ;\n", width, height);

  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroSymmetry(const char* symmetry) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (!symmetry || symmetry == 0 || *symmetry == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   SYMMETRY %s ;\n", symmetry);
  else
     fprintf(lefwFile, "   SYMMETRY %s ;\n", symmetry);

  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroSite(const char* siteName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (!siteName || siteName == 0 || *siteName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   SITE %s ;\n", siteName);
  else
     fprintf(lefwFile, "   SITE %s ;\n", siteName);

  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroSitePattern(const char* name, double origX, double origY,
                         int orient, int numX, int numY, double spaceX,
                         double spaceY) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   SITE %s %g %g %s DO %d BY %d STEP %g %g ;\n",
          name, origX, origY, lefwOrient(orient), numX, numY, spaceX, spaceY);
  else
     fprintf(lefwFile, "   SITE %s %g %g %s DO %d BY %d STEP %g %g ;\n",
          name, origX, origY, lefwOrient(orient), numX, numY, spaceX, spaceY);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPower(double power) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   POWER %g ;\n", power);
  else
     fprintf(lefwFile, "   POWER %g ;\n", power);

  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwEndMacro(const char* macroName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_MACRO_START &&
      lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (!macroName || macroName == 0 || *macroName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"END %s\n\n", macroName);
  else
     fprintf(lefwFile, "END %s\n\n", macroName);

  lefwLines++;
  lefwState = LEFW_MACRO_END;
  return LEFW_OK;
}


int lefwStartMacroPin(const char* pinName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (!pinName || pinName == 0 || *pinName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   PIN %s\n", pinName);
  else
     fprintf(lefwFile, "   PIN %s\n", pinName);
 
  lefwIsMacroPin = 1;
  lefwIsMacroPinPort = 0;    // port within macropin
  lefwLines++;
  return LEFW_OK;
}
 
 
int lefwMacroPinTaperRule(const char* ruleName) { /* opitonal */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (!ruleName || ruleName == 0 || *ruleName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      TAPERRULE %s ;\n", ruleName);
  else
     fprintf(lefwFile, "      TAPERRULE %s ;\n", ruleName);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinForeign(const char* name,
                        double xl, double yl, int orient) { /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      FOREIGN %s ", name);
     if (xl || yl) {
         encPrint(lefwFile, (char*)"STRUCTURE %g %g ", xl, yl);
         if (orient >= 0 && orient <= 7)
            encPrint(lefwFile, (char*)"%s ", lefwOrient(orient));
     } else if (orient > 0 && orient <= 7) {
         encPrint(lefwFile, (char*)";\n");
         return LEFW_BAD_ORDER;    // orient should be inside pt
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      FOREIGN %s ", name);
     if (xl || yl) {
         fprintf(lefwFile, "STRUCTURE %g %g ", xl, yl);
         if (orient >= 0 && orient <= 7)
            fprintf(lefwFile, "%s ", lefwOrient(orient));
     } else if (orient > 0 && orient <= 7) {
         fprintf(lefwFile, ";\n");
         return LEFW_BAD_ORDER;    // orient should be inside pt
     }
     fprintf(lefwFile, ";\n");
  }
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}
 
 
int lefwMacroPinLEQ(const char* pinName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (!pinName || pinName == 0 || *pinName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      LEQ %s ;\n", pinName);
  else
     fprintf(lefwFile, "      LEQ %s ;\n", pinName);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinDirection(const char* direction) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (!direction || direction == 0 || *direction == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      DIRECTION %s ;\n", direction);
  else
     fprintf(lefwFile, "      DIRECTION %s ;\n", direction);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinUse(const char* use) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (!use || use == 0 || *use == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      USE %s ;\n", use);
  else
     fprintf(lefwFile, "      USE %s ;\n", use);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinShape(const char* name) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt) {
     if (!name || name == 0 || *name == 0)
        encPrint(lefwFile, (char*)"      SHAPE ;\n");
     else
        encPrint(lefwFile, (char*)"      SHAPE %s ;\n", name);
  } else {
     if (!name || name == 0 || *name == 0)
        fprintf(lefwFile, "      SHAPE ;\n");
     else
        fprintf(lefwFile, "      SHAPE %s ;\n", name);
  }
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinMustjoin(const char* name) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA; 
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      MUSTJOIN %s ;\n", name);
  else
     fprintf(lefwFile, "      MUSTJOIN %s ;\n", name);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinOutputnoisemargin(int high, int low) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      OUTPUTNOISEMARGIN %d %d ;\n", high, low);
  else
     fprintf(lefwFile, "      OUTPUTNOISEMARGIN %d %d ;\n", high, low);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinOutputresistance(int high, int low) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      OUTPUTRESISTANCE %d %d ;\n", high, low);
  else
     fprintf(lefwFile, "      OUTPUTRESISTANCE %d %d ;\n", high, low);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinInputnoisemargin(int high, int low) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      INPUTNOISEMARGIN %d %d ;\n", high, low);
  else
     fprintf(lefwFile, "      INPUTNOISEMARGIN %d %d ;\n", high, low);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinPower(double power) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      POWER %g ;\n", power);
  else
     fprintf(lefwFile, "      POWER %g ;\n", power);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinLeakage(double leakage) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      LEAKAGE %g ;\n", leakage);
  else
     fprintf(lefwFile, "      LEAKAGE %g ;\n", leakage);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinCapacitance(double capacitance) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      CAPACITANCE %g ;\n", capacitance);
  else
     fprintf(lefwFile, "      CAPACITANCE %g ;\n", capacitance);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinResistance(double resistance) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      RESISTANCE %g ;\n", resistance);
  else
     fprintf(lefwFile, "      RESISTANCE %g ;\n", resistance);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinPulldownres(double resistance) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      PULLDOWNRES %g ;\n", resistance);
  else
     fprintf(lefwFile, "      PULLDOWNRES %g ;\n", resistance);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinTieoffr(double resistance) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      TIEOFFR %g ;\n", resistance);
  else
     fprintf(lefwFile, "      TIEOFFR %g ;\n", resistance);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinVHI(double voltage) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      VHI %g ;\n", voltage);
  else
     fprintf(lefwFile, "      VHI %g ;\n", voltage);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}
 

int lefwMacroPinVLO(double voltage) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      VLO %g ;\n", voltage);
  else
     fprintf(lefwFile, "      VLO %g ;\n", voltage);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}
 

int lefwMacroPinRisevoltagethreshold(double voltage) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      RISEVOLTAGETHRESHOLD %g ;\n", voltage);
  else
     fprintf(lefwFile, "      RISEVOLTAGETHRESHOLD %g ;\n", voltage);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}
 

int lefwMacroPinFallvoltagethreshold(double voltage) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      FALLVOLTAGETHRESHOLD %g ;\n", voltage);
  else
     fprintf(lefwFile, "      FALLVOLTAGETHRESHOLD %g ;\n", voltage);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinRisethresh(double capacitance) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      RISETHRESH %g ;\n", capacitance);
  else
     fprintf(lefwFile, "      RISETHRESH %g ;\n", capacitance);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinFallthresh(double capacitance) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      FALLTHRESH %g ;\n", capacitance);
  else
     fprintf(lefwFile, "      FALLTHRESH %g ;\n", capacitance);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinRisesatcur(double current) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      RISESATCUR %g ;\n", current);
  else
     fprintf(lefwFile, "      RISESATCUR %g ;\n", current);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}


int lefwMacroPinFallsatcur(double current) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      FALLSATCUR %g ;\n", current);
  else
     fprintf(lefwFile, "      FALLSATCUR %g ;\n", current);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}
 

int lefwMacroPinCurrentsource(const char* name) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA;
  if (strcmp(name, "ACTIVE") && strcmp(name, "RESISTIVE")) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      CURRENTSOURCE %s ;\n", name);
  else
     fprintf(lefwFile, "      CURRENTSOURCE %s ;\n", name);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}
 
 
int lefwMacroPinIV_Tables(const char* lowName, const char* highName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (!lowName || lowName == 0 || *lowName == 0) return LEFW_BAD_DATA;
  if (!highName || highName == 0 || *highName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      IV_TABLES %s %s ;\n", lowName, highName);
  else
     fprintf(lefwFile, "      IV_TABLES %s %s ;\n", lowName, highName);
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}
 

int lefwMacroPinAntennasize(double value, const char* layerName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      ANTENNASIZE %g ", value);
     if (layerName)
        encPrint(lefwFile, (char*)"LAYER %s ", layerName);
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      ANTENNASIZE %g ", value);
     if (layerName)
        fprintf(lefwFile, "LAYER %s ", layerName);
     fprintf(lefwFile, ";\n");
  }
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}
 

int lefwMacroPinAntennaMetalArea(double value, const char* layerName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      ANTENNAMETALAREA %g ", value);
     if (layerName)
        encPrint(lefwFile, (char*)"LAYER %s ", layerName);
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      ANTENNAMETALAREA %g ", value);
     if (layerName)
        fprintf(lefwFile, "LAYER %s ", layerName);
     fprintf(lefwFile, ";\n");
  }
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}
 

int lefwMacroPinAntennaMetalLength(double value, const char* layerName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      ANTENNAMETALLENGTH %g ", value);
     if (layerName)
        encPrint(lefwFile, (char*)"LAYER %s ", layerName);
     encPrint(lefwFile, (char*)";\n");
  } else {
     fprintf(lefwFile, "      ANTENNAMETALLENGTH %g ", value);
     if (layerName)
        fprintf(lefwFile, "LAYER %s ", layerName);
     fprintf(lefwFile, ";\n");
  }
 
  lefwLines++;
  lefwState = LEFW_MACRO;
  return LEFW_OK;
}
 

int lefwEndMacroPin(const char* pinName) {
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (!pinName || pinName == 0 || *pinName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   END %s\n\n", pinName);
  else
     fprintf(lefwFile, "   END %s\n\n", pinName);
 
  lefwLines++;
  lefwIsMacroPin = 0;
  return LEFW_OK;
}
 

int lefwStartMacroPinPort(const char* classType) {    /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPin) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt) {
     if (classType) {
        if (strcmp(classType, "NONE") && strcmp(classType, "CORE"))
           return LEFW_BAD_DATA;
        encPrint(lefwFile, (char*)"      PORT CLASS %s\n", classType);
     } else
        encPrint(lefwFile, (char*)"      PORT\n");
  } else {
     if (classType) {
        if (strcmp(classType, "NONE") && strcmp(classType, "CORE"))
           return LEFW_BAD_DATA;
        fprintf(lefwFile, "      PORT CLASS %s\n", classType);
     } else
        fprintf(lefwFile, "      PORT\n");
  }

  lefwLines++;
  lefwIsMacroPinPort = 1;
  lefwIsMacroPinPortLayer = 0;
  return LEFW_OK;
}

 
int lefwMacroPinPortLayer(const char* layerName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPinPort) return LEFW_BAD_ORDER;
  if (!layerName || layerName == 0 || *layerName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"         LAYER %s ;\n", layerName);
  else
     fprintf(lefwFile, "         LAYER %s ;\n", layerName);
  lefwLines++;
  lefwIsMacroPinPortLayer = 1;
  return LEFW_OK;
}

 
int lefwMacroPinPortLayerWidth(double width) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPinPortLayer) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"         WIDTH %g ;\n", width);
  else
     fprintf(lefwFile, "         WIDTH %g ;\n", width);
  lefwLines++;
  lefwIsMacroPinPortLayer = 1;
  return LEFW_OK;
}

 
int lefwMacroPinPortLayerPath(int num_paths, double* xl, double* yl,
                              int numX, int numY,       /* optional */
                              double spaceX, double spaceY) { /* optional */
  int i, numItem;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPinPortLayer) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt) {
     if (numX || numY || spaceX || spaceY) {
        encPrint(lefwFile, (char*)"         PATH ITERATE ");
        for (i = 0; i < num_paths; i++) {
           if (i == 0)
              encPrint(lefwFile, (char*)"%g %g\n", xl[i], yl[i]);
           else
              encPrint(lefwFile, (char*)"           %g %g\n", xl[i], yl[i]);
           lefwLines++;
        }
        encPrint(lefwFile, (char*)
           "           DO %d BY %d STEP %g %g ", numX, numY, spaceX, spaceY);
     } else {
        encPrint(lefwFile, (char*)"         PATH ");
        numItem = 0;
        for (i = 0; i < num_paths; i++) {
           if (numItem > 5) {
              encPrint(lefwFile, (char*)"\n");
              encPrint(lefwFile, (char*)"           %g %g ", xl[i], yl[i]);
              numItem = 0;
              lefwLines++; 
           }
           else encPrint(lefwFile, (char*)"%g %g ", xl[i], yl[i]);
           numItem++;
        }
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     if (numX || numY || spaceX || spaceY) {
        fprintf(lefwFile, "         PATH ITERATE ");
        for (i = 0; i < num_paths; i++) {
           if (i == 0)
              fprintf(lefwFile, "%g %g\n", xl[i], yl[i]);
           else
              fprintf(lefwFile, "           %g %g\n", xl[i], yl[i]);
           lefwLines++;
        }
        fprintf(lefwFile,
           "           DO %d BY %d STEP %g %g ", numX, numY, spaceX, spaceY);
     } else {
        fprintf(lefwFile, "         PATH ");
        numItem = 0;
        for (i = 0; i < num_paths; i++) {
           if (numItem > 5) {
              fprintf(lefwFile, "\n");
              fprintf(lefwFile, "           %g %g ", xl[i], yl[i]);
              numItem = 0;
              lefwLines++; 
           }
           else fprintf(lefwFile, "%g %g ", xl[i], yl[i]);
           numItem++;
        }
     }
     fprintf(lefwFile, ";\n");
  }

  lefwLines++;
  return LEFW_OK;
}

 
int lefwMacroPinPortLayerRect(double xl1, double yl1,
                              double xl2, double yl2,
                              int numX, int numY,             /* optional */
                              double spaceX, double spaceY) { /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPinPortLayer) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt) {
     if (numX || numY || spaceX || spaceY) {
        encPrint(lefwFile, (char*)"            RECT ITERATE %g %g %g %g ",
                 xl1, yl1, xl2, yl2);
        encPrint(lefwFile, (char*)"DO %d BY %d STEP %g %g ", numX, numY, spaceX,
                 spaceY);
     } else {
        encPrint(lefwFile, (char*)"            RECT %g %g %g %g ", xl1, yl1, xl2, yl2);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     if (numX || numY || spaceX || spaceY) {
        fprintf(lefwFile, "            RECT ITERATE %g %g %g %g ",
                xl1, yl1, xl2, yl2);
        fprintf(lefwFile, "DO %d BY %d STEP %g %g ", numX, numY, spaceX,
                spaceY);
     } else {
        fprintf(lefwFile, "            RECT %g %g %g %g ", xl1, yl1, xl2, yl2);
     }
     fprintf(lefwFile, ";\n");
  }

  lefwLines++;
  return LEFW_OK;
}

 
int lefwMacroPinPortLayerPolygon(int num_polys, double* xl, double* yl,
                                 int numX, int numY,             /* optional */
                                 double spaceX, double spaceY) { /* optional */
  int i;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPinPortLayer) return LEFW_BAD_ORDER;
  if (num_polys < 4) return LEFW_BAD_DATA;   // at least 4 points
  if (lefwWriteEncrypt) {
     if (numX || numY || spaceX || spaceY) {
        encPrint(lefwFile, (char*)"            POLYGON ITERATE ");
        for (i = 0; i < num_polys; i++) {
           if (i == 0)
              encPrint(lefwFile, (char*)"%g %g\n", *xl++, *yl++);
           else
              encPrint(lefwFile, (char*)"               %g %g\n", *xl++, *yl++);
           lefwLines++; 
        }
        encPrint(lefwFile, (char*)
           "               DO %d BY %d STEP %g %g ;\n", numX, numY, spaceX,
           spaceY);
     } else {
        encPrint(lefwFile, (char*)"            POLYGON ");
        for (i = 0; i < num_polys; i++) {
           if (i == 0)
              encPrint(lefwFile, (char*)"%g %g\n", *xl++, *yl++);
           else if (i+1 == num_polys)
              encPrint(lefwFile, (char*)"               %g %g ;\n", *xl++, *yl++);
           else
              encPrint(lefwFile, (char*)"               %g %g\n", *xl++, *yl++);
           lefwLines++; 
        }
     }
  } else {
     if (numX || numY || spaceX || spaceY) {
        fprintf(lefwFile, "            POLYGON ITERATE ");
        for (i = 0; i < num_polys; i++) {
           if (i == 0)
              fprintf(lefwFile, "%g %g\n", *xl++, *yl++);
           else
              fprintf(lefwFile, "               %g %g\n", *xl++, *yl++);
           lefwLines++; 
        }
        fprintf(lefwFile,
           "               DO %d BY %d STEP %g %g ;\n", numX, numY, spaceX,
           spaceY);
     } else {
        fprintf(lefwFile, "            POLYGON ");
        for (i = 0; i < num_polys; i++) {
           if (i == 0)
              fprintf(lefwFile, "%g %g\n", *xl++, *yl++);
           else if (i+1 == num_polys)
              fprintf(lefwFile, "               %g %g ;\n", *xl++, *yl++);
           else
              fprintf(lefwFile, "               %g %g\n", *xl++, *yl++);
           lefwLines++; 
        }
     }
  }

  lefwLines++;
  return LEFW_OK;
}

 
int lefwMacroPinPortVia(double xl, double yl, const char* viaName,
                        int numX, int numY,             /* optional */
                        double spaceX, double spaceY) { /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroPinPort) return LEFW_BAD_ORDER;
  if (!viaName || viaName == 0 || *viaName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     if (numX || numY || spaceX || spaceY) {
        encPrint(lefwFile, (char*)"            VIA ITERATE %g %g %s ", xl, yl,
                 viaName);
        encPrint(lefwFile, (char*)"DO %d BY %d STEP %g %g ", numX, numY, spaceX,
                 spaceY);
     } else
        encPrint(lefwFile, (char*)"            VIA %g %g %s ", xl, yl, viaName);
     encPrint(lefwFile, (char*)";\n");
  } else {
     if (numX || numY || spaceX || spaceY) {
        fprintf(lefwFile, "            VIA ITERATE %g %g %s ", xl, yl, viaName);
        fprintf(lefwFile, "DO %d BY %d STEP %g %g ", numX, numY, spaceX,
                spaceY);
     } else
        fprintf(lefwFile, "            VIA %g %g %s ", xl, yl, viaName);
     fprintf(lefwFile, ";\n");
  }

  lefwLines++;
  return LEFW_OK;
}

 
int lefwEndMacroPinPort() {
  if (! lefwIsMacroPinPort) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      END\n");
  else
     fprintf(lefwFile, "      END\n");
 
  lefwLines++;
  lefwIsMacroPinPort = 0;
  return LEFW_OK;
}
 

int lefwStartMacroObs() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (lefwIsMacroObs) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   OBS\n");
  else
     fprintf(lefwFile, "   OBS\n");
 
  lefwIsMacroObs = 1;
  lefwLines++;
  return LEFW_OK;
}
 

int lefwMacroObsLayer(const char* layerName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroObs) return LEFW_BAD_ORDER;
  if (!layerName || layerName == 0 || *layerName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      LAYER %s ;\n", layerName);
  else
     fprintf(lefwFile, "      LAYER %s ;\n", layerName);
  lefwLines++;
  lefwIsMacroObsLayer = 1;
  return LEFW_OK;
}
 
 
int lefwMacroObsLayerWidth(double width) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroObsLayer) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      WIDTH %g ;\n", width);
  else
     fprintf(lefwFile, "      WIDTH %g ;\n", width);
  lefwLines++;
  lefwIsMacroObsLayer = 1;
  return LEFW_OK;
}
 
 
int lefwMacroObsLayerPath(int num_paths, double* xl, double* yl,
                          int numX, int numY,             /* optional */
                          double spaceX, double spaceY) { /* optional */
  int i, numItem;
 
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroObsLayer) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt) {
     if (numX || numY || spaceX || spaceY) {
        encPrint(lefwFile, (char*)"         PATH ITERATE ");
        for (i = 0; i < num_paths; i++) {
           if (i == 0)
              encPrint(lefwFile, (char*)"%g %g\n", *xl++, *yl++);
           else
              encPrint(lefwFile, (char*)"            %g %g\n", *xl++, *yl++);
           lefwLines++;
        }
        encPrint(lefwFile, (char*)
           "           DO %d BY %d STEP %g %g ", numX, numY, spaceX, spaceY);
     } else {
        encPrint(lefwFile, (char*)"         PATH ");
        numItem = 0;
        for (i = 0; i < num_paths; i++) {
           if (numItem > 5) {
              encPrint(lefwFile, (char*)"\n");
              encPrint(lefwFile, (char*)"         %g %g ", xl[i], yl[i]);
              numItem = 0;
              lefwLines++; 
           }
           else encPrint(lefwFile, (char*)"%g %g ", xl[i], yl[i]);
           numItem++;
        }
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     if (numX || numY || spaceX || spaceY) {
        fprintf(lefwFile, "         PATH ITERATE ");
        for (i = 0; i < num_paths; i++) {
           if (i == 0)
              fprintf(lefwFile, "%g %g\n", *xl++, *yl++);
           else
              fprintf(lefwFile, "            %g %g\n", *xl++, *yl++);
           lefwLines++;
        }
        fprintf(lefwFile,
           "           DO %d BY %d STEP %g %g ", numX, numY, spaceX, spaceY);
     } else {
        fprintf(lefwFile, "         PATH ");
        numItem = 0;
        for (i = 0; i < num_paths; i++) {
           if (numItem > 5) {
              fprintf(lefwFile, "\n");
              fprintf(lefwFile, "         %g %g ", xl[i], yl[i]);
              numItem = 0;
              lefwLines++; 
           }
           else fprintf(lefwFile, "%g %g ", xl[i], yl[i]);
           numItem++;
        }
     }
     fprintf(lefwFile, ";\n");
  }
 
  lefwLines++;
  return LEFW_OK;
}
 
 
int lefwMacroObsLayerRect(double xl1, double yl1, double xl2, double yl2,
                          int numX, int numY,             /* optional */
                          double spaceX, double spaceY) { /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroObsLayer) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt) {
     if (numX || numY || spaceX || spaceY) {
        encPrint(lefwFile, (char*)"         RECT ITERATE %g %g %g %g ",
                 xl1, yl1, xl2, yl2);
        encPrint(lefwFile, (char*)"DO %d BY %d STEP %g %g ", numX, numY, spaceX,
                 spaceY);
     } else {
        encPrint(lefwFile, (char*)"         RECT %g %g %g %g ", xl1, yl1, xl2, yl2);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     if (numX || numY || spaceX || spaceY) {
        fprintf(lefwFile, "         RECT ITERATE %g %g %g %g ",
                xl1, yl1, xl2, yl2);
        fprintf(lefwFile, "DO %d BY %d STEP %g %g ", numX, numY, spaceX,
                spaceY);
     } else {
        fprintf(lefwFile, "         RECT %g %g %g %g ", xl1, yl1, xl2, yl2);
     }
     fprintf(lefwFile, ";\n");
  }
 
  lefwLines++;
  return LEFW_OK;
}
 
 
int lefwMacroObsLayerPolygon(int num_polys, double* xl, double* yl,
                             int numX, int numY,             /* optional */
                             double spaceX, double spaceY) { /* optional */
  int i;
 
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroObsLayer) return LEFW_BAD_ORDER;
  if (num_polys < 4) return LEFW_BAD_DATA;   // at least 4 points
  if (lefwWriteEncrypt) {
     if (numX || numY || spaceX || spaceY) {
        encPrint(lefwFile, (char*)"         POLYGON ITERATE ");
        for (i = 0; i < num_polys; i++) {
           if (i == 0)
              encPrint(lefwFile, (char*)"%g %g\n", *xl++, *yl++);
           else
              encPrint(lefwFile, (char*)"           %g %g\n", *xl++, *yl++);
           lefwLines++;
        }
        encPrint(lefwFile, (char*)
           "           DO %d BY %d STEP %g %g ;", numX, numY, spaceX, spaceY);
     } else {
        encPrint(lefwFile, (char*)"         POLYGON ");
        for (i = 0; i < num_polys; i++) {
           if (i == 0)
              encPrint(lefwFile, (char*)"%g %g\n", *xl++, *yl++);
           if (i+1 == num_polys)
              encPrint(lefwFile, (char*)"           %g %g ;\n", *xl++, *yl++);
           else
              encPrint(lefwFile, (char*)"           %g %g\n", *xl++, *yl++);
           lefwLines++;
        }
     }
  } else {
     if (numX || numY || spaceX || spaceY) {
        fprintf(lefwFile, "         POLYGON ITERATE ");
        for (i = 0; i < num_polys; i++) {
           if (i == 0)
              fprintf(lefwFile, "%g %g\n", *xl++, *yl++);
           else
              fprintf(lefwFile, "           %g %g\n", *xl++, *yl++);
           lefwLines++;
        }
        fprintf(lefwFile,
           "           DO %d BY %d STEP %g %g ;", numX, numY, spaceX, spaceY);
     } else {
        fprintf(lefwFile, "         POLYGON ");
        for (i = 0; i < num_polys; i++) {
           if (i == 0)
              fprintf(lefwFile, "%g %g\n", *xl++, *yl++);
           if (i+1 == num_polys)
              fprintf(lefwFile, "           %g %g ;\n", *xl++, *yl++);
           else
              fprintf(lefwFile, "           %g %g\n", *xl++, *yl++);
           lefwLines++;
        }
     }
  }
 
  lefwLines++;
  return LEFW_OK;
}
 
 
int lefwMacroObsVia(double xl, double yl, const char* viaName,
                    int numX, int numY,             /* optional */
                    double spaceX, double spaceY) { /* optional */
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroObs) return LEFW_BAD_ORDER;
  if (!viaName || viaName == 0 || *viaName == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     if (numX || numY || spaceX || spaceY) {
        encPrint(lefwFile, (char*)"      VIA ITERATE %g %g %s ", xl, yl, viaName);
        encPrint(lefwFile, (char*)"DO %d BY %d STEP %g %g ", numX, numY, spaceX,
                 spaceY);
     } else {
        encPrint(lefwFile, (char*)"      VIA %g %g %s ", xl, yl, viaName);
     }
     encPrint(lefwFile, (char*)";\n");
  } else {
     if (numX || numY || spaceX || spaceY) {
        fprintf(lefwFile, "      VIA ITERATE %g %g %s ", xl, yl, viaName);
        fprintf(lefwFile, "DO %d BY %d STEP %g %g ", numX, numY, spaceX,
                spaceY);
     } else {
        fprintf(lefwFile, "      VIA %g %g %s ", xl, yl, viaName);
     }
     fprintf(lefwFile, ";\n");
  }
 
  lefwLines++;
  lefwIsMacroObsLayer = 0;
  return LEFW_OK;
}

 
int lefwEndMacroObs() {
  if (! lefwIsMacroObs) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   END\n");
  else
     fprintf(lefwFile, "   END\n");
 
  lefwLines++;
  lefwIsMacroObs = 0;
  return LEFW_OK;
}
 

int lefwStartMacroTiming() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_MACRO_START &&
     lefwState != LEFW_MACRO) return LEFW_BAD_ORDER;
  if (lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   TIMING\n");
  else
     fprintf(lefwFile, "   TIMING\n");
 
  lefwIsMacroTiming = 1;
  lefwIsMacroTimingModel = 0;
  lefwLines++;
  return LEFW_OK;
}
 

int lefwMacroTimingPin(const char* fromPin, const char* toPin) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (fromPin) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"      FROMPIN %s ;\n", fromPin);
     else
        fprintf(lefwFile, "      FROMPIN %s ;\n", fromPin);
     lefwLines++;
  }
  if (toPin) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"      TOPIN %s ;\n", toPin);
     else
        fprintf(lefwFile, "      TOPIN %s ;\n", toPin);
     lefwLines++;
  }
  return LEFW_OK;
}


int lefwMacroTimingIntrinsic(const char* riseFall, double min, double max,
                    double slewT1, double slewT1Min,       /* optional */
                    double slewT1Max, double slewT2,       /* optional */
                    double slewT2Min, double slewT2Max,    /* optional */
                    double slewT3,                         /* optional */
                    double varMin, double varMax) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (strcmp(riseFall, "RISE") && strcmp(riseFall, "FALL"))
     return LEFW_BAD_DATA;
  if (lefwWriteEncrypt) {
     encPrint(lefwFile, (char*)"      %s INTRINSIC %g %g ", riseFall, min, max);
     if (slewT1 || slewT1Min || slewT1Max || slewT2) {
        encPrint(lefwFile, (char*)"%g %g %g %g ", slewT1, slewT1Min, slewT1Max,
                 slewT2);
        if (slewT2Min || slewT2Max || slewT3)
           encPrint(lefwFile, (char*)"%g %g %g \n", slewT2Min, slewT2Max, slewT3);
     } else if (slewT2Min || slewT2Max || slewT3)
        return LEFW_BAD_DATA;     // slewT2Min... has to be inside of slewT1...
     encPrint(lefwFile, (char*)"         VARIABLE %g %g ;\n", varMin, varMax);
  } else {
     fprintf(lefwFile, "      %s INTRINSIC %g %g ", riseFall, min, max);
     if (slewT1 || slewT1Min || slewT1Max || slewT2) {
        fprintf(lefwFile, "%g %g %g %g ", slewT1, slewT1Min, slewT1Max, slewT2);
        if (slewT2Min || slewT2Max || slewT3)
           fprintf(lefwFile, "%g %g %g \n", slewT2Min, slewT2Max, slewT3);
     } else if (slewT2Min || slewT2Max || slewT3)
        return LEFW_BAD_DATA;     // slewT2Min... has to be inside of slewT1...
     fprintf(lefwFile, "         VARIABLE %g %g ;\n", varMin, varMax);
  }
  lefwLines++;
  return LEFW_OK;
}

                             
int lefwMacroTimingRisers(double min, double max) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      RISERS %g %g ;\n", min, max);
  else
     fprintf(lefwFile, "      RISERS %g %g ;\n", min, max);
  lefwLines++;
  return LEFW_OK;
}


int lefwMacroTimingFallrs(double min, double max) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      FALLRS %g %g ;\n", min, max);
  else
     fprintf(lefwFile, "      FALLRS %g %g ;\n", min, max);
  lefwLines++;
  return LEFW_OK;
}


int lefwMacroTimingRisecs(double min, double max) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      RISECS %g %g ;\n", min, max);
  else
     fprintf(lefwFile, "      RISECS %g %g ;\n", min, max);
  lefwLines++;
  return LEFW_OK;
}


int lefwMacroTimingFallcs(double min, double max) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      FALLCS %g %g ;\n", min, max);
  else
     fprintf(lefwFile, "      FALLCS %g %g ;\n", min, max);
  lefwLines++;
  return LEFW_OK;
}


int lefwMacroTimingRisesatt1(double min, double max) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      RISESATT1 %g %g ;\n", min, max);
  else
     fprintf(lefwFile, "      RISESATT1 %g %g ;\n", min, max);
  lefwLines++;
  return LEFW_OK;
}


int lefwMacroTimingFallsatt1(double min, double max) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      FALLSATT1 %g %g ;\n", min, max);
  else
     fprintf(lefwFile, "      FALLSATT1 %g %g ;\n", min, max);
  lefwLines++;
  return LEFW_OK;
}


int lefwMacroTimingRiset0(double min, double max) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      RISET0 %g %g ;\n", min, max);
  else
     fprintf(lefwFile, "      RISET0 %g %g ;\n", min, max);
  lefwLines++;
  return LEFW_OK;
}


int lefwMacroTimingFallt0(double min, double max) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      FALLT0 %g %g ;\n", min, max);
  else
     fprintf(lefwFile, "      FALLT0 %g %g ;\n", min, max);
  lefwLines++;
  return LEFW_OK;
}

int lefwMacroTimingUnateness(const char* unateness) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (strcmp(unateness, "INVERT") && strcmp(unateness, "NONINVERT") &&
      strcmp(unateness, "NONUNATE")) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"      UNATENESS %s ; \n", unateness);
  else
     fprintf(lefwFile, "      UNATENESS %s ; \n", unateness);
  lefwLines++;
  return LEFW_OK;
}

int lefwEndMacroTiming() {
  if (! lefwIsMacroTiming) return LEFW_BAD_ORDER;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   END TIMING\n\n");
  else
     fprintf(lefwFile, "   END TIMING\n\n");
 
  lefwLines++;
  lefwIsMacroTiming = 0;
  return LEFW_OK;
}
 

int lefwAntenna(const char* type, double value) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_INIT && lefwState < LEFW_END)
     return LEFW_BAD_ORDER;  // not of the ENDs
  if (!type || type == 0 || *type == 0) return LEFW_BAD_DATA;
  if (strcmp(type, "INPUTPINANTENNASIZE") &&
      strcmp(type, "OUTPUTPINANTENNASIZE") &&
      strcmp(type, "INOUTPINANTENNASIZE")) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"%s %g ;\n", type, value);
  else
     fprintf(lefwFile, "%s %g ;\n", type, value);
 
  lefwLines++;
  lefwSynArray[LEFW_ANTENNASIZE] = 1;
  return LEFW_OK;
}
 

int lefwStartBeginext(const char* name) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState == LEFW_BEGINEXT_START ||
     lefwState == LEFW_BEGINEXT) return LEFW_BAD_ORDER; 
  if (!name || name == 0 || *name == 0) return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"BEGINEXT \"%s\"", name);  // \n will be added later
  else
     fprintf(lefwFile, "BEGINEXT \"%s\"", name);

  lefwState = LEFW_BEGINEXT_START;
  lefwLines++;
  return LEFW_OK;
}
 

int lefwBeginextCreator(const char* creatorName) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_BEGINEXT_START &&
     lefwState != LEFW_BEGINEXT) return LEFW_BAD_ORDER; 
  if (!creatorName || creatorName == 0 || *creatorName == 0) 
     return LEFW_BAD_DATA;
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"\n   CREATOR \"%s\"", creatorName);
  else
     fprintf(lefwFile, "\n   CREATOR \"%s\"", creatorName);

  lefwState = LEFW_BEGINEXT;
  lefwLines++;
  return LEFW_OK;
}
 

int lefwBeginextDate() {
  time_t todayTime;
  char * rettime ;

  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_BEGINEXT_START &&
     lefwState != LEFW_BEGINEXT) return LEFW_BAD_ORDER; 
 
  todayTime = time(NULL);             // time in UTC 
  rettime = ctime(&todayTime);        // convert to string
  rettime[strlen(rettime)-1] = '\0';  // replace \n with \0
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"\n   DATE \"%s\"", rettime);
  else
     fprintf(lefwFile, "\n   DATE \"%s\"", rettime);

  lefwState = LEFW_BEGINEXT;
  lefwLines++;
  return LEFW_OK;
}
 

int lefwBeginextRevision(int vers1, int vers2) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_BEGINEXT_START &&
     lefwState != LEFW_BEGINEXT) return LEFW_BAD_ORDER; 
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"\n   REVISION %d.%d", vers1, vers2);
  else
     fprintf(lefwFile, "\n   REVISION %d.%d", vers1, vers2);

  lefwState = LEFW_BEGINEXT;
  lefwLines++;
  return LEFW_OK;
}
 

int lefwBeginextSyntax(const char* title, const char* string) {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_BEGINEXT_START &&
     lefwState != LEFW_BEGINEXT) return LEFW_BAD_ORDER; 
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"\n   - %s %s", title, string);
  else
     fprintf(lefwFile, "\n   - %s %s", title, string);

  lefwState = LEFW_BEGINEXT;
  lefwLines++;
  return LEFW_OK;
}
 

int lefwEndBeginext() {
  if (! lefwFile) return LEFW_UNINITIALIZED;
  if (! lefwDidInit) return LEFW_BAD_ORDER;
  if (lefwState != LEFW_BEGINEXT_START &&
     lefwState != LEFW_BEGINEXT) return LEFW_BAD_ORDER; 
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)";\nENDEXT\n\n");
  else
     fprintf(lefwFile, ";\nENDEXT\n\n");

  lefwState = LEFW_BEGINEXT_END;
  lefwLines++;
  return LEFW_OK;
}
 

int lefwCurrentLineNumber() {
  return lefwLines;
}


void lefwPrintError(int status) {
  switch (status) {
     case LEFW_OK:
          if (lefwWriteEncrypt)
             encPrint(lefwFile, (char*)"No Error.\n");
          else
             fprintf(lefwFile, "No Error.\n");
          break;
     case LEFW_UNINITIALIZED:
          if (lefwWriteEncrypt)
             encPrint(lefwFile, (char*)"Need to call lefwInit first.\n");
          else
             fprintf(lefwFile, "Need to call lefwInit first.\n");
          break;
     case LEFW_BAD_ORDER:
          if (lefwWriteEncrypt)
             encPrint(lefwFile, (char*)"Incorrect order of data.\n");
          else
             fprintf(lefwFile, "Incorrect order of data.\n");
          break;
     case LEFW_BAD_DATA:
          if (lefwWriteEncrypt)
             encPrint(lefwFile, (char*)"Invalid data.\n");
          else
             fprintf(lefwFile, "Invalid data.\n");
          break;
     case LEFW_ALREADY_DEFINED:
          if (lefwWriteEncrypt)
             encPrint(lefwFile, (char*)"Section is allowed to define only once.\n");
          else
             fprintf(lefwFile, "Section is allowed to define only once.\n");
          break;
  }
  return;
}

void lefwAddComment(const char* comment) {
  if (comment) {
     if (lefwWriteEncrypt)
        encPrint(lefwFile, (char*)"# %s\n", comment);
     else
        fprintf(lefwFile, "# %s\n", comment);
  }
  return; 
}

void lefwAddIndent() {
  if (lefwWriteEncrypt)
     encPrint(lefwFile, (char*)"   ");
  else
     fprintf(lefwFile,"   ");
  return;
}

/***************************
   Questions:
 - Is only one row rule allowed
 - Is only one tracks rule allowed
 - In the die area is a zero area allowed? overlaps?
 - What type of checking is needed for the rows and tracks do loop?
 - Can you have a default prop with a number AND a range?
 - What is the pin properties section mentioned in the 5.1 spec?
 *****************************/


