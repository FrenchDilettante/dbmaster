#!/bin/bash
langs="fr"

for lng in $langs
do
	cd $lng
	echo Traduction $lng
	rm -rf html
	mkdir html
	cp ../common/* html/
	cp *.stub html/
	
	for elem in *.markdown
	do
		if [ "$elem" == "00-index.markdown" ];
		then
			markdown $elem >> html/head.stub
			cat html/content.stub >> html/head.stub
		else
			length=${#elem}-9
			output=html/${elem:0:$length}.html
			echo "  $elem > $lng/$output"
		
			cat html/head.stub > $output
			markdown $elem >> $output
			cat html/foot.stub >> $output
		fi
	done
	
	rm html/*.stub
done
