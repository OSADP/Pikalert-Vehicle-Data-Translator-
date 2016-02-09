/**
 * @class MobileMet.util.StationDataMgr
 * Maintains stores of data for stations related to the user's favorites and
 * the app's current flight route.
 */

Ext.define( 'emdss.util.LatestWmsMgr', {
    extend: 'Ext.util.Observable',
    requires:[
        'emdss.util.Config',
        'emdss.util.Util',
    ],
    singleton: true,
    getTimesTask: null,
    stopGetTimes: false,
    beforeLoadCallbacks: [],
    loadedCallbacks: [],
    dateAddedCallbacks: [],
    dateRemovedCallbacks: [],
    constructor: function() {

// TODO: Straighten out all this timing and asynchronous loading code!!!

        this.refreshInterval = 300; // In seconds.
        this.previousLoadTime = 0;
        this.isLoaded = false;
        this.isLoading = false;
    },

    getDates: function() {
        return this.dates;
    },

    getLatestDate: function() {
        if ( typeof this.dates == 'undefined' || this.dates.length < 1 ) {
            return null;
        }
        else {
            return this.dates[this.dates.length - 1];
        }
    },

    removeDate: function(date) {
        if ( typeof this.dates == 'undefined' ) {
            this.dates = [];
        }

        var idx = this.dates.indexOf(date);
        if ( idx > -1 ) {
            this.dates.splice(idx, 1);
            this.executeDateRemovedCallback(date);
        }
    },

    hasDate: function(date) {
        if ( typeof this.dates == 'undefined') {
            console.log("LatestWmsMgr has no times in hasDate()!!!");
            return false;
        }
        var len = this.dates.length;
        for ( var i = 0; i < len; i++ ) {
           if (this.dates[i].getTime() == date.getTime()) {
               return true;
           }
        }
        return false;
    },

    addDate: function(date) {
        if ( typeof this.dates == 'undefined' ) {
            this.dates = [];
        }

        if ( !this.hasDate(date) ) {
            this.dates.push(date);
            this.executeDateAddedCallback(date);
        }
    },

    addDates: function(datesArray) {
        if ( typeof this.dates == 'undefined' ) {
            this.dates = [];
        }
        
        var len = datesArray.length;
        for (var i = 0; i < len; i++) {
            this.addDate(datesArray[i]);
        }
    },

    /**
     * Remove any dates that fall before the passed-in date.
     */
    removeAllDatesBefore: function(referenceDate) {
        if ( typeof this.dates == 'undefined' ) {
            this.dates = [];
        }
        
        var toRemove = [];
        var len = this.dates.length;
        for (var i = 0; i < len; i++) {
            var thisDate = this.dates[i];
            if (thisDate.getTime() < referenceDate.getTime()) {
                toRemove.push(thisDate);
            }
        }
        
        var rLen = toRemove.length;
        for (var j = 0; j < rLen; j++) {
            var removeDate = toRemove[j];
            this.removeDate(removeDate);
        }
    },
    
    /**
     * Make the dates match the passed-in array, and issue appropriate callbacks for all changes.
     *   -- Assumes the passed-in dates array is ordered, with earliest data first.
     */
    updateDates: function(newDatesArray) {
        // Check for empty case.
        if (newDatesArray.length < 1) {
            console.log("Warning: LatestWmsMgr::updateDates() called with empty dates array!");
            this.clearAllDates();
            return;
        }
        
        // Get rid of irrelevant (old/stale) dates.
        var firstDate = newDatesArray[0];
        this.removeAllDatesBefore(firstDate);
        
        // Add relevant dates.
        this.addDates(newDatesArray);
    },
    
    /**
     * Do Not Use!!!
     */
    resetDates: function(newDatesArray, reload) {
        this.clearAllDates();
        if ( reload ) {
            this.load();
        }
        this.addDates(newDatesArray);
    },

    clearAllDates: function() {
        if ( typeof this.dates == 'undefined' ) {
            this.dates = [];
        }
        
        var len = this.dates.length;
        for (var i = 0; i < len; i++) {
            this.executeDateRemovedCallback(this.dates[i]);
        }
        this.dates = [];
    },

    // KEEP!
    load: function() {
        var me = this;

    },
    
    /**
     * Method to register externally for beforeLoad callbacks for dates data.
     *
     * @param {Object} cb
     */
    addBeforeLoadCallback: function(cb) {
        this.beforeLoadCallbacks.push(cb);
    },
    
    /**
     * Method to register externally for loaded callbacks for dates data.
     *
     * @param {Object} cb
     */
    addLoadedCallback: function(cb) {
        this.loadedCallbacks.push(cb);
    },
    
    /**
     * Method to register externally for date added.
     *
     * @param {Object} cb
     */
    addDateAddedCallback: function(cb) {
        this.dateAddedCallbacks.push(cb);
    },
    
    /**
     * Method to register externally for date removed.
     *
     * @param {Object} cb
     */
    addDateRemovedCallback: function(cb) {
        this.dateRemovedCallbacks.push(cb);
    },
    
    /**
     * Notify listeners that the dates are loaded.
     */
    executeLoadedCallback: function() {
        var me = this;

        // console.log('');
        // console.log('Loaded ' + this.metarStore.getCount() + ' metars.');
        // console.log('Loaded ' + this.tafStore.getCount() + ' tafs.');
        // console.log('Loaded ' + this.pirepStore.getCount() + ' pireps.');
        // console.log('');

        var cbArray = this.loadedCallbacks;
        for (var idx in cbArray) {
            var thisCallback = cbArray[idx];
            console.log('CALLING LWM LOADED CALLBACK on registrant: ' + idx);
            try {
                thisCallback(this);
            } catch (err) {
                console.log('Error: LatestWmsMgr could not execute loaded callback: ' + err);
                debugger;
            }
        }

        // console.log('');
    },
    
    /**
     * Notify listeners that a date was added.
     */
    executeDateAddedCallback: function(dt) {
        var me = this;
        var cbArray = this.dateAddedCallbacks;
        for (var idx in cbArray) {
            var thisCallback = cbArray[idx];
            console.log('CALLING LWM DATE_ADDED CALLBACK for date: ' + dt + ' on registrant: ' + idx);
            try {
                thisCallback(this, dt);
            } catch (err) {
                console.log('Error: LatestWmsMgr could not execute date added callback: ' + err);
                debugger;
            }
        }

        // console.log('');
    },
    
    /**
     * Notify listeners that a date was removed.
     */
    executeDateRemovedCallback: function(dt) {
        var me = this;

        var cbArray = this.dateRemovedCallbacks;
        for (var idx in cbArray) {
            var thisCallback = cbArray[idx];
            console.log('CALLING LWM DATE_REMOVED CALLBACK for date: ' + dt + ' on registrant: ' + idx);
            try {
                thisCallback(this, dt);
            } catch (err) {
                console.log('Error: LatestWmsMgr could not execute date removed callback: ' + err);
                debugger;
            }
        }

        console.log('End LatestWmsMgr::executeDateRemovedCallback()');
    },

    waitFor: function(test, expectedValue, msec, count, source, callback) {
        // Check if condition met. If not, re-check later (msec).
        while (test() !== expectedValue) {
            count++;
            setTimeout(function() {
                console.log("Waiting...");
                MobileMet.util.StationDataMgr.waitFor(test, expectedValue, msec, count, source, callback);
            }, msec);
            return;
        }
        // Condition finally met. callback() can be executed.
        console.log(source + ': ' + test() + ', expected: ' + expectedValue + ', ' + count + ' loops.');
        callback();
    },

    /**
     * Start the timer to update this.closestStation, representing the closest airport in the user's route.
     */
    startGetTimesTimer: function(interval_ms) {
        var me = this,
        getTimesTask = me.getTimesTask;
        this.stopGetTimes = false;
        this.started = true;

        if ( !getTimesTask ) {
            getTimesTask = me.getTimesTask = {
                run: function() {
                    var stop = me.getStopTimes();
                    if (stop) {
                        console.log("Stopping getTimes task for LatestWmsMgr...");
                        runner.stop(getTimesTask);
                    } else {
                        if ( !me.isLoaded && !me.isLoading ) {
                            //
                            // Perform initial load of times...
                            //

                            me.getTimesForToday();
                        }
                        else {
                            //
                            // Get the available times for today...
                            //

                            // console.log("Performing update of getTimes in StationDataMgr...");
                            me.getTimesForToday();
                        }
                    }
                },
                interval: interval_ms
            };
            var runner = new emdss.util.TaskRunner();
            runner.start(getTimesTask);
        }
    },

    stopGetTimesTimer: function() {
        console.log("Setting flag to stop getTimes task on next run cycle...");
        this.stopGetTimes = true;
        this.getTimesTask = null;
        this.started = false;
    },

    getStopTimes: function() {
        return this.stopGetTimes;
    },
    
    getMostRecentTimes: function() {
        var numTimes = 20;

    },

// TODO: Straighten out this day transition mess!
//   - Suggestion: have success() call a managment method that will call for yesterday's dates if needed...

    getTimesForToday: function(useYesterday) {

        var me = this;
        var requestDate = new Date();

// return;
        Ext.Ajax.request({
            // url: 'http://manucode:8080/ncWMS/wms?item=timesteps&layerName=MergedBaseReflectivityQC/MergedBaseReflectivityQC&day=2015-11-04&request=GetMetadata',
            url: emdss.util.Config.getNcwmsUrl() + '/ncWMS/wms?item=timesteps&layerName=MergedBaseReflectivityQC/MergedBaseReflectivityQC&request=GetMetadata&day=' + (useYesterday ? emdss.util.Util.getYesterdayDayString() : emdss.util.Util.getTodayDayString()),
            // url: 'http://manucode:8080/ncWMS/wms',
            disableCaching : false,
            method: 'GET',
            // params : {
            //     item : 'timesteps',
            //     layerName : 'MergedBaseReflectivityQC/MergedBaseReflectivityQC',
            //     day : emdss.util.Util.dateToDayString(requestDate),
            //     request : 'GetMetadata'
            // },
            mgr: me,
            requestDate: new Date( requestDate ),
            success : function(response) {
                console.log("Success requesting ncWMS times: " + response.responseText);
                var data = null;
                var timesteps = null;
                var mgr = response.request.options.mgr;
                try {
                    data = JSON.parse(response.responseText);
                } catch (err) {
                    console.log("WARNING: Unable to parse ncWMS times for radar.");
                    debugger;
                }
                
                try {
                    var timesteps = data.timesteps;
                    
                    if (timesteps.length < 1 && useYesterday != true) {
                        mgr.getTimesForToday(true);
                        return;
                    }

                    var requestDate = response.request.options.requestDate;
                    var datesArray = emdss.util.Util.getPastNDatesFromTimes(requestDate, timesteps, emdss.util.Config.getNumRadarTimesteps());
                    
                    if ( typeof datesArray == 'undefined' || datesArray.length < 1 ) {
                        console.log("WARNING: Invalid or empty datesArray from AjaxRequest on LatestWmsMgr.");
                        return;
                    }
                    
                    // 
                    // Make sure the response included a date more recent than the latest date on the mgr.
                    // 
                    var mgrLatestDate = mgr.getLatestDate();
                    var responseLatestDate = datesArray[ (datesArray.length - 1) ];
                    
                    if ( mgrLatestDate == null || (responseLatestDate.getTime() - mgrLatestDate.getTime()) > 0 ) {
                        try {
                            mgr.updateDates(datesArray);
                        } catch (err) {
                            console.log("WARNING: Unable to update dates on LatestWmsMgr.");
                            debugger;
                        }
                        
                        try {
                            mgr.executeLoadedCallback();
                        } catch (err) {
                            console.log("WARNING: Unable to execute loaded callback on LatestWmsMgr.");
                            debugger;
                        }
                    }
                } catch (err) {
                    console.log("WARNING: Unable to process response from ncWMS radar times request.");
                    debugger;
                }
            },

            failure : function(response) {
                console.log("Failure requesting ncWMS times: " + response.responseText);
                debugger;
            }
        });
    }
});
