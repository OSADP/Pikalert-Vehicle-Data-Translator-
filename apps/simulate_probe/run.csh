#!/bin/csh

simulate_probe  -c 1 -m /d4/gerry/simulation_dir/simulation_may_mn_more_dense_no_winds/metar.201405300500-201406010500.txt -w /d4/gerry/simulation_dir/simulation_may_mn_more_dense_no_winds/madis.201405300500-201406010500.txt -r /d4/gerry/simulation_dir/simulation_may_mn_more_dense_no_winds/radar_tile.201405300500-201406010500.txt -a /d4/gerry/simulation_dir/simulation_may_mn_more_dense_no_winds/rtma2p5.201405300500-201406010500.txt /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201405300500 201406010500 /d4/gerry/simulation_dir/simulation_may_mn_more_dense_no_winds/probe_message /d4/gerry/simulation_dir/simulation_may_mn_more_dense_no_winds/road_data

#./simulate_probe -m metar_files.txt -w madis_files.txt -a rtma_files.txt -r radar_files.txt  /d4/gerry/simulation_config/mn_simulation.cfg 300 201412142355 201412150000 probe_message.txt road_data


