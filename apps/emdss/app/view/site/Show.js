/**
 * Class to show the plots and associated data tabs for a site.
 */
Ext.define('emdss.view.site.Show', {
    extend : 'Ext.tab.Panel',
    xtype : 'site-show',
    requires : ['Ext.form.Panel', 'emdss.view.TreatmentExplanationList'],
    config : {
	id : 'show-site',
	layout : 'card',
	items : [{
	    id : 'summary_plots',
	    title : 'Forecast',
	    scrollable : 'both',
	    tpl : ['<div><img src="data:image/png;base64,{summary_plot}"></img></div>'].join('')
	}/*,{
	   },{
	   id:'precip_plots',
	   title:'Precipitation',
	   scrollable:'both',
	   tpl:[
	   '<div><img src="data:image/png;base64,{precip_plot}"></img></div>'
	   ].join('')
	   },{
	   id:'treatment_explanations',
	   title:'Treatment Explanations',
	   xtype:'treatment-explanations'
	   }*/],
	record : null
    },
    
    setTimeSeries: function(time_series) {
		this.time_series = time_series;
    },

    setSiteName: function(site_name) {
		this.site_name = site_name;
    },

    updateRecord : function(newRec) {
		if (newRec && newRec.get('summary_plot') && newRec.get('summary_plot').length > 0) {
	
		    // Clear out the old tabs
	
		    this.down('#summary_plots').setData(newRec.data);
		    if (this.treatment_plots) {
				this.remove(this.treatment_plots);
				this.treatment_plots = null;
		    }
		    if (this.alert_plots) {
				this.remove(this.alert_plots);
				this.alert_plots = null;
		    }
		    if (this.treatment_expl) {
				this.remove(this.treatment_expl);
				this.treatment_expl = null;
		    }
		    if (this.wx_obs) {
				this.remove(this.wx_obs);
				this.wx_obs = null;
		    }
		    if (this.rwis) {
				this.remove(this.rwis);
				this.rwis = null;
		    }
		    if (this.road_segment) {
				this.remove(this.road_segment);
				this.road_segment = null;
		    }
	
		    // Create the new treatment tab
	
		    if (this.time_series) {
	            var filteredTreatments = [];
	            var filteredAlerts = [];
	            this.time_series.each(function (dataPoint, index, length) {
	                var thisTime = parseInt( dataPoint.get('time') );
	
	                var treatmentObj = {time: dataPoint.get('time'),
	                             treatment_alert_code: dataPoint.get('treatment_alert_code'),
	                             chemical: dataPoint.get('chemical'),
	                             plow: dataPoint.get('plow'),
	                             road_temp: dataPoint.get('road_temp')};
	                filteredTreatments.push(treatmentObj);
	
	                var alertObj = {time: dataPoint.get('time'),
	                                alert_code: dataPoint.get('alert_code'),
	                                precip: dataPoint.get('precip'),
	                                pavement: dataPoint.get('pavement'),
	                                visibility: dataPoint.get('visibility')};
	                filteredAlerts.push(alertObj);
	            });
	                                    
				this.treatment_plots = this.add({
				    id : 'treatment_plots',
				    title : 'Treatments',
				    xtype : 'list',
				    scrollable : 'both',
				    itemTpl : this.createSiteTreatmentsItemTpl(),
				    data : filteredTreatments
				});
		
				this.alert_plots = this.add({
				    id : 'alert_plots',
				    title : 'Alerts',
				    xtype : 'list',
				    scrollable : 'both',
				    itemTpl : this.createSiteAlertsItemTpl(),
				    data : filteredAlerts
				});
		    }
	
		    // Create the new weather observations tab
	
		    if (newRec.data['wx_obs']) {
				var wx_obs = newRec.data['wx_obs'];
				this.wx_obs = this.add({
				    id : 'wx_obs',
				    title : 'Wx Observations',
				    xtype : 'formpanel',
				    scrollable : true,
				    items : [{
						xtype : 'textfield',
						name : 'site_name',
						label : 'Site Name',
						readOnly : true,
						value : this.site_name
				    }, {
						xtype : 'textfield',
						name : 'site_type',
						label : 'Site Type',
						readOnly : true,
						value : 'metars'
				    }, {
						xtype : 'textfield',
						name : 'obstime',
						label : 'Observation Time',
						readOnly : true,
						value : wx_obs['obstime']
				    }, {
						xtype : 'textfield',
						name : 'temp',
						label : 'Temperature',
						readOnly : true,
						value : wx_obs['temp']
				    }, {
						xtype : 'textfield',
						name : 'dewp',
						label : 'Dew Point',
						readOnly : true,
						value : wx_obs['dewp']
				    }, {
						xtype : 'textfield',
						name : 'wind_spd',
						label : 'Wind Speed',
						readOnly : true,
						value : wx_obs['wind_spd']
				    }]
				});
		    }
	
		    // Create the new RWIS observations tab
		    if (newRec.data['rwis']) {
				var rwis = newRec.data['rwis'];
				this.rwis = this.add({
				    id : 'rwis',
				    title : 'RWIS Observations',
				    xtype : 'formpanel',
				    scrollable : true,
				    items : [{
						xtype : 'textfield',
						name : 'site_name',
						label : 'Site Name',
						readOnly : true,
						value : this.site_name
				    }, {
						xtype : 'textfield',
						name : 'site_type',
						label : 'Site Type',
						readOnly : true,
						value : 'RWIS'
				    }, {
						xtype : 'textfield',
						name : 'temp',
						label : 'Temperature',
						readOnly : true,
						value : rwis['temp']
				    }, {
						xtype : 'textfield',
						name : 'road_temp_1',
						label : 'Road Temperature',
						readOnly : true,
						value : rwis['road_temp_1']
				    }, {
						xtype : 'textfield',
						name : 'road_temp_2',
						label : 'Road Temperature',
						readOnly : true,
						value : rwis['road_temp_2']
				    }, {
						xtype : 'textfield',
						name : 'sub_surface_1',
						label : 'Sub Surface Temp',
						readOnly : true,
						value : rwis['sub_surface_1']
				    }, {
						xtype : 'textfield',
						name : 'sub_surface_2',
						label : 'Sub Surface Temp',
						readOnly : true,
						value : rwis['sub_surface_2']
				    }, {
						xtype : 'textfield',
						name : 'wind_spd',
						label : 'Wind Speed',
						readOnly : true,
						value : rwis['wind_spd']
				    }, {
						xtype : 'textfield',
						name : 'wind_dir',
						label : 'Wind Dir',
						readOnly : true,
						value : rwis['wind_dir']
				    }, {
						xtype : 'textfield',
						name : 'rel_hum',
						label : 'Relative Humidity',
						readOnly : true,
						value : rwis['rel_hum']
				    }, {
						xtype : 'textfield',
						name : 'obstime',
						label : 'Observation Time',
						readOnly : true,
						value : rwis['obstime']
				    }]
				});
			}
				
			// Create the new Road Segment observations tab
			if (newRec.data['road_segments']) {
				var road_segment = newRec.data['road_segments'];
				this.road_segment = this.add({
				    id : 'road_segment',
				    title : 'Road Segment Observations',
				    xtype : 'formpanel',
				    scrollable : true,
				    items : [{
						xtype : 'textfield',
						name : 'site_name',
						label : 'Site Name',
						readOnly : true,
						value : this.site_name
				    }, {
						xtype : 'textfield',
						name : 'site_type',
						label : 'Site Type',
						readOnly : true,
						value : 'Road Segment'
				    }, {
						xtype : 'textfield',
						name : 'mean_air_temp',
						label : 'Mean Air Temp',
						readOnly : true,
						value : road_segment['nss_air_temp_mean']
				    }, {
						xtype : 'textfield',
						name : 'model_air_temp',
						label : 'Model Air Temp',
						readOnly : true,
						value : road_segment['model_air_temp']
				    }, {
						xtype : 'textfield',
						name : 'radar_cref',
						label : 'Radar CREF',
						readOnly : true,
						value : road_segment['radar_cref']
				    }, {
						xtype : 'textfield',
						name : 'mean_barometric_pressure',
						label : 'Mean Barometric Pressure',
						readOnly : true,
						value : road_segment['nss_bar_press_mean']
				    }, {
						xtype : 'textfield',
						name : 'model_barometric_pressure',
						label : 'Model Barometric Pressure',
						readOnly : true,
						value : road_segment['model_bar_press']
				    }, {
						xtype : 'textfield',
						name : 'time',
						label : 'Time',
						readOnly : true,
						value : this.formatUnixTime(road_segment['time'])
				    }]
				});
		    }
		    /*
		      this.down('#precip_plots').setData(newRec.data);
		      console.log(newRec.data['treatment_explanations']);
		    */
		}
		else {
		    // No plot available...
		    this.down('#summary_plots').setHtml('No Summary Available for This Location');
		}
    },

	formatValue : function(value, numDec) {
		var fvalue = parseFloat(value);
		if ( isNaN(fvalue) ) {
			// ( value == '--' )
			return 'missing';
		}
		else if ( fvalue == -9999.0 || fvalue == -99 ) {
			return 'unknown'
		}
		else {
			return '' + fvalue.toFixed(numDec);
		}
	},
	
	formatUnixTime : function(utime) {
		var udate = new Date(utime*1000);
		return emdss.util.Util.dateToBriefStringWithSecondsNoNBSP(udate);
	},
	
    /**
     * Create the itemTpl used to style the site treatments list -- the list of 
     *   each time step for a site within the selected time range. Uses a
     *   css class to calculate the position of each alert icon within
     *   the alert image sprite. Note that the <img> tag references a
     *   transparent image -- that holds the space for the icon rendered
     *   by the div class reference.
     */
    createSiteTreatmentsItemTpl: function() {
        var me = this;
        
        return new Ext.XTemplate(
            '<div>',
            '  <div class="site_list_time">',
            '    {[this.getPrettyTimeString(values.time)]}',
            '  </div>',
            '  <div class="site_list_alert_img">',
            '    <img class="alert_{treatment_alert_code}" src="resources/icons/Alert_Transparent.png"></img>',
            '  </div>',
            '  <div class="site_list_alert_explanation">',
            '    {[this.getAlertExplanation(values.treatment_alert_code, values.chemical, values.plow, values.road_temp)]}',
            '  </div>',
            '</div>',
            {
                getPrettyTimeString: function(timeStr) {
                    var shortDate = me.getPrettyDateStringFromString(timeStr);
                    return shortDate;
                },
                getAlertExplanation: function(treatmentAlertCode, chemical, plow, roadTemp) {
                    var alert_string =  'Unexpected Alert Code: ' + treatmentAlertCode;
                    if ( treatmentAlertCode == null || treatmentAlertCode == undefined ||
			 treatmentAlertCode == 'missing' ) {
                        alert_string =  'Missing data for this time';
                    }
                    else if (treatmentAlertCode == 'clear' ||
			    treatmentAlertCode == 'Clear') {
                        alert_string =  'Clear';
                    }
                    else if (treatmentAlertCode == 'warning' ||
			     treatmentAlertCode == 'Warning') {
                        alert_string = '<b>Advisory:</b> Chemical: ' + chemical + ', Plow: ' + plow + ', Road temp: ' + roadTemp + ' deg';
                    }
                    else if (treatmentAlertCode == 'alert' ||
			     treatmentAlertCode == 'Alert') {
                        alert_string = '<b>Warning:</b> Chemical: ' + chemical + ', Plow: ' + plow + ', Road temp: ' + roadTemp + ' deg';
                    }
                    

		    return alert_string;
                },
                isPropertyPresent: function(prop) {
                    return (prop != undefined && prop != null);
                }
            }
        );

    },

    /**
     * Create the itemTpl used to style the site alerts list -- the list of 
     *   each time step for a site within the selected time range. Uses a
     *   css class to calculate the position of each alert icon within
     *   the alert image sprite. Note that the <img> tag references a
     *   transparent image -- that holds the space for the icon rendered
     *   by the div class reference.
     */
    createSiteAlertsItemTpl: function() {
        var me = this;
        
        return new Ext.XTemplate(
            '<div>',
            '  <div class="site_list_time">',
            '    {[this.getPrettyTimeString(values.time)]}',
            '  </div>',
            '  <div class="site_list_alert_img">',
            '    <img class="alert_{alert_code}" src="resources/icons/Alert_Transparent.png"></img>',
            '  </div>',
            '  <div class="site_list_alert_explanation">',
            '    {[this.getAlertExplanation(values.alert_code, values.precip, values.pavement, values.visibility)]}',
            '  </div>',
            '</div>',
            {
                getPrettyTimeString: function(timeStr) {
                    var shortDate = me.getPrettyDateStringFromString(timeStr);
                    return shortDate;
                },
                getAlertExplanation: function(alertCode, precip, pavement, visibility) {
                    var alert_string =  'Unexpected Alert Code: ' + alertCode;
                    if (alertCode == null || alertCode == undefined ||
			alertCode == 'missing' ) {
                        alert_string =  'Missing data for this time';
                    }
                    else if (alertCode == 'clear' ||
			     alertCode == 'Clear') {
                        alert_string =  'Clear';
                    }
                    else if (alertCode == 'warning' ||
			     alertCode == 'Warning') {
                        alert_string = '<b>Advisory:</b> Precip: ' + precip + ', Pavement: ' + pavement + ', Visibility: ' + visibility;
                    }
                    else if (alertCode == 'alert' ||
			     alertCode == 'Alert') {
                        alert_string = '<b>Warning:</b> Precip: ' + precip + ', Pavement: ' + pavement + ', Visibility: ' + visibility;
                    }
                    

		    return alert_string;
                },
                isPropertyPresent: function(prop) {
                    return (prop != undefined && prop != null);
                }
            }
        );

    },

    /**
     * Uses Util methods to convert from "yyyymmddhhmm" to a nicer "hh:mm on dd/mm" format
     * @param {Object} uglyDateStr The date string in "yyyymmddhhmm" format
     */
    getPrettyDateStringFromString: function (uglyDateStr) {
        var uglyDate = emdss.util.Util.emdssStringToDate( uglyDateStr );
        var prettyDateStr = this.getPrettyDateStringFromDate( uglyDate );
        return prettyDateStr;
    },
        
    /**
     * Converts from a date object to a nice "hh:mm on dd/mm" string
     * @param {Object} uglyDate The date object
     */
    weekday: ["Sun", "Mon", "Tues", "Weds", "Thurs", "Fri", "Sat"],
                
    getPrettyDateStringFromDate: function (dateObj) {
        var me = this;
        var ap = 'am';
        var hrs = dateObj.getHours();
        if ( hrs >= 12 ) {
            if ( hrs > 12 ) {
                hrs -= 12;
            }
            ap = 'pm';
        }
	var month = dateObj.getMonth() + 1;
	var day = dateObj.getDate();
        var mins = dateObj.getMinutes();
        var dateString = me.weekday[dateObj.getDay()] + ' ' + month + '/' + day + ' ' +
	    hrs + ':' + (mins >= 10 ? mins : '0' + mins) + ' ' + ap;
        return dateString;
    }
        
 });
