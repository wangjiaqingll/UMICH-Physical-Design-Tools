/*
 * This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 * Distribution,  Product Version 5.3, and is subject to the Cadence LEF/DEF
 * Open Source License Agreement.   Your  continued  use  of this file
 * constitutes your acceptance of the terms of the LEF/DEF Open Source
 * License and an agreement to abide by its  terms.   If you  don't  agree
 * with  this, you must remove this and any other files which are part of the
 * distribution and  destroy any  copies made.
 * 
 * For updates, support, or to become part of the LEF/DEF Community, check
 * www.openeda.org for details.
 */
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "defrReader.h"
#include "defiProp.h"
#include "defrCallBacks.h"
#include "defiDebug.h"
#include "defiUtil.h"
#include "lex.h"
#include "FEF_stringhash.h"
extern int defyyparse(void);	/* do the parsing */

extern void lex_init(void);	/* start the lexer */

extern int names_case_sensitive;

/*
 * From defiDebug.cpp
 */ extern void defrNerr(int num);

extern void NWARN(int num);

extern char defDebug[100];

static char defrErrMsg[100];

void 
defrNerr(int i)
{
  sprintf(defrErrMsg, "ERROR number %d\n", i);
  defiError(defrErrMsg);
  exit(2);
}

void 
defrNwarn(int i)
{
  sprintf(defrErrMsg, "WARNING number %d\n", i);
  defiError(defrErrMsg);
  exit(2);
}

/*
 * Global variables
 *//* NEW CALLBACK - If you had to create a global instance of a class for the
 * parser to add to, then define it here.  */

/*
 * List of call back routines
 *//* These are filled in by the user.  See the *//* "set" routines at the end
 * of the file *//* NEW CALLBACK - Here is the actual location where the
 * address of the callback routines is stored.   Place each callback that you
 * create here.  */

/*
 * Flags to control what happens
 *//* Parser control by the user. *//* Reader initialization */ defiUserData defrUserData = 0;

static char *defrFileName = 0;

FILE   *defrFile = 0;

defiSubnet *defrSubnet = 0;

defiPath *defrPath = 0;

defiProp defrProp;

defiSite defrSite;

defiComponent defrComponent;

defiNet defrNet;

defiPinCap defrPinCap;

defiSite defrCannotOccupy;

defiSite defrCanplace;

defiBox defrDieArea;

defiPin defrPin;

defiRow defrRow;

defiTrack defrTrack;

defiGcellGrid defrGcellGrid;

defiVia defrVia;

defiRegion defrRegion;

defiGroup defrGroup;

defiAssertion defrAssertion;

defiScanchain defrScanchain;

defiIOTiming defrIOTiming;

defiFPC defrFPC;

defiTimingDisable defrTimingDisable;

defiPartition defrPartition;

defiPinProp defrPinProp;

defrStringCbkFnType defrDesignCbk = 0;

defrStringCbkFnType defrTechnologyCbk = 0;

defrVoidCbkFnType defrDesignEndCbk = 0;

defrPropCbkFnType defrPropCbk = 0;

defrVoidCbkFnType defrPropDefEndCbk = 0;

defrVoidCbkFnType defrPropDefStartCbk = 0;

defrStringCbkFnType defrArrayNameCbk = 0;

defrStringCbkFnType defrFloorPlanNameCbk = 0;

defrDoubleCbkFnType defrUnitsCbk = 0;

defrStringCbkFnType defrDividerCbk = 0;

defrStringCbkFnType defrBusBitCbk = 0;

defrSiteCbkFnType defrSiteCbk = 0;

defrSiteCbkFnType defrCanplaceCbk = 0;

defrSiteCbkFnType defrCannotOccupyCbk = 0;

defrIntegerCbkFnType defrComponentStartCbk = 0;

defrVoidCbkFnType defrComponentEndCbk = 0;

defrComponentCbkFnType defrComponentCbk = 0;

defrIntegerCbkFnType defrNetStartCbk = 0;

defrVoidCbkFnType defrNetEndCbk = 0;

defrNetCbkFnType defrNetCbk = 0;

defrPathCbkFnType defrPathCbk = 0;

defrDoubleCbkFnType defrVersionCbk = 0;

defrStringCbkFnType defrPinExtCbk = 0;

defrStringCbkFnType defrComponentExtCbk = 0;

defrStringCbkFnType defrViaExtCbk = 0;

defrStringCbkFnType defrNetConnectionExtCbk = 0;

defrStringCbkFnType defrNetExtCbk = 0;

defrStringCbkFnType defrGroupExtCbk = 0;

defrStringCbkFnType defrScanChainExtCbk = 0;

defrStringCbkFnType defrIoTimingsExtCbk = 0;

defrStringCbkFnType defrPartitionsExtCbk = 0;

defrStringCbkFnType defrHistoryCbk = 0;

