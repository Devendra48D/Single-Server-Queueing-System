
set style data lines

set title "Silnikov equation, various stepsize"
set xlabel "t"
set ylabel "x1"
set terminal postscript; set output "silnikov2.ps"
plot "silnikov2.dat"

set grid
set title "Silnikov equation, various stepsize (3D)"
set xlabel "t"
set ylabel "x1"
set zlabel "x2"
set terminal postscript; set output "silnikov2a.ps"
splot "silnikov2.dat"

set title "Silnikov equation, various stepsize (phase space)"
set xlabel "x1"
set ylabel "x2"
set terminal postscript; set output "silnikov2b.ps"
plot "silnikov2.dat"  using 2:3


