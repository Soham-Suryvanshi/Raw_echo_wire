# echo-wire

A minimal single-client TCP echo server written in raw C++ using POSIX sockets. It accepts one connection, reads a message, reverses the bytes, sends it back, and shuts down cleanly.

This is my **first systems programming project** — built to understand how networking actually works at the syscall level before diving into anything higher-level like async I/O or building a Redis clone from scratch.

## What it does

1. Creates a TCP socket and binds it to `0.0.0.0:6379`
2. Listens for incoming connections
3. Accepts exactly **one** client connection
4. Reads whatever the client sends
5. Reverses the raw bytes
6. Sends the reversed data back to the client
7. Closes both the client and server sockets and exits cleanly (exit code 0)

No loops, no concurrency, no protocol parsing — just the bare mechanics of a TCP round trip.

## Concepts used

- **TCP sockets** (`SOCK_STREAM`) — reliable, connection-oriented byte streams
- **Blocking I/O** — `accept()`, `recv()`, and `send()` all block until there's something to do; no `epoll`/`select` yet
- **Socket lifecycle** — `socket()` → `setsockopt()` → `bind()` → `listen()` → `accept()` → `recv()`/`send()` → `close()`
- **File descriptors** — sockets are just fds under Linux; the kernel hands out the lowest free integer (stdin/stdout/stderr take 0–2, so the listening socket is typically fd 3)
- **`SO_REUSEADDR`** — avoids "address already in use" errors when quickly restarting the server
- **Partial writes** — `send()` isn't guaranteed to write all bytes in one call, so a write-loop is used to handle that correctly
- **Clean error handling** — every syscall is checked; failures print via `perror()` and exit instead of crashing or segfaulting

## Build instructions

Requires a C++ compiler with C++11 or later (uses `std::reverse`).

```bash
g++ -std=c++17 -o echo-wire echo-wire.cpp
```

## How to run it

```bash
./echo-wire
```

The server will print its progress (socket created, bound, listening) and then block, waiting for a single connection on port `6379`.

## Example usage

In one terminal:

```bash
./echo-wire
```

In another terminal:

```bash
echo -n "Redis" | nc localhost 6379
```

Expected output from `nc`:

```
sideR
```

The server process exits automatically right after the exchange — no need to kill it manually.

## What I learned

- How a TCP connection is actually established at the syscall level — the difference between the *listening* socket and the *per-connection* socket returned by `accept()`
- Why file descriptors are small reused integers, and how to reason about which fd number to expect
- The difference between `recv()` returning `0` (clean disconnect) vs. a negative value (real error) vs. a positive value (data) — and why treating these differently matters
- That `send()`/`recv()` don't guarantee moving all the bytes you asked for in one call, which is why real network code needs read/write loops even for "small" payloads
- How to fail gracefully — checking every syscall's return value instead of assuming happy-path execution
- The habit of testing with `nc` as a lightweight, dependency-free way to sanity check a server manually

## Possible future improvements

- Loop `accept()` to handle multiple sequential clients instead of exiting after one
- Handle multiple **concurrent** clients using `epoll` (or threads/`fork()`) instead of blocking single-client I/O
- Replace raw byte reversal with real protocol parsing (this is a stepping stone toward implementing RESP, the Redis protocol)
- Add a proper read-loop to handle messages larger than the buffer size or delivered across multiple TCP packets
- Add signal handling (e.g. `SIGINT`) for graceful shutdown instead of relying on the process exiting after one exchange
- Add basic logging/timestamps and configurable port instead of hardcoding `6379`
