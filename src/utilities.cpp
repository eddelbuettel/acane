
#include <Rcpp/Lighter>
#include <nanoarrow/r.h>        	// file exported by nanoarrow package
#include <nanoarrow/nanoarrow.hpp>  // file vendored here in nanoarrow/nanoarrow.hpp

// Forward declarations
void array_xptr_set_schema(SEXP array_xptr, SEXP schema_xptr);
inline void exitIfError(const ArrowErrorCode ec, const std::string& msg);
void show_array(const struct ArrowArray* arr);
void show_schema(const struct ArrowSchema* sch);


//' @title create_struct_example
//' @description Create a Simple \sQuote{struct} Object
//' @details A two-column \sQuote{struct} is created and returned, containing respectively an
//' integer and a floating point column each with one missing value
//' @return A external pointer object is returned containing a pointer to the Arrow array with
//' a pointer to the schema tucked away in the \sQuote{tag} component as is common for
//' \pkg{nanoarrow} objects
//' @examples
//' s <- create_struct_example()
//' s                 # pretty-printed in full details thanks to 'nanoarrow' S3 methods
//' as.data.frame(s)  # converted thanks to 'nanoarrow' S3 methods
// [[Rcpp::export]]
Rcpp::XPtr<ArrowArray> create_struct_example() {
    // First init a schema
    auto sxpsch = nanoarrow_schema_owning_xptr();      		// allocates and wraps as XP for export
    auto sch = nanoarrow_output_schema_from_xptr(sxpsch); 	// underlying arrow schema object

    // We want a 'struct' type, here with two columns
    const int64_t nc = 2;
    exitIfError(ArrowSchemaInitFromType(sch, NANOARROW_TYPE_STRUCT), "Bad schema init");
    exitIfError(ArrowSchemaSetName(sch, ""), "Bad schema name");
    exitIfError(ArrowSchemaAllocateChildren(sch, nc), "Bad schema children alloc");

    // Second create array
    auto sxparr = nanoarrow_array_owning_xptr(); 			// allocates and wraps as XP for export
    auto arr = nanoarrow_output_array_from_xptr(sxparr);    // underlying arrow array object
    exitIfError(ArrowArrayInitFromType(arr, NANOARROW_TYPE_STRUCT), "Bad array init");
    exitIfError(ArrowArrayAllocateChildren(arr, nc), "Bad array children alloc");

    // Now the two children, and we opt for length four
    arr->length = 4;
    exitIfError(ArrowSchemaInitFromType(sch->children[0], NANOARROW_TYPE_INT32), "Bad schema c0 init");
    exitIfError(ArrowSchemaInitFromType(sch->children[1], NANOARROW_TYPE_DOUBLE), "Bad schema c1 init");
    exitIfError(ArrowArrayInitFromType(arr->children[0], NANOARROW_TYPE_INT32), "Bad array c0 init");
    exitIfError(ArrowArrayInitFromType(arr->children[1], NANOARROW_TYPE_DOUBLE), "Bad array c0 init");

    exitIfError(ArrowSchemaSetName(sch->children[0], "fred"), "Bad c0 name");
    exitIfError(ArrowArrayStartAppending(arr->children[0]), "Bad c0 append start");
    exitIfError(ArrowArrayAppendInt(arr->children[0], 100), "Bad c0 append 1");
    exitIfError(ArrowArrayAppendInt(arr->children[0], 101), "Bad c0 append 2");
    exitIfError(ArrowArrayAppendNull(arr->children[0], 1), "Bad c0 append null");
    exitIfError(ArrowArrayAppendInt(arr->children[0], 102), "Bad c0 append 3");
    exitIfError(ArrowArrayFinishBuildingDefault(arr->children[0], nullptr), "Bad c0 finalize");

    exitIfError(ArrowSchemaSetName(sch->children[1], "ginger"), "Bad c1 name");
    exitIfError(ArrowArrayStartAppending(arr->children[1]), "Bad c1 append start");
    exitIfError(ArrowArrayAppendDouble(arr->children[1], 1.0), "Bad c1 append 1");
    exitIfError(ArrowArrayAppendNull(arr->children[1], 1), "Bad c1 append null");
    exitIfError(ArrowArrayAppendDouble(arr->children[1], 1.1), "Bad c1 append 2");
    exitIfError(ArrowArrayAppendDouble(arr->children[1], 1.2), "Bad c1 append 3");
    exitIfError(ArrowArrayFinishBuildingDefault(arr->children[1], nullptr), "Bad c0 finalize");

    exitIfError(ArrowArrayFinishBuildingDefault(arr, nullptr), "Bad finalize");


    // Nanoarrow special: stick schema into xptr tag to return single SEXP
    array_xptr_set_schema(sxparr, sxpsch); 					// embed schema in array

    return Rcpp::XPtr<ArrowArray>(sxparr);   				// return array with emb. schema
}

//' @title Display Arrow Structures Array or Schema
//' @description Helper functions showing elements of \sQuote{Arrow Array} or
//' \sQuote{Arrow Schema} objects provided via an external pointer
//' @param sxparr An external pointer to an \sQuote{Arrow Array} also containing
//' a pointer to the corresponding \sQuote{Arrow Schema} in its \sQuote{tag} slot
//' @return Nothing is returned, the functions are invoked for the side effect
//' of printing
// [[Rcpp::export]]
void show_array(SEXP sxparr) {
    auto arr = nanoarrow_array_from_xptr(sxparr);    // underlying arrow array object
    show_array(arr);
}

//' @rdname show_array
// [[Rcpp::export]]
void show_schema(SEXP sxparr) {
    auto sxpsch = R_ExternalPtrTag(sxparr); 		// schema xptr tucked in with array
    auto sch = nanoarrow_schema_from_xptr(sxpsch); 	// underlying arrow schema object
    show_schema(sch);
}


// Helpers below

// Attaches a schema to an array external pointer. The nanoarrow R package
// attempts to do this whenever possible to avoid misinterpreting arrays.
void array_xptr_set_schema(SEXP array_xptr, SEXP schema_xptr) {
    R_SetExternalPtrTag(array_xptr, schema_xptr);
}

// Simple wrapper alternative to the default macro used by nanoarrow
inline void exitIfError(const ArrowErrorCode ec, const std::string& msg) {
    if (ec != NANOARROW_OK) Rcpp::stop(msg);
}

void show_array(const struct ArrowArray* arr) {
    Rcpp::Rcout << "Array" << std::endl
                << "  Length: " << arr->length << std::endl
                << "  Null_count: " << arr->null_count << std::endl
                << "  Offset: " << arr->offset << std::endl
                << "  N_Buffers: " << arr->n_buffers << std::endl
                << "  N_Children: " << arr->n_children << std::endl
                << "  Dictionary: " << (arr->dictionary == nullptr ? "no" : "yes") << std::endl;
}

void show_schema(const struct ArrowSchema* sch) {
    Rcpp::Rcout << "Schema" << std::endl
                << "  Format: " << sch->format << std::endl
                << "  Name: " << sch->name << std::endl
                << "  Flags: " << sch->flags << std::endl
                << "  N_Children: " << sch->n_children << std::endl;
}
