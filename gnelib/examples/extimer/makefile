extimer.exe: extimer.o
	g++ $< -o $@ -L../../src -mthreads -lalleg -lgnelib -lhawknl -lwsock32 -lpthreadw32
	
%.o: %.c
	g++ $< -c -o $@
	
clean:
	del *.o
	del *.exe	