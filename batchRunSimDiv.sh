#!/bin/bash
#When working with obriens/root-geant4:v0.1
source sourceGeant4.sh
# Energies to use
ENERGIES=35
DIVS=(1 2 3 4 5 10 15 20)
# ENERGIES=(35)
# Number of particles to run
NPART=1000
# NPROC=$(expr `nproc` - 2) 2
NPROC=1


# Create and run a tempory macro for each energy
for DIV in ${DIVS[@]}; do
    sed -e "s|ENG|$ENERGIES|" \
        -e "s|NPART|$NPART|" \
        -e "s|ADIV|$DIV|" \
        -e "s|NTREAD|$NPROC|"  macros/nrc_batch_div.mac > nrc_batch_tmp.mac

    ./bin/HLX nrc_batch_tmp.mac 2>&1 | tee ${DIV}Deg.log
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

    hadd ${DIV}Deg_AllPhysics.root `ls output0_t*.root`
    # hadd ${ENG}MeV_NoBrem.root `ls output1_t*.root`
    # hadd ${ENG}MeV_NoBremNoRayl.root `ls output2_t*.root`
    # hadd ${ENG}MeV_NoBremNoRaylNoCompt.root `ls output3_t*.root`
    # hadd ${ENG}MeV_NoBremNoRaylNoComptNoCoul.root `ls output4_t*.root`
    # hadd ${ENG}MeV_NoBremNoRaylNoComptNoCoulNoMSC.root `ls output5_t*.root`
    # hadd ${ENG}MeV_NoBremNoRaylNoComptNoCoulNoMSCNoIoni.root `ls output6_t*.root`


    rm output*.root
    # rm nrc_batch_tmp.mac
done
