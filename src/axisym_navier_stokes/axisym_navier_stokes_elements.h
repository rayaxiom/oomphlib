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
//Header file for Navier Stokes elements

#ifndef OOMPH_AXISYMMETRIC_NAVIER_STOKES_ELEMENTS_HEADER
#define OOMPH_AXISYMMETRIC_NAVIER_STOKES_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

//OOMPH-LIB headers 
#include "../generic/Qelements.h"
#include "../generic/fsi.h"

namespace oomph
{


//======================================================================
/// A class for elements that solve the unsteady 
/// axisymmetric Navier--Stokes equations in 
/// cylindrical polar coordinates, \f$ x_0^* = r^*\f$ and \f$ x_1^* = z^*  \f$
/// with \f$ \partial / \partial \theta = 0 \f$. We're solving for the
/// radial, axial and azimuthal (swirl) velocities, 
/// \f$ u_0^* = u_r^*(r^*,z^*,t^*) = u^*(r^*,z^*,t^*), 
///  \ u_1^* = u_z^*(r^*,z^*,t^*) = w^*(r^*,z^*,t^*)\f$ and 
/// \f$ u_2^* = u_\theta^*(r^*,z^*,t^*) = v^*(r^*,z^*,t^*) \f$,
/// respectively, and the pressure \f$ p(r^*,z^*,t^*) \f$.
/// This class contains the generic maths -- any concrete 
/// implementation must be derived from this.
///
/// In dimensional form the axisymmetric Navier-Stokes equations are given
/// by the momentum equations (for the \f$ r^* \f$, \f$ z^* \f$ and \f$ \theta
/// \f$
/// directions, respectively)
/// \f[ 
/// \rho\left(\frac{\partial u^*}{\partial t^*} + {u^*}\frac{\partial
///   u^*}{\partial r^*} - \frac{{v^*}^2}{r^*}
///   + {w^*}\frac{\partial u^*}{\partial z^*} \right) =
///   B_r^*\left(r^*,z^*,t^*\right)+ \rho G_r^*+
///   \frac{1}{r^*}
///   \frac{\partial\left({r^*}\sigma_{rr}^*\right)}{\partial r^*}
///   - \frac{\sigma_{\theta\theta}^*}{r^*} +
///   \frac{\partial\sigma_{rz}^*}{\partial z^*},
/// \f] 
/// \f[ 
/// \rho\left(\frac{\partial w^*}{\partial t^*} + {u^*}\frac{\partial
///   w^*}{\partial r^*} + {w^*}\frac{\partial
///   w^*}{\partial z^*} \right) =
///   B_z^*\left(r^*,z^*,t^*\right)+\rho G_z^*+ 
///   \frac{1}{r^*}\frac{\partial\left({r^*}\sigma_{zr}^*\right)}{\partial
///   r^*} + \frac{\partial\sigma_{zz}^*}{\partial z^*},
/// \f] 
/// \f[ 
/// \rho\left(\frac{\partial v^*}{\partial t^*} +
///   {u^*}\frac{\partial v^*}{\partial r^*} +
///   \frac{u^* v^*}{r^*}
///   +{w^*}\frac{\partial v^*}{\partial z^*} \right)=
///   B_\theta^*\left(r^*,z^*,t^*\right)+ \rho G_\theta^*+
///   \frac{1}{r^*}\frac{\partial\left({r^*}\sigma_{\theta
///   r}^*\right)}{\partial r^*} + \frac{\sigma_{r\theta}^*}{r^*} +
///   \frac{\partial\sigma_{\theta z}^*}{\partial z^*},
/// \f] 
/// and
/// \f[ 
/// \frac{1}{r^*}\frac{\partial\left(r^*u^*\right)}{\partial r^*} +
/// \frac{\partial w^*}{\partial z^*} = Q^*.
/// \f] 
/// The dimensional, symmetric stress tensor is defined as:
/// \f[ 
/// \sigma_{rr}^* = -p^* + 2\mu\frac{\partial u^*}{\partial r^*}, 
/// \qquad
/// \sigma_{\theta\theta}^* = -p^* +2\mu\frac{u^*}{r^*},
/// \f] 
/// \f[ 
/// \sigma_{zz}^* = -p^* + 2\mu\frac{\partial w^*}{\partial z^*},
/// \qquad
/// \sigma_{rz}^* = \mu\left(\frac{\partial u^*}{\partial z^*} +
///                 \frac{\partial w^*}{\partial r^*}\right),
/// \f] 
/// \f[ 
/// \sigma_{\theta r}^* = \mu r^*\frac{\partial}{\partial r^*}
///                       \left(\frac{v^*}{r^*}\right),
/// \qquad
/// \sigma_{\theta z}^* = \mu\frac{\partial v^*}{\partial z^*}.
/// \f] 
/// Here, the (dimensional) velocity components are denoted 
/// by \f$ u^* \f$, \f$ w^* \f$
/// and \f$ v^* \f$ for the radial, axial and azimuthal velocities,
/// respectively, and we
/// have split the body force into two components: A constant
/// vector \f$ \rho \ G_i^* \f$ which typically represents gravitational
/// forces; and a variable body force, \f$ B_i^*(r^*,z^*,t^*) \f$.
/// \f$ Q^*(r^*,z^*,t^*)  \f$ is a volumetric source term for the 
/// continuity equation and is typically equal to zero. 
/// \n\n
/// We non-dimensionalise the equations, using problem-specific reference
/// quantities for the velocity, \f$ U \f$, length, \f$ L \f$, and time,
/// \f$ T \f$, and scale the constant body force vector on the 
/// gravitational acceleration, \f$ g \f$, so that
/// \f[ 
/// u^* = U\, u, \qquad
/// w^* = U\, w, \qquad
/// v^* = U\, v, 
/// \f] 
/// \f[ 
/// r^* = L\, r, \qquad
/// z^* = L\, z, \qquad
/// t^* = T\, t, 
/// \f] 
/// \f[ 
/// G_i^* = g\, G_i, \qquad
/// B_i^* = \frac{U\mu_{ref}}{L^2}\, B_i, \qquad
/// p^* = \frac{\mu_{ref} U}{L}\, p, \qquad
/// Q^* = \frac{U}{L}\, Q.
/// \f] 
/// where we note that the pressure and the variable body force have
/// been non-dimensionalised on the viscous scale. \f$ \mu_{ref} \f$
/// and \f$ \rho_{ref} \f$ (used below) are reference values 
/// for the fluid viscosity and density, respectively. In single-fluid
/// problems, they are identical to the viscosity \f$ \mu \f$ and 
/// density \f$ \rho \f$ of the (one and only) fluid in the problem.
/// \n\n
/// The non-dimensional form of the axisymmetric Navier-Stokes equations
/// is then given by
/// \f[ 
/// R_{\rho} Re\left(St\frac{\partial u}{\partial t} + {u}\frac{\partial
///   u}{\partial r} - \frac{{v}^2}{r}
///   + {w}\frac{\partial u}{\partial z} \right) =
///   B_r\left(r,z,t\right)+  R_\rho \frac{Re}{Fr} G_r +
///   \frac{1}{r}
///   \frac{\partial\left({r}\sigma_{rr}\right)}{\partial r}
///   - \frac{\sigma_{\theta\theta}}{r} +
///   \frac{\partial\sigma_{rz}}{\partial z},
/// \f] 
/// \f[ 
/// R_{\rho} Re\left(St\frac{\partial w}{\partial t} + {u}\frac{\partial
///   w}{\partial r} + {w}\frac{\partial
///   w}{\partial z} \right) =
///    B_z\left(r,z,t\right)+ R_\rho \frac{Re}{Fr} G_z+ 
///   \frac{1}{r}\frac{\partial\left({r}\sigma_{zr}\right)}{\partial
///   r} + \frac{\partial\sigma_{zz}}{\partial z},
/// \f] 
/// \f[ 
/// R_{\rho} Re\left(St\frac{\partial v}{\partial t} +
///   {u}\frac{\partial v}{\partial r} +
///   \frac{u v}{r}
///   +{w}\frac{\partial v}{\partial z} \right)=
///   B_\theta\left(r,z,t\right)+  R_\rho \frac{Re}{Fr} G_\theta+
///   \frac{1}{r}\frac{\partial\left({r}\sigma_{\theta
///   r}\right)}{\partial r} + \frac{\sigma_{r\theta}}{r} +
///   \frac{\partial\sigma_{\theta z}}{\partial z},
/// \f] 
/// and
/// \f[ 
/// \frac{1}{r}\frac{\partial\left(ru\right)}{\partial r} +
/// \frac{\partial w}{\partial z} = Q.
/// \f] 
/// Here the non-dimensional, symmetric stress tensor is defined as:
/// \f[ 
/// \sigma_{rr} = -p + 2R_\mu \frac{\partial u}{\partial r}, 
/// \qquad
/// \sigma_{\theta\theta} = -p +2R_\mu \frac{u}{r},
/// \f] 
/// \f[ 
/// \sigma_{zz} = -p + 2R_\mu \frac{\partial w}{\partial z},
/// \qquad
/// \sigma_{rz} = R_\mu \left(\frac{\partial u}{\partial z} +
///                 \frac{\partial w}{\partial r}\right), 
/// \f] 
/// \f[ 
/// \sigma_{\theta r} = R_\mu r
///       \frac{\partial}{\partial r}\left(\frac{v}{r}\right),
/// \qquad
/// \sigma_{\theta z} = R_\mu \frac{\partial v}{\partial z}.
/// \f] 
/// and the dimensionless parameters
/// \f[ 
/// Re = \frac{UL\rho_{ref}}{\mu_{ref}}, \qquad
/// St = \frac{L}{UT}, \qquad
/// Fr = \frac{U^2}{gL},
/// \f] 
/// are the Reynolds number, Strouhal number and Froude number
/// respectively. \f$ R_\rho=\rho/\rho_{ref} \f$ and 
/// \f$ R_\mu =\mu/\mu_{ref}\f$ represent the ratios
/// of the fluid's density and its dynamic viscosity, relative to the
/// density and viscosity values used to form the non-dimensional
/// parameters (By default, \f$ R_\rho  = R_\mu = 1 \f$; other values
/// tend to be used in problems involving multiple fluids). 
//======================================================================
class AxisymmetricNavierStokesEquations : public virtual FiniteElement
{
  private:

