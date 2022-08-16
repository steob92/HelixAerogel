#ifndef HLXPrimaryGeneratorAction_h
#define HLXPrimaryGeneratorAction_h 1
#include "G4GenericMessenger.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class G4GenericMessenger;
class G4Box;

/// The primary generator action class with particle gun.
///
/// The default kinematic is a 6 MeV gamma, randomly distribued 
/// in front of the phantom across 80% of the (X,Y) phantom size.

class HLXPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    HLXPrimaryGeneratorAction();    
    virtual ~HLXPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);         
  
    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    G4bool fUniform;
    G4bool fDivergence;
    G4bool fBeamEnergy;
    // G4double fThetaDiv;
    G4double fMomentum;
    G4double fSigmaMomentum;
    G4double fSigmaAngle;
    G4String fBeamType;
    G4double fsigmaBeamX;
    G4double fsigmaBeamY;

  private:
    G4ParticleGun*  fParticleGun; // pointer a to G4 gun class
    G4Box* fEnvelopeBox;
    G4GenericMessenger* fMessenger;

    void DefineCommands();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
