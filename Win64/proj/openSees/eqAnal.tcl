constraints Transformation
#test        NormDispIncr 1.0e-6 100 2
test NormUnbalance 1.0e-5 100 1
algorithm   Newton
system      ProfileSPD
integrator Newmark 0.5 0.25
analysis Transient


set factor [expr 0.1*$g];
set endTime [expr 30.0];
set dt [expr 0.005];
set fmt1 "%s Pushover analysis: CtrlNode %.3i, dof %.1i, Disp=%.4f %s";	# format for screen/file output of DONE/PROBLEM analysis
set IDctrlNode 13
set IDctrlDOF 1
set filePath "elcentro.txt"
set accelSeries "Series -dt 0.02 -filePath $filePath -factor $factor";	# define acceleration vector from file (dt=0.01 is associated with the input file gm)
pattern UniformExcitation 1 1 -accel $accelSeries;		# define where and how (pattern tag, dof) acceleration is applied
timeSeries Constant 1 1.0
    
set t [getTime]
set ok 0
set internalTimer 1


set RecorderFolder "TransformationHandler"
file mkdir $RecorderFolder
recorder Node -file "$RecorderFolder/FloorDisp.txt" -time  -node 11 12 13 -dof 1 disp 
recorder Node -file "$RecorderFolder/BaseShear.txt" -time  -node 10 20 30 40 50 -dof 1 reaction 
recorder Node -file "$RecorderFolder/FloorAcceleration.txt" -time -node 11 12 13 -dof 1 accel 


while {$t <= $endTime && $ok == 0} {
    set ok [analyze 1 $dt]
    set t [getTime]
    # if {$internalTimer >= $recorderDeltaT} {
    #     set gmGLF [getLoadFactor 1]
    #     puts $groundOut "$t	$gmGLF"
    #     set internalTimer 0
    # }
    # set internalTimer [expr $internalTimer + $dt]
}
#close $groundOut

if {$ok != 0 } {
	puts [format $fmt1 "PROBLEM" $IDctrlNode $IDctrlDOF [nodeDisp $IDctrlNode $IDctrlDOF] $LunitTXT]
} else {
	puts [format $fmt1 "DONE"  $IDctrlNode $IDctrlDOF [nodeDisp $IDctrlNode $IDctrlDOF] $LunitTXT]
}   

