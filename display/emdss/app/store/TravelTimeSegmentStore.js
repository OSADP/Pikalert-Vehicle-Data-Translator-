/**
 * Store for the Districts holding vehicle data.
 * @author Paddy McCarthy
 */
Ext.define('emdss.store.TravelTimeSegmentStore',{
    extend:'Ext.data.Store',

    requires:['emdss.model.TravelTimeSegment',
              'Ext.data.proxy.JsonP'],
    
    config: {
        storeId:    'dummy',
        model:      'emdss.model.TravelTimeSegment',
        autoLoad:   false,
        reloadTask: null,
        stopReload: false,
        proxy:{
            type:             'ajax',
            url:              'JUNK',
            reader: {
                    type:         'json',
                    rootProperty: 'JUNK'
                }
        },
        listeners: {
            'load': function (message) {
                console.log('... In TravelTimeSegmentStore load() listener that was defined in config{}...' + message);
            }
        }
    },
    
    //
    // Used for debugging store loads.
    // 
    setupAfterListener: function() {
        var me = this;
        
        console.log("In TravelTimeSegmentStore::setupAfterListener...");
        
        me.addAfterListener("load",
            function(records, operation, success, container){
 
                console.log("AfterListener for load() in TravelTimeSegment store...");
                console.log("Records:");
                console.log(records);
                console.log("Operation:");
                console.log(operation);
                console.log("Success:");
                console.log(success);
                console.log("Container:");
                console.log(container);
                
                // 
                // Recalc the table of the aggregated data.
                // 
                me.recalcAggregateTable(Ext.getStore('travel-time-store'));
            });
    
        me.addListener("beforeload",
           function(records, operation, success, container){
               console.log("beforeLoad listener in TravelTimeSegmentStore removing all previous records...");
               me.removeAll(true);
           });
    },
    
    recalcAggregateTable: function(ttStore) {
        var me = this;
        var wbSegments = Ext.getStore('wb-travel-time-segment-store');
        var ebSegments = Ext.getStore('eb-travel-time-segment-store');
        if ( wbSegments.getCount() == 0 || ttStore.getCount() == 0) {
            return;
        }
        
        if ( typeof Ext.getStore('wb-travel-time-segment-store').selectedDepartureTime == 'undefined' ||
             Ext.getStore('wb-travel-time-segment-store').selectedDepartureTime == null) {
            return;
        }
        
        // Create an array to hold the "from" names
        var names = [];
        
        // Create an array to hold the values...
        var arr = [];
        for (var i = 0; i < (wbSegments.getCount() + 1); i++) {
            arr[i] = [];
        }

        // Walk through once and fill in all the 'corner' cells with information
        //   delivered from the travel times data service (stored in travel time store)
        // 
        // those would be the 'o's in this diagram.
        //   - X's are all -1
        //   - dots are empty at this point
        // 
        //   0                        (this is the HEADER wb segment)
        //   1  Xo...........
        //   2  oXo..........
        //   3  .oXo.........
        //   4  ..oXo........
        //   5  ...oXo.......
        //   6  ....oXo......
        //   7  .....oXo.....
        //   8  ......oXo....
        //   9  .......oXo...
        //   10 ........oXo..
        //   11 .........oXo.
        //   12 ..........oXo
        //   13 ...........oX         (this the the PLACEHOLDER wb segment)
        var maxIdx = -1;
        wbSegments.each(function (thisSeg, index, length) {
            var toIdx = index + 1;
            var fromIdx = index - 1;
            maxIdx = toIdx;
            var thisWbSegFrom = thisSeg.get('from');
            var thisWbSegTo = thisSeg.get('to');
            var thisWbSegRoute = thisSeg.get('route');
            
            if (thisWbSegFrom == 'HEADER') {
                return true; // continue iterating...
            }
            
            // 
            // Find and process the westbound (WB) segment
            // 
            if ( thisWbSegRoute == 'PLACEHOLDER' ) {
                // The last segment needs it's self-destination initialized.
                thisSeg.beginEdit();
                thisSeg.set(thisWbSegFrom, -1);
                thisSeg.endEdit(true, [thisWbSegFrom]);
            }
            else {
                // Other segments need to have their 'corner' values set -- the travel time to the next adjacent waypoint.
                
                // Find the segment in the travel time store (updated travel time forecast data)
                //   and ensure there is exactly one response.
                var ttSegList = ttStore.queryBy(function(record, id) {
                    if ( record.get('route') == thisWbSegRoute) {
                        return true;
                    }
                    return false;
                });
                if ( ttSegList.getCount() != 1 ) {
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR: Found duplicate or missing WB route in the travel time data. Route: " + thisWbSegRoute);
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    debugger;
                    return;
                }
                
                var wbForecastsStore = ttSegList.first().traveltimeforecasts();
                var selectedTime = Ext.getStore('wb-travel-time-segment-store').selectedDepartureTime;
                if ( !selectedTime ) {
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR: No selected time in recalcAggregateTable when looking for FROM segment in travelTimeStore.");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    debugger;
                    return;
                }
                
                var fromSegList = wbForecastsStore.queryBy(function(record, id) {
                    if ( record.get('departure').getTime() == selectedTime.getTime()) {
                        return true;
                    }
                    return false;
                });
                if ( fromSegList.getCount() != 1 ) {
                    console.log("ERROR: Could not find selected departure time in FROM segment: " + selectedTime);
                    debugger;
                    return;
                }
                var ttSegValueWB = fromSegList.first().get('seconds');
                
                names[index] = thisWbSegFrom;
                names[toIdx] = thisWbSegTo;
                thisSeg.beginEdit();
                thisSeg.set(thisWbSegFrom, -1);
                thisSeg.set(thisWbSegTo, ttSegValueWB);
                thisSeg.endEdit(true, [thisWbSegFrom, thisWbSegTo]);
                arr[index][index] = -1;
                arr[index][toIdx] = ttSegValueWB;
                arr[toIdx][toIdx] = -1;
            }
            
            // 
            // Find and process the eastbound (EB) segment
            // 
            if (index > 1) { // (first row is HEADER, and second row has no eastbound segment - from Peña to Peña)
                var ebSegsList = ebSegments.queryBy(function(record, id) {
                    if ( record.get('from') == thisWbSegFrom && record.get('to') == names[index-1]) {
                        return true;
                    }
                    return false;
                });
                if ( ebSegsList.getCount() != 1 ) {
                    console.log("ERROR: Could not find equivalent EB route for WB route: " + thisWbSegRoute + " (from: " + thisWbSegFrom + " to: " + thisWbSegTo + ")");
                    debugger;
                    return;
                }
                
                var thisEbSeg = ebSegsList.first();
                var thisEbSegFrom = thisEbSeg.get('from');
                var thisEbSegTo = thisEbSeg.get('to');
                var thisEbSegRoute = thisEbSeg.get('route');
                
                ttSegList = ttStore.queryBy(function(record, id) {
                    if ( record.get('route') == thisEbSegRoute) {
                        return true;
                    }
                    return false;
                });
                if ( ttSegList.getCount() != 1 ) {
                    console.log("ERROR: Found duplicate or missing EB route in the travel time data. Route: " + thisEbSegRoute);
                    debugger;
                    return;
                }
                
                var ebForecastsStore = ttSegList.first().traveltimeforecasts();
                var selectedTime = Ext.getStore('wb-travel-time-segment-store').selectedDepartureTime;
                if ( !selectedTime ) {
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR: No selected time in recalcAggregateTable when looking for TO segment in travelTimeStore.");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    console.log("ERROR");
                    debugger;
                    return;
                }
                
                var toSegList = ebForecastsStore.queryBy(function(record, id) {
                    if ( record.get('departure').getTime() == selectedTime.getTime()) {
                        return true;
                    }
                    return false;
                });
                if ( toSegList.getCount() != 1 ) {
                    console.log("ERROR: Could not find selected departure time in TO segment: " + selectedTime);
                    debugger;
                    return;
                }
                var ttSegValueEB = toSegList.first().get('seconds');
                
                if ( fromIdx >= 0 ) {
                    thisSeg.beginEdit();
                    thisSeg.set(thisEbSegTo, ttSegValueEB);
                    thisSeg.endEdit(true, [thisEbSegTo]);
                    arr[index][fromIdx] = ttSegValueEB;
                }
            }
        });
        
        // Go back and aggregate/fill the rest of the wb cells (the right-hand dots in the asci art, above)
        for ( var row = (maxIdx - 2); row > 1; row-- ) {
            var wbCornerVal = arr[row-1][row];
            var segForThisRow = wbSegments.getAt(row-1);
            
            for ( var col = maxIdx; col > row; col-- ) {
                // Each val in the next row up is increased by the corner val on this row.
                var val = arr[row][col] + wbCornerVal;
                arr[row-1][col] = val;
                
                // Set the appropriate record / key
                var nm = names[col];
                segForThisRow.beginEdit();
                segForThisRow.set(nm, val);
                segForThisRow.endEdit(true, [nm]);
            }
        }
        
        // Go back and aggregate/fill the rest of the eb cells (the left dots in the asci art, above)
        for ( var row = 2; row < (maxIdx-1); row++ ) {
            var ebCornerVal = arr[row+1][row];
            var segForThisRow = wbSegments.getAt(row+1);
            
            for ( var col = 1; col < row; col++ ) {
                // Each val in the next row up is increased by the corner val on this row.
                var val = arr[row][col] + ebCornerVal;
                arr[row+1][col] = val;
                
                // Set the appropriate record / key
                var nm = names[col];
                segForThisRow.beginEdit();
                segForThisRow.set(nm, val);
                segForThisRow.endEdit(true, [nm]);
            }
        }
    }
});
