SRCDIR := src
OBJDIR := obj

EXE := NTUlegalize
CXX := g++
CXXFLAGS := -std=c++11 -Iinclude -O3
CPPS := $(wildcard $(SRCDIR)/*/*.cpp)
HPPS := $(wildcard $(SRCDIR)/*/*.hpp)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(CPPS:.cpp=.o)))
LDFLAGS := -Llib
LDLIBS := -ldef -llef -lboost_program_options

.PHONY: all release clean

debug: CXXFLAGS += -g 

release: CXXFLAGS += -DNDEBUG -DOMP -fopenmp -static

all release debug: $(OBJDIR) $(EXE)

$(OBJDIR):
	mkdir $@

$(EXE): $(OBJS) $(SRCDIR)/main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/*/%.cpp $(HPPS)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJDIR) $(EXE)
