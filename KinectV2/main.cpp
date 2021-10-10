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

//�\������
std::vector<BYTE> colorBuffer;

void initialize() {
	//�f�t�H���g��Kinect���擾����
	ERROR_CHECK( ::GetDefaultKinectSensor(&kinect));
	ERROR_CHECK(kinect->Open());

	//�J���[���[�_�[���擾����
	CComPtr<IColorFrameSource>colorFrameSource;
	ERROR_CHECK(kinect->get_ColorFrameSource(&colorFrameSource));
	ERROR_CHECK(colorFrameSource->OpenReader(&colorFrameReader));

	//�J���[�摜�̃T�C�Y���擾����
	CComPtr<IFrameDescription>colorFrameDescription;
	ERROR_CHECK(colorFrameSource->CreateFrameDescription(ColorImageFormat::ColorImageFormat_Bgra, &colorFrameDescription));
	ERROR_CHECK(colorFrameDescription->get_Width(&colorWidth));
	ERROR_CHECK(colorFrameDescription->get_Height(&colorHeight));
	ERROR_CHECK(colorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel));

	//�o�b�t�@�[���쐬����
	colorBuffer.resize(colorWidth*colorHeight*colorBytesPerPixel);
}

void updateColorFrame() {
	//�t���[�����擾����
	CComPtr<IColorFrame>colorFrame;
	auto ret = colorFrameReader->AcquireLatestFrame(&colorFrame);
	if (FAILED(ret)) {
		return;
	}
	//�w��̌`���Ńf�[�^���擾����
	ERROR_CHECK(colorFrame->CopyConvertedFrameDataToArray(colorBuffer.size(), &colorBuffer[0], colorFormat));
}

void drawColorFrame() {
	//�J���[�f�[�^��\������
	cv::Mat colorImage(colorHeight, colorWidth, CV_8UC4, &colorBuffer[0]);
	cv::imshow("Color Image", colorImage);
}

void draw() {
	drawColorFrame();
}

//�f�[�^�̍X�V����
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
		//kinect�Z���T�[�Ɗ֘A�t����
		IKinectSensor*kinect = nullptr;
		ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
		
		//Kinect�Z���T�[�𓮍삳����
		ERROR_CHECK(kinect->Open());
		//kinect�Z���T�[�����������ǂ�����Ԃ��擾����
		BOOLEAN isOpen = false;
		ERROR_CHECK(kinect->get_IsOpen(&isOpen));
		std::cout << "Kinect is " << (isOpen ? "Open" : "Not Open") << std::endl;
		
		//�����҂�
		::Sleep(3000);

		//Kinect�Z���T�[�̓�����~�߂�
		kinect->Close();
		kinect->Release();
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
}