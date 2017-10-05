#
# priklad pouziti programu ode z GNU Plotutils
#
F=.silnikov.ode
STEP="1e-5"  # pri zmene bude potreba opravit nasledujici AWK prikazy

cat >"$F" <<__END__
a = 0.4
b = 0.65
c = 0
d = 1

x1' = x2 
x1  = 0.1234 
x2' = x3
x2  = 0.2 
x3' = -a*x3 - x2 + b*x1*(1 - c*x1 - d*x1*x1)
x3  = 0.1

print t, x1, x2
step 0, 350
__END__

time ode --runge-kutta    $STEP  <$F | 
  awk '{if((NR-1)%5000==0)print}' >ode-rk.dat
time ode --adams-moulton  $STEP  <$F |
  awk '{if((NR-1)%5000==0)print}' >ode-am.dat

rm -f "$F"

gnuplot <<__END__
set key
set style data lines

#set terminal png large; set output "silnikov-ode.png"
set terminal postscript; set output "silnikov-ode.ps"
set title "Silnikov equation, various methods, step = $STEP"
plot [][-1.6:2.6] \
    "ode-rk.dat", \
    "ode-am.dat"

#set terminal png large; set output "silnikov-ode-phase.png"
set terminal postscript; set output "silnikov-ode-phase.ps"
set title "Silnikov equation, various methods, step = $STEP"
plot [-1.6:1.6][-1.6:1.6] \
    "ode-rk.dat" using 2:3, \
    "ode-am.dat" using 2:3

__END__
