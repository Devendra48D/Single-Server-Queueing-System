#
# Model: druzice
#
#set output "3d-test.ps";  set term postscript
#set output "druzice.eps"
#set term corel 
#set output "druzice.obj"
#set term  tgif [2,2]
set grid
set title "Druzice a 2 hmotna telesa"
set nokey
set xlabel "x [1000 km]"
set ylabel "y [1000 km]"
set zlabel "z [1000 km]"
set data style lines
#set data style dots
pause 0 "Loading..."
plot "3d-test.out" using 2:3, "" using 5:6 
pause -1 "enter"
splot "3d-test.out" using 2:3:4, "" using 5:6:7 
pause -1 "enter"
pause 0 "Hotovo."
