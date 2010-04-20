//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.90. August 3, 2009.
//LIC// 
//LIC// Copyright (C) 2006-2009 Matthias Heil and Andrew Hazel
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
// Templated functions for MeshAsGeomObject
// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//oomph-lib headers
#include "mesh.h"
#include "mesh_as_geometric_object.h"

#include <cstdio>

namespace oomph
{
//======================================================================
// Namespace for global multi-domain functions that are used in
// member function of MeshAsGeomObject
//======================================================================
namespace Multi_domain_functions
 {
  /// \short Number of bins in the first dimension in binning method in
  /// setup_multi_domain_interaction(). Default value of 10.
  extern unsigned Nx_bin;

  /// \short Number of bins in the second dimension in binning method in
  /// setup_multi_domain_interaction(). Default value of 10.
  extern unsigned Ny_bin;

  /// \short Number of bins in the third dimension in binning method in
  /// setup_multi_domain_interaction(). Default value of 10.
  extern unsigned Nz_bin;

  /// \short (Measure of) the number of sampling points within the elements 
  /// when populating the bin
  extern unsigned Nsample_points;

  /// \short Boolean to indicate whether to document basic info (to screen)
  ///        during setup_multi_domain_interaction() routines
  extern bool Doc_stats;
}

//========================================================================
/// Helper function for constructor: Pass the pointer to the mesh, 
/// communicator and boolean
/// to specify whether to calculate coordinate extrema or not
/// The dimensions for the GeomObject are read out from the elements and
/// nodes of the mesh.
//========================================================================
 void MeshAsGeomObject::construct_it
 (Mesh* const &mesh_pt, OomphCommunicator* comm_pt,
  const bool& compute_extreme_bin_coords)
 {
#ifdef OOMPH_HAS_MPI
   // Set communicator
   Communicator_pt=comm_pt;
#endif

  //Storage for the Lagrangian and Eulerian dimension
  int dim[2]={0,0};

  //Set the Lagrangian dimension from the dimension of the first element
  //if it exists (if not the Lagrangian dimension will be zero)
  if(mesh_pt->nelement()!=0) 
   {dim[0] = mesh_pt->finite_element_pt(0)->dim();}
 
  //Read out the Eulerian dimension from the first node, if it exists.
  //(if not the Eulerian dimension will be zero);
  if(mesh_pt->nnode()!=0)
   {dim[1] = mesh_pt->node_pt(0)->ndim();}

  // Need to do an Allreduce to ensure that the dimension is consistent
  // even when no elements are assigned to a certain processor
#ifdef OOMPH_HAS_MPI
  //Only a problem if the mesh has been distributed
  if(mesh_pt->mesh_has_been_distributed())
   {
    //Need a non-null communicator
    if(Communicator_pt!=0)
     {
      int n_proc=comm_pt->nproc();
      if (n_proc > 1)
       {
        int dim_reduce[2];
        MPI_Allreduce(&dim,&dim_reduce,2,MPI_INT,
                      MPI_MAX,comm_pt->mpi_comm());
        
        dim[0] = dim_reduce[0]; 
        dim[1] = dim_reduce[1];
       }
     }
   }
#endif

   //Set the Lagrangian and Eulerian dimensions within this geometric object
   this->set_nlagrangian_and_ndim(static_cast<unsigned>(dim[0]),
                                  static_cast<unsigned>(dim[1]));


   // Create temporary storage for geometric Data (don't count 
   // Data twice!
   std::set<Data*> tmp_geom_data;
   
   //Copy all the elements in the mesh into local storage
   //N.B. elements must be able to have a geometric object representation.
   unsigned n_sub_object = mesh_pt->nelement();
   Sub_geom_object_pt.resize(n_sub_object);
   for(unsigned e=0;e<n_sub_object;e++)
    {

     // (Try to) cast to a finite elemnet:
     Sub_geom_object_pt[e]=
      dynamic_cast<FiniteElement*>(mesh_pt->element_pt(e));

#ifdef PARANOID
     if (Sub_geom_object_pt[e]==0)
      {
       std::ostringstream error_message;
       error_message 
        << "Unable to dynamic cast element: " << std::endl
        << "into a FiniteElement: GeomObject representation is not possible\n";
       throw OomphLibError(
        error_message.str(),
        "MeshAsGeomObject::MeshAsGeomObject(...)",
        OOMPH_EXCEPTION_LOCATION);
      }
#endif

     // Add the geometric Data of each element into set
     unsigned ngeom_data=Sub_geom_object_pt[e]->ngeom_data();
     for (unsigned i=0;i<ngeom_data;i++)
      {
       tmp_geom_data.insert(Sub_geom_object_pt[e]->geom_data_pt(i));
      }
    }

   // Now copy unique geom Data values across into vector
   unsigned ngeom=tmp_geom_data.size();
   Geom_data_pt.resize(ngeom);
   typedef std::set<Data*>::iterator IT;
   unsigned count=0;
   for (IT it=tmp_geom_data.begin();it!=tmp_geom_data.end();it++)
    {
     Geom_data_pt[count]=*it;
     count++;
    }

   // Set storage for minimum and maximum coordinates
   const unsigned dim_lagrangian = this->nlagrangian();
   Min_coords.resize(dim_lagrangian);
   Max_coords.resize(dim_lagrangian);

   // Get the default parameters for the number of bins in each 
   // dimension from the Multi_domain_functions namespace
   
   // Parameters are at Nx_bin, Ny_bin, Nz_bin
   Nbin_x=Multi_domain_functions::Nx_bin;
   Nbin_y=Multi_domain_functions::Ny_bin;
   Nbin_z=Multi_domain_functions::Nz_bin;
   
   // Are we computing the extreme bin coordinates here?
   if (compute_extreme_bin_coords)
    {
     // Find the maximum and minimum coordinates for the mesh
     get_min_and_max_coordinates(mesh_pt);

     // Create the bin structure
     create_bins_of_objects();
    }
 }


//========================================================================
/// \short Find the sub geometric object and local coordinate therein that
/// corresponds to the intrinsic coordinate zeta. If sub_geom_object_pt=0
/// on return from this function, none of the constituent sub-objects 
/// contain the required coordinate.
/// Setting the optional bool argument to true means that each
/// time the sub-object's locate_zeta function is called, the coordinate
/// argument "s" is used as the initial guess
//========================================================================
 void MeshAsGeomObject::locate_zeta
 (const Vector<double>& zeta,GeomObject*& sub_geom_object_pt,
  Vector<double>& s, const bool &called_within_spiral)
  {
   // Initialise return to null -- if it's still null when we're
   // leaving we've failed!
   sub_geom_object_pt=0;

   //Get the lagrangian dimension
   const unsigned n_lagrangian = this->nlagrangian();

   // Does the zeta coordinate lie within the current bin structure?
   // If not then modify Min/Max_coords and re-populate the bin structure

   //Boolean to indicate whether bin structure should be repopulated
   bool recreate_bins = false;
   //Loop over the lagrangian dimension
   for(unsigned i=0;i<n_lagrangian;i++)
    {
     //If the i-th coordinate is less than the minimum
     if(zeta[i] < Min_coords[i]) 
      {
       Min_coords[i] = zeta[i];
       recreate_bins = true;
      }
     //Otherwise coordinate may be bigger than the maximum
     else if(zeta[i] > Max_coords[i])
      {
       Max_coords[i] = zeta[i];
       recreate_bins = true;
      }
    }
   
   //Recreate bins if necessary
   if(recreate_bins==true) {create_bins_of_objects();}

   // Use the min and max coords of the bin structure, to find
   // the bin structure containing the current zeta cooordinate
   unsigned bin_number=0;
   //Offset for rows/matrices in higher dimensions
   unsigned multiplier=1;
   unsigned Nbin[3]={Nbin_x,Nbin_y,Nbin_z};

   // Loop over the dimension
   for(unsigned i=0;i<n_lagrangian;i++)
    {
     //Find the bin number of the current coordinate
     unsigned bin_number_i = 
      int(Nbin[i]*((zeta[i]-Min_coords[i])/
                   (Max_coords[i] - Min_coords[i])));
     //Buffer the case when we are exactly on the edge
     if(bin_number_i==Nbin[i]) {bin_number_i -= 1;}
     //Now add to the bin number using the multiplier
     bin_number += multiplier*bin_number_i;
     //Increase the current row/matrix multiplier for the next loop
     multiplier *= Nbin[i];
    }

   if (called_within_spiral)
    {
     // Current "spiral" level
     unsigned i_level=current_spiral_level();

     // Call helper function to find the neighbouring bins at this level
     Vector<unsigned> neighbour_bin;
     get_neighbouring_bins_helper(bin_number,i_level,neighbour_bin);
     unsigned n_nbr_bin=neighbour_bin.size();

     // Set bool for finding zeta
     bool found_zeta=false;
     for (unsigned i_nbr=0;i_nbr<n_nbr_bin;i_nbr++)
      {
       // Get the number of element-sample point pairs in this bin
       unsigned n_sample=
        Bin_object_coord_pairs[neighbour_bin[i_nbr]].size();

       // Don't do anything if this bin has no sample points
       if (n_sample>0)
        {
         for (unsigned i_sam=0;i_sam<n_sample;i_sam++)
          {
           // Get the element
           FiniteElement* el_pt=Bin_object_coord_pairs
            [neighbour_bin[i_nbr]][i_sam].first;

           // Get the local coordinate
           s=Bin_object_coord_pairs[neighbour_bin[i_nbr]][i_sam].second;

           // Use this coordinate as the initial guess
           bool use_coordinate_as_initial_guess=true;

           // Attempt to find zeta within a sub-object
           el_pt->locate_zeta(zeta,sub_geom_object_pt,s,
                              use_coordinate_as_initial_guess);

#ifdef OOMPH_HAS_MPI
           // Dynamic cast the result to a FiniteElement
           FiniteElement* test_el_pt=
            dynamic_cast<FiniteElement*>(sub_geom_object_pt);
           if (test_el_pt!=0)
            {
             // We only want to exit if this is a non-halo element
             if (test_el_pt->is_halo()) {sub_geom_object_pt=0;}
            }
#endif

           // If the FiniteElement is non-halo and has been located, exit
           if (sub_geom_object_pt!=0)
            {
             found_zeta=true;
             break;
            }
          } // end loop over sample points
        }

       if (found_zeta)
        {
         break;
        }

      } // end loop over bins at this level

    }
   else
    {
     // Not called from within a spiral procedure
     // (i.e. the loop in multi_domain.h), so do the spiralling here

     // Loop over all levels... maximum of N*_bin
     unsigned n_level=Nbin[0];
     for(unsigned i=1;i<n_lagrangian;i++)
      {
       if(n_level < Nbin[i]) {n_level = Nbin[i];}
      }
     
     // Set bool for finding zeta
     bool found_zeta=false;
     for (unsigned i_level=0;i_level<n_level;i_level++)
      {
       // Call helper function to find the neighbouring bins at this level
       Vector<unsigned> neighbour_bin;
       get_neighbouring_bins_helper(bin_number,i_level,neighbour_bin);
       unsigned n_nbr_bin=neighbour_bin.size();

       // Loop over neighbouring bins
       for (unsigned i_nbr=0;i_nbr<n_nbr_bin;i_nbr++)
        {
         // Get the number of element-sample point pairs in this bin
         unsigned n_sample=
          Bin_object_coord_pairs[neighbour_bin[i_nbr]].size();

         // Don't do anything if this bin has no sample points
         if (n_sample>0)
          {
           for (unsigned i_sam=0;i_sam<n_sample;i_sam++)
            {
             // Get the element
             FiniteElement* el_pt=Bin_object_coord_pairs
              [neighbour_bin[i_nbr]][i_sam].first;

             // Get the local coordinate
             s=Bin_object_coord_pairs[neighbour_bin[i_nbr]][i_sam].second;

             // Use this coordinate as the initial guess in locate_zeta
             bool use_coordinate_as_initial_guess=true;

             // Attempt to loacte the correct sub-object
             el_pt->locate_zeta(zeta,sub_geom_object_pt,s,
                                use_coordinate_as_initial_guess);

             // If it was found then break
             if (sub_geom_object_pt!=0)
              {
               found_zeta=true;
               break;
              }
            } // end loop over sample points
          }

         // Break out of the bin loop if locate was successful
         if (found_zeta)
          {
           break;
          }

        } // end loop over bins at this level

       // Break out of the spiral loop if locate was successful
       if (found_zeta)
        {
         break;
        }

      } // end loop over levels

    } // end if (called_within_spiral)

  }

//========================================================================
///Get the min and max coordinates for the mesh, in each dimension
//========================================================================
 void MeshAsGeomObject::
 get_min_and_max_coordinates(Mesh* const &mesh_pt)
  {
   //Get the lagrangian dimension
   int n_lagrangian = this->nlagrangian();

   // Storage locally (i.e. in parallel on each processor)
   // for the minimum and maximum coordinates
   double zeta_min_local[n_lagrangian]; double zeta_max_local[n_lagrangian];
   for(int i=0;i<n_lagrangian;i++)
    {zeta_min_local[i] = DBL_MAX; zeta_max_local[i] = -DBL_MAX;}
   
   // Loop over the elements of the mesh
   unsigned n_el=mesh_pt->nelement();
   for (unsigned e=0;e<n_el;e++)
    {
     FiniteElement* el_pt = mesh_pt->finite_element_pt(e);
   
     // Get the number of vertices (nplot=2 does the trick)
     unsigned n_plot=2;
     unsigned n_plot_points=el_pt->nplot_points(n_plot);

     // Loop over the number of plot points
     for (unsigned iplot=0;iplot<n_plot_points;iplot++)
      {
       Vector<double> s_local(n_lagrangian);
       Vector<double> zeta_global(n_lagrangian);

       // Get the local s
       el_pt->get_s_plot(iplot,n_plot,s_local);

       // Now interpolate to global (Lagrangian) coordinates
       el_pt->interpolated_zeta(s_local,zeta_global);

       // Check the max and min in each direction
       for(int i=0;i<n_lagrangian;i++)
        {
         //Is the coordinate less than the minimum?
         if(zeta_global[i] < zeta_min_local[i]) 
          {zeta_min_local[i] = zeta_global[i];}
         //Is the coordinate bigger than the maximum?
         if(zeta_global[i] > zeta_max_local[i]) 
          {zeta_max_local[i] = zeta_global[i];}
        }
      }
    }

   // Global extrema - in parallel, need to get max/min across all processors
   double zeta_min[n_lagrangian]; double zeta_max[n_lagrangian];
   for(int i=0;i<n_lagrangian;i++) {zeta_min[i] = 0.0; zeta_max[i] = 0.0;}

#ifdef OOMPH_HAS_MPI
   // If the mesh has been distributed...
   if (mesh_pt->mesh_has_been_distributed())
    {
     // .. we need a non-null communicator!
     if (Communicator_pt!=0)
      {
       int n_proc=Communicator_pt->nproc();
       if (n_proc>1)
        {
         //Get the minima and maxima over all processors
         MPI_Allreduce(zeta_min_local,zeta_min,n_lagrangian,MPI_DOUBLE,MPI_MIN,
                       Communicator_pt->mpi_comm());
         MPI_Allreduce(zeta_max_local,zeta_max,n_lagrangian,MPI_DOUBLE,MPI_MAX,
                       Communicator_pt->mpi_comm());
        }
      }
     else // Null communicator - throw an error
      {
       std::ostringstream error_message_stream;                           
       error_message_stream                                        
        << "Communicator not set for a MeshAsGeomObject\n"
        << "that was created from a distributed Mesh";
       throw OomphLibError(error_message_stream.str(),                  
                           "MeshAsGeomObject::get_min_and_max_coordinates(..)",
                           OOMPH_EXCEPTION_LOCATION);
      }
    }
   else // If the mesh hasn't been distributed then the 
        // max and min are the same on all processors
    {
     for(int i=0;i<n_lagrangian;i++) 
      {
       zeta_min[i] = zeta_min_local[i];
       zeta_max[i] = zeta_max_local[i];
      }
    }
#else // If we're not using MPI then the mesh can't be distributed
   for(int i=0;i<n_lagrangian;i++) 
    {
     zeta_min[i] = zeta_min_local[i];
     zeta_max[i] = zeta_max_local[i];
    }
#endif

   // Decrease/increase min and max to allow for any overshoot in
   // meshes that may move around
   // There's no point in doing this for DIM_LAGRANGIAN==1
   double percentage_offset=5.0;
   for(int i=0;i<n_lagrangian;i++)
    {
     double length = zeta_max[i] - zeta_min[i];
     zeta_min[i] -= ((percentage_offset/100.0)*length);
     zeta_max[i] += ((percentage_offset/100.0)*length);
    }

   // Set the entries as the Min/Max_coords vector
   for(int i=0;i<n_lagrangian;i++) 
    {
     Min_coords[i] = zeta_min[i]; 
     Max_coords[i] = zeta_max[i];
    }
  }

//========================================================================
///Initialise the "bin" structure for locating coordinates
//========================================================================
 void MeshAsGeomObject::create_bins_of_objects()
  {
   //Store the lagrangian dimension
   const unsigned n_lagrangian = this->nlagrangian();

   // Output message regarding bin structure setup if required
   if (Multi_domain_functions::Doc_stats)
    {
     oomph_info << "============================================" << std::endl;
     oomph_info << " MeshAsGeomObject: set up bin search with:" << std::endl;
     oomph_info << "   Nbin_x=" << Nbin_x << "  ";
     if (n_lagrangian>=2)
      {
       oomph_info << "Nbin_y=" << Nbin_y << "  ";
      }
     if (n_lagrangian==3)
      {
       oomph_info << "Nbin_z=" << Nbin_z;
      }
     oomph_info << std::endl;
     oomph_info << "  Xminmax=" << Min_coords[0] << " " << Max_coords[0] 
                << "  ";
     if (n_lagrangian>=2)
      {
       oomph_info << "Yminmax=" << Min_coords[1] << " " << Max_coords[1]
                  << "  ";
      }
     if (n_lagrangian==3)
      {
       oomph_info << "Zminmax=" << Min_coords[2] << " " << Max_coords[2] 
                  << "  ";
      }
     oomph_info << std::endl;
     oomph_info << "============================================" << std::endl;
    }

   /// Flush all objects out of the bin structure
   flush_bins_of_objects();

   //The storage for these bins is the product of the 
   //number of bins in all directions
   unsigned Nbin[3] ={Nbin_x, Nbin_y, Nbin_z};
   unsigned ntotalbin=Nbin[0];
   for(unsigned i=1;i<n_lagrangian;i++) {ntotalbin *= Nbin[i];}
   Bin_object_coord_pairs.resize(ntotalbin);

   ///Loop over subobjects (elements) to decide which bin they belong in...
   unsigned n_sub=Sub_geom_object_pt.size();

   // Some stats
   if (Multi_domain_functions::Doc_stats)
    {
     oomph_info << "There are " << n_sub << " element[s] to be put into bins"
                << std::endl << std::endl;
    }

   for (unsigned e=0;e<n_sub;e++)
    {
     // Cast to the element (sub-object) first
     FiniteElement* el_pt=dynamic_cast<FiniteElement*>(Sub_geom_object_pt[e]);

     // Get specified number of points within the element
     unsigned n_plot_points=
      el_pt->nplot_points(Multi_domain_functions::Nsample_points);

     for (unsigned iplot=0;iplot<n_plot_points;iplot++)
      {
       // Storage for local and global coordinates
       Vector<double> local_coord(n_lagrangian,0.0);
       Vector<double> global_coord(n_lagrangian,0.0);

       // Get local coordinate and interpolate to global
       el_pt->get_s_plot(iplot,
                         Multi_domain_functions::Nsample_points,local_coord);
       el_pt->interpolated_zeta(local_coord,global_coord);

       //Which bin are the global coordinates in?
       unsigned bin_number=0;
       unsigned multiplier=1;
       // Loop over the dimension
       for(unsigned i=0;i<n_lagrangian;i++)
        {
         unsigned bin_number_i = 
          int(Nbin[i]*(
               (global_coord[i] - Min_coords[i])/
               (Max_coords[i] - Min_coords[i])));
         //Buffer the case when the global coordinate is the maximum
         //value
         if(bin_number_i==Nbin[i]) {bin_number_i -= 1;}
         //Add to the bin number
         bin_number += multiplier*bin_number_i;
         //Sort out the multiplier
         multiplier *= Nbin[i];
        }
       
       //Add element-sample local coord pair to the calculated bin
       Bin_object_coord_pairs[bin_number].push_back
        (std::make_pair(el_pt,local_coord));
      }
    }

  }

//========================================================================
///Calculate the bin numbers of all the neighbours to "bin" given the level
//========================================================================
 void MeshAsGeomObject::get_neighbouring_bins_helper(
  const unsigned& bin, const unsigned& level,
  Vector<unsigned>& neighbour_bin)
  {
   const unsigned n_lagrangian = this->nlagrangian();
   // This will be different depending on the number of Lagrangian
   // coordinates
   if (n_lagrangian==1)
    {
     // Single "loop" in one direction - always a vector of max size 2
     unsigned nbr_bin_left=bin-level;
     if ((nbr_bin_left>=0) && (nbr_bin_left<Nbin_x))
      {
       unsigned nbr_bin=nbr_bin_left;
       neighbour_bin.push_back(nbr_bin);
      }
     unsigned nbr_bin_right=bin+level;
     if ((nbr_bin_right>=0) && (nbr_bin_right<Nbin_x) && 
         (nbr_bin_right!=nbr_bin_left))
      {
       unsigned nbr_bin=nbr_bin_right;
       neighbour_bin.push_back(nbr_bin);
      }
    }
   else if (n_lagrangian==2)
    {
     unsigned n_total_bin=Nbin_x*Nbin_y;

     // Which row of the bin structure is the current bin on?
     // This is just given by the integer answer of dividing bin
     // by Nbin_x (the number of bins in a single row)
     // e.g. in a 6x6 grid, bins 6 through 11 would all have bin_row=1
     unsigned bin_row=bin/Nbin_x;

     // The neighbour_bin vector contains all bin numbers at the 
     // specified "distance" (level) away from the current bin

     // Row/column length
     unsigned n_length=(level*2)+1;

     // Loop over the rows
     for (unsigned j=0;j<n_length;j++)
      {
       // Loop over the columns
       for (unsigned i=0;i<n_length;i++)
        {
         // Only do this for all the first & last row, and the
         // end points of every other row
         if ((j==0) || (j==n_length-1) || (i==0) || (i==n_length-1))
          {
           unsigned nbr_bin=bin-level+i-((level-j)*Nbin_x);
           // This number might fall on the wrong
           // row of the bin structure; this needs to be tested

           // Which row is this number on? (see above)
           unsigned nbr_bin_row=nbr_bin/Nbin_x;

           // Which row should it be on?
           unsigned row=bin_row-level+j;

           // These numbers for the rows must match; 
           // if it is then add nbr_bin to the neighbour scheme
           // (The bin number must also be greater than zero
           //  and less than the total number of bins)
           if ((row==nbr_bin_row) && (nbr_bin>=0) && (nbr_bin<n_total_bin))
            {
             neighbour_bin.push_back(nbr_bin);
            }  
          }
        }

      }
    }
   else if (n_lagrangian==3)
    {
     unsigned n_total_bin=Nbin_x*Nbin_y*Nbin_z;

     // Which layer of the bin structure is the current bin on?
     // This is just given by the integer answer of dividing bin
     // by Nbin_x*Nbin_y (the number of bins in a single layer
     // e.g. in a 6x6x6 grid, bins 72 through 107 would all have bin_layer=2
     unsigned bin_layer=bin/(Nbin_x*Nbin_y);

     // Which row in this layer is the bin number on?
     unsigned bin_row=(bin/Nbin_x)-(bin_layer*Nbin_y);

     // The neighbour_bin vector contains all bin numbers at the 
     // specified "distance" (level) away from the current bin

     // Row/column/layer length
     unsigned n_length=(level*2)+1;

     // Loop over the layers
     for (unsigned k=0;k<n_length;k++)
      {
       // Loop over the rows
       for (unsigned j=0;j<n_length;j++)
        {
         // Loop over the columns
         for (unsigned i=0;i<n_length;i++)
          {
           // Only do this for the end points of every row/layer/column
           if ((k==0) || (k==n_length-1) || (j==0) || 
               (j==n_length-1) || (i==0) || (i==n_length-1))
            {
             unsigned nbr_bin=bin-level+i-((level-j)*Nbin_x)-
              ((level-k)*Nbin_x*Nbin_y);
             // This number might fall on the wrong
             // row or layer of the bin structure; this needs to be tested

             // Which layer is this number on?
             unsigned nbr_bin_layer=nbr_bin/(Nbin_x*Nbin_y);

             // Which row is this number on? (see above)
             unsigned nbr_bin_row=(nbr_bin/Nbin_x)-(nbr_bin_layer*Nbin_y);

             // Which layer and row should it be on, given level?
             unsigned layer=bin_layer-level+k;
             unsigned row=bin_row-level+j;

             // These layers and rows must match up:
             // if so then add nbr_bin to the neighbour schemes
             // (The bin number must also be greater than zero
             //  and less than the total number of bins)
             if ((row==nbr_bin_row) && (layer==nbr_bin_layer)
                 && (nbr_bin>=0) && (nbr_bin<n_total_bin))
              {
               neighbour_bin.push_back(nbr_bin);
              }  
            }

          }
        }
      }

    }
  }
 

}
