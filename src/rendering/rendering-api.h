// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RENDERING_API_H
#define RENDERING_API_H

#include <QColor>
#include <QObject>

/**
 * @brief Abstract rendering API interface
 * 
 * Defines the contract for rendering APIs (OpenGL ES, Vulkan, etc.).
 * This interface is platform-agnostic and works with any platform backend.
 * 
 * The rendering API is responsible for:
 * - Initializing the rendering context
 * - Managing the rendering pipeline
 * - Drawing surfaces to the screen
 * - Managing textures and buffers
 * 
 * Example usage:
 * @code
 * IRenderingAPI* api = new OpenGLRenderingAPI();
 * if (api->initialize(platformContext)) {
 *     api->beginRendering();
 *     api->clearScreen(Qt::black);
 *     // ... render surfaces ...
 *     api->endRendering();
 * }
 * @endcode
 */
class IRenderingAPI : public QObject {
	Q_OBJECT
    public:
	explicit IRenderingAPI(QObject *parent = nullptr)
		: QObject(parent)
	{
	}
	virtual ~IRenderingAPI() = default;

	/**
     * @brief Initialize the rendering API
     * @param platformContext Opaque platform-specific context (e.g., EGLDisplay)
     * @return true if initialization succeeded, false otherwise
     */
	virtual bool initialize(void *platformContext) = 0;

	/**
     * @brief Shutdown the rendering API and release resources
     */
	virtual void shutdown() = 0;

	/**
     * @brief Begin a new rendering frame
     * 
     * Must be called before any rendering operations.
     * Paired with endRendering().
     */
	virtual void beginRendering() = 0;

	/**
     * @brief End the current rendering frame
     * 
     * Must be called after all rendering operations are complete.
     * Paired with beginRendering().
     */
	virtual void endRendering() = 0;

	/**
     * @brief Clear the screen with the specified color
     * @param color The color to clear the screen with
     */
	virtual void clearScreen(const QColor &color) = 0;

    signals:
	/**
     * @brief Emitted when a rendering error occurs
     * @param error Human-readable error message
     */
	void renderingError(const QString &error);

	/**
     * @brief Emitted when the rendering context is lost
     * 
     * The compositor should reinitialize the rendering API when this occurs.
     */
	void contextLost();
};

#endif // RENDERING_API_H
