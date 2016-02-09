Ext.define('mawDesk.view.Main', {
    extend: 'Ext.Container',
    xtype: 'main',
    requires: [
        'Ext.TitleBar',
        'Ext.Video',
        'Ext.Map',
        'Ext.navigation.View',
        'Ext.SegmentedButton',
        'Ext.field.Select'
    ],
    config: {
        // tabBarPosition: 'bottom',
        layout: 'fit',
        
        fullscreen: true,
	
        items: [
            {
		// title: 'Map',
		xtype: 'navigationview',
		// iconCls: 'maps',
		id: 'map-nav-view',
		autoDestroy: false,
		navigationBar: { hidden: true },
		masked: { xtype: 'loadmask', message: 'Loading...' },
		items:[{
		    // title: 'Map',
		    xtype: 'map',
		    id: 'map-view',
		    mapOptions: {
			draggable: true,
			streetViewControl: false,
			panControl: false,
			zoomControlOptions: {
			    position: google.maps.ControlPosition.LT,
			    style: google.maps.ZoomControlStyle.SMALL
			},
			center: mawDesk.util.Config.lookupDefaultCenter(),
			zoom: mawDesk.util.Config.lookupDefaultZoom()
		    },
		    // Todo:
		    // Todo: Can't get this listener to work. See http://stackoverflow.com/questions/6726693/sencha-touch-add-markers-to-map-after-maprender
		    // Todo:
		    listeners : {
			'maprender' : function(comp, map) {
			    // map is undefined, for some reason.
			    mawDesk.app.getController('mawDesk.controller.Application').onMapRendered(comp.getMap());
			    // mawDesk.dispatch({
			    // controller: mawDesk.controller.Application,
			    // action: 'onMapRendered',
			    // map: map
			    // });
			}
   		    }
		}]
    	    },{
		id: 'map-toolbar',
		xtype: 'toolbar',
		docked: 'top',
		ui: 'light',
		title: 'MAW Desktop',
		layout: {
		    type: 'hbox',
		    align: 'stretch',
		    pack: 'center'
		},
		items: [
		    // {
		    // id: 'overlay-select',
		    // xtype: 'segmentedbutton',
		    // hidden: true,
		    // // items:[
		    // // { text: mawDesk.util.Config.getRadarOverlayLabel() }
		    // // ]
		    // }, {
		    // id: 'toolbar-config',
		    // hidden: true,
		    // iconCls: 'settings',
		    // iconMask: true
		    // }, 
		    // {
		    // xtype: 'segmentedbutton',
		    // id: 'start-time',
		    // hidden: false,
		    // items:[
		    // {text: '0'},
		    // {text: '+2', pressed:true},
		    // {text: '+4'},
		    // {text: '+6'}
		    // ]
		    // }, {
		    // xtype: 'segmentedbutton',
		    // id: 'end-time',
		    // hidden: false,
		    // items:[
		    // {text: '1 hr'},
		    // {text: '2 hr'},
		    // {text: '6 hr'},
		    // {text: '24 hr', pressed:true}
		    // ]
		    // }, 
		    {
			xtype: 'container',
			docked: 'left',
			items: [{
			    xtype: 'selectfield',
			    id: 'start-select',
			    hidden: false,
			    options:[
				{text: 'Start', value: 'not_populated_yet'}
			    ]
			    // listeners: {
	                    // 	change: function (select, newValue, oldValue) {
	                    // 		console.log('start-select changed from: ' + oldValue + ' to: ' + newValue);
	                    // 	}
	                    // }
			}]
			
		    }, {
			xtype: 'container',
			docked: 'right',
			items: [{
			    xtype: 'selectfield',
			    id: 'end-select',
			    hidden: false,
			    options:[
				{text: 'End', value: 'not_populated_yet'}
			    ]
			    // listeners: {
	                    // 	change: function (select, newValue, oldValue) {
	                    // 		console.log('end-select changed from: ' + oldValue + ' to: ' + newValue);
	                    // 	}
	                    // }
	                }]
		    } 
		]
	    }
	]
    }
});
