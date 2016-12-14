/**
 * Manage a sequence of times, represented by start date, end date, and interval.
 * @constructor
 *
 * @param {Object} options The options to use to configure the TimeSelector
 * @param {string} mode (optional) Must be either "fixed" or "real-time", defaults to "fixed"
 * @param {date|number} options.minTime (required) The earliest time that is allowed to be selected; or if number, the number of ms before "now"
 * @param {date|number} options.maxTime (required) The latest time that is allowed to be selected; or if number, the number of ms after "now"
 * @param {number} options.updateInterval (optional) Used only in "real-time" mode, updates the time selector's range every X milliseconds, defaults to 60000
 * @param {number} options.intervalMs (required) The time sequence interval, or step size
 * @param {number} options.roundMs (required) Round the selected time to the nearest number in milliseconds (e.g., 300000
 *     to round to nearest 5 minute interval.  Hint: use 1 or undefined for no rounding)
 */
ral = ral || {};
ral.time = ral.time || {};

ral.time.RangeIntervalTimeSequence = function( options )
{
    ral.time.TimeSequenceModel.call( this, options );

    /* save required parameters */
    this.minTime    = options.minTime;
    this.maxTime    = options.maxTime;
    this.intervalMs = options.intervalMs;
    this.roundMs    = options.roundMs;

    /* set default values on optional parameters */
    this.mode           = "fixed";
    this.updateInterval = 60000;

    /* save the optional parameters if provided */
    if( "mode" in options ) this.mode = options.mode;
    if( "updateInterval" in options ) this.updateInterval = options.updateInterval;

    /* if real-time mode was set, start updating */
    if( this.mode == "real-time" )
    {
        this.updateForRealTime();
    }

    console.log("Finished ral.time.RangeIntervalTimeSequence constructor.");
};

/**
 * Inherit from the TimeSequenceModel class...
 */
ral.time.RangeIntervalTimeSequence.prototype = Object.create( ral.time.TimeSequenceModel.prototype );

/**
 * Step forward one frame. Wraps to the beginning of the time range when at the end.
 * @public
 */
ral.time.RangeIntervalTimeSequence.prototype.stepForward = function()
{
    var time  = this.getCurrentFrame();
    var time2 = new Date( time.getTime() + this.intervalMs );
    var range = this.getTimeRange();

    if ( time2 > range.maxTime )
    {
        time2 = range.minTime;
    }

    this._setCurrentFrame( time2 );
};

/**
 * Step forward one frame. Wraps to the end of the time range when at the beginning.
 * @public
 */
ral.time.RangeIntervalTimeSequence.prototype.stepBackward = function()
{
    var time  = this.getCurrentFrame();
    var time2 = new Date( time.getTime() - this.intervalMs );
    var range = this.getTimeRange();

    if ( time2 < range.minTime )
    {
        time2 = range.maxTime;
    }

    this._setCurrentFrame( time2 );
};

/**
 * Verify the passed-in frame is a valid frame.
 * @public
 *
 * @param {date} date The frame to test as a valid frame.
 */
ral.time.RangeIntervalTimeSequence.prototype.isValidFrame = function(date)
{
    /* check that the date is within range */
    if( this.minTime.getTime() > date.getTime() || date.getTime() > this.maxTime.getTime() )
    {
        return false;
    }

    /* check that the time is rounded to the nearest provided ms */
    if( date.getTime() % this.roundMs == 0 )
    {
        return true;
    }
    else {
        return false;
    }
};

/**
 * Verify the passed-in frame is valid, and set it as current. If the frame is not valid, return without doing anything.
 * @public
 *
 * @param {date} date The frame to set as current, if it is a valid frame.
 */
ral.time.RangeIntervalTimeSequence.prototype.setCurrentFrame = function(date)
{
    if ( this.isValidFrame(date) )
    {
        // Set the selected frame to the desired time.
        this._setCurrentFrame(date);
    }
    else {
        // Set the selected frame to the nearest frame to the desired time.
        var rounded = new Date( Math.round( date.getTime() / this.roundMs ) * this.roundMs );
        this._setCurrentFrame(rounded);
    }
};

