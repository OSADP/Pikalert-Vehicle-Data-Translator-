/**
 * Main Application controller -- start data loading timers. build the map, 
 *   and handle UI interactions.
 * @author paddy
 */
Ext.define('mawDesk.controller.Application',{
    extend:'Ext.app.Controller',
    requires:[
	'mawDesk.util.Config',
	// 'mawDesk.store.Sites',
    ],
    config:{
	refs:{
	    main:'mainview',
	    sites:'sites',
	    showSite:'site-show',
	    siteList:'site-list',
	    mapView:'map-view'
	},
	// control:{
	// sites:{
	// itemtap:'onSiteSelect'
	// }
	// }
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
		
		var hrefState = mawDesk.util.Config.getHrefState();
			
		console.log("Configuring MAW for state: " + hrefState);
			
	    	this.districtStore = Ext.create("mawDesk.store.DistrictStore",
	    					{
						    storeId:"district-store",
						    constructor: function(config) {
	        					console.log(config);
							
							console.log("In DistrictStore constuctor()...");
							
							this.callParent([config]);
						    },
						    init:function() {
		        				console.log("In DistrictStore init...");
						    }
						});
		
		console.log("Done creating DistrictStore...");
		
		this.districtStore.setupAfterListener();
			
		this.districtStore.setProxy({
		    type:'ajax',
		    // url: 'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy.php?path=/akdistrictalerts/',
		    url: mawDesk.util.Config.getDistrictAlertsUrl() 
		    		+ '&state=' + mawDesk.util.Config.getHrefState() 
		    		+ ( mawDesk.util.Config.getUseVdt() ? '_vdt' : '' ),
		    reader: {
	                type:         'json',
	                rootProperty: 'districts'
	            }
		});
			
		this.districtStore.on({
		    refresh: 'onSiteListUpdate',
		    scope: this
		});
			
		this.districtStore.load();
		// this.districtStore.startReloadTimer(60000); // 60000 ms == 1 min
		this.districtStore.startReloadTimer(300000); // 300000 ms == 5 min
		
		// Example: How to load with a callback...
		// this.districtStore.load(function(records, operation, success) {
		// 	    
		// 	    console.log("Loaded DistrictStore...");
		// 
		// 	    // debugger;
		// 	    
		// 	    // Do something here;
		// 	    
		// 	}, this);
			
			
		// 
		// Set the alert generation time on each district after load...
		// 
		this.districtStore.on({
		    load: 'setDataTimeOnDistricts',
		    scope: this.districtStore
		});
		this.districtStore.load();
			
		// 
		// Create the store for the latest data times.
		//  This auto-loads the first time, and subsequently reloads via the DistrictStore update timer.
		// 
		this.latestDataTimes = Ext.create("mawDesk.store.DataTimes",{
		    storeId:"latest-data-times-store"
		});
		
		this.latestDataTimes.setProxy({
		    type:'ajax',
		    url:   mawDesk.util.Config.getDataTimesUrl() + '&state=' 
		    		+ mawDesk.util.Config.getHrefState()
		    		+ ( mawDesk.util.Config.getUseVdt() ? '_vdt' : '' )
		    // url: mawDesk.util.Config.getTestDataTimesUrl() 
		    // 		+ '&state=' + mawDesk.util.Config.getHrefState()
		    // 		+ ( mawDesk.util.Config.getUseVdt() ? '_vdt' : '' )
		});
			
		this.latestDataTimes.on({
		    refresh:'onDataTimesUpdate',
		    scope:this
		});
			
		this.latestDataTimes.load();
			
		this.control({
		    '#start-select': {
		        change: function(select, newValue, oldValue) {
		            console.log('start-select changed from: ' + oldValue + ' to: ' + newValue);
		            me.onStartSelect(select, newValue, oldValue);
		        }
		    },
		    '#end-select': {
		        change: function(select, newValue, oldValue) {
		            console.log('end-select changed from: ' + oldValue + ' to: ' + newValue);
		            me.onEndSelect(select, newValue, oldValue);
		        }
		    }
		});
    },

	/**
	 * This is the best place to put callback registration (add to control member)...
	 */
    launch: function() {
    	// var startTimeButton = Ext.getCmp("start-time");
	// startTimeButton.on("toggle", Ext.bind(this.onStartEndSelect, this));
	// var endTimeButton = Ext.getCmp("end-time");
	// endTimeButton.on("toggle", Ext.bind(this.onStartEndSelect, this));
		
		
	// var endSelect   = Ext.getCmp("end-select");
	// endSelect.on('change', Ext.bind(this.onEndChange, this));
	// var startSelect = Ext.getCmp("start-select");
	// startSelect.on('change', Ext.bind(this.onStartChange, this));
    },
    
    //-------------------------------------------------------------------------------
    // Map Marker Managment Functions
    //-------------------------------------------------------------------------------
    
    /**
     * Handle callbacks for changes in the start and end date selection. Refresh
     *   the map markers.
     */
    onStartEndSelect: function(sbtn, btn, isPressed, eOpts) {
    	// debugger;
		var me = this;
		var data = me.latest_data;
		var records = data.items;
		var map = Ext.getCmp("map-view").getMap();
		this.addSitesToMap(records, map);
	},
	
	/**
	 * Handle refresh events from the site data store. Refresh 
	 *   the map markers.
	 */
	onSiteListUpdate: function(store, data, eOpts) {
	    // debugger;
	    this.latest_data = data;
		records = data.items;
		var map = Ext.getCmp("map-view").getMap();
		this.addSitesToMap(records, map);
	},
		
	weekday: ["Sun", "Mon", "Tues", "Weds", "Thurs", "Fri", "Sat"],
			
	/**
	 * Map Rendered for the first time -- add markers for the sites.
	 *   The markers cannot be added until the map is rendered, because
	 *   the map is null until that point...
	 */
	onMapRendered: function(map) {
			
		var districtStore = Ext.data.StoreManager.lookup('district-store');
		var records = [];
		districtStore.each(function (district, index, length) {
		    records.push(district);
		});
			
		this.addSitesToMap(records, map);
			
		// Set the initial map bounds
		var bounds = this.mapBounds;
		if ( bounds != undefined && bounds != null ) {
		    map.fitBounds(bounds);
		}
		else {
		    map.setCenter(mawDesk.util.Config.lookupDefaultCenter());
		    map.setZoom(mawDesk.util.Config.lookupDefaultZoom());
		}
			
		console.log("Map Rendered!");
		console.log("Map Rendered!");
		console.log("Map Rendered!");
		console.log("Map Rendered!");
		console.log("Map Rendered!");
			
		var mapView = Ext.getCmp("map-nav-view");
		mapView.setMasked(false);
	},
		
	/**
	 * Add markers to the map for the site records passed in.
	 *   Iterates over the selected time period and determines the 'worst' alert for each site,
	 *     then adds the appropriate marker symbol to the google map.
	 */
	addSitesToMap: function(records, map) {
		var me = this;
			
		if (records == null || records == undefined) {
		    console.log('Skipping addSitesToMap() because of null records...');
		    return;
		}
			
		if ( map == null ) {
		    console.log("Warning:");
		    console.log("Warning: Map is null in addSitesToMap(). Would have added sites from " + records.length + " districts...");
		    console.log("Warning:");
				
		    return;
		}
			
		var endSelect   = Ext.getCmp("end-select");
		var endTimeStr = endSelect.getValue();
		var startSelect = Ext.getCmp("start-select");
		var startTimeStr = startSelect.getValue();
			
		if (endTimeStr == null || endTimeStr == '0' || startTimeStr == null || startTimeStr == '0' ) {
		    return;
		}
			
		// 
		// Remove old markers...
		// 
		if (me.markers) {
		    for (var i = 0; i < this.markers.length; i++) {
				me.markers[i].setMap(null);
		    }
		}
		me.markers = [];
		
		var startTime = parseInt( startTimeStr, 10 );
		var endTime = parseInt( endTimeStr, 10 );
			
		console.log('Searching for alerts between ' + startTime + ' and ' + endTime + '.');
			
		// For each site district...
		console.log('Calculating map alerts based on ' + records.length + ' districts...');
		for ( var key in records ) {
		    var district = records[key];
				
		    // Iterate over sites in the district...
		    var sites = district.sites(); // returns Ext.Store of sites...
		    sites.each(function (site, index, length) {
					
				var lat = site.get('lat');
				var lon = site.get('lon');
						
				// Determine the 'worst' alert for each site...
				var alerts = site.time_series();
				var worstAlert = 'missing';
				alerts.each(function(alert, index, length) {
				    var thisTime = parseInt( alert.get('time') );
				    if ( thisTime >= startTime && thisTime <= endTime ) {
								
						var thisAlert = alert.get('alert_code');
									
						if ( worstAlert == 'alert') {
						    // Do Nothing.
						}
						else if ( worstAlert == 'warning' ) {
						    if ( thisAlert == 'alert' ) {
								worstAlert = thisAlert;
						    }
						}
						else if ( worstAlert == 'clear' ) {
						    if ( thisAlert == 'warning' || thisAlert == 'alert' ) {
								worstAlert = thisAlert;
						    }
						}
						else if ( worstAlert == 'missing' ) {
						    worstAlert = thisAlert;
						}
				    }
				});
						
				// Create a marker image for the site, based on it's 'worst' alert during the time period.
				//   Note that the loaded image is a sprite (multiple icons in one .png), and an offset
				//   is calculated to show the desired icon.
				var appRoot = window.location.href.substr( 0, window.location.href.indexOf('?') );
				var image = {
				    url: appRoot + 'resources/icons/Alert_Images.png',
				    size: new google.maps.Size(14, 14),
				    origin: me.getAlertImageOffset( worstAlert ),
				    anchor: new google.maps.Point(7, 7)
				};
						
				// Add the marker to the map.
				var lt = new google.maps.LatLng(lat,lon);
				var mark = new google.maps.Marker({
				    position: lt,
				    map:      map,
				    icon:     image
				});
				me.markers.push(mark);
		
				// Add a click listener to the marker.
				var record = site;
				var mv = Ext.getCmp("map-nav-view");
				google.maps.event.addListener(mark, "click", function () {
						        
				    var timeSeriesStore = record.time_series();
				    var filteredTimes = [];
				    timeSeriesStore.each(function (dataPoint, index, length) {
						var thisTime = parseInt( dataPoint.get('time') );
						var end = parseInt( endSelect.getValue() );
						var start = parseInt( startSelect.getValue() );
						if ( thisTime >= start && thisTime <= end ) {
							var tmObj = {time: dataPoint.get('time'),
							alert_code: dataPoint.get('alert_code'),
							precip: dataPoint.get('precip'),
							pavement: dataPoint.get('pavement'),
							visibility: dataPoint.get('visibility')};
						    filteredTimes.push(tmObj);
						}
				    });
						    
				    var viewTitle = record.get('desc');
				    me.showSite = Ext.create("Ext.dataview.List", {
						// fullscreen: true,
						itemTpl: me.createSiteTimeSeriesItemTpl(),
						// This didn't set the title on the nav bar in all browsers...
						title: viewTitle,
						data: filteredTimes
				    });
				    mv.push(me.showSite);
				    me.showSite.on('afterrender', mv.setMasked(false), me);
				    
				    var toolbar = Ext.getCmp("map-toolbar");
				    toolbar.hide();
				    mv.on('pop', me.onSiteListPop, me);
							
				    // Show the map nav bar (with back button).
				    var mapNavBar = mv.getNavigationBar();
				    var lab = Ext.create('Ext.Button', {
						text: viewTitle, 
						align: 'center'
				    });
				    //				    console.debug("Creating label: <" + viewTitle + ">");
				    //			    	var lab = Ext.create('Ext.Label', {
					//					    html: viewTitle
				    //					    html: 'test text'
					//					    html: 'test&nbsp;text'
				    //				        centered: true
					//					});
				    //			    	var aLabel = { xtype: 'button', text: viewTitle, align: 'right' };
				    mapNavBar.spacer.hide();
				    mapNavBar.rightBox.removeAll();
				    mapNavBar.rightBox.add({ xtype: 'container', fullscreen: true, items: [lab] });
				    mapNavBar.show();
				    
				}); // end of marker click listener
		    }); // end of sites.each()
		} // end of for( districts )
    },
    
    //-------------------------------------------------------------------------------
    // UI Management and Formatting Functions
    //-------------------------------------------------------------------------------
	
    /**
     * Rearrange the UI after the site list view has been hidden.
     *   o Destroy the site view component
     *   o Show the map toolbar (contains the start- and end-time selectors)
     *   o Hide the navigation view's navigation bar.
     */
    onSiteListPop: function(show) {
    	var me = this;
    	
		if ( me.showSite != undefined && me.showSite != null ) {
			me.showSite.destroy();
			me.showSite = null;
		}
    	
    	var toolbar = Ext.getCmp("map-toolbar");
    	toolbar.show();
    	
    	var mapView = Ext.getCmp("map-nav-view");
    	var mapNavBar = mapView.getNavigationBar();
    	mapNavBar.rightBox.removeAll();
    	mapNavBar.hide();
    },
    
    /**
     * Create the itemTpl used to style the site time series -- the list of 
     *   each time step for a site within the selected time range. Uses a
     *   css class to calculate the position of each alert icon within
     *   the alert image sprite. Note that the <img> tag references a
     *   transparent image -- that holds the space for the icon rendered
     *   by the div class reference.
     */
    createSiteTimeSeriesItemTpl: function() {
    	var me = this;
    	
		return new Ext.XTemplate(
		    '<div>',
		    '  <div class="site_list_time">',
		    '    {[this.getPrettyTimeString(values.time)]}',
		    '  </div>',
		    '  <div class="site_list_alert_img">',
		    '    <img class="alert_{alert_code}" src="resources/icons/Alert_Transparent.png"></img>',
		    '  </div>',
		    '  <div class="site_list_alert_explanation">',
		    '    {[this.getAlertExplanation(values.alert_code, values.precip, values.pavement, values.visibility)]}',
		    '  </div>',
		    '</div>',
		    {
			getPrettyTimeString: function(timeStr) {
			    var timeObj = mawDesk.util.Util.emdssStringToDate(timeStr);
			    var alertTimeStr = me.getAlertDateStringFromDate(timeObj);
			    return alertTimeStr;
			},
			getAlertExplanation: function(alertCode, precip, pavement, visibility) {
			    if ( alertCode == null || alertCode == undefined || alertCode == 'missing' ) {
				return 'Missing data for this time';
			    }
			    else if ( alertCode == 'clear' ) {
				return 'Clear';
			    }
			    else if ( alertCode == 'warning' ||
				      alertCode == 'Warning') {
				return '<b>Advisory:</b> Precip: ' + precip + ', Pavement: ' + pavement + ', Visibility: ' + visibility;
			    }
			    else if ( alertCode == 'alert' ||
				      alertCode == 'Alert') {
				return '<b>Warning:</b> Precip: ' + precip + ', Pavement: ' + pavement + ', Visibility: ' + visibility;
			    }
			    
			    return 'Unexpected Alert Code: ' + alertCode;
			},
			isPropertyPresent: function(prop) {
			    return (prop != undefined && prop != null);
			}
		    }
		);
    },

    /**
     * Return the appropriate origin for each alert code icon within the image sprite.
     *   Unfortunately this is a second representation of information that's also represented
     *   in css. The css version is used in the itemTpl for the sites list. See the function
     *   createSiteTimeSeriesItemTpl() for a use of the css method for doing this.
     */
    getAlertImageOffset: function(alertCode) {
		if ( alertCode == null || alertCode == undefined || alertCode == 'missing' ) {
		    return new google.maps.Point(0, 0);
		}
		else if ( alertCode == 'clear' ) {
		    return new google.maps.Point(14, 0);
		}
		else if ( alertCode == 'warning' ) {
		    return new google.maps.Point(28, 0);
		}
		else if ( alertCode == 'alert' ) {
		    return new google.maps.Point(42, 0);
		}
    },
	
    /**
     * Uses Util methods to convert from "yyyymmddhhmm" to a nicer "hh:mm on dd/mm" format
     * @param {Object} uglyDateStr The date string in "yyyymmddhhmm" format
     */
    getPrettyDateStringFromString: function (uglyDateStr) {
		var uglyDate = mawDesk.util.Util.emdssStringToDate( uglyDateStr );
		var prettyDateStr = this.getPrettyDateStringFromDate( uglyDate );
		return prettyDateStr;
    },
	
    /**
     * Converts from a date object to a nice "hh:mm on dd/mm" string
     * @param {Object} uglyDate The date object
     */
    getPrettyDateStringFromDate: function (uglyDate) {
		var me = this;
		var ap = 'am';
		var hrs = uglyDate.getHours();
		if ( hrs >= 12 ) {
		    if ( hrs > 12 ) {
			hrs -= 12;
		    }
		    ap = 'pm';
		}
		var mins = uglyDate.getMinutes();
		var shortDate = me.weekday[uglyDate.getDay()] + ' ' + hrs + ':' + (mins >= 10 ? mins : '0' + mins) + ' ' + ap;
		return shortDate;
    },
	
    /**
     * Converts from a date object to a nice "hh:mm on dd/mm" string
     * @param {Object} uglyDate The date object
     */
    getAlertDateStringFromDate: function (timeObj) {
		var me = this;
		var ap = 'am';
		var hrs = timeObj.getHours();
		if ( hrs >= 12 ) {
		    if ( hrs > 12 ) {
			hrs -= 12;
		    }
		    ap = 'pm';
		}
		var month = timeObj.getMonth() + 1;
		var day = timeObj.getDate();
		var mins = timeObj.getMinutes();
		var shortDate = me.weekday[timeObj.getDay()] + ' ' + month + '/' + day + ' ' + hrs + ':' + (mins >= 10 ? mins : '0' + mins) + ' ' + ap;
		return shortDate;
    },
	
    //-------------------------------------------------------------------------------
    // Start- and End-Time Selection Functions
    //-------------------------------------------------------------------------------
	
    /**
     * Populates the start- and end-time selectors with reasonable hourly options,
     *   based on the passed-in start date for the time series data. Populates
     *   the selectors with numHours values, and defaults the selection to the
     *   first and twelth hours. If there was a previous selection, attempts
     *   to replicate that selection. 
     * @param {Object} startString The start date of the time series data
     * @param {Object} numHours The number of hours with which to populate the selectors.
     */
    populateStartEndSelect: function(startString, numHours) {
		var me = this;
			
		// If we can't parse the given date string, leave the options as they were
		var dateInit = mawDesk.util.Util.emdssStringToDate( startString );
		if ( isNaN( dateInit.getTime() ) ) {  
		    return;
		}
			
		// Round off the minutes to the nearest past hour
		var yyyy = dateInit.getUTCFullYear();
		var mm = dateInit.getUTCMonth() + 1;
		var dd = dateInit.getUTCDate();
		var hh = dateInit.getUTCHours();
		var dateRoundStr = yyyy.toString() + (mm >= 10 ? mm.toString() : '0' + mm) +
		    (dd >= 10 ? dd.toString() : '0' + dd) + (hh >= 10 ? hh.toString() : '0' + hh) + '00';
		var dateRound = mawDesk.util.Util.emdssStringToDate( dateRoundStr );
		var endSelect   = Ext.getCmp("end-select");
		var startSelect = Ext.getCmp("start-select");
			
		var prevEndValue = endSelect.getValue();
		var prevStartValue = startSelect.getValue();
	
		var endOptions = [];
		var startOptions = [];
		var endDefaultValue = null;
		var startDefaultValue = null;
	
		// Add the latest data time as the first entry in the time lists
	
		dateInitStr = mawDesk.util.Util.dateToEmdssString(dateInit);
		dateInitPrettyStr = me.getPrettyDateStringFromDate(dateInit);
	
		startOptions.push( {text: 'Start: ' + dateInitPrettyStr, value: dateInitStr} );
		endOptions.push( {text: 'End: ' + dateInitPrettyStr, value: dateInitStr} );
		
		if ( startDefaultValue == null ) {
		    startDefaultValue = dateInitStr;
		}
	
		// Now add times for every hour after the latest time
	
		for ( var i = 1; i <= numHours; i++ ) {
		    var thisDate = mawDesk.util.Util.addHoursToDate(dateRound, i);
		    var thisDateStr = mawDesk.util.Util.dateToEmdssString(thisDate);
		    var newShortDate = me.getPrettyDateStringFromDate(thisDate);
		    
		    startOptions.push( {text: 'Start: ' + newShortDate, value: thisDateStr} );
		    endOptions.push( {text: 'End: ' + newShortDate, value: thisDateStr} );
				
		    // Use the first date as the default for start, and hr 12 for end
		    if ( i == 12 ) {
				endDefaultValue = thisDateStr;
		    }
		}
			
		endSelect.setOptions(endOptions);
		startSelect.setOptions(startOptions);
			
		// Set the selection to the previous selection, if it is still in the allowed range.
		if ( prevEndValue != 'not_populated_yet' ) {
		    // Just try it!
		    endSelect.setValue(prevEndValue);
		}
		if ( prevStartValue != 'not_populated_yet' ) {
		    startSelect.setValue(prevStartValue);
		}
			
		// Check that the selected values have been initialized. If not, set to hour 1 and hour 12
		if ( startSelect.getValue() != prevStartValue ) {
		    startSelect.setValue(startDefaultValue);
		}
		if ( endSelect.getValue() != prevEndValue ) {
		    if ( prevStartValue == prevEndValue && prevStartValue != 'not_populated_yet' ) {
			// Handle the case where the times have walked forward to the point where 
			//   start and end times are equal.
			endSelect.setValue( startSelect.getValue() );
		    }
		    else {
			endSelect.setValue(endDefaultValue);
		    }
		}
    },
	
    /**
     * Listener for selection of end hour. Adjusts the the start-time selector if the
     *   new choice creates an invalid time range. Kicks off addition of new markers
     *   to the map if no change is required to the start-time selector.
     * @param {Object} select
     * @param {Object} newValue
     * @param {Object} oldValue
     */
    onEndSelect: function(select, newValue, oldValue) {
		var me = this;
		if ( newValue == null ) {
		    console.log('end-select got null newValue on selection change. Ignoring.');
		    return;
		}
		// debugger;
			
		// Make sure the selection is valid, and adjust the other selector to accommodate.
		var startSelect = Ext.getCmp("start-select");
		var otherValue = parseInt(startSelect.getValue(), 10);
		var thisValue = parseInt(newValue, 10);
		if ( thisValue < otherValue ) {
		    // The user selected an illegal date range. 
		    //   1) Select the same value in the other field (a 1-hour selection)
		    //   2) Let the callback from that selection trigger map reload
		    startSelect.setValue(newValue);
		}
		else {
		    var data = me.latest_data;
		    var records = data.items;
		    var map = Ext.getCmp("map-view").getMap();
		    me.addSitesToMap(records, map);
		}
    },
	
    /**
     * Listener for selection of start hour. Adjusts the the end-time selector if the
     *   new choice creates an invalid time range. Kicks off addition of new markers
     *   to the map if no change is required to the end-time selector.
     * @param {Object} select
     * @param {Object} newValue
     * @param {Object} oldValue
     */
    onStartSelect: function(select, newValue, oldValue) {
		var me = this;
		if ( newValue == null ) {
		    console.log('start-select got null newValue on selection change. Ignoring.');
		    return;
		}
		// debugger;
			
		// Make sure the selection is valid, and adjust the other selector to accommodate.
		var endSelect   = Ext.getCmp("end-select");
		var otherValue = parseInt(endSelect.getValue(), 10);
		var thisValue = parseInt(newValue, 10);
		if ( thisValue > otherValue ) {
		    // The user selected an illegal date range. 
		    //   1) Select the same value in the other field (a 1-hour selection)
		    //   2) Let the callback from that selection trigger map reload
		    endSelect.setValue(newValue);
		}
		else {
		    var data = me.latest_data;
		    var records = data.items;
		    var map = Ext.getCmp("map-view").getMap();
		    me.addSitesToMap(records, map);
		}
    },
	
    /**
     * Listens for update to the latest data times store, and repopulates the
     * time selectors if there is new data available.
     * @param {Object} store
     * @param {Object} data
     * @param {Object} eOpts
     */
    onDataTimesUpdate: function(store, data, eOpts) {
		var records = data.items;
		var alertsDir = mawDesk.util.Config.getAlertsDir();
		var latestTime = null;
		for ( var idx in records ) {
		    var record = records[idx];
		    if (record.data.dir == alertsDir) {
			latestTime = record.data.latest_time;
		    }
		}
		    
		var updateSelectors = false;
		if ( this.previousDataTime == undefined || this.previousDataTime == null ) {
		    this.previousDataTime = parseInt( latestTime, 10 );
		    updateSelectors = true;
		}
		else {
		    var thisTime = parseInt( latestTime, 10 );
		    if ( thisTime > this.previousDataTime ) {
		    	this.previousDataTime = thisTime;
		    	updateSelectors = true;
		    	
			console.log("");
			console.log("");
			console.log("Got a NEW latest data time: " + latestTime + " (old was: " + this. previousDataTime + ")");
			console.log("");
			console.log("");
		    }
		}
		    
		if ( updateSelectors ) {
		    this.populateStartEndSelect(latestTime, 24);
		}
    }
});
