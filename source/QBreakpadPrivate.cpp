#include "QBreakpadPrivate.h"
#include "QBreakpad.h"
#include <QDir>
#include <QProcess>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDebug>

google_breakpad::ExceptionHandler* QBreakpadPrivate::pHandler = 0;
bool QBreakpadPrivate::bReportCrashesToSystem = false;
QString QBreakpadPrivate::strDumpPath = "";
QString QBreakpadPrivate::strReporter = "";
QString QBreakpadPrivate::strApplication = "";
QStringList QBreakpadPrivate::stlstFiles;

#if defined(Q_OS_WIN)
#include "WindowsDllInterceptor.h"
static WindowsDllInterceptor g_Kernel32Intercept;
typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI *SetUnhandledExceptionFilterRedirection)(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

static SetUnhandledExceptionFilterRedirection SetUnhandledExceptionFilterFunc = 0;
static bool g_BlockUnhandledExceptionFilter = true;
static LPTOP_LEVEL_EXCEPTION_FILTER previousUnhandledExceptionFilter = nullptr;


static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilterPatched (LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
    if (!g_BlockUnhandledExceptionFilter)
    {
        // don't intercept
        return SetUnhandledExceptionFilterFunc(lpTopLevelExceptionFilter);
    }

    if (lpTopLevelExceptionFilter == previousUnhandledExceptionFilter)
    {
        // OK to swap back and forth between the previous filter
        previousUnhandledExceptionFilter = SetUnhandledExceptionFilterFunc(lpTopLevelExceptionFilter);
        return previousUnhandledExceptionFilter;
    }

    // intercept attempts to change the filter
    return nullptr;
}
#endif


#if defined(Q_OS_WIN)
bool DumpCallback(const wchar_t *_dump_dir,
                  const wchar_t *_minidump_id,
                  void *context,
                  EXCEPTION_POINTERS *exinfo,
                  MDRawAssertionInfo *assertion,
                  bool succeeded)
#elif defined(Q_OS_LINUX)
bool DumpCallback(const google_breakpad::MinidumpDescriptor &md,void *context, bool succeeded)
#elif defined(Q_OS_MACX)
bool DumpCallback(const char *_dump_dir,
                  const char *_minidump_id,
                  void *context, bool succeeded)
#endif
{
    Q_UNUSED(context);
#if defined(Q_OS_WIN)
    Q_UNUSED(_dump_dir);
    Q_UNUSED(_minidump_id);
    Q_UNUSED(assertion);
    Q_UNUSED(exinfo);
#elif defined(Q_OS_MACX)
    Q_UNUSED(_dump_dir);
    Q_UNUSED(_minidump_id);
#endif

    if (!QBreakpadPrivate::strReporter.isEmpty())
    {
#if defined(Q_OS_WIN)
        QString strDumpName = QString::fromWCharArray(_dump_dir) + "/" + QString::fromWCharArray(_minidump_id) + ".dmp";
#elif defined(Q_OS_MACX)
        QString strDumpName = QString(_dump_dir) + "/" + QString(_minidump_id) + ".dmp";
#endif
        QStringList stlstArgs;
        stlstArgs << QBreakpadPrivate::strApplication <<
                     strDumpName;
        QProcess::startDetached(QBreakpadPrivate::strReporter, stlstArgs);
    }

    /*
    NO STACK USE, NO HEAP USE THERE !!!
    Creating QString's, using qDebug, etc. - everything is crash-unfriendly.
    */
    return QBreakpadPrivate::bReportCrashesToSystem ? succeeded : true;
}


#if defined(Q_OS_WIN)
void InvalidParameterHandlerFunc(const wchar_t* expression,
                                 const wchar_t* function,
                                 const wchar_t* file,
                                 unsigned int line,
                                 uintptr_t pReserved)
{
    Q_UNUSED(expression);
    Q_UNUSED(function);
    Q_UNUSED(file);
    Q_UNUSED(line);
    Q_UNUSED(pReserved);

    qDebug() << "InvalidParameterHandlerFunc";

    std::wstring strPath = QBreakpadPrivate::strDumpPath.toStdWString();
    bool res = google_breakpad::ExceptionHandler::WriteMinidump(strPath, DumpCallback, 0);
    if (!res)
    {
        qWarning() << "writeMinidump() failed.";
    }

    TerminateProcess(GetCurrentProcess(), 1);
}


void PurecallHandlerFunc()
{
    qDebug() << "PurecallHandlerFunc";
    std::wstring strPath = QBreakpadPrivate::strDumpPath.toStdWString();
    bool res = google_breakpad::ExceptionHandler::WriteMinidump(strPath, DumpCallback, 0);
    if (!res)
    {
        qWarning() << "writeMinidump() failed.";
    }

    TerminateProcess(GetCurrentProcess(), 1);
}
#endif

QBreakpadPrivate::QBreakpadPrivate(QBreakpad *p) : q_ptr(p)
{
    pHandler = 0;
}

QBreakpadPrivate::~QBreakpadPrivate()
{
    delete pHandler;
}

void QBreakpadPrivate::InitCrashHandler(const QString &dumpPath, const QString &strApp)
{
    if (pHandler != 0)
        return;

    QDir dir(dumpPath);
    strDumpPath = dir.absolutePath();
    if (!strApp.isEmpty())
    {
        QFileInfo info(strApp);
        strApplication = info.absoluteFilePath();
    }

#if defined(Q_OS_WIN)
    std::wstring wstrPath = strDumpPath.toStdWString();
    pHandler = new google_breakpad::ExceptionHandler(
                wstrPath,
                0,
                DumpCallback,
                0,
                google_breakpad::ExceptionHandler::HANDLER_ALL
                );
    pHandler->set_handle_debug_exceptions(true);

    _invalid_parameter_handler oldHandler;
    _purecall_handler old_pure_handle;

    oldHandler = _set_invalid_parameter_handler(InvalidParameterHandlerFunc);
    old_pure_handle = _set_purecall_handler(PurecallHandlerFunc);

    ///================================================================
    _CrtSetReportMode(_CRT_ASSERT, 0);

    g_BlockUnhandledExceptionFilter = true;
    g_Kernel32Intercept.Init("kernel32.dll");
    bool ok = g_Kernel32Intercept.AddHook(
                "SetUnhandledExceptionFilter",
                reinterpret_cast<intptr_t>(SetUnhandledExceptionFilterPatched),
                (void**) &SetUnhandledExceptionFilterFunc);
    if (!ok)
        qWarning() << "SetUnhandledExceptionFilter hook failed; crash reporter is vulnerable.";
    ///================================================================

#elif defined(Q_OS_LINUX)
    std::string pathAsStr = dumpPath.toStdString();
    google_breakpad::MinidumpDescriptor md(pathAsStr);
    pHandler = new google_breakpad::ExceptionHandler(
                md,
                /*FilterCallback*/ 0,
                DumpCallback,
                /*context*/ 0,
                true,
                -1
                );

#elif defined(Q_OS_MACX)
    std::string pathAsStr = dumpPath.toStdString();
    pHandler = new google_breakpad::ExceptionHandler(
                pathAsStr,
                /*FilterCallback*/ 0,
                DumpCallback,
                /*context*/ 0,
                true,
                0);
#endif
}


bool QBreakpadPrivate::WriteMinidump()
{

#if defined(Q_OS_WIN)
    std::wstring strPath = strDumpPath.toStdWString();
#elif defined(Q_OS_MACX)
    std::string strPath = strDumpPath.toStdString();
#endif
    bool res = google_breakpad::ExceptionHandler::WriteMinidump(strPath, DumpCallback, 0);
    if (!res)
    {
        qWarning() << "writeMinidump() failed.";
    }
    return res;
}

void QBreakpadPrivate::SetReporter(const QString &strRep)
{
    QFileInfo info(strRep);
    strReporter = info.absoluteFilePath();
}

void QBreakpadPrivate::SetApplication(const QString &strApp)
{
    QFileInfo info(strApp);
    strApplication = info.absoluteFilePath();
}

void QBreakpadPrivate::AppendInfoFile(const QString &strFile)
{
    QFileInfo info(strFile);
    QString strTemp = info.absoluteFilePath();
    if (strTemp.isEmpty())
        return;
    stlstFiles.append(strTemp);
}
