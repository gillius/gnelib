# check if makefile.os is present

ifeq ($(wildcard makefile.os),)
error:
	@echo Run one of the fix* scripts to set up the makefile to your OS.
endif

# start of main makefile

-include makefile.os

.PHONY: clean lib examples install

all: lib examples

lib: $(LIB_STATIC)

examples: lib $(EXAMPLE_EXE)

$(LIB_STATIC): $(LIB_O)
	$(AR) rs $@ $^

$(LIB_O): %.o: %.cpp
	$(CPP) -Iinclude/gnelib -c -o $@ $(CPPFLAGS) $<

$(EXAMPLE_EXE): examples/%.exe: %.o
	$(CPP) -o $@ $(LDFLAGS) $< $(EXAMPLE_LIBS)

$(EXAMPLE_O): %.o: %.cpp
	$(CPP) -c -o $@ $(CPPFLAGS) $<

clean:
	$(RM) -f $(EXAMPLE_O) $(EXAMPLE_EXE) $(LIB_O) $(LIB_STATIC)

veryclean: clean
	$(RM) makefile.os

install: $(LIB_STATIC)
	$(CP) $< $(LIBDIR)
	$(CP) src/gnelib.h $(INCDIR)

uninstall:
	$(RM) -f $(LIBDIR)$(LIB_STATIC)
	$(RM) -f $(INCDIR)gnelib.h
