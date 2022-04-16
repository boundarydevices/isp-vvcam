EXTRA_CFLAGS += -I$(PWD)/../common/ -O2 -Werror
ifeq ($(BASLER_BUILD), yes)
CFLAGS_basler-camera-driver-vvcam.o = -DBASLER_BUILD
endif
obj-m += basler-camera-driver-vvcam.o