 /// \short Static "magic" number that indicates that the pressure is
 /// not stored at a node
 static int Pressure_not_stored_at_node;

 /// Static default value for the physical constants (all initialised to zero)
 static double Default_Physical_Constant_Value;

 /// Static default value for the physical ratios (all are initialised to one)
 static double Default_Physical_Ratio_Value;

 /// Static default value for the gravity vector
 static Vector<double> Default_Gravity_vector; 

protected:
 
 //Physical constants

 /// \short Pointer to the viscosity ratio (relative to the 
 /// viscosity used in the definition of the Reynolds number)
 double *Viscosity_Ratio_pt;
 
 /// \short Pointer to the density ratio (relative to the density used in the 
 /// definition of the Reynolds number)
 double *Density_Ratio_pt;
 
 // Pointers to global physical constants

 /// Pointer to global Reynolds number
 double *Re_pt;
 
 /// Pointer to global Reynolds number x Strouhal number (=Womersley)
 double *ReSt_pt;
 
 /// \short Pointer to global Reynolds number x inverse Froude number
 /// (= Bond number / Capillary number) 
 double *ReInvFr_pt;
 
 /// Pointer to global gravity Vector
 Vector<double> *G_pt;
 
 /// Pointer to body force function
 void (*Body_force_fct_pt)(const double& time, const Vector<double> &x, 
                           Vector<double> &result);
 
