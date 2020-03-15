#ifndef YS_WAVE_offsite_STENCIL
#define YS_WAVE_offsite_STENCIL

//Implement various kinds of stencils required in the SeASiTe project.
//If new stencils are required update this file
#include "StencilBase.hpp"

class WaveBase_offsite : public StencilRadiusBase {

    protected:

        // Generic time-varying spatial-3D grid.
        Grid data;
        Grid vel;
        Grid prev_data;

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

        WaveBase_offsite(const string& name, StencilList& stencils, int radius=4) : StencilRadiusBase(name, stencils, radius) {
            //can support max. upto 3D space dimension
            INIT_GRID_4D(data, t, x, y, z);
            INIT_GRID_4D(vel, t, x, y, z);
            INIT_GRID_4D(prev_data, t, x, y, z);
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
            points(v, data, t, x, y, z);
            // define the value at t+1 to be equivalent to v.
            double c_0 = 0.1;
            GridValue tmp;
            tmp = c_0*data(t,x,y,z) + v;
            data(t+1, x, y, z) EQUALS 2*data(t,x,y,z) - prev_data(t,x,y,z) + vel(t,x,y,z)*tmp;
        }
};

class Wave1dStencil_offsite : public WaveBase_offsite {
    protected:
        virtual double coeff(int dr) const
        {
            return 1/(double)dr;
        }

        // Add additional points to v.
        virtual void points(GridValue& v, Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            for(int r=_radius; r >= 1; --r)
            {
                v += coeff(r) * (data(t, x, y, z+r  ) +  data(t, x, y, z-r  ));
            }

        }

        virtual void addPoints(GridValue& v, Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            points(v, data, t, x, y, z);
            double c_0 = 0.1;
            GridValue tmp;
            tmp = c_0*data(t,x,y,z) + v;
            v += 2*data(t,x,y,z) - prev_data(t,x,y,z) + vel(t,x,y,z)*tmp;
 
        }
        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
             //I do a hack here because unary '+' is not overloaded, adding
             //another term data(t,x,y,z) is also a hack to show dependency to
             //stencil compiler
            data(t, x, y, z) EQUALS (data(t, x, y, z+1)*1.0)
                IF at_first_z;

            //I do a hack here because unary '+' is not overloaded
            data(t, x, y, z) EQUALS (data(t, x, y, z-1)*1.0)
                IF at_last_z;
        }


    public:
        Wave1dStencil_offsite(StencilList& stencils, int radius=4) :
            WaveBase_offsite("Wave1D", stencils, radius) { }
        Wave1dStencil_offsite(const string& name, StencilList& stencils, int radius=4) :
            WaveBase_offsite(name, stencils, radius) { }

};
YS_REGISTER_STENCIL_RADIUS("Wave1D",Wave1dStencil_offsite);

class Wave2dStencil_offsite : public Wave1dStencil_offsite {
    protected:
        virtual double coeff(int dr) const
        {
            return 1/(double)dr;
        }

        // Add additional points to v.
        virtual void points(GridValue& v, Grid &data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            for(int r=_radius; r>=1; --r)
            {
                v += coeff( r ) * (data(t, x, y+r, z  ) + data(t, x, y-r, z  ));
            }
            Wave1dStencil_offsite::points(v, data, t, x, y, z);
        }

        virtual void addPoints(GridValue& v, Grid &data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            points(v, data, t, x, y, z);
            double c_0 = 0.1;
            GridValue tmp;
            tmp = c_0*data(t,x,y,z) + v;
            v += 2*data(t,x,y,z) - prev_data(t,x,y,z) + vel(t,x,y,z)*tmp;
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
        /*    Wave1dStencil_offsite::boundaryHandling(data, t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);

            data(t, x, y, z) EQUALS (data(t,x,y+1,z)*1.0)
                IF at_first_y;

            data(t, x, y, z) EQUALS (data(t,x,y-1,z)*1.0)
                IF at_last_y;
                */
        }


    public:
        Wave2dStencil_offsite(StencilList& stencils, int radius=4) :
            Wave1dStencil_offsite("Wave2D", stencils, radius) { }
        Wave2dStencil_offsite(const string& name, StencilList& stencils, int radius=4) :
            Wave1dStencil_offsite(name, stencils, radius) { }

};
YS_REGISTER_STENCIL_RADIUS("Wave2D",Wave2dStencil_offsite);


class Wave3dStencil_offsite : public Wave2dStencil_offsite {
    protected:
        virtual double coeff(int dr) const
        {
            return 1/(double)dr;
        }

        // Add additional points to v.
        virtual void points(GridValue& v, Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            for(int r=_radius; r>=1; --r)
            {
                v += coeff( r ) * (data(t, x+r, y, z ) + data(t, x-r, y, z ));
            }
            Wave2dStencil_offsite::points(v, data, t, x, y, z);
        }

        virtual void addPoints(GridValue& v, Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z)
        {
            points(v, data, t, x, y, z);
            double c_0 = 0.1;
            GridValue tmp;
            tmp = c_0*data(t,x,y,z) + v;
            v += 2*data(t,x,y,z) - prev_data(t,x,y,z) + vel(t,x,y,z)*tmp;
        }

        //Neumann boundary of first order
        virtual void boundaryHandling(Grid& data, GridIndex t, GridIndex x, GridIndex y, GridIndex z, Condition at_first_x, Condition at_last_x, Condition at_first_y, Condition at_last_y, Condition at_first_z, Condition at_last_z)
        {
            /*
            Wave2dStencil_offsite::boundaryHandling(data, t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);
            data(t, x, y, z) EQUALS (data(t,x+1,y,z)*1.0)
                IF at_first_x;
            data(t, x, y, z) EQUALS (data(t,x-1,y,z)*1.0)
                IF at_last_x;
                */
        }


    public:
        Wave3dStencil_offsite(StencilList& stencils, int radius=4) :
            Wave2dStencil_offsite("Wave3D", stencils, radius) { }
        Wave3dStencil_offsite(const string& name, StencilList& stencils, int radius=4) :
            Wave2dStencil_offsite(name, stencils, radius) { }

};

YS_REGISTER_STENCIL_RADIUS("Wave3D",Wave3dStencil_offsite);

#endif
