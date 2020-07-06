#!/usr/bin/env perl 
BEGIN {unshift @INC,(".","blib/arch");}
use UCLAscript;
package UCLAscript;
$params=new DB_Parameters();
$params->{alwaysCheckConsistency}=1;
$db=new DB("./alt0.aux",$params);
$netlist=$db->getNetlist();
print "Testcase: ",$db->getAuxName(),"\n";
print "Total CCs ",$netlist->getNumCCs(),"\n";
print "Total SCs ",$netlist->getNumSCs(),"\n";
print "Total AllPads ",$netlist->getNumAllPads(),"\n"; 
$netlist->printNetStats();

