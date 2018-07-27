#include <Rcpp.h>
#include <Rdefines.h>

#include <iostream>
#include <string>
#include <vector>

extern "C"
{
#include "libmzdb.h"
#include "sqlite3.h"
}

#define STR_TO_CSTR(_str_, _cstr_)					\
	_cstr_ = new char[_str_.size() + 1];					\
	memcpy(_cstr_, _str_.c_str(), _str_.size() + 1);		\

#define DESTROY_VEROBSE 0

using namespace Rcpp;


class SpectrumHeader
{
public:
	int m_id;
	int m_initial_id;
	std::string m_title;
	int m_cycle;
	float m_time;
	int m_ms_level;
	std::string m_activation_type;
	float m_tic;
	double m_base_peak_mz;
	float m_base_peak_intensity;
	double m_precursor_mz;
	int m_precursor_charge;
	int m_peaks_count;
	std::string m_param_tree_str;
	std::string m_scan_list_str;
	std::string m_precursor_list_str;
	std::string m_product_list_str;
	int m_shared_param_tree_id;
	int m_instrument_configuration_id;
	int m_source_file_id;
	int m_run_id;
	int m_data_processing_id;
	int m_data_encoding_id;
	int m_bb_first_spectrum_id;
	int m_is_high_resolution;


	SpectrumHeader()
	{
		std::string emptystr = "";

		m_id = 0;
		m_initial_id = 0;
		m_title = emptystr.c_str();
		m_cycle = 0;
		m_time = 0;
		m_ms_level = 0;
		m_activation_type = emptystr.c_str();
		m_tic = 0;
		m_base_peak_mz = 0;
		m_base_peak_intensity = 0;
		m_precursor_mz = 0;
		m_precursor_charge = 0;
		m_peaks_count = 0;
		m_param_tree_str = emptystr.c_str();
		m_scan_list_str = emptystr.c_str();
		m_precursor_list_str = emptystr.c_str();
		m_product_list_str = emptystr.c_str();
		m_shared_param_tree_id = 0;
		m_instrument_configuration_id = 0;
		m_source_file_id = 0;
		m_run_id = 0;
		m_data_processing_id = 0;
		m_data_encoding_id = 0;
		m_bb_first_spectrum_id = 0;
		m_is_high_resolution = 0;
	}

	libmzdb_spectrum_header_t to_spectrum_header_struct()
	{
		libmzdb_spectrum_header_t h; //the struct which will be return
		h.id = m_id;
		h.initial_id = m_initial_id;
		STR_TO_CSTR(m_title, h.title);
		//std::copy(title.c_str(), title.size(), h.title);
		//h.title = title.c_str();
		h.cycle = m_cycle;
		h.time = m_time;
		h.ms_level = m_ms_level;
		STR_TO_CSTR(m_activation_type, h.activation_type);
		//std::copy(activation_type.c_str(), activation_type.size(), h.activation_type);
		//h.activation_type = activation_type;
		h.tic = m_tic;
		h.base_peak_mz = m_base_peak_mz;
		h.base_peak_intensity = m_base_peak_intensity;
		h.precursor_mz = m_precursor_mz;
		h.precursor_charge = m_precursor_charge;
		h.peaks_count = m_peaks_count;
		STR_TO_CSTR(m_param_tree_str, h.param_tree_str);
		//std::copy(param_tree_str.c_str(), param_tree_str.size(), h.param_tree_str);
		//h.param_tree_str = param_tree_str;
		STR_TO_CSTR(m_scan_list_str, h.scan_list_str);
		//std::copy(scan_list_str.c_str(), scan_list_str.size(), h.scan_list_str);
		//h.scan_list_str = scan_list_str;
		STR_TO_CSTR(m_precursor_list_str, h.product_list_str);
		//std::copy(precursor_list_str.c_str(), precursor_list_str.size(), h.precursor_list_str);
		//h.precursor_list_str = precursor_list_str;
		STR_TO_CSTR(m_product_list_str, h.product_list_str);
		//std::copy(product_list_str.c_str(), product_list_str.size(), h.product_list_str);
		//h.product_list_str = product_list_str;
		h.shared_param_tree_id = m_shared_param_tree_id;
		h.instrument_configuration_id = m_instrument_configuration_id;
		h.source_file_id = m_source_file_id;
		h.run_id = m_run_id;
		h.data_processing_id = m_data_processing_id;
		h.data_encoding_id = m_data_encoding_id;
		h.bb_first_spectrum_id = m_bb_first_spectrum_id;
		h.is_high_resolution = m_is_high_resolution;
		return h;
	}

