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
//Header file for Advection Diffusion elements
#ifndef OOMPH_ADV_DIFF_REACT_ELEMENTS_HEADER
#define OOMPH_ADV_DIFF_REACT_ELEMENTS_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//OOMPH-LIB headers
#include "../generic/nodes.h"
#include "../generic/Qelements.h"
#include "../generic/oomph_utilities.h"

namespace oomph
{

//=============================================================
/// \short A class for all elements that solve the Advection 
/// Diffusion Reaction equations using isoparametric elements.
/// \f[ 
/// \tau_{i} \frac{\partial C_{i}}{\partial t} 
/// + w_{j} \frac{\partial C_{i}}{\partial x_{j}} = 
/// D_{i}\frac{\partial^2 C_{i}}{\partial x_j^2} = 
/// - R_{i}(C_{i}) + f_{i}
/// \f] 
/// This contains the generic maths. Shape functions, geometric
/// mapping etc. must get implemented in derived class.
//=============================================================
template <unsigned NREAGENT, unsigned DIM>
class AdvectionDiffusionReactionEquations : public virtual FiniteElement
{

public:

 /// \short Function pointer to source function fct(x,f(x)) -- 
 /// x is a Vector! 
 typedef void (*AdvectionDiffusionReactionSourceFctPt)(const Vector<double>& x,
                                                       Vector<double> &f);

 /// \short Function pointer to reaction terms
 typedef void (*AdvectionDiffusionReactionReactionFctPt)(
  const Vector<double> &c, Vector<double> &R);

 /// \short Function pointer to derivative of reaction terms
 typedef void (*AdvectionDiffusionReactionReactionDerivFctPt)(
  const Vector<double> &c, DenseMatrix<double> &dRdC);


 /// \short Function pointer to wind function fct(x,w(x)) -- 
 /// x is a Vector! 
 typedef void (*AdvectionDiffusionReactionWindFctPt)(const double &time,
                                                     const Vector<double>& x,
                                                     Vector<double>& wind);


 /// \short Constructor: Initialise the Source_fct_pt, Wind_fct_pt,
 /// Reaction_fct_pt to null and initialise the dimensionless 
 /// timescale and diffusion ratios
 AdvectionDiffusionReactionEquations() : Source_fct_pt(0), Wind_fct_pt(0),
  Reaction_fct_pt(0), Reaction_deriv_fct_pt(0),
  ALE_is_disabled(false)
  {
   //Set diffusion coefficients to default
   Diff_pt = &Default_dimensionless_number;
   //Set timescales to default
   Tau_pt = &Default_dimensionless_number;
  }
 
 /// Broken copy constructor
 AdvectionDiffusionReactionEquations(
  const AdvectionDiffusionReactionEquations& dummy) 
  { 
   BrokenCopy::broken_copy("AdvectionDiffusionReactionEquations");
  } 
 
 /// Broken assignment operator
 void operator=(const AdvectionDiffusionReactionEquations&) 
  {
   BrokenCopy::broken_assign("AdvectionDiffusionReactionEquations");
  }
 
 /// \short Return the index at which the unknown i-th reagent
 /// is stored. The default value, i, is appropriate for single-physics
 /// problems, when there are only i variables, the values that satisfies
 /// the advection-diffusion-reaction equation. 
 /// In derived multi-physics elements, this function should be overloaded
 /// to reflect the chosen storage scheme. Note that these equations require
 /// that the unknown is always stored at the same index at each node.
 virtual inline unsigned c_index_adv_diff_react(const unsigned &i) 
  const {return i;}

/// \short dc_r/dt at local node n. 
 /// Uses suitably interpolated value for hanging nodes.
 double dc_dt_adv_diff_react(const unsigned &n, const unsigned &r) const
  {
   // Get the data's timestepper
   TimeStepper* time_stepper_pt= this->node_pt(n)->time_stepper_pt();

   //Initialise dudt
   double dudt=0.0;
   //Loop over the timesteps, if there is a non Steady timestepper
   if (!time_stepper_pt->is_steady())
    {
     //Find the index at which the variable is stored
     const unsigned c_nodal_index = c_index_adv_diff_react(r);

     // Number of timsteps (past & present)
     const unsigned n_time = time_stepper_pt->ntstorage();
     
     for(unsigned t=0;t<n_time;t++)
      {
       dudt += time_stepper_pt->weight(1,t)*nodal_value(t,n,c_nodal_index);
      }
    }
   return dudt;
  }


/// \short dc/dt at local node n. 
 /// Uses suitably interpolated value for hanging nodes.
 void dc_dt_adv_diff_react(const unsigned &n, Vector<double> &dc_dt) const
  {
   // Get the data's timestepper
   TimeStepper* time_stepper_pt= this->node_pt(n)->time_stepper_pt();

   //Initialise to zero
   for(unsigned r=0;r<NREAGENT;r++) {dc_dt[r] = 0.0;}
   
   //Loop over the timesteps, if there is a non Steady timestepper
   if (!time_stepper_pt->is_steady())
    {
     // Number of timsteps (past & present)
     const unsigned n_time = time_stepper_pt->ntstorage();
     //Local storage (cache) for the weights
     double weight[n_time];
     for(unsigned t=0;t<n_time;t++) 
      {weight[t] = time_stepper_pt->weight(1,t);}

     //Loop over the reagents
     for(unsigned r=0;r<NREAGENT;r++)
      {
       //Find the index at which the variable is stored
       const unsigned c_nodal_index = c_index_adv_diff_react(r);
       
       for(unsigned t=0;t<n_time;t++)
        {
         dc_dt[r] += weight[t]*nodal_value(t,n,c_nodal_index);
        }
      }
    }
  }