 /// Pointer to volumetric source function
 double (*Source_fct_pt)(const double& time, const Vector<double> &x);

 /// \short Boolean flag to indicate if ALE formulation is disabled when
 /// the time-derivatives are computed. Only set to true if you're sure
 /// that the mesh is stationary
 bool ALE_is_disabled;

 /// \short Access function for the local equation number information for
 /// the pressure.
 /// p_local_eqn[n] = local equation number or < 0 if pinned
 virtual int p_local_eqn(const unsigned &n)=0;

 /// \short Compute the shape functions and derivatives 
 /// w.r.t. global coords at local coordinate s.
 /// Return Jacobian of mapping between local and global coordinates.
 virtual double dshape_and_dtest_eulerian_axi_nst(const Vector<double> &s, 
                                                  Shape &psi, 
                                                  DShape &dpsidx, Shape &test, 
                                                  DShape &dtestdx) const=0;

 /// \short Compute the shape functions and derivatives 
 /// w.r.t. global coords at ipt-th integration point
 /// Return Jacobian of mapping between local and global coordinates.
 virtual double dshape_and_dtest_eulerian_at_knot_axi_nst(const unsigned &ipt, 
                                                          Shape &psi, 
                                                          DShape &dpsidx, 
                                                          Shape &test, 
                                                          DShape &dtestdx) 
  const=0;
 
 /// Compute the pressure shape functions at local coordinate s
 virtual void pshape_axi_nst(const Vector<double> &s, Shape &psi) const=0;

 /// \short Compute the pressure shape and test functions 
 /// at local coordinate s
 virtual void pshape_axi_nst(const Vector<double> &s, Shape &psi, 
                             Shape &test) const=0;

 /// Calculate the body force at a given time and Eulerian position
 void get_body_force(const double& time, const Vector<double> &x, 
                     Vector<double> &result)
  {
   //If the function pointer is zero return zero
   if(Body_force_fct_pt == 0)
    {
     //Loop over dimensions and set body forces to zero
     for(unsigned i=0;i<3;i++) {result[i] = 0.0;}
    }
   //Otherwise call the function
   else
    {
     (*Body_force_fct_pt)(time,x,result);
    }
  }

 /// \short Calculate the source fct at given time and
 /// Eulerian position 
 double get_source_fct(const double& time, const Vector<double> &x)
  {
   //If the function pointer is zero return zero
   if (Source_fct_pt == 0)
    {
     return 0;
    }
   //Otherwise call the function
   else
    {
     return (*Source_fct_pt)(time,x);
    }
  }
 
 ///\short Compute the residuals for the Navier--Stokes equations; 
 /// flag=1(or 0): do (or don't) compute the Jacobian as well. 
 virtual void fill_in_generic_residual_contribution_axi_nst(
  Vector<double> &residuals, DenseMatrix<double> &jacobian, 
  DenseMatrix<double> &mass_matrix, unsigned flag);
    
public:

