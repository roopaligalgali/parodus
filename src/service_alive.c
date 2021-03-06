/**
 * @file service_alive.c
 *
 * @description This file is used to manage keep alive section
 *
 * Copyright (c) 2015  Comcast
 */

#include "ParodusInternal.h"
#include "connection.h"
#include "client_list.h"
#include "service_alive.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define KEEPALIVE_INTERVAL_SEC                         	30

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
/*
 * @brief To handle registered services to indicate that the service is still alive.
 */
void *serviceAliveTask()
{
	void *svc_bytes;
	wrp_msg_t svc_alive_msg;
	int byte = 0;
	size_t size = 0;
	int ret = -1, nbytes = -1;
	reg_list_item_t *temp = NULL; 
	
	svc_alive_msg.msg_type = WRP_MSG_TYPE__SVC_ALIVE;	
	
	nbytes = wrp_struct_to( &svc_alive_msg, WRP_BYTES, &svc_bytes );
        if(nbytes < 0)
        {
                ParodusError(" Failed to encode wrp struct returns %d\n", nbytes);
        }
        else
        {
	        while(1)
	        {
		        ParodusPrint("serviceAliveTask: numOfClients registered is %d\n", get_numOfClients());
		        if(get_numOfClients() > 0)
		        {
			        //sending svc msg to all the clients every 30s
			        temp = get_global_node();
			        size = (size_t) nbytes;
			        while(NULL != temp)
			        {
				        byte = nn_send (temp->sock, svc_bytes, size, 0);
				
				        ParodusPrint("svc byte sent :%d\n", byte);
				        if(byte == nbytes)
				        {
					        ParodusPrint("service_name: %s is alive\n",temp->service_name);
				        }
				        else
				        {
					        ParodusInfo("Failed to send keep alive msg, service %s is dead\n", temp->service_name);
					        //need to delete this client service from list
					
					        ret = deleteFromList((char*)temp->service_name);
				        }
				        byte = 0;
				        if(ret == 0)
				        {
					        ParodusPrint("Deletion from list is success, doing resync with head\n");
					        temp= get_global_node();
					        ret = -1;
				        }
				        else
				        {
					        temp= temp->next;
				        }
			        }
		         	ParodusPrint("Waiting for 30s to send keep alive msg \n");
		         	sleep(KEEPALIVE_INTERVAL_SEC);
	            	}
	            	else
	            	{
	            		ParodusInfo("No clients are registered, waiting ..\n");
	            		sleep(70);
	            	}
	        }
	}
	return 0;
}
