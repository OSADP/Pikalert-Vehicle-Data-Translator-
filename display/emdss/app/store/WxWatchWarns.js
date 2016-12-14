/**
 * Store for the time of the latest model run available from the server.
 * @author Paddy McCarthy
 */
Ext.define('emdss.store.WxWatchWarns',{
    extend:'Ext.data.Store',

    requires:  ['emdss.model.WxWatchWarn',
                'emdss.util.Config'],
    
    config:{
        id: 'watch-warns-store',
        model: 'emdss.model.WxWatchWarn',
        autoLoad: false,
        dir: 'not_initialized',
        date: emdss.util.Config.getDefaultDate(),
        proxy:{
            type: 'ajax',
        }
    },
    
    //
    // Used for debugging store loads.
    // 
    setupAfterListener: function() {
        var me = this;
        
        console.log("In WxWatchWarns::setupAfterListener...");
        
        me.addAfterListener("load",
                              function(records, operation, success, container){
                                console.log("AfterListener for load() in WxWatchWarns store...");
                                console.log("Records:");
                                console.log(records);
                                console.log("Operation:");
                                console.log(operation);
                                console.log("Success:");
                                console.log(success);
                                console.log("Container:");
                                console.log(container);
                             });
        
        me.addListener("beforeload",
                         function(records, operation, success, container){
                            console.log("beforeLoad listener in WxWatchWarns store...");
                        });
    },
});
