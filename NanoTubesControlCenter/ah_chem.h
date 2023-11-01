#pragma once

#include "singletone.h"

#ifndef _APP_CHEMLIB_HELPER_H_INCLUDED_
#define _APP_CHEMLIB_HELPER_H_INCLUDED_

#ifdef USE_CHEM_LIB_FEATURES
#include "..\ChemLib\include\chemlib.h"

class AppThermHelper: public Singletone<AppThermHelper>
{
	friend class Singletone<AppThermHelper>;

public:

	virtual ~AppThermHelper();

protected:

	AppThermHelper(); 	
	AppThermHelper(const AppThermHelper& );
	const AppThermHelper& operator= (const AppThermHelper&);

public:	

	bool Init(std::string filePath);
	bool BuildMixtures();
	bool SetMassFlowComposition(MassFlow& mf, std::string gasName);
	bool AddMixture(IXMLDOMNode* pNode, std::pair<std::string, std::string>& mix);
	const CSpeciesDB& GetTHDB() const { return *pTHDB; };

protected:

	CSpeciesDB* pTHDB;	
	CSpecieSet* pSS;

	std::map<std::string, CIdealGasMix*>	mapMixtures;
	std::map<std::string, std::string>		mapComp;
	std::string thermFilePath;
};
#endif //USE_CHEM_LIB_FEATURES

#endif _APP_CHEMLIB_HELPER_H_INCLUDED_