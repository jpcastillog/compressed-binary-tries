echo -e "---------------Gov2-----------------"
q1=/media/jpcastillog/Respaldo\ 1/PISA/random\ queries\ 4096-15000/Gov2/1mq.txt
for file in /media/jpcastillog/Respaldo\ 1/PISA/Gov2/compressed/Binary\ Tries/* ; do
    ../build/tools/queries "$file" "$q1" --parallel t >> multithread-time-bTries-resultsGov2-4096-15000\(random\).txt
    echo -e "---> ${file} done!"
done;
echo -e "---------------ClueWeb09---------------"
q2=/media/jpcastillog/Respaldo\ 1/PISA/random\ queries\ 4096-15000/ClueWeb09/1mq.txt
for file in /media/jpcastillog/Respaldo\ 1/PISA/ClueWeb09/compressed/Binary\ Tries/* ; do
    ../build/tools/queries "$file" "$q2" --parallel t >> multithread-time-bTries-resultsClueWeb09-4096-15000\(random\).txt
    echo -e "---> ${file} done!"
done;
echo -e "---------------CC-News---------------"
q3=/media/jpcastillog/Respaldo\ 1/PISA/random\ queries\ 4096-15000/CC-News/1mq.txt
for file in /media/jpcastillog/Respaldo\ 1/PISA/CC-News/compressed/Binary\ Tries/* ; do
    ../build/tools/queries "$file" "$q3" --parallel t >> multithread-time-bTries-resultsCC-News-4096-15000\(random\).txt
    echo -e "---> ${file} done!"
done

echo -e "---------------Gov2-----------------"
q1=/media/jpcastillog/Respaldo\ 1/PISA/1mq\ queries\ 4096-15000/Gov2/1mq.txt
for file in /media/jpcastillog/Respaldo\ 1/PISA/Gov2/compressed/Binary\ Tries/* ; do
    ../build/tools/queries "$file" "$q1" --parallel t >> multithread-time-bTries-resultsGov2-4096-15000\(1mq\).txt
    echo -e "---> ${file} done!"
done;
echo -e "---------------ClueWeb09---------------"
q2=/media/jpcastillog/Respaldo\ 1/PISA/1mq\ queries\ 4096-15000/ClueWeb09/1mq.txt
for file in /media/jpcastillog/Respaldo\ 1/PISA/ClueWeb09/compressed/Binary\ Tries/* ; do
    ../build/tools/queries "$file" "$q2" --parallel t >> multithread-time-bTries-resultsClueWeb09-4096-15000\(1mq\).txt
    echo -e "---> ${file} done!"
done;
echo -e "---------------CC-News---------------"
q3=/media/jpcastillog/Respaldo\ 1/PISA/1mq\ queries\ 4096-15000/CC-News/1mq.txt
for file in /media/jpcastillog/Respaldo\ 1/PISA/CC-News/compressed/Binary\ Tries/* ; do
    ../build/tools/queries "$file" "$q3" --parallel t >> multithread-time-bTries-resultsCC-News-4096-15000\(1mq\).txt
    echo -e "---> ${file} done!"
done