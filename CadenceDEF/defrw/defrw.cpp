#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
//#include <iostream>
#include "defrReader.hpp"

char defaultName[64];
char defaultOut[64];

// Global variables
FILE* fout;
int userData;
int numObjs;

int done(defrCallbackType_e c, void* dummy, defiUserData ud) 
{
  return 0;
}


char* orientStr(int orient) {
  switch (orient) {
      case 0: return ((char*)"N");
      case 1: return ((char*)"W");
      case 2: return ((char*)"S");
      case 3: return ((char*)"E");
      case 4: return ((char*)"FN");
      case 5: return ((char*)"FE");
      case 6: return ((char*)"FS");
      case 7: return ((char*)"FW");
  };
  return ((char*)"BOGUS");
}


int compf(defrCallbackType_e c, defiComponent* co, defiUserData ud) 
{
  return 0;
}


int netf(defrCallbackType_e c, defiNet* net, defiUserData ud) 
{
  return 0;
}


int snetf(defrCallbackType_e c, defiNet* net, defiUserData ud) {
  // For net and special net.
  int         i, j, x, y, count, newLayer;
  char*       layerName;
  double      dist, left, right;
  defiPath*   p;
  defiSubnet  *s;
  int         path;
  defiShield* shield;

  fprintf(fout, "SPECIALNET\n");
  fprintf(fout, "- %s ", net->defiNet::name());

  count = 0;
  // compName & pinName
  for (i = 0; i < net->defiNet::numConnections(); i++) {
      // set the limit of only 5 items print out in one line
      count++;
      if (count >= 5) {
          fprintf(fout, "\n");
          count = 0;
      }
      fprintf (fout, "( %s %s ) ", net->defiNet::instance(i),
               net->defiNet::pin(i));
  }

  // specialWiring
  if (net->defiNet::isFixed()) {
    fprintf(fout, "\n+ FIXED ");
    count = 0;
  }
  if (net->defiNet::isRouted()) {
    fprintf(fout, "\n+ ROUTED ");
    count = 0;
  }
  if (net->defiNet::isCover()) {
    fprintf(fout, "\n+ COVER ");
    count = 0;
  }

  fprintf(fout, "****************\n"); 
  fprintf(fout, "Without scope: %d", net->numPaths());
  fprintf(fout, "Scope: %d", net->defiNet::numPaths());
  fprintf(fout, "****************\n"); 

  if (net->defiNet::numPaths()) {
    newLayer = 0;
    for (i = 0; i < net->defiNet::numPaths(); i++) {
      p = net->defiNet::path(i);
      p->initTraverse();
      while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
        // Don't want the line to be too long
        count++;
        if (count >= 5) {
            fprintf(fout, "\n");
            count = 0;
        }
        switch (path) {
          case DEFIPATH_LAYER:
               if (newLayer == 0) {
                   fprintf(fout, "%s ", p->defiPath::getLayer());
                   newLayer = 1;
               } else
                   fprintf(fout, "NEW %s ", p->defiPath::getLayer());
               break;
          case DEFIPATH_VIA:
               fprintf(fout, "%s ", p->defiPath::getVia());
               break;
          case DEFIPATH_WIDTH:
               fprintf(fout, "%d ", p->defiPath::getWidth());
               break;
          case DEFIPATH_POINT:
               p->defiPath::getPoint(&x, &y);
               fprintf(fout, "( %d %d ) ", x, y);
               break;
          case DEFIPATH_TAPER:
               fprintf(fout, "TAPER ");
               break;
          case DEFIPATH_SHAPE:
               fprintf(fout, "+ SHAPE %s ", p->defiPath::getShape());
               break;
        }
      }
    }
  }
 
  return 0;
}


int tname(defrCallbackType_e c, const char* string, defiUserData ud) 
{
  return 0;
}

int dname(defrCallbackType_e c, const char* string, defiUserData ud) 
{
  return 0;
}

int cs(defrCallbackType_e c, int num, defiUserData ud) 
{
  return 0;
}


int constraintst(defrCallbackType_e c, int num, defiUserData ud) 
{
  return 0;
}

int constraint(defrCallbackType_e c, defiAssertion* a, defiUserData ud) 
{
  return 0;
}


int prop(defrCallbackType_e c, defiProp* p, defiUserData ud) 
{
  return 0;
}


int propend(defrCallbackType_e c, void* dummy, defiUserData ud) 
{
  return 0;
}


int hist(defrCallbackType_e c, const char* h, defiUserData ud) 
{
  return 0;
}


int an(defrCallbackType_e c, const char* h, defiUserData ud) 
{
  return 0;
}


int fn(defrCallbackType_e c, const char* h, defiUserData ud) 
{
  return 0;
}


int bbn(defrCallbackType_e c, const char* h, defiUserData ud) 
{
  return 0;
}


int vers(defrCallbackType_e c, double d, defiUserData ud) 
{
  return 0;
}


int units(defrCallbackType_e c, double d, defiUserData ud) 
{
  return 0;
}


int casesens(defrCallbackType_e c, int d, defiUserData ud) 
{
  return 0;
}


int cls(defrCallbackType_e c, void* cl, defiUserData ud) 
{
  return 0;
}


int dn(defrCallbackType_e c, const char* h, defiUserData ud) 
{
  return 0;
}


