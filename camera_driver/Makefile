obj-m := GEC_camera.o
GEC_camera-objs:=s3c2440_ov9650.o s3c2440camif.o sccb.o
PWD=$(shell pwd)
#KERN_DIR = /home/s5pv210/linux-2.6.35.7
KERN_DIR = /home/sources/GEC/linux-2.6.30.4
modules:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules
clean:
	rm -rf *.o *.~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