defrBoxCbkFnType defrDieAreaCbk = 0;

defrPinCapCbkFnType defrPinCapCbk = 0;

defrPinCbkFnType defrPinCbk = 0;

defrIntegerCbkFnType defrStartPinsCbk = 0;

defrVoidCbkFnType defrPinEndCbk = 0;

defrIntegerCbkFnType defrDefaultCapCbk = 0;

defrRowCbkFnType defrRowCbk = 0;

defrTrackCbkFnType defrTrackCbk = 0;

defrGcellGridCbkFnType defrGcellGridCbk = 0;

defrIntegerCbkFnType defrViaStartCbk = 0;

defrVoidCbkFnType defrViaEndCbk = 0;

defrViaCbkFnType defrViaCbk = 0;

defrIntegerCbkFnType defrRegionStartCbk = 0;

defrVoidCbkFnType defrRegionEndCbk = 0;

defrRegionCbkFnType defrRegionCbk = 0;

defrIntegerCbkFnType defrSNetStartCbk = 0;

defrVoidCbkFnType defrSNetEndCbk = 0;

defrNetCbkFnType defrSNetCbk = 0;

defrIntegerCbkFnType defrGroupsStartCbk = 0;

defrVoidCbkFnType defrGroupsEndCbk = 0;

defrStringCbkFnType defrGroupNameCbk = 0;

defrStringCbkFnType defrGroupMemberCbk = 0;

defrGroupCbkFnType defrGroupCbk = 0;

defrIntegerCbkFnType defrAssertionsStartCbk = 0;

defrVoidCbkFnType defrAssertionsEndCbk = 0;

defrAssertionCbkFnType defrAssertionCbk = 0;

defrIntegerCbkFnType defrConstraintsStartCbk = 0;

defrVoidCbkFnType defrConstraintsEndCbk = 0;

defrAssertionCbkFnType defrConstraintCbk = 0;

defrIntegerCbkFnType defrScanchainsStartCbk = 0;

defrVoidCbkFnType defrScanchainsEndCbk = 0;

defrScanchainCbkFnType defrScanchainCbk = 0;

defrIntegerCbkFnType defrIOTimingsStartCbk = 0;

defrVoidCbkFnType defrIOTimingsEndCbk = 0;

defrIOTimingCbkFnType defrIOTimingCbk = 0;

defrIntegerCbkFnType defrFPCStartCbk = 0;

defrVoidCbkFnType defrFPCEndCbk = 0;

defrFPCCbkFnType defrFPCCbk = 0;

defrIntegerCbkFnType defrTimingDisablesStartCbk = 0;

defrVoidCbkFnType defrTimingDisablesEndCbk = 0;

defrTimingDisableCbkFnType defrTimingDisableCbk = 0;

defrIntegerCbkFnType defrPartitionsStartCbk = 0;

defrVoidCbkFnType defrPartitionsEndCbk = 0;

defrPartitionCbkFnType defrPartitionCbk = 0;

defrIntegerCbkFnType defrPinPropStartCbk = 0;

defrVoidCbkFnType defrPinPropEndCbk = 0;

defrPinPropCbkFnType defrPinPropCbk = 0;

defrIntegerCbkFnType defrCaseSensitiveCbk = 0;

int     defrAddPathToNet = 0;

int     defrNeedPathData = 0;

int     defrAllowComponentNets = 0;

int     defrRegisterUnused = 0;

#define CBMAX 100
int     defrUnusedCallbacks[CBMAX];

int 
defrInit()
{

#ifndef __cplusplus
  defiProp_Init(&(defrProp));
  defiSite_Init(&(defrSite));
  defiComponent_Init(&(defrComponent));
  defiNet_Init(&(defrNet));
  defiSite_Init(&(defrCannotOccupy));
  defiSite_Init(&(defrCanplace));
  defiRow_Init(&(defrRow));
  defiTrack_Init(&(defrTrack));
  defiGcellGrid_Init(&(defrGcellGrid));
  defiVia_Init(&(defrVia));
  defiPin_Init(&(defrPin));
  defiRegion_Init(&(defrRegion));
  defiGroup_Init(&(defrGroup));
  defiAssertion_Init(&(defrAssertion));
  defiScanchain_Init(&(defrScanchain));
  defiIOTiming_Init(&(defrIOTiming));
  defiFPC_Init(&(defrFPC));
  defiPartition_Init(&(defrPartition));
  defiPinProp_Init(&(defrPinProp));
#endif

  return 0;
}

int 
defrCountUnused(defrCallbackType_e e, void *v, defiUserData d)
{
  int     i;

  if (defiDebug(23))
    printf("Count %d, 0x%x, 0x%x\n", (int) e, v, d);
  i = (int) e;
  if (i <= 0 || i >= CBMAX) {
    return 1;
  }
  defrUnusedCallbacks[i] += 1;
  return 0;
}

