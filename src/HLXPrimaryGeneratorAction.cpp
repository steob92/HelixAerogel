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
  fBeamEnergy(false),
  //  <2mm C. K. Ross et. al (2008) 
  fsigmaBeamX(2.* mm / (2 * std::sqrt(2 * std::log(2)))), // Converting FWHM to Gaussian width
  fsigmaBeamY(2.* mm / (2 * std::sqrt(2 * std::log(2)))), // Converting FWHM to Gaussian width
  // fThetaDiv(1.00 * CLHEP::pi / 180.),  // default to 0.25 degrees
  fMomentum(35 *MeV), // Particle momentum default to 35 MeV
  fSigmaMomentum(0*MeV), // Particle momentum width
  fSigmaAngle( 2.0 * deg)  // Divergence
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

  // Turing on beam divergence
  #ifdef DIVERGENCE
    fDivergence = true;
  #endif
  // Setting Gaussian beam profile
  #ifdef GAUSSIAN
    fBeamType = "gaussian";
  #endif
  // Variable energy
  #ifdef VARENERGY
    fBeamEnergy = true; 
  #endif

  DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HLXPrimaryGeneratorAction::~HLXPrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fMessenger;
  // delete fEnvelopeBox;
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


  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;

  // Only consider electrons
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="e-");
  

  fParticleGun->SetParticleDefinition(particle);

  // Get and Set the particle's momentum and energy
  auto pp = fMomentum + (G4UniformRand()-0.5)*fSigmaMomentum;
  auto mass = particle->GetPDGMass();
  auto ekin = std::sqrt(pp*pp+mass*mass)-mass;
  fParticleGun->SetParticleEnergy(ekin);



  auto phi = (G4UniformRand()-0.5)*fSigmaAngle; // Beam opening angle (divergence)
  auto theta = 2*CLHEP::pi * G4UniformRand() ; // 0-360 degrees azimutal angle

  // // Spherical to Cartesian coordinates
  // px0 = std::sin(phi) * std::cos(theta);
  // py0 = std::sin(phi) * std::sin(theta);
  // pz0 = std::cos(phi);
  fParticleGun->SetParticleMomentumDirection(
                  G4ThreeVector(
                    std::sin(phi) * std::cos(theta),
                    std::sin(phi) * std::sin(theta),
                    std::cos(phi)));


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

  fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

  fParticleGun->GeneratePrimaryVertex(anEvent);

}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HLXPrimaryGeneratorAction::DefineCommands()
{
  // Define /B5/generator command directory using generic messenger class
  fMessenger
    = new G4GenericMessenger(this,
                             "/HLX/generator/",
                             "Primary generator control");

  // momentum command
  auto& momentumCmd
    = fMessenger->DeclarePropertyWithUnit("momentum", "MeV", fMomentum,
        "Mean momentum of primaries.");
  momentumCmd.SetParameterName("p", true);
  momentumCmd.SetRange("p>=0.");
  momentumCmd.SetDefaultValue("35.");
  // ok
  //momentumCmd.SetParameterName("p", true);
  //momentumCmd.SetRange("p>=0.");

  // sigmaMomentum command
  auto& sigmaMomentumCmd
    = fMessenger->DeclarePropertyWithUnit("sigmaMomentum",
        "MeV", fSigmaMomentum, "Sigma momentum of primaries.");
  sigmaMomentumCmd.SetParameterName("sp", true);
  sigmaMomentumCmd.SetRange("sp>=0.");
  sigmaMomentumCmd.SetDefaultValue("0.");

  // sigmaAngle command
  auto& sigmaAngleCmd
    = fMessenger->DeclarePropertyWithUnit("sigmaAngle", "deg", fSigmaAngle,
        "Sigma angle divergence of primaries.");
  sigmaAngleCmd.SetParameterName("t", true);
  sigmaAngleCmd.SetRange("t>=0.");
  sigmaAngleCmd.SetDefaultValue("0");

}

