// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QTest>
#include <QSignalSpy>
#include <QDebug>
#include <QMetaType>

#include "../src/input/input-router.h"
#include "../src/input/input-events.h"
#include "mocks/mock-view.h"
#include "mocks/mock-surface-compositor.h"

/**
 * @brief Integration tests for CwlInputRouter with MockSurfaceCompositor
 * 
 * Tests the input router's interaction with the surface compositor,
 * including focus management, signal emissions, and event routing
 * coordination with the compositor.
 * 
 * Phase 1: Basic integration testing of router with mock compositor
 */
class TestInputRouter : public QObject {
	Q_OBJECT

    private slots:
	// Test suite setup/teardown
	void initTestCase();

	// Test fixture setup/teardown
	void init();
	void cleanup();

	// Basic construction tests
	void testConstruction();
	void testConstructionWithNullCompositor();

	// Focus management tests
	void testSetKeyboardFocus_EmitsSignal();
	void testSetKeyboardFocus_UpdatesFocusState();
	void testSetKeyboardFocus_NoSignalWhenFocusUnchanged();
	void testSetKeyboardFocus_ClearFocus();
	void testSetKeyboardFocus_SwitchBetweenSurfaces();

	// Event routing tests
	void testRoutePointerEvent_CallsWithValidEvent();
	void testRouteKeyboardEvent_CallsWithValidEvent();
	void testRouteKeyboardEvent_WithNoFocus();

	// Integration with compositor tests
	void testRouterWithEmptyCompositor();
	void testRouterWithPopulatedCompositor();
	void testFocusManagementIntegration();

    private:
	// Test fixtures
	MockSurfaceCompositor *m_compositor = nullptr;
	CwlInputRouter *m_router = nullptr;
	CwlView *m_testSurface1 = nullptr;
	CwlView *m_testSurface2 = nullptr;

	// Helper methods
	PointerEvent
	createPointerEvent(QPointF position = QPointF(100, 100),
			   Qt::MouseButton button = Qt::LeftButton,
			   PointerEventType type = PointerEventType::Press);
	KeyboardEvent
	createKeyboardEvent(Qt::Key key = Qt::Key_A,
			    KeyEventType type = KeyEventType::Press);
};

void TestInputRouter::initTestCase()
{
	// Register metatypes for signal/slot connections and QSignalSpy
	qRegisterMetaType<CwlView *>("CwlView*");
	qDebug() << "Test suite initialized - metatypes registered";
}

void TestInputRouter::init()
{
	// Create mock compositor
	m_compositor = new MockSurfaceCompositor();

	// Create input router with mock compositor
	m_router = new CwlInputRouter(m_compositor);

	// Create mock CwlView objects for testing
	// These are minimal mocks that only provide Qt meta-object support
	m_testSurface1 = new CwlView(this);
	m_testSurface2 = new CwlView(this);

	qDebug() << "Test fixture initialized";
}

void TestInputRouter::cleanup()
{
	// Clean up in reverse order
	delete m_router;
	m_router = nullptr;

	delete m_compositor;
	m_compositor = nullptr;

	// Clean up mock views
	delete m_testSurface1;
	m_testSurface1 = nullptr;

	delete m_testSurface2;
	m_testSurface2 = nullptr;

	qDebug() << "Test fixture cleaned up";
}

void TestInputRouter::testConstruction()
{
	// Verify router was constructed successfully
	QVERIFY(m_router != nullptr);

	// Verify initial state
	QCOMPARE(m_router->keyboardFocus(), nullptr);

	qDebug() << "testConstruction passed";
}

void TestInputRouter::testConstructionWithNullCompositor()
{
	// Create router with null compositor (edge case)
	CwlInputRouter *nullRouter = new CwlInputRouter(nullptr);

	QVERIFY(nullRouter != nullptr);
	QCOMPARE(nullRouter->keyboardFocus(), nullptr);

	delete nullRouter;

	qDebug() << "testConstructionWithNullCompositor passed";
}

