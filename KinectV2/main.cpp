#include <iostream>
#include <sstream>
#include <Kinect.h>
#include <opencv2\opencv.hpp>
#include <atlbase.h>
#define ERROR_CHECK(ret)\
	if((ret)!=S_OK){\
		std::stringstream ss;\
		ss<<"faild "#ret" "<<std::hex<<ret<<std::endl;\
		throw std::runtime_error(ss.str().c_str());\
	}

//Kinect SDK
CComPtr<IKinectSensor>kinect = nullptr;
CComPtr<IColorFrameReader>colorFrameReader = nullptr;
int colorWidth;
int colorHeight;
unsigned int colorBytesPerPixel;
ColorImageFormat colorFormat = ColorImageFormat::ColorImageFormat_Bgra;

//表示部分
std::vector<BYTE> colorBuffer;

void initialize() {
	//デフォルトのKinectを取得する
	ERROR_CHECK( ::GetDefaultKinectSensor(&kinect));
	ERROR_CHECK(kinect->Open());

	//カラーリーダーを取得する
	CComPtr<IColorFrameSource>colorFrameSource;
	ERROR_CHECK(kinect->get_ColorFrameSource(&colorFrameSource));
	ERROR_CHECK(colorFrameSource->OpenReader(&colorFrameReader));

	//カラー画像のサイズを取得する
	CComPtr<IFrameDescription>colorFrameDescription;
	ERROR_CHECK(colorFrameSource->CreateFrameDescription(ColorImageFormat::ColorImageFormat_Bgra, &colorFrameDescription));
	ERROR_CHECK(colorFrameDescription->get_Width(&colorWidth));
	ERROR_CHECK(colorFrameDescription->get_Height(&colorHeight));
	ERROR_CHECK(colorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel));

	//バッファーを作成する
	colorBuffer.resize(colorWidth*colorHeight*colorBytesPerPixel);
}

void updateColorFrame() {
	//フレームを取得する
	CComPtr<IColorFrame>colorFrame;
	auto ret = colorFrameReader->AcquireLatestFrame(&colorFrame);
	if (FAILED(ret)) {
		return;
	}
	//指定の形式でデータを取得する
	ERROR_CHECK(colorFrame->CopyConvertedFrameDataToArray(colorBuffer.size(), &colorBuffer[0], colorFormat));
}

void drawColorFrame() {
	//カラーデータを表示する
	cv::Mat colorImage(colorHeight, colorWidth, CV_8UC4, &colorBuffer[0]);
	cv::imshow("Color Image", colorImage);
}

void draw() {
	drawColorFrame();
}

//データの更新処理
void update() {
	updateColorFrame();
}

void run() {
	while (true) {
		update();
		draw();

		auto key = cv::waitKey(10);
		if (key == 'q')
		{
			break;
		}
	}
}

void main() {
	try {
		//kinectセンサーと関連付ける
		IKinectSensor*kinect = nullptr;
		ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
		
		//Kinectセンサーを動作させる
		ERROR_CHECK(kinect->Open());
		//kinectセンサーが動いたかどうか状態を取得する
		BOOLEAN isOpen = false;
		ERROR_CHECK(kinect->get_IsOpen(&isOpen));
		std::cout << "Kinect is " << (isOpen ? "Open" : "Not Open") << std::endl;
		
		//少し待つ
		::Sleep(3000);

		//Kinectセンサーの動作を止める
		kinect->Close();
		kinect->Release();
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
}