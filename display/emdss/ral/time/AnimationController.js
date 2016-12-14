/**
 * Present an interface to the user to allow the user to control animations.  This basic
 * implementation plays from current position, to end of range in TimeSelector, and jumps
 * back to the beginning of the range in TimeSelector; provides play/pause and step buttons;
 * and provides an animation speed slider.  This class can be extended to provide different
 * play modes; and additional controls and user configurations.
 * @constructor

 * @param {Object} options The options to use to configure the AnimationController
 * @param {TimeSequenceModel} options.timeSequence (required) The time sequence object that maintains state. Note that TimeSequence is
 *           abstract, and an implementation of that interface must be used instead.
 * @param {boolean} options.usesExternalPlayTimer (optional) Mode that turns off play() method calling itself recursively.
 * @param {string} options.mode (optional) String indicating the animation mode (FORWARD, BACKWARD, or SWEEP).
 */
var ral = ral || {};
ral.time = ral.time || {};
ral.time.AnimationController = function( options )
{
    /* set the required parameters */
    this.timeSequence = options.timeSequence;
    this.timeSequence.setAnimationController( this );

    /* set the default values on optional parameters */
    this.mode = 'FORWARD';
    this.usesExternalPlayTimer = false;
    this.pendingFrameTransition = null;

    if( typeof( options.mode ) !== "undefined" )                  this.mode = options.mode;
    if( typeof( options.usesExternalPlayTimer ) !== "undefined" ) this.usesExternalPlayTimer = options.usesExternalPlayTimer;

    if ( this.mode != 'FORWARD' && this.mode != 'BACKWARD' && this.mode != 'SWEEP') {
        console.log("ERROR: Mode for Animation controller must be FORWARD, BACKWARD, or SWEEP. Got: " + this.mode);
        this.mode = 'FORWARD';
    }
};

/**
 * Set the time sequence model.
 * @public
 *
 * @param {ral.time.TimeSequenceModel} timeSequence The new time sequence model
 */
ral.time.AnimationController.prototype.setTimeSequence = function( timeSequence )
{
    this.timeSequence = timeSequence;
};

/**
 * Toggle the playing state. If the mode for this controller is 'SWEEP,' then start
 *   sweeping in the forward direction.
 * @public
 */
ral.time.AnimationController.prototype.toggleAnimation = function()
{
    var wasPlaying = this.timeSequence.getIsPlaying();



    if ( wasPlaying ) {
        this.clearPendingFrameTransition();
        this.timeSequence.setIsPlaying(false);
    }
    else {
        this.timeSequence.setIsPlaying(true);

        // If mode is sweep, start in forward direction.
        if ( this.mode == 'SWEEP' ) {
            this.sweep = 'FORWARD';
        }

        this.play();
    }
};

/**
 * Step forward to the next frame. If on the last frame, restart at
 *   the beginning of the sequence. If the animation is playing, stop
 *   the animation.
 * @public
 */
ral.time.AnimationController.prototype.stepForward = function()
{
    // Pause the animation if it's playing
    if ( this.timeSequence.getIsPlaying() ) {
        this.toggleAnimation();
    }

    if ( this.timeSequence.isCurrentFrameLast() ) {
        this.timeSequence.setFirstFrameCurrent();
    }
    else {
        this.timeSequence.stepForward();
    }
};

/**
 * Step backward to the next frame. If on the first frame, restart at
 *   the end of the sequence. If the animation is playing, stop
 *   the animation.
 * @public
 */
ral.time.AnimationController.prototype.stepBackward = function()
{
    // Pause the animation if it's playing
    if ( this.timeSequence.getIsPlaying() ) {
        this.toggleAnimation();
    }

    if ( this.timeSequence.isCurrentFrameFirst() ) {
        this.timeSequence.setLastFrameCurrent();
    }
    else {
        this.timeSequence.stepBackward();
    }
};

/**
 * Set the current frame to the passed-in Date, which must represent a valid
 *   frame in order for this method to succeed. If an invalid frame Date is provided, this
 *   method does nothing.
 * @public
 *
 * @param {Date} The date of the desired frame.
 * @return {boolean} True if the passed-in frame was a valid frame. False otherwise.
 */
ral.time.AnimationController.prototype.selectFrame = function(date)
{
    if ( !this.timeSequence.isValidFrame(date) ) {
        return false;
    }

    this.timeSequence.setCurrentFrame(date);

    return true;
};

/**
 * Set the frame delay in milliseconds, on the model.
 * @public
 *
 * @param {number} frameDelayMs The number of milliseconds to pause before advancing to the next frame in animation mode
 */
ral.time.AnimationController.prototype.setFrameDelay = function(delay)
{
   this.timeSequence.setFrameDelay(delay);
};

/**
 * Get the frame delay, in milliseconds.
 * @public
 */
ral.time.AnimationController.prototype.getFrameDelay = function()
{
    return this.timeSequence.getFrameDelay();
};

