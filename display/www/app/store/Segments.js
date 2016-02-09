/**
 * Store for the time of the latest model run available from the server.
 * @author Paddy McCarthy
 */
Ext.define('emdss.store.Segments',{
    extend:'Ext.data.Store',

    requires:  ['emdss.model.Segment',
        'emdss.util.Config'],
    
    config:{
        id: 'segments-store',
        model: 'emdss.model.Segment',
        autoLoad: false,
        proxy:{
            type: 'ajax',
        }
    },
    
    //
    // Used for debugging store loads.
    // 
    setupAfterListener: function() {
        var me = this;
        
        console.log("In Segments::setupAfterListener...");
        
        me.addAfterListener("load",
                    function(records, operation, success, container){
                console.log("AfterListener for load() in Segments store...");
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
                console.log("beforeLoad listener in Segments store...");
                console.log("    URL: " + me.getProxy().getUrl());
                console.log("");
            });
    },
});
