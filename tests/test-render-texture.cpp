// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QTest>
#include <QSize>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "rendering/render-texture.h"

/**
 * @brief Unit tests for RenderTexture abstraction
 * 
 * Tests the basic functionality of the RenderTexture interface
 * and the OpenGLRenderTexture implementation.
 */
class TestRenderTexture : public QObject {
	Q_OBJECT

    private slots:
	void initTestCase();
	void cleanupTestCase();

	// OpenGLRenderTexture tests
	void testOpenGLRenderTexture_ValidTexture();
	void testOpenGLRenderTexture_InvalidTexture();
	void testOpenGLRenderTexture_DefaultConstructor();
	void testOpenGLRenderTexture_ZeroTextureId();
	void testOpenGLRenderTexture_EmptySize();
	void testOpenGLRenderTexture_NativeHandle();
	void testOpenGLRenderTexture_SizeReporting();
	void testOpenGLRenderTexture_TextureIdAccess();
	void testOpenGLRenderTexture_TargetAccess();
	void testOpenGLRenderTexture_Texture2DTarget();
	void testOpenGLRenderTexture_ExternalOESTarget();
};

void TestRenderTexture::initTestCase()
{
	// Test suite initialization
	qDebug() << "Starting RenderTexture test suite";
}

void TestRenderTexture::cleanupTestCase()
{
	// Test suite cleanup
	qDebug() << "RenderTexture test suite complete";
}

void TestRenderTexture::testOpenGLRenderTexture_ValidTexture()
{
	// Test that a valid texture reports correct properties
	GLuint textureId = 42;
	QSize size(1920, 1080);
	GLenum target = GL_TEXTURE_2D;

	OpenGLRenderTexture texture(textureId, size, target);

	QVERIFY(texture.isValid());
	QCOMPARE(texture.textureId(), textureId);
	QCOMPARE(texture.size(), size);
	QCOMPARE(texture.target(), target);
}

void TestRenderTexture::testOpenGLRenderTexture_InvalidTexture()
{
	// Test that a texture with ID 0 is invalid
	GLuint textureId = 0;
	QSize size(1920, 1080);
	GLenum target = GL_TEXTURE_2D;

	OpenGLRenderTexture texture(textureId, size, target);

	QVERIFY(!texture.isValid());
	QCOMPARE(texture.textureId(), static_cast<GLuint>(0));
}

void TestRenderTexture::testOpenGLRenderTexture_DefaultConstructor()
{
	// Test that default constructor creates an invalid texture
	OpenGLRenderTexture texture;

	QVERIFY(!texture.isValid());
	QCOMPARE(texture.textureId(), static_cast<GLuint>(0));
	QVERIFY(texture.size().isEmpty());
	QCOMPARE(texture.target(), static_cast<GLenum>(GL_TEXTURE_2D));
}

void TestRenderTexture::testOpenGLRenderTexture_ZeroTextureId()
{
	// Test that texture with ID 0 but valid size is still invalid
	GLuint textureId = 0;
	QSize size(800, 600);
	GLenum target = GL_TEXTURE_2D;

	OpenGLRenderTexture texture(textureId, size, target);

	QVERIFY(!texture.isValid());
}

void TestRenderTexture::testOpenGLRenderTexture_EmptySize()
{
	// Test that texture with valid ID but empty size is invalid
	GLuint textureId = 100;
	QSize size(0, 0);
	GLenum target = GL_TEXTURE_2D;

	OpenGLRenderTexture texture(textureId, size, target);

	QVERIFY(!texture.isValid());
}

void TestRenderTexture::testOpenGLRenderTexture_NativeHandle()
{
	// Test that native handle returns a pointer to the texture ID
	GLuint textureId = 123;
	QSize size(1024, 768);
	GLenum target = GL_TEXTURE_2D;

	OpenGLRenderTexture texture(textureId, size, target);

	void *handle = texture.nativeHandle();
	QVERIFY(handle != nullptr);

	// Verify the handle points to the correct texture ID
	GLuint *idPtr = static_cast<GLuint *>(handle);
	QCOMPARE(*idPtr, textureId);
}

void TestRenderTexture::testOpenGLRenderTexture_SizeReporting()
{
	// Test various texture sizes
	struct TestCase {
		int width;
		int height;
	};

	TestCase cases[] = {
		{ 1, 1 },	{ 256, 256 }, { 1920, 1080 },
		{ 3840, 2160 }, { 640, 480 },
	};

	for (const auto &testCase : cases) {
		QSize size(testCase.width, testCase.height);
		OpenGLRenderTexture texture(1, size, GL_TEXTURE_2D);

		QCOMPARE(texture.size(), size);
		QCOMPARE(texture.size().width(), testCase.width);
		QCOMPARE(texture.size().height(), testCase.height);
	}
}

void TestRenderTexture::testOpenGLRenderTexture_TextureIdAccess()
{
	// Test that texture ID is correctly stored and retrieved
	GLuint testIds[] = { 1, 42, 100, 999, 12345 };

	for (GLuint id : testIds) {
		OpenGLRenderTexture texture(id, QSize(100, 100), GL_TEXTURE_2D);
		QCOMPARE(texture.textureId(), id);
	}
}

void TestRenderTexture::testOpenGLRenderTexture_TargetAccess()
{
	// Test that target is correctly stored and retrieved
	GLuint textureId = 50;
	QSize size(512, 512);

	// Test GL_TEXTURE_2D target
	OpenGLRenderTexture texture2D(textureId, size, GL_TEXTURE_2D);
	QCOMPARE(texture2D.target(), static_cast<GLenum>(GL_TEXTURE_2D));

	// Test GL_TEXTURE_EXTERNAL_OES target
	OpenGLRenderTexture textureOES(textureId, size,
				       GL_TEXTURE_EXTERNAL_OES);
	QCOMPARE(textureOES.target(),
		 static_cast<GLenum>(GL_TEXTURE_EXTERNAL_OES));
}

void TestRenderTexture::testOpenGLRenderTexture_Texture2DTarget()
{
	// Test standard GL_TEXTURE_2D target
	GLuint textureId = 1;
	QSize size(1024, 1024);
	GLenum target = GL_TEXTURE_2D;

	OpenGLRenderTexture texture(textureId, size, target);

	QVERIFY(texture.isValid());
	QCOMPARE(texture.target(), target);
}

void TestRenderTexture::testOpenGLRenderTexture_ExternalOESTarget()
{
	// Test GL_TEXTURE_EXTERNAL_OES target (used for hardware buffers)
	GLuint textureId = 1;
	QSize size(1920, 1080);
	GLenum target = GL_TEXTURE_EXTERNAL_OES;

	OpenGLRenderTexture texture(textureId, size, target);

	QVERIFY(texture.isValid());
	QCOMPARE(texture.target(), target);
}

// Qt Test main macro
QTEST_MAIN(TestRenderTexture)

// Include the moc file for Qt's meta-object system
#include "test-render-texture.moc"
