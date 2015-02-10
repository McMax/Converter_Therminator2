#include "ParticleTree.h"
#include "TRandom2.h"
#include "TH1.h"
#include <iostream>

using namespace std;

struct StructEvent
{
	UInt_t eventid;
	UInt_t entries;
	UInt_t entriesprev;
};

struct ParticleCoor
{
	Float_t mass;
	Float_t t, x,  y,  z;
	Float_t e, px, py, pz;
	Int_t   decayed;
	Int_t   pid;
	Int_t   fatherpid;
	Int_t   rootpid;
	Int_t   eid;
	Int_t   fathereid;
	UInt_t  eventid;
	Double_t  eventweight;
};

int main(int argc, char **argv)
{
	//TODO: Add probability weights switch
	
	bool weighting = true;
	TRandom2 *randGen;
	TDatime time;
	TH1D* cum_distr;

	TFile *inputfile = new TFile("events.root");
	TTree *eventtree = (TTree*)inputfile->Get("events");
	TTree *ptree = (TTree*)inputfile->Get("particles");
	Int_t Nentries;
	StructEvent event;
	ParticleCoor particle;
	ParticleTree particletree("ParticleTree.root");

	eventtree->SetBranchAddress("event",&event);
	ptree->SetBranchAddress("particle",&particle);

	Nentries = eventtree->GetEntries();

	if(weighting)
	{
		cum_distr = new TH1D("cum_distr","Cumulative distribution",Nentries+1, -0.5, Nentries+0.5);
		randGen = new TRandom2();
		randGen->SetSeed(time.GetTime());
	}

	UInt_t part_iter = 0;
	Double_t ev_weight = 0., ev_weight_sum = 0., gen_weight;

	if(weighting)
	{
		//First step: fill cumulative distribution
		for(Int_t i=0; i<Nentries; i++)
		{
			eventtree->GetEntry(i);

			for(UInt_t j=part_iter; j<(part_iter+event.entries); j++)
				ptree->GetEntry(j);

			part_iter += event.entries;

			ev_weight = particle.eventweight;
			ev_weight_sum += ev_weight;

			cum_distr->SetBinContent(i, ev_weight_sum);
		}

		cout << "Event weight sum: " << ev_weight_sum << endl;

		cum_distr->Scale(1/ev_weight_sum);

		TFile *ev_sum_hist_file = new TFile("EventWeight.root","recreate");
		ev_sum_hist_file->cd();
		cum_distr->Write();
		inputfile->cd();
		ev_sum_hist_file->Close();
	}

	part_iter = 0;


	for(Int_t i=0; i<Nentries; i++)
	{
		if(weighting)
		{
			gen_weight = randGen->Uniform();
			
			for(Int_t j=0; j<Nentries; j++)
			{
				if(cum_distr->GetBinContent(j+1) > gen_weight)
				{
					eventtree->GetEntry(j);
					break;
				}
			}

		}
		else
			eventtree->GetEntry(i);

		if(event.entries <= 2)
			continue;			//Pi wall effect cut

		particletree.BeginEvent();

		for(UInt_t j=part_iter; j<(part_iter+event.entries); j++)
		{
			ptree->GetEntry(j);
			if(particle.pid>0)
				particletree.AddParticle(1,0,0,particle.px,particle.py,particle.pz,0,0,0,0,0,0,0,0,particle.mass);
			else if(particle.pid<0)
				particletree.AddParticle(-1,0,0,particle.px,particle.py,particle.pz,0,0,0,0,0,0,0,0,particle.mass);
		}
		part_iter += event.entries;

		particletree.EndEvent();

		if((i%5000)==0)
			cout << "Event: " << i << endl;
	}

	particletree.Close();
	inputfile->Close();

	return 0;
}
