
BINARY_NAME := craisin

BASEDIR := $(PWD)
BINDIR := $(BASEDIR)/bin
SRCDIR := $(BASEDIR)/src

CCP := g++
LD := ld
CCFLAGS := -Wall
LDLIBS := -lc -lm
LDFLAGS :=

SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
OBJECTS := $(patsubst $(BASEDIR)/%.cpp, $(BINDIR)/%.o, $(SOURCES))

########################################

.PHONY: all clean run
all:
	@echo "[DEP] Setting up directories"
	@mkdir -p $(BINDIR)
	@echo "[CCFLAGS] = $(CCFLAGS)"
	@$(MAKE) $(BINARY_NAME)

clean:
	rm -rf $(BINDIR)/*

run:
	$(BINDIR)/$(BINARY_NAME)

########################################

.SECONDEXPANSION:
$(OBJECTS): $$(patsubst $$(BINDIR)%.o, $$(BASEDIR)%.cpp, $$@)
	@echo "[CCP] -c $(shell realpath -m --relative-to=$(PWD) $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.cpp))) -o $(shell realpath -m --relative-to=$(PWD) $(@))"
	@mkdir -p $(dir $@)
	@$(CCP) $(CCFLAGS) -c $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.cpp)) -o $@

$(BINARY_NAME): $(OBJECTS)
	@echo "[CCP] Creating final binary"
	@$(CCP) $(shell find $(BINDIR) -name '*.o') $(LDLIBS) $(LDFLAGS) -o $(BINDIR)/$@
