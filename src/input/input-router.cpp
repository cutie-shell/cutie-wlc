// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2024 Cutie Shell Contributors

#include "input-router.h"
#include "input-events.h"
#include "../surface/surface-compositor.h"
#include "../view.h"
#include <QDebug>

/**
 * @brief Construct a new CwlInputRouter
 * 
 * Initializes the input router with a reference to the surface compositor.
 * Phase 1: Stub implementation with debug logging only.
 * 
 * @param compositor The surface compositor for querying visible surfaces
 * @param parent The parent QObject for memory management
 */
CwlInputRouter::CwlInputRouter(CwlSurfaceCompositor *compositor,
			       QObject *parent)
	: QObject(parent)
	, m_surfaceCompositor(compositor)
	, m_keyboardFocus(nullptr)
{
	qDebug() << "CwlInputRouter: Constructed with compositor" << compositor;

	// Phase 1: No initialization needed yet
	// In Phase 2, we might initialize hit-testing logic or connect signals
}

/**
 * @brief Destroy the CwlInputRouter
 * 
 * Cleans up the input router. QPointer automatically handles cleanup.
 */
CwlInputRouter::~CwlInputRouter()
{
	qDebug() << "CwlInputRouter: Destroyed";

	// Phase 1: No cleanup needed yet
	// QPointer<CwlView> automatically handles null on destruction
}

/**
 * @brief Route a pointer event to the appropriate surface (stub implementation)
 * 
 * Phase 1: This is a stub that compiles but doesn't perform actual event routing.
 * In Phase 2, this will:
 * 1. Validate the event
 * 2. Query the surface compositor for visible surfaces
 * 3. Perform hit-testing to find the surface under the pointer
 * 4. Route the event to the target surface
 * 5. Handle pointer grab state if applicable
 * 
 * @param event The pointer event to route
 */
void CwlInputRouter::routePointerEvent(const PointerEvent &event)
{
	qDebug()
		<< "CwlInputRouter::routePointerEvent: Stub called for event at"
		<< event.globalPosition;

	// Phase 1: Stub implementation
	// TODO Phase 2: Implement actual pointer event routing
	// if (!m_surfaceCompositor) {
	//     qWarning() << "CwlInputRouter::routePointerEvent: No compositor available";
	//     return;
	// }
	//
	// // Get visible surfaces from compositor
	// QList<CwlView*> visibleSurfaces = m_surfaceCompositor->getVisibleSurfaces();
	//
	// // Perform hit-testing (iterate in reverse order, top surface first)
	// CwlView* targetSurface = nullptr;
	// for (auto it = visibleSurfaces.rbegin(); it != visibleSurfaces.rend(); ++it) {
	//     CwlView* surface = *it;
	//     if (surface->geometry().contains(event.globalPosition.toPoint())) {
	//         targetSurface = surface;
	//         break;
	//     }
	// }
	//
	// if (targetSurface) {
	//     // Convert global coordinates to surface-local coordinates
	//     QPointF localPos = event.globalPosition - targetSurface->position();
	//
	//     // Send event to surface
	//     // (Implementation depends on Wayland surface event delivery)
	//     qDebug() << "CwlInputRouter: Routing pointer event to surface" << targetSurface;
	// }
}

/**
 * @brief Route a keyboard event to the focused surface (stub implementation)
 * 
 * Phase 1: This is a stub that compiles but doesn't perform actual event routing.
 * In Phase 2, this will:
 * 1. Validate the event
 * 2. Check if there is a keyboard focus surface
 * 3. Route the event to the focused surface
 * 4. Handle keyboard grab state if applicable
 * 
 * @param event The keyboard event to route
 */
void CwlInputRouter::routeKeyboardEvent(const KeyboardEvent &event)
{
	qDebug() << "CwlInputRouter::routeKeyboardEvent: Stub called for key"
		 << event.key;

	// Phase 1: Stub implementation
	// TODO Phase 2: Implement actual keyboard event routing
	// if (!m_keyboardFocus) {
	//     qDebug() << "CwlInputRouter::routeKeyboardEvent: No keyboard focus surface";
	//     return;
	// }
	//
	// // Send event to focused surface
	// // (Implementation depends on Wayland surface event delivery)
	// qDebug() << "CwlInputRouter: Routing keyboard event to focused surface"
	//          << m_keyboardFocus.data();
}

/**
 * @brief Set the keyboard focus to a specific surface (stub implementation)
 * 
 * Phase 1: This is a stub that stores the focus and emits the signal.
 * In Phase 2, this will:
 * 1. Validate the surface pointer
 * 2. Send leave event to the previously focused surface
 * 3. Send enter event to the newly focused surface
 * 4. Update internal focus state
 * 5. Emit focusChanged signal
 * 
 * @param surface The surface to receive keyboard focus (can be nullptr to clear focus)
 */
void CwlInputRouter::setKeyboardFocus(CwlView *surface)
{
	qDebug() << "CwlInputRouter::setKeyboardFocus: Setting focus to surface"
		 << surface;

	// Phase 1: Basic focus tracking with signal emission
	CwlView *oldFocus = m_keyboardFocus.data();

	if (oldFocus != surface) {
		m_keyboardFocus = surface;

		// Emit focus change signal
		emit focusChanged(oldFocus, surface);

		qDebug() << "CwlInputRouter: Focus changed from" << oldFocus
			 << "to" << surface;
	}

	// TODO Phase 2: Implement actual focus management
	// if (oldFocus) {
	//     // Send keyboard leave event to old focus
	//     oldFocus->keyboardLeave();
	// }
	//
	// if (surface) {
	//     // Send keyboard enter event to new focus
	//     surface->keyboardEnter();
	// }
}

/**
 * @brief Get the currently focused surface
 * 
 * Phase 1: Returns the stored focus pointer.
 * 
 * @return CwlView* The currently focused surface, or nullptr if no surface has focus
 */
CwlView *CwlInputRouter::keyboardFocus() const
{
	return m_keyboardFocus.data();
}
