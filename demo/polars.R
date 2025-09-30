#!/usr/bin/env Rscript

suppressMessages({
    Sys.unsetenv("RETICULATE_PYTHON")
    library(reticulate)
    library(polars)
})

## get the nanoarrow external pointer to the struct aka RecordBatch
narb <- acane::create_struct_example()
pdf <- polars::as_polars_df(narb)


viaR <- function() {
    pdf <- polars::as_polars_df(narb)
    print(pdf)

    ## larger example
    pdf <- polars::as_polars_df( nanoarrow::as_nanoarrow_array( palmerpenguins::penguins ) )
    print(pdf)

    invisible(NULL)
}

## local helper to unpack pyCapsule
capsule_prep <- function(na) {
    ## pointer addresses for array and schema in 'long long int' format
    arrchr <- nanoarrow::nanoarrow_pointer_addr_chr(na)
    schchr <- nanoarrow::nanoarrow_pointer_addr_chr(nanoarrow::infer_nanoarrow_schema(na))

    ## Import with convert = FALSE so that `_import_from_c` returns a Python object
    py <- reticulate::import_builtins(convert = FALSE)
    pa <- reticulate::import("pyarrow", convert = FALSE)
    out <- pa$Array$`_import_from_c`(py$int(reticulate::r_to_py(arrchr)),
                                     py$int(reticulate::r_to_py(schchr)))
    out
}

viaPython <- function() {
    #py_install("pyarrow")

    py_require(c("polars", "pandas", "pyarrow"))

    ## now import Polars
    po <- import("polars")
    pa <- import("pyarrow")

    ## local helper function until `nanoarrow` catches up
    na2 <- capsule_prep(narb)

    ## use it to import the Table into a Polars DataFrame
    df <- po$from_dataframe(na2)
    print(df)

    ## create a record batch
    df <- pa$record_batch(na2)
    ## which we can turn into a table
    ## which can be exported to pandas
    print(pa$table(df)$to_pandas())

    invisible(NULL)
}

#print(narb)

#viaR()
viaPython()
