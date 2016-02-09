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
		
	displayState: 'colorado',
	displayDistrict: '&district_name=district1',
	
	// // RAL Production
	forceSourceDomain: 'dot.rap.ucar.edu',
	districtAlertsUrl:'http://dot.rap.ucar.edu/district_alerts?path=/district_alerts',
	districtVehiclesUrl:'http://dot.rap.ucar.edu/districtvehicles?path=/districtvehicles',
	plotsUrl:'http://dot.rap.ucar.edu/plots?path=/plots',
	sitePlotsUrl:'http://dot.rap.ucar.edu/site_plots?path=/site_plots',
	siteDataUrl:'http://dot.rap.ucar.edu/site_data?path=/site_data',
	radarImagesUrl:'http://dot.rap.ucar.edu/radar_images?path=/radar_images/',
	dataTimesUrl:'http://dot.rap.ucar.edu/datatime?path=/datatime/',
	vehiclesUrl: 'http://dot.rap.ucar.edu/latest_vehicles?path=/latest_vehicles/',
	watchWarnsUrl: 'http://dot.rap.ucar.edu/noaa_watch_warnings?path=/noaa_watch_warnings/',
	
	// // RAL Dev
	// forceSourceDomain: 'www.ral.ucar.edu',
	// districtAlertsUrl:'http://www.ral.ucar.edu/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/district_alerts_dev',
	// districtVehiclesUrl:'http://www.ral.ucar.edu/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/districtvehicles_dev',
	// plotsUrl:'http://www.ral.ucar.edu/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/plots_dev',
	// radarImagesUrl:'http://www.ral.ucar.edu/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/radar_images_dev',
	// dataTimesUrl:'http://www.ral.ucar.edu/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/datatime_dev',
	// vehiclesUrl: 'http://www.ral.ucar.edu/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/latest_vehicles_dev',
	
	// Localhost Production
	// forceSourceDomain: 'localhost',
	// districtAlertsUrl:'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy_cdot.php?path=/district_alerts',
	// districtVehiclesUrl:'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy_cdot.php?path=/districtvehicles',
	// plotsUrl:'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy_cdot.php?path=/plots',
	// sitePlotsUrl:'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy_cdot.php?path=/site_plots',
	// siteDataUrl:'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy_cdot.php?path=/site_data',
	// radarImagesUrl:'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy.php?path=/radar_images/',
	// dataTimesUrl:'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy_cdot.php?path=/datatime/',
	// vehiclesUrl: 'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy_cdot.php?path=/latest_vehicles/',
	// watchWarnsUrl: 'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy_cdot.php?path=/noaa_watch_warnings/',
	// watchWarnsUrl: 'http://localhost:8888/projects/rdwx_mdss/AK_mdss/proxy_cdot.php?path=/example_watches.json',
	
	// // Localhost Dev
	// forceSourceDomain: 'localhost',
	// districtAlertsUrl:'http://localhost:8888/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/district_alerts_dev',
	// districtVehiclesUrl:'http://localhost:8888/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/districtvehicles_dev',
	// plotsUrl:'http://localhost:8888/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/plots_dev',
	// radarImagesUrl:'http://localhost:8888/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/radar_images_dev',
	// dataTimesUrl:'http://localhost:8888/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/datatime_dev',
	// vehiclesUrl: 'http://localhost:8888/projects/rdwx_mdss/cdot_proxy/proxy.php?path=/latest_vehicles_dev',
	
        ncwmsUrl: 'http://dot-maps.rap.ucar.edu',

	useVdtDefault:true,
	obsFcstLabel:'Obs',
	next06HrsFcstLabel:'Next 6 Hrs',
	next24HrsFcstLabel:'6 - 24 Hrs',
	hrs24to72FcstLabel:'24 - 72 Hrs',
	hideVehiclesLabel:'Hide Vehicles',
	showVehiclesLabel:'Show Vehicles',
	hideRadarLabel:'Hide Radar',
	showRadarLabel:'Show Radar',
        hideGreenSegmentsLabel:'Hide Green Segments',
        showGreenSegmentsLabel:'Show Green Segments',
	allSitesLabel:'All',
	roadCondSitesLabel:'Road Cond.',
	wxObsSitesLabel:'Wx Obs',
	rwisSitesLabel:'RWIS',
	hideSitesLabel:'Hide All',
	wxAlertsLabel:"Wx Alerts",
	roadCondAlertsLabel:"Road Cond. Alerts",
	obsAlertsLabel:"Obs Alerts",
	defaultAlertType:"wx",
        gridOverlayOpacity: 0.6,
        numRadarTimesteps: 20,
        animationFrameDelay: 400,
        animationDwell: 1500,
	// defaultZoom:4,
	// defaultCenter:new google.maps.LatLng(64.90491,-147.879272),
	defaultZoom:7,
	defaultCenter:new google.maps.LatLng(61.2167,-149.9000),
	defaultLocationLookup: {
	    'alaska':    { defaultCenter: new google.maps.LatLng(64.8333,-147.7167), defaultZoom: 8 },
	    'alaska_vdt':    { defaultCenter: new google.maps.LatLng(61.2167,-149.9000), defaultZoom: 7 },
	    'colorado': { defaultCenter: new google.maps.LatLng(39,-105.55), defaultZoom: 7 },
	    'colorado_vdt': { defaultCenter: new google.maps.LatLng(39,-105.55), defaultZoom: 7 },
	    'minnesota': { defaultCenter: new google.maps.LatLng(45.025,-95.326667), defaultZoom: 8 },
	    'minnesota_vdt': { defaultCenter: new google.maps.LatLng(45.025,-95.326667), defaultZoom: 8 },
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
				    console.log("State not found in the application url (example: \'&state=colorado\'). Defaulting to colorado");
				    state = 'colorado';
				}
		    }
		    else {
				// Just make up a state -- default to colorado
				console.log("No args provided in the application url. Defaulting to colorado");
				state = 'colorado';
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
