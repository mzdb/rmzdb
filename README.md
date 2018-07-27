# rmzdb
R bindings for the libmzdb C library

# Getting started

## How to open/close an mzDB connection?

To open a mzdb file with rmzdb, you only need to create a new "MzDb" object, with your database's path as argument. 
- mzdb_obj <- new("MzDb", "this_is_a_path")

The database will be closed when the object will be destroyed. 

## How to retrieve a single spectrum?

To retrive a single spectrum, you need to call the function of the class "MzDb" : get_spectrum. It takes as argument the id of the spectrum and will returned an instance of this one.

- id <- 1 #the id of your spectrum
- spectrum <- mzdb_obj$get_spectrum(id)

WARNING: this function is slow, if you need to access to a large number of spectrum, you should probably use an iterator.

### How to display its [m/z ; intensity] data?

The data are stored in mzdb_obj$get_data(). It will returned an object which contains the intensity array ( accessible with get_intensity_list() ) and the mz array ( accessible with get_mz_list() ).

- spectrum_data <- spectrum$get_data()
- mz_list <- spectrum_data$get_mz_list() #the mz list
- intensity_list <- spectrum_data$get_intensity_list() #the intensity list

### How to access its meta-data (spectrum header)?

The meta-data/header is stored in mzdb_obj$get_header(). It will returned an object which contains all the meta-data.

- spectrum_header <- spectrum$get_header()
- spectrum_header$title #will retrieve the title of your spectrum


## How to iterate all spectra?

To iterate, you need to create an object "SpectrumIterator". It's takes as argument a "MzDb" object and a ms_level. If you want to iterate all spectra, you need to set the ms_level argument with -1.

Two function are available, has_next() and next_elem(). The first one will returned "true" if remains spectrum to iterate, else "false". The second, will retrived an instance of the current spectrum. 

- it <- new(SpectrumIterator, m, -1)

- while (it$has_next())
- {
- print(it$next_elem()$get_header()$title)
- }

## How to iterate spectra for a specific MS level?

If you want to iterate on only one ms_level, you must create a "SpectrumIterator" and set the flag ms_level with the whished ms_level.

## Other available mzDB queries

All mzDB queries are available in the "MzDb" object. 

- m$get_model_version()
- m$get_last_time()

## How to handle mzDB error

To know the last error rc, you need to call mzdb_obj$get_last_rc(), if this one is not equals to 0, an error has occured. To get the error message link to the last rc, you need to call mzdb_obj$get_last_error(), if no error occured, the output will be "no error".

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
