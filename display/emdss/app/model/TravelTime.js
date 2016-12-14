/**
 * Data model for the district holding vehicle data.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.TravelTime', {
    extend:'Ext.data.Model',
    requires:[
        'emdss.model.TravelTimeForecast'
    ],
    config:{

        fields:[
            {name:'route',type:'string'},
            {name:'from',type:'string'},
            {name:'to',type:'string'}
        ],
        associations : [
            {
                type :           'hasMany',
                associationKey:  'travelTimeForecasts', // the key to the array in the data...
                instanceName:    'travelTimeForecast', // the name used to access the array in the store record
                associatedModel: 'emdss.model.TravelTimeForecast',
                associatedName:  'travelTimeForecasts'  // the key to the array in the data...
            }
        ]
    }
});