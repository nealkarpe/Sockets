### Directory Structure
- Basic
  - Has a basic server-client connection in which client can request one file per connection
- Advanced
  - Has `non-persistent` connection which allows client to request multiple files, and a new connection is created for each request
  - Has `persistent` connection which allows client to request multiple files, all in one connection
