
GIET_SYS_PATH = /users/enseig/alain/giet_2011/sys
GIET_APP_PATH = /users/enseig/alain/giet_2011/app
AS = /opt/gcc-cross-mipsel/4.3.3/bin/mipsel-unknown-elf-as
CC = /opt/gcc-cross-mipsel/4.3.3/bin/mipsel-unknown-elf-gcc
LD = /opt/gcc-cross-mipsel/4.3.3/bin/mipsel-unknown-elf-ld
DU = /opt/gcc-cross-mipsel/4.3.3/bin/mipsel-unknown-elf-objdump
SYSCFLAGS = -Wall -mno-gpopt -ffreestanding -mips32 -I$(GIET_SYS_PATH) -I.
APPCFLAGS = -Wall -mno-gpopt -ffreestanding -mips32 -I$(GIET_APP_PATH) -I.
all : sys.bin app.bin

# Kernel
reset.o : reset.s
	$(AS) -g -mips32 -o reset.o reset.s
giet.o : $(GIET_SYS_PATH)/giet.s
	$(AS) -g -mips32 -o giet.o $(GIET_SYS_PATH)/giet.s
drivers.o : $(GIET_SYS_PATH)/drivers.c
	$(CC) $(SYSCFLAGS) -c -o drivers.o $(GIET_SYS_PATH)/drivers.c
common.o : $(GIET_SYS_PATH)/common.c
	$(CC) $(SYSCFLAGS) -c -o common.o $(GIET_SYS_PATH)/common.c
ctx_handler.o : $(GIET_SYS_PATH)/ctx_handler.c
	$(CC) $(SYSCFLAGS) -c -o ctx_handler.o $(GIET_SYS_PATH)/ctx_handler.c
irq_handler.o : $(GIET_SYS_PATH)/irq_handler.c
	$(CC) $(SYSCFLAGS) -c -o irq_handler.o $(GIET_SYS_PATH)/irq_handler.c
sys_handler.o : $(GIET_SYS_PATH)/sys_handler.c
	$(CC) $(SYSCFLAGS) -c -o sys_handler.o $(GIET_SYS_PATH)/sys_handler.c
exc_handler.o : $(GIET_SYS_PATH)/exc_handler.c
	$(CC) $(SYSCFLAGS) -c -o exc_handler.o $(GIET_SYS_PATH)/exc_handler.c

sys.bin : sys.ld reset.o giet.o drivers.o common.o ctx_handler.o irq_handler.o\
	  sys_handler.o exc_handler.o
	$(LD) -o sys.bin -T $^

# Application

stdio.o : $(GIET_APP_PATH)/stdio.c
	$(CC) $(APPCFLAGS) -c -o stdio.o $(GIET_APP_PATH)/stdio.c
main.o : main.c 
	$(CC) $(APPCFLAGS) -c -o main.o main.c 
app.bin : app.ld stdio.o main.o
	$(LD) -o app.bin -T app.ld stdio.o main.o

# Debug dissasembly

sys.bin.txt : sys.bin
	$(DU) -D sys.bin > sys.bin.txt
app.bin.txt : app.bin
	$(DU) -D app.bin > app.bin.txt

clean :
	rm -f reset.o giet.o drivers.o common.o ctx_handler.o irq_handler.o\
	sys_handler.o exc_handler.o stdio.o main.o *.txt

mrproper :
	rm -f sys.bin app.bin	
