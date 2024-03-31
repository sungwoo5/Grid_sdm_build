GRID=../../../build/build_grid_omp_Nc4
CONFIG=$(GRID)/grid-config
CXX=$(shell $(CONFIG) --cxx)
CXXFLAGS=$(shell $(CONFIG) --cxxflags)
LDFLAGS=$(shell $(CONFIG) --ldflags)
LIBS=$(shell $(CONFIG) --libs)

BIN_DIR=../bin

SRC=$(wildcard *.cc)
BIN=$(basename $(SRC))

#.PRECIOUS: hmc_SDM.o
.PHONY: clean

all: $(BIN)

%: %.cc
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $< -o $@
	mv $@ $(BIN_DIR)

clean:
	$(RM) -r $(BIN_DIR)
