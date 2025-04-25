#!/bin/bash
REMOTE_HOST="input your remote host here"

# Note: this script requires three different connections to ssh. To avoid long delays, it is recommended to use the
# ControlMaster option in the ~/.ssh/config file. For example:
# Host rpi5-remote # or even * instead of rpi5-remote to apply to all hosts
#     ControlMaster auto
#     ControlPath ~/.ssh/sockets/%r@%h-%p
#     ControlPersist 10s
TEMP_FILE=$(ssh $REMOTE_HOST 'mktemp --suffix=.elf')
scp $1 $REMOTE_HOST:$TEMP_FILE
ssh -L 19021:127.0.0.1:19021 -t $REMOTE_HOST \
    'printf "Sleep 2000\nLoadFile '$TEMP_FILE'\ngo\nSleep 3600000" | JLinkExe -if SWD -device R7FA8M1AH -speed auto'
