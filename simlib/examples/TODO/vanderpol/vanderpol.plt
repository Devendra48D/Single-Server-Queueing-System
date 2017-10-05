############################################################################
# Model: vanderpol.cc
#

set output "vanderpol.ps"; set term postscript
#set size 1,2

set nokey
set grid

set title "Van der Pol equation (various initial conditions)"
set xlabel "Time"
set ylabel "x"
plot "vanderpol.dat" using 1:2 with lines, \
     "vanderpol.dat" using 1:3 with dots
pause 1

set title "Van der Pol equation - phase space"
set xlabel "x"; set xrange [-4:4]
set ylabel "x'"
plot "vanderpol.dat" using 2:3 with lines
pause 1

#end
