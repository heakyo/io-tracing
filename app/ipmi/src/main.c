#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipmi.h>

#define TD_NUM 1
#define REQTOREP_TIME (00000*1000) // 250-20-3=227ms
#define DEVNODE "/dev/ipmi0"

/* IPMB spec v1.0 section 2.11.1 (Figure 2-2) */
#define GET_SEQNUM(__data) (__data[4] >> 2)
#define NXT_SEQNUM(__iseq) ((__iseq + 1) % MAX_SEQNUM)
#define GET_CK1(__data) (__data[2])
#define GET_CK2(__data, __len) (__data[__len - 1])
#define GET_SA(__data) (__data[3])
#define GET_LUN(__data) (__data[4] & 0x03)
#define GET_NETFN(__data) (__data[1] >> 2)
#define GET_CMD(__data) (__data[5])

#define RECV_MSG_AVAIL 1
#define CHK_MSG_AVAIL_INTERVAL 10000
#define CHK_MSG_AVAIL_RETRIES 600

int get_device_id(void);
int get_msgf(unsigned char *recv_data, unsigned short recv_data_len);
int get_led_state(unsigned int sleeptime);

unsigned char cksum(const unsigned char *buf, int len);
void chk_data(unsigned char *rdata, unsigned short rdata_len);

int fd;

int msgid = 0;
int seq = 0;

static void
req_ctor(unsigned char cmd,
         unsigned char netfn,
         unsigned char *data,
	 unsigned short data_len,
         struct ipmi_req *req,
         struct ipmi_system_interface_addr *addr)
{
	req->addr = (unsigned char *)addr;
	req->addr_len = sizeof(*addr);

	req->msg.cmd = cmd;
	req->msg.netfn = netfn; // IPMI_NETFN_APP_REQUEST;
	req->msg.data = data;
	req->msg.data_len = data_len;
	req->msgid = msgid++;
}

static void
recv_ctor(unsigned char *data,
          unsigned short data_len,
	  struct ipmi_recv *recv,
          struct ipmi_system_interface_addr *addr)
{
	recv->addr = (unsigned char *)addr;
	recv->addr_len = sizeof(*addr);

	recv->msg.data = data;
	recv->msg.data_len = data_len;

	recv->timo = 10;
}

static int
exec_cmd(unsigned char cmd,
	            unsigned char *req_data,
                    unsigned short req_data_len,
                    struct ipmi_recv *recv,
	            unsigned char *recv_data,
                    unsigned short recv_data_len,
                    struct ipmi_system_interface_addr *recv_addr)
{
	struct ipmi_req req = {0};
	struct ipmi_system_interface_addr addr = {0};
	int ret;

	seq++;
	req_data[5] |= (seq<<2);
	req_data[10] = cksum(&req_data[4], 6);

	printf("Send Command:0x%02x-------------------\n", cmd);
	printf("cmd seq%d\n", req_data[5]>>2);

	addr.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
	addr.channel = IPMI_BMC_CHANNEL;
	addr.lun = 0;
	req_ctor(cmd, 0x06, req_data, req_data_len, &req, &addr);
	ret = ioctl(fd, IPMICTL_SEND_COMMAND, &req);
	if (ret) {
		perror("Error in ioctl IPMICTL_SEND_COMMAND");
		return -1;
	}

	printf("Please wait for a moment\n");
	//usleep(100000); // 6ms
	usleep(REQTOREP_TIME); // 6ms

	printf("Receive MSG-----\n");
	recv_ctor(recv_data, recv_data_len, recv, recv_addr);
	ret = ioctl(fd, IPMICTL_RECEIVE_MSG_TRUNC, recv);
	if (ret) {
		perror("Error in ioctl IPMICTL_RECEIVE_MSG_TRUNC");
		return -1;
	}

	return 0;
}

static int
poll_msg_flags(int flag, int interval, int max_retry)
{
    int retry, rlen, rc = -1;
    unsigned char rdata[256] = {0};

    for (retry = max_retry; retry > 0; retry--) {

	rc = get_msgf(rdata, sizeof(rdata));
        if (rc) {
            printf("[%d] Failed to get BMC message flags. rc=%d", getpid(), rc);
            break;
        }

        if (rdata[1] & flag) {
            printf("response available after %d ms\n", (interval / 1000) * (max_retry - retry));
            rc = 0;
            break;
        }

        usleep(interval);
    }

    return rc;
}

static void
show_reply_data(struct ipmi_recv *recv)
{
	struct ipmi_system_interface_addr *addr;
	int i;

	addr = (struct ipmi_system_interface_addr *)recv->addr;

	printf("Packet:\t\trecv_type = %d; msgid = %ld\n", recv->recv_type, recv->msgid);

	printf("Address:\t");
	printf("addr_type=0x%x", addr->addr_type);
	printf("; channel=0x%02x", addr->channel);
	printf("; lun=0x%02x", addr->lun);
	printf("\n");

	printf("Msg:\t\t");
	printf("netfn=0x%02x", recv->msg.netfn);
	printf("; cmd=0x%02x", recv->msg.cmd);
	printf("; data_len=%d", recv->msg.data_len);
	printf("\n");

	printf("Data:\t\t");
	for (i = 0; i < recv->msg.data_len; i++)
		printf("%02x, ", recv->msg.data[i]);
	printf("\n");

	printf("Cmpl Code:\t");
	printf("0x%02x", recv->msg.data[0]);
	printf("\n");
}

int
get_device_id(void)
{
	struct ipmi_recv recv = {0};
	unsigned char recv_data[1024] = {0};
	unsigned short recv_data_len = sizeof(recv_data);
	struct ipmi_system_interface_addr recv_addr = {0};

	int ret;

	ret = exec_cmd(IPMI_GET_DEVICE_ID, NULL, 0,
				&recv, recv_data, recv_data_len, &recv_addr);
	if (!ret)
		show_reply_data(&recv);

	return 0;
}

