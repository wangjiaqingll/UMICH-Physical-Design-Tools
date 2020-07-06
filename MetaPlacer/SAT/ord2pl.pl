#!/usr/bin/perl  

# Created By Igor Markov, Winter 2001

# This script converts a linear ordering into a "placement" (a .pl file)
# which can be plotted

@a=();
$i=1;

print "UCLA pl 1.0\n";

while(<>)
{
   next if /^\s*$/;
   die "Expected single integers on input\n" unless /^\s*(\d+)\s*$/;
   $a[$1]=$i++;
}

for($i=1; $i!=@a; $i++)
{ print "    v$i   ",$a[$i],"      0 \n"; }
