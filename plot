#!/usr/bin/gnuplot

set datafile separator ","
set xlabel "Target Sector"
set ylabel "Time (s)"
plot 'results.csv' using 1:2 axes x1y1 title "Random Access Seek Time"
