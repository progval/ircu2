#ifndef INCLUDED_ircd_network_h
#define INCLUDED_ircd_network_h
/*
 * IRC - Internet Relay Chat, include/ircd_network.h
 * Copyright (C) 2001 Kevin L. Mitchell <klmitch@mit.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id$
 */

#ifndef INCLUDED_config_h
#include "config.h"
#endif
#ifndef INCLUDED_sys_types_h
#include <sys/types.h>	/* time_t */
#define INCLUDED_sys_types_h
#endif

typedef void (*EventCallBack)(struct Event*);

enum SocketState {
  SS_CONNECTING,	/* Connection in progress on socket */
  SS_LISTENING,		/* Socket is a listening socket */
  SS_CONNECTED,		/* Socket is a connected socket */
  SS_DATAGRAM,		/* Socket is a datagram socket */
  SS_CONNECTDG		/* Socket is a connected datagram socket */
};

enum TimerType {
  TT_ABSOLUTE,		/* timer that runs at a specific time */
  TT_RELATIVE,		/* timer that runs so many seconds in the future */
  TT_PERIODIC		/* timer that runs periodically */
};

enum EventType {
  ET_READ,		/* Readable event detected */
  ET_WRITE,		/* Writable event detected */
  ET_ACCEPT,		/* Connection can be accepted */
  ET_CONNECT,		/* Connection completed */
  ET_EOF,		/* End-of-file on connection */
  ET_ERROR,		/* Error condition detected */
  ET_SIGNAL,		/* A signal was received */
  ET_EXPIRE,		/* A timer expired */
  ET_DESTROY		/* The generator is being destroyed */
};

struct GenHeader {
  struct GenHeader*  gh_next;	/* linked list of generators */
  struct GenHeader** gh_prev_p;
  unsigned int	     gh_flags;	/* generator flags */
  unsigned int	     gh_ref;	/* reference count */
  EventCallBack	     gh_call;	/* generator callback function */
  void*		     gh_data;	/* extra data */
};

#define GEN_DESTROY	0x0001	/* generator is to be destroyed */

struct Socket {
  struct GenHeader s_header;	/* generator information */
  enum SocketState s_state;	/* state socket's in */
  unsigned int	   s_events;	/* events socket is interested in */
  int		   s_fd;	/* file descriptor for socket */
};

#define SOCK_EVENT_READABLE	0x0001	/* interested in readable */
#define SOCK_EVENT_WRITABLE	0x0002	/* interested in writable */

struct Signal {
  struct GenHeader sig_header;	/* generator information */
  int		   sig_signal;	/* signal number */
  unsigned int	   sig_count;	/* count of number of signals */
};

struct Timer {
  struct GenHeader t_header;	/* generator information */
  enum TimerType   t_type;	/* what type of timer this is */
  time_t	   t_value;	/* value timer was added with */
  time_t	   t_expire;	/* time at which timer expires */
};

struct Event {
  struct Event*	   ev_next;	/* linked list of events on queue */
  struct Event**   ev_prev_p;
  enum EventType   ev_type;	/* Event type */
  union {
    struct GenHeader* gen_header;	/* Generator header */
    struct Socket*    gen_socket;	/* Socket generating event */
    struct Signal*    gen_signal;	/* signal generating event */
    struct Timer*     gen_timer;	/* Timer generating event */
  }		   ev_gen;	/* object generating event */
};

void event_generate(enum EventType type, void* gen);

void timer_init(struct Timer* timer, EventCallBack call, void* data);
void timer_add(struct Timer* timer, enum TimerType type, time_t value);
void timer_del(struct Timer* timer);
time_t timer_next(void);
void timer_run(void);

#endif /* INCLUDED_ircd_network_h */
