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
#include "map_matrix.h"
#include "brick_mesh.h"

namespace oomph
{

//================================================================
/// Setup lookup schemes which establish which elements are located
/// next to which boundaries (Doc to outfile if it's open).
//================================================================
void BrickMeshBase::setup_boundary_element_info(std::ostream &outfile)
{

 // Doc?
 bool doc=false;
 if (outfile) doc=true;
 
 // Number of boundaries
 unsigned nbound=nboundary();
 
 // Wipe/allocate storage for arrays
 Boundary_element_pt.clear();
 Face_index_at_boundary.clear();
 Boundary_element_pt.resize(nbound);
 Face_index_at_boundary.resize(nbound);
 
 // Temporary vector of sets of pointers to elements on the boundaries: 
 // set_of_boundary_element_pt[i] is the set of pointers to all
 // elements that have nodes on boundary i. 
 Vector<std::set<FiniteElement*> > set_of_boundary_element_pt;
 set_of_boundary_element_pt.resize(nbound);
 
 // Matrix map for working out the fixed local coord for elements on boundary
 MapMatrixMixed<unsigned,FiniteElement*,Vector<int>* > 
  boundary_identifier;

 // Tmp container to store pointers to tmp vectors so they can be deleted
 Vector<Vector<int>*> tmp_vect_pt; 

 // Loop over elements
 //-------------------
 unsigned nel=nelement();
 for (unsigned e=0;e<nel;e++)
  {
   // Get pointer to element
   FiniteElement* fe_pt=finite_element_pt(e);

   if (doc) outfile << "Element: " << e << " " << fe_pt << std::endl;

   // Loop over the element's nodes and find out which boundaries they're on
   // ----------------------------------------------------------------------
   unsigned nnode_1d=fe_pt->nnode_1d();

   // Loop over nodes in order
   for (unsigned i0=0;i0<nnode_1d;i0++)
    {
     for (unsigned i1=0;i1<nnode_1d;i1++)
      {
       for (unsigned i2=0;i2<nnode_1d;i2++)
        {
         // Local node number
         unsigned j=i0+i1*nnode_1d+i2*nnode_1d*nnode_1d;
         
         // Get pointer to set of boundaries that this
         // node lives on
         std::set<unsigned>* boundaries_pt=0;
         fe_pt->node_pt(j)->get_boundaries_pt(boundaries_pt);
         

//         std::cout << "element/node: " << e << " " << j << std::endl;

         // If the node lives on some boundaries....
         if (boundaries_pt!=0)
          {

//            std::cout << "lives on " << boundaries_pt->size() 
//                      << " boundaries, namely " << std::endl;

           for (std::set<unsigned>::iterator it=boundaries_pt->begin();
                it!=boundaries_pt->end();++it)
            {

             // What's the boundary?
             unsigned boundary_id=*it;

//              std::cout << boundary_id << " ";

             // Add pointer to finite element to set for the appropriate 
             // boundary -- storage in set makes sure we don't count elements
             // multiple times
             set_of_boundary_element_pt[boundary_id].insert(fe_pt);
             
             // For the current element/boundary combination, create
             // a vector that stores an indicator which element boundaries
             // the node is located (boundary_identifier=-/+1 for nodes
             // on the left/right boundary; boundary_identifier=-/+2 for nodes
             // on the lower/upper boundary. We determine these indices
             // for all corner nodes of the element and add them to a vector
             // to a vector. This allows us to decide which face of the element
             // coincides with the boundary since the (brick!) element must 
             // have exactly four corner nodes on the boundary.
             if (boundary_identifier(boundary_id,fe_pt)==0)
              {
//                std::cout << "       " 
//                          << "    allocating for " << boundary_id 
//                          <<" " << e << std::endl;
               Vector<int>* tmp_pt=new Vector<int>;
               tmp_vect_pt.push_back(tmp_pt); 
               boundary_identifier(boundary_id,fe_pt)=tmp_pt;
              }
             else
              {               
//                std::cout << "       " 
//                          << "NOT allocating for " << boundary_id 
//                          <<" " << e << std::endl;
              }
            
             // Are we at a corner node?
             if (((i0==0)||(i0==nnode_1d-1))&&((i1==0)||(i1==nnode_1d-1))
                 &&((i2==0)||(i2==nnode_1d-1)))
              {
               // Create index to represent position relative to s_0
               (*boundary_identifier(boundary_id,fe_pt)).
                push_back(1*(2*i0/(nnode_1d-1)-1));               
               
               // Create index to represent position relative to s_1
               (*boundary_identifier(boundary_id,fe_pt)).
                push_back(2*(2*i1/(nnode_1d-1)-1));
 
               // Create index to represent position relative to s_2
               (*boundary_identifier(boundary_id,fe_pt)).
                push_back(3*(2*i2/(nnode_1d-1)-1));
              }
            }
//              std::cout << std::endl;
          }
//          else
//           {
//            std::cout << "...doesn't live on a boundary" << std::endl;
//           }

         
        }
      }     
    }  
  }
   

 // Now copy everything across into permanent arrays
 //-------------------------------------------------


 // Loop over boundaries
 //---------------------
 for (unsigned i=0;i<nbound;i++)
  {

//    std::cout << "Boundary: " << i << std::endl;

   // Number of elements on this boundary (currently stored in a set)
   unsigned nel=set_of_boundary_element_pt[i].size();
   
   // Loop over elements on given boundary
   typedef std::set<FiniteElement*>::iterator IT;
   for (IT it=set_of_boundary_element_pt[i].begin();
        it!=set_of_boundary_element_pt[i].end();
        it++)
    {
     // Push back into permanent array
     Boundary_element_pt[i].push_back(*it);
    }
   
   // Allocate storage for the face identifiers
   Face_index_at_boundary[i].resize(nel);
   
   // Loop over elements on this boundary
   //-------------------------------------
   for (unsigned e=0;e<nel;e++)
    {
     
//      std::cout << "Element: " << e << std::endl;

     // Recover pointer to element
     FiniteElement* fe_pt=Boundary_element_pt[i][e];
     
     // Initialise count for boundary identiers (-3,-2,-1,1,2,3)
     std::map<int,unsigned> count;
     
     // Loop over coordinates
     for (int ii=0;ii<3;ii++)
      {
       // Loop over upper/lower end of coordinates
       for (int sign=-1;sign<3;sign+=2)
        {
         count[(ii+1)*sign]=0;
//          std::cout << "Initialising for " << (ii+1)*sign << std::endl;
        }
      }
     
     // Loop over boundary indicators for this element/boundary
     unsigned n_indicators=(*boundary_identifier(i,fe_pt)).size();
     for (unsigned j=0;j<n_indicators;j++)
      {
       count[(*boundary_identifier(i,fe_pt))[j] ]++;
//        std::cout << "Added to " << (*boundary_identifier(i,fe_pt))[j] 
//                  << " new value: "
//                  << count[(*boundary_identifier(i,fe_pt))[j] ] << std::endl;

      }

     // Determine the correct boundary indicator by checking that it 
     // occurs four times (since four corner nodes of the element's boundary
     // need to be located on the domain boundary
     int indicator=-10;

     //Check that we're finding exactly one boundary indicator
     bool found=false;

     // Loop over coordinates
     for (int ii=0;ii<3;ii++)
      {
       // Loop over upper/lower end of coordinates
       for (int sign=-1;sign<3;sign+=2)
        {
         if (count[(ii+1)*sign]==4)
          {
           // Check that we haven't found multiple boundaries
           if (found)
            {
             throw OomphLibError(
              "Trouble: Multiple boundary identifiers!\n",
              "BrickMeshBase::setup_boundary_element_info()",
              OOMPH_EXCEPTION_LOCATION);
            }
           found=true;
           indicator=(ii+1)*sign;
          }
        }
      }
     


//      // Loop over coordinates
//      for (int ii=0;ii<3;ii++)
//       {
//        // Loop over upper/lower end of coordinates
//        for (int sign=-1;sign<3;sign+=2)
//         {
//          std::cout << (ii+1)*sign << " " << count[(ii+1)*sign] << std::endl;
//         }
//       }
//      std::cout << std::endl << std::endl;
     

     // Check if we've found one boundary
     if (!found)
      {
//        fe_pt->output(std::cout,3);
//        pause("about to die..");


       std::ostringstream error_stream;
       error_stream
        << "Failed to find a boundary identifier.\n\n" 
        << "NOTE: This function is not implemented in full generality\n "
        << "      and this error may be thrown erroneously:\n\n"
        << "Cases where fewer than four vertex nodes of a brick element \n"
        << "that has some nodes located on a mesh boundary are located \n"
        << "on the same boundary MAY in fact arise legally, for instance \n"
        << "if only an edge of an element is located on the mesh boundary.\n"
        << "However, there is currently a problem, either with our code\n"
        << "or with the intel compiler, in which this error also gets \n"
        << "thrown when there ARE four vertex nodes on the same domain boundary!\n"
        << "This problem seems to occur only with 3d brick meshes in which\n"
        << "Nodes were initially created as \"ordinary\" Nodes and then\n"
        << "converted to BoundaryNodes, using the \n"
        << "Mesh::convert_to_boundary_node(...) function. \n\n"
        << "There are currently two options:\n"
        << " (1) If you have developed a new mesh in which this error is\n"
        << "     thrown erroneously, rewrite this function: Elements should\n"
        << "     only be added to the boundary element vector IF it has been\n"
        << "     established that four of their vertex nodes are located on a mesh\n"
        << "     boundary. We will implement this rewrite ourselves \n"
        << "     once we have tracked down the problem (or established that \n"
        << "     it is caused by the intel compiler -- wishful thinking?)\n"
        << " (2) If you use one of oomph-lib's existing 3D meshes then \n"
        << "     the error should not occur. In this case a workaround\n"
        << "     is to avoid the conversion to boundary nodes, by\n"
        << "     compiling the meshes (or indeed the entire library)\n "
        << "     with the C++ compiler flag CONVERT_BOUNDARY_NODE_IS_BROKEN.\n"
        << "     With this flag all Nodes in the affected meshes are created\n"
        << "     as BoundaryNodes and the problem is avoided -- at the\n"
        << "     expense of a slight additional memory overhead.\n";
       throw OomphLibError(error_stream.str(),
                           "BrickMeshBase::setup_boundary_element_info()",
                           OOMPH_EXCEPTION_LOCATION);
      }
     

     // Now convert boundary indicator into information required
     // for FaceElements
     switch (indicator)
      {
      case -3:
   
       // s_2 is fixed at -1.0:
       Face_index_at_boundary[i][e] = -3;
       break;
      
      case -2:
   
       // s_1 is fixed at -1.0:
       Face_index_at_boundary[i][e] = -2;
       break;
       
      case -1:
       
       // s_0 is fixed at -1.0:
       Face_index_at_boundary[i][e] = -1;
       break;
       
       
      case 1:
       
       // s_0 is fixed at 1.0:
       Face_index_at_boundary[i][e] = 1;
       break;
       
      case 2:
       
       // s_1 is fixed at 1.0:
       Face_index_at_boundary[i][e] = 2;
       break;
    
      case 3:
       
       // s_2 is fixed at 1.0:
       Face_index_at_boundary[i][e] = 3;
       break;
    
   
      default:

       throw OomphLibError("Never get here",
                           "BrickMeshBase::setup_boundary_element_info()",
                           OOMPH_EXCEPTION_LOCATION);
      }
     
    }
  }
 

 // Doc?
 //-----
 if (doc)
  {
   // Loop over boundaries
   for (unsigned i=0;i<nbound;i++)
    {
     unsigned nel=Boundary_element_pt[i].size();
     outfile << "Boundary: " << i
             << " is adjacent to " << nel
             << " elements" << std::endl;
     
     // Loop over elements on given boundary
     for (unsigned e=0;e<nel;e++)
      {
       FiniteElement* fe_pt=Boundary_element_pt[i][e];
       outfile << "Boundary element:" <<  fe_pt
               << " Face index along boundary is "
               <<  Face_index_at_boundary[i][e] << std::endl;
      }
    }
  }
 

 // Lookup scheme has now been setup yet
 Lookup_for_elements_next_boundary_is_setup=true;


 // Cleanup temporary vectors
 unsigned n=tmp_vect_pt.size();
 for (unsigned i=0;i<n;i++)
  {
   delete tmp_vect_pt[i];
  }

}


}