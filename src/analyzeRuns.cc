#include <iostream>
#include <vector>
#include <string>

// Root Cern Stuff
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TF1.h"


using namespace std;


// Calculate the relativistic velocity of particle
// Assumes energy in MeV
double getBeta(double E)
{
    return TMath::Sqrt(1. - TMath::Power(( 0.511 / E ), 2.) );
}

// Get the cherenkov angle
double getTheta(double n, double E)
{
    double beta = getBeta(E);
    return TMath::ACos(1. / beta / n);
}


// 3rd order parabola
double parabola(Double_t *x, Double_t *par)
{
    // p0 * x^3 + p1 * x^2 + p2 * x + p3
    return par[0] * x[0] * x[0] * x[0] + par[1] * x[0] * x[0] + par[2] * x[0] + par[3];
}


// Get the maximum of the parabola
// Hastily converted from python...
void getParMaxErr(vector <double> &max, vector <double> &max_err, double *par, double *par_err)
{

    // d/dx of 3rd order poly can be expressed as the 2nd order
    double a = 3*par[0];
    double b = 2*par[1];
    double c = par[2];

    // Error on a,b,c
    double da = 3*par_err[0];
    double db = 2*par_err[1];
    double dc = par_err[2];

    // Will reuse alot
    // # b^2 - 4ac
    double inner = TMath::Power(b,2) - 4*a*c;

    // Since we have 2 roots...
    // Clear and reset to zero
    max.clear();
    max.assign(2,0);
    max_err.clear();
    max_err.assign(2,0);
    double dfda[2] = {0,0};
    double dfdb[2] = {0,0};
    double dfdc[2] = {0,0};

    // eqn 1 - [-b +/- sqrt(b^2 - 4ac)] / 2a
    max[0] = (-b + TMath::Sqrt(inner)) / 2 / a;
    max[1] = (-b - TMath::Sqrt(inner)) / 2 / a;


    // eqn 1 can be written as f = uv
    double u[2] = {0,0};
    double v[2] = {0,0};

    u[0] = -b + TMath::Sqrt(inner);
    u[1] = -b - TMath::Sqrt(inner);

    v[0] = 1./(2.*a);
    v[1] = 1./(2.*a);

    // We then calculate the error on the roots
    // by calculating the dervites of u and v
    double duda[2] = {0,0};
    double dudb[2] = {0,0};
    double dudc[2] = {0,0};

    double dvda[2] = {0,0};
    double dvdb[2] = {0,0};
    double dvdc[2] = {0,0};

    duda[0] = - 2*c *TMath::Power(inner,(-1/2));
    duda[1] = 2*c *TMath::Power(inner,(-1/2));
    dvda[0] = - 1./ (2.*TMath::Power(a,2));
    dvda[1] = - 1./ (2.*TMath::Power(a,2));

    //  Product rule
    //  dfdx = dudx *v + u*dvdx
    dfda[0] = duda[0]*v[0] + dvda[0]*u[0];
    dfda[1] = duda[1]*v[1] + dvda[1]*u[1];


    dudb[0] = -1 + b*TMath::Power(inner,(-1/2));
    dudb[1] = -1 - b*TMath::Power(inner,(-1/2));
    dvdb[0] = 0;
    dvdb[1] = 0;

    // Product rule
    // dfdx = dudx *v + u*dvdx
    dfdb[0] = dudb[0]*v[0] + dvdb[0]*u[0];
    dfdb[1] = dudb[1]*v[1] + dvdb[1]*u[1];


    dudc[0] = - 2*a *TMath::Power(inner,(-1/2));
    dudc[1] = 2*a *TMath::Power(inner,(-1/2));
    dvdc[0] = 0;
    dvdc[1] = 0;

    // Product rule
    // dfdx = dudx *v + u*dvdx
    dfdc[0] = dudc[0]*v[0] + dvdc[0]*u[0];
    dfdc[1] = dudc[1]*v[1] + dvdc[1]*u[1];

    // Propagation of errors
    // \Delta f = sqrt(\sum_i (dfdx_i)^2 (\Delta x_i)**2 )
    // Neglecting cross terms because I'm 2 cool 4 school
    max_err[0] = TMath::Power(dfda[0],2) * TMath::Power(da,2) + TMath::Power(dfdb[0],2) * TMath::Power(db,2) + TMath::Power(dfdc[0],2) * TMath::Power(dc,2);
    max_err[1] = TMath::Power(dfda[1],2) * TMath::Power(da,2) + TMath::Power(dfdb[1],2) * TMath::Power(db,2) + TMath::Power(dfdc[1],2) * TMath::Power(dc,2);
    max_err[0] = TMath::Sqrt(max_err[0]);
    max_err[1] = TMath::Sqrt(max_err[1]);

}


