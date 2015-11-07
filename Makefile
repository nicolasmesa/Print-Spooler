# A simple Makefile

# List the object files in one place
OBJADD=core.o addqueue.o
OBJRM=core.o rmqueue.o
OBJSH=core.o showqueue.o

build:	addqueue rmqueue showqueue


addqueue: $(OBJADD)
	cc -o $@ $(OBJADD)


rmqueue: $(OBJRM)
	cc -o $@ $(OBJRM)


showqueue: $(OBJSH)
	cc -o $@ $(OBJSH)


install: build
	./start-environment.sh


uninstall:
	./clean-environment.sh

clean:
	rm -f  *.o addqueue rmqueue showqueue
