#include "RenderHelp.h"
#include <wincodec.h>
#include <iostream>

// 외부 라이브러리는 속성-링커-입력-추가-종속성에 추가하거나
// 이렇게 명시적으로 라이브러리 경로를 적어줌
#pragma comment(lib, "windowscodecs.lib")  // WIC 라이브러리
#pragma comment(lib, "msimg32.lib")        // AlphaBlend 함수가 포함된 라이브러리

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

namespace renderHelp
{
    // 여기서만 사용하려 여기서 선언
    struct WICInitializer
    {
        WICInitializer() = default;

        ~WICInitializer()
        {
        }

        bool Initialize() {     
            //Co 는 COM 객체
            m_LastError = CoCreateInstance(
                CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pFactory)); // 팩토리 받아오기

            if (FAILED(m_LastError)) // FAILED 매크로로 작업 완료 확인
            {
                m_pFactory = nullptr;
                return false;
            }
            return true;
        }



        //filename을 받아 hBitmap을 내보냄
        bool LoadImageFromFile(__in LPCWSTR filename, __out HBITMAP& hBitmap) {
            hBitmap = nullptr;
            if (m_pFactory == nullptr)       //또 검사
            {
                m_LastError = E_FAIL;
                return false;
            }


            m_LastError = m_pFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ,WICDecodeMetadataCacheOnLoad, &m_pDecoder); // 팩토리를 써 파일이름으로 디코더 받아오기
            //또 검사
            if (FAILED(m_LastError))
            {
                return false;
            }
            
            //이미지 로드를 위해서는 첫 프레임만 알면 됨 -> WIC(windows imaging component)에서는 이미지를 프레임 단위로 다루는 구조를 사용함
            //png,jpg,bmp 같은 경우 프레임이 차피 1개 밖에 없는 사진이다. 즉 첫 프레임이 전체 이미지가 된다.
            //gif,tiff 같은 경우는 각 프레임이 한 장의 이미지가 된다.
            //디코더에 파일명(주소)정보가 있기에 이미지를 가져올 수 있다
            m_LastError = m_pDecoder->GetFrame(0, &m_pFrame); // 디코더로 이미지의 첫 프레임 가져오기
            //또 검사
            if (FAILED(m_LastError))
            {
                return false;
            }

            m_LastError = m_pFactory->CreateFormatConverter(&m_pConverter); // 팩토리를 써 컨버터 가져오기 
            //또 검사
            if (FAILED(m_LastError))
            {
                return false;
            }
            // 이미지 로드에 필요한 도구 가져옴

            m_LastError = m_pConverter->Initialize(m_pFrame, GUID_WICPixelFormat32bppPBGRA,WICBitmapDitherTypeNone, nullptr, 0.0f,WICBitmapPaletteTypeCustom); // 컨버터 초기화 때 png 정보를 넘김(m_pFrame)

            if (FAILED(m_LastError))
            {
                return false;
            }

            UINT width = 0, height = 0;

            m_LastError = m_pFrame->GetSize(&width, &height);

            if (FAILED(m_LastError))
            {
                return false;
            }

            //비트맵 방식에도 DIB랑 DDB 가 있음
            //DDB는 특정 디바이스 (화면, GPU)에 종속된 비트 맵으로  일반적인 더블 버퍼링 용 백버퍼에 쓰인다.
            //DIB는 장치(디바이스)에 독립적인 비트맵으로 픽셀 데이터가 (BMP 표준 포맷)으로 저장됨으로 픽셀 직접 접근 / 조작이 필요할 때는 이를 사용한다.

            //png를 DIB로 컨버팅하기 위한 DIB 섹션을 만들기
            BITMAPINFO bmi = { 0 };
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = width; // 비트맵 너비
            bmi.bmiHeader.biHeight = -static_cast<LONG>(height); // top-down DIB
            bmi.bmiHeader.biPlanes = 1; // 비트맵 평면 수
            bmi.bmiHeader.biBitCount = 32; // 32bpp  => rgba  각각 8bit
            bmi.bmiHeader.biCompression = BI_RGB; // 압축 없음

            void* pvImageBits = nullptr;
            HDC hdc = GetDC(nullptr);
            
            // bmi의 정보들 통해 DIB 섹션(DIB 객체 + 픽셀 메모리 포인터 제공)을 생성 그 픽셀 메모리 주소를 pvImageBits에 돌려줌 추가로 DIB 섹션을 다루는 핸들을 반환
            hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvImageBits, nullptr, 0);
            ReleaseDC(nullptr, hdc);

            if (FAILED(m_LastError))
            {
                return false;
            }

            // 
            // 비트맵에 픽셀 복사 (32bppPBGRA로 변환)
            // 컨버터가 가진 현재 이미지 프레임의 픽셀 데이터를 준비한 DIB 섹션의 픽셀 퍼버(pvImageBits)로 복사
            m_LastError = m_pConverter->CopyPixels(nullptr, width * 4, width * height * 4, (BYTE*)pvImageBits);

            if (FAILED(m_LastError))
            {
                return false;
            }

            return true;

        }

        //최종단계에서 에러 유무 확인
        HRESULT GetLastError() const
        {
            return m_LastError;
        }

        //받아온 COM객체 제거 요청
        void Clean()
        {
            if (m_pDecoder) m_pDecoder->Release();
            if (m_pFrame) m_pFrame->Release();
            if (m_pConverter) m_pConverter->Release();
        }

        //
        BitmapInfo* CreateBitmapInfo(HBITMAP hBitmap)
        {
            BitmapInfo* pNewBitmap = new BitmapInfo(hBitmap);

            return pNewBitmap;
        }

        //에러처리 일일이 하기 귀찮으니
        //동작 단위로 S_OK로 작업이 완료된 것을 확인
        HRESULT m_LastError = S_OK; 


        //API 사용 방법
        IWICImagingFactory* m_pFactory = nullptr;
        IWICBitmapDecoder* m_pDecoder = nullptr;
        IWICBitmapFrameDecode* m_pFrame = nullptr;
        IWICFormatConverter* m_pConverter = nullptr;

        // WICInitializer는 복사할 수 없도록 삭제합니다.
        WICInitializer(const WICInitializer&) = delete;
        WICInitializer& operator=(const WICInitializer&) = delete;
    }GWICInitializer;  // WICInitializer GWICInitializer 와 같은 효과 (전역변수 선언임...?)


    //외부에서는 filename으로 생성
    BitmapInfo* CreateBitmapInfo(LPCWSTR filename)
    {
        static bool bCoInit = GWICInitializer.Initialize();
        if (false == bCoInit)
        {
            return nullptr;
        }

        HBITMAP hBitmap = nullptr;
        BitmapInfo* pBitmapInfo = nullptr;
        if (GWICInitializer.LoadImageFromFile(filename, hBitmap))
        {
            pBitmapInfo = GWICInitializer.CreateBitmapInfo(hBitmap);
        }

        GWICInitializer.Clean();

        return pBitmapInfo;
    }
}
