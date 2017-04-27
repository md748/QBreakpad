# QBreakpad
based on google-breakpad. added WindowsDllInterceptor, can catch all exception

1. pro

    LIBS += -L. -lQBreakpad4
    DEFINES += QBREAKPAD_LINRARY
    INCLUDEPATH += ../QBreakpad


2.main

   int main(int argc, char *argv[])
   {
      QApplication a(argc, argv);

      QBreakpad::instance()->Init(".", a.arguments().first());
      QBreakpad::instance()->SetReporter("../bin/reporter.exe");
      QBreakpad::instance()->AppendInfoFile("./log.txt");
      QBreakpad::instance()->AppendInfoFile("../config.init");

      Widget w;
      w.show();

      return a.exec();
   }
