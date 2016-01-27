#include "Process.h"

Space * newSpace() {
	Space * space = NULL;
	char * start = (char*) 0xbb001000;
	start = (char*) mmap(start, 4096 * 4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	//           |class Space|signal_queue|temp_use|swap_space_base|

	if (start != (char*) 0xbb001000) {
		return NULL;
	}
	space = (Space *) start;
	space->initialize();

	return space;
}

Space::Space() {

}

void * Space::get(int start) {
	void * swap_data = (void *) (this->base + start);

	return swap_data;
}

int Space::getLength(int start) {
	int * p_length;
	if (start - 4 < 0) {
		p_length = (int *) (this->base + this->max_size + start - 4);
	} else {
		p_length = (int *) (this->base + start - 4);
	}
	int length = (*p_length);
	return length;
}

int Space::take(int length) {

	int head = this->head;
	this->head = (head + length + 4) % this->max_size;

	if (this->max_size - this->head < 4) {
		this->head = 0;
	}

	int start = (head + 4) % this->max_size;

	int * p_length = (int *) (this->base + head);
	(*p_length) = length;

	return start;
}

void Space::free(int start) {

}

void Space::obverse_memset_slow(int start, char value, int count) {
	if (start < 0 || count <= 0) {
		return;
	}

	char * destination_char = NULL;
	for (int i = 0; i < count; i++) {
		destination_char = this->base + (start + i) % this->max_size;
		*(destination_char) = value;
	}
}

void Space::obverse_memcpy_slow(int start, void * source, int count) {
	if (start < 0 || source == NULL || count <= 0) {
		return;
	}

	char * destination_char = NULL;
	char * source_char = (char *) source;
	for (int i = 0; i < count; i++) {
		destination_char = this->base + (start + i) % this->max_size;
		*(destination_char) = *(source_char + i);
	}
}

bool Space::initialize() {
	int classSize = sizeof(Space);

	thiz = (char *) this;

	this->max_size = 4096;
	this->base = thiz + 4096 * 3;
	this->length = 0;
	this->tail = 0;
	this->head = 0;

	this->mySignalHeadMap = (MySignalHead *) (thiz + 2048);
	this->mySignalQueueBase = (MySignal *) (thiz + 4096);
	this->signal_queue_head = 0;

	return true;
}

MySignalHead * Space::getMySignalHead(int pid) {
	MySignalHead * mySignalHead = this->mySignalHeadMap;
	for (int i = 0; i < 2048 / 16; i++) {
		if (mySignalHead->pid == 0) {
			mySignalHead->pid = pid;
			mySignalHead->tail = -1;
			break;
		}
		if (mySignalHead->pid == pid) {
			break;
		}
		mySignalHead++;
	}
	return mySignalHead;
}

MySignal * Space::getNewSignal(MySignalHead * signalHead) {
	MySignal * newSignal = this->mySignalQueueBase + this->signal_queue_head;
	signalHead->head = this->signal_queue_head;

	this->signal_queue_head++;
	this->signal_queue_head = (this->signal_queue_head) % (4096 / 16);

	if (newSignal->pid != 0) {
		for (int i = 0; i < 4096 / 16; i++) {
			newSignal = this->mySignalQueueBase + this->signal_queue_head;
			signalHead->head = this->signal_queue_head;
			if (newSignal->pid == 0) {
				break;
			}
			this->signal_queue_head++;
			this->signal_queue_head = (this->signal_queue_head) % (4096 / 16);
		}
	}

	return newSignal;
}

ProcessCallback::ProcessCallback() {
	this->onCall = NULL;
	this->onStart = NULL;
}
Process::Process() {
	this->processCallback = NULL;
	this->is_initialized = false;
	this->processCallback = new ProcessCallback();
	this->send_heart_beat_to_pid = -1;
	this->remote_heart_beat = -1;
	this->needThreadHolder = true;
}

bool Process::initialize() {
	if (this->is_initialized == true) {
		return false;
	}

	this->pid = getpid();
	this->mySignalHead = this->swap_space->getMySignalHead(this->pid);

	this->is_onCall = false;
	this->is_running = true;

	this->regeditSignal();
	this->threadHolder();
	return true;
}

void Process::callProcess(int pid, int value, MySignalHead * targetSignalHead) {
	MySignal * headSignal = this->swap_space->mySignalQueueBase + targetSignalHead->head;

	MySignal * newSignal = this->swap_space->getNewSignal(targetSignalHead);

	if (headSignal->pid == pid) {
		headSignal->next = newSignal;
	}
	newSignal->pid = pid;
	newSignal->value = value;
	newSignal->sid = targetSignalHead->count;

	targetSignalHead->count++;

	if (targetSignalHead->tail == -1) {
		targetSignalHead->tail = targetSignalHead->head;
	}

	kill(pid, 58);
}

void Process::callProcess(int pid, void * data, int length, MySignalHead * targetSignalHead) {
	int start = this->swap_space->take(length);
	this->swap_space->obverse_memcpy_slow(start, data, length);
	this->callProcess(pid, start, targetSignalHead);
}

void Process::callProcess(int pid, JSON * json, MySignalHead * targetSignalHead) {
	JSString* json_str = stringifyJSON2JSString(json);
	this->callProcess(pid, json_str->char_string, json_str->length, targetSignalHead);
}

void Process::onCall() {
	if (this->is_onCall == true) {
		return;
	}
	this->is_onCall = true;
	MySignal * tailSignal = this->swap_space->mySignalQueueBase + this->mySignalHead->tail;
	int value = 0;

	if (tailSignal->pid != 0) {
		for (int i = 0; i < 4096 / 16; i++) {
			value = tailSignal->value;
			this->onCall(value);

			MySignal * consumedSignal = tailSignal;
			tailSignal = tailSignal->next;

			consumedSignal->pid = 0;
			consumedSignal->sid = 0;
			consumedSignal->value = 0;
			consumedSignal->next = NULL;
			if (tailSignal == NULL) {
				this->mySignalHead->tail = -1;
				break;
			}
		}
	}
	this->is_onCall = false;

}

void Process::onCall(int value) {
	if (-value > 10000) {
		this->remote_heart_beat = -value;
	} else if (-value > 0) { //-10000~0 define Command without data
		int command = -value;
		if (this->processCallback != NULL && this->processCallback->onCommand != NULL) {
			this->processCallback->onCommand(command);
		}
	} else {

		int start = value;
		void * data = this->swap_space->get(start);
		int length = this->swap_space->getLength(start);
		char *new_data = NULL;
		if (start + length > this->swap_space->max_size) {
			new_data = JSMalloc(length);
			obverse_memcpy_slow(new_data, data, swap_space->max_size - start);
			obverse_memcpy_slow(new_data + swap_space->max_size - start, swap_space->base, length - (swap_space->max_size - start));
			data = new_data;
		}

		JSON * json = parseJSON((char*) data, length);

		if (this->processCallback != NULL && this->processCallback->onCall != NULL) {
			this->processCallback->onCall(json);
		}
		delete (json);
		JSFree(new_data);
	}
}

void Process::threadHolder() {
	if (this->processCallback != NULL && this->processCallback->onStart != NULL) {
		this->processCallback->onStart();
	}
	if (this->needThreadHolder == false) {
		return;
	}
	int j = 0;
	int heart_beat = 10000;
	int old_remote_heart_beat = this->remote_heart_beat;
	while (this->is_running) {
		sleep(1);
		heart_beat++;
		if (this->send_heart_beat_to_pid != -1) {
			this->callProcess(send_heart_beat_to_pid, -heart_beat, this->send_heart_beat_to_SignalHead);
		}
		if (this->remote_heart_beat > 10000 && heart_beat % 2 == 0) {
			if (old_remote_heart_beat == this->remote_heart_beat) {
				if (this->processCallback != NULL && this->processCallback->onLostHeartBeat != NULL) {
					this->processCallback->onLostHeartBeat(old_remote_heart_beat);
				}
				this->remote_heart_beat = -1;
			} else {
				old_remote_heart_beat = this->remote_heart_beat;
			}

		}
	}
}

void Process::regeditSignal() {
	struct sigaction new_sigaction;
	struct sigaction old_sigaction;

	sigemptyset(&new_sigaction.sa_mask);
	new_sigaction.sa_sigaction = this->resolveSignal_sub;
	new_sigaction.sa_flags = SA_SIGINFO;

	sigaction(58, &new_sigaction, &old_sigaction);
	sigaction(SIGTERM, &new_sigaction, &old_sigaction);
}

