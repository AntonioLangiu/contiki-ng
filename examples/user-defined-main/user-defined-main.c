/**
 * \file
 *         A small example to of a user defined main.
 * \author
 *         Antonio Langiu <antonio@langiu.com>
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "contiki-net.h"
#include "sys/platform.h"
#include "sys/energest.h"
#include "dev/watchdog.h"

#if BUILD_WITH_ORCHESTRA
#include "os/services/orchestra/orchestra.h"
#endif /* BUILD_WITH_ORCHESTRA */
#if BUILD_WITH_SHELL
#include "os/services/shell/serial-shell.h"
#endif /* BUILD_WITH_SHELL */

#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "Main"
#define LOG_LEVEL LOG_LEVEL_MAIN
/*---------------------------------------------------------------------------*/
int
#if PLATFORM_MAIN_ACCEPTS_ARGS
main(int argc, char **argv)
{
    platform_process_args(argc, argv);
#else
main(void)
{
#endif
    platform_init_stage_one();


    clock_init();
    rtimer_init();
    process_init();
    process_start(&etimer_process, NULL);
    ctimer_init();
    watchdog_init();

    energest_init();

    platform_init_stage_two();

    LOG_INFO("Starting " CONTIKI_VERSION_STRING "\n");
    LOG_INFO("This is a user defined main\n");

    LOG_INFO(" Net: ");
    LOG_INFO_("%s\n", NETSTACK_NETWORK.name);
    LOG_INFO(" MAC: ");
    LOG_INFO_("%s\n", NETSTACK_MAC.name);

    netstack_init();

    LOG_INFO("Link-layer address ");
    LOG_INFO_LLADDR(&linkaddr_node_addr);
    LOG_INFO_("\n");

#if NETSTACK_CONF_WITH_IPV6
    {
        uip_ds6_addr_t *lladdr;
        memcpy(&uip_lladdr.addr, &linkaddr_node_addr, sizeof(uip_lladdr.addr));
        process_start(&tcpip_process, NULL);

        lladdr = uip_ds6_get_link_local(-1);
        LOG_INFO("Tentative link-local IPv6 address ");
        LOG_INFO_6ADDR(lladdr != NULL ? &lladdr->ipaddr : NULL);
        LOG_INFO_("\n");
    }
#endif /* NETSTACK_CONF_WITH_IPV6 */

    platform_init_stage_three();

#if BUILD_WITH_ORCHESTRA
    orchestra_init();
    LOG_DBG("With Orchestra\n");
#endif /* BUILD_WITH_ORCHESTRA */

#if BUILD_WITH_SHELL
    serial_shell_init();
    LOG_DBG("With Shell\n");
#endif /* BUILD_WITH_SHELL */

    autostart_start(autostart_processes);

    watchdog_start();

#if PLATFORM_PROVIDES_MAIN_LOOP
    platform_main_loop();
#else
    while(1) {
        uint8_t r;
        do {
            r = process_run();
            watchdog_periodic();
        } while(r > 0);

        platform_idle();
    }
#endif

    return 0;
}
/*---------------------------------------------------------------------------*/
PROCESS(user_defined_main_process, "Example of a user defined main");
AUTOSTART_PROCESSES(&user_defined_main_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(user_defined_main_process, ev, data)
{
    PROCESS_BEGIN();

    printf("Hi, I'm a user defined main\n");

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
