
set key
set style data lines

#set terminal png large; set output "silnikov-all.png"
#set terminal postscript; set output "silnikov-all.ps"
set title "Silnikov equation, various methods, step = 1e-5"
plot [][-1.6:2.6] \
    "ode-rk.dat", \
    "ode-am.dat"

pause -1 
#set terminal png large; set output "silnikov-all-phase.png"
#set terminal postscript; set output "silnikov-all-phase.ps"
set title "Silnikov equation, various methods, step = 1e-5"
plot [-1.6:1.6][-1.6:1.6] \
    "ode-rk.dat" using 2:3, \
    "ode-am.dat" using 2:3

pause -1 