int
get_msgf(unsigned char *recv_data, unsigned short recv_data_len)
{
	unsigned char req_data[256] = {0};

	struct ipmi_recv recv = {0};
	struct ipmi_system_interface_addr recv_addr = {0};

	int ret;

	ret = exec_cmd(IPMI_GET_MSG_FLAGS, req_data, 0,
				&recv, recv_data, recv_data_len, &recv_addr);
	if (!ret)
		show_reply_data(&recv);

	return 0;
}

int
get_led_state(unsigned int sleeptime)
{
	unsigned char req_data[256] = {
		0x00, 0x7e, 0xc2, 0xc0,
		0x20, 0x02, 0xc0, 0x88, 0x07, 0x1c, 0x00
	};
	unsigned short req_data_len = 0x0b;

	struct ipmi_recv recv = {0};
	unsigned char recv_data[256] = {0};
	unsigned short recv_data_len = sizeof(recv_data);
	struct ipmi_system_interface_addr recv_addr = {0};

	int ret;


	ret = exec_cmd(IPMI_SEND_MSG, req_data, req_data_len,
				&recv, recv_data, recv_data_len, &recv_addr);
	if (!ret)
		show_reply_data(&recv);

	sleep(sleeptime);

	while (1) {

		printf("Poll MSG Flags---------\n");
		//poll_msg_flags(RECV_MSG_AVAIL, CHK_MSG_AVAIL_INTERVAL, CHK_MSG_AVAIL_RETRIES);
		poll_msg_flags(RECV_MSG_AVAIL, CHK_MSG_AVAIL_INTERVAL, 3);

		ret = exec_cmd(IPMI_GET_MSG, req_data, 0x0,
				&recv, recv_data, recv_data_len, &recv_addr);
		if (!ret)
			show_reply_data(&recv);
		chk_data(&recv.msg.data[1], recv.msg.data_len-1);

		if (!recv.msg.data[0])
			break;
	}

	return 0;
}

int
clear_msg_flag(void)
{
	unsigned char req_data[256] = {
		0x03,
	};
	unsigned short req_data_len = 0x01;

	struct ipmi_recv recv = {0};
	unsigned char recv_data[256] = {0};
	unsigned short recv_data_len = sizeof(recv_data);
	struct ipmi_system_interface_addr recv_addr = {0};

	int ret;

	ret = exec_cmd(IPMI_CLEAR_FLAGS, req_data, req_data_len,
				&recv, recv_data, recv_data_len, &recv_addr);
	if (!ret)
		show_reply_data(&recv);

	return 0;
}

unsigned char
cksum(const unsigned char *buf, int len)
{
        unsigned csum;
        int i;

        /* 8-bit 2s compliment checksum */
        csum = 0;
        for (i = 0; i < len; i++)
           csum = (csum + buf[i]) % 256;
        csum = -csum;

        return(csum);
}

void
chk_data(unsigned char *rdata, unsigned short rdata_len)
{
#if 0
	unsigned char rdata[] = {
		0x00, 0xc6, 0x1a, 0x7e, 0x76, 0xc0, 0x00, 0x81, 0xcb
	};
#endif

	printf("-----------%s-----------\n", __func__);

	printf("cksum(&rdata[0], 2) == GET_CK1(rdata)---(0x%02x, 0x%02x, %d)\n",
		cksum(&rdata[0], 2), GET_CK1(rdata), cksum(&rdata[0], 2)==GET_CK1(rdata));
	printf("cksum(&rdata[3], rdata_len - 4) == GET_CK2(rdata, rdata_len)---(0x%02x, 0x%02x, %d)\n",
		cksum(&rdata[3], rdata_len - 4), GET_CK2(rdata, rdata_len), cksum(&rdata[3], rdata_len - 4) == GET_CK2(rdata, rdata_len));
	printf("sa:0x%02x, lun:0x%02x iseq:0x%02x netfn:0x%02x cmd:0x%02x\n",
		GET_SA(rdata), GET_LUN(rdata), GET_SEQNUM(rdata), (GET_NETFN(rdata) & 0xFE), GET_CMD(rdata));
}

void *ipmi_test(void *arg)
{
	int id = *((int *)arg);
	int base_sleep_time = 1;

	printf("Thread %d is running\n", id);

	if (id == 0 && 0) {
		//sleep(TD_NUM);
		clear_msg_flag();
		return NULL;
	}

	//get_device_id();
	//get_led_state((id));

	sleep(id);
	get_led_state((id)*TD_NUM + 1);
	//usleep(id*50*1000);

	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t tds[TD_NUM];
	void *(*start_routine)(void *) = NULL;
	int tds_ids[TD_NUM];
	int i;

	//chk_data();

	printf("%s:-----------IPMI Test Start-----------\n", __func__);

	fd = open(DEVNODE, O_RDWR);
	if (fd < 0) {
		printf("Error opening dev %s\n", DEVNODE);
		return -1;
	}

	for (i = 0; i < TD_NUM; i++) {
		tds_ids[i] = i;
		if (pthread_create(&tds[i], NULL, ipmi_test, &tds_ids[i])) {
			printf("Error creating thread %d\n", i);
			return -1;
		}
	}


	for (i = 0; i < TD_NUM; i++) {
		if (pthread_join(tds[i], NULL)) {
			printf("Error joining thread %d\n", i);
			return -1;
		}
		printf("%s:Thread %d completed.\n", __func__, i);
	}

	close(fd);

	printf("%s:-----------IPMI Test End-----------\n", __func__);

	return 0;
}
