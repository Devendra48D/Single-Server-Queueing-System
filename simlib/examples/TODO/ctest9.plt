############################################################################
# Model: ctest9
#

#set output "ctest9.prn"
#set term epson_60dpi

#set term emtex 
#set term latex 
#set output "x.t"
#set size 1.1,1.1

#set term vesa 259

set nogrid
set data style lines
set zero 1e-14

#set title "test"
set nokey
set xlabel "Time [s]"
set ylabel "z"
pause 0 "Loading..."
plot "ctest9.dat" using 1:2 title "z"
#plot "ctest9.da" using 1:2 title "z"
#set ylabel "step"
#plot "ctest9.da" using 1:3 title "step"

#end