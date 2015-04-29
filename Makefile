PROG=cautclub
CAUT_DIR=caut

CAUT_HFILES=$(CAUT_DIR)/cauterize.h $(CAUT_DIR)/cauterize_club.h
CAUT_CFILES=$(CAUT_DIR)/cauterize.c $(CAUT_DIR)/cauterize_club.c
CAUT_FILES=$(CAUT_HFILES) $(CAUT_CFILES)

CC=clang
CARGS=-Wall -Wextra --std=c11 -pedantic
INCLUDE=-I$(CAUT_DIR)
CFILES=$(CAUT_CFILES) main.c

default: $(PROG)

clean:
	rm -f $(PROG)
	rm -rf caut/
	rm -f cauterize-club.cautspec

$(CAUT_FILES): cauterize-club.cautspec
	./bin/caut-c11-ref --spec=$< --output=caut

cauterize-club.cautspec: cauterize-club.caut
	./bin/cauterize --schema=$< > $@

$(PROG): $(CFILES)
	$(CC) $(CARGS) $(INCLUDE) $^ -o $(PROG)
