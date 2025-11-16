// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QTest>
#include <QPointF>
#include "input/input-events.h"

/**
 * @brief Unit tests for input event structures
 * 
 * Tests the PointerEvent and KeyboardEvent structures to ensure
 * they correctly store and retrieve event data.
 */
class TestInputEvents : public QObject {
	Q_OBJECT

    private slots:
	/**
	 * @brief Test PointerEvent structure initialization
	 */
	void testPointerEvent_Initialization()
	{
		// Test basic initialization
		PointerEvent event;
		event.globalPosition = QPointF(100.0, 200.0);
		event.button = Qt::LeftButton;
		event.buttons = Qt::LeftButton | Qt::RightButton;
		event.modifiers = Qt::ControlModifier | Qt::ShiftModifier;
		event.type = PointerEventType::Press;

		QCOMPARE(event.globalPosition, QPointF(100.0, 200.0));
		QCOMPARE(event.button, Qt::LeftButton);
		QCOMPARE(event.buttons, Qt::LeftButton | Qt::RightButton);
		QCOMPARE(event.modifiers,
			 Qt::ControlModifier | Qt::ShiftModifier);
		QCOMPARE(event.type, PointerEventType::Press);
	}

	/**
	 * @brief Test PointerEvent with Press type
	 */
	void testPointerEvent_PressType()
	{
		PointerEvent event;
		event.globalPosition = QPointF(50.5, 75.25);
		event.button = Qt::MiddleButton;
		event.buttons = Qt::MiddleButton;
		event.modifiers = Qt::NoModifier;
		event.type = PointerEventType::Press;

		QCOMPARE(event.type, PointerEventType::Press);
		QCOMPARE(event.button, Qt::MiddleButton);
		QCOMPARE(event.globalPosition.x(), 50.5);
		QCOMPARE(event.globalPosition.y(), 75.25);
	}

	/**
	 * @brief Test PointerEvent with Release type
	 */
	void testPointerEvent_ReleaseType()
	{
		PointerEvent event;
		event.globalPosition = QPointF(300.0, 400.0);
		event.button = Qt::RightButton;
		event.buttons = Qt::NoButton;
		event.modifiers = Qt::AltModifier;
		event.type = PointerEventType::Release;

		QCOMPARE(event.type, PointerEventType::Release);
		QCOMPARE(event.button, Qt::RightButton);
		QCOMPARE(event.buttons, Qt::NoButton);
	}

	/**
	 * @brief Test PointerEvent with Move type
	 */
	void testPointerEvent_MoveType()
	{
		PointerEvent event;
		event.globalPosition = QPointF(150.0, 250.0);
		event.button = Qt::NoButton;
		event.buttons = Qt::LeftButton;
		event.modifiers = Qt::NoModifier;
		event.type = PointerEventType::Move;

		QCOMPARE(event.type, PointerEventType::Move);
		QCOMPARE(event.button, Qt::NoButton);
		QVERIFY(event.buttons & Qt::LeftButton);
	}

	/**
	 * @brief Test PointerEvent with multiple modifiers
	 */
	void testPointerEvent_MultipleModifiers()
	{
		PointerEvent event;
		event.modifiers = Qt::ControlModifier | Qt::ShiftModifier |
				  Qt::AltModifier;

		QVERIFY(event.modifiers & Qt::ControlModifier);
		QVERIFY(event.modifiers & Qt::ShiftModifier);
		QVERIFY(event.modifiers & Qt::AltModifier);
		QVERIFY(!(event.modifiers & Qt::MetaModifier));
	}

	/**
	 * @brief Test KeyboardEvent structure initialization
	 */
	void testKeyboardEvent_Initialization()
	{
		// Test basic initialization
		KeyboardEvent event;
		event.nativeScanCode = 42;
		event.key = Qt::Key_A;
		event.modifiers = Qt::ShiftModifier;
		event.text = "A";
		event.type = KeyEventType::Press;

		QCOMPARE(event.nativeScanCode, static_cast<quint32>(42));
		QCOMPARE(event.key, Qt::Key_A);
		QCOMPARE(event.modifiers, Qt::ShiftModifier);
		QCOMPARE(event.text, QString("A"));
		QCOMPARE(event.type, KeyEventType::Press);
	}

