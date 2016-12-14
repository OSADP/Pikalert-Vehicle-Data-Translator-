/**
 * Travel Time controller -- handle travel time-related data and actions
 * @author paddy
 */
Ext.define('emdss.controller.TravelTimeController', {
    extend:'Ext.app.Controller',
    requires:[
        'emdss.util.Config',
        'emdss.util.Util'
    ],
    config:{
        refs:{
            mainTabPanel: 'main[id=main-tab-panel]',
            mapNavView: 'main navigationview[id=map-nav-view]',
            ttNavView: 'main navigationview[id=travel-time-list]',
            mapView: 'main map',
            listItemVehicles: '#vehicle-list-item',
            listTraveltimes: '#travel-time-list-item',
            selectfieldDepartureTime: 'main selectfield[id=departure-time-select]'
        },
        control: {
            'listItemVehicles': {
                itemtap: 'onVehicleSelect'
            },
            'listTraveltimes': {
                itemtap: 'onTraveltimeTap',
                itemtaphold: 'onTraveltimeTapHold',
                itemtouchstart: 'onTraveltimeTouchStart',
                itemtouchmove: 'onTraveltimeTouchMove',
                itemtouchend: 'onTraveltimeTouchEnd',
            },
            'selectfieldDepartureTime': {
                change: 'onDepartureTimeChange'
            }
        },
        
        departureTimesInitialized: false
    },
    
    /**
     * Set up the vehicle store, start its update timer, and register callback for
     *   clicks on the vehicle list.
     */
    init: function() {
        var me = this;
        
        console.log("In TravelTimeController::init()...");
    
        //
        // Travel Time Segments -- eastbound and westbound
        //
        this.ebTTSegs = Ext.create("emdss.store.TravelTimeSegmentStore",{
            storeId:"eb-travel-time-segment-store"
        });
        console.log("Done creating Eastbound TravelTimeSegment Store...");
        // this.ebTTSegs.setupAfterListener();
        
        this.ebTTSegs.setProxy({
            type:'ajax',
            // url: 'http://localhost:8888/resources/travel_time/travel_time_segments_eastbound.json',
            url: 'http://dot.rap.ucar.edu/emdss/resources/travel_time/travel_time_segments_eastbound.json',
            reader: {
                    type:         'json',
                    rootProperty: 'eastboundSegments'
                }
        });
        this.ebTTSegs.load();
        
        this.wbTTSegs = Ext.create("emdss.store.TravelTimeSegmentStore",{
            storeId:"wb-travel-time-segment-store"
        });
        console.log("Done creating Westbound TravelTimeSegment Store...");
        this.wbTTSegs.setupAfterListener();
        
        this.wbTTSegs.setProxy({
            type:'ajax',
            // url: 'http://localhost:8888/resources/travel_time/travel_time_segments_westbound.json',
            url: 'http://dot.rap.ucar.edu/emdss/resources/travel_time/travel_time_segments_westbound.json',
            reader: {
                    type:         'json',
                    rootProperty: 'westboundSegments'
                }
        });
        this.wbTTSegs.load();
        
        //
        // Travel Times
        //
        this.travelTimes = Ext.create("emdss.store.TravelTimeStore",{
            storeId:"travel-time-store"
        });
        console.log("Done creating TravelTimes Store...");
            
        this.travelTimes.setupAfterListener();
        
        this.travelTimes.setProxy({
            type:'ajax',
            url:   emdss.util.Config.getTravelTimesUrl()
                    + '&state=' + emdss.util.Config.getHrefState()
                    + ( emdss.util.Config.getUseVdt() ? '_vdt' : '' ),
            // url: 'http://localhost:8888/resources/travel_time/travel_times_test_file.json',
            reader: {
                    type:         'json',
                    rootProperty: 'travelTimes'
                }
        });
        this.travelTimes.on({
            load:'onTravelTimesLoad',
            scope:this
        });
        this.travelTimes.on({
            refresh:'onTravelTimesRefresh',
            scope:this
        });
        
        this.travelTimes.startReloadTimer(60000);
        
        var appController = emdss.app.getController('emdss.controller.Application');
        this.travelTimes.getProxy().addListener('exception', function (proxy, response, operation) {
                                                                console.log(response.status);
                                                                console.log(response.responseText);
                                                                appController.travelTimesAttemptedLoad = true;
                                                                appController.travelTimesLoadFailed = true;
                                                                appController.checkLoadmaskDisable();
                                                            }, this);
        this.travelTimes.load();
    },
    
    /**
     * Not used.
     */
    launch: function() {
        var me = this;
        
        // 
        // Load previous state of whether vehicles are showing or not.
        // 
        // if ( emdss.util.Util.supports_html5_storage() ) {
            // var showVehicles = window.localStorage.getItem('showVehicles');
            // if ( showVehicles == 'false' ) {
                // me.showVehiclesMethod(false);
            // }
        // }
    },

    /**
     * Handle load of new travel times from server -- called after the TravelTimes store is done loading.
     *   - For debug purposes
     */
    onTravelTimesLoad: function(store, data, eOpts) {
        
        console.log("Loaded travel times...");
        
        var records = data;
        for (var i = 0; i < records.length; i++) {
            var travelTime = records[i];
            // var coords = travelTime.coords(); // returns Ext.Store of WatchWarnCoords
            // debugger;
        }
        
        // debugger;
        
        // var map = this.getMapView().getMap();
        // this.addWatchWarnsToMap(records, map);

        var appController = emdss.app.getController('emdss.controller.Application');
        if ( !appController.travelTimesAttemptedLoad ) {
            appController.travelTimesAttemptedLoad = true;
            appController.travelTimesLoadFailed = false;
            appController.checkLoadmaskDisable();
        }
    },
    
    /**
     * Handle update of new travel times from server -- called after the TravelTimes store is done updating.
     *   - For debug purposes
     */
    onTravelTimesRefresh: function(store, data, eOpts) {
        
        console.log("Refreshed travel times...");
        
        return;
        
        var records = data.items;
        for (var i = 0; i < records.length; i++) {
            var travelTime = records[i];
            // var coords = travelTime.coords(); // returns Ext.Store of WatchWarnCoords
            // debugger;
        }
        
        // var map = this.getMapView().getMap();
        // this.addWatchWarnsToMap(records, map);
        
        // var appController = emdss.app.getController('emdss.controller.Application');
        // if ( !this.travelTimesAttemptedLoad ) {
            // this.travelTimesAttemptedLoad = true;
            // this.travelTimesLoadFailed = false;
            // appController.checkLoadmaskDisable();
        // }
    },
    
    /**
     * Called by the TravelTimeStore when it gets new travel time data.
     * @param {Object} travelTimeStore - A ref to the TravelTimeStore that got refreshed, causing this call.
     */
    refreshDepartureTimes: function(travelTimeStore) {
        var me = this;
        
        me.setDepartureTimesInitialized(true);
        
        var selField = me.getSelectfieldDepartureTime();
        // debugger;
        var prevSelection = selField.getRecord();
        
        var arr = [];
        var times = travelTimeStore.first().traveltimeforecasts();
        times.sort('departure', 'ASC');
        times.each(function (record, index, length) {
            var dep = record.get('departure');
            arr[index] = {text: emdss.util.Util.dateToPrettyShortDateString(dep), value: dep};
        });
        
        selField.setOptions(arr);
        
        // Make the selection match (as closely as possible) the previous selection time.
        if ( prevSelection ) {
            var prevDate = prevSelection.get('value');
            
            times.each(function (record, index, length) {
                var dep = record.get('departure');
                if ( dep.getTime() >= prevDate.getTime() ) {
                    selField.setValue(dep);
                    return false; // stop iterating.
                }
            });
        }
        
        var selVal = selField.getValue();
        var fromSegmentStore = Ext.getStore('wb-travel-time-segment-store');
        fromSegmentStore.selectedDepartureTime = selVal;
    },
    
    onDepartureTimeChange: function( selectfield, newValue, oldValue, eOpts ) {
        var me = this;
        
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("Departure time list changed on GRID controller");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        
        var travelTimeStore = Ext.getStore('travel-time-store');
        var fromSegmentStore = Ext.getStore('wb-travel-time-segment-store');
        fromSegmentStore.previousSelectedDepartureTime = oldValue;
        fromSegmentStore.selectedDepartureTime = newValue;
        fromSegmentStore.recalcAggregateTable(travelTimeStore);
        fromSegmentStore.fireEvent('refresh', fromSegmentStore, fromSegmentStore.getData());
    },
    onTraveltimeTap: function( list, index, target, record, e, eOpts ) {
        var me = this;
        
        console.log("Traveltime tap..." + record.get('from'));
        
        return;
        
        me.ttPopup = me.getTtNavView().add({
            xtype: 'panel',
            layout: 'fit',
            modal: true,
            hideOnMaskTap: true,
            showAnimation: {
                type: 'popIn',
                duration: 250,
                easing: 'ease-out'
            },
            hideAnimation: {
                type: 'popOut',
                duration: 250,
                easing: 'ease-out'
            },
            centered: true,

            width: Ext.os.deviceType == 'Phone' ? 260 : 520,
            height: Ext.os.deviceType == 'Phone' ? 220 : 250,
            
            html: "Travel Times from: " + record.get('from'),
            items: [
                {
                    docked: 'top',
                    xtype: 'toolbar',
                    title: 'Travel Time Chart'
                },
                {
                    xtype: 'panel',
                    itemId: 'Graph1'
                }
            ],
            scrollable: true
        });
        
        // me.ttPopup.show();
        
        // I have never worked with Sencha Touch but I have worked with Sencha Ext JS.
// 
// You don't need an extension in order mix up Sencha ExtJS and Highcharts. All Highchart/HighStock charts are assigned an html div to which they are rendered. You only need to get the html div from the Ext Js element you want to use for your chart. Here is an example from code i have which works fine:
// 
// var myVp = Ext.create('Ext.container.Viewport', {.........}
// 
// chart1 = new Highcharts.StockChart({
   // chart: {
      // renderTo: myVp.down('#Graph1').getEl().dom,
      // zoomType: 'x'
   // },
// ............
// ............
// });
// Here is the Graph1, its somewhere in myVp
// 
  // {
    // flex: 8,
    // xtype : "component", //i have other examples where the xtype is panel that
                         // //work fine. I think this will work with all the type
                         // //on containers
    // itemId: 'Graph1'
  // }
// I know this isnt exactly what you were asking but i think its a very good aproach. The only drawback this has is that you will not be able to assigning a sencha store as the data of the chart. Here is the JsFiddle. Write me with feedbacks.

        
        
        
        
        
        
        
        
        
        
        
        // Another Example:
        // 
        // http://jsfiddle.net/marcme/DmsGx/
        // 
        
        // var store = new Ext.data.ArrayStore({
    // fields: ['month', 'value'],
    // data : [
        // ['jan', 1],
        // ['feb', 2],
        // ['mar', 3],
        // ['apr', 4],
        // ['mai', 5],
        // ['jun', 6],
        // ['jul', 16],
        // ['aug', 2],
        // ['sep', 7],
        // ['oct', 4],
        // ['nov', 3],
        // ['dec', 11]
    // ]
// });
// 
// var chart = Ext.create('Chart.ux.Highcharts', {
    // series:[{
        // dataIndex: 'value'
    // }],
    // xField: 'month',
    // store: store,
    // chartConfig: {
        // chart: {
            // type: 'spline'
        // },
        // title: {
            // text: 'A simple graph'
        // },
        // xAxis: {
            // plotLines: [{
                // color: '#FF0000',
                // width: 5,
                // value: 'mar'
            // }]
        // }
    // }
// });
// 
// Ext.create('Ext.panel.Panel',{
    // title: 'Highcharts-Test',
    // width: 500,
    // height: 500,
    // layout: 'fit',
    // renderTo: Ext.getBody(),
    // items: [ chart ]
// });
// 
// store.load();
            
    },

    onTraveltimeTapHold: function( list, index, target, record, e, eOpts ) {
        var me = this;
        
        console.log("Traveltime tapHold...");
    },
        
    onTraveltimeTouchStart: function( list, index, target, record, e, eOpts ) {
        var me = this;
        
        console.log("Traveltime touchStart...");
    },
        
    onTraveltimeTouchMove: function( list, index, target, record, e, eOpts ) {
        var me = this;
        
        console.log("Traveltime touchMove...");
    },
        
    onTraveltimeTouchEnd: function( list, index, target, record, e, eOpts ) {
        var me = this;
        
        console.log("Traveltime touchEnd...");
    },
        
});
