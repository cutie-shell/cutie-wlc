// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mock-rendering-api.h"

MockRenderingAPI::MockRenderingAPI(QObject *parent)
	: IRenderingAPI(parent)
{
}

bool MockRenderingAPI::initialize(void *platformContext)
{
	MethodCall call;
	call.methodName = "initialize";
	call.context = platformContext;
	m_callHistory.append(call);

	m_lastInitializeContext = platformContext;
	m_initialized = m_initializeResult;
	return m_initializeResult;
}

void MockRenderingAPI::shutdown()
{
	MethodCall call;
	call.methodName = "shutdown";
	m_callHistory.append(call);

	m_shutdown = true;
	m_initialized = false;
}

void MockRenderingAPI::beginRendering()
{
	MethodCall call;
	call.methodName = "beginRendering";
	m_callHistory.append(call);
}

void MockRenderingAPI::endRendering()
{
	MethodCall call;
	call.methodName = "endRendering";
	m_callHistory.append(call);
}

void MockRenderingAPI::clearScreen(const QColor &color)
{
	MethodCall call;
	call.methodName = "clearScreen";
	call.color = color;
	m_callHistory.append(call);

	m_lastClearColor = color;
}

void MockRenderingAPI::setInitializeResult(bool success)
{
	m_initializeResult = success;
}

void MockRenderingAPI::triggerRenderingError(const QString &error)
{
	emit renderingError(error);
}

void MockRenderingAPI::triggerContextLost()
{
	emit contextLost();
}

const QList<MockRenderingAPI::MethodCall> &
MockRenderingAPI::getCallHistory() const
{
	return m_callHistory;
}

void MockRenderingAPI::clearCallHistory()
{
	m_callHistory.clear();
}

bool MockRenderingAPI::wasMethodCalled(const QString &methodName) const
{
	for (const auto &call : m_callHistory) {
		if (call.methodName == methodName) {
			return true;
		}
	}
	return false;
}

int MockRenderingAPI::getCallCount(const QString &methodName) const
{
	int count = 0;
	for (const auto &call : m_callHistory) {
		if (call.methodName == methodName) {
			count++;
		}
	}
	return count;
}

void *MockRenderingAPI::getLastInitializeContext() const
{
	return m_lastInitializeContext;
}

QColor MockRenderingAPI::getLastClearColor() const
{
	return m_lastClearColor;
}

bool MockRenderingAPI::wasInitialized() const
{
	return m_initialized;
}

bool MockRenderingAPI::wasShutdown() const
{
	return m_shutdown;
}
