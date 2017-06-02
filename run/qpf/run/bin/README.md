
EXECUTION SCRIPTS FOR QLA FRAMEWORK
===================================

The scripts in this folder are executed sequentially upon the execution of a
task. The sequence is started by the QLA Framework.  The scripts have the
functionality described below, and the order of execution is explained:

 * runTask.sh <proc.elem.> <cfg.file>

   This script is devoted to the instantiation of a Docker container. The path
   of the provided cfg. file name is used to build the local path name and the
   associated (container related) image path name.  The cfg.file as such could
   be non-exiting, since it is not used in the current version.  This script
   calls the runDocker.sh script, with the name of the processing element to
   be executed, and the local and image paths to be used.

 * runDocker.sh <proc.elem> <local-path> <image-path>

   This script is used to launch the appropriate Docker container, in order to
   execute the desired processing element.  The script also provides in
   <local-path>/docker.id the image id of the launched docker container.

 * QLA_VIS <cfg.file>

   This script is a wrapper for the Python QLA code to process input VIS_IMG
   files in FITS format.  The script runs *inside* the launched Docker
   container.  It uses the path in the cfg.file as the working directory, and
   then calls the Python script in this container.

