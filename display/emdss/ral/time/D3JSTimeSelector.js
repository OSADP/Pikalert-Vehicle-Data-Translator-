var ral = ral || {};
ral.time = ral.time || {};

/**
 * Implementation of a specific timeline view of a TimeSelector using the D3JS library.
 * @constructor
 * @extends TimeSelector

 * @param {Object} options - The options to use to configure the D3JSTimeSelector -- See ral.time.TimeSelector for additional parameters
 * @param {number} options.markerHeight (required) The height of the selection indicator in the UI
 * @param {number} options.markerWidth (required) The width of the selection indicator in the UI
 * @param {number} options.tickMarks (required) The desired number of tick marks on the timeline
 * @param {string} options.dateFormat (requried) How to display the date (see https://github.com/mbostock/d3/wiki/Time-Formatting for details)
 * @param {number} options.tickFormat (optional) Can be a formatting function for tick mark labels
 * @param {string} options.fontFamily (optional) Can also be set with CSS (see D3JSTimeSelector.css)
 * @param {string} options.fontSize (optional) Can also be set with CSS (see D3JSTimeSelector.css)
 * @param {string} options.fontColor (optional) Can also be set with CSS (see D3JSTimeSelector.css)
 * @param {string} options.selectorColor (optional) Can also be set with CSS (see D3JSTimeSelector.css)
 * @param {string} options.padding (optional) Can also be set with CSS (see D3JSTimeSelector.css)
 */
ral.time.D3JSTimeSelector = function( options )
{
  /* call the super constructor */
  ral.time.TimeSelector.call( this, options );

  /* get the vales for the required parameters */
  this.markerHeight = options.markerHeight;
  this.markerWidth  = options.markerWidth;
  this.tickMarks    = options.tickMarks;
  this.dateFormat   = options.dateFormat;

  /* set default values for optional parameters */
  this.tickFormat    = d3.time.format( this.dateFormat );
  this.fontFamily    = "avenir";
  this.fontSize      = "10pt";
  this.fontColor     = "#333333";
  this.selectorColor = "orange";
  this.padding       = 10;

  /* replace defaults with any values provided */
  if( typeof( options.tickFormat ) !== "undefined" ) this.tickFormat = options.tickFormat;
  if( typeof( options.fontFamily ) !== "undefined" ) this.fontFamily = options.fontFamily;
  if( typeof( options.fontSize ) !== "undefined" ) this.fontSize = options.fontSize;
  if( typeof( options.fontColor ) !== "undefined" ) this.fontColor = options.fontColor;
  if( typeof( options.selectorColor ) !== "undefined" ) this.selectorColor = options.selectorColor;
  if( typeof( options.padding ) !== "undefined" ) this.padding = options.padding;
  /* create the view */
  this.initView();
};


/**
 * Inherit from the TimeSelector class
 */
ral.time.D3JSTimeSelector.prototype = Object.create( ral.time.TimeSelector.prototype );


/**
 * A configure gear icon
 */
ral.time.D3JSTimeSelector.CONFIGURE_ICON = "data:image/png;base64,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";

/**
 * Set the desired number of tick marks on the timeline
 * @param tickMarks
 */
ral.time.D3JSTimeSelector.prototype.setTickMarks = function( tickMarks )
{
  this.tickMarks = tickMarks;
}

/**
 * Get the desired number of tick marks on the timeline
 */
ral.time.D3JSTimeSelector.prototype.getTickMarks = function( )
{
  return this.tickMarks;
}

/**
 * Draw the initial time slider
 */
