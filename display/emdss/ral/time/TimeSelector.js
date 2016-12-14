var ral = ral || {};
ral.time = ral.time || {};

/**
 * Present an interface to the user to view and modify the selected time.  Send time change notifications to all listeners.
 * @constructor
 *
 * @param {Object} options - The options to use to configure the TimeSelector
 * @param {string} options.target (required) The target div ID to use for the UI components
 * @param {ral.time.TimeSequenceModel} options.timeSequence (required) The TimeSequenceModel to use for setting and events
 * @param {boolean} options.userConfigurable (optional) Tells this class if the user should have the option to configure the time selector or not.
 *                  Defaults to false. May be true or may contain configuration hints that will be passed to the TimeSelectorConfiguration.
 */
ral.time.TimeSelector = function( options )
{
  this.target       = options.target;
  this.timeSequence = options.timeSequence;

  /* set the default parameters */
  this.userConfigurable = false;

  /* set the optional parameters if provided */
  if( "userConfigurable" in options ) this.userConfigurable = options.userConfigurable;

  /* listen to the TimeSequenceModel */
  this.timeSequence.addListener( this );
};


/**
 * Initialize the UI components of the view
 * @abstract
 */
ral.time.TimeSelector.prototype.initView = function()
{
  console.log( "ral.time.TimeSelector does not have a default view and is abstract." );
};


/**
 * This is called after the selected time is changed to allow the UI to update and reflect the change
 * @abstract
 */
ral.time.TimeSelector.prototype.updateView = function()
{
  console.log( "ral.time.TimeSelector does not have a default view and is abstract." );
};


/**
 * Get the selected time
 * @public
 *
 * @returns {date} The selected time
 */
ral.time.TimeSelector.prototype.getSelectedTime = function()
{
  return this.timeSequence.getCurrentFrame();
};


/**
 * Set the currently selected time
 * @public
 *
 * @param {date} selectedTime Set the selected time to this new value
 */
ral.time.TimeSelector.prototype.setSelectedTime = function( selectedTime )
{
  /* if setting the same time, do nothing */
  if( this.getSelectedTime().getTime() == selectedTime.getTime() )
    return;

  /* set the time on the TimeSequenceModel */
  this.timeSequence.setCurrentFrame( selectedTime );

  /* update the view */
  this.updateView();
};


/**
 * Get the time range that is currently set
 * @public
 *
 * @returns {object} Contains object.minTime and object.maxTime
 */
ral.time.TimeSelector.prototype.getTimeRange = function()
{
  //TODO: Get min(max)Time from this.timeSequence
  return { minTime: new Date( this.minTime ), maxTime: new Date( this.maxTime ) };
};


/**
 * Set the time sequence model
 * @public
 *
 * @param {ral.time.TimeSequence} timeSequence The time sequence model to use for choosing times
 */
ral.time.TimeSelector.prototype.setTimeSequence = function( timeSequence )
{
  this.timeSequence = timeSequence;

  this.initView();
};


/**
 * Get the time sequence model
 * @public
 *
 * @returns {ral.time.TimeSequenceModel}
 */
ral.time.TimeSelector.prototype.getTimeSequence = function()
{
  return this.timeSequence;
};


/**
 * Listen to the TimeSequenceModel for time change events
 *
 * @param {date} selectedTime The new selected time frame
 */
ral.time.TimeSelector.prototype.frameChanged = function( selectedTime )
{
  /* update the view */
  this.updateView();
};


/**
 * Listen to the TimeSequenceModel for time range change events
 *
 * @param {ral.time.TimeSequenceModel} timeSequenceModel Unused since we already have a reference
 */
ral.time.TimeSelector.prototype.sequenceChanged = function( timeSequenceModel )
{
  /* initialize the view since the range has changed */
  this.initView();
};