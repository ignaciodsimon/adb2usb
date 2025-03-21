/* Copyright (c) 2011, Peter Barrett
 * Copyright (c) 2017-2018, Alexander Pruss
**
** Permission to use, copy, modify, and/or distribute this software for
** any purpose with or without fee is hereby granted, provided that the
** above copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
** SOFTWARE.
*/

#include "USBComposite.h" 

#include <string.h>
#include <stdint.h>
#include <libmaple/nvic.h>
#include "usb_hid.h"
#include "usb_composite_serial.h"
#include "usb_generic.h"
#include <libmaple/usb.h>
#include <string.h>
#include <libmaple/iwdg.h>

#include <wirish.h>

/*
 * USB HID interface
 */

bool USBHID::init(USBHID* me) {
    usb_hid_setTXEPSize(me->txPacketSize);
	return true;
}

bool USBHID::registerComponent() {
	return USBComposite.add(&usbHIDPart, this, (USBPartInitializer)&USBHID::init);
}

void USBHID::setReportDescriptor(const uint8_t* report_descriptor, uint16_t report_descriptor_length) {
	usb_hid_set_report_descriptor(report_descriptor, report_descriptor_length);
}

void USBHID::setReportDescriptor(const HIDReportDescriptor* report) {
    setReportDescriptor(report->descriptor, report->length);
}

void USBHID::begin(const uint8_t* report_descriptor, uint16_t report_descriptor_length) {
            
	if (enabledHID)
		return;
	
	setReportDescriptor(report_descriptor, report_descriptor_length);
	
	USBComposite.clear();
	registerComponent();

	USBComposite.begin(); 
	
	enabledHID = true;
}

// Joe: Added by me
void USBHID::begin(const HIDReportDescriptor* reportDescriptor,
                   uint16 vendorID=0, uint16 productID=0, const char* manufacturerString=NULL,
                   const char* productString=NULL, const char* serialString="00000000000000000001")
    {
    if (enabledHID)
        return;
    
    setReportDescriptor(reportDescriptor->descriptor, reportDescriptor->length);
    
    USBComposite.clear();
    registerComponent();

    USBComposite.setVendorId(vendorID);
    USBComposite.setProductId(productID);
    USBComposite.setManufacturerString(manufacturerString);
    USBComposite.setProductString(productString);
    USBComposite.setSerialString(serialString);

    USBComposite.begin(); 
    
    enabledHID = true;
}

void USBHID::begin(const HIDReportDescriptor* report) {
    begin(report->descriptor, report->length);
}

void USBHID::setBuffers(uint8_t type, volatile HIDBuffer_t* fb, int count) {
    usb_hid_set_buffers(type, fb, count);
}

bool USBHID::addBuffer(uint8_t type, volatile HIDBuffer_t* buffer) {
    return 0 != usb_hid_add_buffer(type, buffer);
}

void USBHID::clearBuffers(uint8_t type) {
	usb_hid_clear_buffers(type);
}

void USBHID::clearBuffers() {
	clearBuffers(HID_REPORT_TYPE_OUTPUT);
	clearBuffers(HID_REPORT_TYPE_FEATURE);
}

void USBHID::end(void){
	if(enabledHID) {
		USBComposite.end();
		enabledHID = false;
	}
}

void USBHID::begin(USBCompositeSerial serial, const uint8_t* report_descriptor, uint16_t report_descriptor_length) {	
	USBComposite.clear();

	setReportDescriptor(report_descriptor, report_descriptor_length);
	registerComponent();

	serial.registerComponent();

	USBComposite.begin();
}
		
void USBHID::begin(USBCompositeSerial serial, const HIDReportDescriptor* report) {
    begin(serial, report->descriptor, report->length);
}

void HIDReporter::sendReport() {
//    while (usb_is_transmitting() != 0) {
//    }

    unsigned toSend = bufferSize;
    uint8* b = buffer;
    
    while (toSend) {
        unsigned delta = usb_hid_tx(b, toSend);
        toSend -= delta;
        b += delta;
    }
    
//    while (usb_is_transmitting() != 0) {
//    }

    /* flush out to avoid having the pc wait for more data */
    usb_hid_tx(NULL, 0);
}
        
HIDReporter::HIDReporter(USBHID& _HID, uint8_t* _buffer, unsigned _size, uint8_t _reportID) : HID(_HID) {
    if (_reportID == 0) {
        buffer = _buffer+1;
        bufferSize = _size-1;
    }
    else {
        buffer = _buffer;
        bufferSize = _size;
    }
    memset(buffer, 0, bufferSize);
    reportID = _reportID;
    if (_size > 0 && reportID != 0)
        buffer[0] = _reportID;
}

HIDReporter::HIDReporter(USBHID& _HID, uint8_t* _buffer, unsigned _size) : HID(_HID) {
    buffer = _buffer;
    bufferSize = _size;
    memset(buffer, 0, _size);
    reportID = 0;
}

void HIDReporter::setFeature(uint8_t* in) {
    return usb_hid_set_feature(reportID, in);
}

uint16_t HIDReporter::getData(uint8_t type, uint8_t* out, uint8_t poll) {
    return usb_hid_get_data(type, reportID, out, poll);
}

uint16_t HIDReporter::getFeature(uint8_t* out, uint8_t poll) {
    return usb_hid_get_data(HID_REPORT_TYPE_FEATURE, reportID, out, poll);
}

uint16_t HIDReporter::getOutput(uint8_t* out, uint8_t poll) {
    return usb_hid_get_data(HID_REPORT_TYPE_OUTPUT, reportID, out, poll);
}

