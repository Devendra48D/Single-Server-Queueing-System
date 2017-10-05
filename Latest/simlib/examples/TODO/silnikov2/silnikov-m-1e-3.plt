
set key
set style data lines

#set terminal png large; set output "silnikov-all.png"
set terminal postscript; set output "silnikov-all.ps"
set title "Silnikov equation, various methods, step = 1e-3"
plot [][-1.6:2.6] \
    "silnikov-abm4.dat", \
    "silnikov-euler.dat", \
    "silnikov-fw.dat", \
    "silnikov-rke.dat", \
    "silnikov-rkf3.dat", \
    "silnikov-rkf5.dat", \
    "silnikov-rkf8.dat"

#set terminal png large; set output "silnikov-all-phase.png"
set terminal postscript; set output "silnikov-all-phase.ps"
set title "Silnikov equation, various methods, step = 1e-3"
plot [-1.6:1.6][-1.6:1.6] \
    "silnikov-abm4.dat" using 2:3, \
    "silnikov-euler.dat" using 2:3, \
    "silnikov-fw.dat" using 2:3, \
    "silnikov-rke.dat" using 2:3, \
    "silnikov-rkf3.dat" using 2:3, \
    "silnikov-rkf5.dat" using 2:3, \
    "silnikov-rkf8.dat" using 2:3