	SpectrumHeader * from_spectrum_header_struct(libmzdb_spectrum_header_t h)
	{
		m_id = h.id;
		m_initial_id = h.initial_id;
		m_title = h.title;
		m_cycle = h.cycle;
		m_time = h.time;
		m_ms_level = h.ms_level;
		m_activation_type = h.activation_type;
		m_tic = h.tic;
		m_base_peak_mz = h.base_peak_mz;
		m_base_peak_intensity = h.base_peak_intensity;
		m_precursor_mz = h.precursor_mz;
		m_precursor_charge = h.precursor_charge;
		m_peaks_count = h.peaks_count;
		m_param_tree_str = h.param_tree_str;
		m_scan_list_str = h.scan_list_str;
		if (h.precursor_list_str) m_precursor_list_str = h.precursor_list_str;
		if (h.product_list_str) m_product_list_str = h.product_list_str;
		m_shared_param_tree_id = h.shared_param_tree_id;
		m_instrument_configuration_id = h.instrument_configuration_id;
		m_source_file_id = h.source_file_id;
		m_run_id = h.run_id;
		m_data_processing_id = h.data_processing_id;
		m_data_encoding_id = h.data_encoding_id;
		m_bb_first_spectrum_id = h.bb_first_spectrum_id;
		m_is_high_resolution = h.is_high_resolution;

		return (this);
	}

	~SpectrumHeader() {
		if (DESTROY_VEROBSE) std::cout << "destroying SpectrumHeader object" << std::endl;
	}
};

class SpectrumData
{
public:
	NumericVector m_mz_list;
	NumericVector m_intensity_list;

	SpectrumData()
	{
		m_intensity_list = std::vector<double>();
		m_mz_list = std::vector<double>();
	}

