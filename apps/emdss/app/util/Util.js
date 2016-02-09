/**
 * Class: Util A catch-all for utility functions
 */

// getting a reference to the main namespace if it exists - empty object
// otherwise
emdss = emdss || {};
emdss.util = emdss.util || {};
emdss.util.Util = {

	/**
	 * Checks whether the given variable is defined (not 'undefined') and, if
	 * given, performs the operation on the variable's value prior to returning
	 * the result.
	 * 
	 * @param {}
	 *            variable
	 * @param {}
	 *            operation
	 * @return {}
	 */
	getPrintableString : function(variable, operation) {
		if (typeof variable != 'undefined') {
			if (typeof operation == 'function') {
				return operation(variable);
			} else {
				return variable;
			}
		} else {
			return 'n/a';
		}
	},
		
    dateToBriefStringZulu : function(date) {
        return date.getUTCHours() + ':' + (date.getMinutes() < 10 ? '0' + date.getMinutes() : date.getMinutes()) + 'Z'
         + '&nbsp;on&nbsp;' + (date.getUTCMonth() + 1) + '/' + date.getUTCDate();
    },
    
    dateToBriefString : function(date) {
        return date.getHours() + ':' + (date.getMinutes() < 10 ? '0' + date.getMinutes() : date.getMinutes())
         + '&nbsp;on&nbsp;' + (date.getMonth() + 1) + '/' + date.getDate();
    },
    
    dateToBriefStringWithSeconds : function(date) {
        return date.getHours() 
        	+ ':' + (date.getMinutes() < 10 ? '0' + date.getMinutes() : date.getMinutes())
        	+ ':' + (date.getSeconds() < 10 ? '0' + date.getSeconds() : date.getSeconds())
         	+ '&nbsp;on&nbsp;' + (date.getMonth() + 1) + '/' + date.getDate();
    },
    
    dateToBriefStringWithSecondsNoNBSP : function(date) {
        return date.getHours() 
        	+ ':' + (date.getMinutes() < 10 ? '0' + date.getMinutes() : date.getMinutes())
        	+ ':' + (date.getSeconds() < 10 ? '0' + date.getSeconds() : date.getSeconds())
         	+ ' on ' + (date.getMonth() + 1) + '/' + date.getDate();
    },
    
    dateToEmdssString : function(date) {
		var mon = date.getUTCMonth() + 1;
		var day = date.getUTCDate();
		var hour = date.getUTCHours();
		var min = date.getUTCMinutes();
		var sec = date.getUTCSeconds();
		return '' + date.getUTCFullYear() + ((mon < 10) ? "0" + mon : mon)
				+ ((day < 10) ? "0" + day : day)
				+ ((hour < 10) ? "0" + hour : hour)
				+ ((min < 10) ? "0" + min : min);
	},
	
	emdssStringToDate : function(emdssStr) {
		// zero-fills caused problems with Chrome, even when I used parseInt(str, 10)...
		var y = parseInt( emdssStr.substring(0, 4), 10 );
		var m = parseInt( emdssStr.substring(4, 6), 10 );
		var d = parseInt( emdssStr.substring(6, 8), 10 );
		var h = parseInt( emdssStr.substring(8, 10), 10 );
		var min = parseInt( emdssStr.substring(10, 12), 10 );
		// var dt = Ext.Date.parse(emdssStr, "Ymdgi");
		var dt = new Date( Date.UTC(y, (m-1), d, h, min, 0, 0) );
		dt.set
		return dt;
		// date = new Date();
		// date.setUTCFullYear(arr[0], arr[1] - 1, arr[2]);
		// date.setUTCHours(arr[3], arr[4], arr[5]);
		// return date;
	},
	
	emdssStringWithSecondsToDate : function(emdssStr) {
		// zero-fills caused problems with Chrome, even when I used parseInt(str, 10)...
		var y = parseInt( emdssStr.substring(0, 4), 10 );
		var m = parseInt( emdssStr.substring(4, 6), 10 );
		var d = parseInt( emdssStr.substring(6, 8), 10 );
		var h = parseInt( emdssStr.substring(8, 10), 10 );
		var min = parseInt( emdssStr.substring(10, 12), 10 );
		var sec = parseInt( emdssStr.substring(12, 14), 10 );
		// var dt = Ext.Date.parse(emdssStr, "Ymdgi");
		var dt = new Date( Date.UTC(y, (m-1), d, h, min, sec, 0) );
		dt.set
		return dt;
		// date = new Date();
		// date.setUTCFullYear(arr[0], arr[1] - 1, arr[2]);
		// date.setUTCHours(arr[3], arr[4], arr[5]);
		// return date;
	},
	
	addHoursToDate : function(dt, hours) {
		var ms = dt.getTime();
		var newMs = ms + (hours * 60 * 60 * 1000);
		var newDate = new Date(newMs);
		return newDate;
	},
	
	/**
	 * Converts a javascript Date object to an ISO 8601 string
	 * 
	 * @param {Object}
	 *            date
	 */
	dateToIsoString : function(date) {
		var mon = date.getUTCMonth() + 1;
		var day = date.getUTCDate();
		var hour = date.getUTCHours();
		var min = date.getUTCMinutes();
		var sec = date.getUTCSeconds();
		return date.getUTCFullYear() + "-" + ((mon < 10) ? "0" + mon : mon)
				+ "-" + ((day < 10) ? "0" + day : day) + "T"
				+ ((hour < 10) ? "0" + hour : hour) + ":"
				+ ((min < 10) ? "0" + min : min) + ":"
				+ +((sec < 10) ? "0" + sec : sec) + ".000Z";
	},
	
    dateTimeToIsoString : function(date, timeString) {
        var mon = date.getUTCMonth() + 1;
        var day = date.getUTCDate();
        return date.getUTCFullYear() + "-" + ((mon < 10) ? "0" + mon : mon)
                + "-" + ((day < 10) ? "0" + day : day) + "T"
                + timeString;           
    },
    
	/**
	 * Converts a javascript Date object to an ISO string required by dataserver
	 * (zero-filled seconds with no decimal point)
	 * 
	 * @param {Object}
	 *            date
	 */
	dateToDataserverIsoString : function(date) {
		var mon = date.getUTCMonth() + 1;
		var day = date.getUTCDate();
		var hour = date.getUTCHours();
		var min = date.getUTCMinutes();
		var sec = date.getUTCSeconds();
		return date.getUTCFullYear() + "-" + ((mon < 10) ? "0" + mon : mon)
				+ "-" + ((day < 10) ? "0" + day : day) + "T"
				+ ((hour < 10) ? "0" + hour : hour) + ":"
				+ ((min < 10) ? "0" + min : min) + ":"
				+ ((sec < 10) ? "0" + sec : sec) + "Z";
	},

	/**
	 * Creates a Date object from an ISO 8601 string ("2012-01-01T00:00:00Z")
	 */
	isoStringToDate : function(iso8601Str) {
		var arr = iso8601Str.split(/[- :TZ]/);
		date = new Date();
		date.setUTCFullYear(arr[0], arr[1] - 1, arr[2]);
		date.setUTCHours(arr[3], arr[4], arr[5]);
		return date;
	},
    
    /**
     * Gets a date object which represents the time closest to the given date,
     * which is also before than the given date, or null, if none is found. 
     * The returned date is selected from the given array of ISO8601 time strings.
     * Note: This assumes that the times are for the same day as the given date.
     * @param {} date
     * @param {} dateArray
     */
    getNearestDateFromTimes : function(date, dateArray) {
      var prevTestDate, nextTestDate;
      for( var i=0; i<dateArray.length; i++) {
        nextTestDate = this.isoStringToDate( this.dateTimeToIsoString( date, dateArray[i] ) );
        if( nextTestDate>date ) {
            return prevTestDate;
        } else {
            prevTestDate = nextTestDate;
        }
      }
      return prevTestDate;
    },
    
    /**
     * Gets the value from the given array of values which is closest to the 
     * given value. Note: This assumes that the values are monotonically increasing or decreasing.
     * @param {} value
     * @param {} valuesArray
     */
    getNearestValue : function(value, valueArray) {
        var prevTestDelta = Number.MAX_VALUE, nextTestDelta;
      for( var i=0; i<valueArray.length; i++) {
        nextTestDelta = Math.abs(value-valueArray[i] );
        if( nextTestDelta < prevTestDelta ) {
            prevTestDelta = nextTestDelta;
        } else {
            return valueArray[i-1];
        }
      }
      return valueArray[valueArray.length-1];
    },
    
    getDateOffsetRounded15 : function(baseDateMillis, offsetMillis) {
        var targetDate = new Date( baseDateMillis + offsetMillis);
        targetDate.setUTCMilliseconds(0);
        targetDate.setUTCSeconds(0);
        targetDate.setUTCMinutes( (parseInt((targetDate.getUTCMinutes() + 7.5)/15) * 15) % 60 );
        targetDate.setUTCHours( targetDate.getUTCMinutes() > 52 ? ++targetDate.getUTCHours() : targetDate.getUTCHours() );
        return targetDate;
    },

    /**
     * Converts the given pressure in Pascals to height in meters in a standard atmosphere
     * @param {} presPa
     * @return {}
     */
    presPa2stdAtmosM : function(presPa) {
     return 44307.692 * (1.0 - Math.pow(presPa*0.01/1013.25, 0.190) );
    },
    
    /**
     * Converts the given height in meters into pressure in a standard atmosphere in Pascals
     * @param {} heightM
     * @return {}
     */
    stdAtmosM2presPa : function(heightM) {
     return Math.exp( Math.log(1.0-heightM/44307.692) / 0.19) * 101325.0;
    }
}