 /// \short Disable ALE, i.e. assert the mesh is not moving -- you do this
 /// at your own risk!
 void disable_ALE()
  {
   ALE_is_disabled=true;
  }

 /// \short (Re-)enable ALE, i.e. take possible mesh motion into account
 /// when evaluating the time-derivative. Note: By default, ALE is 
 /// enabled, at the expense of possibly creating unnecessary work 
 /// in problems where the mesh is, in fact, stationary. 
 void enable_ALE()
  {
   ALE_is_disabled=false;
  }


 /// Output with default number of plot points
 void output(std::ostream &outfile) 
  {
   unsigned nplot=5;
   output(outfile,nplot);
  }

 /// \short Output FE representation of soln: x,y,u or x,y,z,u at 
 /// nplot^DIM plot points
 void output(std::ostream &outfile, const unsigned &nplot);


 /// C_style output with default number of plot points
 void output(FILE* file_pt)
  {
   unsigned n_plot=5;
   output(file_pt,n_plot);
  }

 /// \short C-style output FE representation of soln: x,y,u or x,y,z,u at 
 /// n_plot^DIM plot points
 void output(FILE* file_pt, const unsigned &n_plot);


 /// Output exact soln: x,y,u_exact or x,y,z,u_exact at nplot^DIM plot points
 void output_fct(std::ostream &outfile, const unsigned &nplot, 
                 FiniteElement::SteadyExactSolutionFctPt 
                 exact_soln_pt);

 /// \short Output exact soln: x,y,u_exact or x,y,z,u_exact at 
 /// nplot^DIM plot points (dummy time-dependent version to 
 /// keep intel compiler happy)
 virtual void output_fct(std::ostream &outfile, const unsigned &nplot,
                         const double& time, 
  FiniteElement::UnsteadyExactSolutionFctPt exact_soln_pt)
  {
   throw OomphLibError(
    "There is no time-dependent output_fct() for Advection Diffusion elements",
    "AdvectionDiffusionReactionEquations<DIM>::output_fct()",
    OOMPH_EXCEPTION_LOCATION);
  }


 /// Get error against and norm of exact solution
 void compute_error(std::ostream &outfile, 
                    FiniteElement::SteadyExactSolutionFctPt 
                    exact_soln_pt, double& error, double& norm);


 /// Dummy, time dependent error checker
 void compute_error(std::ostream &outfile, 
                    FiniteElement::UnsteadyExactSolutionFctPt 
                    exact_soln_pt,
                    const double& time, double& error, double& norm)
  {
   throw OomphLibError(
    "No time-dependent compute_error() for Advection Diffusion elements",
    "AdvectionDiffusionReactionEquations<DIM>::compute_error()",
    OOMPH_EXCEPTION_LOCATION);
  }


 /// Access function: Pointer to source function
 AdvectionDiffusionReactionSourceFctPt& source_fct_pt() 
  {return Source_fct_pt;}


 /// Access function: Pointer to source function. Const version
 AdvectionDiffusionReactionSourceFctPt source_fct_pt() const 
  {return Source_fct_pt;}


 /// Access function: Pointer to wind function
 AdvectionDiffusionReactionWindFctPt& wind_fct_pt() 
  {return Wind_fct_pt;}

