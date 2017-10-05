# lorenz.plt
#
# GnuPlot script for model lorenz
#
# Data format: Time x y
#
reset  # defaults

#set output "lorenz1.png"; set term png small color
#set output "lorenz.ps";   set term postscript

set grid
set nokey
set title  "Lorenz equation"
set autoscale x
set xlabel "Time"
set ylabel "x"
set yrange [-20:20]
plot 'lorenz.dat' with lines
pause 1    # change to -1 if interactive

set xlabel "x"
set ylabel "y"
set yrange [-25:30]
plot 'lorenz.dat' using 2:3 with lines
pause 1

set xlabel "x"
set ylabel "y"
set zlabel "z"
set yrange [-25:30]
splot 'lorenz.dat' using 2:3:4 with lines
pause 1

# end
