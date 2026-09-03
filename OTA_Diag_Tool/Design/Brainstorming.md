# Webserver design
## Website components
    * HTML contents
    * CSS design
    * Request anchors
## Webserver software's jobs
    [J1] Wait for event (request from client/s).
    [J2] Process sending out response to respective client/s.
    [J3] Process incoming requests and queue up for respective server 
    apps-->diagnostics, bootloader, logger, etc.
    [J4]  Process app-specific requests in app-specific callbacks.
    [J5] Calculate timestamp
    [J6] Keep doing J1 to J5 repeatedly after every 1 ms loop. If nothing happens for 15 seconds, prepare for sleep mode. The wakeup will be triggered whenever a new client request arrives.
