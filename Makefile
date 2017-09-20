TXT=$(wildcard *.txt) 
CSV=$(wildcard *.csv)
graph: graph.c
	gcc -Wall graph.c -o graph

test: graph
	./graph posneg.txt

testall: graph $(TXT)
	for curr_file in $(TXT); do \
		./graph $$curr_file ; \
	done
	for curr_file in $(CSV); do \
		./graph $$curr_file ; \
	done
