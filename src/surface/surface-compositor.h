// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2024 Cutie Shell Contributors

#ifndef SURFACE_COMPOSITOR_H
#define SURFACE_COMPOSITOR_H

#include <QObject>
#include <QList>

#include "surface-types.h"

// Forward declaration
class CwlView;

/**
 * @brief Compositor for managing surfaces organized by layers
 * 
 * CwlSurfaceCompositor manages the lifecycle and organization of surfaces
 * (CwlView objects) within the compositor. It maintains separate lists for
 * each layer (BACKGROUND, BOTTOM, TOP, OVERLAY) and provides methods for
 * adding, removing, and querying visible surfaces.
 * 
 * This class acts as a central registry for all active surfaces and is
 * responsible for determining rendering order based on layer assignment.
 * 
 * Phase 1: Interface only, no implementation logic
 */
class CwlSurfaceCompositor : public QObject {
	Q_OBJECT

    public:
	/**
	 * @brief Construct a new CwlSurfaceCompositor
	 * @param parent Parent QObject for memory management
	 */
	explicit CwlSurfaceCompositor(QObject *parent = nullptr);

	/**
	 * @brief Destroy the CwlSurfaceCompositor
	 */
	~CwlSurfaceCompositor() override;

	/**
	 * @brief Add a surface to the compositor
	 * 
	 * Adds the surface to the appropriate layer based on its layer property.
	 * Emits surfaceAdded() signal upon successful addition.
	 * 
	 * @param surface The surface to add (must not be nullptr)
	 */
	void addSurface(CwlView *surface);

	/**
	 * @brief Remove a surface from the compositor
	 * 
	 * Removes the surface from its current layer.
	 * Emits surfaceRemoved() signal upon successful removal.
	 * 
	 * @param surface The surface to remove
	 */
	void removeSurface(CwlView *surface);

	/**
	 * @brief Get list of all visible surfaces in rendering order
	 * 
	 * Returns surfaces ordered by layer (BACKGROUND to OVERLAY) and within
	 * each layer by their insertion order.
	 * 
	 * @return QList<CwlView*> List of visible surfaces
	 */
	QList<CwlView *> getVisibleSurfaces() const;

    signals:
	/**
	 * @brief Emitted when a surface is added to the compositor
	 * @param surface The surface that was added
	 */
	void surfaceAdded(CwlView *surface);

	/**
	 * @brief Emitted when a surface is removed from the compositor
	 * @param surface The surface that was removed
	 */
	void surfaceRemoved(CwlView *surface);

    private:
	/**
	 * @brief Surface lists organized by layer
	 * 
	 * Array of lists, one for each SurfaceLayer value.
	 * Index corresponds to SurfaceLayer enum value.
	 */
	QList<CwlView *>
		m_surfaces[static_cast<uint32_t>(SurfaceLayer::NUM_LAYERS)];
};

#endif // SURFACE_COMPOSITOR_H
