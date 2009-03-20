//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.85. June 9, 2008.
//LIC// 
//LIC// Copyright (C) 2006-2008 Matthias Heil and Andrew Hazel
//LIC// 
//LIC// This library is free software; you can redistribute it and/or
//LIC// modify it under the terms of the GNU Lesser General Public
//LIC// License as published by the Free Software Foundation; either
//LIC// version 2.1 of the License, or (at your option) any later version.
//LIC// 
//LIC// This library is distributed in the hope that it will be useful,
//LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
//LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//LIC// Lesser General Public License for more details.
//LIC// 
//LIC// You should have received a copy of the GNU Lesser General Public
//LIC// License along with this library; if not, write to the Free Software
//LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//LIC// 02110-1301  USA.
//LIC// 
//LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
//LIC// 
//LIC//====================================================================
// Driver code for a simple unstructured solid problem using a mesh
// generated from an input file generated by the 3d mesh generator
// tetgen

//Generic routines
#include "generic.h"
#include "solid.h"
#include "constitutive.h"

// Get the mesh
#include "meshes/tetgen_mesh.h"

using namespace std;
using namespace oomph;



//=======================start_mesh========================================
/// Triangle-based mesh upgraded to become a solid mesh
//=========================================================================
template<class ELEMENT>
class SolidTetMesh : public virtual TetgenMesh<ELEMENT>, 
                     public virtual SolidMesh 
{
 
public:
 
 /// Constructor: 
 SolidTetMesh(const std::string& node_file_name,
              const std::string& element_file_name,
              const std::string& face_file_name,
              TimeStepper* time_stepper_pt=
              &Mesh::Default_TimeStepper) : 
  TetgenMesh<ELEMENT>(node_file_name, element_file_name,
                      face_file_name, time_stepper_pt)
  {
   //Assign the Lagrangian coordinates
   set_lagrangian_nodal_coordinates();
   
   // Find elements next to boundaries
   setup_boundary_element_info();
  }

 /// Empty Destructor
 virtual ~SolidTetMesh() { }


};


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


//=======start_namespace==========================================
/// Global variables
//================================================================
namespace Global_Parameters
{

 /// Poisson's ratio
 double Nu=0.3;
 
 /// Create constitutive law
 ConstitutiveLaw* Constitutive_law_pt=new GeneralisedHookean(&Nu);

 /// Non-dim gravity
 double Gravity=0.0;

 /// Non-dimensional gravity as body force
 void gravity(const double& time,
              const Vector<double> &xi,
              Vector<double> &b)
 {
  b[0]=-Gravity; 
  b[1]=0.0;
  b[2]=0.0;
 } // end gravity

 /// Uniform pressure
 double P = 0.0;

 /// \short Constant pressure load. The arguments to this function are imposed
 /// on us by the SolidTractionElements which allow the traction to 
 /// depend on the Lagrangian and Eulerian coordinates x and xi, and on the 
 /// outer unit normal to the surface. Here we only need the outer unit
 /// normal.
 void constant_pressure(const Vector<double> &xi, const Vector<double> &x,
                        const Vector<double> &n, Vector<double> &traction)
 {
  unsigned dim = traction.size();
  for(unsigned i=0;i<dim;i++)
   {
    traction[i] = -P*n[i];
   }
 } // end traction
 
 
} //end namespace






//=============start_problem===========================================
/// Unstructured solid problem
//=====================================================================
template<class ELEMENT>
class UnstructuredSolidProblem : public Problem
{

public:

 /// Constructor: 
 UnstructuredSolidProblem();

 /// Destructor (empty)
 ~UnstructuredSolidProblem(){}

 /// Update the problem specs before solve: empty
 void actions_before_newton_solve() {}

 /// Update the problem specs before solve: empty
 void actions_after_newton_solve() {}

 /// Doc the solution
 void doc_solution(DocInfo& doc_info);
 
private:

 /// Create traction elements
 void create_traction_elements();

 /// Bulk solid mesh
 SolidTetMesh<ELEMENT>* Solid_mesh_pt;

