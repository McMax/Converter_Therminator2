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

int test()
{
	TFile *inputfile = new TFile("events.root");
	TTree *eventtree = (TTree*)inputfile->Get("events");
	TTree *particletree = (TTree*)inputfile->Get("particles");
	Int_t Nentries;
	StructEvent event;
	ParticleCoor particle;

	eventtree->SetBranchAddress("event",&event);
	particletree->SetBranchAddress("particle",&particle);

	Nentries = eventtree->GetEntries();

	TH1I *eids = new TH1I("Event IDs", "eids", 500, 0, 5e6);
	TH1I *entries = new TH1I("Number of entries", "entries", 30, 0, 30);

	TH1F *px = new TH1F("Px", "px", 100, -16, 16);
	TH1F *py = new TH1F("Py", "py", 100, -16, 16);
	TH1F *pz = new TH1F("Pz", "pz", 500, -16, 16);

	UInt_t part_iter = 0;

	for(Int_t i=0; i<Nentries; i++)
	{
		eventtree->GetEntry(i);
		eids->Fill(event.eventid);
		entries->Fill(event.entries);

		for(Int_t j=part_iter; j<(part_iter+event.entries); j++)
		{
			particletree->GetEntry(j);
			px->Fill(particle.px);
			py->Fill(particle.py);
			pz->Fill(particle.pz);
			cout << "PID: " << particle.pid << endl;
		}
		
		part_iter += event.entries;
	}


	TCanvas *canva = new TCanvas("Test","canva",1200,600);
	canva->Draw();
	canva->Divide(3,2);
	canva->cd(1);
	eids->Draw();
	canva->cd(2);
	entries->Draw();
	canva->cd(4);
	px->Draw();
	canva->cd(5);
	py->Draw();
	canva->cd(6);
	pz->Draw();


	return 1;
}
