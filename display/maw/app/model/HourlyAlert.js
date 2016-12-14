/**
 * Data model for the time series of alerts for each site 
 *   (alerts on the site store are summary alerts for different time periods).
 * @author Paddy McCarthy
 */
Ext.define('mawDesk.model.HourlyAlert', {
    extend:'Ext.data.Model',
    config:{
		fields:[
		    {name: 'time',           type:'string'},
		    {name: 'alert_code',     type:'string'},
		    {name: 'precip',         type:'string'},
		    {name: 'pavement',       type:'string'},
		    {name: 'visibility',     type:'string'},
		    {name: 'plow',           type:'string'},
		    {name: 'chemical',       type:'string'},
		    {name: 'treatment_alert_code',         type:'string'},
		    {name: 'road_temp',       type:'string'}
		],
		belongsTo: {
			associatedModel: 'mawDesk.model.DistrictSite',
			instanceName:    'site' // the key to the object on the store record.
		}
    }
});