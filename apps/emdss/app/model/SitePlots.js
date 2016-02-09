/**
 * Data model for the site detail data (summary graph with associated data).
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.SitePlots', {
    extend:'Ext.data.Model',
    config:{
		fields:[
			{name:'summary_plot',type:'string'},
			{name:'treatment_plot',type:'string'},
			{name:'treatment_explanations'},
			{name:'wx_obs',type:'auto'},
			{name:'rwis',type:'auto'},
			{name:'road_segments',type:'auto'}
		]
    }
});
