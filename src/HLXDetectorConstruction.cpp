#include "HLXDetectorConstruction.hh"
#ifdef DEVMODE
    #include "HLXAerogel.hh"
#endif

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"


using namespace CLHEP;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Constructor
HLXDetectorConstruction::HLXDetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(0)
{ 
    fMaterials = new HLXMaterials();
    fMaterials->DefineMaterials();
    // fMaterials->PrintMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Dectructor
HLXDetectorConstruction::~HLXDetectorConstruction()
{ 
        delete fMaterials;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Build the detector
G4VPhysicalVolume* HLXDetectorConstruction::Construct()
{  


    // Option to switch on/off checking of volumes overlaps
    //
    G4bool checkOverlaps = true;





    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    // Construct the world
    // World volume 
    // Define the word as a 4m X 4m x 2m cube
    // Will be more convient to work in cm so 400 cm X 400 cm X 200 cm
    G4double worldSizeXY = 400 * cm;
    G4double worldZizeZ  = 200 * cm;

    G4Box* solidWorld =    
        new G4Box("World",                       //its name
            0.5*worldSizeXY, 0.5*worldSizeXY, 0.5*worldZizeZ);     //its size
        
    // Define a logical volume filled with air
    G4LogicalVolume* logicWorld =                         
        new G4LogicalVolume(solidWorld,          //its solid
                            fMaterials->GetMaterial("air"),           //its material
                            "World");            //its name
                                   
    G4VPhysicalVolume* physWorld = 
        new G4PVPlacement(0,                     //no rotation
                            G4ThreeVector(),       //at (0,0,0)
                            logicWorld,            //its logical volume
                            "World",               //its name
                            0,                     //its mother  volume
                            false,                 //no boolean operation
                            0,                     //copy number
                            checkOverlaps);        //overlaps checking
                     





    // Define the radiator plane
    // Assumng a 10cm x 10cm x 1cm radiator block
    // Located 10 cm away from the world centre (i.e. the gun location)
    G4double radiatorLocz = 10 * cm;
    G4double radiatorSizeXY = 10 * cm;
    G4double radiatorSizeZ = 1 * cm;
    

    G4double blockSize = 2*mm;
    G4int nRadiatorXY = radiatorSizeXY / blockSize;
    G4int nRadiatorZ = radiatorSizeZ / blockSize;
    // Building the radiator plane
    // G4Box *solidRadiator = new G4Box("Radiator", blockSize/2.0, blockSize/2.0, blockSize/2.0);
    G4Box *solidRadiator = new G4Box("Radiator", radiatorSizeXY/2.0, radiatorSizeXY/2.0, radiatorSizeZ/2.0);
    G4LogicalVolume* logicRadiator = new G4LogicalVolume(solidRadiator, fMaterials->GetMaterial("aerogel"), "LogicRadiator");


#ifdef DEVMODE
    G4cout << "Building Aerogel" << G4endl;
    radiatorSizeZ = 1.2 * cm;
    HLXAerogel *HLXRadiator = new HLXAerogel();
    HLXRadiator->MakeAerogel(logicWorld,  radiatorLocz,  radiatorSizeXY,  radiatorSizeZ);


#else
    // create logic radiator
    // Use the previously defined aerogel material
    // Construct a lego matrix
    // can later be used to vary thickness refractive index
    // for (int i = 0; i < nRadiatorXY; i++)
    // {
    //     for (int j = 0; j < nRadiatorXY; j++)
    //     {
    //         for (int k = 0; k < nRadiatorZ; k++)
    //         {

    //             // create physical radiator
    //             G4PVPlacement* physRadiator = new G4PVPlacement(0,                  //no rotation
    //                         G4ThreeVector(-5*cm + (i)*blockSize, -5*cm + (j)*blockSize, radiatorLocz -0.5*cm + (k)*blockSize),                       //at position
    //                         logicRadiator,                                          //its logical volume
    //                         "logicRadiator",                                        //its name
    //                         logicWorld,                                             //its mother  volume
    //                         false,                                                  //no boolean operation
    //                         k + 100*j + 10000 * i,                                                      //copy number
    //                         checkOverlaps);                                         //overlaps checking
    //         }
    //     }
        
    // }

    // create physical radiator
    G4PVPlacement* physRadiator = new G4PVPlacement(0,                  //no rotation
                G4ThreeVector(0, 0, radiatorLocz),                      //at position
                logicRadiator,                                          //its logical volume
                "logicRadiator",                                        //its name
                logicWorld,                                             //its mother  volume
                false,                                                  //no boolean operation
                0,                                                      //copy number
                checkOverlaps); 


#endif




    // G4OpticalSurface * OpAirSurface = new G4OpticalSurface("AirSurface");
    // OpAirSurface->SetModel(unified);
    // OpAirSurface->SetType(dielectric_dielectric);
    // OpAirSurface->SetFinish(ground);
    // G4LogicalBorderSurface* WaterSurface = new G4LogicalBorderSurface("AirSurface",physWorld, physRadiator, OpAirSurface);

    // Build the detector plane

    // Define as a square of width 60 cm and thickness 1nm (no need to be thick)
    // Detector is placed 20 cm away from the radiator exit surface
    G4double detectorSizeXY = 400 * cm;
    G4double detectorSizeZ  = 1 * cm;
    G4double detectorLocZ = 27.70926 * cm;   // Typical of values observed at NRC
    G4ThreeVector positionDetector = G4ThreeVector(0, 0, detectorLocZ + radiatorLocz + 0.5*radiatorSizeZ );

    
    // Define our CCD Detector

    G4Box* solidDetector =    
    new G4Box("Detector",                       //its name
        0.5*detectorSizeXY, 0.5*detectorSizeXY, detectorSizeZ);     //its size

    // Defining the detector matrial as a air.
    // We're only tracking the first interaction, this shouldn't really matter
    G4LogicalVolume* logicDetector =                         
        new G4LogicalVolume(solidDetector,         //its solid
                            fMaterials->GetMaterial("air"),          //its material
                            "Detector");           //its name
    
    fScoringVolume = logicDetector;
    // // new G4LogicalSkinSurface("detector_skin", logicDetector, detectorSurface);
    new G4PVPlacement(0,                       //no rotation
                positionDetector,                    //at position
                logicDetector,             //its logical volume
                "Detector",                //its name
                logicWorld,                //its mother  volume
                false,                   //no boolean operation
                0,                       //copy number
                checkOverlaps);          //overlaps checking
    
    


    // Add a thin glass layer infront of CCD
    #ifdef REAL_CCD
        // Detector is placed 20 cm away from the radiator exit surface
        G4double glassSizeXY = 60 * cm;
        G4double glassSizeZ  = 0.7 * mm;   // Glass is 0.7 mm thik
        G4double glassGap = 0.82 * mm;   // distance between the glass and the CCD chip
        G4ThreeVector positionGlass = G4ThreeVector(0, 0, (detectorLocZ + radiatorLocz) - (glassGap + 0.5*glassSizeZ) );

        // Create physical object
        G4Box* solidGlass =    
                new G4Box("Glass",//its name
                          0.5*glassSizeXY, 0.5*glassSizeXY, 0.5*glassSizeZ);
        // create logic radiator
        G4LogicalVolume* logicGlass = new G4LogicalVolume(solidGlass, fMaterials->GetMaterial("glass"), "logicGlass");

        // create physical radiator
        new G4PVPlacement(0,                       //no rotation
                    positionGlass,                    //at position
                    logicGlass,             //its logical volume
                    "logicGlass",                //its name
                    logicWorld,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
        

    #endif

    // (void)physWorld;

    //always return the physical World
    //
    return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



void HLXDetectorConstruction::ConstructSDandField()
{
    // Define our CCD Detector
    // Calling it a SIPM... actually a CCD...
    HLXCCDDetector* detector = new HLXCCDDetector("SIPMs");
    fScoringVolume->SetSensitiveDetector(detector);
}
