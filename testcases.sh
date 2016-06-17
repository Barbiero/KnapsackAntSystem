
for(( t=1; t<32; t*=2 ))
do
    echo ${t} Threads

    for (( n=0; n<10; n++ )) 
    do
        echo ${n} run
        ./main --min_restrictions '0.5 10.0' --max_restrictions '1000.0 50000.0' --knap-cap '10000 200000' --items 10000 --iterations 600 -T ${t} > output/${t}_output${n}.txt
    done
done
