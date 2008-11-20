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
#ifndef OOMPH_REFINEABLE_MESH_HEADER
#define OOMPH_REFINEABLE_MESH_HEADER

#include "mesh.h"
#include "refineable_elements.h"
//Include the tree template to fill in the C++ split function
//Must be called after refineable_element.h
#include "tree.template.cc"
#include "error_estimator.h" 

namespace oomph
{

//=======================================================================
/// Base class for refineable meshes. Provides standardised interfaces
/// for the following standard mesh adaptation routines:
/// - Uniform mesh refinement.
/// - Adaptation, based on the elemental error estimates obtained
///   from an error estimator function which is accessed via a function 
///   pointer. 
///
//=======================================================================
class RefineableMeshBase : public virtual Mesh
{
public:


 /// Constructor sets default values for refinement targets etc.
 /// and initialises pointer to spatial error estimator to NULL.
 RefineableMeshBase()
  {
   // Initialise pointer to spatial error estimator
   Spatial_error_estimator_pt=0;
 
   // Targets
   Min_permitted_error=1.0e-5;
   Max_permitted_error=1.0e-3;

   //Actual errors
   Min_error = 0.0;
   Max_error = 0.0;
    
   // Max/min refinement levels
   Max_refinement_level=5;
   Min_refinement_level=0;
 
   // Stats
   Nrefined=0;
   Nunrefined=0;
      
   // Do I adapt or not?
   Adapt_flag=true;
 
   // Where do I write the documatation of the refinement process?
   Doc_info_pt=0;

   // Number of elements where the refinement is over-ruled
   Nrefinement_overruled = 0;

   // If only a few elements are scheduled for unrefinement, don't bother
   // By default unrefine all
   Max_keep_unrefined=0;
   
   // Initialise the forest pointer to NULL
   Forest_pt = 0;

  };


 /// Broken copy constructor
 RefineableMeshBase(const RefineableMeshBase& dummy) 
  { 
   BrokenCopy::broken_copy("RefineableMeshBase");
  } 
 
 /// Broken assignment operator
 void operator=(const RefineableMeshBase&) 
  {
   BrokenCopy::broken_assign("RefineableMeshBase");
  }

 /// Empty Destructor: 
 virtual ~RefineableMeshBase()
  {
   //Kill the forest if there is one
   if(Forest_pt!=0)
    {
     delete Forest_pt;
     Forest_pt=0;
    }
  }

 /// Set up the tree forest associated with the Mesh (if any)
 virtual void setup_tree_forest()=0;

 /// Return pointer to the Forest represenation of the mesh
 TreeForest* forest_pt(){return Forest_pt;}


 /// Doc the targets for mesh adaptation
 void doc_adaptivity_targets(std::ostream &outfile)
  {
   outfile << std::endl;
   outfile << "Targets for mesh adaptation: " << std::endl;
   outfile << "---------------------------- " << std::endl;
   outfile << "Target for max. error: " << Max_permitted_error << std::endl;
   outfile << "Target for min. error: " << Min_permitted_error << std::endl;
   outfile << "Min. refinement level: " << Min_refinement_level << std::endl;
   outfile << "Max. refinement level: " << Max_refinement_level << std::endl;
   outfile << "Don't unrefine if less than " << Max_keep_unrefined 
           << " elements need unrefinement." << std::endl;
   outfile << std::endl;
  }


 /// Access to spatial error estimator
 ErrorEstimator*& spatial_error_estimator_pt()
  {
   return Spatial_error_estimator_pt;
  }

 /// Access to spatial error estimator (const version
 ErrorEstimator* spatial_error_estimator_pt() const
  {
   return Spatial_error_estimator_pt;
  }

 /// \short Access fct for min. error (i.e. (try to) merge elements if 
 /// their error is smaller)
 double& min_permitted_error() {return Min_permitted_error;}

 /// \short Access fct for max. error (i.e. split elements if their 
 /// error is larger)
 double& max_permitted_error() {return Max_permitted_error;}

 /// \short Access fct for min. actual error in present solution (i.e. before
 /// re-solve on adapted mesh)
 double& min_error() {return Min_error;}

 /// \short Access fct for max. actual error in present solution (i.e. before
 /// re-solve on adapted mesh)
 double& max_error() {return Max_error;}