 /// Meshes of traction elements
 Vector<SolidMesh*> Solid_traction_mesh_pt;

 /// IDs of solid mesh boundaries where displacements are pinned
 Vector<unsigned> Pinned_solid_boundary_id;

 /// \short IDs of solid mesh boundaries which make up the traction interface
 Vector<unsigned> Solid_traction_boundary_id;

};



//=============start_constructor==========================================
/// Constructor for unstructured solid problem
//========================================================================
template<class ELEMENT>
UnstructuredSolidProblem<ELEMENT>::UnstructuredSolidProblem()
{ 

  //Create solid bulk mesh
 string node_file_name="fsi_bifurcation_solid.1.node";
 string element_file_name="fsi_bifurcation_solid.1.ele";
 string face_file_name="fsi_bifurcation_solid.1.face";
 Solid_mesh_pt =  new SolidTetMesh<ELEMENT>(node_file_name,
                                            element_file_name,
                                            face_file_name);
 
 // The following corresponds to the boundaries as specified by
 // facets in the tetgen input:
 
 /// IDs of solid mesh boundaries where displacements are pinned
 Pinned_solid_boundary_id.resize(3);
 Pinned_solid_boundary_id[0]=0;
 Pinned_solid_boundary_id[1]=1;
 Pinned_solid_boundary_id[2]=2;
 
  // The solid mesh boundaries where an internal pressure is applied
 Solid_traction_boundary_id.resize(12);
 for (unsigned i=0;i<12;i++)
  {
   Solid_traction_boundary_id[i]=i+3;
  }
 
 
 // Apply BCs for solid
 //--------------------
 
 // Doc pinned solid nodes
 std::ofstream bc_file("RESLT/pinned_solid_nodes.dat");
 
 // Pin positions at inflow boundary (boundaries 0 and 1)
 unsigned n=Pinned_solid_boundary_id.size();
 for (unsigned i=0;i<n;i++)
  {
   // Get boundary ID
   unsigned b=Pinned_solid_boundary_id[i];
   unsigned num_nod= Solid_mesh_pt->nboundary_node(b);  
   for (unsigned inod=0;inod<num_nod;inod++)
    {    
     // Get node
     SolidNode* nod_pt=Solid_mesh_pt->boundary_node_pt(b,inod);
     
     // Pin all directions
     for (unsigned i=0;i<3;i++)
      {
       nod_pt->pin_position(i);
       
       // ...and doc it as pinned
       bc_file << nod_pt->x(i) << " ";
      }
     
     bc_file << std::endl;
    }
  }
 bc_file.close();
 
 
 
 // Complete the build of all elements so they are fully functional
 //----------------------------------------------------------------
 unsigned n_element = Solid_mesh_pt->nelement();
 for(unsigned i=0;i<n_element;i++)
  {
   //Cast to a solid element
   ELEMENT *el_pt = dynamic_cast<ELEMENT*>(
    Solid_mesh_pt->element_pt(i));
   
   // Set the constitutive law   
   el_pt->constitutive_law_pt() =
    Global_Parameters::Constitutive_law_pt;
   
   //Set the body force
   el_pt->body_force_fct_pt() = Global_Parameters::gravity;
  }
 
 
 // Create traction elements
 //-------------------------
 
 // Create meshes of traction elements
 n=Solid_traction_boundary_id.size();
 Solid_traction_mesh_pt.resize(n);
 for (unsigned i=0;i<n;i++)
  {
   Solid_traction_mesh_pt[i]=new SolidMesh;
  }
 
 // Build the traction elements
 create_traction_elements();


 // Combine the lot
 //----------------
 
 // The solid bulk mesh
 add_sub_mesh(Solid_mesh_pt);

 // The solid traction meshes
 n=Solid_traction_boundary_id.size();
 for (unsigned i=0;i<n;i++)
  {
   add_sub_mesh(Solid_traction_mesh_pt[i]);
  }

 // Build global mesh
 build_global_mesh();

 // Setup equation numbering scheme
 std::cout <<"Number of equations: " << assign_eqn_numbers() << std::endl; 
 
} // end constructor



