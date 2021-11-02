/* ʾ���������߳̿��Ժ��������Լ����źŵ�ϲ��
   ��Դ��http://www.leoox.com/?p=321
   ���룺gcc pthread_sigmask3.c -lpthread
���߳������̳������̶߳��źŵ�ϲ�ã������Լ�����ͨ�������Ŭ���ı䡣
�������߳�ϲ��SIGUSR1�źţ��������߳̿��Բ�ϲ���������ε�SIGUSR1�źš�
�ɴ˿ɼ���linux���ÿ���߳����Լ����ź����룬����ʹ��pthread_kill��ָ���̷߳����ź�ʱ��
һ���������ú��̵߳��ź����롣
��Ȼ����kill�����źţ��ڶ��̻߳����£�kill���������ź�ʱ���ݵ��������̵ģ�
���������̶߳��л����յ�����źţ����������ĸ��̴߳�������źţ��Ͳ�һ����
һ������£��������̴߳�������źš�

���н����
[lgh@lghvm001 thread]$ gcc pthread_sigmask3.c -lpthread
[lgh@lghvm001 thread]$ ./a.out
[main][140613382825728] working hard ...
>>> [1481543657]Thread[140613382817536] Running ......
[main][140613382825728] send signal SIGUSR1 ...
[1481543841]Thread[140613382825728] catch signo = 10 ...		//���߳�sleep�ڼ�,kill -SIGUSR1 2839
[1481543861]Thread[140613382825728] catch signo = 10 ... done
[1481543957]Thread[140613382817536] waitup(0), and say good bye!
[main][140613382825728] good bye and good luck!
[lgh@lghvm001 thread]$
*/

#include "Test.h"
#ifndef  WIN32


#include <pthread.h>
#include <stdio.h>
#include <sys/signal.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>

static bool _stop = false;

void handler(int signo)
{
	pthread_t tid = pthread_self();
	printf("[%u]Thread[%lu] catch signo = %d ...\n", time(NULL), tid, signo);
	sleep(20);
	printf("[%u]Thread[%lu] catch signo = %d ... done\n", time(NULL), tid, signo);
	return;
}

void* run(void* param)
{
	pthread_t tid = pthread_self();
	sigset_t mask;
#if 0
	/* ��������£����߳��������е��ź� */
	sigfillset(&mask);
#endif

#if 1
	/* ��������£����̲߳������κ��ź� */
	sigemptyset(&mask);
#endif

#if 0 
	/* ������������߳��������µ�ָ���ź� */
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGTERM);
#endif 

	pthread_sigmask(SIG_SETMASK, &mask, NULL);

	printf(">>> [%u]Thread[%lu] Running ......\n", time(NULL), tid);

	int rc = sleep(300);

	printf("[%u]Thread[%lu] waitup(%d), and say good bye!\n", time(NULL), tid, rc);
	return NULL;
}

static void signalHandler(int signum)
{
	std::cout << "Interrupt signal (" << signum << ") received.\n";
	//thread::id id = std::this_thread::get_id();
	//io.stop();
	_stop = true;
}

static void signalThreadFunc() {
#ifndef WIN32
	
	//sigset_t waitset;
	//int signum;
	//sigemptyset(&waitset);
	//sigaddset(&waitset, SIGTERM);
	//struct timespec timeout = { 1, 0 };
	//while (!_stop) {
	//	if (-1 == (signum = sigtimedwait(&waitset, NULL, &timeout))) {
	//		//do not log error, because timeout will also return -1.
	//		printf("time out or error, errno=%d, errmsg=%s\n", errno, strerror(errno));
	//	}
	//	else {
	//		printf("sigwaitinfo() fetch the signal: %d\n", signum);
	//		signalHandler(signum);
	//	}
	//}

	/*sigset_t mask;
	sigemptyset(&mask);
	pthread_sigmask(SIG_BLOCK, &mask, NULL);*/


	printf("subthread tid: %u\n", (unsigned int)pthread_self());
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGTERM);
	int res, sig;

	while (1)
	{
		//������򣬵ȴ��źŵĵ���
		res = sigwait(&set, &sig);
		if (res != 0) {
			printf("sigwait error\n");
		}
		printf("tid: %u   ", (unsigned int)pthread_self());

		if (SIGALRM == sig)
		{
			printf("time out\n");
			//ѭ�����������źţ�ÿ5����һ��
			alarm(5);
		}
		else if (SIGTERM == sig)
		{
			printf("catch quit signal\n");
			break;
		}
		else if (SIGINT == sig)
		{
			printf("quit program\n");
			exit(0);
		}
	}
#endif // WIN32
}

void test_signal()
{
	//pthread_t tid = pthread_self();

	///* ע��SIGUSR1�źŴ����� */
	//struct sigaction sa;
	//memset(&sa, 0, sizeof(sa));
	//sigemptyset(&sa.sa_mask);
	//sa.sa_flags = 0;
	//sa.sa_handler = handler;
	//sigaction(SIGUSR1, &sa, NULL);

	//pthread_t threads[1];
	//pthread_create(&threads[0], NULL, run, NULL);
	//printf("[main][%lu] working hard ...\n", tid);
	//sleep(5);

	///* ���߳�������SIGUSR1�źţ��������߳��ղ���SIGUSR1�źš� */
	//pthread_kill(threads[0], SIGUSR1);
	//printf("[main][%lu] send signal SIGUSR1 ...\n", tid);
	//sleep(5);

	//pthread_join(threads[0], NULL);
	//sleep(1);

	//printf("[main][%lu] good bye and good luck!\n", tid);

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGTERM);
	pthread_sigmask(SIG_BLOCK, &mask, NULL);//SIG_BLOCK SIG_SETMASK �����ε�SIGTERM����SIG_UNBLOCK��������SIGTERM

	std::thread* g_sinal_thread = new std::thread(signalThreadFunc);
	g_sinal_thread->join();
}

#endif // ! WIN32