void 
defrSetRegisterUnusedCallbacks()
{
  int     i;

  defrRegisterUnused = 1;
  for (i = 0;
       i < CBMAX;
       i++)
    defrUnusedCallbacks[i] = 0;
  defrSetUnusedCallbacks(defrCountUnused);
}

void 
defrSetUnusedCallbacks(defrVoidCbkFnType f)
{
  if (!defrDesignCbk)
    defrDesignCbk = (defrStringCbkFnType) f;
  if (!defrTechnologyCbk)
    defrTechnologyCbk = (defrStringCbkFnType) f;
  if (!defrDesignEndCbk)
    defrDesignEndCbk = (defrVoidCbkFnType) f;
  if (!defrPropCbk)
    defrPropCbk = (defrPropCbkFnType) f;
  if (!defrPropDefEndCbk)
    defrPropDefEndCbk = (defrVoidCbkFnType) f;
  if (!defrPropDefStartCbk)
    defrPropDefStartCbk = (defrVoidCbkFnType) f;
  if (!defrArrayNameCbk)
    defrArrayNameCbk = (defrStringCbkFnType) f;
  if (!defrFloorPlanNameCbk)
    defrFloorPlanNameCbk = (defrStringCbkFnType) f;
  if (!defrUnitsCbk)
    defrUnitsCbk = (defrDoubleCbkFnType) f;
  if (!defrDividerCbk)
    defrDividerCbk = (defrStringCbkFnType) f;
  if (!defrBusBitCbk)
    defrBusBitCbk = (defrStringCbkFnType) f;
  if (!defrSiteCbk)
    defrSiteCbk = (defrSiteCbkFnType) f;
  if (!defrCanplaceCbk)
    defrCanplaceCbk = (defrSiteCbkFnType) f;
  if (!defrCannotOccupyCbk)
    defrCannotOccupyCbk = (defrSiteCbkFnType) f;
  if (!defrComponentStartCbk)
    defrComponentStartCbk = (defrIntegerCbkFnType) f;
  if (!defrComponentEndCbk)
    defrComponentEndCbk = (defrVoidCbkFnType) f;
  if (!defrComponentCbk)
    defrComponentCbk = (defrComponentCbkFnType) f;
  if (!defrNetStartCbk)
    defrNetStartCbk = (defrIntegerCbkFnType) f;
  if (!defrNetEndCbk)
    defrNetEndCbk = (defrVoidCbkFnType) f;
  if (!defrNetCbk)
    defrNetCbk = (defrNetCbkFnType) f;
  if (!defrPathCbk)
    defrPathCbk = (defrPathCbkFnType) f;
  if (!defrVersionCbk)
    defrVersionCbk = (defrDoubleCbkFnType) f;
  if (!defrPinExtCbk)
    defrPinExtCbk = (defrStringCbkFnType) f;
  if (!defrComponentExtCbk)
    defrComponentExtCbk = (defrStringCbkFnType) f;
  if (!defrViaExtCbk)
    defrViaExtCbk = (defrStringCbkFnType) f;
  if (!defrNetConnectionExtCbk)
    defrNetConnectionExtCbk = (defrStringCbkFnType) f;
  if (!defrNetExtCbk)
    defrNetExtCbk = (defrStringCbkFnType) f;
  if (!defrGroupExtCbk)
    defrGroupExtCbk = (defrStringCbkFnType) f;
  if (!defrScanChainExtCbk)
    defrScanChainExtCbk = (defrStringCbkFnType) f;
  if (!defrIoTimingsExtCbk)
    defrIoTimingsExtCbk = (defrStringCbkFnType) f;
  if (!defrPartitionsExtCbk)
    defrPartitionsExtCbk = (defrStringCbkFnType) f;
  if (!defrHistoryCbk)
    defrHistoryCbk = (defrStringCbkFnType) f;
  if (!defrDieAreaCbk)
    defrDieAreaCbk = (defrBoxCbkFnType) f;
  if (!defrPinCapCbk)
    defrPinCapCbk = (defrPinCapCbkFnType) f;
  if (!defrPinCbk)
    defrPinCbk = (defrPinCbkFnType) f;
  if (!defrStartPinsCbk)
    defrStartPinsCbk = (defrIntegerCbkFnType) f;
  if (!defrPinEndCbk)
    defrPinEndCbk = (defrVoidCbkFnType) f;
  if (!defrDefaultCapCbk)
    defrDefaultCapCbk = (defrIntegerCbkFnType) f;
  if (!defrRowCbk)
    defrRowCbk = (defrRowCbkFnType) f;
  if (!defrTrackCbk)
    defrTrackCbk = (defrTrackCbkFnType) f;
  if (!defrGcellGridCbk)
    defrGcellGridCbk = (defrGcellGridCbkFnType) f;
  if (!defrViaStartCbk)
    defrViaStartCbk = (defrIntegerCbkFnType) f;
  if (!defrViaEndCbk)
    defrViaEndCbk = (defrVoidCbkFnType) f;
  if (!defrViaCbk)
    defrViaCbk = (defrViaCbkFnType) f;
  if (!defrRegionStartCbk)
    defrRegionStartCbk = (defrIntegerCbkFnType) f;
  if (!defrRegionEndCbk)
    defrRegionEndCbk = (defrVoidCbkFnType) f;
  if (!defrRegionCbk)
    defrRegionCbk = (defrRegionCbkFnType) f;
  if (!defrSNetStartCbk)
    defrSNetStartCbk = (defrIntegerCbkFnType) f;
  if (!defrSNetEndCbk)
    defrSNetEndCbk = (defrVoidCbkFnType) f;
  if (!defrSNetCbk)
    defrSNetCbk = (defrNetCbkFnType) f;
  if (!defrGroupsStartCbk)
    defrGroupsStartCbk = (defrIntegerCbkFnType) f;
  if (!defrGroupsEndCbk)
    defrGroupsEndCbk = (defrVoidCbkFnType) f;
  if (!defrGroupNameCbk)
    defrGroupNameCbk = (defrStringCbkFnType) f;
  if (!defrGroupMemberCbk)
    defrGroupMemberCbk = (defrStringCbkFnType) f;
  if (!defrGroupCbk)
    defrGroupCbk = (defrGroupCbkFnType) f;
  if (!defrAssertionsStartCbk)
    defrAssertionsStartCbk = (defrIntegerCbkFnType) f;
  if (!defrAssertionsEndCbk)
    defrAssertionsEndCbk = (defrVoidCbkFnType) f;
  if (!defrAssertionCbk)
    defrAssertionCbk = (defrAssertionCbkFnType) f;
  if (!defrConstraintsStartCbk)
    defrConstraintsStartCbk = (defrIntegerCbkFnType) f;
  if (!defrConstraintsEndCbk)
    defrConstraintsEndCbk = (defrVoidCbkFnType) f;
  if (!defrConstraintCbk)
    defrConstraintCbk = (defrAssertionCbkFnType) f;
  if (!defrScanchainsStartCbk)
    defrScanchainsStartCbk = (defrIntegerCbkFnType) f;
  if (!defrScanchainsEndCbk)
    defrScanchainsEndCbk = (defrVoidCbkFnType) f;
  if (!defrScanchainCbk)
    defrScanchainCbk = (defrScanchainCbkFnType) f;
  if (!defrIOTimingsStartCbk)
    defrIOTimingsStartCbk = (defrIntegerCbkFnType) f;
  if (!defrIOTimingsEndCbk)
    defrIOTimingsEndCbk = (defrVoidCbkFnType) f;
  if (!defrIOTimingCbk)
    defrIOTimingCbk = (defrIOTimingCbkFnType) f;
  if (!defrFPCStartCbk)
    defrFPCStartCbk = (defrIntegerCbkFnType) f;
  if (!defrFPCEndCbk)
    defrFPCEndCbk = (defrVoidCbkFnType) f;
  if (!defrFPCCbk)
    defrFPCCbk = (defrFPCCbkFnType) f;
  if (!defrTimingDisablesStartCbk)
    defrTimingDisablesStartCbk = (defrIntegerCbkFnType) f;
  if (!defrTimingDisablesEndCbk)
    defrTimingDisablesEndCbk = (defrVoidCbkFnType) f;
  if (!defrTimingDisableCbk)
    defrTimingDisableCbk = (defrTimingDisableCbkFnType) f;
  if (!defrPartitionsStartCbk)
    defrPartitionsStartCbk = (defrIntegerCbkFnType) f;
  if (!defrPartitionsEndCbk)
    defrPartitionsEndCbk = (defrVoidCbkFnType) f;
  if (!defrPartitionCbk)
    defrPartitionCbk = (defrPartitionCbkFnType) f;
  if (!defrPinPropStartCbk)
    defrPinPropStartCbk = (defrIntegerCbkFnType) f;
  if (!defrPinPropEndCbk)
    defrPinPropEndCbk = (defrVoidCbkFnType) f;
  if (!defrPinPropCbk)
    defrPinPropCbk = (defrPinPropCbkFnType) f;
  if (!defrCaseSensitiveCbk)
    defrCaseSensitiveCbk = (defrIntegerCbkFnType) f;
}

