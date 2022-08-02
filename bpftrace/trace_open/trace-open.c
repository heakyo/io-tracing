#include <uapi/linux/openat2.h>
#include <linux/sched.h>

struct data_t {
	u32 pid;
	u64 ts;
	char comm[TASK_COMM_LEN];
	char fname[NAME_MAX];
};

/*
 * Creates a BPF table for pushing out custom event data to user space via a perf ring buffer.
 */
BPF_PERF_OUTPUT(events);

int hello_world(struct pt_regs *ctx, int dfd, const char __user * filename, struct open_how *how)
{
	struct data_t data = {}; // "struct data_t data;" is wrong

	data.pid = bpf_get_current_pid_tgid();
	data.ts = bpf_ktime_get_ns();

	if (!bpf_get_current_comm(&data.comm, sizeof(data.comm)))
		bpf_probe_read_user(&data.fname, sizeof(data.fname), (void *)filename);

	/*
	 * https://github.com/iovisor/bcc/blob/master/docs/reference_guide.md#output
	 *
	 * Submit custom event data to user space
	 */
	events.perf_submit(ctx, &data, sizeof(data));

	return 0;
}
