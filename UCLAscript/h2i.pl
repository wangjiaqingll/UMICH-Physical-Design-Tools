#!/usr/bin/env perl
print << 'END';
%module UCLAscript
%{
#include "DB/dbTypes.h"
#include "DB/dbContext.h"
#include "DB/dbDesign.h"
#include "DB/dbSpatial.h"
#include "DB/dbEasyAccess.h"
#include "DB/dbRoutingInfo.h"
#include "Placement/placeWOri.h"

//#include "dbTemporal.h"
#include "DB/DB.h"
#include "DB/dbNetlist.h"
#define DB_Parameters DB::Parameters
#include "Capo/capoPlacer.h"
#include "RBPlace/RBPlacement.h"
%}
END

@files=<DATA>;
#@files=("dbCell.h\n");
for $f(@files){
   if(/^\s*#/o){ next;}
   chop $f;
   open(F,"../$f");
   while(<F>){
      if(/^\s*#ifndef\s+_/o){ last;}
   }
   $pr=0;
   $lpar=0;
   while(<F>){
      if(/^\s*#/o){ next;}
      if(/^\s*class\s+(\w+)/o || /^\s*struct\s+(\w+)/o){
         $old_class=$class; $class=$1;
         if($class eq "ISeqPair" || $class eq "ISPairLayer"){
            while(<F>){ last if /^\s*\}\s*;/o;}
            next;
         }
         $pr=1;
         if($_ !~ /;/o){ $lpar++;}
      }elsif(/^\s*public:/o){
         $pr=1;
      }elsif(/^\s*friend\b/o){
         next;
      }elsif(/operator\b.*?\(/o || /^\s*inline\s.*?(\w+)::(\w+)\s*\(/o){
         $_="//$_";
#         print STDERR "line $.: $_ deleted!\n";
         if($_ !~ /;/o){
            $par=0; $done=0;
            while(<F>){
               if(s|//.*$||o){ next;}
               s/([\{\}])/if($1 eq "{"){$done=1; $par++}else{$par--;}/goe;
               if($par==0 && $done){ last;}
            }
         }
         next;
      }elsif(/^\s*private:/o){
         $pr=0;
      }elsif(/^\s*\}\s*;/o){
         $lpar--;
         if($lpar==0){
            if($pr==0){ $pr=1;}
         }
         if($class eq "Parameters"){
            push @plines,$_;
            $class=$old_class;
            next;
         }elsif(scalar @plines>0){
            print $_;
            print join("",@plines);
            @plines=();
            next;
         }
      }
      s/MakeFriendsWithParsers//o;
      s/(\w+)\s+const\b/const $1 /go; # has to change here
      s/const\s+dbLayer\&\s+layer;/%readonly const dbLayer& layer;\n    %readwrite/o;
      s/std:://go;
      s/=\s*\w+\s*\(.*?\)//og;
      s/:\s*public\s+pair\<.*?\>//go;
      s/vector\<double\>\s+(\S+?)\s*,\s*(\S+?)\s*;/vector<double> $1; vector<double> $2;/o;
      if(/ostream\s*\&/o){ $_="//$_";}

      if($f eq "DB/DB.h" || $f eq "RBPlace/RBPlacement.h"){
         s/Parameters\b/${old_class}_Parameters/g;
         s/=${old_class}_Parameters\(\)//;
         if($class eq "Parameters" && $pr==1){
            push @plines,$_;
            next;
         }
      }
      if($pr){ print;}
   }
   close(F);
}

__END__;
#Geoms/plGeom.h
DB/dbDefines.h
#ABKCommon/abkcommon.h
DB/dbTypes.h
DB/dbRoutingElement.h
#Placement/symOri.h
DB/dbContext.h
DB/dbRoutedNet.h
#Placement/placement.h
DB/dbCell.h
DB/dbPin.h
DB/dbNet.h
DB/dbNetlist.h
DB/dbMasterVia.h
DB/dbRoutingInfo.h
DB/dbCommonPin.h
DB/dbTraverse.h
DB/dbDesign.h
#Geoms/intervalSeqLayer.h
DB/dbEasyAccess.h
#Placement/placeWOri.h
#Combi/mapping.h
#Constraints/constraints.h
DB/dbSpatial.h
#dbTemporal.h
DB/DB.h
RBPlace/RBPlacement.h
Capo/capoPlacer.h

