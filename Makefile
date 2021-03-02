APP=don-de-sang

CXXFLAGS=-I. -std=c++17 -Wall -fno-exceptions -nodefaultlibs -ggdb
LIBS=-lc

.PHONY: all run clean

all: $(APP)

run: clean $(APP)
	./$(APP)

3rd_party/aids-patched.hpp: $(wildcard 3rd_party/*.patch) 3rd_party/aids.hpp
	rm -f 3rd_party/aids-patched.hpp
	cp 3rd_party/aids.hpp 3rd_party/aids-patched.hpp
	git apply 3rd_party/aids-*.patch

$(APP): src/main.cpp 3rd_party/aids-patched.hpp 
	$(CXX) $(CXXFLAGS) src/main.cpp -o $(APP) $(LIBS)

clean:
	git clean -X -f --exclude='!3rd_party/*-dist'

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

