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





static int HandleLEFVersion(lefrCallbackType_e typ, double variable, lefiUserData data);
static int HandleLEFCaseSensitive(lefrCallbackType_e typ, int variable, lefiUserData data);
static int HandleLEFNoWireExtension(lefrCallbackType_e typ, const char* variable, lefiUserData data);
static int HandleLEFBusBitChars(lefrCallbackType_e typ, const char* variable, lefiUserData data);
static int HandleLEFDividerChar(lefrCallbackType_e typ, const char* variable, lefiUserData data);
static int HandleLEFUnits(lefrCallbackType_e typ, lefiUnits* u, lefiUserData data);
static int HandleLEFPropBegin(lefrCallbackType_e typ, void* ptr, lefiUserData data);
static int HandleLEFProp(lefrCallbackType_e typ, lefiProp* p, lefiUserData data);
static int HandleLEFPropEnd(lefrCallbackType_e typ, void* ptr, lefiUserData data);
static int HandleLEFLayer(lefrCallbackType_e typ, lefiLayer* layer, lefiUserData data);
static int HandleLEFVia(lefrCallbackType_e typ, lefiVia* via, lefiUserData data);
static int HandleLEFViaRule(lefrCallbackType_e typ, lefiViaRule* viaRule, lefiUserData data);
static int HandleLEFNonDefault(lefrCallbackType_e typ, lefiNonDefault* def, lefiUserData data);
static int HandleLEFNoiseMargin(lefrCallbackType_e typ, struct lefiNoiseMargin* p, lefiUserData data);
static int HandleLEFEdgeRateThreshold1(lefrCallbackType_e typ, double variable, lefiUserData data);
static int HandleLEFEdgeRateThreshold2(lefrCallbackType_e typ, double variable, lefiUserData data);
static int HandleLEFEdgeRateScaleFactor(lefrCallbackType_e typ, double variable, lefiUserData data);
static int HandleLEFNoiseTable(lefrCallbackType_e typ, lefiNoiseTable* p, lefiUserData data);
static int HandleLEFCorrectionTable(lefrCallbackType_e typ, lefiCorrectionTable* p, lefiUserData data);
static int HandleLEFSpacingBegin(lefrCallbackType_e typ, void* ptr, lefiUserData data);
static int HandleLEFSpacing(lefrCallbackType_e typ, lefiSpacing* spacing, lefiUserData data);
static int HandleLEFSpacingEnd(lefrCallbackType_e typ, void* ptr, lefiUserData data);
static int HandleLEFMinFeature(lefrCallbackType_e typ, lefiMinFeature* min, lefiUserData data);
static int HandleLEFDielectric(lefrCallbackType_e typ, double variable, lefiUserData data);
static int HandleLEFIRDropBegin(lefrCallbackType_e typ, void* ptr, lefiUserData data);
static int HandleLEFIRDrop(lefrCallbackType_e typ, lefiIRDrop* irdrop, lefiUserData data);
static int HandleLEFIRDropEnd(lefrCallbackType_e typ, void* ptr, lefiUserData data);
static int HandleLEFSite(lefrCallbackType_e typ, lefiSite* site, lefiUserData data);
static int HandleLEFArrayBegin(lefrCallbackType_e typ, const char* variable, lefiUserData data);
static int HandleLEFArray(lefrCallbackType_e typ, lefiArray* array_data, lefiUserData data);
static int HandleLEFArrayEnd(lefrCallbackType_e typ, const char* variable, lefiUserData data);
static int HandleLEFMacroBegin(lefrCallbackType_e typ, const char* variable, lefiUserData data);
static int HandleLEFMacro(lefrCallbackType_e typ, lefiMacro* string, lefiUserData data);
static int HandleLEFPin(lefrCallbackType_e typ, lefiPin* l, lefiUserData data);
static int HandleLEFObstruction(lefrCallbackType_e typ, lefiObstruction* obs, lefiUserData data);
static int HandleLEFTiming(lefrCallbackType_e typ, lefiTiming* p, lefiUserData data);
static int HandleLEFInputAntenna(lefrCallbackType_e typ, double variable, lefiUserData data);
static int HandleLEFOutputAntenna(lefrCallbackType_e typ, double variable, lefiUserData data);
static int HandleLEFInOutAntenna(lefrCallbackType_e typ, double variable, lefiUserData data);
static int HandleLEFLibraryEnd(lefrCallbackType_e typ, void* ptr, lefiUserData data);
