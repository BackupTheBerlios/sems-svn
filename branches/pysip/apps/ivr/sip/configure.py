import os
import sipconfig

# The name of the SIP build file generated by SIP and used by the build
# system.
build_file = "py_ivr.sbf"

# Get the SIP configuration information.
config = sipconfig.Configuration()

# Run SIP to generate the code.
os.system(" ".join([config.sip_bin, "-c", ".", "-b", build_file, "py_ivr.sip"]))

# Create the Makefile.
makefile = sipconfig.SIPModuleMakefile(config, build_file, makefile="Makefile.gen")

makefile.extra_cxxflags = ["-Wall -Wno-reorder"]
makefile.extra_include_dirs = ["$(COREPATH)"]
makefile._warnings = 0

# Generate the Makefile itself.
makefile.generate()