	/**
	 * @brief Test KeyboardEvent with Press type
	 */
	void testKeyboardEvent_PressType()
	{
		KeyboardEvent event;
		event.nativeScanCode = 30;
		event.key = Qt::Key_Return;
		event.modifiers = Qt::NoModifier;
		event.text = "\n";
		event.type = KeyEventType::Press;

		QCOMPARE(event.type, KeyEventType::Press);
		QCOMPARE(event.key, Qt::Key_Return);
		QCOMPARE(event.text, QString("\n"));
	}

	/**
	 * @brief Test KeyboardEvent with Release type
	 */
	void testKeyboardEvent_ReleaseType()
	{
		KeyboardEvent event;
		event.nativeScanCode = 17;
		event.key = Qt::Key_W;
		event.modifiers = Qt::ControlModifier;
		event.text = "";
		event.type = KeyEventType::Release;

		QCOMPARE(event.type, KeyEventType::Release);
		QCOMPARE(event.key, Qt::Key_W);
		QVERIFY(event.modifiers & Qt::ControlModifier);
	}

	/**
	 * @brief Test KeyboardEvent with special keys
	 */
	void testKeyboardEvent_SpecialKeys()
	{
		KeyboardEvent event;
		event.nativeScanCode = 1;
		event.key = Qt::Key_Escape;
		event.modifiers = Qt::NoModifier;
		event.text = "";
		event.type = KeyEventType::Press;

		QCOMPARE(event.key, Qt::Key_Escape);
		QVERIFY(event.text.isEmpty());
	}

	/**
	 * @brief Test KeyboardEvent with modifier key combinations
	 */
	void testKeyboardEvent_ModifierCombinations()
	{
		KeyboardEvent event;
		event.modifiers = Qt::ControlModifier | Qt::AltModifier;

		QVERIFY(event.modifiers & Qt::ControlModifier);
		QVERIFY(event.modifiers & Qt::AltModifier);
		QVERIFY(!(event.modifiers & Qt::ShiftModifier));
	}

	/**
	 * @brief Test PointerEvent with zero coordinates
	 */
	void testPointerEvent_ZeroCoordinates()
	{
		PointerEvent event;
		event.globalPosition = QPointF(0.0, 0.0);
		event.button = Qt::LeftButton;
		event.type = PointerEventType::Press;

		QCOMPARE(event.globalPosition.x(), 0.0);
		QCOMPARE(event.globalPosition.y(), 0.0);
	}

	/**
	 * @brief Test PointerEvent with negative coordinates
	 */
	void testPointerEvent_NegativeCoordinates()
	{
		PointerEvent event;
		event.globalPosition = QPointF(-50.0, -100.0);
		event.button = Qt::NoButton;
		event.type = PointerEventType::Move;

		QCOMPARE(event.globalPosition.x(), -50.0);
		QCOMPARE(event.globalPosition.y(), -100.0);
	}

	/**
	 * @brief Test KeyboardEvent with empty text
	 */
	void testKeyboardEvent_EmptyText()
	{
		KeyboardEvent event;
		event.nativeScanCode = 100;
		event.key = Qt::Key_F1;
		event.text = "";
		event.type = KeyEventType::Press;

		QVERIFY(event.text.isEmpty());
		QCOMPARE(event.key, Qt::Key_F1);
	}

	/**
	 * @brief Test KeyboardEvent with Unicode text
	 */
	void testKeyboardEvent_UnicodeText()
	{
		KeyboardEvent event;
		event.nativeScanCode = 50;
		event.key = Qt::Key_unknown;
		event.text = "€";
		event.type = KeyEventType::Press;

		QCOMPARE(event.text, QString("€"));
	}
};

QTEST_MAIN(TestInputEvents)
#include "test-input-events.moc"
