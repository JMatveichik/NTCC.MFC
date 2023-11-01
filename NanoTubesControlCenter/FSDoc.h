// FSDoc.h : interface of the CFSDoc class
//
#pragma once

#include "ah_images.h"
#include "FSLayer.h"

class FSLayer;
class CDrawObj;
class DataSrc;
class AnalogOutputDataSrc;
class DiscreteDataSrc;
class BaseTimer;

#define HINT_UPDATE_WINDOW					0
#define HINT_UPDATE_DRAWOBJ					1
#define HINT_UPDATE_SELECTION				2
#define HINT_DELETE_SELECTION				3
#define HINT_ADD_CHECKPOINT					4

typedef struct tagControlBarItemInfo{
    std::string dsid;
    double precition;
    int colorid;
    bool autoapply;
}CONTROLBAR_ITEM_INFO, *LPCONTROLBAR_ITEM_INFO;

typedef struct tagPresetInfo {
    std::string name;

    double val;
} PRESET_ITEM, *LP_PREST_ITEM;

typedef struct tagControlBarPresetInfo
{
    std::string title;
    std::string icon;
    std::vector<PRESET_ITEM> presetItems;
}CONTROLBAR_PRESET_INFO, *LPCONTROLBAR_PRESET_INFO;

typedef struct tagControlBarInfo
{
    std::string type;
    std::string title;
    std::string iconPath;
    std::string disabledDS;
    std::vector<CONTROLBAR_ITEM_INFO> cbitems;
    std::vector<CONTROLBAR_PRESET_INFO>	  cbpresets;

} CONTROLBAR_INFO, *LPCONTROLBAR_INFO;

typedef struct tagGraphicGroup {

    std::string title;
    std::string xAxisTitle;

    std::string yLeftAxisTitle;
    std::string yRightAxisTitle;

    std::string yLeftAxisLblFormat;
    std::string yRightAxisLblFormat;

    std::pair<double, double> yLeftAxisRange;
    std::pair<double, double> yRightAxisRange;

    std::vector<std::string> dslya;
    std::vector<std::string> dsrya;

    std::vector<int> lt;
    std::vector<int> lw;
    std::vector<std::string> lp;

}GRAPHIC_GROUP, *LPGRAPHIC_GROUP;

typedef struct tagCheckPoint {
    std::string title;
    std::string ds;

    int lt;
    int lw;
    std::string lp;
    int titlepos;
}CHECKPOINTINFO, *LPCHECKPOINTINFO;

typedef struct tagRegionInfo
{
    tagRegionInfo() : pDS(NULL), pTimer(NULL) {};

    struct tagColor
    {
        tagColor() : from(0x000000), to (0x000000) {};

        COLORREF from;
        COLORREF to;
    }color;

    struct tagOpacity{
        tagOpacity () : from(0.0), to(0.0) {};
        double from;
        double to;
    }opacity;

    struct tagHatch{
        tagHatch() : type(-1), color(0x000000){};
        int type;
        COLORREF color;
    } hatch;

    struct tagBorder{
        tagBorder() : type (1), width(1), color(0x000000) {};
        int type;
        int width;
        std::string pat;
        COLORREF color;
    } border;

    struct tagTitle{
        tagTitle() : text(""), color(0xFFFFFF), fontname("Segue UI"), fontsize(28) {};
        std::string text;
        COLORREF color;
        std::string fontname;
        int fontsize;
    } title;

    bool vertical;
    DataSrc* pDS;
    BaseTimer* pTimer;

}REGIONINFO, *LPREGIONINFO;
typedef const REGIONINFO* LPCREGIONINFO;

class DrawItemStyle;
class DrawItemFont;

class CFSDoc : public CDocument
{
protected: // create from serialization only

    CFSDoc();
    DECLARE_DYNCREATE(CFSDoc)

    // Attributes
public:

    // Operations
public:

    // Overrides
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

    BOOL Rebuild();
    BOOL FromXML(const char* pszFileName);

    // Implementation
public:
    virtual ~CFSDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    void BuildDefaultsAttributes(IXMLDOMNode* pMainNode);

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()

public:

    std::string GetDefault(const char* pszName) const;

    void EnumDescribedControlBars(std::vector<CONTROLBAR_INFO>& bi)
    {
        bi.assign( bars.begin(), bars.end() );

    }

    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

    // [1/5/2012 Johnny A. Matveichik]

    //получить число слоев
    int GetLayerCount() const { return (int)layers.size(); };

    const std::list<FSLayer*>& GetLayersList() const { return layers; };
    //получить слой по номеру z-индекса
    //FSLayer* GetLayer(unsigned short zIndex) { return layers.at(zIndex); }

    //показать/скрыть слой
    void ShowLayer(unsigned short zIndex, BOOL bShow = FALSE);

    //заморозить/разморозить слой
    //(запретить / разрешить редактирование элементов слоя)
    void FreezeLayer(unsigned short zIndex, BOOL bFreeze = TRUE);

    // [1/5/2012 Johnny A. Matveichik]

    //Gdiplus::Image* GetBackgroundImage() const { return m_imgBG; }

    //int ItemsCount() const { return (int)items.size(); }
    //FSDocItem* GetItem(int ind) const { return items[ind]; }

    CDrawObj* HitTest(CPoint& pt) const;

    bool IsDebugMode() const { return bDebugMode; };

    CRect GetMessageListPlace() const { return rcMsgList; };

    void UpdateFSDocTitle();

    HICON GetDocumentIcon() const { return m_hDocIcon; };

