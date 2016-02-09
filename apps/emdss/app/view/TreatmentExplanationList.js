
Ext.define("emdss.view.TreatmentExplanationList", {
    extend:'Ext.List',
    xtype:'treatment-explanations',
    config:{
	title:'Treatment Explanations',
	store:'explanation-store',
	itemTpl:[
	    '<div><p>{date_str} -- {explanation}</p></div>'
	].join('')
    }
});