all: main

main: builtin.o job.o process.o shell.o iterator.o collection.o item.o array.o utils.o wrappers.o main.o
	gcc -lreadline builtin.o job.o process.o shell.o iterator.o collection.o item.o array.o utils.o wrappers.o main.o -o shell

# Job control

job.o: job_control/job.c
	gcc -c job_control/job.c

process.o: job_control/process.c
	gcc -c job_control/process.c

shell.o: job_control/shell.c
	gcc -c job_control/shell.c

# Dispatching

builtin.o: dispatch/builtin.c
	gcc -c dispatch/builtin.c

# Collections

iterator.o: collections/interfaces/iterator.c
	gcc -c collections/interfaces/iterator.c

collection.o: collections/interfaces/collection.c
	gcc -c collections/interfaces/collection.c

item.o: collections/interfaces/item.c
	gcc -c collections/interfaces/item.c

array.o: collections/array/array.c
	gcc -c collections/array/array.c

# Main

utils.o: utils.c
	gcc -c utils.c

wrappers.o: wrappers.c
	gcc -c wrappers.c

main.o: main.c
	gcc -c main.c

# Dummy clean target

clean:
	rm -rf *.o shell
