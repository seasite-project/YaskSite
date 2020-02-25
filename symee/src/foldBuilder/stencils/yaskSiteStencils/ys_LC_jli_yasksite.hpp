#ifndef YS_ys_LC_jli_yasksite_STENCIL
#define YS_ys_LC_jli_yasksite_STENCIL

/*
 * !!Do not edit: Automatically generated File
 */

#include "Dummy.hpp"

class ys_LC_jli_yasksite_gen_class : public Dummy {

    protected:

        // Generic time-varying spatial-3D grid.
         Grid Y_0; Grid Y_1; Grid Y_2; Grid Y_3; Grid F_0; Grid F_1; Grid F_2; Grid F_3;

         Param h;

    public:

       ys_LC_jli_yasksite_gen_class(StencilList& stencils) : Dummy("ys_LC_jli_yasksite", stencils) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(Y_0, t, x, y, z); INIT_GRID_4D(Y_1, t, x, y, z); INIT_GRID_4D(Y_2, t, x, y, z); INIT_GRID_4D(Y_3, t, x, y, z); INIT_GRID_4D(F_0, t, x, y, z); INIT_GRID_4D(F_1, t, x, y, z); INIT_GRID_4D(F_2, t, x, y, z); INIT_GRID_4D(F_3, t, x, y, z);

             INIT_PARAM(h);
       }


       ys_LC_jli_yasksite_gen_class(const string& name, StencilList& stencils) : Dummy(name, stencils) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(Y_0, t, x, y, z); INIT_GRID_4D(Y_1, t, x, y, z); INIT_GRID_4D(Y_2, t, x, y, z); INIT_GRID_4D(Y_3, t, x, y, z); INIT_GRID_4D(F_0, t, x, y, z); INIT_GRID_4D(F_1, t, x, y, z); INIT_GRID_4D(F_2, t, x, y, z); INIT_GRID_4D(F_3, t, x, y, z);

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


             GridValue tmp_arr_0; GridValue tmp_arr_1; GridValue tmp_arr_2; GridValue tmp_arr_3;

            tmp_arr_0=0.112999479323160*F_0(t+0,x,y,z);
tmp_arr_0=-0.0403092207235200*F_1(t+0,x,y,z);
tmp_arr_0=0.0258023774203400*F_2(t+0,x,y,z);
tmp_arr_0=-0.00990467650730000*F_3(t+0,x,y,z);
Y_0(t+0,x,y,z) EQUALS tmp_arr_0*h+data(t+0,x,y,z);
tmp_arr_1+=0.234383995747400*F_0(t+0,x,y,z);
tmp_arr_1+=0.206892573935360*F_1(t+0,x,y,z);
tmp_arr_1+=-0.0478571280485400*F_2(t+0,x,y,z);
tmp_arr_1+=0.0160474228065200*F_3(t+0,x,y,z);
Y_1(t+0,x,y,z) EQUALS tmp_arr_1*h+data(t+0,x,y,z);
tmp_arr_2+=0.216681784623250*F_0(t+0,x,y,z);
tmp_arr_2+=0.406123263867370*F_1(t+0,x,y,z);
tmp_arr_2+=0.189036518170060*F_2(t+0,x,y,z);
tmp_arr_2+=-0.0241821048998300*F_3(t+0,x,y,z);
Y_2(t+0,x,y,z) EQUALS tmp_arr_2*h+data(t+0,x,y,z);
tmp_arr_3+=0.220462211176770*F_0(t+0,x,y,z);
tmp_arr_3+=0.388193468843170*F_1(t+0,x,y,z);
tmp_arr_3+=0.328844319980060*F_2(t+0,x,y,z);
tmp_arr_3+=0.0625000000000000*F_3(t+0,x,y,z);
Y_3(t+0,x,y,z) EQUALS tmp_arr_3*h+data(t+0,x,y,z);
;
        }
};

REGISTER_STENCIL(ys_LC_jli_yasksite_gen_class);

#endif
