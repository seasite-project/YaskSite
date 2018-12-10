/*****************************************************************************

YASK: Yet Another Stencil Kernel
Copyright (c) 2014-2017, Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

*****************************************************************************/

// Stencil equations for FSG elastic numerics.
// Contributed by Albert Farres from the Barcelona Supercomputing Center

#include "StencilBase.hpp"
#include "ElasticStencil/ElasticStencil.hpp"

namespace fsg {

    class FSG_ABC;

    class FSGBoundaryCondition : public ElasticBoundaryCondition
    {
    public:
        virtual void velocity (GridIndex t, GridIndex x, GridIndex y, GridIndex z ) {}
        virtual void stress (GridIndex t, GridIndex x, GridIndex y, GridIndex z ) {}
    };

    class FSGElasticStencilBase : public ElasticStencilBase {
        friend FSG_ABC;

    protected:

        //struct V_Node {
        //Grid u, v, w;
        //};
        // Time-varying 3D-spatial velocity grids.
        //V_Node v_bl, v_br, v_tl, v_tr;

        Grid v_bl_u, v_bl_v, v_bl_w;
        Grid v_br_u, v_br_v, v_br_w;
        Grid v_tl_u, v_tl_v, v_tl_w;
        Grid v_tr_u, v_tr_v, v_tr_w;

        //struct S_Node {
        //Grid xx, yy, zz, xy, xz, yz;
        //};
        // Time-varying 3D-spatial Stress grids.
        //S_Node s_bl, s_br, s_tl, s_tr;

        Grid s_bl_xx, s_bl_yy, s_bl_zz, s_bl_xy, s_bl_xz, s_bl_yz;
        Grid s_br_xx, s_br_yy, s_br_zz, s_br_xy, s_br_xz, s_br_yz;
        Grid s_tl_xx, s_tl_yy, s_tl_zz, s_tl_xy, s_tl_xz, s_tl_yz;
        Grid s_tr_xx, s_tr_yy, s_tr_zz, s_tr_xy, s_tr_xz, s_tr_yz;

        // 3D-spatial coefficients.
        Grid c11,c12,c13,c14,c15,c16;
        Grid     c22,c23,c24,c25,c26;
        Grid         c33,c34,c35,c36;
        Grid             c44,c45,c46;
        Grid                 c55,c56;
        Grid                     c66;

    public:

        FSGElasticStencilBase( const string &name, StencilList& stencils ) :
            ElasticStencilBase ( name, stencils )
        {
            initGrids();
        }
    
        FSGElasticStencilBase( const string &name, FSGBoundaryCondition *bc, StencilList& stencils ) :
            ElasticStencilBase ( name, bc, stencils )
        {
            initGrids();
        }

