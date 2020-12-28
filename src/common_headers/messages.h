#ifndef __MESSAGES_H__
#define __MESSAGES_H__

typedef enum  MESSAGE_TYPE {
    SECRETARY_BUILD_REQ,
    SECRETARY_BUILD_RES,
    WORKER_BUILD_ORDER,
    WORKER_BUILD_DONE
} message_type_t;

typedef struct MESSAGE {
    message_type_t          type;
    int                     size;
    char                    buffer[];
} message_t;

typedef struct BUILD_REQ_MSG {
    platform_t      platform;
    int             listen_port;
} build_req_msg_t;

typedef struct BUILD_RES_MSG {
    bool            status;
    int             reason;
} build_res_msg_t;

typedef struct BUILD_ORDER_MSG {
    /* TODO(victor): resolve this */
    int                 client_id;
    struct sockaddr_in  client_addr;
    build_req_msg_t     request;
} build_order_msg_t;

typedef struct BUILD_ORDER_DONE_MSG {
    build_order_msg_t       build_order;
    int                     status;
    int                     reason;
} build_order_done_msg_t;

#endif
