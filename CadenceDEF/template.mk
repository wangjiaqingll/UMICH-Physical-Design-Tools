.SUFFIXES: $(SUFFIXES) .cpp

CXX	= gcc
CXXFLAGS = -O2

TMP1	=	$(LIBSRCS:.cpp=.o)
TMP2	=	$(TMP1:.cc=.o)
LIBOBJS =	$(TMP2:.c=.o)

TMP3	=	$(BINSRCS:.cpp=.o)
TMP4	=	$(TMP3:.cc=.o)
BINOBJS =	$(TMP4:.c=.o)

all: $(LIBTARGET) $(BINTARGET)

ifdef BINTARGET
INSTALLED_BIN = ../bin/$(BINTARGET)
endif

$(INSTALLED_BIN): $(BINTARGET)
	@echo $< =\> $@
	@rm -f $(INSTALLED_BIN)
	@cp $(BINTARGET) $(INSTALLED_BIN)

installbin: $(INSTALLED_BIN)

ifdef LIBTARGET
INSTALLED_LIB = ../lib/$(LIBTARGET)
endif

$(INSTALLED_LIB): $(LIBTARGET)
	@echo $< =\> $@
	@rm -f $(INSTALLED_LIB)
	@cp $(LIBTARGET) $(INSTALLED_LIB)

installlib: $(INSTALLED_LIB)

install release: all installhdrs installlib installbin

INSTALLED_HDRS = $(PUBLIC_HDRS:%=../include/%)
$(INSTALLED_HDRS):	../include/%:	%
	@echo $< =\> $@
	@rm -f $@
	@cp -p $< $@

installhdrs: $(INSTALLED_HDRS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -I../include $<

.c.o:
	$(CXX) $(CXXFLAGS) -c -I../include $<

$(LIBTARGET): $(LIBOBJS)
	rm -f $(LIBTARGET)
	$(AR) $(ARFLAGS) $(LIBTARGET) $^

$(BINTARGET): $(BINOBJS) $(LIBTARGET) $(LDLIBS)
	rm -f $(BINTARGET)
	$(LINK.cc) -o $(BINTARGET) $(BINOBJS) $(LIBTARGET) $(LDLIBS) -lstdc++

clean doclean:
	rm -f $(LIBTARGET) $(LIBOBJS) $(BINTARGET) $(BINOBJS) $(INSTALLED_LIB) \
			$(INSTALLED_BIN) $(INSTALLED_HDRS) *.tab.* *.output
