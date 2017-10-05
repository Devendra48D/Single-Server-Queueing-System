############################################################################
# Model: bessel
#
# GNUplot script
#
# Required data format: Time y y'
#
reset  # defaults

#set output "bessel.ps"; set term postscript

set grid
set style data lines

set title "Bessel equation"
set key
set xlabel "Time"
set ylabel "y, y'"
plot "bessel.dat" title "y", \
     "bessel.dat" using 1:3 title "y'" with dots
pause 1

set title "Bessel equation - phase space"
set nokey
set xlabel "y"
set ylabel "v"
plot "bessel.dat" using 2:3 with lines
pause 1

#end
