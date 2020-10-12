#include "TMath.h"
#include "digitize.h"

double Attenuation(double d, int planeID)
{
  /*
       dE/dx attenuation - Ea=p1*exp(-d/atl1)+(1.-p1)*exp(-d/atl2)
         d    distance from photocatode - 2 cells/cell; d1 and d2
        atl1  50. cm
        atl2  430 cm planes 1-2    innermost plane is 1
              380 cm plane 3
              330 cm planes 4-5
         p1   0.35
  */
  const double p1 = 0.35;
  const double alt1 = 500.;
  double alt2 = 0.0;

  switch (planeID) {
  case 0:
  case 1:
    alt2 = 4300.0;
    break;

  case 2:
    alt2 = 3800.0;
    break;

  case 3:
  case 4:
    alt2 = 3300.0;
    break;

  default:
    // std::cout << "planeID out if range" << std::endl;
    alt2 = -999.0;
    break;
  }

  return p1 * TMath::Exp(-d / alt1) + (1. - p1) * TMath::Exp(-d / alt2);
}

double E2PE(double E)
{
  // Average number of photoelectrons = 25*Ea(MeV)
  const double e2p2 = 25.;
  return e2p2*E;
}
