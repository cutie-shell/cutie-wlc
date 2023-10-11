#include "cutie-shell.h"

QT_BEGIN_NAMESPACE

CutieShell::CutieShell()
    : QWaylandClientExtensionTemplate(/* Supported protocol version */ 1 )
{
    connect(this, &CutieShell::activeChanged, this, &CutieShell::handleExtensionActive);
}

void CutieShell::handleExtensionActive()
{
    if (isActive() && !m_activated) {
        m_activated = true;
        qDebug()<<"activated";
    }
}

void CutieShell::execApp(const QString &path)
{
    qDebug()<<path;
    exec_app(path);
}

QT_END_NAMESPACE