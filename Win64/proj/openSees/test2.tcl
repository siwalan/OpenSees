# SET UP ----------------------------------------------------------------------------
wipe;						# clear opensees model
model basic -ndm 2 -ndf 3;				# 2 dimensions, 3 dof per node
file mkdir data; 					# create data directory

# define GEOMETRY -------------------------------------------------------------
# nodal coordinates:
node 1 0 0;					# node#, X Y
node 2 504 0
node 3 0 432
node 4 504 432 

# Single point constraints -- Boundary Conditions
fix 1 1 1 1; 			# node DX DY RZ
fix 2 1 1 1; 			# node DX DY RZ
fix 3 0 0 0
fix 4 0 0 0

# nodal masses:
mass 3 5.18 0. 0.;					# node#, Mx My Mz, Mass=Weight/g.
mass 4 5.18 0. 0.

# Define ELEMENTS -------------------------------------------------------------
# define geometric transformation: performs a linear geometric transformation of beam stiffness and resisting force from the basic system to the global-coordinate system
geomTransf Linear 1;  		# associate a tag to transformation

# connectivity: (make A very large, 10e6 times its actual value)
element elasticBeamColumn 1 1 3 3600000000 4227 1080000 1;	# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag
element elasticBeamColumn 2 2 4 3600000000 4227 1080000 1
element elasticBeamColumn 3 3 4 5760000000 4227 4423680 1

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

timeSeries Linear 2
pattern Plain 2 2 {	
   # Create the nodal load - command: load nodeID xForce yForce
   load 3 100 -50 0
   load 4 200 -75 0

}