        void initGrids ()
        {
            // Specify the dimensions of each grid.
            // (This names the dimensions; it does not specify their sizes.)
            INIT_GRID_4D(v_bl_u, t, x, y, z);
            INIT_GRID_4D(v_bl_v, t, x, y, z);
            INIT_GRID_4D(v_bl_w, t, x, y, z);
            INIT_GRID_4D(v_br_u, t, x, y, z);
            INIT_GRID_4D(v_br_v, t, x, y, z);
            INIT_GRID_4D(v_br_w, t, x, y, z);
            INIT_GRID_4D(v_tl_u, t, x, y, z);
            INIT_GRID_4D(v_tl_v, t, x, y, z);
            INIT_GRID_4D(v_tl_w, t, x, y, z);
            INIT_GRID_4D(v_tr_u, t, x, y, z);
            INIT_GRID_4D(v_tr_v, t, x, y, z);
            INIT_GRID_4D(v_tr_w, t, x, y, z);
            INIT_GRID_4D(s_bl_xx, t, x, y, z);
            INIT_GRID_4D(s_bl_yy, t, x, y, z);
            INIT_GRID_4D(s_bl_zz, t, x, y, z);
            INIT_GRID_4D(s_bl_yz, t, x, y, z);
            INIT_GRID_4D(s_bl_xz, t, x, y, z);
            INIT_GRID_4D(s_bl_xy, t, x, y, z);
            INIT_GRID_4D(s_br_xx, t, x, y, z);
            INIT_GRID_4D(s_br_yy, t, x, y, z);
            INIT_GRID_4D(s_br_zz, t, x, y, z);
            INIT_GRID_4D(s_br_yz, t, x, y, z);
            INIT_GRID_4D(s_br_xz, t, x, y, z);
            INIT_GRID_4D(s_br_xy, t, x, y, z);
            INIT_GRID_4D(s_tl_xx, t, x, y, z);
            INIT_GRID_4D(s_tl_yy, t, x, y, z);
            INIT_GRID_4D(s_tl_zz, t, x, y, z);
            INIT_GRID_4D(s_tl_yz, t, x, y, z);
            INIT_GRID_4D(s_tl_xz, t, x, y, z);
            INIT_GRID_4D(s_tl_xy, t, x, y, z);
            INIT_GRID_4D(s_tr_xx, t, x, y, z);
            INIT_GRID_4D(s_tr_yy, t, x, y, z);
            INIT_GRID_4D(s_tr_zz, t, x, y, z);
            INIT_GRID_4D(s_tr_yz, t, x, y, z);
            INIT_GRID_4D(s_tr_xz, t, x, y, z);
            INIT_GRID_4D(s_tr_xy, t, x, y, z);
            INIT_GRID_3D(rho, x, y, z);
            INIT_GRID_3D(c11, x, y, z);
            INIT_GRID_3D(c12, x, y, z);
            INIT_GRID_3D(c13, x, y, z);
            INIT_GRID_3D(c14, x, y, z);
            INIT_GRID_3D(c15, x, y, z);
            INIT_GRID_3D(c16, x, y, z);
            INIT_GRID_3D(c22, x, y, z);
            INIT_GRID_3D(c23, x, y, z);
            INIT_GRID_3D(c24, x, y, z);
            INIT_GRID_3D(c25, x, y, z);
            INIT_GRID_3D(c26, x, y, z);
            INIT_GRID_3D(c33, x, y, z);
            INIT_GRID_3D(c34, x, y, z);
            INIT_GRID_3D(c35, x, y, z);
            INIT_GRID_3D(c36, x, y, z);
            INIT_GRID_3D(c44, x, y, z);
            INIT_GRID_3D(c45, x, y, z);
            INIT_GRID_3D(c46, x, y, z);
            INIT_GRID_3D(c55, x, y, z);
            INIT_GRID_3D(c56, x, y, z);
            INIT_GRID_3D(c66, x, y, z);

            // StencilContex specific code
            REGISTER_STENCIL_CONTEXT_EXTENSION
                (
                 virtual void initData() {
                     initDiff();
                 } );
        }

        GridValue cell_coeff( const GridIndex x, const GridIndex y, const GridIndex z, Grid &c, const BR )
        {
            return  1.0 / (0.25*(c(x  , y  , z  ) +
                                 c(x  , y+1, z  ) +
                                 c(x  , y  , z+1) +
                                 c(x  , y+1, z+1)));
        }
        GridValue cell_coeff( const GridIndex x, const GridIndex y, const GridIndex z, Grid &c, const BL )
        {
            return  1.0 / (0.25*(c(x  , y  , z  ) +
                                 c(x+1, y  , z  ) +
                                 c(x  , y  , z+1) +
                                 c(x+1, y  , z+1)));
        }
        GridValue cell_coeff( const GridIndex x, const GridIndex y, const GridIndex z, Grid &c, const TR )
        {
            return  1.0 / (0.25*(c(x  , y  , z  ) +
                                 c(x  , y+1, z  ) +
                                 c(x+1, y  , z  ) +
                                 c(x+1, y+1, z  )));
        }
        GridValue cell_coeff( const GridIndex x, const GridIndex y, const GridIndex z, Grid &c, const TL )
        {
            return  1.0 /        c(x  , y  , z  );
        }
        template<typename N>
        GridValue cell_coeff( const GridIndex x, const GridIndex y, const GridIndex z, Grid &c )
        {
            return cell_coeff( x, y, z, c, N() );
        }

