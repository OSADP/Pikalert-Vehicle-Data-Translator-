/**
 * Store for the data associated with the site detail view -- summary plot, etc.
 * @author Paddy McCarthy
 */
Ext.define('emdss.store.SitePlots', {
    extend: 'Ext.data.Store',
    requires: ['emdss.util.Config'],
    config: {
        model: 'emdss.model.SitePlots',
        id: 'site-plots-store',
        autoLoad: false,
        date: emdss.util.Config.getDefaultDate(),
        proxy: {
            type:'ajax',
            url: emdss.util.Config.getSitePlotsUrl()
        }
    },

    setSite: function(site) {
        this.site = site;
    },

    setDate: function(date) {
        this.date = date;
    },

    loadData: function(container) {
        this.setProxy({
            type:'ajax',
            url: emdss.util.Config.getSitePlotsUrl() 
                + '&date_str=' +  this.date 
                + '&site=' + this.site 
                + '&state=' + emdss.util.Config.getHrefState()
                + ( emdss.util.Config.getUseVdt() ? '_vdt' : '' )
        });
        if (!container) {
            container = Ext.Viewport;
        }
        container.down('#summary_plots').setHtml(' '); // Clears 'No Summary' message, if there was one...
        container.setMasked({
            xtype:'loadmask',
            message:"Loading Plots..."
        });
        var fn = Ext.bind(function(records, operation, success, container) {
            container.setMasked(false);
        }, this, [container], true);
        this.load(fn);
    }
});
