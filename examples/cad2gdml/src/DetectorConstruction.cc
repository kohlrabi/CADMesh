/* ************************************************
 * GEANT4 VCGLIB/CAD INTERFACE - basic example
 *
 * File:      DetectorConstruction.cc
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      20th March, 2011
 **************************************************/

// USER //
#include "DetectorConstruction.hh"

// CADMESH //
#include "CADMesh.hh"

// GEANT4 //
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4AssemblyVolume.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "G4GDMLParser.hh"


DetectorConstruction::DetectorConstruction()
{
    filename = "";
    filetype = "";
    material = "";
    outname = "";
}

DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4NistManager * nist_manager = G4NistManager::Instance();
    //G4Material * vacuum = nist_manager->FindOrBuildMaterial("G4_Galactic");
    G4Material * mat = nist_manager->FindOrBuildMaterial(material);

    //world_solid = new G4Box("world_solid", 50*cm, 50*cm, 50*cm);
    //world_logical = new G4LogicalVolume(world_solid, vacuum,"world_logical",0,0,0);
    //world_physical = new G4PVPlacement(0, G4ThreeVector(), world_logical,
    //                                   "world_physical", 0, false, 0);
    //world_logical->SetVisAttributes(G4VisAttributes::Invisible);

    // CAD model rotation. 
    G4RotationMatrix * rot = new G4RotationMatrix();
    
    // Load CAD file as tessellated solid //
    offset = G4ThreeVector(0, 0, 0);
    
    // Note that offset is applied to the points in mesh directly before placement.
    CADMesh * mesh = new CADMesh((char*) filename.c_str());
    mesh->SetScale(mm);
    mesh->SetOffset(offset);
    mesh->SetReverse(false);

    //if (filetype != "PLY" && filetype != "ply" ) {
        cad_solid = mesh->TessellatedMesh();
        cad_logical = new G4LogicalVolume(cad_solid, mat, "cad_logical", 0, 0, 0);
        cad_physical = new G4PVPlacement(rot, G4ThreeVector(), cad_logical,
                                        "cad_physical", world_logical, false, 0);
    //}
   
    /* disable tetgen for now
    else { 
        // Load CAD file as tetrahedral mesh //
        CADMesh * tet_mesh = new CADMesh((char*) filename.c_str(), (char*) filetype.c_str());
        tet_mesh->SetScale(1.);
        tet_mesh->SetMaterial(mat); // We have to do this before making the G4AssemblyVolume.

        G4AssemblyVolume * cad_assembly = tet_mesh->TetrahedralMesh();

        G4Translate3D translation(0., 0., 0.);
        G4Transform3D rotation = G4Rotate3D(*rot);
        G4Transform3D transform = translation*rotation;

        cad_assembly->MakeImprint(world_logical, transform, 0, 0);
    }
    */

    G4GDMLParser().Write(outname, cad_physical);

    return cad_physical;
}