 /// \short Constructor: NULL the body force and source function
 AxisymmetricNavierStokesEquations() : Body_force_fct_pt(0), Source_fct_pt(0),
  ALE_is_disabled(false)
  {
   //Set all the Physical parameter pointers to the default value zero
   Re_pt = &Default_Physical_Constant_Value;
   ReSt_pt = &Default_Physical_Constant_Value;
   ReInvFr_pt = &Default_Physical_Constant_Value;
   G_pt = &Default_Gravity_vector;
   //Set the Physical ratios to the default value of 1
   Viscosity_Ratio_pt = &Default_Physical_Ratio_Value;
   Density_Ratio_pt = &Default_Physical_Ratio_Value;
  }

 /// Vector to decide whether the stress-divergence form is used or not
 // N.B. This needs to be public so that the intel compiler gets things correct
 // somehow the access function messes things up when going to refineable
 // navier--stokes
 static Vector<double> Gamma;

 //Access functions for the physical constants

 /// Reynolds number
 const double &re() const {return *Re_pt;}

 /// Product of Reynolds and Strouhal number (=Womersley number)
 const double &re_st() const {return *ReSt_pt;}

 /// Pointer to Reynolds number
 double* &re_pt() {return Re_pt;}

 /// Pointer to product of Reynolds and Strouhal number (=Womersley number)
 double* &re_st_pt() {return ReSt_pt;}

 /// \short Viscosity ratio for element: Element's viscosity relative to the 
 /// viscosity used in the definition of the Reynolds number
 const double &viscosity_ratio() const {return *Viscosity_Ratio_pt;}

 /// Pointer to Viscosity Ratio
 double* &viscosity_ratio_pt() {return Viscosity_Ratio_pt;}

 /// \short Density ratio for element: Element's density relative to the 
 ///  viscosity used in the definition of the Reynolds number
 const double &density_ratio() const {return *Density_Ratio_pt;}

 /// Pointer to Density ratio
 double* &density_ratio_pt() {return Density_Ratio_pt;}

 /// Global inverse Froude number
 const double &re_invfr() const {return *ReInvFr_pt;}

 /// Pointer to global inverse Froude number
 double* &re_invfr_pt() {return ReInvFr_pt;}
 
 /// Vector of gravitational components
 const Vector<double> &g() const {return *G_pt;}

 /// Pointer to Vector of gravitational components
 Vector<double>* &g_pt() {return G_pt;}

 /// Access function for the body-force pointer
 void (* &body_force_fct_pt())(const double& time, const Vector<double>& x, 
                               Vector<double> & f) 
  {return Body_force_fct_pt;}
 
 ///Access function for the source-function pointer
 double (* &source_fct_pt())(const double& time, const Vector<double>& x)
  {return Source_fct_pt;}
 
 ///Function to return number of pressure degrees of freedom
 virtual unsigned npres_axi_nst() const=0;
 
   /// \short Return the index at which the i-th unknown velocity component
 /// is stored. The default value, i, is appropriate for single-physics
 /// problems.
 /// In derived multi-physics elements, this function should be overloaded
 /// to reflect the chosen storage scheme. Note that these equations require
 /// that the unknowns are always stored at the same indices at each node.
 virtual inline unsigned u_index_axi_nst(const unsigned &i) const {return i;}
 
