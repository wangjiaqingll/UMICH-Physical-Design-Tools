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

#ifndef LEFRCALLBACKS_H
#define LEFRCALLBACKS_H 1
#include "lefiKRDefs.h"
#include "lefrReader.h"

/*
 * All declarations that are public (for the casual user) are found in
 */

/*
 * lefrReader.hpp.
 */

/*
 * These declarations are not seen by the general user.  Only the
 */

/*
 * parser (lef.y ... lef.tab.c) should use them.
 */

/*
 * These are the callback function pointers.
 */

/*
 * They are set by calls lefined in lefrReader.hpp and lefrReader.cpp
 */

/*
 * The grammer calls these as it parses the file.
 */
extern lefrUnitsCbkFnType lefrUnitsCbk;

extern lefrLayerCbkFnType lefrLayerCbk;

extern lefrIntegerCbkFnType lefrCaseSensitiveCbk;

extern lefrStringCbkFnType lefrNoWireExtensionCbk;

extern lefrStringCbkFnType lefrBusBitCharsCbk;

extern lefrStringCbkFnType lefrDividerCharCbk;

extern lefrDoubleCbkFnType lefrVersionCbk;

extern lefrViaCbkFnType lefrViaCbk;

extern lefrViaRuleCbkFnType lefrViaRuleCbk;

extern lefrSpacingCbkFnType lefrSpacingCbk;

extern lefrIRDropCbkFnType lefrIRDropCbk;

extern lefrDoubleCbkFnType lefrDielectricCbk;

extern lefrMinFeatureCbkFnType lefrMinFeatureCbk;

extern lefrNonDefaultCbkFnType lefrNonDefaultCbk;

extern lefrSiteCbkFnType lefrSiteCbk;

extern lefrStringCbkFnType lefrMacroBeginCbk;

extern lefrPinCbkFnType lefrPinCbk;

extern lefrMacroCbkFnType lefrMacroCbk;

extern lefrObstructionCbkFnType lefrObstructionCbk;

extern lefrArrayCbkFnType lefrArrayCbk;

extern lefrVoidCbkFnType lefrLibraryEndCbk;

extern lefrTimingCbkFnType lefrTimingCbk;

/* NEW CALLBACK add the extern here */
extern lefrVoidCbkFnType lefrSpacingBeginCbk;

extern lefrVoidCbkFnType lefrSpacingEndCbk;

extern lefrStringCbkFnType lefrArrayBeginCbk;

extern lefrStringCbkFnType lefrArrayEndCbk;

extern lefrVoidCbkFnType lefrIRDropBeginCbk;

extern lefrVoidCbkFnType lefrIRDropEndCbk;

extern lefrStringCbkFnType lefrPropBeginCbk;

extern lefrStringCbkFnType lefrPropEndCbk;

extern lefrPropCbkFnType lefrPropCbk;

extern lefrNoiseMarginCbkFnType lefrNoiseMarginCbk;

extern lefrDoubleCbkFnType lefrEdgeRateThreshold1Cbk;

extern lefrDoubleCbkFnType lefrEdgeRateThreshold2Cbk;

extern lefrDoubleCbkFnType lefrEdgeRateScaleFactorCbk;

extern lefrNoiseTableCbkFnType lefrNoiseTableCbk;

extern lefrCorrectionTableCbkFnType lefrCorrectionTableCbk;

extern lefrDoubleCbkFnType lefrInputAntennaCbk;

extern lefrDoubleCbkFnType lefrOutputAntennaCbk;

extern lefrDoubleCbkFnType lefrInoutAntennaCbk;

/*
 * These are data structures that are filled while parsing the file.
 */
extern lefiUserData lefrUserData;

extern lefiUnits lefrUnits;

extern lefiLayer lefrLayer;

extern lefiVia  lefrVia;

extern lefiViaRule lefrViaRule;

extern lefiSpacing lefrSpacing;

extern lefiIRDrop lefrIRDrop;

extern lefiMinFeature lefrMinFeature;

extern lefiNonDefault lefrNonDefault;

extern lefiSite lefrSite;

extern lefiMacro lefrMacro;

extern lefiObstruction lefrObstruction;

extern lefiPin  lefrPin;

extern lefiArray lefrArray;

extern lefiSitePattern *lefrSitePatternPtr;

extern lefiGeometries *lefrGeometriesPtr;

extern lefiTrackPattern *lefrTrackPatternPtr;

extern lefiGcellPattern *lefrGcellPatternPtr;

extern lefiProp lefrProp;

extern lefiTiming lefrTiming;

extern struct lefiNoiseMargin lefrNoiseMargin;

extern lefiNoiseTable lefrNoiseTable;

extern lefiCorrectionTable lefrCorrectionTable;

/* NEW CALLBACK add the one static item here */

/*
 * Flags that control parsing
 */
extern int      lefrDoSite;

extern int      lefrDoGeometries;

extern int      lefrDoTrack;

extern int      lefrDoGcell;

/*
 * Parser utility routines.
 */
#endif
