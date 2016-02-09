/**
 * Store for the Districts holding vehicle data.
 * @author Paddy McCarthy
 */
Ext.define('emdss.store.DistrictVehicleStore',{
    extend:'Ext.data.Store',

    requires:['emdss.model.DistrictForVehicles',
              'emdss.model.Vehicle',
              'emdss.util.Config',
              'emdss.store.DataTimes',
              'emdss.util.TaskRunner',
              'Ext.data.proxy.JsonP'],
    
    config: {
    storeId:    'district-vehicle-store',
    model:      'emdss.model.DistrictForVehicles',
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
        url:              'JUNK',
        reader: {
                type:         'json',
                rootProperty: 'districts'
            }
    },
    listeners: {
        'load': function (message) {
        // console.log('... In DistrictVehicleStore load() listener that was defined in config{}...' + message);
        }
    }
    },
    
    //
    // Used for debugging store loads.
    // 
    setupAfterListener: function() {
        var me = this;
        
        console.log("In DistrictForVehicles::setupAfterListener...");
        
        me.addAfterListener("load",
                    function(records, operation, success, container){
/*
                console.log("AfterListener for load() in Vehicle store...");
                console.log("Records:");
                console.log(records);
                console.log("Operation:");
                console.log(operation);
                console.log("Success:");
                console.log(success);
                console.log("Container:");
                console.log(container);
                                
                var dateString = me.getDataTime();
                // me.each(function (district, index, length) {
                // district.data.dataTime = dateString;
                // });
                
                console.log("Loaded Data Time: " + dateString);
*/
                });
    
    me.addListener("beforeload",
               function(records, operation, success, container){
               // console.log("beforeLoad listener in Vehicle store...");
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
                console.log("Stopping reload task for vehicles...");
                runner.stop(reloadTask);
                } else {
                // console.log("In vehicle reloadTask::run()...");
                if ( !me.isLoaded() && !me.isLoading() ) {
                    // 
                    // Perform initial load of this store...
                    // 
                    // console.log("Performing initial load of vehicles...");
                    // me.loadData();
                    me.load();
                }
                else {
                    // 
                    // Reload the data times store and check if we have new data...
                    // 
                    var dataTimesStore = Ext.data.StoreManager.lookup('latest-data-times-store');
                    dataTimesStore.removeAll(true); // Silently clear out the store...
                    dataTimesStore.load(function(records, operation, success) {
                    var vehiclesDir = emdss.util.Config.getVehiclesDir();
                    // console.log("About to look for " + vehiclesDir + " in dataTimesStore. Num records: " + dataTimesStore.getCount());
                    var relevantRecords = dataTimesStore.queryBy( 
                        function (record, id) {
                        var match = false;
                        // console.log("record.data.dir: " + record.data.dir + " vehiclesDir: " + vehiclesDir);
                        if (record.data.dir == vehiclesDir) {
                            match = true;
                        }
                        // console.log("Match: " + match);
                        return match;
                        });
                    if (relevantRecords == null || relevantRecords.getCount() < 1) {
                        // This is not an error condition -- just a sign that data times haven't loaded yet.
                        // console.log("NOTICE: The vehiclesDir \"" + 
                    //      vehiclesDir + 
                    //      "\" is not referenced in the data returned by the service at: " + 
                    //      emdss.util.Config.getDataTimesUrl());
                    } else {
                        var latestDataTimeStr = relevantRecords.first().data.latest_time;
                        var loadedDataTimeStr = me.getDataTime();
                        console.log("Latest available time for dir: " + vehiclesDir + " is: " + latestDataTimeStr);
                        console.log("Loaded time is: " + loadedDataTimeStr);
                        var latestDate = Ext.Date.parse(latestDataTimeStr,"YmdHi");
                        // var loadedDate = Ext.Date.parse(loadedDataTimeStr,"Ymd.Hi");
                        var loadedDate = Ext.Date.parse(loadedDataTimeStr,"YmdHi");
                        if ( latestDate == undefined || loadedDate == undefined ) {
                        console.log("latestDate or loadedDate uninitialized for vehicle data. Sleeping...");
                        }
                        else if ( latestDate.getTime() > loadedDate.getTime() ) {
                        console.log(" ");
                        console.log("Loading new VEHICLE data!!!...");
                        console.log(" ");
                        me.load();
                        } else {
                        console.log("No new vehicle data available. Sleeping...");
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
        
        // debugger;
        
        // store is loaded, now you can work with it's records, etc.
        // console.log('in setDataTimeOnDistricts() in DistrictVehicleStore...');
        var dateString = this.getDataTime();
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
        
        // debugger;
        
        // this.fireEvent('refresh');
    },
    
    loadData: function(container) {
        
        // This shouldn't be used...
        
        this.setProxy({
            type:'ajax',
            url: emdss.util.Config.getDistrictAlertsUrl() + 'WAH WAH WAH WAH WHAT? &date_str=' +  this.date + "&alert_type=" + this.alertType
        });
        // console.log(this.getProxy['url']);
        if (!container) {
            container = Ext.Viewport;
        }
        container.setMasked({
            xtype:'loadmask',
            message:"Loading DistrictsForVehicles..."
        });
        var fn = Ext.bind(function(records, operation, success, container) {
                            container.setMasked(false);
                        }, this, [container], true);
        this.load(fn);
    },
    
    // setDataTimeOnDistricts: function(store, data, eOpts) {
    //     //store is loaded, now you can work with it's records, etc.
    //     console.log('in setDataTimeOnDistricts() in DistrictVehicleStore...');
    //     var dateString = this.getDataTime();
//      // debugger;
//      this.each(function (district, index, length) {
//          // district.data.dataTime = dateString;
//          // debugger;
//          
//          // 
//          // Calling set() on the record fires an event that updates the view...
//          // 
//          district.set({
//          data_time : dateString
//          });
//      });
//  
//  // this.fireEvent('refresh');
    // },

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