	SpectrumData * from_spectrum_data_struct(libmzdb_spectrum_data_t spectrum_data)
	{
		switch (spectrum_data.data_encoding.peak_encoding)
		{
		case LOW_RES_PEAK:
			//std::cout << " is LOW_RES_PEAK" << std::endl;
			m_mz_list = NumericVector(spectrum_data.mz_array_as_floats, spectrum_data.mz_array_as_floats + spectrum_data.peak_count);
			m_intensity_list = NumericVector(spectrum_data.intensity_array_as_floats, spectrum_data.intensity_array_as_floats + spectrum_data.peak_count);
			break;

		case HIGH_RES_PEAK:
			//std::cout << " is HIGH_RES_PEAK" << spectrum_data.mz_array_as_doubles[0] << std::endl;
			m_mz_list = NumericVector(spectrum_data.mz_array_as_doubles, spectrum_data.mz_array_as_doubles + spectrum_data.peak_count);
			m_intensity_list = NumericVector(spectrum_data.intensity_array_as_floats, spectrum_data.intensity_array_as_floats + spectrum_data.peak_count);
			break;

		default: //NO_LOSS_PEAK
			m_mz_list = NumericVector(spectrum_data.mz_array_as_doubles, spectrum_data.mz_array_as_doubles + spectrum_data.peak_count);
			m_intensity_list = NumericVector(spectrum_data.intensity_array_as_doubles, spectrum_data.intensity_array_as_doubles + spectrum_data.peak_count);

		}

		/*switch (spectrum_data.data_encoding.peak_encoding)
		{
		case LOW_RES_PEAK:
			std::cout << " is LOW_RES_PEAK" << std::endl;
			m_intensity_list.assign(spectrum_data.intensity_array_as_floats, spectrum_data.intensity_array_as_floats + spectrum_data.peak_count);
			m_mz_list.assign(spectrum_data.mz_array_as_floats, spectrum_data.mz_array_as_floats + spectrum_data.peak_count);
		break;

		case HIGH_RES_PEAK:
			std::cout << " is HIGH_RES_PEAK " << spectrum_data.mz_array_as_doubles[0] << std::endl;

			for (int i = 0; i < spectrum_data.peak_count; ++i)
			{
				m_intensity_list.push_back(spectrum_data.intensity_array_as_floats[i]);
				m_mz_list.push_back(spectrum_data.intensity_array_as_doubles[i]);
			}

			//m_intensity_list.assign(spectrum_data.intensity_array_as_floats, spectrum_data.intensity_array_as_floats + spectrum_data.peak_count);
			//m_mz_list.assign(spectrum_data.intensity_array_as_doubles, spectrum_data.intensity_array_as_doubles + spectrum_data.peak_count);
		break;

		default: //NO_LOSS_PEAK
			std::cout << " is NO_LOSS_PEAK" << std::endl;
			m_intensity_list.assign(spectrum_data.mz_array_as_doubles, spectrum_data.mz_array_as_doubles + spectrum_data.peak_count);
			m_mz_list.assign(spectrum_data.intensity_array_as_doubles, spectrum_data.intensity_array_as_doubles + spectrum_data.peak_count);

		}

		std::cout << " first m/z in vector = " << get_mz_list().at(0) << std::endl;*/
		

		return (this);
	}

	NumericVector get_instensity_list() { return m_intensity_list; }
	NumericVector get_mz_list() { return m_mz_list; }

	~SpectrumData() {
		if (DESTROY_VEROBSE) std::cout << "destroying SpectrumData object" << std::endl;
	}
};

class Spectrum
{
public:
	SpectrumHeader * m_header;
	SpectrumData  * m_data;

	Spectrum()
	{
		m_header = NULL;
		m_data = NULL;
	}

	Spectrum * from_spectrum_struct(libmzdb_spectrum_t _spectrum)
	{
		m_header = new SpectrumHeader();
		m_header->from_spectrum_header_struct(_spectrum.header);
		m_data = new SpectrumData();
		m_data->from_spectrum_data_struct(_spectrum.data);

		return (this);
	}

	SpectrumHeader * get_header() { return m_header; }

	SpectrumData * get_data() { return m_data; }

	~Spectrum() {
		if (DESTROY_VEROBSE) std::cout << "destroying Spectrum object" << std::endl;
	}
};

//typedef struct Spectrum Spectrum;
//typedef struct SpectrumHeader SpectrumHeader; 
//typedef struct SpectrumData SpectrumData;

/*void mzdb_reader_consume_for_each_spectrum_cb(libmzdb_spectrum_t _s, Rcpp::Function callback, void** result) {
	//Rcpp::Function callback = (Rcpp::Function) r_cb;

	Spectrum * spectrum = new Spectrum();
	spectrum->from_spectrum_struct(_s);

	SEXP spectrum_sexp = Rcpp::wrap(*spectrum);
	callback(spectrum_sexp);
}*/

class MzDb
{
private:

	std::string m_err_msg; //use to store the last error, can be access with "get_error()"
	int m_rc; //use to store the last rc, can be access with "get_rc()"
	
	//if rc != 0 store the error message in the global error message
	void check_rc(const char * err_msg) { if (m_rc != 0) m_err_msg = err_msg; }

	//convert a c string to a cpp string 
	std::string _cstr_to_str(const char * cstr) {
		if (cstr) {
			std::string str(cstr);
			return str;
		} else return "";
	}
	//convert a cpp string to a c string
	const char * cstr(std::string str) { return str.c_str(); }

public:

