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
    G4double isoMinWL = 400;
    G4double isoMaxWL = 700;
    G4double energyRange[] = {1239.84193*eV/isoMaxWL, 1239.84193*eV/isoMinWL};

    
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    // Define some materials
    // Start off with a vacuum
    G4double atomicNumber = 1.;
    G4double massOfMole = 1.008*g/mole;
    G4double density = 1.e-25*g/cm3;
    G4double temperature = 2.73*kelvin;
    G4double pressure = 3.e-18*pascal;
    
    G4Material* vacuumMaterial =
        new G4Material("interGalactic", atomicNumber,
        massOfMole, density, kStateGas,
        temperature, pressure);

    // create refractive index for vacuum
    G4MaterialPropertiesTable *vacuumMPT = new G4MaterialPropertiesTable();
    G4double vacuumN[] = {1.00, 1.00};
    vacuumMPT->AddProperty("RINDEX", energyRange, vacuumN, 2);
    vacuumMaterial->SetMaterialPropertiesTable(vacuumMPT);

    
    // Define air and set the refractive index
    G4Material* airMaterial = nist->FindOrBuildMaterial("G4_AIR");
    G4MaterialPropertiesTable *airMPT = new G4MaterialPropertiesTable();
    G4double air_n[] = {1.0003, 1.0003};
    airMPT->AddProperty("RINDEX", energyRange, air_n, 2);
    airMaterial->SetMaterialPropertiesTable(airMPT);


    G4Element* elH  = new G4Element("Hydrogen", "H", 1., 1.01*g/mole);
    G4Element* elO  = new G4Element("Oxygen", "O", 8., 16.00*g/mole);
    G4Element* elSi  = new G4Element("Silicon", "Si", 14., 28.1*g/mole);

    // Define Aerogel
    G4int nComponent = 3;
    // "Non interacting aerogel"
    // This is effectively a vacuum the refractive index of aerogel
    // G4Material *aerogelMaterial = new G4Material("Aerogel", 1.e-25*g/cm3, nComponent,
    //                                         kStateGas, 0.1*kelvin,  1.e-19*pascal);

    //G4Material *aerogelMaterial = new G4Material("Aerogel",  1.0,  1.e-8*g/mole,  1.e-25*g/cm3,
    // 				      kStateGas, 0.1*kelvin,  1.e-19*pascal);


    //See Tabata et al 2019 https://arxiv.org/abs/1901.06663
    // Reaslistic Aerogel
    // Room temperate, sea level preasure
    G4cout << "Aerogel Density: " << 0.53*g/cm3 << G4endl;
    G4Material *aerogelMaterial = new G4Material("Aerogel", 0.53*g/cm3, nComponent,
                                       kStateSolid, 293.*kelvin);
    G4double aerogelFixedIndex = 1.15;
    G4double aeroN[]  = {aerogelFixedIndex, aerogelFixedIndex};

    // G4double aerogelFixedAbsoLength = 0.16;
    G4double aerogelFixedAbsoLength = 100000.;
    G4double areogelAbso[] = {aerogelFixedAbsoLength, aerogelFixedAbsoLength};
    
    // add elemental composition of aerogel
    // aerogelMaterial->AddElement(elSi, 1);
    // aerogelMaterial->AddElement(elO, 4);
    // aerogelMaterial->AddElement(elH, 4);

    // G4Material *aerogelMaterial = new G4Material("Aerogel", 0.53*g/cm3, 2);

    G4cout << "Making Si02 \n"; 
    G4Material *SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2);
    SiO2->AddElement(nist->FindOrBuildElement("Si"), 1);
    SiO2->AddElement(nist->FindOrBuildElement("O"), 2);
    
    G4cout << "Making H20 \n"; 
    G4Material *H20 = new G4Material("H20", 1.00*g/cm3, 2);
    H20->AddElement(nist->FindOrBuildElement("H"), 1);
    H20->AddElement(nist->FindOrBuildElement("O"), 2);


    // // // add elemental composition of aerogel
    aerogelMaterial->AddMaterial(SiO2, 0.24);
    aerogelMaterial->AddMaterial(nist->FindOrBuildMaterial("G4_AIR"), 0.75);
    aerogelMaterial->AddMaterial(H20, 0.01);

    // // create aerogel material properties
    G4MaterialPropertiesTable *aero_MPT = new G4MaterialPropertiesTable();
    aero_MPT->AddProperty("RINDEX",  energyRange, aeroN, 2);    
   
    aerogelMaterial->SetMaterialPropertiesTable(aero_MPT);



    // Define glass
    G4Material* glassMaterial =  nist->FindOrBuildMaterial("G4_GLASS_PLATE");
    double glassFixedIndex = 1.5;
    double glassN[]  = {glassFixedIndex, glassFixedIndex};
    G4MaterialPropertiesTable *glassMPT = new G4MaterialPropertiesTable();
    glassMPT->AddProperty("RINDEX",  energyRange, glassN, 2);
    glassMaterial->SetMaterialPropertiesTable(glassMPT);


    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    // Construct the world
    // World volume 
    // Define the word as a 1m X 1m x 2m cube
    // Will be more convient to work in cm so 100 cm X 100 cm X 200 cm
    G4double worldSizeXY = 100 * cm;
    G4double worldZizeZ  = 200 * cm;

    G4Box* solidWorld =    
        new G4Box("World",                       //its name
            0.5*worldSizeXY, 0.5*worldSizeXY, 0.5*worldZizeZ);     //its size
        
    // Define a logical volume filled with air
    G4LogicalVolume* logicWorld =                         
        new G4LogicalVolume(solidWorld,          //its solid
                            airMaterial,           //its material
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
    

    // Building the radiator plane
    G4Box *solidRadiator = new G4Box("Radiator", radiatorSizeXY/2.0, radiatorSizeXY/2.0, radiatorSizeZ/2.0);


    // create logic radiator
    // Use the previously defined aerogel material
    G4LogicalVolume* logicRadiator = new G4LogicalVolume(solidRadiator, aerogelMaterial, "LogicRadiator");

    // create physical radiator
    new G4PVPlacement(0,                       //no rotation
                G4ThreeVector(0, 0,radiatorLocz),                    //at position
                logicRadiator,             //its logical volume
                "logicRadiator",                //its name
                logicWorld,                //its mother  volume
                false,                   //no boolean operation
                0,                       //copy number
                checkOverlaps);          //overlaps checking
    


    // Build the detector plane

    // Define as a square of width 60 cm and thickness 1nm (no need to be thick)
    // Detector is placed 20 cm away from the radiator exit surface
    G4double detectorSizeXY = 60 * cm;
    G4double detectorSizeZ  = 1 * cm;
    G4double detectorLocZ = 27.70926 * cm;   // Typical of values observed at NRC
    G4ThreeVector positionDetector = G4ThreeVector(0, 0, detectorLocZ + radiatorLocz + 0.5*radiatorSizeZ );

    
    // Define our CCD Detector
    // Calling it a SIPM... actually a CCD...
    HLX_CCDDetector* detector = new HLX_CCDDetector("SIPMs");

    G4Box* solidDetector =    
    new G4Box("Detector",                       //its name
        0.5*detectorSizeXY, 0.5*detectorSizeXY, detectorSizeZ);     //its size

    // Defining the detector matrial as a vacuum.
    // We're only tracking the first interaction, this shouldn't really matter
    G4LogicalVolume* logicDetector =                         
        new G4LogicalVolume(solidDetector,         //its solid
                            vacuumMaterial,          //its material
                            "Detector");           //its name
    
    // Add the sensitive detector
    logicDetector->SetSensitiveDetector(detector);

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
        G4LogicalVolume* logicGlass = new G4LogicalVolume(solidGlass, glassMaterial, "logicGlass");

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