#include "stdafx.h"
#include "xyplotconst.h"
#include "group.h"
#include "global.h"

using namespace std;
using namespace xyplot;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

long ProfileGroupMap::CreateGroup()
{
	long currentID = Global::Pointer2ID((void*)1);
	profileGroupMap.insert(make_pair(currentID, long(-1)) );
	return currentID;
}

long ProfileGroupMap::AddProfile(long group, long profile)
{
	if (!group || !profile)
		return PE_INVALID_ARG;

	GroupIterator it = profileGroupMap.find(group);
	if( it == profileGroupMap.end() )
		return PE_INVALID_GROUP;

	if( (*it).second == -1L )
		profileGroupMap.erase(it);

	GroupIterator result = profileGroupMap.insert(make_pair(group, profile));
	return PE_NOERROR;
}

long ProfileGroupMap::FindGroup(long profile)
{
	for (GroupIterator it=profileGroupMap.begin(); it!=profileGroupMap.end(); ++it)
	{
		if( (*it).second == profile )
			return (*it).first;	
	}
	return 0L;
}

void ProfileGroupMap::GetProfiles(long group, std::vector<long>& v)
{
	v.clear();
	GroupIterator it = profileGroupMap.find(group);
	if (it == profileGroupMap.end())
		return;

	for (size_t i=0; i<profileGroupMap.count(group); i++)
	{
		v.push_back(it->second);
		++it;
	}

}