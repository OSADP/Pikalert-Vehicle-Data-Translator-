/**
 * Main Application controller -- start data loading timers. build the map,
 *   and handle UI interactions.
 * @author paddy
 */
Ext.define('emdss.controller.Application',{
    extend:'Ext.app.Controller',
    requires:[
        'emdss.util.Config',
        'emdss.controller.VehicleController'
    ],
    config:{
        refs:{
            mainTabPanel: 'main[id=main-tab-panel]',
            mapNavView: 'main navigationview[id=map-nav-view]',
            mapView: 'main map',
            mapToolbar: 'main toolbar[id=map-toolbar]',
            btnFcstType: 'main segmentedbutton[id=fcst-type-select]',
            btnVehicles: 'main button[id=hide-show-vehicles-button]',
            btnRadar: 'main button[id=hide-show-radar-button]',
            btnHideGreen: 'main button[id=hide-green-segments-button]',
            labelMapValidTime: '#map-valid-time-label',
            labelMapCurrTime: '#map-curr-time-label',
            panelMapTime: '#map-time-panel',
            panelRadar: 'main panel[id=radar-panel]',
            panelDepartureTime: 'main panel[id=departure-time-panel]',
            panelDepartureTimeList: 'main panel[id=departure-time-list-panel]'
        },
        clockTask: null,
        stopClock: false,
        control: {
            
        }
    },

    /**
     * Initialize the app:
     *   - Create data stores for:
     *       Latest data update time on the server
     *       Districts and their related sites
     *   - Wire the load listeners for each of the data stores
     *   - Start the update timers for each of the data stores
     *   - Start the clock timer
     *   - Set up the UI control callbacks
     */
    init: function() {
        var me = this;

        var prefixTest = 'unk';
        var hrefStateTest = emdss.util.Config.getHrefState();
        if ( hrefStateTest == 'colorado' )    { prefixTest = 'co'; }
        else if ( hrefStateTest == 'michigan' )  { prefixTest = 'mi'; }
        else if ( hrefStateTest == 'minnesota' ) { prefixTest = 'mn'; }
        else if ( hrefStateTest == 'nevada' )    { prefixTest = 'nv'; }
        else if ( hrefStateTest == 'alaska' )    { prefixTest = 'ak'; }

        // 
        // Road Segments
        // 

        var appRootTest = window.location.href.substr( 0, window.location.href.indexOf('?') );
        if ( appRootTest.length == 0 ) {
            appRootTest = window.location.href;
        }
        var xhrTest = new XMLHttpRequest();
        var requestUrlTest = appRootTest
                             + '/resources/road_segments/'
                             + prefixTest + '_roads.json';
        xhrTest.open('GET', requestUrlTest);
        xhrTest.setRequestHeader("Cache-Control", "no-cache,max-age=0");
        xhrTest.setRequestHeader("Pragma", "no-cache");
        xhrTest.onload = function(e) {
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("After load of SEGMENTS TEST: ");
            console.log("   time:" + (new Date()));
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");

            var responseTest = JSON.parse(this.response);

            this.onload.applicationController.rawRoadSegments = responseTest;

            this.onload.applicationController.onRoadSegmentsUpdate();
          
        };
        xhrTest.onerror = function(e) {
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("ERROR on load of SEGMENTS TEST: ");
            console.log("   time:" + (new Date()));
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            // console.log("RoadSegments Proxy Exception! response status: " + response.status + "( " + response.statusText + " ). Timed out: " + response.timedout);
            // console.log("RoadSegments Proxy Exception! response text: " + response.responseText);
            this.onerror.applicationController.roadSegmentsAttemptedLoad = true;
            this.onerror.applicationController.roadSegmentsLoadFailed = true;
            this.onerror.applicationController.checkLoadmaskDisable();
        };
        xhrTest.onload.applicationController = this.getApplication().getController('emdss.controller.Application');
        xhrTest.onerror.applicationController = this.getApplication().getController('emdss.controller.Application');

            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("Before load of SEGMENTS TEST: ");
            console.log("   time:" + (new Date()));
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");
        xhrTest.send();

        console.log("In Application::init()...");
        
        // Initialize the timezone code

        //  timezoneJS.timezone.zoneFileBasePath = 'tz';
        //  timezoneJS.timezone.init();

        var hrefState = emdss.util.Config.getHrefState();
        if ( hrefState != 'colorado' ) {
             this.travelTimesAttemptedLoad = true;
             this.travelTimesLoadFailed = false;

             this.travelTimesListAttemptedLoad = true;
             this.travelTimesListLoadFailed = false;
        }

        console.log("Configuring MAW for state: " + hrefState);

        // 
        // Latest Data Times
        // 
        this.latestDataTimes = Ext.create("emdss.store.DataTimes",{
            storeId:"latest-data-times-store"
        });
        this.latestDataTimes.setProxy({
            type:'ajax',
            url:   emdss.util.Config.getDataTimesUrl()
                    + '&state=' + emdss.util.Config.getHrefState()
                    + ( emdss.util.Config.getUseVdt() ? '_vdt' : '' )
            // url: mawDesk.util.Config.getTestDataTimesUrl() + '&state='
            //      + mawDesk.util.Config.getHrefState()
            //      + ( emdss.util.Config.getUseVdt() ? '_vdt' : '' )
        });
        this.latestDataTimes.on({
            refresh:'onDataTimesUpdate',
            scope:this
        });
        this.latestDataTimes.getProxy().addListener('exception', function (proxy, response, operation) {
                                                                     console.log(response.status);
                                                                     console.log(response.responseText);
                                                                     if ( !this.dataTimesAttemptedLoad ) {
                                                                         this.dataTimesAttemptedLoad = true;
                                                                         this.dataTimesLoadFailed = true;
                                                                         this.checkLoadmaskDisable();
                                                                     }
                                                                 }, this);
        this.latestDataTimes.load();

        //
        // NOAA Wx Watches/Warnings
        //
        this.watchWarns = Ext.create("emdss.store.WxWatchWarns",{
            storeId:"watch-warns-store"
        });
        console.log("Done creating WxWatchWarns Store...");
            
        this.watchWarns.setupAfterListener();
        
        this.watchWarns.setProxy({
            type:'ajax',
            url:   emdss.util.Config.getWatchWarnsUrl()
                    + '&state=' + emdss.util.Config.getHrefState()
                    + ( emdss.util.Config.getUseVdt() ? '_vdt' : '' ),
            reader: {
                    type:         'json',
                    rootProperty: 'hazards'
                }
        });
        this.watchWarns.on({
            load:'onWatchWarnsLoad',
            scope:this
        });
        this.watchWarns.on({
            refresh:'onWatchWarnsRefresh',
            scope:this
        });
        this.watchWarns.getProxy().addListener('exception', function (proxy, response, operation) {
                                                                console.log(response.status);
                                                                console.log(response.responseText);
                                                                this.watchWarnsAttemptedLoad = true;
                                                                this.watchWarnsLoadFailed = true;
                                                                this.checkLoadmaskDisable();
                                                            }, this);
        this.watchWarns.load();

        //
        // Start the timer for updating the current time
        //
        this.startClockTimer(1000);

        //
        // Set up the listeners
        // (bad style)
        //
        // this.control({
        //     '#hide-show-vehicles-button': {
        //         release: function(button, released, eOpts) {
        //             console.log('hide-show-vehicles-button button : ' + released);
        //             me.onHideShowVehiclesRelease(button, released, eOpts);
        //         }
        //     }
        // });
    },

    /**
     * Set up some UI callbacks.
     */
    launch: function() {
        var tabPanel = this.getMainTabPanel();
        tabPanel.on('activeitemchange', this.onTabChange, this);
    },

    /**
     * Not currently used.
     */
    // onConfigClick: function(btn, e, eOpts) {
    //     if (!this.configMenu) {
    //         this.configMenu = Ext.create("emdss.view.ConfigMenu",{
    //         });
    //         var list = Ext.getCmp('config-menu-list');
    //         list.on('select',Ext.bind(this.onConfigMenuClick,this));
    //     }
    //     this.configMenu.showBy(btn);
    // },

    /**
     * Not currently used.
     */
    // onConfigMenuClick: function(list, rec, eOpts) {
    //     var l = rec.data.label;
    //     list.deselectAll();
    //     this.configMenu.hide();
    //     if (l == emdss.util.Config.getDateMenuLabel()) {
    //         if (!this.datePicker) {
    //         this.datePicker = Ext.Viewport.add({
    //             xtype:'datepicker',
    //             value:new Date(),
    //             yearFrom:2006
    //         });
    //         this.datePicker.on("change",Ext.bind(this.onDateChange,this));
    //         }
    //         this.datePicker.show();
    //     }
    // },

    /**
     * Not currently used.
     */
    // onDateChange: function(picker, value, eOpts) {
    //     var dateStr = Ext.Date.format(value, "Ymd.Hi");
    //     this.currDate = dateStr;
    //     if (this.siteData) {
    //         this.siteData.setDate(dateStr);
    //         this.siteData.loadData();
    //     }
    //     if (this.sitePlots) {
    //         this.sitePlots.setDate(dateStr);
    //         this.sitePlots.loadData();
    //     }
    //     this.siteStore.setDate(dateStr);
    //     this.siteStore.loadData();
    // },

    /**
     * Handle load of segments file -- called after the segments are done loading.
     */
    onRoadSegmentsUpdate: function() {
        console.log("Updated road segments...");

        // Check for delayed action on adding sites to map.
        if ( !this.roadSegmentsLoaded ) {
            this.roadSegmentsLoaded = true;
            
            var sitesController = emdss.app.getController('emdss.controller.RoadAndRwisSites');
            sitesController.addLatestSitesDataToMap();
        }

        if ( !this.roadSegmentsAttemptedLoad ) {
            this.roadSegmentsAttemptedLoad = true;
            this.roadSegmentsLoadFailed = false;
            this.checkLoadmaskDisable();
        }
    },
        
    /**
     * Handle load of new data time from server -- called after the data times store is done loading.
     *   - Update the UI with the new available data time for the "district_alerts" directory
     */
    onDataTimesUpdate: function(store, data, eOpts) {

        console.log("Updated latest data times...");

        records_time = data.items;
        for (var i = 0; i < records_time.length; i++) {
            var r = records_time[i];
            var directory = r.data.dir;
            var latest_time = r.data.latest_time;
            console.log("    Dir: " + directory + ", LatestTime: " + latest_time);

            if (directory == 'district_alerts') {
                // Get the time string in human-readable format
                var valid_time = emdss.util.Util.emdssStringToDate('' + latest_time);
                var valid_time_string = this.getPrettyDateStringFromDate(valid_time);

                // Update the valid time widget
                var valid_time_label = this.getLabelMapValidTime();
                valid_time_label.setHtml('Data valid: ' + valid_time_string);
            }
        }

        if ( !this.dataTimesAttemptedLoad ) {
            this.dataTimesAttemptedLoad = true;
            this.dataTimesLoadFailed = false;
            this.checkLoadmaskDisable();
        }
    },
    
    /**
     * Handle load of new NWS watches and warnings from server -- called after the WxWatchWarns store is done loading.
     *   - For debug purposes
     */
    onWatchWarnsLoad: function(store, data, eOpts) {
        
        console.log("Loaded watches and warnings...");
        
        var records = data;
        for (var i = 0; i < records.length; i++) {
            var watchWarn = records[i];
            var coords = watchWarn.coords(); // returns Ext.Store of WatchWarnCoords
            // debugger;
        }
        
        // debugger;
        
        var map = this.getMapView().getMap();
        this.addWatchWarnsToMap(records, map);

        if ( !this.watchWarnsAttemptedLoad ) {
            this.watchWarnsAttemptedLoad = true;
            this.watchWarnsLoadFailed = false;
            this.checkLoadmaskDisable();
        }
    },
    
    /**
     * Handle update of new NWS watches and warnings from server -- called after the WxWatchWarns store is done updating.
     *   - For debug purposes
     */
    onWatchWarnsRefresh: function(store, data, eOpts) {
        
        console.log("Refreshed watches and warnings...");
        
        return;
        
        var records = data.items;
        for (var i = 0; i < records.length; i++) {
            var watchWarn = records[i];
            var coords = watchWarn.coords(); // returns Ext.Store of WatchWarnCoords
            // debugger;
        }
        
        var map = this.getMapView().getMap();
        this.addWatchWarnsToMap(records, map);

        if ( !this.watchWarnsAttemptedLoad ) {
            this.watchWarnsAttemptedLoad = true;
            this.watchWarnsLoadFailed = false;
            this.checkLoadmaskDisable();
        }
    },
    
    /**
     * Update the current time indication in the UI.
     */
    updateCurrentTime: function() {
        // Update the current time widget
        var curr_time_label = this.getLabelMapCurrTime();
        
        if ( !curr_time_label ) {
            return;
        }
        
        curr_time_label.setHtml('Current time: ' + this.getPrettyDateStringFromDate(new Date()));
    },

    /**
     * Convert from a date object to a nice "mm/dd hh:mm am/pm" string
     * @param {Object} dateObj  The date object
     */
    weekday: ["Sun", "Mon", "Tues", "Weds", "Thurs", "Fri", "Sat"],
    getPrettyDateStringFromDate: function (dateObj) {
        var me = this;
        var ap = 'am';
        var hrs = dateObj.getHours();
        if ( hrs >= 12 ) {
            if ( hrs > 12 ) {
                hrs -= 12;
            }
            ap = 'pm';
        }
        var month = dateObj.getMonth() + 1;
        var day = dateObj.getDate();
        var mins = dateObj.getMinutes();
        var secs = dateObj.getSeconds();
        var dateString = ' ' + month + '/' + day + ' ' +
        hrs + ':' + (mins >= 10 ? mins : '0' + mins) +
        ':' + (secs >= 10 ? secs : '0' + secs) + ' ' + ap;
        return dateString;
    },

    checkLoadmaskDisable: function() {
        var me = this;
        var siteController = emdss.app.getController('emdss.controller.RoadAndRwisSites');

        console.log("");
        console.log("    dataTimesAttemptedLoad: " + me.dataTimesAttemptedLoad);
        console.log("    watchWarnsAttemptedLoad: " + me.watchWarnsAttemptedLoad);
        console.log("    siteListAttemptedLoad: " + siteController.siteListAttemptedLoad);
        console.log("    roadSegmentsAttemptedLoad: " + me.roadSegmentsAttemptedLoad);
        console.log("    travelTimesAttemptedLoad: " + me.travelTimesAttemptedLoad);
        console.log("    travelTimesListAttemptedLoad: " + me.travelTimesListAttemptedLoad);
        console.log("");

        if ( me.dataTimesAttemptedLoad &&
             me.watchWarnsAttemptedLoad &&
             siteController.siteListAttemptedLoad &&
             me.roadSegmentsAttemptedLoad ) {

            if ( me.dataTimesLoadFailed ) {
                if ( !me.dataTimesMsgShown ) {
                    me.dataTimesMsgShown = true;
                    Ext.Msg.alert('Initialization Error', 'Could not access latest data time service. Please try again later.', Ext.emptyFn);
                }
            }
            else if ( me.watchWarnsLoadFailed ) {
                if ( !me.watchWarnsMsgShown ) {
                    me.watchWarnsMsgShown = true;
                    Ext.Msg.alert('Initialization Error', 'Could not access NOAA Watches/Warnings service. Please try again later.', Ext.emptyFn);
                }
            }
            else if ( me.roadSegmentsLoadFailed ) {
                if ( !me.roadSegmentsMsgShown ) {
                    me.roadSegmentsMsgShown = true;
                    Ext.Msg.alert('Initialization Error', 'Could not access the road segment coordinates list for ' + emdss.util.Config.getHrefState() + '. Please notify tech support.', Ext.emptyFn);
                }
            }
            // else if ( me.travelTimesLoadFailed ) {
            //     if ( !me.travelTimesMsgShown ) {
            //         me.travelTimesMsgShown = true;
            //         Ext.Msg.alert('Initialization Error', 'Could not access the travel times for ' + emdss.util.Config.getHrefState() + '. Please notify tech support.', Ext.emptyFn);
            //     }
            // }
            // else if ( me.travelTimesListLoadFailed ) {
            //     if ( !me.travelTimesListMsgShown ) {
            //         me.travelTimesListMsgShown = true;
            //         Ext.Msg.alert('Initialization Error', 'Could not access the travel times LIST for ' + emdss.util.Config.getHrefState() + '. Please notify tech support.', Ext.emptyFn);
            //     }
            // }
            else if ( siteController.siteListLoadFailed ) {
                if ( !siteController.siteListMsgShown ) {
                    siteController.siteListMsgShown = true;
                    Ext.Msg.alert('Initialization Error', 'Could not access district alerts service. Please try again later.', Ext.emptyFn);
                }
            }
            else {
                Ext.Viewport.setMasked(false);
            }
        }
    },
    
    /**
     * Refresh the NWS Watches / Warnings on the map with data from the passed-in records.
     */
    addWatchWarnsToMap: function(records, map) {
        var me = this;
        // debugger;
        
        if ( !records ) {
            console.log("Skipping controller.Application.addWatchWarnsToMap() because records are undefined...");
            return;
        }

        if ( map == null ) {
            console.log("Warning:");
            console.log("Warning: Map is null in addWatchWarnsToMap(). Would have added " + records.length + " watches/warnings...");
            console.log("Warning:");

            return;
        }
        
        // Delete old polygons
        var lt;
        if (me.polys) {
            for (var i = 0; i < me.polys.length; i++) {
                me.polys[i].setMap(null);
            }
        }
        me.polys = [];
    
        for ( var key in records ) {
            var watchWarn = records[key];
            
            var coords = watchWarn.coords(); // returns Ext.Store of WatchWarnCoords
            
            var poly = [];
            coords.each(function (coord, index, length) {
                var coordObject = { 'lat': coord.get('lat'), 'lng': coord.get('lon') };
                poly.push( coordObject );
            });
            
            var gmPoly = new google.maps.Polygon({ paths: poly,
                                                   strokeColor: '#FF0000',
                                                   strokeOpacity: 0.8,
                                                   strokeWeight: 2,
                                                   fillColor: '#FF0000',
                                                   fillOpacity: 0.35
                                                   });
            gmPoly.setMap(map); 
            // gmPoly.set('id', site.get('site_num')); 
            me.polys.push(gmPoly); 
            google.maps.event.addListener(gmPoly, "click", Ext.bind(me.onPolyClick, me, [gmPoly,lt,watchWarn,map], true));
        }
    },
    
    onPolyClick: function(e, gmPoly, lt, rec, map) {
        console.log("    onPolyClick = " + rec.get('Weather Watch/Warning Text'));
        var me = this;
        var infowindow = new google.maps.InfoWindow({
            content: 'hub bub',
            position: e.latLng
        });
        // infowindow.setContent(rec.get('Weather Watch/Warning Text') + "<input type='button' onClick=me.getDir() value='Go!'>");
        infowindow.setContent('<a target=\"BLANK\" href=\"' + rec.get('Weather Watch/Warning Text') + '\">' + rec.get('Weather Watch/Warning Text') + '</a>');
        infowindow.open(map);
    },
    
    getDir: function() {
        console.log("getDir()");
    },

    /**
     * Handle time range tab changes by removing all map markers and adding new markers for the
     *   newly-selected alert time range.
     */
    onTabChange: function(container, value, oldValue, eOpts) {
        var me = this;

        var buttons = me.getBtnFcstType();
        var veh_button = me.getBtnVehicles();
        var rad_button = me.getBtnRadar();
        var green_segs_button = me.getBtnHideGreen();
        var toolbar = me.getMapToolbar();
        var radarPanel = me.getPanelRadar();
        var data_time_panel = me.getPanelMapTime();
        var travel_time_panel = me.getPanelDepartureTime();
        var travel_time_list_panel = me.getPanelDepartureTimeList();
        
        var wmsController = emdss.app.getController('emdss.controller.WmsData');
        
        if ( value.id == 'map-nav-view' ) {
            buttons.show();
            veh_button.show();
            rad_button.show();
            green_segs_button.show();
            toolbar.setTitle('Pikalert Map');
            data_time_panel.show();
            wmsController.showRadarPanel();
            travel_time_panel.hide();
            travel_time_list_panel.hide();
        }
        else if ( value.id == 'site-list' ) {
            buttons.hide();
            veh_button.hide();
            rad_button.hide();
            green_segs_button.hide();
            toolbar.setTitle('Pikalert Site List');
            data_time_panel.hide();
            radarPanel.hide();
            travel_time_panel.hide();
            travel_time_list_panel.hide();
        }
        else if ( value.id == 'vehicle-list' ) {
            buttons.hide();
            veh_button.hide();
            rad_button.hide();
            green_segs_button.hide();
            toolbar.setTitle('Pikalert Vehicle List');
            data_time_panel.hide();
            radarPanel.hide();
            travel_time_panel.hide();
            travel_time_list_panel.hide();
        }
        else if ( value.id == 'travel-time-list' ) {
            buttons.hide();
            veh_button.hide();
            rad_button.hide();
            green_segs_button.hide();
            toolbar.setTitle('Pikalert Travel Times');
            data_time_panel.hide();
            radarPanel.hide();
            travel_time_panel.show();
            travel_time_list_panel.hide();
            if ( me.travelTimesLoadFailed ) {
                if ( !me.travelTimesMsgShown ) {
                    me.travelTimesMsgShown = true;
                    Ext.Msg.alert('Initialization Error', 'Could not access the travel times for ' + emdss.util.Config.getHrefState() + '. Please notify tech support.', Ext.emptyFn);
                }
            }
        }
        else if ( value.id == 'travel-time-list-list' ) {
            buttons.hide();
            veh_button.hide();
            rad_button.hide();
            green_segs_button.hide();
            toolbar.setTitle('Pikalert Travel Times');
            data_time_panel.hide();
            radarPanel.hide();
            travel_time_panel.hide();
            travel_time_list_panel.show();
            if ( me.travelTimesListLoadFailed ) {
                if ( !me.travelTimesListMsgShown ) {
                    me.travelTimesListMsgShown = true;
                    Ext.Msg.alert('Initialization Error', 'Could not access the travel times LIST for ' + emdss.util.Config.getHrefState() + '. Please notify tech support.', Ext.emptyFn);
                }
            }
        }
    },

    /**
     * Handle clicks in the site list. This rather inelegant code looks around in the css hierarcy
     *   to determine exactly what was clicked.
     */
    onSiteSelect: function(list, index, node, record, senchaEvent) {
        var me = this;

        if ( senchaEvent.event.target.className === 'district_zoom' ) {
            var siblings = senchaEvent.event.target.parentElement.children;
            var districtName = siblings[0].innerText;
            var districtSiteStore = Ext.data.StoreManager.lookup('district-site-store');
            var idx = districtSiteStore.find( 'district_name', districtName, 0, false, true, true );
            if ( idx > -1 ) {
                var district = districtSiteStore.getAt(idx);
                var map = me.getMapView().getMap();
                this.zoomToDistrict(map, district);
            }
        }
        else if ( senchaEvent.event.target.className === 'site_description' ||
                  senchaEvent.event.target.className === 'site_alert' ||
                  senchaEvent.event.target.className === '' ) {
            var districtSiteStore = Ext.data.StoreManager.lookup('district-site-store');
            var siblings = senchaEvent.event.target.parentElement.children;
            if ( senchaEvent.event.target.className === '' ) {
                siblings = senchaEvent.event.target.parentElement.parentElement.children;
            }
            var siteDescription = siblings[0].innerText;
            districtSiteStore.each(function (district, index, length) {
                var siteStore = district.sites();
                var idx = siteStore.find( 'desc', siteDescription, 0, false, true, true );
                if ( idx > -1 ) {
                    var site = siteStore.getAt(idx);
                    me.pushShowSite(site, "site-list");
                    return;
                }
            });
        }
        else {
            // debugger;
        }
    },

    /**
     * Show the map toolbar (contains the forecast tabs, etc)
     *   and hide the navigation toolbar (containing the back button, etc)
     */
    showFcstToolbar: function(show) {
        var toolbar = this.getMapToolbar();
        toolbar.show();

        var tabPanel = this.getMainTabPanel();
        var navBar = tabPanel.getActiveItem().getNavigationBar();
        navBar.hide();
    },

    /**
     * Handle the callback indicating the map was rendered for the first time:
     *   - Set the map bounds (or center and zoom level)
     *   - Add all sites and vehicles to the map
     *   - Add click handler to show MAW alerts
     */
    onMapRendered: function(map) {
        // Set the initial map bounds
        var bounds = this.mapBounds;
        if ( bounds != undefined && bounds != null ) {
            map.fitBounds(bounds);
        }
        else {
            map.setCenter(emdss.util.Config.lookupDefaultCenter());
            map.setZoom(emdss.util.Config.lookupDefaultZoom());
        }

        var districtVehicleStore = Ext.data.StoreManager.lookup('district-vehicle-store');
        var vehicleRecords = [];
        districtVehicleStore.each(function (district, index, length) {
            vehicleRecords.push(district);
        });

        var districtSiteStore = Ext.data.StoreManager.lookup('district-site-store');
        var records = [];
        districtSiteStore.each(function (district, index, length) {
            records.push(district);
        });

        this.getApplication().getController('emdss.controller.VehicleController').addVehiclesToMap(vehicleRecords, map);
        if ( this.roadSegmentsLoaded ) {
            var siteController = emdss.app.getController('emdss.controller.RoadAndRwisSites');
            siteController.addSitesToMap(records, map);
        }
            
        var me = this;
        google.maps.event.addListener(map, 'click', function(event) {
                mapZoom = map.getZoom();
                startLocation = event.latLng;
                setTimeout("emdss.app.getController('emdss.controller.Application').handleMapClick()", 600);
            });

        console.log("Map Rendered!");
        console.log("Map Rendered!");
        console.log("Map Rendered!");
        console.log("Map Rendered!");
        console.log("Map Rendered!");
    },

    handleMapClick: function() {
        var me = this;
        var map = me.getMapView().getMap();
        if (mapZoom == map.getZoom()) {
            console.log("");
            console.log("Got single map click!!!");
            console.log("Got single map click!!! Location: " + startLocation);
            console.log("Got single map click!!!");
            console.log("");

            if ( !me.mawClicksActive ) {
                console.log("");
                console.log("Skipped maw click because they are not set as active...");
                return;
            }

            // Place a marker where the click occurred.
            if ( me.clickMarker ) {
                me.clickMarker.setMap(null);
                me.clickMarker = null;
            }
            var appRoot = window.location.href.substr( 0, window.location.href.indexOf('?') );
            var image = {
                url: appRoot + 'resources/icons/Click_Image.png',
                size: new google.maps.Size(14, 14),
                anchor: new google.maps.Point(7, 7)
            };
            var lt = new google.maps.LatLng(startLocation.lat(), startLocation.lng());
            var mark = new google.maps.Marker({
                position: lt,
                map:      map,
                icon: image
            });
            me.clickMarker = mark;

            var xhr = new XMLHttpRequest();
            var appHost = window.location.href.substr( 0, window.location.href.indexOf('/') );
            var requestUrl = appHost + '/maw_alerts_adhoc?path=/maw_alerts_adhoc&lat='
                + startLocation.lat()
                + '&lon='
                + startLocation.lng()
                // + '&bearing=270.0'
                + '&state=' + emdss.util.Config.getHrefState();
            xhr.open('GET', requestUrl);
            xhr.setRequestHeader("Cache-Control", "no-cache,max-age=0");
            xhr.setRequestHeader("Pragma", "no-cache");
            xhr.onload = function(e) {
                console.log("After load of MAW alert:");
                console.log("   alert:" + this.response);
                // this.onload.mapController. ...

                var mawResponse = JSON.parse(this.response);

                // Pop up a window with the info.
                if ( me.mawPopup ) {
                    me.mawPopup.destroy();
                    me.mawPopup = null;
                }

                me.mawPopup = me.getMapNavView().add({
                    xtype: 'panel',
            //          xtype: 'container',
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
            //          docked: 'left',
            //          floating: true,
            //          shadow: true,
        
                    width: Ext.os.deviceType == 'Phone' ? 260 : 520,
                    height: Ext.os.deviceType == 'Phone' ? 220 : 250,
                    // itemTpl: me.createVehiclePopupTpl(rec),
                    // data: rec,
                    // html: 'text.',
                    html: me.createMawPopupHtml(mawResponse, startLocation.lat(), startLocation.lng()),
                            // style : 'background-color: ' + me.getAlertColor(mawResponse) + ';',
                    items: [
                        {
                            docked: 'top',
                            xtype: 'toolbar',
                            title: 'MAW Alerts Click Tool'
                        }
                    ],
                    scrollable: true
                });
                me.mawPopup.show();
            };
            xhr.onload.applicationController = this.getApplication().getController('emdss.controller.Application');
            xhr.send();
        }
    },

    /**
     * Create the markup for the lastest vehicle observation.
     * @param {Object} record
     */
    createMawPopupHtml: function(record, clickedLat, clickedLon) {
    var html = '';
    if ( record == null ) {
            return 'No Record!';
        }

// debugger;

        html += "<p style=\"background-color:" + this.getAlertColor(record) + ";\">";
    var inRange = true;
        if ( record["alert_action_code"] != null ) {
            var val = parseFloat(record["alert_action_code"]);
            if ( isNaN(val) || val == -9999.0 ) {
                html += 'Out of range. Click closer to a monitored road segment';
            inRange = false;
            }
        }

    if ( inRange && record["alert_code_precip"] != null ) {
        var val = parseFloat(record["alert_code_precip"]);
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Precip Alert: Unknown';
            // else html += '<br>Precip Alert: ' + record["alert_code_precip"];
            else html += '<br>Precip Alert: '
                           + this.lookupPrecipAlert(record["alert_code_precip"]);
    }
    if ( inRange && record["alert_code_pavement"] != null ) {
        var val = parseFloat(record["alert_code_pavement"]);
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Road Condition Alert: Unknown';
            // else html += '<br>Road Condition Alert: ' + record["alert_code_pavement"];
            else html += '<br>Road Condition Alert: '
                           + this.lookupRoadcondAlert(record["alert_code_pavement"]);
    }
    if ( inRange && record["alert_code_visibility"] != null ) {
        var val = parseFloat(record["alert_code_visibility"]);
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Visibility Alert: Unknown';
            // else html += '<br>Visibility Alert: ' + record["alert_code_visibility"];
            else html += '<br>Visibility Alert: '
                           + this.lookupVisibilityAlert(record["alert_code_visibility"]);
    }
    if ( inRange && record["alert_action_code"] != null ) {
        var val = parseFloat(record["alert_action_code"]);
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Recommended Action: Unknown';
            // else html += '<br>Recommended Action: ' + record["alert_action_code"];
            else html += '<br>Recommended Action: ' + this.lookupActionAlert(record["alert_action_code"]);
    }
    // if ( record.get('obs_time') ) {
        // var obs_time = new Date(record.get('obs_time') * 1000);
        // html += '<br>Observed at: ' + emdss.util.Util.dateToBriefString(obs_time);
    // }
    
    if ( !inRange || html.length < 1 ) {
        html += '<br>&nbsp;<br>No Data Available...';
    }

        html += '<br>&nbsp;';
        html += '<br>Lat: ' + clickedLat;
        html += '<br>Lon: ' + clickedLon;

        html += "<br>&nbsp;<br>&nbsp;<br>&nbsp;<br>&nbsp;<br>&nbsp;<br>&nbsp;</p>";

    return html;
    },

    lookupPrecipAlert: function(code) {
        if ( code == 0 ) {
            return 'clear';
        }
        else if ( code == 1 ) {
            return 'light rain';
        }
        else if ( code == 2 ) {
            return 'moderate rain';
        }
        else if ( code == 3 ) {
            return 'heavy rain';
        }
        else if ( code == 4 ) {
            return 'light rain/snow mix';
        }
        else if ( code == 5 ) {
            return 'moderate rain/snow mix';
        }
        else if ( code == 6 ) {
            return 'heavy rain/snow mix';
        }
        else if ( code == 7 ) {
            return 'light snow';
        }
        else if ( code == 8 ) {
            return 'moderate snow';
        }
        else if ( code == 9 ) {
            return 'heavy snow';
        }
        else {
            return 'error: unexpected code - ' + code;
        }
    },

    lookupRoadcondAlert: function(code) {
        if ( code == 0 ) {
            return 'clear';
        }
        else if ( code == 1 ) {
            return 'wet roads';
        }
        else if ( code == 2 ) {
            return 'snowy roads';
        }
        else if ( code == 3 ) {
            return 'snowy, slick roads';
        }
        else if ( code == 4 ) {
            return 'icy roads';
        }
        else if ( code == 5 ) {
            return 'icy, slick roads';
        }
        else if ( code == 6 ) {
            return 'hydroplaning possible';
        }
        else if ( code == 7 ) {
            return 'black ice';
        }
        else if ( code == 8 ) {
            return 'icy roads possible';
        }
        else if ( code == 9 ) {
            return 'icy, slick roads possible';
        }
        else {
            return 'error: unexpected code - ' + code;
        }
    },

    lookupVisibilityAlert: function(code) {
        if ( code == 0 ) {
            return 'clear';
        }
        else if ( code == 1 ) {
            return 'low visibility';
        }
        else if ( code == 2 ) {
            return 'blowing snow';
        }
        else if ( code == 3 ) {
            return 'fog';
        }
        else if ( code == 4 ) {
            return 'haze';
        }
        else if ( code == 5 ) {
            return 'dust';
        }
        else if ( code == 6 ) {
            return 'smoke';
        }
        else {
            return 'error: unexpected code - ' + code;
        }
    },

    lookupActionAlert: function(code) {
        if ( code == 0 ) {
            return 'none';
        }
        else if ( code == 1 ) {
            return 'use caution';
        }
        else if ( code == 2 ) {
            return 'drive slowly and use caution';
        }
        else if ( code == 3 ) {
            return 'delay travel, seek alternate route, or drive slowly and use extreme caution';
        }
        else {
            return 'error: unexpected code - ' + code;
        }
    },

    getAlertColor: function(record) {
    var color = '#ffffff';

        if ( record["alert_action_code"] != null ) {
            var val = parseFloat(record["alert_action_code"]);
            if ( !isNaN(val) && val != -9999.0 ) {
                if ( val == 0 ) {
                    color = '#4ca64c';
                }
                else if ( val == 1 ) {
                    color = '#ffd27f';
                }
                else if ( val == 2 ) {
                    color = '#ffa500';
                }
                else if ( val == 3 ) {
                    color = '#bf0000';
                }
            }
        }

        return color;
    },

    /**
     * Zoom the map to show the bounds of the district
     * @param {Object} map - the google maps object
     * @param {Object} district - the district to zoom to
     */
    zoomToDistrict: function(map, district) {
        //
        // Switch to the map view.
        //
        var tabPanel = this.getMainTabPanel();
        tabPanel.setActiveItem(1);

        if ( district == null || district == undefined ) {
            console.log('Error: no district in zoomToDistrict()');
            return;
        }

        //
        // Calculate the districtBounds. Cache bounds in case map hasn't drawn yet.
        //
        var min = new google.maps.LatLng(district.get('min_lat'), district.get('min_lon'));
        var max = new google.maps.LatLng(district.get('max_lat'), district.get('max_lon'));
        var bounds = new google.maps.LatLngBounds(min, max);
        this.mapBounds = bounds;

        if ( map == null || map == undefined ) {
            console.log('No map yet in zoomToDistrict()');
            return;
        }

        map.fitBounds(bounds);
    },

    /**
     * Handle the display of a new site detail view by showing the navigation bar.
     */
    onPush: function() {
        this.getNavbar().setHidden(false);
    },

    /**
     * Handle the closing of the site detail view by hiding the navigation bar.
     */
    onBack: function() {
        this.getNavbar().setHidden(true);
    },

    /**
     * Start the clock update timer.
     * @param {Object} interval_ms - the update interval in milliseconds
     */
    startClockTimer: function(interval_ms) {
        var me = this,
        clockTask = me.clockTask;
        this.stopClock = false;

        if (!clockTask) {
            clockTask = me.clockTask = {
                    run: function() {
                        var stop = me.getStopClock();
                        if (stop) {
                            console.log("Stopping clock task...");
                            runner.stop(clockTask);
                        } else {
                            me.updateCurrentTime();
                        }
                    },
                    interval: interval_ms
                };
            var runner = new emdss.util.TaskRunner();
            runner.start(clockTask);
        }
    },

    /**
     * Stop the clock update timer.
     */
    stopClockTimer: function() {
        console.log("Setting flag to stop clock task on next run cycle...");
        this.stopClock = true;
        this.clockTask = null;
    },

});
