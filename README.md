# rmzdb
R bindings for the libmzdb C library

# Getting started



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
