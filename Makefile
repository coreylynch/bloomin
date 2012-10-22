HELPTEXT = "\
\n bloomin Makefile usage       \
\n                               \
\n  Options (current value)      \
\n                               \
\n    BLDDIR     ($(BLDDIR))     \
\n    DESTDIR    ($(DESTDIR))    \
\n    PREFIX     ($(PREFIX))     \
\n    LIBDIR     ($(LIBDIR))     \
\n    INCDIR     ($(INCDIR))     \
\n                               \
\n    CC         ($(CC))         \
\n    CFLAGS     ($(CFLAGS))     \
\n    LDFLAGS    ($(LDFLAGS))    \
\n    INSTALL    ($(INSTALL))    \
\n                               \
\n    PYTHON     ($(PYTHON))     \
\n    PY_MOD_DIR ($(PY_MOD_DIR)) \
\n                               \
\n  Targets                      \
\n                               \
\n    all        (c libbloomin) \
\n    install                    \
\n    test                       \
\n    clean                      \
\n    help                       \
\n                               \
\n    pybloomin                 \
\n    install_pybloomin         \
\n    test_pybloomin            \
\n\n"

# only bump major on ABI changing release
SO_VER_MAJOR = 1
SO_VER_MINOR = 1

PREFIX = /usr/local
LIBDIR = $(PREFIX)/lib
INCDIR = $(PREFIX)/include
DESTDIR =
BLDDIR = build

CFLAGS = -g -Wall -fPIC -O2
LDFLAGS = -g
LDLIBS = -lm

INSTALL = install
CC = gcc
AR = ar

SO_VER = $(SO_VER_MAJOR).$(SO_VER_MINOR)
SO_NAME = so
SO_CMD = -soname
SO_EXT_MAJOR = $(SO_NAME).$(SO_VER_MAJOR)
SO_EXT = $(SO_NAME).$(SO_VER)
UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
	SO_NAME = dylib
	SO_CMD = -install_name
	SO_EXT_MAJOR = $(SO_VER_MAJOR).$(SO_NAME)
	SO_EXT = $(SO_VER).$(SO_NAME)
endif

SRCS_LIBBLOOMIN = bloom.c murmur.c
SRCS_TESTS = test_bloom.c
WORDS = /usr/share/dict/words
OBJS_LIBBLOOMIN = $(patsubst %.c, $(BLDDIR)/%.o, $(SRCS_LIBBLOOMIN))
OBJS_TESTS = $(patsubst %.c, $(BLDDIR)/%.o, $(SRCS_TESTS))

LIB_SYMLNKS = libbloomin.$(SO_NAME) libbloomin.$(SO_EXT_MAJOR)
LIB_FILES = libbloomin.a libbloomin.$(SO_EXT) $(LIB_SYMLNKS)

# default target (needs to be first target)
all: libbloomin

# sort removes duplicates
DEPS := $(sort $(patsubst %.o, %.o.deps, $(OBJS_LIBBLOOMIN) $(OBJS_TESTS)))
-include $(DEPS)

libbloomin: $(patsubst %, $(BLDDIR)/%, $(LIB_FILES))

install: install_libbloomin

install_libbloomin: $(patsubst %, $(DESTDIR)$(LIBDIR)/%, $(LIB_FILES)) $(DESTDIR)$(INCDIR)/bloom.h

$(DESTDIR)$(LIBDIR)/libbloomin.a: $(BLDDIR)/libbloomin.a

$(DESTDIR)$(LIBDIR)/libbloomin.$(SO_EXT): $(BLDDIR)/libbloomin.$(SO_EXT)

$(patsubst %, $(DESTDIR)$(LIBDIR)/%, $(LIB_SYMLNKS)): %: $(DESTDIR)$(LIBDIR)/libbloomin.$(SO_EXT)
	@echo " SYMLNK " $@
	@$(INSTALL) -d $(dir $@)
	@ln -fs $(notdir $<) $@

