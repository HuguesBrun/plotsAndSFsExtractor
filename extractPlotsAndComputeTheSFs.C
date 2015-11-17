

//files to get the efficiencies
TFile *efficienciesDATA;
TFile *efficienciesMC;


/*TString typePlot(TString theCanvasWithThePlot){
    TCanvas *theCanvas = (TCanvas*) efficienciesDATA->Get(theCanvasWithThePlot);
    
}*/

float max(float a, float b){
    if (a>b) return a;
    else return b;
}
float min(float a, float b){
    if (a>b) return b;
    else return a;
}


float findPathologocialErrors(TString binName, float centralValue, float highSideError, float lowSideError){
    if ((lowSideError > (3 * highSideError)) || (highSideError > (3 * lowSideError))){
        if (centralValue<0.99){
            if (lowSideError > (3 * highSideError)){
                if (highSideError < 0.333*(1-centralValue)){ //trigger the correction only if the error is small enough
                    cout << "WARNING !!!!, found a patological error in " << binName << " please check" << endl;
                    return min(lowSideError, highSideError);
                }
            }
            else {
                cout << "WARNING !!!!, found a patological error in " << binName << " please check" << endl;
                return min(lowSideError, highSideError);
            }
        }
        else {
            cout << "WARNING !!!!! very assymetrical errors but close to one, no correction triggered, please check plot " << binName << endl;
        }
    }
    return -1;
}


TString makeNameSmaller(TString theInputName){
    TObjArray* Strings = theInputName.Tokenize("PLOT");
    TString nomSF = ((TObjString *)(Strings->At(0)))->String();
    return nomSF.Remove(nomSF.Length()-1);
}


TH1F *convertGraphInHisto_1D(TGraphAsymmErrors *theGraph){
    //init
    Double_t xerr[1000];
    Double_t yerr[1000];
    Double_t xbinlimits[1001];
    for (Int_t j=0;j<1000;j++){
        xerr[j] = 0.0;
        yerr[j] = 0.0;
        xbinlimits[j] = 0.0;
    }
    xbinlimits[1000] = 0.0;
    
    
    //get points and info
    int Npnts= theGraph->GetN();
    double *x = theGraph->GetX();
    double *y = theGraph->GetY();
    TString grName = theGraph->GetName();
    
    //get errors and create bin limits
    for (Int_t j=0;j<Npnts;j++){
        xerr[j] = theGraph->GetErrorXlow(j);
        yerr[j] = max(theGraph->GetErrorYlow(j), theGraph->GetErrorYhigh(j));
        float foundPathoError = findPathologocialErrors(grName, y[j], theGraph->GetErrorYhigh(j), theGraph->GetErrorYlow(j));
        if (foundPathoError>1) yerr[j] = foundPathoError;
        xbinlimits[j]=x[j]-xerr[j];
    }
    xbinlimits[Npnts]=x[Npnts-1]+theGraph->GetErrorXhigh(Npnts-1);

    TH1F *HistoOut = new TH1F(grName, grName,Npnts, xbinlimits);
    for (Int_t j=0;j<Npnts;j++){
        HistoOut->Fill(x[j],y[j]);
        HistoOut->SetBinError(j+1,yerr[j]);
    }
    HistoOut->Print();
    return HistoOut;
}

TH1F *computeTheSF_1D(TGraphAsymmErrors *graphDATA, TGraphAsymmErrors *graphMC){
    TH1F *histoDATA = convertGraphInHisto_1D(graphDATA);
    TH1F *histoMC   = convertGraphInHisto_1D(graphMC);
    TString nomHistogram = graphDATA->GetName();
    nomHistogram.ReplaceAll("_DATA","_ratio");
    TH1F *ratio = (TH1F*) histoDATA->Clone(nomHistogram);
    ratio->Sumw2();
    ratio->Divide(histoDATA, histoMC, 1, 1);
    return ratio;
}

