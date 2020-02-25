#ifndef YS_ys_RHS_LC_jil_yasksite_Heat2D_STENCIL
#define YS_ys_RHS_LC_jil_yasksite_Heat2D_STENCIL

/*
 * !!Do not edit: Automatically generated File
 */

#include "Heat_offsite.hpp"

class ys_RHS_LC_jil_yasksite_Heat2D_gen_class : public Heat2dStencil_offsite {

    protected:

        // Generic time-varying spatial-3D grid.
         Grid __Y___0; Grid __Y___1; Grid __Y___2; Grid __Y___3;

         Param dt;

    public:

       ys_RHS_LC_jil_yasksite_Heat2D_gen_class(StencilList& stencils, int radius=1) : Heat2dStencil_offsite("ys_RHS_LC_jil_yasksite_Heat2D", stencils, radius) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(__Y___0, t, x, y, z); INIT_GRID_4D(__Y___1, t, x, y, z); INIT_GRID_4D(__Y___2, t, x, y, z); INIT_GRID_4D(__Y___3, t, x, y, z);

             INIT_PARAM(dt);
       }


       ys_RHS_LC_jil_yasksite_Heat2D_gen_class(const string& name, StencilList& stencils, int radius=1) : Heat2dStencil_offsite(name, stencils, radius) {
            //can support max. upto 3D space dimension
             INIT_GRID_4D(__Y___0, t, x, y, z); INIT_GRID_4D(__Y___1, t, x, y, z); INIT_GRID_4D(__Y___2, t, x, y, z); INIT_GRID_4D(__Y___3, t, x, y, z);

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


             GridValue tmp; GridValue tmp_arr_0; GridValue tmp_arr_1; GridValue tmp_arr_2; GridValue tmp_arr_3;

            Heat2dStencil_offsite::boundaryHandling(__Y___0,t+0,x,y,z, at_first_x,at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
GridValue tmp_ys_0;
Heat2dStencil_offsite::addPoints(tmp_ys_0,__Y___0,t+0,x,y,z);
tmp=tmp_ys_0;
tmp_arr_0=0.112999479323160*tmp;
tmp_arr_1=0.234383995747400*tmp;
tmp_arr_2=0.216681784623250*tmp;
tmp_arr_3=0.220462211176770*tmp;
Heat2dStencil_offsite::boundaryHandling(__Y___1,t+0,x,y,z, at_first_x,at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
GridValue tmp_ys_1;
Heat2dStencil_offsite::addPoints(tmp_ys_1,__Y___1,t+0,x,y,z);
tmp=tmp_ys_1;
tmp_arr_0+=-0.0403092207235200*tmp;
tmp_arr_1+=0.206892573935360*tmp;
tmp_arr_2+=0.406123263867370*tmp;
tmp_arr_3+=0.388193468843170*tmp;
Heat2dStencil_offsite::boundaryHandling(__Y___2,t+0,x,y,z, at_first_x,at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
GridValue tmp_ys_2;
Heat2dStencil_offsite::addPoints(tmp_ys_2,__Y___2,t+0,x,y,z);
tmp=tmp_ys_2;
tmp_arr_0+=0.0258023774203400*tmp;
tmp_arr_1+=-0.0478571280485400*tmp;
tmp_arr_2+=0.189036518170060*tmp;
tmp_arr_3+=0.328844319980060*tmp;
Heat2dStencil_offsite::boundaryHandling(__Y___3,t+0,x,y,z, at_first_x,at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
GridValue tmp_ys_3;
Heat2dStencil_offsite::addPoints(tmp_ys_3,__Y___3,t+0,x,y,z);
tmp=tmp_ys_3;
tmp_arr_0+=-0.00990467650730000*tmp;
tmp_arr_1+=0.0160474228065200*tmp;
tmp_arr_2+=-0.0241821048998300*tmp;
tmp_arr_3+=0.0625000000000000*tmp;
__Y___0(t+1,x,y,z) EQUALS tmp_arr_0*dt+data(t+0,x,y,z);
__Y___1(t+1,x,y,z) EQUALS tmp_arr_1*dt+data(t+0,x,y,z);
__Y___2(t+1,x,y,z) EQUALS tmp_arr_2*dt+data(t+0,x,y,z);
__Y___3(t+1,x,y,z) EQUALS tmp_arr_3*dt+data(t+0,x,y,z);
;
        }
};

REGISTER_STENCIL(ys_RHS_LC_jil_yasksite_Heat2D_gen_class);

#endif