	sqlite3 * m_db;
	libmzdb_entity_cache_t * m_entity_cache;

	MzDb(std::string filename)
	{

		std::cout << "HELLO WORLD !!" << std::endl;

		char * c_errmsg;
		m_err_msg = "no error"; m_rc = 0; //initialise the error msg and the rc
		
		m_rc = libmzdb_open_mzdb_file(filename.c_str(), &m_db); //TODO : check if the db is open
		check_rc( cstr("couldn't open the mzDB file") );
		
		m_rc = libmzdb_create_entity_cache(m_db, &m_entity_cache, &c_errmsg); //TODO : check if ec was correctly created

		check_rc(c_errmsg);
	}

	~MzDb()			
	{
		//TODO : free entity cache!!
		if (DESTROY_VEROBSE) std::cout << "destroying MzDb object" << std::endl;
		libmzdb_close_mzdb_file(m_db);
	}

	//use to get the data of the last error raised (default value : errmsg = "no error" and rc = 0)
	std::string get_last_error() { return m_err_msg; }
	int get_last_rc() { return m_rc; }

	std::string get_model_version() 
	{
		char * version; char * err_msg;
		m_rc = libmzdb_get_model_version(m_db, &version, &err_msg);
		//std::cout << "version=" << version << std::endl;
		check_rc(err_msg);
		return _cstr_to_str(version);
	}; 

	std::string get_param_tree_chromatogram()
	{
		char * param_tree_chro; char * err_msg;
		m_rc = libmzdb_get_param_tree_chromatogram(m_db, &param_tree_chro, &err_msg);
		check_rc(err_msg);
		return _cstr_to_str(param_tree_chro);
	}

//	std::string get_param_tree_spectrum()
//	{
//		char * param_tree_spectrum; char * err_msg;
//		m_rc = libmzdb_get_param_tree_spectrum(m_db, &param_tree_spectrum, &err_msg);
//		check_rc(err_msg);
//		return _cstr_to_str(param_tree_spectrum);
//	}

//TODO: need to implement a Rcpp module to use it
//	std::string get_param_tree_mzdb()
//	{
//		char * param_tree_mzdb; char * err_msg;
//		m_rc = libmzdb_get_param_tree_mzdb(m_db, &param_tree_mzdb, &err_msg);
//		check_rc(err_msg);
//		return _cstr_to_str(param_tree_mzdb);
//	}

	std::string get_pwiz_version()
	{
		char * err_msg; char * mzdb_pwiz_version;
		m_rc = libmzdb_get_pwiz_mzdb_version(m_db, &mzdb_pwiz_version, &err_msg);
		check_rc(err_msg);
		return _cstr_to_str(mzdb_pwiz_version);
	}

	double get_last_time()
	{
		double last_time; char * err_msg;
		m_rc = libmzdb_get_last_time(m_db, &last_time, &err_msg);
		check_rc(err_msg);
		return last_time;
	}

	int get_max_ms_level()
	{
		int max_ms_level; char * err_msg;
		m_rc = libmzdb_get_max_ms_level(m_db, &max_ms_level, &err_msg);
		check_rc(err_msg);
		return max_ms_level;
	}

	int get_bounding_box_count_from_sequence()
	{
		int bb_count; char * err_msg;
		m_rc = libmzdb_get_bounding_boxes_count_from_sequence(m_db, &bb_count, &err_msg);
		check_rc(err_msg);
		return  bb_count;
	}

	//get mz range for R
	NumericVector R_get_mz_range(int ms_level)
	{
		int * mz_range; char * err_msg;
		m_rc = libmzdb_get_mz_range(m_db, ms_level, &mz_range, &err_msg);
		check_rc(err_msg);
		
		NumericVector r_mz_range;
		if (m_rc == 0) { //if no errors occured, init the output
			r_mz_range[0] = mz_range[0]; //min mz range
			r_mz_range[1] = mz_range[1]; //max mz range
			free(mz_range);
		}
		return r_mz_range;
	}

