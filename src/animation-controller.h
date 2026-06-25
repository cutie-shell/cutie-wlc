#pragma once

#include <QObject>
#include <QPropertyAnimation>
#include <memory>

class CwlCompositor;

/**
 * @file animation-controller.h
 * @brief Animation management system for the Cutie Wayland Compositor
 * 
 * This file contains the CwlAnimationController class which provides centralized
 * animation management for the Cutie compositor, handling blur effects, launcher
 * transitions, and other animated UI elements.
 */

/**
 * @class CwlAnimationController
 * @brief Central animation controller for the Cutie compositor
 * 
 * @details The CwlAnimationController class manages all compositor animations including
 * blur effects, launcher positioning, and transition coordination. This class was 
 * extracted from CwlCompositor to improve code organization and maintainability.
 * 
 * The controller provides a clean interface for triggering animations and managing
 * their lifecycle, while internally handling the complex coordination between
 * different animation types and their effects on the compositor state.
 * 
 * @section features Key Features
 * - Centralized animation management
 * - Configurable animation durations
 * - Thread-safe animation control
 * - Clean separation from compositor logic
 * - Smart pointer memory management
 * 
 * @section usage Usage Example
 * @code
 * // Animation controller is typically initialized by the compositor
 * auto controller = std::make_unique<CwlAnimationController>(compositor);
 * 
 * // Trigger blur animation (e.g., when opening launcher)
 * controller->startBlurAnimation();
 * 
 * // Configure custom animation duration
 * controller->setAnimationDuration(300);
 * @endcode
 * 
 * @note This class requires a valid CwlCompositor instance to function properly.
 * @warning Animation methods should only be called from the main thread.
 * 
 * @see CwlCompositor
 * @see QPropertyAnimation
 * @since Version 1.0
 */
class CwlAnimationController : public QObject {
	Q_OBJECT

    public:
	/**
	 * @brief Constructs a new animation controller
	 * 
	 * @param compositor Pointer to the compositor instance that owns the animated properties.
	 *                  Must remain valid for the lifetime of this controller.
	 * @param parent Optional parent QObject for Qt's object hierarchy. Defaults to nullptr.
	 * 
	 * @pre compositor must not be nullptr
	 * @post All animation objects are initialized and ready for use
	 * 
	 * @note The constructor automatically sets up all animation objects and their connections.
	 */
	explicit CwlAnimationController(CwlCompositor *compositor,
					QObject *parent = nullptr);

	/**
	 * @brief Destroys the animation controller
	 * 
	 * @details Cleans up all animation objects and ensures proper resource deallocation.
	 * Any running animations are stopped before destruction.
	 * 
	 * @post All animation objects are properly destroyed
	 * @post No memory leaks from animation resources
	 */
	~CwlAnimationController();

	/**
	 * @name Animation Control Interface
	 * @brief Methods for controlling specific animation types
	 * @{
	 */

	/**
	 * @brief Starts the blur animation
	 * 
	 * @details Initiates a smooth transition that blurs the compositor background,
	 * typically used when opening the launcher or showing overlay interfaces.
	 * The animation transitions the blur property from its current value to 1.0.
	 * 
	 * @pre Compositor must be in a valid state
	 * @post Blur animation is running and will complete asynchronously
	 * 
	 * @note If another blur animation is already running, it will be stopped
	 * @note This method is thread-safe
	 * 
	 * @see startUnblurAnimation()
	 * @see CwlCompositor::blur()
	 */
	void startBlurAnimation();

	/**
	 * @brief Starts the unblur animation
	 * 
	 * @details Initiates a smooth transition that removes the blur effect from the
	 * compositor background, typically used when closing the launcher or hiding
	 * overlay interfaces. The animation transitions the blur property to 0.0.
	 * 
	 * @pre Compositor must be in a valid state
	 * @post Unblur animation is running and will complete asynchronously
	 * 
	 * @note If another blur animation is already running, it will be stopped
	 * @note This method is thread-safe
	 * 
	 * @see startBlurAnimation()
	 * @see CwlCompositor::blur()
	 */
	void startUnblurAnimation();

	/**
	 * @brief Starts the launcher open animation
	 * 
	 * @details Initiates a smooth transition that moves the launcher into view,
	 * transitioning the launcher position from 0.0 to 1.0 (fully open).
	 * This creates a sliding effect as the launcher appears on screen.
	 * 
	 * @pre Compositor must be in a valid state
	 * @pre Launcher should be in closed position (launcherPosition() == 0.0)
	 * @post Launcher open animation is running and will complete asynchronously
	 * 
	 * @note If another launcher animation is running, it will be stopped
	 * @note This method is thread-safe
	 * 
	 * @see startLauncherCloseAnimation()
	 * @see CwlCompositor::launcherPosition()
	 */
	void startLauncherOpenAnimation();

	/**
	 * @brief Starts the launcher close animation
	 * 
	 * @details Initiates a smooth transition that moves the launcher out of view,
	 * transitioning the launcher position from its current value to 0.0 (fully closed).
	 * This creates a sliding effect as the launcher disappears from screen.
	 * 
	 * @pre Compositor must be in a valid state
	 * @post Launcher close animation is running and will complete asynchronously
	 * 
	 * @note If another launcher animation is running, it will be stopped
	 * @note This method is thread-safe
	 * 
	 * @see startLauncherOpenAnimation()
	 * @see CwlCompositor::launcherPosition()
	 */
	void startLauncherCloseAnimation();

