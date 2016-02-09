/**
 * Data model for the district holding vehicle data.
 * @author Paddy McCarthy
 */
Ext.define('emdss.model.DistrictForVehicles', {
    extend:'Ext.data.Model',
    requires:[
		'emdss.model.Vehicle'
    ],
    config:{
    	name: 'district',
		fields:[
		    {name:'min_lat',type:'float'},
		    {name:'min_lon',type:'float'},
		    {name:'max_lat',type:'float'},
		    {name:'max_lon',type:'float'},
		    {name:'district_name',type:'string'},
		    {name:'display_name',type:'string'}
		],
		associations : [
			{
				type :           'hasMany',
				associationKey:  'vehicles', // the key to the array in the data...
				instanceName:    'vehicles', // the name used to access the array in the store record
				associatedModel: 'emdss.model.Vehicle',
				associatedName:  'vehicles'  // the key to the array in the data...
			}
		]
    }
});