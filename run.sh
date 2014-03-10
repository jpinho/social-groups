echo "cleanning up...";
rm -f test/*.result;

echo "running test suite... hold grips!";
for file in test/*.in
do
	./bin/socialgroups.out < $file > $file.result;
	filename=${file/".in"/".out.result"};
	mv $file.result $filename;
done

echo "validating results...";
for file in test/*.out
do 
	rm -f dump.log;
    diff $file $file.result > dump.log;
	OUT=$(cat dump.log);
	
    if [ "$OUT" == "" ] 
	then
    	echo "\n$file >> \\033[0;32mPASSED\033[0m!";
		cat dump.log;
    else
		green='\e[1;32m';
        echo "\n$file >> \\033[0;31mFAILED\033[0m!";
		cat dump.log;
	fi
done

echo;