	//get mz range for c++
	int* get_mz_range(int ms_level)
	{
		int * mz_range; char * err_msg;
		m_rc = libmzdb_get_mz_range(m_db, ms_level, &mz_range, &err_msg);
		check_rc(err_msg);
		return mz_range;
	}

	int get_bounding_box_count(int bb_run_slice_id)
	{
		int bb_count; char * err_msg;
		m_rc = libmzdb_get_bounding_boxes_count(m_db, bb_run_slice_id, &bb_count, &err_msg);
		check_rc(err_msg);
		return bb_count;
	}

	int get_cycle_count()
	{
		int cycles_count; char * err_msg;
		m_rc = libmzdb_get_cycles_count(m_db, &cycles_count, &err_msg);
		check_rc(err_msg);
		return cycles_count;
	}

	int get_data_encoding_count_from_sequence()
	{
		int de_count; char * err_msg;
		m_rc = libmzdb_get_data_encodings_count_from_sequence(m_db, &de_count, &err_msg);
		check_rc(err_msg);
		return de_count;
	}

	int get_spectra_count_from_sequence()
	{
		int spectra_count; char * err_msg;
		m_rc = libmzdb_get_spectra_count_from_sequence(m_db, &spectra_count, &err_msg);
		check_rc(err_msg);
		return spectra_count;
	}

	int get_spectra_count(int ms_level)
	{
		int spectra_count; char * err_msg;
		m_rc = libmzdb_get_spectra_count(m_db, ms_level, &spectra_count, &err_msg);
		check_rc(err_msg);
		return spectra_count;
	}

	int get_run_slices_count_from_sequence()
	{
		int run_slices_count; char * err_msg;
		m_rc = libmzdb_get_run_slices_count_from_sequence(m_db, &run_slices_count, &err_msg);
		check_rc(err_msg);
		return run_slices_count;
	}

	int get_table_records_count(std::string table_name)
	{
		int table_records_count; char * err_msg;
		char * cstr_table_name = new char[table_name.size() + 1];
		std::copy(table_name.begin(), table_name.end(), cstr_table_name);
		cstr_table_name[table_name.size()] = '\0';
		m_rc = libmzdb_get_table_records_count(m_db, cstr_table_name, &table_records_count, &err_msg);
		check_rc(err_msg);
		return table_records_count;
	}

	//get bb data for R
	NumericVector R_get_bounding_box_data(int bb_id)
	{
		byte * blob; int blob_len = 0; char * err_msg;
		m_rc = libmzdb_get_bounding_box_data(m_db, bb_id, &blob, &blob_len, &err_msg);
		check_rc(err_msg);
		
		NumericVector R_blob;
		while (blob_len --> 0) R_blob[blob_len - 1] = blob[blob_len - 1];
		return R_blob;
	}

	//get bb data for c++
	byte * get_bounding_box_data(int bb_id, int * blob_len)
	{
		byte * blob; char * err_msg;
		m_rc = libmzdb_get_bounding_box_data(m_db, bb_id, &blob, blob_len, &err_msg);
		check_rc(err_msg);
		return blob;
	}

	//int libmzdb_get_chromatogram_data_points(sqlite3 *db, int c_id, libmzdb_data_points_32_32_t **data, int *data_points_count, char **err_msg)
	//TODO !

	//int libmzdb_get_bounding_box_first_spectrum_id(sqlite3 *db, int first_id, long *bb_first_spectrum_id, char **err_msg)
	long get_bounding_box_first_spectrum_id(int first_id)
	{
		long bb_first_spectrum_id; char * err_msg;
		m_rc = libmzdb_get_bounding_box_first_spectrum_id(m_db, first_id, &bb_first_spectrum_id, &err_msg);
		check_rc(err_msg);
		return bb_first_spectrum_id;
	}

