This project was made as a part of a Communications Network Design lab project.
It was tested over a local network simulator, GNS3.
- Server: Maintains radio names and hands out multicast IPs to clients who want to make their own radio station / clients who want to listen to other radio stations.
- Client: Interacts with the server.

To compile, simply use the makefile.
You can run the Server and Client programs without any arguments.
After connecting, using /h will display the following help.

The command syntax is as follows:
/h - Displays this menu.
/H - Request current list of radio stations.
/j <Radio Number> - Joins a radio.
/t <Radio Name> - Creates your new transmission.
/s <Radio Name> - Stops your radio transmission.
/p <Radio Name> <Song Name and Directory> - Plays a song to your radio.
/l - Leave radio station you're listening to.
/q - Quit the program.