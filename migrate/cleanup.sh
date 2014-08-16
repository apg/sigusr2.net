for n in `egrep '[pP]ublished: False' *.txt | cut -f1 -d:`; do
   mv $n $n.unpublished
done

sed -i 's/^[Tt]itle: //' *.txt
sed -i 's/^[Tt]ags: //' *.txt
sed -i 's/^[Dd]ate: //' *.txt

sed -i 's/^[pP]ublished: True$//' *.txt
sed -i 's/^[cC]omments: True$//' *.txt

for n in *.txt; do
    rawlineno=$(grep -n '^---$' $n | cut -d ':' -f1)
    lineno=`expr $rawlineno - 1`
    echo $rawlineno ":" $lineno
    OLDIFS=$IFS
    IFS=$'\n'

    for line in `head -n $lineno $n`; do
        sed -i "s|$line|% &|" $n
    done
    IFS=$OLDIFS
done

for n in `ls *.txt`; do
    sed -i 's/^---$//' $n
    sed -i 's/^$//' $n
    output=${n%%.txt}.md
    ./html2text.py $n > $output
done
