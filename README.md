# Quorum, an inter-core synchronization library

This library is a stub of code meant to provide POSIX-like synchronization
mechanisms between cores.

In effect, it uses `RAM` + `Inter-Core IRQ` + 1 `Mutex` to generate _N_
`Barriers` + _M_ `Mutexes` + _O_ `Semaphores` + _P_ `Signals` +
_Q_ `Mailboxes`.

The benefit being that the original `Mutex` is likely hardware, and thus a
limited, valuable resource, whereas the generated synchronization mechanisms
are software, and thus considerably more numerous.

To keep with the "quorum" nomenclature, the cores accessing a same
synchronization mechanism are considered to be part of a council. Thus, a
council in an instance of Quorum.

## Limitations
A quorum is, by definition, the _minimal_ number of members required to make
decisions valid. Here, this means only _cores_ are synchronizing. The
synchronization mechanisms offered are not meant to be used directly for
_intra_-core synchronization. In effect, two or more processes/threads running
on a same core cannot be differentiated by Quorum. They are represented in the
quorum solely by their core.

A combination of _intra_ and _inter_ cores synchronization is possible, but, by
design, is not within the scope of Quorum. Indeed, the _intra_ core
synchronization may be handled by an operating system. In order to have the
operating system's synchronization mechanisms work over multiple cores,
integrators have to modify the operating system's code to ensure that the local
state of the mechanisms does not contradict Quorum's (e.g. a thread/process
holding an OS mutex requires its core to be holding the associated Quorum
mutex).

*Requirements:*
- Each core in a council must be able to access a shared memory with read &
  write privileges, with temporary exclusive privileges.
- Each core in a council can trigger an interruption on any of the cores
  belonging to the council.

Quorum relies on the integrator to provide:
- The index of each core in a council.
- `quorum_result quorum_integration_trigger_interrupt (void * opaque, quorum_core_index target_core_id)`.
- `quorum_result quorum_integration_lock_council (void * opaque, quorum_attributes attributes)`.
- `quorum_result quorum_integration_unlock_council (void * opaque, quorum_attributes attributes)`.
- Coherent access to the council's `quorum_council` structure from its address: the two functions above should be all that is needed to prevent race conditions (e.g. no caches issues).

## Integration

### Configuration

- `QUORUM_CONFIGURATION_HEADER`. If defined, an `#include
  QUORUM_CONFIGURATION_HEADER` is performed at the top of every Quorum header
  (through `quorum/pervasive.h`). This is intended as a way to ease the
  configuration of all the other macro definitions. Undefined by default.
- `quorum_core_index` is the type that holds the indices of all cores in a
  council. Defaults to `uint8_t`.
- `QUORUM_CORE_INDEX_MAX` is the maximum value for `quorum_core_index`. Defaults
  to `UINT8_MAX`.
- `quorum_mechanism_index` is the type that holds the indices of each mechanism
  in a council (one set of indices per type of mechanism). Defaults to
  `uint16_t`.
- `QUORUM_MECHANISM_INDEX_MAX` is the maximum value for
  `quorum_mechanism_index`. Defaults to `UINT16_MAX`.
- `QUORUM_ENABLE_BARRIER` is `1` if the `Barrier` mechanism should be enabled,
  `0` otherwise. Defaults to `0`.
- `QUORUM_ENABLE_MAILBOX` is `1` if the `Mailbox` mechanism should be enabled,
  `0` otherwise. Defaults to `0`.
- `QUORUM_ENABLE_MUTEX` is `1` if the `Mutex` mechanism should be enabled,
  `0` otherwise. Defaults to `0`.
- `QUORUM_ENABLE_SEMAPHORE` is `1` if the `Semaphore` mechanism should be enabled,
  `0` otherwise. Defaults to `0`.
- `QUORUM_ENABLE_SIGNAL` is `1` if the `Signal` mechanism should be enabled,
  `0` otherwise. Defaults to `0`.

### Baremetal Integration

### FreeRTOS Integration

## API

### Barriers
### Cores
### Councils
### Mailboxes
### Mutexes
### Semaphores
### Signals
