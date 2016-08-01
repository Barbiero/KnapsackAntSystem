for(( i = 1; i < 32; i = i*2 )) do
	mpirun -np $i --hostfile hosts ./main -f mkuniverse_big.txt --iterations 1200 --ants 240
done

