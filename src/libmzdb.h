#ifndef _LIBMZDB_H_
#define _LIBMZDB_H_

//library needed
#include "sqlite3.h"
#include "parser.h"


#define LIBMZDB_ACQUISITION_MODE_DDA "Data Dependant Acquisition (Thermo designation), Warning: in ABI this is called IDA (Information Dependant Acquisition)"
#define LIBMZDB_ACQUISITION_MODE_SWATH "ABI Swath acquisition or Thermo swath acquisition"
#define LIBMZDB_ACQUISITION_MODE_MRM "Multiple reaction monitoring"
#define LIBMZDB_ACQUISITION_MODE_SRM "SRM (Single reaction monitoring) acquisition"
#define LIBMZDB_ACQUISITION_MODE_UNKNOWN "unknown acquisition mode"

//???
#define LIBMZDB_PSI_MS_32_BIT_FLOAT "*0521"
#define LIBMZDB_PSI_MS_64_BIT_FLOAT "*0523"
#define LIBMZDB_ACQUISITION_PARAMETER "*1954"
#define LIBMZDB_ISOLATION_WINDOW_TARGET_MZ "MS:*0827"
#define LIBMZDB_ISOLATION_WINDOW_LOWER_OFFSET "MS:*0828"
#define LIBMZDB_ISOLATION_WINDOW_UPPER_OFFSET "MS:*0829"
#define SELECTED_ION_MZ "MS:*0744"


//TODO

//the acquisition mode
typedef enum libmzdb_acquisition_mode { DDA, SWATH, MRM, SRM, UNKNOW } libmzdb_acquisition_mode_enum;
//an array of each acquisition mode decription, match with the enumeration
static char* _libmzdb_acquisition_mode_desc[5] =
{
	LIBMZDB_ACQUISITION_MODE_DDA,
	LIBMZDB_ACQUISITION_MODE_SWATH,
	LIBMZDB_ACQUISITION_MODE_MRM,
	LIBMZDB_ACQUISITION_MODE_SRM,
	LIBMZDB_ACQUISITION_MODE_UNKNOWN
};
#define LIBMZDB_ACQUISITION_MODE_TO_STRING(_index_) _libmzdb_acquisition_mode_desc[_index_];

//custom data type
typedef unsigned char byte;
//typedef float float32_t;
//typedef double float64_t;

typedef enum libmzdb_data_precision //use to know which precision is use
{
	DATA_PRECISION_UNKNOWN = 0,
	DATA_PRECISION_64_64 = 1,
	DATA_PRECISION_64_32 = 2,
	DATA_PRECISION_32_32 = 3,
	DATA_PRECISION_FITTED_64_32 = 4,
} libmzdb_data_precision_enum;

//an array of each data precision decription, match with the enumeration
static char * _libmzdb_data_precision_str[5] =
{
	"DATA_PRECISION_UNKNOWN",
	"DATA_PRECISION_64_64",
	"DATA_PRECISION_64_32",
	"DATA_PRECISION_32_32",
	"DATA_PRECISION_FITTED_64_32"
};
#define LIBMZDB_DATA_PRECESION_TO_STRING(_index_) _libmzdb_data_precision_str[_index_];

/*typedef struct data_point
{
float64_t x;
float32_t y;
} data_point_t;*/


typedef struct libmzdb_data_point_32_32
{
	float x;
	float y;
} libmzdb_data_point_32_32_t;

typedef struct libmzdb_data_point_64_32
{
	double x;
	float y;
} libmzdb_data_point_64_32_t;


typedef struct libmzdb_data_point_64_64
{
	double x;
	double y;
} libmzdb_data_point_64_64_t;

typedef struct libmzdb_fitted_peak
{
	double x;
	float y;
	float left_hwhm;
	float right_hwhm;
} libmzdb_fitted_peak_t;

//param_tree.h
typedef struct libmzdb_cv_param
{
	char* cv_ref;
	char* accession;
	char* name;
	char* value;
	char* unit_cv_ref;
	char* unit_accession;
	char* unit_name;
} libmzdb_cv_param_t;

typedef struct libmzdb_user_param
{
	char* cv_ref;
	char* accession;
	char* name;
	char* value;
	char* type;
}libmzdb_user_param_t;

typedef struct libmzdb_user_text
{
	char* cv_ref;
	char* accession;
	char* name;
	char* text;
	char* type;
} libmzdb_user_text_t;

typedef struct libmzdb_param_tree
{
	libmzdb_cv_param_t* cv_params;
	libmzdb_user_param_t* user_params;
	libmzdb_user_text_t* user_text;
}libmzdb_param_tree_t;

typedef struct libmzdb_mzdb_param_tree
{
	float ms1_bb_mz_width;
	float msn_bb_mz_width;
	float ms1_bb_time_width;
	float msn_bb_time_width;
	int is_loss_less;
	char* origin_file_format;
}libmzdb_mzdb_param_tree_t;

//data_encoding.h

typedef enum libmzdb_data_mode { PROFILE = -1, CENTROID = 12, FITTED = 20 } libmzdb_data_mode_t;
typedef enum libmzdb_peak_encoding { LOW_RES_PEAK = 8, HIGH_RES_PEAK = 12, NO_LOSS_PEAK = 16 } libmzdb_peak_encoding_t;
typedef enum libmzdb_byte_order { BIG_ENDIAN, LITTLE_ENDIAN } libmzdb_byte_order_t;

typedef struct libmzdb_data_encoding
{
	int id;
	libmzdb_data_mode_t mode;
	libmzdb_peak_encoding_t peak_encoding;
	char* compression;
	libmzdb_byte_order_t byte_order;
	//int peak_struct_size;
} libmzdb_data_encoding_t;

