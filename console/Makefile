CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = console_commande console_controle
SRCS_COMMANDE = console_commande.c
SRCS_CONTROLE = console_controle.c
OBJS_COMMANDE = $(SRCS_COMMANDE:.c=.o)
OBJS_CONTROLE = $(SRCS_CONTROLE:.c=.o)

.PHONY: all clean

all: $(TARGET)

console_commande: $(OBJS_COMMANDE)
	$(CC) $(CFLAGS) -o $@ $^

console_controle: $(OBJS_CONTROLE)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS_COMMANDE) $(OBJS_CONTROLE) $(TARGET)