
all: makefile.os lib examples

.PHONY: all lib examples install clean veryclean uninstall

makefile.os:
	@echo Run one of the fix* scripts to set up the makefile to your OS.

include makefile.os

lib: $(LIB_STATIC)

examples: $(EXAMPLES_DIR) lib $(EXAMPLE_EXE)

$(LIB_STATIC): $(LIB_O)
	$(AR) rs $@ $^

$(LIB_CPP_O): %.o: %.cpp
	$(CPP) -Iinclude/gnelib -c -o $@ -MMD $(CPPFLAGS) $<

$(LIB_C_O): %.o: %.c
	$(CC) -Iinclude/gnelib -c -o $@ -MMD $(CFLAGS) $<

$(EXAMPLE_EXE): $(EXE_MASK): $(EXAMPLE_DIR) %.o $(LIB_STATIC)
	$(CPP) -o $@ $(LDFLAGS) $< $(EXAMPLE_LIBS)

$(EXAMPLE_O): %.o: %.cpp
	$(CPP) -c -o $@ $(CPPFLAGS) $<

-include $(DEPENDENCIES)

ifdef EXAMPLES_DIR

$(EXAMPLES_DIR):
	$(MKDIR) $@

endif

clean:
	$(RM) $(EXAMPLE_O) $(EXAMPLE_EXE) $(LIB_O) $(LIB_STATIC) $(DEPENDENCIES)

veryclean: clean
	$(RM) makefile.os

install: $(LIB_STATIC)
	$(CP) $< $(LIBDIR)
	$(CP) include/gnelib.h $(INCDIR)

uninstall:
	$(RM) $(LIBDIR)$(LIB_STATIC_NAME)
	$(RM) $(INCDIR)gnelib.h
