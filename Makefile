MUSL_VERSION=v1.2.2
DIETLIBC_VERSION=0.34
APP=don-de-sang

CXXFLAGS=-I. -std=c++17 -Wall -fno-exceptions -nodefaultlibs -nostdlib -ggdb -mcpu=native -mtune=native -mfpu=auto
#CXXFLAGS=-I. -I3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/include -std=c++17 -Wall -fno-exceptions -nodefaultlibs -nostdlib -ggdb -mcpu=cortex-a7
LIBS=3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/crt*.o \
-L 3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/ \
-l:libc.a \
-l:libcrypt.a \
-l:libdl.a \
-l:libm.a \
-l:libpthread.a \
-l:libresolv.a \
-l:librt.a \
-l:libutil.a \
-l:libxnet.a \
-L /usr/lib/gcc/arm-linux-gnueabihf/8/ \
-l:libgcc.a
#3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/rcrt1.o \
#3rd_party/musl-${MUSL_VERSION}-dist/usr/local/musl/lib/Scrt1.o \

.PHONY: all run clean

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
	$(CXX) $(CXXFLAGS) -ffreestanding -flto src/main.cpp 3rd_party/dietlibc-${DIETLIBC_VERSION}-dist/opt/diet/lib-arm/start.o  -o $(APP)-dietlibc -L 3rd_party/dietlibc-${DIETLIBC_VERSION}-dist/opt/diet/lib-arm/ -l:libc.a \
		-L /usr/lib/gcc/arm-linux-gnueabihf/8/ \
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