/**
 * Set the dwell in milliseconds, on the model.
 * @public
 *
 * @param {number} dwell The number of milliseconds to pause at the end of the time sequence before advancing to the next frame in animation mode
 */
ral.time.AnimationController.prototype.setDwell = function(dwell)
{
    this.timeSequence.setDwell(dwell);
};

/**
 * Get the dwell, in milliseconds.
 * @public
 */
ral.time.AnimationController.prototype.getDwell = function()
{
    return this.timeSequence.getDwell();
};

/**
 * Get the time sequence model.
 * @public
 */
ral.time.AnimationController.prototype.getTimeSequence = function()
{
    return this.timeSequence;
};

/**
 * Set the time at which the frame change was initiated. Used by external timers
 *   when they want to determine the amount of time that has passed since the
 *   frame change was initiated.
 * @public
 */
ral.time.AnimationController.prototype.setFrameChangeStartTime = function(date)
{
    this.frameChangeStartTime = date;
};

/**
 * Get the time at which the frame change was initiated. Used by external timers
 *   when they want to determine the amount of time that has passed since the
 *   frame change was initiated.
 * @public
 */
ral.time.AnimationController.prototype.getFrameChangeStartTime = function()
{
    return isNaN(this.frameChangeStartTime) ? new Date() : this.frameChangeStartTime;
};

/**
 * Set the delay for the current frame change, if a frame change is underway. Not
 *   strictly thread-safe!
 * @public
 */
ral.time.AnimationController.prototype.setCurrentFrameDelay = function(delayMs)
{
    this.currentFrameDelay = delayMs;
};

/**
 * Get the delay for the current frame change, if a frame change is underway. Not
 *   strictly thread-safe!
 * @public
 */
ral.time.AnimationController.prototype.getCurrentFrameDelay = function()
{
    return isNaN(this.currentFrameDelay) ? this.timeSequence.frameDelayMs : this.currentFrameDelay;
};

/**
 * Set the delay for the next frame change. Not thread-safe!
 * @public
 */
ral.time.AnimationController.prototype.setNextFrameDelay = function(delayMs)
{
    this.nextFrameDelay = delayMs;
};

/**
 * Get the delay for the next frame change. Not thread-safe!
 * @public
 */
ral.time.AnimationController.prototype.getNextFrameDelay = function()
{
    return isNaN(this.nextFrameDelay) ? this.timeSequence.frameDelayMs : this.nextFrameDelay;
};

/**
 * Set the mode inticating that an external play timer is used, so play() should not call itself recursively.
 * @public
 */
ral.time.AnimationController.prototype.setUsesExternalPlayTimer = function(usesExternalTimer)
{
    this.usesExternalPlayTimer = usesExternalTimer;
};


/**
 * Get the mode inticating that an external play timer is used, so play() should not call itself recursively.
 * @public
 */
ral.time.AnimationController.prototype.getUsesExternalPlayTimer = function()
{
    return this.usesExternalPlayTimer;
};

/**
 * Determine whether animation is playing.
 * @public
 */
ral.time.AnimationController.prototype.getIsPlaying = function()
{
    return this.timeSequence.getIsPlaying();
};

/**
 * Call the appropriate step method on the model and repeat while in the playing state
 * @private
 */
ral.time.AnimationController.prototype.play = function()
{
    if ( this.timeSequence.getIsPlaying() )
    {
        // Capture the time at which the frame change was initiated.
        this.setFrameChangeStartTime( new Date() );
        this.setCurrentFrameDelay(this.getNextFrameDelay());

        var delay = 0;
        if ( this.mode == 'FORWARD' ) {
            delay = this.playF();
        }
        else if ( this.mode == 'BACKWARD' ) {
            delay = this.playB();
        }
        else if ( this.mode == 'SWEEP' ) {
            if ( this.sweep == 'FORWARD' ) {
                if ( this.timeSequence.isCurrentFrameLast() ) {
                    this.sweep = 'BACKWARD';
                    delay = this.playB();
                }
                else {
                    delay = this.playF();
                }
            }
            else {
                if ( this.timeSequence.isCurrentFrameFirst() ) {
                    this.sweep = 'FORWARD';
                    delay = this.playF();
                }
                else {
                    delay = this.playB();
                }
            }
        }

        this.setNextFrameDelay(delay);
        if ( !this.getUsesExternalPlayTimer() ) {
            setTimeout( this.play.bind( this ), delay );
        }
    }
};

/**
 * Perform the appropriate step for forward play and calculate delay for the current frame (use dwell time if last frame).
 * @private
 */
ral.time.AnimationController.prototype.playF = function()
{
    if ( this.timeSequence.isCurrentFrameLast() ) {
        this.timeSequence.setFirstFrameCurrent();
    }
    else {
        this.timeSequence.stepForward();
    }

    var delay = this.timeSequence.frameDelayMs;
    if ( this.timeSequence.isCurrentFrameLast() ) {
        delay = this.timeSequence.dwellMs;
    }

    return delay;
};

/**
 * Perform the appropriate step for backward play and calculate delay for the current frame (use dwell time if first frame).
 * @private
 */
