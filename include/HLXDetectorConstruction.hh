#ifndef HLXDetectorConstruction_h
#define HLXDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "HLXCCDDetector.hh"
#include "HLXMaterials.hh"

// class G4VPhysicalVolume;
// class G4LogicalVolume;

/// Detector construction class to define materials and geometry.

class HLXDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    HLXDetectorConstruction();
    virtual ~HLXDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    
    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }

  protected:
    G4LogicalVolume*  fScoringVolume;
   
  private:
    HLXMaterials *fMaterials;
    virtual void ConstructSDandField();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
