/**
 * Data model for the time of the latest data available on the server
 * @author Paddy McCarthy
 */
Ext.define('mawDesk.model.DataTime', {
    extend:'Ext.data.Model',
    config:{
		fields:[
			{name:'dir',type:'string'},
			{name:'latest_time',type:'string'}
		]
    }
});
