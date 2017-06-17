
all: scanner parser interpreter

clean:
	rm -f *.o
	rm plz-samples/*.tok
	rm plz-samples/*.lst
	rm plz-samples/*.cod
cleanall:
	rm -f scanner
	rm -f parser
	rm -f interpreter

scanner.o: scanner.c plz_glob.h
	rm -f scanner.o
	gcc -Wall -ansi -c scanner.c

scanner: scanner.o 
	rm -f scanner 
	gcc -Wall -ansi -o scanner scanner.o 

parser.o: parser.c sempars.c codegen.c plz_glob.h code.h cmdcode.h header.h
	rm -f parser.o
	rm -f sempars.o
	rm -f codegen.o
	gcc -Wall -ansi -g -c parser.c sempars.c codegen.c

parser: parser.o sempars.o codegen.o
	rm -f parser
	gcc -Wall -ansi -o parser parser.o sempars.o codegen.o

plzint.o: plzint.c cmdcode.h
	rm -f interpreter
	gcc -Wall -g -c plzint.c

interpreter: plzint.o cmdcode.h
	rm -f interpreter
	gcc -Wall -o interpreter plzint.o
.c.o:
	gcc -Wall -ansi -c -g $< -o $@


