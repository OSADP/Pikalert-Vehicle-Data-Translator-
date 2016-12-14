Ext.define('emdss.view.Main', {
//    extend: 'Ext.tab.Panel',
    extend: 'Ext.TabPanel',
    xtype: 'main',
    id: 'main-tab-panel',
    iconCls: 'main-tab-panel',
    requires: [
        'emdss.view.Sites',
        'emdss.view.Vehicles',
        'emdss.view.site.Show',
        'emdss.util.Config',
        'emdss.util.MercatorUtils'
    ],
    config: {
        fullscreen : true,
        tabBarPosition: 'bottom',
        activeItem: 1, // Must make the map active or it isn't created by Ext.Map component...

    items: [
            {
                title: 'Site List',
                xtype: 'navigationview',
                navigationBar: { hidden: true },
                id: 'site-list',
                autoDestroy: false,
                iconCls: 'info',
                items: [
                    {
                        xtype: 'sites',
                        id: 'site-list-item',
                        title: 'fozzbot'
                    }
                ]
            },
            {
                title: 'Map',
                xtype: 'navigationview',
                navigationBar: { hidden: true },
                iconCls: 'maps',
                id: 'map-nav-view',
                autoDestroy: false,
                items:[
                    {
                        title: 'Pikalert Map',
                        xtype: 'map',
                        id: 'map-view',
                        mapOptions: {
                            draggable: true,
                            center: emdss.util.Config.lookupDefaultCenter(),
                            zoom: emdss.util.Config.lookupDefaultZoom(),
                            scaleControl: true,
                            mapTypeId: 'OSM',
                            mapTypeControlOptions: {
                                mapTypeIds: ['OSM', 
                                             google.maps.MapTypeId.ROADMAP, 
                                             // google.maps.MapTypeId.SATELLITE, 
                                             // google.maps.MapTypeId.HYBRID, 
                                             // google.maps.MapTypeId.TERRAIN
                                            ],
                                // style: google.maps.MapTypeControlStyle.DROPDOWN_MENU
                            }
                        },
                        // Todo:
                        // Todo: Can't get this listener to work. See http://stackoverflow.com/questions/6726693/sencha-touch-add-markers-to-map-after-maprender
                        // Todo:
                        listeners : {
                            'maprender' : function(comp, map) {
                            // map is undefined, for some reason.
                            emdss.app.getController('emdss.controller.Application').onMapRendered(comp.getMap());
                            // emdss.dispatch({
                            // controller: emdss.controller.Application,
                            // action: 'onMapRendered',
                            // map: map
                            // });
                            }
                        }
                    },
                ]
            },
            {
                title: 'Vehicle List',
                xtype: 'navigationview',
                navigationBar: { hidden: true },
                id: 'vehicle-list',
                autoDestroy: false,
                iconCls: 'info',
                items: [{
                    xtype: 'vehicles',
                    id: 'vehicle-list-item',
                    title: 'vvvvvozzbot'
                }]
            },
            {
                title: 'Travel Grid',
                xtype: 'navigationview',
                navigationBar: { hidden: true },
                id: 'travel-time-list',
                autoDestroy: false,
                iconCls: 'info',
                // hidden: !(emdss.util.Config.getHrefState() == 'colorado'),

                hidden: true,

                // layout: {
                //     type: 'hbox',
                //     align: 'stretch'
                // },
                // scrollable: false,
                // scrollable: {
                //     direction       : 'horizontal',
                //     directionLock   : true
                // },
                // inline:{
                //     wrap: false
                // },
                items: [
                    // {
                        // xtype       : 'container',
                        // id          : 'tthsp', // traveltimehorizontalscrollpanel
                        // layout: {
                        //     type  : 'vbox',
                        //     // align : 'stretch'
                        // },
                        // items: [
                            // {
                            //     html: 'header bar'
                            // },
                            {
                                xtype: 'traveltimegrid',
                                id: 'travel-time-list-item',
                                title: 'vvvvvozzbot',
                                scrollable: {
                                    direction       : 'both',
                                    directionLock   : false,
                                    indicators      : false
                                },
                                // inline: {
                                //     wrap: false
                                // },
                                // width : '1500px',
                                // bottom: 0,
                                // top: 0,
                                // flex: 1
                            }
                        // ]
                    // }
                ]
            },
            {
                title: 'Travel Times List',
                xtype: 'navigationview',
                navigationBar: { hidden: true },
                id: 'travel-time-list-list',
                autoDestroy: false,
                iconCls: 'info',
                hidden: !(emdss.util.Config.getHrefState() == 'colorado'),
                items: [{
                    xtype: 'traveltimelist',
                    id: 'travel-time-list-list-item',
                    title: 'vvvvvozzbut'
                }]
            },
            {
                xtype: 'toolbar',
                id: 'map-toolbar',
                docked: 'top',
                ui: 'light',
                title: 'Pikalert Map',
                layout: {
                    type: 'hbox',
                    align: 'stretch',
                    pack: 'justify',
                },
                items: [
                    {
                        xtype: 'panel',
                        layout: { type: 'hbox' },
                        items: [
                            {
                                xtype: 'panel',
                                id: 'departure-time-panel',
                                hidden: true,
                                layout: { type: 'vbox' },
                                items: [
                                    {
                                        xtype: 'selectfield',
                                        id: 'departure-time-select',
                                        width: '400px',
                                        label: 'Departure time:',
                                        options: []
                                    }
                                ]
                            },{
                                xtype: 'panel',
                                id: 'departure-time-list-panel',
                                hidden: true,
                                layout: { type: 'vbox' },
                                items: [
                                    {
                                        xtype: 'selectfield',
                                        id: 'departure-time-list-select',
                                        width: '400px',
                                        label: 'Departure time:',
                                        options: []
                                    }
                                ]
                            },
                            {
                                xtype: 'segmentedbutton',
                                id: 'fcst-type-select',
                                hidden: false,
                                items:[
                                    {text: emdss.util.Config.getObsFcstLabel()},
                                    {text: emdss.util.Config.getNext06HrsFcstLabel(), pressed:true},
                                    {text: emdss.util.Config.getNext24HrsFcstLabel()},
                                    {text: emdss.util.Config.getHrs24to72FcstLabel()}
                                ]
                            },
                            {
                                xtype: 'panel',
                                layout: { type: 'vbox' },
                                hidden: false,
                                style: 'margin-left: 8px',
                                items: [
                                    {
                                        xtype: 'panel',
                                        layout: { type: 'hbox' },
                                        hidden: false,
                                        items: [
                                            {
                                                xtype: 'button',
                                                id: 'hide-show-vehicles-button',
                                                ui: 'small',
                                                height: '25px',
                                                margin: '0px',
                                                hidden: false,
                                                html: emdss.util.Config.getHideVehiclesLabel()
                                            },
                                            {
                                                xtype: 'button',
                                                id: 'hide-show-radar-button',
                                                ui: 'small',
                                                height: '25px',
                                                margin: '0px',
                                                hidden: false,
                                                html: emdss.util.Config.getHideRadarLabel()
                                            }
                                        ]
                                    },
                                    {
                                        xtype: 'panel',
                                        layout: { type: 'hbox', pack: 'center' },
                                        hidden: false,
                                        items: [
                                            {
                                                xtype: 'button',
                                                id: 'hide-green-segments-button',
                                                ui: 'small',
                                                height: '25px',
                                                margin: '0px',
                                                hidden: false,
                                                html: emdss.util.Config.getHideGreenSegmentsLabel()
                                            }
                                        ]
                                    }
                                ]
                            }
                        ]
                    },
                    {
                        xtype: 'panel',
                        id: 'map-time-panel',
                        layout: { type: 'vbox' },
                        items: [
                            {
                                xtype: 'label',
                                id: 'map-curr-time-label',
                                hidden: false,
                                cls: 'map_times',
                                html: 'Current time: ',
                            },
                            {
                                xtype: 'label',
                                id: 'map-valid-time-label',
                                hidden: false,
                                cls: 'map_times',
                                html: 'Data valid: ',
                            }
                        ]
                    }
                ]
            }
        ]
    },
    
    initialize: function() {
        this.callParent(arguments);
    
        var me = this;
        me.mawClicksActive = false;
        var appCtrl = emdss.app.getController('emdss.controller.Application');
        appCtrl.mawClicksActive = false;
    
        var mapView = Ext.getCmp("map-view");
        
        var panel = Ext.create('Ext.Panel', {
                    id : 'maw-vpanel',
                    style : 'background-color: transparent; position: absolute',
                    layout: {
                        type: 'vbox',
                        align: 'left',
                        pack: 'center',
                    },
                    align: 'center',
                    left: '0px',
                    bottom: '0px',
                    items: [{
                        xtype : 'panel',
                        id : 'maw-hpanel',
                        style : 'background-color: transparent;',
                        layout: {
                            type: 'hbox',
                            align: 'end',
                            pack: 'start',
                        },
                        items: [{
                            xtype : 'togglefield',
                            id : 'maw-checkbox',
                            label: '<div style="width:100%;white-space:normal;font-family:Arial;font-size:10px;">Enable MAW Clicks</div>',
                            labelWidth: '40%',
                            value: 0,
                            listeners: {
                            change: function(field, newValue, oldValue) {
                                console.log("Change! newValue: " + newValue + " oldValue: " + oldValue);
                                    if ( newValue == 1 ) {
                                        appCtrl.mawClicksActive = true;
                                    }
                                    else {
                                        appCtrl.mawClicksActive = false;
                                    }
                            }
                        }
                        }],
                        // items: [{
                        //     xtype : 'checkboxfield',
                        //     id : 'maw-checkbox',
                        //     label: '<div style="width:100%;white-space:normal;font-family:Arial;font-size:10px;">Enable MAW Clicks</div>',
                        //     checked: false,
                        //     listeners: {
                    //         check: function(checkbox, event, opts) {
                    //             console.log("Check");
                        //             appCtrl.mawClicksActive = true;
                    //         }, 
                    //         uncheck: function(checkbox, event, opts) {
                    //             console.log("Uncheck");
                        //             appCtrl.mawClicksActive = false;
                    //         }
                    //     }
                        // }],
                    }],
                });
            panel.updateBaseCls(null,'x-floating'); // necessary to remove annoying dropshadow

            // The animation button
            var buttHt = '28px';
            var smButtHt = '22px';
            var loadUnloadAnimationButton = Ext.create('Ext.Button', {
                id : 'load-unload-animation-button',
                // cls: 'my-openlayers-button',
                iconCls: 'animation-play-icon',
                ui : 'small',
                height: buttHt,
                style : 'margin-right: 12px;',
                disabled: true
            });
            // The radar time label
            var infoLabel = Ext.create('Ext.Label', {
                    id : 'radar-info-label',
                    ui : 'small',
                    cls : 'radar-time-label',
                    // height: buttHt,
                    // margin : 2,
                    html: ' '
                });
            var radarInfoPanel = Ext.create('Ext.Panel', {
                id: 'radar-info-panel',
                layout : {
                    type : 'hbox',
                    align : 'middle', // vertical alignment
                    pack : 'start'    // left pack
                },
                
                // right: '0px',
                // top : '0px',
                // width : 300,
                // align : 'right',
                // style : 'background-color: rgba(255,255,255,0.6); border-bottom-left-radius: 10px; padding: 4px; position: absolute;',
                
                cls : 'transparentContainer',
                items : [loadUnloadAnimationButton, infoLabel]
            });
            var playPauseButton = Ext.create('Ext.Button', {
                id : 'play-pause-button',
                cls: 'my-openlayers-button',
                iconCls: 'animation-pause-icon',
                ui : 'small',
                height: smButtHt,
                margin : 2,
                disabled: false
            });
            var stepForwardButton = Ext.create('Ext.Button', {
                id : 'step-forward-button',
                cls: 'my-openlayers-button',
                iconCls: 'animation-step-forward-icon',
                ui : 'small',
                height: smButtHt,
                margin : 2,
                disabled: false
            });
            var stepBackwardButton = Ext.create('Ext.Button', {
                id : 'step-backward-button',
                cls: 'my-openlayers-button',
                iconCls: 'animation-step-backward-icon',
                ui : 'small',
                height: smButtHt,
                margin : 2,
                disabled: false
            });
            var radarControlsPanel = Ext.create('Ext.Panel', {
                id: 'radar-controls-panel',
                layout : {
                    type : 'hbox',
                    align : 'middle', // vertical alignment
                    pack : 'start'    // left pack
                },
                
                // right: '0px',
                // top : '0px',
                // width : 300,
                // align : 'right',
                // style : 'background-color: rgba(255,255,255,0.6); border-bottom-left-radius: 10px; padding: 4px; position: absolute;',
                
                hidden: true,
                
                cls : 'transparentContainer',
                items : [stepBackwardButton, playPauseButton, stepForwardButton],
                
                showAnimation: {
                    type: 'slideIn',
                    duration: 500,
                    direction: 'left'
                }, 
                
                hideAnimation: {
                    type: 'slideOut',
                    duration: 500,
                    direction: 'right'
                },
            });
            var radarPanel = Ext.create('Ext.Panel', {
                id: 'radar-panel',
                layout : {
                    type : 'vbox',
                    align : 'center', // horizontal alignment
                    pack : 'start'    // top pack
                },
                // floating : true, // deprecated, but necessary :(
                right: '0px',
                top : '0px',
                width : 270,
                align : 'right',
                // these are required for transparency and positioning
                // style : 'background-color: transparent;',
                style : 'background-color: rgba(255,255,255,0.6); border-bottom-left-radius: 10px; padding: 4px; position: absolute;',
                cls : 'transparentContainer',
                items : [radarInfoPanel, radarControlsPanel]
            });
            radarPanel.updateBaseCls(null,'x-floating'); // necessary to remove annoying dropshadow
            this.add(radarPanel);
            
            var radarLoadingPanel = Ext.create('Ext.Panel', {
                id: 'radar-loading-panel',
                layout : {
                    type : 'hbox'
                },
                // floating : true, // deprecated, but necessary :(
                right: '20px',
                top : '34px',
                width : 60,
                height : 30,
                align : 'right',
                // these are required for transparency and positioning
                // style : 'background-color: transparent;',
                style : 'padding: 4px; position: absolute;',
                cls : 'transparentContainer',
                
                hidden: true,
                
                items : [
                    {
                        xtype: 'label',
                        cls: 'animation-loading-label',
                        html: 'Loading...'
                    }
                    // {
                        // xtype: 'image',
                        // // src: '../resources/images/LoadingIcon.png',
                        // cls: 'animation-loading-icon',
                        // flex: 1
                    // }
                ],
                
                showAnimation: {
                    type: 'slideIn',
                    duration: 500,
                    direction: 'left'
                }, 
                
                hideAnimation: {
                    type: 'slideOut',
                    duration: 500,
                    direction: 'right'
                },
            });
            radarLoadingPanel.updateBaseCls(null,'x-floating'); // necessary to remove annoying dropshadow
            this.add(radarLoadingPanel);
            
            // radarControlsPanel.updateBaseCls(null,'x-floating'); // necessary to remove annoying dropshadow
            // this.add(radarControlsPanel);
        
            Ext.Viewport.setMasked({
                xtype: 'loadmask',
                indicator: true
                //message: 'A message...' //Defaults to Loading...
            });

        // var panel2 = Ext.create('Ext.Panel', {
        //             id : 'loading-panel',
        //             // style : 'background-color: transparent; position: absolute',
            //             masked: { xtype: 'loadmask', indicator: true, message: 'loading' },
        //             layout: {
        //                 type: 'fit',
        //                 // align: 'left',
        //                 // pack: 'center',
        //             },
        //             // align: 'center',
        //             // right: '0px',
        //             top: '200px'
        //         });
            // panel2.updateBaseCls(null,'x-floating'); // necessary to remove annoying dropshadow
            // mapView.add(panel2);
            
            // var checkbox = Ext.create('Ext.field.Checkbox', {
            //                 id : 'maw-checkbox',
            //                 label : 'MAW Alerts',
            //                 checked: false,
            //                 docked: 'bottom',
            //                 listeners: {
        //                     check: function(checkbox, event, opts) {
        //                         console.log("Check Check");
        //                     }, 
        //                     uncheck: function(checkbox, event, opts) {
        //                         console.log("Uncheck Uncheck");
        //                     }
        //                 }
        //             });
        if ( emdss.util.Config.getHrefState() == 'minnesota' ) {
            mapView.add(panel);
        }
        
        // Set up Google map with WMS overlay.
        //  - based on example at: view-source:http://www.sumbera.com/lab/GoogleV3/tiledWMSoverlayGoogleV3.htm
        var map = mapView.getMap();

        var osmMapType = new google.maps.ImageMapType({
                    getTileUrl: function (coord, zoom) {
                        return "http://tile.openstreetmap.org/" +
                    zoom + "/" + coord.x + "/" + coord.y + ".png";
                    },
                    tileSize: new google.maps.Size(256, 256),
                    isPng: true,
                    alt: "OpenStreetMap",
                    name: "OSM",
                    maxZoom: 19
                });
        map.mapTypes.set('OSM', osmMapType);
        map.setMapTypeId(google.maps.MapTypeId.ROADMAP);

        // Change scale to imperial units.
        var scaleInterval = setInterval( function() {
          var spn = document.getElementById('map-view').getElementsByTagName('span');
          var pattern = /\d+\s+(m|km)/i;
          for(var i in spn) {
            if ( pattern.test(spn[i].innerHTML) ) {
              spn[i].click();
              clearInterval(scaleInterval);
            }
          }
        }, 500);
        setTimeout( function() { clearInterval(scaleInterval); }, 20000 );
    },
    
    initMap: function() {
        var mapPanel = this.down('map');
        var gMap = mapPanel.getMap();

        if (gMap == null) {
            console.log("WAITING for valid Map.");
            Ext.Function.defer(this.initMap,250,this);
        } else {
            console.log("GOT valid Map.");
        }
    }
});
