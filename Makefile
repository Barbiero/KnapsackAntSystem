IDIR=.
CC=gcc
CFLAGS=-I$(IDIR) -g -Wall -Wextra -pedantic

ODIR=./obj

LIBS=-lm

_DEPS = util.h item.h knapsack.h ant.h const.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o util.o item.o knapsack.o ant.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
