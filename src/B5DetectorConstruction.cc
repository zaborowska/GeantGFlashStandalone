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
// $Id: B5DetectorConstruction.cc 77656 2013-11-27 08:52:57Z gcosmo $
//
/// \file B5DetectorConstruction.cc
/// \brief Implementation of the B5DetectorConstruction class

#include "B5DetectorConstruction.hh"
#include "B5HadCalorimeterSD.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4UserLimits.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

//fast simulation
#include "GFlashHomoShowerParameterisation.hh"
#include "G4FastSimulationManager.hh"
#include "GFlashShowerModel.hh"
#include "GFlashHitMaker.hh"
#include "GFlashParticleBounds.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5DetectorConstruction::B5DetectorConstruction()
: G4VUserDetectorConstruction(),
  fMessenger(0),
  fHadCalScintiLogical(0),
  fVisAttributes(),
  fBoxSize(1*m),
  fCellSize(1*m),
  fInitialPos(0,0,0)
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5DetectorConstruction::B5DetectorConstruction(G4double aBox, G4double aCell, G4ThreeVector aPos0)
: G4VUserDetectorConstruction(),
  fMessenger(0),
  fHadCalScintiLogical(0),
  fVisAttributes(),
  fBoxSize(aBox),
  fCellSize(aCell),
  fInitialPos(aPos0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5DetectorConstruction::~B5DetectorConstruction()
{
    delete fMessenger;

    for (G4int i=0; i<G4int(fVisAttributes.size()); ++i)
    {
      delete fVisAttributes[i];
    }
  delete fFastShowerModel;
  delete fParameterisation;
  delete fParticleBounds;
  delete fHitMaker;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B5DetectorConstruction::Construct()
{
    // Construct materials
    ConstructMaterials();
    G4Material* air = G4Material::GetMaterial("G4_AIR");
    G4Material* argonGas = G4Material::GetMaterial("G4_Ar");
    G4Material* scintillator
      = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    G4Material* csI = G4Material::GetMaterial("G4_CESIUM_IODIDE");
    G4Material* lead = G4Material::GetMaterial("G4_Pb");
    G4Material* pbwo4 = G4Material::GetMaterial("G4_PbWO4");

    // Option to switch on/off checking of volumes overlaps
    //
    G4bool checkOverlaps = true;

    int cell_no =  fBoxSize/fCellSize;
    double half_box = fBoxSize/2.;
    double half_cell = fCellSize/2.;


    // geometries --------------------------------------------------------------
    // experimental hall (world volume)
    G4VSolid* worldSolid
      = new G4Box("worldBox",2*half_box,2*half_box,2*half_box);
    G4LogicalVolume* worldLogical
      = new G4LogicalVolume(worldSolid,air,"worldLogical");
    G4VPhysicalVolume* worldPhysical
      = new G4PVPlacement(0,G4ThreeVector(),worldLogical,"worldPhysical",0,
                          false,0,checkOverlaps);


    // hadron calorimeter
    G4VSolid* hadCalorimeterSolid
      = new G4Box("HadCalorimeterBox",half_box,half_box,half_box);
    G4LogicalVolume* hadCalorimeterLogical
      = new G4LogicalVolume(hadCalorimeterSolid,pbwo4,"HadCalorimeterLogical");
    new G4PVPlacement(0,fInitialPos,hadCalorimeterLogical,
                      "HadCalorimeterPhysical",worldLogical,
                      false,0,checkOverlaps);

    // hadron calorimeter column
    G4VSolid* HadCalColumnSolid
      = new G4Box("HadCalColumnBox",half_cell,half_box,half_box);
    G4LogicalVolume* HadCalColumnLogical
      = new G4LogicalVolume(HadCalColumnSolid,pbwo4,"HadCalColumnLogical");
    new G4PVReplica("HadCalColumnPhysical",HadCalColumnLogical,
                    hadCalorimeterLogical,kXAxis,cell_no,fCellSize);

    // hadron calorimeter cell
    G4VSolid* HadCalCellSolid
      = new G4Box("HadCalCellBox",half_cell,half_cell,half_box);
    G4LogicalVolume* HadCalCellLogical
      = new G4LogicalVolume(HadCalCellSolid,pbwo4,"HadCalCellLogical");
    new G4PVReplica("HadCalCellPhysical",HadCalCellLogical,
                    HadCalColumnLogical,kYAxis,cell_no,fCellSize);

    // hadron calorimeter layers
    G4VSolid* HadCalLayerSolid
      = new G4Box("HadCalLayerBox",half_cell,half_cell,half_cell );
    fHadCalScintiLogical
      = new G4LogicalVolume(HadCalLayerSolid,pbwo4,"HadCalLayerLogical");
    new G4PVReplica("HadCalLayerPhysical",fHadCalScintiLogical,
                    HadCalCellLogical,kZAxis,cell_no,fCellSize);
    
  fRegion = new G4Region("crystals");
  hadCalorimeterLogical->SetRegion(fRegion);
  fRegion->AddRootLogicalVolume(hadCalorimeterLogical);

    // // scintillator plates
    // G4VSolid* HadCalScintiSolid
    //   = new G4Box("HadCalScintiBox",half_cell/2,half_cell/2,half_cell/2);
    // fHadCalScintiLogical
    //   = new G4LogicalVolume(HadCalScintiSolid,scintillator,
    //                         "HadCalScintiLogical");
    // new G4PVPlacement(0,G4ThreeVector(0.,0.,0),fHadCalScintiLogical,
    //                   "HadCalScintiPhysical",HadCalLayerLogical,
    //                   false,0,checkOverlaps);

    // visualization attributes ------------------------------------------------

    G4VisAttributes* visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    visAttributes->SetVisibility(false);
    worldLogical->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);


    visAttributes = new G4VisAttributes(G4Colour(0.0, 0.0, 0.9));
    hadCalorimeterLogical->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.0, 0.9, 0.0));
    // HadCalLayerLogical->SetVisAttributes(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.0, 0.0));
    visAttributes->SetVisibility(false);
    fHadCalScintiLogical->SetVisAttributes(visAttributes);
    HadCalColumnLogical->SetVisAttributes(visAttributes);
    HadCalCellLogical->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    // return the world physical volume ----------------------------------------

    return worldPhysical;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5DetectorConstruction::ConstructSDandField()
{
    // sensitive detectors -----------------------------------------------------
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    G4String SDname;

    G4VSensitiveDetector* hadCalorimeter
      = new B5HadCalorimeterSD(SDname="/HadCalorimeter", fBoxSize/fCellSize);
    std::cout<<"Sensitive detector created with "<<fBoxSize/fCellSize<<" cells"<<std::endl;
    SDman->AddNewDetector(hadCalorimeter);
    fHadCalScintiLogical->SetSensitiveDetector(hadCalorimeter);
  // -- fast simulation models:
  // **********************************************
  // * Initializing shower modell
  // ***********************************************
  G4cout << "Creating shower parameterization models" << G4endl;
  fFastShowerModel = new GFlashShowerModel("fFastShowerModel", fRegion);
  fParameterisation = new GFlashHomoShowerParameterisation(fHadCalScintiLogical->GetMaterial());
  fFastShowerModel->SetParameterisation(*fParameterisation);
  // Energy Cuts to kill particles:
  fParticleBounds = new GFlashParticleBounds();
  fFastShowerModel->SetParticleBounds(*fParticleBounds);
  // Makes the EnergieSpots
  fHitMaker = new GFlashHitMaker();
  fFastShowerModel->SetHitMaker(*fHitMaker);
  G4cout<<"end shower parameterization."<<G4endl;
  // **********************************************

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5DetectorConstruction::ConstructMaterials()
{
    G4NistManager* nistManager = G4NistManager::Instance();

    // Air
    nistManager->FindOrBuildMaterial("G4_AIR");

    // Argon gas
    nistManager->FindOrBuildMaterial("G4_Ar");
    // With a density different from the one defined in NIST
    // G4double density = 1.782e-03*g/cm3;
    // nistManager->BuildMaterialWithNewDensity("B5_Ar","G4_Ar",density);
    // !! cases segmentation fault

    // Scintillator
    // (PolyVinylToluene, C_9H_10)
    nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

    // CsI
    nistManager->FindOrBuildMaterial("G4_CESIUM_IODIDE");

    // Lead
    nistManager->FindOrBuildMaterial("G4_Pb");

    nistManager->FindOrBuildMaterial("G4_PbWO4");

    // Vacuum "Air with low density"
    // G4Material* air = G4Material::GetMaterial("G4_AIR");
    // G4double density = 1.0e-5*air->GetDensity();
    // nistManager
    //   ->BuildMaterialWithNewDensity("Air_lowDensity", "G4_AIR", density);

    G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
