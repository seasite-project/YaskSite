#ifndef YS_HEAT_UPDATE_STENCIL
#define YS_HEAT_UPDATE_STENCIL


//Implement various kinds of stencils required in the SeASiTe project.
//If new stencils are required update this file

class HeatBaseUpdate : public StencilBase {

    protected:

        // Generic time-varying spatial-3D grid.
        Grid data;

        //The coefficient (1/h)
        Param h_inv_sq;
        Param dt;

        // Return a coefficient.  Note: This returns completely fabricated
        // values only for illustrative purposes; they have no mathematical
        // significance.
        virtual double coeff(int di, int dj, int dk) const {
            return 1;
        }

        // Add additional points to expression v.
        virtual void addPoints(GridValue& v, GridIndex t, GridIndex x, GridIndex y, GridIndex z) =0;
        virtual void boundaryHandling(GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z) =0;


    public:

        HeatBaseUpdate(const string& name, StencilList& stencils) : StencilBase(name, stencils) {
            //can support max. upto 3D space dimension
            INIT_GRID_4D(data, t, x, y, z);
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


            // Add additional points from derived class.
            GridValue v;
            boundaryHandling(t,x,y,z, at_first_x, at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
            addPoints(v, t, x, y, z);

            //update operation
            v = data(t, x, y, z) + dt*h_inv_sq*v;
            // define the value at t+1 to be equivalent to v.
            data(t+1, x, y, z) EQUALS v;
        }
};

class Heat1dUpdateStencil : public HeatBaseUpdate {
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
        virtual void addPoints(GridValue& v, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            v += coeff( 0, 0, 0) * data(t, x, y  , z  );
            v += coeff( 0, 0, 1) * data(t, x, y, z+1  ) +
                 coeff( 0, 0,-1) * data(t, x, y, z-1  );

        }

        //Neumann boundary of first order
        virtual void boundaryHandling(GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
             //I do a hack here because unary '+' is not overloaded
            data(t, x, y, z-1) EQUALS (data(t, x, y, z)*1.0)
                IF at_first_z;

            //I do a hack here because unary '+' is not overloaded
            data(t, x, y, z+1) EQUALS (data(t, x, y, z)*1.0)
                IF at_last_z;
        }


    public:
        Heat1dUpdateStencil(StencilList& stencils) :
            HeatBaseUpdate("heat1d_update", stencils) { }
        Heat1dUpdateStencil(const string& name, StencilList& stencils) :
            HeatBaseUpdate(name, stencils) { }

};
YS_REGISTER_STENCIL("heat1d_update",Heat1dUpdateStencil);

class Heat2dUpdateStencil : public Heat1dUpdateStencil {
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
        virtual void addPoints(GridValue& v, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            Heat1dUpdateStencil::addPoints(v, t, x, y, z);
            v += coeff( 0, 1, 0) * data(t, x, y+1, z  ) +
                coeff( 0,-1, 0) * data(t, x, y-1, z  );

        }

        //Neumann boundary of first order
        virtual void boundaryHandling(GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
            Heat1dUpdateStencil::boundaryHandling(t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);

            data(t, x, y-1, z) EQUALS (data(t, x, y, z)*1.0)
                IF at_first_y;

            data(t, x, y+1, z) EQUALS (data(t, x, y, z)*1.0)
                IF at_last_y;
        }


    public:
        Heat2dUpdateStencil(StencilList& stencils) :
            Heat1dUpdateStencil("heat2d_update", stencils) { }
        Heat2dUpdateStencil(const string& name, StencilList& stencils) :
            Heat1dUpdateStencil(name, stencils) { }

};
YS_REGISTER_STENCIL("heat2d_update", Heat2dUpdateStencil);


class Heat3dUpdateStencil : public Heat2dUpdateStencil {
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

        // Add additional points to v.
        virtual void addPoints(GridValue& v, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            Heat2dUpdateStencil::addPoints(v, t, x, y, z);

            v += coeff( 1, 0, 0) * data(t, x+1, y, z ) +
                coeff(-1, 0, 0) * data(t, x-1, y, z );
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
            Heat2dUpdateStencil::boundaryHandling(t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);
            data(t, x-1, y, z) EQUALS (data(t, x, y, z)*1.0)
                IF at_first_x;
            data(t, x+1, y, z) EQUALS (data(t, x, y, z)*1.0)
                IF at_last_x;
        }


    public:
        Heat3dUpdateStencil(StencilList& stencils) :
            Heat2dUpdateStencil("heat3d_update", stencils) { }
        Heat3dUpdateStencil(const string& name, StencilList& stencils) :
            Heat2dUpdateStencil(name, stencils) { }

};

YS_REGISTER_STENCIL("heat3d_update", Heat3dUpdateStencil);

#endif
