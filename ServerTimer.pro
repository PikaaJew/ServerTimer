TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LFLAGS += -static -static-libgcc

SOURCES += \
        interface.cpp \
        main.cpp \
        methods.cpp \
        read_write.cpp \
        server.cpp

HEADERS += \
    interface.hpp \
    methods.hpp \
    read_write.hpp \
    server.hpp
