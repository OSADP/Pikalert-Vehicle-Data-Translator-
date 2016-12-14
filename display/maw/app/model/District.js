/**
 * Data model for the set of districts and their associated sites/alerts.
 * @author Paddy McCarthy
 */
Ext.define('mawDesk.model.District', {
    extend:'Ext.data.Model',
    requires:[
		'mawDesk.model.DistrictSite'
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
		],
		associations : [
			{
				type :           'hasMany',
				associationKey:  'sites', // the key to the array in the data...
				instanceName:    'sites', // the name used to access the array in the store record
				associatedModel: 'mawDesk.model.DistrictSite',
				associatedName:  'sites'  // the key to the array in the data...
			}
		]
    }
});