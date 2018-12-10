#ifndef YS_heat2d_irk_STENCIL
#define YS_heat2d_irk_STENCIL

/*
 * !!Do not edit: Automatically generated File
 */

#include "heat.hpp"

class heat2d_irk_gen_class : public Heat2dStencil {

    protected:

        // Generic time-varying spatial-3D grid.
         Grid F_0; Grid F_1; Grid F_2; Grid F_3; Grid F_4; Grid F_5; Grid F_6; Grid F_7; Grid F_8; Grid F_9; Grid F_10; Grid F_11; Grid F_12; Grid F_13; Grid F_14; Grid F_15; Grid F_16; Grid F_17; Grid F_18; Grid F_19; Grid F_20; Grid F_21; Grid F_22; Grid F_23;

         Param h;

    public:

       heat2d_irk_gen_class(StencilList& stencils, int radius=1) : Heat2dStencil("heat2d_irk", stencils, radius) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(F_0, t, x, y, z); INIT_GRID_4D(F_1, t, x, y, z); INIT_GRID_4D(F_2, t, x, y, z); INIT_GRID_4D(F_3, t, x, y, z); INIT_GRID_4D(F_4, t, x, y, z); INIT_GRID_4D(F_5, t, x, y, z); INIT_GRID_4D(F_6, t, x, y, z); INIT_GRID_4D(F_7, t, x, y, z); INIT_GRID_4D(F_8, t, x, y, z); INIT_GRID_4D(F_9, t, x, y, z); INIT_GRID_4D(F_10, t, x, y, z); INIT_GRID_4D(F_11, t, x, y, z); INIT_GRID_4D(F_12, t, x, y, z); INIT_GRID_4D(F_13, t, x, y, z); INIT_GRID_4D(F_14, t, x, y, z); INIT_GRID_4D(F_15, t, x, y, z); INIT_GRID_4D(F_16, t, x, y, z); INIT_GRID_4D(F_17, t, x, y, z); INIT_GRID_4D(F_18, t, x, y, z); INIT_GRID_4D(F_19, t, x, y, z); INIT_GRID_4D(F_20, t, x, y, z); INIT_GRID_4D(F_21, t, x, y, z); INIT_GRID_4D(F_22, t, x, y, z); INIT_GRID_4D(F_23, t, x, y, z);

             INIT_PARAM(h);
       }


       heat2d_irk_gen_class(const string& name, StencilList& stencils, int radius=1) : Heat2dStencil(name, stencils, radius) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(F_0, t, x, y, z); INIT_GRID_4D(F_1, t, x, y, z); INIT_GRID_4D(F_2, t, x, y, z); INIT_GRID_4D(F_3, t, x, y, z); INIT_GRID_4D(F_4, t, x, y, z); INIT_GRID_4D(F_5, t, x, y, z); INIT_GRID_4D(F_6, t, x, y, z); INIT_GRID_4D(F_7, t, x, y, z); INIT_GRID_4D(F_8, t, x, y, z); INIT_GRID_4D(F_9, t, x, y, z); INIT_GRID_4D(F_10, t, x, y, z); INIT_GRID_4D(F_11, t, x, y, z); INIT_GRID_4D(F_12, t, x, y, z); INIT_GRID_4D(F_13, t, x, y, z); INIT_GRID_4D(F_14, t, x, y, z); INIT_GRID_4D(F_15, t, x, y, z); INIT_GRID_4D(F_16, t, x, y, z); INIT_GRID_4D(F_17, t, x, y, z); INIT_GRID_4D(F_18, t, x, y, z); INIT_GRID_4D(F_19, t, x, y, z); INIT_GRID_4D(F_20, t, x, y, z); INIT_GRID_4D(F_21, t, x, y, z); INIT_GRID_4D(F_22, t, x, y, z); INIT_GRID_4D(F_23, t, x, y, z);

             INIT_PARAM(h);
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


             GridValue tmp; GridValue tmp_0; GridValue tmp_1; GridValue tmp_2; GridValue tmp_3;

            Heat2dStencil::boundaryHandling(data,t+0,x,y,z, at_first_x,at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
GridValue tmp_ys_0;
Heat2dStencil::addPoints(tmp_ys_0,data,t+0,x,y,z);
tmp=tmp_ys_0;
F_1(t+0,x,y,z) EQUALS tmp_0*h+data(t+0,x,y,z);
F[ll]<0>=tmp_1*h+data(t+0,x,y,z);
F_14(t+0,x,y,z) EQUALS tmp_2*h+data(t+0,x,y,z);
F_15(t+0,x,y,z) EQUALS tmp_3*h+data(t+0,x,y,z);
;
        }
};

YS_REGISTER_STENCIL_RADIUS("heat2d_irk", heat2d_irk_gen_class);

#endif
