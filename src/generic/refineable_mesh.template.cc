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
//Templated refineable mesh functions

//Include guards to prevent multiple inclusion of the header
#ifndef OOMPH_REFINEABLE_MESH_TEMPLATE_CC
#define OOMPH_REFINEABLE_MESH_TEMPLATE_CC

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

//oomph-lib headers
#include "refineable_mesh.h"
#include "missing_masters.h"
#include "missing_masters.template.cc"

namespace oomph
{

#ifdef OOMPH_HAS_MPI

 //========================================================================
 /// Additional actions required to synchronise halo nodes where master
 /// nodes could not be found during synchronise_hanging_nodes().
 /// BENFLAG: Overloaded from Mesh class to take care of master nodes on
 ///          the outer edge of the halo layer which do not exist on that
 ///          processor. This fixes problems with the synchronisation of
 ///          hanging nodes for elements with non-uniformly spaced nodes.
 //========================================================================
 template<class ELEMENT>
 void TreeBasedRefineableMesh<ELEMENT>::
 additional_synchronise_hanging_nodes(
  OomphCommunicator* comm_pt,
  const unsigned& ncont_interpolated_values)
 {
  //This provides all the node-adding helper functions required to reconstruct
  //the missing halo master nodes on this processor
  using namespace Missing_masters_functions;
  

  double t_start = 0.0;
  double t_end = 0.0;
  if (Global_timings::Doc_comprehensive_timings)
   {
    t_start=TimingHelpers::timer();
   }

  // Store number of processors and current process 
  MPI_Status status;
  int n_proc=comm_pt->nproc();
  int my_rank=comm_pt->my_rank();

      
#ifdef PARANOID
  //BENFLAG: Paranoid check to make sure nothing else is using the
  //         external storage. This will need to be changed at some
  //         point if we are to use non-uniformly spaced nodes in
  //         multi-domain problems.
  bool err=false;
  //Print out external storage
  for(int d=0; d<n_proc; d++)
   {
    if(d!=my_rank)
     {
      //Check to see if external storage is being used by anybody else
      if(nexternal_haloed_node(d)!=0)
       {
        err=true;
        oomph_info << "Processor " << my_rank << "'s external haloed nodes with processor " << d << " are:" << std::endl;
        for(unsigned i=0; i<nexternal_haloed_node(d); i++)
         {
          oomph_info << "external_haloed_node_pt("<<d<<","<<i<<") = " << external_haloed_node_pt(d,i) << std::endl;
          oomph_info << "x = ( " << external_haloed_node_pt(d,i)->x(0) << " , " << external_haloed_node_pt(d,i)->x(1) << " )" << std::endl;
         }
       }
     }
   }
  for(int d=0; d<n_proc; d++)
   {
    if(d!=my_rank)
     {
      //Check to see if external storage is being used by anybody else
      if(nexternal_halo_node(d)!=0)
       {
        err=true;
        oomph_info << "Processor " << my_rank << "'s external halo nodes with processor " << d << " are:" << std::endl;
        for(unsigned i=0; i<nexternal_halo_node(d); i++)
         {
          oomph_info << "external_halo_node_pt("<<d<<","<<i<<") = " << external_halo_node_pt(d,i) << std::endl;
          oomph_info << "x = ( " << external_halo_node_pt(d,i)->x(0) << " , " << external_halo_node_pt(d,i)->x(1) << " )" << std::endl;
         }
       }
     }
   }
  if(err)
   {
    std::ostringstream err_stream;
    err_stream << "There are already some nodes in the external storage"
               << std::endl
               << "for this mesh. This bit assumes that nothing else"
               << std::endl
               << "uses this storage (for now).";
    throw OomphLibError(
     err_stream.str(),
     "TreeBasedRefineableMesh<ELEMENT>::synchronise_hanging_nodes()",
     OOMPH_EXCEPTION_LOCATION);
   }
#endif



  // BENFLAG: Compare the halo and haloed nodes for discrepancies in hanging
  //          status
  
  // Storage for the hanging status of halo/haloed nodes on elements
  Vector<Vector<int> > haloed_hanging(n_proc);
  Vector<Vector<int> > halo_hanging(n_proc);

  // Storage for the haloed nodes with discrepancies in their hanging status
  // with each processor
  Vector<std::map<Node*,unsigned> >
   haloed_hanging_node_with_discrepancy_pt(n_proc);
 
  if (Global_timings::Doc_comprehensive_timings)
   {
    t_start = TimingHelpers::timer();
   }
 
  // Store number of continuosly interpolated values as int
  int ncont_inter_values=ncont_interpolated_values;
  
  // Loop over processes: Each processor checks that is haloed nodes
  // with proc d have consistent hanging stats with halo counterparts.
  for (int d=0; d<n_proc; d++)
   {
 
    // No halo with self: Setup hang info for my haloed nodes with proc d 
    // then get ready to receive halo info from processor d.
    if (d!=my_rank) 
     {
      
      // Loop over haloed nodes
      unsigned nh=nhaloed_node(d);     
      for (unsigned j=0;j<nh;j++)
       {
        // Get node
        Node* nod_pt=haloed_node_pt(d,j);
        
        // Loop over the hanging status for each interpolated variable
        // (and the geometry)
        for (int icont=-1; icont<ncont_inter_values; icont++)
         { 
          // Store the hanging status of this haloed node
          if (nod_pt->is_hanging(icont))
           {
            unsigned n_master=nod_pt->hanging_pt(icont)->nmaster();
            haloed_hanging[d].push_back(n_master);
           }
          else
           {
            haloed_hanging[d].push_back(0);
           }
         }
       }
      
      // Receive the hanging status information from the corresponding process
      unsigned count_haloed=haloed_hanging[d].size();
      
#ifdef PARANOID
      // Check that number of halo and haloed data match
      unsigned tmp=0;     
      MPI_Recv(&tmp,1,MPI_UNSIGNED,d,0,comm_pt->mpi_comm(),&status);
      if (tmp!=count_haloed)
       {
        std::ostringstream error_stream;
        error_stream  << "Number of halo data, " << tmp  
                      << ", does not match number of haloed data, " 
                      << count_haloed << std::endl;
        throw OomphLibError(
         error_stream.str(),
         "TreeBasedRefineableMeshBase::synchronise_hanging_nodes(...)",
         OOMPH_EXCEPTION_LOCATION);
       }
#endif
      
      // Get the data (if any)
      if (count_haloed!=0)
       {
        halo_hanging[d].resize(count_haloed);
        MPI_Recv(&halo_hanging[d][0],count_haloed,MPI_INT,d,0,
                 comm_pt->mpi_comm(),&status);       
       }
     }
    else // d==my_rank, i.e. current process: Send halo hanging status 
         // to process dd where it's received (see above) and compared
         // and compared against the hang status of the haloed nodes
     {
      for (int dd=0; dd<n_proc; dd++)
       {
        // No halo with yourself
        if (dd!=d)
         {
        
          // Storage for halo hanging status and counter
          Vector<int> local_halo_hanging;
 
          // Loop over halo nodes
          unsigned nh=nhalo_node(dd);
          for (unsigned j=0;j<nh;j++)
           {
            // Get node
            Node* nod_pt=halo_node_pt(dd,j);
            
            // Loop over the hanging status for each interpolated variable
            // (and the geometry)
            for (int icont=-1; icont<ncont_inter_values; icont++)
             { 
              // Store hanging status of halo node
              if (nod_pt->is_hanging(icont))
               {
                unsigned n_master=nod_pt->hanging_pt(icont)->nmaster();
                local_halo_hanging.push_back(n_master);
               }
              else
               {
                local_halo_hanging.push_back(0);
               }
             }
           }
          
          
          // Send the information to the relevant process
          unsigned count_halo=local_halo_hanging.size();
          
#ifdef PARANOID
          // Check that number of halo and haloed data match
          MPI_Send(&count_halo,1,MPI_UNSIGNED,dd,0,comm_pt->mpi_comm());
#endif
          
          // Send data (if any)
          if (count_halo!=0)
           {
            MPI_Send(&local_halo_hanging[0],count_halo,MPI_INT,
                     dd,0,comm_pt->mpi_comm());
           }
         }
       }
     }
   }
 
  if (Global_timings::Doc_comprehensive_timings)
   {
    t_end = TimingHelpers::timer();
    oomph_info << "Time for first all-to-all in synchronise_hanging_nodes(): " 
               << t_end-t_start << std::endl;
    t_start = TimingHelpers::timer();
   }
  
   
  // Now compare equivalent halo and haloed vectors to find discrepancies.
  // It is possible that a master node may not be on either process involved
  // in the halo-haloed scheme; to work round this, we use the shared_node
  // storage scheme, which stores all nodes that are on each pair of processors
  // in the same order on each of the two processors
 
 
  // Loop over domains: Each processor checks consistency of hang status
  // of its haloed nodes with proc d against the halo counterpart. Haloed
  // wins if there are any discrepancies.
  for (int d=0; d<n_proc; d++)
   {
    // No halo with yourself
    if (d!=my_rank)      
     {
      // Counter for traversing haloed data
      unsigned count=0;
 
      // Loop over haloed nodes
      unsigned nh=nhaloed_node(d);
      for (unsigned j=0;j<nh;j++)
       {
        // Get node
        Node* nod_pt=haloed_node_pt(d,j);
        
        // Loop over the hanging status for each interpolated variable
        // (and the geometry)
        for (int icont=-1; icont<ncont_inter_values; icont++)
         { 
          // Compare hanging status of halo/haloed counterpart structure
          
          // Haloed is is hanging and haloed has different number
          // of master nodes (which includes none in which case it isn't
          // hanging)
          if ((haloed_hanging[d][count]>0)&&
              (haloed_hanging[d][count]!=halo_hanging[d][count]))
           {
            // Store this node so it can be synchronised later
            haloed_hanging_node_with_discrepancy_pt[d].insert(
                                  std::pair<Node*,unsigned>(nod_pt,d));
           }
          // Increment counter for number of haloed data
          count++;
         } // end of loop over icont
       } // end of loop over haloed nodes
     }
   }// end loop over all processors

  

  // BENFLAG: Populate external halo(ed) node storage with master nodes of
  //          halo(ed) nodes

  // Loop over domains: Each processor checks consistency of hang status
  // of its haloed nodes with proc d against the halo counterpart. Haloed
  // wins if there are any discrepancies.
  for (int d=0; d<n_proc; d++)
   {
    // No halo with yourself
    if (d!=my_rank)      
     {
      //Now add haloed master nodes to external storage
      //===============================================

      //Storage for data to be sent
      Vector<unsigned> send_unsigneds(0);
      Vector<double> send_doubles(0);

      //Count number of haloed nonmaster nodes for halo process
      unsigned nhaloed_nonmaster_nodes_processed = 0;
      Vector<unsigned> haloed_nonmaster_node_index(0);

      //Loop over hanging halo nodes with discrepancies
      std::map<Node*,unsigned>::iterator j;
      for(j=haloed_hanging_node_with_discrepancy_pt[d].begin(); j!=haloed_hanging_node_with_discrepancy_pt[d].end(); j++)
       {
        Node* nod_pt = (*j).first;
        //Find index of this haloed node in the halo storage of processor d
        //(But find in shared node storage in case it is actually haloed on
        //another processor which we don't know about)
        std::vector<Node*>::iterator it
         = std::find(Shared_node_pt[d].begin(),
                     Shared_node_pt[d].end(),
                     nod_pt);
        if(it != Shared_node_pt[d].end())
         {
          //Tell other processor to create this node
          //send_unsigneds.push_back(1);
          nhaloed_nonmaster_nodes_processed++;

          //Tell the other processor where to find this node in its halo node
          //storage
          unsigned index = it - Shared_node_pt[d].begin();
          haloed_nonmaster_node_index.push_back(index);
         
          //Tell this processor that this node is really a haloed node
          //This also packages up the data which needs to be sent to the
          //processor on which the halo equivalent node lives
          recursively_add_masters_of_external_haloed_node
           (d, nod_pt, this, ncont_inter_values,
            send_unsigneds, send_doubles);
         }
        else
         {
          throw OomphLibError(
                 "Haloed node not found in haloed node storage",
                 "TreeBasedRefineableMesh<ELEMENT>::additional_setup_shared_node_scheme()",
                 OOMPH_EXCEPTION_LOCATION);
         }
       }

      //How much data needs to be sent?
      unsigned send_unsigneds_count = send_unsigneds.size();
      unsigned send_doubles_count = send_doubles.size();

      //Send ammount of data
      MPI_Send(&send_unsigneds_count,1,MPI_UNSIGNED,d,0,comm_pt->mpi_comm());
      MPI_Send(&send_doubles_count,1,MPI_UNSIGNED,d,1,comm_pt->mpi_comm());

      //Send to halo process the number of haloed nodes we processed
      MPI_Send(&nhaloed_nonmaster_nodes_processed,1,MPI_UNSIGNED,d,2,
               comm_pt->mpi_comm());
      if(nhaloed_nonmaster_nodes_processed>0)
       {
        MPI_Send(&haloed_nonmaster_node_index[0],
                 nhaloed_nonmaster_nodes_processed,MPI_UNSIGNED,d,3,
                 comm_pt->mpi_comm());
       }

      //Send data about external halo nodes
      if(send_unsigneds_count>0)
       {
        //Only send if there is anything to send
        MPI_Send(&send_unsigneds[0],send_unsigneds_count,MPI_UNSIGNED,d,4,
                 comm_pt->mpi_comm());
       }
      if(send_doubles_count>0)
       {
        //Only send if there is anything to send
        MPI_Send(&send_doubles[0],send_doubles_count,MPI_DOUBLE,d,5,
                 comm_pt->mpi_comm());
       }
     
     }
    else // (d==my_rank), current process
     {
      //Now construct and add halo versions of master nodes to external storage
      //=======================================================================

      //Loop over processors to get data
      for(int dd=0; dd<n_proc; dd++)
       {
        //Don't talk to yourself
        if(dd!=d)
         {
          //How much data to be received
          unsigned nrecv_unsigneds = 0;
          unsigned nrecv_doubles = 0;
          MPI_Recv(&nrecv_unsigneds,1,MPI_UNSIGNED,dd,0,
                   comm_pt->mpi_comm(),&status);
          MPI_Recv(&nrecv_doubles,1,MPI_UNSIGNED,dd,1,
                   comm_pt->mpi_comm(),&status);

          //Get from haloed process the number of halo nodes we need to process
          unsigned nhalo_nonmaster_nodes_to_process = 0;
          MPI_Recv(&nhalo_nonmaster_nodes_to_process,1,MPI_UNSIGNED,dd,2,
                   comm_pt->mpi_comm(),&status);
          Vector<unsigned> halo_nonmaster_node_index(
                            nhalo_nonmaster_nodes_to_process);
          if (nhalo_nonmaster_nodes_to_process!=0)
           {
            MPI_Recv(&halo_nonmaster_node_index[0],
                     nhalo_nonmaster_nodes_to_process,MPI_UNSIGNED,dd,3,
                     comm_pt->mpi_comm(),&status);
           }

          //Storage for data to be received
          Vector<unsigned> recv_unsigneds(nrecv_unsigneds);
          Vector<double> recv_doubles(nrecv_doubles);
         
          //Receive data about external haloed equivalent nodes
          if(nrecv_unsigneds>0)
           {
            //Only send if there is anything to send
            MPI_Recv(&recv_unsigneds[0],nrecv_unsigneds,MPI_UNSIGNED,dd,4,
                     comm_pt->mpi_comm(),&status);
           }
          if(nrecv_doubles>0)
           {
            //Only send if there is anything to send
            MPI_Recv(&recv_doubles[0],nrecv_doubles,MPI_DOUBLE,dd,5,
                     comm_pt->mpi_comm(),&status);
           }

          //Counters for flat packed data counters
          unsigned recv_unsigneds_count = 0;
          unsigned recv_doubles_count = 0;

          //Loop over halo nodes with discrepancies in their hanging status
          for(unsigned j=0; j<nhalo_nonmaster_nodes_to_process; j++)
           {
            //Get pointer to halo nonmaster node which needs processing
            //(But given index is its index in the shared storage)
            Node* nod_pt = shared_node_pt(dd,halo_nonmaster_node_index[j]);

#ifdef PARANOID
            //Check if we have a MacroElementNodeUpdateNode
            if(dynamic_cast<MacroElementNodeUpdateNode*>(nod_pt))
             {
              //BENFLAG: This currently doesn't work for
              //         MacroElementNodeUpdateNodes because these require
              //         MacroElementNodeUpdateElements to be created for
              //         the missing halo nodes which will be added (which
              //         doesn't currently work but I don't know why not)
              //         We follow the convoluted logic in the multi-domain
              //         functions because
              //         a) it requires only minimal modifications to the
              //            existing functions, and
              //         b) this is probably the correct thing to do if we
              //            are to fix this problem
              std::ostringstream err_stream;
              err_stream << "This currently doesn't work for"
                         << std::endl
                         << "MacroElementNodeUpdateNodes because these require"
                         << std::endl
                         << "MacroElementNodeUpdateElements to be created for"
                         << std::endl
                         << "the missing halo nodes which will be added"
                         << std::endl;
              throw OomphLibError(err_stream.str(),
                                  "TreeBasedRefineableMesh<ELEMENT>::additional_setup_shared_node_scheme()",
                                  OOMPH_EXCEPTION_LOCATION);
             }
#endif
            
            //Construct copy of node and add to external halo node storage.
            unsigned loc_p = (unsigned) dd;
            unsigned node_index;
            recursively_add_masters_of_external_halo_node_to_storage<ELEMENT>
             (nod_pt, this, loc_p, node_index, ncont_inter_values,
              recv_unsigneds_count, recv_unsigneds,
              recv_doubles_count, recv_doubles);
           }

         } // end of dd!=d
       } // end of second loop over all processors
     }
   } // end loop over all processors


  if (Global_timings::Doc_comprehensive_timings)
   {
    t_end = TimingHelpers::timer();
    oomph_info << "Time for second all-to-all in synchronise_hanging_nodes() " 
               << t_end-t_start << std::endl;
    t_start = TimingHelpers::timer();
   }

  // BENFLAG: Populate external halo(ed) node storage with master nodes of
  //          halo(ed) nodes [end]
 
  //Count how many external halo/haloed nodes are added
  unsigned external_halo_count=0;
  unsigned external_haloed_count=0;

  //Flag to test whether we attampt to add any duplicate haloed nodes to the
  //shared storage -- if this is the case then we have duplicate halo nodes
  //on another processor but with different pointers and the shared scheme
  //will not be set up correctly
  bool duplicate_haloed_node_exists = false;

  //Loop over all the processors and add the shared nodes
  for (int d=0;d<n_proc;d++)
   {

    // map of bools for whether the (external) node has been shared,
    // initialised to 0 (false) for each domain d
    std::map<Node*,bool> node_shared;

    // For all domains lower than the current domain: Do halos first
    // then haloed, to ensure correct order in lookup scheme from
    // the other side
    if (d<my_rank)
     {
      //Do external halo nodes
      unsigned nexternal_halo_nod=nexternal_halo_node(d);
      for (unsigned j=0;j<nexternal_halo_nod;j++)
       {
        Node* nod_pt=external_halo_node_pt(d,j);
       
        // Add it as a shared node from current domain
        if (!node_shared[nod_pt])
         {
          this->add_shared_node_pt(d,nod_pt);
          node_shared[nod_pt]=true;
          external_halo_count++;
         }
       
       } // end loop over nodes
     
      //Do external haloed nodes
      unsigned nexternal_haloed_nod=nexternal_haloed_node(d);
      for (unsigned j=0;j<nexternal_haloed_nod;j++)
       {
        Node* nod_pt=external_haloed_node_pt(d,j);
       
        // Add it as a shared node from current domain
        if (!node_shared[nod_pt])
         {
          this->add_shared_node_pt(d,nod_pt);
          node_shared[nod_pt]=true;
          external_haloed_count++;
         }
        else
         {
          duplicate_haloed_node_exists = true;
         }
       
       } // end loop over nodes

     }

    // If the domain is bigger than the current rank: Do haloed first
    // then halo, to ensure correct order in lookup scheme from
    // the other side
    if (d>my_rank)
     {
      //Do external haloed nodes
      unsigned nexternal_haloed_nod=nexternal_haloed_node(d);
      for (unsigned j=0;j<nexternal_haloed_nod;j++)
       {
        Node* nod_pt=external_haloed_node_pt(d,j);
       
        // Add it as a shared node from current domain
        if (!node_shared[nod_pt])
         {
          this->add_shared_node_pt(d,nod_pt);
          node_shared[nod_pt]=true;
          external_haloed_count++;
         }
        else
         {
          duplicate_haloed_node_exists = true;
         }
       
       } // end loop over nodes
     
      //Do external halo nodes
      unsigned nexternal_halo_nod=nexternal_halo_node(d);
      for (unsigned j=0;j<nexternal_halo_nod;j++)
       {
        Node* nod_pt=external_halo_node_pt(d,j);
       
        // Add it as a shared node from current domain
        if (!node_shared[nod_pt])
         {
          this->add_shared_node_pt(d,nod_pt);
          node_shared[nod_pt]=true;
          external_halo_count++;
         }
       
       } // end loop over nodes

     } // end if (d ...)

   } // end loop over processes

  
  //Say how many external halo/haloed nodes were added
  oomph_info << "INFO: " << external_halo_count
             << " external halo nodes and"
             << std::endl;
  oomph_info << "INFO: " << external_haloed_count
             << " external haloed nodes were added to the shared node scheme"
             << std::endl;

  //If we added duplicate haloed nodes, throw an error
  if(duplicate_haloed_node_exists)
   {
    //BENFLAG: Let my_rank==A. If this has happened then it means that
    //         duplicate haloed nodes exist on another processor (B). This
    //         problem arises if a master of a haloed node with a discrepancy
    //         is haloed with a different processor (C). A copy is constructed
    //         in the external halo storage on processor (B) because that node
    //         is not found in the (internal) haloed storage on (A) with (B)
    //         but that node already exists on processor (B) in the (internal)
    //         halo storage with processor (C). Thus two copies of this master
    //         node now exist on processor (B).

    std::ostringstream err_stream;
    err_stream << "Duplicate halo nodes exist on another processor!"
               << std::endl
               << "(See source code for more detailed explanation)"
               << std::endl;

    throw OomphLibError(err_stream.str(),
                        "TreeBasedRefineableMesh<ELEMENT>::additional_setup_shared_node_scheme()",
                        OOMPH_EXCEPTION_LOCATION);
   }


  if (Global_timings::Doc_comprehensive_timings)
   {
    t_end = TimingHelpers::timer();
    oomph_info << "Time for identification of shared nodes: " 
               << t_end-t_start << std::endl;
   }

 }

#endif

}

#endif
