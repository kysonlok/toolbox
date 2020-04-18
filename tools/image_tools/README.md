# HOW TO USE  

$ vi Makefile.16M  

```  
#
# Change uboot/kernel size if necessary
#
UBOOT_SIZE = 0x50000
KERNEL_SIZE = 0x3B0000
#-----------------------------------------------
USER_NAME = $(shell whoami)
#
# Uboot Image Information
#
UBOOT_DIR = .
UBOOT_IMAGE = uboot.bin
#
# Linux Kernel Image Information
#
KERNEL_DIR = .
KERNEL_IMAGE = steven_uImage
#
# Single Image Information
#
PACKED_DIR = .
PACKED_IMAGE = kyson_uImage.img
```  

> make -f Makefile.16M  

The single image can now be burned using the flash programmer.
