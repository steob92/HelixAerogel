
#include "HLXRunAction.hh"

HLXRunAction::HLXRunAction()
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();


    // Photon Data Tree
    fPhotID = 0;
    analysisManager->CreateNtuple("Photon", "Photon Quantities");
    // X = D in CreateNtupleXColumn stands for G4double (I,F,D,S)
    analysisManager->CreateNtupleFColumn("ID");             // Event ID
    analysisManager->CreateNtupleFColumn("X");              // X-loc
    analysisManager->CreateNtupleFColumn("Y");              // Y-loc
    analysisManager->CreateNtupleFColumn("E");              // Energy
    analysisManager->FinishNtuple(fPhotID);

    // Electron Data Tree
    fEleID = 1;
    analysisManager->CreateNtuple("Electron", "Electron Quantities");
    // X = D in CreateNtupleXColumn stands for G4double (I,F,D,S)
    analysisManager->CreateNtupleFColumn("ID");             // Event ID
    analysisManager->CreateNtupleFColumn("X");              // X-loc
    analysisManager->CreateNtupleFColumn("Y");              // Y-loc
    analysisManager->CreateNtupleFColumn("E");              // Energy
    analysisManager->FinishNtuple(fEleID);
}

HLXRunAction::~HLXRunAction()
{}

void HLXRunAction::BeginOfRunAction(const G4Run* run)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    G4int runID = run->GetRunID();

    std::stringstream strRunID;
    strRunID << runID;

    analysisManager->OpenFile("output"+strRunID.str()+".root");
}

void HLXRunAction::EndOfRunAction(const G4Run*)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    analysisManager->Write();
    analysisManager->CloseFile();
}