/**
 * Data model for the hourly alerts -- the time series of alerts for each site. These
 *   differ from the alert values on the sites themselves, which are summary alerts
 *   over a time period.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.HourlyAlert', {
    extend:'Ext.data.Model',
    config:{
        fields:[
            {name: 'time',                 type:'string'},
            {name: 'alert_code',           type:'string'},
            {name: 'precip',               type:'string'},
            {name: 'pavement',             type:'string'},
            {name: 'visibility',           type:'string'},
            {name: 'plow',                 type:'string'},
            {name: 'chemical',             type:'string'},
            {name: 'treatment_alert_code', type:'string'},
            {name: 'road_temp',            type:'string'}
        ],
        belongsTo: {
            associatedModel: 'emdss.model.Site',
            instanceName:    'site' // the key to the object on the store record.
        }
    }
});