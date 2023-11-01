#if !defined(AFX_OMEGA_DEVICES_H_INCLUDED_)
#define AFX_OMEGA_DEVICES_H_INCLUDED_

#include <map>
#include <string>
#include "BaseFlowController.h"


typedef struct tagOMEGA_MFC_GAS
{
	unsigned char  id;
	const char* szFullName;
	const char* szShortName;
	double Viscosity;
	double Density;
	double Compressebility;

} OMEGA_MFC_GAS_INFO, *LP_OMEGA_MFC_GAS_INFO;

typedef const OMEGA_MFC_GAS_INFO* LPC_OMEGA_MFC_GAS_INFO;

enum e_tGasID{ 
	GAS_AIR = 0,
	GAS_AR,
	GAS_CH4,
	GAS_CO,
	GAS_CO2,
	GAS_C2H6,
	GAS_H2,
	GAS_HE,
	GAS_N2,
	GAS_N2O,
	GAS_NE,
	GAS_O2,
	GAS_C3H8,
	GAS_NORMAL_C4H10,
	GAS_C2H2,
	GAS_C2H4,
	GAS_ISO_C2H10,
	GAS_KR,
	GAS_XE,
	GAS_SF6,
	GAS_C_25,
	GAS_C_10,
	GAS_C_8,
	GAS_C_2,
	GAS_C_75,
	GAS_A_75,
	GAS_A_25,
	GAS_A1025,
	GAS_STAR29,
	GAS_P_5,
	GAS_UNKNOWN
};

//���������� � �������������� �����
static const  OMEGA_MFC_GAS_INFO gases[] =
{
	{0, "Air", "Air"								    , 184.918, 1.1840, 0.9997	},
	{1, "Argon", "Ar"									, 225.593, 1.6339, 0.9994	},
	{2, "Methane", "CH4"								, 111.852, 0.6569, 0.9982	},
	{3, "Carbon Monoxide", "CO"							, 176.473, 1.1453, 0.9997	},
	{4, "Carbon Dioxide", "CO2"							, 149.332, 1.8080, 0.9949	},
	{5, "Ethane", "C2H6"								, 93.540, 1.2385, 0.9924	},
	{6, "Hydrogen", "H2"								, 89.153, 0.08235, 1.0006	},
	{7, "Helium", "He"									, 198.457, 0.16353, 1.0005	},
	{8, "Nitrogen", "N2"								, 178.120, 1.1453, 0.9998	},
	{9, "Nitrous", "Oxide N2O"							, 148.456, 1.8088, 0.9946	},
	{10, "Neon", "Ne"									, 311.149, 0.8246, 1.0005	},
	{11, "Oxygen", "O2"									, 204.591, 1.3088, 0.9994	},
	{12, "Propane", "C3H8"								, 81.458, 1.8316, 0.9841	},
	{13, "normal-Butane", "n-C4H10"						, 74.052, 2.4494, 0.9699	},
	{14, "Acetylene", "C2H2"							, 104.448, 1.0720, 0.9928	},
	{15, "Ethylene", "C2H4"								, 103.177, 1.1533, 0.9943	},
	{16, "iso-Butane", "i-C2H10"						, 74.988, 2.4403, 0.9728	},
	{17, "Krypton", "Kr"								, 251.342, 3.4274, 0.9994	},
	{18, "Xenon", "Xe"									, 229.785, 5.3954, 0.9947	},
	{19, "Sulfur Hexafluoride", "SF6"					, 153.532, 6.0380, 0.9887	},
	{20, "75% Argon / 25% CO2", "C-25"					, 205.615, 1.6766, 0.9987	},
	{21, "90% Argon / 10% CO2", "C-10"					, 217.529, 1.6509, 0.9991	},
	{22, "92% Argon / 8% CO2", "C-8"					, 219.134, 1.6475, 0.9992	},
	{23, "98% Argon / 2% CO2", "C-2"					, 223.973, 1.6373, 0.9993	},
	{24, "75% CO2 / 25% Argon", "C-75"					, 167.451, 1.7634, 0.9966	},
	{25, "75% Argon / 25% Helium", "A-75"				, 230.998, 1.2660, 0.9997	},
	{26, "75% Helium / 25% Argon", "A-25"				, 234.306, 0.5306, 1.0002	},
	{27, "90% Helium / 7.5% Argon / 2.5% CO2", "A1025"	, 214.840, 0.3146, 1.0003	},
	{28, "90% Argon / 8% CO2 / 2% Oxygen", "Star29"		, 218.817, 1.6410, 0.9992	},
	{29, "95% Argon / 5% Methane", "P-5"				, 223.483, 1.5850, 0.9993	}
};

