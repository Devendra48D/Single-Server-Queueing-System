############################################################################
# GnuPlot script for model TOPENI
#
# Data format: Time  temperature
#
reset  # defaults

#set output "topeni.ps"; set term postscript

set grid
set style data lines

set title "Heating system response"
set key
set xlabel "Time"
set ylabel "t"
plot "topeni.dat" using 1:2 title "Temperature", "" using 1:3 title "Relay" with steps lw 2 lt 1

pause 1
#pause -1 "stisknete ENTER"

#end
