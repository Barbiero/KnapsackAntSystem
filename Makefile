IDIR=.
CC=mpicc
CFLAGS=-I$(IDIR) -Wall -Wextra -pedantic -DTHREADED -pg -O2

 
ODIR=./obj

LIBS=-lm -lpthread

_DEPS = util.h item.h knapsack.h ant.h const.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

OBJNAMES = main.o item.o knapsack.o ant.o
OBJ = $(patsubst %,$(ODIR)/%,$(OBJNAMES))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
