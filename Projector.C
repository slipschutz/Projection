

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

using namespace std;

int main(){


  Projection myProjection;

  
  TFile * f = new TFile("~/analysis/run306/run-0306-softwareCFD-output.root");
  
  TTree *tree = (TTree*)f->Get("flt");
  cout<<"NUM "<<tree->GetEntries()<<endl;

  TH2F * h = new TH2F("h","title",500,0,10,100,0,5500);

  tree->Project("h","energies[0]:TOF");


  myProjection.SetData(h);
  myProjection.SetRange(400,5500);
  myProjection.MakeProjections();
  
  TFile * out =new TFile("out.root","recreate");
  
  h->Write();
  
  vector <TH1D*>v =myProjection.getProjections();
  
  for (int i=0;i<v.size();i++)
    v[i]->Write();

  TGraphErrors *gr = myProjection.getGraph();
  gr->Write();
  out->Close();


  myProjection.Reset();
  return 0;
}
