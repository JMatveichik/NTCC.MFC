#pragma once

#include "ah_xml.h"

class xml_creation_exception :  public std::exception
{
public:

	xml_creation_exception();
	virtual ~xml_creation_exception();

private:

};

class XmlConfiguredObject
{
public:

	XmlConfiguredObject(void);
	virtual ~XmlConfiguredObject(void);

public:
	
	//создание объекта из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

protected:

	ATTRIBUTES cparams;
	std::string GetCreationParam(std::string param, std::string def = "") const;	
};