 /// Access function: Pointer to reaction function. Const version
 AdvectionDiffusionReactionWindFctPt wind_fct_pt() const 
  {return Wind_fct_pt;}
 
 /// Access function: Pointer to reaction function
 AdvectionDiffusionReactionReactionFctPt& reaction_fct_pt() 
  {return Reaction_fct_pt;}
 
 /// Access function: Pointer to reaction function. Const version
 AdvectionDiffusionReactionReactionFctPt reaction_fct_pt() const 
  {return Reaction_fct_pt;}
 
 /// Access function: Pointer to reaction derivatives function
 AdvectionDiffusionReactionReactionDerivFctPt& reaction_deriv_fct_pt() 
  {return Reaction_deriv_fct_pt;}

 /// Access function: Pointer to reaction function. Const version
 AdvectionDiffusionReactionReactionDerivFctPt reaction_deriv_fct_pt() const 
  {return Reaction_deriv_fct_pt;}
 
 /// Vector of diffusion coefficients
 const Vector<double> &diff() const {return *Diff_pt;}

 /// Pointer to vector of diffusion coefficients
 Vector<double>* &diff_pt() {return Diff_pt;}

 /// Vector of dimensionless timescales
 const Vector<double> &tau() const {return *Tau_pt;}

 /// Pointer to vector of dimensionless timescales
 Vector<double>* &tau_pt() {return Tau_pt;}

 /// \short Get source term at (Eulerian) position x. This function is
 /// virtual to allow overloading in multi-physics problems where
 /// the strength of the source function might be determined by
 /// another system of equations 
 inline virtual void get_source_adv_diff_react(const Vector<double>& x, 
                                               Vector<double> &source) const
  {
   //If no source function has been set, return zero
   if(Source_fct_pt==0) 
    {
     for(unsigned r=0;r<NREAGENT;r++){source[r] = 0.0;}
    }
   else
    {
     // Get source strength
     (*Source_fct_pt)(x,source);
    }
  }

 /// \short Get wind at (Eulerian) position x and/or local coordinate s. 
 /// This function is
 /// virtual to allow overloading in multi-physics problems where
 /// the wind function might be determined by
 /// another system of equations 
 inline virtual void get_wind_adv_diff_react(const Vector<double> &s, 
                                             const Vector<double>& x,
                                             Vector<double>& wind) const
  {
   //If no wind function has been set, return zero
   if(Wind_fct_pt==0)
    {
     for(unsigned i=0;i<DIM;i++) {wind[i]= 0.0;}
    }
   else
    {
     // Get wind
     (*Wind_fct_pt)(time(),x,wind);
    }
  }


 /// \short Get reaction as a function of the given reagent concentrations
 /// This function is
 /// virtual to allow overloading in multi-physics problems where
 /// the reaction function might be determined by
 /// another system of equations 
 inline virtual void get_reaction_adv_diff_react(const Vector<double> &C, 
                                                 Vector<double>& R) const
  {
   //If no wind function has been set, return zero
   if(Reaction_fct_pt==0)
    {
     for(unsigned r=0;r<NREAGENT;r++) {R[r]= 0.0;}
    }
   else
    {
     // Get reaction terms
     (*Reaction_fct_pt)(C,R);
    }
  }

 /// \short Get the derivatives of the reaction terms with respect to the 
 /// concentration variables. If no explicit function pointer is set,
 /// these will be calculated by finite differences
 virtual void get_reaction_deriv_adv_diff_react(const Vector<double> &C,
                                                DenseMatrix<double> &dRdC)
  const
  {
   //If no reaction pointer set, return zero
   if(Reaction_fct_pt==0)
    {
     for(unsigned r=0;r<NREAGENT;r++)
      {
       for(unsigned p=0;p<NREAGENT;p++)
        {
         dRdC(r,p) = 0.0;
        }
      }
    }
   else
    {
     //If no function pointer get finite differences
     if(Reaction_deriv_fct_pt==0)
      {
       //Local copy of the unknowns
       Vector<double> C_local = C;
       //Finite differences
       Vector<double> R(NREAGENT), R_plus(NREAGENT), R_minus(NREAGENT);
       //Get the initial reaction terms
       //(*Reaction_fct_pt)(C,R);
       const double fd_step = GeneralisedElement::Default_fd_jacobian_step;
       //Now loop over all the reagents
       for(unsigned p=0;p<NREAGENT;p++)
        {
         //Store the old value
         double old_var = C_local[p];
         //Increment the value
         C_local[p] += fd_step;
         //Get the new values
         (*Reaction_fct_pt)(C_local,R_plus);
         
         //Reset the value
         C_local[p] = old_var;
         //Decrement the value
         C_local[p] -= fd_step;
         //Get the new values
         (*Reaction_fct_pt)(C_local,R_minus);
         
         //Assemble the column of the jacobian
         for(unsigned r=0;r<NREAGENT;r++)
          {
           dRdC(r,p) = (R_plus[r] - R_minus[r])/(2.0*fd_step);
          }

         //Reset the value
         C_local[p] = old_var;
        }
      }
     //Otherwise get the terms from the function
     else
      {
       (*Reaction_deriv_fct_pt)(C,dRdC);
      }
    }
  }

