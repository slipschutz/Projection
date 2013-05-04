

#include <iostream>
#include <vector>
#include "Projection.hh"
//#include "LendaEvent.hh"

#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include <vector>
#include "TH1D.h"

#include "TGraphErrors.h"

#include <sstream>
using namespace std;


#ifndef __CINT__
typedef struct{
  TH2F* histo;
  int bit;
  int channel;
  string name;
} Corrections;

string ListOfWalkComparisons[]={"TOF","ShiftTOF","TOFP[1]",
			    "TOFW[0]","TOFW[1]"};
string ListOfWalkNames[] ={"TOF","ShiftTOF","TOFP1","TOFW0","TOFW1"};

string ListOfPosComparisons[]={"TOF","ShiftTOF",
			    "TOFP[0]","TOFP[1]"};
string ListOfPosNames[] ={"TOF","ShiftTOF","TOFP0","TOFP1"};

Int_t numComparisons=5;
			

Int_t x_start=-66;
Int_t x_end =-64;
Int_t numChannels=4;



void DoWalkStuff(TTree*tree,Projection myProjection){



  vector <Corrections> WalkCorrections;
  stringstream name;
  stringstream arg;
  stringstream conditions;
  TH2F * temp;
 
  for (int j=0;j<numComparisons;j++){
    for (int i=0;i<numChannels;i++){
      int chan=-1;
      name.str("");
      arg.str("");
      conditions.str("");

      name<<"WalkChannel_"<<i<<ListOfWalkNames[j];

      if (i % 2 == 0){//if even channel
        conditions<<"channels["<<0<<"]=="<<i<<"&&abs(CorGOE)<0.5&&channels[2]==8&&abs("<<ListOfWalkComparisons[j]<<")<200";
        arg<<"energiesCor["<<0<<"]:"<<ListOfWalkComparisons[j];
	chan=i;
      }else{ // odd chanel
        conditions<<"channels["<<1<<"]=="<<i<<"&&abs(CorGOE)<0.5&&channels[2]==8&&abs("<<ListOfWalkComparisons[j]<<")<200";
        arg<<"energiesCor["<<1<<"]:"<<ListOfWalkComparisons[j];
	chan=i;
      }
      cout<<arg.str()<<" "<<conditions.str()<<endl;
      temp = new TH2F(name.str().c_str(),"title",1000,x_start,x_end,1000,0,1500);

      tree->Project(name.str().c_str(),arg.str().c_str(),conditions.str().c_str());
      Corrections c;
      c.histo=temp;
      c.channel=chan;
      if (temp->GetEntries()!=0){
        c.bit=1;
	c.name=ListOfWalkNames[j];
      }else
        c.bit=0;
      WalkCorrections.push_back(c);
    }

  }
  cout<<"Size "<<WalkCorrections.size()<<endl;


  

  //  delete temp;


  TFile * out =new TFile("WalkOut.root","recreate");
  int count=0;
  
  for (int i=0;i<WalkCorrections.size();i++){
    cout<<"*******************************************"<<endl;
    if (WalkCorrections[i].bit==1){
      myProjection.SetData(WalkCorrections[i].histo);
      myProjection.SetRange("auto");
      
      myProjection.MakeProjections();
      vector<double> coef=myProjection.GetGraphCoef();
      cout<<"This is "<<WalkCorrections[i].name<<" the channel is "<<WalkCorrections[i].channel<<endl;
      
      for(int k=0;k<coef.size();k++)
        cout<<"walk"<<WalkCorrections[i].channel<<"cor      walk"<<WalkCorrections[i].channel<<"_"<<k+1<<"      "<<coef[k]<<endl;
      
      vector <TH1D*>v =myProjection.getProjections();
      for (int i=0;i<v.size();i++)
        v[i]->Write();
      
      TGraphErrors *gr = myProjection.getGraph();
      gr->Write();
      count++;
      myProjection.Reset();


    }

  }

  out->Close();




}



