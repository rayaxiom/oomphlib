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
//Header file for (one-dimensional) free surface elements
//Include guards, to prevent multiple includes
#ifndef FIXED_VOLUME_SURFACE_INTERFACE_ELEMENTS_HEADER
#define FIXED_VOLUME_SURFACE_INTERFACE_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//OOMPH-LIB headers
#include "generic/Qelements.h"
#include "generic/spines.h"
#include "fluid_interface/surface_interface_elements.h"

namespace oomph
{

//==========================================================================
/// A \b preliminary class that is used to implement the constraint 
/// that the fluid volume must take a specific value.
/// In order to implement the constraint in total generality,
/// one would need to introduce surface elements all round the mesh
/// and integrate x.n over them. This is not necessary in the special case
/// considered here (a quarter-tube mesh), because the integrals over the 
/// base, symmetry boundaries and curved edge are all zero. The final
/// integral over the free surface is easily peformed.
//=========================================================================
class VolumeConstraintElement : public GeneralisedElement
{
  private:
 
 /// Pointer to the desired value of the volume
 double *Volume_pt;
 
 /// \short The local eqn number for the pressure that has been
 /// "traded" for the volume constraint
 int Ptraded_local_eqn;

 /// \short Pointer to the Data item that stores the pressure that has been
 /// "traded" for the volume constraint in its single value.
 Data* Ptraded_data_pt;

 /// \short The Data that contains the traded pressure is stored
 /// as external Data for the element. Which external Data item is it?
 unsigned External_data_number_of_traded_pressure;

  public:

 ///Constructor, there are no internal values. 
 VolumeConstraintElement() : GeneralisedElement()
  {
   // Initialise pointer to prescribed volume of fluid
   Volume_pt=0;
   // Initialise pointer to "traded" pressure Data.
   Ptraded_data_pt=0;
  } 

 /// Access function to the prescribed volume fluid 
 double*& volume_pt() {return Volume_pt;}

 /// \short Assign the local equation numbers and their coincidence with
 /// the global values
 void assign_additional_local_eqn_numbers()
  {
 //If there is no "traded" pressure data assigned, set 
 //Ptraded_local_equation to -1
 if(Ptraded_data_pt==0)
  {
   Ptraded_local_eqn = -1;
  }
 //Otherwise, copy its local equation number across from the generic
 //equation numbering scheme -- here we're relying on the fact that 
 //the relevant external Data item only stores a single value.
 else
  {
   Ptraded_local_eqn =
    this->external_local_eqn(External_data_number_of_traded_pressure,0);
  }
 
 //Local equation numbering for the single spine is done automatically
 //in the underlying SpineElement.
  }


 /// Fill in the contribution to the residuals
 void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
   int local_eqn = Ptraded_local_eqn;
   if(local_eqn >= 0) 
    {
     residuals[local_eqn] -= *Volume_pt;
    }
  }
 
 /// No contribution to the jacabian
 void fill_in_contribution_to_jacobian(Vector<double> &residuals, 
                                       DenseMatrix<double> &jacobian)
  {
   //Get the residuals
   fill_in_contribution_to_residuals(residuals);
   //No contribution to jacbian
  }
 