 /// \short i-th component of du/dt at local node n. 
 /// Uses suitably interpolated value for hanging nodes.
 double du_dt_axi_nst(const unsigned &n, const unsigned &i) const
  {
   // Get the data's timestepper
   TimeStepper* time_stepper_pt = this->node_pt(n)->time_stepper_pt();

   //Initialise dudt
   double dudt=0.0;
   //Loop over the timesteps, if there is a non Steady timestepper
   if (!time_stepper_pt->is_steady())
    {
     //Get the index at which the velocity is stored
     const unsigned u_nodal_index = u_index_axi_nst(i);
     
     // Number of timsteps (past & present)
     const unsigned n_time = time_stepper_pt->ntstorage();

     //Add the contributions to the time derivative
     for(unsigned t=0;t<n_time;t++)
      {
       dudt+=time_stepper_pt->weight(1,t)*nodal_value(t,n,u_nodal_index);
      }
    }
   
   return dudt;
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

 /// \short Pressure at local pressure "node" n_p
 /// Uses suitably interpolated value for hanging nodes.
 virtual double p_axi_nst(const unsigned &n_p)const=0; 

 /// \short Which nodal value represents the pressure?
 virtual int p_nodal_index_axi_nst() const
  {return Pressure_not_stored_at_node;}

 /// Integral of pressure over element
 double pressure_integral() const;

 /// \short Return integral of dissipation over element
 double dissipation() const;
 
 /// \short Return dissipation at local coordinate s
 double dissipation(const Vector<double>& s) const;

/// \short Get integral of kinetic energy over element
 double kin_energy() const;

 /// \short Strain-rate tensor: \f$ e_{ij} \f$  where \f$ i,j = r,z,\theta \f$
 /// (in that order)
 void strain_rate(const Vector<double>& s, 
                  DenseMatrix<double>& strain_rate) const;
 
 /// \short Compute traction (on the viscous scale) at local coordinate s 
 /// for outer unit normal N
 void traction(const Vector<double>& s, const Vector<double>& N, 
               Vector<double>& traction);

 /// \short Output function: x,y,[z],u,v,[w],p
 /// in tecplot format. Default number of plot points
 void output(std::ostream &outfile)
  {
   unsigned nplot=5;
   output(outfile,nplot);
  }

 /// \short Output function: x,y,[z],u,v,[w],p
 /// in tecplot format. nplot points in each coordinate direction
 void output(std::ostream &outfile, const unsigned &nplot);


 /// \short Output function: x,y,[z],u,v,[w],p
 /// in tecplot format. Default number of plot points
 void output(FILE* file_pt)
  {
   unsigned nplot=5;
   output(file_pt,nplot);
  }

 /// \short Output function: x,y,[z],u,v,[w],p
 /// in tecplot format. nplot points in each coordinate direction
 void output(FILE* file_pt, const unsigned &nplot);

 /// \short Output function: x,y,[z],u,v,[w] in tecplot format.
 /// nplot points in each coordinate direction at timestep t
 /// (t=0: present; t>0: previous timestep)
 void output_veloc(std::ostream &outfile, const unsigned &nplot, 
                   const unsigned& t);

 /// \short Output exact solution specified via function pointer
 /// at a given number of plot points. Function prints as
 /// many components as are returned in solution Vector
 void output_fct(std::ostream &outfile, const unsigned &nplot, 
                 FiniteElement::SteadyExactSolutionFctPt exact_soln_pt);

 /// \short Output exact solution specified via function pointer
 /// at a given time and at a given number of plot points.
 /// Function prints as many components as are returned in solution Vector.
 void output_fct(std::ostream &outfile, const unsigned &nplot, 
                 const double& time,
                 FiniteElement::UnsteadyExactSolutionFctPt exact_soln_pt);

 /// \short Validate against exact solution at given time
 /// Solution is provided via function pointer.
 /// Plot at a given number of plot points and compute L2 error
 /// and L2 norm of velocity solution over element
 void compute_error(std::ostream &outfile,
                    FiniteElement::UnsteadyExactSolutionFctPt exact_soln_pt,
                    const double& time,
                    double& error, double& norm);

 /// \short Validate against exact solution.
 /// Solution is provided via function pointer.
 /// Plot at a given number of plot points and compute L2 error
 /// and L2 norm of velocity solution over element
 void compute_error(std::ostream &outfile,
                    FiniteElement::SteadyExactSolutionFctPt exact_soln_pt,
                    double& error, double& norm);

 /// Compute the element's residual Vector
 void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
   //Call the generic residuals function with flag set to 0
   //and using a dummy matrix argument
   fill_in_generic_residual_contribution_axi_nst(
    residuals,GeneralisedElement::Dummy_matrix,
    GeneralisedElement::Dummy_matrix,0);
  }

 ///\short Compute the element's residual Vector and the jacobian matrix
 /// Virtual function can be overloaded by hanging-node version
 void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                   DenseMatrix<double> &jacobian)
  {
   //Call the generic routine with the flag set to 1
   fill_in_generic_residual_contribution_axi_nst(
    residuals,jacobian,GeneralisedElement::Dummy_matrix,1);
  }
 
 /// Add the element's contribution to its residuals vector,
 /// jacobian matrix and mass matrix
 void fill_in_contribution_to_jacobian_and_mass_matrix(
  Vector<double> &residuals, DenseMatrix<double> &jacobian, 
  DenseMatrix<double> &mass_matrix)
  {
   //Call the generic routine with the flag set to 2
   fill_in_generic_residual_contribution_axi_nst(
    residuals,jacobian,mass_matrix,2);
  }

 /// Return FE interpolated velocity u[i] at local coordinate s
 double interpolated_u_axi_nst(const Vector<double> &s, 
                               const unsigned &i) const
  {
   //Find number of nodes
   unsigned n_node = nnode();
   //Local shape function
   Shape psi(n_node);
   //Find values of shape function
   shape(s,psi);
   
   //Get the index at which the velocity is stored
   unsigned u_nodal_index = u_index_axi_nst(i);

   //Initialise value of u
   double interpolated_u = 0.0;
   //Loop over the local nodes and sum
   for(unsigned l=0;l<n_node;l++) 
    {
     interpolated_u += nodal_value(l,u_nodal_index)*psi[l];
    }
   
   return(interpolated_u);
  }

