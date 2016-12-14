/**
 * Data model for the district holding vehicle data.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.TravelTimeSegment', {
    extend:'Ext.data.Model',
    requires:[
        
    ],
    config:{

        fields:[
            {name:'route',          type:'string'},
            {name:'from',           type:'string'},
            {name:'to',             type:'string'},
            {name:'name',           type:'string'},
            {name:'Pena_Blvd',      type:'float'},
            {name:'I-225',          type:'float'},
            {name:'I-270',          type:'float'},
            {name:'I-25',           type:'float'},
            {name:'I-76_Wadsworth', type:'float'},
            {name:'Ward_Road',      type:'float'},
            {name:'DEN_C-470',      type:'float'},
            {name:'Idaho_Springs',  type:'float'},
            {name:'US_40_Empire',   type:'float'},
            {name:'Georgetown',     type:'flaot'},
            {name:'Tunnel',         type:'float'},
            {name:'Silverthorne',   type:'float'},
            {name:'Vail',           type:'float'},
            
            {name:'Pena_Blvd_tt',      type:'string',
                mapping: 'Pena_Blvd',
                convert: function(val, record) {
                    if ( val == -1 ) {
                        return '';
                    }
                    else {
                        var total_mins = Math.round(val);
                        var hrs = Math.floor( total_mins / 60.0 );
                        var mins = total_mins % 60;
                        
                        if ( hrs == 0 ) {
                            return '' + mins + 'min';
                        }
                        else {
                            return '' + hrs + 'hr ' + mins + 'min';
                        }
                    }
                }
            },
            // {name: 'departure_time',        type: 'float',
            //                                     mapping: 'departure',
            //                                     convert: function(val, record) {
            //                                         return parseFloat(val);
            //                                     }},
        ]
    }
});