const char *
typeToString(defrCallbackType_e num)
{
  switch ((int) num) {
    case defrUnspecifiedCbkType:return "Unspecified";
  case defrDesignStartCbkType:
    return "Design Start";
  case defrTechNameCbkType:
    return "Tech Name";
  case defrPropCbkType:
    return "Property";
  case defrPropDefEndCbkType:
    return "Property Definitions Section End";
  case defrPropDefStartCbkType:
    return "Property Definitions Section Start";
  case defrFloorPlanNameCbkType:
    return "FloorPlanName";
  case defrArrayNameCbkType:
    return "Array Name";
  case defrUnitsCbkType:
    return "Units";
  case defrDividerCbkType:
    return "Divider";
  case defrBusBitCbkType:
    return "BusBit Character";
  case defrSiteCbkType:
    return "Site";
  case defrComponentStartCbkType:
    return "Components Section Start";
  case defrComponentCbkType:
    return "Component";
  case defrComponentEndCbkType:
    return "Components Section End";
  case defrNetStartCbkType:
    return "Nets Section Start";
  case defrNetCbkType:
    return "Net";
  case defrNetEndCbkType:
    return "Nets Section End";
  case defrPathCbkType:
    return "Path";
  case defrVersionCbkType:
    return "Version";
  case defrComponentExtCbkType:
    return "Component User Extention";
  case defrPinExtCbkType:
    return "Pin User Extension";
  case defrViaExtCbkType:
    return "Via User Extension";
  case defrNetConnectionExtCbkType:
    return "NetConnection User Extention";
  case defrNetExtCbkType:
    return "Net User Extension";
  case defrGroupExtCbkType:
    return "Group User Extension";
  case defrScanChainExtCbkType:
    return "ScanChain User Extension";
  case defrIoTimingsExtCbkType:
    return "IoTimings User Extension";
  case defrPartitionsExtCbkType:
    return "Partitions User Extension";
  case defrHistoryCbkType:
    return "History";
  case defrDieAreaCbkType:
    return "DieArea";
  case defrCanplaceCbkType:
    return "Canplace";
  case defrCannotOccupyCbkType:
    return "CannotOccupy";
  case defrPinCapCbkType:
    return "PinCap";
  case defrDefaultCapCbkType:
    return "DefaultCap";
  case defrStartPinsCbkType:
    return "Start Pins Section";
  case defrPinCbkType:
    return "Pin";
  case defrPinEndCbkType:
    return "End Pins Section";
  case defrRowCbkType:
    return "Row";
  case defrTrackCbkType:
    return "Track";
  case defrGcellGridCbkType:
    return "GcellGrid";
  case defrViaStartCbkType:
    return "Start Vias Section";
  case defrViaCbkType:
    return "Via";
  case defrViaEndCbkType:
    return "End Vias Section";
  case defrRegionStartCbkType:
    return "Region Section Start";
  case defrRegionCbkType:
    return "Region";
  case defrRegionEndCbkType:
    return "Region Section End";
  case defrSNetStartCbkType:
    return "Special Net Section Start";
  case defrSNetCbkType:
    return "Special Net";
  case defrSNetEndCbkType:
    return "Special Net Section End";
  case defrGroupsStartCbkType:
    return "Groups Section Start";
  case defrGroupNameCbkType:
    return "Group Name";
  case defrGroupMemberCbkType:
    return "Group Member";
  case defrGroupCbkType:
    return "Group";
  case defrGroupsEndCbkType:
    return "Groups Section End";
  case defrAssertionsStartCbkType:
    return "Assertions Section Start";
  case defrAssertionCbkType:
    return "Assertion";
  case defrAssertionsEndCbkType:
    return "Assertions Section End";
  case defrConstraintsStartCbkType:
    return "Constraints Section Start";
  case defrConstraintCbkType:
    return "Constraint";
  case defrConstraintsEndCbkType:
    return "Constraints Section End";
  case defrScanchainsStartCbkType:
    return "Scanchains Section Start";
  case defrScanchainCbkType:
    return "Scanchain";
  case defrScanchainsEndCbkType:
    return "Scanchains Section End";
  case defrIOTimingsStartCbkType:
    return "IOTimings Section Start";
  case defrIOTimingCbkType:
    return "IOTiming";
  case defrIOTimingsEndCbkType:
    return "IOTimings Section End";
  case defrFPCStartCbkType:
    return "Floor Plan Constraints Section Start";
  case defrFPCCbkType:
    return "Floor Plan Constraint";
  case defrFPCEndCbkType:
    return "Floor Plan Constraints Section End";
  case defrTimingDisablesStartCbkType:
    return "TimingDisables Section Start";
  case defrTimingDisableCbkType:
    return "TimingDisable";
  case defrTimingDisablesEndCbkType:
    return "TimingDisables Section End";
  case defrPartitionsStartCbkType:
    return "Partitions Section Start";
  case defrPartitionCbkType:
    return "Partition";
  case defrPartitionsEndCbkType:
    return "Partitions Section End";
  case defrPinPropStartCbkType:
    return "PinProp Section Start";
  case defrPinPropCbkType:
    return "PinProp";
  case defrPinPropEndCbkType:
    return "PinProp Section End";
  case defrCaseSensitiveCbkType:
    return "CaseSensitive";
  case defrDesignEndCbkType:
    return "DesignEnd";
  default:
    break;
  }
  return "BOGUS";
}

