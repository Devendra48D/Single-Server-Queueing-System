############################################################################
# Model:  Kyvadlo
#
#set output "kyvadlo.prn"
#set term epson_60dpi
#set size 1,2                  # pomer asi 1:1
set grid

#set data style dots
set key
set title "model kyvadla"
set xlabel "Time";  set ylabel "uhel, uhlova rychlost"
pause 0 "Loading..."
plot "kyvadlo.dat" using 1:2 title "uhel", \
     "kyvadlo.dat" using 1:3 title "uhlova rychlost" with dots

pause -1 "dalsi"
set nokey
set title "model kyvadla - fazova rovina"
set xlabel "uhel";  set ylabel "uhlova rychlost"
pause 0 "Loading..."
###plot [4.5:5.5][-.5:.5] "kyvadlo.dat" using 2:3 
plot "kyvadlo.dat" using 2:3 

pause -1 "konec"

#end
