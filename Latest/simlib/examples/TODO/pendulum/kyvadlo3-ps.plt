#!/usr/bin/env gnuplot
############################################################################
# Model: kyvadlo3.cc
#

set term postscript; set output "kyvadlo3.ps"; #set size 1,2 # asi 1:1
#set term png color; set output "kyvadlo3-1.png"

set grid
set data style lines

set key box
set title "model netlumeneho kyvadla"
set xlabel "Time";  set ylabel "uhel, uhlova rychlost"
set yrange [-2.2:11]
plot "kyvadlo3.dat" using 1:2 title "uhel", \
     "kyvadlo3.dat" using 1:3 title "uhlova rychlost" with dots
#pause -1 "Press Enter"

#set output "kyvadlo3-2.png"
set nokey
set title "model netlumeneho kyvadla - fazova rovina"
set xlabel "uhel";  set ylabel "uhlova rychlost"
set data style lines
set yrange [-3:3]
plot "kyvadlo3.dat" using 2:3 
#ause -1 "Press Enter"

#end