void 
defrPrintUnusedCallbacks(FILE * log)
{
  int     i;

  int     first = 1;

  for (i = 0;
       i < CBMAX;
       i++) {
    if (defrUnusedCallbacks[i]) {
      if (first)
	fprintf(log, "DEF items found in the file that had no callback:\n");
      first = 0;
      fprintf(log, "%5d %s\n", defrUnusedCallbacks[i], typeToString((defrCallbackType_e) i));
    }
  }
}

int    *
defUnusedCallbackCount()
{
  return defrUnusedCallbacks;
}

int 
defrCatchAll(defrCallbackType_e typ, void *data, defiUserData ud)
{
  if (defDebug[3])
    printf("CatchAll(%d, 0x%x, 0x%x)\n", typ, data, ud);
  if ((int) typ >= 0 && (int) typ < CBMAX) {
    defrUnusedCallbacks[(int) typ] += 1;
  }
  else {
    defrUnusedCallbacks[0] += 1;
    return 1;
  }
  return 0;
}

const char *
defrFName()
{
  return defrFileName;
}

int 
defrRead(FILE * f, const char *fName, defiUserData uData, int case_sensitive)
{
  int     status;

  defrFileName = (char *) fName;
  defrFile = f;
  defrUserData = uData;
  names_case_sensitive = case_sensitive;
  defrNeedPathData = ((defrNetCbk && defrAddPathToNet) || defrPathCbk) ? 1 : 0;
  if (defrNeedPathData) {
    defrPath = (defiPath *) malloc(sizeof(defiPath));
    defiPath_Init(defrPath);
  }
  lex_init();
  status = defyyparse();
  defiProp_clear(&(defrProp));
  defiSite_clear(&(defrSite));
  defiComponent_clear(&(defrComponent));
  defiNet_clear(&(defrNet));
  defiSite_clear(&(defrCannotOccupy));
  defiSite_clear(&(defrCanplace));
  defiScanchain_clear(&(defrScanchain));
  defiIOTiming_clear(&(defrIOTiming));
  defiFPC_clear(&(defrFPC));
  defiTimingDisable_clear(&(defrTimingDisable));
  defiPartition_clear(&(defrPartition));
  lex_un_init();
  return status;
}

