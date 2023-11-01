#include "stdafx.h"
#include "ah_chem.h"
#include "ah_xml.h"
#include "ah_msgprovider.h"
#include "resource.h"

#ifdef USE_CHEM_LIB_FEATURES

AppThermHelper::AppThermHelper()
{
	pTHDB = new CSpeciesDB();
	pSS   = new CSpecieSet();
}

AppThermHelper::~AppThermHelper()
{

	for(auto it = mapMixtures.begin(); it != mapMixtures.end(); ++it)
	{
		delete it->second;
		//pTHDB->Detach(*(it->second));
	}
	
	pSS->Detach(pTHDB);
	delete pSS;
	delete pTHDB;	
}

bool AppThermHelper::AddMixture(IXMLDOMNode* pNode, std::pair<std::string, std::string>& mix)
{
	if ( pNode == NULL )
		return false;

	CComPtr<IXMLDOMNodeList> pSpeciesList;
	pNode->selectNodes( CComBSTR("SPEC"), &pSpeciesList);

	long lSpecCount;	

	pSpeciesList->get_length(&lSpecCount);		
	pSpeciesList->reset();
	
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	CString gasName;
	if ( !appXML.GetNodeAttributeByName(pNode, "ID", gasName) || gasName.IsEmpty())
		return false;

	mix.first = (LPCTSTR)gasName;

	CString strComp;

	for(int i = 0; i < lSpecCount; i++)
	{

		CComPtr<IXMLDOMNode>	 pSpec;
		pSpeciesList->get_item(i, &pSpec);

		CString val;

		if ( !appXML.GetNodeAttributeByName(pSpec, "NAME", val) || val.IsEmpty())
			return false;

		if ( pTHDB->FindSpecie((LPCTSTR)val) < 0 )
		{
			strComp.Format(IDS_CHEM_CHECK0, val); //Species <%s> was not found in thermal database file
			mix.second = (LPCTSTR)strComp;
			return false;
		}

		pSS->Add((LPCTSTR) val);

		strComp += val; 
		strComp += '='; 

		if ( !appXML.GetNodeAttributeByName(pSpec, "FRACTION", val) || val.IsEmpty())
			return false;

		strComp += val; 
		strComp += ' ';
		double frac  = atof((LPCTSTR)val);		

	}
	mix = std::make_pair( (LPCTSTR)gasName, strComp);
	return mapComp.insert( mix ).second;
}

bool AppThermHelper::Init(std::string filePath)
{
	std::ifstream inp( filePath.c_str() ); 
	if ( inp.good() )
	{
		std::strstream str;

		if ( pTHDB->Create(filePath.c_str(), &str) != ckcl::NOERRORS )
		{
			AppMessagesProviderHelper::Instance().ShowMessage(str, COutMessage::MSG_ERROR);
			return false;
		}	
		return true;
	}
	return false;
}

bool AppThermHelper::BuildMixtures()
{
	if ( pSS->Attach(pTHDB) != ckcl::NOERRORS )
		return false;

	mapMixtures.clear();

	for ( std::map<std::string, std::string>::iterator it = mapComp.begin(); it != mapComp.end(); ++it )
	{
		CIdealGasMix* pGM = new CIdealGasMix(*pTHDB);
		if ( pGM->SetComposition( (*it).second  ) != ckcl::NOERRORS )
			return false;

		mapMixtures.insert( std::make_pair( (*it).first, pGM) );
	}
	return true;
}

bool AppThermHelper::SetMassFlowComposition(MassFlow& mf, std::string gasName)
{
	std::map<std::string, CIdealGasMix*>::iterator itFind = mapMixtures.find(gasName);
	if ( itFind == mapMixtures.end() )
		return false;

	mf.SetComposition( *(itFind->second) );	

	return true;
}

#endif //USE_CHEM_LIB_FEATURES