void TestInputRouter::testSetKeyboardFocus_EmitsSignal()
{
	// Setup signal spy
	QSignalSpy focusChangedSpy(m_router, &CwlInputRouter::focusChanged);

	// Set focus to surface
	m_router->setKeyboardFocus(m_testSurface1);

	// Verify signal was emitted once
	QCOMPARE(focusChangedSpy.count(), 1);

	// Verify signal arguments
	QList<QVariant> arguments = focusChangedSpy.takeFirst();
	QCOMPARE(arguments.at(0).value<CwlView *>(),
		 nullptr); // oldFocus was nullptr
	QCOMPARE(arguments.at(1).value<CwlView *>(),
		 m_testSurface1); // newFocus is testSurface1

	qDebug() << "testSetKeyboardFocus_EmitsSignal passed";
}

void TestInputRouter::testSetKeyboardFocus_UpdatesFocusState()
{
	// Initial state: no focus
	QCOMPARE(m_router->keyboardFocus(), nullptr);

	// Set focus
	m_router->setKeyboardFocus(m_testSurface1);

	// Verify focus state updated
	QCOMPARE(m_router->keyboardFocus(), m_testSurface1);

	qDebug() << "testSetKeyboardFocus_UpdatesFocusState passed";
}

void TestInputRouter::testSetKeyboardFocus_NoSignalWhenFocusUnchanged()
{
	// Set initial focus
	m_router->setKeyboardFocus(m_testSurface1);

	// Setup signal spy AFTER setting initial focus
	QSignalSpy focusChangedSpy(m_router, &CwlInputRouter::focusChanged);

	// Set focus to same surface again
	m_router->setKeyboardFocus(m_testSurface1);

	// Verify NO signal was emitted (focus didn't change)
	QCOMPARE(focusChangedSpy.count(), 0);

	qDebug() << "testSetKeyboardFocus_NoSignalWhenFocusUnchanged passed";
}

void TestInputRouter::testSetKeyboardFocus_ClearFocus()
{
	// Set initial focus
	m_router->setKeyboardFocus(m_testSurface1);
	QCOMPARE(m_router->keyboardFocus(), m_testSurface1);

	// Setup signal spy
	QSignalSpy focusChangedSpy(m_router, &CwlInputRouter::focusChanged);

	// Clear focus by setting to nullptr
	m_router->setKeyboardFocus(nullptr);

	// Verify focus was cleared
	QCOMPARE(m_router->keyboardFocus(), nullptr);

	// Verify signal was emitted
	QCOMPARE(focusChangedSpy.count(), 1);

	// Verify signal arguments
	QList<QVariant> arguments = focusChangedSpy.takeFirst();
	QCOMPARE(arguments.at(0).value<CwlView *>(),
		 m_testSurface1); // oldFocus
	QCOMPARE(arguments.at(1).value<CwlView *>(), nullptr); // newFocus

	qDebug() << "testSetKeyboardFocus_ClearFocus passed";
}

void TestInputRouter::testSetKeyboardFocus_SwitchBetweenSurfaces()
{
	// Set initial focus
	m_router->setKeyboardFocus(m_testSurface1);

	// Setup signal spy
	QSignalSpy focusChangedSpy(m_router, &CwlInputRouter::focusChanged);

	// Switch focus to different surface
	m_router->setKeyboardFocus(m_testSurface2);

	// Verify focus changed
	QCOMPARE(m_router->keyboardFocus(), m_testSurface2);

	// Verify signal was emitted
	QCOMPARE(focusChangedSpy.count(), 1);

	// Verify signal arguments
	QList<QVariant> arguments = focusChangedSpy.takeFirst();
	QCOMPARE(arguments.at(0).value<CwlView *>(),
		 m_testSurface1); // oldFocus
	QCOMPARE(arguments.at(1).value<CwlView *>(),
		 m_testSurface2); // newFocus

	qDebug() << "testSetKeyboardFocus_SwitchBetweenSurfaces passed";
}

void TestInputRouter::testRoutePointerEvent_CallsWithValidEvent()
{
	// Create a pointer event
	PointerEvent event = createPointerEvent();

	// Call routePointerEvent (Phase 1: stub, just verify it doesn't crash)
	m_router->routePointerEvent(event);

	// In Phase 1, we just verify the call completes without error
	// In Phase 2, we would verify the event was routed correctly
	QVERIFY(true); // Test passed if we got here

	qDebug() << "testRoutePointerEvent_CallsWithValidEvent passed";
}

