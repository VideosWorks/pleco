# This project assumes that char is quint8
QMAKE_CXXFLAGS += -funsigned-char

CONFIG += qt debug warn_on ordered
TEMPLATE = subdirs
SUBDIRS = common controller slave netrelay
controller.depends = common
slave.depends = common
