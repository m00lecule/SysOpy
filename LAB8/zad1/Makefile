CC = gcc -Wall -std=gnu99
LOG = Times

all: filter
	$(CC) filter.c -o filter -lpthread -lm

filter:
	$(CC) generate_filter.c -o generate_filter -lpthread -lm

test:
	rm -f $(LOG)
	@for frame in 5 10 20 60; do \
		./generate_filter $$frame filter_$${frame}.txt ;\
	done

	@for image in 'balloons' 'barbara' 'coins' 'feep'; do \
		for frame in 5 10 20 60; do \
			for type in block interleaved; do \
				for thread in 5 20 40; do \
					echo "\n\n=======================\n$$image F:$$frame $$type T:$$thread" >> $(LOG) ;\
					./filter $$thread $$type $${image}.ascii.pgm filter_$${frame}.txt $${image}_$${thread}_$${type}_$${frame}.pgm >> $(LOG) ;\
					done \
			done \
		done \
	done

clean:
	rm -rf *.o filter generate_filter main *.txt
