# rmzdb
R bindings for the libmzdb C library

# Getting started

## How to open/close an mzDB connection?

To open a mzdb file with rmzdb, you only need to create a new "MzDb" object, with your database's path as argument. 

Example:
```R
library(rmzdb)
mzdb_obj <- new("MzDb", "this_is_a_path")
```

The database will be closed when the mzdb_obj object is destroyed. 

## How to retrieve a single spectrum?

To retrive a single spectrum, you need to call the function of the class "MzDb" : get_spectrum. It takes as argument the id of the spectrum and will returned an instance of this one.

Example:
```R
id <- 1 #the id of your spectrum
spectrum <- mzdb_obj$get_spectrum(id)
```

WARNING: this function will not be efficient if you want to access to a large number of spectra. In this case, you should probably better use an iterator.

### How to display its [m/z ; intensity] data?

The data are stored in mzdb_obj$get_data(). It will returned an object which contains the intensity array ( accessible with get_intensity_list() ) and the mz array ( accessible with get_mz_list() ).

Example:
```R
spectrum_data <- spectrum$get_data()
mz_list <- spectrum_data$get_mz_list() #the mz list returned as Numeric Vector
intensity_list <- spectrum_data$get_intensity_list() #the intensity list returned as Numeric Vector
```

### How to access its meta-data (spectrum header)?

The meta-data/header is stored in mzdb_obj$get_header(). It will return an object which contains all the meta-data.

Example:
```R
spectrum_header <- spectrum$get_header()
spectrum_header$title #will retrieve the title of this spectrum
```

### How to access XML meta-data ?

Example:
```R
writeLines(spectrum_header()$param_tree)
writeLines(spectrum_header()$scan_list)
writeLines(spectrum_header()$precursor_list)
```

This should print something similar to

```XML
<params>
  <cvParams>
    <cvParam cvRef="MS" accession="1000511" name="ms level" value="2" />
    <cvParam cvRef="MS" accession="1000580" name="MSn spectrum" />
    <cvParam cvRef="MS" accession="1000130" name="positive scan" />
    <cvParam cvRef="MS" accession="1000127" name="centroid spectrum" />
    <cvParam cvRef="MS" accession="1000505" name="base peak intensity" value="84" />
    <cvParam cvRef="MS" accession="1000504" name="base peak m/z" value="281.028" />
    <cvParam cvRef="MS" accession="1000285" name="total ion current" value="840" />
    <cvParam cvRef="MS" accession="1000128" name="profile spectrum" />
  </cvParams>
  <userParams>
    <userParam cvRef="MS" accession="-1" name="in_high_res" type="xsd:boolean" value="true" />
  </userParams>
</params>

<scanList count="1">
  <cvParam cvRef="MS" accession="MS:1000795" name="no combination" value=""/>
  <scan>
    <cvParam cvRef="MS" accession="MS:1000016" name="scan start time" value="13.0586" unitCvRef="UO" unitAccession="UO:0000031" unitName="minute"/>
    <cvParam cvRef="MS" accession="MS:1000616" name="preset scan configuration" value="4"/>
    <scanWindowList count="1">
      <scanWindow>
        <cvParam cvRef="MS" accession="MS:1000501" name="scan window lower limit" value="100.0" unitCvRef="MS" unitAccession="MS:1000040" unitName="m/z"/>
        <cvParam cvRef="MS" accession="MS:1000500" name="scan window upper limit" value="1600.0" unitCvRef="MS" unitAccession="MS:1000040" unitName="m/z"/>
      </scanWindow>
    </scanWindowList>
  </scan>
</scanList>

<precursor>
  <cvParam cvRef="MS" accession="MS:1000827" name="isolation window target m/z" value="543.945422212578" unitCvRef="MS" unitAccession="MS:1000040" unitName="m/z"/>
  <cvParam cvRef="MS" accession="MS:1000829" name="isolation window upper offset" value="544.445422212578"/>
  <cvParam cvRef="MS" accession="MS:1000828" name="isolation window lower offset" value="543.445422212578"/>
  <selectedIonList count="1">
    <selectedIon>
      <cvParam cvRef="MS" accession="MS:1000744" name="selected ion m/z" value="543.945422212578" unitCvRef="MS" unitAccession="MS:1000040" unitName="m/z"/>
    </selectedIon>
  </selectedIonList>
  <activation>
    <cvParam cvRef="MS" accession="MS:1000422" name="beam-type collision-induced dissociation" value=""/>
  </activation>
</precursor>
```

## How to iterate all spectra?

To iterate, you need to create an object "SpectrumIterator". It takes two arguments: a "MzDb" object and a ms_level (integer value). If you want to iterate all spectra, you need to set the ms_level argument to -1.

Two methods are available from the iterator object: has_next() and next_elem(). The has_next() method will return "true" until there is no remaining spectrum to load. The next_elem() method, will retrieve an instance of the current spectrum. 

Example:
```R
it <- new(SpectrumIterator, mzdb_obj, -1)

while (it$has_next())
{
  print(it$next_elem()$get_header()$title)
}
```

