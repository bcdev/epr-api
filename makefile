#################################################
## Macro definitions for this file
## Note: OUTDIR must be predefined and not empty
#################################################

THISDIR= .
SRCDIR = ./src
OUTDIR = ./build/$(CONFIG)

## CFLAGS = -xCC -Xc -xildoff -v -I.

# for solaris
# COMPILE = cc -Kpic -G -CFLAGS -I$(SRCDIR) -I$(THISDIR) $(OPTIONS)
# LINK    = ld -G
# TARGET  = $(OUTDIR)/libepr_api.so

# for linux
COMPILE = gcc -fPIC -ansi -c -I$(SRCDIR) -I$(THISDIR) $(OPTIONS)
LINK    = ld -shared
TARGET  = $(OUTDIR)/libepr_api.so

# for Mac OS X
# COMPILE = gcc -fPIC -ansi -c -I$(SRCDIR) -I$(THISDIR) $(OPTIONS)
# LINK    = ld -dylib
# TARGET  = $(OUTDIR)/libepr_api.dylib

HEADERS=\
  $(SRCDIR)/epr_api.h\
  $(SRCDIR)/epr_core.h\
  $(SRCDIR)/epr_dddb.h\
  $(SRCDIR)/epr_string.h\
  $(SRCDIR)/epr_ptrarray.h\
  $(SRCDIR)/epr_field.h\
  $(SRCDIR)/epr_record.h\
  $(SRCDIR)/epr_param.h\
  $(SRCDIR)/epr_dsd.h\
  $(SRCDIR)/epr_dataset.h\
  $(SRCDIR)/epr_msph.h\
  $(SRCDIR)/epr_swap.h\
  $(SRCDIR)/epr_band.h\
  $(SRCDIR)/epr_bitmask.h

SOURCES=\
  $(SRCDIR)/epr_api.c\
  $(SRCDIR)/epr_core.c\
  $(SRCDIR)/epr_dddb.c\
  $(SRCDIR)/epr_product.c\
  $(SRCDIR)/epr_dataset.c\
  $(SRCDIR)/epr_string.c\
  $(SRCDIR)/epr_ptrarray.c\
  $(SRCDIR)/epr_field.c\
  $(SRCDIR)/epr_record.c\
  $(SRCDIR)/epr_param.c\
  $(SRCDIR)/epr_dsd.c\
  $(SRCDIR)/epr_msph.c\
  $(SRCDIR)/epr_swap.c\
  $(SRCDIR)/epr_band.c\
  $(SRCDIR)/epr_bitmask.c\
  $(SRCDIR)/epr_dump.c\
  $(SRCDIR)/epr_typconv.c


OBJECTS=\
  $(OUTDIR)/epr_api.o\
  $(OUTDIR)/epr_core.o\
  $(OUTDIR)/epr_dddb.o\
  $(OUTDIR)/epr_product.o\
  $(OUTDIR)/epr_dataset.o\
  $(OUTDIR)/epr_string.o\
  $(OUTDIR)/epr_ptrarray.o\
  $(OUTDIR)/epr_field.o\
  $(OUTDIR)/epr_record.o\
  $(OUTDIR)/epr_param.o\
  $(OUTDIR)/epr_dsd.o\
  $(OUTDIR)/epr_msph.o\
  $(OUTDIR)/epr_swap.o\
  $(OUTDIR)/epr_band.o\
  $(OUTDIR)/epr_bitmask.o\
  $(OUTDIR)/epr_dump.o\
  $(OUTDIR)/epr_typconv.o


###############################################
## Arguments for the 'make' command
###############################################

info:
	@echo '****** Make file for target "EPR_C_API" ******'
	@echo 'For the release build type           "make rel"'
	@echo 'For the debug build type             "make deb"'
	@echo 'For a complete release rebuild type  "make all_rel"'
	@echo 'For a complete debug rebuild type    "make all_deb"'

all: $(TARGET)

deb:
	@echo 'Building debug version of target "EPR_C_API"...'
	@$(MAKE) all "CONFIG=debug" "OPTIONS=-g -D_DEBUG"

