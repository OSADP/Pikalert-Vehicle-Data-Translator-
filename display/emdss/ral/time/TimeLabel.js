var ral = ral || {};
ral.time = ral.time || {};

/**
 * Present an interface to the user to display the selected time.  This class should listen to a TimeSelector.
 * @constructor
 *
 * @param {Object} options The options to use to configure the TimeLabel
 * @param {string} options.target (required) The target div ID to use for the UI components - For default view, DIV must contain IMG elements with IDs: step_back, play_pause, and step_forward; AND additional inner DIV with ID: speed-control
 * @param {string} options.dateFormat (required) The date format for the label (see https://github.com/mbostock/d3/wiki/Time-Formatting for details)
 * @param {date} options.currentTime (optional) The currently selected time, defaults to 1970-JAN-01 00:00:00 UTC
 * @param {date} options.fontFamily (optional) The font family for the label (can also change in TimeLabel.css)
 * @param {date} options.fontSize (optional) The font size for the label (can also change in TimeLabel.css)
 * @param {date} options.fontColor (optional) The font color for the label (can also change in TimeLabel.css)
 * @param {boolean} options.useUTCTime (optional) If true, display in UTC time instead of the client localtime (default is true)
 */
ral.time.TimeLabel = function( options )
{
  /* get required parameters */
  this.target     = options.target;
  this.fontFamily = options.fontFamily;
  this.fontSize   = options.fontSize;
  this.fontColor  = options.fontColor;
  this.dateFormat = options.dateFormat;

  /* set default values for optional parameters */
  this.currentTime = new Date( 0 );
  this.useUTCTime = true;

  /* set values for optional parameters if provided */
  if( typeof( options.currentTime ) !== "undefined" ) this.currentTime = options.currentTime;
  if( typeof( options.useUTCTime ) !== "undefined" ) this.useUTCTime = options.useUTCTime;

  /* initialize the view */
  this.initView();

  /* set the initial time */
  this.frameChanged( this.currentTime );
};


/**
 * Empty out the div element
 * @private
 */
ral.time.TimeLabel.prototype.initView = function()
{
  jQuery( "#" + this.target ).empty();

  jQuery( "#" + this.target )
    .addClass( "TimeLabel" );

  if( typeof( this.fontFamily ) !== "undefined" )
    jQuery( "#" + this.target )
      .css( "font-family", this.fontFamily );

  if( typeof( this.fontSize ) !== "undefined" )
    jQuery( "#" + this.target )
      .css( "font-size", this.fontSize )

  if( typeof( this.fontColor ) !== "undefined" )
    jQuery( "#" + this.target )
      .css( "color", this.fontColor );
};


/**
 * Update the text in the target div - also allows this class to be a listener to a TimeSelector
 * @public
 *
 * @param {date} selectedTime the new time to display
 */
ral.time.TimeLabel.prototype.frameChanged = function( selectedTime )
{
  var timeStr = "";
  if ( this.useUTCTime === true )
  {
    timeStr = d3.time.format.utc( this.dateFormat )( selectedTime );
  }
  else
  {
    timeStr = d3.time.format( this.dateFormat )( selectedTime );
  }

  jQuery( "#" + this.target ).text( timeStr );
};

