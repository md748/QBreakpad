#ifndef QBREAKPAD_H
#define QBREAKPAD_H

#include <QObject>

class QBreakpadPrivate;

#ifdef QBREAKPAD_LINRARY
#define DYNAMIC_LIBRARY Q_DECL_EXPORT
#else
#define DYNAMIC_LIBRARY
#endif 

class DYNAMIC_LIBRARY QBreakpad : public QObject
{
public:
    virtual ~QBreakpad();
    static QBreakpad *instance();

    //strApp:push to reporter's command line arguments, and reporter use to restart program.
    void Init(const QString& dumpPath, const QString &strApp = QString());
    bool WriteMinidump();
    void SetReporter(const QString &strReporter);
    void SetApplication(const QString &strApp);
    //cache extra file name, push to reporter cmd.
    void AppendInfoFile(const QString &strFile);

protected:
    QBreakpad();

private:
    //Q_DECLARE_PRIVATE(QBreakpad)
    friend class QBreakpadPrivate;
    QBreakpadPrivate * const d_ptr;
};

#endif // QBREAKPAD_H
