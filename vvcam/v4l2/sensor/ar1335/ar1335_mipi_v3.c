/*
 * Copyright (C) 2012-2015 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright 2018 NXP
 * Copyright (c) 2020 VeriSilicon Holdings Co., Ltd.
 * Copyright 2020 NXP
 */
/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/regulator/consumer.h>
#include <linux/v4l2-mediabus.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>

#include "vvsensor.h"

#include "ar1335_regs_1080p.h"
#include "ar1335_regs_4kp.h"
#include "ar1335_regs_12MP.h"

#define MOTO_DEFAULT_DISTANCE	0 // 200 // 150   // 0-1023
#define MOTO_I2C_ADDR  0x0C

#define MIN_FPS 15
#define MAX_FPS 30
#define DEFAULT_FPS 30

#define AR1335_XCLK_MIN 6000000
#define AR1335_XCLK_MAX 48000000

#define AR1335_CHIP_ID                  0x153
#define AR1335_CHIP_VERSION_REG 		0x3000
#define AR1335_RESET_REG                0x301A

#define AR1335_SENS_PAD_SOURCE	0
#define AR1335_SENS_PADS_NUM	1

struct ar1335_datafmt {
	u32 code;
	enum v4l2_colorspace colorspace;
};

struct ar1335 {
	struct v4l2_subdev subdev;
	struct v4l2_device *v4l2_dev;
	struct i2c_client *i2c_client;		/* sensor */
	// struct regmap *regmap;  // todo regmap
	struct i2c_client *moto_i2c;		/* moto */

	struct v4l2_pix_format pix;
	const struct ar1335_datafmt *fmt;
	struct v4l2_captureparm streamcap;
	struct media_pad pads[AR1335_SENS_PADS_NUM];
	bool on;

	/* control settings */
	int ae_mode;

	u32 mclk;
	u8 mclk_source;
	u32 sclk;
	struct clk *sensor_clk;
	int csi;

	void (*io_init) (struct ar1335 *);
	int pwn_gpio, rst_gpio;
	bool hdr;
	int fps;

	vvcam_mode_info_t cur_mode;

	struct mutex lock;
};

#define client_to_ar1335(client)\
	container_of(i2c_get_clientdata(client), struct ar1335, subdev)

long ar1335_priv_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg);

static struct vvcam_mode_info par1335_mode_info[] = {
	{
		.index     = 0,
		.width     = 1920,
		.height    = 1080,
		.fps       = 30,
		.hdr_mode  = SENSOR_MODE_LINEAR,
		.bit_width = 10,
		.data_compress.enable = 0,
		.bayer_pattern = BAYER_GRBG,
		.ae_info = {
			.DefaultFrameLengthLines = 0xC4E,
			//one_line_exp_time = line_length_pck/pixel clk, unit s
			.one_line_exp_time_ns = 10601, // 0x1230 / 439.2M
			.max_integration_time = 0xC4E - 1,
			.min_integration_time = 8,			// 8 lines
			.gain_accuracy = 100,
			.max_gain = 24 * 100,
			.min_gain = 1 * 100,
		},
		.preg_data = ar1335_init_setting_1080p,
		.reg_data_count = ARRAY_SIZE(ar1335_init_setting_1080p), 
	},
	{
		.index     = 1,
		.width     = 3840,
		.height    = 2160,
		.fps       = 30,
		.hdr_mode  = SENSOR_MODE_LINEAR,
		.bit_width = 10,
		.bayer_pattern = BAYER_GRBG,
		.ae_info = {
			.DefaultFrameLengthLines = 0xBCC,  // pixel clk 422.40Mhz
			//one_line_exp_time = line_length_pck/pixel clk, unit s
			.one_line_exp_time_ns = 11023,   // 0x1230 / 422.40
			.max_integration_time = 0xBCC - 1,
			.min_integration_time = 8,			// 8 lines
			.gain_accuracy = 100,
			.max_gain = 24 * 100,
			.min_gain = 1 * 100,
		},
		.preg_data = ar1335_init_setting_4kp,
		.reg_data_count = ARRAY_SIZE(ar1335_init_setting_4kp),
	},
	{
		.index     = 2,
		.width     = 4096,
		.height    = 3072,
		.fps       = 30,
		.hdr_mode  = SENSOR_MODE_LINEAR,
		.bit_width = 10,
		.bayer_pattern = BAYER_GRBG,
		.ae_info = {
			.DefaultFrameLengthLines = 0xC1C,  // vt_pixel_clk 432Mhz
			//one_line_exp_time = line_length_pck/vt_pixel_clk, unit s
			.one_line_exp_time_ns = 10778,   // 0x1230 / 432M
			.max_integration_time = 0xC1C - 1,
			.min_integration_time = 8,			// 8 lines
			.gain_accuracy = 100,
			.max_gain = 24 * 100,
			.min_gain = 1 * 100,
		},
		.preg_data = ar1335_init_setting_12MP,
		.reg_data_count = ARRAY_SIZE(ar1335_init_setting_12MP),
	}

};

static const struct ar1335_datafmt ar1335_colour_fmts[] = {
	{MEDIA_BUS_FMT_SGRBG10_1X10, V4L2_COLORSPACE_JPEG},
};

