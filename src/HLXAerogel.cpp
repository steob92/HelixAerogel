#include "HLXAerogel.hh"

using namespace CLHEP;


HLXAerogel::HLXAerogel()
{
    
    LoadSurface("data/surface.dat");
    LoadThickness("data/thickness.dat");
    fMaterials = new HLXMaterials();
    fMaterials->DefineMaterials();

}


HLXAerogel::~HLXAerogel()
{
    if (fMaterials) {delete fMaterials;}
}


void HLXAerogel::LoadSurface(std::string filename)
{
    std::ifstream fileIn(filename.c_str());
    float a;
    int i = 0;
    while (fileIn >> a)
    {
        fSurfaceParms[i] = a;
        i++;
    }
}



void HLXAerogel::LoadThickness(std::string filename)
{
    

    std::ifstream fileIn(filename.c_str());
    float a;
    int i = 0;
    while (fileIn >> a)
    {
        fThicknessParms[i] = a;
        i++;
    }
}


float HLXAerogel::GetSurface(float x, float y)
{
    // Because centre is defined as 0,0 npt 45,45
    x -= 45;
    y -=45;
    float z = fSurfaceParms[0] + fSurfaceParms[1]*x + fSurfaceParms[2]*x*x + fSurfaceParms[3]*y + fSurfaceParms[4]*y*y + fSurfaceParms[5]*x*y +fSurfaceParms[6]*x*x*y + fSurfaceParms[7]*x*y*y +fSurfaceParms[8]*x*x*y*y;
    return z;
}



float HLXAerogel::GetThickness(float x, float y)
{
    // Because centre is defined as 0,0 npt 45,45
    float z = fThicknessParms[0] + fThicknessParms[1]*x + fThicknessParms[2]*x*x + fThicknessParms[3]*y + fThicknessParms[4]*y*y + fThicknessParms[5]*x*y +fThicknessParms[6]*x*x*y + fThicknessParms[7]*x*y*y +fThicknessParms[8]*x*x*y*y;
    return z;
}


// void HLXAerogel::MakeAerogel(G4LogicalVolume *logicRadiator, G4LogicalVolume *logicWorld, G4double radiatorLocz, G4double radiatorSizeXY, G4double radiatorSizeZ)
void HLXAerogel::MakeAerogel( G4LogicalVolume *logicWorld, G4double radiatorLocz, G4double radiatorSizeXY, G4double radiatorSizeZ)
{
    
    // G4bool checkOverlaps = true;
    G4bool checkOverlaps = false;
    G4double blockSizeXY = 1*mm;
    float blockSizeXYf = 1;
    G4double blockSizeZ= 0.1*mm;
    float blockSizeZf = 0.1;

    G4int nRadiatorXY = radiatorSizeXY / blockSizeXY;
    G4int nRadiatorZ = radiatorSizeZ / blockSizeZ;


    float thickness = 0;
    float surface = 0;
    float dist = 0;

    for (int i = 0; i < nRadiatorXY; i++)
    {
        for (int j = 0; j < nRadiatorXY; j++)
        {


            surface = GetSurface((i)*blockSizeXYf, (j)*blockSizeXYf);
            thickness = GetThickness((i)*blockSizeXYf, (j)*blockSizeXYf);

            G4Box *solidRadiator = new G4Box("Radiator", blockSizeXY/2.0, blockSizeXY/2.0, thickness/2.0);
            // G4Box *solidRadiator = new G4Box("Radiator", radiatorSizeXY/2.0, radiatorSizeXY/2.0, radiatorSizeZ/2.0);
            G4LogicalVolume* logicRadiator = new G4LogicalVolume(solidRadiator, fMaterials->GetMaterial("aerogel"), "LogicRadiator");

            G4PVPlacement* physRadiator = new G4PVPlacement(0,                  //no rotation
                        G4ThreeVector(
                            -5*cm + (i)*blockSizeXY, 
                            -5*cm + (j)*blockSizeXY, 
                            radiatorLocz + surface - 0.5 * thickness
                            ),                       //at position
                        logicRadiator,                                          //its logical volume
                        "logicRadiator",                                        //its name
                        logicWorld,                                             //its mother  volume
                        false,                                                  //no boolean operation
                        100*j + 10000 * i,                                                      //copy number
                        checkOverlaps);     

            // for (int k = 0; k < nRadiatorZ; k++)
            // {


            //     surface = GetSurface((i)*blockSizeXYf, (j)*blockSizeXYf);
            //     thickness = GetThickness((i)*blockSizeXYf, (j)*blockSizeXYf);

            //     dist = surface - (k)*blockSizeZf;
            //     if ((dist < thickness) && (dist >= 0))
            //     {

            //         // create physical radiator
            //         G4PVPlacement* physRadiator = new G4PVPlacement(0,                  //no rotation
            //                     G4ThreeVector(-5*cm + (i)*blockSizeXY, -5*cm + (j)*blockSizeXY, radiatorLocz -0.6*cm + (k)*blockSizeZ),                       //at position
            //                     logicRadiator,                                          //its logical volume
            //                     "logicRadiator",                                        //its name
            //                     logicWorld,                                             //its mother  volume
            //                     false,                                                  //no boolean operation
            //                     k + 100*j + 10000 * i,                                                      //copy number
            //                     checkOverlaps);                                         //overlaps checking
            //     }
            // }
        }
        
    }

}