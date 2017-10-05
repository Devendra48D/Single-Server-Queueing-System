set style data lines
splot "silnikov2.dat"  index 0 with lines lt 1, \
      "silnikov2.dat"  index 1 with lines lt 2, \
      "silnikov2.dat"  index 3 with lines lt 3
pause -1