//������������ ������� �� �������� ����� 2600
const std::map< std::string, double >::value_type dataOmega[] = 
{
 	std::pair<std::string, double>("FMA-2620A", 1.0),
 	std::pair<std::string, double>("FMA-2605A", 2.0),
 	std::pair<std::string, double>("FMA-2606A", 5.0),
 	std::pair<std::string, double>("FMA-2607A", 10.0),
 	std::pair<std::string, double>("FMA-2608A", 20.0),
 	std::pair<std::string, double>("FMA-2609A", 50.0),
 	std::pair<std::string, double>("FMA-2610A", 100.0),
 	std::pair<std::string, double>("FMA-2611A", 250.0),
 	std::pair<std::string, double>("FMA-2612A", 500.0),
 	std::pair<std::string, double>("FMA-2613A", 1000.0),
 	std::pair<std::string, double>("FMA-2621A", 1500.0)

};

const unsigned int MFC_MODELS_COUNT = sizeof(dataOmega)/sizeof(dataOmega[0]);
const unsigned int MAX_VAL_IN_DISCRETE = 64000;
const unsigned int MIN_VAL_IN_DISCRETE = 0;



class OmegaMFC : public BaseFlowController, public IAnalogInput, public IDigitalInput/*, public XMLObject<OmegaMFC>*/
{

public:

	OmegaMFC();
	virtual ~OmegaMFC(void);


	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	
	static OmegaMFC* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

public:

	//////////////////////////////////////////////////////////////////////////
	//���������� ���������� IAnalogInput
   
	//��������� ������ ����� ���������� ������
	virtual int GetChannelsCountAI() const { return  INPUT_ANALOG_CHANNELS_COUNT; };

	//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
	virtual void	ReadChannels(double* data  = NULL) const;

	//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
	virtual double  ReadChannel(unsigned char channel)  const;


	// ��������� ������ �� ������ ����� ���������� ����������
	// ��� ������ �� ���������� ������ (������ �� ������ ������ �� �����������)
	// channel = OMFC_CHANNEL_PRESSURE		= 0; //����� ����������� ����� ��� ��������� �������� ����( in PSIA) 	
	// channel = OMFC_CHANNEL_TEMPERATURE	= 1; //����� ����������� ����� ��� ��������� ����������� ���� ( in �C)
	// channel = OMFC_CHANNEL_VOL_FLOWRATE	= 2; //����� ����������� ����� ��� ��������� ��������� ������� ����
												 //(� �������� ��������� �� ������ ������� � ������������ �� �������)
	// channel = OMFC_CHANNEL_MASS_FLOWRATE	= 3; //����� ����������� ����� ��� ��������� ��������� ������� ���� 
												 //(� �������� ��������� �� ������ ������� � ������������ �� �������)	
	// channel = OMFC_AI_CHANNEL_SET_POINT	= 4; //����� ����������� ����� ��� ��������� ��������� �� ������� ������� ������� ���� 
												 //(� �������� ��������� �� ������ ������� � ������������ �� �������)
	// channel =  OMFC_AIC_NOMINAL_FLOW		= 5  //����� ����������� ����� ��� ��������� ������������� ������� ���� 

	virtual double  GetChannelData(unsigned char channel)  const;


	//��������  ���������� �� ���������� ��������� ������
	virtual void GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� �����
	virtual bool SetInputRange(unsigned char ch, unsigned char range);

	//�������� ��� ���������  ���������  ���������
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;



	virtual bool OnFailure();

	//////////////////////////////////////////////////////////////////////////
	//���������� ���������� IAnalogOutput

	//��������� ������ ����� ���������� ������� 
	virtual int GetChannelsCountAO() const { return OUTPUT_ANALOG_CHANNELS_COUNT; };

	//�������� ������ � ���������� ����� 
	//������������� ����� �������� ��������� ������� 
	virtual double  WriteToChannel(unsigned char channel, double val);

	//�������� ������ �� ��� ������
	virtual void WriteToAllChannels(const std::vector<double>& vals) ; 

	//�������� ��������� ���������� � ���������� ����� �������� 
	virtual double GetChannelStateAO (unsigned char channel) const;  

	//�������� �������� �������� ������ 
	virtual void GetMinMax(unsigned char channel, double& minOut, double& maxOut) const
	{
		minOut = 0.0;
		maxOut = pLastData[OMFC_AIC_NOMINAL_FLOW];
	}

