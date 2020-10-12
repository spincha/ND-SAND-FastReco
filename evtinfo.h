#ifndef EVTINFO_H
#define EVTINFO_H

#include <map>
#include <vector>

enum SMEARTYPE { sttsmear, ecalsmear, pi0extern, NsmearBeta, NsmearEqua};

struct particle{
  
  //  int trackid;
  int parentid;  
  int topid;
  int smeartype;
  int pdg;
  int toppdg;
  int lastECALlayer;
  double recoP4[4];
  double trueP4[4];
  double length;
    double dedxID;
};



struct anaevt{
  
  int ientry;
  //  int nfillpar;
  int nprim;
  int target;
  int nupdg;
  
  double vtx[3];
  double nurecoP[3];
  double nutrueP[3];
  std::map<int, particle> particles;
};

struct anaevtMeta{
    std::string edepfullname;
};



#ifdef __MAKECINT__
#pragma link C++ class anaevtMeta+;
#pragma link C++ class anaevt+;
#pragma link C++ class particle+;
#pragma link C++ class std::map<int, particle>+;
#pragma link C++ class std::vector<particle>+;
#endif

#endif
