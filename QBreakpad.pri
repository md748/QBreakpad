INCLUDEPATH += $$PWD/breakpad \
              $$PWD

HEADERS += $$PWD/breakpad/WindowsDllInterceptor.h \
           $$PWD/QBreakpad.h \
           $$PWD/source/QBreakpadPrivate.h

HEADERS += $$PWD/breakpad/common/scoped_ptr.h \
           $$PWD/breakpad/google_breakpad/common/breakpad_types.h \
           $$PWD/breakpad/google_breakpad/common/minidump_format.h


SOURCES += $$PWD/source/QBreakpad.cpp \
           $$PWD/source/QBreakpadPrivate.cpp

win32{

HEADERS += $$PWD/breakpad/common/windows/guid_string.h \
           $$PWD/breakpad/common/windows/string_utils-inl.h \
           $$PWD/breakpad/client/windows/common/ipc_protocol.h \
           $$PWD/breakpad/client/windows/crash_generation/crash_generation_client.h \
           $$PWD/breakpad/client/windows/handler/exception_handler.h

SOURCES += $$PWD/breakpad/client/windows/crash_generation/crash_generation_client.cc \
           $$PWD/breakpad/client/windows/handler/exception_handler.cc \
           $$PWD/breakpad/common/windows/guid_string.cc \
           $$PWD/breakpad/common/windows/string_utils.cc

}
else:linux-g++-32{

}
else:macx{

LIBS  += -framework CoreFoundation -framework CoreServices

HEADERS += $$PWD/breakpad/client/mac/crash_generation/crash_generation_client.h \
           $$PWD/breakpad/client/mac/handler/exception_handler.h \
           $$PWD/breakpad/common/mac/bootstrap_compat.h \
           $$PWD/breakpad/client/mac/handler/minidump_generator.h \
           $$PWD/breakpad/client/minidump_file_writer.h \
           $$PWD/breakpad/client/mac/handler/dynamic_images.h \
           $$PWD/breakpad/client/mac/handler/breakpad_nlist_64.h \
           $$PWD/breakpad/common/string_conversion.h \
           $$PWD/breakpad/common/convert_UTF.h \
           $$PWD/breakpad/common/md5.h \
           $$PWD/breakpad/common/mac/string_utilities.h \
           $$PWD/breakpad/common/mac/file_id.h \
           $$PWD/breakpad/common/mac/macho_id.h \
           $$PWD/breakpad/common/mac/macho_utilities.h \
           $$PWD/breakpad/common/mac/macho_walker.h

OBJECTIVE_HEADERS += $$PWD/breakpad/common/mac/MachIPC.h

SOURCES += $$PWD/breakpad/client/mac/crash_generation/crash_generation_client.cc \
           $$PWD/breakpad/client/mac/handler/exception_handler.cc \
           $$PWD/breakpad/common/mac/bootstrap_compat.cc \
           $$PWD/breakpad/client/mac/handler/minidump_generator.cc \
           $$PWD/breakpad/client/minidump_file_writer.cc \
           $$PWD/breakpad/client/mac/handler/dynamic_images.cc \
           $$PWD/breakpad/client/mac/handler/breakpad_nlist_64.cc \
           $$PWD/breakpad/common/string_conversion.cc \
           $$PWD/breakpad/common/convert_UTF.c \
           $$PWD/breakpad/common/md5.cc \
           $$PWD/breakpad/common/mac/string_utilities.cc \
           $$PWD/breakpad/common/mac/file_id.cc \
           $$PWD/breakpad/common/mac/macho_id.cc \
           $$PWD/breakpad/common/mac/macho_utilities.cc \
           $$PWD/breakpad/common/mac/macho_walker.cc

OBJECTIVE_SOURCES += $$PWD/breakpad/common/mac/MachIPC.mm

} else {
    error(paltform not found)
}






win32-msvc*{
    QMAKE_CFLAGS   += /Zi
    QMAKE_CXXFLAGS += /Zi
    QMAKE_CXXFLAGS += /wd4996 /wd4819 #/WX
    QMAKE_LFLAGS_RELEASE += /MAP /debug /opt:ref /INCREMENTAL:NO /DEBUG
    QMAKE_CXXFLAGS_STL_ON   -= -EHsc
    QMAKE_CXXFLAGS_EXCEPTIONS_ON -= -EHsc
    QMAKE_CXXFLAGS_STL_ON += -EHa
    QMAKE_CXXFLAGS_EXCEPTIONS_ON += -EHa
}


QMAKE_TARGET_COMPANY=
QMAKE_TARGET_PRODUCT=
QMAKE_TARGET_DESCRIPTION=
QMAKE_TARGET_COPYRIGHT=
