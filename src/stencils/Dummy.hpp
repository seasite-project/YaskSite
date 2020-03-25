#ifndef _DUMMY_H
#define _DUMMY_H

//Implement various kinds of stencils required in the SeASiTe project.
//If new stencils are required update this file
#include "yask_compiler_api.hpp"
using namespace std;
using namespace yask;

// Create an anonymous namespace to ensure that types are local.
namespace {
    class Dummy : public yc_solution_base {

        protected:
            // Generic time-varying spatial-3D grid.
            yc_index_node_ptr t = new_step_index("t");           // step in time dim.
            yc_index_node_ptr x = new_domain_index("x");         // spatial dim.
            yc_index_node_ptr y = new_domain_index("y");         // spatial dim.
            yc_index_node_ptr z = new_domain_index("z");         // spatial dim.

            // Vars.
            yc_var_proxy data =
                yc_var_proxy("data", get_soln(), { t, x, y, z }); // time-varying 3D var.

        public:

            Dummy() : yc_solution_base("dummy") {
            }
            Dummy(const string& name) : yc_solution_base(name) {
            }

            // Define equation at t+1 based on values at t.
            virtual void define() {
            }
    };
    YS_REGISTER_STENCIL("dummy", Dummy);
}

#endif
