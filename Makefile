SYSTEM     = x86_linux
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# Set CPLEXDIR and CONCERTDIR to the directories where
# CPLEX and CONCERT are installed.
# Uncomment and set the variables here or set them in
# your .bashrc or .bash_profile file.
#
#------------------------------------------------------------

CPLEXDIR      = /home/adam/Downloads/CPLEX/cplex
CONCERTDIR    = /home/adam/Downloads/CPLEX/concert

# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++ -O0 -std=c++11 -ggdb -Wall

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m32 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)
CCLNFLAGS = -lconcert -lilocplex -lcplex -m32 -lm -lpthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

EXSRCCPP = ./src
EXBINCPP = ./bin

CPP_EX = genome-scaffolder

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)

# ------------------------------------------------------------
#
# The compile commands
#

all: $(EXBINCPP)/genome-scaffolder

$(EXBINCPP)/genome-scaffolder: $(EXBINCPP)/main.o $(EXBINCPP)/graphs.o $(EXBINCPP)/backbone.o $(EXBINCPP)/error_calculator.o $(EXBINCPP)/sign_enumeration.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o $(EXBINCPP)/genome-scaffolder $(EXBINCPP)/main.o $(EXBINCPP)/graphs.o $(EXBINCPP)/backbone.o $(EXBINCPP)/error_calculator.o $(EXBINCPP)/sign_enumeration.o $(CCLNFLAGS)

$(EXBINCPP)/main.o: $(EXSRCCPP)/main.cpp $(EXBINCPP)/graphs.o $(EXBINCPP)/backbone.o $(EXBINCPP)/sign_enumeration.o
	$(CCC) -c $(EXSRCCPP)/main.cpp -o $(EXBINCPP)/main.o

$(EXBINCPP)/error_calculator.o: $(EXSRCCPP)/error_calculator.cpp
	$(CCC) -c $(CCFLAGS) $(EXSRCCPP)/error_calculator.cpp -o $(EXBINCPP)/error_calculator.o

$(EXBINCPP)/backbone.o: $(EXSRCCPP)/backbone.cpp
	$(CCC) -c $(EXSRCCPP)/backbone.cpp -o $(EXBINCPP)/backbone.o

$(EXBINCPP)/graphs.o: $(EXSRCCPP)/graphs.cpp
	$(CCC) -c $(EXSRCCPP)/graphs.cpp -o $(EXBINCPP)/graphs.o
	
$(EXBINCPP)/sign_enumeration.o: $(EXSRCCPP)/sign_enumeration.cpp
	$(CCC) -c $(EXSRCCPP)/sign_enumeration.cpp -o $(EXBINCPP)/sign_enumeration.o

# ------------------------------------------------------------

clean :
	rm -rf $(EXBINCPP)/*.o
	rm -rf $(EXBINCPP)/$(CPP_EX)

# Local Variables:
# mode: makefile
# End:
