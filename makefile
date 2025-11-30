build:
	rm -f spellingBee.exe
	gcc main.c -o spellingBee.exe

run:
	./spellingBee.exe

run_simp:
	./spellingBee.exe -d newsamp.txt

run_play:
	./spellingBee.exe -r 7 -p

run_sample:
	echo "acdeit e" > sampleIn.txt
	./spellingBee.exe -d newsamp.txt < sampleIn.txt

valgrind:
	rm -f spellB_debug.exe
	gcc -g main.c -o spellB_debug.exe
	echo "watched w" > sampleIn.txt
	valgrind -s --tool=memcheck --leak-check=yes --track-origins=yes ./spellB_debug.exe < sampleIn.txt

clean:
	rm -f spellingBee.exe
	rm -f spellB_debug.exe