void 
defrSetUserData(defiUserData d)
{
  defrUserData = d;
}

defiUserData 
defrGetUserData()
{
  return defrUserData;
}

void 
defrSetDesignCbk(defrStringCbkFnType f)
{
  defrDesignCbk = f;
}

void 
defrSetTechnologyCbk(defrStringCbkFnType f)
{
  defrTechnologyCbk = f;
}

void 
defrSetDesignEndCbk(defrVoidCbkFnType f)
{
  defrDesignEndCbk = f;
}

void 
defrSetPropCbk(defrPropCbkFnType f)
{
  defrPropCbk = f;
}

void 
defrSetPropDefEndCbk(defrVoidCbkFnType f)
{
  defrPropDefEndCbk = f;
}

void 
defrSetPropDefStartCbk(defrVoidCbkFnType f)
{
  defrPropDefStartCbk = f;
}

void 
defrSetArrayNameCbk(defrStringCbkFnType f)
{
  defrArrayNameCbk = f;
}

void 
defrSetFloorPlanNameCbk(defrStringCbkFnType f)
{
  defrFloorPlanNameCbk = f;
}

void 
defrSetUnitsCbk(defrDoubleCbkFnType f)
{
  defrUnitsCbk = f;
}

void 
defrSetVersionCbk(defrDoubleCbkFnType f)
{
  defrVersionCbk = f;
}

void 
defrSetDividerCbk(defrStringCbkFnType f)
{
  defrDividerCbk = f;
}

void 
defrSetBusBitCbk(defrStringCbkFnType f)
{
  defrBusBitCbk = f;
}

void 
defrSetSiteCbk(defrSiteCbkFnType f)
{
  defrSiteCbk = f;
}

void 
defrSetCanplaceCbk(defrSiteCbkFnType f)
{
  defrCanplaceCbk = f;
}

void 
defrSetCannotOccupyCbk(defrSiteCbkFnType f)
{
  defrCannotOccupyCbk = f;
}

void 
defrSetComponentStartCbk(defrIntegerCbkFnType f)
{
  defrComponentStartCbk = f;
}

void 
defrSetComponentEndCbk(defrVoidCbkFnType f)
{
  defrComponentEndCbk = f;
}

void 
defrSetComponentCbk(defrComponentCbkFnType f)
{
  defrComponentCbk = f;
}

