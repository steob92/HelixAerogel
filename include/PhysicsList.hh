#ifndef _PHYSICS_LIST
#define _PHYSICS_LIST

#include "G4VUserPhysicsList.hh"
#include "G4Cerenkov.hh"
#include "G4OpBoundaryProcess.hh"

class PhysicsList : public G4VUserPhysicsList {

public:
    PhysicsList();

    // necessary methods
    virtual void ConstructParticle();
    virtual void ConstructProcess();
    virtual void SetCuts();

    // construct physics processes
    void ConstructOp();
    void ConstructEM();

private:
    G4Cerenkov* cerenkovProcess;
    G4OpBoundaryProcess* boundaryProcess;
};


#endif