## How to iterate spectra for a specific MS level?

If you want to iterate on only one ms_level, you must create a "SpectrumIterator" and set the flag ms_level with the wanted ms_level.

Example:
```R
it <- new(SpectrumIterator, mzdb_obj, 2)
```

## Other available mzDB queries

All mzDB queries are available in the "MzDb" object. 

```R

### Read model & ProteoWizard versions ###
mzdb_obj$get_model_version() # return the model version
mzdb_obj$get_pwiz_mzdb_version() # return the pwiz version

### Main MS and LC information  ###
mzdb_obj$get_max_ms_level() # return get_max_ms_level
mzdb_obj$get_cycles_count() # return the cycle count
mzdb_obj$get_last_time() # return the last time in seconds

### Data enconding information  ###
mzdb_obj$get_data_encoding_id(3) # return the bbox data encoding id w/ bb id: 3
mzdb_obj$get_data_encodings_count_from_sequence() # return the data encodings count from sequence

### Spectra statistics ###
mzdb_obj$get_spectra_count_from_sequence() # return the spectrum count from sequence
mzdb_obj$get_spectra_count(1) # return the spectra count w/ ms_level: 1
mzdb_obj$get_spectra_count(2) # return the spectra count w/ ms_level: 2

### Run slices information ###
mzdb_obj$get_run_slices_count_from_sequence() # return the run slices count from sequence
mzdb_obj$get_ms_level_from_run_slice_id_manually(45) # return the ms level from a run slice id w/ run slice id: 45

### Bounding boxes information ###
mzdb_obj$get_bounding_boxes_count_from_sequence() # return the bbox count from sequence
mzdb_obj$get_bounding_boxes_count(1) # return the bbox count w/ spectrum id: 1
mzdb_obj$get_bounding_box_first_spectrum_id(1024) # return the bbox first spectrum id count w/ first id: 1024
mzdb_obj$get_bounding_box_ms_level(72) # return the bbox ms level w/ bb id: 72

```

## How to handle mzDB error

To know the last error RC (SQLite result code, see https://www.sqlite.org/c3ref/c_abort.html), you need to call mzdb_obj$get_last_rc(), if this one is not equals to 0, an error has occured. To get the error message link to the last RC, you need to call mzdb_obj$get_last_error(), if no error occured, the output will be "no error".
Note that when using iterators RC values (returned by has_next and next_elem methods) are different than the SQLite ones.

# Tutorials for curious developers

## How this Rcpp package has been created?

* From Rstudio => install.packages("Rcpp")
* From Rstudio => Rcpp.package.skeleton("rmzdb")
* Add C/CPP files
* In the CPP file add a section "RCPP_MODULE(rmzdb) { ... }" to specify the Rcpp bindings
* Solve issue of C/CPP cross linking using extern "C" (https://stackoverflow.com/questions/38761620/undefined-reference-to-error-while-linking-object-files) => In Cpp files include C file headers by wrapping in an "extern C {}" block
* In the main C++ file don't forget to add RCPP_EXPOSED_CLASS(MzDb) (https://stackoverflow.com/questions/28506079/can-rcpp-expose-a-c-class-method-taking-a-reference-to-the-same-class)
* Delete RcppExports.cpp
* From Rstudio => execute compileAttributes(pkgdir = "./rmzdb", verbose = getOption("verbose"))
* In DESCRIPTION file add: RcppModules: rmzdb
* In NAMESPACE file add: import(Rcpp)
* In R folder add a zzz.R file containing: loadModule("rmzdb", TRUE)
* From parent dir (shell) => R CMD build rmzdb --no-multiarch
* From Rstudio => install.packages("rmzdb", repos = NULL, type="source", INSTALL_opts=c("--no-multiarch"))

For more information read the doc:
http://dirk.eddelbuettel.com/code/rcpp/rcpp.html

## How to perform a partial build on windows?

Execute a script similar to
```
cd D:\rmzdb\src\
c:/Rtools/mingw_64/bin/g++ -I"C:/PROGRA~1/R/R-35~1.1/include" -DNDEBUG  -I"C:/Program Files/R/R-3.5.1/library/Rcpp/include" -O2 -Wall -mtune=generic -c mzdb_reader.cpp -o mzdb_reader.o
c:/Rtools/mingw_64/bin/gcc  -I"C:/PROGRA~1/R/R-35~1.1/include" -DNDEBUG  -I"C:/Program Files/R/R-3.5.1/library/Rcpp/include" -O2 -Wall  -std=gnu99 -mtune=generic -c libmzdb.c -o libmzdb.o
c:/Rtools/mingw_64/bin/g++ -shared -s -static-libgcc -o rmzdb.dll RcppExports.o mzdb_reader.o libmzdb.o sqlite3.o parser.o -LC:/PROGRA~1/R/R-35~1.1/bin/x64 -lR

cp -f D:\rmzdb\src\rmzdb.dll "C:\Program Files\R\R-3.5.1\library\rmzdb\libs\x64\rmzdb.dll"
```
