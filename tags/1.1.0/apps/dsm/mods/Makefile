DSMPATH = ../
COREPATH = $(DSMPATH)/../../core
.DEFAULT_GOAL := all

include $(COREPATH)/../Makefile.defs

exclude_modules ?= 
modules ?= $(filter-out $(subst ;, ,$(exclude_modules))\
		$(wildcard Makefile*) lib, \
		$(wildcard *) )


.PHONY: all
all: modules

.PHONY: clean
clean:
	-@rm -f lib/*.so
	-@for r in $(modules) "" ; do \
		if [ -n "$$r" ]; then \
			echo "" ; \
			echo "" ; \
			COREPATH=../$(COREPATH) $(MAKE) -C $$r clean ; \
		fi ; \
	done

.PHONY: modules
modules:
	-@for r in $(modules) "" ; do \
		if [ -n "$$r" ]; then \
			echo  "" ; \
			echo  "" ; \
			COREPATH=../$(COREPATH) $(MAKE) -C $$r all; \
		fi ; \
	done 

.PHONY: install
install: install-bin install-cfg

.PHONY: install-bin
install-bin:
	-@for r in $(modules) "" ; do \
		if [ -n "$$r" ]; then \
			echo "" ; \
			echo "" ; \
			COREPATH=../$(COREPATH) $(MAKE) -C $$r install; \
		fi ; \
	done

.PHONY: install-cfg
install-cfg: $(DESTDIR)$(cfg-target)
	-@for r in $(modules) "" ; do \
		if [ -n "$$r" ]; then \
			echo "" ; \
			echo "" ; \
			COREPATH=../$(COREPATH) $(MAKE) -C $$r install-cfg; \
		fi ; \
	done
