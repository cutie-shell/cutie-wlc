// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2024 Cutie Shell Contributors

#include "surface-compositor.h"
#include <QDebug>

/**
 * @brief Construct a new CwlSurfaceCompositor
 * 
 * Initializes the surface compositor with empty layer lists.
 * Phase 1: Stub implementation with debug logging only.
 */
CwlSurfaceCompositor::CwlSurfaceCompositor(QObject *parent)
	: QObject(parent)
{
	qDebug() << "CwlSurfaceCompositor: Constructed";

	// Phase 1: No initialization needed yet
	// In Phase 2, we might initialize connections or state
}

/**
 * @brief Destroy the CwlSurfaceCompositor
 * 
 * Cleans up the compositor. Surface lists are automatically cleaned up
 * by Qt's container destructors.
 */
CwlSurfaceCompositor::~CwlSurfaceCompositor()
{
	qDebug() << "CwlSurfaceCompositor: Destroyed";

	// Phase 1: No cleanup needed yet
	// In Phase 2, we might need to disconnect signals or clear state
}

/**
 * @brief Add a surface to the compositor (stub implementation)
 * 
 * Phase 1: This is a stub that compiles but doesn't perform actual surface
 * management. In Phase 2, this will:
 * 1. Validate the surface pointer
 * 2. Determine which layer the surface belongs to
 * 3. Add the surface to the appropriate m_surfaces[] list
 * 4. Emit the surfaceAdded() signal
 * 
 * @param surface The surface to add
 */
void CwlSurfaceCompositor::addSurface(CwlView *surface)
{
	qDebug() << "CwlSurfaceCompositor::addSurface: Stub called for surface"
		 << surface;

	// Phase 1: Stub implementation
	// TODO Phase 2: Implement actual surface management
	// if (!surface) {
	//     qWarning() << "CwlSurfaceCompositor::addSurface: Null surface provided";
	//     return;
	// }
	//
	// // Get the layer from the surface
	// SurfaceLayer layer = static_cast<SurfaceLayer>(surface->layer);
	// if (layer >= SurfaceLayer::NUM_LAYERS) {
	//     qWarning() << "CwlSurfaceCompositor::addSurface: Invalid layer" << static_cast<uint32_t>(layer);
	//     layer = SurfaceLayer::TOP; // Default to TOP layer
	// }
	//
	// // Add to the appropriate layer
	// m_surfaces[static_cast<uint32_t>(layer)].append(surface);
	//
	// // Emit signal
	// emit surfaceAdded(surface);
}

/**
 * @brief Remove a surface from the compositor (stub implementation)
 * 
 * Phase 1: This is a stub that compiles but doesn't perform actual surface
 * management. In Phase 2, this will:
 * 1. Validate the surface pointer
 * 2. Search through all layer lists to find the surface
 * 3. Remove the surface from its layer list
 * 4. Emit the surfaceRemoved() signal
 * 
 * @param surface The surface to remove
 */
void CwlSurfaceCompositor::removeSurface(CwlView *surface)
{
	qDebug()
		<< "CwlSurfaceCompositor::removeSurface: Stub called for surface"
		<< surface;

	// Phase 1: Stub implementation
	// TODO Phase 2: Implement actual surface removal
	// if (!surface) {
	//     qWarning() << "CwlSurfaceCompositor::removeSurface: Null surface provided";
	//     return;
	// }
	//
	// // Search through all layers to find and remove the surface
	// bool removed = false;
	// for (uint32_t i = 0; i < static_cast<uint32_t>(SurfaceLayer::NUM_LAYERS); ++i) {
	//     if (m_surfaces[i].removeOne(surface)) {
	//         removed = true;
	//         break;
	//     }
	// }
	//
	// if (removed) {
	//     emit surfaceRemoved(surface);
	// } else {
	//     qWarning() << "CwlSurfaceCompositor::removeSurface: Surface not found in any layer";
	// }
}

/**
 * @brief Get list of all visible surfaces in rendering order (stub implementation)
 * 
 * Phase 1: This is a stub that returns an empty list. In Phase 2, this will:
 * 1. Iterate through layers from BACKGROUND to OVERLAY
 * 2. For each layer, add all surfaces in that layer to the result list
 * 3. Filter out any hidden surfaces
 * 4. Return the complete list in rendering order
 * 
 * @return QList<CwlView*> Empty list (stub implementation)
 */
QList<CwlView *> CwlSurfaceCompositor::getVisibleSurfaces() const
{
	qDebug() << "CwlSurfaceCompositor::getVisibleSurfaces: Stub called";

	// Phase 1: Return empty list
	// TODO Phase 2: Implement actual surface list generation
	// QList<CwlView*> visibleSurfaces;
	//
	// // Iterate through layers in rendering order (BACKGROUND to OVERLAY)
	// for (uint32_t i = 0; i < static_cast<uint32_t>(SurfaceLayer::NUM_LAYERS); ++i) {
	//     for (CwlView* surface : m_surfaces[i]) {
	//         // Only include visible surfaces
	//         if (surface && !surface->isHidden()) {
	//             visibleSurfaces.append(surface);
	//         }
	//     }
	// }
	//
	// return visibleSurfaces;

	return QList<CwlView *>(); // Empty list for Phase 1
}
