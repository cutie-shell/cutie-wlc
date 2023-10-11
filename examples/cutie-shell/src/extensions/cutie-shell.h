#ifndef CUTIE_SHELL_H
#define CUTIE_SHELL_H

#include <QtWaylandClient/QWaylandClientExtension>
#include <QtGui/QWindow>
#include <QtQml/QQmlEngine>

#include "qwayland-cutie-shell-private.h"

QT_BEGIN_NAMESPACE

class CutieShell : public QWaylandClientExtensionTemplate<CutieShell>
	, public QtWayland::cutie_shell_private

{
	Q_OBJECT
	QML_ELEMENT
public:
	CutieShell();
	Q_INVOKABLE void execApp(const QString &path);

signals:

public slots:
	
private slots:
    void handleExtensionActive();

protected:

private:
	bool m_activated = false;
};

QT_END_NAMESPACE
#endif //CUTIE_SHELL_H