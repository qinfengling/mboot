TARGET=mmupgrade
all: $(TARGET) 

mmupgrade:main.o
	gcc $< -o $@

main.o:main.c hexdump.c mboot.c
	gcc -c $< -o $@

clean:
	rm *.o $(TARGET)
