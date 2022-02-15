#include "HLXDetectorConstruction.hh"

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

using namespace CLHEP;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Constructor
HLXDetectorConstruction::HLXDetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Dectructor
HLXDetectorConstruction::~HLXDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Build the detector
G4VPhysicalVolume* HLXDetectorConstruction::Construct()
{  
    // Get nist material manager
    G4NistManager* nist = G4NistManager::Instance();

    // Option to switch on/off checking of volumes overlaps
    //
    G4bool checkOverlaps = true;



    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    // Define upper and lower range of optical photons
    // Properties later on will be defined over this range
    G4double iso_minWL = 400;
    G4double iso_maxWL = 700;
    G4double energyRange[] = {1239.84193*eV/iso_maxWL, 1239.84193*eV/iso_minWL};

    
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    // Define some materials
    // Start off with a vacuum
    G4double atomicNumber = 1.;
    G4double massOfMole = 1.008*g/mole;
    G4double density = 1.e-25*g/cm3;
    G4double temperature = 2.73*kelvin;
    G4double pressure = 3.e-18*pascal;
    
    G4Material* vacuum_mat =
        new G4Material("interGalactic", atomicNumber,
        massOfMole, density, kStateGas,
        temperature, pressure);

    // create refractive index for vacuum
    G4MaterialPropertiesTable *vacuum_MPT = new G4MaterialPropertiesTable();
    G4double vacuum_n[] = {1.00, 1.00};
    vacuum_MPT->AddProperty("RINDEX", energyRange, vacuum_n, 2);
    vacuum_mat->SetMaterialPropertiesTable(vacuum_MPT);

    
    // Define air and set the refractive index
    G4Material* air_mat = nist->FindOrBuildMaterial("G4_AIR");
    G4MaterialPropertiesTable *air_MPT = new G4MaterialPropertiesTable();
    G4double air_n[] = {1.0003, 1.0003};
    air_MPT->AddProperty("RINDEX", energyRange, air_n, 2);
    air_mat->SetMaterialPropertiesTable(air_MPT);


    G4Element* el_H  = new G4Element("Hydrogen", "H", 1., 1.01*g/mole);
    G4Element* el_O  = new G4Element("Oxygen", "O", 8., 16.00*g/mole);
    G4Element* el_Si  = new G4Element("Silicon", "Si", 14., 28.1*g/mole);

    // Define Aerogel
    G4int nComponent = 3;
    // "Non interacting aerogel"
    // This is effectively a vacuum the refractive index of aerogel
    G4Material *aerogel_mat = new G4Material("Aerogel", 1.e-25*g/cm3, nComponent,
                                            kStateGas, 0.1*kelvin,  1.e-19*pascal);

    //G4Material *aerogel_mat = new G4Material("Aerogel",  1.0,  1.e-8*g/mole,  1.e-25*g/cm3,
    // 				      kStateGas, 0.1*kelvin,  1.e-19*pascal);
    // Room temperate, sea level preasure
    // Reaslistic Aerogel
    // G4Material *Aerogel = new G4Material("Aerogel", 0.53*g/cm3, nComponent);
    //                                 //    kStateGas, 293.*kelvin,  101325.*pascal);

    // //See Buzykaev et al NIM A433 (1999) 396
    G4double aerogelFixedIndex = 1.15;
    G4double aero_n[]  = {aerogelFixedIndex, aerogelFixedIndex};

    // G4double aerogelFixedAbsoLength = 0.16;
    G4double aerogelFixedAbsoLength = 100000.;
    G4double aero_abs[] = {aerogelFixedAbsoLength, aerogelFixedAbsoLength};
    
    // add elemental composition of aerogel
    aerogel_mat->AddElement(el_Si, 1);
    aerogel_mat->AddElement(el_O, 4);
    aerogel_mat->AddElement(el_H, 4);

    // // create aerogel material properties
    G4MaterialPropertiesTable *aero_MPT = new G4MaterialPropertiesTable();
    aero_MPT->AddProperty("RINDEX",  energyRange, aero_n, 2);
    aero_MPT->AddProperty("ABSLENGTH", energyRange, aero_abs, 2);
    aerogel_mat->SetMaterialPropertiesTable(aero_MPT);



    // Define glass
    G4Material* glass_mat =  nist->FindOrBuildMaterial("G4_GLASS_PLATE");
    double glassFixedIndex = 1.5;
    double glass_n[]  = {glassFixedIndex, glassFixedIndex};
    G4MaterialPropertiesTable *glass_MPT = new G4MaterialPropertiesTable();
    glass_MPT->AddProperty("RINDEX",  energyRange, glass_n, 2);
    glass_mat->SetMaterialPropertiesTable(glass_MPT);


    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    // Construct the world
    // World volume 
    // Define the word as a 1m X 1m x 2m cube
    // Will be more convient to work in cm so 100 cm X 100 cm X 200 cm
    G4double world_sizeXY = 100 * cm;
    G4double world_sizeZ  = 200 * cm;

    G4Box* solidWorld =    
        new G4Box("World",                       //its name
            0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
        
    // Define a logical volume filled with air
    G4LogicalVolume* logicWorld =                         
        new G4LogicalVolume(solidWorld,          //its solid
                            air_mat,           //its material
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
    G4double radiator_locZ = 10 * cm;
    G4double radiator_sizeXY = 10 * cm;
    G4double radiator_sizeZ = 1 * cm;
    

    // Building the radiator plane
    G4Box *solidRadiator = new G4Box("Radiator", radiator_sizeXY/2.0, radiator_sizeXY/2.0, radiator_sizeZ/2.0);


    // create logic radiator
    // Use the previously defined aerogel material
    G4LogicalVolume* logicRadiator = new G4LogicalVolume(solidRadiator, aerogel_mat, "LogicRadiator");

    // create physical radiator
    new G4PVPlacement(0,                       //no rotation
                G4ThreeVector(0, 0,radiator_locZ),                    //at position
                logicRadiator,             //its logical volume
                "logicRadiator",                //its name
                logicWorld,                //its mother  volume
                false,                   //no boolean operation
                0,                       //copy number
                checkOverlaps);          //overlaps checking
    


    // Build the detector plane

    // Define as a square of width 60 cm and thickness 1nm (no need to be thick)
    // Detector is placed 20 cm away from the radiator exit surface
    G4double detector_sizeXY = 60 * cm;
    G4double detector_sizeZ  = 1 * cm;
    G4double detector_locZ = 27.70926 * cm;   // Typical of values observed at NRC
    G4ThreeVector pos_detect = G4ThreeVector(0, 0, detector_locZ + radiator_locZ + 0.5*radiator_sizeZ );

    
    // Define our CCD Detector
    // Calling it a SIPM... actually a CCD...
    HLX_CCDDetector* detector = new HLX_CCDDetector("SIPMs");

    G4Box* solidDetector =    
    new G4Box("Detector",                       //its name
        0.5*detector_sizeXY, 0.5*detector_sizeXY, detector_sizeZ);     //its size

    // Defining the detector matrial as a vacuum.
    // We're only tracking the first interaction, this shouldn't really matter
    G4LogicalVolume* logicDetector =                         
        new G4LogicalVolume(solidDetector,         //its solid
                            vacuum_mat,          //its material
                            "Detector");           //its name
    
    // Add the sensitive detector
    logicDetector->SetSensitiveDetector(detector);

    // // new G4LogicalSkinSurface("detector_skin", logicDetector, detectorSurface);
    new G4PVPlacement(0,                       //no rotation
                pos_detect,                    //at position
                logicDetector,             //its logical volume
                "Detector",                //its name
                logicWorld,                //its mother  volume
                false,                   //no boolean operation
                0,                       //copy number
                checkOverlaps);          //overlaps checking
    
    


    // Add a thin glass layer infront of CCD
    #ifdef REAL_CCD
        // Detector is placed 20 cm away from the radiator exit surface
        G4double glass_sizeXY = 60 * cm;
        G4double glass_sizeZ  = 0.7 * mm;   // Glass is 0.7 mm thik
        G4double glass_gap = 0.82 * mm;   // distance between the glass and the CCD chip
        G4ThreeVector pos_glass = G4ThreeVector(0, 0, (detector_locZ + radiator_locZ) - (glass_gap + 0.5*glass_sizeZ) );

        // Create physical object
        G4Box* solidGlass =    
                new G4Box("Glass",//its name
                          0.5*glass_sizeXY, 0.5*glass_sizeXY, 0.5*glass_sizeZ);
        // create logic radiator
        G4LogicalVolume* logicGlass = new G4LogicalVolume(solidGlass, glass_mat, "logicGlass");

        // create physical radiator
        new G4PVPlacement(0,                       //no rotation
                    pos_glass,                    //at position
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