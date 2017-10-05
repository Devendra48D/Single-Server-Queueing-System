############################################################################
# Model: Kolo
#

set output "wheel.ps"; set term postscript

set grid
set data style lines

set title "car and wheel dynamics"
set key
set xlabel "Time [s]"
set ylabel "y [m], y' [m/s],  x [m],  x' [m/s]"
plot "wheel.dat" using 1:2 title "x", \
     "wheel.dat" using 1:3 title "x'" with dots, \
     "wheel.dat" using 1:4 title "y", \
     "wheel.dat" using 1:5 title "y'" with dots
pause 1

set title "car dynamics - phase space"
set nokey
set xlabel "x [m]"
set ylabel "x' [m/s]"
plot [0:0.6] "wheel.dat" using 2:3 with lines, \
     "wheel.dat" using 4:5 with lines
pause 1

#end
