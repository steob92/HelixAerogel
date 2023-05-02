#include "HLXMaterials.hh"

using namespace CLHEP;


HLXMaterials::HLXMaterials()
{
    fIsoMinWL = 400;
    fIsoMaxWL = 700;
    fNPoints = 100;
    fEnergyRange = new G4double[fNPoints];
    fEnergyRangeMin = 1239.84193/fIsoMaxWL;
    fEnergyRangeMax = 1239.84193/fIsoMinWL;
    
    fDeltaEnergy = (fEnergyRangeMax - fEnergyRangeMin) / fNPoints;
    for (int i = 0; i < fNPoints; i++)
    {
        fEnergyRange[i] = (fEnergyRangeMin + i*fDeltaEnergy)*eV;

        G4cout << i << " " << fEnergyRange[i] << G4endl;
    }
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
    G4double *vacuumN = new G4double[fNPoints];
    for (int i = 0; i < fNPoints; i++)
    {
        vacuumN[i] = vacuumFixedIndex;
    }
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
    G4double *airN = new G4double[fNPoints];
    for (int i = 0; i < fNPoints; i++)
    {
        airN[i] = airFixedIndex;
    }    
    airMPT->AddProperty("RINDEX", fEnergyRange, airN, fNPoints);
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
    G4Material *aerogelMaterial = new G4Material("Aerogel", 0.53*g/cm3, nComponent,
                                        kStateGas, 293.*kelvin,  101325.*pascal);

    double aerogelFixedIndex = 1.15;                                                                                                                                                     
    G4double *aeroN = new G4double[fNPoints];
    G4double *aeroAbsEnergyRange = new G4double[fNPoints];
    G4double *aeroAbsLength = new G4double[fNPoints];


    double a = 0.982;
    double c = 0.007;
    double t = 0;


    // Sellmeier formula
    double a0 = 0.3175;
    double lam0 = 50.5;
    double lam = 0;

    // double delEnergy =(fEnergyRange[1] - fEnergyRange[0]) / 100;
    G4cout << "Refractive Index:" << G4endl;
    for (int i = 0; i < fNPoints; i++)
    {
        // aeroN[i] = 1.1500620074151149;
        lam = 1239.84193 *eV /fEnergyRange[i];
        aeroN[i] = sqrt(1. + (a0 * pow(lam,2)) / (pow(lam,2) - pow(lam0,2)));
        // Higher order...
        // aeroN[i] += (a1 * lam^2) / (lam^2 - lam1^2);
        
        // Get the absorption length
        // aeroAbsEnergyRange[i] = fEnergyRange[i];
        // t = a * std::exp( -c * 10 / std::pow(1239.84193*eV / aeroAbsEnergyRange[i] * 1e-3, 4));
        // aeroAbsLength[i] = -1/(std::log(t));
        // aeroAbsLength[i] = 0;
        

        // G4cout << i << " " << fEnergyRange[i]  << " " << lam << " " << aeroN[i] << " " << aeroAbsLength[i] << G4endl;

    }    

    // add elemental composition of aerogel                                                                                                                                                                          
    aerogelMaterial->AddElement(el_Si, 1);
    aerogelMaterial->AddElement(el_O, 4);
    aerogelMaterial->AddElement(el_H, 4);

    // // create aerogel material properties                                                                                                                                                                         
    G4MaterialPropertiesTable *aeroMPT = new G4MaterialPropertiesTable();
    aeroMPT->AddProperty("RINDEX",  fEnergyRange, aeroN, fNPoints);
    // aeroMPT->AddProperty("ABSLENGTH",  aeroAbsEnergyRange, aeroAbsLength, 100);
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
    glassMPT->AddProperty("RINDEX",  fEnergyRange, glassN, fNPoints);
    glassMaterial->SetMaterialPropertiesTable(glassMPT);

    fMaterials.push_back(glassMaterial);
    fMaterialNames.push_back("glass");
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    // delete aeroN;
    // delete airN;
    // delete vacuumN;
    

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