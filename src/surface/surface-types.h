// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2024 Cutie Shell Contributors

#ifndef SURFACE_TYPES_H
#define SURFACE_TYPES_H

#include <cstdint>

/**
 * @brief Surface layer enumeration for organizing surfaces in z-order
 * 
 * Defines the vertical stacking order of surfaces in the compositor.
 * Lower values appear below higher values.
 */
enum class SurfaceLayer : uint32_t {
	BACKGROUND = 0, ///< Background layer (wallpaper, etc.)
	BOTTOM, ///< Bottom layer (below normal windows)
	TOP, ///< Top layer (normal application windows)
	OVERLAY, ///< Overlay layer (panels, notifications, etc.)
	NUM_LAYERS ///< Total number of layers (not a valid layer)
};

/**
 * @brief Surface type enumeration for identifying surface roles
 * 
 * Identifies the protocol role and behavior of a surface.
 */
enum class SurfaceType {
	None, ///< No type assigned yet
	XdgToplevel, ///< XDG toplevel window (normal application window)
	XdgPopup, ///< XDG popup window (menus, tooltips, etc.)
	LayerShell ///< Layer shell surface (panels, backgrounds, etc.)
};

#endif // SURFACE_TYPES_H
