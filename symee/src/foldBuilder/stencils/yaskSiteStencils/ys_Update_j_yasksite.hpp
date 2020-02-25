#ifndef YS_ys_Update_j_yasksite_STENCIL
#define YS_ys_Update_j_yasksite_STENCIL

/*
 * !!Do not edit: Automatically generated File
 */

#include "Dummy.hpp"

class ys_Update_j_yasksite_gen_class : public Dummy {

    protected:

        // Generic time-varying spatial-3D grid.
         Grid dy;

         Param dt;

    public:

       ys_Update_j_yasksite_gen_class(StencilList& stencils) : Dummy("ys_Update_j_yasksite", stencils) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(dy, t, x, y, z);

             INIT_PARAM(dt);
       }


       ys_Update_j_yasksite_gen_class(const string& name, StencilList& stencils) : Dummy(name, stencils) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(dy, t, x, y, z);

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


            

            data(t+1,x,y,z) EQUALS data(t+0,x,y,z)+dt*dy(t+0,x,y,z);
;
        }
};

REGISTER_STENCIL(ys_Update_j_yasksite_gen_class);

#endif
