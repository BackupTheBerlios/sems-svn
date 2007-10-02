#
# Copyright (C) 2006 OpenWrt.org
# Copyright (C) 2007 Stefan Sayer
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 5624 2006-11-23 00:29:07Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME:=sems
PKG_VERSION:=0.10.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-bundle.tar.gz
PKG_SOURCE_URL:=http://ftp.iptel.org/pub/sems/
PKG_MD5SUM:=d4464804083daa1373828f130b5c6fe8
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/sems
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=
  TITLE:=SIP Express Media Server (SEMS)
  DESCRIPTION:= \
	SEMS is a free, high performance, extensible media server \\\
	for SIP (RFC3261) based VoIP services.
  URL:=http://iptel.org/sems/
endef

define Package/sems-codec-gsm
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems 
  TITLE:=GSM codec for SEMS 
  DESCRIPTION:= \
	GSM codec for SEMS. Only use this if your device has \\\
	enough processing power
  URL:=http://iptel.org/sems/
endef

define Package/sems-codec-ilbc
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems 
  TITLE:=iLBC codec for SEMS 
  DESCRIPTION:= \
	iLBC codec for SEMS. Only use this if your device has \\\
	enough processing power
  URL:=http://ilbcfreeware.org/
endef

define Package/sems-conference
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems 
  TITLE:=SEMS conference application
  URL:=http://iptel.org/sems/
endef

define Package/sems-announcement
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems 
  TITLE:=SEMS announcement application
  URL:=http://iptel.org/sems/
endef

define Package/sems-announcement-sampleaudio
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems-announcement 
  TITLE:=sample audio files for SEMS announcement
  URL:=http://iptel.org/sems/
endef

define Package/sems-echo
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems sems-timers
  TITLE:=SEMS echo application
  URL:=http://iptel.org/sems/
endef

define Package/sems-voicemail
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems sems-timers 
  TITLE:=SEMS voicemail application
  URL:=http://iptel.org/sems/
endef

define Package/sems-voicemail-sampleaudio
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems-voicemail 
  TITLE:=sample audio files for SEMS voicemail
  URL:=http://iptel.org/sems/
endef

define Package/sems-regagent
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems sems-uac-auth
  TITLE:=SEMS application for registration at SIP Registrar
  URL:=http://iptel.org/sems/
endef

define Package/sems-uac-auth
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems 
  TITLE:=SEMS module for UAC authentication
  URL:=http://iptel.org/sems/
endef

define Package/sems-timers
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems 
  TITLE:=SEMS module for SIP session timers and user timers
  URL:=http://iptel.org/sems/
endef

define Package/sems-stats
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems 
  TITLE:=SEMS module for statistics and DI invocation
  URL:=http://iptel.org/sems/
endef

define Package/sems-xmlrpc2di
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems 
  TITLE:=SEMS module for DI invocation over XMLRPC
  URL:=http://iptel.org/sems/
endef

define Package/sems-webconference
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems sems-uac-auth sems-timers sems-xmlrpc2di
  TITLE:=SEMS module for conference with interface (e.g. XMLRPC controlled)
  URL:=http://iptel.org/sems/
endef

define Package/sems-webconference-sampleaudio
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems-webconference
  TITLE:=sample audio files for SEMS webconference module
  URL:=http://iptel.org/sems/
endef

define Package/sems-serviceline
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=sems sems-uac-auth
  TITLE:=SEMS serviceline application (auto-attendant)
  URL:=http://iptel.org/sems/
endef

define Build/Configure
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR)/apps \
		CXX=$(TARGET_CROSS)g++ \
		CC=$(TARGET_CROSS)gcc \
		ARCH="$(ARCH)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		exclude_modules="examples announce_transfer conf_auth early_announce ivr \
				mp3 py_sems ann_b2b \
				mailbox pin_collect" \
		install

	$(MAKE) -C $(PKG_BUILD_DIR)/apps/examples/xmlrpc2di \
		CXX=$(TARGET_CROSS)g++ \
		CC=$(TARGET_CROSS)gcc \
		ARCH="$(ARCH)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install-xmlrpcpp install 

	$(MAKE) -C $(PKG_BUILD_DIR)/apps/examples/serviceline \
		CXX=$(TARGET_CROSS)g++ \
		CC=$(TARGET_CROSS)gcc \
		ARCH="$(ARCH)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install 

	$(MAKE) -C $(PKG_BUILD_DIR)/core \
		CXX=$(TARGET_CROSS)g++ \
		CC=$(TARGET_CROSS)gcc \
		ARCH="$(ARCH)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install

	$(MAKE) -C $(PKG_BUILD_DIR)/ser-0.9.6-sems \
		CXX=$(TARGET_CROSS)g++ \
		CC=$(TARGET_CROSS)gcc \
		ARCH="$(ARCH)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define Package/sems/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/sems $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/ser $(1)/usr/sbin/ser-sems

	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/wav.so $(1)/usr/lib/sems/plug-in/

	$(INSTALL_DIR) $(1)/etc/sems/
	$(INSTALL_CONF) ./files/sems.conf $(1)/etc/sems/
	$(INSTALL_CONF) ./files/ser-sems.cfg $(1)/etc/sems/	

	$(INSTALL_DIR) $(1)/lib/ser/modules/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/ser/modules/tm.so $(1)/lib/ser/modules/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/ser/modules/sl.so $(1)/lib/ser/modules/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/sems.init $(1)/etc/init.d/sems
