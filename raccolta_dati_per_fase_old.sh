FILE="new_results/results.txt"
n_nodes=50
dim_pop=1000

for ((i=0;i<10;i++)); do ./tsp_sequential $n_nodes $dim_pop 1000 10; done | grep "Execution" | awk '{sum+=$3} END {print sum/NR}' >> $FILE


for nw in 1 2 4 8 16 32 64 128 256; do
    for portion in "Crossover" "Mutation" "Fitness" "Selection" "Sorting"; do
        for ((i=0;i<1;i++)); do ./tsp_parallel_thread $n_nodes $dim_pop 1000 10 $nw; done | grep $portion | awk '{sum+=$3} END {print sum/NR}' >> $portion.data.txt
    done
    for ((i=0;i<1;i++)); do ./tsp_parallel_thread $n_nodes $dim_pop 
done

# for ((i=0;i<10;i++)); do ./tsp_parallel_thread $n_nodes $dim_pop 1000 10 2; done | grep "Execution" | awk '{sum+=$3} END {print sum/NR}' >> $FILE
# for ((i=0;i<10;i++)); do ./tsp_parallel_thread $n_nodes $dim_pop 1000 10 4; done | grep "Execution" | awk '{sum+=$3} END {print sum/NR}' >> $FILE
# for ((i=0;i<10;i++)); do ./tsp_parallel_thread $n_nodes $dim_pop 1000 10 8; done | grep "Execution" | awk '{sum+=$3} END {print sum/NR}' >> $FILE
# for ((i=0;i<10;i++)); do ./tsp_parallel_thread $n_nodes $dim_pop 1000 10 16; done | grep "Execution" | awk '{sum+=$3} END {print sum/NR}' >> $FILE
# for ((i=0;i<10;i++)); do ./tsp_parallel_thread $n_nodes $dim_pop 1000 10 32; done | grep "Execution" | awk '{sum+=$3} END {print sum/NR}' >> $FILE
# for ((i=0;i<10;i++)); do ./tsp_parallel_thread $n_nodes $dim_pop 1000 10 64; done | grep "Execution" | awk '{sum+=$3} END {print sum/NR}' >> $FILE