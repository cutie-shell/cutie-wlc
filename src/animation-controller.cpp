#include "animation-controller.h"
#include <cutie-wlc.h>

CwlAnimationController::CwlAnimationController(CwlCompositor *compositor,
					       QObject *parent)
	: QObject(parent)
	, m_compositor(compositor)
{
	setupAnimations();
	setupConnections();
}

CwlAnimationController::~CwlAnimationController()
{
	// Smart pointers will automatically clean up animations
}

void CwlAnimationController::setupAnimations()
{
	// Validate compositor before creating animations
	if (!m_compositor) {
		qWarning()
			<< "CwlAnimationController::setupAnimations: No compositor available";
		return;
	}

	// Create animations targeting compositor properties with error checking
	try {
		m_blurAnim = std::make_unique<QPropertyAnimation>(m_compositor,
								  "blur", this);
		if (!m_blurAnim) {
			qWarning()
				<< "CwlAnimationController::setupAnimations: Failed to create blur animation";
			return;
		}

		m_unblurAnim = std::make_unique<QPropertyAnimation>(
			m_compositor, "blur", this);
		if (!m_unblurAnim) {
			qWarning()
				<< "CwlAnimationController::setupAnimations: Failed to create unblur animation";
			return;
		}

		m_launcherOpenAnim = std::make_unique<QPropertyAnimation>(
			m_compositor, "launcherPosition", this);
		if (!m_launcherOpenAnim) {
			qWarning()
				<< "CwlAnimationController::setupAnimations: Failed to create launcher open animation";
			return;
		}

		m_launcherCloseAnim = std::make_unique<QPropertyAnimation>(
			m_compositor, "launcherPosition", this);
		if (!m_launcherCloseAnim) {
			qWarning()
				<< "CwlAnimationController::setupAnimations: Failed to create launcher close animation";
			return;
		}
	} catch (const std::exception &e) {
		qCritical()
			<< "CwlAnimationController::setupAnimations: Exception creating animations:"
			<< e.what();
		return;
	} catch (...) {
		qCritical()
			<< "CwlAnimationController::setupAnimations: Unknown exception creating animations";
		return;
	}

	// Configure durations
	setAnimationDuration(DEFAULT_ANIMATION_DURATION);

	// Configure end values
	m_blurAnim->setEndValue(1.0);
	m_unblurAnim->setEndValue(0.0);
	m_launcherOpenAnim->setEndValue(1.0);
	m_launcherCloseAnim->setEndValue(0.0);
}

void CwlAnimationController::setupConnections()
{
	// Validate all animations exist before connecting signals
	if (!m_blurAnim || !m_unblurAnim || !m_launcherOpenAnim ||
	    !m_launcherCloseAnim) {
		qWarning()
			<< "CwlAnimationController::setupConnections: Some animations are not initialized";
		return;
	}

	// Connect all animations to value changed callback with error checking
	if (!connect(m_blurAnim.get(), &QVariantAnimation::valueChanged, this,
		     &CwlAnimationController::onAnimationValueChanged)) {
		qWarning()
			<< "CwlAnimationController::setupConnections: Failed to connect blur animation valueChanged signal";
	}

	if (!connect(m_unblurAnim.get(), &QVariantAnimation::valueChanged, this,
		     &CwlAnimationController::onAnimationValueChanged)) {
		qWarning()
			<< "CwlAnimationController::setupConnections: Failed to connect unblur animation valueChanged signal";
	}

	if (!connect(m_launcherOpenAnim.get(), &QVariantAnimation::valueChanged,
		     this, &CwlAnimationController::onAnimationValueChanged)) {
		qWarning()
			<< "CwlAnimationController::setupConnections: Failed to connect launcher open animation valueChanged signal";
	}

	if (!connect(m_launcherCloseAnim.get(),
		     &QVariantAnimation::valueChanged, this,
		     &CwlAnimationController::onAnimationValueChanged)) {
		qWarning()
			<< "CwlAnimationController::setupConnections: Failed to connect launcher close animation valueChanged signal";
	}

	// Connect special behavior for unblur animation finished with validation
	if (!connect(m_unblurAnim.get(), &QVariantAnimation::finished, this, [this]() {
		    // Validate compositor and workspace before accessing
		    if (!m_compositor) {
			    qWarning()
				    << "CwlAnimationController: Compositor is null in unblur finished callback";
			    return;
		    }

		    if (!m_compositor->m_workspace) {
			    qWarning()
				    << "CwlAnimationController: Workspace is null in unblur finished callback";
			    return;
		    }

		    // This mimics the original behavior from CwlCompositor
		    m_compositor->m_workspace->showDesktop(true);
	    })) {
		qWarning()
			<< "CwlAnimationController::setupConnections: Failed to connect unblur animation finished signal";
	}
}

void CwlAnimationController::startBlurAnimation()
{
	if (!m_blurAnim) {
		qWarning()
			<< "CwlAnimationController::startBlurAnimation: Blur animation not initialized";
		return;
	}
	m_blurAnim->start();
}

void CwlAnimationController::startUnblurAnimation()
{
	if (!m_unblurAnim) {
		qWarning()
			<< "CwlAnimationController::startUnblurAnimation: Unblur animation not initialized";
		return;
	}
	m_unblurAnim->start();
}

void CwlAnimationController::startLauncherOpenAnimation()
{
	if (!m_launcherOpenAnim) {
		qWarning()
			<< "CwlAnimationController::startLauncherOpenAnimation: Launcher open animation not initialized";
		return;
	}
	m_launcherOpenAnim->start();
}

void CwlAnimationController::startLauncherCloseAnimation()
{
	if (!m_launcherCloseAnim) {
		qWarning()
			<< "CwlAnimationController::startLauncherCloseAnimation: Launcher close animation not initialized";
		return;
	}
	m_launcherCloseAnim->start();
}

void CwlAnimationController::setAnimationDuration(int duration)
{
	m_blurAnim->setDuration(duration);
	m_unblurAnim->setDuration(duration);
	m_launcherOpenAnim->setDuration(duration);
	m_launcherCloseAnim->setDuration(duration);
}

void CwlAnimationController::setBlurAnimationDuration(int duration)
{
	m_blurAnim->setDuration(duration);
	m_unblurAnim->setDuration(duration);
}

void CwlAnimationController::setLauncherAnimationDuration(int duration)
{
	m_launcherOpenAnim->setDuration(duration);
	m_launcherCloseAnim->setDuration(duration);
}

void CwlAnimationController::onAnimationValueChanged(const QVariant &value)
{
	// Validate compositor before triggering render
	if (!m_compositor) {
		qWarning()
			<< "CwlAnimationController::onAnimationValueChanged: Compositor is null";
		return;
	}

	// Trigger compositor render on any animation value change
	m_compositor->triggerRender();
}