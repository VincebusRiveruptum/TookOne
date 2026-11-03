# bsf - OpenWatcom wmake, MS-DOS (DOS/4G)
#
#   wmake -f makefile.wat          -> bsf.obj + bsftest.exe
#
# To use in BMPIO: copy bsf.c and bsf.h into deps/bsf/ and add bsf.obj
# to its makefile.

CC     = wcc386
CFLAGS = -4r -s -otexan -za -w4 -zq -bt=dos

all: bsf.obj bsftest.exe .SYMBOLIC

bsf.obj: bsf.c bsf.h
	$(CC) $(CFLAGS) -fo=$@ bsf.c

bsftest.obj: bsf.c bsf.h
	$(CC) $(CFLAGS) -dSTANDALONE -fo=$@ bsf.c

bsftest.exe: bsftest.obj
	wlink system dos4g name $@ option quiet file bsftest.obj

clean: .SYMBOLIC
	rm -f bsf.obj bsftest.obj bsftest.exe
