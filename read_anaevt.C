#include <cstdio>
#include <iostream>
#include <vector>
#include <TFile.h>
#include "TTree.h"
#include "evtinfo.h"


int main(){

  std::string smtype[5]={"sttsmear", "ecalsmear", "pi0extern", "NsmearBeta", "NsmearEqua"};
  
  TFile *inf=new TFile("/dune/app/users/mylab/ndwork/stt/muID/slv/bm/nc/NC_ECAL11STT_FHC_0058_2e17.201007.edepsmeared.root");
  
  TTree *tree=(TTree*)inf->Get("edepsmear_anaevt_tree");
  TTree *metaTree=(TTree*)inf->Get("edepsmear_meta");
  
  anaevt *evt=new anaevt();
  anaevtMeta *evtMeta;
  
  tree->SetBranchAddress("event",&evt);
  metaTree->SetBranchAddress("meta",&evtMeta);
  
  int nentry=tree->GetEntries();
  nentry=10;
  metaTree->GetEntry(0);
  std::cout<<"parent file:"<<evtMeta->edepfullname<<std::endl;
  for(int i=0;i<nentry;i++){
    tree->GetEntry(i);
    int npar=evt->particles.size();
    std::cout<<"---------- ientry:"<<evt->ientry<<" target:"<<evt->target<<" nupdg:"<<evt->nupdg<<" npar:"<<npar<<std::endl;
    std::cout<<"vx:"<<evt->vtx[0]<<"vy:"<<evt->vtx[1]<<"vz:"<<evt->vtx[2]<<std::endl;
    std::cout<<"true:"<<" Px:"<<evt->nutrueP[0]<<" Py:"<<evt->nutrueP[1]<<" Pz:"<<evt->nutrueP[2]<<std::endl;
    std::cout<<"reco:"<<" Px:"<<evt->nurecoP[0]<<" Py:"<<evt->nurecoP[1]<<" Pz:"<<evt->nurecoP[2]<<std::endl;
    std::printf("%4s %5s %6s %6s %5s %11s %13s %13s %13s %13s %13s %13s %13s %13s %10s %6s\n","pdg","id","parid","topid","tpdg","smtype","Px_true","Py_true","Pz_true","E_true","Px_reco","Py_reco","Pz_reco","E_reco","trklength","ecalll");
    for(auto p:evt->particles){
      particle par=p.second;
      std::printf("%4d %5d %6d %6d %5d %11s %13.3f %13.3f %13.3f %13.3f %13.3f %13.3f %13.3f %13.3f %10.3f %6d %5.2f\n", par.pdg, p.first, par.parentid, par.topid, par.toppdg, smtype[par.smeartype].c_str(), par.trueP4[0], par.trueP4[1],par.trueP4[2],par.trueP4[3], par.recoP4[0], par.recoP4[1],par.recoP4[2],par.recoP4[3], par.length, par.lastECALlayer, par.dedxID);
    }    
    std::cout<<std::endl;
  }



}

