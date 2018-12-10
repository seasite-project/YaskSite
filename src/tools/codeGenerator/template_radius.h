#ifndef YS_#DERIVED_STENCIL#_STENCIL
#define YS_#DERIVED_STENCIL#_STENCIL

/*
 * !!Do not edit: Automatically generated File
 */

#include "#BASE_STENCIL_FILE#"

class #DERIVED_STENCIL#_gen_class : public #BASE_STENCIL# {

    protected:

        // Generic time-varying spatial-3D grid.
        #DECLARE_EXTRA_GRIDS#

        #DECLARE_EXTRA_PARAMS#

    public:

       #DERIVED_STENCIL#_gen_class(StencilList& stencils, int radius=1) : #BASE_STENCIL#("#DERIVED_STENCIL#", stencils, radius) {
            //can support max. upto 3D space dimension
            #DEFINE_EXTRA_GRIDS#

            #DEFINE_EXTRA_PARAMS#
       }


       #DERIVED_STENCIL#_gen_class(const string& name, StencilList& stencils, int radius=1) : #BASE_STENCIL#(name, stencils, radius) {
            //can support max. upto 3D space dimension
            #DEFINE_EXTRA_GRIDS#

            #DEFINE_EXTRA_PARAMS#
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


            #DECLARE_GRID_POINTS#

            #GENERATED_CODE#
        }
};

YS_REGISTER_STENCIL_RADIUS("#DERIVED_STENCIL#", #DERIVED_STENCIL#_gen_class);

#endif
