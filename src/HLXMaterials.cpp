#include "HLXMaterials.hh"

using namespace CLHEP;


HLXMaterials::HLXMaterials()
{
    fIsoMinWL = 400;
    fIsoMaxWL = 700;
    fEnergyRange = new G4double[2];
    fEnergyRange[0] = 1239.84193*eV/fIsoMaxWL;
    fEnergyRange[1] = 1239.84193*eV/fIsoMinWL;
    fNist = G4NistManager::Instance();
}


// Delete and clear matrials
HLXMaterials::~HLXMaterials()
{
    fMaterialNames.clear();
    
    for (auto p : fMaterials)
    {
        delete p;
    }
    fMaterials.clear();

    delete fEnergyRange, fNist;

}


void HLXMaterials::PrintMaterials()
{
    G4cout << "Printing Materials" << G4endl;

    for (unsigned int i = 0 ; i < fMaterialNames.size(); i ++)
    {
        G4cout << "\t\t" << fMaterialNames[i] << G4endl;
        G4MaterialPropertiesTable* mpt = fMaterials[i]->GetMaterialPropertiesTable();
        mpt->DumpTable();
    }
}

void HLXMaterials::DefineMaterials()
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    // Define Vacuum
    G4double atomicNumber = 1.;
    G4double massOfMole = 1.008*g/mole;
    G4double density = 1.e-25*g/cm3;
    G4double temperature = 2.73*kelvin;
    G4double pressure = 3.e-18*pascal;

    G4Material* vacuumMaterial = new G4Material(
        "interGalactic", atomicNumber,
        massOfMole, density, kStateGas,
        temperature, pressure
        );

    // create refractive index for vacuum
    G4MaterialPropertiesTable *vacuumMPT = new G4MaterialPropertiesTable();
    double vacuumFixedIndex = 1.;
    G4double vacuumN[] = {vacuumFixedIndex, vacuumFixedIndex};
    vacuumMPT->AddProperty("RINDEX", fEnergyRange, vacuumN, 2);
    vacuumMaterial->SetMaterialPropertiesTable(vacuumMPT);

    fMaterials.push_back(vacuumMaterial);
    fMaterialNames.push_back("vacuum");
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    // Define Air
    G4Material* airMaterial = fNist->FindOrBuildMaterial("G4_AIR");
    G4MaterialPropertiesTable *airMPT = new G4MaterialPropertiesTable();
    double airFixedIndex = 1.003;
    G4double airN[] = {airFixedIndex, airFixedIndex};
    airMPT->AddProperty("RINDEX", fEnergyRange, airN, 2);
    airMaterial->SetMaterialPropertiesTable(airMPT);

    fMaterials.push_back(airMaterial);
    fMaterialNames.push_back("air");
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    // Define Aerogel
    G4Element* el_H  = new G4Element("Hydrogen", "H", 1., 1.01*g/mole);
    G4Element* el_O  = new G4Element("Oxygen", "O", 8., 16.00*g/mole);
    G4Element* el_Si  = new G4Element("Silicon", "Si", 14., 28.1*g/mole);
                                                                                                                                
    G4int nComponent = 3;

    // Room temperate, sea level preasure                                                                                                                                                                            
    G4Material *aerogelMaterial = new G4Material("Aerogel", 0.531*g/cm3, nComponent,
                                        kStateGas, 293.*kelvin,  101325.*pascal);

    // //See Buzykaev et al NIM A433 (1999) 396       
    double aerogelFixedIndex = 1.15;                                                                                                                                                     
    double aero_n[]  = {aerogelFixedIndex, aerogelFixedIndex};

    // add elemental composition of aerogel                                                                                                                                                                          
    aerogelMaterial->AddElement(el_Si, 1);
    aerogelMaterial->AddElement(el_O, 4);
    aerogelMaterial->AddElement(el_H, 4);

    // // create aerogel material properties                                                                                                                                                                         
    G4MaterialPropertiesTable *aeroMPT = new G4MaterialPropertiesTable();
    aeroMPT->AddProperty("RINDEX",  fEnergyRange, aero_n, 2);
    aerogelMaterial->SetMaterialPropertiesTable(aeroMPT);

    fMaterials.push_back(aerogelMaterial);
    fMaterialNames.push_back("aerogel");
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    // Define glass
    G4Material* glassMaterial =  fNist->FindOrBuildMaterial("G4_GLASS_PLATE");
    double glassFixedIndex = 1.5;
    double glassN[]  = {glassFixedIndex, glassFixedIndex};
    G4MaterialPropertiesTable *glassMPT = new G4MaterialPropertiesTable();
    glassMPT->AddProperty("RINDEX",  fEnergyRange, glassN, 2);
    glassMaterial->SetMaterialPropertiesTable(glassMPT);

    fMaterials.push_back(glassMaterial);
    fMaterialNames.push_back("glass");
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

G4Material* HLXMaterials::GetMaterial(std::string matName)
{
    for (unsigned int i = 0; i < fMaterialNames.size(); i++)
    {
        if (matName == fMaterialNames[i])
        {
            return fMaterials[i];
        }
    }

    G4cout << "Material not found! " << matName << G4endl;
    return 0;
}