endef

define Package/sems-codec-gsm/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/gsm.so $(1)/usr/lib/sems/plug-in/
endef

define Package/sems-codec-ilbc/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/ilbc.so $(1)/usr/lib/sems/plug-in/
endef

define Package/sems-conference/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/conference.so $(1)/usr/lib/sems/plug-in/
	$(INSTALL_DIR) $(1)/usr/lib/sems/audio/conference
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/lib/sems/audio/conference/* $(1)/usr/lib/sems/audio/conference/
	$(INSTALL_DIR) $(1)/etc/sems/etc
	$(INSTALL_CONF) ./files/conference.conf $(1)/etc/sems/etc/
endef

define Package/sems-announcement/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/announcement.so $(1)/usr/lib/sems/plug-in/
	$(INSTALL_DIR) $(1)/usr/lib/sems/audio/announcement
	$(INSTALL_DIR) $(1)/etc/sems/etc
	$(INSTALL_CONF) ./files/announcement.conf $(1)/etc/sems/etc/
endef

define Package/sems-announcement-sampleaudio/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/audio/announcement
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/lib/sems/audio/default_en.wav $(1)/usr/lib/sems/audio/announcement/
endef

define Package/sems-echo/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/echo.so $(1)/usr/lib/sems/plug-in/
endef

define Package/sems-uac-auth/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/uac_auth.so $(1)/usr/lib/sems/plug-in/
endef

define Package/sems-timers/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/session_timer.so $(1)/usr/lib/sems/plug-in/
endef

define Package/sems-xmlrpc2di/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/xmlrpc2di.so $(1)/usr/lib/sems/plug-in/
	$(INSTALL_DIR) $(1)/etc/sems/etc
	$(INSTALL_CONF) ./files/xmlrpc2di.conf $(1)/etc/sems/etc/
endef

define Package/sems-stats/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/stats.so $(1)/usr/lib/sems/plug-in/
endef

define Package/sems-voicemail/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/voicemail.so $(1)/usr/lib/sems/plug-in/
	$(INSTALL_DIR) $(1)/usr/lib/sems/audio/voicemail
	$(INSTALL_DIR) $(1)/etc/sems/etc
	$(INSTALL_CONF) ./files/voicemail.conf $(1)/etc/sems/etc/
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/etc/sems/default.template $(1)/etc/sems/
endef

define Package/sems-voicemail-sampleaudio/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/audio/voicemail
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/lib/sems/audio/voicemail/* $(1)/usr/lib/sems/audio/voicemail/
endef

define Package/sems-regagent/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/reg_agent.so $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/registrar_client.so $(1)/usr/lib/sems/plug-in/
	$(INSTALL_DIR) $(1)/etc/sems/etc
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/etc/sems/etc/reg_agent.conf $(1)/etc/sems/etc/
endef

define Package/sems-webconference/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/webconference.so $(1)/usr/lib/sems/plug-in/
	$(INSTALL_DIR) $(1)/usr/lib/sems/audio/webconference
	$(INSTALL_DIR) $(1)/etc/sems/etc
	$(INSTALL_CONF) ./files/webconference.conf $(1)/etc/sems/etc/
endef

define Package/sems-webconference-sampleaudio/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/audio/webconference
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/lib/sems/audio/webconference/* $(1)/usr/lib/sems/audio/webconference/
endef

define Package/sems-serviceline/install	
	$(INSTALL_DIR) $(1)/usr/lib/sems/plug-in/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/sems/plug-in/serviceline.so $(1)/usr/lib/sems/plug-in/
	$(INSTALL_DIR) $(1)/usr/lib/sems/audio/serviceline
	$(INSTALL_DIR) $(1)/etc/sems/etc
	$(INSTALL_CONF) ./files/serviceline.conf $(1)/etc/sems/etc/
endef

$(eval $(call BuildPackage,sems))
$(eval $(call BuildPackage,sems-codec-gsm))
$(eval $(call BuildPackage,sems-codec-ilbc))
$(eval $(call BuildPackage,sems-echo))
$(eval $(call BuildPackage,sems-conference))
$(eval $(call BuildPackage,sems-uac-auth))
$(eval $(call BuildPackage,sems-announcement))
$(eval $(call BuildPackage,sems-announcement-sampleaudio))
$(eval $(call BuildPackage,sems-voicemail))
$(eval $(call BuildPackage,sems-voicemail-sampleaudio))
$(eval $(call BuildPackage,sems-regagent))
$(eval $(call BuildPackage,sems-timers))
$(eval $(call BuildPackage,sems-xmlrpc2di))
$(eval $(call BuildPackage,sems-webconference))
$(eval $(call BuildPackage,sems-webconference-sampleaudio))
$(eval $(call BuildPackage,sems-serviceline))
