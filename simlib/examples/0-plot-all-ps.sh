#!/usr/bin/env bash

# Plot model outputs using Gnuplot to Postscript
# we expect special formatting of Gnuplot scripts
#  1) '#set output ...; set term postscript ...'  will be uncommented
#  2) 'pause'    will be commented out

function plot2ps () {
  F=${1%%.plt}
  echo Plot: $F
  sed "
/postscript/s/^#*//
/pause/s/pause/#pause/
  " ${F}.plt | gnuplot
}

for i in *.plt; do
  plot2ps $i
done

# end