 /// Get flux: \f$\mbox{flux}[DIM r + i] = \mbox{d}C_{r} / \mbox{d}x_i \f$
 void get_flux(const Vector<double>& s, Vector<double>& flux) const
  {
   //Find out how many nodes there are in the element
   const unsigned n_node = nnode();
   
   //Set up memory for the shape and test functions
   Shape psi(n_node);
   DShape dpsidx(n_node,DIM);
 
   //Call the derivatives of the shape and test functions
   dshape_eulerian(s,psi,dpsidx);
     
   //Initialise to zero
   for(unsigned j=0;j<DIM*NREAGENT;j++) {flux[j] = 0.0;}
   
   //Loop over the reagent terms
   for(unsigned r=0;r<NREAGENT;r++)
    {
     unsigned c_nodal_index = c_index_adv_diff_react(r);

     //Loop over derivative directions
     for(unsigned j=0;j<DIM;j++)
      {
       unsigned index = r*DIM + j;
       //Loop over the nodes
       for(unsigned l=0;l<n_node;l++) 
        {
         flux[index] += nodal_value(l,c_nodal_index)*dpsidx(l,j);
        }
      }
    }
  }

 
 /// Add the element's contribution to its residual vector (wrapper)
 void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
   //Call the generic residuals function with flag set to 0 and using
   //a dummy matrix
   fill_in_generic_residual_contribution_adv_diff_react(
    residuals,GeneralisedElement::Dummy_matrix,
    GeneralisedElement::Dummy_matrix,0);
  }

 
 /// \short Add the element's contribution to its residual vector and 
 /// the element Jacobian matrix (wrapper)
 void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                   DenseMatrix<double> &jacobian)
  {
   //Call the generic routine with the flag set to 1
   fill_in_generic_residual_contribution_adv_diff_react(
    residuals,jacobian,GeneralisedElement::Dummy_matrix,1);
  }
 

 /// Add the element's contribution to its residuals vector,
 /// jacobian matrix and mass matrix
 void fill_in_contribution_to_jacobian_and_mass_matrix(
  Vector<double> &residuals, DenseMatrix<double> &jacobian, 
  DenseMatrix<double> &mass_matrix)
  {
   //Call the generic routine with the flag set to 2
   fill_in_generic_residual_contribution_adv_diff_react(residuals,
                                                        jacobian,
                                                        mass_matrix,2);
  }


 /// Return FE representation of function value c_i(s) at local coordinate s
 inline double interpolated_c_adv_diff_react(const Vector<double> &s,
                                             const unsigned &i) const
  {
   //Find number of nodes
   unsigned n_node = nnode();

   //Get the nodal index at which the unknown is stored
   unsigned c_nodal_index = c_index_adv_diff_react(i);

   //Local shape function
   Shape psi(n_node);

   //Find values of shape function
   shape(s,psi);

   //Initialise value of u
   double interpolated_u = 0.0;

   //Loop over the local nodes and sum
   for(unsigned l=0;l<n_node;l++) 
    {
     interpolated_u += nodal_value(l,c_nodal_index)*psi[l];
    }

   return(interpolated_u);
  }


 /// \short Self-test: Return 0 for OK
 unsigned self_test();

 /// \short Return the integrated reagent concentrations
 void integrate_reagents(Vector<double> &C) const;

