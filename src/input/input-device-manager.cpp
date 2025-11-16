// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "input-device-manager.h"
#include <QDebug>
#include <QInputEvent>
#include <QKeyEvent>
#include <QMouseEvent>

CwlInputDeviceManager::CwlInputDeviceManager(QObject *parent)
	: QObject(parent)
{
	qDebug() << "CwlInputDeviceManager: Created";
}

CwlInputDeviceManager::~CwlInputDeviceManager()
{
	qDebug() << "CwlInputDeviceManager: Destroyed";
}

void CwlInputDeviceManager::processEvent(QInputEvent *event)
{
	if (!event) {
		qWarning() << "CwlInputDeviceManager: Received null event";
		return;
	}

	// TODO: In Phase 2, this will:
	// - Determine the event type (mouse, keyboard, etc.)
	// - Call the appropriate handler method
	// - Convert to compositor event format
	// - Emit the corresponding signal

	switch (event->type()) {
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove:
		handlePointerEvent(event);
		break;

	case QEvent::KeyPress:
	case QEvent::KeyRelease:
		handleKeyEvent(event);
		break;

	default:
		// Ignore other event types for now
		break;
	}
}

void CwlInputDeviceManager::handlePointerEvent(QInputEvent *event)
{
	// Stub implementation - no actual conversion yet
	// TODO: In Phase 2, this will:
	// - Cast to QMouseEvent
	// - Extract position, button, modifiers
	// - Create PointerEvent struct
	// - Emit pointerEvent signal

	qDebug() << "CwlInputDeviceManager: Processing pointer event (stub)";

	// Example of what will be implemented:
	// QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
	// PointerEvent ptrEvent;
	// ptrEvent.globalPosition = mouseEvent->globalPosition();
	// ptrEvent.button = mouseEvent->button();
	// ptrEvent.buttons = mouseEvent->buttons();
	// ptrEvent.modifiers = mouseEvent->modifiers();
	// ptrEvent.type = /* determine from event->type() */;
	// emit pointerEvent(ptrEvent);
}

void CwlInputDeviceManager::handleKeyEvent(QInputEvent *event)
{
	// Stub implementation - no actual conversion yet
	// TODO: In Phase 2, this will:
	// - Cast to QKeyEvent
	// - Extract key, scan code, modifiers, text
	// - Create KeyboardEvent struct
	// - Emit keyboardEvent signal

	qDebug() << "CwlInputDeviceManager: Processing keyboard event (stub)";

	// Example of what will be implemented:
	// QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
	// KeyboardEvent kbdEvent;
	// kbdEvent.nativeScanCode = keyEvent->nativeScanCode();
	// kbdEvent.key = static_cast<Qt::Key>(keyEvent->key());
	// kbdEvent.modifiers = keyEvent->modifiers();
	// kbdEvent.text = keyEvent->text();
	// kbdEvent.type = /* determine from event->type() */;
	// emit keyboardEvent(kbdEvent);
}
