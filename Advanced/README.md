### Running Instructions

##### Persistent
1. Open the server - `cd Server` ; `gcc server_persistent.c`
2. Start the server using `./a.out`
3. In a new terminal, open the client - `cd Client` ; `gcc client_persistent.c`
4. Clear all the existing text files in Client directory `rm *.txt`
5. Run the client with the following test case: `time ./a.out smallfile.txt mediumfile.txt largefile.txt 1.txt 2.txt 3.txt 4.txt 5.txt 6.txt 7.txt 8.txt 9.txt 10.txt 11.txt 12.txt 13.txt 14.txt 15.txt 25.txt 16.txt 17.txt 18.txt 19.txt 20.txt 21.txt 22.txt 23.txt 24.txt i.txt`

##### Non Persistent
1. Follow the same instructions as Persistent, except the server and client files are `server_nonpersistent.c` and `client_nonpersistent.c` respectively.

### Time Analysis
Persistent is faster than non-persistent. This is because the connection is only opened once rather than many times.
##### Observation
Outputs of time command:
- Persistent - `0.00s user 0.02s system`
- Non-Persistent - `0.00s user 0.04s system`

In this example, it is visible that non-persistent connection is 2x slower than persistent.
