plug_in_name = mp3

COREPATH ?=../../core
LAME_DIR = ./lame-3.96.1

module_ldflags =  -lmp3lame 
module_cflags  = -I/usr/include/lame

# if you want to build lame from source use these 
# module_ldflags and module_extra_objs

#module_ldflags    = -L$(LAME_DIR)/libmp3lame/.libs 
#module_extra_objs = $(LAME_DIR)/libmp3lame/.libs/libmp3lame.a
#module_cflags     = -I$(LAME_DIR)/include


extra_clean = 

include $(COREPATH)/plug-in/Makefile.audio_module

#.PHONY: clean_libmp3lame
#clean_libmp3lame:
#	cd $(LAME_DIR); $(MAKE) clean

$(LAME_DIR)/libmp3lame/.libs/libmp3lame.a: 
	cd $(LAME_DIR); ./configure ; $(MAKE)
