// file in fhc_stt_cc_list.txt : ECAL barrel module number : top is 0, then it increase counterclockwise,
//     0
//   1   23
// 2        22
// // cell ID increase counterclockwise, from 0 to 11, then 0 to 11

#include "TArc.h"
#include "TCrown.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TText.h"
#include "TArrow.h"
#include "TBox.h"
#include "TMarker.h"
#include <set>

class eventdisplay{
    
private:
    std::set<int> idlist;
    std::set<int> neutronCelllist;
    std::set<int> primNeutronCelllist;
    std::set<int> topPrimNeutronCelllist;
    std::vector<std::pair<double, double> > pointlist;
    TCanvas *evdCan;
    TText *tt;
    std::string alltext;
    TCrown *cr;
    TBox *bo;
    TBox *bo2;
    TMarker *ma;
    double centerY;
    double centerZ;
    double r6[6];
    TString saveName;
    
public:
  eventdisplay(TString name);
  ~eventdisplay();
  void addpoint(double x, double y){ pointlist.emplace_back(x,y);}
  void addid(int modID, int planeID, int cellID);
  void addid(int id) { idlist.insert(id);}
  void addNeutronBlob(const std::set<int> &ids) { neutronCelllist.insert(ids.begin(), ids.end()); }
  void addPrimNeutronBlob(const std::set<int> &ids) { primNeutronCelllist.insert(ids.begin(), ids.end()); }
  void addTopPrimNeutronBlob(const std::set<int> &ids) { topPrimNeutronCelllist.insert(ids.begin(), ids.end()); }
  void addtext(std::string text) { alltext+=" "+text;}
  void plot1cell(int id);
  void plot();
  void clear();
};

eventdisplay::~eventdisplay(){
  evdCan->Print(saveName+"]");  
}

void eventdisplay::clear(){
  if(evdCan!=0) evdCan->Clear();
  idlist.clear();
  pointlist.clear();
  neutronCelllist.clear();
  primNeutronCelllist.clear();
  topPrimNeutronCelllist.clear();
  alltext.clear();
}

eventdisplay::eventdisplay(TString name){
  
  saveName=name;

  centerY=-238.473;  // cm
  centerZ=2391; // cm

  double sttr=200;
  double thickness[5]={4.4,4.4,4.4,4.4,5.28};

  r6[0]=sttr;
  r6[1]=r6[0]+thickness[0];
  r6[2]=r6[1]+thickness[1];
  r6[3]=r6[2]+thickness[2];
  r6[4]=r6[3]+thickness[3];
  r6[5]=r6[4]+thickness[4];

  double sandr=253;
  evdCan=new TCanvas("evdCan","",600,600);
  evdCan->Range(centerZ-sandr,centerY-sandr,centerZ+sandr,centerY+sandr);
  evdCan->Print(saveName+"[");

}

void eventdisplay::addid(int modID, int planeID, int cellID){
  if(planeID<0 || planeID>4) { std::cout<<"wrong adding planeID"<<std::endl; return;}
  //  if(cellID<0 || cellID> 11) { std::cout<<"wrong adding cellID"<<std::endl;return;}
  idlist.insert(modID*1000+planeID*100+cellID);
}

void eventdisplay::plot1cell(int id){
  //  std::cout<<"plotid:"<<id<<std::endl;
  int modID=id/1000;
  int planeID=(id%1000)/100;
  int cellID=id%100;
  double startPhi=82.5+modID*15+cellID*1.25;
  
  if(modID<30){
    if( primNeutronCelllist.find(id)!=primNeutronCelllist.end())
      cr->SetFillColor(kGreen);
    else if( topPrimNeutronCelllist.find(id)!=topPrimNeutronCelllist.end())
      cr->SetFillColor(kRed);
    else if( neutronCelllist.find(id)!=neutronCelllist.end())
      cr->SetFillColor(kMagenta);
    else
      cr->SetFillColor(kYellow);
    cr->DrawCrown(centerZ,centerY, r6[planeID], r6[planeID+1], startPhi, startPhi+1.25);
  }
  else if(modID==30){
    //    std::cout<<"planeID:"<<planeID<<std::endl;
    double ylow=centerY-200+ 400./90.*cellID;
    double yhigh=centerY-200+ 400./90.*(cellID+1);
    double xleft=centerZ-225.-5.*(planeID+1);
    double xright=centerZ-225.-5.*planeID;
    bo->DrawBox(xleft,ylow, xright,yhigh);
    bo2->DrawBox(xleft,ylow, xright,yhigh);
  }
  else if(modID==40){
    //    std::cout<<"planeID:"<<planeID<<std::endl;
    double ylow=centerY-200+ 400./90.*cellID;
    double yhigh=centerY-200+ 400./90.*(cellID+1);
    double xleft=centerZ+225.+5.*planeID;
    double xright=centerZ+225.+5.*(planeID+1);
    bo->DrawBox(xleft,ylow, xright,yhigh);
    bo2->DrawBox(xleft,ylow, xright,yhigh);
  }
  else std::exit(EXIT_FAILURE);
}

void  eventdisplay::plot(){

  
  double phi1=-7.5;
  double phi2=7.5;
  cr=new TCrown();
  for(int i=0;i<24;i++){
    cr->DrawCrown(centerZ, centerY, r6[0], r6[5], phi1,phi2);
    phi1+=15;
    phi2+=15;
  }
  bo=new TBox();
  bo->SetFillStyle(0);
  bo->SetLineColor(kBlack);
  bo->SetLineWidth(2);
  bo->DrawBox(centerZ-250, centerY-200, centerZ-225, centerY+200);
  bo->DrawBox(centerZ+225, centerY-200, centerZ+250, centerY+200);

  bo2=new TBox();
  bo2->SetFillColor(kYellow);
  //  cr->SetFillColor(kYellow);
  for(auto &id:idlist)
    plot1cell(id);

  ma=new TMarker();
  ma->SetMarkerSize(1);
  ma->SetMarkerStyle(5);
  for(auto &pts: pointlist){
    ma->DrawMarker(pts.first,pts.second);
  }

  if(alltext!=""){
      tt=new TText(2200,centerY,alltext.c_str());
      tt->SetTextFont(43);
      tt->SetTextSize(30);
      tt->Draw();
  }
  evdCan->Print(saveName);

}
