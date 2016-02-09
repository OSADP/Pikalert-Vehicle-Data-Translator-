/**
 * Data model for the latest model data available from the server.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.DataTime', {
    extend:'Ext.data.Model',
    config:{
		fields:[
			{name:'dir',type:'string'},
			{name:'latest_time',type:'string'}
		]
    }
});