void TestInputRouter::testRouteKeyboardEvent_CallsWithValidEvent()
{
	// Create a keyboard event
	KeyboardEvent event = createKeyboardEvent();

	// Call routeKeyboardEvent (Phase 1: stub, just verify it doesn't crash)
	m_router->routeKeyboardEvent(event);

	// In Phase 1, we just verify the call completes without error
	// In Phase 2, we would verify the event was routed correctly
	QVERIFY(true); // Test passed if we got here

	qDebug() << "testRouteKeyboardEvent_CallsWithValidEvent passed";
}

void TestInputRouter::testRouteKeyboardEvent_WithNoFocus()
{
	// Verify no focus
	QCOMPARE(m_router->keyboardFocus(), nullptr);

	// Create a keyboard event
	KeyboardEvent event = createKeyboardEvent();

	// Call routeKeyboardEvent with no focus
	// Should handle gracefully (not crash)
	m_router->routeKeyboardEvent(event);

	// Verify still no focus
	QCOMPARE(m_router->keyboardFocus(), nullptr);

	qDebug() << "testRouteKeyboardEvent_WithNoFocus passed";
}

void TestInputRouter::testRouterWithEmptyCompositor()
{
	// Verify compositor is empty
	QCOMPARE(m_compositor->getSurfaceCount(), 0);

	// Create pointer event
	PointerEvent event = createPointerEvent();

	// Route event with empty compositor
	// Should handle gracefully (not crash)
	m_router->routePointerEvent(event);

	// Verify compositor still empty
	QCOMPARE(m_compositor->getSurfaceCount(), 0);

	qDebug() << "testRouterWithEmptyCompositor passed";
}

void TestInputRouter::testRouterWithPopulatedCompositor()
{
	// Add surfaces to compositor
	// Note: In Phase 1, we can't create real CwlView objects
	// This test demonstrates the integration pattern for later phases
	m_compositor->addSurface(m_testSurface1);
	m_compositor->addSurface(m_testSurface2);

	// Verify compositor has surfaces
	QCOMPARE(m_compositor->getSurfaceCount(), 2);

	// Create pointer event
	PointerEvent event = createPointerEvent();

	// Route event with populated compositor
	m_router->routePointerEvent(event);

	// In Phase 1, we just verify the call completes
	// In Phase 2, we would verify hit-testing and event delivery
	QVERIFY(true);

	qDebug() << "testRouterWithPopulatedCompositor passed";
}

void TestInputRouter::testFocusManagementIntegration()
{
	// This test demonstrates how focus management integrates with compositor

	// Setup signal spies before adding surface
	QSignalSpy routerFocusSpy(m_router, &CwlInputRouter::focusChanged);
	QSignalSpy compositorAddSpy(m_compositor,
				    &CwlSurfaceCompositor::surfaceAdded);

	// Add surface to compositor
	m_compositor->addSurface(m_testSurface1);

	// Set focus to surface that exists in compositor
	m_router->setKeyboardFocus(m_testSurface1);

	// Verify focus was set
	QCOMPARE(m_router->keyboardFocus(), m_testSurface1);

	// Verify focus change signal was emitted
	QCOMPARE(routerFocusSpy.count(), 1);

	// Compositor add signal should have been emitted earlier (during addSurface)
	QCOMPARE(compositorAddSpy.count(), 1);

	// Remove surface from compositor
	m_compositor->removeSurface(m_testSurface1);

	// In Phase 2, removing a focused surface might clear focus automatically
	// For Phase 1, we just verify the operations complete

	qDebug() << "testFocusManagementIntegration passed";
}

// Helper method implementations

PointerEvent TestInputRouter::createPointerEvent(QPointF position,
						 Qt::MouseButton button,
						 PointerEventType type)
{
	PointerEvent event;
	event.globalPosition = position;
	event.button = button;
	event.buttons = button; // Simplified for testing
	event.modifiers = Qt::NoModifier;
	event.type = type;
	return event;
}

KeyboardEvent TestInputRouter::createKeyboardEvent(Qt::Key key,
						   KeyEventType type)
{
	KeyboardEvent event;
	event.nativeScanCode = 0; // Not relevant for Phase 1 testing
	event.key = key;
	event.modifiers = Qt::NoModifier;
	event.text = QString(); // Empty for now
	event.type = type;
	return event;
}

// Qt Test requires this at the end of the file
QTEST_MAIN(TestInputRouter)
#include "test-input-router.moc"
