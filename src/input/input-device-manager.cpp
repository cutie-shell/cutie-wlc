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
	QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
	if (!mouseEvent) {
		qWarning()
			<< "CwlInputDeviceManager: Failed to cast to QMouseEvent";
		return;
	}

	PointerEvent ptrEvent;
	ptrEvent.globalPosition = mouseEvent->globalPosition();
	ptrEvent.button = mouseEvent->button();
	ptrEvent.buttons = mouseEvent->buttons();
	ptrEvent.modifiers = mouseEvent->modifiers();

	// Determine event type
	switch (event->type()) {
	case QEvent::MouseButtonPress:
		ptrEvent.type = PointerEventType::Press;
		break;
	case QEvent::MouseButtonRelease:
		ptrEvent.type = PointerEventType::Release;
		break;
	case QEvent::MouseMove:
		ptrEvent.type = PointerEventType::Move;
		break;
	default:
		qWarning()
			<< "CwlInputDeviceManager: Unknown pointer event type";
		return;
	}

	qDebug() << "CwlInputDeviceManager: Emitting pointer event, type:"
		 << static_cast<int>(ptrEvent.type);
	emit pointerEvent(ptrEvent);
}

void CwlInputDeviceManager::handleKeyEvent(QInputEvent *event)
{
	QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
	if (!keyEvent) {
		qWarning()
			<< "CwlInputDeviceManager: Failed to cast to QKeyEvent";
		return;
	}

	KeyboardEvent kbdEvent;
	kbdEvent.nativeScanCode = keyEvent->nativeScanCode();
	kbdEvent.key = static_cast<Qt::Key>(keyEvent->key());
	kbdEvent.modifiers = keyEvent->modifiers();
	kbdEvent.text = keyEvent->text();

	// Determine event type
	switch (event->type()) {
	case QEvent::KeyPress:
		kbdEvent.type = KeyEventType::Press;
		break;
	case QEvent::KeyRelease:
		kbdEvent.type = KeyEventType::Release;
		break;
	default:
		qWarning()
			<< "CwlInputDeviceManager: Unknown keyboard event type";
		return;
	}

	qDebug() << "CwlInputDeviceManager: Emitting keyboard event, key:"
		 << kbdEvent.key;
	emit keyboardEvent(kbdEvent);
}