        GridValue cell_coeff_artm( const GridIndex x, const GridIndex y, const GridIndex z, Grid &c, const BR )
        {
            return 0.25 *( 1.0 / c(x  , y  , z  ) +
                           1.0 / c(x  , y+1, z  ) +
                           1.0 / c(x  , y  , z+1) +
                           1.0 / c(x  , y+1, z+1) );
        }
        GridValue cell_coeff_artm( const GridIndex x, const GridIndex y, const GridIndex z, Grid &c, const BL )
        {
            return 0.25 *( 1.0 / c(x  , y  , z  ) +
                           1.0 / c(x+1, y  , z  ) +
                           1.0 / c(x  , y  , z+1) +
                           1.0 / c(x+1, y  , z+1) );
        }
        GridValue cell_coeff_artm( const GridIndex x, const GridIndex y, const GridIndex z, Grid &c, const TR )
        {
            return 0.25 *( 1.0 / c(x  , y  , z  ) +
                           1.0 / c(x  , y+1, z  ) +
                           1.0 / c(x+1, y  , z  ) +
                           1.0 / c(x+1, y+1, z  ) );
        }
        GridValue cell_coeff_artm( const GridIndex x, const GridIndex y, const GridIndex z, Grid &c, const TL )
        {
            return  1.0 /        c(x  , y  , z  );
        }
        template<typename N>
        GridValue cell_coeff_artm( const GridIndex x, const GridIndex y, const GridIndex z, Grid &c )
        {
            return cell_coeff_artm( x, y, z, c, N() );
        }

        GridValue stress_update( GridValue c1, GridValue c2, GridValue c3,
                                 GridValue c4, GridValue c5, GridValue c6, 
                                 GridValue u_z, GridValue u_y, GridValue u_x,
                                 GridValue v_z, GridValue v_y, GridValue v_x,
                                 GridValue w_z, GridValue w_y, GridValue w_x )
        {
            return delta_t * c1 * u_x
                + delta_t * c2 * v_y
                + delta_t * c3 * w_z
                + delta_t * c4 * (w_y + v_z)
                + delta_t * c5 * (w_x + u_z)
                + delta_t * c6 * (v_x + u_y);
        }

        //
        // Stress-grid define functions.  For each D in xx, yy, zz, xy, xz, yz,
        // define stress_D at t+1 based on stress_D at t and vel grids at t+1.
        // This implies that the velocity-grid define functions must be called
        // before these for a given value of t.  Note that the t, x, y, z
        // parameters are integer grid indices, not actual offsets in time or
        // space, so half-steps due to staggered grids are adjusted
        // appropriately.

        template<typename N, typename SZ, typename SX, typename SY>
        void define_str(GridIndex t, GridIndex x, GridIndex y, GridIndex z, 
                        Grid &sxx, Grid &syy, Grid &szz, Grid &sxy, Grid &sxz, Grid &syz,
                        Grid &vxu, Grid &vxv, Grid &vxw, Grid &vyu, Grid &vyv, Grid &vyw, Grid &vzu, Grid &vzv, Grid &vzw ) {

            // Interpolate coeffs.
            GridValue ic11 = cell_coeff     <N>(x, y, z, c11);
            GridValue ic12 = cell_coeff     <N>(x, y, z, c12);
            GridValue ic13 = cell_coeff     <N>(x, y, z, c13);
            GridValue ic14 = cell_coeff_artm<N>(x, y, z, c14);
            GridValue ic15 = cell_coeff_artm<N>(x, y, z, c15);
            GridValue ic16 = cell_coeff_artm<N>(x, y, z, c16);
            GridValue ic22 = cell_coeff     <N>(x, y, z, c22);
            GridValue ic23 = cell_coeff     <N>(x, y, z, c23);
            GridValue ic24 = cell_coeff_artm<N>(x, y, z, c24);
            GridValue ic25 = cell_coeff_artm<N>(x, y, z, c25);
            GridValue ic26 = cell_coeff_artm<N>(x, y, z, c26);
            GridValue ic33 = cell_coeff     <N>(x, y, z, c33);
            GridValue ic34 = cell_coeff_artm<N>(x, y, z, c34);
            GridValue ic35 = cell_coeff_artm<N>(x, y, z, c35);
            GridValue ic36 = cell_coeff_artm<N>(x, y, z, c36);
            GridValue ic44 = cell_coeff     <N>(x, y, z, c44);
            GridValue ic45 = cell_coeff_artm<N>(x, y, z, c45);
            GridValue ic46 = cell_coeff_artm<N>(x, y, z, c46);
            GridValue ic55 = cell_coeff     <N>(x, y, z, c55);
            GridValue ic56 = cell_coeff_artm<N>(x, y, z, c56);
            GridValue ic66 = cell_coeff     <N>(x, y, z, c66);

            // Compute stencils. Note that we are using the velocity values at t+1.
            GridValue u_z = stencil_O8<Z,SZ>( t+1, x, y, z, vzu );
            GridValue v_z = stencil_O8<Z,SZ>( t+1, x, y, z, vzv );
            GridValue w_z = stencil_O8<Z,SZ>( t+1, x, y, z, vzw );

            GridValue u_x = stencil_O8<X,SX>( t+1, x, y, z, vxu );
            GridValue v_x = stencil_O8<X,SX>( t+1, x, y, z, vxv );
            GridValue w_x = stencil_O8<X,SX>( t+1, x, y, z, vxw );

            GridValue u_y = stencil_O8<Y,SY>( t+1, x, y, z, vyu );
            GridValue v_y = stencil_O8<Y,SY>( t+1, x, y, z, vyv );
            GridValue w_y = stencil_O8<Y,SY>( t+1, x, y, z, vyw );

            // Compute next stress value
            GridValue next_sxx = sxx(t, x, y, z) +
                stress_update(ic11,ic12,ic13,ic14,ic15,ic16,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y);
            GridValue next_syy = syy(t, x, y, z) +
                stress_update(ic12,ic22,ic23,ic24,ic25,ic26,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y);
            GridValue next_szz = szz(t, x, y, z) +
                stress_update(ic13,ic23,ic33,ic34,ic35,ic36,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y);
            GridValue next_syz = syz(t, x, y, z) +
                stress_update(ic14,ic24,ic34,ic44,ic45,ic46,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y);
            GridValue next_sxz = sxz(t, x, y, z) +
                stress_update(ic15,ic25,ic35,ic45,ic55,ic56,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y);
            GridValue next_sxy = sxy(t, x, y, z) +
                stress_update(ic16,ic26,ic36,ic46,ic56,ic66,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y);

            // define the value at t+1.
            if ( hasBoundaryCondition ) {            
                Condition not_at_bc = bc->is_not_at_boundary(t,x,y,z);
                sxx(t+1, x, y, z) EQUALS next_sxx IF not_at_bc;
                syy(t+1, x, y, z) EQUALS next_syy IF not_at_bc;
                szz(t+1, x, y, z) EQUALS next_szz IF not_at_bc;
                syz(t+1, x, y, z) EQUALS next_syz IF not_at_bc;
                sxz(t+1, x, y, z) EQUALS next_sxz IF not_at_bc;
                sxy(t+1, x, y, z) EQUALS next_sxy IF not_at_bc;
            } else {
                sxx(t+1, x, y, z) EQUALS next_sxx;
                syy(t+1, x, y, z) EQUALS next_syy;
                szz(t+1, x, y, z) EQUALS next_szz;
                syz(t+1, x, y, z) EQUALS next_syz;
                sxz(t+1, x, y, z) EQUALS next_sxz;
                sxy(t+1, x, y, z) EQUALS next_sxy;
            }
        }


