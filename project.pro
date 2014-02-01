TARGET  = $$qtLibraryTarget(ora)
TEMPLATE = lib
CONFIG += plugin
LIBS += -larchive -liconv -lz

SOURCES += OraPlugin.cpp OraHandler.cpp pugixml.cpp
HEADERS =  OraPlugin.hpp OraHandler.hpp pugixml.hpp
OTHER_FILES += ora.json