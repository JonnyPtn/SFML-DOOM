// Needed for FRACUNIT.
#include "m_fixed.hpp"

// Needed for Flat retrieval.
#include "r_data.hpp"


#ifdef __GNUG__
#pragma implementation "r_sky.hpp"
#endif
#include "r_sky.hpp"

//
// sky mapping
//
int			skyflatnum;
int			skytexture;
int			skytexturemid;



//
// R_InitSkyMap
// Called whenever the view size changes.
//
void R_InitSkyMap (void)
{
  // skyflatnum = R_FlatNumForName ( SKYFLATNAME );
    skytexturemid = 100*FRACUNIT;
}

