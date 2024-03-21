# echo -e "---------------Gov2-----------------"
# q1=/media/jpcastillog/Respaldo\ 1/PISA/Gov2/1mq.txt
# for file in /media/jpcastillog/Respaldo\ 1/PISA/Gov2/compressed/* ; do
#     ../build/tools/intersection_querylog_fastTrie "$file" "$q1" >> Gov2-Results.txt
#     echo -e "---> ${file} done!"
# done;
# echo -e "---------------ClueWeb09---------------"
# q2=/media/jpcastillog/Respaldo\ 1/PISA/ClueWeb09/1mq.txt
# for file in /media/jpcastillog/Respaldo\ 1/PISA/ClueWeb09/compressed/* ; do
#     ../build/tools/intersection_querylog_fastTrie "$file" "$q2" >> ClueWeb09-Results.txt
#     echo -e "---> ${file} done!"
# done;
# echo -e "---------------CC-News---------------"
# q3=/media/jpcastillog/Respaldo\ 1/PISA/CC-News/queries_4096.pisa
# for file in /media/jpcastillog/Respaldo\ 2/data/CC-News/compressed/* ; do
#     ../build/tools/intersection_querylog_fastTrie "$file" "$q3" >>CC-News-Results.txt
#     echo -e "---> ${file} done!"
# done;


# echo -e "---------------Gov2-----------------"
# q1=/media/jpcastillog/Respaldo\ 1/PISA/random\ queries\ 4096-15000/Gov2/1mq.txt
# for file in /media/jpcastillog/Respaldo\ 1/PISA/Gov2/compressed/Fast\ Tries/* ; do
#     ../build/tools/intersection_querylog_fastTrie "$file" "$q1" --parallel f >> singlethread-time-fastTries-resultsGov2-4096-15000\(random\).txt
#     echo -e "---> ${file} done!"
# done;
echo -e "---------------ClueWeb09---------------"
q2=/media/jpcastillog/Respaldo\ 1/PISA/random\ queries\ 4096-15000/ClueWeb09/1mq.txt
for file in /media/jpcastillog/Respaldo\ 1/PISA/ClueWeb09/compressed/Fast\ Tries/* ; do
    ../build/tools/intersection_querylog_fastTrie "$file" "$q2" --parallel f >> singlethread-time-fastTries-resultsClueWeb09-4096-15000\(random\)2.txt
    echo -e "---> ${file} done!"
done;
# echo -e "---------------CC-News---------------"
# q3=/media/jpcastillog/Respaldo\ 1/PISA/random\ queries\ 4096-15000/CC-News/1mq.txt
# for file in /media/jpcastillog/Respaldo\ 1/PISA/CC-News/compressed/Fast\ Tries/* ; do
#     ../build/tools/intersection_querylog_fastTrie "$file" "$q3" --parallel f >> singlethread-time-fastTries-resultsCC-News-4096-15000\(random\).txt
#     echo -e "---> ${file} done!"
# done
# RANDOM
# echo -e "---------------Gov2-----------------"
# q1=/media/jpcastillog/Respaldo\ 1/PISA/1mq\ queries\ 4096-15000/Gov2/1mq.txt
# for file in /media/jpcastillog/Respaldo\ 1/PISA/Gov2/compressed/Fast\ Tries/* ; do
#     ../build/tools/intersection_querylog_fastTrie "$file" "$q1" --parallel f >> singlethread-time-fastTries-resultsGov2-4096-15000\(1mq\).txt
#     echo -e "---> ${file} done!"
# done;
# echo -e "---------------ClueWeb09---------------"
# q2=/media/jpcastillog/Respaldo\ 1/PISA/1mq\ queries\ 4096-15000/ClueWeb09/1mq.txt
# for file in /media/jpcastillog/Respaldo\ 1/PISA/ClueWeb09/compressed/Fast\ Tries/* ; do
#     ../build/tools/intersection_querylog_fastTrie "$file" "$q2" --parallel f >> singlethread-time-fastTries-resultsClueWeb09-4096-15000\(1mq\)2.txt
#     echo -e "---> ${file} done!"
# done;
echo -e "---------------CC-News---------------"
q3=/media/jpcastillog/Respaldo\ 1/PISA/1mq\ queries\ 4096-15000/CC-News/1mq.txt
for file in /media/jpcastillog/Respaldo\ 1/PISA/CC-News/compressed/Fast\ Tries/* ; do
    ../build/tools/intersection_querylog_fastTrie "$file" "$q3" --parallel f >> singlethread-time-fastTries-resultsCC-News-4096-15000\(1mq\).txt
    echo -e "---> ${file} done!"
done