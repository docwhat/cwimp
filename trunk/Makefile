## Makefile for Cosmic Wimpout

TARGET = cwimp
APPNAME = "Cosmic Wimpout"
APPID = "CWSS"
LANGUAGE = en

SOURCES = cwimp.c  draw.c  game.c  lowlevel.c  statusmsg.c
OBJECTS = $(TARGET).o lowlevel.o game.o draw.o statusmsg.o
LIBS =
GRESOURCES = code0001.$(TARGET).grc code0000.$(TARGET).grc \
	data0000.$(TARGET).grc *.bin pref0000.$(TARGET).grc \
	 rloc0000.$(TARGET).grc

CC = m68k-palmos-coff-gcc

CFLAGS = -Wall -g -O2 -DDEBUG

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

%.rcp: version
	$(FIXVERSION) $@

$(TARGET).prc: $(GRESOURCES)
	$(BUILDPRC) $(TARGET).prc $(APPNAME) $(APPID) $^

%.grc: $(TARGET)
	$(OBJRES) $^

*.bin $(TARGET)Rsc.h: $(TARGET).rcp $(TARGET).bmp
	@echo $(PILRC) -H $(TARGET)Rsc.h $(TARGET).rcp .
	@$(PILRC) -H $(TARGET)Rsc.h $(TARGET).rcp . > /dev/null
	touch bin.res

$(TARGET): $(TARGET)Rsc.h $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)
	@echo "** Unresolved Symbols for $(TARGET)"
	@! $(NM) -u $(TARGET) | grep .
	@echo "** End of Unresolved Symbols for $(TARGET)"

clean:
	-rm -f *.[oa] $(TARGET) *.bin *.grc *~ core

veryclean: clean
	-rm -f $(TARGET).prc pilot.ram pilot.scratch $(TARGET).*.zip \
	*.bak

spotless: veryclean
	-rm -f *.d $(TARGET)Rsc.h

zip: binzip srczip

$(TARGET).$(VERSION).src.zip srczip: veryclean
	-rm -f $(TARGET).*.src.zip
	cd .. && \
	zip -r $(TARGET).$(VERSION).src.zip \
		cwimp \
		-x cwimp/CVS/\* -x cwimp/\*.d

$(TARGET).$(VERSION).bin.zip binzip: $(TARGET).prc
	-rm -f $(TARGET).*.bin.zip
	cd .. && \
	zip -r $(TARGET).$(VERSION).bin.zip \
		cwimp/$(TARGET).prc \
		cwimp/README

archive: srczip
	-scp -P7070 $(TARGET).*.zip gerf.org:src/archive/

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
