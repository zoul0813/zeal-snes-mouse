# Specify the name of the output binary.
BIN=mouse.bin

# include $(ZGDK_PATH)/base_sdcc.mk
include $(ZVB_SDK_PATH)/sdcc/base_sdcc.mk
# include $(ZOS_PATH)/kernel_headers/sdcc/base_sdcc.mk

all::
	ls -l bin/mouse.bin