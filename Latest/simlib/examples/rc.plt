############################################################################
# Model: RC
#

#reset 

#set output "rc.png"; set term png
#set output "rc.ps"; set term postscript landscape color

set grid
set style data lines

set title "RC circuit"
set key
set xlabel "Time [s]"
set ylabel "U [V]"
plot [][0:6] "rc.dat" using 1:2 title "Uin", \
             "rc.dat" using 1:3 title "Uout" 

pause 1

set title "Phase space"
set nokey
set xlabel "Uc [V]"
set ylabel "Ic [A]"
plot [][] "rc.dat" using 3:4

pause 1
#end