#if 0  // todo regmap
static bool ar1335_readable_register(struct device *dev, unsigned int reg)
{
	if (((reg >= 0x0000) && (reg <= 0x0008)) ||
	    ((reg >= 0x0040) && (reg <= 0x005E)) ||
		((reg >= 0x0080) && (reg <= 0x0092)) ||
		((reg >= 0x00C0) && (reg <= 0x00CE)) ||
		((reg >= 0x0100) && (reg <= 0x0112)) ||
		(reg == 0x120) ||
		((reg >= 0x0202) && (reg <= 0x0214)) ||	
		((reg >= 0x0300) && (reg <= 0x030A)) ||
		((reg >= 0x0340) && (reg <= 0x034E)) ||
		((reg >= 0x0380) && (reg <= 0x0386)) ||
		((reg >= 0x0400) && (reg <= 0x0404)) ||
		(reg == 0x0500) ||
		((reg >= 0x0600) && (reg <= 0x0608)) ||
		((reg >= 0x1000) && (reg <= 0x1006)) ||
		((reg >= 0x1080) && (reg <= 0x1088)) ||
		((reg >= 0x1080) && (reg <= 0x1088)) ||

		((reg >= 0x1108) && (reg <= 0x11C6)) ||
		((reg >= 0x1200) && (reg <= 0x120A)) ||
		(reg == 0x1300) ||
		((reg >= 0x3000) && (reg <= 0x332C)) ||
		((reg >= 0x3600) && (reg <= 0x36E6)) ||
		((reg >= 0x3700) && (reg <= 0x37C6)) ||
		((reg >= 0x3800) && (reg <= 0x3F3C)) ||
		)
		return true;
	else
		return false;
}

static bool ar1335_writeable_register(struct device *dev, unsigned int reg)
{
	if ((reg >= 0x0000) && (reg <= 0x3F3C))
		return true;
	else
		return false;
}

static const struct regmap_config ar1335_i2c_regmap_config = {
	.reg_bits = 16,
	.val_bits = 16,

	.max_register = 0x3F3C,
	.writeable_reg = ar1335_writeable_register,
	.readable_reg = ar1335_readable_register,
	.cache_type = REGCACHE_NONE,
};
#endif

/* Find a data format by a pixel code in an array */
static const struct ar1335_datafmt
                *ar1335_find_datafmt(u32 code)
{
	int i;

	pr_debug("enter %s\n", __func__);
	for (i = 0; i < ARRAY_SIZE(ar1335_colour_fmts); i++)
		if (ar1335_colour_fmts[i].code == code)
			return ar1335_colour_fmts + i;

	return NULL;
}

static inline void ar1335_power_down(struct ar1335 *sensor, int enable)
{
	pr_debug("enter %s\n", __func__);
	if (!gpio_is_valid(sensor->pwn_gpio))
		return;

	gpio_set_value_cansleep(sensor->pwn_gpio, enable);
	udelay(2000);
}

static inline void ar1335_reset(struct ar1335 *sensor)
{
	pr_debug("enter %s\n", __func__);
	if (sensor->pwn_gpio < 0 || sensor->rst_gpio < 0)
		return;

	gpio_set_value_cansleep(sensor->pwn_gpio, 1);
	gpio_set_value_cansleep(sensor->rst_gpio, 0);
	udelay(5000);

	gpio_set_value_cansleep(sensor->pwn_gpio, 0);
	udelay(1000);

	gpio_set_value_cansleep(sensor->rst_gpio, 1);
	msleep(20);
}

static s32 moto_write_reg(struct ar1335 *sensor, u8 reg, u8 val)
{
	struct device *dev = &sensor->i2c_client->dev;
	u8 au8Buf[2] = { reg , val};

	if (i2c_master_send(sensor->moto_i2c, au8Buf, 2) < 0) {
		dev_err(dev, "Write moto reg error: reg=%x, val=%x\n", reg, val);
		return -1;
	}

	return 0;
}

static s32 moto_read_reg(struct ar1335 *sensor, u8 reg, u8 *val)
{
	struct device *dev = &sensor->i2c_client->dev;
	u8 au8RegBuf[1] = { reg };
	u8 au8RdVal[1] = {0};

	if (i2c_master_send(sensor->moto_i2c, au8RegBuf, 1) != 1) {
		dev_err(dev, "Read moto reg error: reg=%x\n", reg);
		return -1;
	}

	if (i2c_master_recv(sensor->moto_i2c, au8RdVal, 1) != 1) {
		dev_err(dev, "Read moto reg error: reg=%x, val=%x\n",
                        reg, au8RdVal[0]);
		return -1;
	}

	*val = au8RdVal[0];

	return *val;
}

static s32 ar1335_write_reg(struct ar1335 *sensor, u16 reg, u16 val)
{
	struct device *dev = &sensor->i2c_client->dev;
	u8 au8Buf[4] = { reg >> 8, reg & 0xff, val >> 8, val & 0xff };

	if (i2c_master_send(sensor->i2c_client, au8Buf, 4) < 0) {
		dev_err(dev, "Write reg error: reg=%x, val=%x\n", reg, val);
		return -1;
	}

	return 0;
}

static s32 ar1335_read_reg(struct ar1335 *sensor, u16 reg, u16 *val)
{
	struct device *dev = &sensor->i2c_client->dev;
	u8 au8RegBuf[2] = { reg >> 8, reg & 0xff };
	u8 au8RdVal[2] = {0};

	if (i2c_master_send(sensor->i2c_client, au8RegBuf, 2) != 2) {
		dev_err(dev, "Read reg error: reg=%x\n", reg);
		return -1;
	}

	if (i2c_master_recv(sensor->i2c_client, au8RdVal, 2) != 2) {
		dev_err(dev, "Read reg error: reg=%x, val=%x %x\n",
                        reg, au8RdVal[0], au8RdVal[1]);
		return -1;
	}

	*val = ((u16)au8RdVal[0] << 8) | (u16)au8RdVal[1];

	return *val;
}

