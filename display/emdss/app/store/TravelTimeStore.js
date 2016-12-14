/**
 * Store for the Districts holding vehicle data.
 * @author Paddy McCarthy
 */
Ext.define('emdss.store.TravelTimeStore',{
    extend:'Ext.data.Store',

    requires:['emdss.model.TravelTime',
              'emdss.util.Config',
              'emdss.util.TaskRunner',
              'Ext.data.proxy.JsonP'],
    
    config: {
        storeId:    'travel-time-store',
        model:      'emdss.model.TravelTime',
        autoLoad:   false,
        reloadTask: null,
        stopReload: false,
        proxy:{
            type:             'ajax',
            url:              'JUNK',
            reader: {
                    type:         'json',
                    rootProperty: 'travelTimes'
                }
        },
        listeners: {
            'load': function (message) {
                console.log('... In TravelTimeStore load() listener that was defined in config{}...' + message);
            }
        }
    },
    
    //
    // Used for debugging store loads.
    // 
    setupAfterListener: function() {
        var me = this;
        
        console.log("In TravelTimeStore::setupAfterListener...");
        
        me.addAfterListener("load",
            function(records, operation, success, container){
 
                console.log("AfterListener for load() in TravelTimes store...");
                console.log("Records:");
                console.log(records);
                console.log("Operation:");
                console.log(operation);
                console.log("Success:");
                console.log(success);
                console.log("Container:");
                console.log(container);
                                
                // var dateString = me.getDataTime();
                // me.each(function (district, index, length) {
                // district.data.dataTime = dateString;
                // });
                
                console.log("TRAVEL_TIMES Loaded ");
                
                // 
                // Capture the first time in one of the segment arrays, and use this as the latest data indicator
                //   for what was loaded.
                // 
                var prevLoadedData = me.latestLoadedData;
                if ( !records.first() ) {
                    console.log("No travel_time records... Skipping.");
                    return;
                }

                me.latestLoadedData = records.first().traveltimeforecasts().first().get('departure');
                var travelTimeController = emdss.app.getController('emdss.controller.TravelTimeController');
                if ( typeof prevLoadedData == 'undefined' || 
                     (me.latestLoadedData.getTime() > prevLoadedData.getTime()) ) {
                    // 
                    // Populate the departure time selectfield, and set it to something close to what it showed before.
                    // 
                    travelTimeController.refreshDepartureTimes(me);
                    
                    // 
                    // Recalc the westbound segments store, which holds a table of the aggregated data.
                    // Fire a refresh event on that store, so that it updates the view.
                    // 
                    var segStore = Ext.getStore('wb-travel-time-segment-store');
                    segStore.recalcAggregateTable(me);
                    segStore.fireEvent('refresh', segStore, segStore.getData());
                }
            });
    
        me.addListener("beforeload",
           function(records, operation, success, container){
               console.log("beforeLoad listener in TravelTimeStore removing all previous records...");
               me.removeAll(true);
           });
    },
    
    startReloadTimer: function(interval_ms) {
        var me = this,
        reloadTask = me.getReloadTask();
        me.setStopReload(false);
        
        if ( !reloadTask ) {
            reloadTask = me.reloadTask = {
                interval: interval_ms,
                
                run: function() {
                    var stop = me.getStopReload();
                    if (stop) {
                        console.log("Stopping reload task for travel times...");
                        runner.stop(reloadTask);
                    }
                    else {
                        me.load();
                    }
                }
            };
            var runner = new emdss.util.TaskRunner();
                runner.start(reloadTask);
        }
    },
    
    stopReloadTimer: function() {
        console.log("Setting flag to stop reload task on next run cycle...");
        this.stopReload = true;
        this.reloadTask = null;
    }
});