typedef struct libmzdb_data_encodings_cache
{
	libmzdb_data_encoding_t* data_encodings; // length = number of data_encodings
	int de_size;
	int* data_encoding_id_mapping; //[ id => idx] ; length of MAX(id) + 1
	int mapping_size;
	int* spectrum_id_to_data_encoding_id; //[ spectrum_id => data_encoding_id] ; length of MAX(spectrum_id) + 1
	int spectrum_count;
} libmzdb_data_encodings_cache_t;

//spectrum.h
/*typedef struct peak
{
double mz;
float intensity;
float left_hwhm;
float right_hwhm;
} peak_t;*/


// pragma usage (specific to MS Compiler):
// - http://www.cplusplus.com/forum/general/14659/
// - https://stackoverflow.com/questions/3318410/pragma-pack-effect
// - https://msdn.microsoft.com/en-us/library/2e70t5y1.aspx

// Exact fit - no padding
#pragma pack(push)
#pragma pack(1)

typedef struct libmzdb_spectrum_peaks
{
	libmzdb_data_precision_enum data_precision;
	int peak_count;
	union {
		libmzdb_data_point_64_64_t* peaks64_64;
		libmzdb_data_point_64_32_t* peaks64_32;
		libmzdb_data_point_32_32_t* peaks32_32;
		libmzdb_fitted_peak_t* fitted_peaks;
		//peak_t* peaks; // upt to peak* -- ???
	};
} libmzdb_spectrum_peaks_t;

//back to whatever the previous packing mode was
#pragma pack(pop)

typedef struct libmzdb_data_points_32_32
{
	float* x_list;
	float* y_list;
} libmzdb_data_points_32_32_t;

typedef struct libmzdb_data_points_64_32
{
	double* x_list;
	float*  y_list;
} libmzdb_data_points_64_32_t;

typedef struct libmzdb_data_points_64_64
{
	double* x_list;
	double* y_list;
} libmzdb_data_points_64_64_t;


// TODO: create macro for is_spectrum_data_fitted
/*int is_spectrum_data_fitted(spectrum_data sd) {
return sd != NULL;
}*/

typedef struct libmzdb_spectrum_data
{
	libmzdb_data_encoding_t data_encoding;
	int peak_count;
	union {
		double* mz_array_as_doubles;
		float* mz_array_as_floats;
	};
	union {
		double* intensity_array_as_doubles;
		float* intensity_array_as_floats;
	};
	float* lwhm_array; // warning: can be NULL
	float* rwhm_array; // warning: can be NULL
} libmzdb_spectrum_data_t;

typedef struct libmzdb_spectrum_header
{
	int id;
	int initial_id;
	char* title;
	int cycle;
	float time;
	int ms_level;
	char* activation_type;
	float tic;
	double base_peak_mz;
	float base_peak_intensity;
	double precursor_mz;
	int precursor_charge;
	int peaks_count;
	char* param_tree_str;
	char* scan_list_str; //xml reference
	char* precursor_list_str; //xml reference
	char* product_list_str; //xml reference
	int shared_param_tree_id;
	int instrument_configuration_id;
	int source_file_id;
	int run_id;
	int data_processing_id;
	int data_encoding_id;
	int bb_first_spectrum_id;
	int is_high_resolution;
} libmzdb_spectrum_header_t;


typedef struct libmzdb_spectrum
{
	libmzdb_spectrum_header_t header;
	libmzdb_spectrum_data_t data;
} libmzdb_spectrum_t;


//run_slice.h
typedef struct libmzdb_spectrum_slice
{
	libmzdb_spectrum_t spectrum;
	int run_slice_id;
} libmzdb_spectrum_slice_t;

typedef struct libmzdb_run_slice_header
{
	int id;
	int ms_level;
	int number;
	double begin_mz;
	double end_mz;
	int run_id;
} libmzdb_run_slice_header_t;

typedef struct libmzdb_run_slice_data
{
	int id;
	libmzdb_spectrum_slice_t * spectrum_slice;
} libmzdb_run_slice_data_t;

typedef struct libmzdb_run_slice
{
	libmzdb_run_slice_header_t header;
	libmzdb_run_slice_data_t data;
} libmzdb_run_slice_t;


//bbox.h

typedef struct libmzdb_bbox_sizes
{
	double BB_MZ_HEIGHT_MS1;
	double BB_MZ_HEIGHT_MSn;
	float BB_RT_WIDTH_MS1;
	float BB_RT_WIDTH_MSn;
} libmzdb_bbox_sizes_t;

typedef struct libmzdb_bounding_box
{
	int id;
	int first_spectrum_id;
	int last_spectrum_id;
	int run_slice_id;
	//int ms_level;
	byte* blob;
	int blob_size;
} libmzdb_bounding_box_t;

typedef struct libmzdb_indexed_bounding_box
{
	libmzdb_bounding_box_t bb;

	//spectrum_slice_t* spectrum_slices;

	int spectrum_slices_count; // number of spectrum slices in the blob
	int* spectra_ids; // list of spectra ids in the blob
	int* slices_indexes; // list of spectrum slice starting positions in the blob
	int* peaks_counts; // number of peaks in each spectrum slice of the blob
} libmzdb_indexed_bounding_box_t;

//chromatogram.h
typedef libmzdb_data_point_64_32_t* libmzdb_chromatogram_t;

//xic_method.h
typedef enum libmzdb_xic_method { MAX = 0, NEAREST = 1, SUM = 2 } libmzdb_xic_method_enum;

typedef struct libmzdb_isolation_window
{
	double min_mz;
	double max_mz;
} libmzdb_isolation_window_t;

typedef struct libmzdb_entity_cache {
	libmzdb_data_encodings_cache_t data_encodings_cache;
	libmzdb_spectrum_header_t* spectrum_headers;
	int spectrum_header_count;
} libmzdb_entity_cache_t;


typedef struct nodec * nodec_ptr_t;
typedef struct attc * attc_ptr_t;


