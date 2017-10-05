############################################################################
# Model KOLO3, popis vìstupu pro program GNU Plot (gnuplot)
#

#set output "kolo3.prn"
#set term epson_60dpi

set grid
set data style lines

set title "dynamika systemu kola"
set key
set xlabel "Time [s]"
set ylabel "y [m],  v [m/s]"
pause 0 "Loading..."
plot "kolo3.dat" title "y", \
     "kolo3.dat" using 1:3 title "v" with dots
pause -1

set ylabel "y [m]"
pause 0 "Loading..."
plot "kolo3.dat" title "y"
pause -1

set title "dynamika systemu kola - fazova rovina"
set nokey
set xlabel "y [m]"
set ylabel "v [m/s]"
pause 0 "Loading..."
plot "kolo3.dat" using 2:3 with lines
pause -1

#end