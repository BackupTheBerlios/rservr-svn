INCLUDE=-I/usr/include -I/usr/local/include
PLUGINS=$(shell if test -z "$(PREFIX)"; then echo /usr/local; else echo "$(PREFIX)"; fi)/libexec/rservr
LINK=-L/usr/lib -L/usr/local/lib -L$(PLUGINS) -Xlinker -rpath=$(PLUGINS)

.PHONY: all clean

all: separate-room system-status system-control

separate-room: src/separate-room.cpp
	g++ $(INCLUDE) $(LINK) src/separate-room.cpp -lrsvp-rqsrvc -lrservr-client -o separate-room

system-status: src/system-status.cpp
	g++ $(INCLUDE) $(LINK) src/system-status.cpp -lrsvp-netcntl -lrsvp-rqconfig -lrsvp-rqsrvc -lrsvp-ready -lrservr-client -o system-status

system-control: src/system-control.c
	gcc $(INCLUDE) $(LINK) src/system-control.c -lrsvp-rqconfig -lrsvp-ready -lrservr-client -o system-control

clean:
	rm -fv separate-room system-status system-control
	rm -fv {,*/}*~
