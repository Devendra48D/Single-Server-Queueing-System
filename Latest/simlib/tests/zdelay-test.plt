############################################################################
#
# data format: Time y1 y2
#
reset  # implicitní nastavení
set grid
set data style lines
set title "test of ZDelay"
set key
set xlabel "cas"
set ylabel "y1, y2"
plot "zdelay-test.out" title "y1" with dots, \
     "zdelay-test.out" using 1:3 title "y2" 
pause -1 "Press ENTER"
#end
