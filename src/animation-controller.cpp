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
	// Create animations targeting compositor properties
	m_blurAnim = std::make_unique<QPropertyAnimation>(m_compositor, "blur",
							  this);
	m_unblurAnim = std::make_unique<QPropertyAnimation>(m_compositor,
							    "blur", this);
	m_launcherOpenAnim = std::make_unique<QPropertyAnimation>(
		m_compositor, "launcherPosition", this);
	m_launcherCloseAnim = std::make_unique<QPropertyAnimation>(
		m_compositor, "launcherPosition", this);

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
	// Connect all animations to value changed callback
	connect(m_blurAnim.get(), &QVariantAnimation::valueChanged, this,
		&CwlAnimationController::onAnimationValueChanged);
	connect(m_unblurAnim.get(), &QVariantAnimation::valueChanged, this,
		&CwlAnimationController::onAnimationValueChanged);
	connect(m_launcherOpenAnim.get(), &QVariantAnimation::valueChanged,
		this, &CwlAnimationController::onAnimationValueChanged);
	connect(m_launcherCloseAnim.get(), &QVariantAnimation::valueChanged,
		this, &CwlAnimationController::onAnimationValueChanged);

	// Connect special behavior for unblur animation finished
	connect(m_unblurAnim.get(), &QVariantAnimation::finished, this,
		[this]() {
			// This mimics the original behavior from CwlCompositor
			m_compositor->m_workspace->showDesktop(true);
		});
}

void CwlAnimationController::startBlurAnimation()
{
	m_blurAnim->start();
}

void CwlAnimationController::startUnblurAnimation()
{
	m_unblurAnim->start();
}

void CwlAnimationController::startLauncherOpenAnimation()
{
	m_launcherOpenAnim->start();
}

void CwlAnimationController::startLauncherCloseAnimation()
{
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
	// Trigger compositor render on any animation value change
	m_compositor->triggerRender();
}