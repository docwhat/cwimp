## Makefile for CWimp

## Set the Language
## To compile with a different language, make sure the
## proper statusmsg.xx.txt exists, then type:
##    make LANGUAGE=xx
## where xx in the country code.
## Possible Values: en
LANGUAGE = en

TARGET	:= cwimp
APPNAME	:= "CWimp"
APPID	:= "CWmp"
URL 	:= "http://docwhat.gerf.org/cwimp/"

# mystart.c MUST go first
SOURCES = mystart.c \
	$(TARGET).c  srand.c lowlevel.c game.c dialog.c draw.c statusmsg.c \
	ai.c queue.c
OBJECTS = $(shell echo $(patsubst %.c,objs/%.o, $(SOURCES)) | $(SED) 's!game.o!game.o $(CRT0)!')
PREDEPENDS = $(TARGET)Rsc.h
LIBS =

CC = m68k-palmos-coff-gcc

DFLAGS = -DDEBUG \
	-DERROR_CHECK_LEVEL=ERROR_CHECK_FULL

CFLAGS = -Wall -O2 $(DFLAGS)\
	-DLANG=$(LANGUAGE) -nostartfiles \
	-DVERSION='"$(VERSION)"'
# ERROR_CHECK_LEVEL sets the level to ERROR_CHECK_FULL or PARTIAL or 0
#
# Debug Rolling turns of 'rigged' rolling
#-DDEBUGROLL

PILRC		 = pilrc
OBJRES		 = m68k-palmos-coff-obj-res
NM		 = m68k-palmos-coff-nm
SED		 = sed
BUILDPRC	 = build-prc
PILOTXFER	 = coldsync -mr
FIXVERSION	:= ./fixversion
MKSTATUSMSG	:= ./mkstatusmsg
DEPEND		:= ./depend.sh
### This is System Dependent ###
# Works for Debian, though.
# See mystart.c for info
CRT0		:= /usr/m68k-palmos-coff/lib/crt0.o

VERSION=$(shell $(FIXVERSION) version)

all: $(TARGET).$(LANGUAGE).prc

deps/%.d: $(PREDEPENDS) $(DEPEND) %.c
	@echo -- Making dependency info for $*.c
	@if [ ! -d deps ]; then mkdir deps ; fi
	$(DEPEND) $(CC) $(CFLAGS) $*.c > $@ || rm -f $@

objs/%.o: %.c
	@echo -- Compiling $*.c
	$(CC) $(CFLAGS) -c -o objs/$*.o $*.c

$(TARGET)Rsc.h: $(TARGET).rcp images
	@echo -- Pilrc\'ing $*
	@if [ ! -d objs ]; then mkdir objs ; fi
	$(PILRC) -q -L $(LANGUAGE) -H $(TARGET)Rsc.h $(TARGET).rcp objs/


$(TARGET).rcp: $(TARGET).rcp.in version $(FIXVERSION) statusmsg.rcp
	@echo -- Fixing version and building $@
	$(FIXVERSION) $<

$(TARGET).$(LANGUAGE).prc: $(TARGET)Rsc.h objs/code0000.cwimp.grc
	@echo -- Building $@
	cd objs && \
	$(BUILDPRC) ../$(TARGET).$(LANGUAGE).prc $(APPNAME) $(APPID) *.grc *.bin

objs/%.grc: objs/$(TARGET)
	@echo -- Converting objects $*
	cd objs && \
	$(OBJRES) $(TARGET)

STATUSMSGS:= statusmsgstrings.h statusmsg.c statusmsg.rcp
$(STATUSMSGS): statusmsg.$(LANGUAGE).txt $(MKSTATUSMSG)
	@echo -- Making status messages
	$(MKSTATUSMSG) $<

objs/$(TARGET): $(TARGET)Rsc.h $(OBJECTS)
	@echo -- Compiling $@
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o $@ 

clean:
	rm -f objs/* *~ *.bak core $(TARGET)Rsc.h

veryclean: clean
	rm -f ChangeLog $(TARGET).*prc TAGS
	rm -f $(STATUSMSGS) rm -f $(TARGET).rcp

spotless: veryclean
	rm -rf deps objs $(TARGET).*.zip .info.$(TARGET)*

ChangeLog:
	cvs2cl

#
# Extra non-compiling rules
#

zip: srczip binzip

srczip $(TARGET).$(VERSION).$(LANGUAGE).src.zip .info.$(TARGET).$(VERSION).src: veryclean ChangeLog
	-rm -f $(TARGET).*.src.zip
	cd .. && \
	zip -r $(TARGET).$(VERSION).src.zip \
		cwimp \
		-x cwimp/CVS/\* -x \*.d
	mv ../$(TARGET).$(VERSION).src.zip .
#	rm -f .info.$(TARGET).$(VERSION).$(LANGUAGE).src
#	echo -e "\nCWimp Source\nVersion $(VERSION)" > \
#	.info.$(TARGET).$(VERSION).$(LANGUAGE).src

binzip $(TARGET).$(VERSION).$(LANGUAGE).bin.zip .info.$(TARGET).$(VERSION).$(LANGUAGE).bin:
	$(MAKE) $(TARGET).$(LANGUAGE).prc ChangeLog
	-rm -f $(TARGET).*.bin.zip
	cd .. && \
	zip -r $(TARGET).$(VERSION).$(LANGUAGE).bin.zip \
		cwimp/$(TARGET).$(LANGUAGE).prc \
		cwimp/ChangeLog \
		cwimp/README
	mv ../$(TARGET).$(VERSION).$(LANGUAGE).bin.zip .
	rm -f .info.$(TARGET).$(VERSION).$(LANGUAGE).bin
#	echo -e "\nCWimp\nVersion $(VERSION)" > \
#	.info.$(TARGET).$(VERSION).$(LANGUAGE).bin


TAGS: $(SOURCES) $(wildcard *.h)
	etags $^

archive: zip
	scp $(TARGET).*.zip .info.$(TARGET)* gerf.org:web/files/palm/cwimp/


install: $(TARGET).$(LANGUAGE).prc
	$(PILOTXFER) $(TARGET).$(LANGUAGE).prc

setver:
	vim version
	cvs commit -m "Upgrade to new version" version
	cvs tag `tail -n 1 version | sed 's/[ \.]/\-/g'`

.PHONY: depend clean veryclean zip archive floppy srczip binzip bin-res \
	setver

# Include the dependency info
include $(patsubst %.c,deps/%.d, $(SOURCES))
# EOF