$(DESTDIR)$(INCDIR)/bloom.h: src/bloom.h

$(DESTDIR)$(PREFIX)/%:
	@echo " INSTALL " $@
	@$(INSTALL) -d $(dir $@)
	@$(INSTALL) $< $@

$(BLDDIR)/%.o: src/%.c
	@echo " CC " $@
	@mkdir -p $(dir $@)
	@$(CC) -o $@ -c $< $(CFLAGS) -MMD -MF $@.deps

$(BLDDIR)/libbloomin.a: $(OBJS_LIBBLOOMIN)
	@echo " AR " $@
	@rm -f $@
	@$(AR) rcs $@ $^

$(BLDDIR)/libbloomin.$(SO_EXT): $(OBJS_LIBBLOOMIN)
	@echo " SO " $@
	@$(CC) -shared -Wl,$(SO_CMD),libbloomin.$(SO_EXT_MAJOR) -o $@ $^ $(LDLIBS)

$(patsubst %, $(BLDDIR)/%, $(LIB_SYMLNKS)): %: $(BLDDIR)/libbloomin.$(SO_EXT)
	@echo " SYMLNK " $@
	@mkdir -p $(dir $@)
	@ln -fs $(notdir $<) $@

$(BLDDIR)/test_bloomin: $(OBJS_TESTS) $(BLDDIR)/libbloomin.a
	@echo " LD " $@
	@$(CC) -o $@ $(LDFLAGS) $(OBJS_TESTS) $(BLDDIR)/libbloomin.a $(LDLIBS)

test: $(BLDDIR)/test_bloomin
	@$(BLDDIR)/test_bloomin $(BLDDIR)/testbloom.bin $(WORDS)

help:
	@printf $(HELPTEXT)

clean:
	rm -f $(DEPS) $(OBJS_LIBBLOOMIN) $(patsubst %, $(BLDDIR)/%, $(LIB_FILES)) $(OBJS_TESTS) $(BLDDIR)/test_bloomin $(BLDDIR)/testbloom.bin
	rmdir $(BLDDIR)

.PHONY: all clean help install test libbloomin install_libbloomin

### pybloomin ###

PYTHON = python
PY_BLDDIR = $(BLDDIR)/python
PY_MOD_DIR := $(shell $(PYTHON) -c "import distutils.sysconfig ; print(distutils.sysconfig.get_python_lib())")
PY_FLAGS = --build-lib=$(PY_BLDDIR) --build-temp=$(PY_BLDDIR)
PY_BLD_ENV = BLDDIR="$(BLDDIR)"

pybloomin: $(PY_BLDDIR)/pybloomin.so

install_pybloomin: $(DESTDIR)$(PY_MOD_DIR)/pybloomin.so

$(DESTDIR)$(PY_MOD_DIR)/pybloomin.so: $(PY_BLDDIR)/pybloomin.so
	@echo " PY_INSTALL " $@
	@$(INSTALL) -d $(dir $@)
	@$(INSTALL) $< $@

$(PY_BLDDIR)/pybloomin.so: $(BLDDIR)/libbloomin.a pybloomin/pybloomin.c
	@echo " PY_BUILD" $@
	@$(PY_BLD_ENV) $(PYTHON) pybloomin/setup.py build $(PY_FLAGS) >/dev/null

test_pybloomin: pybloomin
	@PYTHONPATH=$(PY_BLDDIR) $(PYTHON) pybloomin/test_pybloomin.py $(BLDDIR)/testbloom_py.bin $(WORDS)

clean: clean_pybloomin
clean_pybloomin:
	rm -f $(BLDDIR)/pybloomin.so $(BLDDIR)/testbloom_py.bin
	$(PYTHON) pybloomin/setup.py clean $(PY_FLAGS)

.PHONY: pybloomin install_pybloomin test_pybloomin.clean_pybloomin