        // Call all the define_* functions.
        virtual void define(const IntTuple& offsets) {
            GET_OFFSET(t);
            GET_OFFSET(x);
            GET_OFFSET(y);
            GET_OFFSET(z);
        
            FSGBoundaryCondition &fsg_bc = *static_cast<FSGBoundaryCondition *>(bc);

            // Define velocity components.
            define_vel<TL, B, F, B>(t, x, y, z, v_tl_w, s_tl_yz, s_tr_xz, s_bl_zz);
            define_vel<TR, B, B, F>(t, x, y, z, v_tr_w, s_tr_yz, s_tl_xz, s_br_zz);
            define_vel<BL, F, B, B>(t, x, y, z, v_bl_w, s_bl_yz, s_br_xz, s_tl_zz);
            define_vel<BR, F, F, F>(t, x, y, z, v_br_w, s_br_yz, s_bl_xz, s_tr_zz);
            define_vel<TL, B, F, B>(t, x, y, z, v_tl_u, s_tl_xy, s_tr_xx, s_bl_xz);
            define_vel<TR, B, B, F>(t, x, y, z, v_tr_u, s_tr_xy, s_tl_xx, s_br_xz);
            define_vel<BL, F, B, B>(t, x, y, z, v_bl_u, s_bl_xy, s_br_xx, s_tl_xz);
            define_vel<BR, F, F, F>(t, x, y, z, v_br_u, s_br_xy, s_bl_xx, s_tr_xz);
            define_vel<TL, B, F, B>(t, x, y, z, v_tl_v, s_tl_yy, s_tr_xy, s_bl_yz);
            define_vel<TR, B, B, F>(t, x, y, z, v_tr_v, s_tr_yy, s_tl_xy, s_br_yz);
            define_vel<BL, F, B, B>(t, x, y, z, v_bl_v, s_bl_yy, s_br_xy, s_tl_yz);
            define_vel<BR, F, F, F>(t, x, y, z, v_br_v, s_br_yy, s_bl_xy, s_tr_yz);
        
            if ( hasBoundaryCondition )
                fsg_bc.velocity(t,x,y,z);

            //// Define stresses components.
            define_str<BR, F, B, F>(t, x, y, z, s_br_xx, s_br_yy, s_br_zz, s_br_xy, s_br_xz, s_br_yz,
                                    v_br_u, v_br_v, v_br_w, v_bl_u, v_bl_v, v_bl_w, v_tr_u, v_tr_v, v_tr_w);
            define_str<BL, F, F, B>(t, x, y, z, s_bl_xx, s_bl_yy, s_bl_zz, s_bl_xy, s_bl_xz, s_bl_yz,
                                    v_bl_u, v_bl_v, v_bl_w, v_br_u, v_br_v, v_br_w, v_tl_u, v_tl_v, v_tl_w);
            define_str<TR, B, F, F>(t, x, y, z, s_tr_xx, s_tr_yy, s_tr_zz, s_tr_xy, s_tr_xz, s_tr_yz,
                                    v_tr_u, v_tr_v, v_tr_w, v_tl_u, v_tl_v, v_tl_w, v_br_u, v_br_v, v_br_w);
            define_str<TL, B, B, B>(t, x, y, z, s_tl_xx, s_tl_yy, s_tl_zz, s_tl_xy, s_tl_xz, s_tl_yz,
                                    v_tl_u, v_tl_v, v_tl_w, v_tr_u, v_tr_v, v_tr_w, v_bl_u, v_bl_v, v_bl_w);
                               
            if ( hasBoundaryCondition )
                fsg_bc.stress(t,x,y,z);
        }
    };

