/*
 * Copyright 2020-2021 Huawei Technologies Co.,Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef UADK_ASYNC_H
#define UADK_ASYNC_H

#include <stdbool.h>
#include <openssl/async.h>
#include <semaphore.h>

#define ASYNC_QUEUE_TASK_NUM 1024

struct async_op {
	ASYNC_JOB *job;
	int done;
	int idx;
	int ret;
};

struct uadk_e_cb_info {
	void *priv;
	struct async_op *op;
};

typedef int (*async_recv_t)(void *ctx);

enum task_type {
	ASYNC_TASK_CIPHER,
	ASYNC_TASK_DIGEST,
	ASYNC_TASK_RSA,
	ASYNC_TASK_DH,
	ASYNC_TASK_ECC
};

struct async_poll_task {
	enum task_type type;
	void *ctx;
	struct async_op *op;
};

struct async_poll_queue {
	struct async_poll_task *head;
	int status[ASYNC_QUEUE_TASK_NUM];
	int sid;
	int rid;
	bool is_recv;
	sem_t empty_sem;
	sem_t full_sem;
	pthread_mutex_t async_task_mutex;
	pthread_t thread_id;
};

extern int async_setup_async_event_notification(struct async_op *op);
extern int async_clear_async_event_notification(void);
extern int async_pause_job(void *ctx, struct async_op *op, enum task_type type, int id);
extern int async_register_poll_fn(int type, async_recv_t func);
extern void async_module_init(void);
extern int async_wake_job(ASYNC_JOB *job);
extern void async_free_poll_task(int id, bool is_cb);
extern int async_get_free_task(int *id);
#endif
