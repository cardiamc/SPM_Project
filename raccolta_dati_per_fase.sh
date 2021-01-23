FILE="DEFINITIVE_RESULTS/seq_results.txt"
n_nodes=50
dim_pop=1000

# for ((i=0;i<10;i++)); do ./tsp_sequential $n_nodes $dim_pop 1000 10; done | grep "Execution" | awk '{sum+=$3} END {print sum/NR}' >> $FILE

# #set up appropriate flags

# for portion in "Crossover" "Mutation" "Fitness" "Selection" "Sorting"; do
#     FILE="DEFINITIVE_RESULTS/${portion}_thread.txt"
#     for nw in 1 2 4 8 16 32 64 128 256; do
#         echo "NW : $nw" >> $FILE
#         for ((i=0;i<10;i++)); do ./tsp_parallel_thread $n_nodes $dim_pop 1000 10 $nw; done | grep $portion | awk '{sum+=$3} END {print sum/NR}' >> $FILE
#     done
#     echo "--------------------------------------------------------------" >> $FILE
# done

# for portion in "Crossover" "Mutation" "Fitness" "Selection" "Sorting"; do
#     FILE="DEFINITIVE_RESULTS/${portion}_ff.txt"
#     for nw in 1 2 4 8 16 32 64 128 256; do
#         echo "NW : $nw" >> $FILE
#         for ((i=0;i<10;i++)); do ./tsp_parallel_ff $n_nodes $dim_pop 1000 10 $nw; done | grep $portion | awk '{sum+=$3} END {print sum/NR}' >> $FILE
#     done
#     echo "--------------------------------------------------------------" >> $FILE
# done

#to gather overhead re do these tests with appropriate flags

FILE="DEFINITIVE_RESULTS/parallel_thread_overhead.txt"
for nw in 1 2 4 8 16 32 64 128 256; do
    echo "NW : $nw" >> $FILE
    for ((i=0;i<10;i++)); do ./tsp_parallel_thread $n_nodes $dim_pop 1000 10 $nw; done | grep "Execution" | awk '{sum+=$3} END {print sum/NR}' >> $FILE
    echo "---------------------------------------------------------------" >> $FILE
done

FILE="DEFINITIVE_RESULTS/parallel_ff_overhead.txt"
for nw in 1 2 4 8 16 32 64 128 256; do
    echo "NW : $nw" >> $FILE
    for ((i=0;i<10;i++)); do ./tsp_parallel_ff $n_nodes $dim_pop 1000 10 $nw; done | grep "Execution" | awk '{sum+=$3} END {print sum/NR}' >> $FILE
    echo "---------------------------------------------------------------" >> $FILE
done