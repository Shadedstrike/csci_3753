# csci_3753
CU Boulder, operating systems class, Homework two


Name: April Ott, april.ott@colorado.edu, id: 103303208


Files within the pa2_char_driver_module folder:

Makefile - provides instructions to compile the module_exit

PA2_test.c - test c file to verify that the driver module is working.

pa2_char_driver.ko - main kernel module file, compiled with "sudo make -C /lib/modules/$(uname -r)/build M=$PWD".

pa2_char_driver.c - main source code for the LKM.
