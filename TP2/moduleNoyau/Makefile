obj-m:=chenillard_mod.o
#obj-m:=hello_2crossComp.o
KERNEL_SOURCE=/home/ensea/linux-socfpga
CFLAGS_MODULE=-fno-pic # en lien avec l'architecture arm...

all :
	make -C $(KERNEL_SOURCE) M=$(PWD) modules
clean :
	make -C $(KERNEL_SOURCE) M=$(PWD) clean
install :
	make −C $(KERNEL_SOURCE) M=$(PWD) modules install

