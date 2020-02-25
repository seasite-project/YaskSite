#ifndef YS_test_STENCIL
#define YS_test_STENCIL

/*
 * !!Do not edit: Automatically generated File
 */

#include "Heat_offsite.hpp"

class test_gen_class : public Heat2dStencil_offsite {

    protected:

        // Generic time-varying spatial-3D grid.
         Grid __Y___0;

         Param dt;

    public:

       test_gen_class(StencilList& stencils, int radius=1) : Heat2dStencil_offsite("test", stencils, radius) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(__Y___0, t, x, y, z);

             INIT_PARAM(dt);
       }


       test_gen_class(const string& name, StencilList& stencils, int radius=1) : Heat2dStencil_offsite(name, stencils, radius) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(__Y___0, t, x, y, z);

             INIT_PARAM(dt);
       }

        // Define equation at t+1 based on values at t.
        virtual void define(const IntTuple& offsets) {
            GET_OFFSET(t);
            GET_OFFSET(x);
            GET_OFFSET(y);
            GET_OFFSET(z);

            // A condition that is true when index 'x','y'or 'z' is at boundary.
            Condition at_first_x = (x < first_index(x));
            Condition at_first_y = (y < first_index(y));
            Condition at_first_z = (z < first_index(z));

            Condition at_last_x = (x > last_index(x));
            Condition at_last_y = (y > last_index(y));
            Condition at_last_z = (z > last_index(z));


            

            Heat2dStencil_offsite::boundaryHandling(__Y___0,t+0,x,y,z, at_first_x,at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
GridValue tmp_ys_0;
Heat2dStencil_offsite::addPoints(tmp_ys_0,__Y___0,t+0,x,y,z);
__Y___0(t+1,x,y,z) EQUALS tmp_ys_0;
;
        }
};

REGISTER_STENCIL(test_gen_class);

#endif
