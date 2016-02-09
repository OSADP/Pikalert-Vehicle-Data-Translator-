/**
 * Config file for the application. Looks up zoom level for each state specified 
 *   in the application URL via REST interface.
 * @author Paddy McCarthy
 */
Ext.define("emdss.util.Config", {
    singleton:true,
    config: {
	dateMenuLabel: "Choose date...",
	alertsDir:'district_alerts',
	vehiclesDir:'latest_vehicles',
		
	displayState: 'minnesota',
	displayDistrict: '&district_name=district1',
	
	// // RAL Production
	forceSourceDomain: 'www.ral.ucar.edu',
	districtAlertsUrl:'http://www.ral.ucar.edu/projects/rdwx_mdss/proxy.php?path=/district_alerts',
	districtVehiclesUrl:'http://www.ral.ucar.edu/projects/rdwx_mdss/proxy.php?path=/districtvehicles',
	plotsUrl:'http://www.ral.ucar.edu/projects/rdwx_mdss/proxy.php?path=/plots',
	radarImagesUrl:'http://www.ral.ucar.edu/projects/rdwx_mdss/proxy.php?path=/radar_images/',
	dataTimesUrl:'http://www.ral.ucar.edu/projects/rdwx_mdss/proxy.php?path=/datatime/',
	vehiclesUrl: 'http://www.ral.ucar.edu/projects/rdwx_mdss/proxy.php?path=/latest_vehicles/',
	
	// Localhost Production
	// forceSourceDomain: 'localhost',
	// districtAlertsUrl:'http://localhost:8888/projects/rdwx_mdss/proxy.php?path=/district_alerts',
	// districtVehiclesUrl:'http://localhost:8888/projects/rdwx_mdss/proxy.php?path=/districtvehicles',
	// plotsUrl:'http://localhost:8888/projects/rdwx_mdss/proxy.php?path=/plots',
	// radarImagesUrl:'http://localhost:8888/projects/rdwx_mdss/proxy.php?path=/radar_images/',
	// dataTimesUrl:'http://localhost:8888/projects/rdwx_mdss/proxy.php?path=/datatime/',
	// vehiclesUrl: 'http://localhost:8888/projects/rdwx_mdss/proxy.php?path=/latest_vehicles/',
	
	useVdtDefault:true,
	obsFcstLabel:'Obs',
	next06HrsFcstLabel:'Next 6 Hrs',
	next24HrsFcstLabel:'6 - 24 Hrs',
	hrs24to72FcstLabel:'24 - 72 Hrs',
	hideVehiclesLabel:'Hide Vehicles',
	showVehiclesLabel:'Show Vehicles',
	allSitesLabel:'All',
	roadCondSitesLabel:'Road Cond.',
	wxObsSitesLabel:'Wx Obs',
	rwisSitesLabel:'RWIS',
	hideSitesLabel:'Hide All',
	wxAlertsLabel:"Wx Alerts",
	roadCondAlertsLabel:"Road Cond. Alerts",
	obsAlertsLabel:"Obs Alerts",
	defaultAlertType:"wx",
	// defaultZoom:4,
	// defaultCenter:new google.maps.LatLng(64.90491,-147.879272),
	defaultZoom:7,
	defaultCenter:new google.maps.LatLng(61.2167,-149.9000),
	defaultLocationLookup: {
	    'alaska':    { defaultCenter: new google.maps.LatLng(61.2167,-149.9000), defaultZoom: 7 },
	    'alaska_vdt':    { defaultCenter: new google.maps.LatLng(61.2167,-149.9000), defaultZoom: 7 },
	    'minnesota': { defaultCenter: new google.maps.LatLng(46.025,-95.326667), defaultZoom: 7 },
	    'minnesota_vdt': { defaultCenter: new google.maps.LatLng(46.025,-95.326667), defaultZoom: 7 },
	    'michigan': { defaultCenter: new google.maps.LatLng(44.386692,-84.805298), defaultZoom: 7 },
	    'michigan_vdt': { defaultCenter: new google.maps.LatLng(44.386692,-84.805298), defaultZoom: 7 },
	    'nevada': { defaultCenter: new google.maps.LatLng(38.702659,-116.436768), defaultZoom: 7 },
	    'nevada_vdt': { defaultCenter: new google.maps.LatLng(38.702659,-116.436768), defaultZoom: 7 }
	},
	radarOverlayLabel:'Radar',
	radarUpperRight: new google.maps.LatLng(69.565226,-135.101074),
	radarLowerLeft: new google.maps.LatLng(54.41893,-172.103027)
    },
    constructor:function(config) {
	this.initConfig(config);
	this.callParent([config]);
	var now = new Date();
	this.utc = new Date(
	    now.getUTCFullYear(),
	    now.getUTCMonth(),
	    now.getUTCDate(),
	    now.getUTCHours(),
	    now.getUTCMinutes(),
	    now.getUTCSeconds()
	);
    },
    setHrefState: function(state) {
    	this.hrefState = state;
    },
    getHrefState: function() {
    	var state = this.hrefState;
    	if (state == undefined || state == null) {
    	    console.log("State not initialized in config. Looking it up");
    		
    	    if ( window.location.href.indexOf('?') >= 0 ) {
		var paramsStr = window.location.href.substring( window.location.href.indexOf('?') + 1 );
		var hrefParams = Ext.urlDecode( paramsStr );
		state = hrefParams.state;
		
		if ( state == undefined || state == null ) {
		    console.log("State not found in the application url (example: \'&state=minnesota\'). Defaulting to minnesota");
		    state = 'minnesota';
		}
	    }
	    else {
		// Just make up a state -- default to minnesota
		console.log("No args provided in the application url. Defaulting to alaska");
		state = 'minnesota';
	    }
	    
	    this.hrefState = state;
    	}
    	
    	return this.hrefState;
    },
    getUseVdt: function() {
    	var useVdt = this.getUseVdtDefault();
		if ( window.location.href.indexOf('?') >= 0 ) {
			var paramsStr = window.location.href.substring( window.location.href.indexOf('?') + 1 );
			var hrefParams = Ext.urlDecode( paramsStr );
			vdt = hrefParams.useVdt;
			
			if ( vdt != undefined || vdt != null ) {
				if ( String(vdt).toLowerCase() == 'false' || String(vdt) == '0' ) {
					useVdt = false;
				}
				console.log("useVdt specified in the application url (" + hrefParams + "). Overriding default with value: " + useVdt);
			}
		}
		
		return useVdt;
    },
    lookupDefaultZoom: function() {
    	var state = this.getHrefState();
    	var lookup = this.getDefaultLocationLookup();
    	var zoom = lookup[state].defaultZoom;
    	return zoom;
    },
    lookupDefaultCenter: function() {
    	var state = this.getHrefState();
    	var lookup = this.getDefaultLocationLookup();
    	var center = lookup[state].defaultCenter;
    	return center;
    },
    getDefaultDate: function() {
	return Ext.Date.format(this.utc,"Ymd.Hi");
    },
    getRadarUrl: function(day,hhmm) {
	return this.getRadarImagesUrl() + day + "/AK.N0R." + day + "." + hhmm + ".png";
    },
    getRadarBounds: function() {
	return new google.maps.LatLngBounds(this.getRadarLowerLeft(),this.getRadarUpperRight());
    }
});
