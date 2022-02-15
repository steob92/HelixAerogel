#ifndef HLXActionInitialization_h
#define HLXActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class HLXActionInitialization : public G4VUserActionInitialization
{
  public:
    HLXActionInitialization();
    virtual ~HLXActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
