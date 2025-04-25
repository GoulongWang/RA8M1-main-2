#!/bin/bash
REMOTE_HOST="input your remote host here"

ssh -L -L 2331:127.0.0.1:2331 -L 19021:127.0.0.1:19021 -t $REMOTE_HOST 'JLinkGDBServer -if SWD -device R7FA8M1AH'
