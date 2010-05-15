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
#ifndef OOMPH_XDA_TET_MESH_HEADER
#define OOMPH_XDA_TET_MESH_HEADER


#include<iterator>
#include<algorithm>

#include "../generic/mesh.h"
#include "../generic/tet_mesh.h"

namespace oomph
{


//========================================================================
/// Tet mesh made of quadratic (ten node) tets built from xda input file.
//========================================================================
 template<class ELEMENT>
  class XdaTetMesh : public virtual TetMeshBase
 {

   public:
  
  /// \short Constructor: Pass name of xda file. Boolean specifies if all
  /// boundary elements get their own ID -- this is required for
  /// FSI problems. In this case, the vector containing the oomph-lib
  /// boundary IDs of all oomph-lib boundaries that collectively form
  /// a given boundary as specified in the xda input file can be
  /// obtained from the access function oomph_lib_boundary_ids(...). 
  /// Timestepper defaults to steady pseudo-timestepper.
  XdaTetMesh(const std::string xda_file_name, 
             const bool& use_separate_boundary_ids,
             TimeStepper* time_stepper_pt=
             &Mesh::Default_TimeStepper);
  
  /// \short Setup boundary coordinate on boundary b while is
  /// temporarily flattened to simplex faces. Boundary coordinates are the
  /// x-y coordinates in the plane of that boundary with the
  /// x-axis along the line from the (lexicographically)
  /// "lower left" to the "upper right" node. The y axis
  /// is obtained by taking the cross-product of the positive
  /// x direction with the outer unit normal computed by
  /// the face elements (or its negative if switch_normal is set
  /// to true).
  void setup_boundary_coordinates(const unsigned& b,
                                  const bool& switch_normal)
  {
   std::ofstream outfile;
   setup_boundary_coordinates(b,switch_normal,outfile);
  }
  
  
  /// Setup boundary coordinate on boundary b while is
  /// temporarily flattened to simplex faces. Boundary coordinates are the
  /// x-y coordinates in the plane of that boundary with the
  /// x-axis along the line from the (lexicographically)
  /// "lower left" to the "upper right" node. The y axis
  /// is obtained by taking the cross-product of the positive
  /// x direction with the outer unit normal computed by
  /// the face elements (or its negative if switch_normal is set
  /// to true). Doc faces in output file.
  void setup_boundary_coordinates(const unsigned& b,
                                  const bool& switch_normal,
                                  std::ofstream& outfile);
  
  /// \short Access functions to the Vector of oomph-lib boundary ids
  /// that make up boundary b in the original xda enumeration
  Vector<unsigned> oomph_lib_boundary_ids(const unsigned& xda_boundary_id)
   {
    return Boundary_id[xda_boundary_id];
   }

   private:

  /// \short Vector of vectors containing the boundary IDs of
  /// the overall boundary specified in the xda file.
  Vector<Vector<unsigned> > Boundary_id;

}; 


}

#endif
