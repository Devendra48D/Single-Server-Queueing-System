############################################################################
# Model: 
#

set nokey
set grid

set xlabel "Time"
set ylabel "x"
pause 0 "Loading..."
plot "rovnice1.dat" using 1:2, "rovnice1.dat" using 1:3 with dots

set title "fazova rovina"
set xlabel "x"; 
set ylabel "x'"
pause 0 "Loading..."
plot "rovnice1.dat" using 2:3 with lines

#end