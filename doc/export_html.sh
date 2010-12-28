#!/bin/bash
langs="fr"

for lng in $langs
do
	cd $lng
	echo Traduction $lng
	rm -rf html
	mkdir html
	cp ../common/* html/
	
	for elem in *.markdown
	do
		
		length=${#elem}-9
		output=html/${elem:0:$length}.html
		echo "  $elem > $lng/$output"
		
		cat head.stub > $output
		markdown $elem >> $output
		cat foot.stub >> $output
	done
done