    ULONG GetLMBHoldInterval() const { return m_lLBMouseHoldInterval;};
    ULONG GetUpdateInterval() const { return m_lUpdateInterval;};

    void AddItem(CDrawObj* pDO);

    CDrawObjList* GetItems() { return &m_docItems; }

    void GetDSGroup(std::string name, LPGRAPHIC_GROUP grGroup) const;

    std::vector<REGIONINFO> GetRegions() const
    {
        return regions;
    }

    LPCREGIONINFO GetRegion(const BaseTimer* pTimer) const
    {
        for (std::vector<REGIONINFO>::const_iterator it = regions.begin(); it != regions.end(); ++it)
        {
            if ( it->pTimer == pTimer )
            return &(*it);
        }
        return NULL;
    }

    std::string DSPrefix() const
    {
        return dspref;
    }

    Gdiplus::RectF GetFirstViewRect() const;

private:

    HICON m_hDocIcon ;

    CString m_strEmmebedDocTitle;

    ImagesStrip	ti;				//Список изображений для отображения тенденции
    //изменения аналоговых величин

    bool		bDebugMode;		//Отображение документа в режиме отладки

    ULONG		m_lLBMouseHoldInterval;	//время удержания кнопки мыши
    ULONG		m_lUpdateInterval;		//интервал обновления окна

    CRect	rcMsgList;

    //префикс для внутренних переменных
    std::string dspref;

    //std::vector<FSDocItem*> items;
    std::vector<CONTROLBAR_INFO> bars;
    std::vector<GRAPHIC_GROUP> grGroups;
    std::vector<REGIONINFO>    regions;

    //список слоев схемы
    std::list<FSLayer*> layers;

    //значения поумолчанию для документа
    std::map<std::string, std::string> defaults;

    //список стилей документа
    std::map<std::string, DrawItemStyle*> styles;

    //список кистей
    std::map<std::string, Gdiplus::Brush*> brushes;

    //список перьев
    std::map<std::string, Gdiplus::Pen*> pens;

    //список шрифтов
    std::map<std::string, DrawItemFont*> fonts;

    //список элементов документа
    CDrawObjList m_docItems;

public:

    //добавление именованного стиля
    bool AddStyle(std::string name, const DrawItemStyle& dis);
    //добавить именованую кисть к документу
    bool AddBrush(std::string name, const Gdiplus::Brush& brush);
    //добавить именованное перо к документу
    bool AddPen(std::string name, const Gdiplus::Pen& brush);
    //добавление именногванного шрифта
    bool AddFont(std::string name, const DrawItemFont& font);

    //получение стиля по имени
    DrawItemStyle* GetStyle(std::string name) const;
    //получение кисти по имени
    Gdiplus::Brush* GetBrush(std::string name) const;
    //получение пера по имени
    Gdiplus::Pen* GetPen(std::string name) const;
    //получение шрифта по имени
    DrawItemFont* GetFont(std::string name) const;

public:

    virtual void DeleteContents();
    virtual void OnCloseDocument();

protected:

    void OnAnalogDSChanged(const AnalogOutputDataSrc* pADS);
    void OnDiscreteDSChanged(const DiscreteDataSrc* pDDS);

    void OnTimerStart(const BaseTimer* pTimer);
    void OnTimerStop(const BaseTimer* pTimer);
    void OnTimerOverflowed(const BaseTimer* pTimer);

};

class DrawItemFont
{
public:

    DrawItemFont(const CFSDoc* doc);
    virtual ~DrawItemFont();

    //создание шрифта из XML узла
    virtual BOOL CreateFromXML(IXMLDOMNode* pNode);

    //Создание копии
    virtual DrawItemFont* Clone() const;

public:

    //Шрифт
    const Gdiplus::Font* GetFont() const;

    // Выравнивание текста
    const Gdiplus::StringFormat& GetStringFormat() const;

    // Заполнение
    const Gdiplus::Brush*	GetFontBrush() const;

    //Стиль шрифта
    const Gdiplus::FontStyle& GetFontStyle() const;

protected:

    //Шрифт
    Gdiplus::Font* font;

    // Выравнивание текста
    Gdiplus::StringFormat sf;

    // Шрифт цвет
    Gdiplus::Brush* fontbrush;

    // Стиль шрифта
    Gdiplus::FontStyle fontstyle;

    // Угол наклона
    Gdiplus::REAL ang;

private:

    const CFSDoc* parent;
};

class DrawItemStyle
{
public:

    DrawItemStyle(const CFSDoc* parent);
    virtual ~DrawItemStyle(void);

public:
    //создание стиля из XML узла
    virtual BOOL CreateFromXML(IXMLDOMNode* pNode);

    //Заливка
    std::vector<Gdiplus::Brush*> GetFills() const;

    //Обводка
    std::vector<Gdiplus::Pen*> GetStrokes() const;

    //шрифт
    const DrawItemFont* DrawItemStyle::GetFont() const;

    DrawItemStyle* Clone() const;

protected:

    //создание кисти из XML узла
    static Gdiplus::Brush* BrushFromNode(IXMLDOMNode* pNode);

    //создание пера из XML узла
    static Gdiplus::Pen* PenFromNode(IXMLDOMNode* pNode);

    //создание шрифта из XML узла
    static Gdiplus::Pen* FontFromNode(IXMLDOMNode* pNode);

    //оичcтить стиль
    void Clear();

protected:

    //Заливка
    std::vector<Gdiplus::Brush*> brushes;

    //Обводка
    std::vector<Gdiplus::Pen*> pens;

    //Шрифт
    DrawItemFont* font;

private:

    //родительский документ
    const CFSDoc* parent;
};
