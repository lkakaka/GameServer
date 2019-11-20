
#include "Device.h"

#include "zmq.h"
#include <stdio.h>
//#include <unistd.h>
#include <string.h>
#include <time.h>


/*
 * ���ͻ�������ƽ��ת����Dealer��
 * Ȼ����Dealer�ڲ����ؾ�����ɷ����񵽸���Worker��
 */
static int DoRouter(void* router, void* dealer)
{
	while (1) {
		int64_t more = 1;
		zmq_msg_t message;
		zmq_msg_init(&message);
		int rc = zmq_msg_recv(&message, router, 0);
		size_t more_size = sizeof(more);
		zmq_getsockopt(router, ZMQ_RCVMORE, &more, &more_size);
		zmq_msg_send(&message, dealer, more ? ZMQ_SNDMORE : 0);
		printf("[%ld] router deliver request to dealer. rc = %d, more = %ld\n", time(NULL), rc, more);
		zmq_msg_close(&message);
		if (!more) {
			break; // Last message part
		}
	}
	printf("[%ld]----------DoRouter----------\n\n", time(NULL));
	return 0;
}
/*
 * Dealer�����Worker��Ӧ������ת����Router��
 * Ȼ����RouterѰַ��Ӧ������׼ȷ�Ĵ��ݸ���Ӧ��client��
 * ֵ��ע����ǣ�Router��client��Ѱַ��ʽ���ÿ�client�ġ���ݡ���
 * ��ʱ��ݵ�client��Router��Ϊ������һ��uuid���б�ʶ��
 * ������ݵ�client��Routerֱ��ʹ�ø�client����ݡ�
 */
static int DoDealer(void* router, void* dealer)
{
	while (1) {
		int64_t more = 1;
		zmq_msg_t message;
		// Process all parts of the message
		zmq_msg_init(&message);
		int rc = zmq_msg_recv(&message, dealer, 0);
		size_t more_size = sizeof(more);
		zmq_getsockopt(dealer, ZMQ_RCVMORE, &more, &more_size);
		zmq_msg_send(&message, router, more ? ZMQ_SNDMORE : 0);
		printf("[%ld] dealer deliver reply to router. rc = %d, more = %ld\n", time(NULL), rc, more);
		zmq_msg_close(&message);
		if (!more) {
			break; // Last message part
		}
	}
	printf("[%ld]----------DoDealer----------\n\n", time(NULL));
	return 0;
}
int startDevice(char* router_addr, char* dealer_addr)
{
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);
	printf("Current 0MQ version is %d.%d.%d\n", major, minor, patch);
	printf("===========================================\n\n");
	char addr[128] = { 0 };
	void* context = zmq_ctx_new();
	snprintf(addr, sizeof(addr), "tcp://%s", router_addr);
	void* router = zmq_socket(context, ZMQ_ROUTER);
	int rc = zmq_bind(router, addr);
	printf("[%ld] router bind %s %s.\n", time(NULL), addr, (rc ? "error" : "ok"));
	if (rc) {
		printf("[%ld] router bind error : %s\n", time(NULL), zmq_strerror(zmq_errno()));
		zmq_close(router);
		zmq_ctx_term(context);
		return -2;
	}
	snprintf(addr, sizeof(addr), "tcp://%s", dealer_addr);
	void* dealer = zmq_socket(context, ZMQ_DEALER);
	rc = zmq_bind(dealer, addr);
	printf("[%ld] dealer bind %s %s.\n", time(NULL), addr, (rc ? "error" : "ok"));
	if (rc) {
		printf("[%ld] dealer bind error : %s\n", time(NULL), zmq_strerror(zmq_errno()));
		zmq_close(router);
		zmq_close(dealer);
		zmq_ctx_term(context);
		return -3;
	}
	zmq_pollitem_t items[] = {
							   {router, 0, ZMQ_POLLIN, 0}
							 , {dealer, 0, ZMQ_POLLIN, 0} };
	while (1) {
		rc = zmq_poll(items, sizeof(items) / sizeof(zmq_pollitem_t), -1);
		if (rc < 0) {
			printf("[%ld] zmq_poll error: %d\n", time(NULL), rc);
			break;
		}

		printf("[%ld] zmq_poll rc = %d\n", time(NULL), rc);
		if (rc < 1) {
			continue;
		}
		if (items[0].revents & ZMQ_POLLIN) {
			/* router�ɶ��¼���˵����client��������ˡ� */
			printf("[%ld] zmq_poll catch one router event!\n", time(NULL));
			DoRouter(router, dealer);
		}
		else if (items[1].revents & ZMQ_POLLIN) {
			/* dealer�ɶ��¼���˵����worker��Ӧ�����ݵ����ˡ� */
			printf("[%ld] zmq_poll catch one dealer event!\n", time(NULL));
			DoDealer(router, dealer);
		}
		else {
			printf("[%ld] zmq_poll Don't Care this evnet!\n", time(NULL));
		}
	}
	zmq_close(router);
	zmq_close(dealer);
	zmq_ctx_term(context);
	printf("[%ld] good bye and good luck!\n", time(NULL));
	return 0;
}