/**
 * Data model for the treatment explaination.
 * @author Paddy McCarthy
 */
Ext.define("emdss.model.TreatmentExplanation", {
    extend:'Ext.data.Model',
    config:{
	fields:[
	    {name:'date_str',type:'string'},
	    {name:'explanation',type:'string'}
	]
    }
});