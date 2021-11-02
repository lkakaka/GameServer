/* 示例三：子线程可以后天培养自己对信号的喜好
   来源：http://www.leoox.com/?p=321
   编译：gcc pthread_sigmask3.c -lpthread
子线程天生继承了主线程对信号的喜好，但是自己可以通过后天的努力改变。
比如主线程喜欢SIGUSR1信号，但是子线程可以不喜欢它，屏蔽掉SIGUSR1信号。
由此可见，linux里的每个线程有自己的信号掩码，所以使用pthread_kill给指定线程发送信号时，
一定谨慎设置好线程的信号掩码。
当然，用kill发送信号，在多线程环境下，kill所产生的信号时传递到整个进程的，
并且所有线程都有机会收到这个信号，但具体是哪个线程处理这个信号，就不一定。
一般情况下，都是主线程处理这个信号。

运行结果：
[lgh@lghvm001 thread]$ gcc pthread_sigmask3.c -lpthread
[lgh@lghvm001 thread]$ ./a.out
[main][140613382825728] working hard ...
>>> [1481543657]Thread[140613382817536] Running ......
[main][140613382825728] send signal SIGUSR1 ...
[1481543841]Thread[140613382825728] catch signo = 10 ...		//子线程sleep期间,kill -SIGUSR1 2839
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
	/* 这种情况下，本线程屏蔽所有的信号 */
	sigfillset(&mask);
#endif

#if 1
	/* 这种情况下，本线程不屏蔽任何信号 */
	sigemptyset(&mask);
#endif

#if 0 
	/* 这种情况，本线程屏蔽以下的指定信号 */
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
		//挂起程序，等待信号的到来
		res = sigwait(&set, &sig);
		if (res != 0) {
			printf("sigwait error\n");
		}
		printf("tid: %u   ", (unsigned int)pthread_self());

		if (SIGALRM == sig)
		{
			printf("time out\n");
			//循环设置闹钟信号，每5秒响一次
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

	///* 注册SIGUSR1信号处理函数 */
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

	///* 子线程屏蔽了SIGUSR1信号，所以子线程收不到SIGUSR1信号。 */
	//pthread_kill(threads[0], SIGUSR1);
	//printf("[main][%lu] send signal SIGUSR1 ...\n", tid);
	//sleep(5);

	//pthread_join(threads[0], NULL);
	//sleep(1);

	//printf("[main][%lu] good bye and good luck!\n", tid);

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGTERM);
	pthread_sigmask(SIG_BLOCK, &mask, NULL);//SIG_BLOCK SIG_SETMASK 会屏蔽掉SIGTERM，但SIG_UNBLOCK不会屏蔽SIGTERM

	std::thread* g_sinal_thread = new std::thread(signalThreadFunc);
	g_sinal_thread->join();
}

#endif // ! WIN32
