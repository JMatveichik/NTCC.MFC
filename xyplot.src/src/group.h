#ifndef __GROUP_H_INCLUDED__
#define __GROUP_H_INCLUDED__

#include <string>
#include <map>
#include <vector>
#include <assert.h>

typedef std::multimap<long, long>::iterator GroupIterator;

class ProfileGroupMap
{
	friend class Selection;
public:
	ProfileGroupMap() {};
	virtual ~ProfileGroupMap() {};

	long CreateGroup();
	long DeleteGroup(long group) { profileGroupMap.erase(group); return 0; }
	long FindGroup(long profile);
	
	BOOL IsGroup(long key) { return profileGroupMap.find(key) != profileGroupMap.end(); };

	void GetProfiles(long group, std::vector<long>& v);

	long AddProfile(long group, long profile);
	inline long RemoveProfile(long group, long profile) { assert(FALSE); }
	inline long GroupSize( long group ) const { return int(profileGroupMap.count(group)); }

protected:
	std::multimap<long, long> profileGroupMap;
};

#endif