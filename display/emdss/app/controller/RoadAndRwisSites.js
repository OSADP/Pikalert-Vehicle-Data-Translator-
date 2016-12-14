/**
 * Main Application controller -- start data loading timers. build the map,
 *   and handle UI interactions.
 * @author paddy
 */
Ext.define('emdss.controller.RoadAndRwisSites', {
    extend:'Ext.app.Controller',
    requires:[
        'emdss.util.Config',
        'emdss.controller.VehicleController'
    ],
    config:{
        refs:{
            mainTabPanel: 'main[id=main-tab-panel]',
            mapView: 'main map',
            mapToolbar: 'main toolbar[id=map-toolbar]',
            btnFcstType: 'main segmentedbutton[id=fcst-type-select]',
            btnHideGreen: 'main button[id=hide-green-segments-button]',
            listItemSites: 'main sites[id=site-list-item]',
            panelRadar: 'main panel[id=radar-panel]'
            // FYI, This also works...
            // listSites: '#site-list-item'
        },
        clockTask: null,
        stopClock: false,
        
        control: {
            'btnFcstType': {
                toggle: 'onFcstTypeSelect'
            },
            'btnHideGreen': {
                tap: 'onHideGreenTap'
            },
            'listItemSites': {
                itemtap: 'onSiteSelect'
            }
        }
    },
    latest_data: null,
    hideGreenSegments: false,

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
        var appController = emdss.app.getController('emdss.controller.Application');

        console.log("In RoadAndRwisSites::init()...");
        
        // 
        // Sites and Alerts
        //
        me.setDistrictSiteStore(
            Ext.create("emdss.store.DistrictSiteStore",
                {
                    storeId:"district-site-store",
                    constructor: function(config) {
                        console.log(config);
                        
                        console.log("In DistrictSiteStore constuctor()...");
                        
                        this.callParent([config]);
                    },
                    init:function() {
                        console.log("In DistrictSiteStore init...");
                    }
            })
        );
            
        console.log("Done creating DistrictSiteStore...");

        me.getDistrictSiteStore().setupAfterListener();

        me.getDistrictSiteStore().setProxy({
            type:'ajax',
            url: emdss.util.Config.getDistrictAlertsUrl()
                    + '&state=' + emdss.util.Config.getHrefState()
                    + ( emdss.util.Config.getUseVdt() ? '_vdt' : '' ),
            reader: {
                    type:         'json',
                    rootProperty: 'districts'
                },
            timeout: emdss.util.Config.getDistrictAlertLoadTimeout()
        });

        me.getDistrictSiteStore().on({
            refresh: 'onSiteListUpdate',
            scope: this
        });

        //
        // Set the alert generation time on each district after load...
        //
        me.getDistrictSiteStore().on({
            load: 'onSiteListLoad',
            scope: this
        });
        // this.getDistrictSiteStore().on({
        //     load: 'setDataTimeOnDistricts',
        //     scope: this.getDistrictSiteStore()
        // });

        // 
        // Add an exception handler to the load proxy
        // 
        me.getDistrictSiteStore().getProxy().addListener('exception', function (proxy, response, operation) {
                                                                       console.log(response.status);
                                                                       console.log(response.responseText);
                                                                       me.siteListAttemptedLoad = true;
                                                                       me.siteListLoadFailed = true;
                                                                       appController.checkLoadmaskDisable();
                                                                   }, me);
        // 
        // Load the store and start the reload timer
        //
        me.getDistrictSiteStore().load();
        // this.getDistrictSiteStore().startReloadTimer(300000); // 300000 ms == 5 min
        me.getDistrictSiteStore().startReloadTimer(60000); // 60000 ms == 1 min

        //
        // Initialize the display control information
        //
        me.selectedSite = '';
        me.selectedSiteIsRwis = null;
        me.currentAlertType = 'hr06_alert_code';

        //
        // Set up the listeners
        //   (bad style)
        //
        // me.control({
        //     '#fcst-type-select': {
        //         toggle: function(segbutton, button, isPressed, eOpts) {
        //             console.log('fcst-type-select button: ' + button.getText() + ' isPressed: ' + isPressed);
        //             if ( isPressed ) {
        //                 me.onFcstTypeSelect(segbutton, button, isPressed, eOpts);
        //             }
        //         }
        //     },
        //     '#site-list-item': {
        //         itemtap: function(list, index, node, record, senchaEvent) {
        //             console.log('site-list-item itemtap: ' + senchaEvent.event.target.className );
        //             me.onSiteSelect(list, index, node, record, senchaEvent);
        //         }
        //     },
        // });
    },

    /**
     * Set up any UI callbacks here...
     */
    launch: function() {
        // 
        // See if the user was hiding green segments for the previous session...
        // 
        if ( emdss.util.Util.supports_html5_storage() ) {
            var hideGreenSegments = window.localStorage.getItem('hideGreenSegments');
            if ( hideGreenSegments == 'true' ) {
                var btn = Ext.getCmp("hide-green-segments-button");
                var controller = emdss.app.getController('emdss.controller.RoadAndRwisSites');
                btn.setHtml( emdss.util.Config.getShowGreenSegmentsLabel() );
                controller.hideGreenSegments = true;
            }
        }
    },

    /**
     * Return the appropriate origin for each alert code icon within the image sprite.
     *   Unfortunately this is a second representation of information that's also represented
     *   in css. The css version is used in the itemTpl for the sites list.
     */
    getAlertImageOffset: function(alertCode, isRwisSite, isSelected) {
        var vertOffset = 0;
        var horizOffset = 0;

        if (isSelected) {
            vertOffset = 14;
        }

        if (isRwisSite) {
            horizOffset = 56;
        }

        if ( alertCode == null || alertCode == undefined || alertCode == 'missing' ) {
            return new google.maps.Point(horizOffset + 0, vertOffset);
        }
        else if ( alertCode == 'clear' ) {
            return new google.maps.Point(horizOffset + 14, vertOffset);
        }
        else if ( alertCode == 'warning' ) {
            return new google.maps.Point(horizOffset + 28, vertOffset);
        }
        else if ( alertCode == 'alert' ) {
            return new google.maps.Point(horizOffset + 42, vertOffset);
        }
    },
    
    /**
     * Return the appropriate color for each alert code.
     */
    lookupAlertColor: function(alertCode) {
        
        if ( alertCode == null || alertCode == undefined || alertCode == 'missing' ) {
            return '#BCBCBC';
        }
        else if ( alertCode == 'clear' ) {
            return '#26DD25';
        }
        else if ( alertCode == 'warning' ) {
            return '#FFEE2E';
        }
        else if ( alertCode == 'alert' ) {
            return '#E60214';
        }
    },

    /**
     * Handle selection changes to the forecast tabs.
     *   - Remove all site markers from the map
     *   - Add new site markers to the map representing the selected forecast time range
     */
    onFcstTypeSelect: function(sbtn, btn, isPressed, eOpts) {
        var me = this;

        var label = btn.getText();

        if (label == emdss.util.Config.getObsFcstLabel()) {
            me.currentAlertType = 'obs_alert_code';
        } else if (label == emdss.util.Config.getNext06HrsFcstLabel()) {
            me.currentAlertType = 'hr06_alert_code';
        } else if (label == emdss.util.Config.getNext24HrsFcstLabel()) {
            me.currentAlertType = 'hr24_alert_code';
        } else if (label == emdss.util.Config.getHrs24to72FcstLabel()) {
            me.currentAlertType = 'hr72_alert_code';
        }

        // var map = Ext.getCmp("map-view").getMap();
        var map = me.getMapView().getMap();

        var siteRecords = this.latest_data;
        this.addSitesToMap(siteRecords, map);
                
        if ( me.selectedSite ) {
            // this.styleSelectedSite(this.selectedSite);

            // All styling has been removed except road segment centerline, so add select styling.
            if ( me.selectedSiteIsRwis ) {
                me.addSelectStylingToRwis(me.selectedSite);
            }
            else {
                if ( me.selectedSegmentCenter ) {
                    me.selectedSegmentCenter.setMap(null);
                    me.selectedSegmentCenter = null;
                }

                me.addSelectStylingToRoadSite(me.selectedSite);
            }
        }
    },
    
    /**
     * Handle clicks to the "Hide Green Segments" button.
     *   - Remove all site markers from the map
     *   - Add new site markers to the map, with green ones omitted.
     */
    onHideGreenTap: function(btn, event, eOpts) {
        var me = this;

        var label = btn.getHtml();

        if (label == emdss.util.Config.getHideGreenSegmentsLabel()) {
            btn.setHtml(emdss.util.Config.getShowGreenSegmentsLabel())
            me.hideGreenSegments = true;
        } else if (label == emdss.util.Config.getShowGreenSegmentsLabel()) {
            btn.setHtml(emdss.util.Config.getHideGreenSegmentsLabel())
            me.hideGreenSegments = false;
        }
               
        if ( emdss.util.Util.supports_html5_storage() ) {
            window.localStorage.setItem('hideGreenSegments', me.hideGreenSegments);
        }
        
        var map = me.getMapView().getMap();

        var siteRecords = this.latest_data;
        this.addSitesToMap(siteRecords, map);
    },

    /**
     * Not currently used.
     */
    // onSiteTypeSelect: function(sbtn, btn, isPressed, eOpts) {
    //     var label = btn.getText();
    //     this.siteStore.clearFilter();
    //     if (label == emdss.util.Config.getAllSitesLabel()) {
    //         //do nothing, filter already cleared
    //     } else if (label == emdss.util.Config.getRoadCondSitesLabel()) {
    //         this.siteStore.filter("is_road_cond_site",true);
    //     } else if (label == emdss.util.Config.getWxObsSitesLabel()) {
    //         this.siteStore.filter("is_wx_obs_site",true);
    //     } else if (label == emdss.util.Config.getRwisSitesLabel()) {
    //         this.siteStore.filter("is_rwis_site",true);
    //     } else if (label == emdss.util.Config.getHideSitesLabel()) {
    //         this.siteStore.filter("is_rwis_site","na"); //make up value that it will not be to filter out all sites
    //     }
    // },

    /**
     * Not currently used.
     */
    // onAlertTypeSelect: function(sbtn, btn, isPressed, eOpts) {
    //     var label = btn.getText();
    //     console.log(label);
    //     if (label == emdss.util.Config.getWxAlertsLabel()) {
    //         this.alertType = "wx";
    //     } else if (label == emdss.util.Config.getRoadCondAlertsLabel()) {
    //         this.alertType = "road_cond";
    //     } else if (label == emdss.util.Config.getObsAlertsLabel()) {
    //         this.alertType = "obs";
    //     }
    //     this.siteStore.setAlertType(this.alertType);
    //     this.siteStore.loadData();
    // },

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
    //     if (this.sitePlot) {
    //         this.sitePlot.setDate(dateStr);
    //         this.sitePlot.loadData();
    //     }
    //     this.siteStore.setDate(dateStr);
    //     this.siteStore.loadData();
    // },

    /**
     * Handle refresh of the DistrictSiteStore. Called after the data is refreshed.
     *   Replace all the site markers on the map.
     */
    onSiteListUpdate: function(store, data, eOpts) {
        console.log("In controller.RoadAndRwisSites.onSiteListUpdate()...");

        var appController = emdss.app.getController('emdss.controller.Application');
        
        this.latest_data = data.items;
        var records = data.items;
        // var map = Ext.getCmp("map-view").getMap();
        var map = this.getMapView().getMap();
        if ( appController.roadSegmentsLoaded ) {
            this.addSitesToMap(records, map);
        }

        if ( !this.siteListAttemptedLoad ) {
            this.siteListAttemptedLoad = true;
            this.siteListLoadFailed = false;
            appController.checkLoadmaskDisable();
        }
    },

    /**
     * Handle load of the DistrictSiteStore. Called after the data is loaded.
     *   Call the district site store to set the data time on each district
     *   Replace all the site markers on the map.
     */
    onSiteListLoad: function(store, data, eOpts) {
        console.log("");
        console.log("In controller.RoadAndRwisSites.onSiteListLoad()...");
        console.log("");

        var appController = emdss.app.getController('emdss.controller.Application');

        // Have the sites store set the data time on each district
        this.getDistrictSiteStore().setDataTimeOnDistricts(store, data, eOpts);

        // Replace all the map markers with up-to-date alert icons
        this.latest_data = data;
        var records = data;
        // var map = Ext.getCmp("map-view").getMap();
        var map = this.getMapView().getMap();
        if ( appController.roadSegmentsLoaded ) {
            this.addSitesToMap(records, map);
        }

        if ( !this.siteListAttemptedLoad ) {
            this.siteListAttemptedLoad = true;
            this.siteListLoadFailed = false;
            appController.checkLoadmaskDisable();
        }
    },

    /**
     * Add the latest sites data to the map, if they are loaded
     */
    addLatestSitesDataToMap: function() {
        var siteRecords = this.latest_data;
        if ( siteRecords ) {
            // var map = Ext.getCmp("map-view").getMap();
            var map = this.getMapView().getMap();
            this.addSitesToMap(siteRecords, map);
        }
    },
    
    /**
     * Refresh the site markers on the map with data from the passed-in records.
     */
    addSitesToMap: function(records, map) {
        var me = this;

        if ( !records ) {
            console.log("Skipping controller.Application.addSitesToMap() because records are undefined...");
            return;
        }

        if ( map == null ) {
            console.log("Warning:");
            console.log("Warning: Map is null in addSitesToMap(). Would have added sites from " + records.length + " districts...");
            console.log("Warning:");
 
            return;
        }

        // Road Segment Coordinates
        var appController = emdss.app.getController('emdss.controller.Application');
        var rawSegments = appController.rawRoadSegments;
        if ( rawSegments == null || rawSegments.segments == null || rawSegments.segments.length < 1 ) {
            console.log("Warning:");
            console.log("Warning: rawSegments is null in addSitesToMap(). Would have added sites from " + records.length + " districts...");
            console.log("Warning:");

            return;
        }

        var lt;
        if (me.markers) {
            for (var i = 0; i < this.markers.length; i++) {
                me.markers[i].setMap(null);
            }
        }
        me.markers = [];

        if (me.segments) {
            for (var i = 0; i < this.segments.length; i++) {
                me.segments[i].setMap(null);
            }
        }
        me.segments = [];

        if (me.backgroundSegments) {
            for (var i = 0; i < this.backgroundSegments.length; i++) {
                me.backgroundSegments[i].setMap(null);
            }
        }
        me.backgroundSegments = [];

        for ( var key in records ) {
            var district = records[key];

            // Determine the currently-selected alert time range.
            var timeKey = 'hr06_alert_code';
            if ( me.currentAlertType ) {
                timeKey = me.currentAlertType;
            }

            var sites = district.sites(); // returns Ext.Store of sites...
            var appRoot = window.location.href.substr( 0, window.location.href.indexOf('?') );
            sites.each(function (site, index, length) {

                if ( site.get(timeKey) == 'clear' && 
                     site.get('is_rwis_site') == false &&
                     me.hideGreenSegments == true ) {
                         
                    return true;
                }
                
                var lat = site.get('lat');
                var lon = site.get('lon');
                var desc = site.get('desc');

                if ( site.get('is_rwis_site') ) {                      
                    var image = {
                        url: appRoot + 'resources/icons/Alert_Images.png',
                        size: new google.maps.Size(14, 14),
                        origin: me.getAlertImageOffset( site.get(timeKey), site.get('is_rwis_site'), false),
                        anchor: new google.maps.Point(7, 7)
                    };

                    lt = new google.maps.LatLng(lat,lon);
                    var mark = new google.maps.Marker({
                        position: lt,
                        map:      map,
                        icon:     image
                    });
                    mark.set('id', site.get('site_num'));
                    me.markers.push(mark);
                    google.maps.event.addListener(mark, "click", Ext.bind(me.onMarkerClick, me, [mark,lt,site,map], true));
                }
                else {
                    // Plot the segment for this site.
                    var matchingSegment = me.findSegment(rawSegments.segments, site.get('site_num'));
                    if ( matchingSegment ) {
                        var coords = matchingSegment.coordinates;
                        var coordArray = [];
                        for (var c = 0; c < coords.length; c++) {
                            coordArray.push( {'lat': coords[c].lat, 'lng': coords[c].lon} );
                        }

                        var gmSeg = new google.maps.Polyline({
                            path: coordArray,
                            geodesic: true,
                            zIndex: 1,
                            strokeColor: me.lookupAlertColor(site.get(timeKey)),
                            strokeOpacity: 1.0,
                            strokeWeight: 2
                        });
                        var gmSeg2 = new google.maps.Polyline({
                            path: coordArray,
                            geodesic: true,
                            zIndex: 0,
                            strokeColor: '#000000',
                            strokeOpacity: 0.1,
                            strokeWeight: 12
                        });

                        gmSeg.set('id', site.get('site_num'));
                        gmSeg2.set('id', site.get('site_num'));
                        me.segments.push(gmSeg);
                        me.backgroundSegments.push(gmSeg2);
                        gmSeg.setMap(map);
                        gmSeg2.setMap(map);
                        google.maps.event.addListener(gmSeg, "click", Ext.bind(me.onMarkerClick, me, [gmSeg,coordArray,site,map], true));
                        google.maps.event.addListener(gmSeg2, "click", Ext.bind(me.onMarkerClick, me, [gmSeg,coordArray,site,map], true));
                    }
                    else {
                        console.log("XXXXXXXXXX Could not find segment for site: " + site.get('site_num'));
                    }
                }
            }); // end of sites.each()

        } // end of districts iteration

        // Remove any previous selection styling to road segments
        if ( me.selectedSegmentCenter ) {
            me.selectedSegmentCenter.setMap(null);
            me.selectedSegmentCenter = null;
        }

        // Restore styling to any previously-selected site (RWIS or Segment)
        if ( me.selectedSite ) {
            if ( me.selectedSiteIsRwis ) {
                me.addSelectStylingToRwis(me.selectedSite);
            }
            else {
                me.addSelectStylingToRoadSite(me.selectedSite);
            }
        }
    },

    // onSegmentClick: function(e, seg, coordArray, rec, map) {
    //     console.log("Got segment click!!!");
    // 
    //     // Display the site information
    //     this.pushShowSite(rec, "map-nav-view");
    // },

    /**
     * Handle site clicks:
     *   - Update the selected site with a special marker image
     *   - Remove the special marker image from the previously-selected site
     *   - Open the site detail view for the selected site
     */
    onMarkerClick: function(e, mark, lt, rec, map) {
        console.log("    selected site_num = " + rec.get('site_num'));
        var me = this;
        var isRwis = rec.get('is_rwis_site');

        // If there is a previously-selected site, remove styling
        if ( me.selectedSite ) {
            if ( me.selectedSiteIsRwis ) {
                me.removeSelectStylingFromRwis(me.selectedSite);
            }
            else {
                if ( me.selectedSegmentCenter ) {
                    me.selectedSegmentCenter.setMap(null);
                    me.selectedSegmentCenter = null;
                }

                me.removeSelectStylingFromRoadSite(me.selectedSite);
            }
        }

        // Style the selected site (RWIS or Segment)
        // me.styleSelectedSite(rec.get('site_num'));

        me.selectedSite = rec.get('site_num');
        me.selectedSiteIsRwis = isRwis;

        // Style the selected site (RWIS or Segment)
        if ( me.selectedSiteIsRwis ) {
            me.addSelectStylingToRwis(me.selectedSite);
        }
        else {
            me.addSelectStylingToRoadSite(me.selectedSite);
        }

        // Display the site information
        this.pushShowSite(rec, "map-nav-view");
    },

    addSelectStylingToRwis: function(site_num) {
        var me = this;

        if (me.markers) {
            for (var i = 0; i < me.markers.length; i++) {
                var marker_id = me.markers[i].get('id');

                if (site_num == marker_id) {
                    // Find the site for this marker and get its code
                    // Todo: This looks very inefficient -- why loop through all the sites?
                    var code = null;
                    var isRwisSite = false;
                    var distSiteStore = me.getDistrictSiteStore();
                    distSiteStore.each(function(district, index, length) {
                        var siteStore = district.sites();
                        // siteStore.each(function(site, index, length) {
                        //     if (site.get('site_num') == site_num) {
                        //         code = site.get(me.currentAlertType);
                        //         isRwisSite = site.get('is_rwis_site');
                        //     }
                        // });
                        var site = siteStore.findRecord('site_num', site_num);
                        if ( site ) {
                            code = site.get(me.currentAlertType);
                            isRwisSite = site.get('is_rwis_site');
                            return false; // exits district each() iteration...
                        }
                    });
                        
                    if (code != null) {
                        var appRoot = window.location.href.substr( 0, window.location.href.indexOf('?') );
                        var image = {
                            url: appRoot + 'resources/icons/Alert_Images.png',
                            size: new google.maps.Size(14, 14),
                            origin: me.getAlertImageOffset(code, isRwisSite, true),
                            anchor: new google.maps.Point(7, 7)
                        };
                        me.markers[i].setIcon(image);
                    }
                }
            }
        }
    },

    removeSelectStylingFromRwis: function(site_num) {
        var me = this;

        if (me.markers) {
            for (var i = 0; i < me.markers.length; i++) {
                var marker_id = me.markers[i].get('id');

                if (site_num == marker_id) {
                    // Find the site for this marker and get its code
                    var code = null;
                    var isRwisSite;
                    var distSiteStore = me.getDistrictSiteStore();
                    distSiteStore.each(function(district, index, length) {
                        var siteStore = district.sites();
                        // siteStore.each(function(site, index, length) {
                        //     if (site.get('site_num') == site_num) {
                        //         code = site.get(me.currentAlertType);
                        //         isRwisSite = site.get('is_rwis_site');
                        //     }
                        // });
                        var site = siteStore.findRecord('site_num', site_num);
                        if ( site ) {
                            code = site.get(me.currentAlertType);
                            isRwisSite = site.get('is_rwis_site');
                            return false; // exits district each() iteration...
                        }
                    });

                    if (code != null) {
                        var appRoot = window.location.href.substr( 0, window.location.href.indexOf('?') );
                        var image = {
                            url: appRoot + 'resources/icons/Alert_Images.png',
                            size: new google.maps.Size(14, 14),
                            origin: me.getAlertImageOffset(code, isRwisSite, false),
                            anchor: new google.maps.Point(7, 7)
                        };
                        me.markers[i].setIcon(image);
                    }
                }
            }
        }
    },

    addSelectStylingToRoadSite: function(site_num) {
        var me = this;

        if (me.segments) {
            for (var i = 0; i < me.segments.length; i++) {
                var segment_id = me.segments[i].get('id');

                // If this is the newly selected segment, update its styling to appear selected
                if (site_num == segment_id) {
                    me.segments[i].setOptions( {'strokeWeight': 6} );

                    // Add a black centerline to the segment for this site.
                    var appController = emdss.app.getController('emdss.controller.Application');
                    var rawSegments = appController.rawRoadSegments;
                    var matchingSegment = me.findSegment(rawSegments.segments, site_num);
                    if ( matchingSegment ) {
                        var coords = matchingSegment.coordinates;
                        var coordArray = [];
                        for (var c = 0; c < coords.length; c++) {
                            coordArray.push( {'lat': coords[c].lat, 'lng': coords[c].lon} );
                        }

                        var gmSeg = new google.maps.Polyline({
                            path: coordArray,
                            geodesic: true,
                            zIndex: 2,
                            strokeColor: '#000000',
                            strokeOpacity: 1.0,
                            strokeWeight: 2
                        });

                        gmSeg.set('id', site_num);
                        // var map = Ext.getCmp("map-view").getMap();
                        var map = me.getMapView().getMap();
                        gmSeg.setMap(map);
                        me.selectedSegmentCenter = gmSeg;
                    }
                }
            }
        }
    },

    removeSelectStylingFromRoadSite: function(site_num) {
        var me = this;

        if (me.segments) {
            for (var i = 0; i < me.segments.length; i++) {
                var segment_id = me.segments[i].get('id');

                // If this is the previously selected site, update its styling with standard styling
                if (site_num == segment_id) {
                    me.segments[i].setOptions( {'strokeWeight': 2} );
                }
            }
        }
    },

    styleSelectedSite: function(site_num) {
        var me = this;

        // Clear the old current marker and update the marker for this site.  Don't
        //   need to do anything if the selected site hasn't changed.
            
        // Loop through the markers to find the marker for the selected site, and update with a selected marker image.
        //   Also update the previously-selected marker to indicate that it's no longer selected.
        if (me.markers) {
                for (var i = 0; i < me.markers.length; i++) {
                    var marker_id = me.markers[i].get('id');
    
                    // If this is the previously selected site, update its icon to the unselected icon
                    if (me.selectedSite == marker_id) {
                        // Find the site for this marker and get its code
                        // Todo: This looks very inefficient -- why loop through all the sites?
                        var code = null;
                        var isRwisSite;
                        var distSiteStore = me.getDistrictSiteStore();
                        distSiteStore.each(function(district, index, length) {
                            var siteStore = district.sites();
                            siteStore.each(function(site, index, length) {
                                if (site.get('site_num') == me.selectedSite) {
                                    code = site.get(me.currentAlertType);
                                    isRwisSite = site.get('is_rwis_site');
                                }
                            });
                        });

                        if (code != null) {
                           var appRoot = window.location.href.substr( 0, window.location.href.indexOf('?') );
                           var image = {
                                url: appRoot + 'resources/icons/Alert_Images.png',
                                size: new google.maps.Size(14, 14),
                                origin: me.getAlertImageOffset(code, isRwisSite, false),
                                anchor: new google.maps.Point(7, 7)
                           };
                           me.markers[i].setIcon(image);
                        }
                    }

                    // If this is the newly selected site, update its icon to the selected icon
                    if (site_num == marker_id) {
                        // Find the site for this marker and get its code
                        // Todo: This looks very inefficient -- why loop through all the sites?
                        var code;
                        var isRwisSite = false;
                        var distSiteStore = me.getDistrictSiteStore();
                        distSiteStore.each(function(district, index, length) {
                            var siteStore = district.sites();
                            siteStore.each(function(site, index, length) {
                                if (site.get('site_num') == site_num) {
                                    code = site.get(me.currentAlertType);
                                    isRwisSite = site.get('is_rwis_site');
                                }
                            });
                        });

                        if (code != null) {
                            var appRoot = window.location.href.substr( 0, window.location.href.indexOf('?') );
                            var image = {
                                url: appRoot + 'resources/icons/Alert_Images.png',
                                size: new google.maps.Size(14, 14),
                                origin: me.getAlertImageOffset(code, isRwisSite, true),
                                anchor: new google.maps.Point(7, 7)
                            };
                            me.markers[i].setIcon(image);
                        }
                    }
                }
        }
    
        // Loop through the segments to find the segment for the selected site, and update with a selected segment image.
        //   Also update the previously-selected segment to indicate that it's no longer selected.
        if ( me.selectedSegmentCenter ) {
            me.selectedSegmentCenter.setMap(null);
        }
        if (me.segments) {
            for (var i = 0; i < me.segments.length; i++) {
                    var segment_id = me.segments[i].get('id');
        
                    // If this is the previously selected site, update its styling with standard styling
                    if (me.selectedSite == segment_id) {
                        me.segments[i].setOptions( {'strokeWeight': 2} );
                    }
        
                    // If this is the newly selected segment, update its styling to appear selected
                    if (site_num == segment_id) {
                    me.segments[i].setOptions( {'strokeWeight': 6} );
                    
                    // Add a black centerline to the segment for this site.
                    var appController = emdss.app.getController('emdss.controller.Application');
                    var rawSegments = appController.rawRoadSegments;
                    var matchingSegment = me.findSegment(rawSegments.segments, site_num);
                    if ( matchingSegment ) {
                        var coords = matchingSegment.coordinates;
                        var coordArray = [];
                        for (var c = 0; c < coords.length; c++) {
                            coordArray.push( {'lat': coords[c].lat, 'lng': coords[c].lon} );
                        }
            
                        var gmSeg = new google.maps.Polyline({
                            path: coordArray,
                            geodesic: true,
                            zIndex: 2,
                            strokeColor: '#000000',
                            strokeOpacity: 1.0,
                            strokeWeight: 2
                        });
                     
                        gmSeg.set('id', site_num);
                        // var map = Ext.getCmp("map-view").getMap();
                        var map = me.getMapView().getMap();
                        gmSeg.setMap(map);
                        me.selectedSegmentCenter = gmSeg;
                    }
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
        var appController = emdss.app.getController('emdss.controller.Application');

        if ( senchaEvent.event.target.className === 'district_zoom' ) {
            var siblings = senchaEvent.event.target.parentElement.children;
            var districtName = siblings[0].innerText;
            var distSiteStore = Ext.data.StoreManager.lookup('district-site-store');
            var idx = distSiteStore.find( 'district_name', districtName, 0, false, true, true );
            if ( idx > -1 ) {
                var district = distSiteStore.getAt(idx);
                // var map = Ext.getCmp("map-view").getMap();
                var map = me.getMapView().getMap();
                appController.zoomToDistrict(map, district);
            }
        }
        else if ( senchaEvent.event.target.className === 'site_description' ||
                  senchaEvent.event.target.className === 'site_alert' ||
                  senchaEvent.event.target.className === '' ) {
            var distSiteStore = Ext.data.StoreManager.lookup('district-site-store');
            var siblings = senchaEvent.event.target.parentElement.children;
            if ( senchaEvent.event.target.className === '' ) {
                siblings = senchaEvent.event.target.parentElement.parentElement.children;
            }
            var siteDescription = siblings[0].innerText;
            distSiteStore.each(function (district, index, length) {
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
     * OnLoad() callback from the SitePlods store. Set the record on the
     *   site detail view after the load is complete.
     */
    onSiteDataLoad: function(store, records, success, operation, eOpts) {
        this.showSite.setRecord(records[0]);
    },

    /**
     * OnLoad() callback from the SitePlots store. Set the record on the 
     *   site detail view after the load is complete.
     */
    onSitePlotsLoad: function(store, records, success, operation, eOpts) {
        console.log("");
        console.log("");
        console.log("");
        console.log("In Road controller onSitePlotsLoad()");
        console.log("");
        console.log("");
        console.log("");
        if ( typeof records[0] == 'undefined' ) {
            console.log("    WARNING: onSitePlotsLoad has undefined record[0]!!!");
            return;
        }

        if ( !this.showSite ) {
            return;
        }

        var new_plot = records[0].get('summary_plot');
        var modifiedRecord = this.showSite.getRecord();

        if ( modifiedRecord ) {
            modifiedRecord.set('summary_plot', new_plot);

            console.log("Got a new plot, and set it on an existing record");

            this.showSite.setRecord(modifiedRecord);
            this.showSite.updatePlot(modifiedRecord);
        }
        else {
            console.log("Got a new plot, but no record exists yet");
            this.showSite.cachePlot(new_plot);
        }
    },

    /**
     * Create and show a new site detail view.
     */
    pushShowSite: function(record, cmpId) {
        // This method has a record -- it's the district_alerts record (not the plot record)
        
        console.log("Road controller pushShowSite with site: " + record.get('desc'));

        var me = this;
        var appController = emdss.app.getController('emdss.controller.Application');
        var wmsController = emdss.app.getController('emdss.controller.WmsData');

        // If we haven't create the window yet, then create it now
        if (!me.showSite) {
            me.showSite = Ext.create("emdss.view.site.Show");
        }
        // var tabPanel = Ext.getCmp("main-tab-panel");
        var tabPanel = me.getMainTabPanel();
        var navBar = tabPanel.getActiveItem().getNavigationBar();
        navBar.show();

        // Push the site window on top of the current component

        Ext.getCmp(cmpId).push(me.showSite);
        // var toolbar = Ext.getCmp("map-toolbar");
        var toolbar = me.getMapToolbar();
        toolbar.hide();
        Ext.getCmp(cmpId).on('pop', appController.showFcstToolbar, me);
        
        var radarPanel = me.getPanelRadar();
        radarPanel.hide();
        Ext.getCmp(cmpId).on('pop', wmsController.showRadarPanel, wmsController);

        // Capture pop to clear out the record on me.showSite when done.
        Ext.getCmp(cmpId).on('pop', me.popShowSite, me);
        
        var site = record.get('site_num');
        if (!me.siteData) {
            me.siteData = Ext.create("emdss.store.SiteData",{});
            me.siteData.on("load", Ext.bind(me.onSiteDataLoad, me));
        }
        me.siteData.setSite(site);
        if (!me.sitePlot) {
            me.sitePlot = Ext.create("emdss.store.SitePlots",{});
            me.sitePlot.on("load", Ext.bind(me.onSitePlotsLoad, me));
        }
        me.sitePlot.setSite(site);
        // No longer need to do this, as the alert time series is
        //   delivered with the plot data, not the district_alert data.
        // me.showSite.setTimeSeries(record.time_series());
        me.showSite.setSiteName(record.get('desc'));

        
        var now = new Date();
        var nowUtc = new Date(
            now.getUTCFullYear(),
            now.getUTCMonth(),
            now.getUTCDate(),
            now.getUTCHours(),
            now.getUTCMinutes(),
            now.getUTCSeconds()
        );
        var nowUtcFmt = Ext.Date.format(nowUtc,"Ymd.Hi");
        console.log("Setting date on siteData before data load: " + nowUtcFmt);
        me.siteData.setDate( nowUtcFmt );
        me.siteData.loadData(me.showSite);

        console.log("Setting date on sitePlot before data load: " + nowUtcFmt);
        me.sitePlot.setDate( nowUtcFmt );
        me.sitePlot.loadData(me.showSite);
    },
    
    /**
     * Handle user action -- going back from showSite view.
     */
    popShowSite: function(navView, view, eOpts) {
        var me = this;


        // this.showSite.setRecord(null);
        var ss = this.showSite;
        this.showSite = null;
        this.siteData = null;
        this.sitePlot = null;

        // Unregister for the pop event, and destroy the Show object
        Ext.getCmp(navView.id).un('pop', me.popShowSite, me);
        ss.destroy();
    },

    setDistrictSiteStore: function(store) {
        this.districtSiteStore = store;
    },
    
    getDistrictSiteStore: function() {
        return this.districtSiteStore;
    },

    findSegment: function(rawSegments, siteNum) {
        var seg = null;
        for (var i = 0; i < rawSegments.length; i++) {
            if ( rawSegments[i].dicast_id == siteNum ) {
                return rawSegments[i];
            }
        }

        return seg;
    }

});
