/*
 * Copyright (c) 2017, The Regents of the University of California (Regents).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *    3. Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Please contact the author(s) of this library if you have any questions.
 * Authors: David Fridovich-Keil   ( dfk@eecs.berkeley.edu )
 */

///////////////////////////////////////////////////////////////////////////////
//
// Defines the PlanarDubinsDynamics3D class, which uses the PlanarDubins3D
// state type.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef FASTRACK_DYNAMICS_PLANAR_DUBINS_DYNAMICS_3D_H
#define FASTRACK_DYNAMICS_PLANAR_DUBINS_DYNAMICS_3D_H

#include <fastrack/dynamics/dynamics.h>
#include <fastrack/state/planar_dubins_3d.h>

#include <math.h>

namespace fastrack {
namespace dynamics {

using state::PlanarDubins3D;

class PlanarDubinsDynamics3D
    : public Dynamics<PlanarDubins3D, double,
                      fastrack_srvs::PlanarDubinsPlannerDynamics::Response> {
 public:
  ~QuadrotorDecoupled6D() {}
  explicit QuadrotorDecoupled6D()
      : Dynamics<PlanarDubins3D, double,
                 fastrack_srvs::PlanarDubinsPlannerDynamics::Response>() {}
  explicit QuadrotorDecoupled6D(const double& u_lower, const double& u_upper)
      : Dynamics<PlanarDubins3D, double,
                 fastrack_srvs::PlanarDubinsPlannerDynamics::Response>(
            std::unique_ptr<ControlBound<double>>(
                new ScalarBoundInterval(u_lower, u_upper))) {}

  // Accessors.
  inline double V() const { return v_; }
  inline double MaxOmega() const { return max_omega_; }

  // Initialize by calling base class.
  void Initialize(std::unique_ptr<ControlBound<double>> bound) {
    Dynamics<PlanarDubins3D, double,
             fastrack_srvs::PlanarDubinsPlannerDynamics::Response>::
      Initialize(bound);
  }

  // Initialize from vector.
  bool Initialize(const std::vector<double>& bound_params) {
    if (bound_params.size() != 2) {
      ROS_ERROR("PlanarDubinsDynamics3D: bound params were incorrect size.");
      return false;
    }

    control_bound_.reset(new ScalarBoundInterval(bound_params));
    initialized_ = true;
    return true;
  }

  // Derived classes must be able to give the time derivative of state
  // as a function of current state and control.
  inline PositionVelocity Evaluate(const PlanarDubins3D& x,
                                   const double& u) const {
    const Vector3d x_dot(v_ * std::cos(x.Theta()), v_ * std::sin(x.Theta()), u);
    return x_dot;
  }

  // Derived classes must be able to compute an optimal control given
  // the gradient of the value function at the specified state.
  // In this case (linear dynamics), the state is irrelevant given the
  // gradient of the value function at that state.
  inline PlanarDubins3D OptimalControl(
      const PlanarDubins3D& x, const PlanarDubins3D& value_gradient) const {
    throw std::runtime_error(
        "PlanarDubinsDynamics3D: OptimalControl is unimplemented.");
  }

  // Convert to the appropriate service response type.
  inline fastrack_srvs::PlanarDubinsPlannerDynamics::Response ToRos() const {
    fastrack_srvs::PlanarDubinsPlannerDynamics::Response res;
    res.speed = v_;
    res.max_yaw_rate = max_omega_;

    return res;
  }

  // Convert from the appropriate service response type.
  inline void FromRos(
      const fastrack_srvs::PlanarDubinsPlannerDynamics::Response& res) {
    v_ = res.speed;
    max_omega_ = res.max_yaw_rate;
  }

 private:
  double v_;
  double max_omega_;
};  //\class QuadrotorDecoupled6D

}  // namespace dynamics
}  // namespace fastrack

#endif
