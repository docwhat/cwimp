## Makefile for Cosmic Wimpout

TARGET = cwroller
APPNAME = "Cosmic Wimpout"
APPID = "CWSS"

OBJS = $(TARGET).o lowlevel.o game.c draw.c statusmsg.c
LIBS =

CC = m68k-palmos-coff-gcc

CFLAGS = -Wall -g -O2 -DDEBUG

PILRC = pilrc
OBJRES = m68k-palmos-coff-obj-res
NM = m68k-palmos-coff-nm
BUILDPRC = build-prc
PILOTXFER = pilot-xfer

all: $(TARGET).prc

.S.o:
	$(CC) $(TARGETFLAGS) -c $<

.c.s:
	$(CC) $(CSFLAGS) $<

$(TARGET)Rsc.h: bin.res

$(TARGET).prc: bin.res code0000.$(TARGET).grc code0001.$(TARGET).grc data0000.$(TARGET).grc pref0000.$(TARGET).grc rloc0000.$(TARGET).grc
	$(BUILDPRC) $(TARGET).prc $(APPNAME) $(APPID) code0001.$(TARGET).grc code0000.$(TARGET).grc data0000.$(TARGET).grc *.bin pref0000.$(TARGET).grc rloc0000.$(TARGET).grc

code0000.$(TARGET).grc: $(TARGET)
	$(OBJRES) $(TARGET)

code0001.$(TARGET).grc: code0000.$(TARGET).grc

data0000.$(TARGET).grc: code0000.$(TARGET).grc

pref0000.$(TARGET).grc: code0000.$(TARGET).grc

rloc0000.$(TARGET).grc: code0000.$(TARGET).grc

bin.res: $(TARGET).rcp $(TARGET).bmp
	$(PILRC) -H $(TARGET)Rsc.h $(TARGET).rcp .
	touch bin.res

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)
	@echo "** Unresolved Symbols for $(TARGET)"
	@! $(NM) -u $(TARGET) | grep .
	@echo "** End of Unresolved Symbols for $(TARGET)"

send: $(TARGET).prc
	$(PILOTXFER) -i $(TARGET).prc

depend:
	makedepend -DNOSYSHEADERS -Y -I. *.c

clean:
	-rm -f *.[oa] $(TARGET) *.bin bin.res *.grc Makefile.bak $(TARGET)Rsc.h *~ core

veryclean: clean
	-rm -f $(TARGET).prc pilot.ram pilot.scratch cwroller.*.zip

zip: $(TARGET).prc clean
	-rm -f cwroller.??????.zip
	-zip cwroller.`date +"%y%m%d"`.zip [a-zMNTC]*

archive: zip
	-scp -P7070 cwroller.*.zip gerf.org:src/archive/

floppy: clean
	-mdeltree a:\*
	-mcopy * a:
	-mdir a:

install: $(TARGET).prc
	pilot-xfer -i $(TARGET).prc

checkin: 
	ci -l 	cwroller.c cwroller.h \
		draw.c draw.h \
		game.c game.h \
		statusmsg.c statusmsg.h \
		lowlevel.c lowlevel.h

# DO NOT DELETE

cwroller.o: callback.h statusmsg.h game.h cwrollerRsc.h draw.h cwroller.h
draw.o: callback.h cwroller.h cwrollerRsc.h lowlevel.h game.h statusmsg.h
draw.o: draw.h
game.o: cwroller.h cwrollerRsc.h draw.h game.h statusmsg.h
lowlevel.o: lowlevel.h
statusmsg.o: game.h cwrollerRsc.h statusmsg.h lowlevel.h
