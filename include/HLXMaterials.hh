#ifndef HLXMaterials_h
#define HLXMaterials_h 1

#include <string>
#include <vector>
#include <cmath>

#include "G4NistManager.hh"
#include "G4Material.hh"



/// Class to handle Material Definitions

class HLXMaterials 
{
    public:
        HLXMaterials();
        ~HLXMaterials();

        void DefineMaterials();
        G4Material* GetMaterial(std::string matName);
        void PrintMaterials();

    private:
        std::vector <std::string> fMaterialNames;
        std::vector <G4Material*> fMaterials;
        G4double fIsoMinWL;
        G4double fIsoMaxWL;
        G4double fEnergyRangeMin;
        G4double fEnergyRangeMax;
        G4double fDeltaEnergy;
        G4int fNPoints;
        G4double *fEnergyRange;
        G4NistManager* fNist;


};


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif