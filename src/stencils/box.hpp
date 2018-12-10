#ifndef YS_BOX_STENCIL
#define YS_BOX_STENCIL

//Implement various kinds of stencils required in the SeASiTe project.
//If new stencils are required update this file
#include "StencilBase.hpp"

class BoxBase : public StencilBase {

    protected:

        // Generic time-varying spatial-3D grid.
        Grid data;

        //The coefficient (1/h)
        Param h_inv;

        // Return a coefficient.  Note: This returns completely fabricated
        // values only for illustrative purposes; they have no mathematical
        // significance.
        virtual double coeff(int di, int dj, int dk) const {
            return 1;
        }

        virtual void points(GridValue& v, Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z) =0;
        virtual void addPoints(GridValue& v, Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z) =0;
        virtual void boundaryHandling(Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z) =0;
    public:

        BoxBase(const string& name, StencilList& stencils) : StencilBase(name, stencils) {
            //can support max. upto 3D space dimension
            INIT_GRID_4D(data, t, x, y, z);
            INIT_PARAM(h_inv);
        }

        // Define equation at t+1 based on values at t.
        virtual void define(const IntTuple& offsets) {
            GET_OFFSET(t);
            GET_OFFSET(x);
            GET_OFFSET(y);
            GET_OFFSET(z);

            // A condition that is true when index 'x','y'or 'z' is at boundary.
            // Normally bounding box calculation doesn't go to halo, but I hack
            // it so that the same stencil can be used for all vector foldings
            // without the problem of unimplemented unaligned writes
            Condition at_first_x = (x < first_index(x));
            Condition at_first_y = (y < first_index(y));
            Condition at_first_z = (z < first_index(z));

            Condition at_last_x = (x > last_index(x));
            Condition at_last_y = (y > last_index(y));
            Condition at_last_z = (z > last_index(z));


            GridValue v;
            boundaryHandling(data, t,x,y,z, at_first_x, at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
            addPoints(v, data, t, x, y, z);
            // define the value at t+1 to be equivalent to v.
            data(t+1, x, y, z) EQUALS v;
        }
};

class Box1dStencil : public BoxBase {
    protected:
        virtual double coeff(int di, int dj, int dk) const
        {
            if(dk==0)
            {
                return 1;
            }
            else
            {
                return 1;
            }
        }

        // Add additional points to v.
        virtual void points(GridValue& v,  Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            v += coeff( 0, 0, 0) * data(t, x, y, z  );
            v += coeff( 0, 0, 1) * data(t, x, y, z+1) +
                 coeff( 0, 0,-1) * data(t, x, y, z-1);
        }

        virtual void addPoints(GridValue& v, Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            points(v, data, t, x, y, z);
            //v *= h_inv;
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
             //I do a hack here because unary '+' is not overloaded, adding
             //another term data(t,x,y,z) is also a hack to show dependency to
             //stencil compiler
            data(t, x, y, z) EQUALS ( data(t, x, y, z+1)*1.0)
                IF at_first_z;

            //I do a hack here because unary '+' is not overloaded
            data(t, x, y, z) EQUALS ( data(t, x, y, z-1)*1.0)
                IF at_last_z;
        }


    public:
        Box1dStencil(StencilList& stencils) :
            BoxBase("box1d", stencils) { }
        Box1dStencil(const string& name, StencilList& stencils) :
            BoxBase(name, stencils) { }

};
YS_REGISTER_STENCIL("box1d",Box1dStencil);

class Box2dStencil : public Box1dStencil {
    protected:
        virtual double coeff(int di, int dj, int dk) const
        {
            if(dj==0&&dk==0)
            {
                return 1;
            }
            else
            {
                return 1;
            }
        }

        // Add additional points to v.
        virtual void points(GridValue& v, Grid &data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            Box1dStencil::points(v, data, t, x, y-1, z);
            Box1dStencil::points(v, data, t, x, y, z);
            Box1dStencil::points(v, data, t, x, y+1, z);
        }

        virtual void addPoints(GridValue& v, Grid &data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            points(v, data, t, x, y, z);
           // v *= h_inv*h_inv;
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
            Box1dStencil::boundaryHandling(data, t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);

            data(t, x, y, z) EQUALS (data(t,x,y+1,z)*1.0)
                IF at_first_y;

            data(t, x, y, z) EQUALS (data(t,x,y-1,z)*1.0)
                IF at_last_y;
        }


    public:
        Box2dStencil(StencilList& stencils) :
            Box1dStencil("box2d", stencils) { }
        Box2dStencil(const string& name, StencilList& stencils) :
            Box1dStencil(name, stencils) { }

};
YS_REGISTER_STENCIL("box2d",Box2dStencil);


class Box3dStencil : public Box2dStencil {
    protected:
        virtual double coeff(int di, int dj, int dk) const
        {
            if((dj==0&&dk==0)&&(di==0))
            {
                return 1;//-6;
            }
            else
            {
                return 1;
            }
        }

        // Add additional points to v.
        virtual void points(GridValue& v, Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            Box2dStencil::points(v, data, t, x-1, y, z);
            Box2dStencil::points(v, data, t, x+0, y, z);
            Box2dStencil::points(v, data, t, x+1, y, z);
        }

        virtual void addPoints(GridValue& v, Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            points(v, data, t, x, y, z);
        //    v*=h_inv*h_inv*h_inv;
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
            Box2dStencil::boundaryHandling(data, t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);
            data(t, x, y, z) EQUALS (data(t,x+1,y,z)*1.0)
                IF at_first_x;
            data(t, x, y, z) EQUALS (data(t,x-1,y,z)*1.0)
                IF at_last_x;
        }


    public:
        Box3dStencil(StencilList& stencils) :
            Box2dStencil("box3d", stencils) { }
        Box3dStencil(const string& name, StencilList& stencils) :
            Box2dStencil(name, stencils) { }

};

YS_REGISTER_STENCIL("box3d",Box3dStencil);

#endif