    class FSG_ABC : public FSGBoundaryCondition
    {
        const int abc_width = 20;
    
        // Sponge coefficients.
        Grid sponge_lx;
        Grid sponge_rx;
        Grid sponge_bz;
        Grid sponge_tz;
        Grid sponge_fy;
        Grid sponge_by;
        Grid sponge_sq_lx;
        Grid sponge_sq_rx;
        Grid sponge_sq_bz;
        Grid sponge_sq_tz;
        Grid sponge_sq_fy;
        Grid sponge_sq_by;
    
        FSGElasticStencilBase &fsg;

    public:

        FSG_ABC (FSGElasticStencilBase &_fsg) : fsg(_fsg)
        {
            // fsg.INIT_GRID_3D is a hack on how the macro works. It can break at anytime
            fsg.INIT_GRID_3D(sponge_lx, x, y, z);
            fsg.INIT_GRID_3D(sponge_rx, x, y, z);
            fsg.INIT_GRID_3D(sponge_bz, x, y, z);
            fsg.INIT_GRID_3D(sponge_tz, x, y, z);
            fsg.INIT_GRID_3D(sponge_fy, x, y, z);
            fsg.INIT_GRID_3D(sponge_by, x, y, z);
            fsg.INIT_GRID_3D(sponge_sq_lx, x, y, z);
            fsg.INIT_GRID_3D(sponge_sq_rx, x, y, z);
            fsg.INIT_GRID_3D(sponge_sq_bz, x, y, z);
            fsg.INIT_GRID_3D(sponge_sq_tz, x, y, z);
            fsg.INIT_GRID_3D(sponge_sq_fy, x, y, z);
            fsg.INIT_GRID_3D(sponge_sq_by, x, y, z);        
        }

        Condition is_at_boundary( GridIndex t, GridIndex x, GridIndex y, GridIndex z ) 
        { 
            Condition bc = ( z < first_index(z)+abc_width || z > last_index(z)-abc_width ) ||
                ( y < first_index(y)+abc_width || y > last_index(y)-abc_width ) ||
                ( x < first_index(x)+abc_width || x > last_index(x)-abc_width ); 
            return bc; 
        }
        Condition is_not_at_boundary( GridIndex t, GridIndex x, GridIndex y, GridIndex z ) 
        { 
            return !is_at_boundary(t,x,y,z); 
        }

