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
// $Id: B5DetectorConstruction.hh 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file B5DetectorConstruction.hh
/// \brief Definition of the B5DetectorConstruction class

#ifndef B5DetectorConstruction_h
#define B5DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include <vector>
#include "G4ThreeVector.hh"


class G4VPhysicalVolume;
class G4Material;
class G4VSensitiveDetector;
class G4VisAttributes;
class G4GenericMessenger;

class GFlashHomoShowerParameterisation;
class GFlashShowerModel;
class GFlashHitMaker;
class GFlashParticleBounds;
class G4Region;
/// Detector construction

class B5DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    B5DetectorConstruction();
  B5DetectorConstruction(G4double, G4double, G4ThreeVector);
    virtual ~B5DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    void ConstructMaterials();

private:

    G4GenericMessenger* fMessenger;

    G4LogicalVolume* fHadCalScintiLogical;

    std::vector<G4VisAttributes*> fVisAttributes;
  G4double fBoxSize;
  G4double fCellSize;
  G4ThreeVector fInitialPos;
  GFlashShowerModel* fFastShowerModel; 
  GFlashHomoShowerParameterisation* fParameterisation;
  GFlashParticleBounds* fParticleBounds;
  GFlashHitMaker* fHitMaker;
  G4Region*           fRegion;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
