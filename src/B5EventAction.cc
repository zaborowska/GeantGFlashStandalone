//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B5EventAction.cc 87359 2014-12-01 16:04:27Z gcosmo $
//
/// \file B5EventAction.cc
/// \brief Implementation of the B5EventAction class

#include "B5EventAction.hh"
#include "B5HadCalorimeterHit.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"
// Data model
#include "datamodel/SimCaloHit.h"
#include "datamodel/SimCaloHitCollection.h"
#include "datamodel/SimCaloCluster.h"
#include "datamodel/SimCaloClusterCollection.h"
#include "datamodel/SimCaloClusterHitAssociation.h"
#include "datamodel/SimCaloClusterHitAssociationCollection.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5EventAction::B5EventAction()
: G4UserEventAction(),
  fHHC1ID(-1),
  fHHC2ID(-1),
  fDHC1ID(-1),
  fDHC2ID(-1),
  fECHCID(-1),
  fHCHCID(-1),
  fCellNo(1),
  fOutputName("Geant.root")
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);
  fHadCalEdep.resize(fCellNo*fCellNo*fCellNo, 0.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5EventAction::B5EventAction(G4int aCellNo)
: G4UserEventAction(),
  fHHC1ID(-1),
  fHHC2ID(-1),
  fDHC1ID(-1),
  fDHC2ID(-1),
  fECHCID(-1),
  fHCHCID(-1),
  fCellNo(aCellNo),
  fOutputName("Geant.root")
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);
  fHadCalEdep.resize(fCellNo*fCellNo*fCellNo, 0.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5EventAction::B5EventAction(G4int aCellNo, G4String aOut)
: G4UserEventAction(),
  fHHC1ID(-1),
  fHHC2ID(-1),
  fDHC1ID(-1),
  fDHC2ID(-1),
  fECHCID(-1),
  fHCHCID(-1),
  fCellNo(aCellNo),
  fOutputName(aOut)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);
  fHadCalEdep.resize(fCellNo*fCellNo*fCellNo, 0.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5EventAction::~B5EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5EventAction::BeginOfEventAction(const G4Event*)
{
    if (fHHC1ID==-1) {
      G4SDManager* sdManager = G4SDManager::GetSDMpointer();
      fHCHCID = sdManager->GetCollectionID("HadCalorimeter/HadCalorimeterColl");
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5EventAction::EndOfEventAction(const G4Event* event)
{
    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    if (!hce)
    {
        G4ExceptionDescription msg;
        msg << "No hits collection of this event found." << G4endl;
        G4Exception("B5EventAction::EndOfEventAction()",
                    "B5Code001", JustWarning, msg);
        return;
    }


    // Get hits collections

    B5HadCalorimeterHitsCollection* hcHC
         = static_cast<B5HadCalorimeterHitsCollection*>(hce->GetHC(fHCHCID));


    if ( (!hcHC) )
    {
        G4ExceptionDescription msg;
        msg << "Some of hits collections of this event not found." << G4endl;
        G4Exception("B5EventAction::EndOfEventAction()",
                    "B5Code001", JustWarning, msg);
        return;
    }

    //
    // Fill histograms & ntuple
    //
    auto store = podio::EventStore();
    podio::ROOTWriter writer(fOutputName, &store);
    auto& hitscoll = store.create<fcc::SimCaloHitCollection>("hits");
    writer.registerForWrite<fcc::SimCaloHitCollection>("hits");
    auto& clusterscoll = store.create<fcc::SimCaloClusterCollection>("clusters");
    writer.registerForWrite<fcc::SimCaloClusterCollection>("clusters");
    auto& hitsclusterscoll = store.create<fcc::SimCaloClusterHitAssociationCollection>("hitsclusters");
    writer.registerForWrite<fcc::SimCaloClusterHitAssociationCollection>("hitsclusters");

    // // Get analysis manager
    // G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    // Fill ntuple
    // HCEnergy
    G4int totalHadHit = 0;
    G4double totalHadE = 0.;
    for (G4int i=0;i<fCellNo*fCellNo*fCellNo;i++)
    {
      B5HadCalorimeterHit* hit = (*hcHC)[i];
      G4double eDep = hit->GetEdep();
      if (eDep>0.)
      {
        totalHadHit++;
        totalHadE += eDep;
      }
      fHadCalEdep[i] = eDep;
      if(hit->GetXid() != -1 && hit->GetYid() != -1 && hit->GetZid() != -1) {
        //hit->Print();
        auto edmhit = fcc::SimCaloHit();
        edmhit.Core().Energy = eDep/GeV;
        edmhit.Core().Cellid = fCellNo*fCellNo*hit->GetXid()+fCellNo*hit->GetYid()+hit->GetZid();
        //G4cout<<"Storing hit "<<eDep<<" "<<fCellNo*fCellNo*hit->GetXid()+fCellNo*hit->GetYid()+hit->GetZid()<<G4endl;
        auto edmcluster = fcc::SimCaloCluster();
        edmcluster.Core().Energy = eDep/GeV;
        auto& pos = edmcluster.Core().position;
        pos.X = hit->GetPos().x()/mm;
        pos.Y = hit->GetPos().y()/mm;
        pos.Z = hit->GetPos().z()/mm;
        auto edmhc = fcc::SimCaloClusterHitAssociation();
        edmhc.Cluster(edmcluster);
        edmhc.Hit(edmhit);
        hitscoll.push_back(edmhit);
        clusterscoll.push_back(edmcluster);
        hitsclusterscoll.push_back(edmhc);
      }
    }
      writer.writeEvent();
      store.clearCollections();
    writer.finish();
    // analysisManager->FillNtupleDColumn(3, totalHadE);


    // analysisManager->AddNtupleRow();

    //
    // Print diagnostics
    //

    G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ( printModulo==0 || event->GetEventID() % printModulo != 0) return;

    G4PrimaryParticle* primary = event->GetPrimaryVertex(0)->GetPrimary(0);
    G4cout << G4endl
           << ">>> Event " << event->GetEventID() << " >>> Simulation truth : "
           << primary->GetG4code()->GetParticleName()
           << " " << primary->GetMomentum() << G4endl;

    // Had calorimeter
    G4cout << "Hadron Calorimeter has " << totalHadHit << " hits. Total Edep is "
           << totalHadE/MeV << " (MeV)" << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
