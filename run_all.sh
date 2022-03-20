echo -e "Building ..."
# make clean
# make build
echo -e "-------------------size 4096-------------------"
echo -e "-------------------Gov2-------------------"
echo -e "Running GOV2 rank v ..."
./build /mnt/d/data/Gov2Flat/gov2.sorted --min_size 4096 --rank v --runs f
echo -e "Running GOV2 rank v5 ..."
./build /mnt/d/data/Gov2Flat/gov2.sorted --min_size 4096 --rank v5 --runs f
echo -e "Running GOV2 rank v runs encoding ..."
./build /mnt/d/data/Gov2Flat/gov2.sorted --min_size 4096 --rank v --runs t
echo -e "Running GOV2 rank v5 runs encoding"
./build /mnt/d/data/Gov2Flat/gov2.sorted --min_size 4096 --rank v5 --runs t
echo -e "-------------------ClueWeb09-------------------"
echo -e "Running ClueWeb09 rank v ..."
./build /mnt/d/data/ClueWeb09Flat/clueweb09.sorted --min_size 4096 --rank v --runs f
echo -e "Running ClueWeb09 rank v5..."
./build /mnt/d/data/ClueWeb09Flat/clueweb09.sorted --min_size 4096 --rank v5 --runs f
echo -e "Running ClueWeb09 rank v runs encoding..."
./build /mnt/d/data/ClueWeb09Flat/clueweb09.sorted --min_size 4096 --rank v --runs t
echo -e "Running ClueWeb09 rank v5 runs encoding..."
./build /mnt/d/data/ClueWeb09Flat/clueweb09.sorted --min_size 4096 --rank v5 --runs t
echo -e "-------------------CC-News-------------------"
echo -e "Running CC-News rank v..."
./build /mnt/d/data/CC-News/cc-news-en.pisa.docs --min_size 4096 --rank v --runs f
echo -e "Running CC-News rank v5..."
./build /mnt/d/data/CC-News/cc-news-en.pisa.docs --min_size 4096 --rank v5 --runs f
echo -e "Running CC-News rank v runs encoding..."
./build /mnt/d/data/CC-News/cc-news-en.pisa.docs --min_size 4096 --rank v --runs t
echo -e "Running CC-News rank v5 runs encoding..."
./build /mnt/d/data/CC-News/cc-news-en.pisa.docs --min_size 4096 --rank v5 --runs t
echo -e "-------------------size 100000-------------------"
echo -e "-------------------Gov2-------------------"
echo -e "Running GOV2 rank v ..."
./build /mnt/d/data/Gov2Flat/gov2.sorted --min_size 100000 --rank v --runs f
echo -e "Running GOV2 rank v5 ..."
./build /mnt/d/data/Gov2Flat/gov2.sorted --min_size 100000 --rank v5 --runs f
echo -e "Running GOV2 rank v runs encoding ..."
./build /mnt/d/data/Gov2Flat/gov2.sorted --min_size 100000 --rank v --runs t
echo -e "Running GOV2 rank v5 runs encoding"
./build /mnt/d/data/Gov2Flat/gov2.sorted --min_size 100000 --rank v5 --runs t
echo -e "-------------------ClueWeb09-------------------"
echo -e "Running ClueWeb09 rank v ..."
./build /mnt/d/data/ClueWeb09Flat/clueweb09.sorted --min_size 100000 --rank v --runs f
echo -e "Running ClueWeb09 rank v5..."
./build /mnt/d/data/ClueWeb09Flat/clueweb09.sorted --min_size 100000 --rank v5 --runs f
echo -e "Running ClueWeb09 rank v runs encoding..."
./build /mnt/d/data/ClueWeb09Flat/clueweb09.sorted --min_size 100000 --rank v --runs t
echo -e "Running ClueWeb09 rank v5 runs encoding..."
./build /mnt/d/data/ClueWeb09Flat/clueweb09.sorted --min_size 100000 --rank v5 --runs t
echo -e "-------------------CC-News-------------------"
echo -e "Running CC-News rank v..."
./build /mnt/d/data/CC-News/cc-news-en.pisa.docs --min_size 100000 --rank v --runs f
echo -e "Running CC-News rank v5..."
./build /mnt/d/data/CC-News/cc-news-en.pisa.docs --min_size 100000 --rank v5 --runs f
echo -e "Running CC-News rank v runs encoding..."
./build /mnt/d/data/CC-News/cc-news-en.pisa.docs --min_size 100000 --rank v --runs t
echo -e "Running CC-News rank v5 runs encoding..."
./build /mnt/d/data/CC-News/cc-news-en.pisa.docs --min_size 100000 --rank v5 --runs t