	//int libmzdb_get_bounding_box_min_mz(sqlite3 *db, int bb_r_tree_id, float *bb_min_mz, char **err_msg)
	float get_bounding_box_min_mz(int bb_r_tree_id)
	{
		float bb_min_mz; char * err_msg;
		m_rc = libmzdb_get_bounding_box_min_mz(m_db, bb_r_tree_id, &bb_min_mz, &err_msg);
		check_rc(err_msg);
		return bb_min_mz;
	}

	//int libmzdb_get_bounding_box_min_time(sqlite3 *db, int bb_r_tree_id, float *bb_min_time, char **err_msg)
	float get_bouding_box_min_time(int bb_r_tree_id)
	{
		float bb_min_time; char * err_msg;
		m_rc = libmzdb_get_bounding_box_min_time(m_db, bb_r_tree_id, &bb_min_time, &err_msg);
		check_rc(err_msg);
		return bb_min_time;
	}

	//int libmzdb_get_run_slice_id(sqlite3 *db, int bb_id, int *run_slice_id, char **err_msg)
	int get_run_slice_id(int bb_id)
	{
		int run_slice_id; char * err_msg;
		m_rc = libmzdb_get_run_slice_id(m_db, bb_id, &run_slice_id, &err_msg);
		check_rc(err_msg);
		return run_slice_id;
	}

	//int libmzdb_get_ms_level_from_run_slice_id_manually(sqlite3 *db, int run_slice_id, int *ms_level, char **err_msg)
	int get_ms_level_from_run_slice_id(int run_slice_id)
	{
		int ms_level; char * err_msg;
		m_rc = libmzdb_get_ms_level_from_run_slice_id_manually(m_db, run_slice_id, &ms_level, &err_msg);
		check_rc(err_msg);
		return ms_level;
	}

	//int libmzdb_get_bounding_box_ms_level(sqlite3 *db, int bb_id, int *result_ms_level, char **err_msg)
	int get_bounding_box_ms_level(int bb_id)
	{
		int ms_level; char * err_msg;
		m_rc = libmzdb_get_bounding_box_ms_level(m_db, bb_id, &ms_level, &err_msg);
		check_rc(err_msg);
		return ms_level;
	}

	//int libmzdb_get_data_encoding_id(sqlite3 *db, int bounding_box_id, int *data_encoding_id, char **err_msg)
	int get_data_encoding_id(int bb_id)
	{
		int de_id; char * err_msg;
		m_rc = libmzdb_get_data_encoding_id(m_db, bb_id, &de_id, &err_msg);
		check_rc(err_msg);
		return de_id;
	}

	//int libmzdb_get_spectrum(sqlite3* db, long spectrum_id, libmzdb_entity_cache_t entity_cache, libmzdb_spectrum_t** result)
	Spectrum* get_spectrum(int spectrum_id)
	{
		libmzdb_spectrum_t * c_spectrum;
		std::cout << "step 1" << std::endl;
		m_rc = libmzdb_get_spectrum(m_db, spectrum_id, *m_entity_cache, &c_spectrum);
		std::cout << "step 2" << std::endl;
		check_rc("get_spectrum failed: <empty>");
		std::cout << "step 3" << std::endl;
		Spectrum * spectrum = new Spectrum();
		std::cout << "step 4" << std::endl;
		return spectrum->from_spectrum_struct(*c_spectrum);
	}

	/*void create_for_each_spectrum(Rcpp::Function r_callback, void(*c_callback) (libmzdb_spectrum_t, void*, void**) ) {

	}

	void for_each_spectrum(int ms_level, Rcpp::Function callback, SEXP _spectrum_sexp) {
		std::cout << "from for_each_spectrum2" << std::endl;
		//Spectrum* s = get_spectrum(1);
		//SEXP spectrum_sexp = Rcpp::wrap(*s);
		//callback(spectrum_sexp);

		libmzdb_spectrum_iterator_for_each(m_db, ms_level, *m_entity_cache, &mzdb_reader_consume_for_each_spectrum_cb, callback, NULL);
	}*/

