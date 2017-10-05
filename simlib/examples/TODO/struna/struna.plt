#    
#set terminal postscript eps color; set output "struna.eps"
#set terminal postscript eps ; set output "struna.eps"
unset key
unset border
unset xzeroaxis
unset yzeroaxis
unset xtics
unset ytics
unset ztics

set isosamples 41
#set hidden3d
set pm3d
set view 70, 20, 1.1, 0.5
set nokey
set style data lines
splot "struna.dat" #, 0

pause -1 "enter"
#    EOF
