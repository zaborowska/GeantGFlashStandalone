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
// based on G4 examples/extended/parametrisations/Par01/src/Par01ActionInitialization.cc
//

#include "FCCActionInitialization.hh"
#include "B5EventAction.hh"
#include "B1PrimaryGeneratorAction.hh"
#include "G4UIcommand.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::FCCActionInitialization() : G4VUserActionInitialization(), fFileName("DefaultOutput.root")
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::FCCActionInitialization(const int aCellNo, const G4String aOutName) : G4VUserActionInitialization(), fCellNo(aCellNo), fFileName(aOutName)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::FCCActionInitialization(const int  aCellNo) : G4VUserActionInitialization(), fCellNo(aCellNo), fFileName("DefaultOutput")
{}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::~FCCActionInitialization()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCActionInitialization::BuildForMaster() const
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCActionInitialization::Build() const
{
  SetUserAction(new B1PrimaryGeneratorAction);
  SetUserAction(new B5EventAction(fCellNo,fFileName));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
