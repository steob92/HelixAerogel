#include "PhysicsList.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4EmSaturation.hh"
#include "G4LossTableManager.hh"
#include "G4ComptonScattering.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4GammaConversion.hh"
#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4Scintillation.hh"
//#include "G4eplusAnnihilation"

PhysicsList::PhysicsList() : G4VUserPhysicsList() 
{
}


void PhysicsList::ConstructParticle() 
{

    // all particles used in the simulation must be constructed here
    G4BosonConstructor bConstructor;
    bConstructor.ConstructParticle();

    G4LeptonConstructor lConstructor;
    lConstructor.ConstructParticle();
    G4IonConstructor iConstructor;
    iConstructor.ConstructParticle();

    #ifdef ALL_PHYSICS
        G4cout << "COMPILING WITH ALL PHYSICS!!!" << G4endl;
        G4Geantino::GeantinoDefinition();
        G4Gamma::GammaDefinition();
        G4OpticalPhoton::OpticalPhotonDefinition();

        G4Positron::PositronDefinition();
        G4Electron::ElectronDefinition();

        G4Proton::ProtonDefinition();
        G4AntiProton::AntiProtonDefinition();

        G4Deuteron::DeuteronDefinition();
        G4Triton::TritonDefinition();
        G4He3::He3Definition();
        G4Alpha::AlphaDefinition();
    #endif
    G4GenericIon::GenericIonDefinition();
}

void PhysicsList::ConstructProcess() {
    AddTransportation();
    ConstructEM();
    ConstructOp();
}

void PhysicsList::ConstructOp() {

    // create Cerenkov
    cerenkovProcess = new G4Cerenkov("Cerenkov");
    cerenkovProcess->SetMaxNumPhotonsPerStep(50);
    cerenkovProcess->SetMaxBetaChangePerStep(1);
    cerenkovProcess->SetTrackSecondariesFirst(true);

    // create boundary process to be applied to opticalphotons
    boundaryProcess = new G4OpBoundaryProcess();

    #ifdef ALL_PHYSICS
        // create scinitillation
        G4Scintillation *scintProcess = new G4Scintillation("Scintilation");
        scintProcess->SetScintillationYieldFactor(1.0);
        scintProcess->SetTrackSecondariesFirst(true);

        // create boundary process
        G4OpAbsorption *absProcess = new G4OpAbsorption();
        G4OpRayleigh *rayProcess = new G4OpRayleigh();
    #endif

    GetParticleIterator()->reset();

    while( (*GetParticleIterator())() ) 
    {
        G4ParticleDefinition* particle = GetParticleIterator()->value();
        G4ProcessManager* pManager = particle->GetProcessManager();
        G4String particleName = particle->GetParticleName();

        if (cerenkovProcess->IsApplicable(*particle) || particleName == "GenericIon") 
        {
            pManager->AddProcess(cerenkovProcess);
            pManager->SetProcessOrdering(cerenkovProcess, idxPostStep);
        }
        if (particleName == "opticalphoton") 
        {
            pManager->AddDiscreteProcess(boundaryProcess);
            #ifdef ALL_PHYSICS
                pManager->AddDiscreteProcess(absProcess);
                pManager->AddDiscreteProcess(rayProcess);
                pManager->AddProcess(new G4ComptonScattering());
                pManager->AddProcess(new G4hMultipleScattering());
            #endif
        }
    }
}

void PhysicsList::ConstructEM() {

    // create EM
    GetParticleIterator()->reset();
    while( (*GetParticleIterator())() )
    {
        G4ParticleDefinition* particle = GetParticleIterator()->value();
        G4ProcessManager* pManager = particle->GetProcessManager();
        G4String particleName = particle->GetParticleName();

        if (particleName == "gamma") 
        {
            // gamma
            // Construct processes for gamma
            #ifdef ALL_PHYSICS
                pManager->AddDiscreteProcess(new G4GammaConversion());
                pManager->AddDiscreteProcess(new G4ComptonScattering());
                pManager->AddDiscreteProcess(new G4PhotoElectricEffect());
            #endif

        } 
        
        else if (particleName == "e-") 
        {
            //electron
            // Construct processes for electron
            #ifdef ALL_PHYSICS
                pManager->AddProcess(new G4eMultipleScattering(),-1, 1, 1);
                pManager->AddProcess(new G4eIonisation(),       -1, 2, 2);
                pManager->AddProcess(new G4eBremsstrahlung(),   -1, -1, 3);
            #endif
        } 
        
        else if (particleName == "e+") 
        {
            // Construct processes for positron
            #ifdef ALL_PHYSICS
                pManager->AddProcess(new G4eMultipleScattering(),-1, 1, 1);
                pManager->AddProcess(new G4eIonisation(),       -1, 2, 2);
                pManager->AddProcess(new G4eBremsstrahlung(),   -1, -1, 3);
                //pManager->AddProcess(new G4eplusAnnihilation(),   0, -1, 4);
            #endif

        } 
        
        else if (particleName == "proton" || particleName == "anti_proton") 
        {
            // Construct processes for proton/anti-proton
            #ifdef ALL_PHYSICS
                pManager->AddProcess(new G4hMultipleScattering(),-1, 1, 1);
                pManager->AddProcess(new G4hIonisation(),       -1, 2, 2);
            #endif
        }

        else if ( (particleName == "alpha") && particle->GetPDGCharge() != 0.0 )
        {
            #ifdef ALL_PHYSICS
                pManager->AddProcess(new G4ionIonisation(), -1, 2, 2);
                pManager->AddProcess(new G4hMultipleScattering(),-1, 1, 1);
            #endif
        }
        else if ( (particleName == "GenericIon") &&particle->GetPDGCharge() != 0.0) 
        {
            pManager->AddProcess(new G4ionIonisation(), -1, 2, 2);
        }

        else 
        {
            if ((particle->GetPDGCharge() != 0.0) && (particle->GetParticleName() != "chargedgeantino") && !particle->IsShortLived()) 
            {
                // all others charged particles except geantino
                #ifdef ALL_PHYSICS
                    pManager->AddProcess(new G4hMultipleScattering(),-1,1,1);
                    pManager->AddProcess(new G4hIonisation(),       -1,2,2);
                #endif 
            }
        }
    }
}

void PhysicsList::SetCuts() 
{
  SetCutsWithDefault();
}
