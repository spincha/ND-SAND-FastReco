
#include "musep.h"
#include "TObjString.h"
#include "TGeoTrd2.h"
#include "TGeoTube.h"
#include "digitize.h"
#include "share.h"

void  getYokeSurfaceInfo2(TG4Event *event, int trackid, int &iOuter, int &iInner){

  iOuter=-1;
  iInner=-1;
  double centerY=-2384.73;  // mm
  double centerZ=23910; // mm
  
  double yokeBarrelRmax=3300;
  double yokeBarrelRmin=2930;
  double yokeBarrelX=4300;
  //  double endx=event->Trajectories[trackid].Points.rbegin()->Position.X();
  double endy=event->Trajectories[trackid].Points.rbegin()->Position.Y();
  double endz=event->Trajectories[trackid].Points.rbegin()->Position.Z();
  double r0=sqrt(pow(endy-centerY,2) + pow(endz- centerZ, 2));
  if(r0< yokeBarrelRmin) return;

  double r;
  int nP=event->Trajectories[trackid].Points.size();
  //  std::cout<<"---------------ienry:"<<ientry<<"  trackid:"<<trackid<<" end out of yoke inner surface-----------------------------"<<std::endl;
  for(int i=(nP-1); i>0 ; i--){
    double x=event->Trajectories[trackid].Points[i].Position.X();
    double y=event->Trajectories[trackid].Points[i].Position.Y();
    double z=event->Trajectories[trackid].Points[i].Position.Z();
    r=sqrt(pow(y-centerY,2) + pow(z-centerZ, 2));
    //    std::cout<<"r:"<<r<<"  x:"<<x<<std::endl;
    if( r>yokeBarrelRmax || abs(x)>(yokeBarrelX/2+410+430)){
	//      std::cout<<"r-yokeBarrelRmax:"<<r-yokeBarrelRmax<<"   ientry:"<<ientry<<"  trackid:"<<trackid<<" outer"<<std::endl;
      iOuter=i;  break;
    }
    if( r>yokeBarrelRmin || abs(x)>yokeBarrelX/2 || (r<1730 && abs(x)>1960) ) {
      //      std::cout<<"r-yokeBarrelRmin:"<<r-yokeBarrelRmin<<"    ientry:"<<ientry<<"  trackid:"<<trackid<<" inner"<<std::endl;
      iInner=i;  break; // fillYokeInfo(0, x, y, z, event->Trajectories[trackid].Points[i].Momentum);   break;
    }

  }

  //  if(iOuter!=-1 && iInner==-1) std::cout<<"###### weird, iOuter not -1 but iInner is 1"<<std::endl;

}


