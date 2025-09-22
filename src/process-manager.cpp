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
	// Validate timer creation
	if (!m_retryTimer) {
		qCritical()
			<< "CwlProcessManager: Failed to create retry timer";
		return;
	}

	m_retryTimer->setSingleShot(true);

	// Validate signal connection with error checking
	if (!connect(m_retryTimer.data(), &QTimer::timeout, this,
		     &CwlProcessManager::onRetryTimeout)) {
		qWarning()
			<< "CwlProcessManager: Failed to connect retry timer timeout signal";
	}
}

bool CwlProcessManager::launchComponent(const QString &command)
{
	// Validate command before attempting launch
	if (command.isEmpty()) {
		qWarning()
			<< "CwlProcessManager::launchComponent: Empty command provided";
		return false;
	}

	QStringList args;
	args.append("-c");
	args.append(command);

	// Validate args list
	if (args.size() != 2) {
		qWarning()
			<< "CwlProcessManager::launchComponent: Failed to construct argument list";
		return false;
	}

	try {
		if (!QProcess::startDetached("bash", args)) {
			QString componentName = extractComponentName(command);
			qWarning() << "Failed to launch component:"
				   << componentName;
			notifyComponentFailure(componentName,
					       "Process launch failed");
			return false;
		}
	} catch (const std::exception &e) {
		QString componentName = extractComponentName(command);
		qCritical() << "Exception launching component" << componentName
			    << ":" << e.what();
		notifyComponentFailure(componentName,
				       QString("Exception: %1").arg(e.what()));
		return false;
	} catch (...) {
		QString componentName = extractComponentName(command);
		qCritical() << "Unknown exception launching component"
			    << componentName;
		notifyComponentFailure(componentName,
				       "Unknown exception during launch");
		return false;
	}

	qDebug() << "Successfully launched component:" << command;
	return true;
}

bool CwlProcessManager::launchComponentWithRetry(const QString &command,
						 int maxRetries)
{
	// Validate input parameters
	if (command.isEmpty()) {
		qWarning()
			<< "CwlProcessManager::launchComponentWithRetry: Empty command provided";
		return false;
	}

	if (maxRetries < 1) {
		qWarning()
			<< "CwlProcessManager::launchComponentWithRetry: Invalid maxRetries value:"
			<< maxRetries;
		return false;
	}

	QString componentName = extractComponentName(command);
	if (componentName.isEmpty()) {
		qWarning()
			<< "CwlProcessManager::launchComponentWithRetry: Could not extract component name from command:"
			<< command;
		return false;
	}

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

			// Use a simple delay for retry with error handling
			try {
				QEventLoop loop;
				QTimer::singleShot(1000, &loop,
						   &QEventLoop::quit);
				loop.exec();
			} catch (const std::exception &e) {
				qWarning() << "Exception during retry delay for"
					   << componentName << ":" << e.what();
				// Continue with next attempt despite delay failure
			} catch (...) {
				qWarning()
					<< "Unknown exception during retry delay for"
					<< componentName;
				// Continue with next attempt despite delay failure
			}
		}
	}

	qCritical() << "Failed to launch component" << componentName << "after"
		    << maxRetries << "attempts";

	// Track failed component with validation
	if (!m_failedComponents.contains(componentName)) {
		m_failedComponents.append(componentName);
	}

	// Notify about component failure
	bool critical = isComponentCritical(componentName);
	QString errorMsg = QString("Failed after %1 attempts").arg(maxRetries);
	notifyComponentFailure(componentName, errorMsg);
	emit componentLaunchFailed(componentName, errorMsg, critical);

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
	// Validate command before processing
	if (command.isEmpty()) {
		qWarning()
			<< "CwlProcessManager::extractComponentName: Empty command provided";
		return QString();
	}

	QStringList parts = command.split(" ");
	if (parts.isEmpty()) {
		qWarning()
			<< "CwlProcessManager::extractComponentName: Failed to split command:"
			<< command;
		return QString();
	}

	QString componentName = parts.first();
	if (componentName.isEmpty()) {
		qWarning()
			<< "CwlProcessManager::extractComponentName: Component name is empty for command:"
			<< command;
		return QString();
	}

	return componentName;
}