/**
 * Manage a sequence of times, represented by a list.
 * @constructor

 * @param {Object} options The options to use to configure the TimeSelector
 * @param [{date}] options.frameTimes (required) The array of frame times used to initialize this TimeSequence
 * @param {date} options.frameDelayMs (required) The frame delay in milliseconds
 * @param {date} options.dwellMs (required) The step size in milliseconds
 * @param {number} options.minFrameDelay (optional) The longest possible frame delay in milliseconds on the slider
 * @param {number} options.maxFrameDelay (optional) The shortest possible frame delay in milliseconds on the slider
 */
ral = ral || {};
ral.time = ral.time || {};

ral.time.ListTimeSequence = function( options ) {
    /* set the required parameters */
    if ( Object.prototype.toString.call( options.frameTimes ) !== '[object Array]' || options.frameTimes.length < 1 ) {
        console.log("ERROR: ral.time.ListTimeSequence constructor called without a valid frameTimes array.");
    }
    this.frameTimes   = options.frameTimes;

    this.currentFrameIndex = 0;
    this.currentFrameTime = this.frameTimes[this.currentFrameIndex];
    this.currentFrame = this.currentFrameTime;

    options.currentFrameTime = this.currentFrameTime;
    ral.time.TimeSequenceModel.call( this, options );

    console.log("Finished ral.time.ListTimeSequence constructor.");

    // This failed miserably.
    // this._setCurrentFrame(this.currentFrameTime);
};

/**
 * Inherit from the TimeSequenceModel class...
 */
ral.time.ListTimeSequence.prototype = Object.create( ral.time.TimeSequenceModel.prototype );

/**
 * Step forward one frame. Wraps to the beginning of the time sequence list when at the end.
 * @public
 */
ral.time.ListTimeSequence.prototype.stepForward = function()
{
    var newIdx = this.currentFrameIndex + 1;
    if ( newIdx >= this.frameTimes.length ) {
        console.log("ERROR: ListTimeSequence was stepped forward off the end of the time sequence.");
    }
    else {
        this.currentFrameIndex = newIdx;
        this.currentFrameTime = this.frameTimes[this.currentFrameIndex];

        this._setCurrentFrame(this.currentFrameTime);
    }
};

/**
 * Step forward one frame. Wraps to the end of the time sequence list when at the beginning.
 * @public
 */

ral.time.ListTimeSequence.prototype.stepBackward = function()
{
    var newIdx = this.currentFrameIndex - 1;
    if ( newIdx < 0 ) {
        console.log("ERROR: ListTimeSequence was stepped backward off the beginning of the time sequence.");
    }
    else {
        this.currentFrameIndex = newIdx;
        this.currentFrameTime = this.frameTimes[this.currentFrameIndex];

        this._setCurrentFrame(this.currentFrameTime);
    }
};

/**
 * Verify the passed-in frame is a valid frame.
 * @public
 *
 * @param {date} date The frame to test as a valid frame.
 */
ral.time.ListTimeSequence.prototype.isValidFrame = function(date)
{
    // Iterate through the frame times and see if the passed-in date is one of them.
    for (var i = 0; i < this.frameTimes.length; i++) {
        var thisFrame = this.frameTimes[i];
        if ( thisFrame.getTime() == date.getTime() ) {
            return true;
        }
    }

    return false;
};

/**
 * Verify the passed-in frame is valid, and set it as current. If the frame is not valid, return without doing anything.
 * @public
 *
 * @param {date} date The frame to set as current, if it is a valid frame.
 */
ral.time.ListTimeSequence.prototype.setCurrentFrame = function(date)
{
    // Iterate through the frame times and see if the passed-in date is one of them.
    for (var i = 0; i < this.frameTimes.length; i++) {
        var thisFrame = this.frameTimes[i];
        if ( thisFrame.getTime() == date.getTime() ) {
            this.currentFrameIndex = i;
            this.currentFrameTime = this.frameTimes[this.currentFrameIndex];
            this._setCurrentFrame(this.currentFrameTime);
            return;
        }
    }

    console.log("ERROR: ral.time.ListTimeSequence.prototype.setCurrentFrame() was passed an invalid frame: " + date);
};

