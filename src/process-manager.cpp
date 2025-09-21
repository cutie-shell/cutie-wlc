#include "process-manager.h"

#include <QDebug>
#include <QEventLoop>
#include <QProcess>
#include <QTimer>

CwlProcessManager::CwlProcessManager(QObject *parent)
	: QObject(parent)
	, m_criticalComponents({ "cutie-home", "cutie-panel" })
	, m_retryTimer(new QTimer(this))
	, m_currentRetryAttempt(0)
	, m_maxRetryAttempts(0)
{
	m_retryTimer->setSingleShot(true);
	connect(m_retryTimer, &QTimer::timeout, this,
		&CwlProcessManager::onRetryTimeout);
}

bool CwlProcessManager::launchComponent(const QString &command)
{
	QStringList args;
	args.append("-c");
	args.append(command);

	if (!QProcess::startDetached("bash", args)) {
		QString componentName = extractComponentName(command);
		qWarning() << "Failed to launch component:" << componentName;
		notifyComponentFailure(componentName, "Process launch failed");
		return false;
	}

	qDebug() << "Successfully launched component:" << command;
	return true;
}

bool CwlProcessManager::launchComponentWithRetry(const QString &command,
						 int maxRetries)
{
	QString componentName = extractComponentName(command);

	for (int attempt = 1; attempt <= maxRetries; ++attempt) {
		qDebug() << "Launching component" << componentName
			 << "- attempt" << attempt << "of" << maxRetries;

		if (launchComponent(command)) {
			if (attempt > 1) {
				qInfo() << "Component" << componentName
					<< "launched successfully after"
					<< attempt << "attempts";
			}
			emit componentLaunched(componentName, attempt);
			return true;
		}

		if (attempt < maxRetries) {
			qWarning()
				<< "Launch attempt" << attempt << "failed for"
				<< componentName << "- retrying in 1 second";

			// Use a simple delay for retry
			QEventLoop loop;
			QTimer::singleShot(1000, &loop, &QEventLoop::quit);
			loop.exec();
		}
	}

	qCritical() << "Failed to launch component" << componentName << "after"
		    << maxRetries << "attempts";

	// Track failed component
	if (!m_failedComponents.contains(componentName)) {
		m_failedComponents.append(componentName);
	}

	// Notify about component failure
	bool critical = isComponentCritical(componentName);
	notifyComponentFailure(
		componentName,
		QString("Failed after %1 attempts").arg(maxRetries));
	emit componentLaunchFailed(
		componentName,
		QString("Failed after %1 attempts").arg(maxRetries), critical);

	return false;
}

void CwlProcessManager::launchEssentialComponents(const QString &launcher)
{
	// Launch essential components with retry logic
	launchComponentWithRetry("cutie-home", 3);
	launchComponentWithRetry(launcher, 3);
	launchComponentWithRetry("cutie-panel", 3);
	launchComponentWithRetry("cutie-keyboard",
				 2); // Less critical, fewer retries
	launchComponentWithRetry(
		"env XDG_CURRENT_DESKTOP=GNOME /usr/libexec/feedbackd", 2);
	launchComponentWithRetry("loginctl activate",
				 1); // System command, minimal retry
}

bool CwlProcessManager::isComponentCritical(const QString &component) const
{
	QString componentName = extractComponentName(component);
	return m_criticalComponents.contains(componentName);
}

void CwlProcessManager::addCriticalComponent(const QString &component)
{
	QString componentName = extractComponentName(component);
	if (!m_criticalComponents.contains(componentName)) {
		m_criticalComponents.append(componentName);
	}
}

void CwlProcessManager::removeCriticalComponent(const QString &component)
{
	QString componentName = extractComponentName(component);
	m_criticalComponents.removeAll(componentName);
}

void CwlProcessManager::onRetryTimeout()
{
	// This could be used for more sophisticated async retry logic in the future
	// For now, we use synchronous retries in launchComponentWithRetry()
}

void CwlProcessManager::notifyComponentFailure(const QString &component,
					       const QString &error)
{
	QString componentName = extractComponentName(component);

	if (isComponentCritical(componentName)) {
		qCritical() << "CRITICAL COMPONENT FAILURE:" << componentName
			    << "-" << error;
		qCritical()
			<< "The desktop environment may not function properly.";

		// Provide specific guidance for critical component failures
		if (componentName == "cutie-home") {
			qCritical()
				<< "Home screen failed to start - desktop navigation will be unavailable";
		} else if (componentName == "cutie-panel") {
			qCritical()
				<< "Panel failed to start - system controls may be inaccessible";
		}
	} else {
		qWarning() << "Non-critical component failure:" << componentName
			   << "-" << error;
	}
}

QString CwlProcessManager::extractComponentName(const QString &command) const
{
	return command.split(" ").first();
}