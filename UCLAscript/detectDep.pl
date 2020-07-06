#!/usr/bin/env perl

$inc_sys=0;
if($ARGV[0] eq "-s"){
   print STDERR "Include system header files...\n";
   $inc_sys=1;
}
$args=join(" ",@ARGV);
open(F,"grep '#include' $args|");
while(<F>){
   if(/(\S+?):\s*#include\s+"(\S+?)"/o){
      $depend{$1}{$2}=1;
   }elsif($inc_sys && /(\S+?):\s*#include\s+\<(\S+?)\>/o){
      $h=$1; $dep=$2;
      $depend{$h}{$dep}=1
   }   
}
close(F);

for $f(keys %depend){
   print_h($f);
}
sub print_h{
   my ($h)=@_;
   my $d;
   if(exists $visited{$h}){
      return;
   }
   $visited{$h}=1;
   for $d(keys %{$depend{$h}}){
      print_h($d);
   }
   print "$h\n";
   return;
}