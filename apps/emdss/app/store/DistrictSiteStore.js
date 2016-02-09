/**
 * Store for the Districts holding site data.
 * @author Paddy McCarthy
 */
Ext.define('emdss.store.DistrictSiteStore',{
    extend:'Ext.data.Store',

    requires:['emdss.model.DistrictForSites',
    		  'emdss.model.Site',
    		  'emdss.model.HourlyAlert',
    		  'emdss.util.Config',
    		  'emdss.store.DataTimes',
    		  'emdss.util.TaskRunner',
    		  'Ext.data.proxy.JsonP'],
    
    config: {
		storeId:    'district-site-store',
		model:      'emdss.model.DistrictForSites',
		autoLoad:   false,
		reloadTask: null,
		stopReload: false,
		// date:       emdss.util.Config.getDefaultDate(),
		// date:       '20130514.1717',
		// state:      null,
		// loadedDataTime: null,
		// dataTime:   emdss.util.Config.getDefaultDate(),
		proxy:{
		    type:             'ajax',
		    // url:              emdss.util.Config.getAlertsUrl() + '&date_str=' + emdss.util.Config.getDefaultDate(),
		    url:              'JUNK',
		    reader: {
                type:         'json',
                rootProperty: 'districts'
            }
		},
		listeners: {
			'load': function (message) {
				console.log('... In DistrictSiteStore load() listener that was defined in config{}...' + message);
			}
		}
    },
    
    //
    // Used for debugging store loads.
    // 
    setupAfterListener: function() {
    	var me = this;
    	
    	console.log("In DistrictForSites::setupAfterListener...");
    	
    	me.addAfterListener("load",
		            		  function(records, operation, success, container){
							    console.log("AfterListener for load() in Sites store...");
							    console.log("Records:");
							    console.log(records);
							    console.log("Operation:");
							    console.log(operation);
							 	console.log("Success:");
							 	console.log(success);
							 	console.log("Container:");
							 	console.log(container);
							 	
							 	// 'state' disappeared from the data service for some reason...
							 	// var stateString = me.getState();
							 	// console.log("State: " + stateString);
							 	
							 	var siteList = Ext.getCmp("site-list");
							 	var siteListItem = Ext.getCmp("site-list-item");
							 	
							 	// debugger;
							 	
							 	var dateString = me.getDataTime();
								// me.each(function (district, index, length) {
									// district.data.dataTime = dateString;
								// });
								
							 	console.log("Loaded Data Time: " + dateString);
							 });
		
		me.addListener("beforeload",
		            	 function(records, operation, success, container){
							console.log("beforeLoad listener in Sites store...");
						});
    },
    
    startReloadTimer: function(interval_ms) {
    	var me = this,
    	reloadTask = me.reloadTask;
    	this.stopReload = false;
    	
    	if ( !reloadTask ) {
	    reloadTask = me.reloadTask = {
	    	run: function() {
	    	    var stop = me.getStopReload();
	    	    if (stop) {
	    		console.log("Stopping reload task for site alerts...");
	    		runner.stop(reloadTask);
	    	    } else {
	    		console.log("In site alerts reloadTask::run()...");
	    		if ( !me.isLoaded() && !me.isLoading() ) {
	    		    // 
	    		    // Perform initial load of this store...
	    		    // 
	    		    console.log("Performing initial load of site alerts...");
	    		    // me.loadData();
	    		    me.load();
	    		}
	    		else {
	    		    // else if ( Ext.data.StoreManager.lookup('latest-data-times-store') != undefined ) {
	    		    // 
	    		    // Reload the data times store and check if we have new data...
	    		    // 
	    		    var dataTimesStore = Ext.data.StoreManager.lookup('latest-data-times-store');
	    		    dataTimesStore.removeAll(true); // Silently clear out the store...
	    		    dataTimesStore.load(function(records, operation, success) {
	    			var alertsDir = emdss.util.Config.getAlertsDir();
		    		console.log("About to look for " + alertsDir + " in dataTimesStore. Num records: " + dataTimesStore.getCount());
		    		var relevantRecords = dataTimesStore.queryBy( 
		    		    function (record, id) {
		    			var match = false;
		    			console.log("record.data.dir: " + record.data.dir + " alertsDir: " + alertsDir);
		    			if (record.data.dir == alertsDir) {
		    			    match = true;
		    			}
		    			console.log("Match: " + match);
		    			return match;
		    		    });
		    		if (relevantRecords == null || relevantRecords.getCount() < 1) {
		    		    // This is not an error condition -- just a sign that data times haven't loaded yet.
		    		    console.log("NOTICE: The alertsDir \"" + 
		    				alertsDir + 
		    				"\" is not referenced in the data returned by the service at: " + 
		    				emdss.util.Config.getDataTimesUrl());
		    		} else {
		    		    var latestDataTimeStr = relevantRecords.first().data.latest_time;
		    		    var loadedDataTimeStr = me.getDataTime();
		    		    console.log("Latest available time for dir: " + alertsDir + " is: " + latestDataTimeStr);
		    		    console.log("Loaded time is: " + loadedDataTimeStr);
		    		    var latestDate = Ext.Date.parse(latestDataTimeStr,"YmdHi");
		    		    // var loadedDate = Ext.Date.parse(loadedDataTimeStr,"Ymd.Hi");
		    		    var loadedDate = Ext.Date.parse(loadedDataTimeStr,"YmdHi");
		    		    if ( latestDate == undefined || loadedDate == undefined ) {
		    			console.log("latestDate or loadedDate uninitialized. Sleeping...");
		    		    }
		    		    else if ( latestDate.getTime() > loadedDate.getTime() ) {
		    			console.log(" ");
		    			console.log("LOADING new data!!!...");
		    			console.log(" ");
		    			me.load();
		    		    } else {
		    			console.log("No new data available. Sleeping...");
		    		    }
		    		}
	    		    });
	    		}
	    	    }
	    	},
	    	interval: interval_ms
    	    };
    	    var runner = new emdss.util.TaskRunner();
	    runner.start(reloadTask);
    	}
    },
    
    stopReloadTimer: function() {
    	console.log("Setting flag to stop reload task on next run cycle...");
    	this.stopReload = true;
    	this.reloadTask = null;
    },

    setDate: function(date) {
		this.date = date;
    },

    setAlertType: function(alertType) {
		this.alertType = alertType;
    },
	
	setDataTimeOnDistricts: function(store, data, eOpts) {
        //store is loaded, now you can work with it's records, etc.
        console.log('in setDataTimeOnDistricts() in DistrictSiteStore...');
        var dateString = this.getDataTime();
		// debugger;
	    this.each(function (district, index, length) {
			// district.data.dataTime = dateString;
			// debugger;
			
			// 
			// Calling set() on the record fires an event that updates the view...
			// 
			district.set({
			    data_time : dateString
			});
		});
		
		// this.fireEvent('refresh');
	},
	
    loadData: function(container) {
    	debugger;
    	
    	// This shouldn't be used...
    	
    	debugger;
		this.setProxy({
		    type:'ajax',
		    url: emdss.util.Config.getDistrictAlertsUrl() + 'WAH WAH WAH WAH WHAT? &date_str=' +  this.date + "&alert_type=" + this.alertType
		});
		console.log(this.getProxy['url']);
		if (!container) {
		    container = Ext.Viewport;
		}
		container.setMasked({
		    xtype:'loadmask',
		    message:"Loading DistrictsForSites..."
		});
		var fn = Ext.bind(function(records, operation, success, container) {
		    container.setMasked(false);
		}, this, [container], true);
		this.load(fn);
    },

    getStateFn : null,
    getDataTimeFn : null,

    getState : function () {
        var me = this,
        proxy  = me.getProxy(),
        reader = proxy.getReader(),
        getState = me.getStateFn,
        rawData;

        if (!getState) {
            getState = me.getStateFn = reader.createAccessor('state');
        }

        return (function () {
            rawData = reader.rawData;

            return getState(rawData);
        })();
    },

    getDataTime : function () {
        var me = this,
        proxy  = me.getProxy(),
        reader = proxy.getReader(),
        getDataTime = me.getDataTimeFn,
        rawData;

        if (!getDataTime) {
            getDataTime = me.getDataTimeFn = reader.createAccessor('data_time');
        }

        return (function () {
            if (reader.rawData == undefined || reader.rawData == null) {
                return null;
            }
            var rawData = reader.rawData;

            return getDataTime(rawData);
        })();
    }
});
