NAME=sems
LIBNAME=sems.so

PLUGIN_DIR=plug-in
SIP_STACK_DIR=sip

SRCS=$(filter-out $(NAME).cpp, $(wildcard *.cpp))
HDRS=$(SRCS:.cpp=.h)
OBJS=$(SRCS:.cpp=.o) sip/sip_stack.a
DEPS=$(SRCS:.cpp=.d) $(NAME).d
AUDIO_FILES=$(notdir $(wildcard wav/*.wav))

.PHONY: all
all: ../Makefile.defs
	-@$(MAKE) sip_stack && \
	  $(MAKE) deps    && \
	  $(MAKE) $(NAME) && \
	  $(MAKE) modules


.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) $(NAME) $(NAME).o
	rm -f lib/*.so compat/getos compat/getarch
	$(MAKE) -C $(PLUGIN_DIR) clean

.PHONY: sip_stack
sip_stack:
	-@echo ""
	-@echo "making SIP stack"
	-@cd $(SIP_STACK_DIR); $(MAKE) all

.PHONY: modules
modules:
	-@echo ""
	-@echo "making core modules"
	-@cd $(PLUGIN_DIR); $(MAKE) modules

.PHONY: deps
deps: $(DEPS)

COREPATH=.

include ../Makefile.defs

# implicit rules
%.o : %.cpp %.d ../Makefile.defs
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(CXXFLAGS)

%.d : %.cpp %.h ../Makefile.defs
	$(CXX) -MM $< $(CPPFLAGS) $(CXXFLAGS) > $@

$(NAME): $(NAME).o $(OBJS) ../Makefile.defs
	-@echo ""
	-@echo "making $(NAME) executable"
	$(LD) -o $(NAME) $(NAME).o $(OBJS) $(LDFLAGS) $(EXTRA_LDFLAGS)

install: all mk-install-dirs \
	install-audio \
	install-modules-cfg \
	install-cfg \
	install-bin \
	install-modules \



# note: on solaris 8 sed: ? or \(...\)* (a.s.o) do not work
install-cfg: $(DESTDIR)$(cfg-target)
		sed -e "s#/usr/.*lib/sems/audio/#$(audio-target)#g" \
			-e "s#/usr/.*lib/sems/plug-in/#$(modules-target)#g" \
			-e "s#/usr/.*etc/sems#$(cfg-target)#g" \
			< etc/sems.conf.sample > $(DESTDIR)$(cfg-target)sems.conf.default
		chmod 644 $(DESTDIR)$(cfg-target)sems.conf.default
		if [ ! -f $(DESTDIR)$(cfg-target)sems.conf ]; then \
			cp -f $(DESTDIR)$(cfg-target)sems.conf.default \
				$(DESTDIR)$(cfg-target)sems.conf; \
		fi

# Make sure the destination exists and is a directory!
		mkdir -p $(DESTDIR)$(cfg-target)etc
		$(INSTALL-CFG) etc/app_mapping.conf $(DESTDIR)$(cfg-target)etc/

install-bin: $(DESTDIR)$(bin-prefix)/$(bin-dir)
		$(INSTALL-TOUCH) $(DESTDIR)$(bin-prefix)/$(bin-dir)$(NAME)
		$(INSTALL-BIN) $(NAME) $(DESTDIR)$(bin-prefix)/$(bin-dir)

install-modules: $(PLUGIN_DIR) $(DESTDIR)$(modules-prefix)/$(modules-dir)
	$(MAKE) -C $(PLUGIN_DIR) install

install-modules-cfg: $(PLUGIN_DIR) 
	$(MAKE) -C $(PLUGIN_DIR) install-cfg

install-ser-cfg: etc/ser-sems.cfg
	mkdir -p $(DESTDIR)$(ser-cfg-target)

	if [ ! -f  $(DESTDIR)$(ser-cfg-target)/ser-sems.cfg ]; then \
		sed -e "s#/opt/ser-sems#$(ser-prefix)#g" \
			< etc/ser-sems.cfg > $(DESTDIR)$(ser-cfg-target)ser-sems.cfg; \
	fi

	$(INSTALL-CFG) etc/ser-sems.cfg $(DESTDIR)$(ser-cfg-target)ser-sems.cfg.default

	@echo \*\*\* installed ser-sems.cfg at $(DESTDIR)$(ser-cfg-target)ser-sems.cfg
	@echo \*\*\* run $(DESTDIR)$(ser-prefix)/sbin/ser -f $(DESTDIR)$(ser-cfg-target)ser-sems.cfg
	@echo \*\*\* to start ser for sems.

install-audio: $(DESTDIR)$(audio-prefix)/$(audio-dir)
	for f in $(AUDIO_FILES) ; do \
		if [ -n "wav/$$f" ]; then \
			$(INSTALL-TOUCH) $(DESTDIR)$(audio-prefix)/$(audio-dir)$$f; \
			$(INSTALL-AUDIO) wav/$$f $(DESTDIR)$(audio-prefix)/$(audio-dir)$$f; \
		fi ; \
	done

dist: tar

tar: 
	$(TAR) -C .. \
		--exclude=$(notdir $(CURDIR))/ivr \
		--exclude=$(notdir $(CURDIR))/tmp \
		--exclude=.svn* \
		--exclude=.\#* \
		--exclude=*.[do] \
		--exclude=*.la \
		--exclude=*.lo \
		--exclude=*.so \
		--exclude=*.il \
		--exclude=$(notdir $(CURDIR))/sems \
		--exclude=$(notdir $(CURDIR))/build-stamp \
		--exclude=$(notdir $(CURDIR))/configure-stamp \
		--exclude=$(notdir $(CURDIR))/debian/sems-core \
		--exclude=*.gz \
		--exclude=*.bz2 \
		--exclude=*.tar \
		--exclude=*~ \
		-cf - $(notdir $(CURDIR)) | \
			(mkdir -p tmp/_tar1; mkdir -p tmp/_tar2 ; \
			    cd tmp/_tar1; $(TAR) -xf - ) && \
			    mv tmp/_tar1/$(notdir $(CURDIR)) \
			       tmp/_tar2/"$(NAME)-$(RELEASE)" && \
			    (cd tmp/_tar2 && $(TAR) \
			                    -zcf ../../"$(NAME)-$(notdir $(CURDIR))-$(RELEASE)".tar.gz \
			                               "$(NAME)-$(RELEASE)" ) ; \
			    rm -rf tmp/_tar1; rm -rf tmp/_tar2

ifeq '$(NAME)' '$(MAKECMDGOALS)'
include $(DEPS)
endif


