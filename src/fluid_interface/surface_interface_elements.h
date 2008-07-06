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
//Header file for specific (two-dimensional) free surface elements
//Include guards, to prevent multiple includes
#ifndef OOMPH_SURFACE_INTERFACE_ELEMENTS_HEADER
#define OOMPH_SURFACE_INTERFACE_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//OOMPH-LIB headers
#include "../generic/Qelements.h"
#include "../generic/spines.h"
#include "../generic/hijacked_elements.h"
#include "interface_elements.h"

namespace oomph
{

//======================================================================
/// Two-dimensional interface elements that are used with a spine mesh,
/// i.e. the mesh deformation is handled by Kistler & Scriven's "method
/// of spines". These elements are FaceElements of bulk
/// Fluid elements and the particular type of fluid element is passed 
/// as a template parameter to the element. It 
/// shouldn't matter whether the passed 
/// element is the underlying (fixed) element or the templated 
/// SpineElement<Element>.
/// Optionally, an external pressure may be specified, which must be
/// passed to the element as external data. If there is no such object,
/// the external pressure is assumed to be zero. \n
//======================================================================
template<class ELEMENT>
class SpineSurfaceFluidInterfaceElement : 
public virtual Hijacked<SpineElement<FaceGeometry<ELEMENT> > >, 
public virtual SurfaceFluidInterfaceElement 
{
  private:

 /// \short In spine elements, the kinematic condition is the equation 
 /// used to determine the unknown spine heights. Overload the
 /// function accordingly
 int kinematic_local_eqn(const unsigned &n) 
  {return this->spine_local_eqn(n);}

 /// \short Hijacking the kinematic condition corresponds to hijacking the
 /// spine heights.
 void hijack_kinematic_conditions(const Vector<unsigned> &bulk_node_number)
  {
   //Loop over all the passed nodes
   for(Vector<unsigned>::const_iterator it=bulk_node_number.begin();
       it!=bulk_node_number.end();++it)
    {
     //Hijack the spine heights. (and delete the returned data object)
     delete this->hijack_nodal_spine_value(*it,0);
    }
  }

  public:
 
 /// Constructor, the arguments are a pointer to the  "bulk" element 
 /// and the index of the face to be created
 SpineSurfaceFluidInterfaceElement(FiniteElement* const &element_pt, 
                                   const int &face_index) : 
  Hijacked<SpineElement<FaceGeometry<ELEMENT> > >(), 
  SurfaceFluidInterfaceElement()
  {
#ifdef PARANOID
   //Check that the element is not refineable
   ELEMENT* elem_pt = new ELEMENT;
   if(dynamic_cast<RefineableElement*>(elem_pt))
    {
     //Issue a warning
      OomphLibWarning(
       "This interface element will not work correctly if nodes are hanging\n",
       "SpineSurfaceFludInterfaceElement::Constructor",
       OOMPH_EXCEPTION_LOCATION);
     }
#endif

   //Attach the geometrical information to the element, by
   //making the face element from the bulk element
   element_pt->build_face_element(face_index,this);

   //Find the index at which the velocity unknowns are stored 
   //from the bulk element
   ELEMENT* cast_element_pt = dynamic_cast<ELEMENT*>(element_pt);
   //We must have three velocity components
   this->U_index_interface.resize(3);
   for(unsigned i=0;i<3;i++)
    {
     this->U_index_interface[i] = cast_element_pt->u_index_nst(i);
    }
   }

 /// Calculate the contribution to the jacobian
 void fill_in_contribution_to_jacobian(Vector<double> &residuals, 
                                   DenseMatrix<double> &jacobian)
  {
   //Call the generic routine with the flag set to 1
   fill_in_generic_residual_contribution_interface(residuals,jacobian,1);
   //Call the generic routine to handle the spine variables
   //SpineElement<FaceGeometry<ELEMENT> >::
    this->fill_in_jacobian_from_geometric_data(jacobian);
  }

 /// Overload the output function
 void output(std::ostream &outfile) {FiniteElement::output(outfile);}

 /// Output the element
 void output(std::ostream &outfile, const unsigned &n_plot)
  {SurfaceFluidInterfaceElement::output(outfile,n_plot);}

 ///Overload the C-style output function
 void output(FILE* file_pt) {FiniteElement::output(file_pt);}

 ///C-style Output function: x,y,[z],u,v,[w],p in tecplot format
 void output(FILE* file_pt, const unsigned &n_plot)
  {SurfaceFluidInterfaceElement::output(file_pt,n_plot);}


 /// Create an edge element
 virtual FluidInterfaceEdgeElement* make_edge_element(
  const int &face_index)
  {
   //Create a temporary pointer to the appropriate FaceElement
   SpineLineFluidInterfaceEdgeElement<ELEMENT> *Temp_pt = 
    new SpineLineFluidInterfaceEdgeElement<ELEMENT>;

   //Attach the geometrical information to the new element
   //FaceGeometry<ELEMENT>::
    this->build_face_element(face_index,Temp_pt);

   //Set the value of the nbulk_value, the node is not resized
   //in this problem, so it will just be the actual nvalue
   Temp_pt->nbulk_value(0) = Temp_pt->node_pt(0)->nvalue();


   
   //Set of unique geometric data that is used to update the bulk,
   //but is not used to update the face
   std::set<Data*> unique_additional_geom_data;
   //Get all the geometric data for this (bulk) element
   this->assemble_set_of_all_geometric_data(unique_additional_geom_data);

   //Now assemble the set of geometric data for the face element
   std::set<Data*> unique_face_geom_data_pt;
   Temp_pt->assemble_set_of_all_geometric_data(unique_face_geom_data_pt);
   //Erase the face geometric data from the additional data
   for(std::set<Data*>::iterator it=unique_face_geom_data_pt.begin();
       it!=unique_face_geom_data_pt.end();++it)
    {unique_additional_geom_data.erase(*it);}

   //Finally add all unique additional data as external data
   for(std::set<Data*>::iterator it = unique_additional_geom_data.begin();
       it!= unique_additional_geom_data.end();++it)
    {
     Temp_pt->add_external_data(*it);
    }

   //Return the value of the pointer
   return Temp_pt;
   }
 
};


//=======================================================================
/// Two-dimensional interface elements that are used when the mesh
/// deformation is handled by a set of equations that modify the nodal
/// positions. These elements are FaceElements of bulkFluid elements and
/// the fluid element is passed as a template parameter to the element.
/// Optionally an external pressure may be specified, which must be
/// passed to the element as external data. The default value of the external
/// pressure is zero.
//=======================================================================
template<class ELEMENT>
class ElasticSurfaceFluidInterfaceElement : 
public virtual Hijacked<FaceGeometry<ELEMENT> >, 
public SurfaceFluidInterfaceElement
{
  private:

  int kinematic_local_eqn(const unsigned &n)
  {return this->nodal_local_eqn(n,Nbulk_value[n]);}

 
 void hijack_kinematic_conditions(const Vector<unsigned> &bulk_node_number)
  {
   
   //Loop over all the passed nodes
   for(Vector<unsigned>::const_iterator it=bulk_node_number.begin();
       it!=bulk_node_number.end();++it)
    {
     //Make sure that we delete the returned value
     delete this->hijack_nodal_value(*it,Nbulk_value[*it]);
    }
  }
 
  public:
 /// Constructor, pass a pointer to the bulk elemnet
 ElasticSurfaceFluidInterfaceElement(FiniteElement* const &element_pt, 
                                     const int &face_index) : 
  FaceGeometry<ELEMENT>(), SurfaceFluidInterfaceElement()
  {
#ifdef PARANOID
   //Check that the element is not refineable
   ELEMENT* elem_pt = new ELEMENT;
   if(dynamic_cast<RefineableElement*>(elem_pt))
    {
     //Issue a warning
      OomphLibWarning(
       "This interface element will not work correctly if nodes are hanging\n",
       "ElasticSurfaceFludInterfaceElement::Constructor",
       OOMPH_EXCEPTION_LOCATION);
     }
#endif

   //Attach the geometrical information to the element
   //This function also assigned nbulk_value from required_nvalue of the
   //bulk element
   element_pt->build_face_element(face_index,this);
   
   //Find the index at which the velocity unknowns are stored 
   //from the bulk element
   ELEMENT* cast_element_pt = dynamic_cast<ELEMENT*>(element_pt);
   //We must have three velocity components
   this->U_index_interface.resize(3);
   for(unsigned i=0;i<3;i++)
    {
     this->U_index_interface[i] = cast_element_pt->u_index_nst(i);
    }
   
   //Read out the number of nodes on the face
   //For some reason I need to specify the this pointer here(!)
   unsigned n_node_face = this->nnode();
   //Set the additional data values in the face
   //There is one additional values at each node --- the lagrange multiplier
   Vector<unsigned> additional_data_values(n_node_face);
   for(unsigned i=0;i<n_node_face;i++) additional_data_values[i] = 1;

   //Resize the data arrays accordingly 
   resize_nodes(additional_data_values);
  }

 /// Return the lagrange multiplier at local node n
 double &lagrange(const unsigned &n)
  {
   //The lagrange multiplier is always stored at Nbulk_value[n]
   return *node_pt(n)->value_pt(Nbulk_value[n]);
  }
   
 /// Compute the jacobian
 void fill_in_contribution_to_jacobian(Vector<double> &residuals, 
                                   DenseMatrix<double> &jacobian)
  {
   //Call the generic routine with the flag set to 1
   fill_in_generic_residual_contribution_interface(residuals,jacobian,1);
   //Call the generic finite difference routine for the solid variables
   this->fill_in_jacobian_from_solid_position_by_fd(jacobian);
  }

 /// Overload the output function
 void output(std::ostream &outfile) {FiniteElement::output(outfile);}

 /// Output the element
 void output(std::ostream &outfile, const unsigned &n_plot)
  {SurfaceFluidInterfaceElement::output(outfile,n_plot);}

 ///Overload the C-style output function
 void output(FILE* file_pt) {FiniteElement::output(file_pt);}

 ///C-style Output function: x,y,[z],u,v,[w],p in tecplot format
 void output(FILE* file_pt, const unsigned &n_plot)
  {SurfaceFluidInterfaceElement::output(file_pt,n_plot);}

 /// \short Helper function to calculate the additional bits
 virtual void add_additional_residual_contributions(
  Vector<double> &residuals, DenseMatrix<double> &jacobian,
  const unsigned &flag,
  const Shape &psif,const DShape &dpsifds,
  const Vector<double> &interpolated_n, 
  const double &W)

 {
  //Loop over the shape functions
  unsigned n_node = this->nnode();

  double interpolated_lagrange = 0.0;

  //Loop over the shape functions
   for(unsigned l=0;l<n_node;l++)
    {
     //Note same shape functions used for lagrange multiplier field
     interpolated_lagrange += lagrange(l)*psif(l);
    }
   
   int local_eqn=0, local_unknown = 0;
  //Loop over the shape functions
  for(unsigned l=0;l<n_node;l++)
   {
    //Loop over the directions
    for(unsigned i=0;i<3;i++)
     {
      //Now using the same shape functions for the elastic equations,
      //so we can stay in the loop
      local_eqn = this->position_local_eqn(l,0,i);
      if(local_eqn >= 0)
       {
        //Add in a "lagrange multiplier" normal force term
        //N.B. This is an assumption, we assume that the force acts
        //in the normal direction, that's it!
        //The normal vector includes the appropriate area contribution
        residuals[local_eqn] -= 
         interpolated_lagrange*interpolated_n[i]*psif(l)*W;
        
         //Do the Jacobian calculation
         if(flag)
          {
           //Loop over the nodes 
           for(unsigned l2=0;l2<n_node;l2++)
            {
             //V equations dependence upon x will be handled by FDs
             //That leaves the "lagrange multipliers" only
             local_unknown = kinematic_local_eqn(l2);
             if(local_unknown >= 0)
              {
               jacobian(local_eqn,local_unknown) -=
                psif(l2)*interpolated_n[i]*psif(l)*W;
              }
            }
          } //End of Jacobian calculation
        } //End of V equations
      } //End of loop over i
    
   }
 }


 /// Create an edge element
 virtual FluidInterfaceEdgeElement* make_edge_element(const int &face_index)
  {
   //Create a temporary pointer to the appropriate FaceElement
   ElasticLineFluidInterfaceEdgeElement<ELEMENT> *Temp_pt = 
    new ElasticLineFluidInterfaceEdgeElement<ELEMENT>;

   //Attach the geometrical information to the new element
   this->build_face_element(face_index,Temp_pt);
   
   //Find the nodes
   std::set<SolidNode*> set_of_solid_nodes;
   unsigned n_node = this->nnode();
   for(unsigned n=0;n<n_node;n++)
    {
     set_of_solid_nodes.insert(static_cast<SolidNode*>(this->node_pt(n)));
    }

   //Delete the nodes from the face
   n_node = Temp_pt->nnode();
   for(unsigned n=0;n<n_node;n++)
    {
     //Set the value of the nbulk_value, from the present element
     Temp_pt->nbulk_value(n) = 
      this->nbulk_value(Temp_pt->bulk_node_number(n));
     //Now delete the nodes from the set
    set_of_solid_nodes.erase(static_cast<SolidNode*>(Temp_pt->node_pt(n)));
    }

   //Now add these as external data
   for(std::set<SolidNode*>::iterator it=set_of_solid_nodes.begin();
       it!=set_of_solid_nodes.end();++it)
    {
     Temp_pt->add_external_data((*it)->variable_position_pt());
    }
           

   //Return the value of the pointer
   return Temp_pt;
   }

};

}

#endif






