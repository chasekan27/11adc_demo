#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/drivers/adc.h>
static const int32_t sleep_time = 1000;

//get device tree configurations
#define MY_ADC_CH DT_ALIAS(myadcchannel)
static const struct device *adc = DEVICE_DT_GET(DT_ALIAS(myadc));
static const struct adc_channel_cfg adc_ch = ADC_CHANNEL_CFG_DT(MY_ADC_CH);
int main(void)
{
    int ret;
    uint16_t buf;
    uint16_t val_mv;
    int32_t vref_mv;

    vref_mv = DT_PROP(MY_ADC_CH, zephyr_vref_mv);

    struct adc_sequence seq = {
        .channels = BIT(adc_ch.channel_id),
        .buffer = &buf,
        .buffer_size = sizeof(buf),
        .resolution = DT_PROP(MY_ADC_CH, zephyr_resolution),
    };

    if (!device_is_ready(adc)) {
        printf("ADC device not ready\n");
        return 1;
    }

    ret = adc_channel_setup(adc, &adc_ch);
    if (ret < 0) {
        printf("ADC channel setup failed\n");
        return 1;
    }

    while (1) {
        ret = adc_read(adc, &seq);
        if (ret < 0) {
            printf("ADC read failed\n");
            continue;
        }
        val_mv = buf;
        int32_t voltage_mv = val_mv * vref_mv / (1 << seq.resolution);

        printf("ADC Channel %d: Raw Value = %d, Voltage = %d mV\n",
               adc_ch.channel_id, val_mv, voltage_mv);
        k_sleep(K_MSEC(sleep_time));
    }
}
