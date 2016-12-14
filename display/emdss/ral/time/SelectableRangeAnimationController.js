var ral = ral || {};
ral.time = ral.time || {};

/**
 * Adds a pulldown menu for user-selectable time steps, and a range slider for adjusting the desired range of the
 * animation. Requires jquery-ui plugin jQDateRangeSlider http://ghusse.github.io/jQRangeSlider/documentation.html
 * Also requires a RangeIntervalTimeSequence model
 *
 * @param options See BasicAnimationController for additional options.
 * @param {[{name, numMs}]} options.steps (required) A list of kvps with the label and amount of each selectable step
 * @param {number} options.defaultStepMs (required) The default step size from the list, in milliseconds.
 * @param {Number} options.defaultMinSecs (optional) The absolute minimum number of seconds relative to selected time that the range can be set to
 * @param {Number} options.defaultMaxSecs (optional) The absolute maximum number of seconds relative to selected time that the range can be set to
 * @param {Number} options.defaultRangeMinSecs (optional) The minimum seconds after current that we set our time sequence to when the user chooses a new time
 * @param {Number} options.defaultRangeMaxSecs (optional) The maximum seconds before current that we set our time sequence to when the user chooses a new time
 * @param {[Layer]} options.layers (optional) The animator will wait for all visible Layers in this list to complete loading before advancing
 * @extends ral.time.BasicAnimationController
 * @constructor
 */
ral.time.SelectableRangeAnimationController = function( options )
{
  this.steps = options['steps'];
  this.defaultStepMs = options['defaultStepMs'];

  this.defaultMinSecs = 36 * 3600;
  this.defaultMaxSecs = 36 * 3600;
  this.defaultRangeMinSecs = 0;
  this.defaultRangeMaxSecs = 24 * 3600;
  this.layers = [];
  this.currentMin = null;
  this.currentMax = null;

  if ( typeof(options['defaultMinSecs']) !== "undefined" ) { this.defaultMinSecs = options['defaultMinSecs']; }
  if ( typeof(options['defaultMinSecs']) !== "undefined" ) { this.defaultMaxSecs = options['defaultMinSecs']; }
  if ( typeof(options['defaultRangeMinSecs']) !== "undefined" ) { this.defaultRangeMinSecs = options['defaultRangeMinSecs']; }
  if ( typeof(options['defaultRangeMaxSecs']) !== "undefined" ) { this.defaultRangeMaxSecs = options['defaultRangeMaxSecs']; }
  if ( typeof(options['layers']) !== "undefined" ) { this.layers = options['layers']; }

  /* call the super constructor */
  ral.time.BasicAnimationController.call( this, options );
}

/**
 * Inherit from the BasicAnimationController class
 */
ral.time.SelectableRangeAnimationController.prototype = Object.create( ral.time.BasicAnimationController.prototype );

/**
 * Add components to
 * @private
 */
ral.time.SelectableRangeAnimationController.prototype.initView = function()
{
  ral.time.BasicAnimationController.prototype.initView.call( this );

  var options = [];
  for ( var i = 0; i < this.steps.length; i++ ) {
    var step = this.steps[ i ];
    var opt = '<option value="' + step['numMs'] + '">' + step['name'] + "</option>"
    options.push( opt );
  }

  // Step size select menu
  jQuery( "<select/>" )
    .attr( "id", this.target + "_step" )
    .attr( "title" , "Interval step size")
    .addClass( "AnimationController_select")
    .on( "change", this.setStep.bind( this ) )
    .html( options.join( '' ) )
    .val( this.defaultStepMs )
    .appendTo( "#" + this.target );

  // Adjustable range slider
  jQuery( "#" + this.target ).append("<div id=\"" + this.target + "_rangeSlider\"></div>");
  jQuery( "#" + this.target + "_rangeSlider" ).dateRangeSlider(
    {
      formatter: function(val) {
        var month = ( val.getUTCMonth() < 9 ) ? "0" + (val.getUTCMonth() + 1 ) : (val.getUTCMonth() + 1);
        var date = ( val.getUTCDate() < 10 ) ? "0" + val.getUTCDate() : val.getUTCDate();
        var hour = ( val.getUTCHours() < 10 ) ? "0" + val.getUTCHours() : val.getUTCHours();
        var minute = ( val.getUTCMinutes() < 10 ) ? "0" + val.getUTCMinutes() : val.getUTCMinutes();
        var timeStr = month + "/" + date + " " + hour + ":" + minute;
        return timeStr;
      },
      arrows: false,
      defaultValues: { min: new Date(), max: new Date() }
    });

  jQuery( "#" + this.target + "_rangeSlider").bind( "valuesChanged", function (e, data) {
    this.timeSequence.setTimeRange( data.values.min, data.values.max );
  }.bind( this ) );

  // current time marker
  this.currentPos = jQuery( "<div>^</div>" )
      .attr( "id", this.target + "_current" )
      .addClass("AnimationController_current")
      .css( "float", "left" )
      .appendTo( "#" + this.target );
}

/**
 * Set the interval step size based on the selected value.
 * @private
 */
ral.time.SelectableRangeAnimationController.prototype.setStep = function( )
{
  var value = jQuery( "#" + this.target + "_step" ).val();
  this.timeSequence.setIntervalMs( parseInt(value) );
}

/**
 * If the selected time changed, update our slider with the new range
 * @param {date} date the new selected time
 */
ral.time.SelectableRangeAnimationController.prototype.userSelectedTimeChanged = function( date )
{
  this.currentMin = new Date( date.getTime() - ( this.defaultMinSecs * 1000 ) );
  this.currentMax = new Date( date.getTime() + ( this.defaultMaxSecs * 1000 ) );
  var min = new Date( date.getTime() - ( this.defaultRangeMinSecs * 1000 ) );
  var max = new Date( date.getTime() + ( this.defaultRangeMaxSecs * 1000 ) );

  jQuery( "#" + this.target + "_rangeSlider" ).dateRangeSlider("bounds",  this.currentMin, this.currentMax );
  jQuery( "#" + this.target + "_rangeSlider" ).dateRangeSlider("values", min, max );

  this.drawCurrent();
}

/**
 * Call the appropriate step method on the model and repeat while in the playing state
 * @private
 */
ral.time.SelectableRangeAnimationController.prototype.play = function()
{
  if ( this.timeSequence.getIsPlaying() )
  {
    // make sure all layers have finished loading
    var isLoading = false;

    for (var i=0;i<this.layers.length;i++)
    {
      var l = this.layers[i];
      if (typeof(l.getStatus) !== "undefined") {
        if ( l.getStatus() == ral.gis.layer.Layer.STATUS_LOADING)
        {
          isLoading = true;
          break;
        }
      }
    }

    if ( !isLoading )
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
    else
    {
      if ( !this.getUsesExternalPlayTimer() ) {
        setTimeout( this.play.bind( this ), 10 );
      }
    }
  }
};

ral.time.SelectableRangeAnimationController.prototype.drawCurrent = function()
{
  if ( this.currentMin === null )
  {
    return;
  }
  var width = parseInt( jQuery( "#" + this.target + "_rangeSlider" ).css( "width" ) );
  var rangeMs = this.currentMax.getTime() - this.currentMin.getTime();
  var currentPos = width * ( this.timeSequence.getCurrentFrame().getTime() - this.currentMin.getTime() ) / rangeMs;

  this.currentPos.css("margin-left", ( currentPos - 8 ) + "px" );

}

ral.time.SelectableRangeAnimationController.prototype.frameChanged = function()
{
  this.drawCurrent();
}
