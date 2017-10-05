
set nokey
set style data lines

set terminal png large; set output "silnikov.png"
set title "Silnikov equation, RKE method, maxstep = 1e-3"
plot [][-1.6:1.6] "silnikov.dat" 

set terminal png large; set output "silnikov-phase.png"
set title "Silnikov equation phase space"
plot [-1.6:1.6][-1.6:1.6] "silnikov.dat" using 2:3