/**
* @brief xml_browse_rec recursive xml traversal with callback on all nodes
* @param node the node
* @param level the current level
* @param user_param the user param
*/
void xml_browse_rec(nodec_ptr_t node, int level, void(*callback) (nodec_ptr_t node, int level, void *), void* user_param);

/**
* @brief xml_browse xml traversal with callback on all nodes
* @param root the tree root
* @param user_param the user param
*/
void xml_browse(nodec_ptr_t root, void(*callback) (nodec_ptr_t node, int level, void *), void* user_param);

/**
* @brief get_attribute_value_rec get the attribute value matching attributeName
* @param attribute the attribute
* @param attribute_name the attribute name
* @param attribute_value the attribute value
* @return the attribute value
*/
int get_attribute_value_rec(attc_ptr_t attribute, char* attribute_name, char attribute_value[2048]);

/**
* @brief get_attribute_value get the attribute value matching attributeName.
* @param node the node
* @param attribute_name the attribute name
* @return Static char returned: Do not need to free memory
*/
char* get_attribute_value(nodec_ptr_t node, char* attribute_name);

/**
* @brief get_attribute_value_as_float get the attribute value matching attributeName as float
* @param node the node
* @param attribute_name the attribute name
* @param result the float result
* @return 0 if no error
*/
int get_attribute_value_as_float(nodec_ptr_t node, char* attribute_name, float* result);

/**
* @brief get_attribute_value_as_int  get the attribute value matching attributeName as int
* @param node the node
* @param attribute_name the attribute name
* @param result the integer result
* @return 0 if no error
*/
int get_attribute_value_as_int(nodec_ptr_t node, char* attribute_name, int* result);

/**
* @brief _add_brothers_rec fill the children array
* @param children the children array
* @param node the node
* @param children_count the number of child
*/
void _add_brothers_rec(nodec_ptr_t** children, nodec_ptr_t node, int children_count);

/**
* @brief get_children  get an array containing all children
* @param node the node
* @return an array of child (allocate array memory to be freed)
*/
nodec_ptr_t* get_children(nodec_ptr_t node);

/**
* @brief _add_attributes_brother_rec fill the attributes array
* @param attributes the attributes array
* @param attr the attribute
* @param attribute_count the number of attribute
*/
void _add_attributes_brother_rec(attc_ptr_t** attributes, attc_ptr_t attr, int attribute_count);

/**
* @brief get_attribute get an array containing all attributes
* @param node the node
* @return the attribute array (allocate array memory to be freed)
*/
attc_ptr_t* get_attribute(nodec_ptr_t node);

/**
* @brief fprintf_attribute print an attribute
* @param f the file
* @param attr the attribute
* @param level the level
*/
void fprintf_attribute(FILE* f, attc_ptr_t attr, int level);

/**
* @brief fprintf_node_rec print each node
* @param f the file
* @param node the root
* @param level the level
*/
void fprintf_node_rec(FILE *f, nodec_ptr_t node, int level);

/**
* @brief fprintf_node print each node
* @param f the file
* @param node the node
*/
void fprintf_node(FILE* f, nodec_ptr_t node);

/**
* @brief print_xml_bare print the xml bare
* @param text the text
*/
void print_xml_bare(char* text);

/**
TODO: change it
* @brief xml_parse parse a xml
* @param text the text
* @return the xml root
*/
int xml_parse(char* text, nodec_ptr_t * rootnode);

/**
* @brief xml_browse_callback_mazdb_param_tree browse a xml param tree
* @param node the node
* @param user_param the user param
*/
void xml_browse_callback_mzdb_param_tree(nodec_ptr_t node, int level, void* user_param);


/**
* @brief execute_prepared_stmt execute for each column the callback
* @param db the DB
* @param stmt the stmt
* @param index the index of the column
* @param size_in_bytes the size
* @param result the result
* @param err_msg the error message
* @return SQLITE_OK if there is no error
*/
int libmzdb_execute_prepared_stmt(sqlite3 *db, sqlite3_stmt *stmt, int index, void(*callback)(sqlite3_stmt*, int, long, void*), long size_in_bytes, void *result, char **err_msg);

/**
* @brief execute_select_n_stmt
* @param db
* @param sql
* @param callback
* @param index
* @param size_in_bytes
* @param result
* @param err_msg
* @return
*/
int libmzdb_execute_select_stmt_for_all_types(sqlite3 *db, const char *sql, int index, void(*callback)(sqlite3_stmt*, int, long, void*), long size_in_bytes, void** result, char** err_msg);

/**
* @brief execute_select_stmt execute the sql query and get the result of the index column
* @param db the DB
* @param sql the query
* @param index the index of the column
* @param result the result
* @param err_msg the error message
* @return SQLITE_OK if there is no error
*/
int libmzdb_execute_select_stmt(sqlite3 *db, const char *sql, int index, void(*callback)(sqlite3_stmt*, int, long, void*), void**result, char**err_msg);

/**
* @brief execute_select_first_field_stmt execute the sql and get the result of the 1st column
* @param db the DB
* @param sql the query
* @param result the result
* @param err_msg the error message
* @return SQLITE_OK if there is no error
*/
int libmzdb_execute_select_first_field_stmt(sqlite3* db, const char* sql, void(*callback)(sqlite3_stmt*, int, long, void*), void*result, char** err_msg);

/*************** CALLBACKS ******************/

/**
* @brief get_string_cb get the str at the "index" column
* @param stmt the stmt
* @param index the index of the column
* @param result the result
*/
void libmzdb_get_string_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result); //size in bytes unused

																							 /**
																							 * @brief get_double_cb get the double at the "index" column
																							 * @param stmt the stmt
																							 * @param index the index of the column
																							 * @param result the result
																							 */
void libmzdb_get_double_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);

/**
* @brief get_float_cb get the float at the "index" column
* @param stmt the stmt
* @param index the column index
* @param result the result
*/
void libmzdb_get_float_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);

/**
* @brief get_int_cb get the int at the "index" column
* @param stmt the stmt
* @param index the index of the column
* @param result the result
*/
void libmzdb_get_int_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);

