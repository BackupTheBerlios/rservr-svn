.PHONY: all clean

all: separate-room system-status system-control

separate-room: src/separate-room.cpp
	g++ src/separate-room.cpp -lrsvp-rqsrvc -lrservr-client -o separate-room

system-status: src/system-status.cpp
	g++ src/system-status.cpp -lrsvp-netcntl -lrsvp-rqconfig -lrsvp-rqsrvc -lrsvp-ready -lrservr-client -o system-status

system-control: src/system-control.c
	gcc src/system-control.c -lrsvp-rqconfig -lrsvp-ready -lrservr-client -o system-control

clean:
	rm -fv separate-room system-status system-control
	rm -fv {,*/}*~
