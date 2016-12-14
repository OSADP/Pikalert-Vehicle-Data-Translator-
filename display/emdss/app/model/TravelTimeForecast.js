/**
 * Data model for the vehicle data. Note the large number of reference fields, which
 *   convert the string values provided by the server to number types that can be used
 *   with comparison operators. This is a good example of how to convert values as the 
 *   are loaded into a store.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.TravelTimeForecast', {
    extend:'Ext.data.Model',
    config:{
        fields:[
            // {name: 'departure',             type: 'string'},
            {name: "departure",             type: Ext.data.Types.DATE, dateFormat:'c'},
            {name: 'seconds',               type: 'float'}
            // {name: 'departure_time',        type: 'float',
            //                                     mapping: 'departure',
            //                                     convert: function(val, record) {
            //                                         return parseFloat(val);
            //                                     }},
        ],
        belongsTo: {
            associatedModel: 'emdss.model.TravelTime',
            instanceName:    'travelTimeForecasts' // the key to the object on the store record.
        }
    }
});
