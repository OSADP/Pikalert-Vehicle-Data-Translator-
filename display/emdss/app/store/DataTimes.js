/**
 * Store for the time of the latest model run available from the server.
 * @author Paddy McCarthy
 */
Ext.define('emdss.store.DataTimes',{
    extend:'Ext.data.Store',

    requires:  ['emdss.model.DataTime',
                'emdss.util.Config'],
    
    config:{
        id: 'latest-data-times-store',
        model: 'emdss.model.DataTime',
        autoLoad: false,
        dir: 'not_initialized',
        date: emdss.util.Config.getDefaultDate(),
        proxy:{
            type: 'ajax',
            // url:   emdss.util.Config.getDataTimesUrl() + '&state=' + emdss.util.Config.getDisplayState()
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
            url: emdss.util.Config.getDataTimesUrl() + '&date_str=' +  this.date + "&alert_type=" + this.alertType
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
