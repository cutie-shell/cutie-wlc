// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "input-device-manager.h"
#include "input-events.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QTest>

/**
 * @brief Integration tests for CwlInputDeviceManager
 * 
 * Tests that the InputDeviceManager correctly processes Qt input events
 * and emits the appropriate signals with correct parameters.
 */
class TestInputDeviceManager : public QObject {
	Q_OBJECT

    private slots:
	void initTestCase();
	void cleanupTestCase();
	void init();
	void cleanup();

	// Pointer event tests
	void testPointerPress();
	void testPointerRelease();
	void testPointerMove();
	void testPointerWithModifiers();

	// Keyboard event tests
	void testKeyPress();
	void testKeyRelease();
	void testKeyWithModifiers();
	void testKeyWithText();

	// Edge cases and error handling
	void testNullEvent();
	void testMultipleEvents();

    private:
	CwlInputDeviceManager *m_manager;
};

void TestInputDeviceManager::initTestCase()
{
	// Setup code that runs once before all tests
	qDebug() << "Starting InputDeviceManager integration tests";
}

void TestInputDeviceManager::cleanupTestCase()
{
	// Cleanup code that runs once after all tests
	qDebug() << "Finished InputDeviceManager integration tests";
}

void TestInputDeviceManager::init()
{
	// Setup code that runs before each test
	m_manager = new CwlInputDeviceManager();
}

void TestInputDeviceManager::cleanup()
{
	// Cleanup code that runs after each test
	delete m_manager;
	m_manager = nullptr;
}

void TestInputDeviceManager::testPointerPress()
{
	// Create signal spy to monitor pointerEvent emissions
	QSignalSpy spy(m_manager, &CwlInputDeviceManager::pointerEvent);

	// Create a mouse press event
	QMouseEvent mousePress(QEvent::MouseButtonPress, QPointF(100.0, 200.0),
			       QPointF(100.0, 200.0), QPointF(100.0, 200.0),
			       Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

	// Process the event
	m_manager->processEvent(&mousePress);

	// Verify signal was emitted exactly once
	QCOMPARE(spy.count(), 1);

	// Extract the emitted event
	QList<QVariant> arguments = spy.takeFirst();
	QCOMPARE(arguments.size(), 1);

	PointerEvent event = arguments.at(0).value<PointerEvent>();

	// Verify event parameters
	QCOMPARE(event.type, PointerEventType::Press);
	QCOMPARE(event.button, Qt::LeftButton);
	QCOMPARE(event.buttons, Qt::LeftButton);
	QCOMPARE(event.globalPosition, QPointF(100.0, 200.0));
	QCOMPARE(event.modifiers, Qt::NoModifier);
}

void TestInputDeviceManager::testPointerRelease()
{
	QSignalSpy spy(m_manager, &CwlInputDeviceManager::pointerEvent);

	// Create a mouse release event
	QMouseEvent mouseRelease(QEvent::MouseButtonRelease,
				 QPointF(150.0, 250.0), QPointF(150.0, 250.0),
				 QPointF(150.0, 250.0), Qt::RightButton,
				 Qt::NoButton, Qt::NoModifier);

	m_manager->processEvent(&mouseRelease);

	QCOMPARE(spy.count(), 1);

	PointerEvent event = spy.takeFirst().at(0).value<PointerEvent>();

	QCOMPARE(event.type, PointerEventType::Release);
	QCOMPARE(event.button, Qt::RightButton);
	QCOMPARE(event.buttons, Qt::NoButton);
	QCOMPARE(event.globalPosition, QPointF(150.0, 250.0));
}

void TestInputDeviceManager::testPointerMove()
{
	QSignalSpy spy(m_manager, &CwlInputDeviceManager::pointerEvent);

	// Create a mouse move event
	QMouseEvent mouseMove(QEvent::MouseMove, QPointF(300.0, 400.0),
			      QPointF(300.0, 400.0), QPointF(300.0, 400.0),
			      Qt::NoButton, Qt::LeftButton, Qt::NoModifier);

	m_manager->processEvent(&mouseMove);

	QCOMPARE(spy.count(), 1);

	PointerEvent event = spy.takeFirst().at(0).value<PointerEvent>();

	QCOMPARE(event.type, PointerEventType::Move);
	QCOMPARE(event.button, Qt::NoButton);
	QCOMPARE(event.buttons, Qt::LeftButton);
	QCOMPARE(event.globalPosition, QPointF(300.0, 400.0));
}

void TestInputDeviceManager::testPointerWithModifiers()
{
	QSignalSpy spy(m_manager, &CwlInputDeviceManager::pointerEvent);

	// Create a mouse event with Ctrl+Shift modifiers
	QMouseEvent mousePress(QEvent::MouseButtonPress, QPointF(50.0, 75.0),
			       QPointF(50.0, 75.0), QPointF(50.0, 75.0),
			       Qt::MiddleButton, Qt::MiddleButton,
			       Qt::ControlModifier | Qt::ShiftModifier);

	m_manager->processEvent(&mousePress);

	QCOMPARE(spy.count(), 1);

	PointerEvent event = spy.takeFirst().at(0).value<PointerEvent>();

	QCOMPARE(event.type, PointerEventType::Press);
	QCOMPARE(event.modifiers, Qt::ControlModifier | Qt::ShiftModifier);
}

void TestInputDeviceManager::testKeyPress()
{
	// Create signal spy to monitor keyboardEvent emissions
	QSignalSpy spy(m_manager, &CwlInputDeviceManager::keyboardEvent);

	// Create a key press event for 'A'
	QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "a");

	m_manager->processEvent(&keyPress);

	QCOMPARE(spy.count(), 1);

	KeyboardEvent event = spy.takeFirst().at(0).value<KeyboardEvent>();

	QCOMPARE(event.type, KeyEventType::Press);
	QCOMPARE(event.key, Qt::Key_A);
	QCOMPARE(event.text, QString("a"));
	QCOMPARE(event.modifiers, Qt::NoModifier);
}

