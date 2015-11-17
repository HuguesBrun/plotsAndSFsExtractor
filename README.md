# How it works ? 
` root -b -q -l 'extractPlotsAndComputeTheSFs.C("pathToDataTnPOutputFile","pathToDataTnPOutputFile")' `

## Example:

  1. untar the example directory: `tar xfvz exampleTnPoutputs.tar.gz`
  2. run the macro: `root -b -q -l 'extractPlotsAndComputeTheSFs.C("Medium_EtaBins","Efficiency_v1/DATA_JSON1280/TnP_NUM_Medium_DEN_genTracks_PAR_eta.root","Efficiency_v1/MC_NLO/TnP_NUM_Medium_DEN_genTracks_PAR_eta.root")'`
  3. the efficiencies in data and MC + the SFs should be in the following file: `EfficienciesAndSF_Medium_NUM_Medium_DEN_genTracks_PAR_eta.root`