 /// Return FE interpolated pressure at local coordinate s
 double interpolated_p_axi_nst(const Vector<double> &s) const
  {
   //Find number of nodes
   unsigned n_pres = npres_axi_nst();
   //Local shape function
   Shape psi(n_pres);
   //Find values of shape function
   pshape_axi_nst(s,psi);
   
   //Initialise value of p
   double interpolated_p = 0.0;
   //Loop over the local nodes and sum
   for(unsigned l=0;l<n_pres;l++) 
    {
     interpolated_p += p_axi_nst(l)*psi[l];
    }
   
   return(interpolated_p);
  }

}; 

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//==========================================================================
///Crouzeix_Raviart elements are Navier--Stokes elements with quadratic
///interpolation for velocities and positions, but a discontinuous linear
///pressure interpolation
//==========================================================================
class AxisymmetricQCrouzeixRaviartElement : public virtual QElement<2,3>, 
 public virtual AxisymmetricNavierStokesEquations
{
  private:

 /// Static array of ints to hold required number of variables at nodes
 static const unsigned Initial_Nvalue[];
 
  protected:
 
 /// Internal index that indicates at which internal data the pressure is
 /// stored
 unsigned P_axi_nst_internal_index;
 
 /// \short Velocity shape and test functions and their derivs 
 /// w.r.t. to global coords  at local coordinate s (taken from geometry)
 ///Return Jacobian of mapping between local and global coordinates.
 inline double dshape_and_dtest_eulerian_axi_nst(const Vector<double> &s, 
                                                 Shape &psi, 
                                                 DShape &dpsidx, Shape &test, 
                                                 DShape &dtestdx) const;

 /// \short Velocity shape and test functions and their derivs 
 /// w.r.t. to global coords at ipt-th integation point (taken from geometry)
 ///Return Jacobian of mapping between local and global coordinates.
 inline double dshape_and_dtest_eulerian_at_knot_axi_nst(const unsigned &ipt, 
                                                         Shape &psi, 
                                                         DShape &dpsidx, 
                                                         Shape &test, 
                                                         DShape &dtestdx) const;

 /// Pressure shape functions at local coordinate s
 inline void pshape_axi_nst(const Vector<double> &s, Shape &psi) const;

 /// Pressure shape and test functions at local coordinte s
 inline void pshape_axi_nst(const Vector<double> &s, Shape &psi, 
                            Shape &test) const;

public:

 /// Constructor, there are three internal values (for the pressure)
 AxisymmetricQCrouzeixRaviartElement() : QElement<2,3>(), 
  AxisymmetricNavierStokesEquations()
  {
   //Allocate and add one Internal data object that stores the three
   //pressure values
   P_axi_nst_internal_index = this->add_internal_data(new Data(3));
  }
 
 /// \short Number of values (pinned or dofs) required at local node n. 
 virtual unsigned required_nvalue(const unsigned &n) const;

 /// \short Return the pressure values at internal dof i_internal
 /// (Discontinous pressure interpolation -- no need to cater for hanging 
 /// nodes). 
 double p_axi_nst(const unsigned &i) const
  {return internal_data_pt(P_axi_nst_internal_index)->value(i);}

 /// Return number of pressure values
 unsigned npres_axi_nst() const {return 3;} 

 ///Function to fix the internal pressure dof idof_internal 
 void fix_pressure(const unsigned &p_dof, const double &pvalue)
  {
   this->internal_data_pt(P_axi_nst_internal_index)->pin(p_dof);
   internal_data_pt(P_axi_nst_internal_index)->set_value(p_dof,pvalue);
  }

 /// \short Compute traction at local coordinate s for outer unit normal N
 void get_traction(const Vector<double>& s, const Vector<double>& N,
                   Vector<double>& traction);

 /// \short Overload the access function for the pressure's local
 /// equation numbers
 inline int p_local_eqn(const unsigned &n) 
  {return internal_local_eqn(P_axi_nst_internal_index,n);}

 /// Redirect output to NavierStokesEquations output
 void output(std::ostream &outfile) 
  {AxisymmetricNavierStokesEquations::output(outfile);}

 /// Redirect output to NavierStokesEquations output
 void output(std::ostream &outfile, const unsigned &n_plot)
  {AxisymmetricNavierStokesEquations::output(outfile,n_plot);}


 /// Redirect output to NavierStokesEquations output
 void output(FILE* file_pt) 
  {AxisymmetricNavierStokesEquations::output(file_pt);}

 /// Redirect output to NavierStokesEquations output
 void output(FILE* file_pt, const unsigned &n_plot)
  {AxisymmetricNavierStokesEquations::output(file_pt,n_plot);}

};

//Inline functions

//=======================================================================
/// Derivatives of the shape functions and test functions w.r.t. to global
/// (Eulerian) coordinates. Return Jacobian of mapping between
/// local and global coordinates.
//=======================================================================
inline double AxisymmetricQCrouzeixRaviartElement::
dshape_and_dtest_eulerian_axi_nst( const Vector<double> &s, Shape &psi, 
                                   DShape &dpsidx, Shape &test, 
                                   DShape &dtestdx) const
{
 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian(s,psi,dpsidx);
 //Loop over the test functions and derivatives and set them equal to the
 //shape functions
 for(unsigned i=0;i<9;i++)
  {
   test[i] = psi[i]; 
   dtestdx(i,0) = dpsidx(i,0);
   dtestdx(i,1) = dpsidx(i,1);
  }
 //Return the jacobian
 return J;
}

