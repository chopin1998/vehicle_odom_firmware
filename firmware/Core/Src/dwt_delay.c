#include "main.h"
#include "dwt_delay.h"

void dwt_delay_init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

void dwt_delay_us(uint32_t n)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t end = DWT->CYCCNT + n * (SystemCoreClock / 1000000);

    if (end > start)
    {
        while (DWT->CYCCNT < end);
    }
    else
    {
        while (DWT->CYCCNT > start || DWT->CYCCNT < end);
    }
}