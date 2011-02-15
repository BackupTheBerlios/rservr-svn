BASE:=$(shell if test -z "$(PREFIX)"; then echo "$$( dirname "$$( which rservr )" 2> /dev/null )/.."; else echo "$(PREFIX)"; fi)
PLUGINS=$(BASE)/libexec/rservr
INCLUDE=-I$(BASE)/include
COMPILE=-Wall -O2
LINK=-L$(BASE)/lib -L$(PLUGINS) -Xlinker -rpath=$(PLUGINS)

.PHONY: all clean

all: separate-room system-status system-control

separate-room: src/separate-room.cpp
	g++ $(INCLUDE) $(COMPILE) $(LINK) src/separate-room.cpp -lrsvp-rqsrvc -lrservr-client -o separate-room

system-status: src/system-status.cpp
	g++ $(INCLUDE) $(COMPILE) $(LINK) src/system-status.cpp -lrsvp-netcntl -lrsvp-rqconfig -lrsvp-rqsrvc -lrsvp-ready -lrservr-client -o system-status

system-control: src/system-control.c
	gcc $(INCLUDE) $(COMPILE) $(LINK) src/system-control.c -lrsvp-rqconfig -lrsvp-ready -lrservr-client -o system-control

clean:
	rm -fv separate-room system-status system-control
	rm -fv {,*/}*~
