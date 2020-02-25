/*****************************************************************************

YASK: Yet Another Stencil Kernel
Copyright (c) 2014-2016, Intel Corporation

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

// Stencil equations for SSG elastic numerics.

#include "ElasticStencil/ElasticStencil.hpp"

class SSGElasticStencil : public ElasticStencilBase {

protected:

    // Time-varying 3D-spatial velocity grids.
    Grid v_bl_w;
    Grid v_tl_v;
    Grid v_tr_u;

    // Time-varying 3D-spatial Stress grids.
    Grid s_tl_xx, s_tl_yy, s_tl_zz;
    Grid s_bl_yz;
    Grid s_br_xz;
    Grid s_tr_xy;

    // 3D-spatial coefficients.
    Grid mu;
    Grid lambda;
    Grid lambdamu2;

public:

    SSGElasticStencil( StencilList& stencils) :
        ElasticStencilBase("ssg", stencils)
    {
        // Specify the dimensions of each grid.
        // (This names the dimensions; it does not specify their sizes.)
        INIT_GRID_4D(v_bl_w, t, x, y, z);
        INIT_GRID_4D(v_tl_v, t, x, y, z);
        INIT_GRID_4D(v_tr_u, t, x, y, z);
        INIT_GRID_4D(s_bl_yz, t, x, y, z);
        INIT_GRID_4D(s_br_xz, t, x, y, z);
        INIT_GRID_4D(s_tl_xx, t, x, y, z);
        INIT_GRID_4D(s_tl_yy, t, x, y, z);
        INIT_GRID_4D(s_tl_zz, t, x, y, z);
        INIT_GRID_4D(s_tr_xy, t, x, y, z);
        INIT_GRID_3D(rho, x, y, z);
        INIT_GRID_3D(mu, x, y, z);
        INIT_GRID_3D(lambda, x, y, z);
        INIT_GRID_3D(lambdamu2, x, y, z);

        // Initialize the parameters (both are scalars).
        //INIT_PARAM(delta_t);
        //INIT_PARAM(dxi);
        //INIT_PARAM(dyi);
        //INIT_PARAM(dzi);

        // StencilContex specific code
        REGISTER_STENCIL_CONTEXT_EXTENSION(
           virtual void initData() {
               initDiff();
           }
        );
    }

    GridValue interp_mu( GridIndex x, GridIndex y, GridIndex z, const BR )
    {
        return ( 2.0/ (mu(x  , y  , z  ) +
                       mu(x  , y+1, z  ) +
                       mu(x  , y  , z+1) +
                       mu(x  , y+1, z+1)) );
    }

    GridValue interp_mu( GridIndex x, GridIndex y, GridIndex z, const BL )
    {
        return ( 2.0/ (mu(x  , y  , z  ) +
                       mu(x+1, y  , z  ) +
                       mu(x  , y  , z+1) +
                       mu(x+1, y  , z+1)) );
    }

    GridValue interp_mu( GridIndex x, GridIndex y, GridIndex z, const TR )
    {
        return ( 2.0/ (mu(x  , y  , z  ) +
                       mu(x+1, y  , z  ) +
                       mu(x  , y+1, z  ) +
                       mu(x+1, y+1, z  )) );
    }

    template<typename N>
    GridValue interp_mu( GridIndex x, GridIndex y, GridIndex z )
    {
        return interp_mu( x, y, z, N() );
    }


    // Stress-grid define functions.  For each D in xx, yy, zz, xy, xz, yz,
    // define stress_D at t+1 based on stress_D at t and vel grids at t+1.
    // This implies that the velocity-grid define functions must be called
    // before these for a given value of t.  Note that the t, x, y, z
    // parameters are integer grid indices, not actual offsets in time or
    // space, so half-steps due to staggered grids are adjusted
    // appropriately.

    template<typename N, typename DA, typename SA, typename DB, typename SB>
    void define_str(GridIndex t, GridIndex x, GridIndex y, GridIndex z, 
            Grid &s, Grid &va, Grid &vb) {

        GridValue lcoeff = interp_mu<N>( x, y, z );

        GridValue vta    = stencil_O8<DA,SA>( t, x, y, z, va );
        GridValue vtb    = stencil_O8<DB,SB>( t, x, y, z, vb );

        GridValue next_s = s(t, x, y, z) + ((vta + vtb) * lcoeff) * delta_t;

        // TODO: stress update

        // define the value at t+1.
        s(t+1, x, y, z) == next_s;
    }

    void define_str_TL(GridIndex t, GridIndex x, GridIndex y, GridIndex z )
    {

        GridValue ilambdamu2 = 1.0 / lambdamu2(x,y,z);
        GridValue ilambda    = 1.0 / lambda   (x,y,z);

        GridValue vtx    = stencil_O8<X,F>( t, x, y, z, v_tr_u );
        GridValue vty    = stencil_O8<Y,B>( t, x, y, z, v_tl_v );
        GridValue vtz    = stencil_O8<Z,B>( t, x, y, z, v_bl_w );

        GridValue next_xx = s_tl_xx(t, x, y, z) + ilambdamu2 * vtx * delta_t
                                                + ilambda    * vty * delta_t
                                                + ilambda    * vtz * delta_t;
        GridValue next_yy = s_tl_yy(t, x, y, z) + ilambda    * vtx * delta_t
                                                + ilambdamu2 * vty * delta_t
                                                + ilambda    * vtz * delta_t;
        GridValue next_zz = s_tl_zz(t, x, y, z) + ilambda    * vtx * delta_t
                                                + ilambda    * vty * delta_t
                                                + ilambdamu2 * vtz * delta_t;

        // define the value at t+1.
        s_tl_xx(t+1, x, y, z) == next_xx;
        s_tl_yy(t+1, x, y, z) == next_yy;
        s_tl_zz(t+1, x, y, z) == next_zz;
    }

    // Call all the define_* functions.
    virtual void define(const IntTuple& offsets) {
        GET_OFFSET(t);
        GET_OFFSET(x);
        GET_OFFSET(y);
        GET_OFFSET(z);

        // Define velocity components.
        define_vel<BL, F, B, B>(t, x, y, z, v_bl_w, s_bl_yz, s_br_xz, s_tl_zz);
        define_vel<TR, B, B, F>(t, x, y, z, v_tr_u, s_tr_xy, s_tl_xx, s_br_xz);
        define_vel<TL, B, F, B>(t, x, y, z, v_tl_v, s_tl_yy, s_tr_xy, s_bl_yz);

        //// Define stresses components.
        define_str<BR, X, F, Z, F>(t, x, y, z, s_br_xz, v_bl_w, v_tr_u );
        define_str<TR, X, F, Y, F>(t, x, y, z, s_tr_xy, v_tl_v, v_tr_u );
        define_str<BL, Y, F, Z, F>(t, x, y, z, s_bl_yz, v_bl_w, v_tl_v );
        define_str_TL(t, x, y, z);

    }
};

REGISTER_STENCIL(SSGElasticStencil);
