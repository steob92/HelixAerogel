# include "HLX_CCDDetector.hh"



 // Constructor
HLX_CCDDetector::HLX_CCDDetector(const G4String &name)
    :G4VSensitiveDetector(name)
{
    fDectName = name;

    // Initializing the rest to zero
    fEventNum = 0;
    fEventID = 0;
    fCCounter = 0;
    fX = 0;
    fY = 0;
    fE = 0;



}

// Destructor
HLX_CCDDetector::~HLX_CCDDetector()
{

}

// Methods
void HLX_CCDDetector::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent)
{
    // Increment the internal event number
    fEventNum += 1;
}


G4bool HLX_CCDDetector::ProcessHits(G4Step* step, __attribute__((unused)) G4TouchableHistory*)
{

    G4int iTupID;

    // Only care about the first interaction in the material
    if (step->IsFirstStepInVolume()) 
    {

        G4AnalysisManager* analysisManager =   G4AnalysisManager::Instance();


        G4Track* track = step->GetTrack();
        G4ThreeVector pos = track->GetPosition();
        G4String particleName = track->GetDynamicParticle()->GetParticleDefinition()->GetParticleName();

        fEventNum = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

        // Get the X,y location of the particle
        fX = pos.x();
        fY = pos.y();


        if (particleName == "opticalphoton") 
        {
            G4double depEnergy = step->GetTotalEnergyDeposit();
            fCCounter+=1;
            // fE = depEnergy/CLHEP::eV;
            fE = track->GetKineticEnergy()/CLHEP::eV;
            iTupID = 0;

        }
        else if (particleName == "e-") 
        {

            fE = track->GetKineticEnergy()/CLHEP::MeV;
            iTupID = 1;
        }
        else 
        {
            return true;
        }

        analysisManager->FillNtupleFColumn(iTupID, 0, fEventNum);
        analysisManager->FillNtupleFColumn(iTupID, 1, fX);
        analysisManager->FillNtupleFColumn(iTupID, 2, fY);
        analysisManager->FillNtupleFColumn(iTupID, 3, fE);
        analysisManager->AddNtupleRow(iTupID);


    }

    return true;

}



void HLX_CCDDetector::EndOfEvent(G4HCofThisEvent* hitsCollectionOfThisEvent)
{


}