void DoPosStuff(TTree*tree,Projection myProjection){
  vector <Corrections> PosCorrections;
  stringstream name;
  stringstream arg;
  stringstream conditions;

  TH2F * temp;

  for (int j=0;j<numComparisons-1;j++){
    for (int i=0;i<numChannels;i++){
      name.str("");
      arg.str("");
      conditions.str("");

      name<<"PosChannel_"<<i<<ListOfPosNames[j];
      conditions<<"channels["<<0<<"]=="<<i<<"&&abs(CorGOE)<0.5&&channels[2]==8";
      arg<<"CorGOE:"<<ListOfPosComparisons[j];
      
      cout<<arg.str()<<" "<<conditions.str()<<endl;
      temp = new TH2F(name.str().c_str(),"title",1000,x_start,x_end,1000,-1,1);
      tree->Project(name.str().c_str(),arg.str().c_str(),conditions.str().c_str());
      
      Corrections c;
      c.histo=temp;
      c.channel=i;

      if (temp->GetEntries()!=0){
	c.bit=1;
	c.name =ListOfPosNames[j];
      }else{
	c.bit=0;
      }
      PosCorrections.push_back(c);
    }

  }
  //  delete temp;
  
  TFile * out =new TFile("PosOut.root","recreate");

  int count=0;//counter for the corrections that have bit=1
  
  for (int i=0;i<PosCorrections.size();i++){
    cout<<"*******************************************"<<endl;
    if (PosCorrections[i].bit==1){

      myProjection.SetData(PosCorrections[i].histo);
      myProjection.SetRange("auto");

      myProjection.MakeProjections();
      vector<double> coef=myProjection.GetGraphCoef();
      cout<<"This is "<<PosCorrections[i].name<<" the channel is "<<PosCorrections[i].channel<<endl;
      for(int k=0;k<coef.size();k++)
        cout<<"pos"<<PosCorrections[i].channel<<"cor      goe"<<PosCorrections[i].channel<<"_"<<k+1<<"      "<<coef[k]<<endl;

      
      vector <TH1D*>v =myProjection.getProjections();
      

      for (int i=0;i<v.size();i++)
        v[i]->Write();

      TGraphErrors *gr = myProjection.getGraph();
      gr->Write();


      count++;
      myProjection.Reset();
    }else{
      cout<<"Filler line for "<<i<<endl;
    }

  }
    out->Close();

}



void DoResStuff(TTree *tree){

  

  



}



 

int main(){



  Projection myProjection;

  TTree t;
  
  TFile * f = new TFile("~/analysis/run309/CoRrun-0309-liq_scint.root");
  
  TTree *tree = (TTree*)f->Get("flt");
  cout<<"Num of events: "<<tree->GetEntries()<<endl;

  DoWalkStuff(tree,myProjection);
  DoPosStuff(tree,myProjection);
  
  /*
  vector <Corrections> WalkCorrections;
  stringstream name;
  stringstream arg;
  stringstream conditions;
  for (int j=0;j<4;j++){
    for (int i=0;i<numChannels;i++){
      name.str("");
      arg.str("");
      conditions.str("");
    
      name<<"WalkChannel_"<<i<<ListOfNames[j];

      if (i % 2 == 0){//if even channel
	conditions<<"channels["<<0<<"]=="<<i<<"&&abs(CorGOE)<0.5&&channels[2]==8";
	arg<<"energiesCor["<<0<<"]:"<<ListOfComparisons[j];      
      }else{ // odd chanel
	conditions<<"channels["<<1<<"]=="<<i<<"&&abs(CorGOE)<0.5&&channels[2]==8";
	arg<<"energiesCor["<<1<<"]:"<<ListOfComparisons[j];
      }
      cout<<arg.str()<<" "<<conditions.str()<<endl;
      TH2F * temp = new TH2F(name.str().c_str(),"title",1000,x_start,x_end,1000,0,1500);
      
      tree->Project(name.str().c_str(),arg.str().c_str(),conditions.str().c_str());
      Corrections c;
      c.histo=temp;
      if (temp->GetEntries()!=0){
	c.bit=1;
      }else
	c.bit=0;
      WalkCorrections.push_back(c);
    }

  }


  TFile * out =new TFile("out.root","recreate");
  
  for (int i=0;i<WalkCorrections.size();i++){

    if (WalkCorrections[i].bit==1){
      myProjection.SetData(WalkCorrections[i].histo);
      myProjection.SetRange("auto");

      myProjection.MakeProjections();
      vector<double> coef=myProjection.GetGraphCoef();
      for(int k=0;k<coef.size();k++)
	cout<<"walk"<<i<<"cor      walk"<<i<<"_"<<k<<"      "<<coef[k]<<endl;

      vector <TH1D*>v =myProjection.getProjections();
      for (int i=0;i<v.size();i++)
	v[i]->Write();
            
      TGraphErrors *gr = myProjection.getGraph();
      gr->Write();      

      myProjection.Reset();
    }
   
  }

  out->Close();

  */
  return 0;


}

#endif