/**
* @brief get_n_int_cb get an array of int of each column in [0, index]
* @param stmt the stmt
* @param index the index of the last column in the range
* @param result the result
*/
void libmzdb_get_n_int_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);

typedef struct libmzdb_blob_result {
	unsigned char *blob;
	long blob_size;
} libmzdb_blob_result_t;

/**
* @brief get_blob_cb get the blob at the "index" column
* @param stmt the stmt
* @param index the column index
* @param size_in_bytes the blob size
* @param result the result
*/
void libmzdb_get_blob_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);


/**
* @brief get_int_64_cb get the int64 at the "index" column
* @param stmt the stmt
* @param index the column index
* @param result the result
*/
static void libmzdb_get_int_64_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);

/*************** UTILS ******************/

/**
* @brief die_on_sql_error exit the program if rc != SQLITE_OK
* @param rc the sql result state
* @param err_msg the error message to print
*/
void libmzdb_die_on_sqlite_error(int rc, char** err_msg);


void libmzdb_dump_blob_content(unsigned char* blob, int blob_size);



/**
* @brief open_mzdb_file open a mzdb file
* @param filename the filename
* @param pp_db the DB
* @return SQLITE_OK if no arror occured
*/
int libmzdb_open_mzdb_file(const char* filename, sqlite3** pp_db);


/**
* @brief close_mzbd_file close a mzbd file
* @param pp_db the pointer of the DB
* @return SQLITE_OK if no arror occured
*/
int libmzdb_close_mzdb_file(sqlite3* pp_db);



/**
* @brief create_spectrum_header get a headers from the DB
* @param stmt result of the query
* @param spectrum_header the header to get
* @param err_msg the error message if an error occured
* @return SQLITE_OK if no error occured
*/
int libmzdb_create_spectrum_header(sqlite3_stmt* stmt, libmzdb_spectrum_header_t* libmzdb_spectrum_header, char** err_msg);


/**
* @brief get_spectrum_headers get each spectrum from a DB
* @param db the DB where the headers are stocked
* @param spectrum_headers an array of each spectrum headers
* @param err_msg the error message if an error occured
* @return SQLITE_OK if no error occured
*/
int libmzdb_get_spectrum_headers(sqlite3* db, libmzdb_spectrum_header_t** spectrum_headers, int *spectrum_header_count, char** err_msg);

/**
* @brief create_entity_cache create the entity cache, use to stock data pre computed
* @param db the data base
* @param libmzdb_entity_cache the result
* @param err_msg the error message
* @return SQLITE_OK if no error occured
*/
int libmzdb_create_entity_cache(sqlite3 *db, libmzdb_entity_cache_t** libmzdb_entity_cache, char **err_msg);



// this pragma is needed to pack all data structure without 8bits alignment
#pragma pack(push)
#pragma pack(1)

#define NO_ERR_MSG ""


/**
* @brief get_model_version return the whole statement to find the model Version
* @param db the database on which the SQL executes
* @param model_version the model version retrieved from the mzDB
* @param err_msg write error messages here
* @return SQLITE_OK if no error
*/
int libmzdb_get_model_version(sqlite3* db, char** model_version, char** err_msg);

/**
* @brief get_model_version_or_die only return the model Version from the previous statement
* @param db the database on which the SQL executes
* @return the model version
*/
char* libmzdb_get_model_version_or_die(sqlite3* db);

/**
* @brief get_param_tree_chromatogram stock the Param Tree values from Chromatogram into param_tree_chro
* @param db the database on which the SQL executes
* @param param_tree_chro the param tree of the chromatogram
* @param err_msg  write the error messages here
* @return SQLITE_OK if no error occured
*/
int libmzdb_get_param_tree_chromatogram(sqlite3* db, char** param_tree_chro, char** err_msg);
/**
* @brief get_param_tree_chromatogram_or_die only return the paramTreeChro from the previous statement
* @param db the database on which the SQL executes
* @return the param tree
*/
char* libmzdb_get_param_tree_chromatogram_or_die(sqlite3* db);

/**
* @brief get_param_tree_spectrum stock the Param Tree values from Spectrum into paramTreeSpectrum
* @param db the database on which the SQL executes
* @param param_tree_id the param tree id
* @param param_tree_spectrum the param tree spectrum
* @param err_msg err_msg  write error messages here
* @return SQLITE_OK if no error
*/
int libmzdb_get_param_tree_spectrum(sqlite3* db, int param_tree_id, char** param_tree_spectrum, char**err_msg);

/**
* @brief get_param_tree_spectrum_or_die only return the get param tree spectrum from the previous statement
* @param db the database on which the SQL executes
* @return the spectrum param tree
*/
char* libmzdb_get_param_tree_spectrum_or_die(sqlite3* db, int param_tree_id);

/**
* @brief get_param_tree_mzdb stock the Param Tree values from Mzdb into param_tree_mzdb
* @param db the database on which the SQL executes
* @param param_tree_mzdb the param tree
* @param err_msg write error messages here
* @return SQLITE_OK if no error
*/
int libmzdb_get_param_tree_mzdb(sqlite3* db, libmzdb_mzdb_param_tree_t* param_tree_mzdb, char** err_msg);

/**
* @brief getParamTreeMzdb_or_die only return the paramTreeMzdb values from the previous statement
* @param db the database on which the SQL executes
* @return the param tree MZDB
*/
libmzdb_mzdb_param_tree_t libmzdb_get_param_tree_mzdb_or_die(sqlite3 *db);

/**
* @brief get_pwiz_mzdb_version return the whole statement to find the Pwiz mzdb version
* @param db the database on which the SQL executes
* @param mzdb_version the version
* @param err_msg write error messages here
* @return SQLITE_OK if no error
*/
int libmzdb_get_pwiz_mzdb_version(sqlite3* db, char** mzdb_version, char** err_msg);

