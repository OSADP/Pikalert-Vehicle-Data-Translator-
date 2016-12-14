var ral = ral || {};
ral.time = ral.time || {};

/**
 * Implementation of a specific Calendar-based view of a TimeSelector using the datetimepicker JQueryUI extension.
 * @constructor
 * @extends TimeSelector
 *
 * @param {Object} options - The options to use to configure the CalendarTimeSelector (See ral.time.TimeSelector for additional parameters)
 * @param {number} options.interval - The interval (in minutes) between selectable times (default = 60)
 * @param {boolean} options.useUtc - Use UTC time instead of client local time (default = false);
 */
ral.time.CalendarTimeSelector = function( options )
{
  /* call the super constructor */
  ral.time.TimeSelector.call( this, options );

  /* set default values for optional parameters */
  this.interval = 60;
  this.useUtc = false;

  /* set optional parameters that were provided */
  if( options.interval !== "undefined" ) this.interval = options.interval;

  if( options.useUtc !== "undefined") this.useUtc = options.useUtc;

  this.listeners = [];

  /* initialize the view */
  this.initView();
};


/**
 * Inherit from the TimeSelector class
 */
ral.time.CalendarTimeSelector.prototype = Object.create( ral.time.TimeSelector.prototype );


/**
 * Create the date/time picker
 * @private
 */
ral.time.CalendarTimeSelector.prototype.initView = function()
{
  var date = new Date( this.getSelectedTime().getTime() + 1 );
  if ( this.useUtc === true )
  {
    var offset = date.getTimezoneOffset() * 60 * 1000;
    date = new Date( this.getSelectedTime().getTime() + offset );
  }

  this.datetimepicker =
    jQuery( "#" + this.target )
      .datetimepicker(
        {
          format: "d.m.Y H:i",
          inline: true,
          step: this.interval,
          lang: "en",
          onChangeDateTime: this.calendarTimeChange.bind( this ),
          value: date
        }
      );
};


/**
 * Update the view with the currently selected time
 * @private
 */
ral.time.CalendarTimeSelector.prototype.updateView = function()
{
  console.log( "ral.time.CalendarTimeSelector is not capable of setting the date in the view." );
};


/**
 * Listener function to handle when the time was changed through the UI
 * @private
 *
 * @param {date} date The date object with the new time
 * @param {object} ui [not used]
 */
ral.time.CalendarTimeSelector.prototype.calendarTimeChange = function( date, ui )
{
  if ( this.useUtc === true )
  {
    var offset = date.getTimezoneOffset() * 60 * 1000;
    date = new Date( date.getTime() - offset );
  }
  this.setSelectedTime( date );
  this.fireUserSelectedTimeChanged( date );
};

/**
 * Add a listener to this widget
 * @public
 *
 * @param {object} listener The listener to add
 */
ral.time.CalendarTimeSelector.prototype.addListener = function( listener )
{
  if( typeof( this.listeners ) === "undefined" )
    this.listeners = [];

  this.listeners[ this.listeners.length ] = listener;
}


/**
 * Remove a listener if it is found
 * @public
 *
 * @param {object} listener The listener object to remove
 */
ral.time.CalendarTimeSelector.prototype.removeListener = function( listener )
{
  if( typeof( this.listeners ) === "undefined" )
    return;

  /* find the listener */
  for( var i = 0; i < this.listeners.length; i++ )
  {
    if( listener == this.listeners[i] )
    {
      /* remove the listener */
      this.listeners.splice( i, 1 );
    }
  }
}

/**
 * Remove all listeners
 * @public
 */
ral.time.CalendarTimeSelector.prototype.removeAllListeners = function()
{
  this.listeners = [];
}

/**
 * Notify listeners that a new time was selected by the calendar widget
 * @param {date} date The date object with the new time
 * @private
 */
ral.time.CalendarTimeSelector.prototype.fireUserSelectedTimeChanged = function( date )
{
  /* call all of the listeners that implement the function: 'userSelectedTimeChanged' */
  for( var i = 0; i < this.listeners.length; i++ )
  {
    if( typeof( this.listeners[i].userSelectedTimeChanged ) != "function" )
      continue;

    this.listeners[i].userSelectedTimeChanged( date );
  }
}
