

outplot: speedup1.dat speedup2.dat plot.plt
	gnuplot -e "set terminal latex; set output 'outplot.tex'; load 'plot.plt'"
	
clean:
	del outplot.tex