//=======================================================================
/// Derivatives of the shape functions and test functions w.r.t. to global
/// (Eulerian) coordinates. Return Jacobian of mapping between
/// local and global coordinates.
//=======================================================================
inline double AxisymmetricQCrouzeixRaviartElement::
dshape_and_dtest_eulerian_at_knot_axi_nst(const unsigned &ipt, Shape &psi, 
                                          DShape &dpsidx, Shape &test, 
                                          DShape &dtestdx) const
{
 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian_at_knot(ipt,psi,dpsidx);
 //Loop over the test functions and derivatives and set them equal to the
 //shape functions
 for(unsigned i=0;i<9;i++)
  {
   test[i] = psi[i]; 
   dtestdx(i,0) = dpsidx(i,0);
   dtestdx(i,1) = dpsidx(i,1);
  }
 //Return the jacobian
 return J;
}


//=======================================================================
/// Pressure shape functions
//=======================================================================
inline void AxisymmetricQCrouzeixRaviartElement::
pshape_axi_nst(const Vector<double> &s, Shape &psi)
 const
{
 psi[0] = 1.0;
 psi[1] = s[0];
 psi[2] = s[1];
}

///Define the pressure shape and test functions
inline void AxisymmetricQCrouzeixRaviartElement::
pshape_axi_nst(const Vector<double> &s, Shape &psi, Shape &test) const
{
 //Call the pressure shape functions
 pshape_axi_nst(s,psi);
 //Loop over the test functions and set them equal to the shape functions
 for(unsigned i=0;i<3;i++) test[i] = psi[i];
}

//=======================================================================
/// Face geometry of the Axisymmetric Crouzeix_Raviart elements
//=======================================================================
template<>
class FaceGeometry<AxisymmetricQCrouzeixRaviartElement>: 
public virtual QElement<1,3>
{
  public:
 FaceGeometry() : QElement<1,3>() {}
};

//=======================================================================
/// Face geometry of face geometry of the Axisymmetric Crouzeix_Raviart elements
//=======================================================================
template<>
class FaceGeometry<FaceGeometry<AxisymmetricQCrouzeixRaviartElement> >: 
public virtual PointElement
{
  public:
 FaceGeometry() : PointElement() {}
};


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

//=======================================================================
/// Taylor--Hood elements are Navier--Stokes elements 
/// with quadratic interpolation for velocities and positions and 
/// continous linear pressure interpolation
//=======================================================================
class AxisymmetricQTaylorHoodElement : 
public virtual QElement<2,3>, 
public virtual AxisymmetricNavierStokesEquations
{
  private:
 
 /// Static array of ints to hold number of variables at node
 static const unsigned Initial_Nvalue[];
 
  protected:

 /// \short Static array of ints to hold conversion from pressure 
 /// node numbers to actual node numbers
 static const unsigned Pconv[];
 
 /// \short Velocity shape and test functions and their derivs 
 /// w.r.t. to global coords  at local coordinate s (taken from geometry)
 /// Return Jacobian of mapping between local and global coordinates.
 inline double dshape_and_dtest_eulerian_axi_nst(const Vector<double> &s, 
                                                 Shape &psi, 
                                                 DShape &dpsidx, Shape &test, 
                                                 DShape &dtestdx) const;
 
 /// \short Velocity shape and test functions and their derivs 
 /// w.r.t. to global coords  at local coordinate s (taken from geometry)
 /// Return Jacobian of mapping between local and global coordinates.
 inline double dshape_and_dtest_eulerian_at_knot_axi_nst(const unsigned &ipt, 
                                                         Shape &psi, 
                                                         DShape &dpsidx, 
                                                         Shape &test, 
                                                         DShape &dtestdx) const;
 
 /// Pressure shape functions at local coordinate s
 inline void pshape_axi_nst(const Vector<double> &s, Shape &psi) const;
 
 /// Pressure shape and test functions at local coordinte s
 inline void pshape_axi_nst(const Vector<double> &s, Shape &psi, 
                            Shape &test) const;
 
  public:
 
 /// Constructor, no internal data points
 AxisymmetricQTaylorHoodElement() : QElement<2,3>(),  
  AxisymmetricNavierStokesEquations() {}
 
 /// \short Number of values (pinned or dofs) required at node n. Can
 /// be overwritten for hanging node version
 inline virtual unsigned required_nvalue(const unsigned &n) const 
  {return Initial_Nvalue[n];}

 /// \short Which nodal value represents the pressure? 
 virtual int p_nodal_index_axi_nst() const {return 3;}
 
 /// \short Access function for the pressure values at local pressure 
 /// node n_p (const version)
 double p_axi_nst(const unsigned &n_p) const
  {return nodal_value(Pconv[n_p],p_nodal_index_axi_nst());}
 
 /// Return number of pressure values
 unsigned npres_axi_nst() const {return 4;}
 
 /// Fix the pressure at local pressure node n_p 
 void fix_pressure(const unsigned &n_p, const double &pvalue)
  {
   this->node_pt(Pconv[n_p])->pin(p_nodal_index_axi_nst());
   this->node_pt(Pconv[n_p])->set_value(p_nodal_index_axi_nst(),pvalue);
  }

 /// \short Compute traction at local coordinate s for outer unit normal N
 void get_traction(const Vector<double>& s, const Vector<double>& N,
                   Vector<double>& traction);

  /// \short Overload the access function for the pressure's local
 /// equation numbers
 inline int p_local_eqn(const unsigned &n) 
  {return nodal_local_eqn(Pconv[n],p_nodal_index_axi_nst());}
 
 /// Redirect output to NavierStokesEquations output
 void output(std::ostream &outfile) 
  {AxisymmetricNavierStokesEquations::output(outfile);}

 /// Redirect output to NavierStokesEquations output
 void output(std::ostream &outfile, const unsigned &n_plot)
  {AxisymmetricNavierStokesEquations::output(outfile,n_plot);}

 /// Redirect output to NavierStokesEquations output
 void output(FILE* file_pt) 
  {AxisymmetricNavierStokesEquations::output(file_pt);}

 /// Redirect output to NavierStokesEquations output
 void output(FILE* file_pt, const unsigned &n_plot)
  {AxisymmetricNavierStokesEquations::output(file_pt,n_plot);}

};

