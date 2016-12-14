/**
 * Present an interface to the user to support time sequence operations. Implementations of
 * this interface are used by the AnimationController to manage selectable times, and by
 * various views (button controls, slider controls) to maintain state and be notified of changes.
 * @constructor

 * @param {Object} options The options to use to configure the TimeSelector
 * @param {number} options.frameDelayMs (required) The frame delay in milliseconds
 * @param {number} options.dwell (required) The step size in milliseconds
 * @param {Date} options.currentFrameTime (optional) The initial date to use as the current frame
 */
ral = ral || {};
ral.time = ral.time || {};

ral.time.TimeSequenceModel = function( options ) {

    /* set the required parameters */
    this.frameDelayMs = options.frameDelayMs;
    this.dwellMs      = options.dwellMs;

    /* set the default values on optional parameters */
    this.currentFrame = new Date(0);

    /* override default values on optional parameters if they were provided */
    if( typeof( options.currentFrameTime ) !== "undefined" ) this.currentFrame = options.currentFrameTime;

    /* initialize the listeners to be an empty list */
    if ( typeof this.listeners === "undefined" ) {
        this.listeners = [];
    }

    /* initialize other parameters */
    this.isPlaying = false;
    this.eventListeners = [];
};

/**
 * Step forward one frame. Wrap to the start if at the end.
 * @public abstract
 */
ral.time.TimeSequenceModel.prototype.stepForward = function()
{
    console.log( "TimeSequenceModel does not have a default stepForward() method and is abstract." );
};

/**
 * Step backward one frame. Wrap to the end if at the start.
 * @public abstract
 */
ral.time.TimeSequenceModel.prototype.stepBackward = function()
{
    console.log( "TimeSequenceModel does not have a default stepBackward() method and is abstract." );
};

/**
 * Change the playing state and fire playingStateChanged event.
 * @public
 *
 * @param {boolean} The playing state to apply.
 */
ral.time.TimeSequenceModel.prototype.setIsPlaying = function(isPlaying)
{
    this.isPlaying = isPlaying;
    this.firePlayingStateChangedEvent();
};

/**
 * Get the playing state.
 * @public
 *
 * @return {boolean} The current playing state.
 */
ral.time.TimeSequenceModel.prototype.getIsPlaying = function()
{
    return this.isPlaying;
};

/**
 * Set the current frame and fire frameChangedEvent. NOT TO BE CALLED BY CLIENTS -- use setCurrentFrame().
 *   This method assumes that the passed-in date is a valid frame.
 * @private
 *
 * @param {date} date The frame to set as current.
 */
ral.time.TimeSequenceModel.prototype._setCurrentFrame = function(date)
{
    this.currentFrame = date;
    this.fireFrameChangedEvent();
};

/**
 * Verify the passed-in frame is a valid frame.
 * @public
 *
 * @param {date} date The frame to test as a valid frame.
 */
ral.time.TimeSequenceModel.prototype.isValidFrame = function(date)
{
    console.log( "TimeSequenceModel does not have a default isValidFrame() method and is abstract." );
};

/**
 * Verify the passed-in frame is valid, and set it as current. If the frame is not valid, return without doing anything.
 * @public
 *
 * @param {date} date The frame to set as current, if it is a valid frame.
 */
ral.time.TimeSequenceModel.prototype.setCurrentFrame = function(date)
{
    console.log( "TimeSequenceModel does not have a default setCurrentFrame() method and is abstract." );
};

/**
 * Get the current frame.
 * @public
 *
 * @return {Date} The date of the current frame.
 */
ral.time.TimeSequenceModel.prototype.getCurrentFrame = function()
{
    return this.currentFrame;
};

/**
 * Set the first time sequence frame as the current frame.
 * @public
 */
ral.time.TimeSequenceModel.prototype.setFirstFrameCurrent = function()
{
    console.log( "TimeSequenceModel does not have a default setFirstFrameCurrent() method and is abstract." );
};

/**
 * Get the first time sequence frame.
 * @public
 *
 * @return {Date} The date of the first frame in the sequence.
 */
ral.time.TimeSequenceModel.prototype.getFirstFrame = function()
{
    console.log( "TimeSequenceModel does not have a default getFirstFrame() method and is abstract." );
};

/**
 * Set the last time sequence frame as the current frame.
 * @public
 */
ral.time.TimeSequenceModel.prototype.setLastFrameCurrent = function()
{
    console.log( "TimeSequenceModel does not have a default setLastFrameCurrent() method and is abstract." );
};

/**
 * Get the last time sequence frame.
 * @public
 *
 * @return {Date} The date of the last frame in the sequence.
 */
