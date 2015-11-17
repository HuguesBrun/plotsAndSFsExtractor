#!/bin/csh



root -b -q -l 'extractPlotsAndComputeTheSFs.C("Medium_EtaBins","Efficiency_v1/DATA_JSON1280/TnP_NUM_Medium_DEN_genTracks_PAR_eta.root","Efficiency_v1/MC_NLO/TnP_NUM_Medium_DEN_genTracks_PAR_eta.root")'
root -b -q -l 'extractPlotsAndComputeTheSFs.C("Medium_PtEtaBins","Efficiency_v1/DATA_JSON1280/TnP_NUM_Medium_DEN_genTracks_PAR_pt_eta.root","Efficiency_v1/MC_NLO/TnP_NUM_Medium_DEN_genTracks_PAR_pt_eta.root")'
root -b -q -l 'extractPlotsAndComputeTheSFs.C("Tight_PtEtaBins","Efficiency_v1/DATA_JSON1280/TnP_NUM_Tight2012_DEN_genTracks_PAR_pt_eta.root","Efficiency_v1/MC_NLO/TnP_NUM_Tight2012_DEN_genTracks_PAR_pt_eta.root")'
