CXXFLAGS = -g3

all: ray-tracer

debug: CXXFLAGS += -g -O0
debug: ray-tracer

ray-tracer: ray-tracer.cpp
	$(CXX) $(CXXFLAGS) -std=c++17 -o ray-tracer ray-tracer.cpp

clean:
	rm ray-tracer
