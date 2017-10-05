############################################################################
# GnuPlot script for model KOLO2
#
# Data format: Time y v
#
reset  # defaults

#set output "kolo2.ps"; set term postscript

set grid
set style data lines

set title "Wheel dynamics"
set key
set xlabel "Time [s]"
set ylabel "y [m],  v [m/s]"
plot "kolo2.dat" title "y", \
     "kolo2.dat" using 1:3 title "v" with dots
pause 1

set ylabel "y [m]"
plot "kolo2.dat" title "y"
pause 1

set title "Wheel dynamics - phase space"
set nokey
set xlabel "y [m]"
set ylabel "v [m/s]"
plot "kolo2.dat" using 2:3 with lines
pause 1

#end
