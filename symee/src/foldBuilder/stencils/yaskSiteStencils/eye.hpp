#ifndef YS_EYE_STENCIL
#define YS_EYE_STENCIL

//Implement various kinds of stencils required in the SeASiTe project.
//If new stencils are required update this file
#include "StencilBase.hpp"

class EyeBase : public StencilBase {

    protected:
        // Generic time-varying spatial-3D grid.
        Grid data;


        virtual void addPoints(GridValue& v, Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z) =0;
        virtual void boundaryHandling(Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z) =0;

    public:

        EyeBase(const string& name, StencilList& stencils) : StencilBase(name, stencils) {
            INIT_GRID_4D(data, t, x, y, z);
        }

        // Define equation at t+1 based on values at t.
        virtual void define(const IntTuple& offsets) {
            GET_OFFSET(t);
            GET_OFFSET(x);
            GET_OFFSET(y);
            GET_OFFSET(z);

            GridValue v;
            addPoints(v, data, t, x, y, z);
            // define the value at t+1 to be equivalent to v.
            data(t, x, y, z) EQUALS v;
        }
};

class Eye : public EyeBase {
    protected:

        virtual void addPoints(GridValue& v, Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            v = data(t,x,y,z);
        }
        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
        }

    public:
        Eye(StencilList& stencils) :
            EyeBase("eye", stencils) { }
        Eye(const string& name, StencilList& stencils) :
            EyeBase(name, stencils) { }

};
REGISTER_STENCIL(Eye);
#endif
