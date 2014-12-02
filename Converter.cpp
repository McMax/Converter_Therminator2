#include "ParticleTree.h"
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

int main()
{
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

	UInt_t part_iter = 0;

	for(Int_t i=0; i<Nentries; i++)
	{
		eventtree->GetEntry(i);

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
	}

	particletree.Close();
	inputfile->Close();

	return 0;
}
