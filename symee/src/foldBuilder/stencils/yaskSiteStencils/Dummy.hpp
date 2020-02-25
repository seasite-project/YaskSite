#ifndef YS_DUMMY_STENCIL
#define YS_DUMMY_STENCIL

//Implement various kinds of stencils required in the SeASiTe project.
//If new stencils are required update this file
#include "StencilBase.hpp"

class Dummy : public StencilBase {

    protected:
        // Generic time-varying spatial-3D grid.
        Grid data;



    public:

        Dummy(StencilList& stencils) : StencilBase("dummy", stencils) {
            INIT_GRID_4D(data, t, x, y, z);
        }
        Dummy(const string& name, StencilList& stencils) : StencilBase(name, stencils) {
            INIT_GRID_4D(data, t, x, y, z);
        }

        // Define equation at t+1 based on values at t.
        virtual void define(const IntTuple& offsets) {
        }
};
REGISTER_STENCIL(Dummy);
#endif
