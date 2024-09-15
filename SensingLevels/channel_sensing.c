#include "contiki.h"
#include "CC2420.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define RANGE_CHANNELS 16
static int dBm_channels[RANGE_CHANNELS];

int get_rssi_dBm_from_channel(int channel)
{  
    int current_RSSI_dBm;    
    
    cc2420_on();

    cc2420_set_channel(channel);

    // uncomment for testing in cooja
    // upper= 0;
    // lower= -100;
    // num = (rand() % (upper - lower + 1)) + lower;
    // current_RSSI_dBm= num;

    // comment for testing in cooja
    current_RSSI_dBm = cc2420_rssi();

    cc2420_off();

    return current_RSSI_dBm;
}

void select_best_channel(int rssi_values_dBm[])
{
    int temp_best = 0;
    int best_channel = 0;
    int i;  
    for (i = 0; i < RANGE_CHANNELS; i++)
    {
        if(rssi_values_dBm[i]> temp_best || temp_best== 0)
        {
            temp_best = rssi_values_dBm[i];
            best_channel = 11 + i;
        }
    }
    
    printf("The best channel is %d \n", best_channel);
    cc2420_set_channel(best_channel);
}

/*---------------------------------------------------------------------------*/
PROCESS(channel_sensing_process, "channel_sensing_process");
AUTOSTART_PROCESSES(&channel_sensing_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(channel_sensing_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer;
  static int current_selected_channel;

  static int rssi_dBm_sum;
  static int counter;
  static int average_rssi_dBm;

  cc2420_init();

  etimer_set(&timer, CLOCK_SECOND * 3);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

  for (current_selected_channel = 11; current_selected_channel < 27; current_selected_channel++){
    rssi_dBm_sum = 0;
    for (counter = 0; counter < 3; counter++){
        etimer_set(&timer, (CLOCK_SECOND * 1) / 3);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
        etimer_reset(&timer);

    rssi_dBm_sum = rssi_dBm_sum + get_rssi_dBm_from_channel(current_selected_channel);

    average_rssi_dBm = rssi_dBm_sum / 3;

    dBm_channels[current_selected_channel-11] = average_rssi_dBm;
    }
    printf("The channel %d has an RSSI value of %d dBm\n", current_selected_channel, average_rssi_dBm);
}
  select_best_channel(dBm_channels);
  PROCESS_END();
}
