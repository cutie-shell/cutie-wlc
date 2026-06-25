// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mock-surface-compositor.h"

MockSurfaceCompositor::MockSurfaceCompositor(QObject *parent)
	: CwlSurfaceCompositor(parent)
{
}

void MockSurfaceCompositor::addSurface(CwlView *surface)
{
	// Record the operation
	Operation op;
	op.operationType = "add";
	op.surface = surface;
	m_operationHistory.append(op);

	// Add to simplified surface list if not already present
	if (surface && !m_surfaces.contains(surface)) {
		m_surfaces.append(surface);
	}

	// Emit the signal
	emit surfaceAdded(surface);
}

void MockSurfaceCompositor::removeSurface(CwlView *surface)
{
	// Record the operation
	Operation op;
	op.operationType = "remove";
	op.surface = surface;
	m_operationHistory.append(op);

	// Remove from simplified surface list
	m_surfaces.removeAll(surface);

	// Emit the signal
	emit surfaceRemoved(surface);
}

QList<CwlView *> MockSurfaceCompositor::getVisibleSurfaces() const
{
	// Return simplified list (no layer ordering in mock)
	return m_surfaces;
}

const QList<MockSurfaceCompositor::Operation> &
MockSurfaceCompositor::getOperationHistory() const
{
	return m_operationHistory;
}

void MockSurfaceCompositor::clearOperationHistory()
{
	m_operationHistory.clear();
}

bool MockSurfaceCompositor::wasSurfaceAdded(CwlView *surface) const
{
	for (const auto &op : m_operationHistory) {
		if (op.operationType == "add" && op.surface == surface) {
			return true;
		}
	}
	return false;
}

bool MockSurfaceCompositor::wasSurfaceRemoved(CwlView *surface) const
{
	for (const auto &op : m_operationHistory) {
		if (op.operationType == "remove" && op.surface == surface) {
			return true;
		}
	}
	return false;
}

int MockSurfaceCompositor::getAddSurfaceCallCount() const
{
	int count = 0;
	for (const auto &op : m_operationHistory) {
		if (op.operationType == "add") {
			count++;
		}
	}
	return count;
}

int MockSurfaceCompositor::getRemoveSurfaceCallCount() const
{
	int count = 0;
	for (const auto &op : m_operationHistory) {
		if (op.operationType == "remove") {
			count++;
		}
	}
	return count;
}

bool MockSurfaceCompositor::hasSurface(CwlView *surface) const
{
	return m_surfaces.contains(surface);
}

int MockSurfaceCompositor::getSurfaceCount() const
{
	return m_surfaces.count();
}

void MockSurfaceCompositor::triggerSurfaceAdded(CwlView *surface)
{
	emit surfaceAdded(surface);
}

void MockSurfaceCompositor::triggerSurfaceRemoved(CwlView *surface)
{
	emit surfaceRemoved(surface);
}
