/**
 * Data model for the vehicle data. Note the large number of reference fields, which
 *   convert the string values provided by the server to number types that can be used
 *   with comparison operators. This is a good example of how to convert values as the 
 *   are loaded into a store.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.Vehicle', {
    extend:'Ext.data.Model',
    config:{
		fields:[
		    {name: 'id',                type: 'string'},
		    {name: 'lat',               type: 'string'},
		    {name: 'lon',               type: 'string'},
		    {name: 'obs_time',          type: 'string'},
		    {name: 'speed_mph',         type: 'string'},
		    {name: 'heading_deg',       type: 'string'},
		    {name: 'wipers',            type: 'string'},
		    {name: 'temp_f',            type: 'string'},
		    {name: 'road_temp_f',       type: 'string'},
		    {name: 'wind_speed_ms',     type: 'string'},
		    
		    {name: 'lat_float',   			type: 'float',
											mapping: 'lat',
											convert: function(val, record) {
												return parseFloat(val);
											}},
			{name: 'lon_float',   			type: 'float',
											mapping: 'lon',
											convert: function(val, record) {
												return parseFloat(val);
											}},
		    {name: 'speed_mph_float',   	type: 'float',
											mapping: 'speed_mph',
											convert: function(val, record) {
												return parseFloat(val);
											}},
		    {name: 'heading_deg_float',   	type: 'float',
											mapping: 'heading_deg',
											convert: function(val, record) {
												return parseFloat(val);
											}},
		    {name: 'wipers_on',         	type: 'boolean',  	
											mapping:'wipers', 
											convert: function(val, record) {
												var retval = null;
												if (val === 'on') {
													retval = true;
												}
												else if (val === 'off') {
													retval = false;
												}
												return retval;
											}},
			{name: 'temp_f_float',   		type: 'float',
											mapping: 'temp_f',
											convert: function(val, record) {
			    								return parseFloat(val);
											}},
		    {name: 'road_temp_f_float',   	type: 'float',
											mapping: 'road_temp_f',
											convert: function(val, record) {
												return parseFloat(val);
											}},
		    {name: 'wind_speed_ms_float',   type: 'float',
											mapping: 'wind_speed_ms',
											convert: function(val, record) {
												return parseFloat(val);
											}},
			// {name: 'hr06_alert_image',   type: 'string',  	
		    							// mapping:'hr06_alert_code', 
										// convert: function(val, record) {
											// // Todo: Put code here to look up image from sprite...
											// return val;
										// }},
			// {name: 'hr24_alert_image',   type: 'string',  	
		    							// mapping:'hr24_alert_code', 
										// convert: function(val, record) {
											// // Todo: Put code here to look up image from sprite...
											// return val;
										// }},
			// {name: 'hr72_alert_image',   type: 'string',  	
		    							// mapping:'hr72_alert_code', 
										// convert: function(val, record) {
											// // Todo: Put code here to look up image from sprite...
											// return val;
										// }}
		],
		belongsTo: {
			associatedModel: 'emdss.model.DistrictForVehicles',
			instanceName:    'district' // the key to the object on the store record.
		}
    }
});