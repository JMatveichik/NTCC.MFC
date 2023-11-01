#ifndef __FONTMANAGER_H_INCLUDED__
#define __FONTMANAGER_H_INCLUDED__

#pragma warning( disable : 4786 )

#include <wingdi.h>
#include <vector>
#include <functional>
#include <utility>
#include <map>

class FontManager
{
public:
	static FontManager& Instance()
	{
		static FontManager obj;
		return obj;
	}

	// TODO: reference counting
	unsigned RegisterFont( const LOGFONT& lf )
	{
		EnterCriticalSection(&m_cs);

		LOGFONT lfPush = lf;
		lfPush.lfCharSet = DEFAULT_CHARSET;
		lfPush.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lfPush.lfOutPrecision = OUT_CHARACTER_PRECIS;

		std::pair< LogfontMap::iterator, bool > p = logfontMap.insert( std::make_pair( (const LOGFONT&)lfPush, -1 ) );
		if( !p.second )
		{
			LeaveCriticalSection(&m_cs);
			return (*p.first).second;
		}

		fontPairVector.push_back(std::make_pair(HFONT(0), &p.first->first));

		unsigned res = (*p.first).second = unsigned(fontPairVector.size()-1);

		LeaveCriticalSection(&m_cs);

		return res;
	}

	HFONT GetFont( unsigned index )
	{
		EnterCriticalSection(&m_cs);

		HfontLogfontPair& p = fontPairVector.at( index );

		if( !p.first )
			p.first = CreateFontIndirect( p.second );

		LeaveCriticalSection(&m_cs);

		return p.first;
	}

	unsigned Size() const {	return unsigned(fontPairVector.size()); };

	~FontManager()
	{ 	
		for( int i=0; i < int(fontPairVector.size()); i++ )
			DeleteObject( fontPairVector.at(i).first );
		DeleteCriticalSection(&m_cs);
	}

protected:
	FontManager() {
		InitializeCriticalSection(&m_cs); 
	}

	struct lf_less : public std::binary_function< const LOGFONT&, const LOGFONT&, bool >
	{
		/*int weight(const LOGFONT& lf)
		{
			int weight = 0;
			weight += 1000 * lf.lfHeight;
			weight += 500 * lf.lfWeight;
			weight += lf.lfItalic;
			weight += lf.lfUnderline;
			weight += lf.lfEscapement;
			int i;
			for (i=0; i<strlen(lf.lfFaceName); i++)
				weight += 100 * int(lf.lfFaceName[i]);
			return weight;
		}

		bool operator() ( const LOGFONT& lf, const LOGFONT& compare )
		{
			return weight(lf) < weight(compare);
		}*/

		bool operator() ( const LOGFONT& lf, const LOGFONT& compare ) const
		{
			unsigned lhs = 0;
			for (unsigned i=0; i<sizeof(LOGFONT); i++)
				lhs += reinterpret_cast<const char*>(&lf)[i];

			unsigned rhs = 0;
			for (unsigned i=0; i<sizeof(LOGFONT); i++)
				rhs += reinterpret_cast<const char*>(&compare)[i];

			return lhs < rhs;
		}

		/*bool operator() ( const LOGFONT& lf, const LOGFONT& compare )
		{
			if( _stricmp( lf.lfFaceName, compare.lfFaceName ) < 0 )
				return true;
			
			if( lf.lfHeight < compare.lfHeight )
				return true;

			if( lf.lfWeight < compare.lfWeight )
				return true;

			if( lf.lfItalic < compare.lfItalic )
				return true;

			if( lf.lfUnderline < compare.lfUnderline )
				return true;

			if( lf.lfEscapement < compare.lfEscapement )
				return true;

			return false;
		}*/
	};

	typedef std::map< const LOGFONT, int, lf_less > LogfontMap;
	typedef std::pair< HFONT, const LOGFONT* const > HfontLogfontPair;
	
	LogfontMap logfontMap;
	std::vector< HfontLogfontPair > fontPairVector;
	CRITICAL_SECTION m_cs;
};

#ifdef _MT
static class FontManagerInit
{
public:
	FontManagerInit() { FontManager::Instance(); }
} fm_init_;
#endif //#ifdef _MT


#endif