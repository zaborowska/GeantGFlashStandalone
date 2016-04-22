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

#include "FCCActionInitialization.hh"
#include "G4GlobalFastSimulationManager.hh"
// #include "FCCDetectorConstruction.hh"
#include "ExGflashPhysics.hh"
#include "B5DetectorConstruction.hh"
#include "G4GDMLParser.hh"
#include "FTFP_BERT.hh"
#include "G4VModularPhysicsList.hh"

//sd
// #include "G4LogicalVolumeStore.hh"
// #include "G4TransportationManager.hh"
// #include "G4SDManager.hh"
// #include "B5EmCalorimeterSD.hh"


#include "G4UImanager.hh"
#include "G4RunManager.hh"
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif
#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif
// config file
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

int main(int argc, char** argv)
{
   if (argc<2)
   {
      G4cout << G4endl;
      G4cout << "Error! Mandatory input files are not specified!" << G4endl;
      G4cout << G4endl;
      G4cout << "Usage: "<<argv[0]<<"\n\t\t<intput configuration file : mandatory>"
             <<"\n\t\t<settings macro : optional (if not ->GUI)>"<< G4endl;
      G4cout << G4endl;
      return -1;
   }

   // Parsing a config file
   boost::property_tree::ptree pt;
   boost::property_tree::ini_parser::read_ini(argv[1], pt);
   double boxSize, cellSize, x0, y0, z0;
   boxSize = pt.get<double >("Geometry_in_mm.box_size");
   cellSize = pt.get<double>("Geometry_in_mm.cell_size");
   G4ThreeVector pos0(pt.get<double>("Geometry_in_mm.x0"),
     pt.get<double>("Geometry_in_mm.y0"),
     pt.get<double>("Geometry_in_mm.z0"));
   std::string filename= pt.get<std::string>("output.filename");
   G4cout<<"+=======================================================+"<<G4endl;
   G4cout<<"|               Parsing configuration file              |"<<G4endl;
   G4cout<<"+=======================================================+"<<G4endl;
   std::cout<<"\tBox size: "<<boxSize<<" mm\n\tCell size: "<<cellSize<<" mm\n\tHence, no of cells: "
            <<boxSize/cellSize<<" x "<<boxSize/cellSize<<" x "<<boxSize/cellSize
            <<"\n\tInitial position: "<<pos0<<" mm\n\tOutput file name: "<<filename<<"\n";

   //-------------------------------
   // Initialization of Run manager
   //-------------------------------
   G4RunManager * runManager = new G4RunManager;
   G4cout<<"+-------------------------------------------------------+"<<G4endl;
   G4cout<<"|        Constructing sequential run manager            |"<<G4endl;
   G4cout<<"+-------------------------------------------------------+"<<G4endl;

   //-------------------------------
   // PhysicsList (including G4FastSimulationManagerProcess)
   //-------------------------------
   G4VModularPhysicsList* physicsList = new FTFP_BERT();
  physicsList->RegisterPhysics(new ExGflashPhysics());
   runManager->SetUserInitialization(physicsList);

   G4VUserDetectorConstruction* detector = new B5DetectorConstruction(boxSize, cellSize, pos0);
   runManager->SetUserInitialization(detector);

   // //-------------------------------
   // // Load geometry (from GDML)
   // //-------------------------------
   // G4GDMLParser parser;
   // G4cout << "Geometry loaded from  file " << argv[1]<<G4endl;
   // parser.Read(argv[1]);
   // G4VUserDetectorConstruction* detector = new FCCDetectorConstruction(parser.GetWorldVolume());
   // runManager->SetUserInitialization(detector);
   // //------------------------------------------------ 
   // // Sensitive detectors
   // //------------------------------------------------ 
   
   // G4SDManager* SDman = G4SDManager::GetSDMpointer();
   
   // B5EmCalorimeterSD* aTrackerSD = new B5EmCalorimeterSD("ECal");
   // SDman->AddNewDetector( aTrackerSD );

   // ///////////////////////////////////////////////////////////////////////
   // //
   // // Example how to retrieve Auxiliary Information for sensitive detector
   // //
   // const G4GDMLAuxMapType* auxmap = parser.GetAuxMap();
   // G4cout << "Found " << auxmap->size()
   //           << " volume(s) with auxiliary information."
   //           << G4endl << G4endl;
   // for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin();
   //     iter!=auxmap->end(); iter++) 
   // {
   //   G4cout << "Volume " << ((*iter).first)->GetName()
   //          << " has the following list of auxiliary information: "
   //          << G4endl << G4endl;
   //   for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin();
   //        vit!=(*iter).second.end(); vit++)
   //   {
   //     G4cout << "--> Type: " << (*vit).type
   //               << " Value: " << (*vit).value << G4endl;
   //   }
   // }
   // G4cout << G4endl;

   // // The same as above, but now we are looking for
   // // sensitive detectors setting them for the volumes

   // for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin();
   //     iter!=auxmap->end(); iter++) 
   // {
   //   G4cout << "Volume " << ((*iter).first)->GetName()
   //          << " has the following list of auxiliary information: "
   //          << G4endl << G4endl;
   //   for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin();
   //        vit!=(*iter).second.end();vit++)
   //   {
   //     if ((*vit).type=="SensDet")
   //     {
   //       G4cout << "Attaching sensitive detector " << (*vit).value
   //              << " to volume " << ((*iter).first)->GetName()
   //              <<  G4endl << G4endl;

   //       G4VSensitiveDetector* mydet = 
   //         SDman->FindSensitiveDetector((*vit).value);
   //       if(mydet) 
   //       {
   //         G4LogicalVolume* myvol = (*iter).first;
   //         myvol->SetSensitiveDetector(mydet);
   //       }
   //       else
   //       {
   //         G4cout << (*vit).value << " detector not found" << G4endl;
   //       }
   //     }
   //   }
   // }
   // //
   // // End of Auxiliary Information block
   // //
   // ////////////////////////////////////////////////////////////////////////

   //-------------------------------
   // UserAction classes
   //-------------------------------
   runManager->SetUserInitialization( new FCCActionInitialization(boxSize/cellSize, filename) );

   G4cout << "pre initialize"<<G4endl;
   runManager->Initialize();
   G4cout << "post initialize"<<G4endl;

   //-------------------------------
   // UI
   //-------------------------------
   G4UImanager* UImanager = G4UImanager::GetUIpointer();

   if(argc<3)
   {
   G4cout << "argc<3"<<G4endl;
      //--------------------------
      // Define (G)UI
      //--------------------------
#ifdef G4UI_USE
   G4cout << "G4UI_USE"<<G4endl;
      G4UIExecutive * ui = new G4UIExecutive(argc, argv);
   G4cout << "G4UI_USE 2"<<G4endl;
#ifdef G4VIS_USE
   G4cout << "G4VIS_USE"<<G4endl;
      G4VisManager* visManager = new G4VisExecutive;
   G4cout << "G4VIS_USE 2 "<<G4endl;
      visManager->Initialize();
   G4cout << "G4VIS_USE 3 "<<G4endl;
      UImanager->ApplyCommand("/control/execute ../vis.mac");
#endif
      ui->SessionStart();
#ifdef G4VIS_USE
      delete visManager;
#endif
      delete ui;
#endif
   }
   else
   {
     G4cout << "else"<<G4endl;
      G4String command = "/control/execute ";
      G4String fileName = argv[2];
      G4cout << "/control/execute "<<argv[2]<<G4endl;
      UImanager->ApplyCommand(command+fileName);
   }

   // Free the store: user actions, physics_list and detector_description are
   //                 owned and deleted by the run manager, so they should not
   //                 be deleted in the main() program !

   delete runManager;

   return 0;
}
