echo -e "---------------Gov2-----------------"
q1=/media/jpcastillog/Respaldo\ 1/PISA/Gov2/1mq.txt
for file in /media/jpcastillog/Respaldo\ 1/PISA/Gov2/compressed/* ; do
    ../build/tools/intersection_querylog_fastTrie "$file" "$q1" >> resultsGov2.txt
    echo -e "---> ${file} done!"
done;
echo -e "---------------ClueWeb09---------------"
q2=/media/jpcastillog/Respaldo\ 1/PISA/ClueWeb09/1mq.txt
for file in /media/jpcastillog/Respaldo\ 1/PISA/ClueWeb09/compressed/* ; do
    ../build/tools/intersection_querylog_fastTrie "$file" "$q2" >> resultsClueWeb09.txt
    echo -e "---> ${file} done!"
done;
echo -e "---------------CC-News---------------"
q3=/media/jpcastillog/Respaldo\ 1/PISA/CC-News/queries_4096.pisa
for file in /media/jpcastillog/Respaldo\ 2/data/CC-News/compressed/* ; do
    ../build/tools/intersection_querylog_fastTrie "$file" "$q3" >> resultsCC-News.txt
    echo -e "---> ${file} done!"
done;