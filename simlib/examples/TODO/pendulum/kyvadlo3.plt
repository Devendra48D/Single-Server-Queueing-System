############################################################################
# Model:  Kyvadlo3
#
#set output "kyvadlo3.ps";  set term postscript 
set grid
set data style lines

set output "kyvadlo3a.png";  set term png 
set key
set title "Model kyvadla"
set xlabel "Cas";  set ylabel "uhel, uhlova rychlost"
plot "kyvadlo3.dat" using 1:2 title "uhel", \
     "kyvadlo3.dat" using 1:3 title "uhlova rychlost" with dots

set output "kyvadlo3b.png";  set term png 
set nokey
set title "model kyvadla - fazova rovina"
set xlabel "uhel";  set ylabel "uhlova rychlost"
plot "kyvadlo3.dat" using 2:3 

#end
