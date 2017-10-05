
set style data lines

set terminal postscript; set output "silnikov2.ps"
plot "silnikov2.dat" index 3

set terminal postscript; set output "silnikov2a.ps"
splot "silnikov2.dat" index 3

set terminal postscript; set output "silnikov2b.ps"
plot "silnikov2.dat"  using 2:3 index 3 


