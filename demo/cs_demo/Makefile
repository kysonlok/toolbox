#
# Copyright (C) 2016-2017 kyson Lok <kysonlok@gmail.com>
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=controller
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_CONFIG_DEPENDS:=

include $(INCLUDE_DIR)/package.mk

define Package/controller
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Control 4-wireless 74hc138
endef

define Package/controller/description
  Controlling 4-wireless 74hc138
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += -Wno-error=format-security 

define Build/Configure
endef

define Package/controller/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ctl_client $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ctl_server $(1)/usr/bin
endef

$(eval $(call BuildPackage,controller))

