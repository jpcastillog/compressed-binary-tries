# ./queries /mnt/d/data/Gov2Flat/gov2_rank_v_runs_t.bin /mnt/d/data/Gov2Flat/1mq.txt --rank v --runs t --out /mnt/d/data/gov2_rank_v_runs_t_results.csv
# ./queries /mnt/d/data/Gov2Flat/gov2_rank_v5_runs_t.bin /mnt/d/data/Gov2Flat/1mq.txt --rank v5 --runs t --out /mnt/d/data/gov2_rank_v5_runs_t_results.csv
# ./queries /mnt/d/data/ClueWeb09Flat/clueweb09_rank_v_runs_t.bin /mnt/d/data/ClueWeb09Flat/1mq.txt --rank v --runs t --out /mnt/d/data/clueweb09_rank_v_runs_t_results.csv
# ./queries /mnt/d/data/ClueWeb09Flat/clueweb09_rank_v5_runs_t.bin /mnt/d/data/ClueWeb09Flat/1mq.txt --rank v5 --runs t --out /mnt/d/data/clueweb09_rank_v5_runs_t_results.csv

# ./queries.out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_v_runs_t.bin" "/media/jpcastillog/Nuevo vol/data/Gov2Flat/1mq.txt" --rank v --runs t
# ./queries.out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_v_runs_t.bin" "/media/jpcastillog/Nuevo vol/data/Gov2Flat/1mq.txt" --rank v --runs t
# ./queries "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_v5_runs_t.bin" "/media/jpcastillog/Nuevo vol/data/Gov2Flat/1mq.txt" --rank v5 --runs t
# ./queries "/media/jpcastillog/Nuevo vol/data/ClueWeb09Flat/clueweb09_rank_v_runs_t.bin"  "/media/jpcastillog/Nuevo vol/data/ClueWeb09Flat/1mq.txt" --rank v --runs t
# ./queries "/media/jpcastillog/Nuevo vol/data/ClueWeb09Flat/clueweb09_rank_v5_runs_t.bin" "/media/jpcastillog/Nuevo vol/data/ClueWeb09Flat/1mq.txt" --rank v5 --runs t

# ./queries.out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_il_64_runs_t.bin" "/media/jpcastillog/Nuevo vol/data/Gov2Flat/1mq.txt" --rank il 64 --runs t
# ./queries.out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_il_256_runs_t.bin" "/media/jpcastillog/Nuevo vol/data/Gov2Flat/aol.txt" --#sequences 1100000 --rank il 256 --runs t
./queries.out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_v5_runs_t.bin" "/media/jpcastillog/Nuevo vol/data/Gov2Flat/aol.txt" --rank v5--runs t



# ./build.out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2.sorted" --min_size 1 --rank il 128 --runs t --out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_il_128_runs_t.bin"
# ./build.out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2.sorted" --min_size 1 --rank il 256 --runs t --out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_il_256_runs_t.bin"
# ./build.out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2.sorted" --min_size 1 --rank il 512 --runs t --out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_il_512_runs_t.bin"
# ./build.out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2.sorted" --min_size 1 --rank il 64 --runs t --out "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_il_64_runs_t.bin"