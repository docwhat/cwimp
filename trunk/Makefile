## Makefile for CWimp

TARGET	:= cwimp
APPNAME	:= "CWimp"
APPID	:= "CWmp"
LANGUAGE = en
URL 	:= "http://docwhat.gerf.org/cwimp/"

SOURCES = $(TARGET).c  draw.c  game.c  lowlevel.c  statusmsg.c srand.c
OBJECTS = $(patsubst %.c,objs/%.o, $(SOURCES))
PREDEPENDS = $(TARGET)Rsc.h
LIBS =

CC = m68k-palmos-coff-gcc

CFLAGS = -Wall -g -O2 -DDEBUG 
# Debug Rolling turns of 'rigged' rolling
#-DDEBUGROLL

PILRC		:= pilrc -q
OBJRES		:= m68k-palmos-coff-obj-res
NM		:= m68k-palmos-coff-nm
BUILDPRC	:= build-prc
PILOTXFER	:= pilot-xfer
FIXVERSION	:= ./fixversion
MKSTATUSMSG	:= ./mkstatusmsg
DEPEND		:= ./depend.sh

VERSION=$(shell $(FIXVERSION) version)

all: $(TARGET).prc

deps/%.d: $(PREDEPENDS) $(DEPEND) %.c
	@echo -- Making dependency info for $*.c
	@if [ ! -d deps ]; then mkdir deps ; fi
	$(DEPEND) $(CC) $(CFLAGS) $*.c > $@ || rm -f $@

objs/%.o: %.c
	@echo -- Compiling $*.c
	$(CC) $(CFLAGS) -c -o objs/$*.o $*.c

$(TARGET)Rsc.h: $(TARGET).rcp images
	@echo -- Pilrc\'ing $*
	$(PILRC) -H $(TARGET)Rsc.h $(TARGET).rcp .
	@if [ ! -d objs ]; then mkdir objs ; fi
	mv *.bin objs/


$(TARGET).rcp: $(TARGET).rcp.$(LANGUAGE).in version $(FIXVERSION) statusmsg.rcp
	@echo -- Fixing version and building $@
	$(FIXVERSION) $<

$(TARGET).prc: $(TARGET)Rsc.h objs/code0000.cwimp.grc
	@echo -- Building $@
	cd objs && \
	$(BUILDPRC) ../$(TARGET).prc $(APPNAME) $(APPID) *.grc *.bin

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
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LIBS)

clean:
	rm -f objs/* *~ *.bak core $(TARGET)Rsc.h
	rm -f $(STATUSMSGS)

veryclean: clean
	rm -f ChangeLog $(TARGET).prc TAGS

spotless: veryclean
	rm -rf deps objs $(TARGET).*.zip $(TARGET.)*.zip.dsc

ChangeLog:
	cvs2cl

#
# Extra non-compiling rules
#

zip: srczip binzip

$(TARGET).$(VERSION).src.zip srczip $(TARGET).$(VERSION).src.zip.dsc: veryclean ChangeLog
	-rm -f $(TARGET).*.src.zip
	cd .. && \
	zip -r $(TARGET).$(VERSION).src.zip \
		cwimp \
		-x cwimp/CVS/\* -x \*.d
	mv ../$(TARGET).$(VERSION).src.zip .
	echo "(<a href=\"$(URL)\">$(TARGET)</A> source, version $(VERSION))" > \
	$(TARGET).$(VERSION).src.zip.dsc

$(TARGET).$(VERSION).bin.zip binzip $(TARGET).$(VERSION).bin.zip.dsc:
	$(MAKE) $(TARGET).prc ChangeLog
	-rm -f $(TARGET).*.bin.zip
	cd .. && \
	zip -r $(TARGET).$(VERSION).bin.zip \
		cwimp/$(TARGET).prc \
		cwimp/ChangeLog \
		cwimp/README
	mv ../$(TARGET).$(VERSION).bin.zip .
	echo "(<a href=\"$(URL)\">$(TARGET)</A> binary, version $(VERSION))" > \
	$(TARGET).$(VERSION).bin.zip.dsc

TAGS: $(SOURCES) $(wildcard *.h)
	etags $^

archive: zip
	mv -iv $(TARGET).*.zip $(TARGET).*.dsc $(HOME)/lib/docwhat.web/files/
#-scp -P7070 $(TARGET).*.zip gerf.org:src/archive/


install: $(TARGET).prc
	$(PILOTXFER) -i $(TARGET).prc

.PHONY: depend clean veryclean zip archive floppy srczip binzip bin-res

# Include the dependency info
include $(patsubst %.c,deps/%.d, $(SOURCES))
# EOF
