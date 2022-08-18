import uproot
import numpy as np
from glob import glob
from astropy.io import fits
from astropy.table import Table
import sys
import matplotlib.pyplot as plt
from scipy.signal import medfilt

# Rotate CCD by CCD angle
def RotateCCD(R, ccdID):
    deltaAngle = 360. / 16.0;
    angle = float(ccdID) * deltaAngle + 1.e-9 - 90
    angle = np.deg2rad(angle)
    # Minus because clockwise
    xcomp = -np.cos(angle)
    ycomp = np.sin(angle)
    return R*xcomp, R*ycomp


def plotData(R, Data):


    fig, axs = plt.subplots(4,4, figsize = (16,16), sharex=True, sharey= True)

    for i, ax in enumerate(axs.ravel()):
        
        ax.plot(R, medfilt(Data[i], 21))
        ax.set_title("CCD %d" %(i+1))
        ax.grid()

    return fig



def processFile(f):
    nPixels = 3694
    fCCDSize = 8e-6 # Size in m
    fCCDOffset = 21.4772e-2 # Size in m



    fData = uproot.open(f)
    fEle = fData["Electron"].arrays(library = 'np')
    fPho = fData["Photon"].arrays(library = 'np')
    fPho["R"] = np.sqrt(fPho["X"]**2 + fPho["Y"]**2)


    CCD_locations = np.zeros((3694, 16, 2))

    # Offseting and flipping the CCD order (outside-in)
    R = fCCDOffset - np.arange(nPixels) * fCCDSize
    R = R[::-1]


    CCDData = np.zeros((16, nPixels))

    CCDWidths = np.ones(nPixels) * R[1] - R[0]
    CCDEdges = np.zeros((nPixels+1))
    CCDEdges[:-1] = R - 0.5*CCDWidths
    CCDEdges[-1] = CCDEdges[-2] +CCDWidths[0]
    CCDCenters = CCDEdges[:-1] + 0.5 * CCDWidths


    CCDWidths *= 1000
    CCDEdges *= 1000
    CCDCenters *= 1000
    for ccdID in range(16):

        deltaAngle = 360. / 16.0;
        angle = float(ccdID) * deltaAngle + 1.e-5 - 90
        angle = np.deg2rad(angle)

        x = -np.cos(angle) / np.abs(np.cos(angle))
        y =  np.sin(angle) / np.abs(np.sin(angle))

        mask = np.abs( -np.tan(angle) * fPho["X"] - fPho["Y"] ) / np.sqrt(1 + np.tan(angle)**2) < 15
        if (ccdID != 0) and (ccdID !=8):
            mask *= fPho["X"] *  x > 0

        if (ccdID != 4) and (ccdID != 12):
            mask *= fPho["Y"] *  y > 0

        CCDData[ccdID,:], _ = np.histogram(fPho["R"][mask], bins = CCDEdges)


    data_dict = {}
    for i in range(16):
        data_dict["R_%d"%(i+1)] = CCDCenters/1000
        data_dict["CCD_%d"%(i+1)] = CCDData[i]

    tab = Table(data_dict)

    tabEle = Table(fEle)
    tabPhot = Table({
            "X" : fPho["X"],
            "Y" : fPho["Y"],
            "R" : fPho["R"]
        })

    hdul = fits.HDUList([fits.PrimaryHDU(), 
                     fits.BinTableHDU(tab), 
                     fits.BinTableHDU(tabEle), 
                     fits.BinTableHDU(tabPhot) ])

    hdul[1].name = "Binned CCD"
    hdul[2].name = "Electron Data"
    hdul[3].name = "Photon Data"


    return CCDCenters/1000, CCDData, hdul

if __name__ == "__main__":

    files = glob(sys.argv[1])
    for f in files:

        print (f)
        r, ccd, hdul = processFile(f)

        outfile = f.replace(".root", ".fits")
        hdul.writeto(outfile, overwrite=True)


