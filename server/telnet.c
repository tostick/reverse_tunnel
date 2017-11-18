#include <stdio.h>
//#include "./cli-lib/inc/command.h"
#include "../libs/cli-lib/include/command.h"

#include "utils.h"
#include "user.h"


#define CC_CLI_HOSTNAME "CC"
#define CC_CLI_VIEW_PWD "123456" /*cli password*/
#define CC_CLI_ENABLE_PWD "123654"
#define CC_CLI_CONFIG ""
#define CC_CLI_PORT 5566 /*cli端口*/

#ifndef connect_cli
DEFUN(connect_cli,
      connect_cli_cmd,
      "connect (id) STRING",
      "connect client\n"
      "connect client service\n"
     )
{
    vty_out(vty, "1-%s,2-%s", argv[0], argv[1]);

    

    return CMD_SUCCESS;
}
#endif


#ifndef start_cc
DEFUN(start_cc,
      start_cc_cmd,
      "start cc",
      "monitor service start\n"
      "cc service\n"
     )
{
    vty_out(vty, "start cc successfully!%s", VTY_NEWLINE);

    send_order_request(9, "hello cc");

    return CMD_SUCCESS;
}
#endif

static void mycmd()
{
    install_element(VIEW_NODE, &start_cc_cmd);
    install_element(VIEW_NODE, &connect_cli_cmd);
}

void thread_agent_cli()
{
    init_cli(CC_CLI_HOSTNAME, CC_CLI_VIEW_PWD, CC_CLI_ENABLE_PWD, 
             CC_CLI_PORT, NULL, mycmd, NULL);
}

