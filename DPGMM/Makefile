CC = gcc
OPTIONS = -DDSFMT_MEXP=19937
STD = -std=c99
CCFLAGS = $(OPTIONS) $(STD)

LIBS = -framework Accelerate

SOURCE=  main.c src/crp.c src/list/list.c src/random/random.c src/slicesample/slicesample.c src/utils/utils.c
TARGET = test

build:
	$(CC)  $(CCFLAGS) $(LIBS)  -o  $(TARGET) $(SOURCE)

clean: 
	rm -f *.o test* *~

rebuild: 
	clean build