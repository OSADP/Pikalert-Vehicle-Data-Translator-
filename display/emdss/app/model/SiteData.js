/**
 * Data model for the site detail data.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.SiteData', {
    extend:'Ext.data.Model',
    config:{
        fields:[
            {name:'treatment_explanations'},
            {name:'wx_obs',type:'auto'},
            {name:'rwis',type:'auto'},
            {name:'road_segments',type:'auto'},
            {name:'image',type:'string'},
            {name:'image_display_text',type:'string'}
        ],
        associations : [
            {
                type :           'hasMany',
                associationKey:  'time_series',
                instanceName:    'time_series',
                associatedModel: 'emdss.model.HourlyAlert',
                associatedName:  'time_series'
            }
        ],
    }
});
