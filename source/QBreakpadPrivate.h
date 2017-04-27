#ifndef QBREAKPADPRIVATE_H
#define QBREAKPADPRIVATE_H

#include <QObject>
#include <QDir>
#include <QProcess>
#include <QString>

#if defined(Q_OS_LINUX)
#include "breakpad/client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN)
#include "breakpad/client/windows/handler/exception_handler.h"
#elif defined(Q_OS_MACX)
#include "breakpad/client/mac/handler/exception_handler.h"
#endif

class QBreakpad;

class QBreakpadPrivate : public QObject
{
public:
    QBreakpadPrivate(QBreakpad *p);
    ~QBreakpadPrivate();

private:
    void InitCrashHandler(const QString& dumpPath, const QString &strApp = QString());
    bool WriteMinidump();
    void SetReporter(const QString &strRep);
    void SetApplication(const QString &strApp);
    void AppendInfoFile(const QString &strFile);

private:
    friend class QBreakpad;
    //Q_DECLARE_PUBLIC(QBreakpad)
    QBreakpad * const q_ptr;

public:
    static google_breakpad::ExceptionHandler* pHandler;
    static bool bReportCrashesToSystem;
    static QString strDumpPath;
    static QString strReporter;
    static QString strApplication;
    static QStringList stlstFiles;
};

#endif // QBREAKPADPRIVATE_H

