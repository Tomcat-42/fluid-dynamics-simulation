.EXTRA_PREREQS := $(abspath $(lastword $(MAKEFILE_LIST)))
COMMONDIR := ./

TARGET := $(COMMONDIR)fluid

GEN_TAGS := $(shell command -v ctags 2> /dev/null)
PERF := $(shell command -v perf 2> /dev/null)

ifdef GEN_TAGS
TAGS = $(COMMONDIR)tags
endif

SRCS := $(shell find $(COMMONDIR)src     -type f -name *.cpp)
HDRS := $(shell find $(COMMONDIR)include -type f -name *.hpp)
OBJS := $(SRCS:.cpp=.o)

INCLUDES := -I./include
CXXFLAGS := -Wall -Werror -Wextra -Wpedantic -g ${INCLUDES} -std=c++23 -pipe -fopenacc
LDFLAGS  := -lm

all: $(TARGET)

$(TARGET): ${HDRS}
$(TARGET): ${OBJS}
	${CXX} ${LDFLAGS} -o $(TARGET) ${OBJS} ${CXXFLAGS}
ifdef GEN_TAGS
	ctags -f $(TAGS) -R --language-force=c++
endif

release: CXXFLAGS += -O3 -mtune=native -march=native -fomit-frame-pointer
release: all

debug: all
	@gdb ./${TARGET}

run: release
	@./${TARGET}

.PHONY: clean run debug
ifdef PERF
.PHONY: profile stat
endif

depend: .depend

.depend:
	$(CXX) $(INCLUDES) -MM $(SRCS) > .depend
	@sed -i -E "s/^(.+?).o: ([^ ]+?)\1/\2\1.o: \2\1/g" .depend

include .depend

clean:
	$(RM) ${OBJS} ${TARGET}
ifdef GEN_TAGS
	$(RM) ${TAGS}
endif

ifdef PERF
profile: release
	perf record ./${TARGET}
	perf report

stat: release
	perf stat -B -e cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./${TARGET}
endif
