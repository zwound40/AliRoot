// -*- mode: C++ -*- 

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights
 * reserved. 
 *
 * See cxx source for full Copyright notice                               
 */
// Thil class computes background corrections for the FMD. The correction is computed 
// in eta,phi cells and the objects stored can be put into alien to use with analysis.
//
// Author: Hans Hjersing Dalsgaard, NBI, hans.dalsgaard@cern.ch
//
// 

#ifndef ALIFMDBACKGROUNDCORRECTION_H
#define ALIFMDBACKGROUNDCORRECTION_H

#include "AliFMDInput.h"
#include "TObjArray.h"


class AliFMDBackgroundCorrection : public TNamed {
  
public:
  
  AliFMDBackgroundCorrection() ;
  ~AliFMDBackgroundCorrection() {};
  void GenerateBackgroundCorrection(Int_t nvtxbins=10,
				    Float_t zvtxcut=10,
				    Int_t nBinsEta=100, 
				    Bool_t storeInAlien = kFALSE, 
				    Int_t runNo =0, 
				    Int_t endRunNo=999999999, 
				    const Char_t* filename="background.root", 
				    Bool_t simulate = kFALSE, 
				    Int_t nEvents=10);
  
  class AliFMDInputBG : public AliFMDInput {
    
  public :
    AliFMDInputBG() ; 
   
    Bool_t Init();
    
    Int_t GetNprim() {return fPrim;}
    Int_t GetNhits() {return fHits;}
    void  SetVtxCutZ(Double_t vtxCut) { fZvtxCut = vtxCut;}
    void  SetNvtxBins(Int_t nBins) { fNvtxBins = nBins;}
    void  SetNbinsEta(Int_t nBins) { fNbinsEta = nBins;}
    TObjArray*  GetHits() {return &fHitArray;}
    TObjArray*  GetPrimaries() {return &fPrimaryArray;}
  private:
    Bool_t ProcessTrack(Int_t i, TParticle* p, AliFMDHit* h );
    TObjArray fPrimaryArray;
    TObjArray fHitArray;
    Int_t fPrim;
    Int_t fHits;
    Double_t fZvtxCut;
    Int_t fNvtxBins;
    Int_t fPrevTrack;
    Int_t fPrevDetector;
    Char_t fPrevRing;
    Int_t fNbinsEta;
  };
  
private:
  
  void Simulate(Int_t);
  void ProcessPrimaries(AliRunLoader*);
  void ProcessHits();
  TObjArray fCorrectionArray;
   
  
  ClassDef(AliFMDBackgroundCorrection,0)
  
};
#endif
// EOF
