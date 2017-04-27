#include "QBreakpad.h"
#include "QBreakpadPrivate.h"

QBreakpad::QBreakpad() : d_ptr(new QBreakpadPrivate(this))
{
}

QBreakpad::~QBreakpad()
{
    delete d_ptr;
}

QBreakpad *QBreakpad::instance()
{
    static QBreakpad s_breakpad;
    return &s_breakpad;
}

void QBreakpad::Init(const QString &strPath, const QString &strApp)
{
    d_ptr->InitCrashHandler(strPath, strApp);
}

bool QBreakpad::WriteMinidump()
{
    return d_ptr->WriteMinidump();
}

void QBreakpad::SetReporter(const QString &strReporter)
{
    d_ptr->SetReporter(strReporter);
}

void QBreakpad::SetApplication(const QString &strApp)
{
    d_ptr->SetApplication(strApp);
}

void QBreakpad::AppendInfoFile(const QString &strFile)
{
    d_ptr->AppendInfoFile(strFile);
}