void 
defrSetNetStartCbk(defrIntegerCbkFnType f)
{
  defrNetStartCbk = f;
}

void 
defrSetNetEndCbk(defrVoidCbkFnType f)
{
  defrNetEndCbk = f;
}

void 
defrSetNetCbk(defrNetCbkFnType f)
{
  defrNetCbk = f;
}

void 
defrSetSNetStartCbk(defrIntegerCbkFnType f)
{
  defrSNetStartCbk = f;
}

void 
defrSetSNetEndCbk(defrVoidCbkFnType f)
{
  defrSNetEndCbk = f;
}

void 
defrSetSNetCbk(defrNetCbkFnType f)
{
  defrSNetCbk = f;
}

void 
defrSetPathCbk(defrPathCbkFnType f)
{
  defrPathCbk = f;
}

void 
defrSetAddPathToNet()
{
  defrAddPathToNet = 1;
}

void 
defrSetAllowComponentNets()
{
  defrAllowComponentNets = 1;
}

void 
defrSetComponentExtCbk(defrStringCbkFnType f)
{
  defrComponentExtCbk = f;
}

void 
defrSetPinExtCbk(defrStringCbkFnType f)
{
  defrPinExtCbk = f;
}

void 
defrSetViaExtCbk(defrStringCbkFnType f)
{
  defrViaExtCbk = f;
}

void 
defrSetNetConnectionExtCbk(defrStringCbkFnType f)
{
  defrNetConnectionExtCbk = f;
}

void 
defrSetNetExtCbk(defrStringCbkFnType f)
{
  defrNetExtCbk = f;
}

void 
defrSetGroupExtCbk(defrStringCbkFnType f)
{
  defrGroupExtCbk = f;
}

void 
defrSetScanChainExtCbk(defrStringCbkFnType f)
{
  defrScanChainExtCbk = f;
}

void 
defrSetIoTimingsExtCbk(defrStringCbkFnType f)
{
  defrIoTimingsExtCbk = f;
}

void 
defrSetPartitionsExtCbk(defrStringCbkFnType f)
{
  defrPartitionsExtCbk = f;
}

void 
defrSetHistoryCbk(defrStringCbkFnType f)
{
  defrHistoryCbk = f;
}

void 
defrSetDieAreaCbk(defrBoxCbkFnType f)
{
  defrDieAreaCbk = f;
}

void 
defrSetPinCapCbk(defrPinCapCbkFnType f)
{
  defrPinCapCbk = f;
}

void 
defrSetPinEndCbk(defrVoidCbkFnType f)
{
  defrPinEndCbk = f;
}

void 
defrSetStartPinsCbk(defrIntegerCbkFnType f)
{
  defrStartPinsCbk = f;
}

void 
defrSetDefaultCapCbk(defrIntegerCbkFnType f)
{
  defrDefaultCapCbk = f;
}

void 
defrSetPinCbk(defrPinCbkFnType f)
{
  defrPinCbk = f;
}

void 
defrSetRowCbk(defrRowCbkFnType f)
{
  defrRowCbk = f;
}

void 
defrSetTrackCbk(defrTrackCbkFnType f)
{
  defrTrackCbk = f;
}

void 
defrSetGcellGridCbk(defrGcellGridCbkFnType f)
{
  defrGcellGridCbk = f;
}

void 
defrSetViaStartCbk(defrIntegerCbkFnType f)
{
  defrViaStartCbk = f;
}

void 
defrSetViaEndCbk(defrVoidCbkFnType f)
{
  defrViaEndCbk = f;
}

void 
defrSetViaCbk(defrViaCbkFnType f)
{
  defrViaCbk = f;
}

void 
defrSetRegionStartCbk(defrIntegerCbkFnType f)
{
  defrRegionStartCbk = f;
}

void 
defrSetRegionEndCbk(defrVoidCbkFnType f)
{
  defrRegionEndCbk = f;
}

void 
defrSetRegionCbk(defrRegionCbkFnType f)
{
  defrRegionCbk = f;
}

void 
defrSetGroupsStartCbk(defrIntegerCbkFnType f)
{
  defrGroupsStartCbk = f;
}

void 
defrSetGroupsEndCbk(defrVoidCbkFnType f)
{
  defrGroupsEndCbk = f;
}

void 
defrSetGroupNameCbk(defrStringCbkFnType f)
{
  defrGroupNameCbk = f;
}

void 
defrSetGroupMemberCbk(defrStringCbkFnType f)
{
  defrGroupMemberCbk = f;
}

void 
defrSetGroupCbk(defrGroupCbkFnType f)
{
  defrGroupCbk = f;
}

void 
defrSetAssertionsStartCbk(defrIntegerCbkFnType f)
{
  defrAssertionsStartCbk = f;
}

void 
defrSetAssertionsEndCbk(defrVoidCbkFnType f)
{
  defrAssertionsEndCbk = f;
}

