/**
 * Data model for the road segments.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.Segment', {
    extend:'Ext.data.Model',
    requires:[
        'emdss.model.SegmentCoord'
    ],
    config:{

        fields:[
            {name: 'dicast_id',  type:'string'},
            {name: 'center_lat', type:'float'},
            {name: 'center_lon', type:'float'}
        ],
        associations : [
            {
                type :           'hasMany',
                associationKey:  'coordinates', // the key to the array in the data...
                instanceName:    'coords', // the name used to access the array in the store record
                associatedModel: 'emdss.model.SegmentCoord',
                associatedName:  'coords'  // the method name used to access the array from the parent object...
            }
        ]
    }
});
