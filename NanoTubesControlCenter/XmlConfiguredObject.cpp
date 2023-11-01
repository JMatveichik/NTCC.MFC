#include "StdAfx.h"
#include "XmlConfiguredObject.h"


XmlConfiguredObject::XmlConfiguredObject(void)
{
}


XmlConfiguredObject::~XmlConfiguredObject(void)
{
}

std::string XmlConfiguredObject::GetCreationParam(std::string param, std::string def /*= ""*/) const
{
	auto it = cparams.find(param);
	if (it != cparams.end())
		return it->second;

	return def;
}

bool XmlConfiguredObject::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (pNode == NULL)
		return false;

	const AppXMLHelper& appXML = appXML.Instance();
	appXML.GetNodeAttributes(pNode, cparams);

	if (cparams.empty())
		return false;

	return true;
}