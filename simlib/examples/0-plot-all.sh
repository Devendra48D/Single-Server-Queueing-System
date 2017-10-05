#!/usr/bin/env bash

# Plot model outputs using GNUplot

for i in *.plt; do
    gnuplot $i
done

# end
