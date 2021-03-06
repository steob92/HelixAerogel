#ifndef HLXCCDDetector_h
#define HLXCCDDetector_h 1

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"    
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VSensitiveDetector.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
//#include "g4root.hh"


#include "TFile.h"
#include "TTree.h"



class G4HCofThisEvent;
class G4TouchableHistory;
class G4Step;
using namespace std;
class HLXCCDDetector : public G4VSensitiveDetector {

public:

    // Constructor
    HLXCCDDetector(const G4String &name);

    // Destructor
    virtual ~HLXCCDDetector();

    // Methods
    virtual void Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent);
    virtual G4bool ProcessHits(G4Step* step,G4TouchableHistory* history);
    virtual void EndOfEvent(G4HCofThisEvent* hitsCollectionOfThisEvent);



private:


    // Detector specific details
    G4String fDectName;

    //  Private data memebers
    G4int fEventNum;            // Event number
    G4int fEventID;             // Event Identifier
    G4int fCCounter;            // Count Cherenkov photons
    G4int fPhotID;
    G4int fEleID;


    // Particle details
    G4double fX;                // Particle's x-location
    G4double fY;                // Particle's y-location
    G4double fE;                // Particle's energy


};


#endif
