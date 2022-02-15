#!/bin/bash

# Energies to use
ENERGIES=(35 30 25 20 15 10)
# Number of particles to run
NPART=1000



# Create and run a tempory macro for each energy
for ENG in ${ENERGIES[@]}; do
    sed -e "s|ENG|$ENG|" \
        -e "s|NPART|$NPART|"  macros/nrc_batch.mac > nrc_batch_tmp.mac

    ./bin/HLX nrc_batch_tmp.mac 2>&1 > ${ENG}MeV.log
    mv output0.root ${ENG}MeV.root
    rm nrc_batch_tmp.mac
done