// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INPUT_DEVICE_MANAGER_H
#define INPUT_DEVICE_MANAGER_H

#include "input-events.h"
#include <QObject>

class QInputEvent;

/**
 * @brief Manages input device events and converts them to compositor-friendly format
 * 
 * The CwlInputDeviceManager receives raw Qt input events and converts them into
 * the compositor's internal event format (PointerEvent and KeyboardEvent).
 * It acts as the bridge between Qt's input system and the compositor's input routing.
 * 
 * This class does not handle event routing or focus management - those are the
 * responsibility of CwlInputRouter. It only handles event translation.
 */
class CwlInputDeviceManager : public QObject {
	Q_OBJECT

    public:
	/**
     * @brief Construct a new input device manager
     * @param parent Parent QObject
     */
	explicit CwlInputDeviceManager(QObject *parent = nullptr);

	/**
     * @brief Destructor
     */
	~CwlInputDeviceManager() override;

	/**
     * @brief Process a raw Qt input event
     * 
     * Takes a QInputEvent (mouse, keyboard, etc.) and converts it to the
     * appropriate compositor event type, emitting the corresponding signal.
     * 
     * @param event The Qt input event to process
     */
	void processEvent(QInputEvent *event);

    signals:
	/**
     * @brief Emitted when a pointer (mouse) event occurs
     * @param event The converted pointer event
     */
	void pointerEvent(const PointerEvent &event);

	/**
     * @brief Emitted when a keyboard event occurs
     * @param event The converted keyboard event
     */
	void keyboardEvent(const KeyboardEvent &event);

    private:
	/**
     * @brief Handle a pointer event
     * @param event The Qt mouse event
     */
	void handlePointerEvent(QInputEvent *event);

	/**
     * @brief Handle a keyboard event
     * @param event The Qt keyboard event
     */
	void handleKeyEvent(QInputEvent *event);
};

#endif // INPUT_DEVICE_MANAGER_H