int ext(defrCallbackType_e t, const char* c, defiUserData ud) 
{
  return 0;
}


int main(int argc, char** argv) 
{
  int num = 1734;
  char* inFile;
  char* outFile;
  FILE* f;
  int res;
  int noCalls = 0;

  strcpy(defaultName, "def.in");
  strcpy(defaultOut, "list");
  inFile = defaultName;
  outFile = defaultOut;
  fout = stdout;
  userData = 0x01020304;

  argc--;
  argv++;
  while (argc--) {

    if (strcmp(*argv, "-d") == 0) {
      argv++;
      argc--;
      sscanf(*argv, "%d", &num);
      defiSetDebug(num, 1);

    } else if (strcmp(*argv, "-nc") == 0) {
      noCalls = 1;

    } else if (strcmp(*argv, "-o") == 0) {
      argv++;
      argc--;
      outFile = *argv;
      if ((fout = fopen(outFile, "w")) == 0) {
	fprintf(stderr, "ERROR: could not open output file\n");
	return 2;
      }

    } else if (argv[0][0] != '-') {
      inFile = *argv;
    } else if (strcmp(*argv, "-h") == 0) {
      fprintf(stderr, "Usage: defrw [<defFilename>] [-o <outputFilename>]\n");
      return 2;
    } else {
      fprintf(stderr, "ERROR: Illegal command line option: '%s'\n", *argv);
      return 2;
    }

    argv++;
  }

  defrInit();

  if (noCalls == 0) {
    defrSetUserData((void*)3);
    defrSetDesignCbk(dname);
    defrSetTechnologyCbk(tname);
    defrSetDesignEndCbk(done);
    defrSetPropCbk(prop);
    defrSetPropDefEndCbk(propend);
    defrSetNetCbk(netf);
    defrSetSNetCbk(snetf);
    defrSetComponentCbk(compf);
    defrSetAddPathToNet();
    defrSetHistoryCbk(hist);
    defrSetConstraintCbk(constraint);
    defrSetAssertionCbk(constraint);
    defrSetArrayNameCbk(an);
    defrSetFloorPlanNameCbk(fn);
    defrSetDividerCbk(dn);
    defrSetBusBitCbk(bbn);

    defrSetAssertionsStartCbk(constraintst);
    defrSetConstraintsStartCbk(constraintst);
    defrSetComponentStartCbk(cs);
    defrSetPinPropStartCbk(cs);
    defrSetNetStartCbk(cs);
    defrSetStartPinsCbk(cs);
    defrSetViaStartCbk(cs);
    defrSetRegionStartCbk(cs);
    defrSetSNetStartCbk(cs);
    defrSetGroupsStartCbk(cs);
    defrSetScanchainsStartCbk(cs);
    defrSetIOTimingsStartCbk(cs);
    defrSetFPCStartCbk(cs);
    defrSetTimingDisablesStartCbk(cs);
    defrSetPartitionsStartCbk(cs);

    // All of the extensions point to the same function.
    defrSetNetExtCbk(ext);
    defrSetComponentExtCbk(ext);
    defrSetPinExtCbk(ext);
    defrSetViaExtCbk(ext);
    defrSetNetConnectionExtCbk(ext);
    defrSetGroupExtCbk(ext);
    defrSetScanChainExtCbk(ext);
    defrSetIoTimingsExtCbk(ext);
    defrSetPartitionsExtCbk(ext);

    defrSetUnitsCbk(units);
    defrSetVersionCbk(vers);
    defrSetCaseSensitiveCbk(casesens);

    // The following calls are an example of using one function "cls"
    // to be the callback for many DIFFERENT types of constructs.
    // We have to cast the function type to meet the requirements
    // of each different set function.
    defrSetSiteCbk((defrSiteCbkFnType)cls);
    defrSetCanplaceCbk((defrSiteCbkFnType)cls);
    defrSetCannotOccupyCbk((defrSiteCbkFnType)cls);
    defrSetDieAreaCbk((defrBoxCbkFnType)cls);
    defrSetPinCapCbk((defrPinCapCbkFnType)cls);
    defrSetPinCbk((defrPinCbkFnType)cls);
    defrSetPinPropCbk((defrPinPropCbkFnType)cls);
    defrSetDefaultCapCbk((defrIntegerCbkFnType)cls);
    defrSetRowCbk((defrRowCbkFnType)cls);
    defrSetTrackCbk((defrTrackCbkFnType)cls);
    defrSetGcellGridCbk((defrGcellGridCbkFnType)cls);
    defrSetViaCbk((defrViaCbkFnType)cls);
    defrSetRegionCbk((defrRegionCbkFnType)cls);
    defrSetGroupCbk((defrGroupCbkFnType)cls);
    defrSetScanchainCbk((defrScanchainCbkFnType)cls);
    defrSetIOTimingCbk((defrIOTimingCbkFnType)cls);
    defrSetFPCCbk((defrFPCCbkFnType)cls);
    defrSetTimingDisableCbk((defrTimingDisableCbkFnType)cls);
    defrSetPartitionCbk((defrPartitionCbkFnType)cls);
  }

  if ((f = fopen(inFile,"r")) == 0) {
    fprintf(stderr,"Couldn't open input file '%s'\n", inFile);
    return(2);
  }
  res = defrRead(f, inFile, (void*)userData, 1);

  fclose(fout);

  return 0;
}
