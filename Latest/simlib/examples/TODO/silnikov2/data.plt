
#set terminal png large; set output "silnikov-m.png"

set style data lines
set title "Silnikov equation, various methods, maxstep=1e-3"
plot "data"  index 0 title "abm4"  with lines lt 1, \
     "data"  index 1 title "rkf5"  with lines lt 2, \
     "data"  index 2 title "rkf8"  with lines lt 3, \
     "data"  index 3 title "rke"   with lines lt 4
pause -1 " press ENTER "