ral.time.AnimationController.prototype.playB = function()
{
    if ( this.timeSequence.isCurrentFrameFirst() ) {
        this.timeSequence.setLastFrameCurrent();
    }
    else {
        this.timeSequence.stepBackward();
    }

    var delay = this.timeSequence.frameDelayMs;
    if ( this.timeSequence.isCurrentFrameFirst() ) {
        delay = this.timeSequence.dwellMs;
    }

    return delay;
};


/** ***********************************************************************************************
 * The following methods support external control of the animation frame advance. If classes using
 *   this controller want to delay frame changes until all the tiles are loaded on the OpenLayers layer,
 *   they should:
 *     o set the usesExternalPlayTimer flag on this controller at the time of creation
 *     o start a timer to track elapsed time since the beginning of the frame change
 *     o listen for 'loadend' events from the OpenLayers layer for notification of complete time loading
 *     o issue a call to completeFrameTransition() via a setTimeout() call. The timeout should be reduced
 *         by the amount of time it took for the layer to load all of its tiles.
 *     o Register the pending frame transition with this controller to eliminate threading errors. That is
 *         accomplished by calling setPendingFrameTransition(). The same object used to register must be passed
 *         to completeFrameTranstion() when it is called. A frame transition object can be any arbitrary, unique
 *         javascript object -- it is simply used to identify a valid pending call to completeFrameTransition() -- all others are ignored.
 *
 * If the usesExternalPlayTimer flag is not set on this object, the play() method will recurse
 *   on its own.
 *
 */

/**
 * Determine whether the passed-in object matches the pending frame transition. This can be any arbitrary, unique,
 * javascript object.
 * @return True if the passed-in object matches the pending frame transition.
 */
ral.time.AnimationController.prototype.isPendingFrameTransition = function(pendingFrameTransitionRef) {
    return ( this.pendingFrameTransition != null && this.pendingFrameTransition == pendingFrameTransitionRef);
};

/**
 * Set the pending frame transtion to the passed-in object.
 */
ral.time.AnimationController.prototype.setPendingFrameTransition = function(pendingFrameTransitionRef) {
    this.pendingFrameTransition = pendingFrameTransitionRef;
};

/**
 * Clear the pending frame transtion object if it matches the passed-in object. If the passed-in object
 * does not match the pending frame transition, do not clear it.
 * @return True if the passed-in object matches the pending frame transition, and it was cleared. Otherwise false.
 */
ral.time.AnimationController.prototype.removePendingFrameTransition = function(pendingFrameTransitionRef) {
    var wasPendingFrameTransition = ( this.pendingFrameTransition == pendingFrameTransitionRef );

    if ( wasPendingFrameTransition ) {
        this.pendingFrameTransition = null;
    }

    return wasPendingFrameTransition;
};

/**
 * Clear any pending frame transition object.
 */
ral.time.AnimationController.prototype.clearPendingFrameTransition = function() {
    this.pendingFrameTransition = null;
};

/**
 * Finish the frame transition and start the next frame.
 *   Use this method if you are continuing animation from a 'loadend' event on the openlayers layer, after
 *   waiting for the layer to load all tiles.
 * @private
 */
ral.time.AnimationController.prototype.completeFrameTransition = function(newLayer, oldLayer, newDate, pendingFrameTransitionRef, desiredOpacity) {
    // console.log("            Switching from layer: " + (oldLayer ? oldLayer.name : "Unknown") );
    // console.log("                to new layer:                   " + newLayer.name);
    // console.log("                    with new date:              " + newDate);
    
    if ( typeof desiredOpacity == 'undefined' ) {
        desiredOpacity = 1.0;
    }

    if ( !this.isPendingFrameTransition(pendingFrameTransitionRef) ) {
        // console.log("                CANCELLED CANCELLED CANCELLED CANCELLED CANCELLED CANCELLED (before transition)");
        if (this.pendingFrameTransition) console.log("                Controller ref: [from: " + this.pendingFrameTransition.from + ", to: " + this.pendingFrameTransition.to +", continue: " + this.pendingFrameTransition.continue + "]");
        if (pendingFrameTransitionRef) console.log("                Passed-in ref: [from: " + pendingFrameTransitionRef.from + ", to: " + pendingFrameTransitionRef.to +", continue: " + pendingFrameTransitionRef.continue + "]");
        return;
    }

    newLayer.setOpacity(desiredOpacity);
    if (oldLayer) {
        // oldLayer.setVisibility(false);
        oldLayer.setOpacity(0.0);
        // oldLayer.setVisibility(false);
    }

    // Notify listeners
    this.timeSequence.fireFrameTransitionCompleteEvent(newDate, newLayer);

    if ( !this.removePendingFrameTransition(pendingFrameTransitionRef) ) {
        // console.log("                CANCELLED CANCELLED CANCELLED CANCELLED CANCELLED CANCELLED (before recursion)");
        return;
    }

    // Recurse.
    setTimeout( this.play.bind( this ), 0 );
};




