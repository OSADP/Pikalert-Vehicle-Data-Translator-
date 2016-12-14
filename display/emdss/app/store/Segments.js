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

        var prefixTest = 'unk';
        var hrefStateTest = emdss.util.Config.getHrefState();
        if ( hrefStateTest == 'colorado' )    { prefixTest = 'co'; }
        else if ( hrefStateTest == 'michigan' )  { prefixTest = 'mi'; }
        else if ( hrefStateTest == 'minnesota' ) { prefixTest = 'mn'; }
        else if ( hrefStateTest == 'nevada' )    { prefixTest = 'nv'; }
        else if ( hrefStateTest == 'alaska' )    { prefixTest = 'ak'; }

        var appRootTest = window.location.href.substr( 0, window.location.href.indexOf('?') );
        if ( appRootTest.length == 0 ) {
            appRootTest = window.location.href;
        }
        var xhrTest = new XMLHttpRequest();
        var requestUrlTest = appRootTest
                             + '/resources/road_segments/'
                             + prefixTest + '_roads.json';
        xhrTest.open('GET', requestUrlTest);
        xhrTest.setRequestHeader("Cache-Control", "no-cache,max-age=0");
        xhrTest.setRequestHeader("Pragma", "no-cache");
        xhrTest.onload = function(e) {
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("After load of SEGMENTS TEST: ");
            console.log("   time:" + (new Date()));
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");

            var responseTest = JSON.parse(this.response);

            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("After parse of SEGMENTS TEST: ");
            console.log("   time:" + (new Date()));
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");
        };
        xhrTest.onload.applicationController = emdss.app.getController('emdss.controller.TravelTimeController');
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("Before load of SEGMENTS TEST: ");
            console.log("   time:" + (new Date()));
            console.log("");
            console.log("");
            console.log("");
            console.log("");
            console.log("");
        // xhrTest.send();

            });
        
        me.addListener("beforeload",
                    function(records, operation, success, container){
                console.log("beforeLoad listener in Segments store...");
                console.log("    URL: " + me.getProxy().getUrl());
                console.log("");
            });
    },
});
