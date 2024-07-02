// SPDX-License-Identifier: GPL-2.0
/*
 * HID support for Vivaldi Keyboard
 *
 * Copyright 2020 Google LLC.
 * Author: Sean O'Brien <seobrien@chromium.org>
 */

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/input/vivaldi-fmap.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "hid-vivaldi-common.h"

#define ITE_SET_REPORT 256

static int vivaldi_probe(struct hid_device *hdev,
			 const struct hid_device_id *id)
{
	struct vivaldi_data *drvdata;
	int ret, i;
	u8 *data;
	u16 set_report_size = 64;

	drvdata = devm_kzalloc(&hdev->dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	hid_set_drvdata(hdev, drvdata);

	ret = hid_parse(hdev);
	if (ret)
		return ret;

	// return hid_hw_start(hdev, HID_CONNECT_DEFAULT);

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret)
		return ret;

	/* ITE Debug - No function, just for test
         * Send SET_REPORT request
         */
	data = kzalloc(256, GFP_KERNEL);
	if (!data) {
		dev_warn(&hdev->dev,
			 "%s ITE Debug[%d] - failed to allocate memory",
			 __func__, __LINE__);
		return -ENOMEM;
	}
	data[0] = 0x01;
	for (i = 1; i < set_report_size; i++) {
		data[i] = i;
	}

	dev_warn(&hdev->dev, "%s ITE Debug[%d] - send SET_REPORT(%d bytes)\n",
		 __func__, __LINE__, set_report_size);
	ret = hid_hw_output_report(hdev, data, set_report_size);
	if (ret == -ENOSYS) {
		ret = hid_hw_raw_request(hdev, data[0], data, set_report_size,
					 HID_FEATURE_REPORT,
					 HID_REQ_SET_REPORT);
		if (ret < 0) {
			dev_warn(
				&hdev->dev,
				"%s ITE Debug[%d] - failed to send report, ret %d",
				__func__, __LINE__, ret);
		}
	}

	set_report_size = 256;
	for (i = set_report_size; i > 1; i--) {
		data[i] = i;
	}

	dev_warn(&hdev->dev, "%s ITE Debug[%d] - send SET_REPORT(%d bytes)\n",
		 __func__, __LINE__, set_report_size);
	ret = hid_hw_output_report(hdev, data, set_report_size);
	if (ret == -ENOSYS) {
		ret = hid_hw_raw_request(hdev, data[0], data, set_report_size,
					 HID_FEATURE_REPORT,
					 HID_REQ_SET_REPORT);
		if (ret < 0) {
			dev_warn(
				&hdev->dev,
				"%s ITE Debug[%d] - failed to send report, ret %d",
				__func__, __LINE__, ret);
		}
	}

	kfree(data);

	return 0;
}

static const struct hid_device_id vivaldi_table[] = {
	{ HID_DEVICE(HID_BUS_ANY, HID_GROUP_VIVALDI, HID_ANY_ID, HID_ANY_ID) },
	{ }
};

MODULE_DEVICE_TABLE(hid, vivaldi_table);

static struct hid_driver hid_vivaldi = {
	.name = "hid-vivaldi",
	.id_table = vivaldi_table,
	.probe = vivaldi_probe,
	.feature_mapping = vivaldi_feature_mapping,
	.driver = {
		.dev_groups = vivaldi_attribute_groups,
	},
};

module_hid_driver(hid_vivaldi);

MODULE_AUTHOR("Sean O'Brien");
MODULE_DESCRIPTION("HID vivaldi driver");
MODULE_LICENSE("GPL");
