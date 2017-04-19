#ifndef _LINUX_WORKQUEUE_H
#define _LINUX_WORKQUEUE_H

typedef WORK_QUEUE_ITEM work_struct_t;

#define	INIT_WORK(_work,_func)	ExInitializeWorkItem(_work, _func, _work)
#define	queue_work(wq, _work)	ExQueueWorkItem(_work, CriticalWorkQueue)
#define	flush_workqueue(wq)
#define	create_workqueue(name)	(1)
#define	destroy_workqueue(wq)

#endif


