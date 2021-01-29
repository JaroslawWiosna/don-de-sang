#TODO(#6): Musl version should be updated in only one place
MUSL_VERSION=v1.2.2
APP=don-de-sang

CXXFLAGS=-I. -std=c++17 -Wall -fno-exceptions -nodefaultlibs -nostdlib -ggdb
LIBS=3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/crt*.o \
	 -L 3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/ \
	 -l:libc.a

.PHONY: all run clean

all: $(APP) $(APP)-dietlibc

run: clean $(APP)
	./$(APP)

3rd_party/musl-${MUSL_VERSION}-dist:
	(cd 3rd_party/ && ./build_musl.sh)

3rd_party/dietlibc-0.34-dist:
	(cd 3rd_party/ && ./build_dietlibc.sh)

3rd_party/aids-patched.hpp: $(wildcard 3rd_party/*.patch) 3rd_party/aids.hpp
	rm -f 3rd_party/aids-patched.hpp
	cp 3rd_party/aids.hpp 3rd_party/aids-patched.hpp
	git apply 3rd_party/aids-*.patch

$(APP): src/main.cpp 3rd_party/aids-patched.hpp 3rd_party/musl-${MUSL_VERSION}-dist
	$(CXX) $(CXXFLAGS) src/main.cpp 3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/crtn.o -o $(APP) $(LIBS)

$(APP)-dietlibc: src/main.cpp 3rd_party/aids-patched.hpp 3rd_party/dietlibc-0.34-dist
	$(CXX) $(CXXFLAGS) src/main.cpp -o $(APP)-diet -L 3rd_party/dietlibc-0.34-dist/opt/diet/lib-x86_64/ -l:libc.a

clean:
	git clean -X -f --exclude='!3rd_party/musl-*-dist'

.PHONY: vallgrind callgrind info

valgrind: $(APP)
	valgrind --track-origins=yes --leak-check=full ./$(APP)

callgrind: $(APP)
	valgrind --tool=callgrind --dump-instr=yes ./$(APP)

info: $(APP)
	file ./$(APP)
	ldd ./$(APP) || true

strace: $(APP)
	rm -f strace.log
	strace ./$(APP) 2> strace.log
	cat -n strace.log

