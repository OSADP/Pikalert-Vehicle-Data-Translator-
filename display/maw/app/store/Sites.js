/**
 * Store for the Sites. This store associates a set of HourlyAlerts (time series).
 * @author Paddy McCarthy
 */
Ext.define('mawDesk.store.Sites',{
    extend:'Ext.data.Store',

    requires:['mawDesk.model.Site',
    		  'mawDesk.util.Config',
    		  'mawDesk.store.DataTimes',
    		  'mawDesk.util.TaskRunner',
    		  'Ext.data.proxy.JsonP'],
    
    config:{
		storeId:    'sites-store',
		model:      'mawDesk.model.Site',
		autoLoad:   false,
		reloadTask: null,
		stopReload: false,
		// date:       mawDesk.util.Config.getDefaultDate(),
		date:       '20130514.1717',
		dataTime:   mawDesk.util.Config.getDefaultDate(),
		proxy:{
		    type:             'ajax',
		    // url:              mawDesk.util.Config.getAlertsUrl() + '&date_str=' + mawDesk.util.Config.getDefaultDate(),
		    url:              mawDesk.util.Config.getAlertsUrl() + '&date_str=' + '20130514.1717',
		    reader: {
                type:         'json',
                rootProperty: 'data'
            }
		}
    },
    
    setupAfterListener: function() {
    	
    	console.log("In Sites::setupAfterListener...");
    	
    	this.addAfterListener("load",
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
							 	
							 	var timeString = this.getDataTime();
							 	
							 	console.log("Data_time: " + timeString);
							 	this.dataTime = timeString;
							 });
		
		this.addListener("beforeload",
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
	    					// 
	    					// Reload the data times store and check if we have new data...
	    					// 
	    					var dataTimesStore = Ext.data.StoreManager.lookup('latest-data-times-store');
	    					dataTimesStore.load();
	    					var alertsDir = mawDesk.util.Config.getAlertsDir();
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
	    						console.log("ERROR: The alertsDir \"" + 
	    									alertsDir + 
	    									"\" is not referenced in the data returned by the service at: " + 
	    									mawDesk.util.Config.getDataTimesUrl());
	    					} else {
	    						var latestDataTimeStr = relevantRecords.first().data.latest_time;
	    						var loadedDataTimeStr = me.getDataTime();
	    						console.log("Latest available time for dir: " + alertsDir + " is: " + latestDataTimeStr);
	    						console.log("Loaded time is: " + loadedDataTimeStr);
	    						var latestDate = Ext.Date.parse(latestDataTimeStr,"YmdHi");
	    						var loadedDate = Ext.Date.parse(loadedDataTimeStr,"Ymd.Hi");
	    						if ( latestDate.getTime() > loadedDate.getTime()) {
	    							console.log("Loading new data...");
	    							me.load();
	    						} else {
	    							console.log("No new data available. Sleeping...");
	    						}
	    					}
	    				}
	    			}
	    		},
	    		interval: interval_ms
    		};
    		var runner = new mawDesk.util.TaskRunner();
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

    loadData: function(container) {
		this.setProxy({
		    type: 'ajax',
		    url: mawDesk.util.Config.getAlertsUrl() + '&date_str=' +  this.date + "&alert_type=" + this.alertType
		});
		console.log(this.getProxy['url']);
		if (!container) {
		    container = Ext.Viewport;
		}
		container.setMasked({
		    xtype:'loadmask',
		    message:"Loading Alerts..."
		});
		var fn = Ext.bind(function(records, operation, success, container) {
		    container.setMasked(false);
		}, this, [container], true);
		this.load(fn);
    },

    getDataTimeFn : null,

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
            rawData = reader.rawData;

            return getDataTime(rawData);
        })();
    }
});