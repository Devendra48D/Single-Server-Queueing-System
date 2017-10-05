############################################################################
# Model: ctest8
#

#set output "ctest8.prn"
#set term epson_60dpi

set grid
set data style lines
set zero 1e-14

set title "test"
set key
set xlabel "Time [s]"
set ylabel "delta, stepsize"
pause 0 "Loading..."
plot "ctest8.dat" using 1:2 title "delta", \
     "ctest8.dat" using 1:3 title "step" with dots

pause -1
set ylabel "delta"
pause 0 "Loading..."
plot "ctest8.dat" using 1:2 title "delta" with dots

pause -1
#end