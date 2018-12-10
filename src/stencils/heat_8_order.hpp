#ifndef YS_HEAT_8_order_STENCIL
#define YS_HEAT_8_order_STENCIL

//Implement various kinds of stencils required in the SeASiTe project.
//If new stencils are required update this file
#include "StencilBase.hpp"

class HeatBase_8order : public StencilBase {

    protected:

        // Generic time-varying spatial-3D grid.
        Grid data;

        //The coefficient (1/h)
        Param h_inv_sq;

        // Return a coefficient.  Note: This returns completely fabricated
        // values only for illustrative purposes; they have no mathematical
        // significance.
        virtual double coeff(int di, int dj, int dk) const {
            return 1;
        }

        // Add additional points to expression v.
        virtual void addPoints(GridValue& v, Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z) =0;
        virtual void boundaryHandling(Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z) =0;


    public:

        HeatBase_8order(const string& name, StencilList& stencils) : StencilBase(name, stencils) {
            //can support max. upto 3D space dimension
            INIT_GRID_4D(data, t, x, y, z);
            INIT_PARAM(h_inv_sq);
        }

        // Define equation at t+1 based on values at t.
        virtual void define(const IntTuple& offsets) {
            GET_OFFSET(t);
            GET_OFFSET(x);
            GET_OFFSET(y);
            GET_OFFSET(z);

            // A condition that is true when index 'x','y'or 'z' is at boundary.
            Condition at_first_x = (x == first_index(x));
            Condition at_first_y = (y == first_index(y));
            Condition at_first_z = (z == first_index(z));

            Condition at_last_x = (x == last_index(x));
            Condition at_last_y = (y == last_index(y));
            Condition at_last_z = (z == last_index(z));


            // Add additional points from derived class.
            GridValue v;
            boundaryHandling(data, t,x,y,z, at_first_x, at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
            addPoints(v, data, t, x, y, z);

            v *= h_inv_sq;
            // define the value at t+1 to be equivalent to v.
            data(t+1, x, y, z) EQUALS v;
        }
};

class Heat1dStencil_8order : public HeatBase_8order {
    protected:
        virtual double coeff(int di, int dj, int dk) const
        {
            if(dk==0)
            {
                return -2;
            }
            else
            {
                return 1;
            }
        }

        // Add additional points to v.
        virtual void addPoints(GridValue& v, Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            v += coeff( 0, 0, 0) * data(t, x, y  , z  );
            v += coeff( 0, 0, 1) * data(t, x, y, z+1  ) +
                 coeff( 0, 0, 2) * data(t, x, y, z+2  ) +
                 coeff( 0, 0, 3) * data(t, x, y, z+3  ) +
                 coeff( 0, 0,-1) * data(t, x, y, z-1  ) +
                 coeff( 0, 0,-2) * data(t, x, y, z-2  ) +
                 coeff( 0, 0,-3) * data(t, x, y, z-3  ) ;
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
             //I do a hack here because unary '+' is not overloaded
            data(t, x, y, z-1) EQUALS (data(t, x, y, z)*1.0)
                IF at_first_z;

            //I do a hack here because unary '+' is not overloaded
            data(t, x, y, z+1) EQUALS (data(t, x, y, z)*1.0)
                IF at_last_z;
        }


    public:
        Heat1dStencil_8order(StencilList& stencils) :
            HeatBase_8order("heat_8order_1d", stencils) { }
        Heat1dStencil_8order(const string& name, StencilList& stencils) :
            HeatBase_8order(name, stencils) { }

};
YS_REGISTER_STENCIL("heat_8order_1d",Heat1dStencil_8order);

class Heat2dStencil_8order : public Heat1dStencil_8order {
    protected:
        virtual double coeff(int di, int dj, int dk) const
        {
            if(dj==0&&dk==0)
            {
                return -4;
            }
            else
            {
                return 1;
            }
        }

        // Add additional points to v.
        virtual void addPoints(GridValue& v, Grid &data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            Heat1dStencil_8order::addPoints(v, data, t, x, y, z);
            v += coeff( 0, 1, 0) * data(t, x, y+1, z  ) +
                 coeff( 0, 2, 0) * data(t, x, y+2, z  ) +
                 coeff( 0, 3, 0) * data(t, x, y+3, z  ) +
                 coeff( 0,-1, 0) * data(t, x, y-1, z  ) +
                 coeff( 0,-2, 0) * data(t, x, y-2, z  ) +
                 coeff( 0,-3, 0) * data(t, x, y-3, z  ) ;
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
            Heat1dStencil_8order::boundaryHandling(data, t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);

            data(t, x, y-1, z) EQUALS (data(t, x, y, z)*1.0)
                IF at_first_y;

            data(t, x, y+1, z) EQUALS (data(t, x, y, z)*1.0)
                IF at_last_y;
        }


    public:
        Heat2dStencil_8order(StencilList& stencils) :
            Heat1dStencil_8order("heat_8order_2d", stencils) { }
        Heat2dStencil_8order(const string& name, StencilList& stencils) :
            Heat1dStencil_8order(name, stencils) { }

};
YS_REGISTER_STENCIL("heat_8order_2d",Heat2dStencil_8order);


class Heat3dStencil_8order : public Heat2dStencil_8order {
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
        virtual void addPoints(GridValue& v, Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            Heat2dStencil_8order::addPoints(v, data, t, x, y, z);

            v += coeff( 1, 0, 0) * data(t, x+1, y, z ) +
                 coeff( 2, 0, 0) * data(t, x+2, y, z ) +
                 coeff( 3, 0, 0) * data(t, x+3, y, z ) +
                 coeff(-1, 0, 0) * data(t, x-1, y, z ) +
                 coeff(-2, 0, 0) * data(t, x-2, y, z ) +
                 coeff(-3, 0, 0) * data(t, x-3, y, z ) ;
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
            Heat2dStencil_8order::boundaryHandling(data, t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);
            data(t, x-1, y, z) EQUALS (data(t, x, y, z)*1.0)
                IF at_first_x;
            data(t, x+1, y, z) EQUALS (data(t, x, y, z)*1.0)
                IF at_last_x;
        }


    public:
        Heat3dStencil_8order(StencilList& stencils) :
            Heat2dStencil_8order("heat_8order_3d", stencils) { }
        Heat3dStencil_8order(const string& name, StencilList& stencils) :
            Heat2dStencil_8order(name, stencils) { }

};

YS_REGISTER_STENCIL("heat_8order_3d",Heat3dStencil_8order);

#endif