        template<typename N, typename SZ, typename SX, typename SY>
        void define_vel_abc(GridIndex t, GridIndex x, GridIndex y, GridIndex z, 
                            Grid &v, Grid &sx, Grid &sy, Grid &sz, 
                            Grid &abc_x, Grid &abc_y, Grid &abc_z, Grid &abc_sq_x, Grid &abc_sq_y, Grid &abc_sq_z) {

            Condition at_abc = is_at_boundary(t,x,y,z);

            GridValue next_v = v(t, x, y, z) * abc_x(x,y,z) * abc_y(x,y,z) * abc_z(x,y,z);

            GridValue lrho   = fsg.interp_rho<N>( x, y, z );

            GridValue stx    = fsg.stencil_O2_X<SX>( t, x, y, z, sx );
            GridValue sty    = fsg.stencil_O2_Y<SY>( t, x, y, z, sy );
            GridValue stz    = fsg.stencil_O2_Z<SZ>( t, x, y, z, sz );

            next_v += ((stx + sty + stz) * fsg.delta_t * lrho);
            next_v *= abc_sq_x(x,y,z) * abc_sq_y(x,y,z) * abc_sq_z(x,y,z);

            // define the value at t+1.
            v(t+1, x, y, z) EQUALS next_v IF at_abc;
        }    
    
        void velocity (GridIndex t, GridIndex x, GridIndex y, GridIndex z )
        {
            define_vel_abc<TL, B, F, B>(t, x, y, z, fsg.v_tl_w, fsg.s_tl_yz, fsg.s_tr_xz, fsg.s_bl_zz,
                                        sponge_lx, sponge_by, sponge_tz, sponge_sq_lx, sponge_sq_by, sponge_sq_tz);
            define_vel_abc<TR, B, B, F>(t, x, y, z, fsg.v_tr_w, fsg.s_tr_yz, fsg.s_tl_xz, fsg.s_br_zz,
                                        sponge_rx, sponge_fy, sponge_tz, sponge_sq_rx, sponge_sq_fy, sponge_sq_tz);
            define_vel_abc<BL, F, B, B>(t, x, y, z, fsg.v_bl_w, fsg.s_bl_yz, fsg.s_br_xz, fsg.s_tl_zz,
                                        sponge_lx, sponge_fy, sponge_bz, sponge_sq_lx, sponge_sq_fy, sponge_sq_bz);
            define_vel_abc<BR, F, F, F>(t, x, y, z, fsg.v_br_w, fsg.s_br_yz, fsg.s_bl_xz, fsg.s_tr_zz,
                                        sponge_rx, sponge_by, sponge_bz, sponge_sq_rx, sponge_sq_by, sponge_sq_bz);
            define_vel_abc<TL, B, F, B>(t, x, y, z, fsg.v_tl_u, fsg.s_tl_xy, fsg.s_tr_xx, fsg.s_bl_xz,
                                        sponge_lx, sponge_by, sponge_tz, sponge_sq_lx, sponge_sq_by, sponge_sq_tz);
            define_vel_abc<TR, B, B, F>(t, x, y, z, fsg.v_tr_u, fsg.s_tr_xy, fsg.s_tl_xx, fsg.s_br_xz,
                                        sponge_rx, sponge_fy, sponge_tz, sponge_sq_rx, sponge_sq_fy, sponge_sq_tz);
            define_vel_abc<BL, F, B, B>(t, x, y, z, fsg.v_bl_u, fsg.s_bl_xy, fsg.s_br_xx, fsg.s_tl_xz,
                                        sponge_lx, sponge_fy, sponge_bz, sponge_sq_lx, sponge_sq_fy, sponge_sq_bz);
            define_vel_abc<BR, F, F, F>(t, x, y, z, fsg.v_br_u, fsg.s_br_xy, fsg.s_bl_xx, fsg.s_tr_xz,
                                        sponge_rx, sponge_by, sponge_bz, sponge_sq_rx, sponge_sq_by, sponge_sq_bz);
            define_vel_abc<TL, B, F, B>(t, x, y, z, fsg.v_tl_v, fsg.s_tl_yy, fsg.s_tr_xy, fsg.s_bl_yz,
                                        sponge_lx, sponge_by, sponge_tz, sponge_sq_lx, sponge_sq_by, sponge_sq_tz);
            define_vel_abc<TR, B, B, F>(t, x, y, z, fsg.v_tr_v, fsg.s_tr_yy, fsg.s_tl_xy, fsg.s_br_yz,
                                        sponge_rx, sponge_fy, sponge_tz, sponge_sq_rx, sponge_sq_fy, sponge_sq_tz);
            define_vel_abc<BL, F, B, B>(t, x, y, z, fsg.v_bl_v, fsg.s_bl_yy, fsg.s_br_xy, fsg.s_tl_yz,
                                        sponge_lx, sponge_fy, sponge_bz, sponge_sq_lx, sponge_sq_fy, sponge_sq_bz);
            define_vel_abc<BR, F, F, F>(t, x, y, z, fsg.v_br_v, fsg.s_br_yy, fsg.s_bl_xy, fsg.s_tr_yz,
                                        sponge_rx, sponge_by, sponge_bz, sponge_sq_rx, sponge_sq_by, sponge_sq_bz);
        }

