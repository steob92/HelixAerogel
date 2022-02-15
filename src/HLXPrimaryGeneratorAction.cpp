#include "HLXPrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Generating an electron beam of energy 35 MeV
HLXPrimaryGeneratorAction::HLXPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0), 
  fEnvelopeBox(0),
  fUniform(false),
  fDivergence(false),
  fBeamType("pencil"),
  fsigmaBeamX(0* mm / (2 * std::sqrt(2 * std::log(2)))), // Converting FWHM to Gaussian width
  fsigmaBeamY(0* mm / (2 * std::sqrt(2 * std::log(2)))), // Converting FWHM to Gaussian width
  fThetaDiv(1.00 * CLHEP::pi / 180.)  // default to 0.25 degrees
{

  CLHEP::HepRandom::setTheSeed((unsigned)clock());
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="e-");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  // Defining a mono-energtic beam
  fParticleGun->SetParticleEnergy(35.*MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HLXPrimaryGeneratorAction::~HLXPrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HLXPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of each event
  //

  // Initial particle location
  G4double x0 = 0;
  G4double y0 = 0;
  G4double z0 = 0;

  // Initial particle momentium
  G4double px0 = 0;
  G4double py0 = 0;
  G4double pz0 = 1;


  // Beam divergence
  G4double theta = 0;
  G4double phi = 0;
  fThetaDiv = 2.0 * CLHEP::pi / 180.;



  // Define a uniform beam of 5mm width
  G4double width = 5 * mm;
  if (fBeamType == "uniform")
  {
    // Uniform beam profile
    x0 = width * (G4UniformRand()-0.5);
    y0 = width * (G4UniformRand()-0.5);
  }
  else if (fBeamType == "gaussian")
  {
    // Gaussian beam profile
    x0 = G4RandGauss::shoot( 0., fsigmaBeamX);
    y0 = G4RandGauss::shoot( 0., fsigmaBeamY);
  
  }
  else{

    // Default to pencil beam
    x0 = 0;
    y0 = 0;

  }


  if (fDivergence)
  {
    // Uniformally distributed theta and pi
    // phi is the half opening angle of the beam divergence
    // theta = CLHEP::pi * G4UniformRand() - CLHEP::pi / 2. ; // 0-180 degrees
    theta = 0.5 * fThetaDiv * G4UniformRand(); // 0 - half Divergence
    phi = 0.5 * fThetaDiv * G4UniformRand(); // 0 - half Divergence

    px0=std::sin(theta)*std::cos(phi);
    py0=std::sin(theta)*std::sin(phi);
    pz0=std::cos(theta);
  }



  fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(px0, py0, pz0));
  fParticleGun->GeneratePrimaryVertex(anEvent);

}