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





static int HandleDEFArrayName(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFBusBit(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFCannotOccupy(defrCallbackType_e typ, defiSite* site, defiUserData data);
static int HandleDEFCanplace(defrCallbackType_e typ, defiSite* site, defiUserData data);
static int HandleDEFComponentStart(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFComponent(defrCallbackType_e typ, defiComponent* comp, defiUserData data);
static int HandleDEFComponentEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFConstraintsStart(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFConstraint(defrCallbackType_e c, defiAssertion* a, defiUserData data);
static int HandleDEFConstraintsEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFPath(defrCallbackType_e typ, defiPath* path, defiUserData data);
static int HandleDEFDefaultCap(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFPinCap(defrCallbackType_e typ, defiPinCap* pincap, defiUserData data); 
static int HandleDEFDesign(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFDesignEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFDieArea(defrCallbackType_e typ, defiBox* box, defiUserData data);
static int HandleDEFDivider(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFComponentExt(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFGroupExt(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFIoTimingsExt(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFNetExt(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFNetConnectionExt(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFPartitionsExt(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFPinExt(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFScanChainExt(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFViaExt(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFFloorPlanName(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFGcellGrid(defrCallbackType_e typ, defiGcellGrid* grid, defiUserData data);
static int HandleDEFGroupsStart(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFGroupName(defrCallbackType_e typ, const char* string, defiUserData data); 
static int HandleDEFGroupMember(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFGroup(defrCallbackType_e typ, defiGroup* group, defiUserData data); 
static int HandleDEFGroupsEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFHistory(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFIOTimingsStart(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFIOTiming(defrCallbackType_e typ, defiIOTiming* iot, defiUserData data);
static int HandleDEFIOTimingsEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFCaseSensitive(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFNetStart(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFNet(defrCallbackType_e typ, defiNet* net, defiUserData data);
static int HandleDEFNetEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFStartPins(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFPin(defrCallbackType_e typ, defiPin* pincap, defiUserData data); 
static int HandleDEFPinEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFPinPropStart(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFPinProp(defrCallbackType_e typ, defiPinProp* pp, defiUserData data);
static int HandleDEFPinPropEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFPropDefStart(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFProp(defrCallbackType_e typ, defiProp* prop, defiUserData data);
static int HandleDEFPropDefEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFRegionStart(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFRegion(defrCallbackType_e typ, defiRegion* reg, defiUserData data); 
static int HandleDEFRegionEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFRow(defrCallbackType_e typ, defiRow* row, defiUserData data);
static int HandleDEFScanchainsStart(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFScanchain(defrCallbackType_e typ, defiScanchain* sc, defiUserData data);
static int HandleDEFScanchainsEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFSite(defrCallbackType_e typ, defiSite* site, defiUserData data);
static int HandleDEFSNetStart(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFSNet(defrCallbackType_e typ, defiNet* net, defiUserData data);
static int HandleDEFSNetEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
static int HandleDEFTechnology(defrCallbackType_e typ, const char* string, defiUserData data);
static int HandleDEFTrack(defrCallbackType_e typ, defiTrack* track, defiUserData data);
static int HandleDEFUnits(defrCallbackType_e typ, double variable, defiUserData data);
static int HandleDEFVersion(defrCallbackType_e typ, double variable, defiUserData data);
static int HandleDEFViaStart(defrCallbackType_e typ, int variable, defiUserData data);
static int HandleDEFVia(defrCallbackType_e typ, defiVia* via, defiUserData data); 
static int HandleDEFViaEnd(defrCallbackType_e typ, void* variable, defiUserData data); 