bool findEvis_inecal(TG4Event *event, TGeoManager *geo, const std::map<int, std::vector<int> > &ecalMap, int trackid, std::map<int, std::pair<int,double> > &Id_npe_earliestT){

  //  Id_Evis.clear();
  Id_npe_earliestT.clear();

  if(ecalMap.find(trackid)==ecalMap.end()) return false;
  std::vector<int> hitchains=ecalMap.at(trackid);
  
  for(unsigned int i=0;i<hitchains.size()/2;i++){
    for(int j = hitchains[2*i]; j <= hitchains[2*i+1]; j++){

      const TG4HitSegment& h = event->SegmentDetectors["ECAL"].at(j);
      double de=h.EnergyDeposit;
      double x = 0.5*(h.Start.X()+h.Stop.X());
      double y = 0.5*(h.Start.Y()+h.Stop.Y());
      double z = 0.5*(h.Start.Z()+h.Stop.Z());
      

      TGeoNode* node = geo->FindNode(x,y,z);
      TString slabstr = node->GetName();
      TString modstr=geo->GetMother()->GetName();
      //    std::cout<<"modstr:"<<modstr<<std::endl;
      int id, modID, slabID, planeID, cellID;
      double d1,d2;
      if(slabstr.Contains("volECALActiveSlab") == true)
	{
	  
	  TObjArray* obj1 = slabstr.Tokenize("_");  //volECALActiveSlab_125_PV_0
	  TObjArray* obj2 = modstr.Tokenize("_");  //ECAL_lv_PV_19
	  
	  modID  = ((TObjString*) obj2->At(3))->GetString().Atoi();
	  slabID = ((TObjString*) obj1->At(1))->GetString().Atoi();
	  
	delete obj1;
	delete obj2;

	planeID = slabID/40;

	if (planeID > 4) planeID = 4;
	double Pmaster[3]={x,y,z};
	double Plocal[3];
	geo->GetCurrentNavigator()->MasterToLocal(Pmaster,Plocal);
	TGeoTrd2* trd = (TGeoTrd2*) node->GetVolume()->GetShape();

	double dx1 = trd->GetDx1();  // shorter one along circumferential
	double dx2 = trd->GetDx2();  // longer one along circumferential
	double dz  = trd->GetDz();   // half thickness , along radial
	double dy1 = trd->GetDy1();  // along axial/fiber , same as dy2
	d1 = dy1 + Plocal[1];
	d2 = dy1 - Plocal[1];

	double dx = (dx2 - dx1) / dz * Plocal[2];
	double dis= Plocal[0]>0? (dx1+dx2)/2. + Plocal[0] - dx/2.: (dx1+dx2)/2. + Plocal[0] + dx/2.;
	double cellw = (dx1+dx2) / 12.;
	cellID = dis / cellw;  // cell ID increase counterclockwise, from 0 to 11, then 0 to 11 
	//	if(DaughterIds.size()>0)  std::cout<<"trackid:"<<trackid<<"  modID:"<<modID<<" slabID:"<<slabID<<" planeID:"<<planeID<<" cellID:"<<cellID<<std::endl;
      }
    else if(slabstr.Contains("vol_endECALActiveSlab") == true)
      {

	TObjArray* obja = slabstr.Tokenize("_");
      	modID  = x>0?30:40;
	slabID = ((TObjString*) obja->At(1))->GetString().Atoi();

	delete obja;

        planeID = slabID/40;
	if (planeID > 4) planeID = 4;
	double Pmaster[3]={x,y,z};
	double Plocal[3];
	//      std::cout<<"x:"<<x<<" y:"<<y<<" z:"<<z<<std::endl;
	geo->GetCurrentNavigator()->MasterToLocal(Pmaster,Plocal);

	TGeoTube* tub = (TGeoTube*) node->GetVolume()->GetShape();
	double rmax = tub->GetRmax();
	cellID = int((Plocal[0]/rmax + 1.) * 45);	 
	// 0: larger--> smaller Z 
	// 1: smaller cellId -> smaller Z

      }
    else { continue;}
    //    std::cout<<" "<<id;
    id = cellID + 100 * planeID + 1000 * modID;
    double en1 = de * Attenuation(d1, planeID);
    double en2 = de * Attenuation(d2, planeID);

    double ave_pe1 = E2PE(en1);
    double ave_pe2 = E2PE(en2);

    int pe1 = ran->Poisson(ave_pe1);
    int pe2 = ran->Poisson(ave_pe2);
    int npe=pe1+pe2;
    double t=(h.Start.T()+h.Stop.T())*0.5; //  + ran->Gaus(0,0.26);

    if(Id_npe_earliestT.find(id)!=Id_npe_earliestT.end()) {
      Id_npe_earliestT[id].first+= npe;
      if(t<Id_npe_earliestT[id].second) Id_npe_earliestT[id].second=t;
    }
    else {
      Id_npe_earliestT[id].first = npe;
      Id_npe_earliestT[id].second=t;
    }
    
    /*
    if(Id_Evis.find(id)==Id_Evis.end())
      Id_Evis[id]= h.EnergyDeposit;
    else
      Id_Evis[id]+= h.EnergyDeposit;
    */
  }
  }

  for(auto it = Id_npe_earliestT.begin(); it != Id_npe_earliestT.end(); ) {
    if(it->second.first <2.5)
      it = Id_npe_earliestT.erase(it);
    else
      ++it;
  }
  

  if(Id_npe_earliestT.size()==0) return false;
  else return true;

}

int getLastLayer(std::map<int, std::pair<int, double> > Id_npe_earliestT){

  double layer_Evis[5];
  
  for(int i=0;i<5;i++){
    layer_Evis[i]=0;
  }

  for(auto &cell:Id_npe_earliestT){
    int id=cell.first;
    int planeId=(id%1000)/100;
    int modId=id/1000;
    int cellId=id%100;
    int modCellId=modId*12+cellId;
    
    double cellcalE=cell.second.first/npe1MeV;
   

    layer_Evis[planeId]+=cellcalE; //cell.second;
  }
  

  int latestLayer=-1;
  for(int i=0;i<5;i++){
    if(layer_Evis[i]>0) { latestLayer=i; }
  }
  return latestLayer;

}
