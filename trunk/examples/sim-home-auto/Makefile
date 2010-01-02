INCLUDE=-I/usr/include -I/usr/local/include
LINK=-L/usr/lib -L/usr/local/lib

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