//============start_of_create_traction_elements==========================
/// Create traction elements 
//=======================================================================
template<class ELEMENT>
void UnstructuredSolidProblem<ELEMENT>::create_traction_elements()
{

 // Loop over traction boundaries
 unsigned n=Solid_traction_boundary_id.size();
 for (unsigned i=0;i<n;i++)
  {
   // Get boundary ID
   unsigned b=Solid_traction_boundary_id[i];
   
   // How many bulk elements are adjacent to boundary b?
   unsigned n_element = Solid_mesh_pt->nboundary_element(b);
   
   // Loop over the bulk elements adjacent to boundary b
   for(unsigned e=0;e<n_element;e++)
    {
     // Get pointer to the bulk element that is adjacent to boundary b
     ELEMENT* bulk_elem_pt = dynamic_cast<ELEMENT*>(
      Solid_mesh_pt->boundary_element_pt(b,e));
     
     //What is the index of the face of the element e along boundary b
     int face_index = Solid_mesh_pt->face_index_at_boundary(b,e);
     
     // Create new element 
     SolidTractionElement<ELEMENT>* el_pt=
      new SolidTractionElement<ELEMENT>(bulk_elem_pt,face_index);
     
     // Add it to the mesh
     Solid_traction_mesh_pt[i]->add_element_pt(el_pt);
     
     //Set the traction function
     el_pt->traction_fct_pt() = Global_Parameters::constant_pressure;
    }
  }
 
} // end of create_traction_elements



//========================================================================
/// Doc the solution
//========================================================================
template<class ELEMENT>
void UnstructuredSolidProblem<ELEMENT>::doc_solution(DocInfo& doc_info)
{ 

 ofstream some_file;
 char filename[100];

 // Number of plot points
 unsigned npts;
 npts=5;

 // Output domain boundaries
 //-------------------------
 sprintf(filename,"%s/boundaries%i.dat",doc_info.directory().c_str(),
         doc_info.number());
 some_file.open(filename);
 Solid_mesh_pt->output_boundaries(some_file);
 some_file.close();

 // Output solid solution
 //-----------------------
 sprintf(filename,"%s/solid_soln%i.dat",doc_info.directory().c_str(),
         doc_info.number());
 some_file.open(filename);
 Solid_mesh_pt->output(some_file,npts);
 some_file.close();

    
 // Output traction
 //----------------
 sprintf(filename,"%s/traction%i.dat",doc_info.directory().c_str(),
         doc_info.number());
 some_file.open(filename);
 unsigned n=Solid_traction_boundary_id.size();
 for (unsigned i=0;i<n;i++)
  {
   Solid_traction_mesh_pt[i]->output(some_file,npts);
  }
 some_file.close();
}





//============================start_main==================================
/// Demonstrate how to solve an unstructured 3D solid problem
//========================================================================
int main(int argc, char **argv)
{
 // Store command line arguments
 CommandLineArgs::setup(argc,argv);
  
 // Label for output
 DocInfo doc_info;
 
 // Output directory
 doc_info.set_directory("RESLT");
  
 //Set up the problem
 UnstructuredSolidProblem<TPVDElement<3,3> > problem;
 
 //Output initial configuration
 problem.doc_solution(doc_info);
 doc_info.number()++;   

  // Parameter study
 Global_Parameters::P=0.0; 
 double g_increment=1.0e-3; 
 double p_increment=1.0e-2; 

 unsigned nstep=6;
 if (CommandLineArgs::Argc==2)
  {
   std::cout << "Validation -- only doing two steps" << std::endl;
   nstep=2;
  }
 
 // Do the parameter study
 for (unsigned istep=0;istep<nstep;istep++)
  {
   // Solve the problem
   problem.newton_solve();
   
   //Output solution
   problem.doc_solution(doc_info);
   doc_info.number()++;

   // Bump up load
   Global_Parameters::Gravity+=g_increment;
   Global_Parameters::P+=p_increment; 
   
  }
 
} // end main




