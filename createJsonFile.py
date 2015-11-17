#!/usr/bin/env python
import sys
import os
import re
from ROOT import TFile, TIter, TKey, TH2F
import json
import pickle

args = sys.argv[1:]
if len(args) > 0: inputTree = args[0]
print "input tree=", inputTree

if len(args) > 1: outputJson = args[1]
print "output json=", outputJson

#from array import *
#import math
#import pickle


def getValueError(value, error):
    binEntry={}
    binEntry["value"]=value
    binEntry["error"]=error
    return binEntry

def getHistoContentInJson(histo):
    xBins={}
    histoName=histo.GetName()
    xaxisName = re.split("_",histoName)[0]
    yaxisName = re.split("_",histoName)[1]
    if (histo.GetYaxis().GetNbins()==1):
        print "this is a 1D histo"
        for i in range(1,histo.GetXaxis().GetNbins()+1):
            xBinValue=xaxisName+":["+str(histo.GetXaxis().GetBinLowEdge(i))+","+str(histo.GetXaxis().GetBinUpEdge(i))+"]"
            xBins[xBinValue]=getValueError(histo.GetBinContent(i), histo.GetBinError(i))
    else :
        for i in range(1,histo.GetXaxis().GetNbins()+1):
            yBins={}
            xBinValue=xaxisName+":["+str(histo.GetXaxis().GetBinLowEdge(i))+","+str(histo.GetXaxis().GetBinUpEdge(i))+"]"
            for j in range(1,histo.GetYaxis().GetNbins()+1):
                yBinValue=yaxisName+":["+str(histo.GetYaxis().GetBinLowEdge(j))+","+str(histo.GetYaxis().GetBinUpEdge(j))+"]"
                yBins[yBinValue]=getValueError(histo.GetBinContent(i,j), histo.GetBinError(i,j))
            xBins[xBinValue]=yBins
    return xBins

data={}

rootoutput = TFile.Open(inputTree)

nextkey = TIter(rootoutput.GetListOfKeys())
key = nextkey.Next()
while (key): #loop
    if key.IsFolder() != 1:
        continue
    print "will get the SF in the for ID/bis ", key.GetTitle()
    directory = rootoutput.GetDirectory(key.GetTitle())
    keyInDir = TIter(directory.GetListOfKeys())
    subkey = keyInDir.Next()
    efficienciesForThisID = {}
    while (subkey):
        if "ratio" in subkey.GetName():
            theHistoPlot = rootoutput.Get(key.GetTitle()+"/"+subkey.GetName())
            efficienciesForThisID[subkey.GetName()] = getHistoContentInJson(theHistoPlot)
        subkey = keyInDir.Next()
    data[key.GetTitle()]=efficienciesForThisID
    key = nextkey.Next()


with open(outputJson,"w") as f:
    json.dump(data, f, sort_keys = False, indent = 4)

outputPickle = outputJson.replace('json', 'pkl')

with open(outputPickle,"w") as f:
    pickle.dump(data, f)


