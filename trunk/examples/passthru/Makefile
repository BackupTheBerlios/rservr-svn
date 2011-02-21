BASE:=$(shell if test -z "$(PREFIX)"; then echo "$$( dirname "$$( which rservr )" 2> /dev/null )/.."; else echo "$(PREFIX)"; fi)
PLUGINS=$(BASE)/libexec/rservr
INCLUDE=-I$(BASE)/include
COMPILE=-Wall -O2
LINK=-L$(BASE)/lib -L$(PLUGINS) -Xlinker -rpath=$(PLUGINS)

.PHONY: all clean

all: sender receiver

sender: sender.c
	gcc $(INCLUDE) $(COMPILE) $(LINK) sender.c -lrsvp-netcntl -lrsvp-passthru -lrservr-client -o sender

receiver: receiver.c
	gcc $(INCLUDE) $(COMPILE) $(LINK) receiver.c -lrsvp-passthru -lrservr-client -o receiver

clean:
	rm -fv sender receiver *~
