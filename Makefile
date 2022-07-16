COMMONDIR := ./

TARGET := $(COMMONDIR)fluid

GEN_TAGS := $(shell command -v ctags 2> /dev/null)

ifdef GEN_TAGS
TAGS = $(COMMONDIR)tags
endif

SRCS  := $(shell find $(COMMONDIR)src     -type f -name *.cpp)
HEADS := $(shell find $(COMMONDIR)include -type f -name *.h)
OBJS  := $(SRCS:.cpp=.o)

INCLUDES := -I./include
CXXFLAGS := -Wall -Werror -Wextra -Wpedantic -g ${INCLUDES} -pipe -std=c++23
LDFLAGS  := -lm $(shell pkg-config sfml-all --libs)

all: ${TARGET}

$(TARGET): ${OBJS} ${HEADS}
	${CXX} ${LDFLAGS} -o ${TARGET} ${OBJS}
ifdef GEN_TAGS
	ctags -f $(TAGS) -R --language-force=c++
endif

release: CXXFLAGS += -O3 -mtune=native -march=native -fomit-frame-pointer
release: all

run: all
	@./${TARGET}

.PHONY: depend clean run

clean:
	$(RM) ${OBJS} ${TARGET}
ifdef GEN_TAGS
	$(RM) ${TAGS}
endif

.EXTRA_PREREQS := $(abspath $(lastword $(MAKEFILE_LIST)))
