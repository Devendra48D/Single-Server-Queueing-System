
set style data lines
plot "delay-test2.out" using 1:2 title "delayed", \
     "delay-test2.out" using 1:($3*1e13) title "error * 1e13", \
     "delay-test2.out" using 1:($4*100) title "diffierence of integrals * 100"

pause -1