void TestInputDeviceManager::testKeyRelease()
{
	QSignalSpy spy(m_manager, &CwlInputDeviceManager::keyboardEvent);

	// Create a key release event for 'B'
	QKeyEvent keyRelease(QEvent::KeyRelease, Qt::Key_B, Qt::NoModifier,
			     "b");

	m_manager->processEvent(&keyRelease);

	QCOMPARE(spy.count(), 1);

	KeyboardEvent event = spy.takeFirst().at(0).value<KeyboardEvent>();

	QCOMPARE(event.type, KeyEventType::Release);
	QCOMPARE(event.key, Qt::Key_B);
	QCOMPARE(event.text, QString("b"));
}

void TestInputDeviceManager::testKeyWithModifiers()
{
	QSignalSpy spy(m_manager, &CwlInputDeviceManager::keyboardEvent);

	// Create a key press with Ctrl modifier
	QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier,
			   "");

	m_manager->processEvent(&keyPress);

	QCOMPARE(spy.count(), 1);

	KeyboardEvent event = spy.takeFirst().at(0).value<KeyboardEvent>();

	QCOMPARE(event.type, KeyEventType::Press);
	QCOMPARE(event.key, Qt::Key_C);
	QCOMPARE(event.modifiers, Qt::ControlModifier);
}

void TestInputDeviceManager::testKeyWithText()
{
	QSignalSpy spy(m_manager, &CwlInputDeviceManager::keyboardEvent);

	// Create a key press for a special character
	QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier,
			   " ");

	m_manager->processEvent(&keyPress);

	QCOMPARE(spy.count(), 1);

	KeyboardEvent event = spy.takeFirst().at(0).value<KeyboardEvent>();

	QCOMPARE(event.type, KeyEventType::Press);
	QCOMPARE(event.key, Qt::Key_Space);
	QCOMPARE(event.text, QString(" "));
}

void TestInputDeviceManager::testNullEvent()
{
	// Test that null events are handled gracefully
	QSignalSpy pointerSpy(m_manager, &CwlInputDeviceManager::pointerEvent);
	QSignalSpy keyboardSpy(m_manager,
			       &CwlInputDeviceManager::keyboardEvent);

	// Process null event
	m_manager->processEvent(nullptr);

	// Verify no signals were emitted
	QCOMPARE(pointerSpy.count(), 0);
	QCOMPARE(keyboardSpy.count(), 0);
}

void TestInputDeviceManager::testMultipleEvents()
{
	QSignalSpy pointerSpy(m_manager, &CwlInputDeviceManager::pointerEvent);
	QSignalSpy keyboardSpy(m_manager,
			       &CwlInputDeviceManager::keyboardEvent);

	// Create and process multiple events
	QMouseEvent mouseEvent(QEvent::MouseButtonPress, QPointF(10.0, 20.0),
			       QPointF(10.0, 20.0), QPointF(10.0, 20.0),
			       Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "a");

	m_manager->processEvent(&mouseEvent);
	m_manager->processEvent(&keyEvent);

	// Verify correct number of signals
	QCOMPARE(pointerSpy.count(), 1);
	QCOMPARE(keyboardSpy.count(), 1);

	// Verify event types
	PointerEvent ptrEvent =
		pointerSpy.takeFirst().at(0).value<PointerEvent>();
	KeyboardEvent kbdEvent =
		keyboardSpy.takeFirst().at(0).value<KeyboardEvent>();

	QCOMPARE(ptrEvent.type, PointerEventType::Press);
	QCOMPARE(kbdEvent.type, KeyEventType::Press);
}

// Register metatypes for signal/slot connections
Q_DECLARE_METATYPE(PointerEvent)
Q_DECLARE_METATYPE(KeyboardEvent)

// Qt Test main function
QTEST_MAIN(TestInputDeviceManager)
#include "test-input-device-manager.moc"
