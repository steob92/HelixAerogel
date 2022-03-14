#!/bin/bash

# Energies to use
ENERGIES=(35 30 25 20 15 10)
# ENERGIES=(35)
# Number of particles to run
NPART=10000
NPROC=`nproc`


# Create and run a tempory macro for each energy
for ENG in ${ENERGIES[@]}; do
    sed -e "s|ENG|$ENG|" \
        -e "s|NPART|$NPART|" \
        -e "s|NTREAD|$NPROC|"  macros/nrc_batch.mac > nrc_batch_tmp.mac

    ./bin/HLX nrc_batch_tmp.mac 2>&1 | tee ${ENG}MeV.log
    # hadd output0.root `ls output0_t*.root`
    # hadd output1.root `ls output1_t*.root`
    # hadd output2.root `ls output2_t*.root`
    # hadd output3.root `ls output3_t*.root`
    # hadd output4.root `ls output4_t*.root`
    # hadd output5.root `ls output5_t*.root`

    # mv output0.root ${ENG}MeV_AllPhysics.root
    # mv output1.root ${ENG}MeV_NoMSC.root
    # mv output2.root ${ENG}MeV_NoMSCeBrem.root
    # mv output3.root ${ENG}MeV_NoMSCeBremRayl.root
    # mv output4.root ${ENG}MeV_NoMSCeBremRaylcompt.root
    # mv output5.root ${ENG}MeV_NoMSCeBremRaylcomptCoulombScat.root

    hadd ${ENG}MeV_AllPhysics.root `ls output0_t*.root`
    hadd ${ENG}MeV_NoMSC.root `ls output1_t*.root`
    hadd ${ENG}MeV_NoMSCeBrem.root `ls output2_t*.root`
    hadd ${ENG}MeV_NoMSCeBremRayl.root `ls output3_t*.root`
    hadd ${ENG}MeV_NoMSCeBremRaylcompt.root `ls output4_t*.root`
    hadd ${ENG}MeV_NoMSCeBremRaylcomptCoulombScat.root `ls output5_t*.root`


    rm output*.root
    # rm nrc_batch_tmp.mac
done