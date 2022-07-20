#!/bin/bash

is_valid_stock ()
{
	local symbol=$1
	local type=$2

	if [ `wc -l <./$symbol.$type ` -lt 14 ]; then 
		rm -f $symbol.$type
		echo "cannot fetch $symbol, non exist stock"
		return 1
	fi
	return 0
}
for symbol in "$@"
do
	##Delete existing stocks and eps's, if exist##
	rm -f "./$symbol.stock" 
	rm -f "./$symbol.eps"

	##Getting data from database## IBM AMZ KAKI
	wget -O "./$symbol.stock" "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol=$symbol&interval=60min&outputsize=full&apikey=SLZYI1PBHH5S2DIZD" &> /dev/null
	if ! is_valid_stock $symbol "stock"; then
		continue
	fi
	echo "Fetched $symbol monthly"

	wget -O "./$symbol.eps" "https://www.alphavantage.co/query?function=EARNINGS&symbol=$symbol&apikey=SLZYI1PBHH5S2DIZD" &> /dev/null
	if ! is_valid_stock $symbol "eps"; then
		continue
	fi
	echo "Fetched $symbol eps"
done



for file in ./*.stock
do
	sed -i '1,9d' $file
	sed -i ':a;N;$!ba;s/\n            //g' $file
	sed -i 's/        //g' $file
	sed -i ':a;N;$!ba;s/\n},/}/g' $file
	sed -i 's/"//g' $file
	sed -i 's/}//g' $file

	sed -i 's/: {1. open: / /g' $file
	sed -i 's/,2. high: / /g' $file
	sed -i 's/,3. low: / /g' $file
	sed -i 's/,4. close: / /g' $file
	sed -i 's/,5. volume://g' $file
done

for file1 in ./*.eps
do
	sed -i '1,3d' $file1
	sed -i '/quarterlyEarnings*/q' $file1
	echo "$(head -n -2 $file1)" > $file1
	sed -i 's/        //g' $file1
	sed -i 's/            //g' $file1
	sed -i ':a;N;$!ba;s/,\n/ /g' $file1
	sed -i 's/} {//g' $file1
	sed -i ':a;N;$!ba;s/\n\n/\n/g' $file1
	sed -i 's/    //g' $file1
	sed -i 's/"//g' $file1
	sed -i 's/fiscalDateEnding: //g' $file1
	sed -i 's/reportedEPS: //g' $file1

	echo "$(head -n -1 $file1)" > $file1
	echo "$(tail -n +2 $file1)" > $file1

	sed -i 's/-/ /g' $file1
	echo "$(awk '{print $1 " " $4}' $file1)" > $file1

	
done