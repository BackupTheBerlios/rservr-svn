#ifndef element_interface_hpp
#define element_interface_hpp

#include <string>

#include <hparser/impl-defined.hpp>
#include <hparser/storage-section.hpp>


enum section_type {  empty_section = 0x00,
                      text_section,
                    binary_section,
                     group_section  };


typedef std::string  text_data;
typedef const char  *data_array;


struct element_interface
{
	virtual const text_data &get_name() const  = 0;
	virtual section_type     data_type() const = 0;
	virtual data_array       get_data() const  = 0;
	virtual unsigned int     data_size() const = 0;
};

#endif //element_interface_hpp
