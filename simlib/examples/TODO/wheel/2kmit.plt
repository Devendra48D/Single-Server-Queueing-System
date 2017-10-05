############################################################################
# popis vystupu pro program GnuPlot (gnuplot)

set output "2kmit.ps"; set terminal postscript
set data style lines

set title "dynamika systemu"
set key
set xlabel "Time [s]"
set ylabel "y1 [m],  y2 [m]"
plot "2kmit.dat" using 1:2 title "y1", \
              "" using 1:3 title "y2"

set title "fazova rovina"
set nokey
set xlabel "y1 [m]"
set ylabel "v1 [m/s]"
plot "2kmit.dat" using 2:4 #, "" using 3:5

#end
