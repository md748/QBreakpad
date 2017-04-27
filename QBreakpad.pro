
VERSION  = 1.0.0.1

TEMPLATE = lib

CONFIG += plugin static

TARGET = QBreakpad$$QT_MAJOR_VERSION

!contains(CONFIG, static) {
DEFINES += QBREAKPAD_LINRARY
}

include($$PWD/QBreakpad.pri)

PALTFORM =

win32{
    PALTFORM = Windows
    !contains(CONFIG,static) {
        TARGET_EXT = .dll
    }
}
else:linux-g++-32{
    PALTFORM = Linux
}
else:linux-arm-g++{
    PALTFORM = ArmLinux
}
else:macx{
    PALTFORM = MacOSX
} else {
    error(paltform not found)
}

contains(QMAKE_HOST.arch, x86):{
    ARCHTYPE = x86
}

contains(QMAKE_HOST.arch, x86_64):{
    ARCHTYPE = x64
}

PALTFORM = $$PALTFORM'_'$$ARCHTYPE


CONFIG(debug,debug|release) {
    PALTFORM = $$PALTFORM'_d'
}

for(MSVER , QMAKE_COMPILER_DEFINES){
    verlist = $$split(MSVER, =)
    contains(verlist, _MSC_VER){
        MSVC_VERSION = $$member(verlist,1)
    }
}

DESTDIR  =  $$PWD/bin/breakpad$$MSVC_VERSION/$$PALTFORM