	/** @} */ // end of Animation Control Interface

	/**
	 * @name Configuration Interface
	 * @brief Methods for configuring animation behavior
	 * @{
	 */

	/**
	 * @brief Sets the default duration for all animations
	 * 
	 * @param duration Animation duration in milliseconds. Must be positive.
	 * 
	 * @pre duration > 0
	 * @post All future animations will use the specified duration
	 * 
	 * @note This affects all animation types (blur and launcher)
	 * @note Changes do not affect currently running animations
	 * @note Default duration is DEFAULT_ANIMATION_DURATION (250ms)
	 * 
	 * @see setBlurAnimationDuration()
	 * @see setLauncherAnimationDuration()
	 */
	void setAnimationDuration(int duration);

	/**
	 * @brief Sets the duration specifically for blur animations
	 * 
	 * @param duration Animation duration in milliseconds. Must be positive.
	 * 
	 * @pre duration > 0
	 * @post Future blur animations will use the specified duration
	 * 
	 * @note This overrides the default duration for blur/unblur animations only
	 * @note Changes do not affect currently running animations
	 * 
	 * @see setAnimationDuration()
	 * @see startBlurAnimation()
	 * @see startUnblurAnimation()
	 */
	void setBlurAnimationDuration(int duration);

	/**
	 * @brief Sets the duration specifically for launcher animations
	 * 
	 * @param duration Animation duration in milliseconds. Must be positive.
	 * 
	 * @pre duration > 0
	 * @post Future launcher animations will use the specified duration
	 * 
	 * @note This overrides the default duration for launcher animations only
	 * @note Changes do not affect currently running animations
	 * 
	 * @see setAnimationDuration()
	 * @see startLauncherOpenAnimation()
	 * @see startLauncherCloseAnimation()
	 */
	void setLauncherAnimationDuration(int duration);

	/** @} */ // end of Configuration Interface

    public slots:
	/**
	 * @brief Handles animation value changes during animation playback
	 * 
	 * @param value The new animation value as a QVariant
	 * 
	 * @details This slot is automatically connected to animation value change signals
	 * and ensures proper synchronization between animation progress and compositor
	 * state updates. It handles the conversion from QVariant to appropriate types
	 * and forwards the updates to the compositor.
	 * 
	 * @note This slot is called frequently during animation playback
	 * @note The value type depends on the specific animation being played
	 * @note This method is thread-safe
	 * 
	 * @internal This is primarily for internal use by the animation system
	 */
	void onAnimationValueChanged(const QVariant &value);

    private:
	/**
	 * @brief Initializes all animation objects
	 * 
	 * @details Creates and configures the QPropertyAnimation objects for all
	 * supported animation types. Sets up initial durations, easing curves,
	 * and target properties for each animation.
	 * 
	 * @pre m_compositor must be valid
	 * @post All animation member variables are initialized and configured
	 * 
	 * @note This method is called automatically during construction
	 * @internal Private implementation detail
	 */
	void setupAnimations();

	/**
	 * @brief Establishes signal-slot connections for animations
	 * 
	 * @details Connects animation signals (such as valueChanged, finished)
	 * to appropriate slots for handling animation lifecycle and state updates.
	 * This ensures proper coordination between animations and compositor state.
	 * 
	 * @pre setupAnimations() must have been called
	 * @post All necessary signal-slot connections are established
	 * 
	 * @note This method is called automatically during construction
	 * @internal Private implementation detail
	 */
	void setupConnections();

	/**
	 * @brief Pointer to the compositor instance
	 * 
	 * @details Non-owning pointer to the CwlCompositor that contains the
	 * animated properties. This pointer is used to access compositor
	 * methods and properties during animation updates.
	 * 
	 * @note This pointer must remain valid for the lifetime of the controller
	 * @note The compositor owns the controller, ensuring proper lifetime management
	 */
	CwlCompositor *m_compositor;

	/**
	 * @name Animation Objects
	 * @brief Smart pointers to QPropertyAnimation instances
	 * @{
	 */

	/**
	 * @brief Animation for blur effect activation
	 * @details Animates the compositor's blur property from current value to 1.0
	 */
	std::unique_ptr<QPropertyAnimation> m_blurAnim;

	/**
	 * @brief Animation for blur effect deactivation
	 * @details Animates the compositor's blur property from current value to 0.0
	 */
	std::unique_ptr<QPropertyAnimation> m_unblurAnim;

	/**
	 * @brief Animation for launcher opening
	 * @details Animates the compositor's launcherPosition from 0.0 to 1.0
	 */
	std::unique_ptr<QPropertyAnimation> m_launcherOpenAnim;

	/**
	 * @brief Animation for launcher closing
	 * @details Animates the compositor's launcherPosition to 0.0
	 */
	std::unique_ptr<QPropertyAnimation> m_launcherCloseAnim;

	/** @} */ // end of Animation Objects

	/**
	 * @brief Default animation duration in milliseconds
	 * 
	 * @details This constant defines the standard duration for all animations
	 * when no custom duration is specified. The value is chosen to provide
	 * smooth, responsive animations that feel natural to users.
	 * 
	 * @note This value can be overridden using the configuration methods
	 * @see setAnimationDuration()
	 */
	static constexpr int DEFAULT_ANIMATION_DURATION = 250;
};