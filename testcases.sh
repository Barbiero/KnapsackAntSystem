for(( t=1; t < 32; t *= 2))
do
    echo ${t} Threads

    for (( n=0; n<10; n++ )) 
    do
        echo ${n} run
        ./main -f mkuniverse_big.txt --iterations 1200 --ants 240 -T ${t}  > output/${t}_output${n}.txt
    done
done

echo Sequential

for((n=0; n < 10; n++ ))
do
    echo ${n} run
    ./main_seq -f mkuniverse_big.txt --iterations 1200 --ants 240 > output/s_output${n}.txt
done
