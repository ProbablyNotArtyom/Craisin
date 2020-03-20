
BINARY_NAME := craisin

BASEDIR := $(PWD)
BINDIR := $(BASEDIR)/bin
SRCDIR := $(BASEDIR)/src
INCDIR := $(SRCDIR)/include
TESTDIR := $(BASEDIR)/test

CCP := g++
LD := ld
CCFLAGS := -std=c++17 -Wall -I $(INCDIR) -I $(SRCDIR)/parse
LDLIBS := -lc -lm
LDFLAGS :=

SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
OBJECTS := $(patsubst $(BASEDIR)/%.cpp, $(BINDIR)/%.o, $(SOURCES))

########################################

.PHONY: clean all clean run
all:
	@echo "[DEP] Setting up directories"
	@mkdir -p $(BINDIR)
	@echo "[CCFLAGS] = $(CCFLAGS)"
	@$(MAKE) $(BINARY_NAME)

clean:
	rm -rf $(BINDIR)/*
	rm -f test/*.bin
run:
	$(BINDIR)/$(BINARY_NAME)

sim-tests:
	lwasm -o $(TESTDIR)/coco3.bin $(TESTDIR)/coco3.asm
	xroar -rompath /mnt/Keep2/Projects/project-files/_Retro_/CoCo/ROMs/coco2/ -run $(TESTDIR)/coco3.bin
	rm -f $(TESTDIR)/coco3.bin

########################################

.SECONDEXPANSION:
$(OBJECTS): $$(patsubst $$(BINDIR)%.o, $$(BASEDIR)%.cpp, $$@)
	@echo "[CCP] -c $(shell realpath -m --relative-to=$(PWD) $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.cpp))) -o $(shell realpath -m --relative-to=$(PWD) $(@))"
	@mkdir -p $(dir $@)
	@$(CCP) $(CCFLAGS) -c $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.cpp)) -o $@

$(BINARY_NAME): $(OBJECTS)
	@echo "[CCP] Creating final binary"
	@$(CCP) $(shell find $(BINDIR) -name '*.o') $(LDLIBS) $(LDFLAGS) -o $(BINDIR)/$@