protected:


 /// \short Shape/test functions and derivs w.r.t. to global coords at 
 /// local coord. s; return  Jacobian of mapping
 virtual double dshape_and_dtest_eulerian_adv_diff_react(
  const Vector<double> &s, 
  Shape &psi, 
  DShape &dpsidx, 
  Shape &test, 
  DShape &dtestdx) const=0;

 /// \short Shape/test functions and derivs w.r.t. to global coords at 
 /// integration point ipt; return  Jacobian of mapping
 virtual double dshape_and_dtest_eulerian_at_knot_adv_diff_react(
  const unsigned &ipt, 
  Shape &psi, 
  DShape &dpsidx,
  Shape &test, 
  DShape &dtestdx) 
  const=0;

 /// \short Add the element's contribution to its residual vector only 
 /// (if flag=and/or element  Jacobian matrix 
 virtual void fill_in_generic_residual_contribution_adv_diff_react(
  Vector<double> &residuals, DenseMatrix<double> &jacobian, 
  DenseMatrix<double> &mass_matrix, unsigned flag); 
 
 /// Pointer to global diffusion coefficients
 Vector<double> *Diff_pt;

 /// Pointer to global timescales
 Vector<double> *Tau_pt;

 /// Pointer to source function:
 AdvectionDiffusionReactionSourceFctPt Source_fct_pt;
 
 /// Pointer to wind function:
 AdvectionDiffusionReactionWindFctPt Wind_fct_pt;
 
 /// Pointer to reaction function
 AdvectionDiffusionReactionReactionFctPt Reaction_fct_pt;

 /// Pointer to reaction derivatives
 AdvectionDiffusionReactionReactionDerivFctPt Reaction_deriv_fct_pt;

 /// \short Boolean flag to indicate if ALE formulation is disabled when 
 /// time-derivatives are computed. Only set to true if you're sure
 /// that the mesh is stationary.
 bool ALE_is_disabled;

  private:

 /// Static default value for the dimensionless numbers
 static Vector<double> Default_dimensionless_number;
 
  
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



//======================================================================
/// \short QAdvectionDiffusionReactionElement elements are 
/// linear/quadrilateral/brick-shaped Advection Diffusion elements with 
/// isoparametric interpolation for the function.
//======================================================================
template <unsigned NREAGENT, unsigned DIM, unsigned NNODE_1D>
 class QAdvectionDiffusionReactionElement : 
 public virtual QElement<DIM,NNODE_1D>,
 public virtual AdvectionDiffusionReactionEquations<NREAGENT,DIM>
 {
  public:


 ///\short  Constructor: Call constructors for QElement and 
 /// Advection Diffusion equations
 QAdvectionDiffusionReactionElement() : QElement<DIM,NNODE_1D>(), 
  AdvectionDiffusionReactionEquations<NREAGENT,DIM>()
  { }

 /// Broken copy constructor
 QAdvectionDiffusionReactionElement(
  const QAdvectionDiffusionReactionElement<NREAGENT,DIM,NNODE_1D> &dummy) 
  { 
   BrokenCopy::broken_copy("QAdvectionDiffusionReactionElement");
  } 
 
 /// Broken assignment operator
 void operator=(
  const QAdvectionDiffusionReactionElement<NREAGENT,DIM,NNODE_1D>&) 
  {
   BrokenCopy::broken_assign("QAdvectionDiffusionReactionElement");
  }
 
 /// \short  Required  # of `values' (pinned or dofs) 
 /// at node n
 inline unsigned required_nvalue(const unsigned &n) const {return NREAGENT;}
 
 /// \short Output function:  
 ///  x,y,u   or    x,y,z,u
 void output(std::ostream &outfile)
  {AdvectionDiffusionReactionEquations<NREAGENT,DIM>::output(outfile);}
 
 /// \short Output function:  
 ///  x,y,u   or    x,y,z,u at n_plot^DIM plot points
 void output(std::ostream &outfile, const unsigned &n_plot)
  {AdvectionDiffusionReactionEquations<NREAGENT,DIM>::output(outfile,n_plot);}


 /// \short C-style output function:  
 ///  x,y,u   or    x,y,z,u
 void output(FILE* file_pt)
  {
   AdvectionDiffusionReactionEquations<NREAGENT,DIM>::output(file_pt);
  }

 ///  \short C-style output function:  
 ///   x,y,u   or    x,y,z,u at n_plot^DIM plot points
 void output(FILE* file_pt, const unsigned &n_plot)
  {
   AdvectionDiffusionReactionEquations<NREAGENT,DIM>::output(file_pt,n_plot);
  }

 /// \short Output function for an exact solution:
 ///  x,y,u_exact   or    x,y,z,u_exact at n_plot^DIM plot points
 void output_fct(std::ostream &outfile, const unsigned &n_plot,
                 FiniteElement::SteadyExactSolutionFctPt 
                 exact_soln_pt)
  {
   AdvectionDiffusionReactionEquations<NREAGENT,DIM>::
    output_fct(outfile,n_plot,exact_soln_pt);}


 /// \short Output function for a time-dependent exact solution.
 ///  x,y,u_exact   or    x,y,z,u_exact at n_plot^DIM plot points
 /// (Calls the steady version)
 void output_fct(std::ostream &outfile, const unsigned &n_plot,
                 const double& time,
                 FiniteElement::UnsteadyExactSolutionFctPt 
                 exact_soln_pt)
  {
   AdvectionDiffusionReactionEquations<NREAGENT,DIM>::
    output_fct(outfile,n_plot,time,exact_soln_pt);
  }


protected:

 /// Shape, test functions & derivs. w.r.t. to global coords. Return Jacobian.
 inline double dshape_and_dtest_eulerian_adv_diff_react(
  const Vector<double> &s, 
  Shape &psi, 
  DShape &dpsidx, 
  Shape &test, 
  DShape &dtestdx) const;
 
 /// \short Shape, test functions & derivs. w.r.t. to global coords. at
 /// integration point ipt. Return Jacobian.
 inline double dshape_and_dtest_eulerian_at_knot_adv_diff_react(
  const unsigned& ipt,
  Shape &psi, 
  DShape &dpsidx, 
  Shape &test,
  DShape &dtestdx) 
  const;

};