// Get the radius for a given refractive index and energy
double getRadius(double n, double E)
{
    double thetac = getTheta(n,E);
    double beta = getBeta(E);

    double ze = 5.;//mm
    double d = 277.0926+ 10; //mm
    double n0 = 1.0003;

    // double tanc = TMath::ATan(theta);
    // double r = ze * tanc;
    // r += d * tanc  / TMath::Sqrt(n0*n0*beta*beta - tanc*tanc);
    return d * TMath::Tan(thetac) / (TMath::Sqrt((TMath::Power(n0,2) * TMath::Power(beta,2)) - TMath::Power(TMath::Tan(thetac),2))) + ze * TMath::Tan(thetac);
    // return r;
}


// Wrapping this in a function a little friendlier for TF1
double wrap_getRadius(Double_t *x, Double_t *par)
{
    return getRadius(par[0], x[0]);
}



// Function to reduce single run
void processRun(TTree *data, double &mean, double &std, double &mean_err, double &std_err, double &par, double &par_err)
{
    int rebin = 7;
    int n_bins = 3694/rebin;
    double bin_up = 214.772;
    double bin_low = bin_up - (n_bins * 8e-3 * rebin);
    TH1D *fH1 = new TH1D("fH1", "fH1", n_bins, bin_low, bin_up);
    // Using the draw option to fille the histogram
    data->Draw("TMath::Sqrt(X^2+Y^2)>>fH1");


    TF1 *fGaus = new TF1("fGaus", "gaus", 190,210);

    fH1->Fit("fGaus", "LE");
    mean = fGaus->GetParameter(1);
    std = fGaus->GetParameter(2);
    mean_err = fGaus->GetParError(1);
    std_err = fGaus->GetParError(2);

    double window = 3;

    // Fit parabola to data
    TF1 *fPar = new TF1("fPar", parabola, mean -window,mean +window, 4);
    fH1->Fit("fPar", "LER");

    vector <double> fmax(2);
    vector <double> fmax_err(2);
    // void getParMaxErr(vector <double> &max, vector <double> &max_err, double *par, double *par_err)

    getParMaxErr(fmax, fmax_err, (double*)fPar->GetParameters(), (double*)fPar->GetParErrors());

    // Select the correct root
    int iret = 0;
    if ( fmax[0] < 190 || fmax[0] > 210 ) {iret = 1;}
    par = fmax[iret];
    par_err = fmax_err[iret];

    // delete fH1;
}


// Main function
int main(int argc, char *argv[])
{

    unsigned int i = 0;
    // Define data vectors for later
    vector <string> filename(argc-1);
    vector <double> energy(argc-1);
    vector <double> mean(argc-1);
    vector <double> std(argc-1);
    vector <double> mean_err(argc-1);
    vector <double> std_err(argc-1);
    vector <double> par(argc-1);
    vector <double> par_err(argc-1);

    vector <double> radius(argc-1);

    // Assumed spectral index
    double n = 1.15;
    for (i = 0; i < argc-1 ; i++)
    {
        filename[i] = argv[i+1];
        filename[i].append("MeV.root");
        energy[i] = atof(argv[i+1]);
        TFile f(filename[i].c_str());
        processRun((TTree*) f.Get("Photon"), mean[i], std[i], mean_err[i], std_err[i], par[i], par_err[i]);
        f.Close();

        radius[i] = getRadius(n, energy[i]);

    }


    TF1 *fRadius = new TF1("fRadius", wrap_getRadius, 10, 35, 1);
    fRadius->SetParameter(0, n);

    TGraphErrors *gMean = new TGraphErrors(energy.size(), &(energy[0]), &(mean[0]), 0, &(mean_err[0]));
    TGraphErrors *gPara = new TGraphErrors(energy.size(), &(energy[0]), &(par[0]), 0, &(par_err[0]));
    gMean->SetMarkerStyle(8);
    gMean->SetMarkerColor(kBlue);
    gMean->SetLineColor(kBlue);
    gMean->Fit("fRadius");


    // gPara->SetMarkerStyle(21);
    // gPara->SetMarkerColor(kBlack);
    // gPara->SetLineColor(kBlack);
    // gPara->Fit("fRadius");

    // Plot the cherenkov ring radius as a function of energy
    TCanvas *cRadius = new TCanvas();
    gMean->SetTitle("Cherenkov Ring Radius;Energy [MeV];Radius [mm]");
    gMean->Draw("AP");
    // gPara->Draw("SAME,P");
    // fRadius->Draw("SAME");
    cRadius->SetGrid();
    cRadius->Print("MeanRadius.png");
    for ( i = 0; i < energy.size(); i++)
    {
        cout << i << " " << energy[i] << " " << mean[i] << " " << std[i] << " " << par[i] << " " << par_err[i] << endl;
    }

    delete cRadius;

    return 0;
}