
set terminal png large; set output "silnikov2.png"

set style data lines
set title "Silnikov equation, RKE method, various step sizes"
plot "silnikov2.dat"  index 0 title "default step" with lines lt 1, \
     "silnikov2.dat"  index 1 title "maxstep = 1e-3"  with lines lt 2, \
     "silnikov2.dat"  index 3 title "maxstep = 1e-6"  with lines lt 3
pause -1 " press ENTER "
