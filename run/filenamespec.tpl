# The line after this long comment block shows the reg.ex. for the
# file naming convention.
#
# The application assumes this the product file names are formed with this 
# identifier, followed by a dot and the file extension
#
# The convention is, according to the current convention, in the form:
#   MMM_FFF_<params>_<date>_<release>
# with the following meaning for the different fields:
#  MMM  - Mission / Survey / Catalog / Telescope ID., 3 uppercase letters
#  FFF  - Processing function acronym, 3 uppercase letters
#  <params> - DataProduct parameters including :
#             Instrument (VIS/NIR/SIR ) ObservationId, exposure, 
#             SpectralBand, Spectral range, Data Type, SubType, ...
#             max 45, letters, digits and can contain hyphens
#  <date> - The format of the creation date is fixed on 18 characters :
#                     YYYYMMDDTHHmmss.sZ 
#                     Example : 20201212T120023.5Z
#  <release> - Format is 00.00, 2 characters plus dot plus 2 characters 
#
#
# Example:
#  EUC_SIM_VIS-STACKCALIBR-W-20034-4-ASFGETPIX_20231212T230035.4Z_00.01 

([A-Z]+)_([A-Z0-9]+)_([^_]+)_([0-9]+T[\.0-9]+Z)[_]*([0-9]*\.*[0-9]*)
