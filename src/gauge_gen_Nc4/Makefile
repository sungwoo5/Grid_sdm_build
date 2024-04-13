GRID=../build_grid_omp_Nc4
CONFIG=$(GRID)/grid-config
CXX=$(shell $(CONFIG) --cxx)
CXXFLAGS=$(shell $(CONFIG) --cxxflags)
LDFLAGS=$(shell $(CONFIG) --ldflags)
LIBS=$(shell $(CONFIG) --libs)

BIN_DIR=./bin
INSTALL_DIR=../../install/gauge_gen_Nc4

SRC=$(wildcard *.cc)
BIN=$(basename $(SRC))

#.PRECIOUS: hmc_SDM.o
.PHONY: clean

all: $(BIN)

%: %.cc
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $< -o $@
	mv $@ $(BIN_DIR)

install:
	mkdir -p $(INSTALL_DIR)
	mv $(BIN_DIR) $(INSTALL_DIR)/.

clean:
	$(RM) -r $(BIN_DIR)
