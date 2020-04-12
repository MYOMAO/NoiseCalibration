/// \file CheckDigits.C
/// \brief Simple macro to check ITSU digits


#include <TCanvas.h>
#include <TFile.h>
#include <TH2F.h>
#include <TNtuple.h>
#include <TString.h>
#include <TTree.h>

#include <vector>
#include <map>

#include "ITSBase/GeometryTGeo.h"
#include "ITSMFTBase/Digit.h"
#include "ITSMFTBase/SegmentationAlpide.h"
#include "ITSMFTSimulation/Hit.h"
#include "MathUtils/Utils.h"
#include "SimulationDataFormat/MCTruthContainer.h"
#include "SimulationDataFormat/MCCompLabel.h"
#include "DetectorsBase/GeometryManager.h"

#pragma link C++ class std::vector<std::map<int,int>>+;


using namespace o2::base;

void Make2DMap(std::string digifile = "itsdigits.root", int HitCut = -1)
{

	std::string inputGeom = "O2geometry.root";
	using o2::itsmft::Digit;
	using o2::itsmft::SegmentationAlpide;
	using namespace o2::its;

	//TFile* f = TFile::Open("CheckDigits.root", "recreate");
	//  TNtuple* nt = new TNtuple("ntd", "digit ntuple", "id:x:y:z:rowD:colD:rowH:colH:xlH:zlH:xlcH:zlcH:dx:dz");

	// Geometry
	o2::base::GeometryManager::loadGeometry(inputGeom, "FAIRGeom");
	auto* gman = o2::its::GeometryTGeo::Instance();
	gman->fillMatrixCache(o2::utils::bit2Mask(o2::TransformType::L2G));

	SegmentationAlpide seg;


	// Digits
	TFile* file1 = TFile::Open(digifile.data());
	TTree* digTree = (TTree*)gFile->Get("o2sim");
	std::vector<o2::itsmft::Digit>* digArr = nullptr;
	o2::dataformats::MCTruthContainer<o2::MCCompLabel>* labels = nullptr;
	digTree->SetBranchAddress("ITSDigit", &digArr);


	int nevD = digTree->GetEntries(); // digits in cont. readout may be grouped as few events per entry

	int row;
	int col;
	int key;


	std::vector<std::map<int,int>> mNoisyPixels;
	mNoisyPixels.assign(1,std::map<int,int>());


	std::vector<std::map<int,int>> mNoisyPixelsCut;
	mNoisyPixelsCut.assign(1,std::map<int,int>());


	int nd = -1;

	for (int iev = 0; iev < nevD; iev++) {
		digTree->GetEvent(iev);




		for (const auto& d : *digArr) {
			nd++;
			col = d.getRow();
			row = d.getColumn();
			key = row + col * 1000;
			mNoisyPixels[0][key]++;
		}
	}

	int NPixelCalb = 0;

	for(unsigned int i = 0; i < mNoisyPixels[0].size(); i++){

		if(mNoisyPixels[0][i] > HitCut) 
		{
			std::cout << "Key = " << i << "  Hits = " << mNoisyPixels[0][i] << endl;
			mNoisyPixelsCut[0][i] = mNoisyPixels[0][i];
			NPixelCalb = NPixelCalb + 1;
		}
	}
	

	TFile * fout = new TFile("KeyMap.root","RECREATE");
	fout->cd();
	fout->WriteObject(&mNoisyPixels,"Noise");
	fout->WriteObject(&mNoisyPixelsCut,"NoiseCut");
	fout->Close();

	cout << "Total Digits Processed = " << nd << endl;
	cout << "NPixelCalb = " << NPixelCalb << endl;
	
}
