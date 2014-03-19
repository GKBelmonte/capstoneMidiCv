/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter driver
 *
 * Copyright (C) 2010-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef ADC_H
#define ADC_H

#include <compiler.h>
#include <conf_adc.h>
#include <nvm.h>
#include <parts.h>
#include <sleepmgr.h>
#include <sysclk.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup adc_group Analog to Digital Converter (ADC)
 *
 * This is a driver for the AVR XMEGA ADC. It provides functions for enabling,
 * disabling and configuring the ADC modules and their individual channels.
 *
 * The driver API is split in two parts:
 * - \ref adc_channel_group
 * - \ref adc_module_group
 *
 * Both APIs use structures that contain the configuration. These structures
 * must be set up before the configuration is written to either an ADC module or
 * one of their channels.
 *
 * After the ADC has been configured it must be enabled before any conversions
 * may be performed. To ensure accurate conversions, please wait for at least
 * the specified start-up time between enabling the ADC module, and starting
 * a conversion. For most XMEGA devices the start-up time is specified
 * to be a maximum of 24 ADC clock cycles. Please verify the start-up time for
 * the device in use.
 *
 * \note Not all of the documented functions are available on all devices. This
 * is due to differences in the ADC feature set. Refer to the device manual and
 * datasheet for details on which features are available for a specific device.
 *
 * \note The functions for creating/changing configurations are not protected
 * against interrupts. The functions that read from or write to the ADC's
 * registers are protected unless otherwise noted.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref sysclk_group for peripheral clock control.
 * - \ref sleepmgr_group for setting allowed sleep mode.
 * - \ref nvm_group for getting factory calibration data.
 * - \ref interrupt_group for ISR definition and disabling interrupts during
 * critical code sections.
 * @{
 */

/**
 * \internal
 * \name Workaround definitions
 *
 * \todo Remove workarounds for missing definitions in device header files
 * @{
 */
  
#ifndef ADC_CH_OFFSET_gp
#define ADC_CH_OFFSET_gp        4 
#endif  

#define ADC_EVACT_SYNCHSWEEP_tmpfix_gc 0x06

#define ADC_CH_GAIN_DIV2_tmpfix_gc     (0x07 << 2)

#define ADC_CURRLIMIT_tmpfix_gm        (0x03 << 5)

typedef enum ADC_CURRLIMIT_tmpfix_enum {
	ADC_CURRLIMIT_NO_tmpfix_gc       = (0x00 << 5),
	ADC_CURRLIMIT_SMALL_tmpfix_gc    = (0x01 << 5),
	ADC_CURRLIMIT_MEDIUM_tmpfix_gc   = (0x02 << 5),
	ADC_CURRLIMIT_LARGE_tmpfix_gc    = (0x03 << 5),
} ADC_CURRLIMIT_tmpfix_t;

/* Compatibility macro for IAR */
#if defined(__ICCAVR__)
#  define _WORDREGISTER WORDREGISTER
#endif

typedef struct ADC_CH_tmpfix_struct {
	register8_t CTRL;
	register8_t MUXCTRL;
	register8_t INTCTRL;
	register8_t INTFLAGS;
	_WORDREGISTER(RES);
	register8_t SCAN;
	register8_t reserved_0x07;
} ADC_CH_tmpfix_t;
/** @} */

/**
 * \defgroup adc_module_group ADC module
 *
 * Management and configuration functions for the ADC module.
 *
 * The API functions and definitions can be divided in three groups:
 * - interrupt callback: configure and set interrupt callback function.
 * - module management: direct access for enabling and disabling the ADC,
 * starting conversions, getting interrupt flags, etc.
 * - module configuration: create/change configurations and write/read them
 * to/from an ADC.
 *
 * @{
 */

/**
 * \def ADC_NR_OF_CHANNELS
 * \brief Number of channels per ADC
 */
#if XMEGA_A || XMEGA_AU
#  define ADC_NR_OF_CHANNELS    4
#elif XMEGA_B || XMEGA_C || XMEGA_D
#  define ADC_NR_OF_CHANNELS    1
#endif

/**
 * \def CONFIG_ADC_VERSION
 * \brief XMEGA ADC version
 *
 * This driver supports two versions of the XMEGA ADC. Version 2 is found in
 * all XMEGA AU, B and C devices and features current limitation, input scan,
 * internal GND, VCC/2 reference and 1/2x input gain. This symbol is used to
 * control, at compile-time, which version the driver should support.
 *
 * For XMEGA AU, B and C devices, this symbol is automatically defined as 2.
 * For other devices, this symbol defaults to 1, but can be overridden by
 * defining it in conf_adc.h.
 */
#if XMEGA_AU || XMEGA_B || XMEGA_C || defined(__DOXYGEN__)
#  define CONFIG_ADC_VERSION    2
#elif !defined(CONFIG_ADC_VERSION)
#  define CONFIG_ADC_VERSION    1
#endif

/** ADC configuration */
struct adc_config {
#if ADC_NR_OF_CHANNELS > 1
	/* DMA group request is stored in CTRLA */
	uint8_t ctrla;
#endif
	uint8_t ctrlb;
	uint8_t refctrl;
	uint8_t evctrl;
	uint8_t prescaler;
	uint16_t cmp;
};

/**
 * \name Calibration data addresses
 * \note The temperature sensor calibration is sampled at 85 degrees Celsius
 * with unsigned, 12-bit conversion.
 */
/** @{ */

/** ADC A, calibration byte 0. */
#define ADCACAL0      offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0)
/** ADC A, calibration byte 1. */
#define ADCACAL1      offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1)
/** ADC B, calibration byte 0. */
#define ADCBCAL0      offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL0)
/** ADC B, calibration byte 1. */
#define ADCBCAL1      offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL1)
/** Temperature sensor calibration byte 0. */
#define TEMPSENSE0    offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE0)
/** Temperature sensor calibration byte 1. */
#define TEMPSENSE1    offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE1)

/** @} */

/** \brief ADC calibration data */
enum adc_calibration_data {
	ADC_CAL_ADCA,    /**< ADC A pipeline calibration data. */
	ADC_CAL_ADCB,    /**< ADC B pipeline calibration data. */

	/**
	 * \brief Temperature sensor calibration data.
	 * \note 12-bit unsigned, measured at 85 degrees Celsius, equivalent to
	 * 358.15 kelvin.
	 */
	ADC_CAL_TEMPSENSE,
};

/** \name ADC channel masks */
/** @{ */

#define ADC_CH0      (1U << 0)                 /**< ADC channel 0. */

#if XMEGA_A || XMEGA_AU || defined(__DOXYGEN__)
#  define ADC_CH1    (1U << 1)                 /**< ADC channel 1. */
#  define ADC_CH2    (1U << 2)                 /**< ADC channel 2. */
#  define ADC_CH3    (1U << 3)                 /**< ADC channel 3. */
#endif

/** @} */

/** \name Internal ADC input masks */
/** @{ */

#define ADC_INT_TEMPSENSE    ADC_TEMPREF_bm    /**< Temperature sensor. */
#define ADC_INT_BANDGAP      ADC_BANDGAP_bm    /**< Bandgap reference. */

/** @} */

/**
 * \brief ADC conversion trigger settings
 *
 * \note The choice in conversion triggers varies between device families.
 * Refer to the device manual for detailed information.
 */
enum adc_trigger {
	/** Manually triggered conversions */
	ADC_TRIG_MANUAL,

	/**
	 * \brief Freerunning conversion sweeps
	 * \note These will start as soon as the ADC is enabled.
	 */
	ADC_TRIG_FREERUN_SWEEP,

	/**
	 * \brief Event-triggered conversions on individual channels
	 * Pairs each event channel with an ADC channel.
	 * \note The maximum base event channel that can be used is determined
	 * by the number of channels to trigger conversions on.
	 */
	ADC_TRIG_EVENT_SINGLE,

#if ADC_NR_OF_CHANNELS > 1

	/**
	 * \brief Event-triggered conversion sweeps
	 * \note Only the base event channel is used in this mode.
	 */
	ADC_TRIG_EVENT_SWEEP,
#endif

#if XMEGA_A || (CONFIG_ADC_VERSION == 2)

	/**
	 * \brief Event-triggered, synchronized conversion sweeps
	 * \note Only the base event channel is used in this mode.
	 */
	ADC_TRIG_EVENT_SYNCSWEEP,
#endif
};

/** \brief ADC signedness settings */
enum adc_sign {
	ADC_SIGN_OFF,                    /**< Unsigned conversions. */
	ADC_SIGN_ON = ADC_CONMODE_bm,    /**< Signed conversions. */
};

/** \brief ADC resolution settings */
enum adc_resolution {
	/** 8-bit resolution, right-adjusted. */
	ADC_RES_8       = ADC_RESOLUTION_8BIT_gc,
	/** 12-bit resolution, right-adjusted. */
	ADC_RES_12      = ADC_RESOLUTION_12BIT_gc,
	/** 12-bit resolution, left-adjusted. */
	ADC_RES_12_LEFT = ADC_RESOLUTION_LEFT12BIT_gc,
};

/**
 * \brief ADC reference settings
 *
 * \note The choice in voltage reference varies between device families.
 * Refer to the device manual for detailed information.
 */
enum adc_reference {
	/** Internal 1 V from bandgap reference. */
	ADC_REF_BANDGAP  = ADC_REFSEL_INT1V_gc,
	/** VCC divided by 1.6. */
	ADC_REF_VCC      = ADC_REFSEL_VCC_gc,
#if !XMEGA_B3 || defined(__DOXYGEN__)
	/** External reference on AREFA pin. */
	ADC_REF_AREFA    = ADC_REFSEL_AREFA_gc,
#endif
	/** External reference on AREFB pin. */
	ADC_REF_AREFB    = ADC_REFSEL_AREFB_gc,
#if CONFIG_ADC_VERSION == 2
	/** VCC divided by 2. */
	ADC_REF_VCCDIV2 = ADC_REFSEL_VCCDIV2_gc,
#endif
};

/** \name Internal functions for driver */
/** @{ */

/**
 * \internal
 * \brief Get ADC channel pointer from channel mask
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 *
 * \return Pointer to ADC channel
 */
__always_inline ADC_CH_tmpfix_t *adc_get_channel(ADC_t *adc, uint8_t ch_mask);

__always_inline ADC_CH_tmpfix_t *adc_get_channel(ADC_t *adc, uint8_t ch_mask)
{
	uint8_t index = 0;

	Assert(ch_mask & ((1 << ADC_NR_OF_CHANNELS) - 1));

	/* Use a conditional inline ctz for optimzation. */
#if ADC_NR_OF_CHANNELS > 4
	if (!(ch_mask & 0x0f)) {
		index += 4;
		ch_mask >>= 4;
	}
#endif
#if ADC_NR_OF_CHANNELS > 2
	if (!(ch_mask & 0x03)) {
		index += 2;
		ch_mask >>= 2;
	}
#endif
#if ADC_NR_OF_CHANNELS > 1
	if (!(ch_mask & 0x01)) {
		index++;
	}
#endif

	return (ADC_CH_tmpfix_t *)(&adc->CH0 + index);
}

/** @} */

#if defined(CONFIG_ADC_CALLBACK_ENABLE) || defined(__DOXYGEN__)
/** \name ADC interrupt callback function */
/** @{ */

/**
 * \def CONFIG_ADC_CALLBACK_ENABLE
 * \brief Configuration symbol to enable callback on ADC interrupts
 *
 * Define this symbol in \ref conf_adc.h to enable callbacks on ADC interrupts.
 * A function of type \ref adc_callback_t must be defined by the user, and the
 * driver be configured to use it with \ref adc_set_callback.
 */
#if !defined(CONFIG_ADC_CALLBACK_ENABLE) || defined(__DOXYGEN__)
#  define CONFIG_ADC_CALLBACK_ENABLE
#endif

/**
 * \def CONFIG_ADC_CALLBACK_TYPE
 * \brief Configuration symbol for datatype of result parameter for callback
 *
 * Define the datatype of the ADC conversion result parameter for callback
 * functions. This should be defined according to the signedness and resolution
 * of the conversions:
 * - \c int16_t for signed, 12-bit
 * - \c uint16_t for unsigned, 12-bit (the default type)
 * - \c int8_t for signed, 8-bit
 * - \c uint8_t for unsigned, 8-bit
 *
 * Define this in \ref conf_adc.h if the default datatype is not desired.
 */
#if !defined(CONFIG_ADC_CALLBACK_TYPE) || defined(__DOXYGEN__)
#  define CONFIG_ADC_CALLBACK_TYPE    uint16_t
#endif

/** Datatype of ADC conversion result parameter for callback */
typedef CONFIG_ADC_CALLBACK_TYPE adc_result_t;

/**
 * \brief ADC interrupt callback function pointer
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 * \param res ADC conversion result.
 */
typedef void (*adc_callback_t)(ADC_t *adc, uint8_t ch_mask, adc_result_t res);

void adc_set_callback(ADC_t *adc, adc_callback_t callback);

/** @} */
#endif

/** \name ADC module management */
/** @{ */

void adc_enable(ADC_t *adc);
void adc_disable(ADC_t *adc);
bool adc_is_enabled(ADC_t *adc);

/**
 * \brief Start one-shot conversion on ADC channel(s)
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 *
 * \note The ADC must be enabled for this function to have any effect.
 */
static inline void adc_start_conversion(ADC_t *adc, uint8_t ch_mask)
{
	irqflags_t flags = cpu_irq_save();
	adc->CTRLA |= ch_mask << ADC_CH0START_bp;
	cpu_irq_restore(flags);
}

/**
 * \brief Get result from ADC channel
 *
 * Gets the latest conversion result from the ADC channel.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 *
 * \return Latest conversion result of ADC channel. Signedness does not matter.
 *
 * \note This macro does not protect the 16-bit read from interrupts. If an
 * interrupt may do a 16-bit read or write to the ADC while this macro is
 * executing, interrupts \a must be temporarily disabled to avoid corruption of
 * the read.
 */
#define adc_get_result(adc, ch_mask)    (adc_get_channel(adc, ch_mask)->RES)

/**
 * \brief Get signed result from ADC channel
 *
 * Returns the latest conversion result from the ADC channel as a signed type,
 * with interrupt protection of the 16-bit read.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 *
 * \return Latest conversion result of ADC channel, as signed 16-bit integer.
 */
static inline int16_t adc_get_signed_result(ADC_t *adc, uint8_t ch_mask)
{
	int16_t val;
	irqflags_t flags;
	ADC_CH_tmpfix_t *adc_ch;

	adc_ch = adc_get_channel(adc, ch_mask);

	flags = cpu_irq_save();
	val = adc_ch->RES;
	cpu_irq_restore(flags);

	return val;
}

/**
 * \brief Get unsigned result from ADC channel
 *
 * Returns the latest conversion result from the ADC channel as an unsigned
 * type, with interrupt protection of the 16-bit read.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 *
 * \return Latest conversion result of ADC channel, as unsigned 16-bit integer.
 */
static inline uint16_t adc_get_unsigned_result(ADC_t *adc, uint8_t ch_mask)
{
	uint16_t val;
	irqflags_t flags;
	ADC_CH_tmpfix_t *adc_ch;

	adc_ch = adc_get_channel(adc, ch_mask);

	flags = cpu_irq_save();
	val = adc_ch->RES;
	cpu_irq_restore(flags);

	return val;
}

/**
 * \brief Get interrupt flag of ADC channel(s)
 *
 * Returns the interrupt flag of the masked channels. The meaning of the
 * interrupt flag depends on what mode the individual channels are in.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 *
 * \return Mask with interrupt flags.
 */
static inline uint8_t adc_get_interrupt_flag(ADC_t *adc, uint8_t ch_mask)
{
	return (adc->INTFLAGS >> ADC_CH0IF_bp) & ch_mask;
}

/**
 * \brief Clear interrupt flag of ADC channel(s)
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 *
 * \note The ADC must be enabled for this function to have any effect.
 */
static inline void adc_clear_interrupt_flag(ADC_t *adc, uint8_t ch_mask)
{
	adc->INTFLAGS = ch_mask << ADC_CH0IF_bp;
}

/**
 * \brief Wait for interrupt flag of ADC channel(s)
 *
 * Waits for the interrupt flag of the specified channel(s) to be set, then
 * clears it before returning. If several channels are masked, the function will
 * wait for \a all interrupt flags to be set.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 */
static inline void adc_wait_for_interrupt_flag(ADC_t *adc, uint8_t ch_mask)
{
	do { } while (adc_get_interrupt_flag(adc, ch_mask) != ch_mask);
	adc_clear_interrupt_flag(adc, ch_mask);
}

/**
 * \brief Flush the ADC
 *
 * Forces the ADC to abort any ongoing conversions and restart its clock on the
 * next peripheral clock cycle. Pending conversions are started after the
 * clock reset.
 *
 * \param adc Pointer to ADC module.
 *
 * \note The ADC must be enabled for this function to have any effect.
 */
static inline void adc_flush(ADC_t *adc)
{
	irqflags_t flags = cpu_irq_save();
	adc->CTRLA |= ADC_FLUSH_bm;
	cpu_irq_restore(flags);
}

/**
 * \brief Set compare value directly to ADC
 *
 * Sets the compare value directly to the ADC, for quick access while the ADC is
 * enabled.
 *
 * \param adc Pointer to ADC module.
 * \param val Compare value to set, either signed or unsigned.
 *
 * \note The ADC must be enabled for this function to have any effect.
 */
#define adc_set_compare_value(adc, val)\
	do {\
		irqflags_t ATPASTE2(adc_flags, __LINE__) =  cpu_irq_save();\
		(adc)->CMP = val;\
		cpu_irq_restore(ATPASTE2(adc_flags, __LINE__));\
	}\
	while (0)

/**
 * \brief Get compare value directly from ADC
 *
 * Gets the compare value directly from the ADC, for quick access while the ADC
 * is enabled.
 *
 * \param adc Pointer to ADC module.
 *
 * \return Current compare value of the ADC. Signedness does not matter.
 *
 * \note This macro does not protect the 16-bit read from interrupts. If an
 * interrupt may do a 16-bit read or write to the ADC while this macro is
 * executing, interrupts \a must be temporarily disabled to avoid corruption of
 * the read.
 */
#define adc_get_compare_value(adc)    ((adc)->CMP)

/**
 * \brief Get signed compare value directly from ADC
 *
 * Gets the signed compare value directly from the ADC, with interrupt
 * protection of the 16-bit read, for quick access while the ADC is enabled.
 *
 * \param adc Pointer to ADC module.
 */
static inline int16_t adc_get_signed_compare_value(ADC_t *adc)
{
	int16_t val;
	irqflags_t flags;

	flags = cpu_irq_save();
	val = adc->CMP;
	cpu_irq_restore(flags);

	return val;
}

/**
 * \brief Get unsigned compare value directly from ADC
 *
 * Gets the unsigned compare value directly from the ADC, with interrupt
 * protection of the 16-bit read, for quick access while the ADC is enabled.
 *
 * \param adc Pointer to ADC module.
 */
static inline uint16_t adc_get_unsigned_compare_value(ADC_t *adc)
{
	uint16_t val;
	irqflags_t flags;

	flags = cpu_irq_save();
	val = adc->CMP;
	cpu_irq_restore(flags);

	return val;
}

/**
 * \brief Get calibration data
 *
 * \param cal Identifier for calibration data to get.
 */
static inline uint16_t adc_get_calibration_data(enum adc_calibration_data cal)
{
	uint16_t data;

	switch (cal) {
#ifdef ADCA
	case ADC_CAL_ADCA:
		data = nvm_read_production_signature_row(ADCACAL1);
		data <<= 8;
		data |= nvm_read_production_signature_row(ADCACAL0);
		break;
#endif

#ifdef ADCB
	case ADC_CAL_ADCB:
		data = nvm_read_production_signature_row(ADCBCAL1);
		data <<= 8;
		data |= nvm_read_production_signature_row(ADCBCAL0);
		break;
#endif

#if defined(ADCA) || defined(ADCB)
	case ADC_CAL_TEMPSENSE:
		data = nvm_read_production_signature_row(TEMPSENSE1);
		data <<= 8;
		data |= nvm_read_production_signature_row(TEMPSENSE0);
		break;
#endif

	default:
		Assert(0);
		data = 0;
	}

	return data;
}

/** @} */

/** \name ADC module configuration */
/** @{ */

void adc_write_configuration(ADC_t *adc, const struct adc_config *conf);
void adc_read_configuration(ADC_t *adc, struct adc_config *conf);

/**
 * \brief Set ADC prescaler to get desired clock rate
 *
 * Sets the ADC prescaling so that its clock rate becomes _at most_
 * \a clk_adc_hz. This is done by computing the ratio of the peripheral clock
 * rate to the desired ADC clock rate, and rounding it upward to the nearest
 * prescaling factor.
 *
 * \param conf Pointer to ADC module configuration.
 * \param clk_adc Desired ADC clock rate.
 *
 * \note The sample rate is not determined solely by the ADC clock rate for all
 * devices. Setting the current limit mode on some devices will also affect the
 * maximum ADC sampling rate. Refer to the device manual for detailed
 * information on conversion timing and/or the current limitation mode.
 */
static inline void adc_set_clock_rate(struct adc_config *conf, uint32_t clk_adc)
{
	uint32_t clk_per;
	uint16_t ratio;
	uint8_t psc;

	Assert(clk_adc);
#if XMEGA_A || XMEGA_AU
	Assert(clk_adc <= 2000000UL);
#elif XMEGA_B || XMEGA_D
	Assert(clk_adc <= 1400000UL);
#elif XMEGA_C
	Assert(clk_adc <= 1800000UL);
#endif

	clk_per = sysclk_get_per_hz();
	ratio = clk_per / clk_adc;

	/* Round ratio up to the nearest prescaling factor. */
	if (ratio <= 4) {
		psc = ADC_PRESCALER_DIV4_gc;
	} else if (ratio <= 8) {
		psc = ADC_PRESCALER_DIV8_gc;
	} else if (ratio <= 16) {
		psc = ADC_PRESCALER_DIV16_gc;
	} else if (ratio <= 32) {
		psc = ADC_PRESCALER_DIV32_gc;
	} else if (ratio <= 64) {
		psc = ADC_PRESCALER_DIV64_gc;
	} else if (ratio <= 128) {
		psc = ADC_PRESCALER_DIV128_gc;
	} else if (ratio <= 256) {
		psc = ADC_PRESCALER_DIV256_gc;
	} else {
		psc = ADC_PRESCALER_DIV512_gc;
	}

	conf->prescaler = psc;
}

/**
 * \brief Set ADC conversion parameters
 *
 * Sets the signedness, resolution and voltage reference for conversions in the
 * ADC module configuration.
 *
 * \param conf Pointer to ADC module configuration.
 * \param sign Conversion signedness.
 * \param res Resolution of conversions.
 * \param ref Voltage reference to use.
 */
static inline void adc_set_conversion_parameters(struct adc_config *conf,
		enum adc_sign sign, enum adc_resolution res,
		enum adc_reference ref)
{
	/* Preserve all but conversion and resolution config. */
	conf->ctrlb &= ~(ADC_CONMODE_bm | ADC_RESOLUTION_gm);
	conf->ctrlb |= (uint8_t)res | (uint8_t)sign;

	conf->refctrl &= ~ADC_REFSEL_gm;
	conf->refctrl |= ref;
}

/**
 * \brief Set ADC conversion trigger
 *
 * Configures the conversion triggering of the ADC.
 *
 * For automatic triggering modes, the number of channels to start conversions
 * on must be specified with \a nr_of_ch. The channel selection for these
 * modes is incrementally inclusive, always starting with channel 0.
 *
 * For event triggered modes, the base event channel must also be specified with
 * \a base_ev_ch. The event channels are assigned to the ADC channels in an
 * incremental fashion \a without \a wrap-around (in single-trigger event mode).
 * This means that the maximum base event channel that can be used is determined
 * by the number of ADC channels to start conversions on, i.e., \a nr_of_ch.
 *
 * \param conf Pointer to ADC module configuration.
 * \param trig Conversion trigger to set.
 * \param nr_of_ch Number of ADC channels to trigger conversions on:
 * \arg \c 1 - \c ADC_NR_OF_CHANNELS (must be non-zero).
 * \param base_ev_ch Base event channel, if used.
 */
static inline void adc_set_conversion_trigger(struct adc_config *conf,
		enum adc_trigger trig, uint8_t nr_of_ch, uint8_t base_ev_ch)
{
	Assert(nr_of_ch);
	Assert(nr_of_ch <= ADC_NR_OF_CHANNELS);
#if XMEGA_A || XMEGA_AU
	Assert(base_ev_ch <= 7);
#elif XMEGA_B || XMEGA_C || XMEGA_D
	Assert(base_ev_ch <= 3);
#endif

	switch (trig) {
	case ADC_TRIG_MANUAL:
		conf->ctrlb &= ~ADC_FREERUN_bm;
		conf->evctrl = ADC_EVACT_NONE_gc;
		break;

	case ADC_TRIG_EVENT_SINGLE:
		conf->ctrlb &= ~ADC_FREERUN_bm;
		conf->evctrl = (base_ev_ch << ADC_EVSEL_gp) |
				(nr_of_ch << ADC_EVACT_gp);
		break;

#if ADC_NR_OF_CHANNELS > 1
	case ADC_TRIG_FREERUN_SWEEP:
		conf->ctrlb |= ADC_FREERUN_bm;
		conf->evctrl = (nr_of_ch - 1) << ADC_SWEEP_gp;
		break;

	case ADC_TRIG_EVENT_SWEEP:
		conf->ctrlb &= ~ADC_FREERUN_bm;
		conf->evctrl = (nr_of_ch - 1) << ADC_SWEEP_gp |
				(base_ev_ch << ADC_EVSEL_gp) |
				ADC_EVACT_SWEEP_gc;
		break;
#endif
#if XMEGA_A || (CONFIG_ADC_VERSION == 2)
	case ADC_TRIG_EVENT_SYNCSWEEP:
		conf->ctrlb &= ~ADC_FREERUN_bm;
		conf->evctrl =
#  if ADC_NR_OF_CHANNELS > 1
				((nr_of_ch - 1) << ADC_SWEEP_gp) |
#  endif
				(base_ev_ch << ADC_EVSEL_gp) |
				ADC_EVACT_SYNCHSWEEP_tmpfix_gc;
		break;
#endif

	default:
		Assert(0);
	}
}

#if ADC_NR_OF_CHANNELS > 1

/**
 * \brief Set DMA request group
 *
 * Configures the DMA group request for the specified number of ADC channels.
 * The channel group selection is incrementally inclusive, always starting with
 * channel 0.
 *
 * \param conf Pointer to ADC module configuration.
 * \param nr_of_ch Number of channels for group request:
 * \arg 0 to disable.
 * \arg 2, 3 or 4 to enable.
 *
 * \note The number of channels in the DMA request group cannot be 1.
 * \note Not all device families feature this setting.
 */
static inline void adc_set_dma_request_group(struct adc_config *conf,
		uint8_t nr_of_ch)
{
	Assert(nr_of_ch <= ADC_NR_OF_CHANNELS);
	Assert(nr_of_ch != 1);

	if (nr_of_ch) {
		conf->ctrla = (nr_of_ch - 1) << ADC_DMASEL_gp;
	} else {
		conf->ctrla = ADC_DMASEL_OFF_gc;
	}
}

#endif

/**
 * \brief Enable internal ADC input
 *
 * \param conf Pointer to ADC module configuration.
 * \param int_inp Internal input to enable:
 * \arg \c ADC_INT_TEMPSENSE for temperature sensor.
 * \arg \c ADC_INT_BANDGAP for bandgap reference.
 */
static inline void adc_enable_internal_input(struct adc_config *conf,
		uint8_t int_inp)
{
	conf->refctrl |= int_inp;
}

/**
 * \brief Disable internal ADC input
 *
 * \param conf Pointer to ADC module configuration.
 * \param int_inp Internal input to disable:
 * \arg \c ADC_INT_TEMPSENSE for temperature sensor.
 * \arg \c ADC_INT_BANDGAP for bandgap reference.
 */
static inline void adc_disable_internal_input(struct adc_config *conf,
		uint8_t int_inp)
{
	conf->refctrl &= ~int_inp;
}

#if XMEGA_AU || defined(__DOXYGEN__)
/** \brief ADC gain stage impedance settings */
enum adc_gainstage_impmode {
	/** High impedance sources */
	ADC_GAIN_HIGHIMPEDANCE,
	/** Low impedance sources */
	ADC_GAIN_LOWIMPEDANCE,
};

/**
 * \brief Set ADC gain stage impedance mode
 *
 * \param conf Pointer to ADC module configuration.
 * \param mode Gain stage impedance mode.
 *
 * \note Not all device families feature this setting.
 */
static inline void adc_set_gain_impedance_mode(struct adc_config *conf,
		enum adc_gainstage_impmode impmode)
{
	switch (impmode) {
	case ADC_GAIN_HIGHIMPEDANCE:
		conf->ctrlb &= ~ADC_IMPMODE_bm;
		break;

	case ADC_GAIN_LOWIMPEDANCE:
		conf->ctrlb |= ADC_IMPMODE_bm;
		break;

	default:
		Assert(0);
	}
}

#endif

#if CONFIG_ADC_VERSION == 2
/** \brief ADC current limit settings */
enum adc_current_limit {
	/** No current limit */
	ADC_CURRENT_LIMIT_NO,
	/** Low current limit, max sampling rate 1.5 MSPS */
	ADC_CURRENT_LIMIT_LOW,
	/** Medium current limit, max sampling rate 1 MSPS */
	ADC_CURRENT_LIMIT_MED,
	/** High current limit, max sampling rate 0.5 MSPS */
	ADC_CURRENT_LIMIT_HIGH
};

/**
 * \brief Set ADC current limit
 *
 * Set the current limit mode for the ADC module. This setting affects the max
 * sampling rate of the ADC.
 *
 * \note See the device datasheet and manual for detailed information about
 * current consumption and sample rate limit.
 *
 * \param conf Pointer to ADC module configuration.
 * \param currlimit Current limit setting.
 *
 * \note Not all device families feature this setting.
 */
static inline void adc_set_current_limit(struct adc_config *conf,
		enum adc_current_limit currlimit)
{
	conf->ctrlb &= ~ADC_CURRLIMIT_tmpfix_gm;

	switch (currlimit) {
	case ADC_CURRENT_LIMIT_NO:
		conf->ctrlb |= ADC_CURRLIMIT_NO_tmpfix_gc;
		break;

	case ADC_CURRENT_LIMIT_LOW:
		conf->ctrlb |= ADC_CURRLIMIT_SMALL_tmpfix_gc;
		break;

	case ADC_CURRENT_LIMIT_MED:
		conf->ctrlb |= ADC_CURRLIMIT_MEDIUM_tmpfix_gc;
		break;

	case ADC_CURRENT_LIMIT_HIGH:
		conf->ctrlb |= ADC_CURRLIMIT_LARGE_tmpfix_gc;
		break;

	default:
		Assert(0);
	}
}

#endif

/**
 * \brief Set ADC compare value in configuration
 *
 * \param conf Pointer to ADC module configuration.
 * \param val Compare value to set.
 */
#define adc_set_config_compare_value(conf, val)\
	do {\
		conf->cmp = (uint16_t)val;\
	}\
	while (0)

/**
 * \brief Get ADC compare value from configuration
 *
 * \param conf Pointer to ADC module configuration.
 */
#define adc_get_config_compare_value(conf)    (conf->cmp)

/** @} */

/** @} */

/**
 * \defgroup adc_channel_group ADC channel
 *
 * Management and configuration functions for the individual ADC channels.
 *
 * The API functions and definitions can be divided in two groups:
 * - channel management: direct access for getting conversion result.
 * - channel configuration: create/change configurations and write/read them
 * to/from ADC channels.
 *
 * @{
 */

/**
 * \brief Default ADC channel interrupt level
 *
 * \note To override the channel interrupt level, define this symbol as the
 * desired level in \ref conf_adc.h.
 */
#if !defined(CONFIG_ADC_INTLVL) || defined(__DOXYGEN__)
#  define CONFIG_ADC_INTLVL    ADC_CH_INTLVL_LO_gc
#endif

/** ADC channel configuration */
struct adc_channel_config {
	uint8_t ctrl;
	uint8_t muxctrl;
	uint8_t intctrl;
#if CONFIG_ADC_VERSION == 2
	uint8_t scan;
#endif
};

/**
 * \brief ADC channel positive input
 *
 * Identifies the external and internal signals that can be used as positive
 * input to the ADC channels.
 *
 * \note Some devices can use pins on an alternate port. Refer to the device
 * datasheet for information on the number of available ADC inputs and their
 * mapping to pins.
 */
enum adcch_positive_input {
	ADCCH_POS_PIN0,
	ADCCH_POS_PIN1,
	ADCCH_POS_PIN2,
	ADCCH_POS_PIN3,
	ADCCH_POS_PIN4,
	ADCCH_POS_PIN5,
	ADCCH_POS_PIN6,
	ADCCH_POS_PIN7,

	/** \name Input pins on alternate port. */
	/** @{ */
#if XMEGA_A3U || XMEGA_A4 || XMEGA_A4U || XMEGA_B\
	|| XMEGA_C || XMEGA_D || defined(__DOXYGEN__)
	/* A3U, A4, A4U, B, C and D feature ADC8:11 input pins. */
	ADCCH_POS_PIN8,
	ADCCH_POS_PIN9,
	ADCCH_POS_PIN10,
	ADCCH_POS_PIN11,
#endif
#if XMEGA_A3U || XMEGA_B || XMEGA_C || XMEGA_D3 || defined(__DOXYGEN__)
	/* A3U, B, C and D3 feature ADC12:15 input pins. */
	ADCCH_POS_PIN12,
	ADCCH_POS_PIN13,
	ADCCH_POS_PIN14,
	ADCCH_POS_PIN15,
#endif
	/** @} */

	/** \name Internal inputs. */
	/** @{ */
	ADCCH_POS_TEMPSENSE,     /**< Temperature sensor. */
	ADCCH_POS_BANDGAP,       /**< Bandgap reference. */
	ADCCH_POS_SCALED_VCC,    /**< VCC scaled down by 10. */
#if XMEGA_A || XMEGA_AU || defined(__DOXYGEN__)
	ADCCH_POS_DAC,           /**< DAC output. */
#endif
	/** @} */
};

/**
 * \brief ADC channel negative input
 *
 * Identifies the signals that can be used as negative input to the ADC channels
 * in differential mode. Some of the input signals are only available with
 * certain gain settings, e.g., 1x gain.
 *
 * \note The ADC must be set in signed mode to use differential measurements.
 * For single-ended measurements, ADDCH_NEG_NONE should be specified as negative
 * input.
 *
 * \note Pad and internal GND are not available on all devices. See the device
 * manual for an overview of available input signals.
 */
enum adcch_negative_input {
	/** \name Input pins for differential measurements with 1x gain. */
	/** @{ */
	/** ADC0 pin */
	ADCCH_NEG_PIN0,
	/** ADC1 pin */
	ADCCH_NEG_PIN1,
	/** ADC2 pin */
	ADCCH_NEG_PIN2,
	/** ADC3 pin */
	ADCCH_NEG_PIN3,
	/** @} */

	/** \name Input pins for differential measurements with any gain. */
	/** @{ */
	/** ADC4 pin */
	ADCCH_NEG_PIN4,
	/** ADC5 pin */
	ADCCH_NEG_PIN5,
	/** ADC6 pin */
	ADCCH_NEG_PIN6,
	/** ADC7 pin */
	ADCCH_NEG_PIN7,
	/** @} */

#if CONFIG_ADC_VERSION == 2
	/** \name GND signals for differential measurements. */
	/** @{ */
	/** PAD ground */
	ADCCH_NEG_PAD_GND,
	/** Internal ground */
	ADCCH_NEG_INTERNAL_GND,
	/** @} */
#endif

	/** Single ended mode */
	ADCCH_NEG_NONE,
};

/** \brief ADC channel interrupt modes */
enum adcch_mode {
	/** Set interrupt flag when conversions complete. */
	ADCCH_MODE_COMPLETE = ADC_CH_INTMODE_COMPLETE_gc,
	/** Set interrupt flag when conversion result is below compare value. */
	ADCCH_MODE_BELOW    = ADC_CH_INTMODE_BELOW_gc,
	/** Set interrupt flag when conversion result is above compare value. */
	ADCCH_MODE_ABOVE    = ADC_CH_INTMODE_ABOVE_gc,
};

/** \name ADC channel configuration */
/** @{ */

void adcch_write_configuration(ADC_t *adc, uint8_t ch_mask,
		const struct adc_channel_config *ch_conf);
void adcch_read_configuration(ADC_t *adc, uint8_t ch_mask,
		struct adc_channel_config *ch_conf);

/** Force enabling of gainstage with unity gain. */
#define ADCCH_FORCE_1X_GAINSTAGE 0xff

/**
 * \internal
 * \brief Get ADC channel setting for specified gain
 *
 * Returns the setting that corresponds to specified gain.
 *
 * \param gain Valid gain factor for the measurement.
 *
 * \return Gain setting of type ADC_CH_GAIN_t.
 */
static inline uint8_t adcch_get_gain_setting(uint8_t gain)
{
	switch (gain) {
#if (CONFIG_ADC_VERSION == 2) || XMEGA_D
	case 0:
		return ADC_CH_GAIN_DIV2_tmpfix_gc;
#endif
	case 1:
		return ADC_CH_GAIN_1X_gc;

	case 2:
		return ADC_CH_GAIN_2X_gc;

	case 4:
		return ADC_CH_GAIN_4X_gc;

	case 8:
		return ADC_CH_GAIN_8X_gc;

	case 16:
		return ADC_CH_GAIN_16X_gc;

	case 32:
		return ADC_CH_GAIN_32X_gc;

	case 64:
		return ADC_CH_GAIN_64X_gc;

	case ADCCH_FORCE_1X_GAINSTAGE:
		return ADC_CH_GAIN_1X_gc;

	default:
		Assert(0);
		return 0;
	}
}

/**
 * \brief Set ADC channel input mode, multiplexing and gain
 *
 * Sets up an ADC channel's input mode and multiplexing according to specified
 * input signals, as well as the gain.
 *
 * \param ch_conf Pointer to ADC channel configuration.
 * \param pos Positive input signal.
 * \param neg Negative input signal:
 * \arg \c ADCCH_NEG_NONE for single-ended measurements.
 * \arg \c ADCCH_NEG_PINn , where \c n specifies a pin, for differential
 * measurements.
 * \arg \c ADDCH_x_GND , where \c x specified pad or internal GND, for
 * differential measurements.
 * \param gain Gain factor for measurements:
 * \arg 1 for single-ended or differential with pin 0, 1, 2 or 3, pad or
 * internal GND as negative
 * input.
 * \arg 0 (0.5x), 1, 2, 4, 8, 16, 32 or 64 for differential with pin 4, 5, 6 or
 * 7, pad or internal GND as negative input.
 * \arg ADCCH_FORCE_1X_GAINSTAGE to force the gain stage to be enabled with
 * unity gain for differential measurement.
 *
 * \note The 0.5x gain factor, selected by setting gain to 0, is not available
 * on all devices. Refer to the device manual for information on available gain
 * factors.
 *
 * \note The GND signals are not available on all devices. Refer to the device
 * manual for information on available input signals.
 *
 * \note With unity (1x) gain, some input selections may be possible both with
 * and without the gain stage enabled. The driver will default to the
 * configuration without gainstage to keep the current consumption as low as
 * possible unless the user specifies \ref ADCCH_FORCE_1X_GAINSTAGE as \a gain.
 */
static inline void adcch_set_input(struct adc_channel_config *ch_conf,
		enum adcch_positive_input pos, enum adcch_negative_input neg,
		uint8_t gain)
{
#if (CONFIG_ADC_VERSION == 2) || XMEGA_D
	/* A gain value of 0 on AU, B or D devices is 1/2 gain */
#else
	/* Zero gain is not applicable on other devices */
	Assert(gain);
#endif

	/* Configure for internal input. */
	if (pos >= ADCCH_POS_TEMPSENSE) {
		Assert(gain == 1);
		Assert(neg == ADCCH_NEG_NONE);

		ch_conf->ctrl = ADC_CH_INPUTMODE_INTERNAL_gc;
		ch_conf->muxctrl = (pos - ADCCH_POS_TEMPSENSE) <<
				ADC_CH_MUXPOS_gp;
		return;
	}

	/* Configure for single-ended measurement. */
	if (neg == ADCCH_NEG_NONE) {
		Assert(gain == 1);

		ch_conf->ctrl = ADC_CH_INPUTMODE_SINGLEENDED_gc;
		ch_conf->muxctrl = pos << ADC_CH_MUXPOS_gp;

		/* Configure for differential measurement. */
	} else {
		/* Pins 0-3 can only be used for negative input if the gain
		 * stage is not used, i.e., unity gain.
		 */
		if (neg <= ADCCH_NEG_PIN3) {
			Assert(gain == 1);

			ch_conf->ctrl = ADC_CH_INPUTMODE_DIFF_gc;
			ch_conf->muxctrl = (pos << ADC_CH_MUXPOS_gp) |
					(neg << ADC_CH_MUXNEG_gp);
		} else if (neg <= ADCCH_NEG_PIN7) {
			/* Pins 4-7 can be used for all gain settings, including
			 * unity
			 * gain, which is available even if the gain stage is
			 *active.
			 */
			ch_conf->ctrl = ADC_CH_INPUTMODE_DIFFWGAIN_gc |
					adcch_get_gain_setting(gain);
			ch_conf->muxctrl = (pos << ADC_CH_MUXPOS_gp) |
					((neg - ADCCH_NEG_PIN4) <<
					ADC_CH_MUXNEG_gp);
		}

#if CONFIG_ADC_VERSION == 2
		else {
			/* The bitmasks for the on-chip GND signals change when
			 * gain is
			 * enabled. To avoid unnecessary current consumption, do
			 *not
			 * enable gainstage for unity gain unless user
			 *explicitly
			 * specifies it with the ADCCH_FORCE_1X_GAINSTAGE macro.
			 */
			if (gain == 1) {
				ch_conf->ctrl = ADC_CH_INPUTMODE_DIFF_gc;
				neg = (neg == ADCCH_NEG_PAD_GND) ?
						ADCCH_NEG_PIN5 : ADCCH_NEG_PIN7;
                                                
			} else {
				ch_conf->ctrl = ADC_CH_INPUTMODE_DIFFWGAIN_gc |
						adcch_get_gain_setting(gain);
				neg = (neg == ADCCH_NEG_INTERNAL_GND) ?
						ADCCH_NEG_PIN4 : ADCCH_NEG_PIN7;
			}

			ch_conf->muxctrl = (pos << ADC_CH_MUXPOS_gp) |
					(neg << ADC_CH_MUXNEG_gp);
		}
#endif
	}
}

#if CONFIG_ADC_VERSION == 2

/**
 * \brief Set ADC channel 0 pin scan
 *
 * Sets the parameters for pin scan, which enables measurements on multiple,
 * successive input pins without any reconfiguration between conversions.
 *
 * Pin scan works by adding a offset to the positive MUX setting to get the
 * current input pin. The offset is incremented for each conversion, and is
 * reset to 0 once a conversion with the maximum offset is done.
 *
 * \param start_offset Initial offset to start pin scan at
 * \arg \c 0 - \c max_offset
 * \param max_offset Maximum offset for the pin scan
 * \arg \c 0 to disable
 * \arg \c 1 - \c 15 to enable
 *
 * \note Only the AVR XMEGA AU family features this setting.
 * \note Pin scan is only available on ADC channel 0.
 */
static inline void adcch_set_pin_scan(struct adc_channel_config *ch_conf,
		uint8_t start_offset, uint8_t max_offset)
{
	Assert(start_offset < 16);
	Assert(max_offset < 16);
	Assert(start_offset <= max_offset);

	ch_conf->scan = max_offset | (start_offset << ADC_CH_OFFSET_gp);
}

#endif

/**
 * \brief Set ADC channel interrupt mode
 *
 * \param ch_conf Pointer to ADC channel configuration.
 * \param mode Interrupt mode to set.
 */
static inline void adcch_set_interrupt_mode(struct adc_channel_config *ch_conf,
		enum adcch_mode mode)
{
	ch_conf->intctrl &= ~ADC_CH_INTMODE_gm;
	ch_conf->intctrl |= mode;
}

/**
 * \brief Enable interrupts on ADC channel
 *
 * \param ch_conf Pointer to ADC channel configuration.
 */
static inline void adcch_enable_interrupt(struct adc_channel_config *ch_conf)
{
	ch_conf->intctrl &= ~ADC_CH_INTLVL_gm;
	ch_conf->intctrl |= CONFIG_ADC_INTLVL;
}

/**
 * \brief Disable interrupts on ADC channel
 *
 * \param ch_conf Pointer to ADC channel configuration.
 */
static inline void adcch_disable_interrupt(struct adc_channel_config *ch_conf)
{
	ch_conf->intctrl &= ~ADC_CH_INTLVL_gm;
	ch_conf->intctrl |= ADC_CH_INTLVL_OFF_gc;
}

/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADC_H */
