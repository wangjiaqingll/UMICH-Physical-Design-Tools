#!/usr/bin/env perl -Iblib/arch
use UCLAscript;
package UCLAscript;
print "UCLAscript loaded OK!\n";
while(scalar @ARGV>0){
   $arg=shift @ARGV;
   if($arg eq "-f"){
      $auxFileName=shift @ARGV;
   }elsif($arg eq "-savePl"){
      $savePl=shift @ARGV;
   }elsif($arg =~ /^-h/o){
      printHelp();
      exit 1;
   }
}
if(!defined $auxFileName){
   printHelp();
   exit 1;  
}
$rbParam=new RBPlacement_Parameters(0,undef);
$rbplace=new RBPlacement($auxFileName, $rbParam);
$capoParams=new CapoParameters();
#$capoTimer=new Timer();
$capo=new CapoPlacer($rbplace, $capoParams);
#$capoTimer->stop();

#print "  Capo Runtime: ",$capoTimer->getUserTime(), "\n";
print "  Origin-to-Origin HPWL: ",$rbplace->evalHPWL(0),"\n";
print "  Pin-to-Pin       WPWL: ",$rbplace->evalHPWL(1),"\n";
print "  Origin-to-Origin  WWL: ",$rbplace->evalWeightedWL(0),"\n";
print "  Pin-to-Pin        WWL: ",$rbplace->evalWeightedWL(1),"\n";

if(defined $savePl){
   $rbplace->savePlacement($savePl);
}



sub printHelp{
   print << "END";
Usage: $0 \\
 -h         help
 -help      help
 -f <auxFilename>   auxfile for design to place
 -savePl <fileName> save resulting placement
END
}

