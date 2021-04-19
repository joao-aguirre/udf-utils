#include "udf.h"


/*
    Function to check if the current case is a single phase or a
    multi-phase setup.
    In case it is a multi-phase simulation, also outputs the number of
    fluid phases.
    
    Here I am using the `sg_mphase` enum variable to check the multi-
    phase option for the case. Besides this variable I could also have
    used the `cxboolean` variables: `mp_vof`, `mp_drift`, and
    `mp_mfluid` for VOF, mixture and Eulerian, respectively.
*/
DEFINE_ON_DEMAND(get_number_phases)
{
    Domain * domain = Get_Domain(ROOT_DOMAIN_ID);
    Thread * thread = NULL;
    
    switch (sg_mphase) {
        case MP_OFF:
            Message0("\n...Single-phase setup!\n");
            break;
        case MP_VOF:
            Message0("\n...Multiphase VOF setup!\n");
            break;
        case MP_MULTI_FLUID:
            Message0("\n...Multiphase Eulerian setup!\n");
            Message0("\t...number of fluid phases = %d\n", mp_n);
            break;
        case MP_DRIFT_FLUX:
            Message0("\n...Multiphase Mixture setup!\n");
            Message0("\t...number of fluid phases = %d\n", mp_n);
            break;    
        default:
            Error("\n...Unknown setup!\n");
    }
}