	/*SpectrumIterator *create_spectrum_iterator(int ms_level) {
		std::cout << "from create_spectrum_iterator" << std::endl;

		SpectrumIterator * spec_iter = new SpectrumIterator(this, ms_level);

		return spec_iter;
	}*/

};

class SpectrumIterator {
private:

	libmzdb_spectrum_iterator_t * m_underlying_iter;

	std::string m_err_msg; //use to store the last error, can be access with "get_error()"
	int m_rc; //use to store the last rc, can be access with "get_rc()"

			  //if rc != 0 store the error message in the global error message
	void check_rc(const char * err_msg) { if (m_rc != 0) m_err_msg = err_msg; }

	//convert a c string to a cpp string 
	std::string _cstr_to_str(const char * cstr) {
		if (cstr) {
			std::string str(cstr);
			return str;
		}
		else return "";
	}
	//convert a cpp string to a c string
	const char * cstr(std::string str) { return str.c_str(); }

public:

	SpectrumIterator(MzDb * _mzdb, int _ms_level)
	{
		//Rcpp::XPtr<MzDb> xptr_mzdb(_sexp_mzdb);
		m_underlying_iter = libmzdb_spectrum_iterator_create(_mzdb->m_db, _ms_level, *_mzdb->m_entity_cache);
	}

	bool has_next() {
		return libmzdb_spectrum_iterator_has_next(m_underlying_iter) == IT_STEP;
	}

	Spectrum * next_elem() {
		libmzdb_spectrum_t s;
		m_rc = libmzdb_spectrum_iterator_next(m_underlying_iter, &s);
		Spectrum *  spectrum = new Spectrum();
		spectrum->from_spectrum_struct(s);
		return spectrum;
	}

	//sub libmzdb_spectrum_iterator_dispose(SpectrumIteratorPtr $it) returns void is export is native(LIB) { * };

	~SpectrumIterator() {
		if (DESTROY_VEROBSE) std::cout << "destroying SpectrumIterator object" << std::endl;
		libmzdb_spectrum_iterator_dispose(m_underlying_iter);
	}

};

RCPP_EXPOSED_CLASS(SpectrumHeader);
RCPP_EXPOSED_CLASS(SpectrumData);
RCPP_EXPOSED_CLASS(Spectrum);
RCPP_EXPOSED_CLASS(MzDb);

