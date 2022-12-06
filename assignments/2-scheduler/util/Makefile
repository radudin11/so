CC = gcc
CFLAGS = -Wextra -Wall -g

build: so_scheduler scheduler thread queue libscheduler

so_scheduler:
	$(CC) -fPIC $(CFLAGS) -o so_scheduler.o -c so_scheduler.c
scheduler:
	$(CC) -fPIC $(CFLAGS) -o scheduler.o -c scheduler.c
thread:
	$(CC) -fPIC $(CFLAGS) -o thread.o -c thread.c
queue:
	$(CC) -fPIC $(CFLAGS) -o queue.o -c queue.c
libscheduler:
	$(CC) -fPIC $(CFLAGS) -shared -o libscheduler.so so_scheduler.o scheduler.o thread.o queue.o -lpthread

clean:
	rm -f so_scheduler.o scheduler.o thread.o queue.o libscheduler.so