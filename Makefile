PROG=cautclub
CAUT_DIR=caut

CAUT_HFILES=$(CAUT_DIR)/cauterize.h $(CAUT_DIR)/cauterize_club.h
CAUT_CFILES=$(CAUT_DIR)/cauterize.c $(CAUT_DIR)/cauterize_club.c
CAUT_FILES=$(CAUT_HFILES) $(CAUT_CFILES)

HFILES=$(CAUT_HFILES) \
			 options.h \
			 list.h \
			 app.h \
			 datafile.h \
			 hashing.h \
			 type_utils.h
CFILES=$(CAUT_CFILES) \
			 main.c \
			 options.c \
			 list.c \
			 app.c \
			 datafile.c \
			 hashing.c \
			 type_utils.c

CC=clang
CARGS=-Wall -Wextra --std=c11 -pedantic -O0
LIBS=-lgcrypt
INCLUDE=-Ivendor/socket99

default: $(PROG)

clean:
	rm -f $(PROG)
	rm -rf caut/
	rm -f cauterize-club.cautspec

$(CAUT_FILES): cauterize-club.cautspec
	./bin/caut-c11-ref --spec=$< --output=caut

cauterize-club.cautspec: cauterize-club.caut
	./bin/cauterize --schema=$< --output=$@

$(PROG): $(CFILES) $(HFILES)
	$(CC) $(CARGS) $(INCLUDE) $(LIBS) $(CFILES) -o $(PROG)
