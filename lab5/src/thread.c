#include "thread.h"
#include "time.h"
#include "uart.h"
#include "mem.h"
#include "syscall.h"
// Functions in switch.S
extern void switch_to(Thread *, Thread*);
extern Thread* get_current(void);

//=======================================================
// thread queues
extern Thread_q running = {NULL, NULL};
extern Thread_q waiting = {NULL, NULL};
extern Thread_q deleted = {NULL, NULL};
static thread_count = 0;
static Thread startup;


//========================================================
// Some private functions

void thread_q_add(Thread_q *Q, Thread* t){
	t->prev = NULL;
	t->next = Q->begin;
	if(Q->begin != NULL)
		Q->begin->prev = t;
	Q->begin = t;
	if(Q->end == NULL){
		Q->end = t;
	}
	return;
}

Thread* thread_q_pop(Thread_q *Q){
	Thread* ret = Q->end; 
	if(ret != NULL){ 
		Q->end = ret->prev;
		if(Q->begin == ret){
			Q->begin = ret->prev; 
		}
		ret->prev = NULL;
		ret->next = NULL;
	}
	return ret;
}

// FIXME: Use a better algorithm
Thread* thread_q_delete(Thread_q *Q, Thread* target){
	Thread* t = NULL;
	Thread *s = Q->begin;
	t = thread_q_pop(Q);
	while(t != target && t != NULL){
		thread_q_add(Q, t);
		t = thread_q_pop(Q);
	}
	return t;
}

Thread* thread_q_delete_id(Thread_q *Q, int id){
	Thread* t = NULL;
	Thread *s = Q->begin;
	t = thread_q_pop(Q);
	while(t->id != id && t != NULL){
		thread_q_add(Q, t);
		t = thread_q_pop(Q);
	}
	return t;
}


Thread* thread_create(void (*fn)(void*)){
	Thread *cur = pmalloc(0);	// Get the small size
	cur->regs.lr = fn;
	cur->regs.sp = ((char*)cur) + 0x1000 - 16;	// The stack will grow lower
	cur->regs.fp = ((char*)cur) + 0x1000 - 16;	// FIXME:No matter?
	cur->id = thread_count++;	// Set ID
	cur->status = run;	// Set the status
	cur->sp_el0 = pmalloc(0) + 0x1000 - 16;	// Separate kernel stack
	thread_q_add(&running, cur); // Add to thread queue
	return cur;
}


void idle(void){
	while(1){
		uart_puts("idle()\n");
		kill_zombies();
		schedule();
	}
	return;
}

void kill_zombies(void){
	Thread *t = thread_q_pop(&deleted);
	while(t != NULL){
		pfree(t);
		t = thread_q_pop(&deleted);
	}
	return;
}

void schedule(){
	Thread* t = thread_q_pop(&running);
	// RR
	if(t == NULL){
		idle();
		schedule();
	}
	thread_q_add(&running, t);
	Thread* cur = get_current();
	if(cur != NULL){
		switch_to(cur, t);
	}else{
		uart_puts("initial switch\n");
		switch_to(&startup, t);
	}
	return; // This return may never used
}

void exit(){
	Thread *t = get_current();
	thread_q_delete(&running, t);
	thread_q_add(&deleted, t);
	schedule();
	uart_puts("[exit] ");
	uart_puti(t->id);
	uart_puts("\n");
	return;
}

void foo(void* a){
	Thread *t = get_current();
	for(int i = 0; i < 10; i++){
		uart_puts("Thread id:");
		uart_puti(t->id);
		uart_puts(", iter: ");
		uart_puti(i);
		uart_puts("\n");
		delay(1000000);
		schedule();
	}
	exit();
	return;
}

void test_thread_queue(void){
	// For Root thread
	startup.id = thread_count ++;
	asm volatile(
		"msr	tpidr_el1,	%[startup];"
		:: [startup] "r" (&startup)
	);
	for(int i = 0; i < 4; i++){
		thread_create(foo);
	}
	idle();
}
