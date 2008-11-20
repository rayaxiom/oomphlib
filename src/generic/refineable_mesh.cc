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
#include "refineable_mesh.h"

#ifdef OOMPH_HAS_MPI
#include "mpi.h"
#endif

namespace oomph
{

//========================================================================
/// Get refinement pattern of mesh: Consider the hypothetical mesh
/// obtained by truncating the refinement of the current mesh to a given level 
/// (where \c level=0 is the un-refined base mesh). To advance
/// to the next refinement level, we need to refine (split) the
/// \c to_be_refined[level].size() elements identified by the
/// element numbers contained in \c vector to_be_refined[level][...]
//========================================================================
void RefineableMeshBase::get_refinement_pattern(
 Vector<Vector<unsigned> >& to_be_refined)
{
 // Extract *all* elements from current (fully refined) mesh.
 Vector<Tree*> all_tree_nodes_pt;
 forest_pt()->stick_all_tree_nodes_into_vector(all_tree_nodes_pt);
 
 // Find out maximum refinement level
 unsigned max_level=0;
 unsigned nnodes=all_tree_nodes_pt.size();
 for (unsigned e=0;e<nnodes;e++)
  {
   unsigned level=all_tree_nodes_pt[e]->level();
   if (level>max_level) max_level=level;
  }

 // Assign storage for refinement pattern
 to_be_refined.clear();
 to_be_refined.resize(max_level);
 Vector<unsigned> el_count(max_level);
 
 // Initialise count of elements that exist in mesh when refinement
 // has proceeded to this level
 for (unsigned l=0;l<max_level;l++)
  {
   el_count[l]=0;
  }
   
 // Loop over all levels and extract all elements that exist
 // in reference mesh when refinement has proceeded to this level
 for (unsigned l=0;l<max_level;l++)
  {
   // Loop over all elements (tree nodes)
   for (unsigned e=0;e<nnodes;e++)
    {
     // What level does this element exist on?
     unsigned level=all_tree_nodes_pt[e]->level();
   
     // Element is part of the mesh at this refinement level
     // if it exists at this level OR if it exists at a lower level
     // and is a leaf
     // hierher if ((level==l)||(level<l)&&(all_tree_nodes_pt[e]->is_leaf()))
     if ((level==l)||((level<l)&&(all_tree_nodes_pt[e]->is_leaf())))
      {
       // If element exsts at this level and is not a leaf it will
       // be refined when we move to the next level:
       if ((level==l)&&(!all_tree_nodes_pt[e]->is_leaf()))
        {
         // Add element number (in mesh at current refinement level)
         // to the list of elements that need to be refined
         to_be_refined[l].push_back(el_count[l]);
        }
       // Element exists in this mesh: Add to counter
       el_count[l]++;
      }
    }
  }
}


//========================================================================
/// Refine original, unrefined mesh according to specified refinement 
/// pattern (relative to original, unrefined mesh).
//========================================================================
void RefineableMeshBase::refine_base_mesh(
 Vector<Vector<unsigned> >& to_be_refined)
{


#ifdef PARANOID
 // Sanity check: Mesh has to be unrefined
 unsigned my_max,my_min;
 get_refinement_levels(my_min,my_max);
 if (my_max!=0)
  {
   std::string error_message =
    "Can only refine according to another meshes' refinement\n";
   error_message += "structure if mesh itself is still completely unrefined ";

   throw OomphLibError(error_message,
                       "RefineableMeshBase::refine_base_mesh()",
                       OOMPH_EXCEPTION_LOCATION);
  }
#endif
   

 // Max refinement level:
 unsigned max_level=to_be_refined.size();

 // Do refinement steps in current mesh
 for (unsigned l=0;l<max_level;l++)
  {
   // Loop over elements that need to be refined at this level
   unsigned n_to_be_refined=to_be_refined[l].size();

   // Select relevant elements to be refined
   for (unsigned i=0;i<n_to_be_refined;i++)
    {
     dynamic_cast<RefineableElement*>(
      this->element_pt(to_be_refined[l][i]))->select_for_refinement();
    }

   // Now do the actual mesh refinement
   adapt_mesh();
   
//    // Doc refinement hierarchy
//    char filename[100];   
//    sprintf(filename,"mesh%i.dat",l);
//    string name=filename;
//    output(name);

  }

}


//========================================================================
/// Refine base mesh according to refinement pattern in restart file
//========================================================================
void RefineableMeshBase::refine(std::ifstream& restart_file)
{
 // Assign storage for refinement pattern
 Vector<Vector<unsigned> > to_be_refined;

 // Read refinement pattern
 read_refinement(restart_file,to_be_refined);

 // Refine
 refine_base_mesh(to_be_refined);

}


//========================================================================
/// Dump refinement pattern to allow for rebuild
///
//========================================================================
void RefineableMeshBase::dump_refinement(std::ostream &outfile)
{
 // Assign storage for refinement pattern
 Vector<Vector<unsigned> > to_be_refined;

 // Get refinement pattern of reference mesh:
 get_refinement_pattern(to_be_refined);

 // Dump max refinement level:
 unsigned max_level=to_be_refined.size();
 outfile << max_level << " # max. refinement level " << std::endl;

 // Doc the numbers of the elements that need to be refined at this level
 for (unsigned l=0;l<max_level;l++)
  {
   // Loop over elements that need to be refined at this level
   unsigned n_to_be_refined=to_be_refined[l].size();
   outfile << n_to_be_refined << " # number of elements to be refined. " 
           << "What follows are the numbers of the elements. " << std::endl;

   // Select relevant elements to be refined
   for (unsigned i=0;i<n_to_be_refined;i++)
    {
     outfile << to_be_refined[l][i] << std::endl;
    }
  }   
}


//========================================================================
/// Read refinement pattern to allow for rebuild
///
//========================================================================
void RefineableMeshBase::read_refinement(
 std::ifstream& restart_file, Vector<Vector<unsigned> >& to_be_refined)
{

 std::string input_string;

 // Read max refinement level:

 // Read line up to termination sign
 getline(restart_file,input_string,'#');

 // Ignore rest of line
 restart_file.ignore(80,'\n');

 // Convert
 unsigned max_level=atoi(input_string.c_str());

 // Assign storage for refinement pattern
 to_be_refined.resize(max_level);

 // Read the number of the elements that need to be refined at different levels
 for (unsigned l=0;l<max_level;l++)
  {

   // Read line up to termination sign
   getline(restart_file,input_string,'#');

   // Ignore rest of line
   restart_file.ignore(80,'\n');

   // Convert
   unsigned n_to_be_refined=atoi(input_string.c_str());;

   // Assign storage
   to_be_refined[l].resize(n_to_be_refined);

   // Read numbers of the elements that need to be refined
   for (unsigned i=0;i<n_to_be_refined;i++)
    {
     restart_file >> to_be_refined[l][i];
    }
  }   
}





//========================================================================
/// Do adaptive refinement for mesh.
/// - Pass Vector of error estimates for all elements.
/// - Refine those whose errors exceeds the threshold
/// - (Try to) unrefine those whose errors is less than
///   threshold (only possible if the three brothers also want to be 
///   unrefined, of course.)
/// - Update the nodal positions in the whole lot
/// - Store # of refined/unrefined elements.
/// - Doc refinement process (if required)
//========================================================================
void RefineableMeshBase::adapt(Vector<double>& elemental_error)
 {

  //using namespace QuadTreeNames;

  //Set the refinement tolerance to be the max permissible error
  double refine_tol=max_permitted_error();
  //Set the unrefinement tolerance to be the min permissible error 
  double unrefine_tol=min_permitted_error();

  // Setup doc info
  DocInfo local_doc_info;
  if (doc_info_pt()==0) {local_doc_info.doc_flag()=false;}
  else {local_doc_info=doc_info();}
                              

  // Check that the errors make sense
  if (refine_tol<=unrefine_tol)
   {
    std::ostringstream error_stream;
    error_stream << "Refinement tolerance <= Unrefinement tolerance" 
                 << refine_tol << " " << unrefine_tol << std::endl
                 << "doesn't make sense and will almost certainly crash" 
                 << std::endl
                 << "this beautiful code!" << std::endl;
    
    throw OomphLibError(error_stream.str(),
                        "RefineableMeshBase::adapt()",
                        OOMPH_EXCEPTION_LOCATION);
   }


  //Select elements for refinement and unrefinement
  //==============================================
  // Reset counter for number of elements that would like to be
  // refined further but can't
  nrefinement_overruled()=0;

  // Note: Yes, this needs to be a map because we'll have to check
  // the refinement wishes of brothers (who we only access via pointers)
  std::map<RefineableElement*,bool> wants_to_be_unrefined;

  unsigned n_refine=0;
  // Loop over all elements and mark them according to the error criterion
  unsigned long Nelement=this->nelement();
  for (unsigned long e=0;e<Nelement;e++)
   {
    //(Cast) pointer to the element
    RefineableElement* el_pt =
     dynamic_cast<RefineableElement*>(this->element_pt(e));
 
   //Initially element is not to be refined
    el_pt->deselect_for_refinement();
    
    //If the element error exceeds the threshold ...
    if(elemental_error[e] > refine_tol)
     {
      // ... and its refinement level is less than the maximum desired level
      //mark is to be refined
      if(el_pt->refinement_level() < max_refinement_level())
       {
        el_pt->select_for_refinement();
        n_refine++;
       }
      // ... otherwise mark it as having been over-ruled
      else
       {
        nrefinement_overruled()+=1;
       }
     }
    
    //Now worry about unrefinement (first pass)

    // Is my error too small AND do I have a father?
    if ((elemental_error[e]<unrefine_tol)&&
        (el_pt->tree_pt()->father_pt()!=0))
     {
      //Flag to indicate whether to unrefine
      bool unrefine=true;
      unsigned n_sons = el_pt->tree_pt()->father_pt()->nsons();
      // Are all brothers leaf nodes?
      for (unsigned ison=0;ison<n_sons;ison++)
       {
        // (At least) one brother is not a leaf: end of story; we're not doing
        // it (= the unrefinement)
        if (!(el_pt->tree_pt()->father_pt()->son_pt(ison)->is_leaf()))
         {unrefine=false;}
       }

      // Don't allow unrefinement of elements that would become larger
      // than the minimum legal refinement level
      if (el_pt->refinement_level()-1<min_refinement_level())
       {unrefine=false;}

      // So, all things considered, is the element eligbible for refinement?
      if(unrefine) {wants_to_be_unrefined[el_pt]=true;}
      else {wants_to_be_unrefined[el_pt]=false;}
     }
   }
  
  oomph_info 
   << " \n Number of elements to be refined: " << n_refine << std::endl;
  oomph_info << " \n Number of elements whose refinement was overruled: " 
            << nrefinement_overruled() << std::endl;

  //Second pass for unrefinement --- an element cannot be unrefined unless
  //all brothers want to be unrefined.
  // Loop over all elements again and let the first set of sons check if their
  // brothers also want to be unrefined
  unsigned n_unrefine=0;
  for (unsigned long e=0;e<Nelement;e++)
   {
    //(Cast) pointer to the element
    RefineableElement* el_pt = 
     dynamic_cast<RefineableElement*>(this->element_pt(e));
    
    if (el_pt->tree_pt()->son_type()==OcTreeNames::LDB)
     {
      // Do all sons want to be unrefined?
      bool unrefine=true;
      unsigned n_sons = el_pt->tree_pt()->father_pt()->nsons();
      for (unsigned ison=0;ison<n_sons;ison++)
       {
        if (!(wants_to_be_unrefined[
               dynamic_cast<RefineableElement*>
               (el_pt->tree_pt()->father_pt()->
                son_pt(ison)->object_pt())]))
         {
          // One guy isn't cooperating and spoils the party.
          unrefine=false;
         }
       }

      // Tell father that his sons need to be merged
      if(unrefine)
       {
        el_pt->tree_pt()->father_pt()->object_pt()->
         select_sons_for_unrefinement();
        n_unrefine += n_sons; // n_unrefine++;
       }
      //Otherwise mark the sons as not to be touched
      else
       {
        el_pt->tree_pt()->father_pt()->object_pt()->
         deselect_sons_for_unrefinement();
       }
     }
   }
  oomph_info << " \n Number of elements to be merged : " 
            << n_unrefine << std::endl << std::endl;



  //Now do the actual mesh adaptation
  //---------------------------------

  // Check whether its worth our while
  // Either some elements want to be refined, 
  // or the number that want to be unrefined are greater than the
  // specified tolerance

  // In a parallel job, it is possible that one process may not have
  // any elements to refine, BUT a neighbouring process may refine an
  // element which changes the hanging status of a node that is on
  // both processes (i.e. a halo(ed) node).  To get around this issue, 
  // ALL processes need to call adapt_mesh if ANY refinement is to 
  // take place anywhere.

  unsigned total_n_refine=0;
#ifdef OOMPH_HAS_MPI
  // Sum n_refine across all processors
  if (mesh_has_been_distributed())
   {
    MPI_Allreduce(&n_refine,&total_n_refine,1,MPI_INT,MPI_SUM,
                  MPI_COMM_WORLD);
   }
  else 
   {
    total_n_refine=n_refine;
   }
#else
  total_n_refine=n_refine;
#endif

  // There may be some issues with unrefinement too, but I have not
  // been able to come up with an example (either in my head or in a
  // particular problem) where anything has arisen.  I can see that
  // there may be an issue if n_unrefine differs across processes so
  // that (total_n_unrefine > max_keep_unrefined()) on some but not 
  // all processes. I haven't seen any examples of this yet so the 
  // following code may or may not work!  (Andy, 06/03/08)

  unsigned total_n_unrefine=0;
#ifdef OOMPH_HAS_MPI
  // Sum n_unrefine across all processors
  if (mesh_has_been_distributed())
   {
    MPI_Allreduce(&n_unrefine,&total_n_unrefine,1,MPI_INT,MPI_SUM,
                  MPI_COMM_WORLD);
   }
  else
   {
    total_n_unrefine=n_unrefine;
   }
#else
  total_n_unrefine=n_unrefine;
#endif

  oomph_info << "---> " << total_n_refine << " elements to be refined, and "
             << total_n_unrefine << " to be unrefined, in total." << std::endl;

  if ((total_n_refine > 0) || (total_n_unrefine > max_keep_unrefined()))
   {
    //Refine the mesh
    adapt_mesh(local_doc_info);
    //The number of refineable elements should still be local to each process
    nunrefined()=n_unrefine;
    nrefined()=n_refine;
   }
  //If not worthwhile, say so
  else
   {
    if (n_refine==0)
     {
      oomph_info 
       << "\n Not enough benefit in adapting mesh. " 
       << std::endl << std::endl;
     }
    nunrefined()=0;
    nrefined()=0;
   }

 }

//========================================================================
/// Get max/min refinement level
//========================================================================
void RefineableMeshBase:: 
get_refinement_levels(unsigned& min_refinement_level,
                      unsigned& max_refinement_level)
{ 
 // Initialise
 min_refinement_level=10000;
 max_refinement_level=0;
 
 //Loop over all elements
 unsigned long Nelement=this->nelement();
 for (unsigned long e=0;e<Nelement;e++)
  {
   //Get the refinement level of the element
   unsigned level =
    dynamic_cast<RefineableElement*>(this->element_pt(e))->refinement_level();
 
   if (level>max_refinement_level) max_refinement_level=level;
   if (level<min_refinement_level) min_refinement_level=level;
  }
}


//================================================================
/// Adapt mesh, which exists in two representations,
/// namely as:
///  - a FE mesh 
///  - a forest of Oc or QuadTrees 
///
/// Refinement/derefinement process is documented (in tecplot-able form)
/// if requested. 
///
/// Procedure:
/// - Loop over all elements and do the refinement for those who want to
///   be refined. Note: Refinement/splitting only allocates elements but 
///   doesn't build them.
/// - Build the new elements (i.e. give them nodes (create new ones where
///   necessary), assign boundary conditions, and add nodes to mesh
///   and mesh boundaries.
/// - For all nodes that were hanging on the previous mesh (and are still
///   marked as such), fill in their nodal values (consistent
///   with the current hanging node scheme) to make sure they are fully 
///   functional, should they have become non-hanging during the 
///   mesh-adaptation. Then mark the nodes as non-hanging.
/// - Unrefine selected elements (which may cause nodes to be re-built).
/// - Add the new elements to the mesh (by completely overwriting 
///   the old Vector of elements).
/// - Delete any nodes that have become obsolete.
/// - Mark up hanging nodes and setup hanging node scheme (incl.
///   recursive cleanup for hanging nodes that depend on other
///   hanging nodes).
/// - Adjust position of hanging nodes to make sure their position
///   is consistent with the FE-based represenetation of their larger 
///   neighbours.
/// - run a quick self-test on the neighbour finding scheme and
///   check the integrity of the elements (if PARANOID)
/// - doc hanging node status, boundary conditions, neighbour
///   scheme if requested.
///
///
/// After adaptation, all nodes (whether new or old) have up-to-date
/// current and previous values. 
///
/// If refinement process is being documented, the following information
/// is documented:
/// - The files
///   - "neighbours.dat" 
///   - "all_nodes.dat" 
///   - "new_nodes.dat" 
///   - "hang_nodes_*.dat" 
///     where the * denotes a direction (n,s,e,w) in 2D 
///     or (r,l,u,d,f,b) in 3D
///. 
///   can be viewed with 
///   - QHangingNodes.mcr
///   .
/// - The file
///    - "hangnodes_withmasters.dat"
///    .
///    can be viewed with 
///    - QHangingNodesWithMasters.mcr
///    .
///    to check the hanging node status.
/// - The neighbour status of the elements is documented in
///   - "neighbours.dat"
///   .
///   and can be viewed with 
///   - QuadTreeNeighbours.mcr
///   .
//=================================================================
void RefineableMeshBase::adapt_mesh(DocInfo& doc_info)
{
 // Pointer to mesh needs to be passed to some functions
 Mesh* mesh_pt=this;
 
 // Do refinement(=splitting) of elements that have been selected
 // This function encapsulates the template parameter
 this->split_elements_if_required();

 // Now elements have been created -- build all the leaves
 //-------------------------------------------------------
 //Firstly put all the elements into a vector
 Vector<Tree*> leaf_nodes_pt;
 Forest_pt->stick_leaves_into_vector(leaf_nodes_pt);

 //If we are documenting the output, create the filename
 char fullname[100];
 std::ofstream new_nodes_file;
 if(doc_info.doc_flag())
  {
   sprintf(fullname,"%s/new_nodes%i.dat",doc_info.directory().c_str(),
           doc_info.number());
   new_nodes_file.open(fullname);
  }
 
 // Build all elements and store vector of pointers to new nodes
 // (Note: build() checks if the element has been built 
 // already, i.e. if it's not a new element).
 Vector<Node*> new_node_pt;
 bool was_already_built;
 unsigned long num_tree_nodes=leaf_nodes_pt.size();
 for (unsigned long e=0;e<num_tree_nodes;e++)
  {
   leaf_nodes_pt[e]->object_pt()
    ->build(mesh_pt,new_node_pt,was_already_built,new_nodes_file);
  }

 //Close the new nodes files, if it was opened
 if(doc_info.doc_flag()) {new_nodes_file.close();}

 // Loop over all nodes in mesh and free the dofs of those that were
 //-----------------------------------------------------------------
 // pinned only because they were hanging nodes. Also update their
 //-----------------------------------------------------------------
 // nodal values so that they contain data that is consistent
 //----------------------------------------------------------
 // with the hanging node representation
 //-------------------------------------
 // (Even if the nodal data isn't actually accessed because the node 
 // is still hanging -- we don't know this yet, and this step makes
 // sure that all nodes are fully functional and up-to-date, should
 // they become non-hanging below).
 unsigned long n_node=this->nnode();
 for (unsigned long n=0;n<n_node;n++)
  {
   //Get the pointer to the node
   Node* nod_pt=this->node_pt(n);
  
   //Get the number of values in the node
   unsigned n_value=nod_pt->nvalue();
   
   //We need to find if any of the values are hanging
   bool is_hanging = nod_pt->is_hanging();
   //Loop over the values and find out whether any are hanging
   for(unsigned n=0;n<n_value;n++)
    {is_hanging |= nod_pt->is_hanging(n);}

   //If the node is hanging then ...
   if(is_hanging)
    {
     // Unless they are turned into hanging nodes again below
     // (this might or might not happen), fill in all the necessary
     // data to make them 'proper' nodes again.
     
     // Reconstruct the nodal values/position from the node's 
     // hanging node representation
     unsigned nt=nod_pt->ntstorage();
     Vector<double> values(n_value);
     unsigned n_dim=nod_pt->ndim();
     Vector<double> position(n_dim);
     // Loop over all history values
     for(unsigned t=0;t<nt;t++)
      {
       nod_pt->value(t,values);
       for(unsigned i=0;i<n_value;i++) {nod_pt->set_value(t,i,values[i]);}
       nod_pt->position(t,position);
       for(unsigned i=0;i<n_dim;i++) {nod_pt->x(t,i)=position[i];}
      }

     // If it's an algebraic node: Update its previous nodal positions too
     AlgebraicNode* alg_node_pt=dynamic_cast<AlgebraicNode*>(nod_pt);
     if (alg_node_pt!=0)
      {
       bool update_all_time_levels=true;
       alg_node_pt->node_update(update_all_time_levels);
      }
     
     
     //If it's a Solid node, update Lagrangian coordinates
     // from its hanging node representation
     SolidNode* solid_node_pt = dynamic_cast<SolidNode*>(nod_pt);
     if(solid_node_pt!=0)
      {
       unsigned n_lagrangian = solid_node_pt->nlagrangian();
       for(unsigned i=0;i<n_lagrangian;i++)
        {
         solid_node_pt->xi(i) = solid_node_pt->lagrangian_position(i);
        }
      }
    }

   // Initially mark all nodes as 'non-hanging' and `obsolete' 
   nod_pt->set_nonhanging();
   nod_pt->set_obsolete();
  }

 // Unrefine all the selected elements: This needs to be
 // all elements, because the father elements are not actually leaves.
 //--------------------------------
 for (unsigned long e=0;e<Forest_pt->ntree();e++)
  {Forest_pt->tree_pt(e)->traverse_all(&Tree::merge_sons_if_required,
                                       mesh_pt);}

 // Add the newly created elements to mesh
 //---------------------------------------

 // Stick all elements into a new vector
 //(note the leaves may have changed, so this is not duplicated work)
 Vector<Tree*> tree_nodes_pt;
 Forest_pt->stick_leaves_into_vector(tree_nodes_pt);

 //Copy the elements into the mesh Vector
 num_tree_nodes=tree_nodes_pt.size();
 Element_pt.resize(num_tree_nodes);
 for (unsigned long e=0;e<num_tree_nodes;e++)
  {
   Element_pt[e]=tree_nodes_pt[e]->object_pt();

   // Now loop over all nodes in element and mark them as non-obsolete
   // Logic: Initially all nodes in the unrefined mesh were labeled
   // as deleteable. Then we create new elements (whose newly created
   // nodes are obviously non-obsolete), and killed some other elements (by
   // by deleting them and marking the nodes that were not shared by
   // their father as obsolete. Now we loop over all the remaining
   // elements and (re-)label all their nodes as non-obsolete. This
   // saves some nodes that were regarded as obsolete by deleted
   // elements but are still required in some surviving ones
   // from a tragic early death...
   FiniteElement* this_el_pt=this->finite_element_pt(e); // andy 
   unsigned n_node=this_el_pt->nnode(); // andy caching pre-loop
   for (unsigned n=0;n<n_node;n++)
    {
     this_el_pt->node_pt(n)->set_non_obsolete(); // andy
    }
  }

 // Cannot delete nodes that are still marked as obsolete
 // because they may still be required to assemble the hanging schemes
 //-------------------------------------------------------------------

 // Mark up hanging nodes
 //----------------------
 
 //Output streams for the hanging nodes
 Vector<std::ofstream*> hanging_output_files;
 //Setup the output files for hanging nodes, this must be called
 //precisely once for the forest. Note that the files will only
 //actually be opened if doc_info.doc_flag() is true
 Forest_pt->open_hanging_node_files(doc_info,hanging_output_files);

 for(unsigned long e=0;e<num_tree_nodes;e++)
  {
   //Generic setup
   tree_nodes_pt[e]->object_pt()->setup_hanging_nodes(hanging_output_files);
   //Element specific setup
   tree_nodes_pt[e]->object_pt()->further_setup_hanging_nodes();
  }

 //Close the hanging node files and delete the memory allocated 
 //for the streams
 Forest_pt->close_hanging_node_files(doc_info,hanging_output_files);

 // Read out the number of continously interpolated values
 // from one of the elements (assuming it's the same in all elements)
 unsigned ncont_interpolated_values=
  tree_nodes_pt[0]->object_pt()->ncont_interpolated_values();

 // Complete the hanging nodes schemes by dealing with the
 // recursively hanging nodes
 complete_hanging_nodes(ncont_interpolated_values);

  /// Update the boundary element info -- this can be a costly procedure
 /// and for this reason the mesh writer might have decided not to set up this
 /// scheme. If so, we won't change this and suppress its creation...
 if (Lookup_for_elements_next_boundary_is_setup)
  {
   this->setup_boundary_element_info(); 
  }

#ifdef PARANOID
 
 // Doc/check the neighbours
 //-------------------------
 Vector<Tree*> all_tree_nodes_pt;
 Forest_pt->stick_all_tree_nodes_into_vector(all_tree_nodes_pt);

 //Check the neighbours
 Forest_pt->check_all_neighbours(doc_info);
 
 // Check the integrity of the elements
 // -----------------------------------
 
 // Loop over elements and get the elemental integrity
 double max_error=0.0;
 for (unsigned long e=0;e<num_tree_nodes;e++)
  {
   double max_el_error;
   tree_nodes_pt[e]->object_pt()->check_integrity(max_el_error);
   //If the elemental error is greater than our maximum error
   //reset the maximum
   if(max_el_error > max_error) {max_error=max_el_error;}
  }

 if (max_error>RefineableElement::max_integrity_tolerance())
  {
   std::ostringstream error_stream;
   error_stream << "Mesh refined: Max. error in integrity check: " 
                << max_error << " is too big\n";
   error_stream
    << "i.e. bigger than RefineableElement::max_integrity_tolerance()="
    << RefineableElement::max_integrity_tolerance() << std::endl;

   std::ofstream some_file;
   some_file.open("ProblemMesh.dat");
   for (unsigned long n=0;n<n_node;n++)
    {
     //Get the pointer to the node
     Node* nod_pt = this->node_pt(n);
     //Get the dimension
     unsigned n_dim = nod_pt->ndim();
     //Output the coordinates
     for(unsigned i=0;i<n_dim;i++)
      {
       some_file << this->node_pt(n)->x(i) << " ";
      }
     some_file << std::endl;
    }
   some_file.close();

   error_stream << "Doced problem mesh in ProblemMesh.dat" << std::endl;

   throw OomphLibError(error_stream.str(),
                       "RefineableMeshBase::adapt_mesh()",
                       OOMPH_EXCEPTION_LOCATION);
  }
 else
  {
   oomph_info << "Mesh refined: Max. error in integrity check: " 
        << max_error << " is OK" << std::endl;
   oomph_info << "i.e. less than RefineableElement::max_integrity_tolerance()="
        << RefineableElement::max_integrity_tolerance() << std::endl;
  }
 
#endif

 //Loop over all elements other than the final level and deactivate the
 //objects, essentially set the pointer that point to nodes that are
 //about to be deleted to NULL. This must take place here because nodes
 //addressed by elements that are dead but still living in the tree might
 //have been made obsolete in the last round of refinement
 for (unsigned long e=0;e<Forest_pt->ntree();e++)
  {
   Forest_pt->tree_pt(e)->
    traverse_all_but_leaves(&Tree::deactivate_object);
  }
 
 //Now we can prune the dead nodes from the mesh.
 this->prune_dead_nodes();

 // Finally: Reorder the nodes within the mesh's node vector
 // to establish a standard ordering regardless of the sequence
 // of mesh refinements -- this is required to allow dump/restart
 // on refined meshes
 this->reorder_nodes();
  
 // Final doc
 //-----------
 if (doc_info.doc_flag())
  {
   // Doc the boundary conditions ('0' for non-existent, '1' for free, 
   //----------------------------------------------------------------
   // '2' for pinned -- ideal for tecplot scatter sizing.
   //----------------------------------------------------
   //num_tree_nodes=tree_nodes_pt.size();

   // Determine maximum number of values at any node in this type of element
   RefineableElement* el_pt = tree_nodes_pt[0]->object_pt();
   //Initalise max_nval
   unsigned max_nval=0;
   for (unsigned n=0;n<el_pt->nnode();n++)
    {
     if (el_pt->node_pt(n)->nvalue()>max_nval)
      {max_nval=el_pt->node_pt(n)->nvalue();}
    }

   //Open the output file
   std::ofstream bcs_file;
   sprintf(fullname,"%s/bcs%i.dat",doc_info.directory().c_str(),
           doc_info.number());
   bcs_file.open(fullname);  
   
   // Loop over elements
   for(unsigned long e=0;e<num_tree_nodes;e++)
    {
     el_pt = tree_nodes_pt[e]->object_pt();
     // Loop over nodes in element
     unsigned n_nod=el_pt->nnode();
     for(unsigned n=0;n<n_nod;n++)
      {
       //Get pointer to the node
       Node* nod_pt=el_pt->node_pt(n);
       //Find the dimension of the node
       unsigned n_dim = nod_pt->ndim();
       //Write the nodal coordinates to the file
       for(unsigned i=0;i<n_dim;i++)
        {bcs_file << nod_pt->x(i) << " ";}
       
       // Loop over all values in this element
       for(unsigned i=0;i<max_nval;i++)
        {
         // Value exists at this node:
         if (i<nod_pt->nvalue())
          {
           bcs_file << " " << 1+nod_pt->is_pinned(i);
          }
         // ...if not just dump out a zero
         else
          {
           bcs_file << " 0 ";
          }
        }
       bcs_file << std::endl;
      }  
    }
   bcs_file.close();
   
   // Doc all nodes
   //---------------
   std::ofstream all_nodes_file;
   sprintf(fullname,"%s/all_nodes%i.dat",doc_info.directory().c_str(),
           doc_info.number());
   all_nodes_file.open(fullname);  
   
   all_nodes_file << "ZONE \n"; 
   for(unsigned long n=0;n<n_node;n++)
    {
     Node* nod_pt = this->node_pt(n);
     unsigned n_dim = nod_pt->ndim();
     for(unsigned i=0;i<n_dim;i++)
      {
       all_nodes_file << this->node_pt(n)->x(i) << " ";
      }
     all_nodes_file << std::endl;
    }
   
   all_nodes_file.close();


   // Doc all hanging nodes:
   //-----------------------
   std::ofstream some_file;
   sprintf(fullname,"%s/all_hangnodes%i.dat",doc_info.directory().c_str(),
           doc_info.number());
   some_file.open(fullname);
   for(unsigned long n=0;n<n_node;n++)
    {
     Node* nod_pt=this->node_pt(n);

     if (nod_pt->is_hanging())
      {
       unsigned n_dim = nod_pt->ndim();       
       for(unsigned i=0;i<n_dim;i++)
        {
         some_file << nod_pt->x(i) << " ";
         }
       
       //ALH: Added this to stop Solid problems seg-faulting
       if(this->node_pt(n)->nvalue() > 0)
        {
         some_file  << " " << nod_pt->raw_value(0);
        }
       some_file << std::endl;
      }
    }
   some_file.close();

   // Doc all hanging nodes and their masters 
   // View with QHangingNodesWithMasters.mcr
   sprintf(fullname,"%s/geometric_hangnodes_withmasters%i.dat",
           doc_info.directory().c_str(),doc_info.number());
   some_file.open(fullname);
   for(unsigned long n=0;n<n_node;n++)
    {
     Node* nod_pt=this->node_pt(n);
     if (nod_pt->is_hanging())
      {
       unsigned n_dim = nod_pt->ndim();
       unsigned nmaster=nod_pt->hanging_pt()->nmaster();
       some_file << "ZONE I="<<nmaster+1 << std::endl;
       for(unsigned i=0;i<n_dim;i++)
        {
         some_file << nod_pt->x(i) << " ";
         }
       some_file << " 2 " <<  std::endl;
       
       for (unsigned imaster=0;imaster<nmaster;imaster++)
        {
         Node* master_nod_pt = 
          nod_pt->hanging_pt()->master_node_pt(imaster);
         unsigned n_dim = master_nod_pt->ndim();
         for(unsigned i=0;i<n_dim;i++)
          {
           some_file << master_nod_pt->x(i) << " ";
          }
         some_file << " 1 " << std::endl;
        }
      }
    }
   some_file.close();
   
   // Doc all hanging nodes and their masters 
   // View with QHangingNodesWithMasters.mcr
   for(unsigned i=0;i<ncont_interpolated_values;i++)
    {
     sprintf(fullname,"%s/nonstandard_hangnodes_withmasters%i_%i.dat",
             doc_info.directory().c_str(),i,doc_info.number());
     some_file.open(fullname);
     unsigned n_nod=this->nnode();
     for(unsigned long n=0;n<n_nod;n++)
      {
       Node* nod_pt=this->node_pt(n);
       if (nod_pt->is_hanging(i))
        {
         if (nod_pt->hanging_pt(i)!=nod_pt->hanging_pt())
          {
           unsigned nmaster=nod_pt->hanging_pt(i)->nmaster();
           some_file << "ZONE I="<<nmaster+1 << std::endl;
           unsigned n_dim = nod_pt->ndim();
           for(unsigned j=0;j<n_dim;j++)
            {
             some_file << nod_pt->x(j) << " ";
             }
           some_file << " 2 " << std::endl;
           for (unsigned imaster=0;imaster<nmaster;imaster++)
            {
             Node* master_nod_pt = 
              nod_pt->hanging_pt(i)->master_node_pt(imaster);
             unsigned n_dim = master_nod_pt->ndim();
             for(unsigned j=0;j<n_dim;j++)
              {
//               some_file << master_nod_pt->x(i) << " ";
              }
             some_file << " 1 " << std::endl;
            }
          }
        }
      }
     some_file.close();
    }
   
  } //End of documentation

}


//========================================================================
/// Refine mesh uniformly
//========================================================================
void RefineableMeshBase::refine_uniformly(DocInfo& doc_info)
{ 
 //Select all elements for refinement
 unsigned long Nelement=this->nelement();
 for (unsigned long e=0;e<Nelement;e++)
  {
   dynamic_cast<RefineableElement*>
    (this->element_pt(e))->select_for_refinement();
  }
 
 // Do the actual mesh adaptation
 adapt_mesh(doc_info); 
 }



//========================================================================
/// Refine mesh by splitting the elements identified
/// by their numbers.
//========================================================================
void RefineableMeshBase::refine_selected_elements(
 const Vector<unsigned>& elements_to_be_refined)
{ 
 //Select elements for refinement
 unsigned long nref=elements_to_be_refined.size();
 for (unsigned long e=0;e<nref;e++)
  {
   dynamic_cast<RefineableElement*>
    (this->element_pt(elements_to_be_refined[e]))->select_for_refinement();
  }
 
 // Do the actual mesh adaptation
 adapt_mesh(); 
 }



//========================================================================
/// Refine mesh by splitting the elements identified
/// by their pointers
//========================================================================
void RefineableMeshBase::refine_selected_elements(
 const Vector<RefineableElement*>& elements_to_be_refined_pt)
{ 
 //Select elements for refinement
 unsigned long nref=elements_to_be_refined_pt.size();
 for (unsigned long e=0;e<nref;e++)
  {
   elements_to_be_refined_pt[e]->select_for_refinement();
  }
 
 // Do the actual mesh adaptation
 adapt_mesh(); 
 }



//========================================================================
/// Refine to same degree as the reference mesh.
///
//========================================================================
void RefineableMeshBase::refine_base_mesh_as_in_reference_mesh(
 RefineableMeshBase* const &ref_mesh_pt)
{
 // Assign storage for refinement pattern
 Vector<Vector<unsigned> > to_be_refined;

 // Get refinement pattern of reference mesh:
 ref_mesh_pt->get_refinement_pattern(to_be_refined);

 // Refine mesh according to given refinement pattern
 refine_base_mesh(to_be_refined);
}

//========================================================================
/// Refine mesh once so that its topology etc becomes that of the 
/// (finer!) reference mesh -- if possible! Useful for meshes in multigrid 
/// hierarchies. If the meshes are too different and the conversion
/// cannot be performed, the code dies (provided PARANOID is enabled).
//========================================================================
void RefineableMeshBase::refine_as_in_reference_mesh(
 RefineableMeshBase* const &ref_mesh_pt)
{
 oomph_info << "WARNING : This haas not been checked comprehensively yet"
           << std::endl
           << "Check it and remove this break " << std::endl;
 pause("Yes really pause");

#ifdef PARANOID
 // The max. refinement levels of the two meshes need to differ 
 // by one, otherwise what we're doing here doesn't make sense.
 unsigned my_min,my_max;
 get_refinement_levels(my_min,my_max);

 unsigned ref_min,ref_max;
 ref_mesh_pt->get_refinement_levels(ref_min,ref_max);

 if (ref_max!=my_max+1)
  {
   std::ostringstream error_stream;
   error_stream
    << "Meshes definitely don't differ by one refinement level \n"
    << "max. refinement levels: "<< ref_max << " " << my_max << std::endl;

   throw OomphLibError(error_stream.str(),
                       "RefineableMeshBase::refine_as_in_reference_mesh()",
                       OOMPH_EXCEPTION_LOCATION);
  }
#endif

 // Vector storing the elements of the uniformly unrefined mesh
 Vector<Tree*> coarse_elements_pt;

 // Map storing which father elements have already been added to coarse mesh
 // (Default return is 0).
 std::map<Tree*,unsigned> father_element_included;

 // Extract active elements (=leaf nodes in the quadtree) from reference mesh.
 Vector<Tree*> leaf_nodes_pt;
 ref_mesh_pt->forest_pt()->
  stick_leaves_into_vector(leaf_nodes_pt);

 // Loop over all elements (in their quadtree impersonation) and
 // check if their fathers's sons are all leaves too:
 unsigned nelem=leaf_nodes_pt.size();
 for (unsigned e=0;e<nelem;e++)
  {
   //Pointer to leaf node
   Tree* leaf_pt=leaf_nodes_pt[e];

   // Get pointer to father:
   Tree* father_pt=leaf_pt->father_pt();
   
   // If we don't have a father we're at the root level in which
   // case this element can't be unrefined.
   if (0==father_pt)
    {
     coarse_elements_pt.push_back(leaf_pt);
    }
   else
    {
     // Loop over the father's sons to check if they're
     // all non-leafs, i.e. if they can be unrefined
     bool can_unrefine=true;
     unsigned n_sons = father_pt->nsons();
     for (unsigned i=0;i<n_sons;i++)
      {
       // If (at least) one of the sons is not a leaf, we can't unrefine
       if (!father_pt->son_pt(i)->is_leaf()) can_unrefine=false;
      }

     // If we can unrefine, the father element will be 
     // an element in the coarse mesh, the sons won't
     if (can_unrefine)
      {
       if (father_element_included[father_pt]==0)
        {
         coarse_elements_pt.push_back(father_pt);
         father_element_included[father_pt]=1;
        }
      }
     // Son will still be there on the coarse mesh
     else
      {
       coarse_elements_pt.push_back(leaf_pt);
      }
    }
  }

 // Number of elements in ref mesh if it was unrefined uniformly:
 unsigned nel_coarse=coarse_elements_pt.size();


#ifdef PARANOID
 bool stop_it=false;
 // The numbers had better match otherwise we might as well stop now...
 if (nel_coarse!=this->nelement())
  {
   oomph_info << "Number of elements in uniformly unrefined reference mesh: " 
        << nel_coarse<< std::endl;
   oomph_info << "Number of elements in 'this' mesh: " 
        << nel_coarse<< std::endl;
   oomph_info << "don't match" << std::endl;
   stop_it=true;
  }
#endif

 // Now loop over all elements in uniformly coarsened reference mesh
 // and check if add the number of any element that was created
 // by having had its sons merged to the vector of elements that
 // need to get refined if we go the other way
 Vector<unsigned> elements_to_be_refined;
 for (unsigned i=0;i<nel_coarse;i++)
  {
   if (father_element_included[coarse_elements_pt[i]]==1)
    {
     elements_to_be_refined.push_back(i);
    }
  }

 
#ifdef PARANOID
  // Doc troublesome meshes:
 if (stop_it)
  {
   std::ofstream some_file;
   some_file.open("orig_mesh.dat");
   this->output(some_file);
   some_file.close();
   oomph_info << "Documented original ('this')mesh in orig_mesh.dat" 
             << std::endl;
  }
#endif


 // Now refine precisely these elements in "this" mesh.
 refine_selected_elements(elements_to_be_refined);


#ifdef PARANOID

   // Check if the nodal positions of all element's nodes agree
   // in the two fine meshes:
   double tol=1.0e-5;
   for (unsigned e=0;e<nelem;e++)
    {
     // Get elements
     FiniteElement* ref_el_pt=ref_mesh_pt->finite_element_pt(e);
     FiniteElement* el_pt=this->finite_element_pt(e);

     // Loop over nodes
     unsigned nnod=ref_el_pt->nnode();
     for (unsigned j=0;j<nnod;j++)
      {
       // Get nodes
       Node* ref_node_pt=ref_el_pt->node_pt(j);
       Node* node_pt=el_pt->node_pt(j);

       // Check error in position
       double error=0.0;
       unsigned ndim=node_pt->ndim();
       for (unsigned i=0;i<ndim;i++)
        {
         error+=pow(node_pt->x(i)-ref_node_pt->x(i),2);
        }
       error=sqrt(error);

       if (error>tol)
        {
         oomph_info << "Error in nodal position of node " << j << ": " 
              << error << "           [tol=" << tol<< "]" << std::endl; 
         stop_it=true;
        }
      }
    }

   // Do we have a death wish?
   if (stop_it)
    {
     // Doc troublesome meshes:
     std::ofstream some_file;
     some_file.open("refined_mesh.dat");
     this->output(some_file);
     some_file.close();
     
     some_file.open("finer_mesh.dat");
     ref_mesh_pt->output(some_file);
     some_file.close();
 
     throw OomphLibError(
      "Bailing out. Doced refined_mesh.dat finer_mesh.dat\n",
      "RefineableMeshBase::refine_as_in_reference_mesh()",
      OOMPH_EXCEPTION_LOCATION);
    }

#endif
   
}


//========================================================================
/// Unrefine mesh uniformly. Return 0 for success,
/// 1 for failure (if unrefinement has reached the coarsest permitted
/// level)
//========================================================================
unsigned RefineableMeshBase::unrefine_uniformly()
{ 

 // We can't just select all elements for unrefinement
 // because they need to merge with their brothers.
 // --> Rather than repeating the convoluted logic of
 // RefineableQuadMesh<ELEMENT>::adapt(Vector<double>& elemental_error) 
 // here (code duplication!) hack it by filling the error
 // vector with values that ensure unrefinement for all
 // elements where this is possible

 // Create dummy vector for elemental errors
 unsigned long Nelement=this->nelement();
 Vector<double> elemental_error(Nelement);

 // Set it error to 1/100 of the min. error to force unrefinement
 double error=min_permitted_error()/100.0;
 for (unsigned long e=0;e<Nelement;e++)
  {
   elemental_error[e]=error;
  }

 // Temporarily lift any restrictions on the minimum number of
 // elements that need to be unrefined to make it worthwhile
 unsigned backup=max_keep_unrefined();
 max_keep_unrefined()=0;

  // Do the actual mesh adaptation with fake error vector
 adapt(elemental_error); 

 // Reset the minimum number of elements that need to be unrefined 
 // to make it worthwhile
 max_keep_unrefined()=backup;

 // Has the unrefinement actually changed anything?
 if (Nelement==this->nelement())
  {
   return 1;
  }
 else
  {
   return 0;
  }

}

//==================================================================
/// Given a node, return a vector of pointers to master nodes and a
/// vector of the associated weights.
/// This is done recursively, so if a node is not hanging, 
/// the node is regarded as its own master node which has weight 1.0.
//==================================================================
void RefineableMeshBase::
complete_hanging_nodes_recursively(Node*& nod_pt,
                                   Vector<Node*>& master_nodes,
                                   Vector<double>& hang_weights,
                                   const int& i)
{
 // Is the node hanging in the variable i
 if(nod_pt->is_hanging(i))
  {
   // Loop over all master nodes
   HangInfo* const hang_pt = nod_pt->hanging_pt(i); // andy
   unsigned nmaster=hang_pt->nmaster();

   for(unsigned m=0;m<nmaster;m++)
    {
     // Get the master node
     Node* master_nod_pt=hang_pt->master_node_pt(m);

     // Keep in memory the size of the list before adding the nodes this 
     // master node depends on. This is required so that the recursion is
     // only performed on these particular master nodes. A master node
     // could contain contributions from two separate pseudo-masters.
     // These contributions must be summed, not multiplied. 
     int first_new_node=master_nodes.size();

     // Now check which master nodes this master node depends on
     complete_hanging_nodes_recursively(master_nod_pt,
                                        master_nodes,hang_weights,i);
     
     // Multiply old weight by new weight for all the nodes this master 
     // node depends on
     unsigned n_new_master_node = master_nodes.size();

     double mtr_weight=hang_pt->master_weight(m); // andy

     for(unsigned k=first_new_node;k<n_new_master_node;k++)
      {
       hang_weights[k]=mtr_weight*hang_weights[k];
      }
    }
  }
 else
  // Node isn't hanging so it enters itself with the full weight
  {
   master_nodes.push_back(nod_pt);
   hang_weights.push_back(1.0);
  }
 
}


//==================================================================
/// Complete the hanging node scheme recursively. 
/// After the initial markup scheme, hanging nodes
/// can depend on other hanging nodes ---> AAAAAAAAARGH!
/// Need to translate this into a scheme where all
/// hanging  nodes only depend on non-hanging nodes...
//==================================================================
void RefineableMeshBase::
complete_hanging_nodes(const int& ncont_interpolated_values)
 {
  //Number of nodes in mesh
  unsigned long n_node=this->nnode();
  double min_weight=1.0e-8; //RefineableBrickElement::min_weight_value();

  //Loop over the nodes in the mesh
  for (unsigned long n=0;n<n_node;n++)
   {
    //Assign a local pointer to the node
    Node* nod_pt=this->node_pt(n);

    //Loop over the values,
    //N.B. geometric hanging data is stored at the index -1
    for(int i=-1;i<ncont_interpolated_values;i++)
     {
      // Is the node hanging?
      if (nod_pt->is_hanging(i))
       {
        //If it is geometric OR has hanging node data that differs
        //from the geometric data, we must do some work
        if((i==-1) || (nod_pt->hanging_pt(i)!=nod_pt->hanging_pt()))
         {
          // Find out the ultimate map of dependencies: Master nodes
          // and associated weights
          Vector<Node*> master_nodes;
          Vector<double> hanging_weights;
          complete_hanging_nodes_recursively(nod_pt,
                                             master_nodes,hanging_weights,i);
          
          // put them into a map to merge all the occurences of the same node 
          // (add the weights)
          std::map<Node*,double> hang_weights;   
          unsigned n_master=master_nodes.size();
          for(unsigned k=0;k<n_master;k++)
           {
            if(std::abs(hanging_weights[k])>min_weight)
             hang_weights[master_nodes[k]]+=hanging_weights[k];
           }
          
          //Create new hanging data (we know how many data there are)
          HangInfo* hang_pt = new HangInfo(hang_weights.size());
          
          unsigned hang_weights_index=0;
          //Copy the map into the HangInfo object
          typedef std::map<Node*,double>::iterator IT;
          for (IT it=hang_weights.begin();it!=hang_weights.end();++it)
           {
            hang_pt->set_master_node_pt(hang_weights_index,it->first,
                                        it->second);
            ++hang_weights_index;
           }
          
          //Assign the new hanging pointer to the appropriate value
          nod_pt->set_hanging_pt(hang_pt,i);
         }
       }
     }
   }

#ifdef PARANOID

  // Check hanging node scheme: The weights need to add up to one
  //-------------------------------------------------------------
  //Loop over all values indices
  for (int i=-1;i<ncont_interpolated_values;i++)
   {
    //Loop over all nodes in mesh
    for (unsigned long n=0;n<n_node;n++)
     {
      //Set a local pointer to the node
      Node* nod_pt=this->node_pt(n);
      
      // Is it hanging?
      if (nod_pt->is_hanging(i))
       {
        unsigned nmaster= nod_pt->hanging_pt(i)->nmaster();
        double sum=0.0;
        for (unsigned imaster=0;imaster<nmaster;imaster++)
         {
          sum+=nod_pt->hanging_pt(i)->master_weight(imaster);
         }
        if (std::abs(sum-1.0)>1.0e-7)
         {
          oomph_info << "WARNING: Sum of master node weights abs(sum-1.0) " 
               << std::abs(sum-1.0) << " for node number " << n 
               << " at value " << i << std::endl;
         }
       }
     }
   }
#endif
  
 }

// Sorting out the cases of nodes that are hanging on at least one but not
// all of the processors for which they are part of the local mesh.

#ifdef OOMPH_HAS_MPI

//========================================================================
/// Deal with nodes that are hanging on one process but not another
/// (i.e. the hanging status of the haloed and halo layers disagrees)
//========================================================================
void RefineableMeshBase::synchronise_hanging_nodes
                  (const unsigned& ncont_interpolated_values)
 {
  MPI_Status status;
  int ncont_inter_values=ncont_interpolated_values;

  Vector<Vector<int> > vector_haloed_hanging(MPI_Helpers::Nproc);
  Vector<Vector<int> > vector_halo_hanging(MPI_Helpers::Nproc);

  // Loop over the hanging status for each interpolated variable
  for (int icont=-1; icont<ncont_inter_values; icont++)
   { 
     // geometric hanging data is stored at -1
     // is it possible to ignore i>=0 if its hanging data is the same
     // as the geometric case? I'm not sure it is in this case - it
     // makes sense in complete_hanging_nodes(...) above but not here
     // From nodes.h, once it's done for icont=-1, all other hanging
     // data that is the same as the geometric case is also done

  for (int d=0; d<MPI_Helpers::Nproc; d++)
  {
   if (d!=MPI_Helpers::My_rank) // no halo with yourself!
    {
     // store the hanging state of each haloed node
     unsigned nhd_nod=nhaloed_node(d);
     Vector<int> nhaloed_hanging(nhd_nod);
     for (unsigned j=0; j<nhd_nod; j++)
      {
       // Need to receive hanging information from appropriate process,
       // but MPI cannot send bools, so convert to integers first
       if (haloed_node_pt(d,j)->is_hanging(icont))
        {
         nhaloed_hanging[j]=1;
        }
       else
        {
         nhaloed_hanging[j]=0;
        }
      }
     // Receive the hanging state of the equivalent halo nodes
     Vector<int> nhalo_hanging(nhd_nod);     
     MPI_Recv(&nhalo_hanging[0],nhd_nod,MPI_INT,d,0,MPI_COMM_WORLD,&status);

     // Store these vectors in a vector of length MPI_Helpers::Nproc
     vector_haloed_hanging[d]=nhaloed_hanging;
     vector_halo_hanging[d]=nhalo_hanging;
    }
   else // d==MPI_Helpers::My_rank
    {
     // send halo hanging status to relevant process
     for (int dd=0; dd<MPI_Helpers::Nproc; dd++)
      {
       if (dd!=d) // no halo with yourself
        {
         unsigned nh_nod=nhalo_node(dd);
         Vector<int> nhalo_hanging(nh_nod);
         for (unsigned j=0; j<nh_nod; j++)
          {
           // Convert bools into integers
           if (halo_node_pt(dd,j)->is_hanging(icont))
            {
             nhalo_hanging[j]=1; // can't send bools via MPI
            }
           else
            {
             nhalo_hanging[j]=0;
            }
          }
         // send this to the relevant process
         MPI_Send(&nhalo_hanging[0],nh_nod,MPI_INT,dd,0,MPI_COMM_WORLD);
        }
      }
    }
  }


// Next, compare equivalent halo and haloed vectors to find discrepancies.
// It is possible that a node may not be on either process; to work round
// this, a new concept of a "shared node" between two processes has been
// introduced, which stores all nodes that are on each process in the same
// order on each process

  for (int d=0; d<MPI_Helpers::Nproc; d++)
   {
    if (d!=MPI_Helpers::My_rank) // no halo with yourself
     {
      Vector<int> haloed_hanging=vector_haloed_hanging[d];
      Vector<int> halo_hanging=vector_halo_hanging[d];
//      unsigned nhaloed=haloed_hanging.size();
//      unsigned nhalo=halo_hanging.size();
      unsigned nnod_haloed=nhaloed_node(d);
      unsigned nnod_shared=nshared_node(d);
      Vector<int> hanging_nodes(nnod_haloed);
      unsigned count_masters=0; // count for hanging_masters size
      Vector<int> hanging_masters;
      unsigned count_weights=0; // count for hanging_master_weights size
      Vector<double> hanging_master_weights;
//      oomph_info << "With process " << d << std::endl 
//                 << ", haloed check: " << nnod_haloed << std::endl
//                 << "-- Shared nodes: " << nnod_shared << std::endl;

      for (unsigned j=0; j<nnod_haloed; j++)
       {
        // Compare hanging status of halo/haloed counterpart structure
        if ((haloed_hanging[j]==1) && (halo_hanging[j]==0))
         {
          // Something needs to be done about the hanging status of the halo
          // if it is geometric OR has hanging data that
          // is different from the geometric data

          // Find master nodes of haloed node
          HangInfo* hang_pt=haloed_node_pt(d,j)->hanging_pt(icont);
          unsigned nhd_master=hang_pt->nmaster();

          // Create a vector of size nhaloed with nhd_master in
          // entries where there's a discrepancy, zero otherwise (see later)
          hanging_nodes[j]=nhd_master;

          unsigned master_haloed=0;

          for (unsigned m=0; m<nhd_master; m++)
           {
            // Get mth master node
            Node* nod_pt=hang_pt->master_node_pt(m);

              // This node will be shared: find it!
              for (unsigned k=0; k<nnod_shared; k++)
               {
                if (nod_pt==shared_node_pt(d,k))
                 {
                  // found a master: increment counter
                  master_haloed++;
                  // put its number into a vector
                  hanging_masters.push_back(k);
                  // increase count for vector size
                  count_masters++;
                  // put the weight into another vector
                  hanging_master_weights.push_back(hang_pt->master_weight(m));
                  // increase count for this vector size
                  count_weights++;
                 }
               }

           } // loop over master nodes

          if (master_haloed!=nhd_master)
           {
            oomph_info << "----- WARNING, only found " << master_haloed 
                       << " nodes out of " << nhd_master 
                       << " master nodes in synchronise_hanging_nodes! -----"
                       << std::endl;
           }

         }
        else if ((haloed_hanging[j]==0) && (halo_hanging[j]==1))
         {
          // The halo node should not be hanging in this instance
          // as far as I can tell from the instances I have seen so far
          // Can't access this directly - it has to be "sent" to the correct
          // process - use a negative number to denote this
          hanging_nodes[j]=-1;
          hanging_masters.push_back(-1);
  
          // This needs to be counted as a "master"
          count_masters++; 
         }
        else if (haloed_hanging[j]==halo_hanging[j]) 
         {
          // No discrepancy
          hanging_nodes[j]=0;
         }

       } // loop over haloed nodes

      // Now send all the required info to the halo layer
      MPI_Send(&count_masters,1,MPI_INT,d,0,MPI_COMM_WORLD);

      // If there are no discrepancies, no need to send anything
      if (count_masters!=0)
       {
        // Send the master node numbers
        MPI_Send(&hanging_masters[0],count_masters,MPI_INT,d,1,MPI_COMM_WORLD);
        // Send the master weights
        MPI_Send(&count_weights,1,MPI_INT,d,2,MPI_COMM_WORLD);
        MPI_Send(&hanging_master_weights[0],count_weights,MPI_DOUBLE,d,3,
                 MPI_COMM_WORLD);
        // Send the vector showing where the discrepancies are
        MPI_Send(&hanging_nodes[0],nnod_haloed,MPI_INT,d,4,MPI_COMM_WORLD);
       }
     
     }
    else // (d==MPI_Helpers::My_rank)
     {
      // Recevie the master nodes and weights in order to create new 
      // hanging nodes in the halo layer

      for (int dd=0; dd<MPI_Helpers::Nproc; dd++)
       {
        if (dd!=d) // don't talk to yourself
         {
          // Loop over the halo layer of dd
          Vector<int> haloed_hanging=vector_haloed_hanging[dd];
          Vector<int> halo_hanging=vector_halo_hanging[dd];
//          unsigned nhaloed=haloed_hanging.size();
//          unsigned nnod_halo=halo_hanging.size();
          // the above two lines would appear to be unnecessary
          unsigned nhalo=nhalo_node(dd);
//          unsigned nshare=nshared_node(dd);
          Vector<int> hanging_nodes(nhalo);
          unsigned count_masters; // count for hanging_masters size
          Vector<int> hanging_masters;
          unsigned count_weights; // count for hanging_master_weights size
          Vector<double> hanging_master_weights;
//          oomph_info << "From process " << dd << std::endl
//                     << ", halo check: " << nhalo << std::endl
//                     << "-- Shared nodes: " << nshare << std::endl;

          // How mcuh information are we receiving?
          MPI_Recv(&count_masters,1,MPI_INT,dd,0,MPI_COMM_WORLD,&status);

          // If no information, no need to do anything else
          if (count_masters!=0)
           {
            // Receive the master node numbers
            hanging_masters.resize(count_masters);
            MPI_Recv(&hanging_masters[0],count_masters,MPI_INT,dd,1,
                   MPI_COMM_WORLD,&status);

            // Receive the master weights
            MPI_Recv(&count_weights,1,MPI_INT,dd,2,MPI_COMM_WORLD,&status);
            hanging_master_weights.resize(count_weights);
            MPI_Recv(&hanging_master_weights[0],count_weights,MPI_DOUBLE,dd,3,
                   MPI_COMM_WORLD,&status);

            // Receive the vector describing the position of discrepancies
            hanging_nodes.resize(nhalo);
            MPI_Recv(&hanging_nodes[0],nhalo,MPI_INT,dd,4,
                     MPI_COMM_WORLD,&status);

            count_masters=0;
            count_weights=0; // reset counters

            for (unsigned j=0; j<nhalo; j++)
             {
              // Find positive entries of the hanging_nodes vector
              if (hanging_nodes[j]>0) 
               {
                // This entry tells us how many master nodes we have
                unsigned nhd_master=hanging_nodes[j];
//                count_masters++;

                // Set up a new HangInfo for this node
                HangInfo* hang_pt = new HangInfo(nhd_master);
  
                // Now set up the master nodes and weights
                for (unsigned m=0; m<nhd_master; m++)
                 {
                  // Get the sent master node (a shared node) and the weight
                  Node* nod_pt=shared_node_pt(dd,
                                            hanging_masters[count_masters]);
                  count_masters++;
                  double mtr_weight=hanging_master_weights[count_weights];
                  count_weights++;

                  // Set as a master node (with corresponding weight)
                  hang_pt->set_master_node_pt(m,nod_pt,mtr_weight);
                 }

                // set the hanging pointer to the current halo node
                halo_node_pt(dd,j)->set_hanging_pt(hang_pt,icont);

                // This is just for a Node: should it also be done for a 
                // SolidNode?  It would seem not - is_hanging==1 for all
                // examples that I've seen so far... (andy, 20/02/08)
                // I can't think of a case where this would be needed,
                // so I've commented it out for now

//                SolidNode* solid_nod_pt=dynamic_cast<SolidNode*>
//                 (halo_node_pt(dd,j));
//
//                if (solid_nod_pt!=0)
//                 {
//                  oomph_info << "This is also a solid node with is_hanging="
//                             << solid_nod_pt->is_hanging(icont)
//                             << " (" << solid_nod_pt << ")" << std::endl;
//                  solid_nod_pt->set_hanging_pt(hang_pt,icont);
//                 }

               }
              else if (hanging_nodes[j]<0)
               {
                // The hanging node already exists, but it shouldn't!
                // Set it to nonhanging
                halo_node_pt(dd,j)->set_nonhanging();

                count_masters++;
               }
             } // loop over halo nodes
          
           } // if count_masters!=0
         
         } // if dd!=d
         
       } // loop over other processors
     }

   } // loop over all processors

   } // loop over interpolated values

 }

#endif

}