/**
* @brief get_pwiz_mzdb_version_or_die only return the PwizMzDbVersion from the previous statement
* @param db the database on which the SQL executes
* @return the version
*/
char* libmzdb_get_pwiz_mzdb_version_or_die(sqlite3 *db);

/**
* @brief get_last_time return the whole statement to find the last Time from spectrum table
* @param db the database on which the SQL executes
* @param last_time the last time
* @param err_msg err_msg write error messages here
* @return SQLITE_OK if no error
*/
int libmzdb_get_last_time(sqlite3* db, double* last_time, char** err_msg);

/**
* @brief get_last_time_or_die only return the lastTime from the previous statement
* @param db the database on which the SQL executes
* @return the last time
*/
double libmzdb_get_last_time_or_die(sqlite3* db);

/**
* @brief get_max_ms_level return the whole statement to find the max_ms_level from run_slice table
* @param db the database on which the SQL executes
* @param  the max ms level
* @param err_msg write error messages here
* @return SQLITE_OK if no error
*/
int libmzdb_get_max_ms_level(sqlite3* db, int*, char** err_msg);

/**
* @brief get_max_ms_level_or_die only return the max_ms_level from the previous statement
* @param db the database on which the SQL executes
* @return the max ms level
*/
int libmzdb_get_max_ms_level_or_die(sqlite3 *db);



//Uses the method get_table_records_count to specifically return the Table Records Count from the bounding_box table
/**
* @brief get_bounding_boxes_count_from_sequence uses the method get_table_records_count to specifically return the Table Records Count from the bounding_box table
* @param db the database on which the SQL executes
* @param bb_count the bounding boxe count
* @param err_msg err_msg write error messages here
* @return SQLITE_OK if no error
*/
int libmzdb_get_bounding_boxes_count_from_sequence(sqlite3 *db, int *bb_count, char **err_msg);

/**
* @brief get_bounding_boxes_count_from_sequence_or_die only return the Bounding Boxes count from the previous statement
* @param db the database on which the SQL executes
* @return the bounding box count
*/
int libmzdb_get_bounding_boxes_count_from_sequence_or_die(sqlite3 *db);


//Return the Mz Range depending on the ms_level
/**
* @brief get_mz_range hte mz range depending on the ms level
* @param db the database on which the SQL executes
* @param ms_level the ms level
* @param min_max_mz the min
* @param err_msg err_msg write error messages here
* @return SQLITE_OK if no error
*/
int libmzdb_get_mz_range(sqlite3 *db, int ms_level, int **min_max_mz, char **err_msg);

//FIXME: check that we return a usable array
/**
* @brief get_mz_range_or_die
* @param db the database on which the SQL executes
* @param ms_level the ms level
* @return the mz_range
*/
int *libmzdb_get_mz_range_or_die(sqlite3 *db, int ms_level);

/**
* @brief get_bounding_boxes_count return the whole statement to find the BoundingBoxesCount from bounding_box table where the id is given by the user
* @param db the database on which the SQL executes
* @param bb_run_slice_id hte bb run slice id
* @param bb_count the result
* @param err_msg err_msg write error messages here
* @return SQLITE_OK if no error
*/
int libmzdb_get_bounding_boxes_count(sqlite3 *db, int bb_run_slice_id, int *bb_count, char **err_msg);

/**
* @brief get_bounding_boxes_count_or_die Only return the bb_count from the previous statement
* @param db the database on which the SQL executes
* @return the bounding boxes count
*/
int libmzdb_get_bounding_boxes_count_or_die(sqlite3 *db, int bb_run_slice_id);

/**
* @brief get_cycles_count Return the whole statement to find the Cycles Count from spectrum table
* @param db the database on which the SQL executes
* @param cycles_count
* @param err_msg
* @return SQLITE_OK if no error
*/
int libmzdb_get_cycles_count(sqlite3 *db, int *cycles_count, char **err_msg);

/**
* @brief get_cycles_count_or_die Only return the cycles_count from the previous statement
* @param db the database on which the SQL executes
* @return
*/
int libmzdb_get_cycles_count_or_die(sqlite3 *db);

/**
* @brief get_data_encodings_count_from_sequence Uses the method get_table_records_count to specifically return the Data Encodings Count from the data_encoding table
* @param db the database on which the SQL executes
* @param count
* @param err_msg
* @return
*/
int libmzdb_get_data_encodings_count_from_sequence(sqlite3 *db, int *count, char **err_msg);

/**
* @brief get_data_encodings_count_from_sequence_or_die Only return the Data Encodings count from the previous statement
* @param db the database on which the SQL executes
* @return
*/
int libmzdb_get_data_encodings_count_from_sequence_or_die(sqlite3 *db);

/**
* @brief get_spectra_count_from_sequence Uses the method get_table_records_count to specifically return the Spectra Count from the spectrum table
* @param db the database on which the SQL executes
* @param spectra_count
* @param err_msg
* @return
*/
int libmzdb_get_spectra_count_from_sequence(sqlite3 *db, int *spectra_count, char **err_msg);

/**
* @brief get_spectra_count_from_sequence_or_die Only return the Spectra count from the previous statement
* @param db the database on which the SQL executes
* @return
*/
int libmzdb_get_spectra_count_from_sequence_or_die(sqlite3 *db);

/**
* @brief get_spectra_count Return the whole statement to find the Spectra Count from spectrum table where the ms_level is given by the user
* @param db the database on which the SQL executes
* @param ms_level
* @param spectra_count
* @param err_msg
* @return
*/
int libmzdb_get_spectra_count(sqlite3 *db, int ms_level, int *spectra_count, char **err_msg);

/**
* @brief get_spectra_count_or_die Only return the spectra_count from the previous statement
* @param db the database on which the SQL executes
* @param ms_level
* @return
*/
int libmzdb_get_spectra_count_or_die(sqlite3 *db, int ms_level);