 /// \short Set the Data that contains the single pressure value
 /// that is "traded" for the volume constraint.
 /// The Data item must only contain a single value!
 void set_traded_pressure_data(Data* traded_pressure_data_pt)
  {
#ifdef PARANOID
   if (traded_pressure_data_pt->nvalue()!=1)
    {
     std::ostringstream error_stream;
     error_stream 
      << "The `traded` pressure Data must only contain a single value\n"
      << "This one contains " << traded_pressure_data_pt->nvalue() 
      << std::endl;
     
     std::string function_name = 
      "SpineVolumConstraintPointElement::\n";
     function_name += "set_traded_pressure_data()";
     
     throw OomphLibError(error_stream.str(),function_name,
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif
     // Store pointer explicitly
     Ptraded_data_pt=traded_pressure_data_pt;
     // Add to the element's external data so it gets included
     // in the black-box local equation numbering scheme
     External_data_number_of_traded_pressure = 
      this->add_external_data(traded_pressure_data_pt);
  }

}; 



//==========================================================================
/// A \b preliminary class that is used to implement the constraint 
/// that the fluid volume must take a specific value for a Spine-based 
/// formulation. This is required in steady free-surface/interface
/// problems, where, in general, a family of solutions may exist.
/// \n\n
/// Note that as written, the function assumes that the mesh contains
/// three-straight boundaries at (x(0) = 0, x(1) = 0, x[0] = L) and
/// one free surface. In order to implement the constraint in total generality,
/// one would need to introduce surface elements all round the mesh
/// and integrate x.n along them. This is not necessary in the special case
/// described above, because the integrals along the three straight edges
/// are easily evaluated. The first two are zero and the third is
/// included in the residuals calculated in this element.
//=========================================================================
template<class ELEMENT>
class SpineVolumeConstraintLineElement : 
public SpineLineFluidInterfaceEdgeElement<ELEMENT>
{
  private:
 
 /// Pointer to the desired value of the volume
 double *Volume_pt;
 
 /// \short The local eqn number for the pressure that has been
 /// "traded" for the volume constraint
 int Ptraded_local_eqn;

 /// \short Pointer to the Data item that stores the pressure that has been
 /// "traded" for the volume constraint in its single value.
 Data* Ptraded_data_pt;

 /// \short The Data that contains the traded pressure is stored
 /// as external Data for the element. Which external Data item is it?
 unsigned External_data_number_of_traded_pressure;

  public:

 ///Constructor, there are no internal values. The pointer to the 
 ///element's (single) spine has to be set manually "from the outside"
 SpineVolumeConstraintLineElement() : 
  SpineLineFluidInterfaceEdgeElement<ELEMENT>() 
  {
   // Initialise pointer to prescribed volume of fluid
   Volume_pt=0;
   // Initialise pointer to "traded" pressure Data.
   Ptraded_data_pt=0;
  } 

 /// Access function to the prescribed volume fluid 
 double*& volume_pt() {return Volume_pt;}

 /// \short Assign the local equation numbers and their coincidence with
 /// the global values
 void assign_additional_local_eqn_numbers()
  {
 //If there is no "traded" pressure data assigned, set 
 //Ptraded_local_equation to -1
 if(Ptraded_data_pt==0)
  {
   Ptraded_local_eqn = -1;
  }
 //Otherwise, copy its local equation number across from the generic
 //equation numbering scheme -- here we're relying on the fact that 
 //the relevant external Data item only stores a single value.
 else
  {
   Ptraded_local_eqn =
    this->external_local_eqn(External_data_number_of_traded_pressure,0);
  }
 
 //Local equation numbering for the single spine is done automatically
 //in the underlying SpineElement.
  }

 void add_additional_residual_contributions(
  Vector<double> &residuals, DenseMatrix<double> &jacobian,
  const unsigned &flag,const Shape &psif,
  const DShape &dpsifds,
  const Vector<double> &interpolated_n, const double &W) 
  {
   int local_eqn = Ptraded_local_eqn;
   if(local_eqn >= 0)
    {
     Vector<double> x(3,0.0);
     const unsigned n_node = this->nnode();
     
     for(unsigned n=0;n<n_node;n++)
      {
       for(unsigned i=0;i<3;i++)
        {
         x[i] += this->nodal_position(n,i)*psif(n);
        }
      }

     //This is the component of x.n (divide by three because we
     //are working in 3D)
     residuals[local_eqn] += x[2]*W/3.0;
    }
  }


 /// \short Set the Data that contains the single pressure value
 /// that is "traded" for the volume constraint.
 /// The Data item must only contain a single value!
 void set_traded_pressure_data(Data* traded_pressure_data_pt)
  {
#ifdef PARANOID
   if (traded_pressure_data_pt->nvalue()!=1)
    {
     std::ostringstream error_stream;
     error_stream 
      << "The `traded` pressure Data must only contain a single value\n"
      << "This one contains " << traded_pressure_data_pt->nvalue() 
      << std::endl;
     
     std::string function_name = 
      "SpineVolumConstraintPointElement::\n";
     function_name += "set_traded_pressure_data()";
     
     throw OomphLibError(error_stream.str(),function_name,
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif
     // Store pointer explicitly
     Ptraded_data_pt=traded_pressure_data_pt;
     // Add to the element's external data so it gets included
     // in the black-box local equation numbering scheme
     External_data_number_of_traded_pressure = 
      this->add_external_data(traded_pressure_data_pt);
  }

}; 



//========================================================================
/// New one-dimensional interface elements that assemble the fixed volume
/// constraint in addition to the standard free-surface equations. The
/// class simply inherits from SpineLineFluidInterfaceElements
/// adds an additional unknown that is traded for the volume constraint
/// and overloads the residual assembly scheme.
/// Note that the implementation is rather specific for our meshes used
/// in this demo driver. You may have to alter this class for different
/// problems.
//======================================================================
template<class ELEMENT>
class FixedVolumeSpineSurfaceFluidInterfaceElement : 
public SpineSurfaceFluidInterfaceElement<ELEMENT>
{
  private:

 /// \short Integer to hold the local equation number for the single pressure
 /// value that has been traded for the volume constraint.
 int Ptraded_local_eqn;

 /// \short The Data that contains the traded pressure is stored
 /// as external Data for the element. Which external Data item is it?
 unsigned External_data_number_of_traded_pressure;

 /// \short Pointer to the Data item that stores the pressure that has been
 /// "traded" for the volume constraint in its single value.
 Data* Ptraded_data_pt;


 /// \short Overload the Helper function to calculate the residuals and 
 /// jacobian entries. This particular function ensures that the
 /// additional entries are calculated inside the integration loop
 void add_additional_residual_contributions(
  Vector<double> &residuals, DenseMatrix<double> &jacobian,
  const unsigned &flag,
  const Shape &psif, const DShape &dpsifds,
  const Vector<double> &interpolated_n, const double &W)
  {
   //Add in the volume constraint term if required -- HACKY!
   //The volume of the fluid is found by integrating x.n around the boundary
   int local_eqn= Ptraded_local_eqn; 
   if(local_eqn >=0)
    {
     //Find out how many nodes there are
     unsigned n_node = this->nnode();
     
     //Find the x position
     Vector<double> interpolated_x(3,0.0);
     //Fill in the x position
     for(unsigned l=0;l<n_node;l++)
      {
       const double psi_local = psif(l);
       for(unsigned i=0;i<3;i++)
        {
         interpolated_x[i] += this->nodal_position(l,i)*psi_local;
        }
      }
     
     //Find the dot product
     double dot = 0.0;
     for(unsigned k=0;k<3;k++) {dot += interpolated_x[k]*interpolated_n[k];}

     //This is the component of x.n (divide by 3 because we are working
     //in 3D)
     residuals[local_eqn] += dot*W/3.0;
    }
  }
 
public:

 /// Constructor, the arguments are a pointer to the  "bulk" element 
 /// and the index of the face
 FixedVolumeSpineSurfaceFluidInterfaceElement(
  FiniteElement* const &element_pt, const int &face_index) : 
  SpineSurfaceFluidInterfaceElement<ELEMENT>
  (element_pt,face_index)
  {
   // Initialise pointer to pressure data that has been traded
   // for volume constraint
   Ptraded_data_pt=0;
  }

 /// Assign the local equation numbers arrays
 void assign_additional_local_eqn_numbers()
  {
    //If there is no "traded" pressure data assigned, set 
 //Ptraded_local_equation to -1
 if(Ptraded_data_pt==0)
  {
   Ptraded_local_eqn=-1;
  }
 //Otherwise, copy its local equation number across from the generic
 //equation numbering scheme 
 else
  {
#ifdef PARANOID
   if (Ptraded_data_pt->nvalue()!=1)
    {
     std::ostringstream error_stream;
     error_stream 
      << "The `traded` pressure item should only have a single\n"
      <<"value but is has " << Ptraded_data_pt->nvalue() 
      << std::endl;
     
     std::string function_name = 
      "FixedVolumeSpineLineFluidInterfaceElement::\n";
     function_name += "assign_additional_local_eqn_numbers()";

     throw OomphLibError(error_stream.str(),function_name,
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif
   Ptraded_local_eqn = 
    this->external_local_eqn(External_data_number_of_traded_pressure,0);
  }
  }

 /// \short Set the Data that contains the single pressure value
 /// that is "traded" for the volume constraint.
 /// The Data item must only contain a single value!
 void set_traded_pressure_data(Data* traded_pressure_data_pt)
  {
#ifdef PARANOID
   if (traded_pressure_data_pt->nvalue()!=1)
    {
     std::ostringstream error_stream;
     error_stream 
      << "The `traded` pressure Data must only contain a single value\n"
      << "This one contains " << traded_pressure_data_pt->nvalue() 
      << std::endl;
     
     std::string function_name = 
      "FixedVolumeSpineLineFluidInterfaceElement::\n";
     function_name += "set_traded_pressure_data()";
     
     throw OomphLibError(error_stream.str(),function_name,
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif
   // Store pointer explicitly
   Ptraded_data_pt=traded_pressure_data_pt;
   // Add the traded pressure to the external data.
   // If it's a duplicate it will be reported as such
   External_data_number_of_traded_pressure =
    this->add_external_data(traded_pressure_data_pt);
  }

  
 /// \short Create a volume constraint element at face of the element given
 /// by the fac_index
 FluidInterfaceEdgeElement*
  make_edge_element(const int &face_index)
  {
   //Create a temporary pointer to the appropriate FaceElement
   SpineVolumeConstraintLineElement<ELEMENT> *Temp_pt =
    new SpineVolumeConstraintLineElement<ELEMENT>;
   

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

}

#endif






