/**
 * Vehicle controller -- handle vehicle-related data and actions
 * @author paddy
 */
Ext.define('emdss.controller.VehicleController', {
    extend:'Ext.app.Controller',
    requires:[
        'emdss.util.Config',
        'emdss.util.Util'
    ],
    config:{
        refs:{
            mainTabPanel: 'main[id=main-tab-panel]',
            mapNavView: '#map-nav-view',
            mapView: 'main map',
            btnVehicles: 'main button[id=hide-show-vehicles-button]',
            listItemVehicles: '#vehicle-list-item'
        },
        control: {'btnVehicles': {
                release: 'onHideShowVehiclesRelease'
        },
            'listItemVehicles': {
                itemtap: 'onVehicleSelect'
            }
        }
    },
    
    latest_data: Array,
    
    /**
     * Set up the vehicle store, start its update timer, and register callback for
     *   clicks on the vehicle list.
     */
    init: function() {
        var me = this;
        
        console.log("In VehicleController::init()...");
    
        //
        // Initialize the show vehicles toggle.  This state must match the label
        // given to the hide-show-vehicles-button in view/Main.js (i.e. must be
        // true if label is "Hide Vehicles" and false if label is "Show Vehicles".
        //
    
        me.showVehicles = true;
    
        // 
        // District Vehicles
        // 
        this.districtVehicleStore = Ext.create("emdss.store.DistrictVehicleStore",
                                   {
                               storeId:"district-vehicle-store",
                               constructor: function(config) {
                                       console.log(config);
                                   
                                   console.log("In DistrictVehicleStore constuctor()...");
                                   
                                   this.callParent([config]);
                               },
                               init:function() {
                                       console.log("In DistrictVehicleStore init...");
                               }
                               });
        
        console.log("Done creating DistrictVehicleStore...");
        
        this.districtVehicleStore.setupAfterListener();
        
        this.districtVehicleStore.setProxy({
            type:'ajax',
            url: emdss.util.Config.getVehiclesUrl() + '&state=' + emdss.util.Config.getHrefState() + '_vdt',
            // url: emdss.util.Config.getTestVehiclesUrl(),
            reader: {
                    type:         'json',
                    rootProperty: 'districts'
                }
        });
        this.districtVehicleStore.on({
            refresh: 'onVehicleListUpdate',
            scope: this
        });
        
        this.districtVehicleStore.on({
            load: 'setDataTimeOnDistricts',
            scope: this.districtVehicleStore
        });
        
        this.districtVehicleStore.load();
        this.districtVehicleStore.startReloadTimer(60000); // 60000 ms == 1 min
        
        
        // this.control({
        //     '#vehicle-list-item': {
        //         itemtap: function(list, index, node, record, senchaEvent) {
        //             console.log('site-list-item itemtap: ' + senchaEvent.event.target.className );
        //             me.onVehicleSelect(list, index, node, record, senchaEvent);
        //         }
        //     },
        // });
    },
    
    /**
     * Not used.
     */
    launch: function() {
        var me = this;
        
        // 
        // Load previous state of whether vehicles are showing or not.
        // 
        if ( emdss.util.Util.supports_html5_storage() ) {
            var showVehicles = window.localStorage.getItem('showVehicles');
            if ( showVehicles == 'false' ) {
                me.showVehiclesMethod(false);
            }
        }
    },

    /**
     * Handle clicks to the vehicle visibility button.
     */
    onHideShowVehiclesRelease: function(button, released, eOpts) {
        this.toggleShowVehicles(button);
    },
    
    /**
     * Handle clicks within the vehicle list. This looks around in the css hierarcy to figure out exactly what
     *   was clicked, and performs the appropriate action:
     *     - switch to the map view and zoom to the bounds of the selected district
     *     - switch to the map view and highlight the selected vehicle
     * @param {Object} list
     * @param {Object} index
     * @param {Object} node
     * @param {Object} record
     * @param {Object} senchaEvent
     */
    onVehicleSelect: function(list, index, node, record, senchaEvent) {
        var me = this;
        
        console.log("VehicleController::onVehicleSelect()...");
        
        if ( senchaEvent.event.target.className === 'district_zoom' ) {
            
            // Zoom in to the bounds of the district.
            
            var siblings = senchaEvent.event.target.parentElement.children;
            var districtName = siblings[0].innerText;
            var districtVehicleStore = Ext.data.StoreManager.lookup('district-vehicle-store');
            var idx = districtVehicleStore.find( 'district_name', districtName, 0, false, true, true );
            if ( idx > -1 ) {
                var district = districtVehicleStore.getAt(idx);
                var map = me.getMapView().getMap();
                this.getApplication().getController('emdss.controller.Application').zoomToDistrict(map, district);
            }
        }
        else if ( senchaEvent.event.target.className === 'vehicle_node' ||
              senchaEvent.event.target.className === 'vehicle_description' ||
              senchaEvent.event.target.className === 'vehicle_report' ) {
                
            // Highlight the selected vehicle on the map.
            
            var districtVehicleStore = Ext.data.StoreManager.lookup('district-vehicle-store');
            var parent = senchaEvent.event.target.parentElement;
            if ( senchaEvent.event.target.className === '' ) {
                parent = senchaEvent.event.target.parentElement.parentElement;
            }
            var vehicleLine = parent.textContent;
            var vehicleStart = vehicleLine.substr(vehicleLine.indexOf('Vehicle: ') + 9); // Strip off preceeding "Vehicle: "...
            var vehicleName = vehicleStart.substr(0, vehicleStart.indexOf(' '));
            districtVehicleStore.each(function (district, index, length) {
                var vehicleStore = district.vehicles();
                var idx = vehicleStore.find( 'id', vehicleName, 0, false, true, true );
                if ( idx > -1 ) {
                    var vehicleRecord = vehicleStore.getAt(idx);
                    me.setSelectedVehicle(vehicleRecord);
                    
                    // 
                    // Switch to the map view.
                    // 
                    var tabPanel = Ext.getCmp("main-tab-panel");
                    tabPanel.setActiveItem(1);
                    return;
                }
            });
        }
        else {
            debugger;
        }
    },
    
    /**
     * Highlight the indicated vehicle on the map.
     * @param {Object} vehicleRecord
     */
    setSelectedVehicle: function(vehicleRecord) {
        var me = this;
        
        // 
        // Set the selected vehicle record and re-initialize the map vehicles.
        // 
        me.selectedVehicleRecord = vehicleRecord;
        var map = me.getMapView().getMap();
        me.addVehiclesToMap(me.latest_vehicle_data.items, map);
    },
    
    /**
     * Handle vehicle store refresh events. Refresh the vehicle markers on the map.
     */
    onVehicleListUpdate: function(store, data, eOpts) {
        this.latest_vehicle_data = data;
        records = data.items;
        var map = this.getMapView().getMap();
        this.addVehiclesToMap(records, map);
    },
    
    /**
     * Clear vehicle markers from the map.
     */
    clearMarkers: function() {
        var me = this;
        
        // Clear the current vehicle markers
        
        var lt;
        if (me.markers) {
            for (var i = 0; i < this.markers.length; i++) {
                me.markers[i].setMap(null);
            }
        }
    },
    
    /**
     * Add vehicle markers to the map.
     */
    displayMarkers: function() {
        var me = this;
        
        // Get a reference to the map
        var map = me.getMapView().getMap();
    
        // Display the current vehicle markers
        
        var lt;
        if (me.markers) {
            for (var i = 0; i < this.markers.length; i++) {
                me.markers[i].setMap(map);
            }
        }
    },
    
    /**
     * Refresh the map with vehicle markers for the passed-in records.
     * @param {Object} records
     * @param {Object} map
     */
    addVehiclesToMap: function(records, map) {
        var me = this;
        
        if ( map == null ) {
            console.log("Warning:");
            console.log("Warning: Map is null in addVehiclesToMap(). Would have added vehicles from " + records.length + " districts...");
            console.log("Warning:");
            
            return;
        }
        
        // Clear the current vehicle markers from the map and clear out the marker list
        
        me.clearMarkers();
        me.markers = [];
        
        // Process all of the new records.  There is a record of vehicles for each district.
        
        for ( var key in records ) {
            var district = records[key];
            
            // Process all of the vehicles in the district
            
            var vehicles = district.vehicles(); // returns Ext.Store of vehicles...
            vehicles.each(function (vehicle, index, length) {
            
                var lat = vehicle.get('lat_float');
                var lon = vehicle.get('lon_float');
                
                // 
                // Construct the image name according to heading and selection status
                // 
                var imageName = 'NewTruck_';
                
                if ( vehicle.get('heading_deg') ) {
                    var heading = vehicle.get('heading_deg');
                    if ( heading < 0 ) {
                        imageName += 'E';
                    }
                    else if ( heading >= 355 || heading < 5 ) {
                        imageName += 'N';
                    }
                    else if ( heading >= 5 && heading < 65 ) {
                        imageName += 'NE';
                    }
                    else if ( heading >= 65 && heading < 115 ) {
                        imageName += 'E';
                    }
                    else if ( heading >= 115 && heading < 175 ) {
                        imageName += 'SE';
                    }
                    else if ( heading >= 175 && heading < 185 ) {
                        imageName += 'S';
                    }
                    else if ( heading >= 185 && heading < 245 ) {
                        imageName += 'SW';
                    }
                    else if ( heading >= 245 && heading < 295 ) {
                        imageName += 'W';
                    }else if ( heading >= 295 && heading < 355 ) {
                        imageName += 'NW';
                    }
                    else {
                        console.log('WARNING: Unexpected truck heading: ' + heading);
                    }
                }
                else {
                    imageName += 'E';
                }
                
                if ( vehicle === me.selectedVehicleRecord ) {
                    imageName += '_selected';
                }
                
                imageName += '.png';
                
                var appRoot = window.location.href.substr( 0, window.location.href.indexOf('?') );                      
                var image = {
                    url: appRoot + 'resources/icons/' + imageName,
                    size: new google.maps.Size(30, 30),
                    anchor: new google.maps.Point(15, 15)
                };
                
                lt = new google.maps.LatLng(lat,lon);
                var mark = new google.maps.Marker({
                    position: lt,
                    map:      null,
                    icon:     image
                });
                me.markers.push(mark);
                google.maps.event.addListener(mark, "click", Ext.bind(me.onVehicleMapClick, me, [mark,lt,vehicle,map], true));
            });
        }
    
        // Display the current vehicle markers
        
        if (me.showVehicles) {
            me.displayMarkers();
        }
    },
    
    /**
     * Handle clicks on vehicle markers on the map. Show the lastest observation for the vehicle.
     * @param {Object} e
     * @param {Object} mark
     * @param {Object} lt
     * @param {Object} rec
     * @param {Object} map
     */
    onVehicleMapClick: function(e, mark, lt, rec, map) {
        var me = this;
        
        // this.pushShowSite(rec, "map-nav-view");
        console.log('Clicked a truck: ' + rec.get('id'));
        
        if ( me.vehicleOverlay ) {
            me.vehicleOverlay.destroy();
            me.vehicleOverlay = null;
        }
        
        me.vehicleOverlay = me.getMapNavView().add({
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
    
            width: Ext.os.deviceType == 'Phone' ? 260 : 500,
            height: Ext.os.deviceType == 'Phone' ? 220 : 500,
            // itemTpl: me.createVehiclePopupTpl(rec),
            // data: rec,
            // html: 'text.',
            html: me.createVehiclePopupHtml(rec),
            items: [
                {
                    docked: 'top',
                    xtype: 'toolbar',
                    title: 'Vehicle: ' + rec.get('id')
                }
            ],
            scrollable: true
        });
        me.vehicleOverlay.show();
    },
    
    /**
     * Create the markup for the lastest vehicle observation.
     * @param {Object} record
     */
    createVehiclePopupHtml: function(record) {
        var html = '';
        if ( record.get('obs_time') ) {
            var obs_time = new Date(record.get('obs_time') * 1000);
            html += '<br>Observed at: ' + emdss.util.Util.dateToBriefString(obs_time);
        }
        if ( record.get('speed_mph') ) {
            var val = parseFloat(record.get('speed_mph'));
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Speed: Unknown';
            else html += '<br>Speed: ' + record.get('speed_mph') + ' mph';
        }
        if ( record.get('heading_deg') ) {
            var val = parseFloat(record.get('heading_deg'));
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Heading: Unknown';
            else html += '<br>Heading: ' + record.get('heading_deg') + ' degrees';
        }
        if ( record.get('temp_f') ) {
            var val = parseFloat(record.get('temp_f'));
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Air Temperature: Unknown';
            else html += '<br>Air Temperature: ' + record.get('temp_f') + ' F';
        }
        if ( record.get('humidity') ) {
            var val = parseFloat(record.get('humidity'));
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Humidity: Unknown';
            else html += '<br>Humidity: ' + record.get('humidity') + ' %';
        }
        if ( record.get('pressure') ) {
            var val = parseFloat(record.get('pressure'));
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Air Pressure: Unknown';
            else html += '<br>Air Pressure: ' + record.get('pressure') + ' mb';
        }
        if ( record.get('road_temp_f') ) {
            var val = parseFloat(record.get('road_temp_f'));
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Road Temperature: Unknown';
            else html += '<br>Road Temperature: ' + record.get('road_temp_f') + ' F';
        }
        if ( record.get('wind_speed_ms') ) {
            var val = parseFloat(record.get('wind_speed_ms'));
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Wind Speed: Unknown';
            else html += '<br>Wind Speed: ' + record.get('wind_speed_ms') + ' m/s';
        }
        // if ( record.get('wipers_on') ) {
        //     html += '<br>Wipers: ' + (record.get('wipers_on') ? 'On' : 'Off');
        // }
        if ( record.get('wipers_level') ) {
            html += '<br>Front Wiper Status: ' + (record.get('wipers_level'));
        }
        if ( record.get('abs_level') ) {
            html += '<br>Anti-Lock Brake Status: ' + (record.get('abs_level'));
        }
        if ( record.get('trac_level') ) {
            html += '<br>Traction Control State: ' + (record.get('trac_level'));
        }
        if ( record.get('lon') ) {
            var val = parseFloat(record.get('lon'));
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Longitude: Unknown';
            else html += '<br>Longitude: ' + record.get('lon');
        }
        if ( record.get('lat') ) {
            var val = parseFloat(record.get('lat'));
            if ( isNaN(val) || val == -9999.0 ) html += '<br>Latitude: Unknown';
            else html += '<br>Latitude: ' + record.get('lat');
        }
        
        if ( html.length < 1 ) {
            html = 'No Data Available...';
        }
        return html;
        
        // return new Ext.XTemplate(
        // '<div class="swabmopper">',
        // '  Swabmopper',
        // // '  {id}',
        // '</div>',
        // {
        // getThisDataLoadedTime: function(data_time) {
        // var dataTime = data_time;
        // return dataTime;
        // }
        // }
        // );
    },
    
    toggleShowVehicles: function() {
        var me = this;
    
        me.showVehiclesMethod(!me.showVehicles);
            
        if ( emdss.util.Util.supports_html5_storage() ) {
            window.localStorage.setItem('showVehicles', me.showVehicles);
        }
    },
        
    showVehiclesMethod: function(show) {
        var me = this;
        var button = me.getBtnVehicles();
        
        if (show) {
            me.showVehicles = true;
            me.displayMarkers();
            button.setHtml(emdss.util.Config.getHideVehiclesLabel());
        } else {
            me.showVehicles = false;
            me.clearMarkers();
            button.setHtml(emdss.util.Config.getShowVehiclesLabel());
        }
    }

});
