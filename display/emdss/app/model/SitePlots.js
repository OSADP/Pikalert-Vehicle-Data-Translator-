/**
 * Data model for the site detail summary graph.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.SitePlots', {
    extend:'Ext.data.Model',
    config:{
        fields:[
            {name:'summary_plot',type:'string'},
            {name:'image',type:'string'}
        ]
    }
});
