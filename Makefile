#------------------------------------
#
include $(PROJDIR:%=%/)builder/proj.mk

.DEFAULT_GOAL=all

export APP_ATTR?=

#------------------------------------
#
#/home/joelai/07_sw/pkg/qt5.12.3/5.12.3/gcc_64
QT_PATH?=$(lastword $(wildcard \
  $(HOME)/07_sw/pkg/qt*/**/gcc_64 \
  $(HOME)/07_sw/qt*/**/gcc_64 \
  /mnt/hd_dc/07_sw/pkg/qt*/**/gcc_64))
QT_SPEC?=linux-g++
QMAKE?=$(QT_PATH)/bin/qmake -makefile -spec $(QT_SPEC) -o Makefile

#------------------------------------
#
comsvc_DIR=$(PROJDIR)/comsvc

comsvc_clean comsvc_distclean:
	if [ -e $(comsvc_DIR)/Makefile ]; then \
	  $(MAKE) -C $(comsvc_DIR) $(patsubst _%,%,$(@:comsvc%=%)); \
	fi

comsvc_makefile:
	cd $(comsvc_DIR) && $(QMAKE) CONFIG+=debug comsvc.pro

$(comsvc_DIR)/Makefile: $(comsvc_DIR)/comsvc.pro
	$(MAKE) comsvc_makefile

comsvc: $(comsvc_DIR)/Makefile
	$(MAKE) -C $(comsvc_DIR) $(patsubst _%,%,$(@:comsvc%=%))

.PHONY: comsvc

#------------------------------------
#------------------------------------
#------------------------------------
#------------------------------------
#------------------------------------
