#pragma once

#include <QObject>
#include <QStringList>

class QTimer;

/**
 * @brief Manages component lifecycle, process spawning, and dependency management
 * 
 * This class encapsulates all process management functionality that was previously
 * scattered throughout CwlCompositor, providing a cleaner separation of concerns
 * and better error handling capabilities.
 */
class CwlProcessManager : public QObject {
	Q_OBJECT

    public:
	explicit CwlProcessManager(QObject *parent = nullptr);

	/**
	 * @brief Launch a cutie component without retry
	 * @param command Command to execute
	 * @return true if launched successfully, false otherwise
	 */
	bool launchComponent(const QString &command);

	/**
	 * @brief Launch a cutie component with retry mechanism
	 * @param command Command to execute
	 * @param maxRetries Maximum number of retry attempts
	 * @return true if launched successfully, false otherwise
	 */
	bool launchComponentWithRetry(const QString &command, int maxRetries);

	/**
	 * @brief Launch all essential cutie components
	 * @param launcher Launcher command to use
	 */
	void launchEssentialComponents(const QString &launcher);

	/**
	 * @brief Check if a component is considered critical for system operation
	 * @param component Component name to check
	 * @return true if critical, false otherwise
	 */
	bool isComponentCritical(const QString &component) const;

	/**
	 * @brief Get the list of failed components
	 * @return List of component names that failed to launch
	 */
	const QStringList &getFailedComponents() const
	{
		return m_failedComponents;
	}

	/**
	 * @brief Get the list of critical components
	 * @return List of component names that are considered critical
	 */
	const QStringList &getCriticalComponents() const
	{
		return m_criticalComponents;
	}

	/**
	 * @brief Add a component to the critical components list
	 * @param component Component name to add
	 */
	void addCriticalComponent(const QString &component);

	/**
	 * @brief Remove a component from the critical components list
	 * @param component Component name to remove
	 */
	void removeCriticalComponent(const QString &component);

    signals:
	/**
	 * @brief Emitted when a component fails to launch
	 * @param component Component name that failed
	 * @param error Error description
	 * @param isCritical Whether the component is critical
	 */
	void componentLaunchFailed(const QString &component,
				   const QString &error, bool isCritical);

	/**
	 * @brief Emitted when a component launches successfully
	 * @param component Component name that launched
	 * @param attempts Number of attempts it took
	 */
	void componentLaunched(const QString &component, int attempts);

    private slots:
	/**
	 * @brief Handle retry timeout
	 */
	void onRetryTimeout();

    private:
	/**
	 * @brief Notify about component failure with appropriate logging level
	 * @param component Component name that failed
	 * @param error Error description
	 */
	void notifyComponentFailure(const QString &component,
				    const QString &error);

	/**
	 * @brief Extract component name from command string
	 * @param command Full command string
	 * @return Component name (first word of command)
	 */
	QString extractComponentName(const QString &command) const;

    private:
	// Component tracking
	QStringList m_failedComponents;
	QStringList m_criticalComponents;

	// Retry management
	QTimer *m_retryTimer;
	QString m_currentRetryCommand;
	int m_currentRetryAttempt;
	int m_maxRetryAttempts;
};