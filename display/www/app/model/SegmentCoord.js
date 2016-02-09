/**
 * Data model for the road segment coordinates.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.SegmentCoord', {
    extend:'Ext.data.Model',
    config:{

        fields:[
            {name: 'lon',                 type:'float'},
            {name: 'lat',                 type:'float'}
        ],
        belongsTo: {
            associatedModel: 'emdss.model.Segment',
            instanceName:    'segment' // the key to the Segment object on the store record.
        }
    }
});
