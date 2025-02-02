CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread
INCLUDES = -I/usr/lib/jvm/java-17-openjdk-amd64/include -I/usr/lib/jvm/java-17-openjdk-amd64/include/linux
LDFLAGS = -L/usr/lib/jvm/java-17-openjdk-amd64/lib/server -ljvm
SOURCES = systeme_central.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = systeme_central

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)