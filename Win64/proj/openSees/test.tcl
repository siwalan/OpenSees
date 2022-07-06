# units: kip, in

# Remove existing model
wipe

# Create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Create nodes
# ------------
# Create nodes & add to Domain - command: node nodeId xCrd yCrd
node 1   0.0  0.0
node 2 144.0  0.0
node 3 168.0  0.0
node 4  72.0 96.0
    
# Set the boundary conditions - command: fix nodeID xResrnt? yRestrnt?
fix 1 1 1 
fix 2 1 1
fix 3 1 1
    
# Define materials for truss elements
# -----------------------------------
# Create Elastic material prototype - command: uniaxialMaterial Elastic matID E
uniaxialMaterial Elastic 1 3000

# 
# Define elements
#

# Create truss elements - command: element truss trussID node1 node2 A matID
element Truss 1 1 4 10.0 1
element Truss 2 2 4 5.0 1
element Truss 3 3 4 5.0 1
        
constraints Transformation
numberer    Plain
test        NormDispIncr 1.0e-6 100 2
algorithm   Newton
system      ProfileSPD
integrator Newmark 0.5 0.25
analysis Transient

set factor [expr 1.0];
set endTime [expr 30.0];
set dt [expr 0.02];
set filePath "elcentro.txt"
set accelSeries "Series -dt 0.02 -filePath $filePath -factor $factor";	# define acceleration vector from file (dt=0.01 is associated with the input file gm)
pattern UniformExcitation 1 1 -accel $accelSeries;		# define where and how (pattern tag, dof) acceleration is applied
timeSeries Constant 1 

timeSeries Constant 2
pattern Plain 2 2 {	
   # Create the nodal load - command: load nodeID xForce yForce
   load 4 100 -50
}

# recorder LoadRecorder -file test.txt -time  -pattern 2
# recorder LoadRecorder -file test.txt -time  -pattern 1


# set t [getTime]
# set ok 0

# while {$t <= $endTime && $ok == 0} {
#     set ok [analyze 1 $dt]
#     set t [getTime]
# }