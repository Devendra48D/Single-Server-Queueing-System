############################################################################
# Model: RLC
#

reset 

#set output "rlc.png"; set term png
#set output "rlc.ps"; set term postscript landscape color

set grid
set style data lines

set title "RLC circuit"
set key
set xlabel "Time [s]"
set ylabel "U [V]"
plot [][] "rlc.dat" using 1:2 title "Uout" 
pause 1

set title "RLC circuit - phase space"
set nokey
set xlabel "U [V]"
set ylabel "Il [A]"
plot [][] "rlc.dat" using 2:3

pause 1

#end
