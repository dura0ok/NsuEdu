#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ucontext.h>

// Two are reserved.
#define MAX_THREADS 4
#define STACK_SIZE (1024*1024)

typedef unsigned ThreadId;

typedef void *ThreadFunc( void * );

typedef struct {
   ucontext_t context;
   int state;
   void *arg_rv;
} Thread;

enum {
   STATE_FREE,
   STATE_CREATED,
   STATE_RUNNING,
   STATE_ZOMBIE,
};

static ucontext_t on_exit_cx;
static Thread threads[ MAX_THREADS ];
static ThreadId current_thread_id = 0;

ThreadId get_next_thread( void ) {
   ThreadId thread_id = current_thread_id;
   for ( ThreadId i = MAX_THREADS; i--; ) {
      thread_id = ( thread_id + 1 ) % MAX_THREADS;

      int state = threads[ thread_id ].state;
      if ( state == STATE_CREATED || state == STATE_RUNNING )
         return thread_id;
   }

   return MAX_THREADS;
}

void switch_to_thread( ThreadId new_thread_id ) {
   ThreadId old_thread_id = current_thread_id;
   current_thread_id = new_thread_id;

   swapcontext(
      &( threads[ old_thread_id ].context ),
      &( threads[ new_thread_id ].context )
   );
}

void yield( void ) {
   switch_to_thread( get_next_thread() );
}

void launch_thread( ThreadId thread_id, ThreadFunc *func ) {
   Thread *thread = &( threads[ thread_id ] );
   thread->state  = STATE_RUNNING;
   thread->arg_rv = func( thread->arg_rv );
   thread->state  = STATE_ZOMBIE;
}

ThreadId create_thread( ThreadFunc *func, void *arg ) {
   Thread *thread;
   ThreadId thread_id = 0;
   while ( 1 ) {
      thread = &( threads[ thread_id ] );
      if ( thread->state == STATE_FREE )
         break;

      ++thread_id;
   }

   ucontext_t *cx = &( thread->context );
   getcontext( cx );
   cx->uc_stack.ss_sp = malloc( STACK_SIZE );
   cx->uc_stack.ss_size = STACK_SIZE;
   cx->uc_link = &on_exit_cx;
   makecontext( cx, ( void (*)( void ) )launch_thread, 2, thread_id, func );

   thread->arg_rv = arg;
   thread->state = STATE_CREATED;

   return thread_id;
}

void *wait_for_thread( ThreadId thread_id ) {
   Thread *thread = &( threads[ thread_id ] );

   // This trivially becomes a busy wait. But that's fine for this.
   while ( thread->state != STATE_ZOMBIE ) {
      yield();
   }

   thread->state = STATE_FREE;
   return thread->arg_rv;
}

void thread_sleep( time_t seconds ) {
   // This is quite imprecise. But that's fine for this.
   time_t sleep_until = time( NULL ) + seconds;

   // This trivially becomes a busy wait. But that's fine for this.
   while ( time( NULL ) < sleep_until ) {
      yield();
   }
}

void *func1( void *dummy ) {
   (void)dummy;

   for ( int i=5; i--; ) {
      printf( "func1\n" );
      if ( i )
         thread_sleep( 1 );
   }

   return NULL;
}

void *func2( void *dummy ) {
   (void)dummy;

   for ( int i=5; i--; ) {
      printf( "func2\n" );
      if ( i )
         thread_sleep( 1 );
   }

   return NULL;
}

void *main_thread( void* dummy ) {
   (void)dummy;

   // Create the threads.
   ThreadId thread1_id = create_thread( func1, NULL );
   ThreadId thread2_id = create_thread( func2, NULL );

   // Wait for them to finish.
   wait_for_thread( thread1_id );
   wait_for_thread( thread2_id );

   return NULL;
}

int main( void ) {
   current_thread_id = 0;
   for (ThreadId thread_id = MAX_THREADS; thread_id--; )
      threads[ thread_id ].state = STATE_FREE;

   bool started = false;
   ThreadId main_thread_id;
   getcontext( &on_exit_cx );

   if ( !started ) {
      started = true;
      main_thread_id = create_thread( main_thread, NULL );
      setcontext( &( threads[ main_thread_id ].context ) );
   }

   if ( current_thread_id != main_thread_id ) {
      // We get here  (via `on_exit_cx`) if a thread other than the main thread ends.
      while ( ( current_thread_id = get_next_thread() ) != MAX_THREADS ) {
         setcontext( &( threads[ current_thread_id ].context ) );
      }
   }

   // We get here if the main thread exits, or if no live threads remain.
}
