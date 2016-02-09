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
	    main:'mainview',
	    sites:'sites',
	    showSite:'site-show',
	    siteList:'site-list',
	    mapView:'map-view'
	},
	clockTask: null,
	stopClock: false,
    },
    
    latest_data: Array,

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
    	
    	console.log("In Application::init()...");
    	
		// Initialize the timezone code
	
		//	timezoneJS.timezone.zoneFileBasePath = 'tz';
		//	timezoneJS.timezone.init();

	    var hrefState = emdss.util.Config.getHrefState();
			
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
		    // 		+ mawDesk.util.Config.getHrefState()
		    // 		+ ( emdss.util.Config.getUseVdt() ? '_vdt' : '' )
		});
		this.latestDataTimes.on({
		    refresh:'onDataTimesUpdate',
		    scope:this
		});
		this.latestDataTimes.load();
			
		// 
		// Sites and Alerts
		// 
		this.districtSiteStore =
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
			       });
			
		console.log("Done creating DistrictSiteStore...");
			
		this.districtSiteStore.setupAfterListener();
			
		this.districtSiteStore.setProxy({
		    type:'ajax',
		    url: emdss.util.Config.getDistrictAlertsUrl() 
		    		+ '&state=' + emdss.util.Config.getHrefState()
		    		+ ( emdss.util.Config.getUseVdt() ? '_vdt' : '' ),
		    reader: {
	                type:         'json',
	                rootProperty: 'districts'
	            }
		});
			
		this.districtSiteStore.on({
		    refresh: 'onSiteListUpdate',
		    scope: this
		});
			
		// 
		// Set the alert generation time on each district after load...
		// 
		this.districtSiteStore.on({
		    load: 'setDataTimeOnDistricts',
		    scope: this.districtSiteStore
		});
			
		// 
		// Load the store and start the reload timer
		// 
		this.districtSiteStore.load();
		this.districtSiteStore.startReloadTimer(300000); // 300000 ms == 5 min
				
		// 
		// Load the treatment explanations store.
		//   NOTE: I DO NOT KNOW IF THIS DOES ANYTHING -- I JUST DISCOVERED IT AS I WAS COMMENTING THE CODE!
		// 
		this.date = emdss.util.Config.getDefaultDate();
		this.alertType = emdss.util.Config.getDefaultAlertType();
		this.explanationStore = Ext.create("emdss.store.TreatmentExplanations",{
		    storeId:"explanation-store"
		});
		
		//
		// Start the timer for updating the current time
		//
		this.startClockTimer(1000);
	
		//
		// Initialize the display control information
		//
		this.selectedSite = '';
		this.currentAlertType = 'hr06_alert_code';
	
		// 
		// Set up the listeners
		// 
		this.control({
		    '#fcst-type-select': {
		        toggle: function(segbutton, button, isPressed, eOpts) {
		            console.log('fcst-type-select button: ' + button.getText() + ' isPressed: ' + isPressed);
		            if ( isPressed ) {
		                me.onFcstTypeSelect(segbutton, button, isPressed, eOpts);
		            }
		        }
		    },
		    '#hide-show-vehicles-button': {
		        release: function(button, released, eOpts) {
		            console.log('hide-show-vehicles-button button : ' + released);
		            me.onHideShowVehiclesRelease(button, released, eOpts);
		        }
		    },
		    '#site-list-item': {
		       	itemtap: function(list, index, node, record, senchaEvent) {
		       	    console.log('site-list-item itemtap: ' + senchaEvent.event.target.className );
		       	    me.onSiteSelect(list, index, node, record, senchaEvent);
		       	}
		    },
		})
    },
    
	/**
	 * Set up some UI callbacks.
	 */
    launch: function() {
    	
		var siteList = Ext.getCmp("site-list");
		// var siteListItem = Ext.getCmp("site-list-item");
		var tabPanel = Ext.getCmp("main-tab-panel");
								 	
    	// siteListItem.on('itemtap', this.onSiteSelect, this);
    	tabPanel.on('activeitemchange', this.onTabChange, this);
			
		// Todo: Not sure what this is for. Nancy?
		var listdata = Ext.create('Ext.data.Store', {
		    fields : ['title'],
		    data   : [ {title: 'Alpha'}, {title: 'Bravo'}, {title: 'Charley'} ]
	});
    	
	
    	// No settings for now...
	// var configButton = Ext.getCmp("toolbar-config");
	// configButton.on("tap",Ext.bind(this.onConfigClick,this));
		
		
	// var typeSelectButton = Ext.getCmp("fcst-type-select");
	// typeSelectButton.on("toggle",Ext.bind(this.onFcstTypeSelect,this));
	// var typeSelectButton = Ext.getCmp("site-type-select");
	// typeSelectButton.on("toggle",Ext.bind(this.onSiteTypeSelect,this));
	// var overlaySelectButton = Ext.getCmp("overlay-select");
	// overlaySelectButton.on("toggle",Ext.bind(this.onOverlaySelect,this));
	// var alertTypeSelectButton = Ext.getCmp("alert-type-select");
	// alertTypeSelectButton.on("toggle",Ext.bind(this.onAlertTypeSelect,this));
    },

	/**
	 * Get the latest radar images.
	 */
    getRadarImages: function() {
		console.log(this.date);
		var date = Ext.Date.parse(this.date, "Ymd.Hi");
		var sec = Ext.Date.format(date,"U");
		sec = sec - (sec % 900);
		var fnames = [];
		for (var i = 0; i < 5; i++) {
		    var s = sec - (i*900);
		    var d = Ext.Date.parse(s,"U");
		    var day = Ext.Date.format(d,"Ymd");
		    var hhmm = Ext.Date.format(d,"Hi");
		    var fname = emdss.util.Config.getRadarUrl(day,hhmm);
		    fnames.push(fname);
		}
		fnames.reverse();
			return fnames;
	    },
	
	    stepOverlay: function(fnames) {
		if (!this.overlayIndex) {
		    this.overlayIndex = 0;
		}
		console.log(this.overlayIndex);
		var map = Ext.getCmp("map-view").getMap();
		
		if ( map == null ) {
		    console.log("Warning:");
		    console.log("Warning: Map is null in stepOverlay(). Would have added sites from " + records.length + " districts...");
		    console.log("Warning:");
		    
		    return;
		}
			
		var radarBounds = emdss.util.Config.getRadarBounds();
		this.overlayIndex = this.overlayIndex % fnames.length;
		var fname = fnames[this.overlayIndex];
		console.log(fname);
		this.overlayIndex ++;
		var newOverlay = new google.maps.GroundOverlay(fname,radarBounds);
		newOverlay.setMap(map);
		if (this.overlay) {
		    this.overlay.setMap(null);
		}
		this.overlay = newOverlay;
    },
	
	/**
	 * Start the radar image loop
	 */
    startOverlayLoop: function(fnames) {
		this.stopOverlayLoop();
		this.overlayLoopTask = setInterval(Ext.bind(this.stepOverlay,this,[fnames]),750);
    },
	
	/** 
	 * Stop the radar image loop
	 */
    stopOverlayLoop: function() {
		if (this.overlayLoopTask) {
		    clearInterval(this.overlayLoopTask);
		}
    },
	
	/**
	 * Handle clicks on the overlay button to stop, start, or restart the radar loop.
	 */
    onOverlaySelect: function(sbtn, btn, isPressed, eOpts) {
		var label = btn.getText();
		this.stopOverlayLoop();
		if (this.overlay) {
		    this.overlay.setMap(null);
		}
		if (label == emdss.util.Config.getRadarOverlayLabel() && isPressed) {
		    var fnames = this.getRadarImages();
		    this.startOverlayLoop(fnames);
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
	 * Handle selection changes to the forecast tabs. 
	 *   - Remove all site markers from the map
	 *   - Add new site markers to the map representing the selected forecast time range
	 * 
	 * Todo: This loses any previously-selected site. Need to add a special image if the site is marked as selected.
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
	
		// Don't clear the filter -- just need to refresh the map display to show the new images.
		// this.siteStore.clearFilter();
			
		var map = Ext.getCmp("map-view").getMap();
			
		if ( map == null ) {
		    console.log("Warning:");
		    console.log("Warning: Map is null in onFcstTypeSelect(). Would have added sites from " + records.length + " districts...");
		    console.log("Warning:");
				
		    return;
		}
			
		var lt;
		if (this.markers) {
		    for (var i = 0; i < this.markers.length; i++) {
			this.markers[i].setMap(null);
		    }
		}
		this.markers = [];
		this.selectedSite = '';
	
		var districtSiteStore = me.districtSiteStore;
		districtSiteStore.each(function (district, index, length) {
		    var siteStore = district.sites(); // returns Ext.Store of sites...
		    siteStore.each(function (site, index, length) {
			var lat = site.get('lat');
			var lon = site.get('lon');
			var code = site.get(me.currentAlertType);
			var isRwisSite = site.get('is_rwis_site');
			
			// Load image with the correct origin (image is a sprite) based on the alert code...
			var appRoot = window.location.href.substr( 0, window.location.href.indexOf('?') );
			var image = {
			    url: appRoot + 'resources/icons/Alert_Images.png',
			    size: new google.maps.Size(14, 14),
			    origin: me.getAlertImageOffset(code, isRwisSite, false),
			    anchor: new google.maps.Point(7, 7)
			};
					
			lt = new google.maps.LatLng(lat,lon);
			var mark = new google.maps.Marker({
			    position: lt,
			    map:      map,
			    icon: image
			});
			mark.set('id', site.get('site_num'));
			me.markers.push(mark);
			google.maps.event.addListener(mark, "click", Ext.bind(me.onMarkerClick, me, [mark, lt, site, map], true));
		    });
		});
    },
    
    /**
     * Handle clicks to the vehicle visibility button.
     */
    onHideShowVehiclesRelease: function(button, released, eOpts) {
		this.getApplication().getController('emdss.controller.VehicleController').toggleShowVehicles(button);
    },

	/**
	 * Not currently used.
	 */
    onSiteTypeSelect: function(sbtn, btn, isPressed, eOpts) {
		var label = btn.getText();
		this.siteStore.clearFilter();
		if (label == emdss.util.Config.getAllSitesLabel()) {
		    //do nothing, filter already cleared
		} else if (label == emdss.util.Config.getRoadCondSitesLabel()) {
		    this.siteStore.filter("is_road_cond_site",true);
		} else if (label == emdss.util.Config.getWxObsSitesLabel()) {
		    this.siteStore.filter("is_wx_obs_site",true);
		} else if (label == emdss.util.Config.getRwisSitesLabel()) {
		    this.siteStore.filter("is_rwis_site",true);
		} else if (label == emdss.util.Config.getHideSitesLabel()) {
		    this.siteStore.filter("is_rwis_site","na"); //make up value that it will not be to filter out all sites
		}
    },

	/**
	 * Not currently used.
	 */
    onAlertTypeSelect: function(sbtn, btn, isPressed, eOpts) {
		var label = btn.getText();
		console.log(label);
		if (label == emdss.util.Config.getWxAlertsLabel()) {
		    this.alertType = "wx";
		} else if (label == emdss.util.Config.getRoadCondAlertsLabel()) {
		    this.alertType = "road_cond";
		} else if (label == emdss.util.Config.getObsAlertsLabel()) {
		    this.alertType = "obs";
		}
		this.siteStore.setAlertType(this.alertType);
		this.siteStore.loadData();
    },

	/**
	 * Not currently used.
	 */
    onConfigClick: function(btn, e, eOpts) {
		if (!this.configMenu) {
		    this.configMenu = Ext.create("emdss.view.ConfigMenu",{
		    });
		    var list = Ext.getCmp('config-menu-list');
		    list.on('select',Ext.bind(this.onConfigMenuClick,this));
		}
		this.configMenu.showBy(btn);
    },

	/**
	 * Not currently used.
	 */
    onConfigMenuClick: function(list, rec, eOpts) {
		var l = rec.data.label;
		list.deselectAll();
		this.configMenu.hide();
		if (l == emdss.util.Config.getDateMenuLabel()) {
		    if (!this.datePicker) {
			this.datePicker = Ext.Viewport.add({
			    xtype:'datepicker',
			    value:new Date(),
			    yearFrom:2006
			});
			this.datePicker.on("change",Ext.bind(this.onDateChange,this));
		    }
		    this.datePicker.show();
		}
    },

	/**
	 * Not currently used.
	 */
    onDateChange: function(picker, value, eOpts) {
		var dateStr = Ext.Date.format(value, "Ymd.Hi");
		this.date = dateStr;
		if (this.sitePlots) {
		    this.sitePlots.setDate(dateStr);
		    this.sitePlots.loadData();
		}
		this.siteStore.setDate(dateStr);
		this.siteStore.loadData();
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
				var valid_time_label = Ext.getCmp('map-valid-time-label');
				valid_time_label.setHtml('Data valid: ' + valid_time_string);
		    }
		}
    },
    
    /**
     * Update the current time indication in the UI. 
     */
    updateCurrentTime: function() {
		// Update the current time widget
		var curr_time_label = Ext.getCmp('map-curr-time-label');
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
    
    /**
     * Handle refresh of the DistrictSiteStore. Called after the data is refreshed.
     *   Replace all the site markers on the map.
     */
    onSiteListUpdate: function(store, data, eOpts) {
    	this.latest_data = data;
		records = data.items;
		var map = Ext.getCmp("map-view").getMap();
		this.addSitesToMap(records, map);
    },
	
	/**
	 * Refresh the site markers on the map with data from the passed-in records.
	 */
    addSitesToMap: function(records, map) {
		var me = this;
			
		if ( map == null ) {
		    console.log("Warning:");
		    console.log("Warning: Map is null in addSitesToMap(). Would have added sites from " + records.length + " districts...");
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
	
		for ( var key in records ) {
		    var district = records[key];
		    
		    // debugger;
				
		    var sites = district.sites(); // returns Ext.Store of sites...
		    sites.each(function (site, index, length) {
					
			var lat = site.get('lat');
			var lon = site.get('lon');
							
			var appRoot = window.location.href.substr( 0, window.location.href.indexOf('?') );						
			var image = {
			    url: appRoot + 'resources/icons/Alert_Images.png',
			    size: new google.maps.Size(14, 14),
			    origin: me.getAlertImageOffset( site.get('hr06_alert_code'), site.get('is_rwis_site'), false),
			    anchor: new google.maps.Point(7, 7)
			};
					
			// debugger;
					
			lt = new google.maps.LatLng(lat,lon);
			var mark = new google.maps.Marker({
			    position: lt,
			    map:      map,
			    icon:     image
			});
			mark.set('id', site.get('site_num'));
			me.markers.push(mark);
			google.maps.event.addListener(mark, "click", Ext.bind(me.onMarkerClick, me, [mark,lt,site,map], true));
		    });
		}
    },

	/**
	 * Handle site clicks:
	 *   - Update the selected site with a special marker image
	 *   - Remove the special marker image from the previously-selected site
	 *   - Open the site detail view for the selected site
	 */
    onMarkerClick: function(e, mark, lt, rec, map) {
		console.log("    selected site_num = " + rec.get('site_num'));
		var me = this;
	
		var time_series = null;
	
		// Clear the old current marker and update the marker for this site.  Don't
		// need to do anything if the selected site hasn't changed.
		
		if (rec.get('site_num') != me.selectedSite) {
			
		    // Loop through the markers to find the marker for the selected site, and update with a selected marker image.
		    //   Also update the previously-selected marker to indicate that it's no longer selected.
		    if (me.markers) {
				for (var i = 0; i < me.markers.length; i++) {
				    var marker_id = me.markers[i].get('id');
		
				    // If this is the previously selected site, update its icon to the unselected icon
				    if (me.selectedSite == marker_id) {
						// Find the site for this marker and get its code
						// Todo: This looks very inefficient -- why loop through all the sites?
						var code;
						var isRwisSite;
						var districtSiteStore = me.districtSiteStore;
						districtSiteStore.each(function(district, index, length) {
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
				    if (rec.get('site_num') == marker_id) {
						// Find the site for this marker and get its code
						// Todo: This looks very inefficient -- why loop through all the sites?
						var code;
						var isRwisSite = false;
						var districtSiteStore = me.districtSiteStore;
						districtSiteStore.each(function(district, index, length) {
						    var siteStore = district.sites();
						    siteStore.each(function(site, index, length) {
								if (site.get('site_num') == rec.get('site_num')) {
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
	
		    // Keep track of the selected site for next time
		    me.selectedSite = rec.get('site_num');
		}
	
		// Display the site information
		this.pushShowSite(rec, "map-nav-view");
    },
    
    /**
     * Handle time range tab changes by removing all map markers and adding new markers for the
     *   newly-selected alert time range.
     */
    onTabChange: function(container, value, oldValue, eOpts) {
		var buttons = Ext.getCmp("fcst-type-select");
		var veh_button = Ext.getCmp("hide-show-vehicles-button");
		var toolbar = Ext.getCmp('map-toolbar');
		var data_time_panel = Ext.getCmp('map-time-panel');
		if ( value.id == 'map-nav-view' ) {
		    buttons.show();
		    veh_button.show();
		    toolbar.setTitle('Pikalert Map');
		    data_time_panel.show();
		}
		else if ( value.id == 'site-list' ) {
		    buttons.hide();
		    veh_button.hide();
		    toolbar.setTitle('Pikalert Site List');
		    data_time_panel.hide();
		}
		else if ( value.id = 'vehicle-list' ) {
		    buttons.hide();
		    veh_button.hide();
		    toolbar.setTitle('Pikalert Vehicle List');
		    data_time_panel.hide();
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
				var map = Ext.getCmp("map-view").getMap();
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
	 * OnLoad() callback from the SitePlods store. Set the record on the 
	 *   site detail view after the load is complete.
	 */
    onSiteLoad: function(store, records, success, operation, eOpts) {
		this.showSite.setRecord(records[0]);
    },

    /**
     * Create and show a new site detail view.
     */
    pushShowSite: function(record, cmpId) {
		// If we haven't create the window yet, then create it now
	
		if (!this.showSite) {
		    this.showSite = Ext.create("emdss.view.site.Show");
		}
		var tabPanel = Ext.getCmp("main-tab-panel");
		var navBar = tabPanel.getActiveItem().getNavigationBar();
		navBar.show();
		
		// Push the site window on top of the current component
	
		Ext.getCmp(cmpId).push(this.showSite);
		var toolbar = Ext.getCmp("map-toolbar");
		toolbar.hide();
		Ext.getCmp(cmpId).on('pop', this.showFcstToolbar, this);
		var site = record.get('site_num');
		if (!this.sitePlots) {
		    this.sitePlots = Ext.create("emdss.store.SitePlots",{});
		    this.sitePlots.on("load", Ext.bind(this.onSiteLoad,this));
		}
		this.sitePlots.setSite(site);
		this.showSite.setTimeSeries(record.time_series());
		this.showSite.setSiteName(record.get('site_id'));
		this.sitePlots.setDate(this.date);
		this.sitePlots.loadData(this.showSite);
    },
    
    /**
     * Show the map toolbar (contains the forecast tabs, etc) 
     *   and hide the navigation toolbar (containing the back button, etc)
     */
    showFcstToolbar: function(show) {
    	var toolbar = Ext.getCmp("map-toolbar");
    	toolbar.show();
    	
    	var tabPanel = Ext.getCmp("main-tab-panel");
    	var navBar = tabPanel.getActiveItem().getNavigationBar();
    	navBar.hide();
    },
    
    /**
     * Handle the callback indicating the map was rendered for the first time:
     *   - Set the map bounds (or center and zoom level)
     *   - Add all sites and vehicles to the map 
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
		this.addSitesToMap(records, map);
			
			
		console.log("Map Rendered!");
		console.log("Map Rendered!");
		console.log("Map Rendered!");
		console.log("Map Rendered!");
		console.log("Map Rendered!");
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
		var tabPanel = Ext.getCmp("main-tab-panel");
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
