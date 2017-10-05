############################################################################
# Model: BALL2
#

#set output "ball2.prn"
#set term epson_60dpi

#set data style dots
#set grid

set title "Bouncing ball"
set key
set xlabel "Time [s]"
set ylabel "height [m],  speed [m/s^2]"
pause 0 "Loading..."
plot [19.3:19.4] [] "ball2.dat" using 1:2 with linespoints title "height"
#plot [18:20] [] "ball2.dat" using 1:2 title "height"#, "ball2.dat" using 1:3 title "speed" 
pause -1

set title "Bouncing ball - detection error"
set nokey
set zero 1e-12
set ylabel "height [m]"
set ytics (0,-1e-9)
plot [] [-1e-9:0] "ball2.dat" using 1:2 title "height"
pause -1

set title "Bouncing ball - phase space"
set nokey
set xlabel "height [m]"
set ylabel "speed [m/s^2]"
set ytics
plot "ball2.dat" using 2:3 
pause -1

#end