ral.time.TimeSequenceModel.prototype.getLastFrame = function()
{
  console.log( "TimeSequenceModel does not have a default getLastFrame() method and is abstract." );
};

/**
 * Get the array of frame times.
 * @public abstract
 *
 * @return {[Date]} An array of Date objects representing the frames of this time sequence.
 */
ral.time.TimeSequenceModel.prototype.getTimeSequenceFrames = function()
{
    console.log( "TimeSequenceModel does not have a default getTimeSequenceFrames() method and is abstract." );
    return [];
};

/**
 * Set the frame delay in milliseconds
 * @public
 *
 * @param {number} frameDelayMs The number of milliseconds to pause before advancing to the next frame in animation mode
 */
ral.time.TimeSequenceModel.prototype.setFrameDelay = function(delay)
{
    this.frameDelayMs = delay;
    this.fireFrameDelayChangedEvent();
};

/**
 * Get the frame delay, in ms.
 * @public
 *
 * @return {number} The frame delay in ms.
 */
ral.time.TimeSequenceModel.prototype.getFrameDelay = function()
{
    return this.frameDelayMs;
};

/**
 * Set the dwell in milliseconds. Dwell is the time the animation pauses before restarting the loop,
 *   or switching directions (when in SWEEP mode).
 * @public
 *
 * @param {number} dwell The number of milliseconds to pause at the end of the time sequence before restarting the loop in animation mode
 */
ral.time.TimeSequenceModel.prototype.setDwell = function(dwell)
{
    this.dwellMs = dwell;
    this.fireDwellChangedEvent();
};

/**
 * Get dwell in ms. Dwell is the time the animation pauses before restarting the loop, or switching directions (when in SWEEP mode).
 * @public
 *
 * @return {boolean} The dwell value in ms.
 */
ral.time.TimeSequenceModel.prototype.getDwell = function()
{
    return this.dwellMs;
};

/**
 * Determine whether the current frame is the first frame in the time sequence
 * @public
 *
 * @return {boolean} Whether the current frame is first in the time sequence.
 */
ral.time.TimeSequenceModel.prototype.isCurrentFrameFirst = function() {
    console.log( "TimeSequenceModel does not have a default isCurrentFrameFirst() method and is abstract." );
};

/**
 * Determine whether the current frame is the last frame in the time sequence
 * @public
 *
 * @return {boolean} Whether the current frame is last in the time sequence.
 */
ral.time.TimeSequenceModel.prototype.isCurrentFrameLast = function() {
    console.log( "TimeSequenceModel does not have a default isCurrentFrameLast() method and is abstract." );
};

/**
 * Add a listener. The following events are broadcast by this object. If the listener implements the associated
 *   method, it will be called when the event is broadcast. Otherwise the listener will be ignored for that event.
 * playingStateChangedEvent - playingStateChanged(boolean) is called on listeners that implement this method when
 *     the the value of isPlaying changes on this object. This is used to change the play button to a pause
 *     button, etc.
 * frameChangedEvent - frameChanged(Date) is called on listeners that implement this method when the current frame
 *     changes on this object.
 * sequenceChangedEvent - sequenceChanged([TimeSequence Object]) is called on listeners that implement this method when the set
 *     of possible frames changes on this object. This object is passed to listeners because the listener may need to
 *     call implementation-specific methods to get in sync with the advertised changes.
 * frameDelayChangedEvent - frameDelayChanged(number) is called on listeners that implement this method when the
 *     frame delay is changed on this object.
 * dwellChangedEvent - dwellChanged(number) is called on listeners that implement this method when the
 *     dwell is changed on this object.
 * @public
 *
 * @param {Object} Object to receive events.
 */
ral.time.TimeSequenceModel.prototype.addListener = function( listener )
{
    if ( typeof listener === "undefined" || listener == null ) {
        console.log("ERROR: Attempting to add bogus listener to TimeSelectorModel");
        return;
    }

    this.listeners.push( listener );
};

/**
 * Remove a listener.
 * @public
 *
 * @param {Object} Object to remove from the set of listeners.
 */
ral.time.TimeSequenceModel.prototype.removeListener = function( listener )
{
    for ( var i = 0; i < this.listeners.length; i++ ) {
        if ( this.listeners[i] == listener ) {
            this.listeners.splice( i, 1 );
        }
    }
};

/**
 * Set the animation controller for the time sequence model.  This is useful if the app will
 * replace the time sequence model.  It will allow the new animation controller to be easily
 * transferred to the new time sequence model.
 *
 * @param animationController {ral.time.AnimationController} The animation controller
 */
ral.time.TimeSequenceModel.prototype.setAnimationController = function( animationController )
{
    this.animationController = animationController;
};

