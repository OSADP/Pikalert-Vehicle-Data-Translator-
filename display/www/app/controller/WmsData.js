/**
 * WMS Data controller -- Everything related to gridded data.
 * @author paddy
 */
Ext.define('emdss.controller.WmsData',{
    extend:'Ext.app.Controller',
    requires:[
        'emdss.util.Config',
        'emdss.controller.VehicleController',
        'emdss.util.LatestWmsMgr'
    ],
    config:{
        refs:{
            mainTabPanel: 'main[id=main-tab-panel]',
            mapView: 'main map',
            mapToolbar: 'main toolbar[id=map-toolbar]',
            btnRadar: 'main button[id=hide-show-radar-button]',
            panelRadar: 'main panel[id=radar-panel]',
            panelRadarInfo: '#radar-info-panel',
            animControlsPanel: '#radar-controls-panel',
            labelRadarInfo: '#radar-info-label',
            loadUnloadAnimationButton : '#load-unload-animation-button'
        },
        control: {
            btnRadar: {
                release: 'onHideShowRadarRelease'
            }
        }
    },
    
    initialLoadComplete: false,
    
    visibleLayer: null,
    timesteps: [],
    availableLayers: [],
    delayedInitLayers: [],

    /**
     * Initialize the WMS Data elements
     */
    init: function() {
        var me = this;
        
        console.log("In WmsData::init()...");

        me.showRadar = true;
        me.latestRadarDate = null;

        //
        // Set up the listeners
        // (bad style)
        //
        // this.control({
        //     '#hide-show-radar-button': {
        //         release: function(button, released, eOpts) {
        //             console.log('hide-show-radar-button button : ' + released);
        //             me.onHideShowRadarRelease(button, released, eOpts);
        //         }
        //     }
        // });
    },
    
    /**
     * Initialize the WMS Data elements:
     *   - Start the timer on the LatestWmsMgr singleton.
     *   - Register dateAdded and dateRemoved callbacks with LatestWmsMgr
     */
    launch: function() {
        var me = this;

        console.log("In WmsData::launch()...");

        // 
        // Load previous state of whether radar is showing or not.
        // 
        if ( emdss.util.Util.supports_html5_storage() ) {
            var showRadar = window.localStorage.getItem('showRadar');
            if ( showRadar == 'false' ) {
                me.showRadarLayerAndButtons(false);
            }
        }
        
        //
        // Gridded data times
        // 
        me.latestWmsMgr = emdss.util.LatestWmsMgr;
        // this.latestWmsMgr.startGetTimesTimer(60000); // 1 min
        me.latestWmsMgr.startGetTimesTimer(180000); // 3 min
        
        // 
        // Handle only the initial load of the LatestWmsMgr. 
        //   Subsequent updates are managed incrementally via dateAdded() and dateRemoved() callbacks, below.
        // 
        emdss.util.LatestWmsMgr.addLoadedCallback( function(mgr) {
            if ( !me.initialLoadComplete ) {
                me.initialLoadComplete = true;
                
                var latestTimestep = me.latestWmsMgr.getLatestDate();
                me.makeDelayedInitTimestepVisible(latestTimestep);
                
                me.processDelayedLayers();
            }
            else {
                console.log("=============");
                console.log("Load Complete");
                console.log("=============");
                if ( emdss.app.getController('MapAnimation').isAnimLoggingEnabled() ) {
                    me.printTimesteps();
                    me.printAvailableRadarLayers();
                    me.printMapRadarLayers();
                }
        
                emdss.app.getController('MapAnimation').syncAnimationTimes(new Date(), me.timesteps);
                
                // Let the sequenceChanged() callback update what's visible.
                // var latestTimestep = me.latestWmsMgr.getLatestDate();
                // me.makeVisibilitySwitch(latestTimestep);
            }
        });
        
        // 
        // Add an overlay for a new time on the LatestWmsMgr.
        //  - based on example at: view-source:http://www.sumbera.com/lab/GoogleV3/tiledWMSoverlayGoogleV3.htm
        // 
        var map = me.getMapView().getMap();
        emdss.util.LatestWmsMgr.addDateAddedCallback( function(mgr, addedDate) {
            me.addLayerForTimestep(addedDate);
        });
        
        // 
        // Remove the overlay when it's no longer relevant to the LatestWmsMgr.
        // 
        emdss.util.LatestWmsMgr.addDateRemovedCallback( function(mgr, removedDate) {
            me.removeLayerForTimestep(removedDate);
        });
    },

    WMSGetTileUrl: function(tile, zoom) {
        var mapView = Ext.getCmp("map-view");
        var map = mapView.getMap();

        // Get the tile time out of the MapLayerType name
        //   ( context here is WmsMapType )
        var name = this.name;
        var timeStr = name.replace("Radar-", "");

        var projection = map.getProjection();
        var zpow = Math.pow(2, zoom);
        var ul = new google.maps.Point(tile.x * 256.0 / zpow, (tile.y + 1) * 256.0 / zpow);
        var lr = new google.maps.Point((tile.x + 1) * 256.0 / zpow, (tile.y) * 256.0 / zpow);
        var ulw = projection.fromPointToLatLng(ul);
        var lrw = projection.fromPointToLatLng(lr);
        
        var mercatorBounds = emdss.util.Mercator.tileBounds(tile.x, tile.y, zoom);
        
        // http://weather.aero/mm-ncWMS/wms?LAYERS=CREF/CREF&VERSION=1.3.0&SRS=EPSG:3857&STYLES=boxfill/dbz&TRANSPARENT=true&FORMAT=image/png&mode=32bit&BELOWMINCOLOR=transparent&ABOVEMAXCOLOR=transparent&EXCEPTIONS=INIMAGE&SERVICE=WMS&REQUEST=GetMap&TIME=2014-05-07T20:04:0.000Z&CRS=EPSG:3857&BBOX=39.375,-81.5625,42.1875,-78.75&WIDTH=256&HEIGHT=256
        // http://weather.aero/mm-ncWMS/wms?LAYERS=CREF/CREF&VERSION=1.3.0&STYLES=boxfill/dbz&TRANSPARENT=true&FORMAT=image/png&mode=32bit&BELOWMINCOLOR=transparent&ABOVEMAXCOLOR=transparent&EXCEPTIONS=INIMAGE&SERVICE=WMS&REQUEST=GetMap&TIME=2014-05-07T20:04:0.000Z&CRS=EPSG:3857&BBOX=39.375,-81.5625,42.1875,-78.75&WIDTH=256&HEIGHT=256
        
        var nowDate = new Date();

        // baseURL = "http://weather.aero/mm-ncWMS/wms?REQUEST=GetMap&SERVICE=WMS&VERSION=1.3.0&EXCEPTIONS=INIMAGE";
        baseURL = "http://dot-maps.rap.ucar.edu/ncWMS/wms?REQUEST=GetMap&SERVICE=WMS&VERSION=1.3.0&EXCEPTIONS=INIMAGE";
        // baseURL = "http://manucode:8080/ncWMS2/wms?REQUEST=GetMap&SERVICE=WMS&VERSION=1.3.0&EXCEPTIONS=INIMAGE";
        var format = "image%2Fpng%3Bmode=32bit";
        // var layers = "CREF%2FCREF";
        var layers = "MergedBaseReflectivityQC%2FMergedBaseReflectivityQC";
        var srs = "EPSG:3857";
        // var bbox = ulw.lat() + "," + ulw.lng() + "," + lrw.lat() + "," + lrw.lng();
        // var bbox = ul.x + "," + ul.y + "," + lr.x + "," + lr.y;
        var bbox = mercatorBounds[0] + "," + Math.abs(mercatorBounds[3]) + "," + mercatorBounds[2] + "," + Math.abs(mercatorBounds[1]);
        var url = baseURL 
                  + "&CRS=" + srs 
                  + "&LAYERS=" + layers
                  + "&WIDTH=256&HEIGHT=256"
                  // + "&STYLES=boxfill/dbz&TRANSPARENT=true&FORMAT=" + format 
                  + "&STYLES=boxfill/dbz&TRANSPARENT=true&FORMAT=" + format 
                  // + "&STYLES=default&TRANSPARENT=true&FORMAT=" + format 
                  + "&BELOWMINCOLOR=transparent&ABOVEMAXCOLOR=transparent&EXCEPTIONS=INIMAGE"
                  // + "&TIME=2014-05-07T20%3A04%3A0.000Z"
                  // + "&TIME=2015-11-04T13%3A46%3A00.000Z"
                  // + "&TIME=2015-11-04T15%3A38%3A00.000Z"
                  // + "&TIME=" + emdss.util.Util.dateToIsoString(emdss.util.LatestWmsMgr.getLatestDate())
                  + "&TIME=" + timeStr
                  + "&BBOX=" + bbox
                  + "&voznot=" + nowDate.getTime();
        // //The user will enter the address to the public WMS layer here.  The data must be in WGS84
        // var baseURL = "http://sampleserver1.arcgisonline.com/arcgis/services/Specialty/ESRI_StatesCitiesRivers_USA/MapServer/WMSServer?&REQUEST=GetMap&SERVICE=WMS&VERSION=1.3&LAYERS="; //begining of the WMS URL ending with a "?" or a "&".
        // var format = "image%2Fjpeg"; //type of image returned  or image/jpeg
        // //The layer ID.  Can be found when using the layers properties toolin ArcMap
        // var layers = "0";
        // var srs = "EPSG:4326"; //projection to display. This is the projection of google map. Don't change unless you know what you are doing.
        // var bbox = ulw.lat() + "," + ulw.lng() + "," + lrw.lat() + "," + lrw.lng();
                // //Add the components of the URL together
        // var url = baseURL + layers + "&Styles=default" + "&SRS=" + srs + "&BBOX=" + bbox + "&width=256" + "&height=256" + "&format=" + format + "&BGCOLOR=0xFFFFFF&TRANSPARENT=true" + "&reaspect=false" + "&CRS=EPSG:4326";
        return url;
    },

    /**
     * Get the latest radar images.
     */
    getRadarImages: function() {
        var appController = emdss.app.getController('emdss.controller.Application');
        console.log(appController.currDate);
        var date = Ext.Date.parse(appController.currDate, "Ymd.Hi");
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
        var map = this.getMapView().getMap();

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

    processDelayedLayers: function() {
        
        var me = this;
        var map = me.getMapView().getMap();
        var len = me.delayedInitLayers.length;
        console.log("Executing delayed init on " + len + " layers...");
        for ( var i = len-1; i >= 0; i-- ) {
            var currOccupant = me.availableLayers[i];
            if ( typeof currOccupant != 'undefined' && currOccupant != null ) {
                console.log("Warning: Clobbing a radar layer in the availableLayers array at index: " + i);
                debugger;
            }
            
            // Don't load these in the map yet -- causes cascade of ncWMS requests.
            // map.overlayMapTypes.setAt(i, me.delayedInitLayers[i]);
            
            // Update our cache of available layers
            me.availableLayers[i] = me.delayedInitLayers[i];
        }
        
        me.delayedInitLayers = [];
        
        // Sync up the timesteps and notify the animation controller.
        me.updateTimesteps();
        console.log("");
        emdss.app.getController('MapAnimation').syncAnimationTimes(new Date(), me.timesteps);
    },
    
    /**
     * Handle the initial load of LatestWmsMgr, onLoadCallback(), where one layer is added to the map
     *   and made visible.
     */
    makeDelayedInitTimestepVisible: function(visDate) {
        var me = this;
        
        // 
        // First, find the timestep in the delayed array, and set it:
        //   - as availableLayers[i]
        //   - as map.overlayMapTypes[len-1]
        //   - as the current radarMap on this controller.
        // 
        var map = me.getMapView().getMap();
        var delayedLen = me.delayedInitLayers.length;
        for ( var i = 0; i < delayedLen; i++ ) {
            thisDate = me.delayedInitLayers[i].emdssDate;
            if ( thisDate.getTime() == visDate.getTime() ) {
                console.log("\n\n\nFound delayed layer: " + me.delayedInitLayers[i].name + '\n\n');
                var currOccupant = map.overlayMapTypes.getAt(delayedLen-1);
                if ( typeof currOccupant != 'undefined' && currOccupant != null ) {
                    console.log("Warning: Clobbing a radar layer at index: " + (delayedLen-1));
                    debugger;
                }
                me.findPlaceForLayerInMapOverlays(map.overlayMapTypes, me.delayedInitLayers[i]);
                me.availableLayers[i] = me.delayedInitLayers[i];
                me.delayedInitLayers.splice(i, 1);
                
                break;
            }
        }
        
        // 
        // Then, set the desired layer to opaque and all others (shouldn't be any!) to transparent.
        // 
        var len = map.overlayMapTypes.length;
        for ( var i = 0; i < len; i++ ) {
            var currLayer = map.overlayMapTypes.getAt(i);
            if ( currLayer == null ) {
                continue;
            }
            
            if (currLayer.emdssDate.getTime() == visDate.getTime()) {
                currLayer.setOpacity(me.showRadar ? emdss.util.Config.getGridOverlayOpacity() : 0.0);
                me.setRadarMap(currLayer);
                me.setVisibleLayer(currLayer);
                
                var timeLabel = me.getLabelRadarInfo();
                timeLabel.setHtml("Radar time: " + emdss.util.Util.dateToPrettyShortDateString(currLayer.emdssDate));
            }
            else if (currLayer.getOpacity() > 0.0) {
                currLayer.setOpacity(0.0);
                console.log("Warning: Found unexpected layer in WmsData::makeDelayedInitTimestepVisible(): " + currLayer.name);
            }
        }
    },
    
    /**
     * Find a layer in the map corresponding to the passed-in date, or load one in the 
     *   map from the available layers. Return the layer, or null if nothing was found.
     */
    findOrLoadLayerForTimestep: function(visDate) {
        var me = this;
        
        // 
        // Search the map layers for the desired timestep.
        // 
        var loadedLayer = null;
        var map = me.getMapView().getMap();
        var len = map.overlayMapTypes.length;
        for ( var i = 0; i < len; i++ ) {
            var currLayer = map.overlayMapTypes.getAt(i);
            if ( currLayer == null ) {
                continue;
            }
            
            if (currLayer.emdssDate.getTime() == visDate.getTime()) {
                loadedLayer = currLayer;
                break;
            }
        }
        
        // 
        // If it's not found in the map, locate it in the available layers and load it in the map.
        // 
        if ( loadedLayer == null ) {
            // Find the available layer and load it
            var len2 = me.availableLayers.length;
            for ( var j = 0; j < len2; j++ ) {
                var currLayer = me.availableLayers[j];
                if ( currLayer == null ) {
                    continue;
                }
                
                if ( currLayer.emdssDate.getTime() == visDate.getTime() ) {
                    loadedLayer = currLayer;
                    currLayer.setOpacity(0.0);
                    me.findPlaceForLayerInMapOverlays(map.overlayMapTypes, currLayer);
                    break;
                }
            }
        }
        
        return loadedLayer;
    },
    
    /**
     * Find a layer corresponding to the passed-in date and make sure it is loaded in the map. Return it
     *   with a reference to the currently-visible layer.
     */
    prepareLayersForVisibilitySwitch: function(visDate) {
        var me = this;
        
        var loadedLayer = me.findOrLoadLayerForTimestep(visDate);
        
        var oldLayer = me.getVisibleLayer();
        var oldLayerDate = oldLayer.emdssDate;
        
        // 
        // If it's still not found -- Error! Or make it visible.
        // 
        if ( loadedLayer == null ) {
            console.log('ERROR!!!! Could not find an available layer for timestep: ' + visDate);
        }
        else {
            // Nada.
        }
        
        return {new: loadedLayer, old: oldLayer};
    },
    
    /**
     * Transition from the currently visible layer to a layer corresponding to the passed-in date.
     *   This method makes the new layer opaque, then makes the old layer transparent.
     */
    makeVisibilitySwitch: function(visDate) {
        var me = this;
        
        var layers = me.prepareLayersForVisibilitySwitch(visDate);
        var newLayer = layers.new;
        var oldLayer = layers.old;
        
        newLayer.setOpacity( emdss.util.Config.getGridOverlayOpacity() );
        me.setRadarMap(newLayer);
        me.setVisibleLayer(newLayer);
        
        if ( oldLayer ) {
            oldLayer.setOpacity(0.0);
        }
        
        if ( emdss.app.getController('MapAnimation').isAnimLoggingEnabled() ) {
            console.log("At end of makeVisibilitySwitch():");
            console.log("    Visible Layer: " + me.getVisibleLayer().name);
            console.log("    Visible Layer Opacity: " + me.getVisibleLayer().getOpacity());
            me.printMapRadarLayers();
        }
        
        return newLayer;
    },
    
    /**
     * Find a layer corresponding to the passed-in date and make it opaque. Then make all
     *   other layers transparent.
     */
    ensureTimestepVisibility: function(visDate) {
        var me = this;
        
        var desiredLayer = me.findOrLoadLayerForTimestep(visDate);
        
        // 
        // Make the desiredLayer opaque and all others transparent
        // 
        me.setRadarMap(null);
        me.setVisibleLayer(null);
        var map = me.getMapView().getMap();
        var len = map.overlayMapTypes.length;
        for ( var i = 0; i < len; i++ ) {
            var currLayer = map.overlayMapTypes.getAt(i);
            if ( currLayer == null ) {
                continue;
            }
            
            if (currLayer == desiredLayer) {
                currLayer.setOpacity( emdss.util.Config.getGridOverlayOpacity() );
                me.setRadarMap(currLayer);
                me.setVisibleLayer(currLayer);
            }
            else {
                currLayer.setOpacity(0.0);
            }
        }
    },
    
    addLayerForTimestep: function(addedDate) {
        var me = this;
        var map = me.getMapView().getMap();
        
        // Add a new radar overlay
        var wmsOptions = {
            name: "Radar-" + emdss.util.Util.dateToIsoString(addedDate),
            alt: "NcWMS Radar Layer",
            getTileUrl: me.WMSGetTileUrl,
            isPng: false,
            maxZoom: 17,
            minZoom: 6,
            tileSize: new google.maps.Size(256, 256),
            credit: 'credit here',
            // opacity: emdss.util.Config.getGridOverlayOpacity()
            opacity: 0.0
        };
        wmsMapType = new google.maps.ImageMapType(wmsOptions);
        wmsMapType.emdssDate = addedDate;
        var prevRadarLayer = me.getRadarMap();

        if ( me.initialLoadComplete ) {
            console.log("Directly adding layer: " + wmsMapType.name);
            me.availableLayers.push(wmsMapType);  // add onto end of array.
            me.findPlaceForLayerInMapOverlays(map.overlayMapTypes, wmsMapType); // fit in wherever.
            me.setRadarMap(wmsMapType);
        }
        else {
            console.log("Scheduling delayed addition of layer: " + wmsMapType.name);
            me.delayedInitLayers.push(wmsMapType);
        }

        // 
        // Register for boundschanged event (testing for future animation work)
        // 
        var prevBounds = map.getBounds();
        wmsMapType.prevBounds = prevBounds;
        google.maps.event.addListener(wmsMapType, 'tilesloaded', function(evt) {
            var currBounds = map.getBounds();
            
            console.log("");
            console.log("");
            console.log("    BOUNDS CHANGED!!! (" + this.name + "). Bounds: " + currBounds);
            console.log("");
            console.log("");
            
            if ( this.prevBounds !== currBounds ) {
                this.tilesNeedLoading = true;
                this.prevBounds = currBounds;
            }
        });
        
        // 
        // Register for tilesloaded event (testing for future animation work)
        // 
        google.maps.event.addListener(wmsMapType, 'tilesloaded', function(evt) {
            console.log("");
            console.log("");
            console.log("    ALL RADAR TILES LOADED!!! (" + this.name + ")");
            console.log("");
            console.log("");
            
            this.tilesNeedLoading = false;
            me.getLoadUnloadAnimationButton().enable();
        });

        if ( me.initialLoadComplete ) {
            // Fix up the timestep array
            me.updateTimesteps();
        }
        
        console.log("\nAdded Layer" + (me.initialLoadComplete ? ": " : " (DELAYED): ") + wmsMapType.name);
    },
    
    removeLayerForTimestep: function(visDate) {
        var me = this;
        var layerRemoved = false;
        
        
        // 
        // TODO: Make sure that timestep wasn't visible!
        // 
        
        
        // 
        // Remove the timestep from the map overlays.
        // 
        var map = me.getMapView().getMap();
        var len = map.overlayMapTypes.length;
        for ( var i = 0; i < len; i++ ) {
            var currLayer = map.overlayMapTypes.getAt(i);
            if ( currLayer == null ) {
                continue;
            }
            
            if (currLayer.emdssDate.getTime() == visDate.getTime()) {
                currLayer.setOpacity(0.0);
                map.overlayMapTypes.setAt(i, null);
                layerRemoved = true;
                break;
            }
        }
        
        // 
        // Remove the timestep from the available overlays.
        // 
        var len2 = me.availableLayers.length;
        for ( var j = 0; j < len2; j++ ) {
            var currLayer = me.availableLayers[j];
            if ( !currLayer ) {
                continue;
            }
            
            if ( currLayer.emdssDate.getTime() == visDate.getTime() ) {
                me.availableLayers.splice(j, 1);
                break;
            }
        }
        
        if ( layerRemoved ) {
            me.updateTimesteps();
        }
        else {
            console.log('Warning: Layer not found for timestep ' + visDate + ' in WmsData::removeLayerForTimestep')
        }
    },
    
    // updateTimesteps: function() {
        // var me = this;
        // var newTimesteps = [];
        // var map = me.getMapView().getMap();
        // var len = map.overlayMapTypes.length;
        // for ( var i = 0; i < len; i++ ) {
            // var currLayer = map.overlayMapTypes.getAt(i);
            // if ( currLayer == null ) {
                // continue;
            // }
//             
            // newTimesteps.push(currLayer.emdssDate);
        // }
//         
        // newTimesteps.sort( function(a, b){ return (a.getTime() - b.getTime())} );
        // me.timesteps = newTimesteps;
    // },
    
    updateTimesteps: function() {
        var me = this;
        var newTimesteps = [];
        var len = me.availableLayers.length;
        for ( var i = 0; i < len; i++ ) {
            var currLayer = me.availableLayers[i];
            if ( currLayer == null ) {
                console.log("Error: Found null element in availableLayers...");
                continue;
            }
            
            newTimesteps.push(currLayer.emdssDate);
        }
        
        newTimesteps.sort( function(a, b){ return (a.getTime() - b.getTime())} );
        me.timesteps = newTimesteps;
    },
    
    printMapRadarLayers: function() {
        var me = this;
        var map = me.getMapView().getMap();
        var len = map.overlayMapTypes.length;
        console.log("\nCurrent Radar Layers:");
        for ( var i = 0; i < len; i++ ) {
            var layer = map.overlayMapTypes.getAt(i);
            console.log("    map.overlayMapTypes[" + i + "]: (Opacity " + (layer ? layer.getOpacity() : 'unknown') + ") " + (layer ? layer.name : 'null'));
        }
        console.log("\n");
    },
    
    printAvailableRadarLayers: function() {
        var me = this;
        var len = me.availableLayers.length;
        console.log("\nAvailable Radar Layers:");
        for ( var i = 0; i < len; i++ ) {
            var layer = me.availableLayers[i];
            console.log("    availableLayers[" + i + "]: " + (layer ? layer.name : 'null'));
        }
        console.log("\n");
    },
    
    printTimesteps: function() {
        var me = this;
        var len = me.timesteps.length;
        console.log("\nCurrent Timesteps:");
        for ( var i = 0; i < len; i++ ) {
            var step = me.timesteps[i];
            console.log("    timestep[" + i + "]: " + step);
        }
        console.log("\n");
    },
    
    verifyOneTimestepVisible: function() {
        var me = this;
        var map = me.getMapView().getMap();
        
        var len = map.overlayMapTypes.length;
        var lastVisibleLayer = null;
        var currentlyVisibleLayer = me.getVisibleLayer();
        var currentlyVisibleLayerName = (currentlyVisibleLayer ? currentlyVisibleLayer.name : 'null');
        var visibleTimestepCount = 0;
        for ( var i = 0; i < len; i++ ) {
            var currLayer = map.overlayMapTypes.getAt(i);
            if ( currLayer == null ) {
                continue;
            }
            
            if (currLayer.getOpacity() > 0.0) {
                lastVisibleLayer = currLayer;
                visibleTimestepCount++;
            }
        }
        
        // TODO: Take this out (dependence on MapAnimation controller) -- debugging only.
        var currAnimFrame = emdss.app.getController('MapAnimation').animationController.getTimeSequence().getCurrentFrame();
        
        var lastVisibleLayerName = (lastVisibleLayer ? lastVisibleLayer.name : 'null');
        if ( visibleTimestepCount == 1 ) {
            if ( lastVisibleLayer !== currentlyVisibleLayer ) {
                console.log("WARNING: One animation timestep visible (" + lastVisibleLayerName + "), but WmsData controller thinks another one is visible: " + currentlyVisibleLayerName + " AnimationController currentFrame: " + currAnimFrame);
                
                return false;
            }
            if ( currAnimFrame.getTime() !== currentlyVisibleLayer.emdssDate.getTime() ) {
                console.log("WARNING: One animation timestep visible (" + lastVisibleLayerName + "), but animation controller thinks another frame is current: " + currAnimFrame);
                
                return false;
            }
            return true;
        }
        else if ( visibleTimestepCount > 1 ) {
            console.log("WARNING: More than one visible animation timestep. Count: " + visibleTimestepCount + " Current visible layer: " + currentlyVisibleLayerName + " Animation Controller currentFrame: " + currAnimFrame);
            
            return false;
        }
        else {
            console.log("WARNING: No visible animation timestep. Count: " + visibleTimestepCount + " AnimationController currentFrame: " + currAnimFrame);
            return false;
        }
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
     * Handle clicks to the radar visibility button.
     */
    onHideShowRadarRelease: function(button, released, eOpts) {
        this.toggleShowRadar(button);
    },

    toggleShowRadar: function(button) {
        var me = this;

        me.showRadarLayerAndButtons(!me.showRadar);
            
        if ( emdss.util.Util.supports_html5_storage() ) {
            window.localStorage.setItem('showRadar', me.showRadar);
        }
    },
    
    showRadarLayerAndButtons: function(show) {
        var me = this;
        
        if ( me.pendingHide ) {
            clearTimeout(me.pendingHide);
            me.pendingHide = null;
        }
        
        // Check and see if animation is loaded.
        var loadUnloadAnimationButton = me.getLoadUnloadAnimationButton();
        if ( loadUnloadAnimationButton.isLoaded ) {
            // Unload the animation (handles case where animation is in progress).
            emdss.app.getController('MapAnimation').loadUnloadAnimationButtonTapped(loadUnloadAnimationButton);
        }
        
        var map = me.getMapView().getMap();
        var button = me.getBtnRadar();
        var radar = me.getRadarMap();
        var radarPanel = me.getPanelRadar();
        var radarInfoPanel = me.getPanelRadarInfo();
        var animControlsPanel = me.getAnimControlsPanel();
        
        if (show) {
            me.showRadar = true;
            button.setHtml(emdss.util.Config.getHideRadarLabel());
            if ( radar ) {
                radar.setOpacity(emdss.util.Config.getGridOverlayOpacity());
            }
            radarPanel.show();
            radarInfoPanel.show();
            animControlsPanel.hide();
        } else {
            me.showRadar = false;
            button.setHtml(emdss.util.Config.getShowRadarLabel());
            if ( radar ) {
                radar.setOpacity(0.0);
            }
            radarInfoPanel.hide();
            animControlsPanel.hide();
            me.pendingHide = setTimeout(function() {
                radarPanel.hide();
                me.pendingHide = null;
            }, 1000);
        }
    },
    
    playButtonDisableChanged: function(btn, value, oldValue, eOpts) {
        console.log("playButtonDisableChanged() with value: " + value);
        if ( btn.isPlaying ) {
            btn.setIconCls( value ? 'animation-pause-icon-disabled' : 'animation-pause-icon');
        }
        else {
            btn.setIconCls( value ? 'animation-play-icon-disabled' : 'animation-play-icon');
        }
    },
    
    findPlaceForLayerInMapOverlays: function(overlays, newLayer) {
        var foundSpot = false;
        var len = overlays.length;
        for (var i = 0; i < len; i++) {
            var currLayer = overlays.getAt(i);
            if ( currLayer == null ) {
                foundSpot = true;
                overlays.setAt(i, newLayer);
                break;
            }
        }
        
        if ( foundSpot == false ) {
            overlays.push(newLayer);
        }
    },
    
    /**
     * Show the radar controls panel (contains the time label, etc) -- if radar is showing and the map panel is selected.
     */
    showRadarPanel: function() {
        var me = this;
        var radarPanel = this.getPanelRadar();
        var tabPanel = me.getMainTabPanel();
        if ( tabPanel.getActiveItem().getId() == 'map-nav-view' && me.showRadar ) {
            radarPanel.show();
        }
    },
    
    setRadarMap: function(newMap) {
        this.radarMap = newMap;
    },
    
    getRadarMap: function() {
        return this.radarMap ? this.radarMap : null;
    },
    
    setLatestRadarDate: function(newDate) {
        this.latestRadarDate = newDate;
    },
    
    getLatestRadarDate: function() {
        return this.latestRadarDate ? null : this.latestRadarDate;
    },
    
    setVisibleLayer: function(layer) {
        if (emdss.app.getController('MapAnimation').isAnimLoggingEnabled()) console.log("WmsData::setVisibleLayer() --> " + (layer ? layer.name : 'null'));
        this.visibleLayer = layer;
    },
    
    getVisibleLayer: function() {
        return this.visibleLayer;
    }

});
