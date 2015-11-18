# How it works ? 
## SF extractor:
   * ` root -b -q -l 'extractPlotsAndComputeTheSFs.C("IDname_BINSname","pathToDataTnPOutputFile","pathToMCTnPOutputFile")' `

## JSON + Pkl dumper:
   * `createJsonFile.py theInputFile.root theOutputJsonFileName.json`

# Example:

  1. untar the example directory: 
   * `tar xfvz exampleTnPoutputs.tar.gz`
  2. run the macro: (one single time per TnP ouput file -> 3 times in the example) 
   * `root -b -q -l 'extractPlotsAndComputeTheSFs.C("Medium_EtaBins","Efficiency_v1/DATA_JSON1280/TnP_NUM_Medium_DEN_genTracks_PAR_eta.root","Efficiency_v1/MC_NLO/TnP_NUM_Medium_DEN_genTracks_PAR_eta.root")'`
   * `root -b -q -l 'extractPlotsAndComputeTheSFs.C("Medium_PtEtaBins","Efficiency_v1/DATA_JSON1280/TnP_NUM_Medium_DEN_genTracks_PAR_pt_eta.root","Efficiency_v1/MC_NLO/TnP_NUM_Medium_DEN_genTracks_PAR_pt_eta.root")'`
   * `root -b -q -l 'extractPlotsAndComputeTheSFs.C("Tight_PtEtaBins","Efficiency_v1/DATA_JSON1280/TnP_NUM_Tight2012_DEN_genTracks_PAR_pt_eta.root","Efficiency_v1/MC_NLO/TnP_NUM_Tight2012_DEN_genTracks_PAR_pt_eta.root")'`
   * It will produce in the example 3 root files starting by `EfficienciesAndSF_`
  3. merge the root files togeither
   * `$ROOTSYS/bin/hadd EfficienciesAndSF.root EfficienciesAndSF_*.root`
  4. dump the SFs in a json and pkl file
   * `createJsonFile.py EfficienciesAndSF.root theJSONfile.json`


