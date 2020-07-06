//
//       Copyright (C) 1997 Cadence Design Systems Inc.
//                     All Rights Reserved.
//
 
 
// This program will diff two lef files or two def files and list the
// different between the two files.  This problem is not intend to
// diff a real design.  If user runs this program will a big design,
// they may experience long execution time and may even ran out of
// memory.
//
// This program is to give user a feel of whether they are using the
// parser correctly.  After they read the lef/def file in, and
// write them back out in lef/def format.
//
// This program support lef/def 5.2 only

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "defrReader.hpp"
#include "diffDefRW.hpp"

char * exeName;   // use to save the executable name

// This program requires 3 input, the type of the file, lef or def
// fileName1 and fileName2
void diffUsage() {
   printf("Usage: %s fileName1 fileName2 [-o outputFileName]\n", exeName);
}

main(int argc, char** argv) {
  char *fileName1, *fileName2;   // For the filenames to compare
  char *outFile = NULL;          // For the output filename
  char defOut1[100], defOut2[100];
  
  exeName = argv[0];

  if (argc < 3 || argc == 4 || argc > 5) {  // Has to be at least 3 arguments
      diffUsage();                          // but no more than 5 arguments
      return(1);
  }

  fileName1 = argv[1];
  fileName2 = argv[2];

  if (argc == 5) {
      if (strcasecmp(argv[3], "-o") != 0) {
          diffUsage();
          return(1);
      }
      outFile = argv[4];
  }

  // Temporary output files, to whole the def file information as
  // they are read in.  Later these files will be sorted for compare
  strcpy(defOut1, "/tmp/defOut1");
  strcpy(defOut2, "/tmp/defOut2");

  // def files
  printf("Reading file: %s\n", fileName1);
  if (diffDefReadFile(fileName1, defOut1) != 0)
      return(1);
  printf("Reading file: %s\n", fileName2);
  if (diffDefReadFile(fileName2, defOut2) != 0)
      return(1); 
  return (0);
}
