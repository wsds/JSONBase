//#include "test4_subProcess.h"
//
//Process * thizSubProcess;
//
//void callMainProcess(int value) {
////	callProcess(thizSubProcess->parent_pid, value);
//}
//
//void threadHolder_sub() {
//	int pid = getpid();
//	int j = 0;
//	while (true) {
//		sleep(5);
//		j += 5;
////		std::cout << "Sub pid : " << pid << "  j = " << j << std::endl;
//		if (j % 10 == 0) {
//			callMainProcess(100000 + j);
//		}
//
//	}
//}
//
//void onCall_sub(siginfo_t * signal_info) {
//	int value = signal_info->si_int;
//	std::cout << "SubProcess is Called: " << value << std::endl;
//
//}
//
//void resolveSignal_sub(int signal, siginfo_t * signal_info, void * none) {
//	std::cout << "resolveSignal_sub" << std::endl;
//
//	if (signal == 58) {
//		onCall_sub(signal_info);
//	} else if (signal == SIGSEGV) {
//	}
//
//	return;
//}
//
//void regeditSignal_sub() {
//	struct sigaction new_sigaction;
//	struct sigaction old_sigaction;
//
//	sigemptyset(&new_sigaction.sa_mask);
//	new_sigaction.sa_sigaction = &resolveSignal_sub;
//	new_sigaction.sa_flags = SA_SIGINFO;
//
//	int a = sigaction(58, &new_sigaction, &old_sigaction);
//}
//
//void runInSubProcess4(Process * subProcess) {
//	thizSubProcess = subProcess;
////	thizSubProcess->sub_pid = getpid();
//
//	regeditSignal_sub();
//
//	threadHolder_sub();
//
//}
//
