include $(TOPDIR)/rules.mk

PKG_NAME:=udemod
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/udemod
  SECTION:=Vendor
  CATEGORY:=Vendor
  DEPENDS:= +libmosquitto +libpthread 
  TITLE:=udemod
endef

define Package/udemod/description
	for iot api demo
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
TARGET_CFLAGS += -lm

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/udemod/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/udemod $(1)/usr/sbin/udemod
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,udemod))
