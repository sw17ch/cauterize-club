PROG=cautclub
CAUT_DIR=caut
CLUB_DIR=club

CAUT_HFILES=$(CAUT_DIR)/cauterize.h $(CAUT_DIR)/cauterize_club.h
CAUT_CFILES=$(CAUT_DIR)/cauterize.c $(CAUT_DIR)/cauterize_club.c
CAUT_FILES=$(CAUT_HFILES) $(CAUT_CFILES)

HFILES=$(CAUT_HFILES) \
			 $(CLUB_DIR)/options.h \
			 $(CLUB_DIR)/list.h \
			 $(CLUB_DIR)/app.h \
			 $(CLUB_DIR)/datafile.h \
			 $(CLUB_DIR)/hashing.h \
			 $(CLUB_DIR)/type_utils.h
CFILES=$(CAUT_CFILES) \
			 main.c \
			 $(CLUB_DIR)/options.c \
			 $(CLUB_DIR)/list.c \
			 $(CLUB_DIR)/app.c \
			 $(CLUB_DIR)/datafile.c \
			 $(CLUB_DIR)/hashing.c \
			 $(CLUB_DIR)/type_utils.c

CC=clang
CARGS=-Wall -Wextra --std=c11 -pedantic -O0
LIBS=-lgcrypt
INCLUDE=-Ivendor/socket99 -I.

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
