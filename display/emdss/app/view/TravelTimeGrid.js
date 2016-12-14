
Ext.define('emdss.view.TravelTimeGrid', {
        extend: 'Ext.dataview.DataView',
        xtype:  'traveltimegrid',
        config: {
        title: 'Travel Times',
        cls: 'x-sites',
        disableSelection: true,
        fullscreen: true,
            
        store: 'wb-travel-time-segment-store'
    },
    
    initialize : function() {
        this.callParent(arguments);
        
        var me = this;
        me.setItemTpl( me.createItemTpl() );
        
        me.ttStore = Ext.getStore('travel-time-store');
        console.log(me.ttStore.first());
    },
    
    createItemTpl: function() {
        return new Ext.XTemplate(
            '<div>',
            '  <tpl if="this.isHeaderRecord(name)">',
            '    <tpl exec="this.resetRowCount(name)"></tpl>',
            '    <div style="height:150px; float:none; background-color:#288cd2; color:white;">',
            '      <div class="traveltime_row_header">&nbsp;</div>',
            '      <div class="traveltime_column_header"><br>To:<br>Peña Blvd</div>',
            '      <div class="traveltime_column_header"><br>To:<br>I-225</div>',
            '      <div class="traveltime_column_header"><br>To:<br>I-270</div>',
            '      <div class="traveltime_column_header"><br>To:<br>I-25</div>',
            '      <div class="traveltime_column_header"><br>To:<br>I-76/Wadsworth</div>',
            '      <div class="traveltime_column_header"><br>To:<br>Ward Road</div>',
            '      <div class="traveltime_column_header"><br>To:<br>DEN/C-470</div>',
            '      <div class="traveltime_column_header"><br>To:<br>Idaho Springs</div>',
            '      <div class="traveltime_column_header"><br>To:<br>US 40/Empire</div>',
            '      <div class="traveltime_column_header"><br>To:<br>Georgetown</div>',
            '      <div class="traveltime_column_header"><br>To:<br>Tunnel</div>',
            '      <div class="traveltime_column_header"><br>To:<br>Silverthorne</div>',
            '      <div class="traveltime_column_header"><br>To:<br>Vail</div>',
            '    </div>',
            '    <hr style="float:none;" class="traveltime_separator">',
            '  <tpl else>',
            '    <tpl if="this.isEvenRow(name)">',
            '      <div style="width:1500px; float:none; background-color:#f6eeee;">',
            '      <div class="traveltime_row_header" style="background-color:#2e92d8; color:white;">From: {name}</div>',
            '    <tpl else>',
            '      <div style="width:1500px; float:none; background-color:#eeeeee;">',
            '      <div class="traveltime_row_header" style="background-color:#288cd2; color:white;">From: {name}</div>',
            '    </tpl>',
            '      <div class="traveltime_first_cell">{[this.hrMinFmt(values["Pena_Blvd"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["I-225"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["I-270"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["I-25"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["I-76_Wadsworth"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["Ward_Road"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["DEN_C-470"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["Idaho_Springs"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["US_40_Empire"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["Georgetown"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["Tunnel"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["Silverthorne"])]}</div>',
            '      <div class="traveltime_cell">{[this.hrMinFmt(values["Vail"])]}</div>',
            '      <div style="float:none;"><hr class="traveltime_separator"></div>',
            '    </div>',
            '  </tpl>',
            '</div>',
        {
            isHeaderRecord: function(text) {
                if ( text == 'HEADER' ) {
                    return true;
                }
                
                return false;
            },
            resetRowCount: function(dummy) {
                console.log("Resetting row count.");
                this.rowCount = 1;
                return false;
            },
            isEvenRow: function(dummy) {
                if ( !this.rowCount ) {
                    console.log("Resetting row count.");
                    this.rowCount = 1;
                    return false;
                }

                this.rowCount++;
                console.log("" + dummy + " Row count: " + this.rowCount);
                if ( (this.rowCount % 2) == 0 ) {
                    console.log("    EVEN");
                    return true;
                }
                
                console.log("    ODD");
                return false;
            },
            hrMinFmt: function(val) {
                if ( val == -1 ) {
                    console.log("hrMinFmt returning space for value: " + val);
                    return '&nbsp;';
                }
                else {
                    var total_mins = Math.round (val / 60.0 );
                    var hrs = Math.floor( total_mins / 60.0 );
                    var mins = total_mins % 60;
                    
                    if ( hrs == 0 ) {
                        return '' + mins + 'min';
                    }
                    else {
                        return '' + hrs + 'hr ' + mins + 'min';
                    }
                }
            },
        }
    );
    }
});
