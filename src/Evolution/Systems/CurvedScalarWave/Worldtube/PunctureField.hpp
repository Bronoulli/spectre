// Distributed under the MIT License.
// See LICENSE.txt for details.

#pragma once

#include <cstddef>

#include "DataStructures/DataBox/Prefixes.hpp"
#include "DataStructures/DataVector.hpp"
#include "DataStructures/Tensor/Tensor.hpp"
#include "DataStructures/Variables.hpp"
#include "Evolution/Systems/CurvedScalarWave/Tags.hpp"
#include "NumericalAlgorithms/LinearOperators/PartialDerivatives.hpp"
#include "Utilities/Gsl.hpp"

namespace CurvedScalarWave::Worldtube {
/*!
 * \brief Computes the puncture/singular field \f$\Psi^\mathcal{P}\f$ of a
 * scalar charge on a generic orbit in Schwarzschild spacetime.
 * described in \cite Detweiler2003.
 *
 * \details The field is computed using a Detweiler-Whiting singular
 * Green's function and perturbatively expanded in the geodesic distance from
 * the particle. It solves the inhomogeneous wave equation
 *
 * \f{align*}{
 * \Box \Psi^\mathcal{P} = -4 \pi q \int \sqrt{-g} \delta^4(x^i, z(\tau)) d \tau
 * \f}
 *
 * where \f$q\f$ is the scalar charge and \f$z(\tau)\f$ is the worldline of the
 * particle. The expression is expanded up to a certain order in geodesic
 * distance and transformed to Kerr-Schild coordinates.
 *
 * The function given here assumes that the particle has scalar charge \f$q=1\f$
 * and is on a fixed geodesic orbit. It returns the
 * singular field at the requested coordinates as well as its time and spatial
 * derivative. For non-geodesic orbits, corresponding acceleration terms have to
 * be added to the puncture field.
 *
 * \note The expressions were computed with Mathematica and optimized by
 * applying common subexpression elimination with sympy. The memory allocations
 * of temporaries were optimized manually.
 */
void puncture_field(
    gsl::not_null<Variables<tmpl::list<
        CurvedScalarWave::Tags::Psi, ::Tags::dt<CurvedScalarWave::Tags::Psi>,
        ::Tags::deriv<CurvedScalarWave::Tags::Psi, tmpl::size_t<3>,
                      Frame::Inertial>>>*>
        result,

    const tnsr::I<DataVector, 3, Frame::Inertial>& centered_coords,
    const tnsr::I<double, 3>& particle_position,
    const tnsr::I<double, 3>& particle_velocity,
    const tnsr::I<double, 3>& particle_acceleration, double bh_mass,
    size_t order);

/*!
 * \brief Computes the puncture/singular field \f$\Psi^\mathcal{P}\f$ of a
 * scalar charge on a generic orbit in Schwarzschild spacetime.
 * described in \cite Detweiler2003.
 *
 * \details The appropriate expression can be found in Eq. (36) of
 * \cite Wittek:2024gxn. For non-geodesic orbits, there are
 * additional contributions, see `acceleration_terms_0`.
 */
void puncture_field_0(
    gsl::not_null<Variables<tmpl::list<
        CurvedScalarWave::Tags::Psi, ::Tags::dt<CurvedScalarWave::Tags::Psi>,
        ::Tags::deriv<CurvedScalarWave::Tags::Psi, tmpl::size_t<3>,
                      Frame::Inertial>>>*>
        result,
    const tnsr::I<DataVector, 3, Frame::Inertial>& centered_coords,
    const tnsr::I<double, 3>& particle_position,
    const tnsr::I<double, 3>& particle_velocity,
    const tnsr::I<double, 3>& particle_acceleration, double bh_mass);

/*!
 * \brief Computes the puncture/singular field \f$\Psi^\mathcal{P}\f$ of a
 * scalar charge on a generic orbit in Schwarzschild spacetime.
 * described in \cite Detweiler2003.
 *
 * \details For non-geodesic orbits, there are additional contributions, see
 * `acceleration_terms_0`.
 */
void puncture_field_1(
    gsl::not_null<Variables<tmpl::list<
        CurvedScalarWave::Tags::Psi, ::Tags::dt<CurvedScalarWave::Tags::Psi>,
        ::Tags::deriv<CurvedScalarWave::Tags::Psi, tmpl::size_t<3>,
                      Frame::Inertial>>>*>
        result,
    const tnsr::I<DataVector, 3, Frame::Inertial>& centered_coords,
    const tnsr::I<double, 3>& particle_position,
    const tnsr::I<double, 3>& particle_velocity,
    const tnsr::I<double, 3>& particle_acceleration, double bh_mass);

/*!
 * \brief Computes the acceleration terms of a puncture/singular field
 * \f$\Psi^\mathcal{P}\f$ of a scalar charge on a generic orbit in Schwarzschild
 * spacetime up to zeroth order in coordinate distance.
 * \details The appropriate expression can be found in Eq. (37) of
 * \cite Wittek:2024gxn. The values ft, fx, fy are the time, x and y component
 * of the self force per unit mass evaluated at the position of the particle;
 * dt_ft, dt_fx, dt_fy are the respective total time derivatives. The code in
 * this function was auto-generated by generating the full expressions with
 * Mathematica and employing common subexpression elimination with sympy. The
 * mathematica file and generating script can be found at
 * https://github.com/nikwit/puncture-field.
 */
void acceleration_terms_0(
    gsl::not_null<Variables<tmpl::list<
        CurvedScalarWave::Tags::Psi, ::Tags::dt<CurvedScalarWave::Tags::Psi>,
        ::Tags::deriv<CurvedScalarWave::Tags::Psi, tmpl::size_t<3>,
                      Frame::Inertial>>>*>
        result,
    const tnsr::I<DataVector, 3, Frame::Inertial>& centered_coords,
    const tnsr::I<double, 3>& particle_position,
    const tnsr::I<double, 3>& particle_velocity,
    const tnsr::I<double, 3>& particle_acceleration, double ft, double fx,
    double fy, double dt_ft, double dt_fx, double dt_fy, double bh_mass);

/*!
 * \brief Computes the acceleration terms of a puncture/singular field
 * \f$\Psi^\mathcal{P}\f$ of a scalar charge on a generic orbit in Schwarzschild
 * spacetime up to first order in coordinate distance (i.e. zeroth and first
 * order).
 * \details The appropriate expression can be found in Eq. (37) of
 * \cite Wittek:2024gxn. The values ft, fx, fy are the time, x and y component
 * of the self force per unit mass evaluated at the position of the particle;
 * dt_ft, dt_fx, dt_fy are the respective total time derivatives. The code in
 * this function was auto-generated by generating the full expressions with
 * Mathematica and employing common subexpression elimination with sympy. The
 * mathematica file and generating script can be found at
 * https://github.com/nikwit/puncture-field.
 *
 */
void acceleration_terms_1(
    gsl::not_null<Variables<tmpl::list<
        CurvedScalarWave::Tags::Psi, ::Tags::dt<CurvedScalarWave::Tags::Psi>,
        ::Tags::deriv<CurvedScalarWave::Tags::Psi, tmpl::size_t<3>,
                      Frame::Inertial>>>*>
        result,
    const tnsr::I<DataVector, 3, Frame::Inertial>& centered_coords,
    const tnsr::I<double, 3>& particle_position,
    const tnsr::I<double, 3>& particle_velocity,
    const tnsr::I<double, 3>& particle_acceleration, double ft, double fx,
    double fy, double dt_ft, double dt_fx, double dt_fy, double Du_ft,
    double Du_fx, double Du_fy, double dt_Du_ft, double dt_Du_fx,
    double dt_Du_fy, double bh_mass);
}  // namespace CurvedScalarWave::Worldtube
