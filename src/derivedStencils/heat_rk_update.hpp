#ifndef YS_HEAT_RK_UPDATE_STENCIL
#define YS_HEAT_RK_UPDATE_STENCIL

//Implement various kinds of stencils required in the SeASiTe project.
//If new stencils are required update this file
#include "StencilBase.hpp"

class HeatRKBaseUpdate : public StencilBase {

    protected:

        // Generic time-varying spatial-3D grid.
        Grid data;
        Grid k1;
        Grid k2;

        //The coefficient (1/h)
        Param h_inv_sq;
        Param dt;

        //Butcher table parameters,
        int s;

        virtual double a(int x, int y)
        {
            if(x==1 && y==0)
                return 1.0;

            else if(x==2)
                if(y==0)
                    return 1/4.0;
                else if(y==1)
                    return 1/4.0;
                else
                    return 0.0;
            else
                return 0.0;
        }

        virtual double b(int x)
        {
            if(x==0 || x==1)
                return 1/2.0;
            else
                return 0.0;
        }

        //gets dimension of each stencil
        //required for proper settings
        virtual int getDim()=0;

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

        HeatRKBaseUpdate(const string& name, StencilList& stencils) : StencilBase(name, stencils), s(3) {
            //can support max. upto 3D space dimension
            INIT_GRID_4D(data, t, x, y, z);
            INIT_GRID_4D(k1, t, x, y, z);
            INIT_GRID_4D(k2, t, x, y, z);
            INIT_PARAM(h_inv_sq);
            INIT_PARAM(dt);
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

            GridValue k1_val;
            GridValue k2_val;
            GridValue k3_val;

            boundaryHandling(data, t,x,y,z, at_first_x, at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
            addPoints(k1_val, data, t, x, y, z);

            k1(t,x,y,z) EQUALS k1_val*h_inv_sq;

            boundaryHandling(k1, t,x,y,z, at_first_x, at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
            addPoints(k2_val, k1, t, x, y, z);

            k2(t,x,y,z) EQUALS k1(t,x,y,z) + dt*a(1,0)*h_inv_sq*k2_val;

            boundaryHandling(k2, t,x,y,z, at_first_x, at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
            addPoints(k3_val, k2, t, x, y, z);

            data(t+1,x,y,z) EQUALS data(t,x,y,z) + dt* (b(0)*k1(t,x,y,z) + b(1)*k2(t,x,y,z) + b(2)* (k1(t,x,y,z) + dt*(a(2,0)/a(1,0))*(k2(t,x,y,z)-k1(t,x,y,z)) + dt*a(2,1)*h_inv_sq*k3_val) );
        }
};

class Heat1dRKUpdateStencil : public HeatRKBaseUpdate {
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

        virtual int getDim()
        {
            return 1;
        }
        // Add additional points to v.
        virtual void addPoints(GridValue& v, Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            v += coeff( 0, 0, 0) * grid(t, x, y  , z  );
            v += coeff( 0, 0, 1) * grid(t, x, y, z+1  ) +
                 coeff( 0, 0,-1) * grid(t, x, y, z-1  );
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
            //I do a hack here because unary '+' is not overloaded
            grid(t, x, y, z-1) EQUALS (grid(t, x, y, z)*1.0)
                IF at_first_z;

            //I do a hack here because unary '+' is not overloaded
            grid(t, x, y, z+1) EQUALS (grid(t, x, y, z)*1.0)
                IF at_last_z;

        }


    public:
        Heat1dRKUpdateStencil(StencilList& stencils) :
            HeatRKBaseUpdate("heat1d_rk_update", stencils) { }
        Heat1dRKUpdateStencil(const string& name, StencilList& stencils) :
            HeatRKBaseUpdate(name, stencils) { }

};
YS_REGISTER_STENCIL("heat1d_rk_update", Heat1dRKUpdateStencil);

class Heat2dRKUpdateStencil : public Heat1dRKUpdateStencil {
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

        virtual int getDim()
        {
            return 2;
        }

        // Add additional points to v.
        virtual void addPoints(GridValue& v, Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            Heat1dRKUpdateStencil::addPoints(v, grid, t, x, y, z);
            v += coeff( 0, 1, 0) * grid(t, x, y+1, z  ) +
                coeff( 0,-1, 0) * grid(t, x, y-1, z  );
        }


        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
            Heat1dRKUpdateStencil::boundaryHandling(grid, t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);
            grid(t, x, y-1, z) EQUALS (grid(t, x, y, z)*1.0)
                IF at_first_y;
            grid(t, x, y+1, z) EQUALS (grid(t, x, y, z)*1.0)
                IF at_last_y;
        }

    public:
        Heat2dRKUpdateStencil(StencilList& stencils) :
            Heat1dRKUpdateStencil("heat2d_rk_update", stencils) { }
        Heat2dRKUpdateStencil(const string& name, StencilList& stencils) :
            Heat1dRKUpdateStencil(name, stencils) { }

};
YS_REGISTER_STENCIL("heat2d_rk_update", Heat2dRKUpdateStencil);


class Heat3dRKUpdateStencil : public Heat2dRKUpdateStencil {
    protected:
        virtual double coeff(int di, int dj, int dk) const
        {
            if((dj==0&&dk==0)&&(di==0))
            {
                return -6;
            }
            else
            {
                return 1;
            }
        }

        virtual int getDim()
        {
            return 3;
        }
        // Add additional points to v.
        virtual void addPoints(GridValue& v, Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            Heat2dRKUpdateStencil::addPoints(v, grid, t, x, y, z);

            v += coeff( 1, 0, 0) * grid(t, x+1, y, z ) +
                coeff(-1, 0, 0) * grid(t, x-1, y, z );
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& grid, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
               Heat2dRKUpdateStencil::boundaryHandling(grid, t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);
                grid(t, x-1, y, z) EQUALS (grid(t, x, y, z)*1.0)
                    IF at_first_x;
                grid(t, x+1, y, z) EQUALS (grid(t, x, y, z)*1.0)
                    IF at_last_x;
        }


    public:
        Heat3dRKUpdateStencil(StencilList& stencils) :
            Heat2dRKUpdateStencil("heat3d_rk_update", stencils) { }
};

YS_REGISTER_STENCIL("heat3d_rk_update",Heat3dRKUpdateStencil);

#endif