/**
 * Set the first time sequence frame as the current frame.
 * @public
 */
ral.time.ListTimeSequence.prototype.setFirstFrameCurrent = function()
{
    this.currentFrameIndex = 0;
    this.currentFrameTime = this.frameTimes[this.currentFrameIndex];

    this._setCurrentFrame(this.currentFrameTime);
};

/**
 * Set the last time sequence frame as the current frame.
 * @public
 */
ral.time.ListTimeSequence.prototype.setLastFrameCurrent = function()
{
    this.currentFrameIndex = this.frameTimes.length - 1;
    this.currentFrameTime = this.frameTimes[this.currentFrameIndex];

    this._setCurrentFrame(this.currentFrameTime);
};

ral.time.ListTimeSequence.prototype.setTimeSequenceFrames = function(dates)
{
    if ( Object.prototype.toString.call( dates ) !== '[object Array]' || dates.length < 1 ) {
        console.log("ERROR: ral.time.ListTimeSequence setTimeSequenceFrames() called with invalid frameTimes array.");
    }

    this.frameTimes   = dates;

    // Pick the closest new frame to the previously-visible frame
    var prevCurrentFrame = this.currentFrameTime || this.frameTimes[0];
    var closestFrame = this.frameTimes[0];
    var closestFrameIdx = this.currentFrameIndex;
    for (var j = 0; j < dates.length; j++) {
        var thisDate = dates[j];
        if ( thisDate.getTime() <= prevCurrentFrame.getTime() ) {
            closestFrame = thisDate;
            closestFrameIdx = j;
        }
    }

    this.currentFrameIndex = closestFrameIdx;
    this.currentFrameTime = this.frameTimes[this.currentFrameIndex];
    this.currentFrame = this.currentFrameTime;

    this.fireSequenceChangedEvent();





    //   TODO: Is this correct? Change the frame without notifying?


    // The following code resulted in a *SECOND* animation thread running consecutively, due to
    //     the fact that it results in a frameChanged() notification outside of the normal animation loop.

    // // Only call _setCurrentFrame if playing, because it notifies the animation controller.
    // if ( this.getIsPlaying() ) {
        // this._setCurrentFrame(this.currentFrameTime);
    // }
    // else {
        // // Reset the current frame manually, without notifying.
        // //   TODO: Perhaps perform this action with a method on the superclass, since it's superclass data
        // this.currentFrameTime = this.currentFrameTime;
    // }
};

ral.time.ListTimeSequence.prototype.getTimeSequenceFrames = function()
{
    return this.frameTimes;
};

/**
 * Determine whether the current frame is the first frame in the time sequence
 * @public
 *
 * @return {boolean} Whether the current frame is first in the time sequence.
 */
ral.time.ListTimeSequence.prototype.isCurrentFrameFirst = function() {
    return (this.currentFrameIndex == 0);
};

/**
 * Determine whether the current frame is the last frame in the time sequence
 * @public
 *
 * @return {boolean} Whether the current frame is last in the time sequence.
 */
ral.time.ListTimeSequence.prototype.isCurrentFrameLast = function() {
    return (this.currentFrameIndex == (this.frameTimes.length - 1));
};


/**
 * Get the first time sequence frame.
 * @public
 *
 * @return {Date} The date of the first frame in the sequence.
 */
ral.time.ListTimeSequence.prototype.getFirstFrame = function()
{
  return this.frameTimes[0];
};

/**
 * Get the last time sequence frame.
 * @public
 *
 * @return {Date} The date of the last frame in the sequence.
 */
ral.time.ListTimeSequence.prototype.getLastFrame = function()
{
  return this.frameTimes[this.frameTimes.length - 1];
};
