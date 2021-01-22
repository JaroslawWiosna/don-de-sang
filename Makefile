MUSL_VERSION=v1.2.1
APP=don-de-sang

CXXFLAGS=-I. -std=c++17 -Wall -fno-exceptions -nodefaultlibs -nostdlib -ggdb
LIBS=3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/crt*.o \
	 -L 3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/ \
	 -l:libc.a

.PHONY: all run

all: $(APP)

run: $(APP)
	./$(APP)

3rd_party/musl-${MUSL_VERSION}-dist:
	(cd 3rd_party/ && ./build_musl.sh)

$(APP): src/main.cpp 3rd_party/aids.hpp 3rd_party/musl-${MUSL_VERSION}-dist
	$(CXX) $(CXXFLAGS) src/main.cpp 3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/crtn.o -o $(APP) $(LIBS)

