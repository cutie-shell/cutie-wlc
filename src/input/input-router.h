// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INPUT_ROUTER_H
#define INPUT_ROUTER_H

#include <QObject>
#include <QPointer>

#include "input-events.h"

// Forward declarations
class CwlSurfaceCompositor;
class CwlView;

/**
 * @brief Routes input events to appropriate surfaces
 * 
 * CwlInputRouter manages the routing of input events (pointer and keyboard)
 * to the appropriate surfaces based on compositor state. It maintains keyboard
 * focus state and determines which surface should receive each input event.
 * 
 * The router integrates with CwlSurfaceCompositor to query surface state and
 * layer information when determining event targets.
 * 
 * Phase 1: Interface only, no implementation logic
 */
class CwlInputRouter : public QObject {
	Q_OBJECT

    public:
	/**
	 * @brief Construct a new CwlInputRouter
	 * 
	 * @param compositor Surface compositor for querying surface state
	 * @param parent Parent QObject for memory management
	 */
	explicit CwlInputRouter(CwlSurfaceCompositor *compositor,
				QObject *parent = nullptr);

	/**
	 * @brief Destroy the CwlInputRouter
	 */
	~CwlInputRouter() override;

	/**
	 * @brief Route a pointer event to the appropriate surface
	 * 
	 * Determines which surface should receive the pointer event based on
	 * event position, surface geometry, and layer priority. May update
	 * pointer focus state.
	 * 
	 * @param event The pointer event to route
	 */
	void routePointerEvent(const PointerEvent &event);

	/**
	 * @brief Route a keyboard event to the focused surface
	 * 
	 * Sends the keyboard event to the surface with current keyboard focus.
	 * If no surface has focus, the event may be discarded or handled by
	 * the compositor.
	 * 
	 * @param event The keyboard event to route
	 */
	void routeKeyboardEvent(const KeyboardEvent &event);

	/**
	 * @brief Set keyboard focus to a specific surface
	 * 
	 * Changes keyboard focus to the specified surface, sending appropriate
	 * focus enter/leave notifications. Emits focusChanged() signal.
	 * 
	 * @param surface The surface to focus (or nullptr to clear focus)
	 */
	void setKeyboardFocus(CwlView *surface);

	/**
	 * @brief Get the surface with current keyboard focus
	 * 
	 * @return CwlView* The focused surface, or nullptr if no focus
	 */
	CwlView *keyboardFocus() const;

    signals:
	/**
	 * @brief Emitted when keyboard focus changes
	 * 
	 * @param oldFocus Previously focused surface (may be nullptr)
	 * @param newFocus Newly focused surface (may be nullptr)
	 */
	void focusChanged(CwlView *oldFocus, CwlView *newFocus);

    private:
	/**
	 * @brief Reference to surface compositor for querying surface state
	 * 
	 * Non-owning pointer - the compositor lifetime is managed elsewhere
	 */
	CwlSurfaceCompositor *m_surfaceCompositor;

	/**
	 * @brief Surface with current keyboard focus
	 * 
	 * Uses QPointer to safely handle surface destruction
	 */
	QPointer<CwlView> m_keyboardFocus;
};

#endif // INPUT_ROUTER_H
