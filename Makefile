ifndef FF_ROOT 
FF_ROOT		= ${HOME}/fastflow
endif

CXX		= g++ -std=c++17 
INCLUDES	= -I $(FF_ROOT)
CXXFLAGS  	= -g # -DNO_DEFAULT_MAPPING -DBLOCKING_MODE -DFF_BOUNDED_BUFFER

LDFLAGS 	= -pthread
OPTFLAGS	= -O3 -finline-functions -DNDEBUG

TARGETS		= tsp_sequential tsp_parallel_thread tsp_parallel_ff 


.PHONY: all clean cleanall
.SUFFIXES: .cpp 


%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS)

all		: $(TARGETS)
clean		: 
	rm -f $(TARGETS)
cleanall	: clean
	\rm -f *.o *~