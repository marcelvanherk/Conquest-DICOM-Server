#!/bin/bash
#
# conquest-pacs.sh  SysV init script for Conquest PACS.
#
# Written by Miquel van Smoorenburg <miquels>.
# Modified for Debian GNU/Linux by Ian Murdock <imurdock>.
# Customized for Conquest by Mark Pearson <markp>
#
# HOME and PACSUSER should be the only variables that may need to be modified.
#
PATH=/sbin:/bin:/usr/sbin:/usr/bin

# Modify HOME to suit your environment.
HOME=/home/conquest/ConquestDICOM
# This is the user to run as. Modify it if you don't use username conquest.
PACSUSER=conquest

DAEMON=$HOME/dgate
INI=$HOME/dicom.ini
NAME=conquest_pacs.sh

# All defaults here will be overridden by values from $HOME/dicom.ini
STATUSLOG=$HOME/serverstatus.log
PORT=104
DESC="Conquest PACS Server"

STOPPACS=$HOME"/dgate --quit:"
STARTAS=$DAEMON

test -f $DAEMON || echo "Cannot find $DAEMON" exit 0

test -f $INI || echo "Cannot find $INI" exit 0

set -e

if grep "TCPPort" $INI > /dev/null ; then
    PORT=`egrep -i '^*TCPPort *= ' $INI | sed 's/\r//' | awk '{ print $3}'`
fi

if [ $PORT -le 1024 ]; then
    test -f /usr/bin/authbind || echo "authbind is needed for access to ports < 1024" exit 0
    STARTAS="/usr/bin/authbind "
fi

if grep -is "^ *StatusLog" $INI > /dev/null ; then
    STATUSLOG=`egrep -i '^*StatusLog' $INI | sed 's/\r//' | awk '{ print $3}'`
fi

PIDFILE=/var/run/$NAME.$PORT.pid
if [ $STARTAS = $DAEMON ]; then
    ARGS=" -^$STATUSLOG"
else
    ARGS="$DAEMON -^$STATUSLOG"
fi

case "$1" in
    start)
        if [ -f $HOME/disable_autostart ]; then
            echo "Not starting $DESC: disabled via $HOME/disable_autostart"
            exit 0
        fi

        echo -n "Starting $DESC: "
        start-stop-daemon --start --quiet --pidfile $PIDFILE \
            --chuid $PACSUSER --chdir $HOME --exec $DAEMON \
            --startas $STARTAS --background -- $ARGS
        echo "$NAME."
        ;;

    stop)
        echo -n "Stopping $DESC: "
        cd $HOME
        $STOPPACS

        start-stop-daemon --oknodo --stop --quiet --pidfile $PIDFILE \
            --exec $DAEMON -- $ARGS
        echo "$NAME."
        echo
        ;;

    restart|force-reload)
        echo -n "Restarting $DESC: "
        start-stop-daemon --stop --oknodo --quiet --pidfile $PIDFILE \
            --exec $DAEMON -- $ARGS
        sleep 1
        start-stop-daemon --start --quiet --pidfile $PIDFILE \
            --chuid $PACSUSER --chdir $HOME --exec $DAEMON -- $ARGS
        echo "$NAME."
        ;;
    *)
        N=/etc/init.d/$NAME
        echo "Usage: $N {start|stop|restart|force-reload}" >&2
        exit 1
        ;;
esac

exit 0
