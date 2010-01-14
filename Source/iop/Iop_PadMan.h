#ifndef _IOP_PADMAN_H_
#define _IOP_PADMAN_H_

#include "Iop_Module.h"
#include "Iop_SifMan.h"
#include "../PadListener.h"
#include <functional>
#include "zip/ZipArchiveWriter.h"
#include "zip/ZipArchiveReader.h"

//#define USE_EX

namespace Iop
{
	class CPadMan : public CModule, public CPadListener, public CSifModule
	{
	public:
                            CPadMan(CSifMan&);
        std::string         GetId() const;
        std::string         GetFunctionName(unsigned int) const;
        void                Invoke(CMIPS&, unsigned int);
        virtual bool        Invoke(uint32, uint32*, uint32, uint32*, uint32, uint8*);
        virtual void		SaveState(Framework::CZipArchiveWriter&);
        virtual void		LoadState(Framework::CZipArchiveReader&);
        virtual void		SetButtonState(unsigned int, PS2::CControllerInfo::BUTTON, bool, uint8*);
        virtual void        SetAxisState(unsigned int, PS2::CControllerInfo::BUTTON, uint8, uint8*);

		enum MODULE_ID
		{
			MODULE_ID_1 = 0x80000100,
			MODULE_ID_2 = 0x80000101,
			MODULE_ID_3 = 0x8000010F,
			MODULE_ID_4 = 0x0000011F,
		};

	private:
		struct PADDATAEXEX
		{
			uint8			nData[32];			//+0x00
			uint32			nReserved0;			//+0x20
			uint32			nReserved1;			//+0x24
			uint32			nReserved2;			//+0x28
			uint32			nReserved3;			//+0x2C
			uint8			nReserved4[0x20];	//+0x30
			uint16			nModeTable[4];		//+0x50
			uint32			nFrame;				//+0x58
			uint32			nReserved5;			//+0x5C
			uint32			nLength;			//+0x60
			uint8			nModeOk;			//+0x64
			uint8			nModeCurId;			//+0x65
			uint8			nReserved6[2];		//+0x66
			uint8			nNrOfModes;			//+0x68
			uint8			nModeCurOffset;		//+0x69
			uint8			nReserved7[0x6];	//+0x6A
			uint8			nState;				//+0x70
			uint8			nReqState;			//+0x71
			uint8			nOk;				//+0x72
			uint8			nReserved8;			//+0x73
			uint32			nReserved9[3];		//+0x74
		};
		BOOST_STATIC_ASSERT(sizeof(PADDATAEXEX) == 0x80);

		struct PADDATAEX
		{
			uint8			nData[32];
			uint32			nReserved1[4];
			uint8			nActuator[32];
			uint16			nModeTable[4];
			uint32			nFrame;
			uint32			nReserved2;
			uint32			nLength;
			uint8			nModeOk;
			uint8			nModeCurId;
			uint8			nReserved3[2];
			uint8			nNrOfModes;
			uint8			nModeCurOffset;
			uint8			nActuatorCount;
			uint8			nReserved4[5];
			uint8			nState;
			uint8			nReqState;
			uint8			nOk;
			uint8			nReserved5[13];
		};

		struct PADDATA
		{
			uint32			nFrame;
			uint8			nState;
			uint8			nReqState;
			uint8			nOk;
			uint8			nReserved0;
			uint8			nData[32];
			uint32			nLength;
			uint32			nReserved1[5];
		};

		class CPadDataInterface
		{
		public:
			virtual				~CPadDataInterface() {}
			virtual void 		SetData(unsigned int, uint8) = 0;
			virtual uint8		GetData(unsigned int) = 0;
			virtual void		SetFrame(unsigned int) = 0;
			virtual void		SetState(unsigned int) = 0;
			virtual void		SetReqState(unsigned int) = 0;
			virtual void		SetLength(unsigned int) = 0;
			virtual void		SetOk(unsigned int) = 0;
			virtual void		SetModeCurId(unsigned int) = 0;
			virtual void		SetModeCurOffset(unsigned int) = 0;
			virtual void		SetModeTable(unsigned int, unsigned int) = 0;
			virtual void		SetNumberOfModes(unsigned int) = 0;
		};

		template <typename T> class CPadDataHandler : public CPadDataInterface
		{
		public:
			CPadDataHandler(void* pPtr)
			{
				m_pPadData = reinterpret_cast<T*>(pPtr);
			}

			virtual ~CPadDataHandler()
			{

			}

			virtual uint8 GetData(unsigned int nIndex)
			{
				return m_pPadData->nData[nIndex];
			}

			virtual void SetData(unsigned int nIndex, uint8 nValue)
			{
				m_pPadData->nData[nIndex] = nValue;
			}

			virtual void SetFrame(unsigned int nValue)
			{
				m_pPadData->nFrame = nValue;
			}

			virtual void SetState(unsigned int nValue)
			{
				m_pPadData->nState = nValue;
			}

			virtual void SetReqState(unsigned int nValue)
			{
				m_pPadData->nReqState = nValue;
			}

			virtual void SetLength(unsigned int nValue)
			{
				m_pPadData->nLength = nValue;
			}

			virtual void SetOk(unsigned int nValue)
			{
				m_pPadData->nOk = nValue;
			}

			virtual void SetModeCurId(unsigned int nValue)
			{
				m_pPadData->nModeCurId = nValue;
			}

			virtual void SetModeCurOffset(unsigned int nValue)
			{
				m_pPadData->nModeCurOffset = nValue;
			}

			virtual void SetModeTable(unsigned int nIndex, unsigned int nValue)
			{
				m_pPadData->nModeTable[nIndex] = nValue;
			}

			virtual void SetNumberOfModes(unsigned int number)
			{
				m_pPadData->nNrOfModes = number;
			}

		private:
			T*		m_pPadData;
		};

        typedef std::tr1::function< void (CPadDataInterface*) > PadDataFunction;

		PADDATA*			m_pPad;

		unsigned int		m_nPadDataType;
		uint32				m_nPadDataAddress;

		void				Open(uint32*, uint32, uint32*, uint32, uint8*);
		void				SetActuatorAlign(uint32*, uint32, uint32*, uint32, uint8*);
		void				Init(uint32*, uint32, uint32*, uint32, uint8*);
		void				GetModuleVersion(uint32*, uint32, uint32*, uint32, uint8*);
		void				ExecutePadDataFunction(const PadDataFunction&, void*, size_t);

		static void			PDF_InitializeStruct0(CPadDataInterface*);
		static void			PDF_InitializeStruct1(CPadDataInterface*);
		static void			PDF_SetButtonState(CPadDataInterface*, PS2::CControllerInfo::BUTTON, bool);
        static void         PDF_SetAxisState(CPadDataInterface*, PS2::CControllerInfo::BUTTON, uint8);
	};

    template <> void CPadMan::CPadDataHandler<CPadMan::PADDATA>::SetModeCurId(unsigned int);
    template <> void CPadMan::CPadDataHandler<CPadMan::PADDATA>::SetModeCurOffset(unsigned int);
    template <> void CPadMan::CPadDataHandler<CPadMan::PADDATA>::SetModeTable(unsigned int, unsigned int);
	template <> void CPadMan::CPadDataHandler<CPadMan::PADDATA>::SetNumberOfModes(unsigned int);
}

#endif
