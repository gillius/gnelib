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
	$(GCC) -Iinclude/gnelib -c -o $@ $(COMPILER_OPTIONS) $<	

$(EXAMPLE_EXE): examples/%.exe: %.o
	$(GCC) -o $@ $(LINKER_OPTIONS) $< $(EXAMPLE_LIBS)
	
$(EXAMPLE_O): %.o: %.cpp	
	$(GCC) -c -o $@ $(COMPILER_OPTIONS) $<
	
clean:
	rm -f $(EXAMPLE_O) $(EXAMPLE_EXE) $(LIB_O) $(LIB_STATIC)
	
veryclean: clean
	rm makefile.os	
	
install: $(LIB_STATIC)
	cp $< $(MINGDIR)/lib
	cp src/gnelib.h $(MINGDIR)/include	
	
uninstall:
	rm -f $(MINGDIR)/lib/$(LIB_STATIC)
	rm -f $(MINGDIR)/include/gnelib.h