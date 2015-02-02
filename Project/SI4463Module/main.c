
#include "contiki.h"
#include "lib/random.h"
#include "bsp.h"
#include "radio.h"
#include "packetsbuf.h"
#include "global.h"
/*---------------------------------------------------------------------------*/
#if CLOCK_CONF_STACK_FRIENDLY
extern volatile __bit sleep_flag;
#endif
/*---------------------------------------------------------------------------*/
static  int r;

/*---------------------------------------------------------------------------*/
#if ENERGEST_CONF_ON
static unsigned long irq_energest = 0;
#define ENERGEST_IRQ_SAVE(a) do { \
    a = energest_type_time(ENERGEST_TYPE_IRQ); } while(0)
#define ENERGEST_IRQ_RESTORE(a) do { \
    energest_type_set(ENERGEST_TYPE_IRQ, a); } while(0)
#else
#define ENERGEST_IRQ_SAVE(a) do {} while(0)
#define ENERGEST_IRQ_RESTORE(a) do {} while(0)
#endif

void main(void)
{
  /* Hardware initialization */
  eepromInit();
  clockInit();
  gpioInit();
  timerInit();
  spiInit();
  uartInit();
  rtimer_init();
  vRadio_Init();
  pktbuf_init();
  globalInit();
  //stack_poison();

  /* Init LEDs here */
  //leds_init();
  //leds_off(LEDS_ALL);
  //fade(LEDS_GREEN);

  /* initialize process manager. */
  process_init();

#if DMA_ON
  //dma_init();
#endif

  //io_arch_init();

  //watchdog_init();

  /* Initialise the H/W RNG engine. */
  random_init(0);

  /* start services */
  process_start(&etimer_process, NULL);
  ctimer_init();

  /* initialize the netstack */
  //netstack_init();
  //set_rime_addr();
  ENABLE_INTERRUPTS();
#if BUTTON_SENSOR_ON || ADC_SENSOR_ON
  process_start(&sensors_process, NULL);
  BUTTON_SENSOR_ACTIVATE();
  ADC_SENSOR_ACTIVATE();
#endif
  autostart_start(autostart_processes);
  watchdog_start();
  while(1) {
    do {
      /* Reset watchdog and handle polls and events */
      watchdog_periodic();

#if CLOCK_CONF_STACK_FRIENDLY
      if(sleep_flag) {
        if(etimer_pending() &&
            (etimer_next_expiration_time() - clock_time() - 1) > MAX_TICKS) {
          etimer_request_poll();
        }
        sleep_flag = 0;
      }
#endif
      r = process_run();
    } while(r > 0);
    /*len = NETSTACK_RADIO.pending_packet();
    if(len) {
      packetbuf_clear();
      len = NETSTACK_RADIO.read(packetbuf_dataptr(), PACKETBUF_SIZE);
      if(len > 0) {
        packetbuf_set_datalen(len);
        NETSTACK_RDC.input();
      }
    }*/

#if 0//LPM_MODE
#if (LPM_MODE==LPM_MODE_PM2)
    SLEEP &= ~OSC_PD;            /* Make sure both HS OSCs are on */
    while(!(SLEEP & HFRC_STB));  /* Wait for RCOSC to be stable */
    CLKCON |= OSC;               /* Switch to the RCOSC */
    while(!(CLKCON & OSC));      /* Wait till it's happened */
    SLEEP |= OSC_PD;             /* Turn the other one off */
#endif /* LPM_MODE==LPM_MODE_PM2 */

    /*
     * Set MCU IDLE or Drop to PM1. Any interrupt will take us out of LPM
     * Sleep Timer will wake us up in no more than 7.8ms (max idle interval)
     */
    SLEEPCMD = (SLEEPCMD & 0xFC) | (LPM_MODE - 1);

#if (LPM_MODE==LPM_MODE_PM2)
    /*
     * Wait 3 NOPs. Either an interrupt occurred and SLEEP.MODE was cleared or
     * no interrupt occurred and we can safely power down
     */
    __asm
      nop
      nop
      nop
    __endasm;

    if(SLEEPCMD & SLEEP_MODE0) {
#endif /* LPM_MODE==LPM_MODE_PM2 */

      ENERGEST_OFF(ENERGEST_TYPE_CPU);
      ENERGEST_ON(ENERGEST_TYPE_LPM);

      /* We are only interested in IRQ energest while idle or in LPM */
      ENERGEST_IRQ_RESTORE(irq_energest);

      /* Go IDLE or Enter PM1 */
      PCON |= PCON_IDLE;

      /* First instruction upon exiting PM1 must be a NOP */
      __asm
        nop
      __endasm;

      /* Remember energest IRQ for next pass */
      ENERGEST_IRQ_SAVE(irq_energest);

      ENERGEST_ON(ENERGEST_TYPE_CPU);
      ENERGEST_OFF(ENERGEST_TYPE_LPM);

#if (LPM_MODE==LPM_MODE_PM2)
      SLEEPCMD &= ~SLEEP_OSC_PD;            /* Make sure both HS OSCs are on */
      while(!(SLEEPCMD & SLEEP_XOSC_STB));  /* Wait for XOSC to be stable */
      CLKCONCMD &= ~CLKCONCMD_OSC;              /* Switch to the XOSC */
      /*
       * On occasion the XOSC is reported stable when in reality it's not.
       * We need to wait for a safeguard of 64us or more before selecting it
       */
      clock_delay_usec(65);
      while(CLKCONCMD & CLKCONCMD_OSC);         /* Wait till it's happened */
    }
#endif /* LPM_MODE==LPM_MODE_PM2 */
#endif /* LPM_MODE */
  }

}