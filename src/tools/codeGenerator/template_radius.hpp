#ifndef YS_#DERIVED_STENCIL#_STENCIL
#define YS_#DERIVED_STENCIL#_STENCIL

/*
 * !!Do not edit: Automatically generated File
 */

#include "yask_compiler_api.hpp"
using namespace std;
using namespace yask;

#include "#BASE_STENCIL_FILE#"

namespace {

class #DERIVED_STENCIL#_gen_class : public #BASE_STENCIL# {

    protected:
        yc_index_node_ptr t = new_step_index("t");           // step in time dim.
        yc_index_node_ptr x = new_domain_index("x");         // spatial dim.
        yc_index_node_ptr y = new_domain_index("y");         // spatial dim.
        yc_index_node_ptr z = new_domain_index("z");         // spatial dim.


        // Generic time-varying spatial-3D grid.
        #DECLARE_EXTRA_GRIDS#

        #DECLARE_EXTRA_PARAMS#

    public:

       #DERIVED_STENCIL#_gen_class(int radius=1) : #BASE_STENCIL#("#DERIVED_STENCIL#", radius) {
            //can support max. upto 3D space dimension
       }


       #DERIVED_STENCIL#_gen_class(const string& name,  int radius=1) : #BASE_STENCIL#(name, radius) {
            //can support max. upto 3D space dimension
       }

        // Define equation at t+1 based on values at t.
        virtual void define() {

            // A condition that is true when index 'x','y'or 'z' is at boundary.
            const yc_bool_node_ptr& at_first_x = (x < first_domain_index(x));
            const yc_bool_node_ptr& at_first_y = (y < first_domain_index(y));
            const yc_bool_node_ptr& at_first_z = (z < first_domain_index(z));

            const yc_bool_node_ptr& at_last_x = (x > last_domain_index(x));
            const yc_bool_node_ptr& at_last_y = (y > last_domain_index(y));
            const yc_bool_node_ptr& at_last_z = (z > last_domain_index(z));


            #DECLARE_GRID_POINTS#

            #GENERATED_CODE#
        }
};

YS_REGISTER_STENCIL_RADIUS("#DERIVED_STENCIL#", #DERIVED_STENCIL#_gen_class);

}

#endif
