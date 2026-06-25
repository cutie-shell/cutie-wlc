// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MOCK_RENDERING_API_H
#define MOCK_RENDERING_API_H

#include "../../src/rendering/rendering-api.h"
#include <QColor>
#include <QList>
#include <QString>

/**
 * @brief Mock implementation of IRenderingAPI for testing
 * 
 * This mock tracks all method calls and allows configuring return values
 * for testing purposes. It records the call history so tests can verify
 * that the correct methods were called with the expected parameters.
 */
class MockRenderingAPI : public IRenderingAPI {
	Q_OBJECT

    public:
	/**
     * @brief Represents a method call on the mock API
     */
	struct MethodCall {
		QString methodName;
		void *context = nullptr;
		QColor color;
	};

	explicit MockRenderingAPI(QObject *parent = nullptr);
	~MockRenderingAPI() override = default;

	// IRenderingAPI implementation
	bool initialize(void *platformContext) override;
	void shutdown() override;
	void beginRendering() override;
	void endRendering() override;
	void clearScreen(const QColor &color) override;

	// Mock configuration methods
	/**
     * @brief Configure the return value for initialize()
     * @param success Whether initialize() should return true or false
     */
	void setInitializeResult(bool success);

	/**
     * @brief Trigger the renderingError signal for testing
     * @param error The error message to emit
     */
	void triggerRenderingError(const QString &error);

	/**
     * @brief Trigger the contextLost signal for testing
     */
	void triggerContextLost();

	// Call history inspection methods
	/**
     * @brief Get the list of all recorded method calls
     * @return List of method calls in chronological order
     */
	const QList<MethodCall> &getCallHistory() const;

	/**
     * @brief Clear the call history
     */
	void clearCallHistory();

	/**
     * @brief Check if a specific method was called
     * @param methodName Name of the method to check
     * @return true if the method was called at least once
     */
	bool wasMethodCalled(const QString &methodName) const;

	/**
     * @brief Get the number of times a method was called
     * @param methodName Name of the method to count
     * @return Number of times the method was called
     */
	int getCallCount(const QString &methodName) const;

	/**
     * @brief Get the platform context passed to initialize()
     * @return The context pointer, or nullptr if initialize was not called
     */
	void *getLastInitializeContext() const;

	/**
     * @brief Get the last color passed to clearScreen()
     * @return The color, or an invalid QColor if clearScreen was not called
     */
	QColor getLastClearColor() const;

	/**
     * @brief Check if initialize was called
     * @return true if initialize was called
     */
	bool wasInitialized() const;

	/**
     * @brief Check if shutdown was called
     * @return true if shutdown was called
     */
	bool wasShutdown() const;

    private:
	QList<MethodCall> m_callHistory;
	bool m_initializeResult{ true };
	bool m_initialized{ false };
	bool m_shutdown{ false };
	void *m_lastInitializeContext{ nullptr };
	QColor m_lastClearColor;
};

#endif // MOCK_RENDERING_API_H
