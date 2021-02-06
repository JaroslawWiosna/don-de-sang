MUSL_VERSION=v1.2.2
DIETLIBC_VERSION=0.34
APP=don-de-sang

CXXFLAGS=-I. -std=c++17 -Wall -fno-exceptions -nodefaultlibs -nostdlib -ggdb -march=armv8-a+crc -mtune=cortex-a72
LIBS=3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/crt*.o \
	 -L 3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/ \
	 -l:libc.a \
	 -L /usr/lib/gcc/aarch64-unknown-linux-gnu/10.2.0/ \
	 -l:libgcc.a
.PHONY: all run clean
#         -l:librt.a \
#         -l:libresolv.a \
#         -l:libcrypt.a \
#         -l:libutil.a \
#         -l:libc.a \
#         -l:libxnet.a \
#         -l:libdl.a \
#         -l:libpthread.a \
#         -l:libm.a \

all: $(APP)

run: clean $(APP)
	./$(APP)

3rd_party/musl-${MUSL_VERSION}-dist:
	(cd 3rd_party/ && ./build_musl.sh ${MUSL_VERSION})

3rd_party/dietlibc-${DIETLIBC_VERSION}-dist:
	(cd 3rd_party/ && ./build_dietlibc.sh ${DIETLIBC_VERSION})

3rd_party/aids-patched.hpp: $(wildcard 3rd_party/*.patch) 3rd_party/aids.hpp
	rm -f 3rd_party/aids-patched.hpp
	cp 3rd_party/aids.hpp 3rd_party/aids-patched.hpp
	git apply 3rd_party/aids-*.patch

$(APP): src/main.cpp 3rd_party/aids-patched.hpp 3rd_party/musl-${MUSL_VERSION}-dist
	$(CXX) $(CXXFLAGS) src/main.cpp 3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/crtn.o -o $(APP) $(LIBS)

$(APP)-dietlibc: src/main.cpp 3rd_party/aids-patched.hpp 3rd_party/dietlibc-${DIETLIBC_VERSION}-dist
	$(CXX) $(CXXFLAGS) -ffreestanding -flto src/main.cpp 3rd_party/dietlibc-${DIETLIBC_VERSION}-dist/opt/diet/lib-aarch64/start.o -o $(APP)-dietlibc -L 3rd_party/dietlibc-${DIETLIBC_VERSION}-dist/opt/diet/lib-aarch64/ -l:libc.a \
	 -L /usr/lib/gcc/aarch64-unknown-linux-gnu/10.2.0/ \
	 -l:libgcc.a

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