        template<typename N, typename SZ, typename SX, typename SY>
        void define_str_abc(GridIndex t, GridIndex x, GridIndex y, GridIndex z, 
                            Grid &sxx, Grid &syy, Grid &szz, Grid &sxy, Grid &sxz, Grid &syz,
                            Grid &vxu, Grid &vxv, Grid &vxw, Grid &vyu, Grid &vyv, Grid &vyw, Grid &vzu, Grid &vzv, Grid &vzw,
                            Grid &abc_x, Grid &abc_y, Grid &abc_z, Grid &abc_sq_x, Grid &abc_sq_y, Grid &abc_sq_z) {

            GridValue abc = abc_x(x,y,z) * abc_y(x,y,z) * abc_z(x,y,z);
            GridValue next_sxx = sxx(t, x, y, z) * abc;
            GridValue next_syy = syy(t, x, y, z) * abc;
            GridValue next_szz = szz(t, x, y, z) * abc;
            GridValue next_syz = syz(t, x, y, z) * abc;
            GridValue next_sxz = sxz(t, x, y, z) * abc;
            GridValue next_sxy = sxy(t, x, y, z) * abc;

            // Interpolate coeffs.
            GridValue ic11 = fsg.cell_coeff     <N>(x, y, z, fsg.c11);
            GridValue ic12 = fsg.cell_coeff     <N>(x, y, z, fsg.c12);
            GridValue ic13 = fsg.cell_coeff     <N>(x, y, z, fsg.c13);
            GridValue ic14 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c14);
            GridValue ic15 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c15);
            GridValue ic16 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c16);
            GridValue ic22 = fsg.cell_coeff     <N>(x, y, z, fsg.c22);
            GridValue ic23 = fsg.cell_coeff     <N>(x, y, z, fsg.c23);
            GridValue ic24 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c24);
            GridValue ic25 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c25);
            GridValue ic26 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c26);
            GridValue ic33 = fsg.cell_coeff     <N>(x, y, z, fsg.c33);
            GridValue ic34 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c34);
            GridValue ic35 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c35);
            GridValue ic36 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c36);
            GridValue ic44 = fsg.cell_coeff     <N>(x, y, z, fsg.c44);
            GridValue ic45 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c45);
            GridValue ic46 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c46);
            GridValue ic55 = fsg.cell_coeff     <N>(x, y, z, fsg.c55);
            GridValue ic56 = fsg.cell_coeff_artm<N>(x, y, z, fsg.c56);
            GridValue ic66 = fsg.cell_coeff     <N>(x, y, z, fsg.c66);

            // Compute stencils. Note that we are using the velocity values at t+1.
            GridValue u_z = fsg.stencil_O2_Z<SZ>( t+1, x, y, z, vzu );
            GridValue v_z = fsg.stencil_O2_Z<SZ>( t+1, x, y, z, vzv );
            GridValue w_z = fsg.stencil_O2_Z<SZ>( t+1, x, y, z, vzw );

            GridValue u_x = fsg.stencil_O2_X<SX>( t+1, x, y, z, vxu );
            GridValue v_x = fsg.stencil_O2_X<SX>( t+1, x, y, z, vxv );
            GridValue w_x = fsg.stencil_O2_X<SX>( t+1, x, y, z, vxw );

            GridValue u_y = fsg.stencil_O2_Y<SY>( t+1, x, y, z, vyu );
            GridValue v_y = fsg.stencil_O2_Y<SY>( t+1, x, y, z, vyv );
            GridValue w_y = fsg.stencil_O2_Y<SY>( t+1, x, y, z, vyw );

            // Compute next stress value
            GridValue abc_sq = abc_sq_x(x,y,z) * abc_sq_y(x,y,z) * abc_sq_z(x,y,z);
            next_sxx += fsg.stress_update(ic11,ic12,ic13,ic14,ic15,ic16,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y) * abc_sq;
            next_syy += fsg.stress_update(ic12,ic22,ic23,ic24,ic25,ic26,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y) * abc_sq;
            next_szz += fsg.stress_update(ic13,ic23,ic33,ic34,ic35,ic36,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y) * abc_sq;
            next_syz += fsg.stress_update(ic14,ic24,ic34,ic44,ic45,ic46,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y) * abc_sq;
            next_sxz += fsg.stress_update(ic15,ic25,ic35,ic45,ic55,ic56,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y) * abc_sq;
            next_sxy += fsg.stress_update(ic16,ic26,ic36,ic46,ic56,ic66,u_z,u_x,u_y,v_z,v_x,v_y,w_z,w_x,w_y) * abc_sq;

            // define the value at t+1.
            Condition at_abc = is_at_boundary(t,x,y,z);
            sxx(t+1, x, y, z) EQUALS next_sxx IF at_abc;
            syy(t+1, x, y, z) EQUALS next_syy IF at_abc;
            szz(t+1, x, y, z) EQUALS next_szz IF at_abc;
            syz(t+1, x, y, z) EQUALS next_syz IF at_abc;
            sxz(t+1, x, y, z) EQUALS next_sxz IF at_abc;
            sxy(t+1, x, y, z) EQUALS next_sxy IF at_abc;
        }
        
        void stress (GridIndex t, GridIndex x, GridIndex y, GridIndex z )
        {
            define_str_abc<BR, F, B, F>(t, x, y, z, fsg.s_br_xx, fsg.s_br_yy, fsg.s_br_zz, fsg.s_br_xy, fsg.s_br_xz, 
                                        fsg.s_br_yz, fsg.v_br_u,  fsg.v_br_v,  fsg.v_br_w,  fsg.v_bl_u, 
                                        fsg.v_bl_v,  fsg.v_bl_w,  fsg.v_tr_u,  fsg.v_tr_v,  fsg.v_tr_w, 
                                        sponge_rx, sponge_by, sponge_bz, sponge_sq_rx, sponge_sq_by, sponge_sq_bz);        
            define_str_abc<BL, F, F, B>(t, x, y, z, fsg.s_bl_xx, fsg.s_bl_yy, fsg.s_bl_zz, fsg.s_bl_xy, fsg.s_bl_xz, 
                                        fsg.s_bl_yz, fsg.v_bl_u,  fsg.v_bl_v,  fsg.v_bl_w,  fsg.v_br_u, 
                                        fsg.v_br_v,  fsg.v_br_w,  fsg.v_tl_u,  fsg.v_tl_v,  fsg.v_tl_w, 
                                        sponge_lx, sponge_fy, sponge_bz, sponge_sq_lx, sponge_sq_fy, sponge_sq_bz);
            define_str_abc<TR, B, F, F>(t, x, y, z, fsg.s_tr_xx, fsg.s_tr_yy, fsg.s_tr_zz, fsg.s_tr_xy, fsg.s_tr_xz, 
                                        fsg.s_tr_yz, fsg.v_tr_u,  fsg.v_tr_v,  fsg.v_tr_w,  fsg.v_tl_u, 
                                        fsg.v_tl_v,  fsg.v_tl_w,  fsg.v_br_u,  fsg.v_br_v,  fsg.v_br_w, 
                                        sponge_rx, sponge_fy, sponge_tz, sponge_sq_rx, sponge_sq_fy, sponge_sq_tz);
            define_str_abc<TL, B, B, B>(t, x, y, z, fsg.s_tl_xx, fsg.s_tl_yy, fsg.s_tl_zz, fsg.s_tl_xy, fsg.s_tl_xz, 
                                        fsg.s_tl_yz, fsg.v_tl_u,  fsg.v_tl_v,  fsg.v_tl_w,  fsg.v_tr_u, 
                                        fsg.v_tr_v,  fsg.v_tr_w,  fsg.v_bl_u,  fsg.v_bl_v,  fsg.v_bl_w, 
                                        sponge_lx, sponge_by, sponge_tz, sponge_sq_lx, sponge_sq_by, sponge_sq_tz);
        }
    
    };


    struct FSGElasticStencil : public FSGElasticStencilBase {
        FSGElasticStencil(StencilList& stencils) :
            FSGElasticStencilBase("fsg", stencils) { }
    };

    struct FSGABCElasticStencil : public FSGElasticStencilBase {
        FSG_ABC abc; // Absorbing Boundary Condition
    
        FSGABCElasticStencil(StencilList& stencils) :
            FSGElasticStencilBase("fsg_abc", &abc, stencils),
            abc(*this) { }
    };

    REGISTER_STENCIL(FSGElasticStencil);
    REGISTER_STENCIL(FSGABCElasticStencil);

}
