#!/bin/bash

set -x

./probe_to_json -l log/probe_to_json_mn 201612122020 input/mn_cdf_config.cfg input/vdt.20161212.2020.nc output/vdt.20161212.2020.json