/**
* @brief get_run_slices_count_from_sequence Uses the method get_table_records_count to specifically return the Run Slices Count from the run_slice table
* @param db the database on which the SQL executes
* @param run_slices_count
* @param err_msg
* @return
*/
int libmzdb_get_run_slices_count_from_sequence(sqlite3 *db, int *run_slices_count, char **err_msg);

/**
* @brief get_run_slices_count_from_sequence_or_die Only return the Run Slices count from the previous statement
* @param db the database on which the SQL executes
* @return
*/
int libmzdb_get_run_slices_count_from_sequence_or_die(sqlite3 *db);

/**
* @brief get_table_records_count Return the sequence from a specific table where the user gives the table name
* @param db the database on which the SQL executes
* @param table_name
* @param table_records_count
* @param err_msg
* @return
*/
int libmzdb_get_table_records_count(sqlite3 *db, char *table_name, int *table_records_count, char **err_msg);


/**
* @brief get_table_records_count_or_die Only return the table record count from the previous statement
* @param db the database on which the SQL executes
* @param table_name
* @return
*/
int libmzdb_get_table_records_count_or_die(sqlite3 *db, char *table_name);

/**
* @brief get_bounding_box_data Return blobs from the bounding box table where the bounding box id is given by the user.
* @param db the database on which the SQL executes
* @param bb_id
* @param blob
* @param blob_length
* @param err_msg
* @return
*/
int libmzdb_get_bounding_box_data(sqlite3 *db, int bb_id, byte **byte, int *blob_length, char **err_msg);

/**
* @brief get_bounding_box_data_or_die Only return the data contained in a bounding box from the previous statement
* @param db the database on which the SQL executes
* @param bb_id
* @param blob_length
* @return
*/
byte *libmzdb_get_bounding_box_data_or_die(sqlite3 *db, int bb_id, int *blob_length);

/**
* @brief get_chromatogram_data_points Gets an array of points of the chromatogram specified with c_id
* @param db the database on which the SQL executes
* @param c_id the chromatogram id
* @param data the array of data points
* @param data_points_count the length of the array
* @param err_msg the error message
* @return
*/
int libmzdb_get_chromatogram_data_points(sqlite3 *db, int c_id, libmzdb_data_points_32_32_t **data, int *data_points_count, char **err_msg);

/**
* @brief get_chromatogram_data_points_or_die
* @param db the database on which the SQL executes
* @param c_id
* @return
*/
int libmzdb_get_chromatogram_data_points_or_die(sqlite3 *db, libmzdb_data_points_32_32_t** data_points, int c_id);

/**
* @brief get_bounding_box_first_spectrum_id Return the whole statement to find the bounding box first spectrum id from spectrum table where the id is given by the user
* @param db the database on which the SQL executes
* @param first_id
* @param bb_first_spectrum_id
* @param err_msg
* @return
*/
int libmzdb_get_bounding_box_first_spectrum_id(sqlite3 *db, int first_id, long *bb_first_spectrum_id, char **err_msg);

/**
* @brief get_bounding_box_first_spectrum_id_or_die Only return the bb_first_spectrum_id from the previous statement
* @param db the database on which the SQL executes
* @param first_id
* @return
*/
long libmzdb_get_bounding_box_first_spectrum_id_or_die(sqlite3 *db, int first_id);

/**
* @brief get_bounding_box_min_mz Return the whole statement to find the bounding box minimum Mz from bounding_box_rtree table where the bounding box rtree id is given by the user
* @param db the database on which the SQL executes
* @param bb_r_tree_id
* @param bbmin_mz
* @param err_msg
* @return
*/
int libmzdb_get_bounding_box_min_mz(sqlite3 *db, int bb_r_tree_id, float *bb_min_mz, char **err_msg);

/**
* @brief get_bounding_box_min_mz_or_die Only return the bbmin_mz from the previous statement
* @param db the database on which the SQL executes
* @param bb_r_tree_id
* @return
*/
float libmzdb_get_bounding_box_min_mz_or_die(sqlite3 *db, int bb_r_tree_id);

/**
* @brief get_bounding_box_min_time Return the whole statement to find the bounding box minimum time from bounding_box_rtree table where the bounding box rtree id is given by the user
* @param db the database on which the SQL executes
* @param bb_r_tree_id
* @param bb_min_time
* @param err_msg
* @return
*/
int libmzdb_get_bounding_box_min_time(sqlite3 *db, int bb_r_tree_id, float *bb_min_time, char **err_msg);

/**
* @brief get_bounding_box_min_time_or_die Only return the bb_min_time from the previous statement
* @param db the database on which the SQL executes
* @param bb_r_tree_id
* @return
*/
float libmzdb_get_bounding_box_min_time_or_die(sqlite3 *db, int bb_r_tree_id);

/**
* @brief get_run_slice_id Return the whole statement to find the run Slice Id from bounding_box table where the id is given by the user
* @param db the database on which the SQL executes
* @param bb_id
* @param run_slice_id
* @param err_msg
* @return
*/
int libmzdb_get_run_slice_id(sqlite3 *db, int bb_id, int *run_slice_id, char **err_msg);

/**
* @brief get_run_slice_id_or_die Only return the Run Slice ID from the previous statement
* @param db the database on which the SQL executes
* @param bb_id
* @return
*/
int libmzdb_get_run_slice_id_or_die(sqlite3 *db, int bb_id);


/**
* @brief get_ms_level_from_run_slice_id_manually
* Return the whole statement to find the Ms Level from run_slice table where the run_slice_id is given by the user
* The user must type the result from "get_run_slice_id" himself to obtain the related Ms Level
* @param db the database on which the SQL executes
* @param run_slice_id
* @param ms_level
* @param err_msg
* @return
*/
int libmzdb_get_ms_level_from_run_slice_id_manually(sqlite3 *db, int run_slice_id, int *ms_level, char **err_msg);

