.PHONY: all clean

CXX := gcc

all: tiny_jit libmath.so

tiny_jit: main.c
	$(CXX) $< -o $@

libmath.so: math_lib.c
	$(CXX) -shared -fPIC $< -o $@

clean:
	rm -rf *.so ./tiny_jit