ral.time.D3JSTimeSelector.prototype.initView = function()
{
  /* get the min/max time */
  this.minTime = this.timeSequence.getFirstFrame();
  this.maxTime = this.timeSequence.getLastFrame();

  /* clear the plot */
  this.clearPlot();

  var div     = d3.select( "#" + this.target ).classed( "D3JSTimeSelector", true );
  var width   = d3.select( "#" + this.target )[0][0].clientWidth - this.padding - this.padding;
  var height  = d3.select( "#" + this.target )[0][0].clientHeight - this.padding - this.padding;

  var parseDate     = d3.time.format( this.dateFormat ).parse;
  var formatDate    = d3.time.format( this.dateFormat );
  var timeLineWidth = width - this.padding - this.padding;

  this.timeScale = d3.time.scale()
      .range( [ 0, timeLineWidth ] )
      .domain( [ this.minTime, this.maxTime ] );

  this.timeAxis = d3.svg.axis()
      .scale( this.timeScale )
      .ticks( this.tickMarks )
      .tickFormat( this.tickFormat )
      .orient( "bottom" );

  /* clean up */
  this.clearPlot();

  this.timeLineTranslateX = ( ( (width-timeLineWidth+this.padding+this.padding) / 2 ) );
  this.timeLineTranslateY = this.markerHeight;

  this.timeLineSVG = div.append( "svg" )
    .attr( "width", width )
    .attr( "height", height )
    .on( "click", this.mouseClicked.bind( this ) );

  this.timeLineSVG
    .append( "g" )
      .attr( "transform", "translate( " + this.timeLineTranslateX + "," + (this.timeLineTranslateY+3) + " )" )
      .append( "g" )
        .attr( "class", "x axis" )
        .style( "font-size", this.fontSize )
        .style( "fill", this.fontColor )
        .style( "font-family", this.fontFamily )
        .call( this.timeAxis );

  this.timeMarker = this.timeLineSVG.append( "rect" )
    .attr( "width", this.markerWidth )
    .attr( "height", this.markerHeight )
    .attr( "transform", "translate( " + ( this.timeLineTranslateX - (this.markerWidth/2) )  + "," + ( this.timeLineTranslateY ) + " )" )
    .style( "fill", this.selectorColor );

  if( this.userConfigurable )
  {
    this.configurator = new ral.time.TimeSelectorConfiguration( {
      timeSelector: this,
      userConfigurable: this.userConfigurable} );

    jQuery( "<img>" )
      .attr( "src", ral.time.D3JSTimeSelector.CONFIGURE_ICON )
      .css( "position", "absolute" )
      .css( "left", "95%" )
      .css( "top", "60%" )
      .click( this.configurator.openUserConfiguration.bind( this.configurator ) )
      .appendTo( "#" + this.target );
  }

  this.drawRangeBars();

  this.updateView();
};


/**
 * Event handler for a mouse click on the timeline SVG element
 * @private
 */
ral.time.D3JSTimeSelector.prototype.mouseClicked = function()
{
  /* get the mouse click position relative to the time line SVG element (immediate container) */
  var mouse = d3.mouse( this.timeLineSVG[0][0] );

  /* calculate the edge of the time line with the far left being zero */
  //TODO: Why is 7 a magic number?
  var xEdge = mouse[0] - (this.markerWidth/2) - 1 - this.padding - 7;

  /* get the selected time */
  var selectedTime = this.timeScale.invert( xEdge );
  this.setSelectedTime( selectedTime );
};


/**
 * Move the time selector to match the selected time
 * @private
 */
ral.time.D3JSTimeSelector.prototype.updateView = function()
{
  /* move the selector box to this x,y */ //TODO: Why is 7 a magic number?
  var x = this.timeScale( this.timeSequence.getCurrentFrame() ) + this.padding + 7;
  var y = this.timeLineTranslateY - this.markerHeight;
  this.timeMarker.attr( "transform", "translate( " + x + "," + y + ")" );
};


/**
 * Remove everything from the slider div
 * @private
 */
ral.time.D3JSTimeSelector.prototype.clearPlot = function()
{
  d3.select( "#" + this.target )
    .selectAll( "*" )
    .remove();

  d3.select( "#"+this.target )
    .selectAll( "svg" )
    .data( [] )
      .exit()
      .remove();
};


ral.time.D3JSTimeSelector.prototype.addRangeBar = function( start, end, color )
{
  if( typeof( this.rangeBars ) === "undefined" )
    this.rangeBars = [];

  this.rangeBars[ this.rangeBars.length ] = [ start, end, color ];
  this.drawRangeBar( start, end, color );
};


ral.time.D3JSTimeSelector.prototype.drawRangeBars = function()
{
  if( typeof( this.rangeBars ) === "undefined" )
    return;

  for( var i = 0; i < this.rangeBars.length; i++ )
  {
    this.drawRangeBar( this.rangeBars[i][0], this.rangeBars[i][1], this.rangeBars[i][2], i );
  }
};


