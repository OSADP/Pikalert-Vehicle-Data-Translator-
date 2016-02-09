/**
 * Data model for the hourly alerts -- the time series of alerts for each site. These
 *   differ from the alert values on the sites themselves, which are summary alerts
 *   over a time period.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.WatchWarnCoord', {
    extend:'Ext.data.Model',
    config:{

        fields:[
            {name: 'lon',                 type:'float'},
            {name: 'lat',                 type:'float'}
        ],
        belongsTo: {
            associatedModel: 'emdss.model.WxWatchWarns',
            instanceName:    'watch_warn' // the key to the WxWatchWarn object on the store record.
        }
    }
});