/**
 * Set the animation controller for the time sequence model.  This is useful if the app will
 * replace the time sequence model.  It will allow the new animation controller to be easily
 * transferred to the new time sequence model.
 *
 * @param animationController {ral.time.AnimationController} The animation controller
 */
ral.time.TimeSequenceModel.prototype.getAnimationController = function()
{
    return this.animationController;
};

/**
 * Fire an event to broadcast that the playing state has changed. Call
 * playingStateChanged(boolean) on all listeners that implement that method.
 * @private
 */
ral.time.TimeSequenceModel.prototype.firePlayingStateChangedEvent = function()
{
    for ( var i = 0; i < this.listeners.length; i++ ) {
        if ( typeof(this.listeners[i].playingStateChanged) == "function" ) {
            this.listeners[i].playingStateChanged( this.getIsPlaying() );
        }
    }
};

/**
 * Fire an event to broadcast that the current frame has changed. Call
 * frameChanged(Date) on all listeners that implement that method.
 * @private
 */
ral.time.TimeSequenceModel.prototype.fireFrameChangedEvent = function()
{
    for ( var i = 0; i < this.listeners.length; i++ ) {
        if ( typeof(this.listeners[i].frameChanged) == "function" ) {
            this.listeners[i].frameChanged( this.getCurrentFrame() );
        }
    }
};

/**
 * Fire an event to broadcast that the frame transition is complete. Call
 * frameTransitionComplete(Date) on all listeners that implement that method.
 * @private
 */
ral.time.TimeSequenceModel.prototype.fireFrameTransitionCompleteEvent = function(newDate, newLayer)
{
    if ( newDate.getTime() != this.getCurrentFrame().getTime() ) {
        console.log("WARNING: TimeSequenceModel::fireFrameTransitionCompleteEvent is firing with newDate: " + newDate + " that does not match the currentFrame: " + this.getCurrentFrame());
    }
    
    for ( var i = 0; i < this.listeners.length; i++ ) {
        if ( typeof(this.listeners[i].frameTransitionComplete) == "function" ) {
            this.listeners[i].frameTransitionComplete( newDate, newLayer );
        }
    }
};

/**
 * Fire an event to broadcast that the set of possible frames has changed. Call
 * sequenceChanged(TimeSequence Object) on all listeners that implement that method.
 * @private
 */
ral.time.TimeSequenceModel.prototype.fireSequenceChangedEvent = function()
{
    for ( var i = 0; i < this.listeners.length; i++ ) {
        if ( typeof(this.listeners[i].sequenceChanged) == "function" ) {
            this.listeners[i].sequenceChanged( this );
        }
    }
};

/**
 * Fire an event to broadcast that the frame delay has changed. Call
 * frameDelayChanged(number) on all listeners that implement that method.
 * @private
 */
ral.time.TimeSequenceModel.prototype.fireFrameDelayChangedEvent = function()
{
    for ( var i = 0; i < this.listeners.length; i++ ) {
        if ( typeof(this.listeners[i].frameDelayChanged) == "function" ) {
            this.listeners[i].frameDelayChanged( this.getFrameDelay() );
        }
    }
};

/**
 * Fire an event to broadcast that the dwell has changed. Call
 * dwellChanged(number) on all listeners that implement that method.
 * @private
 */
ral.time.TimeSequenceModel.prototype.fireDwellChangedEvent = function()
{
    for ( var i = 0; i < this.listeners.length; i++ ) {
        if ( typeof(this.listeners[i].dwellChanged) == "function" ) {
            this.listeners[i].dwellChanged( this.getDwell() );
        }
    }
};

/**
 * Transfer all of the listeners and animation controller from 'this' TSM into the
 * new TSM.  Useful if the application's TSM is changing.
 *
 * @param tsm2 {ral.time.TimeSequenceModel} The target to receive the listeners and AnimationController
 */
ral.time.TimeSequenceModel.prototype.transferListenersInto = function( tsm2 )
{
    /* transfer the listeners */
    tsm2.listeners = this.listeners;
    this.listeners = [];

    /* set the time delays */
    tsm2.frameDelayMs = this.frameDelayMs;
    tsm2.dwellMs      = this.dwellMs;

    /* transfer the animation controller if it exists */
    var animController = this.getAnimationController();
    if( typeof( animController ) !== "undefined" )
    {
        animController.setTimeSequence( tsm2 );
        tsm2.setAnimationController( animController );
    }

    /* fire an event so that listeners know the TimeSequenceModel changed */
    tsm2.fireSequenceChangedEvent();
    tsm2.fireFrameChangedEvent();
};

