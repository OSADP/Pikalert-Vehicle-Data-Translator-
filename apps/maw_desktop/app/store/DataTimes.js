/**
 * Store for the latest data time (latest model update on the server).
 * @author Paddy McCarthy
 */
Ext.define('mawDesk.store.DataTimes',{
    extend:'Ext.data.Store',

    requires:['mawDesk.model.DataTime',
	      	  'mawDesk.util.Config'],
    
    config:{
		id: 'latest-data-times-store',
		model: 'mawDesk.model.DataTime',
		autoLoad: false,
		dir: 'not_initialized',
		date: mawDesk.util.Config.getDefaultDate(),
		proxy:{
		    type: 'ajax',
		    // url:   mawDesk.util.Config.getDataTimesUrl() 
		    // 		+ '&state=' + mawDesk.util.Config.getHrefState()
		    // 		+ ( mawDesk.util.Config.getUseVdt() ? '_vdt' : '' )
		    // url: mawDesk.util.Config.getTestDataTimesUrl()
		}
    },

    setDir: function(dir) {
	this.dir = dir;
    },

    setDate: function(date) {
	this.date = date;
    },

    loadData: function(container) {
		this.setProxy({
		    type:'ajax',
		    url: mawDesk.util.Config.getDataTimesUrl() + '&date_str=' +  this.date + "&alert_type=" + this.alertType
		});
		console.log(this.getProxy['url']);
		if (!container) {
		    container = Ext.Viewport;
		}
		container.setMasked({
		    xtype:'loadmask',
		    message:"Loading Data Times..."
		});
		var fn = Ext.bind(function(records, operation, success, container) {
		    container.setMasked(false);
		}, this, [container], true);
		this.load(fn);
    }
});
