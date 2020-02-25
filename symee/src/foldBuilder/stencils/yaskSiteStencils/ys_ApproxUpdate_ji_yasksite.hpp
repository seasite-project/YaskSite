#ifndef YS_ys_ApproxUpdate_ji_yasksite_STENCIL
#define YS_ys_ApproxUpdate_ji_yasksite_STENCIL

/*
 * !!Do not edit: Automatically generated File
 */

#include "Dummy.hpp"

class ys_ApproxUpdate_ji_yasksite_gen_class : public Dummy {

    protected:

        // Generic time-varying spatial-3D grid.
         Grid F_0; Grid F_1; Grid F_2; Grid F_3;

         Param dt; Param h;

    public:

       ys_ApproxUpdate_ji_yasksite_gen_class(StencilList& stencils) : Dummy("ys_ApproxUpdate_ji_yasksite", stencils) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(F_0, t, x, y, z); INIT_GRID_4D(F_1, t, x, y, z); INIT_GRID_4D(F_2, t, x, y, z); INIT_GRID_4D(F_3, t, x, y, z);

             INIT_PARAM(dt); INIT_PARAM(h);
       }


       ys_ApproxUpdate_ji_yasksite_gen_class(const string& name, StencilList& stencils) : Dummy(name, stencils) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(F_0, t, x, y, z); INIT_GRID_4D(F_1, t, x, y, z); INIT_GRID_4D(F_2, t, x, y, z); INIT_GRID_4D(F_3, t, x, y, z);

             INIT_PARAM(dt); INIT_PARAM(h);
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


             GridValue tmp; GridValue b_0; GridValue b_1; GridValue b_2; GridValue b_3;

            tmp=tmp+h*0.220462211176770*F_0(t+0,x,y,z);
tmp=tmp+h*0.388193468843170*F_1(t+0,x,y,z);
tmp=tmp+h*0.328844319980060*F_2(t+0,x,y,z);
tmp=tmp+h*0.0625000000000000*F_3(t+0,x,y,z);
data(t+0,x,y,z) EQUALS tmp;
;
        }
};

REGISTER_STENCIL(ys_ApproxUpdate_ji_yasksite_gen_class);

#endif
