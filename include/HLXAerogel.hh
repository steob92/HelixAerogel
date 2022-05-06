#ifndef HLXAerogel_h
#define HLXAerogel_h 1

#include <fstream>
#include <string>

// Root
// #include "TGraph2D.h"
// #include "TH2F.h"

#include "HLXMaterials.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Box.hh"


/// Class for construction aerogel with complex shape and refractive index arangements

class HLXAerogel
{
    public:

        HLXAerogel();
        ~HLXAerogel();

        // Return the surface distance at (x,y)
        float GetSurface(float x, float y);
        // Return the thickness distance at (x,y)
        float GetThickness(float x, float y);

        // Construct Aerogel
        void MakeAerogel(G4LogicalVolume *logicWorld, G4double radiatorLocz, G4double radiatorSizeXY, G4double radiatorSizeZ);
        // void MakeAerogel(G4LogicalVolume *logicRadiator, G4LogicalVolume *logicWorld, G4double radiatorLocz, G4double radiatorSizeXY, G4double radiatorSizeZ);



    private:
        void LoadSurface(std::string filename);
        void LoadThickness(std::string filename);

        float fSurfaceParms [9];
        float fThicknessParms[9];
        // TH2F *fThickness;
        std::vector <std::vector <float>> fThicknessVector;


        HLXMaterials *fMaterials;
        

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif