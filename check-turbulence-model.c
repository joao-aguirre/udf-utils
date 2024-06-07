#include "udf.h"

DEFINE_ON_DEMAND(check_turbulence_model)
{
    /*
    Fluent's internal variable `M_turb_model` stores the information of which
    turbulence model is selected for the current case. The possible values for
    this variable are defined in the enumeration `Turb_Model_Type` defined in
    the header file `turb.h`.
    */
    if (I_AM_NODE_HOST_P)
    {
        Message("\n... Viscous model selected = %d", M_turb_model);
    }

    /*
    In this same file, some other useful variables and functions to check
    viscous model options are defined.
    */
    if (I_AM_NODE_HOST_P)
    {
        Message("\n... Inviscid model? %s", rp_inviscid ? "TRUE" : "FALSE");
        Message("\n... Viscous model? %s", rp_visc ? "TRUE" : "FALSE");
        Message("\n... Laminar model? %s", rp_lam ? "TRUE" : "FALSE");
        Message("\n... Using a turbulence model? %s", rp_turb ? "TRUE" : "FALSE");
    }
}
