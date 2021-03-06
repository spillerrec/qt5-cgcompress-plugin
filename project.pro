TARGET  = $$qtLibraryTarget(cgcompress)
TEMPLATE = lib
CONFIG += plugin
LIBS += -larchive -lz
win32{
	LIBS += -liconv
}

# C++11 support
QMAKE_CXXFLAGS += -std=c++11

SOURCES += OraPlugin.cpp OraHandler.cpp pugixml.cpp
HEADERS =  OraPlugin.hpp OraHandler.hpp pugixml.hpp
OTHER_FILES += ora.json
