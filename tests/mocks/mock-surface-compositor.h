// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MOCK_SURFACE_COMPOSITOR_H
#define MOCK_SURFACE_COMPOSITOR_H

#include "../../src/surface/surface-compositor.h"
#include "../../src/surface/surface-types.h"
#include <QList>
#include <QString>

// Forward declaration for testing
class CwlView;

/**
 * @brief Mock implementation of CwlSurfaceCompositor for testing
 * 
 * This mock tracks all surface operations and provides simplified
 * surface storage for testing purposes. It records the operation
 * history so tests can verify that surfaces were added/removed
 * correctly and signals were emitted as expected.
 */
class MockSurfaceCompositor : public CwlSurfaceCompositor {
	Q_OBJECT

    public:
	/**
     * @brief Represents an operation on the mock compositor
     */
	struct Operation {
		QString operationType; // "add" or "remove"
		CwlView *surface = nullptr;
	};

	explicit MockSurfaceCompositor(QObject *parent = nullptr);
	~MockSurfaceCompositor() override = default;

	// Override CwlSurfaceCompositor methods
	void addSurface(CwlView *surface);
	void removeSurface(CwlView *surface);
	QList<CwlView *> getVisibleSurfaces() const;

	// Mock-specific methods for testing

	/**
     * @brief Get the list of all recorded operations
     * @return List of operations in chronological order
     */
	const QList<Operation> &getOperationHistory() const;

	/**
     * @brief Clear the operation history
     */
	void clearOperationHistory();

	/**
     * @brief Check if a surface was added
     * @param surface The surface to check
     * @return true if the surface was added at least once
     */
	bool wasSurfaceAdded(CwlView *surface) const;

	/**
     * @brief Check if a surface was removed
     * @param surface The surface to check
     * @return true if the surface was removed at least once
     */
	bool wasSurfaceRemoved(CwlView *surface) const;

	/**
     * @brief Get the number of times addSurface was called
     * @return Number of add operations
     */
	int getAddSurfaceCallCount() const;

	/**
     * @brief Get the number of times removeSurface was called
     * @return Number of remove operations
     */
	int getRemoveSurfaceCallCount() const;

	/**
     * @brief Check if a specific surface is currently managed
     * @param surface The surface to check
     * @return true if the surface is in the compositor
     */
	bool hasSurface(CwlView *surface) const;

	/**
     * @brief Get the total number of surfaces currently managed
     * @return Number of surfaces
     */
	int getSurfaceCount() const;

	/**
     * @brief Manually trigger the surfaceAdded signal for testing
     * @param surface The surface to emit the signal for
     */
	void triggerSurfaceAdded(CwlView *surface);

	/**
     * @brief Manually trigger the surfaceRemoved signal for testing
     * @param surface The surface to emit the signal for
     */
	void triggerSurfaceRemoved(CwlView *surface);

    private:
	QList<Operation> m_operationHistory;
	QList<CwlView *> m_surfaces;
};

#endif // MOCK_SURFACE_COMPOSITOR_H
