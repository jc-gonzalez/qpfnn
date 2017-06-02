# The line after this long comment block shows the reg.ex. for the
# file naming convention.
#
# This is just a first attempt for a reg.ex. that works with the current
# File Naming Convention, a more elaborated one will be released soon,
# when the FNC is in a more mature state.
#
# The application assumes this the product file names are formed with this 
# identifier, followed by a dot and the file extension
#
# The convention is, according to the current convention, in the form:
#   MMM_FFF_<params>_<date>_<release>
# with the following meaning for the different fields:
#  MMM       - Mission / Survey / Catalog / Telescope ID., 3 upcase letters
#  FFF       - Processing function acronym, 3 uppercase letters
#  <params>  - DataProduct parameters, hyphen separated, including (in the
#              following order, in case they appear):
#               + Instrument (VIS/NIaR/SIR)
#               + ObsMode
#               + ObservationId
#               + Exposure 
#               + SpectralBand
#               + rest of parameters, in no special order (Spectral range,
#                 Data Type, SubType, ...)
#              Max 45, letters, digits and can contain hyphens
#  <date>    - The format of the creation date is fixed on 18 characters :
#                     YYYYMMDDTHHmmss.sZ 
#              Example : 20201212T120023.5Z
#  <release> - Format is 00.00, 2 characters plus dot plus 2 characters 
#
# Example:
#  EUC_SIM_VIS-W-20034-4-STACKCALIBR-ASFGETPIX_20231212T230035.4Z_00.01 

([A-Z]+)_([A-Z0-9]+)_([^_]+)_([0-9]+T[\.0-9]+Z)[_]*([0-9]*\.*[0-9]*)
