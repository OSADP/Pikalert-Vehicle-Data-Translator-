/**
 * Manage a sequence of times, represented by start date, end date, and interval.
 * @constructor
 *
 * @param {Object} options The options to use to configure the TimeSequence
 * @param {date|number} options.msBefore (required) The number of ms before "now"
 * @param {date|number} options.msAfter (required) The number of ms after "now"
 * @param {number} options.updateInterval (optional) Updates the time selector's range every X milliseconds, defaults to 60 secs
 */
ral = ral || {};
ral.time = ral.time || {};

ral.time.RealTimeRangeIntervalTimeSequence = function( options )
{
    ral.time.TimeSequenceModel.call( this, options );

    /* save required parameters */
    this.msBefore   = options.msBefore;
    this.msAfter    = options.msAfter;

    /* set default values on optional parameters */
    this.updateInterval = 60000;

    /* save the optional parameters if provided */
    if( "updateInterval" in options ) this.updateInterval = options.updateInterval;

    /* set the min and max time */
    options.minTime = new Date( new Date().getTime() + this.msBefore );
    options.maxTime = new Date( new Date().getTime() + this.msAfter );
    ral.time.RangeIntervalTimeSequence.call( this, options );

    /* start to update */
    this.start();
};

/**
 * Inherit from the TimeSequenceModel class...
 */
ral.time.RealTimeRangeIntervalTimeSequence.prototype = Object.create( ral.time.RangeIntervalTimeSequence.prototype );

/**
 * Start real-time updates on this TimeSequenceModel
 */
ral.time.RealTimeRangeIntervalTimeSequence.prototype.start = function()
{
    this.running = true;
    this.updateForRealTime();
};

/**
 * Stop real-time updates on this TimeSequenceModel
 */
ral.time.RealTimeRangeIntervalTimeSequence.prototype.stop = function()
{
    this.running = false;
};

ral.time.RealTimeRangeIntervalTimeSequence.prototype.updateForRealTime = function()
{
    if( this.running )
    {
        /* set the min and max time */
        var minTime = new Date( new Date().getTime() - this.msBefore );
        var maxTime = new Date( new Date().getTime() + this.msAfter );
        this.setTimeRange( minTime, maxTime );

        setTimeout( this.updateForRealTime.bind( this ), this.updateInterval );
    }
};
