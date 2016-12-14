/**
 * @author paddy
 */
Ext.define('emdss.view.Sites',{
    extend: 'Ext.dataview.List',
    xtype:  'sites',
    config: {
    title: 'Pikalert Sites',
    cls: 'x-sites',
    disableSelection: true,
    store: 'district-site-store',
    },
    
    initialize : function() {
        this.callParent(arguments);
        
        var me = this;
        me.setItemTpl( me.createItemTpl() );
    },
    
    /**
     * Create the itemTpl used to style the district/site list -- the list of 
     *   districts and sites within the State. Uses a
     *   css class to calculate the position of each alert icon within
     *   the alert image sprite. Note that the <img> tag references a
     *   transparent image -- that holds the space for the icon rendered
     *   by the div class reference.
     */
    createItemTpl: function() {
    return new Ext.XTemplate(
        '<div class="district_node">',
        '  <div class="district_name">{district_name}</div>',
        '  <div class="district_zoom">[zoom...]</div>',
        '</div>',
        '<div class="district_summary">',
        '  Obs: <img class="alert_{obs_alert_summary_code}" src="resources/icons/Alert_Transparent.png"></img>&nbsp;&nbsp;',
        '  Next 6 Hrs: <img class="alert_{hr06_alert_summary_code}" src="resources/icons/Alert_Transparent.png"></img>&nbsp;&nbsp;',
        '  6 - 24 Hrs: <img class="alert_{hr24_alert_summary_code}" src="resources/icons/Alert_Transparent.png"></img>&nbsp;&nbsp;',
        '  24 - 72 Hrs: <img class="alert_{hr72_alert_summary_code}" src="resources/icons/Alert_Transparent.png"></img>',
        '</div>',
        '<div class="data_time">',
        '  <tpl if="this.isPropertyPresent(data_time)">',
        '    <div>(All alerts generated at: {[this.getPrettyDateText(values.data_time)]})</div>',
        '  <tpl else>',
        '    <div>Can not determine alert generation time...</div>',
        '  </tpl>',
        '</div>',
        '<hr>',
        '<tpl for="sites">',
        '  <div class="site_node">',
        '    <div class="site_description">{desc}</div>',
        '    <div class="site_alert">',
        '      Obs: <img class="alert_{obs_alert_code}" src="resources/icons/Alert_Transparent.png"></img>&nbsp;&nbsp;',
        '      Next 6 Hrs: <img class="alert_{hr06_alert_code}" src="resources/icons/Alert_Transparent.png"></img>&nbsp;&nbsp;',
        '      6 - 24 Hrs: <img class="alert_{hr24_alert_code}" src="resources/icons/Alert_Transparent.png"></img>&nbsp;&nbsp;',
        '      24 - 72 Hrs: <img class="alert_{hr72_alert_code}" src="resources/icons/Alert_Transparent.png"></img>',
        '    </div>',
        '  </div>',
        '</tpl>',
        {
        getPrettyDateText: function(data_time) {
            var dataTime = emdss.util.Util.dateToBriefString(emdss.util.Util.emdssStringToDate('' + data_time));
            return dataTime;
        },
        isPropertyPresent: function(prop) {
            return (prop != undefined && prop != null);
        }
        }
    );
    }
});