static int ar1335_set_clk_rate(struct ar1335 *sensor)
{
	u32 tgt_xclk;		/* target xclk */
	int ret;

	/* mclk */
	tgt_xclk = sensor->mclk;
	tgt_xclk = min_t(u32, tgt_xclk, (u32) AR1335_XCLK_MAX);
	tgt_xclk = max_t(u32, tgt_xclk, (u32) AR1335_XCLK_MIN);
	sensor->mclk = tgt_xclk;

	pr_debug("   Setting mclk to %d MHz\n", tgt_xclk / 1000000);
	ret = clk_set_rate(sensor->sensor_clk, sensor->mclk);
	if (ret < 0)
		pr_debug("set rate filed, rate=%d\n", sensor->mclk);
	return ret;
}

static int ar1335_stream_on(struct ar1335 *sensor)
{
	int ret;
	u16 val = 0;
	pr_info("enter %s\n", __func__);
	ret = ar1335_read_reg(sensor, AR1335_RESET_REG, &val);
	if (ret < 0)
		return ret;
	val |= 0x0004;
	ret = ar1335_write_reg(sensor, AR1335_RESET_REG, val);

	return ret;
}

static int ar1335_stream_off(struct ar1335 *sensor)
{
	int ret;
	u16 val = 0;
	pr_info("enter %s\n", __func__);
	ret = ar1335_read_reg(sensor, AR1335_RESET_REG, &val);
	if (ret < 0)
		return ret;
	val &= (~0x0004);
	ret = ar1335_write_reg(sensor, AR1335_RESET_REG, val);

	return ret;
}

/* download ar1335 settings to sensor through i2c */
static int ar1335_download_firmware(struct ar1335 *sensor,
				    struct vvsensor_ar1335_reg_value_t *mode_setting,
				    s32 size)
{
	register u32 delay_ms = 0;
	register u16 reg_addr = 0;
	register u16 mask = 0;
	register u16 val = 0;
	u16 reg_val = 0;
	int i, retval = 0;

	pr_info("enter %s\n", __func__);
	for (i = 0; i < size; ++i, ++mode_setting) {
		delay_ms = mode_setting->delay;
		reg_addr = mode_setting->addr;
		val = mode_setting->val;
		mask = mode_setting->mask;

		if (mask) {
			printk("mask\n");
			retval = ar1335_read_reg(sensor, reg_addr, &reg_val);
			if (retval < 0)
				break;

			reg_val &= ~(u16)mask;
			val &= mask;
			val |= reg_val;
		}

		retval = ar1335_write_reg(sensor, reg_addr, val);
		if (retval < 0)
			break;

		if (delay_ms)
			msleep(delay_ms);
	}

	ar1335_stream_off(sensor);

	return retval;
}

static int ar1335_s_power(struct v4l2_subdev *sd, int on)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ar1335 *sensor = client_to_ar1335(client);

	pr_debug("enter %s\n", __func__);
	if (on)
		clk_prepare_enable(sensor->sensor_clk);
	else
		clk_disable_unprepare(sensor->sensor_clk);

	sensor->on = on;
	return 0;
}

