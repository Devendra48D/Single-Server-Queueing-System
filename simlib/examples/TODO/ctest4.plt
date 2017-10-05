############################################################################
# Model: ctest4
#

#set output "ctest4.prn"
#set term epson_60dpi

set grid
set data style lines
set zero 1e-14

set title "test"
set key
set xlabel "Time [s]"
set ylabel "delta, step size"
pause 0 "Loading..."
plot "ctest4.dat" using 1:2 title "delta", \
     "ctest4.dat" using 1:3 title "step size" with dots
pause -1

set ylabel "delta"
pause 0 "Loading..."
plot "ctest4.dat" using 1:2 title "delta" 
pause -1

#end