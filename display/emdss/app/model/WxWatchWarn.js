/**
 * Data model for the hourly alerts -- the time series of alerts for each site. These
 *   differ from the alert values on the sites themselves, which are summary alerts
 *   over a time period.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.WxWatchWarn', {
    extend:'Ext.data.Model',
    requires:[
        'emdss.model.WatchWarnCoord'
    ],
    config:{

        fields:[
            {name: 'Begin Time',                 type:'string'},
            {name: 'End Time',                   type:'string'},
            {name: 'Weather Watch/Warning Text', type:'string'},
            {name: 'Type',                       type:'string'}
        ],
        associations : [
            {
                type :           'hasMany',
                associationKey:  'coordinates', // the key to the array in the data...
                instanceName:    'coords', // the name used to access the array in the store record
                associatedModel: 'emdss.model.WatchWarnCoord',
                associatedName:  'coords'  // the method name used to access the array from the parent object...
            }
        ]
    }
});