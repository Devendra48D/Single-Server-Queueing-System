############################################################################
# Model: druzice
#

#set output "druzice.prn"
#set term epson_60dpi

#set grid

set title "Druzice a 2 hmotna telesa"
set nokey
set xlabel "x [1000 km]"
set ylabel "y [1000 km]"
pause 0 "Loading..."
plot [-10:4] "druzice.dat" using 2:3

pause -1

plot [-0.1:0.1] [-0.1:0.1] "druzice.dat" using 2:3

pause -1