static int ar1335_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *a)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ar1335 *sensor = client_to_ar1335(client);
	struct v4l2_captureparm *cparm = &a->parm.capture;
	int ret = 0;

	pr_debug("enter %s\n", __func__);
	switch (a->type) {
		/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
	case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
		memset(a, 0, sizeof(*a));
		a->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		cparm->capability = sensor->streamcap.capability;
		cparm->timeperframe = sensor->streamcap.timeperframe;
		cparm->capturemode = sensor->streamcap.capturemode;
		ret = 0;
		break;

		/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		ret = -EINVAL;
		break;

	default:
		pr_debug("   type is unknown - %d\n", a->type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int ar1335_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *a)
{
	int ret = 0;

	pr_debug("enter %s\n", __func__);
	switch (a->type) {
		/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
	case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		pr_debug("   type is not V4L2_BUF_TYPE_VIDEO_CAPTURE but %d\n",
			 a->type);
		ret = -EINVAL;
		break;

	default:
		pr_debug("   type is unknown - %d\n", a->type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int ar1335_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ar1335 *sensor = client_to_ar1335(client);
    int ret;

	pr_debug("enter %s\n", __func__);
	if (enable) {
        ret = ar1335_stream_on(sensor);
        if (ret < 0)
            return ret;
    }
	else {
        ret = ar1335_stream_off(sensor);
        if (ret < 0)
            return ret;
    }

	sensor->on = enable;
	return 0;
}

static int ar1335_set_fmt(struct v4l2_subdev *sd,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;
	const struct ar1335_datafmt *fmt = ar1335_find_datafmt(mf->code);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ar1335 *sensor = client_to_ar1335(client);
	unsigned int i;
	struct vvsensor_ar1335_reg_value_t *mode_setting = NULL;
	int array_size = 0;

	pr_debug("enter %s\n", __func__);
	if (format->pad) {
		return -EINVAL;
	}

	if (!fmt) {
		mf->code = ar1335_colour_fmts[0].code;
		mf->colorspace = ar1335_colour_fmts[0].colorspace;
	}

	mf->field = V4L2_FIELD_NONE;
	/*  old search method,  vsi need change to
	    search resolution by width/height */
	/*  try_to_find_resolution(sensor, mf); */
	if (format->which == V4L2_SUBDEV_FORMAT_TRY)
		return 0;

	for (i=0; i<ARRAY_SIZE(par1335_mode_info); i++)
	{
		if (sensor->hdr)
		{
		    if (mf->width == par1335_mode_info[i].width && 
			    mf->height == par1335_mode_info[i].height &&
			    par1335_mode_info[i].hdr_mode != SENSOR_MODE_LINEAR)
			{
				memcpy(&(sensor->cur_mode), &par1335_mode_info[i], sizeof(struct vvcam_mode_info));
				mode_setting = par1335_mode_info[i].preg_data;
				array_size = par1335_mode_info[i].reg_data_count;
				sensor->pix.width = par1335_mode_info[i].width;
				sensor->pix.height = par1335_mode_info[i].height;
				return ar1335_download_firmware(sensor, mode_setting, array_size);
			}
		}else
		{
			if (mf->width == par1335_mode_info[i].width && 
			    mf->height == par1335_mode_info[i].height &&
			    par1335_mode_info[i].hdr_mode == SENSOR_MODE_LINEAR)
			{
				memcpy(&(sensor->cur_mode), &par1335_mode_info[i], sizeof(struct vvcam_mode_info));
				mode_setting = par1335_mode_info[i].preg_data;
				array_size = par1335_mode_info[i].reg_data_count;
				sensor->pix.width = par1335_mode_info[i].width;
				sensor->pix.height = par1335_mode_info[i].height;
				printk("test %s w %d, h %d, w %d, h %d\n", __func__, mf->width, mf->height,
								par1335_mode_info[i].width, par1335_mode_info[i].height);
				return ar1335_download_firmware(sensor, mode_setting, array_size);
			}
		}
	}

	pr_err("%s search error: %d %d\n", __func__, mf->width, mf->height);
	return -EINVAL;;
}

static int ar1335_get_fmt(struct v4l2_subdev *sd,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ar1335 *sensor = client_to_ar1335(client);

	pr_debug("enter %s\n", __func__);
	if (format->pad)
		return -EINVAL;

	memset(mf, 0, sizeof(struct v4l2_mbus_framefmt));

	mf->code = ar1335_colour_fmts[0].code;
	mf->colorspace = ar1335_colour_fmts[0].colorspace;
	mf->width = sensor->pix.width;
	mf->height = sensor->pix.height;
	mf->field = V4L2_FIELD_NONE;

	dev_info(&client->dev,
		"%s code=0x%x, w/h=(%d,%d), colorspace=%d, field=%d\n",
		__func__, mf->code, mf->width, mf->height, mf->colorspace,
		mf->field);

	return 0;
}

static int ar1335_enum_code(struct v4l2_subdev *sd,
			    struct v4l2_subdev_pad_config *cfg,
			    struct v4l2_subdev_mbus_code_enum *code)
{
	pr_debug("enter %s\n", __func__);
	if (code->pad || code->index >= ARRAY_SIZE(ar1335_colour_fmts))
		return -EINVAL;

	code->code = ar1335_colour_fmts[code->index].code;
	return 0;
}

static int ar1335_enum_framesizes(struct v4l2_subdev *sd,
				  struct v4l2_subdev_pad_config *cfg,
				  struct v4l2_subdev_frame_size_enum *fse)
{
	pr_debug("enter %s\n", __func__);
	
	if (fse->index > ARRAY_SIZE(par1335_mode_info))
		return -EINVAL;

	fse->min_width = par1335_mode_info[fse->index].width;   
	fse->max_width = fse->min_width;
	fse->min_height = par1335_mode_info[fse->index].height;    
	fse->max_height = fse->min_height;

	return 0;
}

static int ar1335_enum_frameintervals(struct v4l2_subdev *sd,
				      struct v4l2_subdev_pad_config *cfg,
				      struct v4l2_subdev_frame_interval_enum
				      *fie)
{
	pr_debug("enter %s\n", __func__);
	if (fie->index < 0 || fie->index > ARRAY_SIZE(par1335_mode_info))
		return -EINVAL;

	if (fie->width == 0 || fie->height == 0 || fie->code == 0) {
		pr_warn("Please assign pixel format, width and height.\n");
		return -EINVAL;
	}

	fie->interval.numerator = 1;
	fie->interval.denominator = par1335_mode_info[fie->index].fps;

	return 0;
}

static int ar1335_link_setup(struct media_entity *entity,
			     const struct media_pad *local,
			     const struct media_pad *remote, u32 flags)
{
	return 0;
}

static struct v4l2_subdev_video_ops ar1335_subdev_video_ops = {
	.g_parm = ar1335_g_parm,
	.s_parm = ar1335_s_parm,
	.s_stream = ar1335_s_stream,
};

static const struct v4l2_subdev_pad_ops ar1335_subdev_pad_ops = {
	.enum_frame_size = ar1335_enum_framesizes,
	.enum_frame_interval = ar1335_enum_frameintervals,
	.enum_mbus_code = ar1335_enum_code,
	.set_fmt = ar1335_set_fmt,
	.get_fmt = ar1335_get_fmt,
};

static struct v4l2_subdev_core_ops ar1335_subdev_core_ops = {
	.s_power = ar1335_s_power,
	.ioctl = ar1335_priv_ioctl,
};

static struct v4l2_subdev_ops ar1335_subdev_ops = {
	.core = &ar1335_subdev_core_ops,
	.video = &ar1335_subdev_video_ops,
	.pad = &ar1335_subdev_pad_ops,
};

static const struct media_entity_operations ar1335_sd_media_ops = {
	.link_setup = ar1335_link_setup,
};

static int ar1335_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
    struct pinctrl *pinctrl;
	struct device *dev = &client->dev;
	struct v4l2_subdev *sd;
	struct ar1335 *sensor;
    int retval;
    u16 chip_id;

    pr_debug("enter %s\n", __func__);
	sensor = devm_kmalloc(dev, sizeof(*sensor), GFP_KERNEL);
	if (!sensor)
		return -ENOMEM;
    /* Set initial values for the sensor struct. */
	memset(sensor, 0, sizeof(*sensor));

    pinctrl = devm_pinctrl_get_select_default(dev);
	if (IS_ERR(pinctrl)) {
		dev_err(dev, "setup pinctrl failed\n");
		return PTR_ERR(pinctrl);
	}

   /* request power down pin */
	sensor->pwn_gpio = of_get_named_gpio(dev->of_node, "pwn-gpios", 0);
	if (!gpio_is_valid(sensor->pwn_gpio))
		dev_warn(dev, "No sensor pwdn pin available");
	else {
		retval = devm_gpio_request_one(dev, sensor->pwn_gpio,
					       GPIOF_OUT_INIT_HIGH,
					       "ar1335_mipi_pwdn");
		if (retval < 0) {
			dev_warn(dev, "Failed to set power pin\n");
			dev_warn(dev, "retval=%d\n", retval);
			return retval;
		}
	}

	/* request reset pin */
	sensor->rst_gpio = of_get_named_gpio(dev->of_node, "rst-gpios", 0);
	if (!gpio_is_valid(sensor->rst_gpio))
		dev_warn(dev, "No sensor reset pin available");
	else {
		retval = devm_gpio_request_one(dev, sensor->rst_gpio,
					       GPIOF_OUT_INIT_HIGH,
					       "ar1335_mipi_reset");
		if (retval < 0) {
			dev_warn(dev, "Failed to set reset pin\n");
			return retval;
		}
	}

    /* Set initial values for the sensor struct. */
	sensor->sensor_clk = devm_clk_get(dev, "csi_mclk");
	if (IS_ERR(sensor->sensor_clk)) {
		/* assuming clock enabled by default */
		sensor->sensor_clk = NULL;
		dev_err(dev, "clock-frequency missing or invalid\n");
		return PTR_ERR(sensor->sensor_clk);
	}

	retval = of_property_read_u32(dev->of_node, "mclk", &(sensor->mclk));
	if (retval) {
		dev_err(dev, "mclk missing or invalid\n");
		return retval;
	}

	retval = of_property_read_u32(dev->of_node, "mclk_source",
				      (u32 *)&(sensor->mclk_source));
	if (retval) {
		dev_err(dev, "mclk_source missing or invalid\n");
		return retval;
	}

	retval = of_property_read_u32(dev->of_node, "csi_id", &(sensor->csi));
	if (retval) {
		dev_err(dev, "csi id missing or invalid\n");
		return retval;
	}

    /* Set mclk rate before clk on */
	ar1335_set_clk_rate(sensor);

	retval = clk_prepare_enable(sensor->sensor_clk);
	if (retval < 0) {
		dev_err(dev, "%s: enable sensor clk fail\n", __func__);
		return -EINVAL;
	}
	mdelay(2);

    sensor->io_init = ar1335_reset;
	sensor->i2c_client = client;

	sensor->moto_i2c = i2c_new_dummy(client->adapter,
						MOTO_I2C_ADDR);
	if (!sensor->moto_i2c) {
		// retval = -ENODEV;
		return -ENODEV;
	}

	sensor->pix.pixelformat = V4L2_PIX_FMT_UYVY;
	sensor->pix.width = par1335_mode_info[0].width;
	sensor->pix.height = par1335_mode_info[0].height;
	sensor->streamcap.capability = V4L2_MODE_HIGHQUALITY |
	    V4L2_CAP_TIMEPERFRAME;
	sensor->streamcap.capturemode = 0;
	sensor->streamcap.timeperframe.denominator = par1335_mode_info[0].fps;
	sensor->streamcap.timeperframe.numerator = 1;

	ar1335_reset(sensor);

    retval = ar1335_read_reg(sensor, AR1335_CHIP_VERSION_REG, &chip_id);
	if ((retval < 0) | (chip_id != AR1335_CHIP_ID)) {
		dev_err(dev, "Sensor AR1335 is not found\n");
        return -ENODEV;
    }

    sd = &sensor->subdev;
	v4l2_i2c_subdev_init(sd, client, &ar1335_subdev_ops);
	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	sd->entity.function = MEDIA_ENT_F_CAM_SENSOR;
	sensor->pads[AR1335_SENS_PAD_SOURCE].flags = MEDIA_PAD_FL_SOURCE;

	retval = media_entity_pads_init(&sd->entity, AR1335_SENS_PADS_NUM,
					sensor->pads);
	sd->entity.ops = &ar1335_sd_media_ops;
	if (retval < 0)
		return retval;

    retval = v4l2_async_register_subdev_sensor_common(sd);
	if (retval < 0) {
		dev_err(&client->dev,
			"%s--Async register failed, ret=%d\n", __func__,
			retval);
		media_entity_cleanup(&sd->entity);
	}

	mutex_init(&sensor->lock);
#ifdef MOTO_DEFAULT_DISTANCE
	{
		u8 val;
		u16 dist;
		// init 
		retval = moto_write_reg(sensor, 0x2, 0x0);
		if (retval == 0) {
			// set distance 10bit
			dist = MOTO_DEFAULT_DISTANCE << 6;

			// low 2bit
			retval = moto_read_reg(sensor, 0x1, &val);
			if (retval == 0) {
				val = val & 0x3f;
				val = val | (dist & 0xc0);
				retval = moto_write_reg(sensor, 0x1, val);
			}

			// high 8bit
			retval = moto_write_reg(sensor, 0x0, (dist >> 8) & 0xff);

			moto_read_reg(sensor, 0x0, &val);
			dist = val << 2;
			moto_read_reg(sensor, 0x1, &val);
			dist += (val >> 6) & 0x3;
			printk("set default distance to %x\n", dist);
		}
		else {
			printk("moto init failed\n");
		}
	}
#endif

    pr_info("%s camera mipi ar1335, is found\n", __func__);

    return retval;
}

static int ar1335_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct ar1335 *sensor = client_to_ar1335(client);

	pr_info("enter %s\n", __func__);

	i2c_unregister_device(sensor->moto_i2c);
	v4l2_async_unregister_subdev(sd);

	/* clk_unprepare(sensor->sensor_clk); */
	ar1335_power_down(sensor, 1);

	mutex_destroy(&sensor->lock);
	return 0;
}

static const struct i2c_device_id ar1335_id[] = {
	{"ar1335", 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, ar1335_id);

static const struct of_device_id ar1335_dt_ids[] = {
	{ .compatible = "onsemi,ar1335" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ar1335_dt_ids);

static struct i2c_driver ar1335_i2c_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name  = "ar1335",
		.of_match_table	= ar1335_dt_ids,
	},
	.probe  = ar1335_probe,
	.remove = ar1335_remove,
	.id_table = ar1335_id,
};

module_i2c_driver(ar1335_i2c_driver);
MODULE_DESCRIPTION("AR1335 MIPI Camera Subdev Driver");
MODULE_LICENSE("GPL");

static const u16 reg_gain_setting [] = {
0x2010, 0x2014, 0x2018, 0x201C, 0x2020, 0x2022, 0x2024, 0x2026, 
0x2028, 0x202A, 0x202C, 0x202E, 0x2030, 0x2031, 0x2032, 0x2033, 
0x2034, 0x2035, 0x2036, 0x2037, 0x2038, 0x2039, 0x203A, 0x203B, 
0x203C, 0x203D, 0x203E, 0x203F, 0x213F, 0x223F, 0x233F, 0x243F, 
0x253F, 0x263F, 0x273F, 0x28BF, 0x29BF, 0x2ABF, 0x2BBF, 0x2CBF, 
0x2DBF, 0x2EBF, 0x2FBF, 0x30BF, 0x31BF, 0x32BF, 0x33BF, 0x34BF, 
0x35BF, 0x36BF, 0x37BF, 0x393F, 0x3A3F, 0x3B3F, 0x3C3F, 0x3D3F, 
0x3E3F, 0x3F3F, 0x403F, 0x413F, 0x423F, 0x433F, 0x443F, 0x453F, 
0x463F, 0x473F, 0x48BF, 0x49BF, 0x4ABF, 0x4BBF, 0x4CBF, 0x4DBF, 
0x4EBF, 0x4FBF, 0x50BF, 0x51BF, 0x52BF, 0x53BF, 0x54BF, 0x55BF, 
0x56BF, 0x57BF, 0x593F, 0x5A3F, 0x5B3F, 0x5C3F, 0x5D3F, 0x5E5F, 
0x5F3F, 0x603F, 0x613F, 0x623F, 0x633F
};


int ar1335_s_gain(struct ar1335 *sensor, __u32 new_gain)
{
#if 0
	u16 reg_gain_setting[16] = {0x2010, 0x2020, 0x2028, 0x2030, 0x2034,
					0x2038, 0x203C, 0x213F, 0x253F, 0x29BF, 0x2DBF,
					0x31BF, 0x35BF, 0x3A3F, 0x3E3F, 0x423F};
	int ret = 0;
	ret = ar1335_write_reg(sensor, 0x305E, reg_gain_setting[new_gain-1]);
#else
	__u32 gain_set = 0;
	int ret = 0;
	gain_set = (new_gain / 25) * 25;
	if (gain_set < new_gain) {
		gain_set += 25; 
	}
	
	printk("set new gain %d, %d\n", new_gain, gain_set);
	if (gain_set < 100) {
		printk("invalid new gain value %d\n", gain_set);
		gain_set = 100;
	}

	ret = ar1335_write_reg(sensor, 0x305E, reg_gain_setting[(gain_set - 100) / 25]);
	new_gain = gain_set;
#endif
	return ret;
}

struct ar1335_gain_context {
	//u16 digital_gain;
	//u16 analog_coarse_gain;
	//u16 analog_fine_gain ;
	u16 tgain_int;    // total gain (include dgital and analog)
	u16 tgain_step;
};

int ar1335_g_gain(struct ar1335 *sensor, struct ar1335_gain_context *ctx)
{
#if 0
	__u16 val = 0;
	int ret = 0;
	ret = ar1335_read_reg(sensor, 0x305E, &val);
	//R0x305E[15:7], digital_gain
	//R0x305E[6:4],  analog_coarse_gain
	//R0x305E[3:0],  analog_fine_gain
	//total_gain = (digital_gain/64) * (2**(analog_coarse_gain - 1))
	//				* ((16 + analog_fine_gain) / 16)
	gain->digital_gain = (val & 0xFF80) >> 7;
	gain->analog_coarse_gain = (val & 0x0070) >> 4; 
	gain->analog_fine_gain = val & 0x000F;
#else
	int i;
	__u16 val = 0;
	int ret = 0;
	ret = ar1335_read_reg(sensor, 0x305E, &val);

	for (i = 0; i < 93/*sizeof(reg_gain_setting) / sizeof(__u16)*/ ; i++ ) {
		if (val == reg_gain_setting[i])	{
			break;
		}
	}

	if (i >= 93 ) {
		pr_info("failed sensor gain register value\n");
		ret = -1;
	}
	else {
		ctx->tgain_int =  (i / 4 + 1);   // total gain =1.75 =>  ctx->tgain_int =1 , ctx->tgain_step = 3 (3*0.25)
		ctx->tgain_step = (i % 4);
	}
#endif
	return ret;
}

int ar1335_s_exp(struct ar1335 *sensor, __u32 exp)
{
#if 0
	int array_clk = 439200000;
	__u16 val = 0;
	int ret = 0;

	/* pr_info("enter %s 0x%x\n", __func__, exp); */
	//frame_length_lines * line_length_pck * fps = array clk
	//R0x300A or R0x0340, frame_length_lines
	//R0x300C or R0x0342, line_length_pck
	//R0x3012, coarse_integration_time
	//coarse_integration_time = exp / (line_length_pck/pixel clk)
	ret = ar1335_read_reg(sensor, 0x300c, &val);
	val = (u16)(exp * array_clk / val / 1000); // exp unit ms
	ret = ar1335_write_reg(sensor, 0x3012, val);
#else
	int ret = 0;
	if (exp < 8) {
		printk("invalid new exp line %d\n", exp);
		exp = 8;
	}
	printk("new exp line %d\n", exp);
	ret = ar1335_write_reg(sensor, 0x3012, exp);  // ioctl set expline directly
#endif
	return ret;
}

int ar1335_g_exp(struct ar1335 *sensor, __u32 *exp)
{
#if 0
	int array_clk = 439200000;
	__u16 line_length_pck = 0;
	__u16 coarse_integration_time = 0;
	int ret = 0;

	ret = ar1335_read_reg(sensor, 0x300c, &line_length_pck);
	ret |= ar1335_read_reg(sensor, 0x3012, &coarse_integration_time);
	*exp = (__u32)(coarse_integration_time * line_length_pck / array_clk);
#else
	int ret = 0;
	__u16 val = 0;
	ret = ar1335_read_reg(sensor, 0x3012, &val);  // ioctl set expline directly
	*exp = val;
#endif
	return ret;
}

int ar1335_g_chipid(struct ar1335 *sensor, __u32 *chip_id)
{
	int ret = 0;
	ret = ar1335_read_reg(sensor, AR1335_CHIP_VERSION_REG, (u16 *)chip_id);

	return ret;
}

int ar1335_ioc_qcap(struct ar1335 *sensor, void *args)
{
	struct v4l2_capability *cap = (struct v4l2_capability *)args;

	strcpy((char *)cap->driver, "ar1335");
	sprintf((char *)cap->bus_info, "csi%d",sensor->csi);//bus_info[0:7]-csi number
	if(sensor->i2c_client->adapter)
	{//bus_info[8]-i2c bus dev number
		cap->bus_info[VVCAM_CAP_BUS_INFO_I2C_ADAPTER_NR_POS] = (__u8)sensor->i2c_client->adapter->nr;
	}
	else
	{
		cap->bus_info[VVCAM_CAP_BUS_INFO_I2C_ADAPTER_NR_POS] = 0xFF;
	}
	
	return 0;
}

int ar1335_ioc_query_mode(struct ar1335 *sensor, struct vvcam_mode_info_array *array)
{
	array->count = ARRAY_SIZE(par1335_mode_info);
	memcpy(&array->modes,par1335_mode_info,sizeof(par1335_mode_info));

	return 0;
}

int ar1335_g_mode(struct ar1335 *sensor, struct vvcam_mode_info *pmode)
{
	int i = 0;
	struct vvcam_mode_info *pcur_mode = NULL;

	if (sensor->cur_mode.index == pmode->index && 
	    sensor->cur_mode.width != 0 &&
	    sensor->cur_mode.height != 0)
	{
		pcur_mode = &(sensor->cur_mode);
		memcpy(pmode,pcur_mode,sizeof(struct vvcam_mode_info));
		return 0;
	}
	
	for(i=0; i < ARRAY_SIZE(par1335_mode_info); i++)
	{
		if (pmode->index == par1335_mode_info[i].index)
		{
			pcur_mode = &par1335_mode_info[i];
			sensor->fps = par1335_mode_info[i].fps;
			break;
		}
	}

	if (pcur_mode == NULL)
	{
		return -1;
	}

	memcpy(pmode,pcur_mode,sizeof(struct vvcam_mode_info));
	return 0;
}

/*
Use USER_TO_KERNEL/KERNEL_TO_USER to fix "uaccess" exception on run time.
Also, use "copy_ret" to fix the build issue as below.
error: ignoring return value of function declared with 'warn_unused_result' attribute.
*/

#ifdef CONFIG_HARDENED_USERCOPY
#define USER_TO_KERNEL(TYPE) \
	do {\
		TYPE tmp; \
		unsigned long copy_ret; \
		arg = (void *)(&tmp); \
		copy_ret = copy_from_user(arg, arg_user, sizeof(TYPE));\
	} while (0)

#define KERNEL_TO_USER(TYPE) \
	do {\
		unsigned long copy_ret; \
		copy_ret = copy_to_user(arg_user, arg, sizeof(TYPE));\
	} while (0)
#else
#define USER_TO_KERNEL(TYPE)
#define KERNEL_TO_USER(TYPE)
#endif

long ar1335_priv_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg_user)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ar1335 *sensor = client_to_ar1335(client);
	struct vvcam_sccb_data reg;
	int ret = 0;
	void *arg = arg_user;

	/* pr_info("enter %s\n", __func__); */
	mutex_lock(&sensor->lock);
	switch (cmd) {
	case VVSENSORIOC_WRITE_REG: {
		USER_TO_KERNEL(struct vvcam_sccb_data);
		reg = *(struct vvcam_sccb_data *)arg;
		ret = ar1335_write_reg(sensor, reg.addr, (u16)reg.data) < 0;
		break;
	}
	case VVSENSORIOC_READ_REG: {
		struct vvcam_sccb_data *preg;
		u16 val;
		USER_TO_KERNEL(struct vvcam_sccb_data);
		preg = (struct vvcam_sccb_data *)arg;
		ret = ar1335_read_reg(sensor, (u16) preg->addr, &val) < 0;
		preg->data = val;
		KERNEL_TO_USER(struct vvcam_sccb_data);
		break;
	}
	case VVSENSORIOC_S_STREAM: {
		USER_TO_KERNEL(__u32);
		ret = ar1335_s_stream(sd, *(__u32 *)arg);
		break;
	}
	case VVSENSORIOC_S_EXP: {
		USER_TO_KERNEL(__u32);
		ret = ar1335_s_exp(sensor, *(__u32 *)arg);
		break;
	}
#if 0  // P9 removed VVSENSORIOC_G_EXP
	case VVSENSORIOC_G_EXP: {
		USER_TO_KERNEL(__u32);
		ret = ar1335_g_exp(sensor, (__u32 *)arg);
		KERNEL_TO_USER(__u32);
		break;
	}
#endif
	case VVSENSORIOC_S_GAIN: {
		USER_TO_KERNEL(__u32);
		ret = ar1335_s_gain(sensor, *(__u32 *)arg);
		break;
	}
#if 0  // P9 removed VVSENSORIOC_G_GAIN
	case VVSENSORIOC_G_GAIN: {
		USER_TO_KERNEL(struct ar1335_gain_context);
		ret = ar1335_g_gain(sensor, (struct ar1335_gain_context *)arg);
		KERNEL_TO_USER(struct ar1335_gain_context);
		break;
	}
#endif
	case VIDIOC_QUERYCAP:
		ret = ar1335_ioc_qcap(sensor, arg);
		break;
	case VVSENSORIOC_G_CHIP_ID: {
		USER_TO_KERNEL(__u32);
		ret = ar1335_g_chipid(sensor, (__u32 *)arg);
		ret = (ret < 0) ? -1 : 0;
		KERNEL_TO_USER(__u32);
		break;
	}
	case VVSENSORIOC_QUERY: {
		USER_TO_KERNEL(struct vvcam_mode_info_arry);
		ret = ar1335_ioc_query_mode(sensor, arg);
		KERNEL_TO_USER(struct vvcam_mode_info_arry);
		break;
	}
	case VVSENSORIOC_G_SENSOR_MODE:{
		USER_TO_KERNEL(struct vvcam_mode_info);
		ret = ar1335_g_mode(sensor, arg);
		KERNEL_TO_USER(struct vvcam_mode_info);
		break;
	}
	case VVSENSORIOC_S_FPS: { // todo
		USER_TO_KERNEL(__u32);
	   //	ret = ar1335_s_fps(sensor, *(__u32 *)arg);
		break;
	}
	case VVSENSORIOC_G_FPS: {
		USER_TO_KERNEL(__u32);
		//ret = ar1335_g_fps(sensor, (__u32 *)arg);
		KERNEL_TO_USER(__u32);
		break;
	}
	case VVSENSORIOC_S_HDR_MODE: {
		USER_TO_KERNEL(bool);
		// ret = ar1335_s_hdr(sensor, *(bool *)arg);
		break;
	}
	case VVSENSORIOC_AF_WRITE_REG: {
		USER_TO_KERNEL(struct vvcam_sccb_data);
		reg = *(struct vvcam_sccb_data *)arg;
		ret = moto_write_reg(sensor, (u8)reg.addr, (u8)reg.data) < 0;
		break;
	}
	case VVSENSORIOC_AF_READ_REG: {
		struct vvcam_sccb_data *preg;
		u8 val;
		USER_TO_KERNEL(struct vvcam_sccb_data);
		preg = (struct vvcam_sccb_data *)arg;
		ret = moto_read_reg(sensor, (u8) preg->addr, &val) < 0;
		preg->data = val;
		KERNEL_TO_USER(struct vvcam_sccb_data);
		break;
	}

	default:
		pr_err("unsupported ar1335 command %d.", cmd);
		ret = -1;
		break;
	}
	mutex_unlock(&sensor->lock);
	return ret;
}
