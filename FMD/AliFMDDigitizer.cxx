/**************************************************************************
 * Copyright(c) 1998-2000, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

 //////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Forward Multiplicity Detector based on Silicon plates                    //
//  This class contains the procedures simulation ADC  signal for            //
//  the Forward Multiplicity detector  : hits -> digits                      //
//  ADC signal consists                                                      //
//   - number of detector;                                                   //
//   - number of ring;                                                       //
//   - number of sector;                                                     //
//   - ADC signal in this channel                                            //
//                                                                           //
 //////////////////////////////////////////////////////////////////////////////

#include <TTree.h> 
#include <TVector.h>
#include <TObjArray.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TRandom.h>


#include "AliFMDDigitizer.h"
#include "AliFMD.h"
#include "AliFMDhit.h"
#include "AliFMDdigit.h"
#include "AliRunDigitizer.h"

#include "AliRun.h"
#include "AliPDG.h"
#include "AliLoader.h"
#include "AliRunLoader.h"

#include <stdlib.h>
#include <Riostream.h>
#include <Riostream.h>

ClassImp(AliFMDDigitizer)

//___________________________________________
  AliFMDDigitizer::AliFMDDigitizer()  :AliDigitizer()
{
// Default ctor - don't use it
  ;
}

//___________________________________________
AliFMDDigitizer::AliFMDDigitizer(AliRunDigitizer* manager) 
    :AliDigitizer(manager) 
{
  // ctor which should be used
  //  fDebug =0;
  // if (GetDebug()>2)
  //  cerr<<"AliFMDDigitizer::AliFMDDigitizer"
  //     <<"(AliRunDigitizer* manager) was processed"<<endl;
}

//------------------------------------------------------------------------
AliFMDDigitizer::~AliFMDDigitizer()
{
// Destructor
}

 //------------------------------------------------------------------------
Bool_t AliFMDDigitizer::Init()
{
// Initialization
// cout<<"AliFMDDigitizer::Init"<<endl;
 return kTRUE;
}
 

//---------------------------------------------------------------------

void AliFMDDigitizer::Exec(Option_t * /*option*/)
{

  /*
   Conver hits to digits:
   - number of detector;
   - number of ring;
   - number of sector;
   - ADC signal in this channel
  */

  AliRunLoader *inRL, *outRL;//in and out Run Loaders
  AliLoader *ingime, *outgime;// in and out ITSLoaders

  outRL = AliRunLoader::GetRunLoader(fManager->GetOutputFolderName());
  outgime = outRL->GetLoader("FMDLoader");

#ifdef DEBUG
  cout<<"AliFMDDigitizer::>SDigits2Digits start...\n";
#endif


  Int_t volume, sector, ring, charge;
  Float_t e;
  Float_t de[10][50][520];
  Int_t hit;
  Int_t digit[5];
  Int_t ivol, iSector, iRing;
  for (Int_t i=0; i<10; i++)
    for(Int_t j=0; j<50; j++)
      for(Int_t ij=0; ij<520; ij++)
     de[i][j][ij]=0;
  Int_t numberOfRings[5]=
  {512,256,512,256,512};
  Int_t numberOfSector[5]=
  {20,40,20,40,20}; 
  
  AliFMDhit *fmdHit=0;
  TTree *tH=0;
  TBranch *brHits=0;
  TBranch *brD=0;

  inRL = AliRunLoader::GetRunLoader(fManager->GetInputFolderName(0));
  if (inRL == 0x0)
    {
      Error("Exec","Can not find Run Loader for input stream 0");
      return;
    }
  //  Info("Exec","inRL->GetAliRun() %#x",inRL->GetAliRun());

  if (!inRL->GetAliRun()) inRL->LoadgAlice();

  AliFMD * fFMD = (AliFMD *) inRL->GetAliRun()->GetDetector("FMD");
  //  Info("Exec","inRL->GetAliRun(): %#x, FMD: %#x, InRL %#x.",inRL->GetAliRun(),fFMD,inRL);
  if (fFMD == 0x0)
   {
     Error("Exec","Can not get FMD from gAlice");
     return;
   }
// Loop over files to digitize

  Int_t nFiles=GetManager()->GetNinputs();
  for (Int_t inputFile=0; inputFile<nFiles;inputFile++) 
   {
    cout<<" event "<<fManager->GetOutputEventNr()<<endl;
    if (fFMD)
     {

      inRL = AliRunLoader::GetRunLoader(fManager->GetInputFolderName(inputFile));
      ingime = inRL->GetLoader("FMDLoader");
      ingime->LoadHits("READ");//probably it is necessary to load them before
      
      
      tH = ingime->TreeH();
      if (tH == 0x0)
       {
         ingime->LoadHits("read");
         tH = ingime->TreeH();
       }
      brHits = tH->GetBranch("FMD");
      if (brHits) {
  //      brHits->SetAddress(&fHits);
          fFMD->SetHitsAddressBranch(brHits);
      }else{
        Fatal("Exec","EXEC Branch FMD hit not found");
      }
      TClonesArray *fFMDhits = fFMD->Hits ();
      
      Int_t ntracks    = (Int_t) tH->GetEntries();
      cout<<"Number of tracks TreeH"<<ntracks<<endl;
      for (Int_t track = 0; track < ntracks; track++)
       {
         brHits->GetEntry(track);
         Int_t nhits = fFMDhits->GetEntries ();
         // if(nhits>0) cout<<"nhits "<<nhits<<endl;
         for (hit = 0; hit < nhits; hit++)
           {
             fmdHit = (AliFMDhit *) fFMDhits->UncheckedAt(hit);

             volume = fmdHit->Volume ();
             sector = fmdHit->NumberOfSector ();
             ring = fmdHit->NumberOfRing ();
             e = fmdHit->Edep ();
             de[volume][sector][ring] += e;
	     //   if (fManager->GetOutputEventNr()>1)
                    //      cout<<" "<<volume<<" "<<sector<<" "<<ring<<endl;
           }          //hit loop
       }               //track loop
    }               
//if FMD

 
  // Put noise and make ADC signal
   Float_t mipI = 1.664 * 0.04 * 2.33 / 22400;     // = 6.923e-6;
   for ( ivol=1; ivol<=5; ivol++){
     for ( iSector=1; iSector<=numberOfSector[ivol-1]; iSector++){
       for ( iRing=1; iRing<=numberOfRings[ivol-1]; iRing++){
         digit[0]=ivol;
         digit[1]=iSector;
         digit[2]=iRing;
         charge = Int_t (de[ivol][iSector][iRing] / mipI);
         Int_t pedestal=Int_t(gRandom->Gaus(500,250));
  //       digit[3]=PutNoise(charge);
         digit[3]=charge + pedestal;
         if(digit[3]<= 500) digit[3]=500; 
    //dynamic range from MIP(0.155MeV) to 30MIP(4.65MeV)
    //1024 ADC channels 
         Float_t channelWidth=(22400*50)/1024;
         digit[4]=Int_t(digit[3]/channelWidth);
         if (digit[4]>1024) digit[4]=1024; 
         fFMD->AddDigit(digit);
       } //ivol
     } //iSector
   } //iRing

   TTree* treeD = outgime->TreeD();
   cout<<" treeD "<<treeD;
   if (treeD == 0x0) {
     outgime->MakeTree("D");
     treeD = outgime->TreeD();
     cout<<" After MakeTree "<<treeD<<endl;
   }
   cout<<" Before reset "<<treeD<<endl;
   //   treeD->Clear();
   treeD->Reset();
   fFMD->MakeBranchInTreeD(treeD);
   brD = treeD->GetBranch("FMD");
   cout<<" Make branch "<<brD<<endl;

   treeD->Fill();  //this operator does not work for events >1
   //PH   treeD->Print();
   outgime->WriteDigits("OVERWRITE");
  
   gAlice->ResetDigits();
   }
}
 




