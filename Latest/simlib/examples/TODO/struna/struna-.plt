#    
#set terminal postscript eps color; set output "struna.eps"
set terminal postscript eps ; set output "struna.eps"
unset key
#unset border
#unset xzeroaxis
#unset xtics
#unset ytics


set style data lines
set xlabel "x"
set ylabel "y"
plot [][-.03:.03] "struna.dat" using 2:3

pause -1 "enter"
#    EOF