rel:
	@echo 'Building release version of target "EPR_C_API"...'
	@$(MAKE) all "CONFIG=release" "OPTIONS=-DNDEBUG"

all_deb:
	@$(MAKE) clean "CONFIG=debug"
	@$(MAKE) deb

all_rel:
	@$(MAKE) clean "CONFIG=release"
	@$(MAKE) rel

clean:
	@echo 'Deleting output files for target "EPR_C_API"...'
	@rm -f $(OBJECTS) $(TARGET)

###############################################
## Link object files to executable target
###############################################


$(TARGET) : $(OBJECTS)
	$(LINK) -o $@ $(OBJECTS) -lm

SRC_1 = $(SRCDIR)/epr_api.c
$(OUTDIR)/epr_api.o : $(HEADERS) $(SRC_1)
	$(COMPILE) -o $@ $(SRC_1)

SRC_2 = $(SRCDIR)/epr_core.c
$(OUTDIR)/epr_core.o : $(HEADERS) $(SRC_2)
	$(COMPILE) -o $@ $(SRC_2)

SRC_3 = $(SRCDIR)/epr_dddb.c
$(OUTDIR)/epr_dddb.o : $(HEADERS) $(SRC_3)
	$(COMPILE) -o $@ $(SRC_3)

SRC_4 = $(SRCDIR)/epr_product.c
$(OUTDIR)/epr_product.o : $(HEADERS) $(SRC_4)
	$(COMPILE) -o $@ $(SRC_4)

SRC_5 = $(SRCDIR)/epr_dataset.c
$(OUTDIR)/epr_dataset.o : $(HEADERS) $(SRC_5)
	$(COMPILE) -o $@ $(SRC_5)

SRC_6 = $(SRCDIR)/epr_string.c
$(OUTDIR)/epr_string.o : $(HEADERS) $(SRC_6)
	$(COMPILE) -o $@ $(SRC_6)

SRC_7 = $(SRCDIR)/epr_ptrarray.c
$(OUTDIR)/epr_ptrarray.o : $(HEADERS) $(SRC_7)
	$(COMPILE) -o $@ $(SRC_7)

SRC_8 = $(SRCDIR)/epr_field.c
$(OUTDIR)/epr_field.o : $(HEADERS) $(SRC_8)
	$(COMPILE) -o $@ $(SRC_8)

SRC_9 = $(SRCDIR)/epr_record.c
$(OUTDIR)/epr_record.o : $(HEADERS) $(SRC_9)
	$(COMPILE) -o $@ $(SRC_9)

SRC_10 = $(SRCDIR)/epr_param.c
$(OUTDIR)/epr_param.o : $(HEADERS) $(SRC_10)
	$(COMPILE) -o $@ $(SRC_10)

SRC_11 = $(SRCDIR)/epr_dsd.c
$(OUTDIR)/epr_dsd.o : $(HEADERS) $(SRC_11)
	$(COMPILE) -o $@ $(SRC_11)

SRC_12 = $(SRCDIR)/epr_msph.c
$(OUTDIR)/epr_msph.o : $(HEADERS) $(SRC12)
	$(COMPILE) -o $@ $(SRC_12)

SRC_13 = $(SRCDIR)/epr_swap.c
$(OUTDIR)/epr_swap.o : $(HEADERS) $(SRC_13)
	$(COMPILE) -o $@ $(SRC_13)

SRC_14 = $(SRCDIR)/epr_band.c
$(OUTDIR)/epr_band.o : $(HEADERS) $(SRC_14)
	$(COMPILE) -o $@ $(SRC_14)

SRC_15 = $(SRCDIR)/epr_bitmask.c
$(OUTDIR)/epr_bitmask.o : $(HEADERS) $(SRC_15)
	$(COMPILE) -o $@ $(SRC_15)

SRC_16 = $(SRCDIR)/epr_dump.c
$(OUTDIR)/epr_dump.o : $(HEADERS) $(SRC_16)
	$(COMPILE) -o $@ $(SRC_16)

SRC_17 = $(SRCDIR)/epr_typconv.c
$(OUTDIR)/epr_typconv.o : $(HEADERS) $(SRC_17)
	$(COMPILE) -o $@ $(SRC_17)

###############################################