void extractPlotsAndComputeTheSFs(TString theIDname, TString dataFile, TString mcFile){
    efficienciesDATA = new TFile(dataFile);
    efficienciesMC = new TFile(mcFile);
    TDirectory *plotDirectory = efficienciesDATA->GetDirectory("tpTree");
    TIter listPlotsDATA(plotDirectory->GetListOfKeys());
    TKey *keyDATA;
    while ((keyDATA = (TKey*)listPlotsDATA())) {
        TString typeDirectory = keyDATA->GetClassName();
        TString nomDirectory = keyDATA->GetTitle();
        if (typeDirectory== "TDirectoryFile"){
            cout << "getting the efficiencies in " << nomDirectory << endl;
            TFile *myOutFile = new TFile("EfficienciesAndSF_"+nomDirectory+".root","RECREATE");
            TDirectory *theMainDirectory = myOutFile->mkdir(theIDname);
            TDirectory *dataPlots = theMainDirectory->mkdir("efficienciesDATA");
            TDirectory *mcPlots = theMainDirectory->mkdir("efficienciesMC");
            TDirectory *plotFinalDirectory = efficienciesDATA->GetDirectory("tpTree/"+nomDirectory+"/fit_eff_plots");
            TIter binsEffienciesDATA(plotFinalDirectory->GetListOfKeys());
            TKey *keyEffDATA;
            /// The same key in MC will be used later
            TDirectory *plotFinalDirectoryMC = efficienciesMC->GetDirectory("tpTree/"+nomDirectory+"/fit_eff_plots");
            TIter binsEffienciesMC(plotFinalDirectoryMC->GetListOfKeys());
            TKey *keyEffMC;
            /// end of MC keys
            cout << plotFinalDirectory->GetNkeys() << endl;
            int nbOfHistos = plotFinalDirectory->GetNkeys();
            if (nbOfHistos==1) {
                keyEffDATA = (TKey*)binsEffienciesDATA();
                cout << "only 1 histo found -> will be 1D efficiencies and SF" << endl;
                TString nomContent = keyEffDATA->GetName();
                TCanvas *theCanvas = (TCanvas*) efficienciesDATA->Get("tpTree/"+nomDirectory+"/fit_eff_plots/"+nomContent);
                TIter nextObject(theCanvas->GetListOfPrimitives());
                TObject *obj;
                TGraphAsymmErrors *theGraphDATA;
                while ((obj = (TObject*)nextObject())) {
                    if (obj->InheritsFrom("TGraphAsymmErrors")) {
                        cout << "histo: " << obj->GetName() << endl;
                        theGraphDATA = (TGraphAsymmErrors*) obj;
                        break;
                    }
                }
                dataPlots->cd();
                TString smaller1Dname = makeNameSmaller(nomContent);
                theGraphDATA->SetName(smaller1Dname+"_DATA");
                theGraphDATA->Write(smaller1Dname+"_DATA");
                TH1F *theHistoDATA = convertGraphInHisto_1D(theGraphDATA);
                theHistoDATA->Write("histo_"+smaller1Dname+"_DATA");
                delete theHistoDATA;
                cout << "now getting the corresponding histo in MC" << endl;
                TCanvas *theCanvasMC = (TCanvas*) efficienciesMC->Get("tpTree/"+nomDirectory+"/fit_eff_plots/"+nomContent);
                TIter nextObjectMC(theCanvasMC->GetListOfPrimitives());
                TGraphAsymmErrors *theGraphMC;
                while ((obj = (TObject*)nextObjectMC())) {
                    if (obj->InheritsFrom("TGraphAsymmErrors")) {
                        cout << "histo: " << obj->GetName() << endl;
                        theGraphMC = (TGraphAsymmErrors*) obj;
                        break;
                    }
                }
                mcPlots->cd();
                theGraphMC->SetName(smaller1Dname+"_MC");
                theGraphMC->Write(smaller1Dname+"_MC");
                TH1F *theHistoMC = convertGraphInHisto_1D(theGraphMC);
                theHistoMC->Write("histo_"+smaller1Dname+"_MC");
                delete theHistoMC;
                // now will compute the SF !
                TH1F *the1Dhisto = computeTheSF_1D(theGraphDATA, theGraphMC);
                theMainDirectory->cd();
                the1Dhisto->Write();
                
            }
            else {
                cout << "will get the 2D efficiencies and SF" << endl;
                while ((keyEffDATA = (TKey*)binsEffienciesDATA())) {
                    TString typeContent = keyEffDATA->GetClassName();
                    TString nomContent = keyEffDATA->GetName();
                    keyEffMC = (TKey*)binsEffienciesMC();
                    TString typeContentMC = keyEffMC->GetClassName();
                    TString nomContentMC = keyEffMC->GetName();
                    TCanvas *theCanvas = (TCanvas*) efficienciesDATA->Get("tpTree/"+nomDirectory+"/fit_eff_plots/"+nomContent);
                    TObject *theObj;
                    if ((theObj = theCanvas->FindObject(nomContent))) {
                        TH2F *histo2D = (TH2F*) theObj;
                        TH2F *histo2DMC = (TH2F*) histo2D->Clone("histo2D_MC");
                        int nbYBins = histo2D->GetYaxis()->GetNbins();
                        int nbXBins = histo2D->GetXaxis()->GetNbins();
                        TKey *nextKey1D;
                        TKey *nextKey1Dmc;
                        for (int oneDhistos = 0 ; oneDhistos < nbYBins ; oneDhistos++ ){
                            nextKey1D =(TKey*)binsEffienciesDATA();
                            TString name1D = nextKey1D->GetName();
                            TCanvas *theCanvas1D = (TCanvas*) efficienciesDATA->Get("tpTree/"+nomDirectory+"/fit_eff_plots/"+name1D);
                            TIter nextObject1D(theCanvas1D->GetListOfPrimitives());
                            TObject *obj1D;
                            TGraphAsymmErrors *the1DAssymGraph;
                            while ((obj1D = (TObject*)nextObject1D())) {
                                if (obj1D->InheritsFrom("TGraphAsymmErrors")) {
                                    the1DAssymGraph = (TGraphAsymmErrors*) obj1D;
                                    break;
                                }
                            }
                            dataPlots->cd();
                            the1DAssymGraph->Write(name1D+"_DATA");
                            for (int xaxisBin = 0  ; xaxisBin < nbXBins ; xaxisBin++){
                                double x,y, lowError, highError;
                                the1DAssymGraph->GetPoint(xaxisBin,x,y);
                                lowError = the1DAssymGraph->GetErrorYlow(xaxisBin);
                                highError = the1DAssymGraph->GetErrorYhigh(xaxisBin);
                                histo2D->SetBinContent(xaxisBin+1,  oneDhistos+1, y);
                                if (lowError>highError) histo2D->SetBinError(xaxisBin+1,  oneDhistos+1, lowError);
                                else histo2D->SetBinError(xaxisBin+1,  oneDhistos+1, highError);
                                float foundPathoError = findPathologocialErrors(name1D, y, highError, lowError);
                                if (foundPathoError>1) histo2D->SetBinError(xaxisBin+1,  oneDhistos+1, foundPathoError);
                            }
                            nextKey1Dmc =(TKey*)binsEffienciesMC();
                            TString name1Dmc = nextKey1Dmc->GetName();
                            TCanvas *theCanvas1Dmc = (TCanvas*) efficienciesMC->Get("tpTree/"+nomDirectory+"/fit_eff_plots/"+name1Dmc);
                            TIter nextObject1Dmc(theCanvas1Dmc->GetListOfPrimitives());
                            TObject *obj1Dmc;
                            TGraphAsymmErrors *the1DAssymGraphMC;
                            while ((obj1Dmc = (TObject*)nextObject1Dmc())) {
                                if (obj1Dmc->InheritsFrom("TGraphAsymmErrors")) {
                                    the1DAssymGraphMC = (TGraphAsymmErrors*) obj1Dmc;
                                    break;
                                }
                            }
                            mcPlots->cd();
                            the1DAssymGraphMC->Write(name1Dmc+"_MC");
                            for (int xaxisBin = 0  ; xaxisBin < nbXBins ; xaxisBin++){
                                double x,y, lowError, highError;
                                the1DAssymGraphMC->GetPoint(xaxisBin,x,y);
                                lowError = the1DAssymGraphMC->GetErrorYlow(xaxisBin);
                                highError = the1DAssymGraphMC->GetErrorYhigh(xaxisBin);
                                histo2DMC->SetBinContent(xaxisBin+1,  oneDhistos+1, y);
                                if (lowError>highError) histo2DMC->SetBinError(xaxisBin+1,  oneDhistos+1, lowError);
                                else histo2DMC->SetBinError(xaxisBin+1,  oneDhistos+1, highError);
                                float foundPathoError = findPathologocialErrors(name1D, y, highError, lowError);
                                if (foundPathoError>1) histo2DMC->SetBinError(xaxisBin+1,  oneDhistos+1, foundPathoError);
                            }
                        }
                        theMainDirectory->cd();
                        TString shortNamePlot = makeNameSmaller(nomContent);
                        dataPlots->cd();
                        histo2D->Write(shortNamePlot+"_DATA");
                        mcPlots->cd();
                        histo2DMC->Write(shortNamePlot+"_MC");
                        theMainDirectory->cd();
                        TH2F *histo2Dratio = (TH2F*) histo2D->Clone(shortNamePlot+"_ratio");
                        histo2Dratio->Sumw2();
                        histo2Dratio->Divide(histo2D,histo2DMC,1,1);
                        histo2Dratio->Write();
                    }
                }
            }
            myOutFile->Close();
        }
    }
    
}