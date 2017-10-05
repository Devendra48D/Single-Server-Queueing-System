ode --adams-moulton "1e-3" >ode-3.dat <<__END__
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