 ///  Access fct for number of elements that were refined
 unsigned& nrefined() {return Nrefined;}
 
 /// Access fct for  number of elements that were unrefined
 unsigned& nunrefined() {return Nunrefined;}
 
 ///  Access fct for pointer to DocInfo
 DocInfo*& doc_info_pt() {return Doc_info_pt;}
 
 /// Access fct for flag that requests adaptation
 bool& adapt_flag() {return Adapt_flag;}
 
 ///  Access fct for DocInfo
 DocInfo doc_info()
  {
   return *Doc_info_pt;
  }
 
 /// Access fct for max. permissible refinement level (relative to base mesh)
 unsigned& max_refinement_level() {return Max_refinement_level;}
 
 /// Access fct for min. permissible refinement level (relative to base mesh)
 unsigned& min_refinement_level() {return Min_refinement_level;}
 
 /// \short Number of elements that would have liked to be refined further 
 /// but can't because they've reached the max. refinement level
 unsigned& nrefinement_overruled(){return Nrefinement_overruled;}
 
 
 /// \short Max. number of elements that we allow to remain unrefined
 /// if no other mesh adaptation is required (to avoid 
 /// mesh-adaptations that would only unrefine a few elements
 /// and then force a new solve -- this can't be worth our while!)
 unsigned& max_keep_unrefined(){return Max_keep_unrefined;}
 
 /// \short Adapt mesh: Refine elements whose error is lager than err_max
 /// and (try to) unrefine those whose error is smaller than err_min
 virtual void adapt(Vector<double>& elemental_error);

 /// \short Perform the actual mesh adaptation, documenting the progress in
 /// the directory specified in DocInfo object.
 virtual void adapt_mesh(DocInfo& doc_info);

 /// \short Perform the actual mesh adaptation. A simple wrapper to 
 /// call the function without documentation.
 virtual void adapt_mesh()
  {
   //Create a dummy doc_info object
   DocInfo doc_info;
   doc_info.directory()="";
   doc_info.doc_flag()=false;
   //Call the other adapt mesh
   adapt_mesh(doc_info);
  }

 /// Refine mesh uniformly and doc process
 virtual void refine_uniformly(DocInfo& doc_info);
 
 /// Refine mesh uniformly
 virtual void refine_uniformly()
  {
   DocInfo doc_info;
   doc_info.directory()="";
   doc_info.doc_flag()=false;
   refine_uniformly(doc_info);
  }

 /// \short Refine mesh by splitting the elements identified
 /// by their numbers.
 virtual void refine_selected_elements(const Vector<unsigned>& 
                                       elements_to_be_refined);

 /// \short Refine mesh by splitting the elements identified
 /// by their pointers.
 virtual void refine_selected_elements(const Vector<RefineableElement*>& 
                                       elements_to_be_refined);

 
 /// Refine base mesh to same degree as reference mesh (relative
 /// to original unrefined mesh).
 virtual void refine_base_mesh_as_in_reference_mesh(
  RefineableMeshBase* const &ref_mesh_pt);

 /// \short Refine mesh once so that its topology etc becomes that of the 
 /// (finer!) reference mesh -- if possible! Useful for meshes in multigrid 
 /// hierarchies. If the meshes are too different and the conversion
 /// cannot be performed, the code dies (provided PARANOID is enabled).
 virtual void refine_as_in_reference_mesh(RefineableMeshBase* 
                                          const &ref_mesh_pt);

 /// \short Unrefine mesh uniformly: Return 0 for success,
 /// 1 for failure (if unrefinement has reached the coarsest permitted
 /// level)
 virtual unsigned unrefine_uniformly();


 /// Get max/min refinement levels in mesh
 virtual void get_refinement_levels(unsigned& min_refinement_level,
                                    unsigned& max_refinement_level);

 /// \short Extract refinement pattern: Consider the hypothetical mesh 
 /// obtained by truncating the refinement of the current mesh to a given 
 /// level (where \c level=0 is the un-refined base mesh). To advance
 /// to the next refinement level, we need to refine (split) the
 /// \c to_be_refined[level].size() elements identified by the
 /// element numbers contained in \c vector to_be_refined[level][...]
 virtual void get_refinement_pattern(Vector<Vector<unsigned> >& 
                                     to_be_refined);

