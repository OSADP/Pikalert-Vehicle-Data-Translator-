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
	'emdss.util.Config'
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
			    zoom: emdss.util.Config.lookupDefaultZoom()
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
				xtype: 'button',
				id: 'hide-show-vehicles-button',
				hidden: false,
				html: emdss.util.Config.getHideVehiclesLabel()
			    }],
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
			    },
			]
		    }
	    	]
	    }
	    ]
	    
	    
    },
    
	initialize: function() {
		this.callParent(arguments);
   
		// Todo: Remove this attempt at waiting to initialize map markers.
		// // wait 100 ms
		// Ext.Function.defer(this.initMap, 100, this);
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
