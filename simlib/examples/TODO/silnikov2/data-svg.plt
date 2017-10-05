
set terminal svg; set output "silnikov-m1.svg"

set xlabel "t"
set ylabel "x1"
set xrange [200:]
set yrange [-1.7:2.2]
set style data lines
set title "Silnikov equation, various methods, maxstep=1e-3"
plot "data"  index 0 title "abm4"  with lines lt 1, \
     "data"  index 1 title "rkf5"  with lines lt 2, \
     "data"  index 2 title "rkf8"  with lines lt 3, \
     "data"  index 3 title "rke"   with lines lt 4
#pause -1 " press ENTER "

set terminal svg; set output "silnikov-m2.svg"

set xlabel "x1"
set ylabel "x2"
set xrange [-1.5:1.5]
set yrange [-1.5:1.5]
set style data lines
set title "Silnikov equation, various methods, maxstep=1e-3"
plot "data"  index 0 using 2:3 title "abm4"  with lines lt 1, \
     "data"  index 1 using 2:3 title "rkf5"  with lines lt 2, \
     "data"  index 2 using 2:3 title "rkf8"  with lines lt 3, \
     "data"  index 3 using 2:3 title "rke"   with lines lt 4
#pause -1 " press ENTER "