 /// Refine base mesh according to specified refinement pattern
 virtual void refine_base_mesh(Vector<Vector<unsigned> >& to_be_refined);

 /// Refine mesh according to refinement pattern in restart file
 virtual void refine(std::ifstream& restart_file);

 /// Dump refinement pattern to allow for rebuild
 virtual void dump_refinement(std::ostream &outfile);

 /// Read refinement pattern to allow for rebuild
 virtual void read_refinement(std::ifstream& restart_file, 
                              Vector<Vector<unsigned> >& to_be_refined); 

#ifdef OOMPH_HAS_MPI
 /// Synchronise the hanging nodes if the mesh is distributed
 void synchronise_hanging_nodes(const unsigned& ncont_interpolated_values);
#endif

protected:
 
 /// \short Split all the elements in the mesh if required. This template free
 /// interface will be overloaded in RefineableMesh<ELEMENT> so that
 /// any new elements that are created will be of the correct type.
 virtual void split_elements_if_required()=0;

 /// \short Complete the hanging node scheme recursively
 void complete_hanging_nodes(const int& ncont_interpolated_values);

 
 /// Auxiliary routine for recursive hanging node completion
 void complete_hanging_nodes_recursively(Node*& nod_pt,
                                         Vector<Node*>& master_nodes,
                                         Vector<double>& hang_weights,
                                         const int &ival);

 /// \short Pointer to spatial error estimator
 ErrorEstimator* Spatial_error_estimator_pt;

 /// Max. error (i.e. split elements if their error is larger)
 double Max_permitted_error;
 
 /// Min. error (i.e. (try to) merge elements if their error is smaller)
 double Min_permitted_error;
 
 /// Min.actual error
 double Min_error;
 
 /// Max. actual error 
 double Max_error;
 
 /// Max. permissible refinement level (relative to base mesh)
 unsigned Max_refinement_level;

 /// Min. permissible refinement level (relative to base mesh)
 unsigned Min_refinement_level;

 /// Stats: Number of elements that were refined
 unsigned Nrefined;

 /// Stats: Number of elements that were unrefined
 unsigned Nunrefined;

 /// Flag that requests adaptation
 bool Adapt_flag;

 /// Pointer to DocInfo
 DocInfo* Doc_info_pt;

 /// \short Number of elements that would like to be refined further but can't
 /// because they've reached the max. refinement level
 unsigned Nrefinement_overruled;

 /// \short Max. number of elements that can remain unrefined
 /// if no other mesh adaptation is required (to avoid 
 /// mesh-adaptations that would only unrefine a few elements
 /// and then force a new solve -- this can't be worth our while!)
 unsigned Max_keep_unrefined;

 /// Forest representation of the mesh
 TreeForest* Forest_pt;

};

//=======================================================================
/// Templated base class for refineable meshes. The use of the template
/// parameter is required only for creating new elements during mesh
/// adaptation. This class overloaded the template-free inteface to
/// the function split_elements_if_required() to make use of the template
/// parameter.
/// All refineable meshes should inherit directly from 
///  RefineableMesh<ELEMENT>
//=======================================================================
template <class ELEMENT>
class RefineableMesh : public RefineableMeshBase
 {
   private:
   
  /// \short Split all the elements if required. Overload the template-free
  /// interface so that any new elements that are created 
  /// will be of the correct type.
  void split_elements_if_required()
   {
    //Find the number of trees in the forest
    unsigned n_tree = this->Forest_pt->ntree();
    //Loop over all "active" elements in the forest and split them
    //if required
    for (unsigned long e=0;e<n_tree;e++)
     {
      this->Forest_pt->tree_pt(e)->
       traverse_leaves(&Tree::split_if_required<ELEMENT>);
     }
   }

   public:

  ///Constructor, call the constructor of the base class
  RefineableMesh() : RefineableMeshBase() { }
  
  /// Broken copy constructor
  RefineableMesh(const RefineableMesh& dummy) 
   { 
    BrokenCopy::broken_copy("RefineableMesh");
   } 
  
  ///Empty virtual destructor
  virtual ~RefineableMesh() { }

 };

}

#endif