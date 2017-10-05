############################################################################
# GNUplot script for model ctest
#
# Data format: Time y
#
reset  # defaults

#set output "ctest.ps"; set term postscript

set grid
set style data lines

set title "ctest - continuous system response"
set key
set xlabel "Time"
set ylabel "y"
plot "ctest.dat" title "y"
pause 1

#end
