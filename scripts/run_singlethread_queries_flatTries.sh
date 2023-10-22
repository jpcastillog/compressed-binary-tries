echo -e "---------------Gov2-----------------"
q1=/media/jpcastillog/Respaldo\ 1/PISA/Gov2/1mq.txt
for file in /media/jpcastillog/Respaldo/Data/compressed/Gov2/* ; do
    ../build/tools/queries "$file" "$q1" --parallel f >> singlethread-time-flaTries-resultsGov2.txt
    echo -e "---> ${file} done!"
done;
echo -e "---------------ClueWeb09---------------"
q2=/media/jpcastillog/Respaldo\ 1/PISA/ClueWeb09/1mq.txt
for file in /media/jpcastillog/Respaldo/Data/compressed/ClueWeb09/* ; do
    ../build/tools/queries "$file" "$q2" --parallel f >> singlethread-time-flaTries-resultsClueWeb09.txt
    echo -e "---> ${file} done!"
done;
echo -e "---------------CC-News---------------"
q3=/media/jpcastillog/Respaldo\ 1/PISA/CC-News/queries_4096.pisa
for file in /media/jpcastillog/Respaldo/Data/compressed/CC-News/* ; do
    ../build/tools/queries "$file" "$q3" --parallel f >> singlethread-time-flaTries-resultsCC-News.txt
    echo -e "---> ${file} done!"
done;