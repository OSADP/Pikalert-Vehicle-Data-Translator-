var ral = ral || {};
ral.time = ral.time || {};

/**
 * Implementation of a specific timeline view of a TimeSelector using the D3JS library.
 * @constructor
 *
 * @param options TODO:
 */
ral.time.TimeSelectorConfiguration = function( options )
{
  this.timeSelector = options.timeSelector;
  this.configModes = "realtime,fixed,event"; // Default to all modes

  /* replace defaults with any values provided */
  if( typeof( options.userConfigurable ) === "string"
          && options.userConfigurable.toLowerCase().trim() != 'true' ) {
    this.configModes = options.userConfigurable.toLowerCase().trim();
  }
};


ral.time.TimeSelectorConfiguration.prototype.openUserConfiguration = function()
{
  this.dialog = jQuery( "<div>" ).dialog(
    {
      autoOpen: true,
      height: 500,
      width: 700,
      modal: true,
      title: "Time Selector Configuration",
      buttons:
      {
        "OK": this.closeUserConfiguration.bind( this ),
        Cancel: function() { this.dialog.dialog( "close" ); this.dialog.remove(); }.bind( this )
      },
      close: function() { }
    }
  );

  if( this.configModes.indexOf("realtime") != -1 ) {
    jQuery( "<input>" )
            .addClass( "D3JSTimeSelector-RadioButton" )
            .attr( "id", "D3JSTimeSelector-RadioButton-realtime" )
            .attr( "type", "radio" )
            .attr( "name", "mode" )
            .attr( "value", "realtime" )
            .appendTo( this.dialog );
    jQuery( "<text>" )
            .text( "Real-time" )
            .appendTo( this.dialog );
  }

  if( this.configModes.indexOf("fixed") != -1 ) {
    jQuery( "<input>" )
            .addClass( "D3JSTimeSelector-RadioButton" )
            .attr( "id", "D3JSTimeSelector-RadioButton-fixed" )
            .attr( "type", "radio" )
            .attr( "name", "mode" )
            .attr( "value", "fixed" )
            .appendTo( this.dialog );
    jQuery( "<text>" )
            .text( "Fixed Period" )
            .appendTo( this.dialog );
  }

  if( this.configModes.indexOf("event") != -1 ) {
    jQuery( "<input>" )
            .addClass( "D3JSTimeSelector-RadioButton" )
            .attr( "id", "D3JSTimeSelector-RadioButton-event" )
            .attr( "type", "radio" )
            .attr( "name", "mode" )
            .attr( "value", "event" )
            .appendTo( this.dialog );
    jQuery( "<text>" )
            .text( "Event" )
            .appendTo( this.dialog );
  }

  jQuery( "<div>" )
    .addClass( "D3JSTimeSelector-ConfigDetails" )
    .attr( "id", "configDetails" )
    .appendTo( this.dialog );

  jQuery( "input[type=radio][name=mode]" )
    .change( this.radioChange.bind( this ) );

  if( typeof( this.mode ) === "undefined" )
  {
    var separatorIndex = this.configModes.indexOf( "," );
    this.mode = this.configModes.substring( 0, separatorIndex == -1 ? this.configModes.length : separatorIndex );
  }
  this.setMode( this.mode );
};


ral.time.TimeSelectorConfiguration.prototype.radioChange = function( event )
{
  if( typeof( event.target.value ) === "undefined" ) return;

  this.setMode( event.target.value );
};


ral.time.TimeSelectorConfiguration.prototype.setMode = function( mode )
{
  this.mode = mode;

  jQuery( "#D3JSTimeSelector-RadioButton-" + this.mode )
    .attr( "checked", "checked" );

  jQuery.ajax( "form-"+this.mode+".html", { dataType: "html" } )
    .done( this.initDetails.bind( this ) );
};


ral.time.TimeSelectorConfiguration.prototype.initDetails = function( html, success, responseObj )
{
  console.log( "loaded..." + success );

  if( success )
    jQuery( "#configDetails").html( html );

  jQuery( ".TimeSelectorConfiguration-DateChooser" )
    .datetimepicker();

  if( this.mode == "fixed" )
  {
    var startDate = this.timeSelector.getTimeSequence().getFirstFrame();
    var endDate   = this.timeSelector.getTimeSequence().getLastFrame();
    var startStr  = this.formatDate( startDate );
    var endStr    = this.formatDate( endDate );
    jQuery( "#startDate" ).val( startStr );
    jQuery( "#endDate" ).val( endStr );
  }
};


ral.time.TimeSelectorConfiguration.prototype.formatDate = function( date )
{
  var y = date.getFullYear();
  var m = ( "0" + ( date.getMonth() + 1 ) ).slice( -2 );
  var d = ( "0" + date.getDate() ).slice( -2 );
  var h = ( "0" + date.getHours() ).slice( -2 );
  var i = ( "0" + date.getMinutes() ).slice( -2 );

  return y + "/" + m + "/" + d + " " + h + ":" + i;
};


