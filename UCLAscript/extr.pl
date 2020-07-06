#!/usr/bin/env perl
$f=$ARGV[0];
open(F,$f);
while(<F>){
   if(/^\s*#ifndef\s+_/o){ last;}
}
$pr=0;
while(<F>){
   if(/^\s*#/o){ next;}
   if(/^\s*class\s+/o){
      $pr=1;
   }elsif(/^\s*public:/o){
      $pr=1;
   }elsif(/^\s*private:/o){
      $pr=0;
   }elsif(/operator\s*[\>\<]+\s*\(/o){
      next;
   }elsif(/^\s*\};/o && $pr==0){
      $pr=1;
   }
   if($pr){ print;}
}
