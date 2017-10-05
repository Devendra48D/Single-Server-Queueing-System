############################################################################
# Gnuplot script for model multiexp
#
# Data format: RunNumber QueueLen_at_time500 at1000 at1500 at2000
#
reset  # defaults

#set output "multiexp.png"; set term png small color
#set output "multiexp.ps"; set term postscript 

set style data linespoints
set grid
set key
set title "Model: multiexp.cc "
set xlabel "Mean value of arrival intervals"
set ylabel "Average total time"
set yrange [0:500]
plot "multiexp.dat" using 1:2 title "t=500" lw 2, \
     "multiexp.dat" using 1:3 title "t=1000" lw 2, \
     "multiexp.dat" using 1:4 title "t=1500" lw 2, \
     "multiexp.dat" using 1:5 title "t=2000" lw 2, \
     10 title "service time" with dots lw 3
pause 1

#end