/**
* @brief get_ms_level_from_run_slice_id_manually_or_die
* @param db the database on which the SQL executes
* @param run_slice_id
* @return
*/
int libmzdb_get_ms_level_from_run_slice_id_manually_or_die(sqlite3 *db, int run_slice_id);

/**
* @brief get_bouding_box_ms_level
* Combine last two functions in one
* Get the run slice Id from the bounding_box_id and its associated ms_level
* @param db the database on which the SQL executes
* @param bb_id
* @param result_ms_level
* @param err_msg
* @return
*/
int libmzdb_get_bounding_box_ms_level(sqlite3 *db, int bb_id, int *result_ms_level, char **err_msg);

/**
* @brief get_bouding_box_ms_level_or_die Only return the bounding box ms level from the previous statement
* @param db the database on which the SQL executes
* @param bb_id
* @return
*/
int libmzdb_get_bouding_box_ms_level_or_die(sqlite3 *db, int bb_id);

/**
* @brief get_data_encoding_id get the data_encoding id from spectrum where the bounding box ID is given by the user
* @param db the database on which the SQL executes
* @param bounding_box_id
* @param data_encoding_id
* @param err_msg
* @return
*/
int libmzdb_get_data_encoding_id(sqlite3 *db, int bounding_box_id, int *data_encoding_id, char **err_msg);

/**
* @brief get_data_encoding_id_or_die Only return the Data Encoding ID from the previous statement
* @param db the database on which the SQL executes
* @param bounding_box_id
* @return
*/
int libmzdb_get_data_encoding_id_or_die(sqlite3 *db, int bounding_box_id);

/**
* @brief get_data_precision_size_in_bytes get the size of each data Precision (used for storage allocation)
* @param data_precision
* @return
*/
int libmzdb_get_data_precision_size_in_bytes(libmzdb_data_precision_enum libmzdb_data_precision);

/**
* @brief get_spectrum_data
* @param db
* @param spectrum_id
* @param entity_cache
* @param result
* @return
*/
int libmzdb_get_spectrum_data(sqlite3* db, long spectrum_id, const libmzdb_entity_cache_t libmzdb_entity_cache, libmzdb_spectrum_data_t **result);

/**
* @brief get_data_encodings_cache
* @param db
* @param data_encodings_cache
* @return
*/
int libmzdb_get_data_encodings_cache(sqlite3 *db, libmzdb_data_encodings_cache_t** libmzdb_data_encodings_cache);

libmzdb_data_encoding_t* libmzdb_get_data_encoding_from_cache(libmzdb_data_encodings_cache_t* libmzdb_data_encodings_cache, int data_encoding_id);

/**
* @brief read_spectrum_slice_data_at
* @param indexed_bbox
* @param cache
* @param spectrum_slice_idx
* @param spectrum_id
* @param min_mz
* @param max_mz
* @return
*/
libmzdb_spectrum_data_t* libmzdb_read_spectrum_slice_data_at(
	const libmzdb_indexed_bounding_box_t indexed_bbox,
	libmzdb_data_encodings_cache_t cache,
	const int spectrum_slice_idx,
	const long spectrum_id,
	const double min_mz,
	const double max_mz
);

/**
* @brief read_spectrum_slice_data
* @param bb_bytes
* @param peaks_start_pos
* @param peaks_count
* @param de
* @param min_mz
* @param max_mz
* @return
*/
libmzdb_spectrum_data_t* libmzdb_read_spectrum_slice_data(
	const byte* bb_bytes,
	const int peaks_start_pos,
	const int peaks_count,
	const libmzdb_data_encoding_t de,
	const double min_mz,
	const double max_mz
);

/**
* @brief merge_spectrum_slices
* @param sd_slices
* @param bb_count
* @param peak_count
* @return
*/
libmzdb_spectrum_data_t libmzdb_merge_spectrum_slices(libmzdb_spectrum_data_t* sd_slices, int bb_count, int peak_count);

/**
* @brief build_bbox
* @param stmt
* @return
*/
libmzdb_bounding_box_t libmzdb_build_bbox(sqlite3_stmt* stmt, int do_malloc);

/**
* @brief index_bbox
* @param bbox
* @param de_cache
* @return
*/
libmzdb_indexed_bounding_box_t libmzdb_index_bbox(libmzdb_bounding_box_t bbox, libmzdb_data_encodings_cache_t de_cache);

/**
* @brief free_spectrum_data_content
* @param spectrum_data
*/
void libmzdb_free_spectrum_data_content(libmzdb_spectrum_data_t libmzdb_spectrum_data);

/**
* @brief free_spectrum_header_content
* @param spectrum_header
*/
void libmzdb_free_spectrum_header_content(libmzdb_spectrum_header_t libmzdb_spectrum_header);

/**
* @brief free_spectrum
* @param spectrum
*/
void libmzdb_free_spectrum(libmzdb_spectrum_t* libmzdb_spectrum);

int libmzdb_get_spectrum(sqlite3* db, long spectrum_id, libmzdb_entity_cache_t entity_cache, libmzdb_spectrum_t** result);



#define IT_ERROR -1
#define IT_DONE 0
#define IT_OK 0
#define IT_STEP 1

#define ALL_MS_LEVELS -1

#define WAIT_HAS_NEXT   0
#define WAIT_NEXT       1
#define BUFFER_MAX_SIZE 1024

///**
// * @brief memdisp Display the current memory when the function is call
// * @param msg This message will be displayed before displaying the memory
// */
//void libmzdb_memdisp(char* msg);

/**
* @brief meminit Use to init the memory (FIXME SegFault), will increase the time of process for the next queries
* @param db The database on which the SQL executes
*/
void libmzdb_meminit(sqlite3* db);



/*==============================STMT ITERATOR=======================================*/

typedef struct libmzdb_stmt_iterator
{
	sqlite3_stmt* stmt;
	int last_rc;
	int is_iterator_checked; //check if the function step has been called
}libmzdb_stmt_iterator_t;

