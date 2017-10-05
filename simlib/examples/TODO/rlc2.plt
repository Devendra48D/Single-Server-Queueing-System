############################################################################
# Model: RC
#

#reset 

#set output "rlc.png"; set term png
#set output "rlc.ps"; set term postscript landscape color

set grid
set data style lines

set title "LC circuit"
set key
set xlabel "Time [s]"
set ylabel "U [V]"
plot [][] "rlc.dat" using 1:2 title "Uin", \
          "rlc.dat" using 1:3 title "Uout" 
pause 1
plot "rlc.dat" using 1:3 title "Uout", \
     "rlc.dat" using 1:(($5/1e6 - 1)*100) title "DeltaFreq", \
     "rlc.dat" using 1:6 title "local max", \
     "rlc.dat" using 1:7 title "max" 
pause 1

set title "LC circuit - phase space"
set nokey
set xlabel "Uout [V]"
set ylabel "I2 [A]"
plot [][] "rlc.dat" using 3:4

pause 1

#end
