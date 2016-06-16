IDIR=.
CC=gcc
CFLAGS=-I$(IDIR)  -Wall -Wextra -pedantic -O2 -DTHREADED
 
ODIR=./obj

LIBS=-lm -lpthread

_DEPS = util.h item.h knapsack.h ant.h const.h barrier.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

OBJNAMES = main.o util.o item.o knapsack.o ant.o barrier.o
OBJ = $(patsubst %,$(ODIR)/%,$(OBJNAMES))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