//Inline functions

//==========================================================================
/// Derivatives of the shape functions and test functions w.r.t to
/// global (Eulerian) coordinates. Return Jacobian of mapping between
/// local and global coordinates.
//==========================================================================
inline double AxisymmetricQTaylorHoodElement::
dshape_and_dtest_eulerian_axi_nst( const Vector<double> &s,
                                   Shape &psi, 
                                   DShape &dpsidx, Shape &test, 
                                   DShape &dtestdx) const
{
 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian(s,psi,dpsidx);
 //Loop over the test functions and derivatives and set them equal to the
 //shape functions
 for(unsigned i=0;i<9;i++)
  {
   test[i] = psi[i]; 
   dtestdx(i,0) = dpsidx(i,0);
   dtestdx(i,1) = dpsidx(i,1);
  }
 //Return the jacobian
 return J;
}

//==========================================================================
/// Derivatives of the shape functions and test functions w.r.t to
/// global (Eulerian) coordinates. Return Jacobian of mapping between
/// local and global coordinates.
//==========================================================================
inline double AxisymmetricQTaylorHoodElement::
dshape_and_dtest_eulerian_at_knot_axi_nst(const unsigned &ipt,
                                          Shape &psi, DShape &dpsidx, 
                                          Shape &test, 
                                          DShape &dtestdx) const
{
 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian_at_knot(ipt,psi,dpsidx);
 //Loop over the test functions and derivatives and set them equal to the
 //shape functions
 for(unsigned i=0;i<9;i++)
  {
   test[i] = psi[i]; 
   dtestdx(i,0) = dpsidx(i,0);
   dtestdx(i,1) = dpsidx(i,1);
  }
 //Return the jacobian
 return J;
}

//==========================================================================
///Pressure shape functions
//==========================================================================
inline void AxisymmetricQTaylorHoodElement::
pshape_axi_nst(const Vector<double> &s, Shape &psi)
 const
{
 //Local storage
 double psi1[2], psi2[2];
 //Call the OneDimensional Shape functions
 OneDimLagrange::shape<2>(s[0],psi1);
 OneDimLagrange::shape<2>(s[1],psi2);

 //Now let's loop over the nodal points in the element
 //s1 is the "x" coordinate, s2 the "y" 
 for(unsigned i=0;i<2;i++)
  {
   for(unsigned j=0;j<2;j++)
    {
     /*Multiply the two 1D functions together to get the 2D function*/
     psi[2*i + j] = psi2[i]*psi1[j];
    }
  }
}

//==========================================================================
/// Pressure shape and test functions
//==========================================================================
inline void AxisymmetricQTaylorHoodElement::
pshape_axi_nst(const Vector<double> &s, Shape &psi, Shape &test) const
{
 //Call the pressure shape functions
 pshape_axi_nst(s,psi);
 //Loop over the test functions and set them equal to the shape functions
 for(unsigned i=0;i<4;i++) test[i] = psi[i];
}

//=======================================================================
/// Face geometry of the Axisymmetric Taylor_Hood elements
//=======================================================================
template<>
class FaceGeometry<AxisymmetricQTaylorHoodElement>: public virtual QElement<1,3>
{
  public:
 FaceGeometry() : QElement<1,3>() {}
};

//=======================================================================
/// Face geometry of the face geometry of the Axisymmetric Taylor_Hood elements
//=======================================================================
template<>
class FaceGeometry<FaceGeometry<AxisymmetricQTaylorHoodElement> > : 
public virtual PointElement
{
  public:
 FaceGeometry() : PointElement() {}
};


}

#endif