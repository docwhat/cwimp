## Makefile for Cosmic Wimpout

TARGET = cwimp
APPNAME = "Cosmic Wimpout"
APPID = "CWSS"
LANGUAGE = en
URL = "http://docwhat.gerf.org/cwimp/"

SOURCES = cwimp.c  draw.c  game.c  lowlevel.c  statusmsg.c
OBJECTS = $(TARGET).o lowlevel.o game.o draw.o statusmsg.o
LIBS =
GRESOURCES = code0001.$(TARGET).grc code0000.$(TARGET).grc \
	data0000.$(TARGET).grc *.bin pref0000.$(TARGET).grc \
	 rloc0000.$(TARGET).grc

CC = m68k-palmos-coff-gcc

CFLAGS = -Wall -g -O2 -DDEBUG 
# Debug Rolling turns of 'rigged' rolling
#-DDEBUGROLL

PILRC = pilrc
OBJRES = m68k-palmos-coff-obj-res
NM = m68k-palmos-coff-nm
BUILDPRC = build-prc
PILOTXFER = pilot-xfer
FIXVERSION = ./fixversion

VERSION=$(shell $(FIXVERSION) version)

all: $(TARGET).prc

.S.o:
	$(CC) $(TARGETFLAGS) -c $<

.c.s:
	$(CC) $(CSFLAGS) $<

%.d: %.c
	$(SHELL) -ec '$(CC) -DDEPENDENCY -MM $(CFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

#	perl -pi -e 's/resource.h/resource.h $(TARGET)Rsc.h/g' $@

%.rcp: %.rcp.in version
	rm -f $@ $(TARGET)Rsc.h
	$(FIXVERSION) $<

$(TARGET).prc: $(GRESOURCES)
	$(BUILDPRC) $(TARGET).prc $(APPNAME) $(APPID) $^

%.grc: $(TARGET)
	$(OBJRES) $^

*.bin $(TARGET)Rsc.h: $(TARGET).rcp $(TARGET).bmp
	@echo $(PILRC) -H $(TARGET)Rsc.h $(TARGET).rcp .
	@$(PILRC) -H $(TARGET)Rsc.h $(TARGET).rcp . > /dev/null

$(TARGET): $(TARGET)Rsc.h *.bin $(OBJECTS) 
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)
	@echo "** Unresolved Symbols for $(TARGET)"
	@! $(NM) -u $(TARGET) | grep .
	@echo "** End of Unresolved Symbols for $(TARGET)"

ChangeLog:
	cvs2cl

clean:
	-rm -f *.[oa] $(TARGET) *.bin *.grc *~ *.rcp core ChangeLog

veryclean: clean
	-rm -f $(TARGET).prc pilot.ram pilot.scratch \
	*.bak TAGS

spotless: veryclean
	-rm -f *.d $(TARGET)Rsc.h $(TARGET).*.zip $(TARGET.)*.zip.dsc

zip: srczip binzip

$(TARGET).$(VERSION).src.zip srczip $(TARGET).$(VERSION).src.zip.dsc: veryclean ChangeLog
	-rm -f $(TARGET).*.src.zip
	cd .. && \
	zip -r $(TARGET).$(VERSION).src.zip \
		cwimp \
		-x cwimp/CVS/\* -x cwimp/\*.d
	mv ../$(TARGET).$(VERSION).src.zip .
	echo "(<a href=\"$(URL)\">$(TARGET)</A> source, version $(VERSION))" > \
	$(TARGET).$(VERSION).src.zip.dsc

$(TARGET).$(VERSION).bin.zip binzip $(TARGET).$(VERSION).bin.zip.dsc: $(TARGET).prc ChangeLog
	-rm -f $(TARGET).*.bin.zip
	cd .. && \
	zip -r $(TARGET).$(VERSION).bin.zip \
		cwimp/$(TARGET).prc \
		cwimp/README
	mv ../$(TARGET).$(VERSION).bin.zip .
	echo "(<a href=\"$(URL)\">$(TARGET)</A> binary, version $(VERSION))" > \
	$(TARGET).$(VERSION).bin.zip.dsc

TAGS: $(SOURCES) $(wildcard *.h)
	etags $^

archive: zip
	mv -v $(TARGET).*.zip $(TARGET).*.dsc $(HOME)/lib/docwhat.web/files/
#-scp -P7070 $(TARGET).*.zip gerf.org:src/archive/

floppy: clean
	-mdeltree a:\*
	-mcopy * a:
	-mdir a:

install: $(TARGET).prc
	$(PILOTXFER) -i $(TARGET).prc

money:
	@echo What, you think I\'m made of money\?

.PHONY: depend clean veryclean zip archive floppy srczip binzip bin-res

# Include the dependency info
include $(SOURCES:.c=.d)
# EOF
