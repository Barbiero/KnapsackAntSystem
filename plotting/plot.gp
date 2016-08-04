set terminal latex
set output "out.tex"
set title "Tempo de Execucao"
set ylabel "Speedup"
set xlabel "Numero de Nos"

lin(x) = x

set grid
plot "speedup.data" title "", lin(x) title "Linear"