//Inline functions:


//======================================================================
/// \short Define the shape functions and test functions and derivatives
/// w.r.t. global coordinates and return Jacobian of mapping.
///
/// Galerkin: Test functions = shape functions
//======================================================================
template<unsigned NREAGENT, unsigned DIM, unsigned NNODE_1D>
double QAdvectionDiffusionReactionElement<NREAGENT, DIM,NNODE_1D>::
 dshape_and_dtest_eulerian_adv_diff_react(const Vector<double> &s,
                                          Shape &psi, 
                                          DShape &dpsidx,
                                          Shape &test, 
                                    DShape &dtestdx) const
{
 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian(s,psi,dpsidx);

 //Loop over the test functions and derivatives and set them equal to the
 //shape functions
 for(unsigned i=0;i<NNODE_1D;i++)
  {
   test[i] = psi[i]; 
   for(unsigned j=0;j<DIM;j++)
    {
     dtestdx(i,j) = dpsidx(i,j);
    }
  }

 //Return the jacobian
 return J;
}



//======================================================================
/// Define the shape functions and test functions and derivatives
/// w.r.t. global coordinates and return Jacobian of mapping.
///
/// Galerkin: Test functions = shape functions
//======================================================================
template<unsigned NREAGENT, unsigned DIM, unsigned NNODE_1D>
double QAdvectionDiffusionReactionElement<NREAGENT,DIM,NNODE_1D>::
 dshape_and_dtest_eulerian_at_knot_adv_diff_react(
  const unsigned &ipt,
 Shape &psi, 
 DShape &dpsidx,
 Shape &test, 
 DShape &dtestdx) const
{
 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian_at_knot(ipt,psi,dpsidx);

 //Set the test functions equal to the shape functions (pointer copy)
 test = psi;
 dtestdx = dpsidx;

 //Return the jacobian
 return J;
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



//=======================================================================
/// \short Face geometry for the QAdvectionDiffusionReactionElement elements: 
/// The spatial dimension of the face elements is one lower than that 
/// of the bulk element but they have the same number of points along 
/// their 1D edges.
//=======================================================================
template<unsigned NREAGENT, unsigned DIM, unsigned NNODE_1D>
class FaceGeometry<
 QAdvectionDiffusionReactionElement<NREAGENT,DIM,NNODE_1D> >: 
public virtual QElement<DIM-1,NNODE_1D>
{
 
  public:
 
 /// \short Constructor: Call the constructor for the
 /// appropriate lower-dimensional QElement
 FaceGeometry() : QElement<DIM-1,NNODE_1D>() {}

};



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//=======================================================================
/// Face geometry for the 1D QAdvectionDiffusionReaction elements: Point elements
//=======================================================================
template<unsigned NREAGENT,unsigned NNODE_1D>
class FaceGeometry<QAdvectionDiffusionReactionElement<NREAGENT,1,NNODE_1D> >: 
 public virtual PointElement
{

  public:
 
 /// \short Constructor: Call the constructor for the
 /// appropriate lower-dimensional QElement
 FaceGeometry() : PointElement() {}

};

}

#endif