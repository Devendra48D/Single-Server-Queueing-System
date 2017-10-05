############################################################################
# Model: ball2
#
reset  # defaults

#set output "ball2.ps"; set term postscript

set style data lines
#set grid

set title "Bouncing ball"
set key
set xlabel "Time [s]"
set ylabel "Height [m],  Speed [m/s]"
plot "ball2.dat" using 1:2 title "Height", \
     "ball2.dat" using 1:3 title "Speed", \
     0 notitle with dots
pause 1

set title "Bouncing ball - phase space"
set nokey
set xlabel "Height [m]"
set ylabel "Speed [m/s]"
set ytics
plot "ball2.dat" using 2:3
pause 1

set title "Bouncing ball - error of detection"
set style data linespoints
set nokey
set zero 1e-12
set ylabel "Height [m]"
set ytics (0,-1e-9)
plot [] [-1e-9:0] "ball2.dat" using 1:2 title "Height"
pause 1

#end
