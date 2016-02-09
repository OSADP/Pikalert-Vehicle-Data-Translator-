/**
 * Data model for the district holding site data.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.DistrictForSites', {
    extend:'Ext.data.Model',
    requires:[
		'emdss.model.Site'
    ],
    config:{
    	name: 'district',
		fields:[
		    {name:'min_lat',type:'float'},
		    {name:'min_lon',type:'float'},
		    {name:'max_lat',type:'float'},
		    {name:'max_lon',type:'float'},
		    {name:'district_name',type:'string'},
		    {name: 'obs_alert_summary_code', type: 'string'},
		    {name: 'hr06_alert_summary_code', type: 'string'},
		    {name: 'hr24_alert_summary_code', type: 'string'},
		    {name: 'hr72_alert_summary_code', type: 'string'},
		    {name: 'obs_alert_summary_image', type: 'string'},
		    {name: 'hr06_alert_summary_image', type: 'string'},
		    {name: 'hr24_alert_summary_image', type: 'string'},
		    {name: 'hr72_alert_summary_image', type: 'string'},
		    
		    // {name: 'data_time',                type: 'string',  	mapping:'district_name', 
		    											// convert: function(val, record) {
		    												// debugger;
		    												// return "Tester";
		    											// }},
		],
		associations : [
			{
				type :           'hasMany',
				associationKey:  'sites', // the key to the array in the data...
				instanceName:    'sites', // the name used to access the array in the store record
				associatedModel: 'emdss.model.Site',
				associatedName:  'sites'  // the key to the array in the data...
			}
		]
    }
});