	//��������  ���������� �� ��������� ������
	virtual void GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� ������
	virtual bool SetOutputRange(unsigned char ch, unsigned char range);

	//�������� ��� ���������  ���������  
	virtual void EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;


	//////////////////////////////////////////////////////////////////////////
	//���������� ���������� IDigitalInput
	//��������� ������ ����� ���������� ������
	virtual int GetChannelsCountDI() const { return INPUT_DIGITAL_CHANNELS_COUNT; };

	//�������� ��������� ���������� ������	
	virtual bool UpdateChannelsStateDI() const;

	//��������� ��������� ������ �� ���������� ������	
	virtual bool  GetChannelStateDI(unsigned char channel, bool update)  const;

	
	//////////////////////////////////////////////////////////////
	/// ���������� �����

	enum tagOmegaMFCAnalogInputChannels : unsigned char {
		//����� ����������� ����� ��� ��������� �������� ����( in PSIA)
		OMFC_AIC_PRESSURE		= 0,
		//����� ����������� ����� ��� ��������� ����������� ���� ( in �C)
		OMFC_AIC_TEMPERATURE	= 1,
		//����� ����������� ����� ��� ��������� ��������� ������� ����
		//(� �������� ��������� �� ������ ������� � ������������ �� �������)
		OMFC_AIC_VOL_FLOWRATE	= 2,
		//����� ����������� ����� ��� ��������� ��������� ������� ���� 
		//(� �������� ��������� �� ������ ������� � ������������ �� �������)
		OMFC_AIC_MASS_FLOWRATE	= 3,
		//����� ����������� ����� ��� ��������� ��������� �� ������� ������� ������� ���� 
		//(� �������� ��������� �� ������ ������� � ������������ �� �������)
		OMFC_AIC_SET_POINT		= 4,
		//����� ����������� ����� ��� ��������� ������������� ������� ���� 		
		OMFC_AIC_NOMINAL_FLOW	= 5
	};

	//////////////////////////////////////////////////////////////
	/// �������� �����

	enum tagOmegaMFCDigitalInputChannels : unsigned char{
		//�������� ���� - ������ ���� ����� 
		OMFC_DI_CHANNEL_SET_POINT_NOT_NULL	= 0,
		//�������� ���� - ������������ ������ ���� 
		OMFC_DI_CHANNEL_FLOW_NOT_NULL		= 1,
		//�������� ���� - ������������ ���������� ��������� � ���������������� ������� ���� 
		OMFC_DI_CHANNEL_FLOW_DEVIATION		= 2
	};

	
	//////////////////////////////////////////////////////////////////////////
	//���������� ����������� �������
	void ControledGas( OMEGA_MFC_GAS_INFO& gasInfo) const;	

	virtual void FlowSet(double newFlow);

	virtual double FlowGet() const { return GetChannelData (OMFC_AIC_MASS_FLOWRATE); }


	LPC_OMEGA_MFC_GAS_INFO lpGas;

protected:

	virtual bool Create(std::string name, std::string model, unsigned long updateInterval, std::string dataProviderId, char address, std::string gas);

	//������� ���, ������ �������� ����� ����������
	bool SelectGas(int gasIndex);

	static LPC_OMEGA_MFC_GAS_INFO GetGasInfo(std::string name);

	virtual void OnExit();

protected:

	//������������ ������ �� ������� ������������ �������
	static std::map< std::string, double > omegaMaxFlowByModel;

	//����� ���������� ������� �������
	const int INPUT_ANALOG_CHANNELS_COUNT;

	//����� ���������� �������� �������
	const int OUTPUT_ANALOG_CHANNELS_COUNT;

	//����� �������� ������� �������
	const int INPUT_DIGITAL_CHANNELS_COUNT;

	//��������������� ���������� RS-485 � RS-232 ��� �������� ������ 
	IDataProvider* pDataProvider;
	
	//
	int curGasIndex;
	
	double* pLastData;

	double critDev; // = 5.0 %

	//����� ����������� �������� �������� "A-Z"
	char mfcAdress;

	mutable std::string strActualGas;

	//���������� � ���������� ����������
	std::vector<MEASSURE_RANGE_INFO> rangesAI;

	//���������� � ���������� ����������
	std::vector<MEASSURE_RANGE_INFO> rangesAO;

protected:

	virtual void ControlProc();

};

#endif //AFX_OMEGA_DEVICES_H_INCLUDED_