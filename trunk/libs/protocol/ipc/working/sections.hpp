#ifndef sections_hpp
#define sections_hpp

#include "element-interface.hpp"

#include <hparser/linked-section.hpp>


class data_section :
	private element_interface,
	public linked_section
{
public:
	data_section(const text_data &nName) :
	name(nName) {}

	element_interface *extract_interface()
	{ return this; }

	const element_interface *extract_interface() const
	{ return this; }

	bool allow_child(const storage_section*)
	{ return false; }

	const text_data &get_name() const
	{ return name; }

private:
	text_data name;
};


class empty_data_section :
	public data_section
{
public:
	empty_data_section(const text_data &nName) :
	data_section(nName) {}

	section_releaser copy() const
	{ return section_releaser(new empty_data_section(*this)); }

	bool allow_child(const storage_section*)
	{ return false; }

	section_type data_type() const
	{ return empty_section; }

	data_array get_data() const
	{return NULL;  }

	unsigned int data_size() const
	{ return 0; }
};


class actual_data_section :
	public data_section
{
public:
	actual_data_section(const text_data &nName, data_array dData, unsigned int sSize = 0) :
	data_section(nName), binary(dData && sSize), data((!binary && dData)? dData : "")
	{
	if (binary)
	 {
	data.resize(sSize);
	memcpy(&data[0], dData, sSize);
	 }
	}

	actual_data_section(const text_data &nName, const text_data &dData) :
	data_section(nName), binary(true), data(dData) {}

	section_releaser copy() const
	{ return section_releaser(new actual_data_section(*this)); }

	bool allow_child(const storage_section*)
	{ return false; }

	section_type data_type() const
	{ return binary? binary_section : text_section; }

	data_array get_data() const
	{return &data[0];  }

	unsigned int data_size() const
	{ return data.size(); }

private:
	bool      binary;
	text_data data;
};


class group_data_section :
	public data_section
{
public:
	group_data_section(const text_data &nName) :
	data_section(nName) {}

	section_releaser copy() const
	{ return section_releaser(new group_data_section(*this)); }

	section_type data_type() const
	{ return group_section; }

	data_array get_data() const
	{return NULL;  }

	unsigned int data_size() const
	{ return 0; }
};


#endif //sections_hpp
