
Configuration management after an ESA release

1) Increment version number in
   epr_api.h (EPR_PRODUCT_API_VERSION_STR), build.xml,
   VERSION.txt, README.txt, CHANGELOG.txt, doxyfile.txt
2) Provide new entry in CHANGELOG.txt
3) Adapt / give a hint in README.txt
4) Optional: Generate API documentation with "doxygen"
   cd $MY_PROJECTS_DIR$
   cd docs
   doxygen Doxyfile.txt

watch out:
    Check english in API log and error messages!
    Don't modify or hack example files without testing!
    Remove test dumps (MPH.txt ...) from release builds!
    Test release builds (with code optimization on)!
    Check & run examples before a release!
    Search in files for: "todo"
