############################################################################
# Model:  Schema, Schema2
#

set output "schema.ps"; set term postscript

set style data lines
set grid
#########################################
set key
set title "System dynamics"
set xlabel "Time";  set ylabel "x, y"
plot "schema.dat" using 1:2 title "x", \
     "schema.dat" using 1:3 title "y"
pause 1
#########################################
set nokey
set title "Phase space"
set xlabel "x";  set ylabel "y"
plot "schema.dat" using 2:3, \
     "schema.dat" using 2:4
pause 1
#end
