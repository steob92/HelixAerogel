#include "HLXActionInitialization.hh"
#include "HLXPrimaryGeneratorAction.hh"
#include "HLXRunAction.hh"
// #include "HLXEventAction.hh"
// #include "HLXSteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HLXActionInitialization::HLXActionInitialization()
 : G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HLXActionInitialization::~HLXActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HLXActionInitialization::BuildForMaster() const
{
  HLXRunAction* runAction = new HLXRunAction;
  SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HLXActionInitialization::Build() const
{
  SetUserAction(new HLXPrimaryGeneratorAction);

  HLXRunAction* runAction = new HLXRunAction;
  SetUserAction(runAction);
  
  // HLXEventAction* eventAction = new HLXEventAction(runAction);
  // SetUserAction(eventAction);
  
  // SetUserAction(new HLXSteppingAction(eventAction));
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
