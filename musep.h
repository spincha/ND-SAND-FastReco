#include "TG4Event.h"
#include <TGeoManager.h>
#include <map>
#include <vector>
#include <utility>
#include <functional>
bool findEvis_inecal(TG4Event *event, TGeoManager *geo, const std::map<int, std::vector<int> > &ecalMap, int trackid, std::map<int, std::pair<int,double> > &Id_npe_earliestT);
int getLastLayer(std::map<int, std::pair<int, double> > Id_npe_earliestT);
void  getYokeSurfaceInfo2(TG4Event *event, int trackid, int &iOuter, int &iInner);