ral.time.D3JSTimeSelector.prototype.drawRangeBar = function( start, end, color, index )
{
  var x1 = this.timeScale( start ) + 20;
  var x2 = this.timeScale( end ) + 20;
  var height = 3;

  this.rangeBarAtmos = this.timeLineSVG.append( "rect" )
    .attr( "width", x2-x1 )
    .attr( "height", 3 )
    .attr( "transform", "translate( " + x1  + "," + (index*3) + " )" )
    .style( "fill", color );
};


ral.time.D3JSTimeSelector.prototype.domainChanged = function( domain )
{
  this.domain = domain.id;
  this.updateRangeBars();
};


ral.time.D3JSTimeSelector.prototype.rcpChanged = function( rcp )
{
  this.rcp = rcp.id;
  this.updateRangeBars();
};

ral.time.D3JSTimeSelector.prototype.anomalyChanged = function( anom )
{
  this.anomaly = anom.state;
  this.updateRangeBars();
};

ral.time.D3JSTimeSelector.prototype.updateRangeBars = function()
{
  var oceanColor = "#ccccff";
  var skyColor   = "#88ff88";

  if( this.anomaly )
  {
    if( this.rcp == "20THC" && ( this.domain == "d01" || this.domain == "d02" ) )
      this.rangeBars = [[Date.parse( "1986-01-01T00:00:00+0000"), Date.parse( "2005-12-31T23:59:59+0000"), skyColor ],[Date.parse( "2000-01-01T00:00:00+0000"), Date.parse( "2019-12-31T23:59:59+0000"), oceanColor ] ];
    else if( this.rcp == "20THC" && this.domain == "d03" )
      this.rangeBars = [ [Date.parse( "2000-01-01T00:00:00+0000"), Date.parse( "2019-12-31T23:59:59+0000"), oceanColor ] ];
    else if( ( this.rcp == "RCP45" || this.rcp == "RCP85" ) && ( this.domain == "d01" || this.domain == "d02" ) )
      this.rangeBars = [ [Date.parse( "2060-01-01T00:00:00+0000"), Date.parse( "2079-12-31T23:59:59+0000"), skyColor ],[Date.parse( "2080-01-01T00:00:00+0000"), Date.parse( "2099-12-31T23:59:59+0000"), oceanColor ] ];
    else if( this.domain == "d03" && ( this.rcp == "RCP45" || this.rcp == "RCP85" ) )
      this.rangeBars = [ [Date.parse( "2080-01-01T00:00:00+0000"), Date.parse( "2099-12-31T23:59:59+0000"), oceanColor ] ];
    else
      this.rangeBars = [];
  }
  else
  {
    if( this.rcp == "20THC" && ( this.domain == "d01" || this.domain == "d02" ) )
      this.rangeBars = [[Date.parse( "1986-01-01T00:00:00+0000"), Date.parse( "2005-12-31T23:59:59+0000"), skyColor ],[Date.parse( "2000-01-01T00:00:00+0000"), Date.parse( "2019-12-31T23:59:59+0000"), oceanColor ] ];
    else if( this.rcp == "20THC" && this.domain == "d03" )
      this.rangeBars = [ [Date.parse( "1990-01-01T00:00:00+0000"), Date.parse( "1999-12-31T23:59:59+0000"), skyColor ],[Date.parse( "2000-01-01T00:00:00+0000"), Date.parse( "2019-12-31T23:59:59+0000"), oceanColor ] ];
    else if( ( this.rcp == "RCP45" || this.rcp == "RCP85" ) && ( this.domain == "d01" || this.domain == "d02" ) )
      this.rangeBars = [ [Date.parse( "2060-01-01T00:00:00+0000"), Date.parse( "2079-12-31T23:59:59+0000"), skyColor ],[Date.parse( "2080-01-01T00:00:00+0000"), Date.parse( "2099-12-31T23:59:59+0000"), oceanColor ] ];
    else if( this.domain == "d03" && ( this.rcp == "RCP45" || this.rcp == "RCP85" ) )
      this.rangeBars = [ [Date.parse( "2080-01-01T00:00:00+0000"), Date.parse( "2099-12-31T23:59:59+0000"), oceanColor ] ];
    else
      this.rangeBars = [];
  }
  this.initView();
};