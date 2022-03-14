#ifndef HLXRUNACTION_HH
#define HLXRUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4Run.hh"
//#include "g4root.hh"
#include "G4AnalysisManager.hh" 
#include "G4Threading.hh"
#include "globals.hh"


#include "TTree.h"
#include "TFile.h"


class HLXRunAction : public G4UserRunAction
{
public:
    HLXRunAction();
    ~HLXRunAction();
    
    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);


private:
    G4int fPhotID;
    G4int fEleID;

    TFile *fFile;
    TTree *fEleTree;
    TTree *fPhotTree;

    
};

#endif
