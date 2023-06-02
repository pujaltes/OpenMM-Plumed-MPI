#ifndef OPENMM_PLUMEDFORCE_H_
#define OPENMM_PLUMEDFORCE_H_

/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2016 Stanford University and the Authors.           *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include <mpi.h>
#include "openmm/Context.h"
#include "openmm/Force.h"
#include <cstdio>
#include <string>
#include "internal/windowsExportPlumed.h"

namespace PlumedPlugin {

/**
 * This class implements a connection between OpenMM and PLUMED (http://www.plumed.org).  It is a Force object that you
 * add to the System with addForce().  Its behavior is defined by a PLUMED input script, which you pass to the constructor
 * as a string.  Within that script you can specify bias forces to apply to the System, as well as values to write to
 * output files every time the force is computed.
 *
 * For example, the following code performs metadynamics based on the distance between atoms 0 and 9:
 *
 * <tt><pre>
 * string script =
 *     "d: DISTANCE ATOMS=1,10\n"
 *     "METAD ARG=d SIGMA=0.2 HEIGHT=0.3 PACE=500";
 * system.addForce(new PlumedForce(script));
 * </pre></tt>
 *
 * Be aware the PLUMED numbers atoms starting from 1, whereas OpenMM numbers them starting from 0.
 */

class OPENMM_EXPORT_PLUMED PlumedForce : public OpenMM::Force {
public:
    /**
     * Create a PlumedForce.
     *
     * @param script    the PLUMED input script
     */
    PlumedForce(const std::string& script, const MPI_Comm intra_comm, const MPI_Comm inter_comm);
    /**
     * Get the PLUMED input script
     */
    const std::string& getScript() const;

    const MPI_Comm getIntracom() const;

    const MPI_Comm getIntercom() const;
    /**
     * Returns true if the force uses periodic boundary conditions and false otherwise. Your force should implement this
     * method appropriately to ensure that `System.usesPeriodicBoundaryConditions()` works for all systems containing
     * your force.
     */
    bool usesPeriodicBoundaryConditions() const {
        return false;
    }
    /**
     * Set temperature, measured in Kelvin. A negative value means undefined and not passed to PLUMED.
     * By default it is set to -1.
     */
    void setTemperature(double temperature);
    /**
     * Get temperature, measured in Kelvin.
     */
    double getTemperature() const;
    /**
     * Set particle masses, messured in Dalton. If not set, the OpenMM::System
     * masses are used.
     *
     * This is useful when the mass mass repartion of hydrogen bonds is
     * applied, but PLUMED expects the physical masses for the CV definition
     * (i.e. the center of mass).
     */
    void setMasses(const std::vector<double>& masses);
    /**
     * Get particle masses. An empty array means that the OpenMM::System masses
     * are used.
     */
    const std::vector<double>& getMasses() const;
    /**
     * Set the C stream of the PLUMED log. By default it is set to `stdout`.
     */
    void setLogStream(FILE* stream);
    /**
     * Get the C sream of the PLUMED log.
     */
    FILE* getLogStream() const;
    /**
     * Set the state of PLUMED restart (https://www.plumed.org/doc-master/user-doc/html/_r_e_s_t_a_r_t.html). By default it is `false`.
     */
    void setRestart(bool restart);
    /**
     * Get the state of PLUMED restart.
     */
    bool getRestart() const;
protected:
    OpenMM::ForceImpl* createImpl() const;
private:
    std::string script;
    MPI_Comm intra_comm;
    MPI_Comm inter_comm;
    double temperature;
    std::vector<double> masses;
    FILE* logStream;
    bool restart;
};

} // namespace PlumedPlugin

#endif /*OPENMM_PLUMEDFORCE_H_*/
