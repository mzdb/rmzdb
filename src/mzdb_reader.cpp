#include <Rcpp.h>
#include <Rdefines.h>

#include <string>
#include <iostream>

extern "C"
{
#include "libmzdb.h"
#include "sqlite3.h"
}

using namespace Rcpp;

class MzDb
{
private:

	sqlite3 * m_db;
	libmzdb_entity_cache_t * m_entity_cache;

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
		else {
			return "";
		}
	}
	//convert a cpp string to a c string
	const char * cstr(std::string str) { return str.c_str(); }

public:

	MzDb(std::string filename)
	{

		//std::cout << "flag 0 " << std::endl;

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
		std::cout << "destructing MzDb object" << std::endl;
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
		m_rc = libmzdb_get_param_tree_chromatogram(db, &param_tree_chro, &err_msg);
		check_rc(err_msg);
		return _cstr_to_str(param_tree_chro);
	}

	std::string get_param_tree_spectrum()
	{
		char * param_tree_spectrum; char * err_msg;
		m_rc = libmzdb_get_param_tree_spectrum(db, &param_tree_spectrum, &err_msg);
		check_rc(err_msg);
		return _cstr_to_str(param_tree_spectrum);
	}

	std::string get_param_tree_mzdb()
	{
		char * param_tree_mzdb; char * err_msg;
		m_rc = libmzdb_get_param_tree_mzdb(db, &param_tree_mzdb, &err_msg);
		check_rc(err_msg);
		return _cstr_to_str(param_tree_mzdb);
	}

	int get_pwiz_version()
	{
		char * err_msg; char * mzdb_pwiz_version;
		m_rc = libmzdb_get_pwiz_mzdb_version(db, &mzdb_pwiz_version, &err_msg);
		check_rc(err_msg);
		return _cstr_to_str(mzdb_pwiz_version);
	}

	double get_last_time()
	{
		double last_time; char * err_msg;
		m_rc = libmzdb_get_last_time(db, &last_time, &err_msg);
		check_rc(err_msg);
		return last_time;
	}

};

RCPP_EXPOSED_CLASS(MzDb);
RCPP_MODULE(libmzdbR)
{
	class_ <MzDb>("MzDb")
		.constructor<std::string>()
		.method("get_last_error", &MzDb::get_last_error)
		.method("get_last_rc", &MzDb::get_last_rc)
		.method("get_model_version", &MzDb::get_model_version)
		.method("get_param_tree_chromatogram", &MzDb::get_param_tree_chromatogram)
		.method("get_param_tree_spectrum", &MzDb::get_param_tree_spectrum)
		.method("get_param_tree_mzdb", &MzDb::get_param_tree_mzdb)
		.method("get_param_pwiz_version", &MzDb::get_param_tree_mzdb)
		.method("get_last_time", &MzDb::get_last_time)
	; //END RCPP_Module definition : MzDb

	//TODO...IN THE NEXT EPISODE;
};