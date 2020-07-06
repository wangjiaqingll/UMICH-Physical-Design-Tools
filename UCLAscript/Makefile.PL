#!/usr/bin/env perl
use ExtUtils::MakeMaker;
$version="0.03";
$cxx="-c++";
$shadow="-shadow";
$compat="";
$language="python";
$module="UCLAscript";
$pkg="UCLAscript";
$noSWIG=0;
while($ARGV[0] =~ /^-/o){
   $arg=shift @ARGV;
   if($arg eq "-v"){
      $version=shift @ARGV;
   }elsif($arg =~ /^-py/o){
      $language="python";
   }elsif($arg =~ /^-perl/o){
      $language="perl5";
      $compat="-compat";
   }elsif($arg =~ /^-tcl/o){
      $language="tcl8";
      $shadow="";
   }elsif($arg eq "-c++"){
      $cxx="-c++";
   }elsif($arg eq "-shadow"){
      $shadow="-shadow";
   }elsif($arg eq "-compat"){
      $compat="-compat";
   }elsif($arg eq "-pkg"){
      $pkg=shift @ARGV;
   }elsif($arg eq "-noSWIG"){
      $noSWIG=1;
   }
}

if(scalar @ARGV>0){
   $module=shift @ARGV;
   if(!defined $module){
      print STDERR "Please specify a module name!\n";
      exit 1;
   }
}
system "touch h2i.pl";

$CC="gcc";
$csuffix="";
if($cxx eq "-c++"){
   $CC="g++";
}
if($shadow ne ""){
   $csuffix="c";
   if(-e "${module}.so"){
      print "mv ${module}.so ${module}.so.bak\n";
      system "mv ${module}.so ${module}.so.bak\n";
   }
}

$inc=""; $lib_a=""; $lib_L="";
if($language eq "perl5"){
   if($noSWIG){
      print "Copy pre-generated UCLAscript_wrap.c\n";
      system '\cp -f wrap4perl/* .';
   }
   $inc=`perl -MExtUtils::Embed -e ccopts`; chop $inc;
   $lib_a=`perl -MExtUtils::Embed -e ldopts`; chop $lib_a;
   if(!-e "test_$module.pl"){
      open(F,">test_$module.pl");
      print F << "END";
#!/usr/bin/env perl -Iblib/arch
use $module;\npackage $module;
print "$module loaded OK!\n";
END
      close F;
   }
}elsif($language eq "python"){
   if($noSWIG){
      print "Copy pre-generated UCLAscript_wrap.c\n";
      system '\cp -f wrap4python/* .';
   }
   $inc='-I'.`python -c 'import distutils.sysconfig; print distutils.sysconfig.get_python_inc()'`;
   chop $inc; #print "INC=$inc\n";
   $lib_a=`python -c 'import distutils.sysconfig; print distutils.sysconfig.get_config_vars("LIBS")'`;
   $lib_a=@{eval($lib_a)}[0];
   $lib_L=`python -c 'import distutils.sysconfig; print distutils.sysconfig.get_config_vars("DESTLIB")'`;
   $lib_L="-L".@{eval($lib_L)}[0]; $lib_L.="/config";

   if(!-e "test_$module.py"){
      open(F,">test_$module.py");
      print F << "END";
#!/usr/bin/env python
from $module import *
print "$module loaded OK!"
END
      close F;
   }
}elsif($language eq "tcl8"){
   if($noSWIG){
      print "Copy pre-generated UCLAscript_wrap.c\n";
      system '\cp -f wrap4tcl/UCLAscript_wrap.c .';
   }
   $lib_L=`tclsh <<! \
puts -nonewline \\\$tcl_pkgPath
!`;
   $inc='-I'.$lib_L; 
   $lib_L="-L".$lib_L;
   $inc =~ s|/([^/])+/?$|/include|o;
}

WriteMakefile(
  'NAME' => $module,
  #'MYEXTLIB' => '-lDB -lc -lm -L../../lib \
  #           -lParserLEFDEF -lConstraints -lPlacement  -lCtainers \
  #           -lStats -lCombi -lGeoms -lABKCommon  -lfl',
  'INC' => $inc." -I.. -DHAVE_CONFIG_H -I/usr/local/include",
  'LIBS' => [$lib_L.' -L/usr/local/lib -lm'],
  'OBJECT'=> "${module}_wrap.o",
  'LD' => $CC,
  'LDDLFLAGS' => '-shared -lc -lm -L../lib \
                   -lMLPart -lFMPart -lPartEvals -lPartitioning -lPartitioners\
                   -lNetTopology -lPartLegality\
                   -lDB -lABKCommon -lConstraints -lPlacement -lStats -lHGraph\
                   -lParserLEFDEF -lfl\
                   -lCombi -lPlaceEvals -lClusteredHGraph -lSmallPart \
                   -lSmallPlacers -lRBPlace -lRBPlFromDB -lHGraphWDims\
                   -lGeomTrees -lGeoms -lCtainers -lSmallPlacement -lShellPart\
                   -lGenHier -lFilledHier -lCapo '.
                   "-Xlinker -rpath -Xlinker $ENV{PWD}/../lib ",
#  'CCFLAGS' => '-g -Dbool=char',
  'CCFLAGS' => ' ',
  'OPTIMIZE' => ' ',
  'VERSION' => $version,
  'CC' => $CC
);


sub MY::postamble {

$swig="";
if(!$noSWIG){
   $swig="$module.i :	 h2i.pl
	perl h2i.pl > $module.i
${module}_wrap.c :: $module.i 
	swig -$language $cxx $shadow $compat $module.i\n";
}

return << "END";

perl:: ${module}_wrap.o pure_all

python:	${module}_wrap.o
	\$(LD) \$(LDDLFLAGS) \$(OBJECT) \$(EXTRALIBS) \$(MYEXTLIB) -o ${module}${csuffix}.so

tcl:	${module}_wrap.o
	\$(LD) \$(LDDLFLAGS) \$(OBJECT) \$(EXTRALIBS) \$(MYEXTLIB) -o ${module}.so

$swig

test_py:	python
	python test_$module.py

test_pl:	all
	perl test_$module.pl

test_tcl:	tcl
	tclsh test_$module.tcl
tar:	
	(\\rm -rf blib; \\rm -rf include; \\rm *.bs *.so *.pm pm_to_blib tmp* *~ \\
         $module.pyc $module.py seeds.* $module.i lex.*.c *.tab.* core *_wrap.* *.so *.o *.doc Makefile Makefile.old; \\
         cd ..;tar cvf $pkg-$version.tar $pkg-$version; \\
         gzip $pkg-$version.tar)
END
}