void 
defrSetAssertionCbk(defrAssertionCbkFnType f)
{
  defrAssertionCbk = f;
}

void 
defrSetConstraintsStartCbk(defrIntegerCbkFnType f)
{
  defrConstraintsStartCbk = f;
}

void 
defrSetConstraintsEndCbk(defrVoidCbkFnType f)
{
  defrConstraintsEndCbk = f;
}

void 
defrSetConstraintCbk(defrAssertionCbkFnType f)
{
  defrConstraintCbk = f;
}

void 
defrSetScanchainsStartCbk(defrIntegerCbkFnType f)
{
  defrScanchainsStartCbk = f;
}

void 
defrSetScanchainsEndCbk(defrVoidCbkFnType f)
{
  defrScanchainsEndCbk = f;
}

void 
defrSetScanchainCbk(defrScanchainCbkFnType f)
{
  defrScanchainCbk = f;
}

void 
defrSetIOTimingsStartCbk(defrIntegerCbkFnType f)
{
  defrIOTimingsStartCbk = f;
}

void 
defrSetIOTimingsEndCbk(defrVoidCbkFnType f)
{
  defrIOTimingsEndCbk = f;
}

void 
defrSetIOTimingCbk(defrIOTimingCbkFnType f)
{
  defrIOTimingCbk = f;
}

void 
defrSetFPCStartCbk(defrIntegerCbkFnType f)
{
  defrFPCStartCbk = f;
}

void 
defrSetFPCEndCbk(defrVoidCbkFnType f)
{
  defrFPCEndCbk = f;
}

void 
defrSetFPCCbk(defrFPCCbkFnType f)
{
  defrFPCCbk = f;
}

void 
defrSetTimingDisablesStartCbk(defrIntegerCbkFnType f)
{
  defrTimingDisablesStartCbk = f;
}

void 
defrSetTimingDisablesEndCbk(defrVoidCbkFnType f)
{
  defrTimingDisablesEndCbk = f;
}

void 
defrSetTimingDisableCbk(defrTimingDisableCbkFnType f)
{
  defrTimingDisableCbk = f;
}

void 
defrSetPartitionsStartCbk(defrIntegerCbkFnType f)
{
  defrPartitionsStartCbk = f;
}

void 
defrSetPartitionsEndCbk(defrVoidCbkFnType f)
{
  defrPartitionsEndCbk = f;
}

void 
defrSetPartitionCbk(defrPartitionCbkFnType f)
{
  defrPartitionCbk = f;
}

void 
defrSetPinPropStartCbk(defrIntegerCbkFnType f)
{
  defrPinPropStartCbk = f;
}

void 
defrSetPinPropEndCbk(defrVoidCbkFnType f)
{
  defrPinPropEndCbk = f;
}

void 
defrSetPinPropCbk(defrPinPropCbkFnType f)
{
  defrPinPropCbk = f;
}

void 
defrSetCaseSensitiveCbk(defrIntegerCbkFnType f)
{
  defrCaseSensitiveCbk = f;
}

/* NEW CALLBACK - Put the set functions for the new callbacks here.  */
extern DEFI_LOG_FUNCTION defiErrorLogFunction;

void 
defrSetLogFunction(DEFI_LOG_FUNCTION f)
{
  defiErrorLogFunction = f;
}

void 
pathIsDone(int shield)
{
  if (defrNetCbk && defrAddPathToNet) {
    if (defrSubnet) {
      defiSubnet_addPath(defrSubnet,
			 defrPath);
    }
    else {
      if (shield)
	defiNet_addShieldPath(&(defrNet),
			      defrPath);
      else
	defiNet_addPath(&(defrNet),
			defrPath);
    }
  }
  else if (defrPathCbk) {
    (*defrPathCbk) (defrPathCbkType, defrPath, defrUserData);
    defiPath_Destroy(defrPath);
    free((char *) defrPath);
  }
  defrPath = (defiPath *) malloc(sizeof(defiPath));
  defiPath_Init(defrPath);
}

/* from lexer  */
extern char commentChar;

void 
defrSetCommentChar(char c)
{
  commentChar = c;
}

extern void *def_alias_set;

void 
defrAddAlias(const char *key, const char *value, int marked)
{
  char   *k1;

  char   *v1;

  int     len = strlen(key) + 1;

  k1 = (char *) malloc(len);
  strcpy(k1, key);
  len = strlen(value) + 1 + 1;
  v1 = (char *) malloc(len);
  if (marked != 0)
    marked = 1;
  sprintf(v1, "%d%s", marked, value);
  if (def_alias_set == NULL)
    def_alias_set = STR_make_stringtab(TABLE_CALLER_ALLOCS, "&alias", TABLE_USE_CASE);
  STR_add_to_stringtab(def_alias_set, k1, v1);
}
