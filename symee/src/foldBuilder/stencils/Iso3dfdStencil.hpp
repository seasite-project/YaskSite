/*****************************************************************************

YASK: Yet Another Stencil Kernel
Copyright (c) 2014-2017, Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

*****************************************************************************/

// Implement isotropic 3D finite-difference (FD) stencil, nth-order accurate in
// space (where n = 2 * radius) and 2nd-order accurate in time.

#include "StencilBase.hpp"

class Iso3dfdStencil : public StencilRadiusBase {

protected:
    Grid pressure;              // time-varying 3D pressure grid.
    Grid vel;                   // constant 3D vel grid.
    Param coeff;                // coefficients.
    
public:

    // For this stencil, the 'radius' is half the FD accuracy in space.  For
    // example, radius=8 implements a 16th-order accurate FD stencil.  To
    // obtain the correct result, the 'coeff' array should be initialized
    // with the corresponding central FD coefficients.  The accuracy in time
    // is fixed at 2nd order.
    Iso3dfdStencil(StencilList& stencils, int radius=8) :
        StencilRadiusBase("iso3dfd", stencils, radius)
    {
        INIT_GRID_4D(pressure, t, x, y, z);
        INIT_GRID_3D(vel, x, y, z);
        INIT_PARAM_1D(coeff, r, radius + 1); // c0, c1 .. c<radius>.
    }
    virtual ~Iso3dfdStencil() { }

    // Set radius.
    // Return true if successful.
    virtual bool setRadius(int radius) {
        if (!StencilRadiusBase::setRadius(radius))
            return false;
        coeff.setVal("r", radius + 1); // change dimension of coeff.
        return true;
    }

    // Define equation for pressure at t+1 based on values from vel and pressure at t.
    virtual void define(const IntTuple& offsets) {
        GET_OFFSET(t);
        GET_OFFSET(x);
        GET_OFFSET(y);
        GET_OFFSET(z);

        // Start with center value multiplied by coeff 0.
        GridValue next_p = pressure(t, x, y, z) * coeff(0);

        // Add values from x, y, and z axes multiplied by the
        // coeff for the given radius.
        for (int r = 1; r <= _radius; r++) {

            // Add values from axes at radius r.
            next_p += (
                       // x-axis.
                       pressure(t, x-r, y, z) +
                       pressure(t, x+r, y, z) +
                
                       // y-axis.
                       pressure(t, x, y-r, z) +
                       pressure(t, x, y+r, z) +
                
                       // z-axis.
                       pressure(t, x, y, z-r) +
                       pressure(t, x, y, z+r)

                       ) * coeff(r);
        }

        // Finish equation, including t-1 and velocity components.
        next_p = (2.0 * pressure(t, x, y, z))
            - pressure(t-1, x, y, z) // subtract pressure from t-1.
            + (next_p * vel(x, y, z));       // add next_p * velocity.

        // Define the value at t+1 to be equal to next_p.
        // Since this implements the finite-difference method, this
        // is actually an approximation.
        pressure(t+1, x, y, z) EQUALS next_p;
    }
};

REGISTER_STENCIL(Iso3dfdStencil);
