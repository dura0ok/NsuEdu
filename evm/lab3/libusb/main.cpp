#include <iostream>
#include <libusb-1.0/libusb.h>

using namespace std;

string getStringForEnum(int enumVal){
	switch (enumVal) {
	case libusb_class_code::LIBUSB_CLASS_PER_INTERFACE: return "All interfaces operate independently";
	case libusb_class_code::LIBUSB_CLASS_AUDIO: return "Audio";
	case libusb_class_code::LIBUSB_CLASS_COMM: return "Communications";
	case libusb_class_code::LIBUSB_CLASS_HID: return "Human Interface Device";
	case libusb_class_code::LIBUSB_CLASS_PHYSICAL: return "Physical";
	case libusb_class_code::LIBUSB_CLASS_PRINTER: return "Printer";
	case libusb_class_code::LIBUSB_CLASS_PTP: return "Image";
	case libusb_class_code::LIBUSB_CLASS_MASS_STORAGE: return "Mass storage";
	case libusb_class_code::LIBUSB_CLASS_HUB: return "Hub";
	case libusb_class_code::LIBUSB_CLASS_DATA: return "Data";
	case libusb_class_code::LIBUSB_CLASS_SMART_CARD: return "Smart Card";
	case libusb_class_code::LIBUSB_CLASS_CONTENT_SECURITY: return "Content Security";
	case libusb_class_code::LIBUSB_CLASS_VIDEO: return "Video";
	case libusb_class_code::LIBUSB_CLASS_PERSONAL_HEALTHCARE: return "Per-sonal Healthcare";
	case libusb_class_code::LIBUSB_CLASS_DIAGNOSTIC_DEVICE: return "Diagnostic Device";
	case libusb_class_code::LIBUSB_CLASS_WIRELESS: return "Wireless";
	case libusb_class_code::LIBUSB_CLASS_APPLICATION: return "Application";
	case libusb_class_code::LIBUSB_CLASS_VENDOR_SPEC: return "Class is vendor-specific";
	default: return "Not recognized";
	}
}

void printDevConfig(libusb_device* p_device){
	int rc;
	libusb_device_descriptor desc{};
	libusb_config_descriptor *config{};

	if ((rc = libusb_get_device_descriptor(p_device, &desc)) < 0) {
		string exitCode = to_string(rc);
		throw logic_error("Initial error. Exit code:" + exitCode);
	}

	libusb_get_config_descriptor(p_device, 0, &config);
	cout << "Class : " << getStringForEnum((int)desc.bDeviceClass) << "(" << (int)desc.bDeviceClass << ")" << endl;
	cout << "Vendor : " << desc.idVendor << endl;
	cout << "Product : " << desc.idProduct << endl;

	libusb_device_handle *handle = nullptr;
	if (libusb_open(p_device, &handle)) {
		throw logic_error("Open error");
	}
	unsigned char data[BUFSIZ] = {'\0'};
	if (libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, data, sizeof(data))) {
		cout << "Serial number : " << data << endl;
	}

	unsigned char data1[BUFSIZ] = {'\0'};
	if (libusb_get_string_descriptor_ascii(handle, desc.iProduct, data1, sizeof(data1))) {
		cout << "iProduct : " << data1 << endl;
	}

	unsigned char data2[BUFSIZ] = {'\0'};
	if (libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, data2, sizeof(data2))) {
		cout << "iManufacturer : " << data2 << endl;
	}



	cout << "=========================" << endl;
}


int main(){
	libusb_device **devs;
	libusb_context *context = nullptr;
	int ret = libusb_init(&context);

	if(ret < 0){
		cerr << "libusb_init";
		exit(1);
	}

	size_t num = libusb_get_device_list(context, &devs);
	if (num == 0) {
		throw logic_error("List of devices is not received");
	}

	printf("There are %zu devices found\n", num);
	cout << "=========================" << endl;

	for (size_t i = 0; i < num; i++) {
		printDevConfig(devs[i]);
	}

	libusb_free_device_list(devs, 1);
	libusb_exit(context);
}