/**
 * Set the first time sequence frame as the current frame.
 * @public
 */
ral.time.RangeIntervalTimeSequence.prototype.setFirstFrameCurrent = function()
{
    var range = this.getTimeRange();
    this._setCurrentFrame( range.minTime );
};

/**
 * Set the last time sequence frame as the current frame.
 * @public
 */
ral.time.RangeIntervalTimeSequence.prototype.setLastFrameCurrent = function()
{
    var range = this.getTimeRange();
    this._setCurrentFrame( range.maxTime );
};

ral.time.RangeIntervalTimeSequence.prototype.getTimeSequenceFrames = function()
{
    // TODO: IMPLEMENT THIS!!!
};

/**
 * Determine whether the current frame is the first frame in the time sequence
 * @public
 *
 * @return {boolean} Whether the current frame is first in the time sequence.
 */
ral.time.RangeIntervalTimeSequence.prototype.isCurrentFrameFirst = function() {
     return ( this.getCurrentFrame().getTime() == this.minTime.getTime() );
};

/**
 * Determine whether the current frame is the last frame in the time sequence
 * @public
 *
 * @return {boolean} Whether the current frame is last in the time sequence.
 */
ral.time.RangeIntervalTimeSequence.prototype.isCurrentFrameLast = function() {
   return ( this.getCurrentFrame().getTime() == this.maxTime.getTime() );
};

/**
 * Set a new interval for this sequence
 * @public
 *
 * @param {Number} newInterval The new interval. in milliseconds
 */
ral.time.RangeIntervalTimeSequence.prototype.setIntervalMs = function( newInterval )
{
  this.intervalMs = newInterval;
}

//
// ============================================================================
// Methods not part of the TimeSequenceModel interface -- impl-specific methods.
// ============================================================================
//

/**
 * Take a date object and return a new date rounded to the nearest time that is evenly divisible by 'roundMs' parameter
 * @private
 *
 * @param {date} dateObj round the value of this date object, but return a new object and leave it unmodified
 */
ral.time.RangeIntervalTimeSequence.prototype.roundTime = function( dateObj )
{
    // If round is not set, then ignore
    if( ! this.roundMS ) return dateObj;

    // Pull out some params for easy access
    var min   = this.minTime.getTime();
    var time  = dateObj.getTime();
    var round = this.roundMS;
    var max   = this.maxTime.getTime();

    // Round the value
    var rounded = Math.round( time / round ) * round;

    // Make sure that the rounded time is not greater than max
    while( rounded > max )
    {
        rounded -= round;
    }

    return new Date( rounded );
};

/**
 * Set the time range to define the range of allowable selected times and update the UI
 * @public
 *
 * @param {date} minTime The minimum time that may be selected
 * @param {date} maxTime The maximum time that may be selected
 */
ral.time.RangeIntervalTimeSequence.prototype.setTimeRange = function( minTime, maxTime )
{
    // If setting the same time range, do nothing
    if ( this.mode == "fixed" && this.minTime.getTime() == minTime.getTime() && this.maxTime.getTime() == maxTime.getTime() )
    {
        return;
    }

    this.minTime = new Date( minTime );
    this.maxTime = new Date( maxTime );

    this.fireSequenceChangedEvent();
};

/**
 * Get the current time range for the TimeSequence
 * @public
 *
 * @returns {object} Contains object.minTime and object.maxTime
 */
ral.time.RangeIntervalTimeSequence.prototype.getTimeRange = function()
{
    return { minTime: new Date( this.minTime ), maxTime: new Date( this.maxTime ) };
};

/**
 * Get the first time sequence frame.
 * @public
 *
 * @return {Date} The date of the first frame in the sequence.
 */
ral.time.RangeIntervalTimeSequence.prototype.getFirstFrame = function()
{
  return this.minTime;
};

/**
 * Get the last time sequence frame.
 * @public
 *
 * @return {Date} The date of the last frame in the sequence.
 */
ral.time.RangeIntervalTimeSequence.prototype.getLastFrame = function()
{
  return this.maxTime;
};