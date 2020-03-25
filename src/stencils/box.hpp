#ifndef _BOX_H
#define _BOX_H

//Implement various kinds of stencils required in the SeASiTe project.
//If new stencils are required update this file
#include "yask_compiler_api.hpp"
using namespace std;
using namespace yask;

namespace {

    class BoxBase : public yc_solution_base {

        protected:
            // Indices & dimensions.
            yc_index_node_ptr t = new_step_index("t");           // step in time dim.
            yc_index_node_ptr x = new_domain_index("x");         // spatial dim.
            yc_index_node_ptr y = new_domain_index("y");         // spatial dim.
            yc_index_node_ptr z = new_domain_index("z");         // spatial dim.

            // Vars.
            yc_var_proxy data =
                yc_var_proxy("data", get_soln(), { t, x, y, z }); // time-varying 3D var.

            yc_var_proxy h_inv =
                yc_var_proxy("h_inv", get_soln()); // time-varying 3D var.


            // Return a coefficient.  Note: This returns completely fabricated
            // values only for illustrative purposes; they have no mathematical
            // significance.
            virtual double coeff(int di, int dj, int dk) const {
                return 1;
            }

            virtual void points(yc_number_node_ptr& v, yc_var_proxy& grid, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z) =0;
            virtual void addPoints(yc_number_node_ptr& v, yc_var_proxy& grid, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z) =0;
            virtual void boundaryHandling(yc_var_proxy& grid, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z, const yc_bool_node_ptr& at_first_x, const yc_bool_node_ptr& at_last_x, const yc_bool_node_ptr& at_first_y, const yc_bool_node_ptr& at_last_y, const yc_bool_node_ptr& at_first_z, const yc_bool_node_ptr& at_last_z) =0;
        public:

            BoxBase(const string& name) : yc_solution_base(name) {
                //can support max. upto 3D space dimension
            }

            // Define equation at t+1 based on values at t.
            virtual void define() {

                // A condition that is true when index 'x','y'or 'z' is at boundary.
                // Normally bounding box calculation doesn't go to halo, but I hack
                // it so that the same stencil can be used for all vector foldings
                // without the problem of unimplemented unaligned writes
                const yc_bool_node_ptr& at_first_x = (x < first_domain_index(x));
                const yc_bool_node_ptr& at_first_y = (y < first_domain_index(y));
                const yc_bool_node_ptr& at_first_z = (z < first_domain_index(z));

                const yc_bool_node_ptr& at_last_x = (x > last_domain_index(x));
                const yc_bool_node_ptr& at_last_y = (y > last_domain_index(y));
                const yc_bool_node_ptr& at_last_z = (z > last_domain_index(z));


                yc_number_node_ptr v;
                addPoints(v, data, t, x, y, z);
                // define the value at t+1 to be equivalent to v.
                data(t+1, x, y, z) EQUALS v;
                boundaryHandling(data, t,x,y,z, at_first_x, at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
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
            virtual void points(yc_number_node_ptr& v,  yc_var_proxy& data, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z)
            {
                v += coeff( 0, 0, 0) * data(t, x, y, z  );
                v += coeff( 0, 0, 1) * data(t, x, y, z+1) +
                    coeff( 0, 0,-1) * data(t, x, y, z-1);
            }

            virtual void addPoints(yc_number_node_ptr& v, yc_var_proxy& data, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z)
            {
                points(v, data, t, x, y, z);
                //v *= h_inv;
            }

            //Neumann boundary of first order
            virtual void boundaryHandling(yc_var_proxy& data, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z, const yc_bool_node_ptr& at_first_x, const yc_bool_node_ptr& at_last_x, const yc_bool_node_ptr& at_first_y, const yc_bool_node_ptr& at_last_y, const yc_bool_node_ptr& at_first_z, const yc_bool_node_ptr& at_last_z)
            {
                //I do a hack here because unary '+' is not overloaded, adding
                //another term data(t,x,y,z) is also a hack to show dependency to
                //stencil compiler
                data(t+2, x, y, z) EQUALS ( data(t+1, x, y, z+1)*1.0)
                    IF_DOMAIN at_first_z;

                //I do a hack here because unary '+' is not overloaded
                data(t+2, x, y, z) EQUALS ( data(t+1, x, y, z-1)*1.0)
                    IF_DOMAIN at_last_z;
            }


        public:
            Box1dStencil() :
                BoxBase("box1d") { }
            Box1dStencil(const string& name) :
                BoxBase(name) { }

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
            virtual void points(yc_number_node_ptr& v, yc_var_proxy& data, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z)
            {
                Box1dStencil::points(v, data, t, x, y-1, z);
                Box1dStencil::points(v, data, t, x, y, z);
                Box1dStencil::points(v, data, t, x, y+1, z);
            }

            virtual void addPoints(yc_number_node_ptr& v, yc_var_proxy& data, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z)
            {
                points(v, data, t, x, y, z);
                // v *= h_inv*h_inv;
            }

            //Neumann boundary of first order
            virtual void boundaryHandling(yc_var_proxy& data, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z, const yc_bool_node_ptr& at_first_x, const yc_bool_node_ptr& at_last_x, const yc_bool_node_ptr& at_first_y, const yc_bool_node_ptr& at_last_y, const yc_bool_node_ptr& at_first_z, const yc_bool_node_ptr& at_last_z)
            {
                Box1dStencil::boundaryHandling(data, t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);

                data(t+2, x, y, z) EQUALS (data(t+1,x,y+1,z)*1.0)
                    IF_DOMAIN at_first_y;

                data(t+2, x, y, z) EQUALS (data(t+1,x,y-1,z)*1.0)
                    IF_DOMAIN at_last_y;
            }


        public:
            Box2dStencil() :
                Box1dStencil("box2d") { }
            Box2dStencil(const string& name) :
                Box1dStencil(name) { }

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
            virtual void points(yc_number_node_ptr& v, yc_var_proxy& data, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z)
            {
                Box2dStencil::points(v, data, t, x-1, y, z);
                Box2dStencil::points(v, data, t, x+0, y, z);
                Box2dStencil::points(v, data, t, x+1, y, z);
            }

            virtual void addPoints(yc_number_node_ptr& v, yc_var_proxy& data, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z)
            {
                points(v, data, t, x, y, z);
                //    v*=h_inv*h_inv*h_inv;
            }

            //Neumann boundary of first order
            virtual void boundaryHandling(yc_var_proxy& data, const yc_number_node_ptr& t, const yc_number_node_ptr& x, const yc_number_node_ptr& y, const yc_number_node_ptr& z, const yc_bool_node_ptr& at_first_x, const yc_bool_node_ptr& at_last_x, const yc_bool_node_ptr& at_first_y, const yc_bool_node_ptr& at_last_y, const yc_bool_node_ptr& at_first_z, const yc_bool_node_ptr& at_last_z)
            {
                Box2dStencil::boundaryHandling(data, t,x,y,z,at_first_x,at_last_x,at_first_y,at_last_y,at_first_z,at_last_z);
                data(t+2, x, y, z) EQUALS (data(t+1,x+1,y,z)*1.0)
                    IF_DOMAIN at_first_x;
                data(t+2, x, y, z) EQUALS (data(t+1,x-1,y,z)*1.0)
                    IF_DOMAIN at_last_x;
            }


        public:
            Box3dStencil() :
                Box2dStencil("box3d") { }
            Box3dStencil(const string& name) :
                Box2dStencil(name) { }

    };

    YS_REGISTER_STENCIL("box3d",Box3dStencil);

}

#endif
