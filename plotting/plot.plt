

set title "Tempo de Execucao"


set xlabel "Numero de Nos"
set ylabel "Speedup"
lin(x) = x

set style data linespoints


plot "speedup1.dat" title "Speedup relativo com duas máquinas" lt 5 lc 0 dt 2 pt 9, "speedup2.dat" title "Speedup relativo com uma máquina" lt -1 pt 7, lin(x) title "Linear" lc 0 dt 0