RCPP_MODULE(libmzdbR)
{
	class_<SpectrumHeader>("SpectrumHeader")
		.constructor()
		.field("id", &SpectrumHeader::m_id)
		.field("initial_id", &SpectrumHeader::m_initial_id)
		.field("cycle", &SpectrumHeader::m_cycle)
		.field("time", &SpectrumHeader::m_time)
		.field("ms_level", &SpectrumHeader::m_ms_level)
		.field("activation_type", &SpectrumHeader::m_activation_type)
		.field("tic", &SpectrumHeader::m_tic)
		.field("base_peak_mz", &SpectrumHeader::m_base_peak_mz)
		.field("base_peak_intensity", &SpectrumHeader::m_base_peak_intensity)
		.field("precursor_mz", &SpectrumHeader::m_precursor_mz)
		.field("precursor_charge", &SpectrumHeader::m_precursor_charge)
		.field("peaks_count", &SpectrumHeader::m_peaks_count)
		.field("param_tree", &SpectrumHeader::m_param_tree_str)
		.field("scan_list", &SpectrumHeader::m_scan_list_str)
		.field("precursor_list", &SpectrumHeader::m_precursor_list_str)
		.field("product_list", &SpectrumHeader::m_product_list_str)
		.field("shared_param_tree_id", &SpectrumHeader::m_shared_param_tree_id)
		.field("instrument_configuration_id", &SpectrumHeader::m_instrument_configuration_id)
		.field("source_file_id", &SpectrumHeader::m_source_file_id)
		.field("run_id", &SpectrumHeader::m_run_id)
		.field("data_processing_id", &SpectrumHeader::m_data_processing_id)
		.field("data_encoding_id", &SpectrumHeader::m_data_encoding_id)
		.field("bb_first_spectrum_id", &SpectrumHeader::m_bb_first_spectrum_id)
		.field("is_high_resolution", &SpectrumHeader::m_is_high_resolution)

		; //End RCPP_Module defintion : SpectrumHeader

	class_<SpectrumData>("SpectrumData")
		.constructor()
		.method("get_instensity_list", &SpectrumData::get_instensity_list)
		.method("get_mz_list", &SpectrumData::get_mz_list)
		; //End Rcpp_Module definition : SpectrumData

	class_<Spectrum>("Spectrum")
		.constructor()
		.method("get_header", &Spectrum::get_header)
		.method("get_data", &Spectrum::get_data)
		//.field("header", &Spectrum::header)
		//.field("data", &Spectrum::data)
		; //End Rcpp_Module definition : Spectrum

	class_ <MzDb>("MzDb")
		.constructor<std::string>()
		.method("get_last_error", &MzDb::get_last_error)
		.method("get_last_rc", &MzDb::get_last_rc)
		.method("get_model_version", &MzDb::get_model_version)
		.method("get_param_tree_chromatogram", &MzDb::get_param_tree_chromatogram)
		//.method("get_param_tree_spectrum", &MzDb::get_param_tree_spectrum)
		//.method("get_param_tree_mzdb", &MzDb::get_param_tree_mzdb)
		.method("get_pwiz_version", &MzDb::get_pwiz_version)
		.method("get_last_time", &MzDb::get_last_time)
		.method("get_max_ms_level", &MzDb::get_max_ms_level)
		.method("get_bounding_box_count_from_sequence", &MzDb::get_bounding_box_count_from_sequence)
		.method("get_mz_range", &MzDb::R_get_mz_range)
		.method("get_bounding_box_count", &MzDb::get_bounding_box_count)
		.method("get_cycle_count", &MzDb::get_cycle_count)
		.method("get_data_encoding_count_from_sequence", &MzDb::get_data_encoding_count_from_sequence)
		.method("get_spectra_count_from_sequence", &MzDb::get_spectra_count_from_sequence)
		.method("get_spectra_count", &MzDb::get_spectra_count)
		.method("get_run_slices_count_from_sequence", &MzDb::get_run_slices_count_from_sequence)
		.method("get_table_records_count", &MzDb::get_table_records_count)
		.method("get_bounding_box_data", &MzDb::R_get_bounding_box_data)
		.method("get_bounding_box_first_spectrum_id", &MzDb::get_bounding_box_first_spectrum_id)
		.method("get_bounding_box_min_mz", &MzDb::get_bounding_box_min_mz)
		.method("get_bouding_box_min_time", &MzDb::get_bouding_box_min_time)
		.method("get_run_slice_id", &MzDb::get_run_slice_id)
		.method("get_ms_level_from_run_slice_id", &MzDb::get_ms_level_from_run_slice_id)
		.method("get_bounding_box_ms_level", &MzDb::get_bounding_box_ms_level)
		.method("get_bounding_box_ms_level", &MzDb::get_bounding_box_ms_level)
		.method("get_data_encoding_id3", &MzDb::get_data_encoding_id)
		.method("get_spectrum", &MzDb::get_spectrum)
	; //End RCPP_Module definition : MzDb

	class_<SpectrumIterator>("SpectrumIterator")
		.constructor<MzDb*, int>()
		.method("has_next", &SpectrumIterator::has_next)
		.method("next_elem", &SpectrumIterator::next_elem)
	;

	//TODO...IN THE NEXT EPISODE;
};