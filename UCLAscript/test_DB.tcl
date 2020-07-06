load ./UCLAscript.so UCLAscript
puts "UCLAscript loaded OK!"
DB_Parameters params
params configure -alwaysCheckConsistency 1
DB db "./alt0.aux" [params cget -this]
dbNetlist netlist -this [db getNetlist]
puts "Testcase: [db getAuxName]"
puts "Total CCs [netlist getNumCCs]"
puts "Total SCs [netlist getNumSCs]"
puts "Total AllPads [netlist getNumAllPads]" 
netlist printNetStats