ral.time.TimeSelectorConfiguration.prototype.closeUserConfiguration = function()
{
  if( this.mode == "realtime" )
  {
    this.updateRealTimeParams();
  }
  else if( this.mode == "fixed" )
  {
    this.updateFixedParams();
  }
  else if( this.mode == "event" )
  {
    this.updateEventParams();
  }

  this.dialog.dialog( "close" );
  this.dialog.remove();
};


ral.time.TimeSelectorConfiguration.prototype.updateFixedParams = function()
{
  var startDate = new Date( Date.parse( jQuery( "#startDate").val() ) );
  var endDate   = new Date( Date.parse( jQuery( "#endDate").val() ) );

  var timeSequenceModel = new ral.time.RangeIntervalTimeSequence(
    {
      minTime: startDate,
      maxTime: endDate,
      intervalMs: 360000,
      roundMs: 360000
    }
  );
  timeSequenceModel.setCurrentFrame( startDate );

  /* transfer the listeners to the new time sequence model */
  var oldTSM = this.timeSelector.getTimeSequence();
  oldTSM.transferListenersInto( timeSequenceModel );
  this.timeSelector.setTimeSequence( timeSequenceModel );

  /* fire a new event to update all listeners */
  timeSequenceModel.setCurrentFrame( startDate );
};


ral.time.TimeSelectorConfiguration.prototype.updateRealTimeParams = function()
{
  var unitsToMs = [];
  unitsToMs[ "d" ] = 86400000;
  unitsToMs[ "h" ] = 3600000;
  unitsToMs[ "m" ] = 60000;

  var reference = [];
  reference[ "b" ] = -1;
  reference[ "a" ] = 1;

  var startValue = jQuery( "#start-value" ).val();
  var startUnits = jQuery( "#start-units" ).val();
  var startRef   = jQuery( "#start-reference" ).val();
  var startRelMs = startValue * unitsToMs[ startUnits ] * reference[ startRef ];

  var endValue = jQuery( "#end-value" ).val();
  var endUnits = jQuery( "#end-units" ).val();
  var endRef   = jQuery( "#end-reference" ).val();
  var endRelMs = endValue * unitsToMs[ endUnits ] * reference[ endRef ];

  var selectValue = jQuery( "#select-value" ).val();
  var selectUnits = jQuery( "#select-units" ).val();
  var selectRef   = jQuery( "#select-reference" ).val();
  var selectRelMs = selectValue * unitsToMs[ selectUnits ] * reference[ selectRef ];

  var refreshValue = jQuery( "#refresh-value" ).val();
  var refreshUnits = jQuery( "#refresh-units" ).val();
  var refreshMs    = refreshValue * unitsToMs[ refreshUnits ];

  /* get the current/old/about-to-be-replaced TimeSequenceModel */
  var oldTSM = this.timeSelector.getTimeSequence();

  /* create the new TimeSequenceModel */
  var timeSequenceModel = new ral.time.RealTimeRangeIntervalTimeSequence(
    {
      msBefore      : startRelMs,
      msAfter       : endRelMs,
      updateInterval: refreshMs,
      intervalMs    : oldTSM.intervalMs,
      roundMs       : oldTSM.roundMs
    }
  );

  /* transfer the listeners to the new time sequence model */
  oldTSM.transferListenersInto( timeSequenceModel );
  this.timeSelector.setTimeSequence( timeSequenceModel );

  /* set the current time */
  timeSequenceModel.setCurrentFrame( new Date( new Date().getTime() + selectRelMs ) );
};

ral.time.TimeSelectorConfiguration.prototype.updateEventParams = function()
{
  var event = $("input[type='radio'][name='event']:checked").val();
  var startDate;
  var endDate;

  /* find the selected event */
  if( event == "2013_co_flood" )
  {
    startDate = new Date( "2013-09-12T00:00:00+0000" );
    endDate   = new Date( "2013-09-15T00:00:00+0000" );
  }
  else if( event == "2015_co_spring_rain" )
  {
    startDate = new Date( "2015-05-12T00:00:00+0000" );
    endDate   = new Date( "2015-05-21T00:00:00+0000" );
  }

  /* get the current/old/about-to-be-replaced TimeSequenceModel */
  var oldTSM = this.timeSelector.getTimeSequence();

  /* create a new time sequence model with the right times */
  var timeSequenceModel = new ral.time.RangeIntervalTimeSequence(
    {
      minTime: startDate,
      maxTime: endDate,
      intervalMs: oldTSM.intervalMs,
      roundMs: oldTSM.roundMs
    }
  );

  /* transfer the listeners to the new time sequence model */
  oldTSM.transferListenersInto( timeSequenceModel );
  this.timeSelector.setTimeSequence( timeSequenceModel );

  /* set the current time */
  timeSequenceModel.setFirstFrameCurrent();
};