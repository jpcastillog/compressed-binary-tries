echo -e "---------------Gov2-----------------"

./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v --runs f --wsize 64 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv_runsf_64.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v --runs t --wsize 64 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv_runst_64.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v5 --runs f --wsize 64 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv5_runsf_64.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v5 --runs t --wsize 64 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv5_runst_64.fastbin

./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v --runs f --wsize 32 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv_runsf_32.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v --runs t --wsize 32 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv_runst_32.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v5 --runs f --wsize 32 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv5_runsf_32.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v5 --runs t --wsize 32 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv5_runst_32.fastbin

./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v --runs f --wsize 16 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv_runsf_16.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v --runs t --wsize 16 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv_runst_16.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v5 --runs f --wsize 16 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv5_runsf_16.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v5 --runs t --wsize 16 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv5_runst_16.fastbin

./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v --runs f --wsize 8 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv_runsf_8.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v --runs t --wsize 8 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv_runst_8.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v5 --runs f --wsize 8 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv5_runsf_8.fastbin
./build/tools/build_fastTries /mnt/e/PISA/Gov2/gov2.docs --min_size 1 --rank v5 --runs t --wsize 8 --out /mnt/e/PISA/Gov2/compressed/gov2_rankv5_runst_8.fastbin

echo -e "---------------ClueWeb09-----------------"
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rGov2 gov2f --wsize 64 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv_runsf_64.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v --runs t --wsize 64 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv_runst_64.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v5 --runs f --wsize 64 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv5_runsf_64.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v5 --runs t --wsize 64 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv5_runst_64.fastbin

./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v --runs f --wsize 32 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv_runsf_32.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v --runs t --wsize 32 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv_runst_32.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v5 --runs f --wsize 32 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv5_runsf_32.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v5 --runs t --wsize 32 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv5_runst_32.fastbin

./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v --runs f --wsize 16 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv_runsf_16.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v --runs t --wsize 16 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv_runst_16.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v5 --runs f --wsize 16 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv5_runsf_16.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v5 --runs t --wsize 16 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv5_runst_16.fastbin

./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v --runs f --wsize 8 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv_runsf_8.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v --runs t --wsize 8 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv_runst_8.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v5 --runs f --wsize 8 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv5_runsf_8.fastbin
./build/tools/build_fastTries /mnt/e/PISA/ClueWeb09/clueweb09.docs --min_size 1 --rank v5 --runs t --wsize 8 --out /mnt/e/PISA/ClueWeb09/compressed/clueweb09_rankv5_runst_8.fastbin