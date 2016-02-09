/**
 * Store for the treatment explanation data. Not used.
 * @author Paddy McCarthy
 */
Ext.define('emdss.store.TreatmentExplanations',{
    extend:'Ext.data.Store',

    requires:['emdss.util.Config',
	      	  'emdss.model.TreatmentExplanation'],

    config:{
		id: 'explanation-store',
		model:'emdss.model.TreatmentExplanation',
		autoLoad: false,
		autoDestroy: true
    },
});