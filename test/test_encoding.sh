echo -e "Testing Encoding"
echo -e "* Rank v"
echo -e "** Flat representation"
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank v --runs f --levelwise f
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank v --runs t --levelwise f
echo -e "** Level-wise representation"
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank v --runs f --levelwise t
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank v --runs t --levelwise t
echo -e "* Rank v5"
echo -e "** Flat representation"
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank v5 --runs f --levelwise f
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank v5 --runs t --levelwise f
echo -e "** Level-wise representation"
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank v5 --runs f --levelwise t
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank v5 --runs t --levelwise t
echo -e "* Rank il"
echo -e "** Flat representation"
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 128 --runs f --levelwise f
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 256 --runs f --levelwise f
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 512 --runs f --levelwise f
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 128 --runs t --levelwise f
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 256 --runs t --levelwise f
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 512 --runs t --levelwise f
echo -e "** Level-wise representation"
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 128 --runs f --levelwise t
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 256 --runs f --levelwise t
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 512 --runs f --levelwise t
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 128 --runs t --levelwise t
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 256 --runs t --levelwise t
../build/test/test_encoding ~/Documents/Inverted\ Indexes/Gov2Flat/gov2.sorted.bin --min_size 4096 --rank il 512 --runs t --levelwise t
