# Cutie-WLC Test Suite

This directory contains unit tests and integration tests for cutie-wlc.

## Structure

```
tests/
├── CMakeLists.txt          # Test build configuration
├── README.md               # This file
├── mocks/                  # Mock objects for testing (to be created)
│   ├── CMakeLists.txt
│   ├── mock-rendering-api.h
│   └── mock-surface-compositor.h
├── test-render-texture.cpp      # RenderTexture unit tests
├── test-input-events.cpp        # Input event structure tests
├── test-input-device-manager.cpp # InputDeviceManager integration tests
├── test-input-router.cpp        # InputRouter integration tests
└── test-surface-compositor.cpp  # SurfaceCompositor integration tests
```

## Building Tests

Tests are built automatically when `BUILD_TESTING` is enabled (default):

```bash
mkdir build && cd build
cmake ..
make
```

To disable tests:

```bash
cmake -DBUILD_TESTING=OFF ..
```

## Running Tests

Run all tests using CTest:

```bash
ctest
```

Run all tests using the custom target:

```bash
make run-tests
```

Run specific test:

```bash
./tests/test-render-texture
```

Run tests with verbose output:

```bash
ctest --verbose
```

Run tests with output on failure:

```bash
ctest --output-on-failure
```

## Writing Tests

Tests use Qt Test framework. Each test file should:

1. Include `<QTest>`
2. Create a test class inheriting from `QObject`
3. Define test methods as private slots
4. Use `QTEST_MAIN()` macro to generate main function

Example:

```cpp
#include <QTest>

class TestExample : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();    // Called before first test
    void cleanupTestCase(); // Called after last test
    void init();            // Called before each test
    void cleanup();         // Called after each test
    
    void testSomething();   // Actual test method
};

void TestExample::testSomething() {
    QCOMPARE(1 + 1, 2);
    QVERIFY(true);
}

QTEST_MAIN(TestExample)
#include "test-example.moc"
```

## Test Organization

- **Unit tests**: Test individual classes in isolation
- **Integration tests**: Test interaction between components
- **Mock objects**: Simplified implementations for testing dependencies

## Debian Package Building

When building the Debian package, tests are automatically run during the build process. This is configured in `debian/rules` with:

- `BUILD_TESTING=ON`: Ensures tests are built
- `override_dh_auto_test`: Runs CTest with output on failure

To build the package locally:

```bash
dpkg-buildpackage -us -uc
```

If tests fail, the package build will fail. To skip tests during package build (not recommended):

```bash
DEB_BUILD_OPTIONS=nocheck dpkg-buildpackage -us -uc
```

## Code Coverage

To generate coverage report (requires gcov/lcov):

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage" ..
make
ctest
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage-html
```