/**
* @brief stmt_iterator_create Create the statement iterator
* @param stmt The queries
* @return The iterator
*/
libmzdb_stmt_iterator_t *libmzdb_stmt_iterator_create(sqlite3_stmt* stmt);

/**
* @brief stmt_iterator_dispose Destroy the stmt iterator
* @param it The iterator to destroy
* @return IT_DONE if no error occured
*/
int libmzdb_stmt_iterator_dispose(libmzdb_stmt_iterator_t* it);

/**
* @brief stmt_iterator_next Get the next object (column) of the queries
* @param it The iterator
* @return NULL if an error occured else the stmt to process
*/
sqlite3_stmt* libmzdb_stmt_iterator_next(libmzdb_stmt_iterator_t* it);

/**
* @brief stmt_iterator_has_next Check if there is still column to process
* @param it The iterator
* @return IT_ERROR if an error occured, IT_STEP if there is still column to process else IT_DONE
*/
int libmzdb_stmt_iterator_has_next(libmzdb_stmt_iterator_t* it);

/**
* @brief stmt_iterator_for_each For each column of the queries, call a callback with the stmt as argument
* @param stmt The queries to execute
* @param arg Possible argument for the callback
* @param result Possible location for the result of the callback
* @return IT_ERROR if an error occured, else IT_DONE
*/
int libmzdb_stmt_iterator_for_each(sqlite3_stmt *stmt, void(*callback) (sqlite3_stmt*, void*, void**), void *arg, void **result);


/*==============================BBOX ITERATOR====================================*/


typedef struct libmzdb_bbox_iterator
{
	libmzdb_stmt_iterator_t* stmt_it;
	libmzdb_entity_cache_t entity_cache;
} libmzdb_bbox_iterator_t;

/**
* @brief bbox_iterator_create Create the bounding box iterator
* @param stmt The Queries to execute
* @param de_cache The data encoding cache
* @return The bounding box iterator
*/
libmzdb_bbox_iterator_t* libmzdb_bbox_iterator_create(sqlite3_stmt* stmt, libmzdb_entity_cache_t entity_cache);

/**
* @brief bbox_iterator_dispose Destroy a bounding box iterator
* @param bb_it The bounding box iterator
*/
void libmzdb_bbox_iterator_dispose(libmzdb_bbox_iterator_t* bb_it);

/**
* @brief bbox_iterator_next Get the next BBox
* @param bb_it The bounding box iterator
* @param bbox The pointer of the next Bbox
* @return It_ERROR if an error occured else IT_OK
*/
int libmzdb_bbox_iterator_next(libmzdb_bbox_iterator_t* bb_it, libmzdb_bounding_box_t* bbox);

/**
* @brief bbox_iterator_has_next Check if the iterator has at least one next bbox waiting to process
* @param bbox_it The bounding box iterator
* @return IT_ERROR if an error occured, IT_STEP if there is still column to process else IT_DONE
*/
int libmzdb_bbox_iterator_has_next(libmzdb_bbox_iterator_t* bbox_it);

/**
* @brief bbox_iterator_for_each For each bouding box call a callback
* @param stmt The query to execute
* @param entity_cache The entity cache
* @param arg The possible argument for the callback
* @param result The possible result for the callback
* @return IT_ERROR if an error occured, else IT_DONE
*/
int libmzdb_bbox_iterator_for_each(sqlite3_stmt* stmt, libmzdb_entity_cache_t libmzdb_entity_cache, void(*callback) (libmzdb_bounding_box_t, void*, void**), void* arg, void **result);


/*==============================SPECTRUM ITERATOR====================================*/


typedef struct libmzdb_spectrum_iterator
{
	libmzdb_bbox_iterator_t* bbox_it;
	libmzdb_bounding_box_t* first_bb;

	int spectrum_idx;
	libmzdb_spectrum_t* spectrum_buffer;
	int spectrum_count;

	int has_next_bb;
	int run_slices_count;

	int is_single_ms_level_mode;

} libmzdb_spectrum_iterator_t;

//FIXME: In the process of iterator, there is still some memory leak (few MegaB)

/**
* @brief spectrum_iterator_create Create a spectrum iterator
* @param db The database on which the SQL executes
* @param stmt The query to execute
* @param entity_cache The entity cache
* @return NULL if an error occured, else the spectrum iterator
*/
libmzdb_spectrum_iterator_t* libmzdb_spectrum_iterator_create(sqlite3* db, int ms_level, libmzdb_entity_cache_t entity_cache);

/**
* @brief spectrum_iterator_dispose Destroy the spectrum it
* @param spectrum_it The spectrum iterator
*/
void libmzdb_spectrum_iterator_dispose(libmzdb_spectrum_iterator_t* spectrum_it);

/**
* @brief spectrum_iterator_next Get the next spectrum
* @param it The spectrum iterator
* @param result the next spectrum
* @return It_OK if no error occured
*/
int libmzdb_spectrum_iterator_next(libmzdb_spectrum_iterator_t* it, libmzdb_spectrum_t* result);

/**
* @brief spectrum_iterator_has_next Check if there is at least one spectrum waiting to process in the iterator
* @return IT_STEP if a spectrum waits to be process else IT_DONE
*/
int libmzdb_spectrum_iterator_has_next(libmzdb_spectrum_iterator_t* it);

/**
* @brief spectrum_iterator_for_each For each spectrum, call a callback
* @param db The database on which the SQL executes
* @param stmt The query to execute
* @param entity_cache The entity cache
* @param arg Possible argument for the callback
* @param result Possible result for the callback
* @return IT_ERROR if an error occured, else IT_DONE
*/
int libmzdb_spectrum_iterator_for_each(sqlite3* db, int ms_level, libmzdb_entity_cache_t entity_cache, void(*callback) (libmzdb_spectrum_t, void*, void**), void* arg, void **result);

#endif //_LIBMZDB_H_
