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
//Non-inline member functions for Qelements

//Include the appropriate headers
#include "Qelements.h"
#include "Qelement_face_coordinate_translation_schemes.h"

namespace oomph
{


//Assign the static dimension of each node
template<unsigned NNODE_1D>
const unsigned QElement<1,NNODE_1D>::Every_node_ndim=1;

 /// Assign min value of local coordinate
template<unsigned NNODE_1D>
const double QElement<1,NNODE_1D>::S_min=-1.0;

 /// Assign max. value of local coordinate
template<unsigned NNODE_1D>
const double QElement<1,NNODE_1D>::S_max=1.0;

//=======================================================================
/// Assign the static Default_integration_scheme
//=======================================================================
template<unsigned NNODE_1D>
Gauss<1,NNODE_1D> QElement<1,NNODE_1D>::Default_integration_scheme;

//=======================================================================
///Shape function for specific QElement<1,..>
//=======================================================================
template <unsigned NNODE_1D>
void QElement<1,NNODE_1D>::shape(const Vector<double> &s, Shape &psi) 
 const
{
 //Local storage for the shape functions
 double Psi[NNODE_1D];
 //Call the OneDimensional Shape functions
 OneDimLagrange::shape<NNODE_1D>(s[0],Psi);

 //Now let's loop over the nodal points in the element
 //and copy the values back in  
 for(unsigned i=0;i<NNODE_1D;i++) {psi[i] = Psi[i];}
}

//=======================================================================
///Derivatives of shape functions for specific  QElement<1,..>
//=======================================================================
template <unsigned NNODE_1D>
void QElement<1,NNODE_1D>::dshape_local(const Vector<double> &s, Shape &psi, 
                                 DShape &dpsids) const
{
 //Local storage
 double Psi[NNODE_1D];
 double DPsi[NNODE_1D];
 //Call the shape functions and derivatives
 OneDimLagrange::shape<NNODE_1D>(s[0],Psi);
 OneDimLagrange::dshape<NNODE_1D>(s[0],DPsi);

 //Loop over shape functions in element and assign to psi
 for(unsigned l=0;l<NNODE_1D;l++) 
  {
   psi[l] = Psi[l];
   dpsids(l,0) = DPsi[l];
  }
}

//=======================================================================
/// Second derivatives of shape functions for specific  QElement<1,..>: \n
/// d2psids(i,0) = \f$ d^2 \psi_j / d s^2 \f$
//=======================================================================
template <unsigned NNODE_1D>
void QElement<1,NNODE_1D>::d2shape_local(const Vector<double> &s, Shape &psi, 
                                         DShape &dpsids, DShape &d2psids) const
{
 //Local storage for the shape functions
 double Psi[NNODE_1D];
 double DPsi[NNODE_1D];
 double D2Psi[NNODE_1D];
 //Call the shape functions and derivatives
 OneDimLagrange::shape<NNODE_1D>(s[0],Psi);
 OneDimLagrange::dshape<NNODE_1D>(s[0],DPsi);
 OneDimLagrange::d2shape<NNODE_1D>(s[0],D2Psi);

 //Loop over shape functions in element and assign to psi
 for(unsigned l=0;l<NNODE_1D;l++) 
  {
   psi[l] = Psi[l];
   dpsids(l,0) = DPsi[l];
   d2psids(l,0) = D2Psi[l];
  }
}

//=======================================================================
/// The output function for general 1D QElements
//=======================================================================
template <unsigned NNODE_1D>
void QElement<1,NNODE_1D>::output(std::ostream &outfile)
{
 //Tecplot header info 
 outfile << "ZONE I=" << NNODE_1D << std::endl;

 //Find the dimension of the node
 unsigned n_dim = this->nodal_dimension();

 //Loop over element nodes
 for(unsigned l=0;l<NNODE_1D;l++)
  {
   //Loop over the dimensions and output the position
   for(unsigned i=0;i<n_dim;i++)
    {
     outfile << node_pt(l)->x(i) << " ";
    }
   //Find out how many data values at the node
   unsigned initial_nvalue = node_pt(l)->nvalue();
   //Lopp over the data and output whether pinned or not
   for(unsigned i=0;i<initial_nvalue;i++)
    {
     outfile << node_pt(l)->is_pinned(i) << " ";
    }
   outfile << std::endl;
  }
  outfile << std::endl;
}

//=======================================================================
/// The output function for n_plot points in each coordinate direction
//=======================================================================
template <unsigned NNODE_1D>
void QElement<1,NNODE_1D>::output(std::ostream &outfile, 
                                  const unsigned &n_plot)
{
 //Local variables
 Vector<double> s(1);

 //Tecplot header info 
 outfile << "ZONE I=" << n_plot << std::endl;

 //Find the dimension of the nodes
 unsigned n_dim = this->nodal_dimension();

 //Loop over plot points
 for(unsigned l=0;l<n_plot;l++)
  {
   s[0] = -1.0 + l*2.0/(n_plot-1);
   //Output the coordinates
   for (unsigned i=0;i<n_dim;i++)
    {
     outfile << interpolated_x(s,i) << " " ;
    }
   outfile <<  std::endl;
  }
  outfile << std::endl;
}



//=======================================================================
/// C style output function for general 1D QElements
//=======================================================================
template <unsigned NNODE_1D>
void QElement<1,NNODE_1D>::output(FILE* file_pt)
{

 //Tecplot header info 
 fprintf(file_pt,"ZONE I=%i\n",NNODE_1D);

 //Find the dimension of the nodes
 unsigned n_dim = this->nodal_dimension();

 //Loop over element nodes
 for(unsigned l=0;l<NNODE_1D;l++)
  {
   //Loop over the dimensions and output the position
   for(unsigned i=0;i<n_dim;i++)
    {
     //outfile << Node_pt[l]->x(i) << " ";
     fprintf(file_pt,"%g ",node_pt(l)->x(i));
    }
   //Find out how many data values at the node
   unsigned initial_nvalue = node_pt(l)->nvalue();
   //Lopp over the data and output whether pinned or not
   for(unsigned i=0;i<initial_nvalue;i++)
    {
     //outfile << Node_pt[l]->is_pinned(i) << " ";
     fprintf(file_pt,"%i ",node_pt(l)->is_pinned(i));
    }
   //outfile << std::endl;
   fprintf(file_pt,"\n");
  }
 //outfile << std::endl;
 fprintf(file_pt,"\n");
}

//=======================================================================
/// C style output function for n_plot points in each coordinate direction
//=======================================================================
template <unsigned NNODE_1D>
void QElement<1,NNODE_1D>::output(FILE* file_pt, const unsigned &n_plot)
{
 //Local variables
 Vector<double> s(1);

 //Tecplot header info 
 //outfile << "ZONE I=" << n_plot << std::endl;
 fprintf(file_pt,"ZONE I=%i\n",n_plot);

 //Find the dimension of the first node
 unsigned n_dim = this->nodal_dimension();

 //Loop over plot points
 for(unsigned l=0;l<n_plot;l++)
  {
   s[0] = -1.0 + l*2.0/(n_plot-1);

   //Output the coordinates
   for (unsigned i=0;i<n_dim;i++)
    {
     //outfile << interpolated_x(s,i) << " " ;
     fprintf(file_pt,"%g ",interpolated_x(s,i));
    }
   //outfile <<  std::endl;
    fprintf(file_pt,"\n");
  }
 //outfile << std::endl;
 fprintf(file_pt,"\n");
}


//===========================================================
/// Function to setup geometrical information for lower-dimensional 
/// FaceElements (which are of type QElement<0,NNODE_1D>).
//===========================================================
template<unsigned NNODE_1D>
void QElement<1,NNODE_1D>::build_face_element(const int &face_index,
                                              FaceElement *face_element_pt)
{
 // Overload the nodal dimension by reading out the value from the node
 face_element_pt->set_nodal_dimension(node_pt(0)->ndim());
 
 // Set the pointer to the "bulk" element
 face_element_pt->bulk_element_pt()=this;
 
 // Resize the storage for the original number of values at the (one and only)
 // node of the face element.
 face_element_pt->nbulk_value_resize(1);

 // Resize the storage for the bulk node number corresponding to the (one
 // and only) node of the face element
 face_element_pt->bulk_node_number_resize(1);
 
 //Set the face index in the face element
 face_element_pt->face_index() = face_index;

 //Now set up the node pointer
 //The convention is that the "normal", should always point
 //out of the element
 switch(face_index)
  {
   //Bottom, normal sign is negative (coordinate points into element)
  case(-1):
   face_element_pt->node_pt(0) = node_pt(0);
   face_element_pt->bulk_node_number(0) = 0;
   face_element_pt->normal_sign() = -1;
   
   //Set the pointer to the function that determines the bulk coordinates
   //in the face element
   face_element_pt->face_to_bulk_coordinate_fct_pt() = 
    &QElement1FaceToBulkCoordinates::face0;

   //Set the pointer to the function that determines the mapping of
   //derivatives
   face_element_pt->bulk_coordinate_derivatives_fct_pt() =
    &QElement1BulkCoordinateDerivatives::faces0;


   //Set the number of values stored when the node is part of the "bulk"
   //element. The required_nvalue() must be used, rather than nvalue(),
   //because otherwise nodes on boundaries will be resized multiple 
   //times. If you want any other behaviour, you MUST set nbulk_value()
   //manually after construction of your specific face element.
   face_element_pt->nbulk_value(0) = required_nvalue(0);
   break;
   
   //Top, normal sign is positive (coordinate points out of element)
  case(1):
   face_element_pt->node_pt(0) = node_pt(NNODE_1D-1);
   face_element_pt->bulk_node_number(0) = NNODE_1D-1;
   face_element_pt->normal_sign() = +1;

      
   //Set the pointer to the function that determines the bulk coordinates
   //in the face element
   face_element_pt->face_to_bulk_coordinate_fct_pt() = 
    &QElement1FaceToBulkCoordinates::face1;

   //Set the pointer to the function that determines the mapping of
   //derivatives
   face_element_pt->bulk_coordinate_derivatives_fct_pt() =
    &QElement1BulkCoordinateDerivatives::faces0;
   

   //Set the number of values stored when the node is part of the "bulk"
   //element.
   face_element_pt->nbulk_value(0) = required_nvalue(NNODE_1D-1);
   break;
   
   //All other cases, throw an error
  default:
   std::ostringstream error_message;
   error_message << "Face_index should only take "
                 << "the values +/-1, not " << face_index << std::endl;

   throw OomphLibError(error_message.str(),
                       "QElement<1,..>::build_face_element()",
                       OOMPH_EXCEPTION_LOCATION);
  }
}

////////////////////////////////////////////////////////////////
//       2D Qelements
////////////////////////////////////////////////////////////////

/// Assign min value of local coordinate
template<unsigned NNODE_1D>
const double QElement<2,NNODE_1D>::S_min=-1.0;

/// Assign max. value of local coordinate
template<unsigned NNODE_1D>
const double QElement<2,NNODE_1D>::S_max=1.0;

//Assign the static dimension of each node
template<unsigned NNODE_1D>
const unsigned QElement<2,NNODE_1D>::Every_node_ndim=2;

/// Assign the spatial integration scheme
template<unsigned NNODE_1D>
Gauss<2,NNODE_1D> QElement<2,NNODE_1D>::Default_integration_scheme;


//==================================================================
/// Return the node at the specified local coordinate
//==================================================================
template<unsigned NNODE_1D>
Node* QElement<2,NNODE_1D>::
get_node_at_local_coordinate(const Vector<double> &s)
{
 //Load the tolerance into a local variable
 double tol = FiniteElement::Node_location_tolerance;
 //There are two possible indices.
 Vector<int> index(2);
 //Loop over the coordinates and determine the indices
 for(unsigned i=0;i<2;i++)
  {
   //If we are at the lower limit, the index is zero
   if(std::abs(s[i] + 1.0) < tol)
    {
     index[i] = 0;
    }
   //If we are at the upper limit, the index is the number of nodes minus 1
   else if(std::abs(s[i] - 1.0) < tol)
    {
     index[i] = NNODE_1D-1;
    }
   //Otherwise, we have to calculate the index in general
   else
    {
     //For uniformly spaced nodes the node number would be
     double float_index = 0.5*(1.0 + s[i])*(NNODE_1D-1);
     //Convert to an integer by taking the floor (rounding down) 
     index[i] = static_cast<int>(std::floor(float_index));
     //What is the excess. This should be safe because the
     //we have rounded down
     double excess = float_index - index[i];
     //If the excess is bigger than our tolerance there is no node,
     //return null
     //Note that we test at both lower and upper ends.
     if((excess > tol) && ((1.0 - excess) > tol))
      {
       return 0;
      }
     //If we are at the upper end (i.e. the system has actually rounded up)
     //we need to add one to the index
     if((1.0 - excess) <= tol) {index[i] += 1;}
    }
  }
 //If we've got here we have a node, so let's return a pointer to it
 return node_pt(index[0] + NNODE_1D*index[1]);
}



//=======================================================================
/// Shape function for specific QElement<2,..>
///
//=======================================================================
template <unsigned NNODE_1D>
void QElement<2,NNODE_1D>::shape(const Vector<double> &s, Shape &psi) 
 const
{
 //Local storage
 double Psi[2][NNODE_1D];
 //Call the OneDimensional Shape functions
 OneDimLagrange::shape<NNODE_1D>(s[0],Psi[0]);
 OneDimLagrange::shape<NNODE_1D>(s[1],Psi[1]);
 //Index for the shape functions
 unsigned index=0;

 //Now let's loop over the nodal points in the element
 //s1 is the "x" coordinate, s2 the "y" 
 for(unsigned i=0;i<NNODE_1D;i++)
  {
   for(unsigned j=0;j<NNODE_1D;j++)
    {
     //Multiply the two 1D functions together to get the 2D function
     psi[index] = Psi[1][i]*Psi[0][j];
     //Incremenet the index
     ++index;
    }
  }
}

//=======================================================================
///Derivatives of shape functions for specific  QElement<2,..>
//=======================================================================
template <unsigned NNODE_1D>
void QElement<2,NNODE_1D>::dshape_local(const Vector<double> &s, Shape &psi, 
                                 DShape &dpsids) const
{
 //Local storage
 double Psi[2][NNODE_1D];
 double DPsi[2][NNODE_1D];
 unsigned index=0;

 //Call the shape functions and derivatives
 OneDimLagrange::shape<NNODE_1D>(s[0],Psi[0]);
 OneDimLagrange::shape<NNODE_1D>(s[1],Psi[1]);
 OneDimLagrange::dshape<NNODE_1D>(s[0],DPsi[0]);
 OneDimLagrange::dshape<NNODE_1D>(s[1],DPsi[1]);

 //Loop over shape functions in element
 for(unsigned i=0;i<NNODE_1D;i++)
  {
   for(unsigned j=0;j<NNODE_1D;j++)
    {
     //Assign the values
     dpsids(index,0) =  Psi[1][i]*DPsi[0][j];
     dpsids(index,1) = DPsi[1][i]* Psi[0][j];
     psi[index]      =  Psi[1][i]* Psi[0][j];
     //Increment the index
     ++index;
    }
  }
}


//=======================================================================
/// Second derivatives of shape functions for specific  QElement<2,..>: \n
/// d2psids(i,0) = \f$ \partial^2 \psi_j / \partial s_0^2 \f$ \n
/// d2psids(i,1) = \f$ \partial^2 \psi_j / \partial s_1^2 \f$ \n
/// d2psids(i,2) = \f$ \partial^2 \psi_j / \partial s_0 \partial s_1 \f$ \n
//=======================================================================
template <unsigned NNODE_1D>
void QElement<2,NNODE_1D>::d2shape_local(const Vector<double> &s, Shape &psi, 
                                         DShape &dpsids, DShape &d2psids) const
{
 //Local storage
 double Psi[2][NNODE_1D];
 double DPsi[2][NNODE_1D];
 double D2Psi[2][NNODE_1D];
 //Index for the assembly process
 unsigned index=0;
 
 //Call the shape functions and derivatives
 OneDimLagrange::shape<NNODE_1D>(s[0],Psi[0]);
 OneDimLagrange::shape<NNODE_1D>(s[1],Psi[1]);
 OneDimLagrange::dshape<NNODE_1D>(s[0],DPsi[0]);
 OneDimLagrange::dshape<NNODE_1D>(s[1],DPsi[1]);
 OneDimLagrange::d2shape<NNODE_1D>(s[0],D2Psi[0]);
 OneDimLagrange::d2shape<NNODE_1D>(s[1],D2Psi[1]);

 //Loop over shape functions in element
 for(unsigned i=0;i<NNODE_1D;i++)
  {
   for(unsigned j=0;j<NNODE_1D;j++)
    {
     //Assign the values
     psi[index]       = Psi[1][i]*Psi[0][j];
     //First derivatives
     dpsids(index,0) =  Psi[1][i]*DPsi[0][j];
     dpsids(index,1) = DPsi[1][i]* Psi[0][j];
     //Second derivatives 
     //N.B. index 2 is the mixed derivative
     d2psids(index,0) =   Psi[1][i]*D2Psi[0][j];
     d2psids(index,1) = D2Psi[1][i]*  Psi[0][j];
     d2psids(index,2) =  DPsi[1][i]* DPsi[0][j];
     //Increment the index
     ++index;
    }
  }
}




//===========================================================
/// The output function for QElement<2,NNODE_1D>
//===========================================================
template <unsigned NNODE_1D>
void QElement<2,NNODE_1D>::output(std::ostream &outfile)
{
 //Find the dimension of the nodes
 unsigned n_dim = this->nodal_dimension();
 //Node number
 unsigned l=0;

 //Tecplot header info 
 outfile << "ZONE I=" << NNODE_1D << ", J=" << NNODE_1D << std::endl;
 //Loop over element nodes
 for(unsigned l2=0;l2<NNODE_1D;l2++)
  {
   for(unsigned l1=0;l1<NNODE_1D;l1++)
    {
     //Loop over the dimensions and output the position
     for(unsigned i=0;i<n_dim;i++)
      {
       outfile << node_pt(l)->x(i) << " ";
      }
     //Find out how many data values at the node
     unsigned initial_nvalue = node_pt(l)->nvalue();
     //Loop over the data and output whether pinned or not
     for(unsigned i=0;i<initial_nvalue;i++)
      {
       outfile << node_pt(l)->is_pinned(i) << " ";
      }
     outfile << std::endl;

     //Increase the node number
     ++l;
    }
  }
  outfile << std::endl;
}

//=======================================================================
/// The output function for n_plot points in each coordinate direction
//=======================================================================
template <unsigned NNODE_1D>
void QElement<2,NNODE_1D>::output(std::ostream &outfile, 
                                  const unsigned &n_plot)
{
 //Local variables
 Vector<double> s(2);

 //Tecplot header info 
 outfile << "ZONE I=" << n_plot << ", J=" << n_plot << std::endl;

 //Find the dimension of the nodes
 unsigned n_dim = this->nodal_dimension();

 //Loop over plot points
 for(unsigned l2=0;l2<n_plot;l2++)
  {
   s[1] = -1.0 + l2*2.0/(n_plot-1);
   for(unsigned l1=0;l1<n_plot;l1++)
    {
     s[0] = -1.0 + l1*2.0/(n_plot-1);
     
     //Output the coordinates
     for (unsigned i=0;i<n_dim;i++)
      {
       outfile << interpolated_x(s,i) << " " ;
      }
     outfile << std::endl;
    }
  }
  outfile << std::endl;
}




//===========================================================
/// C-style output function for QElement<2,NNODE_1D>
//===========================================================
template <unsigned NNODE_1D>
void QElement<2,NNODE_1D>::output(FILE* file_pt)
{
 //Tecplot header info 
 //outfile << "ZONE I=" << NNODE_1D << ", J=" << NNODE_1D << std::endl;
 fprintf(file_pt,"ZONE I=%i, J=%i\n",NNODE_1D,NNODE_1D);

 //Find the dimensions of the nodes
 unsigned n_dim = this->nodal_dimension();
 //Node number
 unsigned l=0;

 //Loop over element nodes
 for(unsigned l2=0;l2<NNODE_1D;l2++)
  {
   for(unsigned l1=0;l1<NNODE_1D;l1++)
    {
     //Loop over the dimensions and output the position
     for(unsigned i=0;i<n_dim;i++)
      {
       //outfile << Node_pt[l]->x(i) << " ";
       fprintf(file_pt,"%g ",node_pt(l)->x(i));
      }
     //Find out how many data values at the node
     unsigned initial_nvalue = node_pt(l)->nvalue();
     //Loop over the data and output whether pinned or not
     for(unsigned i=0;i<initial_nvalue;i++)
      {
       //outfile << Node_pt[l]->is_pinned(i) << " ";
       fprintf(file_pt,"%i ",node_pt(l)->is_pinned(i));
      }
     //outfile << std::endl;
     fprintf(file_pt,"\n");
     //Increase the node number
     ++l;
    }
  }
 //outfile << std::endl;
 fprintf(file_pt,"\n");
}

//=======================================================================
/// C-style  output function for n_plot points in each coordinate direction
//=======================================================================
template <unsigned NNODE_1D>
void QElement<2,NNODE_1D>::output(FILE* file_pt, const unsigned &n_plot)
{
 //Local variables
 Vector<double> s(2);

 //Find the dimension of the nodes
 unsigned n_dim = this->nodal_dimension();

 //Tecplot header info 
 //outfile << "ZONE I=" << n_plot << ", J=" << n_plot << std::endl;
 fprintf(file_pt,"ZONE I=%i, J=%i\n",n_plot,n_plot);

 //Loop over element nodes
 for(unsigned l2=0;l2<n_plot;l2++)
  {
   s[1] = -1.0 + l2*2.0/(n_plot-1);
   for(unsigned l1=0;l1<n_plot;l1++)
    {
     s[0] = -1.0 + l1*2.0/(n_plot-1);
    
     //Output the coordinates
     for (unsigned i=0;i<n_dim;i++)
      {
       //outfile << interpolated_x(s,i) << " " ;
       fprintf(file_pt,"%g ",interpolated_x(s,i));
       
      }
     //outfile << std::endl;
     fprintf(file_pt,"\n");
    }
  }
 //outfile << std::endl;
 fprintf(file_pt,"\n");
}

//=======================================================================
/// \short Function to setup geometrical information for lower-dimensional 
/// FaceElement (which are of type QElement<1,NNODE_1D>).
//=======================================================================
template<unsigned NNODE_1D>
void QElement<2,NNODE_1D>::build_face_element(const int &face_index,
                                              FaceElement* face_element_pt)
{

 // Set the nodal dimension from the first node
 face_element_pt->set_nodal_dimension(node_pt(0)->ndim());

 // Set the pointer to the "bulk" element
 face_element_pt->bulk_element_pt()=this;

 // Resize the storage for the original number of values at 
 // NNODE_1D nodes of the FaceElement
 face_element_pt->nbulk_value_resize(NNODE_1D);

 // Resize the storage for the bulk node numbers corresponding
 // to the NNODE_1D nodes of the FaceElement
 face_element_pt->bulk_node_number_resize(NNODE_1D);

 // Set the face index in the face element
 // The faces are
 //                       +1    East       
 //                       -1    West
 //                       +2    North
 //                       -2    South

 face_element_pt->face_index() = face_index;


 //Now set up the node pointers
 //The convention here is that interior_tangent X tangent X tangent 
 //is the OUTWARD normal
 switch(face_index)
  {
   unsigned bulk_number;
   //West face, normal sign is positive
  case(-1):
   //Set the pointer to the bulk coordinate translation scheme
   face_element_pt->face_to_bulk_coordinate_fct_pt() = 
    &QElement2FaceToBulkCoordinates::face0;

   //Set the pointer to the derivative mappings
   face_element_pt->bulk_coordinate_derivatives_fct_pt() = 
    &QElement2BulkCoordinateDerivatives::faces0;

   for(unsigned i=0;i<NNODE_1D;i++) 
    {
     bulk_number = i*NNODE_1D;
     face_element_pt->node_pt(i) = node_pt(bulk_number);
     face_element_pt->bulk_node_number(i) = bulk_number;
     face_element_pt->normal_sign() = 1;
     //Set the number of values originally stored at this node
     face_element_pt->nbulk_value(i) = required_nvalue(bulk_number);
    }
   break;
   //South face, normal sign is positive
  case(-2):
   //Set the pointer to the bulk coordinate translation scheme
   face_element_pt->face_to_bulk_coordinate_fct_pt() = 
    &QElement2FaceToBulkCoordinates::face1;

   //Set the pointer to the derivative mappings
   face_element_pt->bulk_coordinate_derivatives_fct_pt() = 
    &QElement2BulkCoordinateDerivatives::faces1;
   
   for(unsigned i=0;i<NNODE_1D;i++) 
    {
     bulk_number = i;
     face_element_pt->node_pt(i) = node_pt(bulk_number);
     face_element_pt->bulk_node_number(i) = bulk_number;
     face_element_pt->normal_sign() = 1;
     //Set the number of values originally stored at this node
     face_element_pt->nbulk_value(i) = required_nvalue(bulk_number);
    }
   break;
   //East face, normal sign is negative
  case(1):
   //Set the pointer to the bulk coordinate translation scheme
   face_element_pt->face_to_bulk_coordinate_fct_pt() = 
    &QElement2FaceToBulkCoordinates::face2;
   
   //Set the pointer to the derivative mappings
   face_element_pt->bulk_coordinate_derivatives_fct_pt() = 
    &QElement2BulkCoordinateDerivatives::faces0;
   
   for(unsigned i=0;i<NNODE_1D;i++) 
    {
     bulk_number = NNODE_1D*i + NNODE_1D-1;
     face_element_pt->node_pt(i) = node_pt(bulk_number);
     face_element_pt->bulk_node_number(i) = bulk_number;
     face_element_pt->normal_sign() = -1;
     //Set the number of values originally stored at this node
     face_element_pt->nbulk_value(i) = required_nvalue(bulk_number);
    }
   break;
   //North face, normal sign is negative
  case(2):
   //Set the pointer to the bulk coordinate translation scheme
   face_element_pt->face_to_bulk_coordinate_fct_pt() = 
    &QElement2FaceToBulkCoordinates::face3;
   
   //Set the pointer to the derivative mappings
   face_element_pt->bulk_coordinate_derivatives_fct_pt() = 
    &QElement2BulkCoordinateDerivatives::faces1;

   for(unsigned i=0;i<NNODE_1D;i++)
    {
     bulk_number = NNODE_1D*(NNODE_1D-1) + i;
     face_element_pt->node_pt(i) = node_pt(bulk_number);
     face_element_pt->bulk_node_number(i) = bulk_number;
     face_element_pt->normal_sign() = -1;
     //Set the number of values originally stored at this node
     face_element_pt->nbulk_value(i) = required_nvalue(bulk_number);
    }
   break;

   //Now cover the other cases
  default:
   std::ostringstream error_message;
   error_message << "Face index should only take the values +/- 1 or +/- 2,"
                 << " not " << face_index << std::endl;
   throw OomphLibError(error_message.str(),
                       "QElement<2,..>::build_face_element",
                       OOMPH_EXCEPTION_LOCATION);
  }
}



////////////////////////////////////////////////////////////////
//       3D Qelements
////////////////////////////////////////////////////////////////


/// Assign min value of local coordinate
template<unsigned NNODE_1D>
const double QElement<3,NNODE_1D>::S_min=-1.0;

/// Assign max. value of local coordinate
template<unsigned NNODE_1D>
const double QElement<3,NNODE_1D>::S_max=1.0;

//Assign the static dimension of each node
template<unsigned NNODE_1D>
const unsigned QElement<3,NNODE_1D>::Every_node_ndim=3;


/// Assign the spatial integration scheme
template<unsigned NNODE_1D>
Gauss<3,NNODE_1D> QElement<3,NNODE_1D>::Default_integration_scheme;

//==================================================================
/// Return the node at the specified local coordinate
//==================================================================
template<unsigned NNODE_1D>
Node* QElement<3,NNODE_1D>::
get_node_at_local_coordinate(const Vector<double> &s)
{
 //Load the tolerance into a local variable
 double tol = FiniteElement::Node_location_tolerance;
 //There are now three possible indices
 Vector<int> index(3);
 //Loop over the coordinates
 for(unsigned i=0;i<3;i++)
  {
   //If we are at the lower limit, the index is zero
   if(std::abs(s[i] + 1.0) < tol)
    {
     index[i] = 0;
    }
   //If we are at the upper limit, the index is the number of nodes minus 1
   else if(std::abs(s[i] - 1.0) < tol)
    {
     index[i] = NNODE_1D-1;
    }
   //Otherwise, we have to calculate the index in general
   else
    {
     //For uniformly spaced nodes the node number would be
     double float_index = 0.5*(1.0 + s[i])*(NNODE_1D-1);
     //Conver to an integer by taking the floor (rounding down)
     index[i] = static_cast<int>(std::floor(float_index));
     //What is the excess. This should be safe because 
     //we have rounded down
     double excess = float_index - index[i];
     //If the excess is bigger than our tolerance there is no node,
     //return null. Note that we test at both ends 
     if((excess > tol) && ((1.0 - excess) > tol))
      {
       return 0;
      }
     //If we are at the upper end (i.e. the system has actually rounded up)
     //we need to add one to the index
     if((1.0 - excess) <= tol) {index[i] += 1;}
    }
  }
 //If we've got here we have a node, so let's return a pointer to it
 return node_pt(index[0] + NNODE_1D*index[1] + NNODE_1D*NNODE_1D*index[2]);
}



//=======================================================================
/// Shape function for specific QElement<3,..>
//=======================================================================
template <unsigned NNODE_1D>
void QElement<3,NNODE_1D>::shape(const Vector<double> &s, Shape &psi) 
 const
{
 //Local storage
 double Psi[3][NNODE_1D];

 //Call the OneDimensional Shape functions
 OneDimLagrange::shape<NNODE_1D>(s[0],Psi[0]);
 OneDimLagrange::shape<NNODE_1D>(s[1],Psi[1]);
 OneDimLagrange::shape<NNODE_1D>(s[2],Psi[2]);
 
 //Index for the shape functions
 unsigned index=0;

 //Now let's loop over the nodal points in the element
 //s1 is the "x" coordinate, s2 the "y" 
 for(unsigned i=0;i<NNODE_1D;i++)
  {
   for(unsigned j=0;j<NNODE_1D;j++)
    {
     for(unsigned k=0;k<NNODE_1D;k++)
      {
     /*Multiply the three 1D functions together to get the 3D function*/
     psi[index] = Psi[2][i]*Psi[1][j]*Psi[0][k];
     //Increment the index
     ++index;
      }
    }
  }
}

//=======================================================================
/// Derivatives of shape functions for specific  QElement<3,..>
//=======================================================================
template <unsigned NNODE_1D>
void QElement<3,NNODE_1D>::dshape_local(const Vector<double> &s, Shape &psi, 
                                 DShape &dpsids) const
{
 //Local storage
 double Psi[3][NNODE_1D];
 double DPsi[3][NNODE_1D];
 //Index of the total shape function
 unsigned index=0;

 //Call the OneDimensional Shape functions and derivatives
 OneDimLagrange::shape<NNODE_1D>(s[0],Psi[0]);
 OneDimLagrange::shape<NNODE_1D>(s[1],Psi[1]);
 OneDimLagrange::shape<NNODE_1D>(s[2],Psi[2]);
 OneDimLagrange::dshape<NNODE_1D>(s[0],DPsi[0]);
 OneDimLagrange::dshape<NNODE_1D>(s[1],DPsi[1]);
 OneDimLagrange::dshape<NNODE_1D>(s[2],DPsi[2]);

 
 //Loop over shape functions in element
 for(unsigned i=0;i<NNODE_1D;i++)
  {
   for(unsigned j=0;j<NNODE_1D;j++)
    {
     for(unsigned k=0;k<NNODE_1D;k++)
      {
       //Assign the values
       dpsids(index,0) = Psi[2][i] *  Psi[1][j]  * DPsi[0][k];
       dpsids(index,1) = Psi[2][i] *  DPsi[1][j] *  Psi[0][k];
       dpsids(index,2) = DPsi[2][i] *  Psi[1][j] *  Psi[0][k];
       
       psi[index] =  Psi[2][i]*Psi[1][j]*Psi[0][k];
       //Increment the index
       ++index;
      }
    }
  }
}

//=======================================================================
/// Second derivatives of shape functions for specific  QElement<3,..>: \n
/// d2psids(i,0) = \f$ \partial^2 \psi_j / \partial s_0^2 \f$ \n
/// d2psids(i,1) = \f$ \partial^2 \psi_j / \partial s_1^2 \f$ \n
/// d2psids(i,2) = \f$ \partial^2 \psi_j / \partial s_2^2 \f$ \n
/// d2psids(i,3) = \f$ \partial^2 \psi_j / \partial s_0 \partial s_1 \f$ \n
/// d2psids(i,4) = \f$ \partial^2 \psi_j / \partial s_0 \partial s_2 \f$ \n
/// d2psids(i,5) = \f$ \partial^2 \psi_j / \partial s_1 \partial s_2 \f$ \n
//=======================================================================
template <unsigned NNODE_1D>
void QElement<3,NNODE_1D>::d2shape_local(const Vector<double> &s, Shape &psi, 
                                         DShape &dpsids, DShape &d2psids) const
{
 //Local storage
 double Psi[3][NNODE_1D];
 double DPsi[3][NNODE_1D];
 double D2Psi[3][NNODE_1D];
 //Index of the shape function
 unsigned index=0;

 //Call the OneDimensional Shape functions and derivatives
 OneDimLagrange::shape<NNODE_1D>(s[0],Psi[0]);
 OneDimLagrange::shape<NNODE_1D>(s[1],Psi[1]);
 OneDimLagrange::shape<NNODE_1D>(s[2],Psi[2]);
 OneDimLagrange::dshape<NNODE_1D>(s[0],DPsi[0]);
 OneDimLagrange::dshape<NNODE_1D>(s[1],DPsi[1]);
 OneDimLagrange::dshape<NNODE_1D>(s[2],DPsi[2]);
 OneDimLagrange::d2shape<NNODE_1D>(s[0],D2Psi[0]);
 OneDimLagrange::d2shape<NNODE_1D>(s[1],D2Psi[1]);
 OneDimLagrange::d2shape<NNODE_1D>(s[2],D2Psi[2]);
 
 //Loop over shape functions in element
 for(unsigned i=0;i<NNODE_1D;i++)
  {
   for(unsigned j=0;j<NNODE_1D;j++)
    {
     for(unsigned k=0;k<NNODE_1D;k++)
      {
       //Assign the values
       psi[index] =  Psi[2][i]*Psi[1][j]*Psi[0][k];  
    
       dpsids(index,0) = Psi[2][i]*Psi[1][j]*DPsi[0][k];
       dpsids(index,1) = Psi[2][i]*DPsi[1][j]*Psi[0][k];
       dpsids(index,2) = DPsi[2][i]* Psi[1][j]*Psi[0][k];
  
       //Second derivative values
       d2psids(index,0) = Psi[2][i]*Psi[1][j]*D2Psi[0][k];
       d2psids(index,1) = Psi[2][i]*D2Psi[1][j]*Psi[0][k];
       d2psids(index,2) = D2Psi[2][i]* Psi[1][j]*Psi[0][k];
       //Convention for higher indices
       //3: mixed 12, 4: mixed 13, 5: mixed 23
       d2psids(index,3) = Psi[2][i]*DPsi[1][j]*DPsi[0][k];
       d2psids(index,4) = DPsi[2][i]*Psi[1][j]*DPsi[0][k];
       d2psids(index,5) = DPsi[2][i]*DPsi[1][j]*Psi[0][k];
       //Increment the index
       ++index;
      }
    }
  }
}

//===========================================================
/// The output function for QElement<3,NNODE_1D>
//===========================================================
template <unsigned NNODE_1D>
void QElement<3,NNODE_1D>::output(std::ostream &outfile)
{
 //Find the dimension of the nodes
 unsigned n_dim = this->nodal_dimension();
 //The node number
 unsigned l=0;

 //Tecplot header info 
 outfile << "ZONE I=" << NNODE_1D << ", J=" << NNODE_1D 
         << ", K=" << NNODE_1D<< std::endl;
 //Loop over element nodes
 for(unsigned l3=0;l3<NNODE_1D;l3++)
  {
   for(unsigned l2=0;l2<NNODE_1D;l2++)
    {
     for(unsigned l1=0;l1<NNODE_1D;l1++)
      {

       //Loop over the dimensions and output the position
       for(unsigned i=0;i<n_dim;i++)
        {
         outfile << node_pt(l)->x(i) << " ";
        }
       //Find out how many data values at the node
       unsigned initial_nvalue = node_pt(l)->nvalue();
       //Loop over the data and output whether pinned or not
       for(unsigned i=0;i<initial_nvalue;i++)
        {
         outfile << node_pt(l)->is_pinned(i) << " ";
        }
       outfile << std::endl;
       //Increase the node number
       ++l;
      }
    }
  }
 outfile << std::endl;
}

//=======================================================================
/// The output function for n_plot points in each coordinate direction
//=======================================================================
template <unsigned NNODE_1D>
void QElement<3,NNODE_1D>::output(std::ostream &outfile, 
                                  const unsigned &n_plot)
{
 //Local variables
 Vector<double> s(3);

 //Tecplot header info 
 outfile << "ZONE I=" << n_plot << ", J=" << n_plot 
         << ", K=" << n_plot << std::endl;

 //Find the dimension of the nodes
 unsigned n_dim = this->nodal_dimension();

 //Loop over element nodes
 for(unsigned l3=0;l3<n_plot;l3++)
  {
   s[2] = -1.0 + l3*2.0/(n_plot-1);
   for(unsigned l2=0;l2<n_plot;l2++)
    {
     s[1] = -1.0 + l2*2.0/(n_plot-1);
     for(unsigned l1=0;l1<n_plot;l1++)
      {
       s[0] = -1.0 + l1*2.0/(n_plot-1);
       
       //Output the coordinates
       for (unsigned i=0;i<n_dim;i++)
        {
         outfile << interpolated_x(s,i) << " " ;
        }
       outfile << std::endl;
      }
    }
  }
 outfile << std::endl;
}




//===========================================================
/// C-style output function for QElement<3,NNODE_1D>
//===========================================================
template <unsigned NNODE_1D>
void QElement<3,NNODE_1D>::output(FILE* file_pt)
{
 //Find the dimension of the nodes
 unsigned n_dim = this->nodal_dimension();
 //The node number
 unsigned l=0;

 //Tecplot header info 
 fprintf(file_pt,"ZONE I=%i, J=%i, K=%i\n",
         NNODE_1D,NNODE_1D,NNODE_1D);

 //Loop over element nodes
 for(unsigned l3=0;l3<NNODE_1D;l3++)
  {
   for(unsigned l2=0;l2<NNODE_1D;l2++)
    {
     for(unsigned l1=0;l1<NNODE_1D;l1++)
      {
       //Loop over the dimensions and output the position
       for(unsigned i=0;i<n_dim;i++)
        {
         fprintf(file_pt,"%g ",node_pt(l)->x(i));
        }

       //Find out how many data values at the node
       unsigned initial_nvalue = node_pt(l)->nvalue();

       //Loop over the data and output whether pinned or not
       for(unsigned i=0;i<initial_nvalue;i++)
        {
         fprintf(file_pt,"%i ",node_pt(l)->is_pinned(i));
        }
       fprintf(file_pt,"\n");
       //Increase the node number
       ++l;
      }
    }
  }
 fprintf(file_pt,"\n");

}

//=======================================================================
/// C-style output function for n_plot points in each coordinate direction
//=======================================================================
template <unsigned NNODE_1D>
void QElement<3,NNODE_1D>::output(FILE* file_pt, const unsigned &n_plot)
{
 //Local variables
 Vector<double> s(3);

 //Tecplot header info 
 fprintf(file_pt,"ZONE I=%i, J=%i, K=%i\n",n_plot,n_plot,n_plot);

 //Find the dimension of the nodes
 unsigned n_dim = this->nodal_dimension();

 //Loop over element nodes
 for(unsigned l3=0;l3<n_plot;l3++)
  {
   s[2] = -1.0 + l3*2.0/(n_plot-1);
   for(unsigned l2=0;l2<n_plot;l2++)
    {
     s[1] = -1.0 + l2*2.0/(n_plot-1);
     for(unsigned l1=0;l1<n_plot;l1++)
      {
       s[0] = -1.0 + l1*2.0/(n_plot-1);
       
       //Output the coordinates
       for (unsigned i=0;i<n_dim;i++)
        {
         fprintf(file_pt,"%g ",interpolated_x(s,i));
        }
       fprintf(file_pt,"\n");
      }
    }
  }
 fprintf(file_pt,"\n");
}



//=======================================================================
/// Function to setup geometrical information for lower-dimensional 
/// FaceElements (which are of type QElement<2,NNODE_1D>).
//=======================================================================
 template<unsigned NNODE_1D>
 void QElement<3,NNODE_1D>::build_face_element(const int &face_index,
                                               FaceElement* face_element_pt)
{
 //Set the nodal dimension from the first node
 face_element_pt->set_nodal_dimension(node_pt(0)->ndim());

 //Set the pointer to the orginal "bulk" element
 face_element_pt->bulk_element_pt()=this;

 // Resize storage for the number of values originally stored
 // at the face element's NNODE_1D*NNODE_1D nodes.
 face_element_pt->nbulk_value_resize(NNODE_1D*NNODE_1D);

 // Resize storage for the bulk node numbers corresponding to
 // the NNODE_1D*NNODE_1D nodes of the face
 face_element_pt->bulk_node_number_resize(NNODE_1D*NNODE_1D);

 // Set the face index in the element
 // The faces are
 // -3 : BACK  (OLD: Bottom
 // -2 : DOWN  (OLD: Front 
 // -1 : LEFT  (OLD: Left Side
 //  1 : RIGHT (OLD: Right Side
 //  2 : UP    (OLD: Back
 //  3 : FRONT (OLD: Top

 face_element_pt->face_index() = face_index;


 //Now set up the node pointers and the normal vectors
 switch(face_index)
   {
    unsigned bulk_number;
    // BACK 
    //-----
   case -3:

    //Set the pointer to the bulk coordinate translation scheme
    face_element_pt->face_to_bulk_coordinate_fct_pt() = 
     &QElement3FaceToBulkCoordinates::face2;

   //Set the pointer to the derivative mappings
    face_element_pt->bulk_coordinate_derivatives_fct_pt() = 
     &QElement3BulkCoordinateDerivatives::faces2;

    // Copy nodes
    for (unsigned i=0;i<(NNODE_1D*NNODE_1D);i++)
     {
      bulk_number = i;
      face_element_pt->node_pt(i)=node_pt(bulk_number);
      face_element_pt->bulk_node_number(i) = bulk_number;
      //set the number of values originally stored at this node
      face_element_pt->nbulk_value(i) = required_nvalue(bulk_number);
      
     }
    // Outer unit normal is negative of cross product of two in plane
    // tangent vectors
    face_element_pt->normal_sign()=-1;

    break;

    // FRONT
    //------
   case 3:
    
    //Set the pointer to the bulk coordinate translation scheme
    face_element_pt->face_to_bulk_coordinate_fct_pt() = 
     &QElement3FaceToBulkCoordinates::face5;

   //Set the pointer to the derivative mappings
    face_element_pt->bulk_coordinate_derivatives_fct_pt() = 
     &QElement3BulkCoordinateDerivatives::faces2;


    // Copy nodes
    for (unsigned i=0;i<(NNODE_1D*NNODE_1D);i++)
     {
      bulk_number = i+(NNODE_1D*NNODE_1D)*(NNODE_1D-1);
      face_element_pt->node_pt(i)=node_pt(bulk_number);
      face_element_pt->bulk_node_number(i) = bulk_number;
      face_element_pt->nbulk_value(i) = required_nvalue(bulk_number);
     }
    // Outer unit normal is cross product of two in plane
    // tangent vectors
    face_element_pt->normal_sign()=1;


    break;

    // DOWN:
    //------
   case -2:
    
   {
    //Set the pointer to the bulk coordinate translation scheme
    face_element_pt->face_to_bulk_coordinate_fct_pt() = 
     &QElement3FaceToBulkCoordinates::face1;

   //Set the pointer to the derivative mappings
    face_element_pt->bulk_coordinate_derivatives_fct_pt() = 
     &QElement3BulkCoordinateDerivatives::faces1;
    

    // Copy nodes
    unsigned count=0;
    for (unsigned i=0;i<NNODE_1D;i++)
     {
      for (unsigned j=0;j<NNODE_1D;j++)
       {
        bulk_number = j+i*(NNODE_1D*NNODE_1D);
        face_element_pt->node_pt(count)=node_pt(bulk_number);
        face_element_pt->bulk_node_number(count) = bulk_number;
        face_element_pt->nbulk_value(count) = required_nvalue(bulk_number);
        count++;
       }
     }
    
    // Outer unit normal is cross product of two in plane
    // tangent vectors
    face_element_pt->normal_sign()=1;
   }
   break;


   // UP:
   //----
   case 2:
 
   {
    //Set the pointer to the bulk coordinate translation scheme
    face_element_pt->face_to_bulk_coordinate_fct_pt() = 
     &QElement3FaceToBulkCoordinates::face4;
    
    //Set the pointer to the derivative mappings
    face_element_pt->bulk_coordinate_derivatives_fct_pt() = 
     &QElement3BulkCoordinateDerivatives::faces1;
    

    // Copy nodes
    unsigned count=0;
    for (unsigned i=0;i<NNODE_1D;i++)
     {
      for (unsigned j=0;j<NNODE_1D;j++)
       {
        bulk_number = j+i*(NNODE_1D*NNODE_1D)+(NNODE_1D*(NNODE_1D-1)); 
        face_element_pt->node_pt(count)= node_pt(bulk_number);
        face_element_pt->bulk_node_number(count) = bulk_number;
        face_element_pt->nbulk_value(count) = required_nvalue(bulk_number);
        count++;
       }
     }

    // Outer unit normal is negative of cross product of two in plane
    // tangent vectors
    face_element_pt->normal_sign()=-1;
   }
   break;

   // LEFT:
   //------
  case -1:
 
   {
    //Set the pointer to the bulk coordinate translation scheme
    face_element_pt->face_to_bulk_coordinate_fct_pt() = 
     &QElement3FaceToBulkCoordinates::face0;

   //Set the pointer to the derivative mappings
    face_element_pt->bulk_coordinate_derivatives_fct_pt() = 
     &QElement3BulkCoordinateDerivatives::faces0;


    // Copy nodes
    unsigned count=0;
    for (unsigned i=0;i<NNODE_1D;i++)
     {
      for (unsigned j=0;j<NNODE_1D;j++)
       {
        bulk_number = j*NNODE_1D+i*(NNODE_1D*NNODE_1D);
        face_element_pt->node_pt(count)=node_pt(bulk_number);
        face_element_pt->bulk_node_number(count) = bulk_number;
        face_element_pt->nbulk_value(count) = required_nvalue(bulk_number);
        count++;
       }
     }

    // Outer unit normal is negative of cross product of two in plane
    // tangent vectors
    face_element_pt->normal_sign()=-1;
   }
   break;

   
   // RIGHT:
   //-------
   case 1:
 
   {
    //Set the pointer to the bulk coordinate translation scheme
    face_element_pt->face_to_bulk_coordinate_fct_pt() = 
     &QElement3FaceToBulkCoordinates::face3;

   //Set the pointer to the derivative mappings
    face_element_pt->bulk_coordinate_derivatives_fct_pt() = 
     &QElement3BulkCoordinateDerivatives::faces0;

    // Copy nodes
    unsigned count=0;
    for (unsigned i=0;i<NNODE_1D;i++)
     {
      for (unsigned j=0;j<NNODE_1D;j++)
       {
        bulk_number = j*NNODE_1D+i*(NNODE_1D*NNODE_1D)+(NNODE_1D-1);
        face_element_pt->node_pt(count)=node_pt(bulk_number);
        face_element_pt->bulk_node_number(count) = bulk_number;
        face_element_pt->nbulk_value(count) = required_nvalue(bulk_number);
        count++;
       }
     }

    // Outer unit normal is cross product of two in plane
    // tangent vectors
    face_element_pt->normal_sign()=1;
   }
   break;


   // Cover all other cases
   default:
    std::ostringstream error_message;
    error_message 
     << "Face index should only take the values +/- 1, +/- 2 or +/- 3,"
     << " not " << face_index << std::endl;
    throw OomphLibError(error_message.str(),
                        "QElement<3,..>::build_face_element()",
                        OOMPH_EXCEPTION_LOCATION);
   } //end switch
 
}


//===================================================================
// Build required templates
//===================================================================
template class QElement<1,2>;
template class QElement<1,3>;
template class QElement<1,4>;

template class QElement<2,2>;
template class QElement<2,3>;
template class QElement<2,4>;

template class QElement<3,2>;
template class QElement<3,3>;
template class QElement<3,4>;

//template class SolidQElement<1,2>;
//template class SolidQElement<1,3>;
//template class SolidQElement<1,4>;
//template class SolidQElement<2,2>;
//template class SolidQElement<2,3>;
//template class SolidQElement<2,4>;

}